
#include "pch.h"
#include "MainFrm.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "AuxFunctions.h"
#include "CFileOperations.h"
#include "WaveFile.h"
#include <iostream>
#include <fstream>

#include <string>
#include <algorithm>
#include <cctype>



//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************

//typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);


bool CLineParser::isNumber(char data)
{
	if ((data >= 48) && (data <= 57)) {
		return(true);
	}
	else if ((data == 46) || (data == 43) || (data == 45)) {
		return(true);
	}
	return(false);
}

bool CLineParser::isOpenParenthesis(char data)
{
	if (data == 40) {
		return(true);
	}
	return(false);
}

bool CLineParser::isCloseParenthesis(char data)
{
	if (data == 41) {
		return(true);
	}
	return(false);
}

bool CLineParser::isForwardSlash(char data)
{
	if (data == 47) {
		return(true);
	}
	return(false);
}

bool CLineParser::isBackSlash(char data)
{
	if (data == '\\') {
		return(true);
	}
	return(false);
}

bool CLineParser::isColon(char data)
{
	if (data == ':') {
		return(true);
	}
	return(false);
}

bool CLineParser::isComma(char data)
{
	if (data == ',') {
		return(true);
	}
	return(false);
}

bool CLineParser::isUnderscore(char data)
{
	if (data == '_') {
		return(true);
	}
	return(false);
}

bool CLineParser::isSpace(char data)
{
	if (data == ' ') {
		return(true);
	}
	return(false);
}

bool CLineParser::isWhiteSpace(char data)
{
	if ((data == ' ') || (data == 9)) {
		return(true);
	}
	return(false);
}

bool CLineParser::isSeparator(char data)
{
	if ((data == 32) || (data == 44) || (data == 58)) {
		return(true);
	}
	return(false);
}

bool CLineParser::isSeparator2(char data)
{
	if ((data == 32) || (data == 44) || (data == 58) || isOpenParenthesis(data) || isCloseParenthesis(data) || isForwardSlash(data) || isEOL(data) || (data == '+')) {
		return(true);
	}
	return(false);
}

bool CLineParser::isSeparator3(char data)
{
	if (isSeparator2(data) || (data == '_') || (data == ';')) {
		return(true);
	}
	return(false);
}

bool CLineParser::isSeparator4(char data)
{
	if ((data == 44)) {
		return(true);
	}
	return(false);
}

bool CLineParser::isValidCharacter(char data)
{
	if (((data >= 48) && (data <= 57)) ||
		((data >= 65) && (data <= 90)) ||
		((data >= 97) && (data <= 122)) ||
		(data == 39)) {
		return(true);
	}
	return(false);
}

bool CLineParser::isEOL(char mdata)
{
	if ((mdata == 10) || (mdata == 13)) {
		return(true);
	}
	return(false);
}

int CLineParser::findSeparator(int index, string line)
{
	for (int i = index; i < line.length(); i++) {
		if (isSeparator4((char)line[i]) || isEOL((char)line[i])) {
			return(i);
		}
	}
	return((int)line.length());
}

int CLineParser::findOpenParenthesis(int index, string line)
{
	for (int i = index; i < line.length(); i++) {
		if (isOpenParenthesis((char)line[i])) {
			return(i + 1);
		}
	}
	return((int)line.length());
}

int CLineParser::findCloseParenthesis(int index, string line)
{
	for (int i = index; i < line.length(); i++) {
		if (isCloseParenthesis((char)line[i])) {
			return(i + 1);
		}
	}
	return((int)line.length());
}

int CLineParser::findForwardSlash(int index, string line)
{
	for (int i = index; i < line.length(); i++) {
		if (isForwardSlash((char)line[i])) {
			return(i + 1);
		}
	}
	return((int)line.length());
}

int CLineParser::findLastSpace(string line)
{
	for (int i = ((int)line.length() - 1); i >= 0; i--) {
		if (isSeparator((char)line[i])) {
			return(i + 1);
		}
	}
	return(0);
}

int CLineParser::findUnderscore(int index, string line)
{
	for (int i = index; i < (int)line.length(); i++) {
		if (isUnderscore((char)line[i])) {
			return(i + 1);
		}
	}
	return((int)line.length());
}

