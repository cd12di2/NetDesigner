// CTrainInitializationDlg1.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CTrainInitializationDlg1.h"
#include "afxdialogex.h"


// CTrainInitializationDlg1 dialog

IMPLEMENT_DYNAMIC(CTrainInitializationDlg1, CDialog)

CTrainInitializationDlg1::CTrainInitializationDlg1(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INITIALIZATION1, pParent)
{
	continue_weights = false;
	saved_weights = false;
	randomize = false;
}

CTrainInitializationDlg1::~CTrainInitializationDlg1()
{
}

void CTrainInitializationDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTrainInitializationDlg1, CDialog)
	ON_BN_CLICKED(IDC_CONTINUE, &CTrainInitializationDlg1::OnBnClickedContinue)
	ON_BN_CLICKED(IDC_SAVED_WEIGHTS, &CTrainInitializationDlg1::OnBnClickedSavedWeights)
	ON_BN_CLICKED(IDC_RANDOMIZE, &CTrainInitializationDlg1::OnBnClickedRandomize)
END_MESSAGE_MAP()


// CTrainInitializationDlg1 message handlers


void CTrainInitializationDlg1::OnBnClickedContinue()
{
	continue_weights = true;
	saved_weights = false;
	randomize = false;
	OnOK();
}

void CTrainInitializationDlg1::OnBnClickedSavedWeights()
{
	continue_weights = false;
	saved_weights = true;
	randomize = false;
	OnOK();
}

void CTrainInitializationDlg1::OnBnClickedRandomize()
{
	continue_weights = false;
	saved_weights = false;
	randomize = true;
	OnOK();
}
