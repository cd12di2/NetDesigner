// DefaultOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "DefaultOptionsDlg.h"
#include "afxdialogex.h"


// DefaultOptionsDlg dialog

IMPLEMENT_DYNAMIC(DefaultOptionsDlg, CDialog)

DefaultOptionsDlg::DefaultOptionsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DEFAULT_OPTIONS, pParent)
{
	m_activation_function == _T("Sigmoid");
	m_eta = _T("0.00200000");
	m_alpha = _T("0.800000");
	m_rand = _T("0.020000");
	m_filter = _T("10000");
	m_sleakage = _T("0.010000");
	m_rleakage = _T("0.001000");
	m_minimum_sound_level = _T("20");
	m_pprocessing = _T("100");
	m_normalize_fileset = false;
	m_normalize_files = true;
	m_cartoon = true;
}

DefaultOptionsDlg::~DefaultOptionsDlg()
{
}

void DefaultOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_activation_function_control);
	DDX_CBString(pDX, IDC_COMBO1, m_activation_function);
	DDX_Text(pDX, IDC_ETA, m_eta);
	DDX_Text(pDX, IDC_ALPHA, m_alpha);
	DDX_Text(pDX, IDC_RAND, m_rand);
	DDX_Text(pDX, IDC_SLEAK, m_sleakage);
	DDX_Text(pDX, IDC_RLEAK, m_rleakage);
	DDX_Text(pDX, IDC_FILTER, m_filter);
	DDX_Text(pDX, IDC_MIN_SOUND_LEVEL, m_minimum_sound_level);
	DDX_Radio(pDX, IDC_NORMALIZE_FILE_SET, m_normalize_fileset);
	DDX_Radio(pDX, IDC_NORMALIZE_PER_FILE, m_normalize_files);
	DDX_Text(pDX, IDC_PPROCESSING, m_pprocessing);
	DDX_Radio(pDX, IDC_CARTOON, m_cartoon);
}


BEGIN_MESSAGE_MAP(DefaultOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_CLEAR, &DefaultOptionsDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_RESET, &DefaultOptionsDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_SAVE_OPTIONS, &DefaultOptionsDlg::OnBnClickedSaveOptions)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_NORMALIZE_FILE_SET, &DefaultOptionsDlg::OnBnClickedNormalizeFileSet)
	ON_BN_CLICKED(IDC_NORMALIZE_PER_FILE, &DefaultOptionsDlg::OnBnClickedNormalizePerFile)
	ON_BN_CLICKED(IDC_CARTOON, &DefaultOptionsDlg::OnBnClickedCartoon)
END_MESSAGE_MAP()


// DefaultOptionsDlg message handlers


void DefaultOptionsDlg::OnBnClickedClear()
{
	CWinApp* pApp = AfxGetApp();
	pApp->Unregister();
}


void DefaultOptionsDlg::OnBnClickedReset()
{
	m_activation_function == _T("Sigmoid");
	m_activation_function_control.GetLBText(0, m_activation_function);
	m_eta = _T("0.00200000");
	m_alpha = _T("0.800000");
	m_rand = _T("0.200000");
	m_filter = _T("10000");
	m_sleakage = _T("0.010000");
	m_rleakage = _T("0.001000");
	m_minimum_sound_level = _T("20");
	m_pprocessing = _T("100");
	m_cartoon = true;
	UpdateData(false);
}

void DefaultOptionsDlg::OnBnClickedSaveOptions()
{
	UpdateData();

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	pDoc->dfuncn = m_activation_function;
	pDoc->deta = (double)_tstof((LPCTSTR)m_eta);
	pDoc->dalpha = (double)_tstof((LPCTSTR)m_alpha);
	pDoc->drand = (double)_tstof((LPCTSTR)m_rand);
	pDoc->sigmoid_leakage = (double)_tstof((LPCTSTR)m_sleakage);
	pDoc->relu_leakage = (double)_tstof((LPCTSTR)m_rleakage);
	pDoc->errorfilter = (double)_tstof((LPCTSTR)m_filter);
	pDoc->minimum_sound_level = (int)_tstof((LPCTSTR)m_minimum_sound_level);
	pDoc->parallel_processing_test = (int)_tstof((LPCTSTR)m_pprocessing);
	pDoc->cartoon = !m_cartoon;
	if (pDoc->errorfilter < 1) {
		pDoc->errorfilter = 1;
	}

	CWinApp* pApp = AfxGetApp();

	pApp->WriteProfileString(_T("NetDefaults"), _T("Function"), pDoc->dfuncn);
	pApp->WriteProfileInt(_T("NetDefaults"), _T("Eta"), (int)(pDoc->deta * 1e8));
	pApp->WriteProfileInt(_T("NetDefaults"), _T("Alpha"), (int)(pDoc->dalpha * 1e6));
	pApp->WriteProfileInt(_T("NetDefaults"), _T("Rand"), (int)(pDoc->drand * 1e6));
	pApp->WriteProfileInt(_T("NetDefaults"), _T("SLeakage"), (int)(pDoc->sigmoid_leakage * 1e6));
	pApp->WriteProfileInt(_T("NetDefaults"), _T("RLeakage"), (int)(pDoc->relu_leakage * 1e6));
	pApp->WriteProfileInt(_T("NetDefaults"), _T("ErrorFilter"), (int)pDoc->errorfilter);
	pApp->WriteProfileInt(_T("NetDefaults"), _T("ParallelProcessing"), (int)pDoc->parallel_processing_test);
	pApp->WriteProfileInt(_T("NetDefaults"), _T("Cartoon"), (int)pDoc->cartoon);

	AfxMessageBox(_T("Net train values have been saved"));

}