bool CLineParser::isInt(string data)
{
	for (int i = 0; i < (int)data.length(); i++) {
		if (!((data[i] >= 48) && (data[i] <= 57)) && (data[i] != 43) && (data[i] != 45)) {
			return(false);
		}
	}
	return(true);
}

int CLineParser::getNextWord(int start, string data, string* word)
{
	int index = start;
	bool done = false;
	while (!done) {
		if (index < data.length()) {
			if (isValidCharacter((char)data[index])) {
				index++;
			}
			else {
				done = true;
			}
		}
		else {
			done = true;
		}
	}
	word[0] = data.substr(start, (int64_t)index - start);
	return(index);
}

bool CLineParser::isWordNumber(string word)
{
	int ecnt = 0;
	bool leading_space = true;
	for (int i = 0; i < word.length(); i++) {
		if (!isNumber((char)word[i]) && ((char)word[i] != 'e') && ((char)word[i] != 'E') && ((char)word[i] != ' ')) {
			return(false);
		}
		else if (((char)word[i] == 'e') || ((char)word[i] == 'E')) {
			ecnt++;
			leading_space = false;
		}
		else if ((char)word[i] == ' ') {
			if (!leading_space) {
				return(false);
			}
		}
		else {
			leading_space = false;
		}
	}
	if (ecnt < 2) {
		return(true);
	}
	return(false);
}

int CLineParser::myStoi(string s)
{
	if (isWordNumber(s)) {
		return(stoi(s));
	}
	addErrorString(s);
	return(0);
}

bool CLineParser::hasSingleColon(string cname)
{
	for (int i = 0; i < cname.length(); i++) {
		if (cname[i] == ':') {
			if (i < (cname.length() - 1)) {
				if (cname[i + 1] != ':') {
					return(true);
				}
			}
			else {
				return(true);
			}
		}
	}
	return(false);
}

string CLineParser::getStringBeforeColon(string cname)
{
	string mname = cname;
	for (int i = 0; i < mname.length(); i++) {
		if (mname[i] == ':') {
			return(mname.substr(0, i));
		}
	}
	return(cname);
}

string CLineParser::getStringAfterColon(string cname)
{
	string mname = cname;
	for (int i = 0; i < mname.length(); i++) {
		if (mname[i] == ':') {
			return(mname.substr(i + 1));
		}
	}
	return(cname);
}

string CLineParser::getStringAfterLastColon(string cname)
{
	string tname = "";
	string mname = cname;
	bool done = false;
	while (!done) {
		if (hasSingleColon(mname)) {
			string bname = getStringBeforeColon(mname);
			if ((bname.substr(0, 6) != "Output") && (bname.substr(0, 5) != "Index") && (bname.substr(0, 3) != "FFT")) {
				done = true;
			}
			else {
				tname = mname;
				mname = getStringAfterColon(mname);
				if (tname == mname) {
					done = true;
				}
			}
		}
		else {
			done = true;
		}
	}
	return(mname);
}

int CLineParser::getNumberFromString(string cname)
{
	string mname = cname;
	for (int i = 0; i < mname.length(); i++) {
		string t3 = cname.substr(i);
		//addErrorString("getNumberFromString : " + mname + " : " + t3);
		if (isWordNumber(t3)) {
			return(myStoi(t3));
		}
	}
	return(-1);
}

string CLineParser::makeLower(string mstring)
{
	string rval = mstring;
	transform(rval.begin(), rval.end(), rval.begin(),::tolower);
	return(rval);
}

vector<string> CLineParser::parseLine(string line)
{
	vector<string> csvariables;
	long index = 0;
	string data = "";
	while (index < line.length()) {
		if (isComma(line[index])) {
			csvariables.push_back(data);
			data = "";
			index++;
		}
		else {
			data += line[index];
			index++;
		}
	}
	if (data != "") {
		csvariables.push_back(data);
	}
	return(csvariables);
}

vector<string> CLineParser::parseLine2(string line)
{
	long index = 0;
	string data = "";
	vector<string> rval;
	while (index < line.length()) {
		if (isUnderscore((char)line[index]) || isSpace((char)line[index]) || isBackSlash((char)line[index])) {
			rval.push_back(data);
			data = "";
		}
		else {
			data += line[index];
		}
		index++;
	}
	if (data != "") {
		rval.push_back(data);
	}
	return(rval);
}

