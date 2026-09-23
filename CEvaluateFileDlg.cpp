// CEvaluateFileDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesignerDoc.h"
#include "MainFrm.h"
#include "NetDesignerView.h"
#include "NetDesigner.h"
#include "CEvaluateFileDlg.h"
#include "AuxFunctions.h"
#include "afxdialogex.h"


// CEvaluateFileDlg dialog

bool already4 = false;
CRect lastRect4(0, 0, 0, 0);

IMPLEMENT_DYNAMIC(CEvaluateFileDlg, CDialog)

CEvaluateFileDlg::CEvaluateFileDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_EVALUATE_FILE, pParent)
{
	ncols = 0;
	filename = _T("");
}

CEvaluateFileDlg::~CEvaluateFileDlg()
{
}

void CEvaluateFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_control);
	DDX_Control(pDX, IDC_SAVE_CSV, m_save_control);
	DDX_Control(pDX, IDC_LOAD_DATA, m_file_ctrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
}


BEGIN_MESSAGE_MAP(CEvaluateFileDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_LOAD_DATA, &CEvaluateFileDlg::OnBnClickedLoadData)
	ON_BN_CLICKED(IDC_SAVE_CSV, &CEvaluateFileDlg::OnBnClickedSaveCsv)
END_MESSAGE_MAP()


// CEvaluateFileDlg message handlers


BOOL CEvaluateFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_brush.CreateSolidBrush(GetGlobalData()->clrBarFace);

	adjustSize();

	if (already4) {
		MoveWindow(lastRect4);
	}

	m_list_control.SetExtendedStyle(m_list_control.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_progress_ctrl.ShowWindow(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEvaluateFileDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	adjustSize();
}

HBRUSH CEvaluateFileDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return m_brush;
}

void CEvaluateFileDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(400, 300);
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CEvaluateFileDlg::OnBnClickedLoadData()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CFileDialog dlg2(true, _T("csv"), filename, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, _T("Comma Separated (*.csv)|*.csv||"));

	if (dlg2.DoModal() == IDOK) {

		filename = dlg2.GetPathName();

		if (filename != _T("")) {

			CAuxFunctions func;

			CString ext = func.getFileExtension(filename).MakeLower();

			if (ext = _T("csv")) {

				headers.clear();
				data.clear();

				outputData.clear();
				outputLabels.clear();

				headers = func.getHeadersFromDataSource(filename);

				inputIndices.clear();
				for (int i = 0; i < pDoc->nnet.num_inputs; i++) {
					bool found = false;
					CString input = (CString)pDoc->nnet.netInputs[i].nodeId.c_str();
					for (int j = 0; j < (int)headers.size(); j++) {
						if (doesInputMatch(input, headers[j])) {
							inputIndices.push_back(j);
							found = true;
							j = (int)headers.size();
						}
					}
					if (!found) {
						pDoc->addErrorString(_T("Evaluate CSV File : Unable to find input ") + input + _T(" in ") + filename);
						return;
					}
				}

				for (int i = 0; i < pDoc->nnet.num_outputs; i++) {
					CString output = _T("netOutput_") + getOutputString((CString)pDoc->nnet.onodes[i].id.c_str());
					outputLabels.push_back(output);
				}

				vector<CString> fnames;
				fnames.push_back(filename);

				double* inputs = new double[pDoc->nnet.num_inputs];

				if (inputs != 0) {
					if (((int)data.size() > 0) && ((int)headers.size() > 0)) {
						for (int i = 0; i < (int)data.size(); i++) {
							int num_rows = (int)data[i].size() / (int)headers.size();
							for (int j = 0; j < num_rows; j++) {
								int offset = j * (int)headers.size();
								for (int k = 0; k < pDoc->nnet.num_inputs; k++) {
									double tdata = (double)_tstof((LPCTSTR)data[i][offset + inputIndices[k]]);
									inputs[k] = pDoc->processData((CString)pDoc->nnet.netInputs[k].nodeId.c_str(), tdata);
								}
								pDoc->nnet.forward(inputs, false);
								vector<CString> vdata;
								for (int k = 0; k < pDoc->nnet.num_outputs; k++) {
									double odata = pDoc->reProcessData((CString)pDoc->nnet.onodes[k].id.c_str(), pDoc->nnet.getOutput(k));
									CString s = _T("");
									s.Format(_T("%.15e"), odata);
									vdata.push_back(s);
								}
								outputData.push_back(vdata);
							}
						}
					}
					delete[] inputs;
					fillHeaders();
					fillData();
				}

			}
			else {
				pDoc->addErrorString(_T("Evaluate CSV File : Need csv file to evaluate"));
			}

		}

	}

}

