

#include "pch.h"
#include "framework.h"
#include "MainFrm.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "AuxFunctions.h"
#include "WaveFile.h"
#include "CFileTypeSelectDlg.h"

#include <sys\timeb.h>
#include <time.h>
#include <ppl.h>
#include <numeric>
#include <math.h>
#include <VersionHelpers.h>


void Timer::Start()
{
	unsigned long time, millitime;
	struct _timeb tstruct;
	_ftime_s(&tstruct);
	millitime = (unsigned long)tstruct.millitm;
	time = (unsigned long)tstruct.time;
	start_time = (double)time + ((double)millitime) / 1000;
}

void Timer::Hold()
{
	unsigned long time, millitime;
	double now;
	struct _timeb tstruct;
	_ftime_s(&tstruct);
	millitime = (unsigned long)tstruct.millitm;
	time = (unsigned long)tstruct.time;
	now = (double)time + ((double)millitime) / 1000;
	hold_time = now - start_time;;
}

void Timer::Restart()
{
	Start();
	start_time = start_time - hold_time;
}

double Timer::GetDelay()
{
	unsigned long time, millitime;
	double now, delay;
	CString sbuf;

	struct _timeb tstruct;

	_ftime_s(&tstruct);
	time = (unsigned long)tstruct.time;
	millitime = (unsigned long)tstruct.millitm;
	now = (double)time + ((double)millitime) / 1000;

	delay = now - start_time;

	return(delay);

}



//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************
//************************************************************************************************************


CAuxFunctions::CAuxFunctions(void) 
{ 
	errordisplayed = false;  
	num_ones = 4; 

	last_wave_fnames.clear();
	last_wave_check_results.files_shortest = 0;
	last_wave_check_results.files_longest = 0;
	last_wave_check_results.files_min = 0;
	last_wave_check_results.files_max = 0;
	last_wave_check_results.wave_stride = 0;
	last_wave_check_results.wave_paths.clear();
	last_wave_check_results.wave_data_lengths.clear();
	last_wave_check_results.num_copy_iterations.clear();

};

void CAuxFunctions::addErrorString(string err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString((CString)err.c_str());
}

void CAuxFunctions::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

void CAuxFunctions::addPanel1String(CString msg)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addPanel1String(msg);
}

CString CAuxFunctions::getFileExtension(CString fname)
{
	CString rval = fname;
	int i = rval.GetLength() - 1;
	while ((i >= 0) && (rval[i] != '.')) {
		i--;
	}
	rval = rval.Right(rval.GetLength() - i - 1);
	return(rval);
}

CString CAuxFunctions::getFileExtension2(CString fname)
{
	CString rval = fname;
	int i = rval.GetLength() - 1;
	while ((i >= 0) && (rval[i] != '.')) {
		i--;
	}
	if (i <= 0) {
		rval = _T("");
	}
	else {
		rval = rval.Right(rval.GetLength() - i - 1);
	}
	return(rval);
}

CString CAuxFunctions::removePathFromFilename(CString path)
{
	CString rval = path;
	int i = rval.GetLength() - 1;
	while ((i >= 0) && (rval[i] != '\\')) {
		i--;
	}
	rval = rval.Right(rval.GetLength() -  i - 1);
	return(rval);
}

CString CAuxFunctions::getPathFromFilename(CString path)
{
	CString rval = removeExtensionFromFilename(path);
	int i = rval.GetLength() - 1;
	while ((i >= 0) && (rval[i] != '\\')) {
		i--;
	}
	if (i > 0) {
		rval = rval.Left(i);
	}
	return(rval);
}

CString CAuxFunctions::removeExtensionFromFilename(CString path)
{
	CString rval = path;
	int i = rval.GetLength() - 1;
	while ((i >= 0) && (rval[i] != '.')) {
		i--;
	}
	if (i > 0) {
		rval = rval.Left(i);
	}
	return(rval);
}

bool CAuxFunctions::directoryExists(CString path)
{
	DWORD dwAttrib = GetFileAttributes(path);

	// If GetFileAttributes returns INVALID_FILE_ATTRIBUTES,
	// the directory does not exist or an error occurred.
	if (dwAttrib == INVALID_FILE_ATTRIBUTES)
	{
		// You can check GetLastError() here for more specific error information
		// For example, if GetLastError() == ERROR_FILE_NOT_FOUND or ERROR_PATH_NOT_FOUND,
		// then the directory truly doesn't exist.
		return false;
	}

	// Check if the attribute indicates it's a directory
	return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void CAuxFunctions::RecursiveDelete(CString szPath)
{
	CFileFind finder;
	CString strWildcard = szPath + _T("\\*.*");
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
		{
			// Skip "." and ".." entries
			continue;
		}

		if (finder.IsDirectory())
		{
			// Recursively delete subdirectories
			CString strSubDirPath = finder.GetFilePath();
			RecursiveDelete(strSubDirPath);
			// After deleting contents, remove the empty subdirectory
			RemoveDirectory(strSubDirPath);
		}
		else
		{
			// Delete files
			CString strFilePath = finder.GetFilePath();
			DeleteFile(strFilePath);
		}
	}
	finder.Close();

	// Finally, remove the main directory after all its contents are deleted
	RemoveDirectory(szPath);
}

bool CAuxFunctions::createFolder(CString mfolder)
{
	// Optional: Define security attributes for the new directory
	// For a simple directory creation without specific security, you can pass NULL
	SECURITY_ATTRIBUTES saPermissions;
	saPermissions.nLength = sizeof(SECURITY_ATTRIBUTES);
	saPermissions.lpSecurityDescriptor = NULL; // Default security descriptor
	saPermissions.bInheritHandle = TRUE;       // Inherit security from parent
	// Call CreateDirectory to create the new folder
	if (CreateDirectory(mfolder, &saPermissions) == TRUE) {
		return(true);
	}
	// Handle error if directory creation fails
	//DWORD dwError = GetLastError();
	//CString strErrorMessage;
	//strErrorMessage.Format(L"Failed to create directory. Error code: %d", dwError);
	//addErrorString(strErrorMessage);
	return(false);
}

bool CAuxFunctions::copyFile(CString from, CString to)
{
	if (CopyFile(from, to, true)) {
		return(true);
	}
	return(false);
}

vector<CString> CAuxFunctions::getCsvFileName(CString filename)
{
	vector<CString> rval;
	CString myfilename = filename;
	CFileDialog dlg2(true, _T("csv"), myfilename, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, _T("Comma Separated (*.csv)|*.csv||"));

	CString data;
	int max_file_names = 40;

	dlg2.m_pOFN->nMaxFile = (max_file_names * (MAX_PATH + 1)) + 1;
	dlg2.m_pOFN->lpstrFile = data.GetBuffer((max_file_names * (MAX_PATH + 1)) + 1);
	if (dlg2.DoModal() == IDOK) {
		POSITION pos(dlg2.GetStartPosition());
		while (pos) {
			CString myfilename = dlg2.GetNextPathName(pos);
			CString myfilename2 = myfilename.Right(4);
			if (myfilename.Right(4) == _T(".csv")) {
				rval.push_back(myfilename);
			}
		}
	}

	data.ReleaseBuffer();

	return(rval);
}

vector<CString> CAuxFunctions::getDataFileName(CString filename, int ftype)
{
	vector<CString> rval;
	CString myfilename = filename;

	CString lpszDefExt = _T("");
	//CString lpszFilter = _T("Comma Separated (*.csv)|*.csv|Image Files (*.jpg)|*.jpg|Image Files (*.raw)|*.raw|Wave Files (*.wav)|*.wav|Text Files (*.txt)|*.txt||");
	CString lpszFilter = _T("Comma Separated (*.csv)|*.csv|Image Files (*.jpg)|*.jpg|Image Files (*.raw)|*.raw|Wave Files (*.wav)|*.wav||");
	if ((ftype == FILE_TYPE_CSV) || (ftype == FILE_TYPE_CSV_WITH_JPG)) {
		lpszDefExt = _T("csv");
		lpszFilter = _T("Comma Separated (*.csv)|*.csv||");
	}
	else if (ftype == FILE_TYPE_JPG) {
		lpszDefExt = _T("jpg");
		lpszFilter = _T("Image Files (*.jpg)|*.jpg||");
	}
	else if (ftype == FILE_TYPE_RAW) {
		lpszDefExt = _T("raw");
		lpszFilter = _T("Image Files (*.raw)|*.raw||");
	}
	else if (ftype == FILE_TYPE_WAV) {
		lpszDefExt = _T("wav");
		lpszFilter = _T("Wave Files (*.wav)|*.wav||");
	}
	else if (ftype == FILE_TYPE_TXT) {
		lpszDefExt = _T("txt");
		lpszFilter = _T("Text Files (*.txt)|*.txt||");
	}

	CFileDialog dlg2(true, lpszDefExt, myfilename, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, lpszFilter);

	if (lpszDefExt == _T("csv")) {
		dlg2.GetOFN().nFilterIndex = 1;
	}
	else if (lpszDefExt == _T("jpg")) {
		dlg2.GetOFN().nFilterIndex = 2;
	}
	else if (lpszDefExt == _T("raw")) {
		dlg2.GetOFN().nFilterIndex = 3;
	}
	else if (lpszDefExt == _T("wav")) {
		dlg2.GetOFN().nFilterIndex = 4;
	}
	else if (lpszDefExt == _T("txt")) {
		dlg2.GetOFN().nFilterIndex = 5;
	}

	CString data;
	int max_file_names = 400;

	CString myfirstext = _T("");

	dlg2.m_pOFN->nMaxFile = (max_file_names * (MAX_PATH + 1)) + 1;
	dlg2.m_pOFN->lpstrFile = data.GetBuffer((max_file_names*(MAX_PATH+1))+1);
	if (dlg2.DoModal() == IDOK) {
		POSITION pos(dlg2.GetStartPosition());
		while (pos) {
			CString myfilename = dlg2.GetNextPathName(pos);
			//addErrorString(myfilename);
			CString myext = myfilename.Right(4);
			if (rval.size() == 0) {
				myfirstext = myext;
			}
			if (myext == myfirstext) {
				rval.push_back(myfilename);
				//addErrorString(_T("getDataFileName : ") + myfilename);
			}
			else {
				addErrorString(_T("getDataFileName : need expanded filename limit"));
				addErrorString(_T("getDataFileName : " + myfilename + _T(" does not match extension ") + myfirstext));
			}
		}
	}

	data.ReleaseBuffer();

	return(rval);
}

CString CAuxFunctions::getDataFolder(CString folder)
{
	CString rval;
	CString myfolder = folder;

	CFolderPickerDialog dlg2(myfolder, OFN_HIDEREADONLY | BIF_NEWDIALOGSTYLE);

	if (dlg2.DoModal() == IDOK) {
		rval = dlg2.GetPathName();
	}

	return(rval);
}

unsigned long CAuxFunctions::getCSVFile(CString filename, unsigned char *filedata, unsigned long maxfilelength, bool display_error)
{
	CFile file;
	unsigned long filelength = 0;
	if (file.Open(filename, CFile::modeRead | CFile::shareDenyWrite)) {
		filelength = (unsigned long) file.GetLength();
		if (filelength == 0) {
			if (display_error) {
				AfxMessageBox(_T("getCSVFile() : Zero filelength"));
			}
			file.Close();
			return(0);
		}
		if (filelength > maxfilelength) {
			if (display_error) {
				CString s = _T("");
				//s.Format(_T("getCSVFile() : filelength (%li) > maxlimit (%li) - entering unknown territory"), filelength, maxfilelength);
				s.Format(_T("getCSVFile() : filelength (%li) > maxlimit (%li) - truncating data"), filelength, maxfilelength);
				AfxMessageBox(s);
			}
			filelength = maxfilelength;
		}
		file.Read(filedata,filelength);
		file.Close();
	} else {
		if (display_error) {
			CString sbuf = _T("getCSVFile() : Unable to open file: ");
			sbuf += filename;
			//AfxMessageBox(sbuf);
		}
	}
	return(filelength);
}

vector<CString> CAuxFunctions::parseCSVHeaders(unsigned char* filedata, unsigned long filelength)
{
	int tries = 0;
	long start = 0;
	long end = 0;
	vector<CString> headers;
	while ((tries < 10) && (headers.size() < 1)) {
		end = findEndOfLine(start, filedata, filelength);
		headers = parseLine(start, end, filedata);
		start = end = findAfterEndOfLine(end, filedata, filelength);
		tries++;
	}
	return(headers);
}

long CAuxFunctions::findEndOfLine(long start, unsigned char *filedata, unsigned long filelength)
{
	long end = start;
	while ((end<(long)filelength) && !isEOL(filedata[end])) {
		end++;
	}
	return(end);
}

long CAuxFunctions::findAfterEndOfLine(long start, unsigned char *filedata, unsigned long filelength) 
{
	long end = start;
	while ((end<(long)filelength) && isEOL(filedata[end])) {
		end++;
	}
	return(end);
}

long CAuxFunctions::parseLine(long start, long end, unsigned char *filedata, CString *dataset, long maxcols)
{
	int ncols = 0;
	long index = start;
	CString data = _T("");
	while (index < end) {
		if (isComma(filedata[index])) {
			dataset[ncols] = data;
			data = "";
			if (ncols < maxcols) {
				ncols++;
			} else if (!errordisplayed) {
				AfxMessageBox(_T("CAuxFunctions::parseLine() : maxcols reached"));
				errordisplayed = true;
			}
			index++;
		} else {
			data += filedata[index];
			index++;
		}
	}
	if (data != _T("")) {
		dataset[ncols] = data;
		if (ncols < maxcols) {
			ncols++;
		} else if (!errordisplayed) {
			AfxMessageBox(_T("CAuxFunctions::parseLine() : maxcols reached"));
			errordisplayed = true;
		}
	}
	return(ncols);
}

vector<CString> CAuxFunctions::parseLine(long start, long end, unsigned char* filedata, long maxcols)
{
	vector<CString> headers;
	long index = start;
	CString data = _T("");
	while ((index < end) && (headers.size() < maxcols)) {
		if (isComma(filedata[index])) {
			headers.push_back(data);
			data = "";
			index++;
		}
		else {
			data += filedata[index];
			index++;
		}
	}
	if (data != _T("")) {
		if (headers.size() < maxcols) {
			headers.push_back(data);
		}
	}
	if (headers.size() >= maxcols) {
		if (!errordisplayed) {
			AfxMessageBox(_T("CAuxFunctions::parseLine() : maxcols reached"));
			errordisplayed = true;
		}
	}
	return(headers);
}

vector<CString> CAuxFunctions::parseLine(CString line)
{
	long index = 0;
	CString data = _T("");
	vector<CString> rval;
	while (index < line.GetLength()) {
		if (isComma((char)line[index])) {
			rval.push_back(data);
			data = "";
		}
		else {
			data += line[index];
		}
		index++;
	}
	if (data != _T("")) {
		rval.push_back(data);
	}
	return(rval);
}

vector<CString> CAuxFunctions::parseLine2(CString line)
{
	long index = 0;
	CString data = _T("");
	vector<CString> rval;
	while (index < line.GetLength()) {
		if (isUnderscore((char)line[index]) || isSpace((char)line[index]) || isBackSlash((char)line[index])) {
			rval.push_back(data);
			data = "";
		}
		else {
			data += line[index];
		}
		index++;
	}
	if (data != _T("")) {
		rval.push_back(data);
	}
	return(rval);
}

vector<CString> CAuxFunctions::getDataNamesFromFilenames(vector<CString> dataPaths)
{
	vector<CString> datanames;
	for (int i = 0; i < dataPaths.size(); i++) {
		vector<CString> names = parseKeyNamesFromFilename(dataPaths[i]);
		for (int j = 0; j < names.size(); j++) {
			if (!isWordNumber(string(CT2CA(names[j])))) {
				bool found = false;
				for (int k = 0; k < datanames.size(); k++) {
					if (names[j] == datanames[k]) {
						k = (int)datanames.size();
						found = true;
					}
				}
				if (!found) {
					datanames.push_back(names[j]);
				}
			}
		}
	}
	return(datanames);
}

bool CAuxFunctions::isDataInFilenames(CString label, CString dataPath)
{
	vector<CString> names = parseKeyNamesFromFilename(dataPath);
	for (int j = 0; j < names.size(); j++) {
		if (label == names[j]) {
			return(true);
		}
	}
	return(false);
}

vector<CString> CAuxFunctions::parseKeyNamesFromFilename(CString dataPath)
{
	CString s = removeExtensionFromFilename(dataPath);
	s.MakeLower();
	long index = 0;
	while (index < s.GetLength()) {
		if (isColon((char)s[index])) {
			s = s.Right(s.GetLength() - index - 1);
			index = s.GetLength();
		}
		index++;
	}
	vector<CString> names = parseLine2(s);
	return(names);
}

long CAuxFunctions::getDirectoryFiles(CString path, CString extension, CString *filenames, long maxfiles)
{
	long nfiles = 0;
	bool error_displayed = false;
	CString mpath = path + extension;	// T("*.jpg");
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(mpath,&data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (nfiles < maxfiles) {
				filenames[nfiles] = data.cFileName;
				nfiles++;
			} else {
				if (!error_displayed) {
					AfxMessageBox(_T("CAuxFunctions::getDirectoryFiles : max num files reached"));
					error_displayed = true;
				}
			}
		} while (FindNextFile(hFind,&data));
		FindClose(hFind);
	} else {
		AfxMessageBox(_T("CAuxFunctions::getDirectoryFiles() : Invalid File Handle"));
	}
	for (long i=0; i<nfiles; i++) {
		filenames[i] = filenames[i].Left(filenames[i].GetLength()-4);
	}
	return(nfiles);
}