unsigned char* CLineParser::downscaleImageData(int inrows, int incols, int instride, unsigned char* rgbin, int outrows, int outcols, unsigned char* rgbout)
{
	CString s = _T("");

	if ((inrows <= 0) || (incols <= 0) || (outrows <= 0) || (outcols <= 0) || (inrows < outrows) || (incols < outcols)) {
		char s[200];  sprintf_s(s, "downscaleImageData error : inrows=%i, incols=%i, outrows=%i, outcols=%i", inrows, incols, outrows, outcols);
		addErrorString(s);
		return(rgbout);
	}

	int minrows = inrows;  // -18;
	int mincols = incols;

	float reduction = (float)minrows / (float)outrows;
	if (((float)mincols / (float)outcols) > reduction) {
		reduction = (float)mincols / (float)outcols;
	}

	int index = 0;
	float rstartval = 0;
	float rendval = reduction;
	float cstartval = 0;
	float cendval = reduction;

	int trows = (int)((((float)minrows) - reduction) / reduction);
	int tcols = (int)((((float)mincols) - reduction) / reduction);
	int xoff = (outcols - tcols) / 2;
	int yoff = (outrows - trows) / 2;

	unsigned char gray = 50;

	for (int i = 0; i < outrows; i++) {
		for (int j = 0; j < outcols; j++) {
			index = (i * outcols * 3) + (j * 3);
			rgbout[index + 0] = gray;
			rgbout[index + 1] = gray;
			rgbout[index + 2] = gray;
		}
	}

	rgb_value clr;
	clr.red = 0;
	clr.green = 0;
	clr.blue = 0;

	int rindex = 0;
	int cindex = 0;

	while (rstartval < ((float)minrows) - reduction) {

		while (cstartval < ((float)mincols - reduction)) {

			clr = downscaleImageRowSlice(rstartval, rendval, cstartval, cendval, instride, rgbin);

			index = ((rindex + yoff) * outcols * 3) + ((cindex + xoff) * 3);

			if ((index + 2) < (outrows * outcols * 3)) {

				rgbout[index + 0] = (unsigned char)clr.red;
				rgbout[index + 1] = (unsigned char)clr.green;
				rgbout[index + 2] = (unsigned char)clr.blue;

			}
			cstartval += reduction;
			cendval += reduction;

			if (cindex < (outcols - 3)) {
				cindex++;
			}

		}

		cstartval = 0;
		cendval = reduction;
		rstartval += reduction;
		rendval += reduction;

		cindex = 0;
		if (rindex < (outrows - 3)) {
			rindex++;
		}

	}

	return(rgbout);

}

rgb_value CLineParser::downscaleImageRowSlice(float rstart, float rend, float cstart, float cend, int rstride, unsigned char* rgbin)
{
	rgb_value rval;
	rval.red = 0;
	rval.green = 0;
	rval.blue = 0;

	rgb_value tclr;
	tclr.red = 0;
	tclr.green = 0;
	tclr.blue = 0;

	int tindex = (int)rstart;
	int endindex = (int)rend;
	int rowoffset = 0;

	float sumr = 0.0F;
	float sumg = 0.0F;
	float sumb = 0.0F;
	float div = 0.0F;
	float factor = 1.0F;

	if ((float)tindex < rstart) {
		factor = (float)(tindex + 1) - rstart;
		rowoffset = tindex * rstride/* * 3*/;
		tclr = downscaleImageColumnSlice(cstart, cend, rowoffset, rgbin);
		sumr += (factor * tclr.red);
		sumg += (factor * tclr.green);
		sumb += (factor * tclr.blue);
		div += factor;
		tindex++;
	}

	for (int i = tindex; i < endindex; i++) {
		rowoffset = i * rstride/* * 3*/;
		tclr = downscaleImageColumnSlice(cstart, cend, rowoffset, rgbin);
		sumr += tclr.red;
		sumg += tclr.green;
		sumb += tclr.blue;
		div += 1.0F;
	}

	if ((float)endindex < rend) {
		factor = rend - (float)endindex;
		rowoffset = endindex * rstride/* * 3*/;
		tclr = downscaleImageColumnSlice(cstart, cend, rowoffset, rgbin);
		sumr += (factor * tclr.red);
		sumg += (factor * tclr.green);
		sumb += (factor * tclr.blue);
		div += factor;
	}

	if (div > 0) {
		rval.red = sumr / div;
		rval.green = sumg / div;
		rval.blue = sumb / div;
	}

	return(rval);

}