bool CEvaluateFileDlg::doesInputMatch(CString mname, CString tname)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (mname == tname) {
		return(true);
	}
	int nindex = pDoc->netData.getIndexFromID(string(CT2CA(mname)));
	if (pDoc->netData.getNodeMode(nindex) == "PreProcess") {
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConnect(i)) {
				if (pDoc->netData.getOutputNode(i) == string(CT2CA(mname))) {
					CString input_node_id = (CString)pDoc->netData.getInputNode(i).c_str();
					if (input_node_id == tname) {
						return(true);
					}
					int nindex2 = pDoc->netData.getIndexFromID(string(CT2CA(input_node_id)));
					string input_node_field = pDoc->netData.getDataField(nindex2);
					if (input_node_field == string(CT2CA(tname))) {
						return(true);
					}
				}
			}
		}
	}
	else {
		string fname = pDoc->netData.getDataField(nindex);
		if (fname == string(CT2CA(tname))) {
			return(true);
		}
	}
	return(false);
}

CString CEvaluateFileDlg::getOutputString(CString nname)
{
	CString rval = _T("");
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	int nindex = pDoc->netData.getIndexFromID(string(CT2CA(nname)));
	rval = (CString)pDoc->netData.getDataField(nindex).c_str();
	if (rval == _T("")) {
		rval = (CString)pDoc->netData.getID(nindex).c_str();
	}
	return(rval);
}

