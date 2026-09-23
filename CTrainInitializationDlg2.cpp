// CTrainInitializationDlg2.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CTrainInitializationDlg2.h"
#include "afxdialogex.h"


// CTrainInitializationDlg2 dialog

IMPLEMENT_DYNAMIC(CTrainInitializationDlg2, CDialog)

CTrainInitializationDlg2::CTrainInitializationDlg2(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INITIALIZATION2, pParent)
{

}

CTrainInitializationDlg2::~CTrainInitializationDlg2()
{
}

void CTrainInitializationDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTrainInitializationDlg2, CDialog)
	ON_BN_CLICKED(IDC_SAVED_WEIGHTS, &CTrainInitializationDlg2::OnBnClickedSavedWeights)
	ON_BN_CLICKED(IDC_RANDOMIZE, &CTrainInitializationDlg2::OnBnClickedRandomize)
END_MESSAGE_MAP()


// CTrainInitializationDlg2 message handlers


void CTrainInitializationDlg2::OnBnClickedSavedWeights()
{
	saved_weights = true;
	randomize = false;
	OnOK();
}

void CTrainInitializationDlg2::OnBnClickedRandomize()
{
	saved_weights = false;
	randomize = true;
	OnOK();
}
