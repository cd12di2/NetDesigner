// CAutomationDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CAutomationDlg.h"
#include "afxdialogex.h"
//#include "AuxFunctions.h"


// CAutomationDlg dialog

bool already7 = false;
CRect lastRect7(0, 0, 0, 0);


IMPLEMENT_DYNAMIC(CAutomationDlg, CDialog)

CAutomationDlg::CAutomationDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_AUTOMATION_DLG, pParent)
{
	ready = false;
	displayed = false;
	changed = false;
	inputUp = false;
	currentTab = 0;
}

CAutomationDlg::~CAutomationDlg()
{
}

void CAutomationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab1_control);
	DDX_Control(pDX, IDOK, m_ok_ctrl);
}


BEGIN_MESSAGE_MAP(CAutomationDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAutomationDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CAutomationDlg message handlers


BOOL CAutomationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initializeTabs();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	adjustSize();

	m_tab1_control.SetCurSel(currentTab);

	setCurrentTab();

	changed = true;
	
	if (already7) {
		MoveWindow(lastRect7);
	}

	m_tab1_control.EnableWindow(true);
	m_tab1_control.ShowWindow(SW_SHOW);

	ready = true;

	setSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutomationDlg::CreateDlg()
{
	if (!displayed) {
		Create(IDD_AUTOMATION_DLG);
		m_tab1_control.EnableWindow(true);
		m_tab1_control.ShowWindow(SW_SHOW);
		displayed = true;
	}
}

void CAutomationDlg::DestroyDlg()
{
	DestroyWindow();
	displayed = false;
}

BOOL CAutomationDlg::DestroyWindow()
{
	displayed = false;
	return CDialog::DestroyWindow();
}

void CAutomationDlg::OnCancel()
{
	DestroyDlg();
}

void CAutomationDlg::initializeTabs()
{
	m_tab1_control.InsertItem(0, _T(" Input Optimization "));
}

void CAutomationDlg::OnOK()
{
	GetWindowRect(lastRect7);
	already7 = true;
	displayed = false;
	DestroyDlg();
}

HBRUSH CAutomationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

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
}

void CAutomationDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(500, 380);
}

void CAutomationDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	adjustSize();
	setSize();
}

void CAutomationDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages
}

void CAutomationDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	setTab(m_tab1_control.GetCurSel());
	setDataToDialogs();
	setSize();
	*pResult = 0;
}

void CAutomationDlg::adjustSize()
{
	CRect rect1;
	CRect rect2;
	GetWindowRect(&rect1);
	m_ok_ctrl.GetWindowRect(&rect2);
	m_ok_ctrl.MoveWindow(rect1.Width() - 80, rect1.Height() - 83, rect2.Width(), rect2.Height(), true);
	m_tab1_control.MoveWindow(9, 12, rect1.Width() - 37, rect1.Height() - 103, true);
}

void CAutomationDlg::setSize()
{
	if (ready) {
		CRect rect1;
		m_tab1_control.GetWindowRect(&rect1);
		CRect rect2;
		rect2.left = 2;
		rect2.top = 22 + 4;
		rect2.right = (rect1.right - rect1.left) - 2;
		rect2.bottom = (rect1.bottom - rect1.top) - 2 + 4;
		inputDlg.setSize(rect2);
	}
}

void CAutomationDlg::setDataToDialogs()
{
}

void CAutomationDlg::setCurrentTab()
{
	inputUp = false;

	if (currentTab == 0) {
		inputDlg.Create(IDD_INPUT_OPTIMIZATION, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		inputUp = true;
	}
	else if (currentTab == 1) {
	}
	else if (currentTab == 2) {
	}
	else if (currentTab == 3) {
	}
	else if (currentTab == 4) {
	}
	else if (currentTab == 5) {
	}
	else {
		AfxMessageBox(_T("Unsupported Tab"));
	}
}

void CAutomationDlg::setTab(int ntab)
{
	if ((currentTab == ntab) && !changed) {
		return;
	}

	if (inputUp) {
		inputDlg.DestroyWindow();
		inputUp = false;
	}

	CRect rect1;
	m_tab1_control.GetWindowRect(&rect1);
	CRect rect2;
	rect2.left = 2;
	rect2.top = 22 + 4;
	rect2.right = (rect1.right - rect1.left) - 2;
	rect2.bottom = (rect1.bottom - rect1.top) - 2 + 4;

	if (ntab == 0) {
		inputDlg.Create(IDD_INPUT_OPTIMIZATION, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		inputDlg.setSize(rect2);
		inputUp = true;
		currentTab = 0;
	}
	else if (ntab == 1) {
		currentTab = 1;
	}
	else if (ntab == 2) {
		currentTab = 2;
	}
	else if (ntab == 3) {
		currentTab = 3;
	}
	else if (ntab == 4) {
		currentTab = 4;
	}
	else if (ntab == 5) {
		currentTab = 5;
	}
	else {
		AfxMessageBox(_T("Unsupported Tab"));
	}

	m_tab1_control.SetCurSel(currentTab);

	changed = false;

}

void CAutomationDlg::addInputOptimizationLine(CString line)
{
	if (inputUp) {
		inputDlg.addLine1(line);
	}
}






void CAutomationDlg::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}







