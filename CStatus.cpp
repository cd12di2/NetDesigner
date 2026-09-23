// CStatus.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CStatus.h"
#include "afxdialogex.h"


// CStatus dialog

IMPLEMENT_DYNAMIC(CStatus, CDialog)

CStatus::CStatus(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_STATUS, pParent)
{
	m_label = _T("Working . . .");
	displayed = false;
	cancel = false;
	cancellable = false;
}

CStatus::~CStatus()
{
}

void CStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
	DDX_Text(pDX, IDC_LABEL, m_label);
	DDX_Control(pDX, IDC_STATUS_CANCEL, m_cancel_ctrl);
}


BEGIN_MESSAGE_MAP(CStatus, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_STATUS_CANCEL, &CStatus::OnBnClickedStatusCancel)
END_MESSAGE_MAP()


// CStatus message handlers

BOOL CStatus::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_progress_ctrl.SetRange(0, 100);

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	if (!cancellable) {
		m_cancel_ctrl.ShowWindow(false);
		int top = 33;
		int space = 15;
		CRect rect;
		CRect rectClient;
		CRect rectProgress;
		GetWindowRect(&rect);
		GetClientRect(&rectClient);
		m_progress_ctrl.GetWindowRect(&rectProgress);
		top = rectProgress.top - rect.top;
		space = rectProgress.left - rect.left;
		m_progress_ctrl.MoveWindow(rectClient.left + space, rectClient.top + top, rectClient.Width() - (2 * space), rectProgress.Height(), true);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CStatus::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CStatus::CreateDlg()
{
	Create(IDD_STATUS);
	displayed = true;
	cancel = false;
}

void CStatus::DestroyDlg()
{
	if (displayed) {
		DestroyWindow();
		displayed = false;
		cancel = false;
	}
}
/*
void CStatus::updateProgress(int percent)
{
	if (displayed) {
		m_progress_ctrl.SetPos(percent);
		//m_progress_ctrl.UpdateWindow();
		//UpdateData(false);
	}
}
*/
void CStatus::setProgress(int percent)
{
	m_progress_ctrl.SetPos(percent);
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CStatus::setLabel(string label)
{
	m_label = (CString)label.c_str();
	UpdateData(false);
}

void CStatus::setLabel(CString label)
{
	m_label = label;
	UpdateData(false);
}

void CStatus::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

void CStatus::OnBnClickedStatusCancel()
{
	cancel = true;
}
