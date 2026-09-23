// CStatus2.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CStatus2.h"
#include "afxdialogex.h"


// CStatus2 dialog

IMPLEMENT_DYNAMIC(CStatus2, CDialog)

CStatus2::CStatus2(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_STATUS2, pParent)
{
	m_label = _T("Working . . .");
	displayed = false;
}

CStatus2::~CStatus2()
{
}

void CStatus2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LABEL, m_label);
}


BEGIN_MESSAGE_MAP(CStatus2, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CStatus2 message handlers


BOOL CStatus2::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CStatus2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	hbr = mbrush;
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
		pDC->SetTextColor(RGB(0, 255, 0));
		break;
	case CTLCOLOR_MSGBOX:
		pDC->SetTextColor(RGB(255, 0, 255));
		break;
	case CTLCOLOR_SCROLLBAR:
		pDC->SetTextColor(RGB(0, 255, 255));
		break;
	default:
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
	}
	return hbr;
}

void CStatus2::CreateDlg()
{
	Create(IDD_STATUS2);
	displayed = true;
}

void CStatus2::DestroyDlg()
{
	DestroyWindow();
	displayed = false;
}

void CStatus2::setLabel(CString label)
{
	m_label = label;
	if (displayed) {
		UpdateData(false);
	}
}