vector<CString> CAuxFunctions::getSelectFileType(vector<CString> files, CString folder)
{
	vector<CString> rfiles;
	vector<CString> csvfiles;
	vector<CString> jpgfiles;
	vector<CString> wavfiles;
	vector<CString> txtfiles;
	for (int i = 0; i < (int)files.size(); i++) {
		CString ext = getFileExtension2(files[i]).MakeLower();
		if (ext == _T("csv")) {
			csvfiles.push_back(files[i]);
		}
		else if (ext == _T("jpg")) {
			jpgfiles.push_back(files[i]);
		}
		else if (ext == _T("wav")) {
			wavfiles.push_back(files[i]);
		}
		else if (ext == _T("txt")) {
			txtfiles.push_back(files[i]);
		}
	}
	int total = (int)csvfiles.size() + (int)jpgfiles.size() + (int)wavfiles.size() + (int)txtfiles.size();
	if (total > 0) {
		CString s = _T("");
		float pcsv = (float)csvfiles.size() / (float)total;
		float pjpg = (float)jpgfiles.size() / (float)total;
		float pwav = (float)wavfiles.size() / (float)total;
		float ptxt = (float)txtfiles.size() / (float)total;
		if (pcsv > 0.8) {
			rfiles = csvfiles;
			s.Format(_T("Selecting %i csv files from folder "), (int)rfiles.size());
			addErrorString(s + folder);
		}
		else if (pjpg > 0.8) {
			rfiles = jpgfiles;
			s.Format(_T("Selecting %i jpg files from folder "), (int)rfiles.size());
			addErrorString(s + folder);
		}
		else if (pwav > 0.8) {
			rfiles = wavfiles;
			s.Format(_T("Selecting %i wav files from folder "), (int)rfiles.size());
			addErrorString(s + folder);
		}
		else if (ptxt > 0.8) {
			rfiles = txtfiles;
			s.Format(_T("Selecting %i txt files from folder "), (int)rfiles.size());
			addErrorString(s + folder);
		}
		else {
			CFileTypeSelectDlg dlg;
			vector<pair<CString, int>> file_types;
			if (csvfiles.size() > 0) {
				pair<CString, int> mtype;
				mtype.first = _T("csv");
				mtype.second = (int)csvfiles.size();
				file_types.push_back(mtype);
			}
			if (jpgfiles.size() > 0) {
				pair<CString, int> mtype;
				mtype.first = _T("jpg");
				mtype.second = (int)jpgfiles.size();
				file_types.push_back(mtype);
			}
			if (wavfiles.size() > 0) {
				pair<CString, int> mtype;
				mtype.first = _T("wav");
				mtype.second = (int)wavfiles.size();
				file_types.push_back(mtype);
			}
			if (txtfiles.size() > 0) {
				pair<CString, int> mtype;
				mtype.first = _T("txt");
				mtype.second = (int)txtfiles.size();
				file_types.push_back(mtype);
			}
			dlg.setFiles(file_types);
			if (dlg.DoModal() == IDOK) {
				CString type = dlg.getSelectedType().Left(3);
				if (type == _T("csv")) {
					rfiles = csvfiles;
				}
				else if (type == _T("jpg")) {
					rfiles = jpgfiles;
				}
				else if (type == _T("wav")) {
					rfiles = wavfiles;
				}
				else if (type == _T("txt")) {
					rfiles = txtfiles;
				}
				s.Format(_T("Selecting %i "), (int)rfiles.size());
				addErrorString(s + type + _T(" files from folder") + folder);
			}
			else {
				addErrorString(_T("Not selecting any files from folder ") + folder);
				rfiles.clear();
			}
			//s.Format(_T("num csv files = %i : num jpg files = %i : num wav files = %i : num txt files = %i"), (int)csvfiles.size(), (int)jpgfiles.size(), (int)wavfiles.size(), (int)txtfiles.size());
			//addErrorString(s);
		}
	}
	else {
		addErrorString(_T("Not finding any suitable file types from selected folder"));
	}
	return(rfiles);
}

vector<CString> CAuxFunctions::getDirectoryAndSubdirectoryFiles(CString path, CString extension)
{
	vector<CString> rfiles;
	vector<CString> dfiles;

	vector<CString> tfiles = getDirectoryFiles(path, extension);
	for (int i = 0; i < (int)tfiles.size(); i++) {
		CString s = path + _T("\\") + tfiles[i];
		CString ext = getFileExtension2(s).MakeLower();
		if (ext == _T("")) {
			dfiles.push_back(s);
		} 
		else if ((ext == _T("csv")) || (ext == _T("jpg")) || (ext == _T("wav")) || (ext == _T("txt"))) {
			rfiles.push_back(s);
		}
	}

	for (int i = 0; i < (int)dfiles.size(); i++) {
		vector<CString> tfiles2 = getDirectoryAndSubdirectoryFiles(dfiles[i], extension);
		for (int j = 0; j < (int)tfiles2.size(); j++) {
			rfiles.push_back(tfiles2[j]);
		}
	}

	return(rfiles);
}

