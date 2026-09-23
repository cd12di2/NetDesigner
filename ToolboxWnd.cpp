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

#include "ToolboxWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"

#include <afxcmn.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CToolboxWnd::CToolboxWnd() noexcept
{
	//m_nComboHeight = 0;
}

CToolboxWnd::~CToolboxWnd()
{
}

BEGIN_MESSAGE_MAP(CToolboxWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CToolboxWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndToolList2.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

}

int CToolboxWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_TreeViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASSVIEW16 : IDB_CLASS_VIEW;
	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return-1;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_TreeViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_TreeViewImages.Add(&bmp, RGB(255, 255, 255));


	const DWORD dwViewStyle2 = WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndToolList2.Create(dwViewStyle2, rectDummy, this, 2))
	{
		TRACE0("Failed to create Toollist \n");
		return -1;      // fail to create
	}

	m_wndToolList2.SetImageList(&m_TreeViewImages, TVSIL_NORMAL);
	//m_wndToolList2.SetImageList(&m_TreeViewImages, TVSIL_STATE);


	InitToolbox();

	AdjustLayout();

	return 0;
}

void CToolboxWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CToolboxWnd::InitToolbox()
{
	m_wndToolList2.DeleteAllItems();

	SetToolboxFont();

	HTREEITEM hItem;
	hItem = m_wndToolList2.InsertItem(L"Components", 0, 0, TVI_ROOT);
	if (theme == 0) {
		m_wndToolList2.InsertItem(L"Input Node", 4, 4, hItem);
		m_wndToolList2.InsertItem(L"Hidden Node", 2, 2, hItem);
		m_wndToolList2.InsertItem(L"Output Node", 1, 1, hItem);
		m_wndToolList2.InsertItem(L"PreProcess", 6, 6, hItem);
		m_wndToolList2.InsertItem(L"Connect", 3, 3, hItem);
	}
	else {
		m_wndToolList2.InsertItem(L"Input Node", 19, 19, hItem);
		m_wndToolList2.InsertItem(L"Hidden Node", 18, 18, hItem);
		m_wndToolList2.InsertItem(L"Output Node", 21, 21, hItem);
		m_wndToolList2.InsertItem(L"PreProcess", 20, 20, hItem);
		m_wndToolList2.InsertItem(L"Connect", 3, 3, hItem);
	}
	m_wndToolList2.SetItemState(hItem, TVIS_BOLD, TVIS_BOLD);
	m_wndToolList2.Expand(hItem, TVE_EXPAND);
	hItem = m_wndToolList2.InsertItem(L"Modules", TVI_ROOT);
	m_wndToolList2.InsertItem(L"Net", 9, 9, hItem);
	m_wndToolList2.InsertItem(L"CNN", 10, 10, hItem);
	m_wndToolList2.InsertItem(L"AutoEncoder", 11, 11, hItem);
	m_wndToolList2.InsertItem(L"RNN", 12, 12, hItem);
	m_wndToolList2.InsertItem(L"LSTM", 13, 13, hItem);
	m_wndToolList2.SetItemState(hItem, TVIS_BOLD, TVIS_BOLD);
	m_wndToolList2.Expand(hItem, TVE_EXPAND);
	//hItem = m_wndToolList2.InsertItem(L"Data Sources", TVI_ROOT);
	//m_wndToolList2.InsertItem(L"CSV", 17, 17, hItem);
	//m_wndToolList2.InsertItem(L"JPG", 15, 15, hItem);
	//m_wndToolList2.InsertItem(L"SQL", 16, 16, hItem);
	m_wndToolList2.SetItemState(hItem, TVIS_BOLD, TVIS_BOLD);
	m_wndToolList2.Expand(hItem, TVE_EXPAND);

	
}

void CToolboxWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
}

void CToolboxWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetToolboxFont();
}

void CToolboxWnd::SetToolboxFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndToolList2.SetFont(&m_fntPropList);

}

void CToolboxWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDockablePane::OnLButtonDown(nFlags, point);
}



//************************************************************************
//************************************************************************


BEGIN_MESSAGE_MAP(CTreeCtrlCustom, CTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CTreeCtrlCustom::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonDown(nFlags, point);
	HTREEITEM hItem = HitTest(point);
	CString label = GetItemText(hItem);
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->startPlaceComponent(label);
}

void CTreeCtrlCustom::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

BOOL CTreeCtrlCustom::PreTranslateMessage(MSG* pMsg)
{
	int key = (int)pMsg->wParam;
	if (pMsg->message == WM_KEYDOWN) {
		if (key == VK_ESCAPE) {
			CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
			CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
			CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
			pView->escapePlaceComponent();
		}
	}
	return CTreeCtrl::PreTranslateMessage(pMsg);
}