rgb_value CLineParser::downscaleImageColumnSlice(float start, float end, int rowoffset, unsigned char* rgbin)
{
	rgb_value rval;
	rval.red = 0;
	rval.green = 0;
	rval.blue = 0;

	int tindex = (int)start;
	int endindex = (int)end;

	float sumr = 0.0F;
	float sumg = 0.0F;
	float sumb = 0.0F;
	float div = 0.0F;
	float factor = 1.0F;

	if ((float)tindex < start) {
		factor = (float)(tindex + 1) - start;
		sumr += (factor * rgbin[rowoffset + (3 * tindex) + 0]);
		sumg += (factor * rgbin[rowoffset + (3 * tindex) + 1]);
		sumb += (factor * rgbin[rowoffset + (3 * tindex) + 2]);
		div += factor;
		tindex++;
	}

	for (int i = tindex; i < endindex; i++) {
		sumr += rgbin[rowoffset + (3 * i) + 0];
		sumg += rgbin[rowoffset + (3 * i) + 1];
		sumb += rgbin[rowoffset + (3 * i) + 2];
		div += 1.0F;
	}

	if (end > (float)endindex) {
		factor = end - (float)endindex;
		sumr += (factor * rgbin[rowoffset + (3 * endindex) + 0]);
		sumg += (factor * rgbin[rowoffset + (3 * endindex) + 1]);
		sumb += (factor * rgbin[rowoffset + (3 * endindex) + 2]);
		div += factor;
	}

	if (div > 0) {
		rval.red = sumr / div;
		rval.green = sumg / div;
		rval.blue = sumb / div;
	}

	return(rval);

}

string CLineParser::removePathFromFilename(string path)
{
	string rval = path;
	int i = (int)rval.length() - 1;
	while ((i >= 0) && (rval[i] != '\\')) {
		i--;
	}
	rval = rval.substr(/*rval.length() - **/i + 1);
	return(rval);
}

string CLineParser::getFileExtension(string fname)
{
	string rval = fname;
	int i = (int)rval.length() - 1;
	while ((i >= 0) && (rval[i] != '.')) {
		i--;
	}
	if (i <= 0) {
		rval = "";
	}
	else {
		rval = rval.substr(i + 1);
	}
	//addErrorString(rval);
	return(rval);
}


void CLineParser::addErrorString(string err) {
#ifdef GCC
	cout << err << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString((CString)err.c_str());
#endif
}

void CLineParser::addPanel1String(string msg)
{
#ifdef GCC
	cout << msg << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addPanel1String((CString)msg.c_str());
#endif
}





//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************

File::File(void)
{
	init();
}

File::~File(void)
{
}

void File::init()
{
	filename = "";
	display_error = true;
	flines.clear();
}

bool File::loadFile(string fname, bool derror)
{
	flines.clear();
	filename = fname;
	display_error = derror;
	ifstream inputFile(filename);
	if (inputFile.is_open()) {
		string line;
		while (getline(inputFile, line)) {
			flines.push_back(line);
		}
		inputFile.close();
	}
	else {
		if (display_error) {
			addErrorString("Error opening file " + filename);
		}
		return(false);
	}
	return(true);
}

void File::addErrorString(string err) {
#ifdef GCC
	cout << err << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString((CString)err.c_str());
#endif
}



//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************

CSVFile::CSVFile(void)
{
}

CSVFile::~CSVFile(void)
{
}

void CSVFile::init()
{
	File::init();
	headers.clear();
	sdata.clear();
	ddata.clear();
	columnIsNumbersData.clear();
}

bool CSVFile::isCSVFile(string fname)
{
	string ext = getFileExtension(fname);
	transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	if (ext == "csv") {
		return(true);
	}
	return(false);
}

bool CSVFile::loadCSVFile(string fname, bool derror)
{
	init();
	if (loadFile(fname, derror)) {
		return(parseCSVFile(derror));
	}
	return(false);
}

