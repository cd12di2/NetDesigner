// CGraphOptions.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CGraphOptions.h"
#include "afxdialogex.h"


// CGraphOptions dialog

IMPLEMENT_DYNAMIC(CGraphOptions, CDialog)

CGraphOptions::CGraphOptions(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_GRAPH_OPTIONS, pParent)
{
	default_ymin = _T("0.0");
	default_ymax = _T("0.5");
	m_ymin = default_ymin;
	m_ymax = default_ymax;
	yminauto = true;
	ymaxauto = true;
}

CGraphOptions::~CGraphOptions()
{
}

void CGraphOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_YMIN, m_ymin);
	DDX_Text(pDX, IDC_YMAX, m_ymax);
	DDX_Control(pDX, IDC_YMIN_BUTTON, m_ymin_button_ctrl);
	DDX_Control(pDX, IDC_YMAX_BUTTON, m_ymax_button_ctrl);
}


BEGIN_MESSAGE_MAP(CGraphOptions, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_YMIN_BUTTON, &CGraphOptions::OnBnClickedYminButton)
	ON_BN_CLICKED(IDC_YMAX_BUTTON, &CGraphOptions::OnBnClickedYmaxButton)
	ON_EN_CHANGE(IDC_YMIN, &CGraphOptions::OnEnChangeYmin)
	ON_EN_CHANGE(IDC_YMAX, &CGraphOptions::OnEnChangeYmax)
END_MESSAGE_MAP()


// CGraphOptions message handlers

BOOL CGraphOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CGraphOptions::OnOK()
{
	UpdateData(false);
	CDialog::OnOK();
}

HBRUSH CGraphOptions::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	hbr = mbrush;

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

void CGraphOptions::setButtons()
{
	if (yminauto) {
		m_ymin_button_ctrl.SetWindowText(_T("Auto"));
	}
	else {
		m_ymin_button_ctrl.SetWindowText(_T("Reset"));
	}
	if (ymaxauto) {
		m_ymax_button_ctrl.SetWindowText(_T("Auto"));
	}
	else {
		m_ymax_button_ctrl.SetWindowText(_T("Reset"));
	}
}

void CGraphOptions::OnBnClickedYminButton()
{
	yminauto = true;
	m_ymin = default_ymin;
	setButtons();
	UpdateData(false);
}

void CGraphOptions::OnBnClickedYmaxButton()
{
	ymaxauto = true;
	m_ymax = default_ymax;
	setButtons();
	UpdateData(false);
}

void CGraphOptions::OnEnChangeYmin()
{
	UpdateData();
	ymaxauto = false;
	setButtons();
}

void CGraphOptions::OnEnChangeYmax()
{
	UpdateData();
	yminauto = false;
	setButtons();
}
