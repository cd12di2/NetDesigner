// CArchiveWeights.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CArchiveWeights.h"
#include "afxdialogex.h"


// CArchiveWeights dialog

IMPLEMENT_DYNAMIC(CArchiveWeights, CDialog)

CArchiveWeights::CArchiveWeights(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SAVE_WEIGHTS_DLG, pParent)
{
	modified = false;
	m_save_weights = false;
	m_train_response = false;
	m_validation_response = false;
	m_delay_epochs = _T("");
	m_repeat_epochs = _T("");
}

CArchiveWeights::~CArchiveWeights()
{
}

void CArchiveWeights::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SAVE_WEIGHTS, m_save_weights);
	DDX_Check(pDX, IDC_TRAIN_RESPONSE, m_train_response);
	DDX_Check(pDX, IDC_VALIDATION_RESPONSE, m_validation_response);
	DDX_Text(pDX, IDC_SAVE_DELAY_EPOCHS, m_delay_epochs);
	DDX_Text(pDX, IDC_SAVE_DELAY_EPOCHS2, m_repeat_epochs);
}


BEGIN_MESSAGE_MAP(CArchiveWeights, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RESTORE_WEIGHTS, &CArchiveWeights::OnBnClickedRestoreWeights)
END_MESSAGE_MAP()


// CArchiveWeights message handlers


BOOL CArchiveWeights::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	m_save_weights = pDoc->auto_save;
	m_train_response = pDoc->train_response;
	m_validation_response = pDoc->validation_response;
	m_delay_epochs.Format(_T("%i"), pDoc->delay_epochs1);
	m_repeat_epochs.Format(_T("%i"), pDoc->delay_epochs2);

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CArchiveWeights::OnOK()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	UpdateData();

	if ((pDoc->auto_save != (bool)m_save_weights) || (pDoc->train_response != (bool)m_train_response) || 
		(pDoc->validation_response != (bool)m_validation_response) || (pDoc->delay_epochs1 != (int)_tstof((LPCTSTR)m_delay_epochs) || 
		(pDoc->delay_epochs2 != (int)_tstof((LPCTSTR)m_repeat_epochs)))) {
		modified = true;
	}

	pDoc->auto_save = m_save_weights;
	pDoc->train_response = m_train_response;
	pDoc->validation_response = m_validation_response;
	pDoc->delay_epochs1 = (int)_tstof((LPCTSTR)m_delay_epochs);
	pDoc->delay_epochs2 = (int)_tstof((LPCTSTR)m_repeat_epochs);

	CDialog::OnOK();
}


HBRUSH CArchiveWeights::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
		//pDC->SetBkMode(TRANSPARENT);
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

void CArchiveWeights::OnBnClickedRestoreWeights()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->nnet.restoreWeights();
}