vector<CString> CAuxFunctions::getDirectoryFiles(CString path, CString extension)
{
	vector<CString> rval;
	CString mpath = path + extension;	// T("\\*.*");
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(mpath, &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			CString s = data.cFileName;
			if ((s != _T("..")) && (s != _T("."))) {
				rval.push_back(s);
			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	else {
		AfxMessageBox(_T("CAuxFunctions::getDirectoryFiles() : Invalid File Handle"));
	}
	return(rval);
}

bool CAuxFunctions::areAllNumbers(vector<CString> testwords)
{
	for (int i = 0; i < testwords.size(); i++) {
		if (!isWordNumber(string(CT2CA(testwords[i])))) {
			return(false);
		}
	}
	return(true);
}

bool CAuxFunctions::areAnyNumbers(vector<CString> testwords)
{
	for (int i = 0; i < testwords.size(); i++) {
		if (isWordNumber(string(CT2CA(testwords[i])))) {
			return(true);
		}
	}
	return(false);
}

vector<CString> CAuxFunctions::getHeadersFromDataSource(CString pname, bool display_error)
{
	vector<CString> headers;
	CFile file;
	if (file.Open(pname, CFile::modeRead | CFile::shareDenyWrite)) {
		unsigned long filelength = (unsigned long)file.GetLength();
		if (filelength == 0) {
			if (display_error) {
				AfxMessageBox(_T("getHeadersFromDataSource() : Zero filelength"));
			}
			file.Close();
			return(headers);
		}
		int tries = 0;
		bool done = false;
		CString line = _T("");
		unsigned long bufferlength = 1000;
		unsigned char* bufferdata = new unsigned char[bufferlength + static_cast<unsigned __int64>(100)];
		while (!done) {
			unsigned int numread = file.Read(bufferdata, bufferlength);
			for (unsigned int i = 0; i < numread; i++) {
				if (bufferdata[i] == '\n') {
					//line += bufferdata[i];
					//addErrorString(_T("parseLine : ") + line);
					headers = parseLine(line);
					if (headers.size() > 0) {
						i = numread;
						done = true;
					}
					else {
						tries++;
					}
					line = _T("");
				}
				else {
					line += bufferdata[i];
				}
			}
			if (numread < bufferlength) {
				done = true;
			}
			else if (tries > 10) {
				done = true;
			}
		}
		file.Close();
		delete[] bufferdata;
	}
	if (headers.size() > 0) {
		for (int i = 0; i < (int)headers.size(); i++) {
			headers[i] = cleanHeader(headers[i]);
		}
	}
	return(headers);
}

CString CAuxFunctions::cleanHeader(CString header)
{
	CString rval = _T("");
	for (int i = 0; i < (int)header.GetLength(); i++) {
		if ((header[i] != '\n') && (header[i] != '\r')) {
			rval += header[i];
		}
	}
	return(rval);
}

vector<CString> CAuxFunctions::checkHeadersFromDataSources(vector<CString> pnames)
{
	vector<CString> mnames = pnames;
	if (mnames.size() > 1) {
		bool error_displayed = false;
		vector<CString> headers;
		vector<CString> theaders;
		CStatus progressDlg;
		progressDlg.CreateDlg();
		progressDlg.setLabel(_T("Checking headers from data sources . . ."));
		progressDlg.setProgress(0);
		int tcnt = 0;
		int ttotal = 1 + (int)mnames.size();
		Timer ttimer;
		ttimer.Start();
		for (int i = 0; i < mnames.size(); i++) {
			if (i == 0) {
				headers = getHeadersFromDataSource(pnames[i]); 
			}
			else {
				theaders = getHeadersFromDataSource(pnames[i]); 
				if (headers.size() == theaders.size()) {
					for (int j = 0; j < theaders.size(); j++) {
						if (theaders[j] != headers[j]) {
							bool found = false;
							for (int k = 0; k < headers.size(); k++) {
								if (theaders[j] == headers[k]) {
									k = (int)headers.size();
									found = true;
								}
							}
							if (!found) {
								CString s = _T("Warning - Checking headers from files : ") + mnames[i] + _T(" does not match ") + mnames[0] + _T(" : Removing ") + mnames[i];
								addErrorString(s);
								if (!error_displayed) {
									AfxMessageBox(s);
									error_displayed = true;
								}
								mnames.erase(mnames.begin() + i);
								j = (int)theaders.size();
								i--;
							}
						}
					}
				}
				else {
					CString s = _T("Warning - Checking headers from files : ") + mnames[i] + _T(" does not match ") + mnames[0] + _T(" : Removing ") + mnames[i];
					addErrorString(s);
					if (!error_displayed) {
						AfxMessageBox(s);
						error_displayed = true;
					}
					mnames.erase(mnames.begin() + i);
					i--;
				}
			}
			tcnt++;
			if (tcnt > 0) {
				float dsec = (float)ttimer.GetDelay();
				dsec = (dsec / (float)tcnt) * (ttotal - tcnt);
				CString stime = getTimeString(dsec);
				progressDlg.setLabel(_T("Checking data headers - time remaining ") + stime);
			}
			progressDlg.setProgress((100 * i) / (int)pnames.size());
			if (progressDlg.cancel) {
				progressDlg.DestroyDlg();
				addPanel1String(_T(""));
				return(mnames);
			}
		}
		progressDlg.DestroyDlg();
	}
	return(mnames);
}

vector<CString> CAuxFunctions::getHeadersFromDataSources()
{
	vector<CString> headers;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->loadFileData();
	if (pDoc->nnet.csvFileData.size() > 0) {
		for (int i = 0; i < (int)pDoc->nnet.csvFileData.size(); i++) {
			if (pDoc->nnet.csvFileData[i].set_data.size() > 0) {
				for (int j = 0; j < (int)pDoc->nnet.csvFileData[i].set_data.size(); j++) {
					CString ts = (CString)pDoc->nnet.csvFileData[i].set_data[j].header.c_str();
					//addErrorString(ts);
					if (headers.size() > 0) {
						bool found = false;
						for (int k = 0; k < (int)headers.size(); k++) {
							if (ts == headers[k]) {
								found = true;
							}
						}
						if (!found) {
							headers.push_back(ts);
						}
					}
					else {
						headers.push_back(ts);
					}
				}
			}
		}
	}
	bool headers_all_numbers = true;
	for (int i = 0; i < headers.size(); i++) {
		if (isWordNumber(string(CT2CA(headers[i])))) {
			CString header = _T("");
			header.Format(_T("Data%i"), i + 1);
			headers[i] = header;
		}
		else {
			headers_all_numbers = false;
		}
	}
	if (headers_all_numbers) {
		AfxMessageBox(_T("There seems to be a problem with the headers from the CSV files"));
	}
	return(headers);
}

vector<vector<vector<CString>>> CAuxFunctions::getDataFromDataSources(vector<CString> headers)
{
	vector<vector<vector<CString>>> data;
	if (headers.size() > 0) {
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		pDoc->loadFileData();
		if (pDoc->nnet.csvFileData.size() > 0) {
			addPanel1String(_T("Processing file data"));
			CStatus myProgressDlg;
			myProgressDlg.CreateDlg();
			myProgressDlg.setLabel(_T("Processing file data . . ."));
			myProgressDlg.setProgress(0);
			for (int i = 0; i < (int)pDoc->nnet.csvFileData.size(); i++) {
				if (pDoc->nnet.csvFileData[i].set_data.size() > 0) {
					vector<vector<CString>> tdata;
					for (int k = 0; k < (int)headers.size(); k++) {
						bool found = false;
						for (int j = 0; j < (int)pDoc->nnet.csvFileData[i].set_data.size(); j++) {
							CString ts = (CString)pDoc->nnet.csvFileData[i].set_data[j].header.c_str();
							//addErrorString(_T("<") + ts + _T("> : <") + headers[k] + _T(">"));
							if (ts == headers[k]) {
								vector<CString> tsdata;
								for (int m = 0; m < (int)pDoc->nnet.csvFileData[i].set_data[j].column_string_data.size(); m++) {
									tsdata.push_back((CString)pDoc->nnet.csvFileData[i].set_data[j].column_string_data[m].c_str());
								}
								tdata.push_back(tsdata);
								j = (int)pDoc->nnet.csvFileData[i].set_data.size();
								found = true;
							}
						}
						if (!found) {
							addErrorString(_T("getDataFromDataSources : unable to find match for header ") + headers[k] + _T(" in csvFileData : ") + (CString)pDoc->nnet.csvFileData[i].file_path.c_str());
							data.clear();
							myProgressDlg.DestroyDlg();
							addPanel1String(_T(""));
							return(data);
						}
					}
					data.push_back(tdata);
					myProgressDlg.setProgress((i * 100) / (int)pDoc->nnet.csvFileData.size());
				}
			}
		}
	}
	myProgressDlg.DestroyDlg();
	addPanel1String(_T(""));
	return(data);
}

void CAuxFunctions::getStackedColumnDataFromDataSources(CString header, vector<vector<CString>> *sdata, vector<vector<double>> *ddata)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (sdata->size() > 0) {
		for (int i = 0; i < (int)sdata->size(); i++) {
			sdata[0][i].clear();
		}
		sdata->clear();
	}
	if (ddata->size() > 0) {
		for (int i = 0; i < (int)ddata->size(); i++) {
			ddata[0][i].clear();
		}
		ddata->clear();
	}
	pDoc->loadFileData();
	if (pDoc->nnet.csvFileData.size() > 0) {
		for (int i = 0; i < (int)pDoc->nnet.csvFileData.size(); i++) {
			if (pDoc->nnet.csvFileData[i].set_data.size() > 0) {
				vector<double> mddata;
				vector<CString> msdata;
				bool found = false;
				for (int j = 0; j < (int)pDoc->nnet.csvFileData[i].set_data.size(); j++) {
					CString ts = (CString)pDoc->nnet.csvFileData[i].set_data[j].header.c_str();
					//addErrorString(_T("<") + ts + _T("> : <") + header + _T("> : ") + (CString)pDoc->nnet.csvFileData[i].file_path.c_str());
					if (ts == header) {
						for (int k = 0; k < (int)pDoc->nnet.csvFileData[i].set_data[j].column_string_data.size(); k++) {
							mddata.push_back(pDoc->nnet.csvFileData[i].set_data[j].column_data[k]);
							msdata.push_back((CString)pDoc->nnet.csvFileData[i].set_data[j].column_string_data[k].c_str());
						}
						sdata->push_back(msdata);
						ddata->push_back(mddata);
						j = (int)pDoc->nnet.csvFileData[i].set_data.size();
						found = true;
					}
				}
				if (!found) {
					addErrorString(_T("getStackedColumnDataFromDataSources : Unable to find data for ") + header + _T(" in file ") + (CString)pDoc->nnet.csvFileData[i].file_path.c_str());
					return;
				}
			}
		}
	}
}

vector<CString> CAuxFunctions::getHeadersFromJpgSources(vector<vector<CString>> pnames)
{
	vector<CString> headers;
	if (pnames.size() > 0) {
		for (int i = 0; i < pnames.size(); i++) {
			vector<CString> theaders = getDataNamesFromFilenames(pnames[i]);
			if (theaders.size() > 0) {
				if (headers.size() > 0) {
					for (int m = 0; m < theaders.size(); m++) {
						bool found = false;
						for (int n = 0; n < headers.size(); n++) {
							if (headers[n] == theaders[m]) {
								found = true;
								n = (int)headers.size();
							}
						}
						if (!found) {
							headers.push_back(theaders[m]);
						}
					}
				}
				else {
					headers = theaders;
				}
			}
		}
	}
	return(headers);
}

COleDateTime CAuxFunctions::getTimeFromString(CString sdate)
{
	COleDateTime t = COleDateTime::GetCurrentTime();
	t.ParseDateTime(sdate);
	return t;
}

double CAuxFunctions::firstDerivativeBackward(int index, int stride, bool relative/*, vector<double> data*/)
{
	double rval = 0;
	if (index < (int)derivative_data.size()) {
		if (index > ((3*stride)-1)) {
			rval = 1.833333 * derivative_data[index] - 3 * derivative_data[index - stride] + 1.5 * derivative_data[index - (2 * stride)] - 0.333333 * derivative_data[index - (3 * stride)];
		}
		else if (index > ((2 * stride) - 1)) {
			rval = 1.5 * derivative_data[index ] - 2 * derivative_data[index - stride] + 0.5 * derivative_data[index - (2 * stride)];
		}
		else if (index > ((1 * stride) - 1)) {
			rval = derivative_data[index] - derivative_data[index - stride];
		}
		if (relative) {
			double rel = derivative_data[index];
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

double CAuxFunctions::secondDerivativeBackward(int index, int stride, bool relative)
{
	double rval = 0;
	if (index < (int)derivative_data.size()) {
		if (index > ((4 * stride) - 1)) {
			rval = 2 * derivative_data[index] - 5 * derivative_data[index - stride] + 4 * derivative_data[index - (2 * stride)] - derivative_data[index - (3 * stride)];
		}
		else if (index > ((3 * stride) - 1)) {
			rval = derivative_data[index] - 2 * derivative_data[index - stride] + derivative_data[index - (2 * stride)];
		}
		if (relative) {
			double rel = derivative_data[index];
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

double CAuxFunctions::thirdDerivativeBackward(int index, int stride, bool relative)
{
	double rval = 0;
	if (index < (int)derivative_data.size()) {
		if (index > ((5 * stride) - 1)) {
			rval = 2.5 * derivative_data[index] - 9 * derivative_data[index - stride] + 12 * derivative_data[index - (2 * stride)] - 7 * derivative_data[index - (3 * stride)] + 1.5 * derivative_data[index - (4 * stride)];
		}
		else if (index > ((4 * stride) - 1)) {
			rval = derivative_data[index] - 3 * derivative_data[index - stride] + 3 * derivative_data[index - (2 * stride)] - derivative_data[index - (3 * stride)];
		}
		if (relative) {
			double rel = derivative_data[index];
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

double CAuxFunctions::fourthDerivativeBackward(int index, int stride, bool relative)
{
	double rval = 0;
	if (index < (int)derivative_data.size()) {
		if (index > ((6 * stride) - 1)) {
			rval = 3 * derivative_data[index] - 14 * derivative_data[index - stride] + 26 * derivative_data[index - (2 * stride)] - 24 * derivative_data[index - (3 * stride)] + 11 * derivative_data[index - (4 * stride)] - 2 * derivative_data[index - (5 * stride)];
		}
		else if (index > ((5 * stride) - 1)) {
			rval = derivative_data[index] - 4 * derivative_data[index - stride] + 6 * derivative_data[index - (2 * stride)] - 4 * derivative_data[index - (3 * stride)] + derivative_data[index - (4 * stride)];
		}
		if (relative) {
			double rel = derivative_data[index];
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

vector<double> CAuxFunctions::getFFT(int index, int N, int downsample, bool normalize, vector<double> data)
{
	vector<double> rval;
	if (index < (int)data.size()) {
		int i = 0;
		int myN = 1;
		while (myN < N) {
			i++;
			myN = (int)pow(2.0, i);
		}
		complex* mydata = new complex[myN];
		if (mydata == 0) {
			if (!errordisplayed) {
				AfxMessageBox(_T("Unable to allocate memory for mydata in getFFT"));
			}
			errordisplayed = true;
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
		}
	}
	return(rval);
}

void CAuxFunctions::fft(complex* v, int n)
{
	complex* scratch = new complex[n/* + 10*/];
	if ((scratch == 0) && !errordisplayed) {
		AfxMessageBox(_T("Unable to allocate memory for scratch in fft"));
		errordisplayed = true;
		return;
	}
	fft(v, n, scratch);
	delete[] scratch;
}

void CAuxFunctions::fft(complex* v, int n, complex* tmp)
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

void CAuxFunctions::ifft(complex* v, int n, complex* tmp)
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

		ifft(ve, n / 2, v);
		ifft(vo, n / 2, v);

		for (m = 0; m < n / 2; m++) {

			w.Re = cos(2 * pi * (double)m / (double)n);
			w.Im = sin(2 * pi * (double)m / (double)n);

			z.Re = w.Re * vo[m].Re - w.Im * vo[m].Im;
			z.Im = w.Re * vo[m].Im + w.Im * vo[m].Re;

			v[m].Re = ve[m].Re + z.Re;
			v[m].Im = ve[m].Im + z.Im;

			v[m + n / 2].Re = ve[m].Re - z.Re;
			v[m + n / 2].Im = ve[m].Im - z.Im;

		}

	}

}

void CAuxFunctions::rebuildWaveform(unsigned long n, unsigned long nn, complex* datain, complex* dataout)
{
	unsigned long i, j;
	double omega = 0;
	double omult = 0;
	for (i = 0; i < n; i++) {
		dataout[i].Re = 0;
		dataout[i].Im = 0;
	}
	for (i = 0; i < n; i++) {
		omega = 2.0 * 3.14159 * double(i) / double(nn - 1);
		for (j = 0; j < (nn / 2); j++) {
			omult = double(j);
			dataout[i].Re += (datain[j].Re * cos(omult * omega) - datain[j].Im * sin(omult * omega));
			dataout[i].Im += (datain[j].Im * cos(omult * omega) + datain[j].Re * sin(omult * omega));
		}
		dataout[i].Re = dataout[i].Re / double(nn);
		dataout[i].Im = dataout[i].Im / double(nn);
	}
}

void CAuxFunctions::OnSaveCSV(complex* data, unsigned long nn)
{
	unsigned long i;

	CFile file;

	if (!file.Open(_T("fft.csv"), CFile::modeCreate | CFile::modeReadWrite)) {
		AfxMessageBox(_T("Unable to open fft.csv"));
		return;
	}

	unsigned long max_data = 50000;
	char* cdata = new char[max_data];
	complex* fdata = new complex[max_data];
	if ((cdata == 0) || (fdata == 0)) {
		AfxMessageBox(_T("Unable to allocate memory to save fft.csv"));
		return;
	}

	rebuildWaveform(nn, nn, data, fdata);

	CString sline = _T("");

	for (i = 0; i < nn; i++) {
		sline.Format(_T("%.9f,%.9f,%.9f,%.9f\n"), data[i].Re, data[i].Im, fdata[i].Re, fdata[i].Im);
		if (sline.GetLength() < int(max_data)) {
			for (int j = 0; j < sline.GetLength(); j++) {
				cdata[j] = (char)sline.GetAt(j);
			}
		}
		file.Write(cdata, sline.GetLength());
	}

	file.Flush();

	file.Close();

	delete[] cdata;
	delete[] fdata;

}

void CAuxFunctions::OnSaveCSV(CString fname, vector<CString> lines)
{
	unsigned long i;

	CFile file;

	if (!file.Open(fname, CFile::modeCreate | CFile::modeReadWrite)) {
		AfxMessageBox(_T("Unable to open or create ") + fname);
		return;
	}

	unsigned long max_data = 50000;
	char* cdata = new char[max_data];
	complex* fdata = new complex[max_data];
	if ((cdata == 0) || (fdata == 0)) {
		AfxMessageBox(_T("Unable to allocate memory to save fft.csv"));
		return;
	}

	for (i = 0; i < (int)lines.size(); i++) {
		if (lines[i].GetLength() < int(max_data)) {
			for (int j = 0; j < lines[i].GetLength(); j++) {
				cdata[j] = (char)lines[i].GetAt(j);
			}
		}
		file.Write(cdata, lines[i].GetLength());
	}

	file.Flush();

	file.Close();

	delete[] cdata;
	delete[] fdata;

}

CString CAuxFunctions::convertNumber(double data, int precision)
{
	CString rval = _T("");
	double mdata = data;
	if (mdata < 0) {
		mdata = -mdata;
	}
	if (mdata == 0) {
		CString f = _T("");
		f.Format(_T("%%.%i"), precision);
		f += _T("f");
		rval.Format(f, 0);
	}
	else if (mdata < 1.0) {
		int cnt = 0;
		double tdata = mdata;
		while ((tdata < 1.0) && (cnt < 8)) {
			tdata = 10 * tdata;
			cnt++;
		}
		CString f = _T("");
		f.Format(_T("%%.%i"), precision + cnt);
		f += _T("f");
		rval.Format(f, data);
	}
	else if (mdata >= 10) {
		int cnt = 0;
		double tdata = mdata;
		while ((tdata > 1.0) && (cnt < precision)) {
			tdata = tdata / 10;
			cnt++;
		}
		CString f = _T("");
		f.Format(_T("%%.%i"), precision - cnt + 1);
		f += _T("f");
		rval.Format(f, data);
	}
	else {
		CString f = _T("");
		f.Format(_T("%%.%i"), precision);
		f += _T("f");
		rval.Format(f, data);
	}
	return(rval);
}

CRect CAuxFunctions::getJpgSize(CString fname)
{
	CRect rval;
	rval.left = 0;
	rval.top = 0;
	rval.right = 0;
	rval.bottom = 0;
	CImage image;
	HRESULT rslt = image.Load(fname);
	if (rslt == S_OK) {
		CBitmap bitmap;
		bitmap.Attach(image.Detach());
		BITMAP pBitMap;
		if (bitmap.GetBitmap(&pBitMap)) {
			rval.left = 0;
			rval.top = 0;
			rval.right = pBitMap.bmWidth;
			rval.bottom = pBitMap.bmHeight;
		}
	}
	return(rval);
}

imageCheckData CAuxFunctions::checkJpgSources(vector<CString> fnames)
{
	CStatus progressDlg;
	progressDlg.CreateDlg();
	progressDlg.setLabel(_T("Checking images . . ."));
	progressDlg.setProgress(0);
	addPanel1String(_T("Checking images"));
	imageCheckData rval;
	bool first = true;
	rval.min_width = 10;
	rval.min_height = 10;
	rval.optimum_aspect = 1.0F;
	rval.paths.clear();
	if (fnames.size() > 0) {
		int mcnt = 0;
		int tcnt = 0;
		int ttotal = 1 + (int)fnames.size();
		Timer ttimer;
		ttimer.Start();
		bool found = false;
		float taspect = 0;
		vector<int> counts;
		vector<float> aspects;
		for (int i = 0; i < fnames.size(); i++) {
			CRect trect = getJpgSize(fnames[i]);
			if ((trect.right > 0) && (trect.bottom > 0)) {
				rval.paths.push_back(fnames[i]);
				if (first) {
					rval.min_width = trect.right;
					rval.min_height = trect.bottom;
					first = false;
				}
				else {
					if (trect.right < rval.min_width) {
						rval.min_width = trect.right;
					}
					if (trect.bottom < rval.min_height) {
						rval.min_height = trect.bottom;
					}
				}
				found = false;
				taspect = (float)trect.Width() / (float)trect.Height();
				for (int j = 0; j < aspects.size(); j++) {
					if (taspect == aspects[j]) {
						counts[j] = counts[j] + 1;
						found = true;
					}
				}
				if (!found && (taspect > 0)) {
					aspects.push_back(taspect);
					counts.push_back(1);
				}
			}
			mcnt++;
			tcnt++;
			if (mcnt > 10) {
				if (tcnt > 0) {
					float dsec = (float)ttimer.GetDelay();
					dsec = (dsec / (float)tcnt) * (ttotal - tcnt);
					CString stime = getTimeString(dsec);
					progressDlg.setLabel(_T("Checking images - time remaining ") + stime);
				}
				progressDlg.setProgress((100 * i) / (int)fnames.size());
				mcnt = 0;
			}
			if (progressDlg.cancel) {
				progressDlg.DestroyDlg();
				addPanel1String(_T(""));
				return(rval);
			}
		}
		if (aspects.size() > 1) {
			float tarea = 0;
			float marea = 0;
			for (int i = 0; i < aspects.size(); i++) {
				tarea = 0;
				for (int j = 0; j < aspects.size(); j++) {
					if (i != j) {
						if (aspects[i] > aspects[j]) {
							tarea += ((float)counts[j] * (aspects[i] - aspects[j]));
						}
						else {
							tarea += ((float)counts[j] * (1 - aspects[i] / aspects[j]));
						}
					}
				}
				if ((tarea < marea) || (marea == 0)) {
					marea = tarea;
					rval.optimum_aspect = aspects[i];
				}
			}
		}
		else if (aspects.size() > 0) {
			rval.optimum_aspect = aspects[0];
		}
	}
	progressDlg.DestroyDlg();
	addPanel1String(_T(""));
	return(rval);
}

bool CAuxFunctions::checkRawSources(vector<CString> fnames)
{
	CStatus progressDlg;
	progressDlg.CreateDlg();
	progressDlg.setLabel(_T("Checking images . . ."));
	progressDlg.setProgress(0);
	addPanel1String(_T("Checking images"));
	if (fnames.size() > 0) {
		CFile file;
		int mcnt = 0;
		int tcnt = 0;
		int ttotal = 1 + (int)fnames.size();
		Timer ttimer;
		ttimer.Start();
		unsigned long filelength = 0;
		unsigned long pfilelength = 0;
		for (int i = 0; i < fnames.size(); i++) {
			if (file.Open(fnames[i], CFile::modeRead | CFile::shareDenyWrite)) {
				filelength = (unsigned long)file.GetLength();
				if (i > 0) {
					if (filelength != pfilelength) {
						CString s = _T("");
						s.Format(_T("checkRawSources() : i = %i filelength (%li) != previous filelength (%li)"), i, filelength, pfilelength);
						AfxMessageBox(s);
						progressDlg.DestroyDlg();
						addPanel1String(_T(""));
						return(false);
					}
				}
				pfilelength = filelength;
			}
			else {
				CString s = _T("checkRawSources() : Unable to open file: ");
				s += fnames[i];
				AfxMessageBox(s);
				progressDlg.DestroyDlg();
				addPanel1String(_T(""));
				return(false);
			}
			mcnt++;
			tcnt++;
			if (mcnt > 10) {
				if (tcnt > 0) {
					float dsec = (float)ttimer.GetDelay();
					dsec = (dsec / (float)tcnt) * (ttotal - tcnt);
					CString stime = getTimeString(dsec);
					progressDlg.setLabel(_T("Checking images - time remaining ") + stime);
				}
				progressDlg.setProgress((100 * i) / (int)fnames.size());
				mcnt = 0;
			}
		}
	}
	progressDlg.DestroyDlg();
	addPanel1String(_T(""));
	return(true);
}

wave_data CAuxFunctions::getWaveDataFromFile(CString fname)
{
	wave_data rval;
	rval.length = 0;
	rval.file_length = 0;
	rval.sample_rate = 0;
	rval.max = 0.0;
	CWaveFile wavefile;
	if (wavefile.ReadWaveFile(string(CT2CA(fname)))) {
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
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			double tvalue = pDoc->minimum_sound_level;
			double nvalue = rval.max;
			double fvalue = pDoc->getDataWavMax();
			if (pDoc->normaiize_file_set) {
				if (pDoc->wresults.files_max > 0) {
					if (pDoc->wresults.files_max >= nvalue) {
						nvalue = pDoc->wresults.files_max;
					}
					else if (fvalue >= nvalue) {
						nvalue = fvalue;
					}
					else {
						CString s = _T("");
						s.Format(_T("getWaveDataFromFile : resetting max from wresults.files_max (%.1f) to file max (%.1f) for file "), pDoc->wresults.files_max, rval.max);
						addErrorString(s + fname);
					}
				}
				else if (fvalue >= nvalue) {
					nvalue = fvalue;
				}
				else {
					CString s = _T("");
					s.Format(_T("getWaveDataFromFile : resetting max from wresults.files_max (%.1f) to file max (%.1f) for file "), pDoc->wresults.files_max, rval.max);
					addErrorString(s + fname);
				}
			}
			//CString s = _T("");
			//s.Format(_T("getWaveDataFromFile : rval.max = %.0f : nvalue = %.0f : file "), rval.max, nvalue);
			//addErrorString(s + fname);
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
					//CString s = _T("");
					//s.Format(_T("getWaveDataFromFile : data_start = %i : data_end = %i : processed length = %i : wave length = %i : cnt = %i : "), data_start, data_end, rval.length, wavefile.data_length, cnt);
					//s.Format(_T("getWaveDataFromFile : avg = %.3f : sigma = %.3f : tmin = %.3f : "), avg, sigma, tmin);
					//addErrorString(s + fname);
				}
				else {
					if (tvalue == 0) {
						done = true;
					}
					tvalue = tvalue / 2;
				}
			}
			if (rval.length == 0) {
				addErrorString(_T("getWaveDataFromFile : Unable to process data for ") + fname);
				rval.max = 0.0;
			}
		}
	}
	return(rval);
}

wave_check_results CAuxFunctions::getMaxWaveMagnitude(vector<CString> fnames, bool anyway)
{
	if (fnames == last_wave_fnames) {
		return(last_wave_check_results);
	}
	wave_check_results rval;
	rval.files_shortest = 0;
	rval.files_longest = 0;
	rval.files_min = 0;
	rval.files_max = 0;
	rval.wave_stride = 0;
	rval.wave_paths.clear();
	rval.wave_data_lengths.clear();
	rval.num_copy_iterations.clear();
	if (fnames.size() > 0) {
		CStatus progressDlg;
		progressDlg.CreateDlg();
		progressDlg.setLabel(_T("Getting wave size data . . ."));
		progressDlg.setProgress(0);
		addPanel1String(_T("Getting wave size data"));
		vector<int> samplerates;
		vector<double> maxes;
		int mcnt = 0;
		int tcnt = 0;
		int ttotal = 1 + (int)fnames.size();
		Timer ttimer;
		ttimer.Start();
		for (int i = 0; i < fnames.size(); i++) {
			wave_data wdata = getWaveDataFromFile(fnames[i]);
			if (wdata.length > 0) {
				rval.wave_paths.push_back(string(CT2CA(fnames[i])));
				rval.wave_data_lengths.push_back(wdata.length);
				samplerates.push_back(wdata.sample_rate);
				maxes.push_back(wdata.max);
			}
			else {
				CString s = _T("");
				s.Format(_T(" : wdata length = %i"), wdata.length);
				addErrorString(_T("getMaxWaveMagnitude : ") + fnames[i] + s);
			}
			tcnt++;
			mcnt++;
			if (mcnt > 10) {
				if (tcnt > 0) {
					float dsec = (float)ttimer.GetDelay();
					dsec = (dsec / (float)tcnt) * (ttotal - tcnt);
					CString stime = getTimeString(dsec);
					progressDlg.setLabel(_T("Getting wave size data - time remaining ") + stime);
				}
				progressDlg.setProgress((100 * i) / (int)fnames.size());
				mcnt = 0;
			}
			if (progressDlg.cancel) {
				rval.files_shortest = 0;
				rval.files_longest = 0;
				rval.files_min = 0;
				rval.files_max = 0;
				rval.wave_stride = 0;
				rval.wave_paths.clear();
				rval.wave_data_lengths.clear();
				rval.num_copy_iterations.clear();
				progressDlg.DestroyDlg();
				addPanel1String(_T(""));
				return(rval);
			}
		}
		if ((samplerates.size() == maxes.size()) && (samplerates.size() == rval.wave_paths.size()) && (maxes.size() > 0)) {
			bool show_error = false;
			rval.files_shortest = rval.wave_data_lengths[0];
			rval.files_longest = rval.wave_data_lengths[0];
			rval.files_min = maxes[0];
			rval.files_max = maxes[0];
			int srate = samplerates[0];
			double max_sum = maxes[0];
			double max_sumsq = (maxes[0] * maxes[0]);
			for (int i = 1; i < maxes.size(); i++) {
				max_sum += maxes[i];
				max_sumsq += (maxes[i] * maxes[i]);
				if (srate != samplerates[i]) {
					show_error = true;
				}
				srate = samplerates[i];
				if (maxes[i] > rval.files_max) {
					rval.files_max = maxes[i];
				}
				if (maxes[i] < rval.files_min) {
					rval.files_min = maxes[i];
				}
				if (rval.wave_data_lengths[i] > rval.files_longest) {
					rval.files_longest = rval.wave_data_lengths[i];
				}
				if (rval.wave_data_lengths[i] < rval.files_shortest) {
					rval.files_shortest = rval.wave_data_lengths[i];
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
					CString s = _T("");
					CString s2 = _T("");
					s.Format(_T("Observation : wave magnitude = %.0f for file "), maxes[i]);
					s2.Format(_T("  (set avg = %.2f, set std dev = %.2f)"), max_avg, max_std);
					addErrorString(s + fnames[i] + s2);
				}
			}
			double center = (rval.files_max + rval.files_min) / 2;
			if (center > 0) {
				double test = (rval.files_max - rval.files_min) / center;
				if (test > 400) {
					show_error = true;
				}
			}
			if (show_error) {
				for (int i = 0; i < maxes.size(); i++) {
					CString s = _T("");
					s.Format(_T(" : sample rate = %i : max = %.0f"), samplerates[i], maxes[i]);
					addErrorString(_T("getMaxWaveMagnitude : ") + (CString)rval.wave_paths[i].c_str() + s);
				}
			}
		}
		progressDlg.DestroyDlg();
	}
	if (rval.files_max <= 0) {
		rval.files_max = 100;
	}
	addPanel1String(_T(""));
	last_wave_fnames = fnames;
	last_wave_check_results = rval;
	return(rval);
}

vector<CString> CAuxFunctions::checkWavSources(vector<CString> mpaths)
{
	addPanel1String(_T("Checking wave files"));
	vector<CString> npaths;
	CStatus progressDlg;
	CWaveFile wavefile;
	//progressDlg.setCancellable(true);
	progressDlg.CreateDlg();
	progressDlg.setLabel(_T("Checking wave files . . ."));
	progressDlg.setProgress(0);
	if (mpaths.size() > 0) {
		int tcnt = 0;
		int ttotal = 1 + (int)mpaths.size();
		Timer ttimer;
		ttimer.Start();
		for (int i = 0; i < mpaths.size(); i++) {
			if (wavefile.ReadWaveFile(string(CT2CA(mpaths[i])))) {
				if ((wavefile.filelength > 0) && (wavefile.data_length > 0)) {
					npaths.push_back(mpaths[i]);
				}
			}
			tcnt++;
			if (tcnt > 0) {
				float dsec = (float)ttimer.GetDelay();
				dsec = (dsec / (float)tcnt) * (ttotal - tcnt);
				CString stime = getTimeString(dsec);
				progressDlg.setLabel(_T("Checking wave files - time remaining ") + stime);
			}
			progressDlg.setProgress((100 * i) / (int)mpaths.size());
			if (progressDlg.cancel) {
				addPanel1String(_T(""));
				return(npaths);
			}
		}
	}
	progressDlg.DestroyDlg();
	addPanel1String(_T(""));
	return(npaths);
}

vector<CPoint> CAuxFunctions::getLayerPoints2(int radius, int convtype)
{
	vector<CPoint> rpoints;
	if (convtype >= 0) {
		rpoints = getLayerPoints3(radius, convtype);
	}
	else {
		rpoints.push_back(CPoint(0, 0));
		rpoints.push_back(CPoint(0, -4 * radius));
		rpoints.push_back(CPoint(0, 4 * radius));
	}
	return(rpoints);
}

vector<CPoint> CAuxFunctions::getLayerPoints3(int radius, int convtype)
{
	vector<CPoint> rpoints;
	if (convtype >= 0) {
		if (convtype == HIDDEN_NET) {
			rpoints.push_back(CPoint(0, 0 * radius));
			//rpoints.push_back(CPoint(0, 0 * radius));
			//rpoints.push_back(CPoint(0, 0 * radius));
		}
		else {
			rpoints.push_back(CPoint(0, 0));
			rpoints.push_back(CPoint(0, -4 * radius));
			rpoints.push_back(CPoint(0, 4 * radius));
		}
	}
	else {
		rpoints.push_back(CPoint(0, 0));
		rpoints.push_back(CPoint(0, -4 * radius));
		rpoints.push_back(CPoint(0, 4 * radius));
	}
	return(rpoints);
}

vector<twoValues> CAuxFunctions::getLayerPoints4(int radius, int convtype)
{
	vector<twoValues> rpoints;
	if (convtype >= 0) {
		if (convtype == HIDDEN_NET) {
			twoValues mpt;  mpt.x = 0;  mpt.y = 0 * radius;
			rpoints.push_back(mpt);// CPoint(0, 0 * radius));
		}
		else {
			twoValues mpt1;  mpt1.x = mpt1.y = 0;
			rpoints.push_back(mpt1);
			twoValues mpt2;  mpt2.x = 0;  mpt2.y = -4 * radius;
			rpoints.push_back(mpt2);// CPoint(0, -4 * radius));
			twoValues mpt3;  mpt3.x = 0;  mpt3.y = 4 * radius;
			rpoints.push_back(mpt3);// CPoint(0, 4 * radius));
		}
	}
	else {
		twoValues mpt1;  mpt1.x = mpt1.y = 0;
		rpoints.push_back(mpt1);// CPoint(0, 0));
		twoValues mpt2;  mpt2.x = 0;  mpt2.y = -4 * radius;
		rpoints.push_back(mpt2);// CPoint(0, -4 * radius));
		twoValues mpt3;  mpt3.x = 0;  mpt3.y = 4 * radius;
		rpoints.push_back(mpt3);// CPoint(0, 4 * radius));
	}
	return(rpoints);
}

vector<CPoint> CAuxFunctions::getAttentionPoints(int radius)
{
	vector<CPoint> rpoints;
	rpoints.push_back(CPoint(0, 0));
	rpoints.push_back(CPoint(0, -4 * radius));
	rpoints.push_back(CPoint(0, 4 * radius));
	return(rpoints);
}

// Replaced with version in Doc
/*vector<CPoint> CAuxFunctions::getAssemblyPoints(int radius)
{
	vector<CPoint> rpoints;
	rpoints.push_back(CPoint(0, 0));
	return(rpoints);
}*/

vector<CPoint> CAuxFunctions::getNetClusterPoints(int radius)
{
	vector<CPoint> rpoints;
	rpoints.push_back(CPoint(0, 0));
	return(rpoints);
}

imageSizeData CAuxFunctions::getImageSizeData(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int index = pDoc->netData.getIndexFromID(id);
	return(getImageSizeData(index));
}

imageSizeData CAuxFunctions::getImageSizeData(int cindex)
{
	imageSizeData rval;
	rval.input_width = 0;
	rval.input_height = 0;
	rval.output_width = 0;
	rval.output_height = 0;
	rval.num_width_output_steps = 0;
	rval.num_height_output_steps = 0;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	rval = getImageSizeData(cindex, pDoc->netData);
	if ((rval.output_width == 0) || (rval.output_height == 0)) {
		rval = getImageSizeDataFromOutputImage(cindex, pDoc->netData);
		if ((rval.output_width == 0) || (rval.output_height == 0)) {
			rval = getInputImageSizeWithNoSupportingImage(cindex, pDoc->netData);
		}
	}
	return(rval);
}

imageSizeData CAuxFunctions::getImageSizeData(string id, NetData ndata)
{
	imageSizeData rval;
	rval.input_width = 0;
	rval.input_height = 0;
	rval.output_width = 0;
	rval.output_height = 0;
	rval.num_width_output_steps = 0;
	rval.num_height_output_steps = 0;
	int index = ndata.getIndexFromID(id);
	rval = getImageSizeData(index, ndata);
	if ((rval.output_width == 0) || (rval.output_height == 0)) {
		rval = getImageSizeDataFromOutputImage(index, ndata);
		if ((rval.output_width == 0) || (rval.output_height == 0)) {
			rval = getInputImageSizeWithNoSupportingImage(index, ndata);
		}
	}
	return(rval);
}

imageSizeData CAuxFunctions::getImageSizeData(int sindex, NetData ndata)
{
	imageSizeData rval;
	rval.input_width = 0;
	rval.input_height = 0;
	rval.output_width = 0;
	rval.output_height = 0;
	rval.num_width_output_steps = 0;
	rval.num_height_output_steps = 0;

	if (ndata.isConvLayer(sindex)) {

		if (ndata.getNodeMode(sindex) == "Input") {

			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

			pDoc->getDataImageWidth();
			pDoc->getDataImageHeight();
			rval.input_width = pDoc->image_width;
			rval.input_height = pDoc->image_height;

			rval.num_width_output_steps = 1;		// num out steps wide
			rval.output_width = rval.input_width;	// out width
			rval.num_height_output_steps = 1;		// num steps high
			rval.output_height = rval.input_height;	// out height

		}
		else {

			int mxin = getConvInputX(sindex, ndata);
			int myin = getConvInputY(sindex, ndata);
			int mxout = getConvOutputX(sindex, ndata);
			int myout = getConvOutputY(sindex, ndata);
			int stridex = getConvOutputStrideX(sindex, ndata);
			int stridey = getConvOutputStrideY(sindex, ndata);

			vector<int> sn0 = getImmediateSupportingConvNets(sindex, ndata);

			bool hidden_net = false;
			if (sn0.size() > 0) {
				for (int i = 0; i < sn0.size(); i++) {
					if (getConvType(getIDFromNetData(sn0[i])) == HIDDEN_NET) {
						i = (int)sn0.size();
						hidden_net = true;
					}
				}
			}

			if ((sn0.size() > 0) && !hidden_net) {

				imageSizeData tval1 = getImageSizeData(sn0[0], ndata);

				for (int i = 1; i < sn0.size(); i++) {
					imageSizeData tval2 = getImageSizeData(sn0[i], ndata);
					if ((tval1.input_width != tval2.input_width) || (tval1.input_height != tval2.input_height) || 
						(tval1.output_width != tval2.output_height) || (tval1.output_height != tval2.output_height) || 
						(tval1.num_width_output_steps != tval2.num_width_output_steps) || (tval1.num_height_output_steps != tval2.num_height_output_steps)) {
						addErrorString("Error : getImageSizeData found different sizes for " + ndata.getID(sn0[0]) + " and " + ndata.getID(sn0[i]));
					}
				}

				rval.input_width = tval1.output_width;
				if (stridex > 0) {
					rval.num_width_output_steps = rval.input_width / stridex;		// num out steps wide
					if (stridex < mxin) {
						rval.num_width_output_steps = (rval.input_width - mxin) / stridex;
					}
				}
				rval.output_width = rval.num_width_output_steps * mxout;

				rval.input_height = tval1.output_height;
				if (stridey > 0) {
					rval.num_height_output_steps = rval.input_height / stridey;		// num out steps high
					if (stridey < myin) {
						rval.num_height_output_steps = (rval.input_height - myin) / stridey;
					}
				}
				rval.output_height = rval.num_height_output_steps * myout;

			}
			else {
				//addErrorString(_T("getImageSizeData : no supporting nodes for ") + ndata.getID(sindex));
			}

		}

		/*CString s = _T("");
		s.Format(_T(" : input_width = %i : input_height = %i : output_width = %i : output_height = %i : num_width_output_steps = %i : num_height_output_steps = %i"),
			rval.input_width, rval.input_height, rval.output_width, rval.output_height, rval.num_width_output_steps, rval.num_height_output_steps);
		addErrorString(_T("getImageSizeData : ") + ndata.getID(sindex) + s);*/

	}

	return(rval);
}

imageSizeData CAuxFunctions::getImageSizeDataFromOutputImage(int sindex, NetData ndata)
{
	imageSizeData rval;
	rval.input_width = 0;
	rval.input_height = 0;
	rval.output_width = 0;
	rval.output_height = 0;
	rval.num_width_output_steps = 0;
	rval.num_height_output_steps = 0;

	if (ndata.isConvLayer(sindex)) {

		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

		int mxin = getConvInputX(sindex, ndata);
		int myin = getConvInputY(sindex, ndata);
		int mxout = getConvOutputX(sindex, ndata);
		int myout = getConvOutputY(sindex, ndata);
		int stridex = getConvOutputStrideX(sindex, ndata);
		int stridey = getConvOutputStrideY(sindex, ndata);

		if (ndata.getNodeMode(sindex) == "Output") {

			rval.output_width = pDoc->image_width;
			rval.output_height = pDoc->image_height;
			for (int i = 0; i < ndata.size(); i++) {
				if (ndata.isDataSource(i)) {
					rval.output_width = ndata.getDataImageWidth(i);
					rval.output_height = ndata.getDataImageHeight(i);
					i = ndata.size();
				}
			}

			if (mxin > 0) {
				rval.num_width_output_steps = rval.output_width / mxout;		// num steps wide
				rval.input_width = rval.num_width_output_steps * mxin;		// in width
			}
			if (myin > 0) {
				rval.num_height_output_steps = rval.output_height / myout;	// num steps high
				rval.input_height = rval.num_height_output_steps * myin;	// in height
			}

		}
		else if (ndata.getNodeMode(sindex) == "Hidden") {

			vector<int> sn0 = getImmediateFollowingConvNets(sindex, ndata);

			if (sn0.size() > 0) {

				imageSizeData tval1 = getImageSizeDataFromOutputImage(sn0[0], ndata);

				for (int i = 1; i < sn0.size(); i++) {
					imageSizeData tval2 = getImageSizeDataFromOutputImage(sn0[i], ndata);
					if ((tval1.input_width != tval2.input_width) || (tval1.input_height != tval2.input_height) ||
						(tval1.output_width != tval2.output_height) || (tval1.output_height != tval2.output_height) ||
						(tval1.num_width_output_steps != tval2.num_width_output_steps) || (tval1.num_height_output_steps != tval2.num_height_output_steps)) {
						addErrorString("Error : getImageSizeDataFromOutputImage found different sizes for " + ndata.getID(sn0[0]) + " and " + ndata.getID(sn0[i]));
					}
				}

				rval.output_width = tval1.input_width;
				if (mxout > 0) {
					rval.num_width_output_steps = rval.output_width / mxout;		// num steps wide
					rval.input_width = rval.num_width_output_steps * mxin;		// in width
				}

				rval.output_height = tval1.input_height;
				if (myout > 0) {
					rval.num_height_output_steps = rval.output_height / myout;	// num steps high
					rval.input_height = rval.num_height_output_steps * myin;	// in height
				}

			}

		}

		//CString s = _T("");
		//s.Format(_T(" : input_width = %i : input_height = %i : output_width = %i : output_height = %i : num_width_output_steps = %i : num_height_output_steps = %i"),
			//rval.input_width, rval.input_height, rval.output_width, rval.output_height, rval.num_width_output_steps, rval.num_height_output_steps);
		//addErrorString(_T("getImageSizeDataFromOutputImage : ") + ndata.getID(sindex) + s);

	}

	return(rval);
}

imageSizeData CAuxFunctions::getInputImageSizeWithNoSupportingImage(int sindex, NetData ndata)
{
	imageSizeData rval;
	rval.input_width = 0;
	rval.input_height = 0;
	rval.output_width = 0;
	rval.output_height = 0;
	rval.num_width_output_steps = 0;
	rval.num_height_output_steps = 0;

	if (ndata.isConvLayer(sindex)) {

		string mname = ndata.getID(sindex);

		int nxsteps = 0;
		int nysteps = 0;
		int mwidth = 0;
		int mheight = 0;

		bool mgray = getImageGray();
		int mxin = getConvInputX(sindex, ndata);
		int myin = getConvInputY(sindex, ndata);
		int mxout = getConvOutputX(sindex, ndata);
		int myout = getConvOutputY(sindex, ndata);
		int stridex = getConvOutputStrideX(sindex, ndata);
		int stridey = getConvOutputStrideY(sindex, ndata);

		int min_cinput_size = mxin * myin;
		if (!mgray) {
			min_cinput_size = 3 * min_cinput_size;
		}
		vector<string> supportingNodes = getImmediateSupportingNodes(mname);
		if (supportingNodes.size() >= min_cinput_size) {
			if ((myin > 0) && (mxin > 0) && (stridex > 0)) {
				nysteps = 1;
				mheight = myin;
				int inlength = (int)supportingNodes.size() / mheight;
				if (!mgray) {
					inlength = inlength / 3;
				}
				mwidth = inlength;
				if ((mwidth > 0) && (mheight > 0)) {
					if (mgray) {
						float ft = (float)supportingNodes.size() / (float)(mwidth * mheight);
						int it = (int)ft;
						if (ft != (float)it) {
							//CString s = _T("");  s.Format(_T(" : mwidth = %i : mheight = %i"), mwidth, mheight);
							char s[200];  sprintf_s(s, " : mwidth = %i : mheight = %i", mwidth, mheight);
							addErrorString("Error for " + mname + " : For convs supported by nodes and not using color images, num supporting nodes must be increment of (conv input width * conv input height)" + s);
						}
					}
					else {
						float ft = (float)supportingNodes.size() / (float)(3 * mwidth * mheight);
						int it = (int)ft;
						if (ft != (float)it) {
							//CString s = _T("");  s.Format(_T(" : mwidth = %i : mheight = %i"), mwidth, mheight);
							char s[200];  sprintf_s(s, " : mwidth = %i : mheight = %i", mwidth, mheight);
							addErrorString("Error for " + mname + " : For convs supported by nodes and using color images, num supporting nodes must be increment of (3 * conv input width * conv input height)" + s);
						}
					}
				}
				else {
					//CString s = _T("");  s.Format(_T(" : mwidth = %i : mheight = %i"), mwidth, mheight);
					char s[200];  sprintf_s(s, " : mwidth = %i : mheight = %i", mwidth, mheight);
					addErrorString("Error for " + mname + " : image width and height must be > zero" + s);
				}
				if (stridex >= mxin) {
					nxsteps = mwidth / stridex;		// num out steps wide
				}
				else {
					nxsteps = (mwidth - mxin) / stridex;
				}
				rval.output_width = mwidth;				// out width
				rval.num_width_output_steps = nxsteps;	// num out steps wide
				rval.output_height = mheight;			// out height
				rval.num_height_output_steps = nysteps;	// num steps high
			}
		}

	}

	return(rval);
}

CPoint CAuxFunctions::getInputImageSize()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	return(CPoint(pDoc->image_width, pDoc->image_height));
}

bool CAuxFunctions::getImageGray()
{
	bool rval = false;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	//NetData ndata = pDoc->netData;
	rval = pDoc->gray_images;;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isDataSource(i)) {
			rval = pDoc->netData.getDataImageGray(i);
			i = pDoc->netData.size();
		}
	}
	return(rval);
}

int CAuxFunctions::getNodesPerLayer(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int rval = pDoc->netData.getConvNodesPerLayer(cindex);
	if (pDoc->netData.isConvLayer(cindex)) {
		vector<int> dims = pDoc->netData.getConvDimensions(cindex);
		if (dims.size() > 3) {
			if ((pDoc->netData.getNodeMode(cindex) == "Output") && (pDoc->netData.getConvType(cindex) == IMAGE_CONV)) {
				vector<string> snodes = getImmediateSupportingNodes(pDoc->netData.getID(cindex));
				if (snodes.size() > 0) {
					rval = pDoc->getOutputImageSize();
				}
				else {
					rval = dims[2] * dims[3];
					if (!pDoc->getDataImageGray() && (pDoc->getDataConvType() != CONV_SHARED)) {
						rval = 3 * rval;
					}
				}
			}
			else if (pDoc->netData.getConvType(cindex) == IMAGE_CONV) {
				rval = dims[2] * dims[3];
				if (!pDoc->getDataImageGray() && (pDoc->getDataConvType() != CONV_SHARED)) {
					rval = 3 * rval;
				}
			}
			else if ((pDoc->netData.getConvType(cindex) == DATA_CONV) || (pDoc->netData.getConvType(cindex) == SOUND_CONV)) {
				rval = dims[2] * dims[3];
			}

			if (pDoc->netData.getNodeFunction(cindex) == "MaxPool") {
				rval = dims[0] * dims[1];
			}
		}
	}
	return(rval);
}

double CAuxFunctions::getUpdateEpochs(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	double rval = pDoc->netData.getClusterUpdateEpochs(cindex);
	return(rval);
}

int CAuxFunctions::getNumLayerInputs(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int rval = pDoc->netData.getConvNodesPerLayer(cindex);
	if (pDoc->netData.isConvLayer(cindex)) {
		if (pDoc->netData.getConvType(cindex) == HIDDEN_NET) {
			rval = (int)getImmediateSupportingNodes(cindex).size();
			vector<string> supportingConvs = getImmediateFollowingConvNets(cindex);
			for (int i = 0; i < supportingConvs.size(); i++) {
				int mindex = pDoc->netData.getIndexFromID(supportingConvs[i]);
				vector<int> dims = pDoc->netData.getConvDimensions(mindex);
				if (dims.size() > 3) {
					int ninputs = dims[0] * dims[1];
					if (pDoc->netData.getConvType(mindex) == IMAGE_CONV) {
						if (!pDoc->getDataImageGray() && (pDoc->getDataConvType() != CONV_SHARED)) {
							ninputs = 3 * rval;
						}
					}
					rval += ninputs;
				}
			}
		}
		else {
			vector<int> dims = pDoc->netData.getConvDimensions(cindex);
			if (dims.size() > 3) {
				rval = dims[0] * dims[1];
				if (pDoc->netData.getConvType(cindex) == IMAGE_CONV) {
					if (!pDoc->getDataImageGray() && (pDoc->getDataConvType() != CONV_SHARED)) {
						rval = 3 * rval;
					}
				}
			}
		}
	}
	return(rval);
}

bool CAuxFunctions::displayModelDimensionsForHiddenNet(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isHiddenNetOnly(cindex)) {
		vector<string> supportingNodes = getImmediateSupportingNodes(cindex);
		if (supportingNodes.size() > 0) {
			int mindex = getIndexFromID(supportingNodes[0]);
			if ((pDoc->netData.getNodeMode(mindex) == "Input") || (pDoc->netData.getNodeMode(mindex) == "PreProcess")) {
				return(true);
			}
		}
	}
	return(false);
}

int CAuxFunctions::getAttentionDk(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isAttentionLayer(cindex)) {
		return(pDoc->netData.getDk(cindex));
	}
	return(0);
}

int CAuxFunctions::getAttentionDv(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isAttentionLayer(cindex)) {
		return(pDoc->netData.getDv(cindex));
	}
	return(0);
}

int CAuxFunctions::getAttentionDm(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isAttentionLayer(cindex) || pDoc->netData.isConvLayer(cindex)) {
		return(pDoc->netData.getD(cindex));
	}
	return(0);
}

double CAuxFunctions::getAttentionSumAddLimit(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isAttentionLayer(cindex) || pDoc->netData.isConvLayer(cindex)) {
		return(pDoc->netData.getSumAddLimit(cindex));
	}
	return(0.01);
}

