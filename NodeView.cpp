// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#include "pch.h"
#include "framework.h"
#include "mainfrm.h"
#include "NodeView.h"
#include "Resource.h"
#include "NetDesigner.h"
#include "NetDesignerView.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CNodeView

CNodeView::CNodeView() noexcept
{
	lastSelectedItem = nullptr;
}

CNodeView::~CNodeView()
{
}

BEGIN_MESSAGE_MAP(CNodeView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CNodeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndNodeView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create node view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_NodeViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndNodeView.SetImageList(&m_NodeViewImages, TVSIL_NORMAL);

	OnChangeVisualStyle();

	// Fill in some static tree view data (dummy code, nothing magic here)
	//FillFileView();
	AdjustLayout();

	return 0;
}

void CNodeView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CNodeView::FillFileView()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	addPanel1String(_T("FillNodeView"));

	m_wndNodeView.DeleteAllItems();

	CString desc = _T("");

	//addPanel1String(_T("FillFileView : Source"));

	bool source = false;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isDataSource(i)) {
			source = true;
			i = pDoc->netData.size();
		}
	}
	if (source) {
		HTREEITEM hRoot = m_wndNodeView.InsertItem(_T("Data Sources"), 0, 0);
		m_wndNodeView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isDataSource(i)) {
				CString lbl = (CString)pDoc->netData.getID(i).c_str();
				CString mode = (CString)pDoc->netData.getDataMode(i).c_str();
				vector<string> paths = pDoc->netData.getFilePaths(i);
				desc = lbl + _T(" : ") + mode + _T(" : ");
				if (paths.size() > 0) {
					desc += (CString)paths[0].c_str();
				}
				else {
					desc += _T("Null Path");
				}
				HTREEITEM hRoot2 = m_wndNodeView.InsertItem(desc, 17, 17, hRoot);
			}
		}
		m_wndNodeView.Expand(hRoot, TVE_EXPAND);
	}


	//addPanel1String(_T("FillFileView : Convolutions"));

	bool convolutions = false;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConvLayer(i)) {
			convolutions = true;
			i = pDoc->netData.size();
		}
	}
	if (convolutions) {
		HTREEITEM hRoot = m_wndNodeView.InsertItem(_T("Convolutional Layers"), 0, 0);
		m_wndNodeView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConvLayer(i)) {
				string lbl = pDoc->netData.getID(i);
				string mode = pDoc->netData.getNodeMode(i);
				CAuxFunctions func;
				int mxin = func.getConvInputX(lbl);
				int myin = func.getConvInputY(lbl);
				int mxout = func.getConvOutputX(lbl);
				int myout = func.getConvOutputY(lbl);
				int stridex = func.getConvOutputStrideX(lbl);
				int stridey = func.getConvOutputStrideY(lbl);
				CString sdims = _T("");
				if ((mode == "Input") || (mode == "Hidden")) {
					sdims.Format(_T(" : Width Out = %i : Height Out = %i : StrideW = %i : StrideH = %i"), mxin, myin, stridex, stridey);
				}
				CString sdims2 = _T("");
				if ((mode == "Output") || (mode == "Hidden")) {
					sdims2.Format(_T(" : Width In = %i : Height In = %i"), mxout, myout);
				}
				desc = (CString)(lbl + " : " + mode).c_str() + sdims2 + sdims;
				HTREEITEM hRoot2 = m_wndNodeView.InsertItem(desc, 10, 10, hRoot);

				/*for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						CAuxFunctions func;
						CString nname = netData.getOutputNode(j);
						CString nname2 = func.getConvNameFromConnectName(nname);
						if (nname2 == lbl) {
							desc = netData.getID(j) + _T(" from ") + netData.getInputNode(j);
							m_wndNodeView.InsertItem(desc, 3, 3, hRoot2);
						}
					}
				}
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						CAuxFunctions func;
						CString nname = netData.getInputNode(j);
						CString nname2 = func.getConvNameFromConnectName(nname);
						if (nname2 == lbl) {
							desc = netData.getID(j) + _T(" to ") + netData.getOutputNode(j);
							m_wndNodeView.InsertItem(desc, 3, 3, hRoot2);
						}
					}
				}*/
			}
		}
		m_wndNodeView.Expand(hRoot, TVE_EXPAND);
	}


	//addPanel1String(_T("FillFileView : Nodes"));

	HTREEITEM hRoot = m_wndNodeView.InsertItem(_T("Nodes"), 0, 0);
	m_wndNodeView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	struct indices {
		int index1 = 0;
		int index2 = 0;
	};

	//addPanel1String(_T("FillFileView : Nodes1"));

	indices mind;
	CString lbl = _T("");
	CString num = _T("");
	vector<indices> nodes;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			mind.index1 = i;
			lbl = (CString)pDoc->netData.getID(i).c_str();
			num = lbl.Right(lbl.GetLength() - 4);
			mind.index2 = (int)_tstof((LPCTSTR)num);
			nodes.push_back(mind);
			addPanel1String(lbl);
		}
	}

	//CString s = _T("");
	//s.Format(_T(" : nodes.size = %i"), (int)nodes.size());
	//addPanel1String(_T("FillFileView : Nodes2") + s);

	if (nodes.size() > 1) {
		int tind1 = 0;
		int tind2 = 0;
		for (int i = 0; i < (nodes.size() - 1); i++) {
			for (int j = i + 1; j < nodes.size(); j++) {
				if (nodes[j].index2 < nodes[i].index2) {
					tind1 = nodes[i].index1;
					tind2 = nodes[i].index2;
					nodes[i].index1 = nodes[j].index1;
					nodes[i].index2 = nodes[j].index2;
					nodes[j].index1 = tind1;
					nodes[j].index2 = tind2;
				}
			}
		}
	}

	//addPanel1String(_T("FillFileView : Nodes3"));

	for (int i = 0; i < nodes.size(); i++) {
		int index = nodes[i].index1;
		if (pDoc->netData.isNode(index)) {
			int clr = 1;
			string mode = pDoc->netData.getNodeMode(index);
			string func = pDoc->netData.getNodeFunction(index);
			string data = pDoc->netData.getDataField(index);
			string pproc = pDoc->netData.getNodePreProcessor(index);
			if (data == "") {
				data = "No Data";
			}
			if (theme == 0) {
				if (mode == "Hidden") {
					clr = 2;
				}
				else if (mode == "Input") {
					clr = 4;
				}
				else if (mode == "PreProcess") {
					clr = 6;
				}
			}
			else {
				clr = 21;
				if (mode == "Hidden") {
					clr = 18;
				}
				else if (mode == "Input") {
					clr = 19;
				}
				else if (mode == "PreProcess") {
					clr = 20;
				}
			}
			string lbl = pDoc->netData.getID(index);
			desc = (CString)(lbl + " : " + mode).c_str();
			if ((mode == "Output") || (mode == "Hidden")) {
				desc += (CString)(" : " + func).c_str();
			}
			if ((mode == "Output") || (mode == "Input")) {
				desc += (CString)((" : " + data).c_str());
			}
			if (((mode == "PreProcess") || (mode == "Output")) && (pproc != "")) {
				desc += (CString)(" : " + pproc).c_str();
			}
			if ((mode == "Output") || (mode == "Hidden")) {
				CString s2 = _T("");
				s2.Format(_T(" : bias = %.4lf, rms weights = %.4lf"), pDoc->getBiasFromNode((CString)lbl.c_str()), pDoc->getRMSWeightsFromNode((CString)lbl.c_str()));
				desc += s2;
			}
			HTREEITEM hRoot2 = m_wndNodeView.InsertItem(desc, clr, clr, hRoot);

			if (pDoc->netData.size() < size_limit) {
				for (int j = 0; j < pDoc->netData.size(); j++) {
					if (pDoc->netData.isConnect(j)) {
						if (pDoc->netData.getOutputNode(j) == lbl) {
							desc = (CString)pDoc->netData.getID(j).c_str() + _T(" from ") + (CString)pDoc->netData.getInputNode(j).c_str();
							string tmode = pDoc->netData.getNodeMode(pDoc->netData.getIndexFromID(pDoc->netData.getInputNode(j)));
							if ((mode != "PreProcess") || (tmode != "Input")) {
								CString s2 = _T("");
								s2.Format(_T(" : weight = %.4lf"), pDoc->getWeightFromConnection((CString)pDoc->netData.getID(j).c_str()));
								desc += s2;
							}
							m_wndNodeView.InsertItem(desc, 3, 3, hRoot2);
						}
					}
				}
			}
		}
	}

	m_wndNodeView.Expand(hRoot, TVE_EXPAND);

	addPanel1String(_T(""));

}

