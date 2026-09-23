// CFileTypeSelectDlg.cpp : implementation file
//

#include "pch.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CFileTypeSelectDlg.h"
#include "afxdialogex.h"


// CFileTypeSelectDlg dialog

IMPLEMENT_DYNAMIC(CFileTypeSelectDlg, CDialogEx)

CFileTypeSelectDlg::CFileTypeSelectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILE_TYPE, pParent)
{
	type = _T("");
	m_list_value = _T("");
}

CFileTypeSelectDlg::~CFileTypeSelectDlg()
{
}

void CFileTypeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
	DDX_LBString(pDX, IDC_LIST1, m_list_value);
}


BEGIN_MESSAGE_MAP(CFileTypeSelectDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CFileTypeSelectDlg message handlers


BOOL CFileTypeSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	CString s = _T("");
	for (int i = 0; i < (int)mfile_types.size(); i++) {
		if (mfile_types[i].second > 0) {
			s.Format(_T(" : num files = %i"), mfile_types[i].second);
			m_list_ctrl.AddString(mfile_types[i].first + s);
		}
	}
	m_list_ctrl.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileTypeSelectDlg::OnOK()
{
	UpdateData();
	type = m_list_value;
	CDialogEx::OnOK();
}

HBRUSH CFileTypeSelectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

CString CFileTypeSelectDlg::getSelectedType()
{
	return(type);
}

void CFileTypeSelectDlg::setFiles(vector<pair<CString, int>> file_types)
{
	mfile_types = file_types;
}