bool CAuxFunctions::hasSoftmaxProcessing(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isAttentionLayer(cindex)) {
		return(pDoc->netData.hasSoftmaxProcessing(cindex));
	}
	return(true);
}

int CAuxFunctions::getAttentionInputSize(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isAttentionLayer(cindex)) {
		int dk = pDoc->netData.getDk(cindex);
		int dv = pDoc->netData.getDv(cindex);
		int dm = pDoc->netData.getD(cindex);
		if (pDoc->netData.getConvType(cindex) == SELFATTENTION) {
			int input_size = (dk * dv * dm) + (dv * dm);
			return(input_size);
		}
		else {
			int input_size = (2 * dk * dm) + (dv * dm);
			return(input_size);
		}
	}
	return(0);
}

int CAuxFunctions::getAttentionOutputSize(int cindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isAttentionLayer(cindex)) {
		int dv = pDoc->netData.getDv(cindex);
		int dm = pDoc->netData.getD(cindex);
		int output_size = dv * dm;
		return(output_size);
	}
	return(0);
}

vector<string> CAuxFunctions::getSupportingConvNets(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	vector<int> following = getSupportingConvNets(sindex, pDoc->netData);
	vector<string> rval;
	for (int i = 0; i < following.size(); i++) {
		rval.push_back(pDoc->netData.getID(following[i]));
	}
	return(rval);
}