void CNodeView::selectItem() 
{
	if (lastSelectedItem != nullptr) {
		m_wndNodeView.SelectItem(lastSelectedItem);
	}
}

void CNodeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	/*
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndNodeView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != nullptr)
		{
			pWndTree->SelectItem(hTreeItem);
			lastSelectedItem = hTreeItem;
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
	*/
}

void CNodeView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndNodeView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CNodeView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndNodeView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (pDoc != NULL) {
		if ((pDoc->netData.size() >= size_limit) && !pDoc->nodeview_message_displayed) {
			CString s = _T("");
			s.Format(_T("Node View not showing connection weights due to net size (%i) exceeding process delay (%i)"), pDoc->netData.size(), size_limit);
			pDoc->addErrorString(s);
			pDoc->nodeview_message_displayed = true;
		}
	}

}

void CNodeView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	
	m_wndNodeView.SetFocus();
}

void CNodeView::OnChangeVisualStyle()
{
	m_NodeViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASSVIEW16 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_NodeViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_NodeViewImages.Add(&bmp, RGB(255, 255, 255));

	m_wndNodeView.SetImageList(&m_NodeViewImages, TVSIL_NORMAL);

}

void CNodeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDockablePane::OnLButtonDown(nFlags, point);
}

void CNodeView::addPanel1String(CString msg)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addPanel1String(msg);
}


//***********************************************************************************
//***********************************************************************************


BEGIN_MESSAGE_MAP(CViewTreeCustom, CViewTree)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CViewTreeCustom::OnLButtonDown(UINT nFlags, CPoint point)
{
	CViewTree::OnLButtonDown(nFlags, point);
	HTREEITEM hItem = GetSelectedItem();
	CString iText = GetItemText(hItem);
	CString test1 = iText.Left(4);
	if (test1 == _T("Node")) {
		CString test2 = iText.Right(iText.GetLength() - 4);
		int index = (int)_tstof((LPCTSTR)test2) - 1;
		CString test3 = _T("");
		test3.Format(_T("%s%i"), test1, index+1);
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->selectComponent(test3);
	}
	else if (test1 == _T("Conn")) {
		CString test2 = iText.Right(iText.GetLength() - 7);
		int index = (int)_tstof((LPCTSTR)test2) - 1;
		CString test3 = _T("");
		test3.Format(_T("Connect%i"), index + 1);
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->selectComponent(test3);
	}
	else if (test1 == _T("Data")) {
		CString test2 = iText.Right(iText.GetLength() - 4);
		int index = (int)_tstof((LPCTSTR)test2) - 1;
		CString test3 = _T("");
		test3.Format(_T("Data%i"), index + 1);
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->selectComponent(test3);
	}
}
