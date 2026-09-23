
#include "pch.h"
#include "MainFrm.h"
#include "NetDesignerDoc.h"
#include "CDataOperations.h"
#include "CFileOperations.h"
#include "CStatus2.h"


CDataOperations::CDataOperations(void) 
{
	fill_method = 0;
	fill_type = false;
	fill_with = 0.0;
	csv_with_filename = false;
	csv_filename_header = "";
};

CDataOperations::~CDataOperations(void)
{

}

bool CDataOperations::areFilesCsv(vector<string> fnames)
{
	if (fnames.size() > 0) {
		CSVFile mfile;
		for (int i = 0; i < (int)fnames.size(); i++) {
			if (!mfile.isCSVFile(fnames[i])) {
				return(false);
			}
		}
		return(true);
	}
	return(false);
}

bool CDataOperations::areFilesJpg(vector<string> fnames)
{
	if (fnames.size() > 0) {
		JPGFile mfile;
		for (int i = 0; i < (int)fnames.size(); i++) {
			if (!mfile.isJPGFile(fnames[i])) {
				return(false);
			}
		}
		return(true);
	}
	return(false);
}

bool CDataOperations::areFilesRaw(vector<string> fnames)
{
	if (fnames.size() > 0) {
		RawFile mfile;
		for (int i = 0; i < (int)fnames.size(); i++) {
			if (!mfile.isRawFile(fnames[i])) {
				return(false);
			}
		}
		return(true);
	}
	return(false);
}

bool CDataOperations::areFilesWave(vector<string> fnames)
{
	if (fnames.size() > 0) {
		WaveFile mfile;
		for (int i = 0; i < (int)fnames.size(); i++) {
			if (!mfile.isWaveFile(fnames[i])) {
				return(false);
			}
		}
		return(true);
	}
	return(false);
}

bool CDataOperations::getDataFromCsv(string fname, vector<dataset2>* fileData)
{
	CSVFile mfile;
	mfile.addPanel1String("Loading data from " + fname);
	// Check if CSV file
	if (!mfile.isCSVFile(fname)) {
		addErrorString("getDataFromCsv : " + fname + " not a csv file");
		mfile.addPanel1String("");
		return(false);
	}
	// Try to load file
	if (!mfile.loadCSVFile(fname, true)) {
		addErrorString("getDataFromCsv : failure loading " + fname);
		mfile.addPanel1String("");
		return(false);
	}
	// Get data column for fields
	dataset2 dset;
	dset.file_path = fname;
	for (int j = 0; j < (int)mfile.headers.size(); j++) {
		data_column2 dcol = getColumnFromField(mfile.headers[j], j, &mfile);
		dset.set_data.push_back(dcol);
	}
	bool ok = true;
	if (dset.set_data.size() > 0) {
		// Do some error checking
		if (dset.set_data[0].column_data.size() != dset.set_data[0].column_string_data.size()) {
			char s[200];  sprintf_s(s, "getDataFromCsv : column_data size (%i) not equal to column_string_data size (%i) for file ", (int)dset.set_data[0].column_data.size(), (int)dset.set_data[0].column_string_data.size());
			addErrorString(s + fname);
			ok = false;
		}
		else if (ok) {
			// More error checking
			for (int j = 1; j < (int)dset.set_data.size(); j++) {
				if (dset.set_data[j].column_data.size() != dset.set_data[j].column_string_data.size()) {
					char s[200];  sprintf_s(s, "getDataFromCsv : column_data size (%i) not equal to column_string_data size (%i) for file ", (int)dset.set_data[j].column_data.size(), (int)dset.set_data[j].column_string_data.size());
					addErrorString(s + fname);
					j = (int)dset.set_data.size();
					ok = false;
				}
				if (ok) {
					if (dset.set_data[j].column_data.size() != dset.set_data[j - 1].column_data.size()) {
						char s[200];  sprintf_s(s, "getDataFromCsv : column_data[%i] size (%i) not equal to column_data[%i] size (%i) for file ", j, (int)dset.set_data[j].column_data.size(), (j - 1), (int)dset.set_data[j].column_string_data.size());
						addErrorString(s + fname);
						j = (int)dset.set_data.size();
						ok = false;
					}
				}
			}
			if (ok) {
				fileData->push_back(dset);
			}
		}
	}
	else {
		ok = false;
	}
	mfile.addPanel1String("");
	return(ok);
}

bool CDataOperations::getProcessedDataFromFileData(string fname, vector<string> fields, vector<dataset2>* processedData, vector<dataset2>* fileData)
{
	CLineParser func;
	int findex = -1;
	func.addPanel1String("Processing data for " + fname);
	if (fileData->size() > 0) {
		for (int i = 0; i < (int)fileData->size(); i++) {
			if (fname == fileData[0][i].file_path) {
				findex = i;
				i = (int)fileData->size();
			}
		}
	}
	else {
		addErrorString("getProcessedDataFromFileData : no file data");
		func.addPanel1String("");
		return(false);
	}
	if (findex < 0) {
		addErrorString("getProcessedDataFromFileData : unable to find " + fname + " in file data");
		func.addPanel1String("");
		return(false);
	}
	if (fileData[0][findex].set_data.size() <= 0) {
		addErrorString("getProcessedDataFromFileData : no data for " + fname);
		func.addPanel1String("");
		return(false);
	}
	// Get data column for fields
	dataset2 dset;
	dset.file_path = fname;
	vector<string> headeradders;
	for (int j = 0; j < (int)fields.size(); j++) {
		// Try to save data column
		int hindex = getColumnIndexFromHeader(findex, fields[j], fileData);
		if (hindex >= 0) {
			// If data name found in csv file, save the data
			if (!isFieldInDataset(fields[j], &dset)) {
				data_column2 dcol = getColumnFromField(findex, hindex, fileData);
				dset.set_data.push_back(dcol);
			}
		}
		else {
			// Check to see if last string after colon is a header
			if (func.hasSingleColon(fields[j])) {
				string theader = func.getStringAfterLastColon(fields[j]);
				hindex = getColumnIndexFromHeader(findex, theader, fileData);
				if (hindex >= 0) {
					// If data name found in csv file, save the data
					if (!isFieldInDataset(theader, &dset)) {
						data_column2 dcol = getColumnFromField(findex, hindex, fileData);
						dset.set_data.push_back(dcol);
					}
				}
				else {
					string theader2 = func.getStringAfterColon(fields[j]);
					if (theader2.length() > 0) {
						if (theader2[0] == 'm') {
							string ntheader = func.getStringBeforeColon(fields[j]);
							int nhindex = getColumnIndexFromHeader(findex, ntheader, fileData);
							if (nhindex >= 0) {
								int offset = func.getNumberFromString(theader2);
								data_column2 dcol = getColumnFromFieldWithOffset(findex, nhindex, offset, fileData);
								dcol.header = fields[j];
								dset.set_data.push_back(dcol);
								//char s[200];  sprintf_s(s, "getProcessedDataFromFileData : hindex = %i : offset = %i : need processing for m : ", hindex, offset); addErrorString(s + fields[j] + " : " + fname);
							}
							else {
								addErrorString("getProcessedDataFromFileData : unable to process data for header " + fields[j]);
							}
						}
						else if (fields[j].substr(0, 7) != "Output:") {
							char s[200];  sprintf_s(s, "getProcessedDataFromFileData : hindex = %i : unable to handle header ", hindex); addErrorString(s + fields[j] + " for " + fname);
						}
					}
				}
			}
			// If unable to find data name in csv file, save for later processing
			headeradders.push_back(fields[j]);
		}
	}
	// Check if jpg filename exists in file columns
	for (int j = 0; j < (int)fileData[0][findex].set_data.size(); j++) {
		if (fileData[0][findex].set_data[j].column_data.size() > 0) {
			if (!columnIsNumbersData(&fileData[0][findex].set_data[j].column_data)) {
				string ext = func.makeLower(func.getFileExtension(fileData[0][findex].set_data[j].column_string_data[0]));
				if (ext == "jpg") {
					//AfxMessageBox(_T("initializeDataFromCsv : found jpg files in csv data"));
					csv_with_filename = true;
					csv_filename_header = fileData[0][findex].set_data[j].header;
					data_column2 dcol = fileData[0][findex].set_data[j];
					dset.set_data.push_back(dcol);
					for (int k = 0; k < (int)dcol.column_string_data.size(); k++) {
						csv_image_filenames.push_back(dcol.column_string_data[k]);
					}
					j = (int)fileData[0][findex].set_data.size();
				}
				else if (ext == "raw") {
					//AfxMessageBox(_T("initializeDataFromCsv : found raw files in csv data"));
					csv_with_filename = true;
					csv_filename_header = fileData[0][findex].set_data[j].header;
					data_column2 dcol = fileData[0][findex].set_data[j];
					dset.set_data.push_back(dcol);
					for (int k = 0; k < (int)dcol.column_string_data.size(); k++) {
						csv_image_filenames.push_back(dcol.column_string_data[k]);
					}
					j = (int)fileData[0][findex].set_data.size();
				}
			}
		}
	}
	bool ok = true;
	if (dset.set_data.size() > 0) {
		// Do some error checking
		if (dset.set_data[0].column_data.size() != dset.set_data[0].column_string_data.size()) {
			char s[200];  sprintf_s(s, "getProcessedDataFromFileData : column_data size (%i) not equal to column_string_data size (%i) for file ", (int)dset.set_data[0].column_data.size(), (int)dset.set_data[0].column_string_data.size());
			addErrorString(s + fname);
			ok = false;
		}
		else if (ok) {
			// More error checking
			for (int j = 1; j < (int)dset.set_data.size(); j++) {
				if (dset.set_data[j].column_data.size() != dset.set_data[j].column_string_data.size()) {
					char s[200];  sprintf_s(s, "getProcessedDataFromFileData : column_data size (%i) not equal to column_string_data size (%i) for file ", (int)dset.set_data[j].column_data.size(), (int)dset.set_data[j].column_string_data.size());
					addErrorString(s + fname);
					j = (int)dset.set_data.size();
					ok = false;
				}
				if (ok) {
					if (dset.set_data[j].column_data.size() != dset.set_data[j - 1].column_data.size()) {
						char s[200];  sprintf_s(s, "getProcessedDataFromFileData : column_data[%i] size (%i) not equal to column_data[%i] size (%i) for file ", j, (int)dset.set_data[j].column_data.size(), (j - 1), (int)dset.set_data[j].column_string_data.size());
						addErrorString(s + fname);
						j = (int)dset.set_data.size();
						ok = false;
					}
				}
			}
			if (ok) {
				// If good make empty columns for output data and for ffts
				if (headeradders.size() > 0) {
					int nrows = (int)dset.set_data[0].column_data.size();
					for (int j = 0; j < (int)headeradders.size(); j++) {
						//addErrorString("header " + headeradders[j] + " needs processing");
						data_column2 dcol;
						dcol.header = headeradders[j];
						for (int k = 0; k < nrows; k++) {
							dcol.column_string_data.push_back("");
							dcol.column_data.push_back(0.0);
						}
						dset.set_data.push_back(dcol);
					}
				}
				processedData->push_back(dset);
			}
		}
	}
	else {
		ok = false;
	}
	func.addPanel1String("");
	return(ok);
}

