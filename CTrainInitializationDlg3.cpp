// CTrainInitializationDlg3.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CTrainInitializationDlg3.h"
#include "afxdialogex.h"


// CTrainInitializationDlg3 dialog

IMPLEMENT_DYNAMIC(CTrainInitializationDlg3, CDialog)

CTrainInitializationDlg3::CTrainInitializationDlg3(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INITIALIZATION3, pParent)
{
	continue_weights = false;
	randomize = false;
}

CTrainInitializationDlg3::~CTrainInitializationDlg3()
{
}

void CTrainInitializationDlg3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTrainInitializationDlg3, CDialog)
	ON_BN_CLICKED(IDC_CONTINUE, &CTrainInitializationDlg3::OnBnClickedContinue)
	ON_BN_CLICKED(IDC_RANDOMIZE, &CTrainInitializationDlg3::OnBnClickedRandomize)
END_MESSAGE_MAP()


// CTrainInitializationDlg3 message handlers

void CTrainInitializationDlg3::OnBnClickedContinue()
{
	continue_weights = true;
	randomize = false;
	OnOK();
}

void CTrainInitializationDlg3::OnBnClickedRandomize()
{
	continue_weights = false;
	randomize = true;
	OnOK();
}