void CEvaluateFileDlg::OnBnClickedSaveCsv()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	if ((headers.size() > 0) && (data.size() > 0) && (outputData.size() > 0)) {
		int num_rows = (int)data[0].size() / (int)headers.size();
		if (num_rows == outputData.size()) {
			if ((ncols - 1) == (int)(headers.size() + outputData[0].size())) {

				CAuxFunctions func;

				CString myfilename = func.removeExtensionFromFilename(filename) + _T("_WithNetOutput.csv");

				CFileDialog dlg2(FALSE, _T("csv"), myfilename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Comma Separated (*.csv)|*.csv||"));

				if (dlg2.DoModal() == IDOK) {

					myfilename = dlg2.GetPathName();

					CFile file;

					// Open csv file
					if (!file.Open(myfilename, CFile::modeCreate | CFile::modeReadWrite)) {
						CString msg = _T("OnBnClickedSaveCsv : Unable to open ") + myfilename;
						AfxMessageBox(msg);
						return;
					}

					// Allocate memory used to parse lines of data
					unsigned long max_data = 50000;
					char* cdata = new char[max_data];
					if (cdata == 0) {
						AfxMessageBox(_T("OnBnClickedSaveCsv : Unable to allocate memory to save csv file"));
						return;
					}

					CString sline = _T("");

					for (int i = 0; i < headers.size(); i++) {
						sline += headers[i];
						sline += _T(",");
					}
					for (int i = 0; i < outputLabels.size(); i++) {
						sline += outputLabels[i];
						if (i < (outputLabels.size() - 1)) {
							sline += _T(",");
						}
						else {
							sline += _T("\n");
						}
					}
					if (sline.GetLength() < int(max_data)) {
						for (int j = 0; j < sline.GetLength(); j++) {
							cdata[j] = (char)sline.GetAt(j);
						}
					}
					file.Write(cdata, sline.GetLength());

					CString s = _T("");
					for (int i = 0; i < num_rows; i++) {
						sline = _T("");
						int offset = i * (int)headers.size();
						for (int j = 0; j < (int)headers.size(); j++) {
							double tdata = (double)_tstof((LPCTSTR)data[0][offset + j]);
							s.Format(_T("%.6f"), tdata);
							sline += s;
							sline += _T(",");
						}
						for (int j = 0; j < outputData[i].size(); j++) {
							double tdata = (double)_tstof((LPCTSTR)outputData[i][j]);
							s.Format(_T("%.6f"), tdata);
							sline += s;
							if (j < (outputData[i].size() - 1)) {
								sline += _T(",");
							}
							else {
								sline += _T("\n");
							}
						}
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

					AfxMessageBox(_T("Evaluate finished saving CSV"));

				}

			}
		}
	}
	else {
		pDoc->addErrorString(_T("Evaluate CSV File : Unable to save data"));
	}
}

void CEvaluateFileDlg::adjustSize()
{
	int top = 8;
	int left = 11;
	int space = 12;

	CRect rect1;
	CRect rect2;
	CRect rect3;
	CRect rect4;
	CRect rect5;

	GetWindowRect(&rect1);

	m_file_ctrl.GetWindowRect(&rect2);
	m_save_control.GetWindowRect(&rect3);
	m_list_control.GetWindowRect(&rect4);
	m_progress_ctrl.GetWindowRect(&rect5);

	m_file_ctrl.MoveWindow(left, top, rect2.Width(), rect2.Height(), true);
	m_file_ctrl.GetWindowRect(&rect2);
	m_save_control.MoveWindow(rect1.Width() - (2*left + rect3.Width() + 8), top, rect3.Width(), rect3.Height(), true);
	m_save_control.GetWindowRect(&rect3);
	int pwidth = rect1.Width() - (rect2.Width() + 2*left + 2*space + rect3.Width() + 19);
	m_progress_ctrl.MoveWindow(left + rect2.Width() + space, top + 2, pwidth, rect5.Height());
	m_list_control.MoveWindow(left - 2, top + rect2.Height() + space - 4, rect3.right - rect2.left + 4, rect1.Height() - (top + rect2.Height() + space + 55), true);
}

void CEvaluateFileDlg::fillHeaders()
{
	m_list_control.DeleteAllItems();
	for (int i = 0; i < ncols; i++) {
		m_list_control.DeleteColumn(0);
	}
	ncols = 0;
	m_list_control.InsertColumn(1, _T(""), LVCFMT_CENTER, 40);
	ncols++;
	for (int i = 0; i < headers.size(); i++) {
		int width = 20 + (7 * headers[i].GetLength());
		if (width < 100) {
			width = 100;
		}
		m_list_control.InsertColumn(ncols + 1, headers[i], LVCFMT_CENTER, width);
		ncols++;
	}
	for (int i = 0; i < outputLabels.size(); i++) {
		int width = 20 + (7 * outputLabels[i].GetLength());
		if (width < 80) {
			width = 80;
		}
		m_list_control.InsertColumn(ncols + 1, outputLabels[i], LVCFMT_CENTER, width);
		ncols++;
	}
}

void CEvaluateFileDlg::fillData()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if ((headers.size() > 0) && (data.size() > 0) && (outputData.size() > 0)) {
		int num_rows = (int)data[0].size() / (int)headers.size();
		if (num_rows == outputData.size()) {
			if ((ncols - 1) == (int)(headers.size() + outputData[0].size())) {
				CString s = _T("");
				for (int i = 0; i < num_rows; i++) {
					s.Format(_T("%i"), i + 1);
					int nIndex = m_list_control.InsertItem(i + 1, s);
					int offset = i * (int)headers.size();
					for (int j = 0; j < (int)headers.size(); j++) {
						double tdata = (double)_tstof((LPCTSTR)data[0][offset + j]);
						s.Format(_T("%.6f"), tdata);
						m_list_control.SetItemText(nIndex, j + 1, s);
					}
					for (int j = 0; j < outputData[i].size(); j++) {
						double tdata = (double)_tstof((LPCTSTR)outputData[i][j]);
						s.Format(_T("%.6f"), tdata);
						m_list_control.SetItemText(nIndex, j + (int)headers.size() + 1, s);
					}
				}
			}
			else {
				CString s = _T("");
				s.Format(_T("Evaluate CSV File : Problem with data sizes (3) : num cols = %i : headers size = %i : output data col size = %i"), ncols, (int)headers.size(), (int)outputData[0].size());
				pDoc->addErrorString(s);
			}
		}
		else {
			CString s = _T("");
			s.Format(_T("Evaluate CSV File : Problem with data sizes (2) : num rows = %i : output data size = %i"), num_rows, (int)outputData.size());
			pDoc->addErrorString(s);
		}
	}
	else {
		CString s = _T("");
		s.Format(_T("Evaluate CSV File : Problem with data sizes (1) : headers size = %i : data size = %i : output data size = %i"), (int)headers.size(), (int)data.size(), (int)outputData.size());
		pDoc->addErrorString(s);
	}
}

void CEvaluateFileDlg::OnCancel()
{
	GetWindowRect(lastRect4);
	already4 = true;
	MoveWindow(lastRect4);
	CDialog::OnCancel();
}