vector<int> CAuxFunctions::getSupportingConvNets(int sindex, NetData ndata)
{
	vector<int> supportingConvNets;

	if (!ndata.isConvLayer(sindex)) {
		return(supportingConvNets);
	}

	string nconvname = ndata.getID(sindex);
	
	vector<vector<int>> nodeconnections;

	vector<int> nodeset0;
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConnect(i)) {
			string nname = ndata.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((nconvname == nname2) || (nconvname == nname)) {
				string nname10 = ndata.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = ndata.getIndexFromID(nname12);
				int cindex2 = ndata.getIndexFromID(nname10);
				if (ndata.isConvLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < supportingConvNets.size(); k++) {
						if (cindex1 == supportingConvNets[k]) {
							found = true;
						}
					}
					if (!found) {
						supportingConvNets.push_back(cindex1);
					}
				}
				else if (ndata.isNode(cindex2)) {
					//addErrorString(_T("*** getSupportingConvNets found node ") + ndata.getID(cindex2));
					if ((ndata.getNodeMode(cindex2) == "Hidden") || (ndata.getNodeMode(cindex2) == "PreProcess")) {
						//addErrorString(_T("*** getSupportingConvNets adding node ") + ndata.getID(cindex2));
						bool found = false;
						for (int k = 0; k < nodeset0.size(); k++) {
							if (nodeset0[k] == cindex2) {
								found = true;
							}
						}
						if (!found) {
							nodeset0.push_back(cindex2);
						}
					}
				}
			}
		}
	}
	nodeconnections.push_back(nodeset0);

	bool done = false;
	while (!done) {
		vector<int> nodeset1;
		for (int i = 0; i < ndata.size(); i++) {
			if (ndata.isConnect(i)) {
				for (int j = 0; j < nodeset0.size(); j++) {
					if (ndata.getID(nodeset0[j]) == (ndata.getOutputNode(i))) {
						string nname = ndata.getInputNode(i);
						string nname2 = getConvNameFromConnectName(nname);
						int cindex1 = ndata.getIndexFromID(nname2);
						int cindex2 = ndata.getIndexFromID(nname);
						if (ndata.isConvLayer(cindex1)) {
							bool found = false;
							for (int k = 0; k < supportingConvNets.size(); k++) {
								if (cindex1 == supportingConvNets[k]) {
									found = true;
								}
							}
							if (!found) {
								supportingConvNets.push_back(cindex1);
							}
						}
						else if (ndata.isNode(cindex2)) {
							//addErrorString(_T("*** getSupportingConvNets found node ") + ndata.getID(cindex2));
							if ((ndata.getNodeMode(cindex2) == "Hidden") || (ndata.getNodeMode(cindex2) == "PreProcess")) {
								//addErrorString(_T("*** getSupportingConvNets adding node ") + ndata.getID(cindex2));
								bool found = false;
								for (int k = 0; k < nodeset1.size(); k++) {
									if (nodeset1[k] == cindex2) {
										found = true;
									}
								}
								if (!found) {
									nodeset1.push_back(cindex2);
								}
							}
						}
					}
				}
			}
		}
		if (nodeset1.size() == 0) {
			done = true;
		}
		else {
			nodeconnections.push_back(nodeset1);
			nodeset0 = nodeset1;
		}
	}
	
	return(supportingConvNets);

}

vector<string> CAuxFunctions::getImmediateSupportingConvNets(string id, bool from_any)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	vector<int> following = getImmediateSupportingConvNets(sindex, pDoc->netData, from_any);
	vector<string> rval;
	for (int i = 0; i < following.size(); i++) {
		rval.push_back(pDoc->netData.getID(following[i]));
	}
	return(rval);
}

vector<string> CAuxFunctions::getImmediateSupportingConvNets(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> supportingConvNets;
	string nconvname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname = pDoc->netData.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((nconvname == nname2) || (nconvname == nname)) {
				string nname10 = pDoc->netData.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = pDoc->netData.getIndexFromID(nname12);
				//int cindex2 = pDoc->netData.getIndexFromID(nname10);
				if (pDoc->netData.isConvLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < supportingConvNets.size(); k++) {
						if (nname12 == supportingConvNets[k]) {
							found = true;
						}
					}
					if (!found) {
						supportingConvNets.push_back(nname12);
					}
				}
			}
		}
	}
	return(supportingConvNets);
}

vector<string> CAuxFunctions::getImmediateSupportingLayers(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> supportingConvNets;
	string nconvname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname = pDoc->netData.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((nconvname == nname2) || (nconvname == nname)) {
				string nname10 = pDoc->netData.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = pDoc->netData.getIndexFromID(nname12);
				if (pDoc->netData.isConvLayer(cindex1) || pDoc->netData.isAttentionLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < supportingConvNets.size(); k++) {
						if (nname12 == supportingConvNets[k]) {
							found = true;
						}
					}
					if (!found) {
						//addErrorString("tp3 : " + nname12 + " : " + nname2);
						supportingConvNets.push_back(nname12);
					}
				}
			}
		}
	}
	return(supportingConvNets);
}

vector<string> CAuxFunctions::getImmediateSupportingLayersReturnStrings(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> supportingConvNets;
	string nconvname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname = pDoc->netData.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((nconvname == nname2) || (nconvname == nname)) {
				string nname10 = pDoc->netData.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = pDoc->netData.getIndexFromID(nname12);
				if (pDoc->netData.isConvLayer(cindex1) || pDoc->netData.isAttentionLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < supportingConvNets.size(); k++) {
						if (nname12 == supportingConvNets[k]) {
							found = true;
						}
					}
					if (!found) {
						supportingConvNets.push_back(nname12);
					}
				}
			}
		}
	}
	return(supportingConvNets);
}

vector<int> CAuxFunctions::getImmediateSupportingConvNets(int sindex, NetData ndata, bool from_any)
{
	vector<int> supportingConvNets;
	if (!from_any) {
		if (!ndata.isConvLayer(sindex)) {
			return(supportingConvNets);
		}
	}
	string nconvname = ndata.getID(sindex);
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConnect(i)) {
			string nname = ndata.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((nconvname == nname2) || (nconvname == nname)) {
				string nname10 = ndata.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = ndata.getIndexFromID(nname12);
				int cindex2 = ndata.getIndexFromID(nname10);
				if (ndata.isConvLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < supportingConvNets.size(); k++) {
						if (cindex1 == supportingConvNets[k]) {
							found = true;
						}
					}
					if (!found) {
						supportingConvNets.push_back(cindex1);
					}
				}
			}
		}
	}
	return(supportingConvNets);
}

vector<string> CAuxFunctions::getFollowingConvNets(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	vector<int> following = getFollowingConvNets(sindex);
	vector<string> rval;
	for (int i = 0; i < following.size(); i++) {
		rval.push_back(pDoc->netData.getID(following[i]));
	}
	return(rval);
}

vector<int> CAuxFunctions::getFollowingConvNets(int sindex)
{
	vector<int> followingConvNets;

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	if (!pDoc->netData.isConvLayer(sindex) && !pDoc->netData.isAttentionLayer(sindex)) {
		return(followingConvNets);
	}

	string nconvname = pDoc->netData.getID(sindex);

	vector<vector<int>> nodeconnections;

	vector<int> nodeset0;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname = pDoc->netData.getInputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((nconvname == nname2) || (nconvname == nname)) {
				string nname10 = pDoc->netData.getOutputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = pDoc->netData.getIndexFromID(nname12);
				int cindex2 = pDoc->netData.getIndexFromID(nname10);
				if (pDoc->netData.isConvLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < followingConvNets.size(); k++) {
						if (cindex1 == followingConvNets[k]) {
							found = true;
						}
					}
					if (!found) {
						followingConvNets.push_back(cindex1);
					}
				}
				else if (pDoc->netData.isAttentionLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < followingConvNets.size(); k++) {
						if (cindex1 == followingConvNets[k]) {
							found = true;
						}
					}
					if (!found) {
						followingConvNets.push_back(cindex1);
					}
				}
				else if (pDoc->netData.isNode(cindex2)) {
					if (pDoc->netData.getNodeMode(cindex2) == "Hidden") {
						bool found = false;
						for (int k = 0; k < nodeset0.size(); k++) {
							if (nodeset0[k] == cindex2) {
								found = true;
							}
						}
						if (!found) {
							nodeset0.push_back(cindex2);
						}
					}
				}
			}
		}
	}
	nodeconnections.push_back(nodeset0);

	bool done = false;
	while (!done) {
		vector<int> nodeset1;
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConnect(i)) {
				for (int j = 0; j < nodeset0.size(); j++) {
					if (pDoc->netData.getID(nodeset0[j]) == pDoc->netData.getInputNode(i)) {
						string nname = pDoc->netData.getOutputNode(i);
						string nname2 = getConvNameFromConnectName(nname);
						int cindex1 = pDoc->netData.getIndexFromID(nname2);
						int cindex2 = pDoc->netData.getIndexFromID(nname);
						if (pDoc->netData.isConvLayer(cindex1)) {
							bool found = false;
							for (int k = 0; k < followingConvNets.size(); k++) {
								if (cindex1 == followingConvNets[k]) {
									found = true;
								}
							}
							if (!found) {
								followingConvNets.push_back(cindex1);
							}
						}
						if (pDoc->netData.isAttentionLayer(cindex1)) {
							bool found = false;
							for (int k = 0; k < followingConvNets.size(); k++) {
								if (cindex1 == followingConvNets[k]) {
									found = true;
								}
							}
							if (!found) {
								followingConvNets.push_back(cindex1);
							}
						}
						else if (pDoc->netData.isNode(cindex2)) {
							if (pDoc->netData.getNodeMode(cindex2) == "Hidden") {
								bool found = false;
								for (int k = 0; k < nodeset1.size(); k++) {
									if (nodeset1[k] == cindex2) {
										found = true;
									}
								}
								if (!found) {
									nodeset1.push_back(cindex2);
								}
							}
						}
					}
				}
			}
		}
		if (nodeset1.size() == 0) {
			done = true;
		}
		else {
			nodeconnections.push_back(nodeset1);
			nodeset0 = nodeset1;
		}
	}

	return(followingConvNets);

}

vector<string> CAuxFunctions::getImmediateFollowingConvNets(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	vector<int> following = getImmediateFollowingConvNets(sindex, pDoc->netData);
	vector<string> rval;
	for (int i = 0; i < following.size(); i++) {
		rval.push_back(pDoc->netData.getID(following[i]));
	}
	return(rval);
}

vector<string> CAuxFunctions::getImmediateFollowingConvNets(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> followingNodes;
	string fname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname2 = getConvNameFromConnectName(pDoc->netData.getInputNode(i));
			if (fname == nname2) {
				string nname3 = getConvNameFromConnectName(pDoc->netData.getOutputNode(i));
				int cindex1 = pDoc->netData.getIndexFromID(nname3);
				if (pDoc->netData.isConvLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < followingNodes.size(); k++) {
						if (nname3 == followingNodes[k]) {
							found = true;
						}
					}
					if (!found) {
						followingNodes.push_back(pDoc->netData.getOutputNode(i));
					}
				}
			}
		}
	}
	return(followingNodes);
}

vector<int> CAuxFunctions::getImmediateFollowingConvNets(int sindex, NetData ndata)
{
	vector<int> followingNodes;
	string fname = ndata.getID(sindex);
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConnect(i)) {
			string nname2 = getConvNameFromConnectName(ndata.getInputNode(i));
			if (fname == nname2) {
				int cindex1 = ndata.getIndexFromID(getConvNameFromConnectName(ndata.getOutputNode(i)));
				if (ndata.isConvLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < followingNodes.size(); k++) {
						if (cindex1 == followingNodes[k]) {
							found = true;
						}
					}
					if (!found) {
						followingNodes.push_back(cindex1);
					}
				}
			}
		}
	}
	return(followingNodes);
}

vector<string> CAuxFunctions::getImmediateFollowingAttentionLayers(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> followingNodes;
	string fname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname2 = getConvNameFromConnectName(pDoc->netData.getInputNode(i));
			if (fname == nname2) {
				string nname3 = getConvNameFromConnectName(pDoc->netData.getOutputNode(i));
				int cindex1 = pDoc->netData.getIndexFromID(nname3);
				if (pDoc->netData.isAttentionLayer(cindex1)) {
					bool found = false;
					for (int k = 0; k < followingNodes.size(); k++) {
						if (nname3 == followingNodes[k]) {
							found = true;
						}
					}
					if (!found) {
						followingNodes.push_back(nname3);
					}
				}
			}
		}
	}
	return(followingNodes);
}

vector<string> CAuxFunctions::getFollowingNodes(string id, bool nodes_only)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	vector<int> following = getFollowingNodes(sindex, pDoc->netData, nodes_only);
	vector<string> rval;
	for (int i = 0; i < following.size(); i++) {
		rval.push_back(pDoc->netData.getID(following[i]));
	}
	return(rval);
}

vector<int> CAuxFunctions::getFollowingNodes(int sindex, NetData ndata, bool nodes_only)
{
	vector<int> followingNodes;
	string fname = ndata.getID(sindex);
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConnect(i)) {
			string nname = ndata.getInputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((fname == nname2) || (fname == nname)) {
				string nname10 = ndata.getOutputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = ndata.getIndexFromID(nname12);
				int cindex2 = ndata.getIndexFromID(nname10);
				if (ndata.isConvLayer(cindex1)) {
					if (!nodes_only) {
						bool found = false;
						for (int k = 0; k < followingNodes.size(); k++) {
							if (cindex1 == followingNodes[k]) {
								found = true;
							}
						}
						if (!found) {
							followingNodes.push_back(cindex1);
						}
					}
				}
				else if (ndata.isNode(cindex2)) {
					bool found = false;
					for (int k = 0; k < followingNodes.size(); k++) {
						if (followingNodes[k] == cindex2) {
							found = true;
						}
					}
					if (!found) {
						followingNodes.push_back(cindex2);
					}
				}
			}
		}
	}
	return(followingNodes);
}

vector<string> CAuxFunctions::getImmediateFollowingNodes(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> followingNodes;
	string fname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname = pDoc->netData.getInputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((fname == nname2) || (fname == nname)) {
				string nname10 = pDoc->netData.getOutputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = pDoc->netData.getIndexFromID(nname12);
				int cindex2 = pDoc->netData.getIndexFromID(nname10);
				if (pDoc->netData.isNode(cindex2)) {
					bool found = false;
					for (int k = 0; k < followingNodes.size(); k++) {
						if (followingNodes[k] == nname10) {
							found = true;
						}
					}
					if (!found) {
						followingNodes.push_back(nname10);
					}
				}
			}
		}
	}
	return(followingNodes);
}

vector<string> CAuxFunctions::getImmediateSupportingNodes(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	vector<int> following = getImmediateSupportingNodes(sindex, pDoc->netData);
	vector<string> rval;
	for (int i = 0; i < following.size(); i++) {
		rval.push_back(pDoc->netData.getID(following[i]));
	}
	return(rval);
}

vector<string> CAuxFunctions::getImmediateSupportingNodes(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> supportingNodes;
	string fname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname = pDoc->netData.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((fname == nname2) || (fname == nname)) {
				string nname10 = pDoc->netData.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = pDoc->netData.getIndexFromID(nname12);
				int cindex2 = pDoc->netData.getIndexFromID(nname10);
				if (pDoc->netData.isNode(cindex2)) {
					bool found = false;
					for (int k = 0; k < supportingNodes.size(); k++) {
						if (supportingNodes[k] == nname10) { 
							found = true;
						}
					}
					if (!found) {
						supportingNodes.push_back(nname10);
					}
				}
			}
		}
	}
	return(supportingNodes);
}

vector<string> CAuxFunctions::getImmediateSupportingNodesReturnStrings(int sindex)  
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> supportingNodes;
	string fname = pDoc->netData.getID(sindex);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string nname = pDoc->netData.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((fname == nname2) || (fname == nname)) {
				string nname10 = pDoc->netData.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = pDoc->netData.getIndexFromID(nname12);
				int cindex2 = pDoc->netData.getIndexFromID(nname10);
				if (pDoc->netData.isNode(cindex2)) {
					bool found = false;
					for (int k = 0; k < supportingNodes.size(); k++) {
						if (supportingNodes[k] == nname10) {
							found = true;
						}
					}
					if (!found) {
						supportingNodes.push_back(nname10);
					}
				}
			}
		}
	}
	return(supportingNodes);
}

vector<int> CAuxFunctions::getImmediateSupportingNodes(int sindex, NetData ndata)
{
	vector<int> supportingNodes;
	string fname = ndata.getID(sindex);
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConnect(i)) {
			string nname = ndata.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if ((fname == nname2) || (fname == nname)) {
				string nname10 = ndata.getInputNode(i);
				string nname12 = getConvNameFromConnectName(nname10);
				int cindex1 = ndata.getIndexFromID(nname12);
				int cindex2 = ndata.getIndexFromID(nname10);
				if (ndata.isNode(cindex2)) {
					bool found = false;
					for (int k = 0; k < supportingNodes.size(); k++) {
						if (supportingNodes[k] == cindex2) {
							found = true;
						}
					}
					if (!found) {
						supportingNodes.push_back(cindex2);
					}
				}
			}
		}
	}
	return(supportingNodes);
}

