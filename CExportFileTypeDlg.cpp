// CExportFileTypeDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CExportFileTypeDlg.h"
#include "afxdialogex.h"


// CExportFileTypeDlg dialog

int my_c_file = 0;

IMPLEMENT_DYNAMIC(CExportFileTypeDlg, CDialog)

CExportFileTypeDlg::CExportFileTypeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_EXPORT, pParent)
{
	m_c_file = my_c_file;
}

CExportFileTypeDlg::~CExportFileTypeDlg()
{
}

void CExportFileTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Radio(pDX, IDC_RADIO3, m_text_file);
	//DDX_Radio(pDX, IDC_RADIO2, m_swift_file);
	DDX_Radio(pDX, IDC_RADIO1, m_c_file);
}


BEGIN_MESSAGE_MAP(CExportFileTypeDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO1, &CExportFileTypeDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CExportFileTypeDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CExportFileTypeDlg::OnBnClickedRadio3)
END_MESSAGE_MAP()


// CExportFileTypeDlg message handlers

BOOL CExportFileTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	OnBnClickedRadio1();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportFileTypeDlg::OnOK()
{
	UpdateData();
	/*if (m_c_file == 0) {
		AfxMessageBox(_T("c code"));
	}
	else if (m_c_file == 1) {
		AfxMessageBox(_T("swift code"));
	}
	else if (m_c_file == 2) {
		AfxMessageBox(_T("text file"));
	}*/
	my_c_file = m_c_file;
	CDialog::OnOK();
}

HBRUSH CExportFileTypeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
		hbr = mbrush2;
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

void CExportFileTypeDlg::OnBnClickedRadio1()
{
}

void CExportFileTypeDlg::OnBnClickedRadio2()
{
}

void CExportFileTypeDlg::OnBnClickedRadio3()
{
}
