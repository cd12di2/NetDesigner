// CGuideSettings.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CGuideSettings.h"
#include "afxdialogex.h"


// CGuideSettings dialog

IMPLEMENT_DYNAMIC(CGuideSettings, CDialog)

CGuideSettings::CGuideSettings(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GUIDESETTINGS, pParent)
{
	m_spacing = _T("");
	m_spacing2 = _T("");
}

CGuideSettings::~CGuideSettings()
{
}

void CGuideSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPACING, m_spacing_control);
	DDX_CBString(pDX, IDC_SPACING, m_spacing);
	DDX_Control(pDX, IDC_SPACING2, m_spacing2_control);
	DDX_CBString(pDX, IDC_SPACING2, m_spacing2);
}


BEGIN_MESSAGE_MAP(CGuideSettings, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CGuideSettings message handlers


BOOL CGuideSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	m_spacing_control.AddString(_T("None"));
	m_spacing_control.AddString(_T("4 ndus"));
	m_spacing_control.AddString(_T("8 ndus"));
	//m_spacing_control.AddString(_T("16 ndus"));
	m_spacing_control.SetCurSel(m_spacing_control.FindString(0, m_spacing));

	m_spacing2_control.AddString(_T("None"));
	m_spacing2_control.AddString(_T("16 ndus"));
	m_spacing2_control.AddString(_T("32 ndus"));
	//m_spacing2_control.AddString(_T("64 ndus"));
	m_spacing2_control.SetCurSel(m_spacing2_control.FindString(0, m_spacing2));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CGuideSettings::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

HBRUSH CGuideSettings::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

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
