// COptions.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "COptions.h"
#include "afxdialogex.h"


int pcurrentTab = 0;

// COptions dialog

IMPLEMENT_DYNAMIC(COptions, CDialog)

COptions::COptions(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SETTINGS, pParent)
{
	ready = false;
	currentTab = pcurrentTab;
}

COptions::~COptions()
{
}

void COptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab1_control);
}


BEGIN_MESSAGE_MAP(COptions, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &COptions::OnTcnSelchangeTab1)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// COptions message handlers

BOOL COptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	m_tab1_control.InsertItem(0, _T(" Default Values "));
	m_tab1_control.InsertItem(1, _T(" Activation Functions "));
	m_tab1_control.InsertItem(2, _T(" Copyright "));

	m_tab1_control.SetCurSel(currentTab);

	if (currentTab == 0) {
		defaultOptionsDlg.Create(IDD_DEFAULT_OPTIONS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
	}
	else if (currentTab == 1) {
		activationFunctionsDlg.Create(IDD_ACTIVATION, (CTabCtrl*)GetDlgItem(IDC_TAB1));
	}
	else if (currentTab == 2) {
		copyrightOptionsDlg.Create(IDD_COPYRIGHT_OPTIONS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
	}
	else {
		AfxMessageBox(_T("Unsupported Tab"));
	}

	ready = true;

	setSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void COptions::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (currentTab == m_tab1_control.GetCurSel()) {
		return;
	}

	if (currentTab == 0) {
		defaultOptionsDlg.DestroyWindow();
	}
	else if (currentTab == 1) {
		activationFunctionsDlg.DestroyWindow();
	}
	else if (currentTab == 2) {
		copyrightOptionsDlg.DestroyWindow();
	}

	if (m_tab1_control.GetCurSel() == 0) {
		defaultOptionsDlg.Create(IDD_DEFAULT_OPTIONS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		currentTab = 0;
	}
	else if (m_tab1_control.GetCurSel() == 1) {
		activationFunctionsDlg.Create(IDD_ACTIVATION, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		currentTab = 1;
	}
	else if (m_tab1_control.GetCurSel() == 2) {
		copyrightOptionsDlg.Create(IDD_COPYRIGHT_OPTIONS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		currentTab = 2;
	}
	else {
		AfxMessageBox(_T("Unsupported Tab"));
	}

	setSize();

	*pResult = 0;
}

HBRUSH COptions::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//return mbrush;

	HBRUSH hbr = mbrush;

	if (pWnd->GetDlgCtrlID() == IDC_TAB1) {
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	else {

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
			pDC->SetBkMode(TRANSPARENT);
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

	}

	return hbr;

}

void COptions::OnOK()
{
	if (currentTab == 0) {
		defaultOptionsDlg.saveOptions();
	}
	pcurrentTab = currentTab;
	CDialog::OnOK();
}

void COptions::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	setSize();
}

void COptions::setSize()
{
	if (ready) {
		CRect rect1;
		m_tab1_control.GetWindowRect(&rect1);
		CRect rect2;
		rect2.left = 2;
		rect2.top = 22;
		rect2.right = (rect1.right - rect1.left) - 2;
		rect2.bottom = (rect1.bottom - rect1.top) - 2;
		if (currentTab == 0) {
			defaultOptionsDlg.setSize(rect2);
		}
		else if (currentTab == 1) {
			activationFunctionsDlg.setSize(rect2);
		}
		else if (currentTab == 2) {
			copyrightOptionsDlg.setSize(rect2);
		}
	}
}

