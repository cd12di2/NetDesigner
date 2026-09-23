// CSeedDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CSeedDlg.h"
#include "afxdialogex.h"


// CSeedDlg dialog

IMPLEMENT_DYNAMIC(CSeedDlg, CDialog)

CSeedDlg::CSeedDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_RANDOM_SEED, pParent)
{
	m_seed = _T("");
}

CSeedDlg::~CSeedDlg()
{
}

void CSeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_seed);
}


BEGIN_MESSAGE_MAP(CSeedDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSeedDlg message handlers

BOOL CSeedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CSeedDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
		pDC->SetTextColor(RGB(0, 255, 0));
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