void DefaultOptionsDlg::saveOptions()
{
	UpdateData();

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	pDoc->dfuncn = m_activation_function;
	pDoc->deta = (double)_tstof((LPCTSTR)m_eta);
	pDoc->dalpha = (double)_tstof((LPCTSTR)m_alpha);
	pDoc->drand = (double)_tstof((LPCTSTR)m_rand);
	pDoc->sigmoid_leakage = (double)_tstof((LPCTSTR)m_sleakage);
	pDoc->relu_leakage = (double)_tstof((LPCTSTR)m_rleakage);
	pDoc->errorfilter = (double)_tstof((LPCTSTR)m_filter);
	pDoc->minimum_sound_level = (int)_tstof((LPCTSTR)m_minimum_sound_level);
	pDoc->parallel_processing_test = (int)_tstof((LPCTSTR)m_pprocessing);
	pDoc->cartoon = !m_cartoon;
	if (pDoc->errorfilter < 1) {
		pDoc->errorfilter = 1;
	}

}

BOOL DefaultOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	m_activation_function_control.AddString(_T("Sigmoid"));
	m_activation_function_control.AddString(_T("Relu"));
	m_activation_function_control.AddString(_T("Tanh"));
	//m_activation_function_control.AddString(_T("MaxPool"));

	CWinApp* pApp = AfxGetApp();

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	m_activation_function = pDoc->dfuncn;// dfuncn;
	m_activation_function_control.GetLBText(0, m_activation_function);
	m_eta.Format(_T("%.8lf"), pDoc->deta);// tdeta);
	m_alpha.Format(_T("%.6lf"), pDoc->dalpha);// tdalpha);
	m_rand.Format(_T("%.6lf"), pDoc->drand);// tdrand);
	m_sleakage.Format(_T("%.6lf"), pDoc->sigmoid_leakage);// sleak);
	m_rleakage.Format(_T("%.6lf"), pDoc->relu_leakage);// rleak);
	if (pDoc->errorfilter < 1) {
		pDoc->errorfilter = 1;
	}
	m_filter.Format(_T("%.0lf"), pDoc->errorfilter);// errorfilter);
	m_minimum_sound_level.Format(_T("%i"), pDoc->minimum_sound_level);
	m_pprocessing.Format(_T("%i"), pDoc->parallel_processing_test);
	m_cartoon = !pDoc->cartoon;

	if (pDoc->normaiize_file_set) {
		m_normalize_fileset = false;
		m_normalize_files = true;
	}
	else {
		m_normalize_fileset = true;
		m_normalize_files = false;
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void DefaultOptionsDlg::OnOK()
{
	//CDialog::OnOK();
}

HBRUSH DefaultOptionsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

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
		hbr = mbrush2;
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
		//pDC->SetBkMode(TRANSPARENT);
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

	//return hbr;
}

void DefaultOptionsDlg::setSize(CRect rect)
{
	MoveWindow(rect, true);
}

void DefaultOptionsDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void DefaultOptionsDlg::OnBnClickedNormalizeFileSet()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->normaiize_file_set = true;
	m_normalize_fileset = false;
	m_normalize_files = true;
	UpdateData();
}

void DefaultOptionsDlg::OnBnClickedNormalizePerFile()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->normaiize_file_set = false;
	m_normalize_fileset = true;
	m_normalize_files = false;
	UpdateData();
}

void DefaultOptionsDlg::OnBnClickedCartoon()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	//UpdateData();
	if (m_cartoon) {
		m_cartoon = false;
		pDoc->cartoon = true;
	}
	else {
		m_cartoon = true;
		pDoc->cartoon = false;
	}
	UpdateData(false);
}
