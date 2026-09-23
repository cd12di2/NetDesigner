// CClusterAnalysisDlg.cpp : implementation file
//

#include "pch.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CClusterAnalysisDlg.h"
#include "afxdialogex.h"


// CClusterAnalysisDlg dialog

bool already5 = false;
CRect lastRect5(0, 0, 0, 0);

IMPLEMENT_DYNAMIC(CClusterAnalysisDlg, CDialog)

CClusterAnalysisDlg::CClusterAnalysisDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CLUSTER_DLG, pParent)
{
	ready = false;
	dataOk = false;
	displayed = false;
	m_nclusters = _T("4");
	m_check = false;
	validGraphBox = false;;
	graphBox = CRect(0,0,0,0);
}

CClusterAnalysisDlg::~CClusterAnalysisDlg()
{
}

void CClusterAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NUM_CLUSTERS, m_nclusters_ctrl);
	DDX_Text(pDX, IDC_NUM_CLUSTERS, m_nclusters);
	DDX_Control(pDX, IDC_CHECK1, m_check_ctrl);
	DDX_Check(pDX, IDC_CHECK1, m_check);
	DDX_Control(pDX, IDC_STATIC1, m_static1_ctrl);
	DDX_Control(pDX, IDC_STATIC2, m_static2_ctrl);
	DDX_Control(pDX, IDC_STATIC3, m_static3_ctrl);
	DDX_Control(pDX, IDC_LIST1, m_list1_ctrl);
	DDX_Control(pDX, IDC_LIST2, m_list2_ctrl);
	DDX_Control(pDX, IDC_BUTTON1, m_button_ctrl);
	DDX_Control(pDX, IDC_SAVE, m_save_ctrl);
}


BEGIN_MESSAGE_MAP(CClusterAnalysisDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, &CClusterAnalysisDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_SAVE, &CClusterAnalysisDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// CClusterAnalysisDlg message handlers


void CClusterAnalysisDlg::CreateDlg()
{
	if (!displayed) {
		Create(IDD_CLUSTER_DLG);
		displayed = true;
	}
}

void CClusterAnalysisDlg::DestroyDlg()
{
	DestroyWindow();
	displayed = false;
}

BOOL CClusterAnalysisDlg::OnInitDialog()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	vector<CString> lbls = pDoc->getClusterInputs();
	if (lbls.size() == 0) {
		addMyErrorString(_T("Insufficient input data for cluster analysis"));
	}
	else {
		dataOk = true;
	}
	for (int i = 0; i < lbls.size(); i++) {
		m_list1_ctrl.AddString(lbls[i]);
	}

	mgraph.xlabel = _T("Cluster Density (N / Standard Deviation)");
	mgraph.xlabel2 = _T("Cluster Iteration");

	validGraphBox = false;;
	graphBox = CRect(0, 0, 0, 0);

	if (already5) {
		MoveWindow(lastRect5);
	}

	ready = true;
	setSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CClusterAnalysisDlg::OnOK()
{
	GetWindowRect(lastRect5);
	already5 = true;
	MoveWindow(lastRect5);
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	m_button_ctrl.SetWindowText(_T("Run"));
	pDoc->stopClustering();
	displayed = false;
	DestroyDlg();
}

void CClusterAnalysisDlg::OnCancel()
{
	GetWindowRect(lastRect5);
	already5 = true;
	MoveWindow(lastRect5);
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	m_button_ctrl.SetWindowText(_T("Run"));
	pDoc->stopClustering();
	displayed = false;
	DestroyWindow();
}

HBRUSH CClusterAnalysisDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CClusterAnalysisDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	setSize();
}

void CClusterAnalysisDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(600, 420);
}