bool CSVFile::parseCSVFile(bool derror)
{
	if (flines.size() > 1) {
		bool get_headers = true;
		for (int i = 0; i < (int)flines.size(); i++) {
			//AfxMessageBox((CString)flines[i].c_str());
			vector<string> data = parseLine(flines[i]);
			if (data.size() > 0) {
				if (get_headers) {
					bool all_numbers = true;
					for (int j = 0; j < (int)data.size(); j++) {
						if (!isWordNumber(data[j])) {
							all_numbers = false;
							j = (int)data.size();
						}
					}
					if (all_numbers) {
						if (derror) {
							addErrorString("Parsing " + filename + " unable to find valid headers - exiting");
						}
						return(false);
					}
					else {
						headers = data;
					}
					get_headers = false;
				}
				else {
					if (data.size() != headers.size()) {
						if (derror) {
							addErrorString("Parsing " + filename + " finding num headers to num data mismatch - exiting");
						}
						return(false);
					}
					if ((int)sdata.size() == 0) {
						if ((int)ddata.size() != 0) {
							if (derror) {
								addErrorString("Parsing " + filename + " finding sdata to ddata size mismatch - exiting");
							}
							return(false);
						}
						for (int j = 0; j < (int)data.size(); j++) {
							vector<string> tsdata;
							sdata.push_back(tsdata);
							vector<double> tddata;
							ddata.push_back(tddata);
							bool bdata = true;
							columnIsNumbersData.push_back(bdata);
						}
					}
					if (data.size() == sdata.size()) {
						for (int j = 0; j < (int)data.size(); j++) {
							sdata[j].push_back(data[j]);
							if (isWordNumber(data[j])) {
								ddata[j].push_back(stold(data[j]));
							}
							else {
								ddata[j].push_back(0.0);
								columnIsNumbersData[j] = false;
							}
						}
					}
					else {
						if (derror) {
							char s[200];  sprintf_s(s, " : data size = %i : sdata size = %i", (int)data.size(), (int)sdata.size());
							addErrorString("Parsing " + filename + " size mismatch" + s);
						}
						return(false);
					}
				}
			}
		}
		//AfxMessageBox(_T("parseCSVFile returning true"));
		return(true);
	}
	if (derror) {
		addErrorString("Parsing " + filename + " not finding enough lines - exiting");
	}
	//AfxMessageBox(_T("parseCSVFile returning false"));
	return(false);
}

int CSVFile::getColumnIndexFromHeader(string hname)
{
	if (headers.size() > 0) {
		for (int i = 0; i < (int)headers.size(); i++) {
			if (hname == headers[i]) {
				return(i);
			}
		}
	}
	//char s[200];  sprintf_s(s, " : headers size = %i : name = ", (int)headers.size());
	//addErrorString("getColumnIndexFromHeader for " + filename + " : index past headers size" + s + hname);
	return(-1);
}

bool CSVFile::isColumnNumbersData(int cindex)
{
	if (cindex < (int)columnIsNumbersData.size()) {
		return(columnIsNumbersData[cindex]);
	}
	char s[200];  sprintf_s(s, " : cindex = %i : columnIsNumbersData size = %i", cindex, (int)columnIsNumbersData.size());
	addErrorString("isColumnNumbersData for " + filename + " : cindex past columnIsNumbersData size" + s);
	return(false);
}

string CSVFile::getStringData(int cindex, int rindex)
{
	if (cindex < (int)sdata.size()) {
		if (rindex < (int)sdata[cindex].size()) {
			return(sdata[cindex][rindex]);
		}
	}
	addErrorString("getStringData for " + filename + " : invalid cindex or rindex");
	return("");
}

vector<string> CSVFile::getColumnStringData(int cindex)
{
	if (cindex < (int)sdata.size()) {
		return(sdata[cindex]);
	}
	addErrorString("getColumnStringData for " + filename + " : cindex past sdata size");
	vector<string> nada;
	return(nada);
}

vector<double> CSVFile::getColumnDoubleData(int cindex)
{
	if (cindex < (int)ddata.size()) {
		return(ddata[cindex]);
	}
	addErrorString("getColumnDoubleData for " + filename + " : cindex past ddata size");
	vector<double> nada;
	return(nada);
}