string CDataOperations::checkFilenamesInCsv(vector<dataset2>* fileData)
{
	CLineParser func;
	int findex = 0;
	for (int j = 0; j < (int)fileData[0][findex].set_data.size(); j++) {
		if (fileData[0][findex].set_data[j].column_data.size() > 0) {
			if (!columnIsNumbersData(&fileData[0][findex].set_data[j].column_data)) {
				string ext = func.makeLower(func.getFileExtension(fileData[0][findex].set_data[j].column_string_data[0]));
				if (ext == "jpg") {
					return(ext);
				}
				else if (ext == "raw") {
					return(ext);
				}
			}
		}
	}
	return("");
}

int CDataOperations::getColumnIndexFromHeader(int findex, string hname, vector<dataset2>* fileData)
{
	if (findex < (int)fileData->size()) {
		if (fileData[0][findex].set_data.size() > 0) {
			for (int i = 0; i < (int)fileData[0][findex].set_data.size(); i++) {
				if (hname == fileData[0][findex].set_data[i].header) {
					return(i);
				}
			}
		}
	}
	return(-1);
}

data_column2 CDataOperations::getColumnFromField(int findex, int hindex, vector<dataset2>* fileData)
{
	data_column2 dcol;
	if (findex < (int)fileData->size()) {
		if (hindex < (int)fileData[0][findex].set_data.size()) {
			dcol.header = fileData[0][findex].set_data[hindex].header;
			dcol.column_string_data = fileData[0][findex].set_data[hindex].column_string_data;
			dcol.column_data = fileData[0][findex].set_data[hindex].column_data;
		}
	}
	return(dcol);
}

data_column2 CDataOperations::getColumnFromFieldWithOffset(int findex, int hindex, int offset, vector<dataset2>* fileData)
{
	data_column2 dcol;
	if (findex < (int)fileData->size()) {
		if (hindex < (int)fileData[0][findex].set_data.size()) {
			dcol.header = fileData[0][findex].set_data[hindex].header;
			for (int i = 0; i < (int)fileData[0][findex].set_data[hindex].column_string_data.size(); i++) {
				int index = i - offset;
				if ((index >= 0) && (index < (int)fileData[0][findex].set_data[hindex].column_string_data.size())) {
					dcol.column_string_data.push_back(fileData[0][findex].set_data[hindex].column_string_data[index]);
					dcol.column_data.push_back(fileData[0][findex].set_data[hindex].column_data[index]);
				}
				else {
					dcol.column_string_data.push_back("0.0");
					dcol.column_data.push_back(0.0);
				}
			}
		}
	}
	return(dcol);
}

bool CDataOperations::columnIsNumbersData(vector<double>* col)
{
	if (col->size() > 0) {
		for (int i = 0; i < (int)col->size(); i++) {
			if (col[0][i] != 0.0) {
				return(true);
			}
		}
	}
	return(false);
}