void CClusterAnalysisDlg::setSize()
{
	if (ready) {

		CRect rect;
		GetWindowRect(&rect);

		int t0 = 18;
		int l1 = 21;
		int hd1 = (9 * rect.Width()) / 32;
		int vd1 = (5 * rect.Height()) / 8;
		
		if (hd1 < 179) {
			hd1 = 179;
		}

		/*CRect rect1;
		m_combo1_ctrl.GetWindowRect(&rect1);
		int height1 = rect1.Height();
		rect1.left = l1;
		rect1.right = hd1;
		rect1.top = t0;
		rect1.bottom = rect1.top + height1;
		m_combo1_ctrl.MoveWindow(rect1);*/

		CRect rect2;
		m_button_ctrl.GetWindowRect(&rect2);
		int width2 = rect2.Width();
		int height2 = rect2.Height();
		rect2.top = t0 - 3;
		rect2.right = rect.Width() - 40;
		rect2.left = rect2.right - width2;
		rect2.bottom = rect2.top + height2;
		m_button_ctrl.MoveWindow(rect2);

		CRect rect3;
		m_static1_ctrl.GetWindowRect(&rect3);
		int width3 = rect3.Width();
		int height3 = rect3.Height();
		rect3.left = l1 + 2;
		rect3.right = rect3.left + width3;
		rect3.top = t0 + 3;// rect1.bottom + 14;
		rect3.bottom = rect3.top + height3;
		m_static1_ctrl.MoveWindow(rect3);

		CRect rect4;
		m_nclusters_ctrl.GetWindowRect(&rect4);
		int width4 = rect4.Width();
		int height4 = rect4.Height();
		rect4.left = rect3.right + 11;
		rect4.right = rect4.left + width4;
		rect4.top = rect3.top - 6;
		rect4.bottom = rect4.top + height4;
		m_nclusters_ctrl.MoveWindow(rect4);

		CRect rect5;
		m_check_ctrl.GetWindowRect(&rect5);
		int width5 = rect5.Width();
		int height5 = rect5.Height();
		rect5.left = l1;
		rect5.right = rect5.left + width5;
		rect5.top = rect3.bottom + 12;
		rect5.bottom = rect5.top + height5;
		m_check_ctrl.MoveWindow(rect5);

		CRect rect6;
		m_static3_ctrl.GetWindowRect(&rect6);
		int width6 = rect6.Width();
		int height6 = rect6.Height();
		rect6.left = rect5.right + 4;
		rect6.right = rect6.left + width6;
		rect6.top = rect5.top + 2;
		rect6.bottom = rect6.top + height6;
		m_static3_ctrl.MoveWindow(rect6);

		CRect rect7;
		m_static2_ctrl.GetWindowRect(&rect7);
		int width7 = rect7.Width();
		int height7 = rect7.Height();
		rect7.left = l1 + 2;
		rect7.right = rect7.left + width7;
		rect7.top = rect6.bottom + 18;
		rect7.bottom = rect7.top + height7;
		m_static2_ctrl.MoveWindow(rect7);

		CRect rect8;
		m_list1_ctrl.GetWindowRect(&rect8);
		rect8.left = l1;
		rect8.right = hd1;// rect1.right;
		rect8.top = rect7.bottom + 6;
		rect8.bottom = vd1;
		m_list1_ctrl.MoveWindow(rect8);

		CRect rect9;
		m_list2_ctrl.GetWindowRect(&rect9);
		rect9.left = l1;
		rect9.right = rect2.right;
		rect9.top = rect8.bottom + 12;
		rect9.bottom = rect.Height()- 68;
		m_list2_ctrl.MoveWindow(rect9);

		CRect rect11;
		m_save_ctrl.GetWindowRect(&rect11);
		int width11 = rect11.Width();
		int height11 = rect11.Height();
		rect11.top = t0 - 3;
		rect11.left = hd1 + 12;
		rect11.right = rect11.left + width11;
		rect11.bottom = rect11.top + height11;
		m_save_ctrl.MoveWindow(rect11);

		CClientDC dc(this);
		if (validGraphBox) {
			mgraph.eraseGraph(&dc, &mbrush);
		}
		CRect rect10;
		rect10.left = hd1 + 12;
		rect10.right = rect2.right;
		rect10.top = rect2.bottom + 12;
		rect10.bottom = rect8.bottom;
		mgraph.setSize(rect10);
		mgraph.drawGraph(&dc);
		mgraph.drawMultiData(&dc);
		validGraphBox = true;
		graphBox = rect10;

	}

}

void CClusterAnalysisDlg::addMyErrorString(CString err)
{
	m_list2_ctrl.InsertString(0, err);
	while (m_list2_ctrl.GetCount() > 1000) {
		m_list2_ctrl.DeleteString(m_list2_ctrl.GetCount() - 1);
	}
	int extent = 8 * err.GetLength();
	if (extent > m_list2_ctrl.GetHorizontalExtent()) {
		m_list2_ctrl.SetHorizontalExtent(extent + 20);
	}
}

void CClusterAnalysisDlg::clearMyErrorString()
{
	m_list2_ctrl.ResetContent();
}

void CClusterAnalysisDlg::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

void CClusterAnalysisDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages

	CRect crect;
	dc.GetClipBox(crect);
	dc.FillRect(&crect, &mbrush);

	mgraph.drawGraph(&dc);

}

BOOL CClusterAnalysisDlg::OnEraseBkgnd(CDC* pDC)
{
	return(true);
	//return CDialog::OnEraseBkgnd(pDC);
}

void CClusterAnalysisDlg::setButtonOff()
{
	if (displayed) {
		m_button_ctrl.SetWindowText(_T("Run"));
	}
}

void CClusterAnalysisDlg::OnBnClickedButton1()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (!pDoc->clustering) {
		if (dataOk) {
			clearMyErrorString();
			UpdateData();
			int nclusters = (int)_tstof((LPCTSTR)m_nclusters);
			//CString s = _T(""); s.Format(_T("nclusters = %i"), nclusters); AfxMessageBox(s);
			if (pDoc->setupClusterData(nclusters)) {
				addMyErrorString(_T("Clustering Started"));
				m_button_ctrl.SetWindowText(_T("Stop"));
				pDoc->clustering = true;
			}
			else {
				addMyErrorString(_T("Net verification error : Not ready"));
			}
		}
	}
	else {
		addMyErrorString(_T("Clustering stopped"));
		m_button_ctrl.SetWindowText(_T("Run"));
		pDoc->stopClustering();
	}
}

void CClusterAnalysisDlg::displayClusterStandardDeviations()
{
	if (dataOk) {
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		CString s = _T("");
		s.Format(_T("Cluster Data : cluster_inc = %i : "), pDoc->cluster_inc);
		for (int i = 0; i < pDoc->num_cluster_groups; i++) {
			CString s2 = _T("");
			s2.Format(_T("Cluster%i, cnt = %i, rms = %.4f"), i, pDoc->getClusterCount(i), pDoc->getClusterRMS(i));
			if (i > 0) {
				s += _T(" : ");
			}
			s += s2;
		}
		addMyErrorString(s);
	}
}

void CClusterAnalysisDlg::updateGraph(vector<double> sxdata, vector<vector<double>> sydata)
{
	if (dataOk) {
		CClientDC dc(this);
		mgraph.updateGraph(sxdata, sydata, &dc);
	}
}

void CClusterAnalysisDlg::OnBnClickedSave()
{
	AfxMessageBox(_T("Under construction"));
}
