// CDataSetDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CDataSetDlg.h"
#include "afxdialogex.h"


// CDataSetDlg dialog

IMPLEMENT_DYNAMIC(CDataSetDlg, CDialog)

CDataSetDlg::CDataSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DATA_SETS, pParent)
{
	m_percent = _T("10");
	m_no_validation = 0;
	m_combo = _T("");
}

CDataSetDlg::~CDataSetDlg()
{
}

void CDataSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_percent_control);
	DDX_Text(pDX, IDC_EDIT1, m_percent);
	DDX_Radio(pDX, IDC_NO_VALIDATION, m_no_validation);
	DDX_Control(pDX, IDC_COMBO1, m_combo_ctrl);
	DDX_CBString(pDX, IDC_COMBO1, m_combo);
	DDX_Control(pDX, IDC_SELECT_FILE2, m_select_control);
}


BEGIN_MESSAGE_MAP(CDataSetDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FILE_OPEN, &CDataSetDlg::OnBnClickedFileOpen)
	ON_BN_CLICKED(IDC_NO_VALIDATION, &CDataSetDlg::OnBnClickedNoValidation)
	ON_BN_CLICKED(IDC_RANDOMIZED, &CDataSetDlg::OnBnClickedRandomized)
	ON_BN_CLICKED(IDC_FROM_FILE, &CDataSetDlg::OnBnClickedFromFile)
	ON_BN_CLICKED(IDC_SELECT_FILE2, &CDataSetDlg::OnBnClickedSelectFile2)
END_MESSAGE_MAP()


// CDataSetDlg message handlers

BOOL CDataSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	if (m_no_validation == 1) {
		OnBnClickedRandomized();
	}
	else if (m_no_validation == 2) {
		OnBnClickedFromFile();
	}
	else {
		OnBnClickedNoValidation();
	}

	setFilesnamesToList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataSetDlg::OnOK()
{
	if (m_no_validation > 0) {
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		vector<CString> paths = pDoc->getDataPathNames();
		if (!pDoc->checkDataSource() || (paths.size() == 0)) {
			m_no_validation = 0;
			UpdateData(false);
			AfxMessageBox(_T("Net must have data source to select validation data"));
			return;
		}
		if (m_no_validation > 1) {
			UpdateData();
			pDoc->clearValidationPathNames();
		}
	}
	CDialog::OnOK();
}

HBRUSH CDataSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = mbrush;
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_BTN:
		pDC->SetTextColor(RGB(255, 0, 0));
		break;
	case CTLCOLOR_DLG:
		pDC->SetTextColor(RGB(255, 255, 0));
		break;
	case CTLCOLOR_EDIT:
		hbr = mbrush2;
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_LISTBOX:
		//pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
		break;
	case CTLCOLOR_MSGBOX:
		pDC->SetTextColor(RGB(255, 0, 255));
		break;
	case CTLCOLOR_SCROLLBAR:
		pDC->SetTextColor(RGB(0, 0, 255));
		break;
	default:
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
	}
	return hbr;
}

void CDataSetDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages


}

void CDataSetDlg::OnBnClickedFileOpen()
{
	UpdateData();
}

void CDataSetDlg::OnBnClickedNoValidation()
{
	UpdateData();
	m_percent_control.EnableWindow(false);
	m_combo_ctrl.EnableWindow(false);
	m_select_control.EnableWindow(false);
	UpdateData(true);
}

void CDataSetDlg::OnBnClickedRandomized()
{
	UpdateData();
	m_percent_control.EnableWindow(true);
	m_combo_ctrl.EnableWindow(false);
	m_select_control.EnableWindow(false);
	UpdateData(true);
}

void CDataSetDlg::OnBnClickedFromFile()
{
	UpdateData();
	m_percent_control.EnableWindow(false);
	m_combo_ctrl.EnableWindow(true);
	m_select_control.EnableWindow(true);
	UpdateData(true);
}

void CDataSetDlg::OnBnClickedSelectFile2()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	CAuxFunctions func;
	vector<CString> mpaths;
	if (pDoc->select_folder) {
		CString mfolder = _T("");
		vector<CString> mpnames = pDoc->getDataPathNames();
		if (mpnames.size() > 0) {
			mfolder = mpnames[0];
		}
		mfolder = func.getDataFolder(mfolder);
		if (mfolder != _T("")) {
			mpaths = func.getDirectoryAndSubdirectoryFiles(mfolder);
			if (mpaths.size() > 0) {
				mpaths = func.getSelectFileType(mpaths, mfolder);
			}
		}
	}
	else {
		mpaths = pDoc->getValidationPathNames();
		int ftype = pDoc->getDataPathType(pDoc->getDataPathNames());
		if (mpaths.size() == 0) {
			mpaths = func.getDataFileName(_T(""), ftype);
		}
		else {
			mpaths = func.getDataFileName(pDoc->getFirstDataPathName(), ftype);
		}
	}
	if (mpaths.size() > 0) {
		int ftype = pDoc->getDataPathType(pDoc->getDataPathNames());
		data_type = pDoc->getDataPathType(mpaths);
		if (data_type == ftype) {
			if (data_type == FILE_TYPE_CSV) {
				if (mpaths[0] != _T("")) {
					UpdateData();
					mfilenames = mpaths;
					mfilenames = func.checkHeadersFromDataSources(mfilenames);
				}
			}
			else if (data_type == FILE_TYPE_CSV_WITH_JPG) {
				if (mpaths[0] != _T("")) {
					UpdateData();
					mfilenames = mpaths;
					mfilenames = func.checkHeadersFromDataSources(mfilenames);
				}
			}
			else if (data_type == FILE_TYPE_JPG) {
				UpdateData();
				imageCheckData idata = func.checkJpgSources(mpaths);
				mfilenames = idata.paths;
			}
			else if (data_type == FILE_TYPE_RAW) {
				UpdateData();
				if (func.checkRawSources(mpaths)) {
					mfilenames = mpaths;
				}
			}
			else if (data_type == FILE_TYPE_WAV) {
				UpdateData();
				mfilenames = mpaths;
				mfilenames = func.checkWavSources(mfilenames);
			}
			else if (data_type == FILE_TYPE_TXT) {
				UpdateData();
				mfilenames = mpaths;
			}
			setFilesnamesToList();
		}
		else {
			addErrorString(_T("Data and Validation file types must match"));
		}
	}
}

void CDataSetDlg::setFilesnamesToList()
{
	m_combo = _T("");
	m_combo_ctrl.ResetContent();
	if (mfilenames.size() > 0) {
		for (int i = 0; i < mfilenames.size(); i++) {
			m_combo_ctrl.AddString(mfilenames[i]);
		}
		m_combo = mfilenames[0];
	}
	UpdateData(false);
}

void CDataSetDlg::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