bool CDataOperations::addMissingDataColumns(string fname, vector<string> fields, vector<outputData> outdata, vector<dataset2>* processedData)
{
	if (fields.size() > 0) {
		int findex = -1;
		int clength = 1;
		bool filenamefound = false;
		if (processedData->size() > 0) {
			for (int i = 0; i < (int)processedData->size(); i++) {
				//addErrorString(fname + " : " + processedData[0][i].file_path);
				if (fname == processedData[0][i].file_path) {
					findex = i;
				}
			}
			if (findex >= 0) {
				for (int j = 0; j < (int)fields.size(); j++) {
					bool found = false;
					if (processedData[0][findex].set_data.size() > 0) {
						for (int k = 0; k < (int)processedData[0][findex].set_data.size(); k++) {
							if (k == 0) {
								clength = (int)processedData[0][findex].set_data[k].column_data.size();
							}
							if (fields[j] == processedData[0][findex].set_data[k].header) {
								k = (int)processedData[0][findex].set_data.size();
								found = true;
							}
						}
					}
					if (!found) {
						//char s[200];  sprintf_s(s, "clength = %i for ", clength); addErrorString(s + fields[j] + " and " + fname);
						data_column2 dcol;
						dcol.header = fields[j];
						for (int k = 0; k < clength; k++) {
							dcol.column_string_data.push_back("");
							dcol.column_data.push_back(0.0);
						}
						processedData[0][findex].set_data.push_back(dcol);
					}
				}
				filenamefound = true;
			}
		}
		if (!filenamefound) {
			dataset2 dset;
			dset.file_path = fname;
			for (int j = 0; j < (int)fields.size(); j++) {
				int mclength = 1;
				data_column2 dcol;
				dcol.header = fields[j];
				for (int k = 0; k < mclength; k++) {
					dcol.column_string_data.push_back("");
					dcol.column_data.push_back(0.0);
				}
				dset.set_data.push_back(dcol);
			}
			processedData->push_back(dset);
		}
		else {
			// Add column for specific field key
			if (outdata.size() > 0) {
				for (int j = 0; j < (int)fields.size(); j++) {
					for (int i = 0; i < (int)outdata.size(); i++) {
						if (fields[j] == outdata[i].outputField) {
							if (outdata[i].processdata.process == "FieldKey") {
								if (findex >= 0) {
									if (processedData[0][findex].set_data.size() > 0) {
										bool found = false;
										for (int k = 0; k < (int)processedData[0][findex].set_data.size(); k++) {
											if (processedData[0][findex].set_data[k].header == outdata[i].processdata.key1) {
												k = (int)processedData[0][findex].set_data.size();
												found = true;
											}
										}
										if (!found) {
											//addErrorString("addMissingDataColumns : adding column for " + fields[j] + " : " + outdata[i].processdata.key1);
											data_column2 dcol;
											dcol.header = outdata[i].processdata.key1;
											for (int k = 0; k < clength; k++) {
												dcol.column_string_data.push_back("");
												dcol.column_data.push_back(0.0);
											}
											processedData[0][findex].set_data.push_back(dcol);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		dataset2 dset;
		dset.file_path = fname;
		int clength = 1;
		data_column2 dcol;
		dcol.header = "dummy";
		for (int k = 0; k < clength; k++) {
			dcol.column_string_data.push_back("");
			dcol.column_data.push_back(0.0);
		}
		dset.set_data.push_back(dcol);
		processedData->push_back(dset);
	}
	return(true);
}

bool CDataOperations::addSoundConvDataRows(string fname, int nsteps, vector<dataset2>* processedData)
{
	int findex = -1;
	if (processedData->size() > 0) {
		for (int i = 0; i < (int)processedData->size(); i++) {
			if (fname == processedData[0][i].file_path) {
				findex = i;
			}
		}
		if (findex >= 0) {
			if (processedData[0][findex].set_data.size() > 0) {
				for (int j = 0; j < (int)processedData[0][findex].set_data.size(); j++) {
					if (nsteps > (int)processedData[0][findex].set_data[j].column_data.size()) {
						if (processedData[0][findex].set_data[j].column_data.size() == processedData[0][findex].set_data[j].column_string_data.size()) {
							int nadd = nsteps - (int)processedData[0][findex].set_data[j].column_data.size();
							double column_data0 = 0.0;
							string column_string_data0 = "0.0";
							if (processedData[0][findex].set_data[j].column_data.size() > 0) {
								column_data0 = processedData[0][findex].set_data[j].column_data[0];
								column_string_data0 = processedData[0][findex].set_data[j].column_string_data[0];
							}
							for (int k = 0; k < nadd; k++) {
								processedData[0][findex].set_data[j].column_data.push_back(column_data0);
								processedData[0][findex].set_data[j].column_string_data.push_back(column_string_data0);
							}
						}
					}
				}
			}
		}
	}
	return(true);
}

bool CDataOperations::isFieldInDataset(string header, dataset2 *dset)
{
	if (dset->set_data.size() > 0) {
		for (int i = 0; i < (int)dset->set_data.size(); i++) {
			if (header == dset->set_data[i].header) {
				return(true);
			}
		}
	}
	return(false);
}

data_column2 CDataOperations::getColumnFromField(string header, int hindex, CSVFile *mfile)
{
	data_column2 dcol;
	dcol.header = header;
	dcol.column_string_data = mfile->getColumnStringData(hindex);
	if (mfile->isColumnNumbersData(hindex)) {
		dcol.column_data = mfile->getColumnDoubleData(hindex);
	}
	else {
		vector<double> cdata;
		for (int k = 0; k < (int)dcol.column_string_data.size(); k++) {
			cdata.push_back(0.0);
		}
		dcol.column_data = cdata;
	}
	return(dcol);
}

void CDataOperations::applyOffsetToData(vector<dataset2>* processedData)
{
	if (processedData->size() > 0) {
		for (int i = 0; i < (int)processedData->size(); i++) {
			if (processedData[0][i].set_data.size() > 0) {
				for (int j = 0; j < (int)processedData[0][i].set_data.size(); j++) {
					if (processedData[0][i].set_data[j].header.substr(0, 5) == "Index") {
						CSVFile func;
						string before = func.getStringBeforeColon(processedData[0][i].set_data[j].header);
						string after = func.getStringAfterColon(processedData[0][i].set_data[j].header);
						int index = stoi(before.substr(6));
						if (processedData[0][i].set_data[j].header.substr(5, 1) == "M") {
							index = -1 * index;
						}
						//char s[200];  sprintf_s(s, "index %i for ", index); addErrorString(s + processedData[0][i].set_data[j].header + " : " + after);
						if (after.substr(0, 7) != "Output:") {
							//addErrorString("processing index data for " + processedData[0][i].set_data[j].header + " for file " + processedData[0][i].file_path);
							string after2 = func.getStringAfterLastColon(processedData[0][i].set_data[j].header);
							for (int k = 0; k < (int)processedData[0][i].set_data.size(); k++) {
								if (processedData[0][i].set_data[k].header == after2) {
									for (int m = 0; m < (int)processedData[0][i].set_data[j].column_data.size(); m++) {
										int offset = m + index;
										if ((offset >= 0) && (offset < (int)processedData[0][i].set_data[k].column_data.size())) {
											processedData[0][i].set_data[j].column_data[m] = processedData[0][i].set_data[k].column_data[offset];
											processedData[0][i].set_data[j].column_string_data[m] = processedData[0][i].set_data[k].column_string_data[offset];
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

bool CDataOperations::processDataColumn(string *fieldname, processData mprocess, double leakage, vector<dataset2>* processedData)
{
	CSVFile func;

	if (processedData->size() > 0) {

		//char s[200];  sprintf_s(s, " : %.8f : %.8f", mprocess.lfactor, mprocess.ufactor);
		//addErrorString("processDataColumn : " + *fieldname + " : " + mprocess.process + s);// " : " + mprocess.key1 + " : " + mprocess.key2 + " : " + mprocess.key3);

		string fromname = func.getStringAfterLastColon(*fieldname);

		for (int j = 0; j < (int)processedData->size(); j++) {

			int findex = getColumnIndexFromHeader(*fieldname, j, processedData);
			int tindex = getColumnIndexFromHeader(fromname, j, processedData);

			if ((findex >= 0) && (tindex >= 0)) {

				if ((processedData[0][j].set_data[findex].column_data.size() == processedData[0][j].set_data[findex].column_string_data.size()) &&
					(processedData[0][j].set_data[findex].column_data.size() == processedData[0][j].set_data[tindex].column_data.size()) &&
					(processedData[0][j].set_data[findex].column_data.size() == processedData[0][j].set_data[tindex].column_string_data.size())) {

					if (mprocess.process == "NormLinear") {
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
							processedData[0][j].set_data[findex].column_data[i] = mprocess.lfactor + (processedData[0][j].set_data[tindex].column_data[i] * mprocess.ufactor);
							//char s[200];  sprintf_s(s, "%.4f", processedData[0][j].set_data[findex].column_data[i]); addErrorString(s);
						}
					}
					else if (mprocess.process == "NormLog") {
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
							if (processedData[0][j].set_data[tindex].column_data[i] > 0) {
								processedData[0][j].set_data[findex].column_data[i] = mprocess.lfactor + (log(processedData[0][j].set_data[tindex].column_data[i]) * mprocess.ufactor);
							}
							else {
								processedData[0][j].set_data[findex].column_data[i] = mprocess.lfactor;
							}
						}
					}
					else if ((mprocess.process == "NormSeconds") || (mprocess.process == "Seconds")) {
#ifndef GCC
						COleDateTime t1 = COleDateTime::GetCurrentTime();
						COleDateTime t2 = COleDateTime::GetCurrentTime();
						t1.ParseDateTime((CString)mprocess.key3.c_str());
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_string_data.size(); i++) {
							t2.ParseDateTime((CString)processedData[0][j].set_data[tindex].column_string_data[i].c_str());
							COleDateTimeSpan span = t2 - t1;
							double delta = 8640 * (double)span.GetDays() + 360 * (double)span.GetHours() + 60 * (double)span.GetMinutes() + (double)span.GetSeconds();
							if (mprocess.process == "NormSeconds") {
								delta = mprocess.ufactor * delta;
							}
							processedData[0][j].set_data[findex].column_data[i] = delta;
						}
#endif
					}
					else if ((mprocess.process == "NormMinutes") || (mprocess.process == "Minutes")) {
#ifndef GCC
						COleDateTime t1 = COleDateTime::GetCurrentTime();
						COleDateTime t2 = COleDateTime::GetCurrentTime();
						t1.ParseDateTime((CString)mprocess.key3.c_str());
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_string_data.size(); i++) {
							t2.ParseDateTime((CString)processedData[0][j].set_data[tindex].column_string_data[i].c_str());
							COleDateTimeSpan span = t2 - t1;
							double delta = 1440 * (double)span.GetDays() + 60 * (double)span.GetHours() + (double)span.GetMinutes();
							if (mprocess.process == "NormMinutes") {
								delta = mprocess.ufactor * delta;
							}
							processedData[0][j].set_data[findex].column_data[i] = delta;
						}
#endif
					}
					else if ((mprocess.process == "NormHours") || (mprocess.process == "Hours")) {
#ifndef GCC
						COleDateTime t1 = COleDateTime::GetCurrentTime();
						COleDateTime t2 = COleDateTime::GetCurrentTime();
						t1.ParseDateTime((CString)mprocess.key3.c_str());
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_string_data.size(); i++) {
							t2.ParseDateTime((CString)processedData[0][j].set_data[tindex].column_string_data[i].c_str());
							COleDateTimeSpan span = t2 - t1;
							double delta = 24 * (double)span.GetDays() + (double)span.GetHours();
							if (mprocess.process == "NormHours") {
								delta = mprocess.ufactor * delta;
							}
							processedData[0][j].set_data[findex].column_data[i] = delta;
						}
#endif
					}
					else if ((mprocess.process == "NormDays") || (mprocess.process == "Days")) {
#ifndef GCC
						COleDateTime t1 = COleDateTime::GetCurrentTime();
						COleDateTime t2 = COleDateTime::GetCurrentTime();
						t1.ParseDateTime((CString)mprocess.key3.c_str());
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_string_data.size(); i++) {
							t2.ParseDateTime((CString)processedData[0][j].set_data[tindex].column_string_data[i].c_str());
							COleDateTimeSpan span = t2 - t1;
							double delta = (double)span.GetDays();
							if (mprocess.process == "NormDays") {
								delta = mprocess.ufactor * delta;
							}
							processedData[0][j].set_data[findex].column_data[i] = delta;
						}
#endif
					}
					else if (mprocess.process.substr(0, 10) == "Derivative") {
						bool relative = false;
						if (mprocess.process[mprocess.process.length() - 1] == 'R') {
							relative = true;
						}
						if (mprocess.process == "Derivative1") {
							for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
								double der = firstDerivativeBackward(i, mprocess.stride, relative, &processedData[0][j].set_data[tindex].column_data);
								double conv = mprocess.lfactor + (der * mprocess.ufactor);
								processedData[0][j].set_data[findex].column_data[i] = conv;
							}
						}
						else if (mprocess.process == "Derivative2") {
							for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
								double der = secondDerivativeBackward(i, mprocess.stride, relative, &processedData[0][j].set_data[tindex].column_data);
								double conv = mprocess.lfactor + (der * mprocess.ufactor);
								processedData[0][j].set_data[findex].column_data[i] = conv;
							}
						}
						else if (mprocess.process == "Derivative3") {
							for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
								double der = thirdDerivativeBackward(i, mprocess.stride, relative, &processedData[0][j].set_data[tindex].column_data);
								double conv = mprocess.lfactor + (der * mprocess.ufactor);
								processedData[0][j].set_data[findex].column_data[i] = conv;
							}
						}
						else if (mprocess.process == "Derivative4") {
							for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
								double der = fourthDerivativeBackward(i, mprocess.stride, relative, &processedData[0][j].set_data[tindex].column_data);
								double conv = mprocess.lfactor + (der * mprocess.ufactor);
								processedData[0][j].set_data[findex].column_data[i] = conv;
							}
						}
					}
					else if (mprocess.process == "FieldKey") {
						string key = mprocess.key1;
						int toindex = getColumnIndexFromHeader(key, j, processedData);
						if (toindex >= 0) {
							for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
								string sdata = processedData[0][j].set_data[tindex].column_string_data[i];
								double val = leakage;
								if (key == sdata) {
									val = 1.0 - leakage;
								}
								processedData[0][j].set_data[toindex].column_data[i] = val;
								*fieldname = mprocess.key1;
								//char s[200];  sprintf_s(s, " : j = %i : findex = %i : tindex = %i : val = %3f", j, findex, tindex, val); addErrorString(sdata + " : " + key + s);

							}
						}
						else {
							//AfxMessageBox(_T("processDataColumn : unable to find to coumn for ") + (CString)key.c_str());
							addErrorString("processDataColumn : unable to find to coumn for " + key);
						}
					}
					else if (mprocess.process == "FNParser") {
						string key = mprocess.key2;
						double val = leakage;
						if (isLabelInFilename(key, processedData[0][j].file_path)) {
							val = 1.0 - leakage;
						}
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
							processedData[0][j].set_data[findex].column_data[i] = val;
						}
					}
					else if (mprocess.process.substr(0, 3) == "FFT") {
						addErrorString("processDataColumn : need to process FFT for fieldname " + *fieldname);
					}
					else {
						if (*fieldname != fromname) {
							for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
								processedData[0][j].set_data[findex].column_data[i] = processedData[0][j].set_data[tindex].column_data[i];
							}
						}
					}

				}
				else {
					addErrorString("processDataColumn : problem with data column sizes for " + *fieldname + " in " + processedData[0][j].file_path);
					j = (int)processedData->size();
				}

			}
			else if (findex < 0) {
				char s[200];  sprintf_s(s, " : j = %i", j); 
				addErrorString("processDataColumn : unable to find data for field " + *fieldname + s);
				return(false);
			}
			else {
				addErrorString("processDataColumn : unable to find data for processed field " + fromname);
				return(false);
			}

		}

	}

	return(true);
}

bool CDataOperations::processFFTColumn(string fieldname, processData mprocess, vector<dataset2>* processedData)
{
	CLineParser func;
	if (processedData->size() > 0) {
		// For all data files
		for (int i = 0; i < (int)processedData->size(); i++) {
			if (processedData[0][i].set_data.size() > 0) {
				// We're going to find N and indices to store data once FFT is calculated
				int N = 0;
				vector<fftIndices> findices;
				for (int j = 0; j < (int)processedData[0][i].set_data.size(); j++) {
					// Find all FFT column headers
					if (processedData[0][i].set_data[j].header.substr(0, 3) == "FFT") {
						// Constrained to those that match target fieldname
						if (fieldname == func.getStringAfterLastColon(processedData[0][i].set_data[j].header)) {
							// If match extract FFT number
							string fftstring = func.getStringBeforeColon(processedData[0][i].set_data[j].header);
							int fftnum = stoi(fftstring.substr(3));
							// Save the max FFT number as N
							if (fftnum > N) {
								N = fftnum;
							}
							// Then save column indices to store data from FFTs
							fftIndices fi;
							fi.index = j;
							fi.fftnum = fftnum;
							if (findices.size() == 0) {
								findices.push_back(fi);
							}
							else {
								if (fftnum > findices[(int)findices.size() - 1].fftnum) {
									findices.push_back(fi);
								}
								else {
									for (int k = 0; k < (int)findices.size(); k++) {
										if (fftnum > findices[k].fftnum) {
											findices.insert(findices.begin() + k, fi);
											k = (int)findices.size();
										}
									}
								}
							}
							//char s[200];  sprintf_s(s, " : fftnum = %i", fftnum); addErrorString(fieldname + s);
						}
					}
				}
				// Now finally calculate FFT
				if (findices.size() > 0) {
					// Get input column index
					int cindex = -1;
					for (int j = 0; j < (int)processedData[0][i].set_data.size(); j++) {
						if (processedData[0][i].set_data[j].header == fieldname) {
							cindex = j;
							j = (int)processedData[0][i].set_data.size();
						}
					}
					if (cindex >= 0) {
						N += 1;
						int downsample = mprocess.downsample;  
						vector<double> fdata = processedData[0][i].set_data[cindex].column_data;
						for (int j = 0; j < (int)fdata.size(); j++) {
							vector<double> mdata = getFFT(j, (2 * N), downsample, true, fdata);
							//string s2 = "";
							if (mdata.size() == N) {
								for (int k = 0; k < N; k++) {
									processedData[0][i].set_data[findices[k].index].column_data[j] = mdata[k];
									/*/if (j == 200) {
										char s[200];  sprintf_s(s, "%.4f  ", mdata[k]);
										s2 += s;
									}*/
								}
							}
							else {
								char s[200];  sprintf_s(s, " : %i : %i", N, (int)mdata.size());  
								addErrorString("processFFTColumn : size mismatch for " + fieldname + s);
								return(false);
							}
							/*if (j == 200) {
								addErrorString(s2);
							}*/
						}
					}
					else {
						addErrorString("processFFTColumn for " + fieldname + " unable to find input column");
						return(false);
					}
				}
				else {
					addErrorString("processFFTColumn for " + fieldname + " unable to find output columns");
					return(false);
				}
			}
		}
	}
	return(true);
}

bool CDataOperations::getProcessData(string* fieldname, processData* mprocess, vector<dataset2>* processedData)
{
	CSVFile func;

	if (processedData->size() > 0) {

		//addErrorString("getProcessData : " + *fieldname + " : " + mprocess.process + " : " + mprocess.key1 + " : " + mprocess.key2 + " : " + mprocess.key3);

		string fromname = func.getStringAfterLastColon(*fieldname);

		for (int j = 0; j < (int)processedData->size(); j++) {

			int tindex = getColumnIndexFromHeader(fromname, j, processedData);

			if (tindex >= 0) {

				if (processedData[0][j].set_data[tindex].column_data.size() > 0) {

					if (mprocess->process == "NormLinear") {
						double mmin = processedData[0][j].set_data[tindex].column_data[0];
						double mmax = processedData[0][j].set_data[tindex].column_data[0];
						for (int i = 1; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
							double data = processedData[0][j].set_data[tindex].column_data[i];
							if (data < mmin) {
								mmin = data;
							}
							if (data > mmax) {
								mmax = data;
							}
						}
						if (mmax != mmin) {
							mprocess->lfactor = -mmin / (mmax - mmin);
							mprocess->ufactor = 1.0 / (mmax - mmin);
						}
						else {
							mprocess->lfactor = 0.0;
							mprocess->ufactor = 1.0;
						}
						return(true);
					}
					else if (mprocess->process == "NormLog") {
						bool zero_log = false;
						for (int i = 0; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
							double data = processedData[0][j].set_data[tindex].column_data[i];
							if (data <= 0) {
								i = (int)processedData[0][j].set_data[tindex].column_data.size();
								zero_log = true;
							}
						}
						if (!zero_log) {
							double mmin = log(processedData[0][j].set_data[tindex].column_data[0]);
							double mmax = log(processedData[0][j].set_data[tindex].column_data[0]);
							for (int i = 1; i < (int)processedData[0][j].set_data[tindex].column_data.size(); i++) {
								double data = log(processedData[0][j].set_data[tindex].column_data[i]);
								if (data < mmin) {
									mmin = data;
								}
								if (data > mmax) {
									mmax = data;
								}
							}
							if (mmax != mmin) {
								mprocess->lfactor = -mmin / (mmax - mmin);
								mprocess->ufactor = 1.0 / (mmax - mmin);
							}
							else {
								mprocess->lfactor = 0.0;
								mprocess->ufactor = 1.0;
							}
						}
						return(true);
					}

				}
				else {
					addErrorString("getProcessData : problem with data column sizes for " + *fieldname + " in " + processedData[0][j].file_path);
					j = (int)processedData->size();
					return(false);
				}

			}
			else {
				char s[200];  sprintf_s(s, " : j = %i", j);
				addErrorString("getProcessData : unable to find data for field " + *fieldname + s);
				return(false);
			}

		}

	}

	return(false);
}

vector<double> CDataOperations::getFFT(int index, int N, int downsample, bool normalize, vector<double> data)
{
	vector<double> rval;
	static bool ffterrordisplayed1 = false;
	if (index < (int)data.size()) {
		int i = 0;
		int myN = 1;
		while (myN < N) {
			i++;
			myN = (int)pow(2.0, i);
		}
		complex* mydata = new complex[myN];
		if (mydata == 0) {
			if (!ffterrordisplayed1) {
				addErrorString("Unable to allocate memory for mydata in getFFT");
			}
			ffterrordisplayed1 = true;
		}
		else {
			double max = 0;
			for (int j = 0; j < myN; j++) {
				mydata[j].Im = 0;
				int jindex = index - (j * downsample);
				if ((jindex >= 0) && (jindex < data.size())) {
					mydata[j].Re = data[jindex];
					if (normalize) {
						if (data[jindex] > max) {
							max = data[jindex];
						}
						else if (data[jindex] < -max) {
							max = -data[jindex];
						}
					}
				}
				else {
					mydata[j].Re = 0;
				}
			}
			if (normalize && (max > 0)) {
				max = 2 * max;
				for (int j = 0; j < myN; j++) {
					mydata[j].Re = mydata[j].Re / max;
				}
			}
			fft(mydata, myN);
			for (int j = 0; j < (myN / 2); j++) {
				double r = sqrt((mydata[j].Re * mydata[j].Re) + (mydata[j].Im * mydata[j].Im));
				rval.push_back(r);
			}
			delete[] mydata;
			// Normalize
			if ((int)rval.size() > 1) {
				if (rval[0] > 0.0) {
					for (int j = 0; j < (int)rval.size(); j++) {
						rval[j] = rval[j] / rval[0];
					}
				}
			}
		}
	}
	return(rval);
}

vector<double> CDataOperations::getFFT2(int index, int N, int downsample, vector<double> data)
{
	vector<double> rval;
	static bool ffterrordisplayed1 = false;
	if (index < (int)data.size()) {
		int i = 0;
		int myN = 1;
		while (myN < N) {
			i++;
			myN = (int)pow(2.0, i);
		}
		complex* mydata = new complex[myN];
		if (mydata == 0) {
			if (!ffterrordisplayed1) {
				addErrorString("Unable to allocate memory for mydata in getFFT");
			}
			ffterrordisplayed1 = true;
		}
		else {
			for (int j = 0; j < myN; j++) {
				mydata[j].Im = 0;
				int jindex = index + (j * downsample);
				if ((jindex >= 0) && (jindex < data.size())) {
					mydata[j].Re = data[jindex];
				}
				else {
					mydata[j].Re = 0;
				}
			}
			fft(mydata, myN);
			for (int j = 0; j < (myN / 2); j++) {
				double r = sqrt((mydata[j].Re * mydata[j].Re) + (mydata[j].Im * mydata[j].Im));
				rval.push_back(r);
			}
			delete[] mydata;
			// Normalize
			if ((int)rval.size() > 1) {
				if (rval[0] > 0.0) {
					for (int j = 0; j < (int)rval.size(); j++) {
						rval[j] = rval[j] / rval[0];
					}
				}
			}
		}
	}
	return(rval);
}

void CDataOperations::fft(complex* v, int n)
{
	static bool ffterrordisplayed2 = false;
	complex* scratch = new complex[n/* + 10*/];
	if ((scratch == 0) && !ffterrordisplayed2) {
		addErrorString("Unable to allocate memory for scratch in fft");
		ffterrordisplayed2 = true;
		return;
	}
	fft(v, n, scratch);
	delete[] scratch;
}

void CDataOperations::fft(complex* v, int n, complex* tmp)
{
	if (n > 1) {

		int k, m;
		complex z, w, * vo, * ve;

		ve = tmp;
		vo = tmp + n / 2;

		for (k = 0; k < n / 2; k++) {
			ve[k] = v[2 * k];
			vo[k] = v[2 * k + 1];
		}

		fft(ve, n / 2, v);
		fft(vo, n / 2, v);

		for (m = 0; m < n / 2; m++) {

			w.Re = cos(2 * pi * (double)m / (double)n);
			w.Im = -sin(2 * pi * (double)m / (double)n);

			z.Re = w.Re * vo[m].Re - w.Im * vo[m].Im;
			z.Im = w.Re * vo[m].Im + w.Im * vo[m].Re;

			v[m].Re = ve[m].Re + z.Re;
			v[m].Im = ve[m].Im + z.Im;

			v[m + n / 2].Re = ve[m].Re - z.Re;
			v[m + n / 2].Im = ve[m].Im - z.Im;

		}
	}
}

double CDataOperations::firstDerivativeBackward(int index, int stride, bool relative, vector<double> *data)
{
	double rval = 0;
	if (index < (int)data->size()) {
		if (index > ((3 * stride) - 1)) {
			rval = 1.833333 * data[0][index] - 3 * data[0][index - stride] + 1.5 * data[0][index - (2 * stride)] - 0.333333 * data[0][index - (3 * stride)];
		}
		else if (index > ((2 * stride) - 1)) {
			rval = 1.5 * data[0][index] - 2 * data[0][index - stride] + 0.5 * data[0][index - (2 * stride)];
		}
		else if (index > ((1 * stride) - 1)) {
			rval = data[0][index] - data[0][index - stride];
		}
		if (relative) {
			double rel = data[0][index];
			if (rel < 0) {
				rel = -1 * rel;
			}
			if (rel != 0) {
				rval = rval / rel;
			}
			else {
				rval = 0;
			}
			if ((rval > 100) || (rval < -100)) {
				rval = 0;
			}
		}
	}
	return(rval);
}

double CDataOperations::secondDerivativeBackward(int index, int stride, bool relative, vector<double>* data)
{
	double rval = 0;
	if (index < (int)data->size()) {
		if (index > ((4 * stride) - 1)) {
			rval = 2 * data[0][index] - 5 * data[0][index - stride] + 4 * data[0][index - (2 * stride)] - data[0][index - (3 * stride)];
		}
		else if (index > ((3 * stride) - 1)) {
			rval = data[0][index] - 2 * data[0][index - stride] + data[0][index - (2 * stride)];
		}
		if (relative) {
			double rel = data[0][index];
			if (rel < 0) {
				rel = -1 * rel;
			}
			if (rel != 0) {
				rval = rval / rel;
			}
			else {
				rval = 0;
			}
			if ((rval > 100) || (rval < -100)) {
				rval = 0;
			}
		}
	}
	return(rval);
}

double CDataOperations::thirdDerivativeBackward(int index, int stride, bool relative, vector<double>* data)
{
	double rval = 0;
	if (index < (int)data->size()) {
		if (index > ((5 * stride) - 1)) {
			rval = 2.5 * data[0][index] - 9 * data[0][index - stride] + 12 * data[0][index - (2 * stride)] - 7 * data[0][index - (3 * stride)] + 1.5 * data[0][index - (4 * stride)];
		}
		else if (index > ((4 * stride) - 1)) {
			rval = data[0][index] - 3 * data[0][index - stride] + 3 * data[0][index - (2 * stride)] - data[0][index - (3 * stride)];
		}
		if (relative) {
			double rel = data[0][index];
			if (rel < 0) {
				rel = -1 * rel;
			}
			if (rel != 0) {
				rval = rval / rel;
			}
			else {
				rval = 0;
			}
			if ((rval > 100) || (rval < -100)) {
				rval = 0;
			}
		}
	}
	return(rval);
}

double CDataOperations::fourthDerivativeBackward(int index, int stride, bool relative, vector<double>* data)
{
	double rval = 0;
	if (index < (int)data->size()) {
		if (index > ((6 * stride) - 1)) {
			rval = 3 * data[0][index] - 14 * data[0][index - stride] + 26 * data[0][index - (2 * stride)] - 24 * data[0][index - (3 * stride)] + 11 * data[0][index - (4 * stride)] - 2 * data[0][index - (5 * stride)];
		}
		else if (index > ((5 * stride) - 1)) {
			rval = data[0][index] - 4 * data[0][index - stride] + 6 * data[0][index - (2 * stride)] - 4 * data[0][index - (3 * stride)] + data[0][index - (4 * stride)];
		}
		if (relative) {
			double rel = data[0][index];
			if (rel < 0) {
				rel = -1 * rel;
			}
			if (rel != 0) {
				rval = rval / rel;
			}
			else {
				rval = 0;
			}
			if ((rval > 100) || (rval < -100)) {
				rval = 0;
			}
		}
	}
	return(rval);
}

bool CDataOperations::isLabelInFilename(string label, string dataPath)
{
	vector<string> names = parseKeyNamesFromFilename(dataPath);
	for (int j = 0; j < names.size(); j++) {
		if (label == names[j]) {
			return(true);
		}
	}
	return(false);
}

vector<string> CDataOperations::parseKeyNamesFromFilename(string dataPath)
{
	CSVFile func;
	string s = removeExtensionFromFilename(dataPath);
	s = func.makeLower(s);
	long index = 0;
	while (index < s.length()) {
		if (func.isColon((char)s[index])) {
			s = s.substr(index + 1);
			index = (int)s.length();
		}
		index++;
	}
	vector<string> names = func.parseLine2(s);
	return(names);
}

string CDataOperations::removeExtensionFromFilename(string path)
{
	string rval = path;
	int i = (int)rval.length() - 1;
	while ((i >= 0) && (rval[i] != '.')) {
		i--;
	}
	if (i > 0) {
		rval = rval.substr(0, i);
	}
	return(rval);
}

int CDataOperations::getColumnIndexFromHeader(string fieldname, int fileindex, vector<dataset2>* processedData)
{
	if (processedData[0].size() > 0) {
		if (fileindex < (int)processedData->size()) {
			if (processedData[0][fileindex].set_data.size() > 0) {
				for (int i = 0; i < (int)processedData[0][fileindex].set_data.size(); i++) {
					if (fieldname == processedData[0][fileindex].set_data[i].header) {
						return(i);
					}
				}
			}
		}
	}
	return(-1);
}

bool CDataOperations::getImages(vector<string> fnames, int image_width, int image_height, _int64 image_train_data_size, double* image_train_data)
{
	if (fnames.size() > 0) {

		int mimage_index = 0;

		CLineParser func;

		unsigned char* mimage_data = new unsigned char[3 * ((int64_t)image_width + 0) * ((int64_t)image_height + 2)];

		unsigned char* filedata = NULL;

		if (mimage_data != 0) {

			int imoffset = 0;

			CStatus progressDlg;
			progressDlg.CreateDlg();
			progressDlg.setLabel("Getting images . . .");

			int num_images = (int)fnames.size();

			int mcnt = 0;
			int tcnt = 0;
			int ttotal = 1 + num_images;

			int color = 3;
			int image_size = color * image_width * image_height;

			int rawwidth = -1;
			int rawheight = -1;

			progressDlg.setProgress(0);

			while (mimage_index < num_images) {

				string name = fnames[mimage_index];

				func.addPanel1String("Loading data from " + name);

				string ext = func.makeLower(func.getFileExtension(name));

				if (ext == "jpg") {

					CImage image;
					HRESULT rslt = image.Load((CString)name.c_str());

					if (rslt == S_OK) {

						CBitmap bitmap;
						bitmap.Attach(image.Detach());

						BITMAP pBitMap;
						if (bitmap.GetBitmap(&pBitMap)) {

							//char s[200];  sprintf_s(s, " : bmWidth (%li) : bmHeight (%li) : image_width (%i) : image_height (%i)", pBitMap.bmWidth, pBitMap.bmHeight, image_width, image_height);
							//addErrorString("getImages : processing raw image : " + name + s);

							unsigned char* mbits = (unsigned char*)pBitMap.bmBits;
							mimage_data = func.downscaleImageData(pBitMap.bmHeight, pBitMap.bmWidth, pBitMap.bmWidthBytes, mbits, image_height, image_width, mimage_data);

							for (int j = 0; j < image_size; j++) {
								int imindex = j + imoffset + 0;
								if (imindex < image_train_data_size) {
									image_train_data[imindex] = (double)mimage_data[j + 0] / 256;
								}
								else {
									addErrorString("getImages returning false - image data exceeding allocated memory size");
									func.addPanel1String("");
									return(false);
								}
							}

							imoffset += image_size;// (mimage_index * image_size);

						}

					}
					else {
						addErrorString("Getting JPG Images - unable to load image from " + name);
					}

				}
				else if (ext == "raw") {

					if ((rawwidth < 0) || (rawheight < 0)) {
						CWnd* pWndMain = AfxGetMainWnd();
						CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
						for (int i = 0; i < pDoc->netData.size(); i++) {
							if (pDoc->netData.isDataSource(i)) {
								rawwidth = pDoc->netData.getRawImageWidth(i);
								rawheight = pDoc->netData.getRawImageHeight(i);
								filedata = new unsigned char[3 * (int64_t)rawwidth * (int64_t)rawheight];
								i = pDoc->netData.size();
							}
						}
					}

					int test_size = 3 * rawwidth * rawheight;

					CFile file;
					if (file.Open((CString)name.c_str(), CFile::modeRead | CFile::shareDenyWrite)) {
						unsigned long filelength = (unsigned long)file.GetLength();
						if (filelength == test_size) {

							//char s[200];  sprintf_s(s, " : rawwidth (%li) : rawheight (%li) : image_width (%i) : image_height (%i)", rawwidth, rawheight, image_width, image_height);
							//addErrorString("getImages : processing raw image : " + name + s);

							file.Read(filedata, filelength);
							file.Close();

							mimage_data = func.downscaleImageData(rawheight, rawwidth, 3*rawwidth, filedata, image_height, image_width, mimage_data);

							for (int j = 0; j < image_size; j++) {
								int imindex = (image_size - j - 1) + imoffset + 0;
								if (imindex < image_train_data_size) {
									image_train_data[imindex] = (double)mimage_data[j + 0] / 256;
								}
								else {
									addErrorString("getImages returning false - image data exceeding allocated memory size");
									func.addPanel1String("");
									return(false);
								}
							}

							imoffset += image_size;

						}
						else {
							char s[200];  sprintf_s(s, "Getting RAW Images - filelength (%li) != test_size (%li)", filelength, test_size);
							addErrorString(s);
							return(false);
						}
					}

				}

				tcnt++;
				mcnt++;
				if (mcnt > 5) {
					if (tcnt > 0) {
						progressDlg.setLabel("Getting image files . . .");
					}
					mcnt = 0;
				}
				progressDlg.setProgress((100 * mimage_index) / num_images);

				mimage_index++;

			}

			delete[] mimage_data;

			if (filedata != NULL) {
				if ((rawwidth > 0) && (rawheight > 0)) {
					delete[] filedata;
				}
			}

			progressDlg.DestroyDlg();

			func.addPanel1String("");

			return(true);

		}

	}

	return(false);

}

bool CDataOperations::getSoundData(vector<string> fnames, wave_check_results* wresults, _int64 wave_allocation_size, double* wave_train_data)
{
	CDataOperations dops;

	for (int j = 0; j < wave_allocation_size; j++) {
		wave_train_data[j] = 0.0;
	}

	int i = 0;
	int mwave_index = 0;
	int mnum_pathnames = (int)fnames.size();

	WaveFile func;

	CStatus myProgressDlg;
	myProgressDlg.CreateDlg();
	myProgressDlg.setLabel(_T("Getting wave files . . ."));

	myProgressDlg.setProgress(0);

	if (mnum_pathnames > 0) {

		int ttotal = 1 + mnum_pathnames;

		while (i < mnum_pathnames) {

			if ((i < wresults->wave_data_lengths.size()) && (i < wresults->wave_start_indices.size())) {

				func.addPanel1String("Getting wave files : " + fnames[i]);

				wave_data wdata = func.getWaveDataFromFile(fnames[i]);

				if (wdata.length > 0) {

					int data_length = wdata.length;

					if (data_length >= wresults->wave_data_lengths[i]) {

						if (data_length > 0) {
							_int64 index = wresults->wave_start_indices[i];
							for (int j = 0; j < wresults->wave_data_lengths[i]; j++) {
								if (index < wave_allocation_size) {
									wave_train_data[index] = (double)wdata.data[j];
								}
								else {
									char s[200];  sprintf_s(s, "getSoundData : index >= max_sound_allocation_size : %i : ", (int)wave_allocation_size);
									addErrorString(s + fnames[i]);
									i = mnum_pathnames;
								}
								index++;
							}
						}
						else {
							char s[200];  sprintf_s(s, "getSoundData : size error : data_length = %i : ", data_length);
							addErrorString(s + fnames[i]);
							i = mnum_pathnames;
						}

						mwave_index++;

					}
					else {
						char s[200];  sprintf_s(s, "getSoundData : size error : data_length = %i : nnet.wave_data_lengths[%i] = %i : ", data_length, i, (int)wresults->wave_data_lengths[i]);
						addErrorString(s + fnames[i]);
						i = mnum_pathnames;
					}

				}
				else {
					addErrorString("getSoundData : unable to read " + fnames[i]);
					i = mnum_pathnames;
				}

			}
			else {
				char s[200];  sprintf_s(s, "getSoundData : size error : combinedPathnames.size = %i : nnet.wave_data_lengths.size = %i : wave_start_indices.size = %i", mnum_pathnames, (int)wresults->wave_data_lengths.size(), (int)wresults->wave_start_indices.size());
				addErrorString(s);
				i = mnum_pathnames;
			}

			myProgressDlg.setProgress((100 * i) / mnum_pathnames);

			i++;

		}
	}
	else {
		char s[200];  sprintf_s(s, "getSoundData : no combinedPathnames (size = %i)", (int)fnames.size());
		addErrorString(s);
	}

	func.addPanel1String("");
	myProgressDlg.DestroyDlg();

	if (mwave_index != (int)fnames.size()) {
		char s[200];  sprintf_s(s, "getSoundData : mwave_index (%i) != combinedPathnames.size (%i)", mwave_index, (int)fnames.size());
		addErrorString(s);
	}

	return(true);

}

int CDataOperations::getLongestWaveFile(vector<string> fnames)
{
	WaveFile func;
	int files_longest = 0;
	if (fnames.size() > 0) {
		CStatus progressDlg;
		progressDlg.CreateDlg();
		progressDlg.setLabel("Getting wave max length . . .");
		progressDlg.setProgress(0);
		func.addPanel1String("Getting wave max length");
		int mcnt = 0;
		int ttotal = 1 + (int)fnames.size();
		for (int i = 0; i < fnames.size(); i++) {
			func.addPanel1String("Getting wave size data : " + fnames[i]);
			wave_data wdata = func.getWaveDataFromFile(fnames[i]);
			if (wdata.length > files_longest) {
				files_longest = wdata.length;
			}
			mcnt++;
			if (mcnt > 10) {
				progressDlg.setProgress((100 * i) / (int)fnames.size());
				mcnt = 0;
			}
		}
		progressDlg.DestroyDlg();
	}
	func.addPanel1String("");
	return(files_longest);
}

bool CDataOperations::getMaxWaveMagnitude(vector<string> fnames, wave_check_results* wresults)
{
	WaveFile func;
	if (fnames == last_wave_fnames) {
		return(true);
	}
	wresults->files_shortest = 0;
	wresults->files_longest = 0;
	wresults->files_min = 0;
	wresults->files_max = 0;
	wresults->wave_stride = 0;
	wresults->wave_paths.clear();
	wresults->wave_data_lengths.clear();
	wresults->wave_start_indices.clear();
	wresults->num_copy_iterations.clear();
	if (fnames.size() > 0) {
		CStatus progressDlg;
		progressDlg.CreateDlg();
		progressDlg.setLabel("Getting wave size data . . .");
		progressDlg.setProgress(0);
		func.addPanel1String("Getting wave size data");
		vector<int> samplerates;
		vector<double> maxes;
		int mcnt = 0;
		int ttotal = 1 + (int)fnames.size();
		_int64 start_index = 0;
		for (int i = 0; i < fnames.size(); i++) {
			func.addPanel1String("Getting wave size data : " + fnames[i]);
			wave_data wdata = func.getWaveDataFromFile(fnames[i]);
			if (wdata.length > 0) {
				wresults->wave_paths.push_back(fnames[i]);
				wresults->wave_data_lengths.push_back(wdata.length);
				wresults->wave_start_indices.push_back(start_index);
				wresults->num_copy_iterations.push_back(1);
				samplerates.push_back(wdata.sample_rate);
				maxes.push_back(wdata.max);
				start_index += wdata.length;
				//char s[200];  sprintf_s(s, " : wdata length = %i", wdata.length);  addErrorString("getMaxWaveMagnitude : " + fnames[i] + s);
			}
			else {
				char s[200];  sprintf_s(s, " : wdata length = %i", wdata.length);  addErrorString("getMaxWaveMagnitude : " + fnames[i] + s);
			}
			mcnt++;
			if (mcnt > 10) {
				progressDlg.setProgress((100 * i) / (int)fnames.size());
				mcnt = 0;
			}
		}
		if ((samplerates.size() == maxes.size()) && (samplerates.size() == wresults->wave_paths.size()) && (maxes.size() > 0)) {
			func.addPanel1String("Getting max wave data");
			bool show_error = false;
			wresults->files_shortest = wresults->wave_data_lengths[0];
			wresults->files_longest = wresults->wave_data_lengths[0];
			wresults->files_min = maxes[0];
			wresults->files_max = maxes[0];
			int srate = samplerates[0];
			double max_sum = maxes[0];
			double max_sumsq = (maxes[0] * maxes[0]);
			for (int i = 1; i < maxes.size(); i++) {
				max_sum += maxes[i];
				max_sumsq += (maxes[i] * maxes[i]);
				if (srate != samplerates[i]) {
					show_error = true;
				}
				if (maxes[i] > wresults->files_max) {
					wresults->files_max = maxes[i];
				}
				if (maxes[i] < wresults->files_min) {
					wresults->files_min = maxes[i];
				}
				if (wresults->wave_data_lengths[i] > wresults->files_longest) {
					wresults->files_longest = wresults->wave_data_lengths[i];
				}
				if (wresults->wave_data_lengths[i] < wresults->files_shortest) {
					wresults->files_shortest = wresults->wave_data_lengths[i];
				}
			}
			double max_avg = max_sum / (double)maxes.size();
			double max_std = (max_sumsq - (max_sum * max_sum) / (double)maxes.size()) / (double)maxes.size();
			if (max_std > 0) {
				max_std = sqrt(max_std);
			}
			double max_limit = max_avg + (15.0 * max_std);
			for (int i = 0; i < maxes.size(); i++) {
				if (maxes[i] > max_limit) {
					char s[200];  sprintf_s(s, "Observation : wave magnitude = %.0f for file ", maxes[i]);
					char s2[200];  sprintf_s(s2, "  (set avg = %.2f, set std dev = %.2f)", max_avg, max_std);
					addErrorString(s + fnames[i] + s2);
				}
			}
			double center = (wresults->files_max + wresults->files_min) / 2;
			if (center > 0) {
				double test = (wresults->files_max - wresults->files_min) / center;
				if (test > 400) {
					show_error = true;
				}
			}
			if (show_error) {
				for (int i = 0; i < maxes.size(); i++) {
					char s[200];  sprintf_s(s, " : sample rate = %i : max = %.0f", samplerates[i], maxes[i]);
					addErrorString("getMaxWaveMagnitude : " +wresults->wave_paths[i] + s);
				}
			}
		}
		else {
			addErrorString("getMaxWaveMagnitude : size mismatch error");
		}
		progressDlg.DestroyDlg();
	}
	if (wresults->files_max <= 0) {
		wresults->files_max = 100;
	}
	func.addPanel1String("");
	last_wave_fnames = fnames;

	//char s[200];  sprintf_s(s, "getMaxWaveMagnitude : wave_paths size = %i : wave_data_lengths size = %i : wave_start_indices size = %i", (int)wresults->wave_paths.size(), (int)wresults->wave_data_lengths.size(), (int)wresults->wave_start_indices.size());
	//addErrorString(s);

	return(true);
}

bool CDataOperations::fillSoundFFTProcessData(int fft_size, int soundfftstride, int soundfftdownsample, wave_check_results* wresults, vector<dataset2>* processedData)
{
	// The goal here is to expand out the processed data for each file to the number of fft strides
	//   with the original data and leaving zeros to be filled in later with the calculated ffts
	
	WaveFile func;

	vector<bool> copyDataOnAdd;

	// Calculate and fill in processedData FFT data
	for (int i = 0; i < (int)processedData->size(); i++) {

		if (processedData[0][i].file_path != "") {

			// Getting the number of fft iterations and cross checking the calculated number
			int msteps = 0;
			int mlength = -1;
			for (int j = 0; j < wresults->wave_paths.size(); j++) {
				if (wresults->wave_paths[j] == processedData[0][i].file_path) {
					msteps = wresults->num_copy_iterations[j];
					mlength = wresults->wave_data_lengths[j];
					j = (int)wresults->wave_paths.size();
				}
			}
			int tsteps = mlength / ((soundfftdownsample * fft_size) + soundfftstride);
			if (msteps != tsteps) {
				char s[200];  sprintf_s(s, "fillSoundFFTProcessData : Error - msteps (%i) not equal to tsteps (%i) for ", msteps, tsteps);
				addErrorString(s + func.removePathFromFilename(processedData[0][i].file_path));
				return(false);
			}

			// copyDataOnAdd holds the columns that we want to copy as we add rows (i.e., not fft columns)
			copyDataOnAdd.clear();
			for (int n = 0; n < processedData[0][i].set_data.size(); n++) {
				copyDataOnAdd.push_back(true);
			}

			// For all fft columns, make copyDataOnAdd false
			string mname = "";
			for (int m = 0; m < fft_size; m++) {
				char s[200];  sprintf_s(s, "FFT%i:SoundFile", m);
				mname = s;
				for (int n = 0; n < processedData[0][i].set_data.size(); n++) {
					//addErrorString(processedData[i].set_data[n].header + " : " + mname);
					if (mname == processedData[0][i].set_data[n].header) {
						copyDataOnAdd[n] = false;
						n = (int)processedData[0][i].set_data.size();
					}
				}
			}

			// Expand processed data rows to contain the number of ffts we are going to process, copying original data and leaving ffts zero
			// For each column . . .
			for (int j = 0; j < processedData[0][i].set_data.size(); j++) {
				// This should be true by default - keeping in case some other error occurs that throws this off
				if (((int)processedData[0][i].set_data[j].column_data.size() != msteps) || ((int)processedData[0][i].set_data[j].column_string_data.size() != msteps)) {
					// Column size should be 1 before expansion or there is some other previous error in coding or conception
					if ((int)processedData[0][i].set_data[j].column_data.size() == 1) {
						// Another redundant verification - num to add should be fft steps minus one
						int numadd = msteps - (int)processedData[0][i].set_data[j].column_data.size();
						double double_add_data = 0.0;
						string string_add_data = "0.0";
						if (copyDataOnAdd[j]) {
							double_add_data = processedData[0][i].set_data[j].column_data[0];
							string_add_data = processedData[0][i].set_data[j].column_string_data[0];
						}
						for (int k = 0; k < numadd; k++) {
							processedData[0][i].set_data[j].column_data.push_back(double_add_data);
							processedData[0][i].set_data[j].column_string_data.push_back(string_add_data);
						}
					}
					else {
						char s[200];  sprintf_s(s, "fillSoundFFTProcessData : Error - train data sound step length error (%i : %i : %i) for ", j, msteps, (int)processedData[0][i].set_data[j].column_data.size());
						addErrorString(s + func.removePathFromFilename(processedData[0][i].file_path));
						return(false);
					}
				}
			}

		}
		else {
			addErrorString("fillSoundFFTData : Error - unable to process null file name");
			return(false);
		}

	}

	return(true);

}

bool CDataOperations::processSoundFFTData(int fft_size, int soundfftstride, int soundfftdownsample, bool normaiize_file_set, wave_check_results *wresults, vector<dataset2> *processedData)
{
	WaveFile func;

	if (fft_size > 0) {

		int myN = 2 * fft_size;

		if (wresults->files_max <= 0) {
			addErrorString("processSoundFFTData : Error - unable to process FFT data with file size max = 0");
			return(false);
		}

		vector<double> mdata;
		vector<vector<int>> dataIndices;

		vector<double> process_maxes;

		int mcnt = 0;

		CStatus myProgressDlg;
		myProgressDlg.CreateDlg();
		myProgressDlg.setLabel(_T("Processing Sound FFT Data . . ."));
		myProgressDlg.setProgress(0);

		// Calculate and fill in processedData FFT data
		for (int i = 0; i < (int)processedData->size(); i++) {

			if ((processedData[0][i].file_path != "") && (processedData[0][i].set_data.size() > 0)) {

				mcnt++;
				if (mcnt > 20) {
					myProgressDlg.setProgress((100 * i) / (int)processedData->size());
					mcnt = 0;
				}

				int msteps = 0;
				int mlength = -1;
				for (int j = 0; j < wresults->wave_paths.size(); j++) {
					if (wresults->wave_paths[j] == processedData[0][i].file_path) {
						msteps = wresults->num_copy_iterations[j];
						mlength = wresults->wave_data_lengths[j];
						j = (int)wresults->wave_paths.size();
					}
				}
				int tsteps = mlength / ((soundfftdownsample * fft_size) + soundfftstride);
				if (msteps != tsteps) {
					char s[200];  sprintf_s(s, "processSoundFFTData : Error - msteps (%i) not equal to tsteps (%i) for ", msteps, tsteps);
					addErrorString(s + func.removePathFromFilename(processedData[0][i].file_path));
					myProgressDlg.DestroyDlg();
					return(false);
				}
				if (msteps != (int)processedData[0][i].set_data[0].column_data.size()) {
					char s[200];  sprintf_s(s, "processSoundFFTData : Error - msteps (%i) not equal to column_data size (%i) for ", msteps, (int)processedData[0][i].set_data[0].column_data.size());
					addErrorString(s + func.removePathFromFilename(processedData[0][i].file_path));
					myProgressDlg.DestroyDlg();
					return(false);
				}

				vector<int> tindoces;
				dataIndices.push_back(tindoces);
				string mname = "";
				for (int m = 0; m < fft_size; m++) {
					char s[200];  sprintf_s(s, "FFT%i:SoundFile", m);
					mname = s;
					for (int n = 0; n < processedData[0][i].set_data.size(); n++) {
						if (mname == processedData[0][i].set_data[n].header) {
							dataIndices[i].push_back(n);
							n = (int)processedData[0][i].set_data.size();
						}
					}
				}
				if (fft_size != (int)dataIndices[i].size()) {
					addErrorString("fillSoundFFTData : Error - mismatch for dataIndices.size in " + func.removePathFromFilename(processedData[0][i].file_path));
					myProgressDlg.DestroyDlg();
					return(false);
				}

				int max_length = wresults->files_longest;
				if ((mlength <= 0) || (max_length <= 0)) {
					addErrorString("processSoundFFTData : Error - unable to process FFT data with saved length = 0 for " + func.removePathFromFilename(processedData[0][i].file_path));
					myProgressDlg.DestroyDlg();
					return(false);
				}

				for (int j = 0; j < processedData[0][i].set_data.size(); j++) {
					if ((processedData[0][i].set_data[j].column_data.size() != msteps) || (processedData[0][i].set_data[j].column_string_data.size() != msteps)) {
						char s[200];  sprintf_s(s, "processSoundFFTData : Error - train data sound step length error (%i : %i : %i) for ", j, msteps, (int)processedData[0][i].set_data[j].column_data.size());
						addErrorString(s + func.removePathFromFilename(processedData[0][i].file_path));
						myProgressDlg.DestroyDlg();
						return(false);
					}
				}

				wave_data wdata = func.getWaveDataFromFile(processedData[0][i].file_path);

				if (wdata.length > 0) {
					int data_length = wdata.length;
					if ((data_length == mlength) && (data_length <= max_length)) {
						double max = 0;
						bool display_msg = true;
						for (int j = 0; j < msteps; j++) {
							mdata.clear();
							mdata = getFFT2((j * soundfftstride), myN, soundfftdownsample, wdata.data);
							if ((int)mdata.size() == fft_size) {
								for (int m = 0; m < mdata.size(); m++) {
									processedData[0][i].set_data[dataIndices[i][m]].column_data[j] = mdata[m];
									if (mdata[m] > max) {
										max = mdata[m];
									}
									//if (i == 0) {
										//char s[200];  sprintf_s(s, "i = %i : j = %i : m = %i : %.4f", i, j, m, mdata[m]); addErrorString(s);
									//}
								}
							}
							else if (display_msg) {
								char s[200];  sprintf_s(s, "fillSoundFFTData : mdata.size (%i) not equal to fft_size (%i) for ", (int)mdata.size(), fft_size);
								addErrorString(s + func.removePathFromFilename(processedData[0][i].file_path));
								display_msg = false;
							}
						}
						if (max > 0) {
							process_maxes.push_back(max);
						}
						else {
							addErrorString("fillSoundFFTData : Error - no sound data for " + func.removePathFromFilename(processedData[0][i].file_path));
							myProgressDlg.DestroyDlg();
							return(false);
						}

					}
					else {
						char s[200];  sprintf_s(s, "fillSoundFFTData : Error - sound data length error (%i / %i) for ", data_length, mlength);
						addErrorString(s + func.removePathFromFilename(processedData[0][i].file_path));
						myProgressDlg.DestroyDlg();
						return(false);
					}
				}
				else {
					addErrorString("fillSoundFFTData : Error - unable to load sound data for " + func.removePathFromFilename(processedData[0][i].file_path));
					myProgressDlg.DestroyDlg();
					return(false);
				}

			}
			else {
				addErrorString("fillSoundFFTData : Error - unable to process null file name");
				myProgressDlg.DestroyDlg();
				return(false);
			}

		}

		if (process_maxes.size() != processedData->size()) {
			char s[200];  sprintf_s(s, "fillSoundFFTData : Error - process_maxes size (%i) not equal to processedData size (%i)", (int)process_maxes.size(), (int)processedData->size());
			addErrorString(s);
			myProgressDlg.DestroyDlg();
			return(false);
		}


		// Normalize FFT data
		myProgressDlg.setLabel(_T("Normalizing Sound FFT Data . . ."));
		CString s = _T("");
		CString mname = _T("");

		double normalization_max = 0;
		for (int j = 0; j < process_maxes.size(); j++) {
			if (process_maxes[j] > normalization_max) {
				normalization_max = process_maxes[j];
			}
		}

		for (int i = 0; i < processedData->size(); i++) {
			mcnt++;
			if (mcnt > 20) {
				myProgressDlg.setProgress((100 * i) / (int)processedData->size());
				mcnt = 0;
			}
			int msteps = (int)processedData[0][i].set_data[0].column_data.size();;
			double tdata = 0;
			if (!normaiize_file_set) {
				normalization_max = process_maxes[i];
			}
			if (normalization_max > 0) {
				for (int j = 0; j < msteps; j++) {
					// Here we are ignoring all previous normalizations and normalizing to the zero frequency fft component 
					//   (this seems to make the best normalization sense)
					if (processedData[0][i].set_data[dataIndices[i][0]].column_data[j] > 0) {
						normalization_max = processedData[0][i].set_data[dataIndices[i][0]].column_data[j];
					}
					for (int m = 0; m < fft_size; m++) {
						tdata = processedData[0][i].set_data[dataIndices[i][m]].column_data[j] / normalization_max;
						processedData[0][i].set_data[dataIndices[i][m]].column_data[j] = tdata;
						//if (i == 0) {
							//char s[200];  sprintf_s(s, "i = %i : j = %i : m = %i : %.4f", i, j, m, tdata); addErrorString(s);
						//}
					}
				}
			}
			else {
				myProgressDlg.DestroyDlg();
				addErrorString("fillSoundFFTData : Error - normalization_max = 0");
				return(false);
			}
			if (myProgressDlg.cancel) {
				myProgressDlg.DestroyDlg();
				return(false);
			}
		}

		myProgressDlg.DestroyDlg();

	}

	return(true);
}









void CDataOperations::addErrorString(string err) {
#ifdef GCC
	cout << err << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString((CString)err.c_str());
#endif
}