vector<string> CAuxFunctions::getConvOrder()
{
	vector<string> rval;

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	vector <string> conv_names;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConvLayer(i) || pDoc->netData.isAttentionLayer(i)) {
			bool found = false;
			for (int j = 0; j < conv_names.size(); j++) {
				if (pDoc->netData.getID(i) == conv_names[j]) {
					found = true;
					j = (int)conv_names.size();
				}
			}
			if (!found) {
				conv_names.push_back(pDoc->netData.getID(i));
				//addErrorString("getConvOrder adding " + pDoc->netData.getID(i));
			}
		}
	}

	if (conv_names.size() > 0) {

		typedef struct loopControlData {
			string convin;
			string convout;
		} loopControlData;
		vector<loopControlData> cdata;

		vector<bool> convs_used;
		for (int i = 0; i < conv_names.size(); i++) {
			convs_used.push_back(false);
		}

		int cnt = 0; 
		bool done = false;

		while (!done && (cnt < pDoc->netData.size())) {

			bool changed = false;

			for (int i = 0; i < conv_names.size(); i++) {

				if (!convs_used[i]) {

					vector<string> following = getFollowingConvNets(conv_names[i]);

					if (following.size() > 1) {
						addErrorString("getSortedConvolutions : parallel convolutions currently not supported");
						return(rval);
					}
					else if (following.size() == 1) {

						loopControlData mdata;
						mdata.convin = conv_names[i];
						mdata.convout = following[0];

						if (cdata.size() == 0) {
							cdata.push_back(mdata);
							convs_used[i] = true;
							changed = true;
						}
						else {
							for (int j = 0; j < cdata.size(); j++) {
								if (mdata.convin == cdata[j].convout) {
									if (j == (cdata.size() - 1)) {
										cdata.push_back(mdata);
									}
									else {
										cdata.insert(cdata.begin() + j + 1, mdata);
									}
									convs_used[i] = true;
									j = (int)cdata.size();
									changed = true;
								}
								else if (mdata.convout == cdata[j].convin) {
									cdata.insert(cdata.begin() + j, mdata);
									convs_used[i] = true;
									j = (int)cdata.size();
									changed = true;
								}
							}
						}

					}
					else {

						loopControlData mdata;
						mdata.convin = conv_names[i];
						mdata.convout = "";

						if (cdata.size() == 0) {
							cdata.push_back(mdata);
							convs_used[i] = true;
							changed = true;
						}
						else {
							for (int j = 0; j < cdata.size(); j++) {
								if (mdata.convin == cdata[j].convout) {
									if (j == (cdata.size() - 1)) {
										cdata.push_back(mdata);
									}
									else {
										cdata.insert(cdata.begin() + j + 1, mdata);
									}
									convs_used[i] = true;
									j = (int)cdata.size();
									changed = true;
								}
							}
							if (!changed) {
								vector<string> supporting = getSupportingConvNets(mdata.convin);
								if (supporting.size() > 0) {
									for (int m = 0; m < supporting.size(); m++) {
										for (int n = 0; n < conv_names.size(); n++) {
											if (convs_used[n]) {
												if (supporting[m] == conv_names[n]) {
													cdata.push_back(mdata);
													convs_used[i] = true;
													changed = true;
													m = (int)supporting.size();
													n = (int)conv_names.size();
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

			if (!changed) {
				done = false;
			}
			else {
				done = true;
				for (int i = 0; i < conv_names.size(); i++) {
					if (!convs_used[i]) {
						done = false;
					}
				}
			}

			cnt++;

		}

		/*for (int i = 0; i < conv_names.size(); i++) {
			if (!convs_used[i]) {
				addErrorString("*** getConvOrder : Unable to find order for " + conv_names[i]);
			}
		}*/

		for (int i = 0; i < cdata.size(); i++) {
			rval.push_back(cdata[i].convin);
			//addErrorString(_T("*** conv sorted : ") + rval[i]);
		}

	}

	return(rval);
}

int CAuxFunctions::getConvInputX(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	return(getConvInputX(sindex, pDoc->netData));
}

int CAuxFunctions::getConvInputX(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<int> dimensions = pDoc->netData.getConvDimensions(sindex);
	if (pDoc->netData.getConvType(sindex) == DATA_CONV) {
		return(getInputWidthFromLayerName(pDoc->netData.getID(sindex)));
	}
	else {
		if (dimensions.size() > 0) {
			return(dimensions[0]);
		}
	}
	return(0);
}

int CAuxFunctions::getConvInputX(int sindex, NetData ndata)
{
	vector<int> dimensions = ndata.getConvDimensions(sindex);
	if (ndata.getConvType(sindex) == DATA_CONV) {
		return(getInputWidthFromLayerName(ndata.getID(sindex)));
	}
	else {
		if (dimensions.size() > 0) {
			return(dimensions[0]);
		}
	}
	return(0);
}

int CAuxFunctions::getConvInputY(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	return(getConvInputY(sindex, pDoc->netData));
}

int CAuxFunctions::getConvInputY(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<int> dimensions = pDoc->netData.getConvDimensions(sindex);
	if (dimensions.size() > 1) {
		return(dimensions[1]);
	}
	return(0);
}

int CAuxFunctions::getConvInputY(int sindex, NetData ndata)
{
	vector<int> dimensions = ndata.getConvDimensions(sindex);
	if (dimensions.size() > 1) {
		return(dimensions[1]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputX(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	return(getConvOutputX(sindex, pDoc->netData));
}

int CAuxFunctions::getConvOutputX(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<int> dimensions = pDoc->netData.getConvDimensions(sindex);
	if (dimensions.size() > 2) {
		return(dimensions[2]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputX(int sindex, NetData ndata)
{
	vector<int> dimensions = ndata.getConvDimensions(sindex);
	if (dimensions.size() > 2) {
		return(dimensions[2]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputY(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	return(getConvOutputY(sindex, pDoc->netData));
}

int CAuxFunctions::getConvOutputY(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<int> dimensions = pDoc->netData.getConvDimensions(sindex);
	if (dimensions.size() > 3) {
		return(dimensions[3]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputY(int sindex, NetData ndata)
{
	vector<int> dimensions = ndata.getConvDimensions(sindex);
	if (dimensions.size() > 3) {
		return(dimensions[3]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputStrideX(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	return(getConvOutputStrideX(sindex, pDoc->netData));
}

int CAuxFunctions::getConvOutputStrideX(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<int> strides = pDoc->netData.getConvStrides(sindex);
	if (strides.size() > 0) {
		return(strides[0]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputStrideX(int sindex, NetData ndata)
{
	vector<int> strides = ndata.getConvStrides(sindex);
	if (strides.size() > 0) {
		return(strides[0]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputStrideY(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int sindex = pDoc->netData.getIndexFromID(id);
	return(getConvOutputStrideY(sindex, pDoc->netData));
}

int CAuxFunctions::getConvOutputStrideY(int sindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<int> strides = pDoc->netData.getConvStrides(sindex);
	if (strides.size() > 1) {
		//char s[200];  sprintf_s(s, "CAuxFunctions::getConvOutputStrideY = %i : ", strides[1]); addErrorString(s + pDoc->netData.getID(sindex));
		return(strides[1]);
	}
	return(0);
}

int CAuxFunctions::getConvOutputStrideY(int sindex, NetData ndata)
{
	vector<int> strides = ndata.getConvStrides(sindex);
	if (strides.size() > 1) {
		return(strides[1]);
	}
	return(0);
}

int CAuxFunctions::getImageConvOutputWidth(int cindex)
{
	int numxsteps = 0;
	int nxin = getConvInputX(cindex);
	int nxout = getConvOutputX(cindex);
	int nxstride = getConvOutputStrideX(cindex);
	imageSizeData imageSize = getImageSizeData(cindex);
	int input_width = imageSize.input_width;
	if (nxstride > 0) {
		if (nxstride > nxin) {
			numxsteps = (input_width - nxin) / nxstride;
		}
		else {
			numxsteps = input_width / nxstride;
		}
		if (numxsteps <= 0) {
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			//CString s = _T("");  s.Format(_T(" : nxin = %i : nxstride = %i : input_width = %i"), nxin, nxstride, input_width);
			char s[200]; sprintf_s(s, " : nxin = %i : nxstride = %i : input_width = %i", nxin, nxstride, input_width);
			addErrorString("getImageConvOutputWidth for " + pDoc->netData.getID(cindex) + " zero x steps not allowed (1)" + s);
		}
	}
	else {
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		//CString s = _T("");  s.Format(_T(" : nxin = %i : nxstride = %i : input_width = %i"), nxin, nxstride, input_width);
		char s[200]; sprintf_s(s, " : nxin = %i : nxstride = %i : input_width = %i", nxin, nxstride, input_width);
		addErrorString("getImageConvOutputWidth for " + pDoc->netData.getID(cindex) + " zero x stride not allowed (2)" + s);
	}
	int output_width = numxsteps * nxout;
	return(output_width);
}

int CAuxFunctions::getImageConvOutputHeight(int cindex)
{
	int numysteps = 0;
	int nyin = getConvInputY(cindex);
	int nyout = getConvOutputY(cindex);
	int nystride = getConvOutputStrideY(cindex);
	imageSizeData imageSize = getImageSizeData(cindex);
	int input_height = imageSize.input_height;
	if (nystride > 0) {
		if (nystride > nyin) {
			numysteps = (input_height - nyin) / nystride;
		}
		else {
			numysteps = input_height / nystride;
		}
		if (numysteps <= 0) {
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			//CString s = _T("");  s.Format(_T(" : nyin = %i : nystride = %i : input_height = %i"), nyin, nystride, input_height);
			char s[200]; sprintf_s(s, " : nyin = %i : nystride = %i : input_height = %i", nyin, nystride, input_height);
			addErrorString("getImageConvOutputHeight for " + pDoc->netData.getID(cindex) + " zero y steps not allowed (1)" + s);
		}
	}
	else {
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		//CString s = _T("");  s.Format(_T(" : nyin = %i : nystride = %i : input_height = %i"), nyin, nystride, input_height);
		char s[200]; sprintf_s(s, " : nyin = %i : nystride = %i : input_height = %i", nyin, nystride, input_height);
		addErrorString("getConvOutputHeight for " + pDoc->netData.getID(cindex) + " zero y stride not allowed (2)" + s);
	}
	int output_height = numysteps * nyout;
	return(output_height);
}

int CAuxFunctions::getDataConvOutputSize(int cindex)
{
	int outsize = 0;
	int mxin = getConvInputX(cindex);
	int dimyin = getConvInputY(cindex);
	int dimyout = getConvOutputY(cindex);
	int stridexin = getConvOutputStrideX(cindex);
	int strideyin = getConvOutputStrideY(cindex);
	if (stridexin > 0) {
		outsize = dimyout * (1 + (strideyin - dimyin) / stridexin);
	}
	if (outsize < 0) {
		outsize = 0;
	}
	outsize = mxin * outsize;
	return(outsize);
}

int CAuxFunctions::countOutputNodes(int sindex, NetData ndata)
{
	int num_output_nodes = 0;

	if (!ndata.isConvLayer(sindex)) {
		return(num_output_nodes);
	}

	string nconvname = ndata.getID(sindex);

	vector<int> nodeset0;
	vector<int> nodeset2;
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConnect(i)) {
			string nname = ndata.getInputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if (nconvname == nname2) {
				int cindex = ndata.getIndexFromID(ndata.getOutputNode(i));
				if (ndata.isNode(cindex)) {
					if (ndata.getNodeMode(cindex) == "Hidden") {
						bool found = false;
						for (int k = 0; k < nodeset0.size(); k++) {
							if (nodeset0[k] == cindex) {
								found = true;
							}
						}
						if (!found) {
							nodeset0.push_back(cindex);
						}
					}
					else if (ndata.getNodeMode(cindex) == "Output") {
						bool found = false;
						for (int k = 0; k < nodeset2.size(); k++) {
							if (nodeset2[k] == cindex) {
								found = true;
							}
						}
						if (!found) {
							nodeset2.push_back(cindex);
							num_output_nodes++;
						}
					}
				}
			}
		}
	}

	bool done = false;
	while (!done) {
		vector<int> nodeset1;
		for (int i = 0; i < ndata.size(); i++) {
			if (ndata.isConnect(i)) {
				for (int j = 0; j < nodeset0.size(); j++) {
					if (ndata.getID(nodeset0[j]) == ndata.getInputNode(i)) {
						int cindex2 = ndata.getIndexFromID(ndata.getOutputNode(i));
						if (ndata.isNode(cindex2)) {
							if (ndata.getNodeMode(cindex2) == "Hidden") {
								bool found = false;
								for (int k = 0; k < nodeset1.size(); k++) {
									if (nodeset1[k] == cindex2) {
										found = true;
									}
								}
								if (!found) {
									nodeset1.push_back(cindex2);
								}
							}
							else if (ndata.getNodeMode(cindex2) == "Output") {
								bool found = false;
								for (int k = 0; k < nodeset2.size(); k++) {
									if (nodeset2[k] == cindex2) {
										found = true;
									}
								}
								if (!found) {
									nodeset2.push_back(cindex2);
									num_output_nodes++;
								}
							}
						}
					}
				}
			}
		}
		if (nodeset1.size() == 0) {
			done = true;
		}
		else {
			nodeset0 = nodeset1;
		}
	}

	return(num_output_nodes);

}

int CAuxFunctions::countInputNodes(int sindex, NetData ndata)
{
	int num_input_nodes = 0;

	if (!ndata.isConvLayer(sindex)) {
		return(num_input_nodes);
	}

	string nconvname = ndata.getID(sindex);

	vector<int> nodeset0;
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConnect(i)) {
			string nname = ndata.getOutputNode(i);
			string nname2 = getConvNameFromConnectName(nname);
			if (nconvname == nname2) {
				int cindex = ndata.getIndexFromID(ndata.getInputNode(i));
				if (ndata.isNode(cindex)) {
					if (ndata.getNodeMode(cindex) == "Hidden") {
						bool found = false;
						for (int k = 0; k < nodeset0.size(); k++) {
							if (nodeset0[k] == cindex) {
								found = true;
							}
						}
						if (!found) {
							nodeset0.push_back(cindex);
						}
					}
					else if (ndata.getNodeMode(cindex) == "Input") {
						num_input_nodes++;
					}
				}
			}
		}
	}

	bool done = false;
	while (!done) {
		vector<int> nodeset1;
		for (int i = 0; i < ndata.size(); i++) {
			if (ndata.isConnect(i)) {
				for (int j = 0; j < nodeset0.size(); j++) {
					if (ndata.getID(nodeset0[j]) == ndata.getOutputNode(i)) {
						int cindex2 = ndata.getIndexFromID(ndata.getInputNode(i));
						if (ndata.isNode(cindex2)) {
							if (ndata.getNodeMode(cindex2) == "Hidden") {
								bool found = false;
								for (int k = 0; k < nodeset1.size(); k++) {
									if (nodeset1[k] == cindex2) {
										found = true;
									}
								}
								if (!found) {
									nodeset1.push_back(cindex2);
								}
							}
							else if (ndata.getNodeMode(cindex2) == "Input") {
								num_input_nodes++;
							}
						}
					}
				}
			}
		}
		if (nodeset1.size() == 0) {
			done = true;
		}
		else {
			nodeset0 = nodeset1;
		}
	}

	return(num_input_nodes);

}

bool CAuxFunctions::getConvFromConnectName(string cname)
{
	if ((cname.substr(0,9) == "ConvLayer") || (cname.substr(0, 5) == "Layer") || (cname.substr(0, 9) == "ImageData") ||
		(cname.substr(0, 9) == "Attention") || (cname.substr(0, 10) == "PatternNet") || (cname.substr(0, 7) == "Dropout") || 
		(cname.substr(0, 8) == "Assembly")) {
		return(true);
	}
	return(false);
}

bool CAuxFunctions::getConvFromConnectName(CString cname)
{
	if ((cname.Left(9) == _T("ConvLayer")) || (cname.Left(5) == _T("Layer")) || (cname.Left(9) == _T("ImageData")) ||
		(cname.Left(9) == _T("Attention")) || (cname.Left(10) == _T("PatternNet")) || (cname.Left(7) == _T("Dropout")) || 
		(cname.Left(8) == _T("Assembly"))) {
		return(true);
	}
	return(false);
}

string CAuxFunctions::getConvNameFromConnectName(string cname)
{
	string rval = cname.substr(0, 5);
	if ((cname.substr(0, 9) == "ConvLayer") || (rval == "Layer") || (cname.substr(0, 9) == "ImageData") ||
		(cname.substr(0, 9) == "Attention") || (cname.substr(0, 10) == "PatternNet") || (cname.substr(0, 7) == "Dropout") || 
		(cname.substr(0, 8) == "Assembly")) {
		int k = (int)rval.length();
		while ((k < cname.length()) && (cname[k] != ':')) {
			rval += cname[k];
			k++;
		}
		return(rval);
	}
	return(cname);
}

CString CAuxFunctions::getConvNameFromConnectName(CString cname)
{
	CString rval = cname.Left(5);
	if ((cname.Left(9) == _T("ConvLayer")) || (rval == _T("Layer")) || (cname.Left(9) == _T("ImageData")) ||
		(cname.Left(9) == _T("Attention")) || (cname.Left(10) == _T("PatternNet")) || (cname.Left(7) == _T("Dropout")) || 
		(cname.Left(8) == _T("Assembly"))) {
		int k = rval.GetLength();
		while ((k < cname.GetLength()) && (cname[k] != ':')) {
			rval += cname[k];
			k++;
		}
		return(rval);
	}
	return(cname);
}

int CAuxFunctions::getConvNodeFromConnectName(CString cname)
{
	int rval = -1;
	CString name = cname.Left(5);
	if ((cname.Left(9) == _T("ConvLayer")) || (cname.Left(5) == _T("Layer")) || (cname.Left(9) == _T("ImageData"))
		|| (cname.Left(9) == _T("Attention")) || (cname.Left(10) == _T("PatternNet")) || (cname.Left(7) == _T("Dropout")) ||
		(cname.Left(8) == _T("Assembly"))) {
		int k = name.GetLength();
		while ((k < cname.GetLength()) && (cname[k] != ':')) {
			k++;
		}
		if ((k + 3) < cname.GetLength()) {
			CString t3 = cname.Right(cname.GetLength() - (k + 3));
			rval = (int)_tstof((LPCTSTR)t3);
		}
	}
	return(rval);
}

bool CAuxFunctions::getConvInputFromConnectName(CString cname)
{
	int rval = true;
	CString name = cname.Left(5);
	if ((cname.Left(9) == _T("ConvLayer")) || (cname.Left(5) == _T("Layer")) || (cname.Left(9) == _T("ImageData")) || 
		(cname.Left(9) == _T("Attention")) || (cname.Left(10) == _T("PatternNet")) || (cname.Left(7) == _T("Dropout")) || 
		(cname.Left(8) == _T("Assembly"))) {
		int k = name.GetLength();
		while ((k < cname.GetLength()) && (cname[k] != ':')) {
			k++;
		}
		if ((k + 1) < cname.GetLength()) {
			if (cname[k + 1] == 'o') {
				rval = false;
			}
		}
	}
	return(rval);
}

bool CAuxFunctions::getConvGrayFromConnectName(CString cname)
{
	bool rval = false;
	CString name = cname.Left(5);
	if ((cname.Left(9) == _T("ConvLayer")) || (cname.Left(5) == _T("Layer")) || (cname.Left(9) == _T("ImageData"))) {
		int k = name.GetLength();
		while ((k < cname.GetLength()) && (cname[k] != ':')) {
			k++;
		}
		k++;
		while ((k < cname.GetLength()) && (cname[k] != ':')) {
			k++;
		}
		if (k <= cname.GetLength()) {
			if (cname[k - 1] == 'y') {
				rval = true;
			}
		}
	}
	return(rval);
}

bool CAuxFunctions::hasSingleColon(CString cname)
{
	CString mname = cname;
	for (int i = 0; i < mname.GetLength(); i++) {
		if (mname[i] == ':') {
			if (i < (mname.GetAllocLength() - 1)) {
				if (mname[i + 1] != ':') {
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

string CAuxFunctions::getStringBeforeColon(string cname)
{
	string mname = cname;
	for (int i = 0; i < mname.length(); i++) {
		if (mname[i] == ':') {
			return(mname.substr(0, i));
		}
	}
	return(cname);
}

CString CAuxFunctions::getStringBeforeColon(CString cname)
{
	CString mname = cname;
	for (int i = 0; i < mname.GetLength(); i++) {
		if (mname[i] == ':') {
			return(mname.Left(i));
		}
	}
	return(cname);
}

string CAuxFunctions::getStringAfterColon(string cname)
{
	string mname = cname;
	for (int i = 0; i < mname.length(); i++) {
		if (mname[i] == ':') {
			//addErrorString("getStringAfterColon : " + cname + " : " + mname.substr(i + 1));
			return(mname.substr(i + 1));
		}
	}
	return(cname);
}

CString CAuxFunctions::getStringAfterColon(CString cname)
{
	CString mname = cname;
	for (int i = 0; i < mname.GetLength(); i++) {
		if (mname[i] == ':') {
			return(mname.Right(cname.GetLength() - i - 1));
		}
	}
	return(cname);
}

int CAuxFunctions::getFirstNumberAfterColon(CString cname)
{
	CString mname = cname;
	for (int i = 0; i < mname.GetLength(); i++) {
		if (mname[i] == ':') {
			CString t3 = cname.Right(cname.GetLength() - i - 1);
			if (isWordNumber(string(CT2CA(t3)))) {
				return((int)_tstof((LPCTSTR)t3));
			}
			else {
				i = mname.GetLength();
			}
		}
	}
	return(-1);
}

int CAuxFunctions::getSecondNumberAfterColon(string cname)
{
	int k = 0;
	string name = cname;
	while ((k < cname.length()) && (cname[k] != ':')) {
		k++;
	}
	k++;
	while ((k < cname.length()) && (cname[k] != ':')) {
		k++;
	}
	if (k <= cname.length()) {
		string t3 = cname.substr(k + 1);
		//addErrorString("getSecondNumberAfterColon : " + cname + " : " + t3);
		if (isWordNumber(t3)) {
			return((int)_tstof((LPCTSTR)(CString)t3.c_str()));
		}
	}
	return(-1);
}

int CAuxFunctions::getSecondNumberAfterColon(CString cname)
{
	int k = 0;
	CString name = cname;
	while ((k < cname.GetLength()) && (cname[k] != ':')) {
		k++;
	}
	k++;
	while ((k < cname.GetLength()) && (cname[k] != ':')) {
		k++;
	}
	if (k <= cname.GetLength()) {
		CString t3 = cname.Right(cname.GetLength() - k - 1);
		if (isWordNumber(string(CT2CA(t3)))) {
			return((int)_tstof((LPCTSTR)t3));
		}
	}
	return(-1);
}

vector<string> CAuxFunctions::getInputFieldsFromNodeName(int index)
{
	vector<string> mfields;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isNode(index)) {
		if (pDoc->netData.getNodeMode(index) == "Input") {
			if (pDoc->netData.getNodeIndexMode(index) == "Index") {
				int mindex = pDoc->netData.getNodeXIndex(index);
				if (mindex < 0) {
					//string cindex = _T("");  cindex.Format(_T(":%i"), -mindex);
					char cindex[200]; sprintf_s(cindex, ":%i", -mindex);
					if (pDoc->netData.getNumDataFields(index) > 0) {
						mfields = pDoc->netData.getDataFields(index);
						for (int k = 0; k < mfields.size(); k++) {
							mfields[k] = mfields[k] + cindex;
						}
					}
					else {
						mfields.push_back(pDoc->netData.getDataField(index) + cindex);
					}
				}
			}
		}
	}
	return(mfields);
}

vector<string> CAuxFunctions::getInputFieldsFromNodeName(string cname)
{
	vector<string> mfields;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			if (pDoc->netData.getNodeMode(i) == "Input") {
				if (cname == pDoc->netData.getID(i)) {
					if (pDoc->netData.getNodeIndexMode(i) == "Index") {
						int mindex = pDoc->netData.getNodeXIndex(i);
						if (mindex < 0) {
							//CString cindex = _T("");  cindex.Format(_T(":%i"), -mindex);
							char cindex[200];  sprintf_s(cindex, ":%i", -mindex);
							if (pDoc->netData.getNumDataFields(i) > 0) {
								vector<string> tfields = pDoc->netData.getDataFields(i);
								for (int k = 0; k < tfields.size(); k++) {
									mfields[k] = tfields[k] + cindex;
								}
							}
							else {
								mfields.push_back(pDoc->netData.getDataField(i) + cindex);
							}
						}
					}
				}
			}
		}
	}
	return(mfields);
}

vector<string> CAuxFunctions::getInputFieldsFromLayerName(int index)
{
	vector<string> mfields;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isConvLayer(index)) {
		if ((pDoc->netData.getNodeMode(index) == "Input") && (pDoc->netData.getConvType(index) == DATA_CONV)) {
			if (pDoc->netData.getNumDataFields(index) > 0) {
				mfields = pDoc->netData.getDataFields(index);
			}
			else {
				mfields.push_back(pDoc->netData.getDataField(index));
			}
		}
	}
	return(mfields);
}

vector<string> CAuxFunctions::getInputFieldsFromLayerNameReturnString(int index)
{
	vector<string> mfields;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isConvLayer(index)) {
		if ((pDoc->netData.getNodeMode(index) == "Input") && (pDoc->netData.getConvType(index) == DATA_CONV)) {
			if (pDoc->netData.getNumDataFields(index) > 0) {
				for (int i = 0; i < pDoc->netData.getDataFields(index).size(); i++) {
					mfields.push_back(pDoc->netData.getDataFields(index)[i]);
				}
			}
			else {
				mfields.push_back(pDoc->netData.getDataField(index));
			}
		}
	}
	return(mfields);
}

vector<string> CAuxFunctions::getInputFieldsFromLayerName(string cname)
{
	vector<string> mfields;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConvLayer(i)) {
			if ((pDoc->netData.getNodeMode(i) == "Input") && (pDoc->netData.getConvType(i) == DATA_CONV)) {
				if (pDoc->netData.getID(i) == cname) {
					if (pDoc->netData.getNumDataFields(i) > 0) {
						vector<string> tfields = pDoc->netData.getDataFields(i);
						for (int i = 0; i < tfields.size(); i++) {
							mfields.push_back(tfields[i]);
						}
					}
					else {
						mfields.push_back(pDoc->netData.getDataField(i));
					}
				}
			}
		}
	}
	return(mfields);
}

vector<string> CAuxFunctions::getOutputFieldsFromLayerName(string cname)
{
	vector<string> mfields;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConvLayer(i)) {
			if ((pDoc->netData.getNodeMode(i) == "Output") && (pDoc->netData.getConvType(i) == DATA_CONV)) {
				if (cname == pDoc->netData.getID(i)) {
					if (pDoc->netData.getNumDataFields(i) > 0) {
						vector<string> tfields = pDoc->netData.getDataFields(i);
						for (int j = 0; j < tfields.size(); j++) {
							mfields.push_back(tfields[j]);
						}
					}
					else {
						mfields.push_back(pDoc->netData.getDataField(i));
					}
				}
			}
		}
	}
	return(mfields);
}

int CAuxFunctions::getInputWidthFromLayerName(string cname)
{
	int supporting_count = 0;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	vector<string> supporting = getImmediateSupportingNodes(cname);
	if (supporting.size() > 0) {
		for (int k = 0; k < supporting.size(); k++) {
			int sindex = pDoc->netData.getIndexFromID(supporting[k]);
			string process = pDoc->netData.getNodePreProcessor(sindex);
			if (pDoc->netData.isNode(sindex) && (pDoc->netData.getNodeMode(sindex) == "PreProcess") && (process.substr(0, 3) == "FFT")) {
				int size = (int)_tstof((LPCTSTR)(CString)process.substr(3).c_str());
				//char p[200]; sprintf_s(p, "%i", size);  addErrorString("getInputWidthFromLayerName : " + process + " : " + process.substr(3) + " : " + p);
				supporting_count += size;
			}
			else if (pDoc->netData.isNode(sindex) && (pDoc->netData.getNodeMode(sindex) == "PreProcess")) {
				int nfields = (int)getInputFieldsFromNodeName(supporting[k]).size();
				if (nfields > 0) {
					supporting_count += nfields;
				}
				else {
					vector<string> supporting3 = getSupportingConvNets(cname);
					if (supporting3.size() > 0) {
						nfields = (int)getInputFieldsFromLayerName(supporting3[0]).size();
						if (nfields > 0) {
							supporting_count += nfields;
						}
						else {
							supporting_count++;
						}
					}
					else {
						supporting_count++;
					}
				}
			}
			else {
				supporting_count++;
			}
		}
	}
	else {
		vector<string> supporting2 = getImmediateSupportingConvNets(cname);
		if (supporting2.size() == 1) {
			supporting_count = getInputWidthFromLayerName(supporting2[0]);
		}
		else if (supporting2.size() > 1) {
			//addErrorString(_T("Error 6783412"));
			for (int i = 0; i < supporting2.size(); i++) {
				supporting_count += getInputWidthFromLayerName(supporting2[i]);
			}
		}
		else {
			//addErrorString(_T("Error 6783413"));
		}
	}
	return(supporting_count);
}

int CAuxFunctions::getConvType(int index)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc->netData.isConvLayer(index) || pDoc->netData.isAttentionLayer(index)) {
		return(pDoc->netData.getConvType(index));
	}
	return(NONE);
}

int CAuxFunctions::getConvType(string id)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConvLayer(i) || pDoc->netData.isAttentionLayer(i)) {
			if (pDoc->netData.getID(i) == id) {
				return(pDoc->netData.getConvType(i));
			}
		}
	}
	return(NONE);
}

CString CAuxFunctions::getTimeString(float dsec)
{
	float hrs = (float)(long)(dsec / 3600);
	float min = (float)(long)((dsec / 60) - (hrs * 60));
	float sec = dsec - (hrs * 3600) - (min * 60);
	CString smin = _T("");
	if ((int)min > 9) {
		smin.Format(_T("%i:%i"), (int)hrs, (int)min);
	}
	else {
		smin.Format(_T("%i:0%i"), (int)hrs, (int)min);
	}
	CString ssec = _T("");
	if ((int)sec > 9) {
		ssec.Format(_T(":%.1f"), sec);
	}
	else {
		ssec.Format(_T(":0%.1f"), sec);
	}
	return(smin + ssec);
}

bool CAuxFunctions::areDataNumbers(vector<CString> sdata)
{
	int cnt = 0;
	int tcnt = 100;
	if (sdata.size() > 0) {
		for (int j = 0; j < sdata.size(); j++) {
			if (!isWordNumber(string(CT2CA(sdata[j])))) {
				return(false);
			}
			cnt++;
			if (cnt > tcnt) {
				return(true);
			}
		}
	}
	return(true);
}

bool CAuxFunctions::areDataNumbers(vector<vector<CString>> sdata)
{
	int cnt = 0;
	int tcnt = 100;
	if (sdata.size() > 0) {
		for (int i = 0; i < sdata.size(); i++) {
			if (sdata[i].size() > 0) {
				for (int j = 0; j < sdata[i].size(); j++) {
					if (!isWordNumber(string(CT2CA(sdata[i][j])))) {
						return(false);
					}
					cnt++;
					if (cnt > tcnt) {
						return(true);
					}
				}
			}
		}
	}
	else {
		return(false);
	}
	return(true);
}

/*bool CAuxFunctions::isStringFilename(CString string)
{
	if (string.GetLength() > 7) {
		CString lowercase = string.MakeLower();
		for (int i = 0; i < (lowercase.GetLength() - 6); i++) {
			CString ext = getFileExtension2(lowercase).MakeLower();
			if (ext == _T("jpg")) {
				return(true);
			}
		}
	}
	return(false);
}

void CAuxFunctions::saveLines(CString fname, vector<CString> lines)
{
	CFileDialog dlg2(FALSE, _T("csv"), fname, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("CSV Files (*.csv)|*.csv||"));

	if (dlg2.DoModal() == IDOK) {

		CFile file;

		CString myfilename = dlg2.GetPathName();

		// Open csv file
		if (!file.Open(myfilename, CFile::modeCreate | CFile::modeReadWrite)) {
			CString msg = _T("Unable to open ") + myfilename;
			AfxMessageBox(msg);
			return;
		}

		// Allocate memory used to parse lines of data
		unsigned long max_data = 50000;
		char* cdata = new char[max_data];
		if (cdata == 0) {
			AfxMessageBox(_T("Unable to allocate memory to save net file"));
			return;
		}

		for (int i = 0; i < lines.size(); i++) {

			if (lines[i].GetLength() < int(max_data)) {
				for (int j = 0; j < lines[i].GetLength(); j++) {
					cdata[j] = (char)lines[i].GetAt(j);
				}
			}
			file.Write(cdata, lines[i].GetLength());

		}

		file.Flush();
		file.Close();

		delete[] cdata;

	}

}*/

bool CAuxFunctions::checkWindowsVersion()
{
	if (!IsWindows8OrGreater()) {
		AfxMessageBox(_T("Windows Version Not Supported"));
		return(false);
	}
	return(true);
}

bool CAuxFunctions::checkCountry()
{
	vector<CString> acceptible{ _T("US"), _T("AU"), _T("AL"), _T("BE"), _T("BG"), _T("CA"), _T("HR"), _T("CZ"), _T("DK"), _T("EE"), 
		_T("FI"), _T("FR"), _T("DE"), _T("GR"), _T("HU"), _T("IS"), _T("IT"), _T("LV"), _T("LT"), _T("LU"), _T("ME"), _T("NL"), 
		_T("MK"), _T("NO"), _T("PL"), _T("PT"), _T("RO"), _T("SK"), _T("SI"), _T("ES"), _T("GB"), _T("UM"), _T("JP"), _T("KR") };

	int sizeOfBuffer = 10;
	WCHAR* buffer = new WCHAR[sizeOfBuffer];
	if (GetUserDefaultGeoName(buffer, sizeOfBuffer) > 0) {
		CString s = buffer;
		for (int i = 0; i < acceptible.size(); i++) {
			if (buffer == acceptible[i]) {
				return(true);
			}
		}
		CString s3 = _T("");
		for (int i = 0; i < s.GetLength(); i++) {
			CString s2 = _T("");
			s2.Format(_T("%i"), buffer[i]);
			s3 += s2;
		}
		AfxMessageBox(_T("Error code 51") + s3 + _T(", startup verification failed"));
	}
	else {
		AfxMessageBox(_T("Error code 52, startup verification failed"));
	}
	return(false);

	/*
	Afghanistan,AF,AFG,004,ISO 3166-2:AF
	Åland Islands,AX,ALA,248,ISO 3166-2:AX
	Albania,AL,ALB,008,ISO 3166-2:AL
	Algeria,DZ,DZA,012,ISO 3166-2:DZ
	American Samoa,AS,ASM,016,ISO 3166-2:AS
	Andorra,AD,AND,020,ISO 3166-2:AD
	Angola,AO,AGO,024,ISO 3166-2:AO
	Anguilla,AI,AIA,660,ISO 3166-2:AI
	Antarctica,AQ,ATA,010,ISO 3166-2:AQ
	Antigua and Barbuda,AG,ATG,028,ISO 3166-2:AG
	Argentina,AR,ARG,032,ISO 3166-2:AR
	Armenia,AM,ARM,051,ISO 3166-2:AM
	Aruba,AW,ABW,533,ISO 3166-2:AW
	Australia,AU,AUS,036,ISO 3166-2:AU
	Austria,AT,AUT,040,ISO 3166-2:AT
	Azerbaijan,AZ,AZE,031,ISO 3166-2:AZ
	Bahamas,BS,BHS,044,ISO 3166-2:BS
	Bahrain,BH,BHR,048,ISO 3166-2:BH
	Bangladesh,BD,BGD,050,ISO 3166-2:BD
	Barbados,BB,BRB,052,ISO 3166-2:BB
	Belarus,BY,BLR,112,ISO 3166-2:BY
	Belgium,BE,BEL,056,ISO 3166-2:BE
	Belize,BZ,BLZ,084,ISO 3166-2:BZ
	Benin,BJ,BEN,204,ISO 3166-2:BJ
	Bermuda,BM,BMU,060,ISO 3166-2:BM
	Bhutan,BT,BTN,064,ISO 3166-2:BT
	Bolivia,BO,BOL,068,ISO 3166-2:BO
	Bosnia and Herzegovina,BA,BIH,070,ISO 3166-2:BA
	Botswana,BW,BWA,072,ISO 3166-2:BW
	Bouvet Island,BV,BVT,074,ISO 3166-2:BV
	Brazil,BR,BRA,076,ISO 3166-2:BR
	British Indian Ocean Territory,IO,IOT,086,ISO 3166-2:IO
	Brunei Darussalam,BN,BRN,096,ISO 3166-2:BN
	Bulgaria,BG,BGR,100,ISO 3166-2:BG
	Burkina Faso,BF,BFA,854,ISO 3166-2:BF
	Burundi,BI,BDI,108,ISO 3166-2:BI
	Cambodia,KH,KHM,116,ISO 3166-2:KH
	Cameroon,CM,CMR,120,ISO 3166-2:CM
	Canada,CA,CAN,124,ISO 3166-2:CA
	Cape Verde,CV,CPV,132,ISO 3166-2:CV
	Cayman Islands,KY,CYM,136,ISO 3166-2:KY
	Central African Republic,CF,CAF,140,ISO 3166-2:CF
	Chad,TD,TCD,148,ISO 3166-2:TD
	Chile,CL,CHL,152,ISO 3166-2:CL
	China,CN,CHN,156,ISO 3166-2:CN
	Christmas Island,CX,CXR,162,ISO 3166-2:CX
	Cocos (Keeling) Islands,CC,CCK,166,ISO 3166-2:CC
	Colombia,CO,COL,170,ISO 3166-2:CO
	Comoros,KM,COM,174,ISO 3166-2:KM
	Congo,CG,COG,178,ISO 3166-2:CG
	"Congo, the Democratic Republic of the",CD,COD,180,ISO 3166-2:CD
	Cook Islands,CK,COK,184,ISO 3166-2:CK
	Costa Rica,CR,CRI,188,ISO 3166-2:CR
	Côte d'Ivoire,CI,CIV,384,ISO 3166-2:CI
	Croatia,HR,HRV,191,ISO 3166-2:HR
	Cuba,CU,CUB,192,ISO 3166-2:CU
	Cyprus,CY,CYP,196,ISO 3166-2:CY
	Czech Republic,CZ,CZE,203,ISO 3166-2:CZ
	Denmark,DK,DNK,208,ISO 3166-2:DK
	Djibouti,DJ,DJI,262,ISO 3166-2:DJ
	Dominica,DM,DMA,212,ISO 3166-2:DM
	Dominican Republic,DO,DOM,214,ISO 3166-2:DO
	Ecuador,EC,ECU,218,ISO 3166-2:EC
	Egypt,EG,EGY,818,ISO 3166-2:EG
	El Salvador,SV,SLV,222,ISO 3166-2:SV
	Equatorial Guinea,GQ,GNQ,226,ISO 3166-2:GQ
	Eritrea,ER,ERI,232,ISO 3166-2:ER
	Estonia,EE,EST,233,ISO 3166-2:EE
	Ethiopia,ET,ETH,231,ISO 3166-2:ET
	Falkland Islands (Malvinas),FK,FLK,238,ISO 3166-2:FK
	Faroe Islands,FO,FRO,234,ISO 3166-2:FO
	Fiji,FJ,FJI,242,ISO 3166-2:FJ
	Finland,FI,FIN,246,ISO 3166-2:FI
	France,FR,FRA,250,ISO 3166-2:FR
	French Guiana,GF,GUF,254,ISO 3166-2:GF
	French Polynesia,PF,PYF,258,ISO 3166-2:PF
	French Southern Territories,TF,ATF,260,ISO 3166-2:TF
	Gabon,GA,GAB,266,ISO 3166-2:GA
	Gambia,GM,GMB,270,ISO 3166-2:GM
	Georgia,GE,GEO,268,ISO 3166-2:GE
	Germany,DE,DEU,276,ISO 3166-2:DE
	Ghana,GH,GHA,288,ISO 3166-2:GH
	Gibraltar,GI,GIB,292,ISO 3166-2:GI
	Greece,GR,GRC,300,ISO 3166-2:GR
	Greenland,GL,GRL,304,ISO 3166-2:GL
	Grenada,GD,GRD,308,ISO 3166-2:GD
	Guadeloupe,GP,GLP,312,ISO 3166-2:GP
	Guam,GU,GUM,316,ISO 3166-2:GU
	Guatemala,GT,GTM,320,ISO 3166-2:GT
	Guernsey,GG,GGY,831,ISO 3166-2:GG
	Guinea,GN,GIN,324,ISO 3166-2:GN
	Guinea-Bissau,GW,GNB,624,ISO 3166-2:GW
	Guyana,GY,GUY,328,ISO 3166-2:GY
	Haiti,HT,HTI,332,ISO 3166-2:HT
	Heard Island and McDonald Islands,HM,HMD,334,ISO 3166-2:HM
	Holy See (Vatican City State),VA,VAT,336,ISO 3166-2:VA
	Honduras,HN,HND,340,ISO 3166-2:HN
	Hong Kong,HK,HKG,344,ISO 3166-2:HK
	Hungary,HU,HUN,348,ISO 3166-2:HU
	Iceland,IS,ISL,352,ISO 3166-2:IS
	India,IN,IND,356,ISO 3166-2:IN
	Indonesia,ID,IDN,360,ISO 3166-2:ID
	Iran,IR,IRN,364,ISO 3166-2:IR
	Iraq,IQ,IRQ,368,ISO 3166-2:IQ
	Ireland,IE,IRL,372,ISO 3166-2:IE
	Isle of Man,IM,IMN,833,ISO 3166-2:IM
	Israel,IL,ISR,376,ISO 3166-2:IL
	Italy,IT,ITA,380,ISO 3166-2:IT
	Jamaica,JM,JAM,388,ISO 3166-2:JM
	Japan,JP,JPN,392,ISO 3166-2:JP
	Jersey,JE,JEY,832,ISO 3166-2:JE
	Jordan,JO,JOR,400,ISO 3166-2:JO
	Kazakhstan,KZ,KAZ,398,ISO 3166-2:KZ
	Kenya,KE,KEN,404,ISO 3166-2:KE
	Kiribati,KI,KIR,296,ISO 3166-2:KI
	"Korea, Democratic People's Republic of",KP,PRK,408,ISO 3166-2:KP
	"Korea, Republic of (South Korea)",KR,KOR,410,ISO 3166-2:KR
	Kuwait,KW,KWT,414,ISO 3166-2:KW
	Kyrgyzstan,KG,KGZ,417,ISO 3166-2:KG
	Lao People's Democratic Republic,LA,LAO,418,ISO 3166-2:LA
	Latvia,LV,LVA,428,ISO 3166-2:LV
	Lebanon,LB,LBN,422,ISO 3166-2:LB
	Lesotho,LS,LSO,426,ISO 3166-2:LS
	Liberia,LR,LBR,430,ISO 3166-2:LR
	Libyan Arab Jamahiriya,LY,LBY,434,ISO 3166-2:LY
	Liechtenstein,LI,LIE,438,ISO 3166-2:LI
	Lithuania,LT,LTU,440,ISO 3166-2:LT
	Luxembourg,LU,LUX,442,ISO 3166-2:LU
	Macao,MO,MAC,446,ISO 3166-2:MO
	"Macedonia, the former Yugoslav Republic of",MK,MKD,807,ISO 3166-2:MK
	Madagascar,MG,MDG,450,ISO 3166-2:MG
	Malawi,MW,MWI,454,ISO 3166-2:MW
	Malaysia,MY,MYS,458,ISO 3166-2:MY
	Maldives,MV,MDV,462,ISO 3166-2:MV
	Mali,ML,MLI,466,ISO 3166-2:ML
	Malta,MT,MLT,470,ISO 3166-2:MT
	Marshall Islands,MH,MHL,584,ISO 3166-2:MH
	Martinique,MQ,MTQ,474,ISO 3166-2:MQ
	Mauritania,MR,MRT,478,ISO 3166-2:MR
	Mauritius,MU,MUS,480,ISO 3166-2:MU
	Mayotte,YT,MYT,175,ISO 3166-2:YT
	Mexico,MX,MEX,484,ISO 3166-2:MX
	"Micronesia, Federated States of",FM,FSM,583,ISO 3166-2:FM
	"Moldova, Republic of",MD,MDA,498,ISO 3166-2:MD
	Monaco,MC,MCO,492,ISO 3166-2:MC
	Mongolia,MN,MNG,496,ISO 3166-2:MN
	Montenegro,ME,MNE,499,ISO 3166-2:ME
	Montserrat,MS,MSR,500,ISO 3166-2:MS
	Morocco,MA,MAR,504,ISO 3166-2:MA
	Mozambique,MZ,MOZ,508,ISO 3166-2:MZ
	Myanmar,MM,MMR,104,ISO 3166-2:MM
	Namibia,NA,NAM,516,ISO 3166-2:NA
	Nauru,NR,NRU,520,ISO 3166-2:NR
	Nepal,NP,NPL,524,ISO 3166-2:NP
	Netherlands,NL,NLD,528,ISO 3166-2:NL
	Netherlands Antilles,AN,ANT,530,ISO 3166-2:AN
	New Caledonia,NC,NCL,540,ISO 3166-2:NC
	New Zealand,NZ,NZL,554,ISO 3166-2:NZ
	Nicaragua,NI,NIC,558,ISO 3166-2:NI
	Niger,NE,NER,562,ISO 3166-2:NE
	Nigeria,NG,NGA,566,ISO 3166-2:NG
	Niue,NU,NIU,570,ISO 3166-2:NU
	Norfolk Island,NF,NFK,574,ISO 3166-2:NF
	Northern Mariana Islands,MP,MNP,580,ISO 3166-2:MP
	Norway,NO,NOR,578,ISO 3166-2:NO
	Oman,OM,OMN,512,ISO 3166-2:OM
	Pakistan,PK,PAK,586,ISO 3166-2:PK
	Palau,PW,PLW,585,ISO 3166-2:PW
	"Palestinian Territory, Occupied",PS,PSE,275,ISO 3166-2:PS
	Panama,PA,PAN,591,ISO 3166-2:PA
	Papua New Guinea,PG,PNG,598,ISO 3166-2:PG
	Paraguay,PY,PRY,600,ISO 3166-2:PY
	Peru,PE,PER,604,ISO 3166-2:PE
	Philippines,PH,PHL,608,ISO 3166-2:PH
	Pitcairn,PN,PCN,612,ISO 3166-2:PN
	Poland,PL,POL,616,ISO 3166-2:PL
	Portugal,PT,PRT,620,ISO 3166-2:PT
	Puerto Rico,PR,PRI,630,ISO 3166-2:PR
	Qatar,QA,QAT,634,ISO 3166-2:QA
	Reunion,RE,REU,638,ISO 3166-2:RE
	Romania,RO,ROU,642,ISO 3166-2:RO
	Russia,RU,RUS,643,ISO 3166-2:RU
	Rwanda,RW,RWA,646,ISO 3166-2:RW
	Saint Barthélemy,BL,BLM,652,ISO 3166-2:BL
	"Saint Helena, Ascension and Tristan da Cunha",SH,SHN,654,ISO 3166-2:SH
	Saint Kitts and Nevis,KN,KNA,659,ISO 3166-2:KN
	Saint Lucia,LC,LCA,662,ISO 3166-2:LC
	Saint Martin,MF,MAF,663,ISO 3166-2:MF
	Saint Pierre and Miquelon,PM,SPM,666,ISO 3166-2:PM
	Saint Vincent and the Grenadines,VC,VCT,670,ISO 3166-2:VC
	Samoa,WS,WSM,882,ISO 3166-2:WS
	San Marino,SM,SMR,674,ISO 3166-2:SM
	Sao Tome and Principe,ST,STP,678,ISO 3166-2:ST
	Saudi Arabia,SA,SAU,682,ISO 3166-2:SA
	Senegal,SN,SEN,686,ISO 3166-2:SN
	Serbia,RS,SRB,688,ISO 3166-2:RS
	Seychelles,SC,SYC,690,ISO 3166-2:SC
	Sierra Leone,SL,SLE,694,ISO 3166-2:SL
	Singapore,SG,SGP,702,ISO 3166-2:SG
	Slovakia,SK,SVK,703,ISO 3166-2:SK
	Slovenia,SI,SVN,705,ISO 3166-2:SI
	Solomon Islands,SB,SLB,090,ISO 3166-2:SB
	Somalia,SO,SOM,706,ISO 3166-2:SO
	South Africa,ZA,ZAF,710,ISO 3166-2:ZA
	South Georgia and the South Sandwich Islands,GS,SGS,239,ISO 3166-2:GS
	Spain,ES,ESP,724,ISO 3166-2:ES
	Sri Lanka,LK,LKA,144,ISO 3166-2:LK
	Sudan,SD,SDN,736,ISO 3166-2:SD
	Suriname,SR,SUR,740,ISO 3166-2:SR
	Svalbard and Jan Mayen,SJ,SJM,744,ISO 3166-2:SJ
	Swaziland,SZ,SWZ,748,ISO 3166-2:SZ
	Sweden,SE,SWE,752,ISO 3166-2:SE
	Switzerland,CH,CHE,756,ISO 3166-2:CH
	Syrian Arab Republic,SY,SYR,760,ISO 3166-2:SY
	Taiwan,TW,TWN,158,ISO 3166-2:TW
	Tajikistan,TJ,TJK,762,ISO 3166-2:TJ
	"Tanzania, United Republic of",TZ,TZA,834,ISO 3166-2:TZ
	Thailand,TH,THA,764,ISO 3166-2:TH
	Timor-Leste,TL,TLS,626,ISO 3166-2:TL
	Togo,TG,TGO,768,ISO 3166-2:TG
	Tokelau,TK,TKL,772,ISO 3166-2:TK
	Tonga,TO,TON,776,ISO 3166-2:TO
	Trinidad and Tobago,TT,TTO,780,ISO 3166-2:TT
	Tunisia,TN,TUN,788,ISO 3166-2:TN
	Turkey,TR,TUR,792,ISO 3166-2:TR
	Turkmenistan,TM,TKM,795,ISO 3166-2:TM
	Turks and Caicos,TC,TCA,796,ISO 3166-2:TC
	Tuvalu,TV,TUV,798,ISO 3166-2:TV
	Uganda,UG,UGA,800,ISO 3166-2:UG
	Ukraine,UA,UKR,804,ISO 3166-2:UA
	United Arab Emirates,AE,ARE,784,ISO 3166-2:AE
	United Kingdom,GB,GBR,826,ISO 3166-2:GB
	United States Of America,US,USA,840,ISO 3166-2:US
	United States Minor Outlying Islands,UM,UMI,581,ISO 3166-2:UM
	Uruguay,UY,URY,858,ISO 3166-2:UY
	Uzbekistan,UZ,UZB,860,ISO 3166-2:UZ
	Vanuatu,VU,VUT,548,ISO 3166-2:VU
	Venezuela,VE,VEN,862,ISO 3166-2:VE
	Vietnam,VN,VNM,704,ISO 3166-2:VN
	"Virgin Islands, British",VG,VGB,092,ISO 3166-2:VG
	"Virgin Islands, U.S.",VI,VIR,850,ISO 3166-2:VI
	Wallis and Futuna,WF,WLF,876,ISO 3166-2:WF
	Western Sahara,EH,ESH,732,ISO 3166-2:EH
	Yemen,YE,YEM,887,ISO 3166-2:YE
	Zambia,ZM,ZMB,894,ISO 3166-2:ZM
	Zimbabwe,ZW,ZWE,716,ISO 3166-2:ZW
	*/
}

int CAuxFunctions::checkForwardDirection()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	//NetData ndata = pDoc->netData;
	
	int vectorx = 0;
	int vectory = 0;

	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			CPoint pt1 = pDoc->getInputNodePoint(i);
			CPoint pt2 = pDoc->getOutputNodePoint(i);
			vectorx += (pt2.x - pt1.x);
			vectory += (pt2.y - pt1.y);
		}
	}

	int abs_vectorx = vectorx;
	if (abs_vectorx < 0) {
		abs_vectorx = -1 * abs_vectorx;
	}
	int abs_vectory = vectory;
	if (abs_vectory < 0) {
		abs_vectory = -1 * abs_vectory;
	}

	if (abs_vectorx > abs_vectory) {
		if (vectorx > 0) {
			addErrorString(_T("Forward propagation check - right"));
			return(RIGHT);
		}
		else {
			addErrorString(_T("Forward propagation check - left"));
			return(LEFT);
		}
	}
	else if (abs_vectory > abs_vectorx) {
		if (vectory < 0) {
			addErrorString(_T("Forward propagation check - up"));
			return(UP);
		}
		else {
			addErrorString(_T("Forward propagation check - down"));
			return(DOWN);
		}
	}

	addErrorString(_T("Forward propagation check - unable to determine"));
	return(UNKNOWN);

}

int CAuxFunctions::getIndexFromID(string mid)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	return(pDoc->netData.getIndexFromID(mid));
}

bool CAuxFunctions::isInputConnectedToOutputNode(CString id)
{
	//addErrorString(_T("isInputConnectedToOutputNode : ") + id);
	if (id == _T("Output:")) {
		return(true);
	}
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int index = pDoc->netData.getIndexFromID(string(CT2CA(id)));
	if (pDoc->netData.getNodeMode(index) == "PreProcess") {
		//addErrorString(_T("isInputConnectedToOutputNode : PreProcess : ") + id);
		vector<string> snodes = getImmediateSupportingNodes(index);
		if (snodes.size() == 1) {
			//addErrorString(_T("isInputConnectedToOutputNode : PreProcess with one supporting node : ") + id);
			index = pDoc->netData.getIndexFromID(snodes[0]);
			if (pDoc->netData.getNodeMode(index) == "Input") {
				//addErrorString(_T("isInputConnectedToOutputNode : PreProcess with one supporting input node : ") + id);
				string dfield = pDoc->netData.getDataField(index);
				//addErrorString(_T("isInputConnectedToOutputNode : PreProcess with one supporting input node : ") + id + _T(" with data field ") + dfield);
				if (dfield.substr(0, 7) == "Output:") {
					return(true);
				}
			}
		}
	}
	else if (pDoc->netData.getNodeMode(index) == "Input") {
		string dfield = pDoc->netData.getDataField(index);
		//addErrorString(_T("isInputConnectedToOutputNode : Input : ") + id + _T(" with data field ") + dfield);
		if (dfield.substr(0, 7) == "Output:") {
			return(true);
		}
	}
	return(false);
}

string CAuxFunctions::getIDFromNetData(int index)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	return(pDoc->netData.getID(index));
}

vector<string> CAuxFunctions::getNetInputNodes()
{
	vector<string> rval;

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	// Look for all nodes and convnets in net data
	if (pDoc->netData.size() > 0) {
		for (int i = 0; i < (int)pDoc->netData.size(); i++) {
			if (pDoc->netData.isNode(i)) {
				//addErrorString("getNetInputNodes : tp1 : " + pDoc->netData.getID(i));
				if (pDoc->netData.getNodeMode(i) == "Input") {
					bool add = true;
					vector<string> fnodes = pDoc->netData.getImmediateFollowingNodes(i);
					if (fnodes.size() == 1) {
						if (pDoc->netData.getNodeMode(pDoc->netData.getIndexFromID(fnodes[0])) == "PreProcess") {
							add = false;
						}
					}
					if (add) {
						string nname = pDoc->netData.getID(i);
						if (rval.size() > 0) {
							bool found = false;
							for (int j = 0; j < (int)rval.size(); j++) {
								if (nname == rval[j]) {
									j = (int)rval.size();
									found = true;
								}
							}
							if (!found) {
								rval.push_back(nname);
							}
						}
						else {
							rval.push_back(nname);
						}
					}

				}
				else if (pDoc->netData.getNodeMode(i) == "PreProcess") {
					vector<string> snodes = getImmediateSupportingNodes(i);
					if (snodes.size() == 1) {
						string nname = pDoc->netData.getID(i);
						if (rval.size() > 0) {
							bool found = false;
							for (int j = 0; j < (int)rval.size(); j++) {
								if (nname == rval[j]) {
									j = (int)rval.size();
									found = true;
								}
							}
							if (!found) {
								rval.push_back(nname);
							}
						}
						else {
							rval.push_back(nname);
						}
					}
				}
			}
		}
	}

	return(rval);
}

string CAuxFunctions::getInputNodeName(string node)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int index = pDoc->netData.getIndexFromID(node);
	if (index >= 0) {
		if (pDoc->netData.isNode(index)) {
			if (pDoc->netData.getNodeMode(index) == "Input") {
				return(node);
			}
			else if (pDoc->netData.getNodeMode(index) == "PreProcess") {
				vector<string> snodes = getImmediateSupportingNodes(node);
				if (snodes.size() == 1) {
					int index2 = pDoc->netData.getIndexFromID(snodes[0]);
					if (index2 >= 0) {
						if (pDoc->netData.isNode(index2)) {
							if (pDoc->netData.getNodeMode(index2) == "Input") {
								return(snodes[0]);
							}
						}
					}
				}
			}
		}
	}
	return("");
}

string CAuxFunctions::getInputFieldFromNodeName(string cname)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int index = pDoc->netData.getIndexFromID(cname);
	if (index >= 0) {
		if (pDoc->netData.isNode(index)) {
			if (pDoc->netData.getNodeMode(index) == "Input") {
				return(pDoc->netData.getDataField(index));
			}
		}
	}
	return("");
}