//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************

JPGFile::JPGFile(void)
{
	init();
}

JPGFile::~JPGFile(void)
{
}

void JPGFile::init()
{
	File::init();
}

bool JPGFile::isJPGFile(string fname)
{
	string ext = getFileExtension(fname);
	transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	if (ext == "jpg") {
		return(true);
	}
	return(false);
}



//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************

RawFile::RawFile(void)
{
	init();
}

RawFile::~RawFile(void)
{
}

void RawFile::init()
{
	File::init();
}

bool RawFile::isRawFile(string fname)
{
	string ext = getFileExtension(fname);
	transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	if (ext == "raw") {
		return(true);
	}
	return(false);
}



//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************

WaveFile::WaveFile(void)
{
	init();
}

WaveFile::~WaveFile(void)
{
}

void WaveFile::init()
{
	File::init();
	minimum_sound_level = 20;
	normaiize_file_set = false;
}

bool WaveFile::isWaveFile(string fname)
{
	string ext = getFileExtension(fname);
	transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	if (ext == "wav") {
		return(true);
	}
	return(false);
}

wave_data WaveFile::getWaveDataFromFile(string fname)
{
	wave_data rval;
	rval.length = 0;
	rval.file_length = 0;
	rval.sample_rate = 0;
	rval.max = 0.0;
	CWaveFile wavefile;
	if (wavefile.ReadWaveFile(fname)) {
		rval.file_length = wavefile.data_length;
		rval.sample_rate = wavefile.sample_rate;
		int cnt = 0;
		double sum = 0;
		double sumsq = 0;
		for (int i = 0; i < wavefile.data_length; i++) {
			if ((double)wavefile.wave_data[i] > rval.max) {
				rval.max = (double)wavefile.wave_data[i];
			}
			else if ((double)wavefile.wave_data[i] < -rval.max) {
				rval.max = -(double)wavefile.wave_data[i];
			}
		}
		if (rval.max > 0) {
			double tvalue = minimum_sound_level;
			double nvalue = rval.max;
			double fvalue = 100;// pDoc->getDataWavMax();
			if (normaiize_file_set) {
				/*if (pDoc->wresults.files_max > 0) {
					if (pDoc->wresults.files_max >= nvalue) {
						nvalue = pDoc->wresults.files_max;
					}
					else if (fvalue >= nvalue) {
						nvalue = fvalue;
					}
					else {
						char s[200];  sprintf_s(s, "getWaveDataFromFile : resetting max from wresults.files_max (%.1f) to file max (%.1f) for file ", pDoc->wresults.files_max, rval.max);
						addErrorString(s + fname);
					}
				}
				else */if (fvalue >= nvalue) {
					nvalue = fvalue;
				}
				/*else {
					char s[200];  sprintf_s(s, "getWaveDataFromFile : resetting max from wresults.files_max (%.1f) to file max (%.1f) for file ", pDoc->wresults.files_max, rval.max);
					addErrorString(s + fname);
				}*/
			}
			bool done = false;
			while (!done) {
				int data_start = 0;
				for (int i = 0; i < wavefile.data_length; i++) {
					if (((double)wavefile.wave_data[i] > tvalue) || ((double)wavefile.wave_data[i] < -tvalue)) {
						data_start = i;
						i = wavefile.data_length;
					}
				}
				int data_end = wavefile.data_length;
				for (int i = (wavefile.data_length - 1); i > data_start; i--) {
					if (((double)wavefile.wave_data[i] > tvalue) || ((double)wavefile.wave_data[i] < -tvalue)) {
						data_end = i;
						i = -1;
					}
				}
				rval.data.clear();
				if ((data_end - data_start) > (rval.file_length / 4)) {
					for (int i = data_start; i < data_end; i++) {
						rval.data.push_back((double)wavefile.wave_data[i] / nvalue);
					}
					rval.length = (int)rval.data.size();
					done = true;
				}
				else {
					if (tvalue == 0) {
						done = true;
					}
					tvalue = tvalue / 2;
				}
			}
			if (rval.length == 0) {
				addErrorString("getWaveDataFromFile : Unable to process data for " + fname);
				rval.max = 0.0;
			}
		}
	}
	return(rval);
}






