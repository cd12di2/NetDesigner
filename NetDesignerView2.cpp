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

// NetDesignerView.cpp : implementation of the CNetDesignerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "NetDesigner.h"
#endif

#include "MainFrm.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CGuideSettings.h"
#include "COptions.h"
#include "AuxFunctions.h"
#include "FileDataView.h"
#include "CPreProcessor.h"
#include "NetVerify.h"
#include "NetOutputDlg.h"
#include "CDataSetDlg.h"
#include "CSeedDlg.h"
#include "CAutoNetConfigDlg.h"
#include "CAutoRnnConfigDlg.h"
#include "CStatus.h"
#include "CEvaluateDlg.h"
#include "CAutoEncoderConfigDlg.h"

#include <math.h>

//#include <gdiplus.h>

//using namespace Gdiplus;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScrollViewCustom

BEGIN_MESSAGE_MAP(CViewCustom, CView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CViewCustom::OnEraseBkgnd(CDC* pDC)
{
	//return(true);
	return CView::OnEraseBkgnd(pDC);
}


// CNetDesignerView

IMPLEMENT_DYNCREATE(CNetDesignerView, CView)

BEGIN_MESSAGE_MAP(CNetDesignerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CNetDesignerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_GUIDE_SETTINGS, &CNetDesignerView::OnGuideSettings)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_SNAP, &CNetDesignerView::OnSnap)
	ON_COMMAND(ID_GRID, &CNetDesignerView::OnGrid)
	ON_UPDATE_COMMAND_UI(ID_SNAP, &CNetDesignerView::OnUpdateSnap)
	ON_UPDATE_COMMAND_UI(ID_GRID, &CNetDesignerView::OnUpdateGrid)
	ON_COMMAND(ID_100_PERCENT, &CNetDesignerView::On100Percent)
	ON_COMMAND(ID_ZOOM_FIT, &CNetDesignerView::OnZoomFit)
	ON_COMMAND(ID_ZOOM_SELECTION, &CNetDesignerView::OnZoomSelection)
	ON_COMMAND(ID_UNDO, &CNetDesignerView::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_UNDO, &CNetDesignerView::OnUpdateEditUndo)
	ON_COMMAND(ID_REDO, &CNetDesignerView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_REDO, &CNetDesignerView::OnUpdateEditRedo)
	ON_COMMAND(ID_TRAIN_BUTTON, &CNetDesignerView::OnTrain)
	ON_UPDATE_COMMAND_UI(ID_TRAIN_BUTTON, &CNetDesignerView::OnUpdateTrain)
	ON_COMMAND(ID_STOP_TRAIN, &CNetDesignerView::OnStopTrain)
	ON_UPDATE_COMMAND_UI(ID_STOP_TRAIN, &CNetDesignerView::OnUpdateStopTrain)
	ON_COMMAND(ID_EDIT_FIND, &CNetDesignerView::OnEditFind)
	ON_COMMAND(ID_EDIT_COPY, &CNetDesignerView::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CNetDesignerView::OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, &CNetDesignerView::OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CNetDesignerView::OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CNetDesignerView::OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CNetDesignerView::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CNetDesignerView::OnUpdateEditPaste)
	ON_COMMAND(ID_SELECT_ALL, &CNetDesignerView::OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_SELECT_ALL, &CNetDesignerView::OnUpdateSelectAll)
	ON_COMMAND(ID_SELECT_NODES, &CNetDesignerView::OnSelectNodes)
	ON_UPDATE_COMMAND_UI(ID_SELECT_NODES, &CNetDesignerView::OnUpdateSelectNodes)
	ON_COMMAND(ID_SELECT_CONNECTS, &CNetDesignerView::OnSelectConnects)
	ON_UPDATE_COMMAND_UI(ID_SELECT_CONNECTS, &CNetDesignerView::OnUpdateSelectConnects)
	ON_COMMAND(ID_SELECT_OTHER, &CNetDesignerView::OnSelectOther)
	ON_UPDATE_COMMAND_UI(ID_SELECT_OTHER, &CNetDesignerView::OnUpdateSelectOther)
	ON_COMMAND(ID_REDRAW, &CNetDesignerView::OnRedraw)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_MOVE, &CNetDesignerView::OnEditMove)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MOVE, &CNetDesignerView::OnUpdateEditMove)
	ON_COMMAND(ID_OPTIONS, &CNetDesignerView::OnOptions)
	ON_COMMAND(ID_ZOOM_OUT, &CNetDesignerView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &CNetDesignerView::OnUpdateEditFind)
	ON_COMMAND(ID_HIDDEN, &CNetDesignerView::OnHidden)
	ON_COMMAND(ID_OUTPUT, &CNetDesignerView::OnOutput)
	ON_COMMAND(ID_CONNECT, &CNetDesignerView::OnConnect)
	ON_COMMAND(ID_DATA_INPUT, &CNetDesignerView::OnDataInput)
	ON_COMMAND(ID_BUFFER, &CNetDesignerView::OnBuffer)
	ON_COMMAND(ID_CSV_DATA, &CNetDesignerView::OnCsvData)
	ON_UPDATE_COMMAND_UI(ID_HIDDEN, &CNetDesignerView::OnUpdateHidden)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT, &CNetDesignerView::OnUpdateOutput)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, &CNetDesignerView::OnUpdateConnect)
	ON_UPDATE_COMMAND_UI(ID_DATA_INPUT, &CNetDesignerView::OnUpdateDataInput)
	ON_UPDATE_COMMAND_UI(ID_BUFFER, &CNetDesignerView::OnUpdateBuffer)
	ON_UPDATE_COMMAND_UI(ID_CSV_DATA, &CNetDesignerView::OnUpdateCsvData)
	ON_COMMAND(ID_VERIFY_NET, &CNetDesignerView::OnVerifyNet)
	ON_COMMAND(ID_VIEW_DATA, &CNetDesignerView::OnViewData)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DATA, &CNetDesignerView::OnUpdateViewData)
	ON_COMMAND(ID_VALIDATION_SET, &CNetDesignerView::OnValidationSet)
	ON_UPDATE_COMMAND_UI(ID_VALIDATION_SET, &CNetDesignerView::OnUpdateValidationSet)
	ON_COMMAND(ID_EQUATION, &CNetDesignerView::OnEquation)
	ON_UPDATE_COMMAND_UI(ID_EQUATION, &CNetDesignerView::OnUpdateEquation)
	ON_COMMAND(ID_ARROWS, &CNetDesignerView::OnArrows)
	ON_COMMAND(ID_TIME_HRS, &CNetDesignerView::OnTimeHrs)
	ON_COMMAND(ID_EPOCHS, &CNetDesignerView::OnEpochs)
	ON_COMMAND(ID_MIN_EPOCHS, &CNetDesignerView::OnMinEpochs)
	ON_COMMAND(ID_ERROR_RATE, &CNetDesignerView::OnErrorRate)
	ON_COMMAND(ID_DELTA_ERROR, &CNetDesignerView::OnDeltaError)
	ON_UPDATE_COMMAND_UI(ID_RANDOMIZE, &CNetDesignerView::OnUpdateRandomize)
	ON_COMMAND(ID_UPDATE_RATE, &CNetDesignerView::OnUpdateRate)
	ON_COMMAND(ID_RANDOMIZE, &CNetDesignerView::OnRandomize)
	ON_UPDATE_COMMAND_UI(ID_VERIFY_NET, &CNetDesignerView::OnUpdateVerifyNet)
	ON_COMMAND(ID_OUTPUT_DATA, &CNetDesignerView::OnOutputData)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_DATA, &CNetDesignerView::OnUpdateOutputData)
	ON_COMMAND(ID_L1NORM, &CNetDesignerView::OnL1norm)
	ON_UPDATE_COMMAND_UI(ID_L1NORM, &CNetDesignerView::OnUpdateL1norm)
	ON_COMMAND(ID_L2NORM, &CNetDesignerView::OnL2norm)
	ON_UPDATE_COMMAND_UI(ID_L2NORM, &CNetDesignerView::OnUpdateL2norm)
	ON_WM_DESTROY()
	ON_COMMAND(ID_AUTOROUTE, &CNetDesignerView::OnAutoroute)
	ON_UPDATE_COMMAND_UI(ID_AUTOROUTE, &CNetDesignerView::OnUpdateAutoroute)
	ON_COMMAND(ID_KMEANS, &CNetDesignerView::OnKmeans)
	ON_UPDATE_COMMAND_UI(ID_KMEANS, &CNetDesignerView::OnUpdateKmeans)
	ON_COMMAND(PCA, &CNetDesignerView::OnPca)
	ON_UPDATE_COMMAND_UI(PCA, &CNetDesignerView::OnUpdatePca)
	ON_COMMAND(ID_RANDOM_SEED, &CNetDesignerView::OnRandomSeed)
	ON_UPDATE_COMMAND_UI(ID_RANDOM_SEED, &CNetDesignerView::OnUpdateRandomSeed)
	ON_COMMAND(ID_TRAIN_ENABLED, &CNetDesignerView::OnEnableTrain)
	ON_UPDATE_COMMAND_UI(ID_TRAIN_ENABLED, &CNetDesignerView::OnUpdateEnableTrain)
	ON_COMMAND(ID_NODE_ANALYSIS, &CNetDesignerView::OnNodeAnalysis)
	ON_UPDATE_COMMAND_UI(ID_NODE_ANALYSIS, &CNetDesignerView::OnUpdateNodeAnalysis)
	ON_COMMAND(ID_TEST, &CNetDesignerView::OnTest)
	ON_UPDATE_COMMAND_UI(ID_TEST, &CNetDesignerView::OnUpdateTest)
	END_MESSAGE_MAP()

// CNetDesignerView construction/destruction

CNetDesignerView::CNetDesignerView() noexcept
{
	netLoaded = false;
	set_initial_scroll = true;
	placing_component_type = _T("");
	zoomScale = 1.0;
	bZoomScale = 0.0;
	pZoomScale = 1.0;
	logicalWidth = 4000;
	logicalHeight = 2000;
	print_width = 1100;
	print_height = -850;
	displayWidth = logicalWidth;
	displayHeight = logicalHeight;
	displayCenterX = displayWidth / 2;
	displayCenterY = displayHeight / 2;
	lVertScollPos = 0;
	lHorzScollPos = 0;
	dVertScollPos = 0;
	dHorzScollPos = 0;
	selection_type = SELECT_ALL;
	move_state = MOVE_DONE;
	paste_state = PASTE_DONE;
	past_mindex = -1;
	selection_box = false;
	selection_box_erase = false;
	searchItemActive = false;
	connecting = false;
	connecting_erase = false;
	routing = false;
	route_origin = CPoint(0, 0);
	route_angle = 0;
	printing = false;
	connectingPoints.clear();
	connectingComponents.clear();
	new_group = 0;

	copyData.clear();
	newGroupData.clear();

}

CNetDesignerView::~CNetDesignerView()
{
}

BOOL CNetDesignerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);	
}

void CNetDesignerView::OnInitialUpdate()
{

	initScroll();

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CNetDesignerDoc* pDoc = GetDocument();

	if (pDoc->metalNodes) {
		pMainWnd->m_wndNodeView.setTheme(0);
	}
	else {
		pMainWnd->m_wndNodeView.setTheme(1);
	}
	pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);

	if (pDoc->metalNodes) {
		pMainWnd->m_wndToolbox.setTheme(0);
	}
	else {
		pMainWnd->m_wndToolbox.setTheme(1);
	}
	pMainWnd->m_wndToolbox.InitToolbox();

	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.SetMapMode(MM_TEXT);
	dc.SetViewportExt(logicalWidth, logicalHeight);
	dc.SetWindowExt(logicalWidth, logicalHeight);

	escapePlaceComponent();
	endSelect();
	selection_box = false;
	move_state = MOVE_DONE;
	paste_state = PASTE_DONE;
	past_mindex = -1;
	//On100Percent();

	addPanel1String(_T(""));

	CString data = _T("");

	data.Format(_T("%.0lf"), pDoc->maxEpochs);
	pMainWnd->setEpochs(data);
	data.Format(_T("%.0lf"), pDoc->minEpochs);
	pMainWnd->setMinEpochs(data);
	data.Format(_T("%.1lf"), pDoc->trainTime);
	pMainWnd->setTrainTime(data);
	data.Format(_T("%.3lf"), pDoc->targetError);
	pMainWnd->setTargetError(data);
	data.Format(_T("%.0lf"), pDoc->targetDeltaError);
	pMainWnd->setTargetDeltaError(data);
	data.Format(_T("%.0lf"), pDoc->updateRate);
	pMainWnd->setUpdateRate(data);

	/*
	_int64* add1 = new _int64[10];
	double* dbuf1 = new double[10];
	for (int i = 0; i < 10; i++) {
		dbuf1[i] = (double)(i * 12345678) / 4312;
	}
	CString s = _T("");
	s.Format(_T("%.6f : %.6f : %.6f : %.6f"), dbuf1[0], dbuf1[1], dbuf1[2], dbuf1[3]);
	AfxMessageBox(s);
	add1[0] = (_int64)(dbuf1 + 0);
	add1[1] = (_int64)(dbuf1 + 1);
	add1[2] = (_int64)(dbuf1 + 2);
	add1[3] = (_int64)(dbuf1 + 3);
	s.Format(_T("%.6f : %.6f : %.6f : %.6f"), *(double*)add1[0], *(double*)add1[1], *(double*)add1[2], *(double*)add1[3]);
	AfxMessageBox(s);
	*/

}

void CNetDesignerView::OnDestroy()
{
	//AfxMessageBox(_T("OnDestroy"));
	CView::OnDestroy();
}

// CNetDesignerView drawing

void CNetDesignerView::OnDraw(CDC* pDC)
{
	CNetDesignerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	checkZoom();

	UpdateDrawingData();

	DrawGrid(pDC);
	DrawComponents(pDC);

}


// CNetDesignerView printing


void CNetDesignerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CNetDesignerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	return DoPreparePrinting(pInfo);
}

void CNetDesignerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CPrintDialog* printdlg = new CPrintDialog(FALSE);
	printdlg->GetDefaults();

	DEVMODE FAR* dmode = printdlg->GetDevMode();

	dmode->dmOrientation = DMORIENT_LANDSCAPE;
	dmode->dmCopies = 1;
	pDC->ResetDC(dmode);
	pDC->SetMapMode(MM_LOENGLISH);
	delete printdlg;

	printing = true;

}

void CNetDesignerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	printing = false;
}

void CNetDesignerView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CNetDesignerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	print_width = (float)pInfo->m_rectDraw.Width();;
	print_height = (float)pInfo->m_rectDraw.Height();

	UpdatePrintingData();

	CPen pen(PS_SOLID, 4, clr_connect);
	pDC->SelectObject(pen);
	pDC->MoveTo(50,-50);
	pDC->LineTo((int)print_width - 50,-50);
	pDC->MoveTo(50, (int)print_height + 50);
	pDC->LineTo((int)print_width - 50, (int)print_height + 50);

	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			DrawConnect(pDC, i, pZoomScale, DRAW_NORMAL, false);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			DrawNode(pDC, i, pZoomScale, DRAW_NORMAL, false);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isDataSource(i)) {
			//DrawDataSource(pDC, i, pZoomScale);
		}
	}

	int mfont = 3;
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = -MulDiv(mfont, pDC->GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_BOLD;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;

	CFont newFont;
	if (!newFont.CreateFontIndirect(&lf)) {
		return;
	}
	CFont* pOldFont = pDC->SelectObject(&newFont);

	CString s = pDoc->GetPathName();
	for (int i = (s.GetLength() - 1); i > 0; i--) {
		if (s[i] == '\\') {
			s = s.Right(s.GetLength() - i - 1);
			i = 0;
		}
	}
	if (s == _T("")) {
		s = _T("Untitled.nnt");
	}

	int x = (int)(print_width - 80 - ((float)s.GetLength() * 12.2));
	int y = (int)print_height + 50 + 10 - lf.lfHeight;

	pDC->SetTextColor(clr_connect);
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(x, y, s);
	
	pDC->SelectObject(pOldFont);
	DeleteObject(newFont);

}

void CNetDesignerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (placing_component_type != _T("")) {
		escapePlaceComponent();
	}
	else if (selectedComponents.size() > 0) {
		//CPoint pt = DPtoLP(point);
		//if (pDoc->checkComponentPlacement(pt.x, pt.y, selection_type) >= 0) {
		if (checkValidSelection(point)) {
			ClientToScreen(&point);
			OnContextMenu(this, point);
		}
		else {
			endSelect();
			selection_box = false;
			move_state = MOVE_DONE;
			paste_state = PASTE_DONE;
			past_mindex = -1;
			OnDraw();
		}
	}
	else {
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}
}

void CNetDesignerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
	if (connecting && (placing_component_type == _T("Connect"))) {
		CNetDesignerDoc* pDoc = GetDocument();
		CPoint mpt = DPtoLP(point);
		int pindex = pDoc->checkComponentPlacement(mpt.x, mpt.y, SELECT_NODES);
		if (connectingPoints.size() == 0) {
			if (pindex >= 0) {
				connecting_erase = false;
				connectingComponents.push_back(pindex);
				CPoint npt(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex));
				npt = LPtoDP(npt);
				connectingPoints.push_back(npt);
			}
		}
		else if (connectingPoints.size() > 0) {
			CPoint npt = LPtoDP(applyGrid(DPtoLP(point)));
			if ((pindex >= 0) && (pindex != connectingComponents[connectingComponents.size()-1])) {
				connectingComponents.push_back(pindex);
				CPoint npt2(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex));
				npt2 = LPtoDP(npt2);
				connectingEraseLine();
				connectingPoints.push_back(npt2);
				endPlaceComponent(point);
				CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
				pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
				pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			}
			else if (npt != connectingPoints[connectingPoints.size()-1]) {
				connectingDrawLine(npt);
				connectingPoints.push_back(npt);
			}
		}
	}
	else if (checkIfNodeSelected() && (move_state == MOVE_CAPTURE) && checkValidSelection(point)) {
		moveStartPoint = point;
		moveLastPoint = moveStartPoint;
		move_state = MOVE_MOVING;
		addStragglingConnectionsToSelectedComponents();
		drawSelectedComponents(DRAW_ERASE);
		drawSelectedComponents(DRAW_HIGHLIGHT);
		addPanel1String(_T("Moving"));
	}
	else if (checkIfNodeSelected() && (move_state == MOVE_DONE) && checkValidSelection(point) && !(nFlags & MK_CONTROL)) {
		moveStartPoint = point;
		moveLastPoint = moveStartPoint;
		move_state = MOVE_CHECKING;
	}
	else if (move_state == MOVE_MOVING) {
		move_state = MOVE_DONE;
		CPoint mpt = point;
		CPoint pt2;
		pt2.x = mpt.x - moveStartPoint.x;
		pt2.y = mpt.y - moveStartPoint.y;
		pt2.x = (int)((float)pt2.x / zoomScale);
		pt2.y = (int)((float)pt2.y / zoomScale);
		finalMoveSelectedItems(pt2);
		updateMovedProperties(moveStartPoint, moveLastPoint);
		addPanel1String(_T(""));
		endSelect();
	}
	else if (paste_state == PASTE_WAITING) {
		pasteLastPoint = applyGrid(point);
		drawCopiedComponents(pasteLastPoint, past_mindex, DRAW_HIGHLIGHT);
		paste_state = PASTE_MOVING;
		addPanel1String(_T("Ready to Paste"));
	}
	else if (paste_state == PASTE_MOVING) {
		paste_state = PASTE_DONE;
		CPoint mpt = applyGrid(point);
		drawCopiedComponents(pasteLastPoint, past_mindex, DRAW_NORMAL);
		updatePastedProperties(pasteLastPoint, past_mindex);
		addPanel1String(_T(""));
		endSelect();
	}
	else if (routing) {
		autoRoute();
	}
	else if (placing_component_type == _T("")) {
		bool add = false;
		if (nFlags & MK_CONTROL) {
			add = true;
		}
		if (!selectComponent(point, add)) {
			selection_box = true;
			selection_box_erase = false;
			selection_pt1 = point;
			CNetDesignerDoc* pDoc = GetDocument();
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
			addPanel1String(_T("Selecting"));
		}
		move_state = MOVE_DONE;
		paste_state = PASTE_DONE;
	}
}

void CNetDesignerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);
	if (move_state == MOVE_CHECKING) {
		move_state = MOVE_DONE;
	}
	if (endPlaceComponent(point)) {
		CNetDesignerDoc* pDoc = GetDocument();
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
	}
	if (selection_box) {
		
		CClientDC dc(this);
		OnPrepareDC(&dc);

		CPen pen(PS_DOT, 0, PALETTERGB(128, 128, 128));
		dc.SelectObject(&pen);
		SelectObject(dc, GetStockObject(NULL_BRUSH));

		int hdc = dc.SetROP2(R2_NOT);
		if (selection_box_erase) {
			dc.MoveTo(selection_pt1);
			dc.LineTo(selection_pt1.x, selection_pt2.y);
			dc.LineTo(selection_pt2);
			dc.LineTo(selection_pt2.x, selection_pt1.y);
			dc.LineTo(selection_pt1);
		}
		selection_box = false;
		dc.SetROP2(hdc);

		CNetDesignerDoc* pDoc = GetDocument();

		CPoint mpt(0,0);
		bool add = false;
		if (nFlags & MK_CONTROL) {
			add = true;
		}
		for (int i = 0; i < pDoc->netData.size(); i++) {
			bool found = false;
			if (pDoc->netData.isNode(i)) {
				mpt.x = pDoc->netData.getXDisp(i);
				mpt.y = pDoc->netData.getYDisp(i);
				found = true;
			}
			else if (pDoc->netData.isConnect(i)) {
				CPoint lpt1 = pDoc->netData.getInputNodePoint(i);
				CPoint lpt2 = pDoc->netData.getOutputNodePoint(i);
				mpt.x = (lpt1.x + lpt2.x) / 2;
				mpt.y = (lpt1.y + lpt2.y) / 2;
				found = true;
			}
			else if (pDoc->netData.isDataSource(i)) {
				mpt.x = pDoc->netData.getXDisp(i);
				mpt.y = pDoc->netData.getYDisp(i);
				found = true;
			}
			if (found) {
				mpt = LPtoDP(mpt);
				if ((((mpt.x > selection_pt1.x) && (mpt.x < point.x)) || ((mpt.x < selection_pt1.x) && (mpt.x > point.x))) &&
					(((mpt.y > selection_pt1.y) && (mpt.y < point.y)) || ((mpt.y < selection_pt1.y) && (mpt.y > point.y)))) {
					selectComponent(i, add);
					add = true;
				}
			}
		}

		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
		//pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);

	}
}

void CNetDesignerView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!nFlags && MK_LBUTTON && selection_box) {
		selection_box = false;
		OnDraw();
	}
	if (move_state == MOVE_CHECKING) {
		float dx = (float)(point.x - moveStartPoint.x);
		float dy = (float)(point.y - moveStartPoint.y);
		float dr = sqrt(dx*dx + dy*dy);
		if (dr > 5) {
			move_state = MOVE_MOVING;
			addStragglingConnectionsToSelectedComponents();
			drawSelectedComponents(DRAW_HIGHLIGHT);
			drawSelectedComponents(DRAW_ERASE);
			drawSelectedComponents(DRAW_HIGHLIGHT);
			addPanel1String(_T("Moving"), point);
		}
	}
	else if (move_state == MOVE_MOVING) {	
		CPoint pt1 = moveLastPoint - moveStartPoint;
		pt1.x = (int)((float)pt1.x / zoomScale);
		pt1.y = (int)((float)pt1.y / zoomScale);
		CPoint mpt = point;
		CPoint pt2;
		pt2.x = mpt.x - moveStartPoint.x;
		pt2.y = mpt.y - moveStartPoint.y;
		pt2.x = (int)((float)pt2.x / zoomScale);
		pt2.y = (int)((float)pt2.y / zoomScale);
		moveSelectedItems(pt1, pt2);
		moveLastPoint = mpt;
		addPanel1String(_T("Moving"), point);
	}
	else if (paste_state == PASTE_MOVING) {
		CPoint mpt = applyGrid(point);
		moveCopiedItems(past_mindex, pasteLastPoint, mpt);
		pasteLastPoint = mpt;
		addPanel1String(_T("Paste waiting"), point);
	}
	else if (connecting) {
		connectingDrawLine(point);
		addPanel1String(_T("Connecting"), point);
	}
	else if (new_group == 1) {
		CPoint mpt = applyGrid(point);
		moveNewGroupItems(past_mindex, pasteLastPoint, mpt);
		pasteLastPoint = mpt;
		addPanel1String(_T("Adding Group"), point);
	}
	else if (selection_box) {
		
		CClientDC dc(this);
		OnPrepareDC(&dc);

		CPen pen(PS_DOT, 0, PALETTERGB(128, 128, 128));
		dc.SelectObject(&pen);
		SelectObject(dc, GetStockObject(NULL_BRUSH));

		int hdc = dc.SetROP2(R2_NOT);
		if (selection_box_erase) {
			dc.MoveTo(selection_pt1);
			dc.LineTo(selection_pt1.x, selection_pt2.y);
		}
		dc.MoveTo(selection_pt1);
		dc.LineTo(selection_pt1.x, point.y);
		if (selection_box_erase) {
			dc.MoveTo(selection_pt1.x, selection_pt2.y);
			dc.LineTo(selection_pt2);
		}
		dc.MoveTo(selection_pt1.x, point.y);
		dc.LineTo(point);
		if (selection_box_erase) {
			dc.MoveTo(selection_pt2);
			dc.LineTo(selection_pt2.x, selection_pt1.y);
		}
		dc.MoveTo(point);
		dc.LineTo(point.x, selection_pt1.y);
		if (selection_box_erase) {
			dc.MoveTo(selection_pt2.x, selection_pt1.y);
			dc.LineTo(selection_pt1);
		}
		dc.MoveTo(point.x, selection_pt1.y);
		dc.LineTo(selection_pt1);
		selection_box_erase = TRUE;
		selection_pt2 = point;
		dc.SetROP2(hdc);

		addPanel1String(_T("Selecting"), point);

	}
	else if (routing) {
		updateArrow(point);
		addPanel1String(_T("Routing"), point);
	}

	CView::OnMouseMove(nFlags, point);
}

void CNetDesignerView::connectingDrawLine(CPoint point)
{
	if (connectingPoints.size() > 0) {

		CClientDC dc(this);
		OnPrepareDC(&dc);

		CPen pen(PS_SOLID, 0, PALETTERGB(128, 128, 128));
		dc.SelectObject(&pen);
		SelectObject(dc, GetStockObject(NULL_BRUSH));

		int hdc = dc.SetROP2(R2_NOT);
		if (connecting_erase) {
			dc.MoveTo(connectingPoints[connectingPoints.size() - 1]);
			dc.LineTo(connecting_pt2);
		}
		dc.MoveTo(connectingPoints[connectingPoints.size() - 1]);
		dc.LineTo(point);

		connecting_erase = TRUE;
		connecting_pt2 = point;
		dc.SetROP2(hdc);

	}
}

void CNetDesignerView::connectingEraseLine()
{
	if (connecting_erase) {

		CClientDC dc(this);
		OnPrepareDC(&dc);

		CPen pen(PS_SOLID, 0, PALETTERGB(128, 128, 128));
		dc.SelectObject(&pen);
		SelectObject(dc, GetStockObject(NULL_BRUSH));

		int hdc = dc.SetROP2(R2_NOT);
		dc.MoveTo(connectingPoints[connectingPoints.size() - 1]);
		dc.LineTo(connecting_pt2);
		dc.SetROP2(hdc);

	}
}

BOOL CNetDesignerView::PreTranslateMessage(MSG* pMsg)
{
	int key = (int)pMsg->wParam;
	if (pMsg->message == WM_KEYDOWN) {
		if (key == VK_ESCAPE) {
			clearEditor();
		}
		else if (key == VK_DELETE) {
			addStragglingConnectionsToSelectedComponents();
			deleteSelectedComponents();
		}
		else if (key == VK_BACK) {
			addStragglingConnectionsToSelectedComponents();
			deleteSelectedComponents();
		}
		else if (key == VK_INSERT) {
			OnEditPaste(pMsg->pt);
			return true;
		}
		else if ((GetKeyState(VK_CONTROL) < 0) && (key == 0x43)) {
			OnEditCopy();
		}
		else if ((GetKeyState(VK_CONTROL) < 0) && (key == 0x56)) {
			OnEditPaste(pMsg->pt);
			return true;
		}
		else if ((GetKeyState(VK_CONTROL) < 0) && (key == 0x58)) {
			OnEditCut(pMsg->pt);
			return true;
		}
		else if ((GetKeyState(VK_CONTROL) < 0) && (key == 'M')) {
			OnEditMove();
		}
		else if ((GetKeyState(VK_CONTROL) < 0) && (key == 'Z')) {
			OnEditUndo();
		}
	}
	return CView::PreTranslateMessage(pMsg);
}

void CNetDesignerView::clearEditor()
{
	escapePlaceComponent();
	endSelect();
	selection_box = false;
	routing = false;
	move_state = MOVE_DONE;
	paste_state = PASTE_DONE;
	past_mindex = -1;
	OnDraw();
	addPanel1String(_T(""));
}

void CNetDesignerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	CNetDesignerDoc* pDoc = GetDocument();
	bool preprocess = false;
	bool learnable_node = false;
	if ((selectedComponents.size() == 1) && ((pDoc->netData.getNodeMode(selectedComponents[0]) == _T("PreProcess")) ||
		(pDoc->netData.getNodeMode(selectedComponents[0]) == _T("Output")))) {
		preprocess = true;
	}
	for (int i = 0; i < selectedComponents.size(); i++) {
		if (pDoc->netData.isNode(selectedComponents[i])) {
			if ((pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Hidden")) || 
				(pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Output"))) {
				learnable_node = true;
			}
		}
	}
	if (selectedComponents.size() == 0) {
		preprocess = false;
	}
	if ((selectedComponents.size() == 1) && (pDoc->netData.isDataSource(selectedComponents[0]))) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT2, point.x, point.y, this, TRUE);
	}
	else if (preprocess && !learnable_node) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT3, point.x, point.y, this, TRUE);
	}
#if (RELEASIBILITY > 2) 
	else if (learnable_node && !preprocess) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT4, point.x, point.y, this, TRUE);
	}
	else if (learnable_node && preprocess) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT5, point.x, point.y, this, TRUE);
	}
#endif
	else {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
	}
#endif
}

BOOL CNetDesignerView::OnEraseBkgnd(CDC* pDC)
{
	return(true);
	//return CViewCustom::OnEraseBkgnd(pDC);
}


// CNetDesignerView diagnostics

#ifdef _DEBUG
void CNetDesignerView::AssertValid() const
{
	CView::AssertValid();
}

void CNetDesignerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNetDesignerDoc* CNetDesignerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNetDesignerDoc)));
	return (CNetDesignerDoc*)m_pDocument;
}
#endif //_DEBUG


// CNetDesignerView message handlers

void CNetDesignerView::OnGuideSettings()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CGuideSettings guideDlg;
	guideDlg.m_spacing = pDoc->snap_spacing;
	guideDlg.m_spacing2 = pDoc->guide_spacing;
	if (guideDlg.DoModal() == IDOK) {
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileString(_T("GuideSettings"), _T("SnapGrid"), guideDlg.m_spacing);
		pApp->WriteProfileString(_T("GuideSettings"), _T("GuideGrid"), guideDlg.m_spacing2);
		pDoc->snap_spacing = guideDlg.m_spacing;
		pDoc->guide_spacing = guideDlg.m_spacing2;
		OnDraw();
	}
}

void CNetDesignerView::OnEditCopy()
{
	drawSelectedComponents();
	deleteStragglingConnectionsFromSelectedComponents();
	copySelectedComponents();
}

void CNetDesignerView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((selectedComponents.size() > 0) && !pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEditCut()
{
	drawSelectedComponents();
	deleteStragglingConnectionsFromSelectedComponents();
	copySelectedComponents();
	addStragglingConnectionsToSelectedComponents();
	deleteSelectedComponents();
	OnEditPaste();
}

void CNetDesignerView::OnEditCut(CPoint pt)
{
	drawSelectedComponents();
	deleteStragglingConnectionsFromSelectedComponents();
	copySelectedComponents();
	addStragglingConnectionsToSelectedComponents();
	deleteSelectedComponents();
	OnEditPaste(pt);
}

void CNetDesignerView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((selectedComponents.size() > 0) && !pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEditPaste()
{
	if (copyData.size() > 0) {
		past_mindex = getCenterNodeOfCopiedNodes();
		if (past_mindex >= 0) {
			endSelect();
			paste_state = PASTE_WAITING;
			addPanel1String(_T("Paste waiting"));
		}
	}
}

void CNetDesignerView::OnEditPaste(CPoint pt)
{
	if (isPtOnScreen(pt)) {
		if (copyData.size() > 0) {
			past_mindex = getCenterNodeOfCopiedNodes();
			if (past_mindex >= 0) {
				endSelect();
				CRect wrect;
				GetWindowRect(&wrect);
				CPoint mpt(pt.x-wrect.left, pt.y-wrect.top);
				pasteLastPoint = applyGrid(mpt);
				drawCopiedComponents(pasteLastPoint, past_mindex, DRAW_HIGHLIGHT);
				paste_state = PASTE_MOVING;
				addPanel1String(_T("Paste moving"), pt);
			}
		}
	}
	else {
		OnEditPaste();
	}
}

bool CNetDesignerView::isPtOnScreen(CPoint pt)
{
	CRect wrect;
	GetWindowRect(&wrect);
	if ((pt.x > wrect.left) && (pt.x < wrect.right) && (pt.y > wrect.top) && (pt.y < wrect.bottom)) {
		return(true);
	}
	return false;
}

bool CNetDesignerView::isPtOnScreen2(CPoint pt)
{
	CRect crect;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.GetClipBox(crect);
	if ((pt.x > crect.left) && (pt.x < crect.right) && (pt.y > crect.top) && (pt.y < crect.bottom)) {
		return(true);
	}
	return false;
}

void CNetDesignerView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((copyData.size() > 0) && !pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEditMove()
{
	if (checkIfNodeSelected()) {
		move_state = MOVE_CAPTURE;
		addPanel1String(_T("Move waiting"));
	}
}

void CNetDesignerView::OnUpdateEditMove(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (checkIfNodeSelected() && !pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEditSelectAll()
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool add = false;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		selectComponent(i, add);
		add = true;
	}
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
	//pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
}

void CNetDesignerView::OnSelectAll()
{
	selection_type = SELECT_ALL;
}

void CNetDesignerView::OnUpdateSelectAll(CCmdUI* pCmdUI)
{
	if (selection_type == SELECT_ALL) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnSelectNodes()
{
	selection_type = SELECT_NODES;
}

void CNetDesignerView::OnUpdateSelectNodes(CCmdUI* pCmdUI)
{
	if (selection_type == SELECT_NODES) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnSelectConnects()
{
	selection_type = SELECT_CONNECTIONS;
}

void CNetDesignerView::OnUpdateSelectConnects(CCmdUI* pCmdUI)
{
	if (selection_type == SELECT_CONNECTIONS) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnSelectOther()
{
	selection_type = SELECT_OTHER;
}

void CNetDesignerView::OnUpdateSelectOther(CCmdUI* pCmdUI)
{
	if (selection_type == SELECT_OTHER) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnTrain()
{
	clearEditor();
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	bool reset = true;
	bool randomize = true;
	if (pDoc->new_net) {
		if (pDoc->activeWeights()) {
			if (AfxMessageBox(_T("Reset weights?"), MB_YESNO) == IDNO) {
				randomize = false;
			}
		}
	}
	else if (pDoc->train_count > 0) {
		if (AfxMessageBox(_T("Reset Net Before Training?"),MB_YESNO) == IDNO) {
			reset = false;
			randomize = false;
		}
	}
	if (reset) {
		if (pDoc->verifyNet(randomize)) {
			if (!randomize) {
				pDoc->setWeights();
			}
			pMainWnd->m_wndOutput.ShowPane(true, false, true);
			pMainWnd->m_wndOutput.setTab(0);
			pDoc->training = true;
			pDoc->train_timer.Start();
		}
	}
	else {
		pDoc->softRestart(randomize);
		pDoc->training = true;
	}
}

void CNetDesignerView::OnUpdateTrain(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnVerifyNet()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (!pDoc->training) {
		pMainWnd->m_wndOutput.ShowPane(true, false, true);
		pMainWnd->m_wndOutput.setTab(1);
		pDoc->verifyNet(false);
	}
}

void CNetDesignerView::OnUpdateVerifyNet(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnStopTrain()
{
	CNetDesignerDoc* pDoc = GetDocument();
	pDoc->stopTrain();
}

void CNetDesignerView::OnUpdateStopTrain(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnViewData()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->checkDataSource()) {
		CAuxFunctions func;
		vector<CString> mpaths = pDoc->getDataPathNames();
		if (mpaths.size() == 0) {
			mpaths = func.getCsvFileName(pDoc->getFirstDataPathName());
		}
		else if ((pDoc->getFirstDataPathName() == _T("")) || (pDoc->getFirstDataPathName() == _T("Null Filename"))) {
			mpaths = func.getCsvFileName(pDoc->getFirstDataPathName());
		}
		mpaths = func.checkHeadersFromDataSources(mpaths);
		if (mpaths.size() > 0) {
			if (mpaths[0] != _T("")) {
				pDoc->setDataPathNames(mpaths);
				//pDoc->trainData.getHeadersFromDataSources(pDoc->dataPathnames);
				CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
				pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
				FileDataView dlg;
				dlg.fnames = mpaths;
				if (dlg.DoModal() == IDOK) {
					if (dlg.fnames != mpaths) {
						pDoc->setDataPathNames(dlg.fnames);
						pDoc->dataPathnames = func.checkHeadersFromDataSources(pDoc->dataPathnames);
						//pDoc->trainData.getHeadersFromDataSources(pDoc->dataPathnames);
						pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
					}
				}
			}
		}
	}
	else {
		AfxMessageBox(_T("Please insert data source into net project"));
	}
}

void CNetDesignerView::OnUpdateViewData(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnValidationSet()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CDataSetDlg dlg;
	if (pDoc->use_validation_percent) {
		dlg.m_no_validation = 1;
	}
	else if (pDoc->use_validation_file) {
		dlg.m_no_validation = 2;
	}
	else {
		dlg.m_no_validation = 0;
	}
	dlg.m_percent.Format(_T("%.0f"), pDoc->validation_percent);
	dlg.m_browse = pDoc->validationPathnames[0];
	if (dlg.DoModal() == IDOK) {
		if (dlg.m_no_validation == 0) {
			pDoc->use_validation_percent = false;
			pDoc->use_validation_file = false;
		}
		else if (dlg.m_no_validation == 1) {
			pDoc->use_validation_percent = true;
			pDoc->use_validation_file = false;
		}
		else if (dlg.m_no_validation == 2) {
			pDoc->use_validation_percent = false;
			pDoc->use_validation_file = true;
		}
		pDoc->validationPathnames.clear();
		pDoc->validationPathnames.push_back(dlg.m_browse);
		pDoc->validation_percent = (float)_tstof((LPCTSTR)dlg.m_percent);
		pDoc->SetModifiedFlag(true);
	}
}

void CNetDesignerView::OnUpdateValidationSet(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnEquation()
{
	bool showdlg = false;
	CPreProcessor dlg;
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();
	if (selectedComponents.size() == 1) {
		dlg.node_id = pDoc->netData.getLabel(selectedComponents[0]);
		if (pDoc->netData.getNodeMode(selectedComponents[0]) == _T("PreProcess")) {
			for (int i = 0; i < pDoc->netData.size(); i++) {
				if (pDoc->netData.isConnect(i)) {
					if (pDoc->netData.getOutputNode(i) == dlg.node_id) {
						dlg.input_node_id = pDoc->netData.getInputNode(i);
						int index = pDoc->netData.getIndexFromID(dlg.input_node_id);
						dlg.input_node_id = pDoc->netData.getLabel(index);
						dlg.input_data = pDoc->netData.getDataField(index);
						//pDoc->dataSourceHeaders = func.getHeadersFromDataSource(pDoc->dataPathnames[0]);
						dlg.setMode(pDoc->netData.getNodePreProcessor(selectedComponents[0]));
						dlg.setLowerNormalizationFactor(pDoc->netData.getNodeLowerNormLimit(selectedComponents[0]));
						dlg.setUpperNormalizationFactor(pDoc->netData.getNodeUpperNormLimit(selectedComponents[0]));
						dlg.setDisallow(pDoc->netData.getNodeNormDisallow(selectedComponents[0]));
						dlg.setTime(pDoc->netData.getNodePreProcessorTime(selectedComponents[0]));
						dlg.setStride(pDoc->netData.getNodePreProcessorStride(selectedComponents[0]));
						dlg.stringdata.clear();
						for (int j = 0; j < pDoc->dataPathnames.size(); j++) {
							vector<CString> sdata = pDoc->getStringDataByFieldName(pDoc->dataPathnames[j], dlg.input_data);
							for (int k = 0; k < sdata.size(); k++) {
								dlg.stringdata.push_back(sdata[k]);
							}
						}
						i = pDoc->netData.size();
						showdlg = true;
					}
				}
			}
			if (!showdlg || (dlg.input_data == _T(""))) {
				AfxMessageBox(_T("Please select a PreProcess Node connected to an Input Node with a Data Reference"));
				showdlg = false;
			}
		}
		else if (pDoc->netData.getNodeMode(selectedComponents[0]) == _T("Output")) {
			dlg.input_node_id = pDoc->netData.getLabel(selectedComponents[0]);
			dlg.input_data = pDoc->netData.getDataField(selectedComponents[0]);
			if (dlg.input_data == _T("")) {
				AfxMessageBox(_T("Please add a Data Reference to Node"));
			}
			else {
				//pDoc->dataSourceHeaders = func.getHeadersFromDataSource(pDoc->dataPathnames[0]);
				dlg.setMode(pDoc->netData.getNodePreProcessor(selectedComponents[0]));
				dlg.setLowerNormalizationFactor(pDoc->netData.getNodeLowerNormLimit(selectedComponents[0]));
				dlg.setUpperNormalizationFactor(pDoc->netData.getNodeUpperNormLimit(selectedComponents[0]));
				dlg.setDisallow(pDoc->netData.getNodeNormDisallow(selectedComponents[0]));
				dlg.stringdata.clear();
				for (int j = 0; j < pDoc->dataPathnames.size(); j++) {
					vector<CString> sdata = pDoc->getStringDataByFieldName(pDoc->dataPathnames[j], dlg.input_data);
					for (int k = 0; k < sdata.size(); k++) {
						dlg.stringdata.push_back(sdata[k]);
					}
				}
				dlg.outputnode = true;
				showdlg = true;
			}
		}
		else {
			AfxMessageBox(_T("Please select a PreProcess Node or Output Node"));
		}
	}
	else {
		dlg.node_id = _T("Multiple");
	}
	if (showdlg) {
		if (dlg.DoModal() == IDOK) {
			if (!pDoc->training) {
				CString pmode = dlg.getMode();
				CString pmode2 = pmode.Left(10);
				CString pmode3 = pmode.Left(3);;
				if (((pmode == _T("NormLinear")) || (pmode == _T("NormLog"))) && (selectedComponents.size() == 1)) {
					netDataStruct tdata = pDoc->netData.getNode(selectedComponents[0]);
					tdata.setNodePreProcessor(pmode);
					tdata.setNodeLowerNormLimit(dlg.getLowerNormalizationFactor());
					tdata.setNodeUpperNormLimit(dlg.getUpperNormalizationFactor());
					tdata.setNodeNormDisallow(dlg.getDisallow());
					tdata.setNodePreProcessorTime(dlg.getTime());
					pDoc->pushEditNodeData(tdata);
					pDoc->processEditData();
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
					pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
				}
				else if ((selectedComponents.size() == 1) && ((pmode == _T("NormSeconds")) || (pmode == _T("NormMinutes")) || 
					(pmode == _T("NormHours")) || (pmode == _T("NormDays")) || (pmode == _T("Seconds")) || (pmode == _T("Minutes")) ||
					(pmode == _T("Hours")) || (pmode == _T("Days")))) {
					netDataStruct tdata = pDoc->netData.getNode(selectedComponents[0]);
					tdata.setNodePreProcessor(pmode);
					tdata.setNodeLowerNormLimit(dlg.getLowerNormalizationFactor());
					tdata.setNodeUpperNormLimit(dlg.getUpperNormalizationFactor());
					tdata.setNodeNormDisallow(dlg.getDisallow());
					tdata.setNodePreProcessorTime(dlg.getTime());
					pDoc->pushEditNodeData(tdata);
					pDoc->processEditData();
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
					pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
				}
				else if ((selectedComponents.size() == 1) && (pmode2 == _T("Derivative"))) {
					netDataStruct tdata = pDoc->netData.getNode(selectedComponents[0]);
					tdata.setNodePreProcessor(pmode);
					tdata.setNodeLowerNormLimit(dlg.getLowerNormalizationFactor());
					tdata.setNodeUpperNormLimit(dlg.getUpperNormalizationFactor());
					tdata.setNodeNormDisallow(dlg.getDisallow());
					tdata.setNodePreProcessorTime(dlg.getTime());
					tdata.setNodePreProcessorStride(dlg.getStride());
					pDoc->pushEditNodeData(tdata);
					pDoc->processEditData();
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
					pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
				}
				else if ((selectedComponents.size() == 1) && (pmode3 == _T("FFT"))) {
					netDataStruct tdata = pDoc->netData.getNode(selectedComponents[0]);
					tdata.setNodePreProcessor(pmode);
					tdata.setNodeLowerNormLimit(dlg.getLowerNormalizationFactor());
					tdata.setNodeUpperNormLimit(dlg.getUpperNormalizationFactor());
					tdata.setNodeNormDisallow(dlg.getDisallow());
					tdata.setNodePreProcessorTime(dlg.getTime());
					tdata.setNodePreProcessorStride(dlg.getStride());
					pDoc->pushEditNodeData(tdata);
					pDoc->processEditData();
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
					pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
				}
			}
		}
	}
}

void CNetDesignerView::OnUpdateEquation(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		bool enable = true;
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (!pDoc->netData.isNode(selectedComponents[i]) ||
				((pDoc->netData.getNodeMode(selectedComponents[0]) != _T("PreProcess")) && 
				(pDoc->netData.getNodeMode(selectedComponents[0]) != _T("Output")))) {
				enable = false;
				i = (int)selectedComponents.size();
			}
		}
		if (enable && (selectedComponents.size() > 0)) {
			pCmdUI->Enable(true);
		}
		else {
			pCmdUI->Enable(false);
		}
	}
}

void CNetDesignerView::OnEditUndo()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->undoLast(zoomScale, lHorzScollPos, lVertScollPos)) {
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
	}
}

void CNetDesignerView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((pDoc->backData.size() > 0) && !pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEditRedo()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->redoLast(zoomScale, lHorzScollPos, lVertScollPos)) {
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
	}
}

void CNetDesignerView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((pDoc->redoData.size() > 0) && !pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEditFind()
{
	showSelectedomponents();
}

void CNetDesignerView::OnUpdateEditFind(CCmdUI* pCmdUI)
{
	if (searchItemActive) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnRedraw()
{
	OnDraw();
}

void CNetDesignerView::OnUpdateSnap(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->snap_spacing == _T("None")) {
		pCmdUI->SetCheck(false);
	}
	else {
		pCmdUI->SetCheck(true);
	}
}

void CNetDesignerView::OnSnap()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->snap_spacing == _T("None")) {
		pDoc->snap_spacing = _T("8 ndus");
	}
	else {
		pDoc->snap_spacing = _T("None");
	}
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("GuideSettings"), _T("SnapGrid"), pDoc->snap_spacing);
}

void CNetDesignerView::OnUpdateGrid(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->guide_spacing == _T("None")) {
		pCmdUI->SetCheck(false);
	}
	else {
		pCmdUI->SetCheck(true);
	}
}

void CNetDesignerView::OnGrid()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->guide_spacing == _T("None")) {
		pDoc->guide_spacing = _T("32 ndus");
	}
	else {
		pDoc->guide_spacing = _T("None");
	}
	OnDraw();
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("GuideSettings"), _T("GuideGrid"), pDoc->guide_spacing);
}

void CNetDesignerView::OnArrows()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->arrows) {
		pDoc->arrows = false;
	}
	else {
		pDoc->arrows = true;
	}
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(_T("GuideSettings"), _T("Arrows"), pDoc->arrows);
	OnDraw();
}

void CNetDesignerView::OnTimeHrs()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	float data = (float)_tstof((LPCTSTR)pMainWnd->getTrainTime());
	if (data >= 0.01) {
		pDoc->trainTime = data;
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(_T("NetDefaults"), _T("MinTime"), (int)(100*data));
	}
}

void CNetDesignerView::OnEpochs()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	float data = (float)_tstof((LPCTSTR)pMainWnd->getEpochs());
	if (data >= 1) {
		pDoc->maxEpochs = data;
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(_T("NetDefaults"), _T("MaxEpochs"), (int)data);
	}
}

void CNetDesignerView::OnMinEpochs()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	float data = (float)_tstof((LPCTSTR)pMainWnd->getMinEpochs());
	if (data >= 1) {
		pDoc->minEpochs = data;
		pDoc->relative_min_epoch_count = data;
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(_T("NetDefaults"), _T("MinEpochs"), (int)data);
	}
}

void CNetDesignerView::OnErrorRate()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	float data = (float)_tstof((LPCTSTR)pMainWnd->getTargetError());
	if (data >= 0.001) {
		pDoc->targetError = data;
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(_T("NetDefaults"), _T("ErrorRate"), (int)(1000*data));
	}
}

void CNetDesignerView::OnDeltaError()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	float data = (float)_tstof((LPCTSTR)pMainWnd->getTargetDeltaError());
	if (data < 0) {
		data = -1 * data;
	}
	pDoc->targetDeltaError = data;
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(_T("NetDefaults"), _T("DeltaErrorRate"), (int)data);
}

void CNetDesignerView::OnUpdateRate()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	float data = (float)_tstof((LPCTSTR)pMainWnd->getUpdateRate());
	if (data >= 1) {
		pDoc->updateRate = data;
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(_T("NetDefaults"), _T("UpdateRate"), (int)data);
	}
}

void CNetDesignerView::OnRandomize()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		if (pDoc->randomize_data) {
			pDoc->randomize_data = false;
		}
		else {
			pDoc->randomize_data = true;
		}
	}
}

void CNetDesignerView::OnUpdateRandomize(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->randomize_data) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnOutputData()
{
	NetOutputDlg dlg;
	vector<CString> inputheaders;
	vector<CString> outputheaders;
	vector<CString> outputheadernodes;
	vector<CString> data;
	CNetDesignerDoc* pDoc = GetDocument();

	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			if (pDoc->netData.getNodeMode(i) == _T("Output")) {
				outputheaders.push_back(pDoc->netData.getDataField(i));
				outputheadernodes.push_back(pDoc->netData.getLabel(i));
			}
		}
	}

	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			if (pDoc->netData.getNodeMode(i) == _T("Input")) {
				inputheaders.push_back(pDoc->netData.getDataField(i));
				inputheaders.push_back(pDoc->netData.getLabel(i));
			}
		}
	}
	dlg.inputheaders = inputheaders;

	int index1 = 0;
	int index2 = 0;
	CString s = _T("");

	int mrows = pDoc->num_filedata_rows;
	if (mrows > 5000) {
		mrows = 5000;
	}

	dlg.drows = mrows;

	CStatus progress;
	progress.CreateDlg();
	progress.setLabel(_T("Calculating data . . ."));
	progress.updateProgress(0);

	for (int i = 0; i < mrows; i++) {
		index1 = i * pDoc->nnet.num_inputs;
		index2 = i * pDoc->nnet.num_outputs;
		pDoc->nnet.forward(pDoc->netinputs + index1);
		for (int j = 0; j < outputheaders.size(); j++) {
			s.Format(_T("%.3lf"), pDoc->getProcessedDataByFieldNameAndRowIndex(outputheaders[j], i));
			data.push_back(s);
			s.Format(_T("%.3lf"), pDoc->reProcessData(outputheadernodes[j], pDoc->nnet.getOutput(j)));
			data.push_back(s);
		}
		for (int j = 0; j < (int)inputheaders.size(); j++) {
			s = pDoc->getProcessedStringDataByFieldNameAndRowIndex(inputheaders[j], i);
			data.push_back(s);
		}
		progress.updateProgress((100 * (i + 1)) / (pDoc->num_filedata_rows - 1));
	}

	vector<CString> outputheaders2;
	for (int i = 0; i < outputheaders.size(); i++) {
		outputheaders2.push_back(outputheaders[i]);
		outputheaders2.push_back(outputheadernodes[i]);
	}
	dlg.outputheaders = outputheaders2;

	dlg.dcols = (int)outputheaders2.size() + (int)inputheaders.size();
	dlg.data = data;

	progress.DestroyDlg();

	dlg.DoModal();
}

void CNetDesignerView::OnUpdateOutputData(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnL1norm()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->l1norm) {
		pDoc->l1norm = false;
	}
	else {
		pDoc->l1norm = true;
		pDoc->l2norm = false;
	}
}

void CNetDesignerView::OnUpdateL1norm(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
	if (pDoc->l1norm) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnL2norm()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->l2norm) {
		pDoc->l2norm = false;
	}
	else {
		pDoc->l2norm = true;
		pDoc->l1norm = false;
	}
}

void CNetDesignerView::OnUpdateL2norm(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
	if (pDoc->l2norm) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnAutoroute()
{
	routing = true;
	route_angle = 0;
	route_origin.x = 0;
	route_origin.y = 0;
	if (selectedComponents.size() > 0) {
		CRect srect = getSelectedComponentsLogicalRectangle(true);
		route_origin.x = (srect.left + srect.right) / 2;
		route_origin.y = (srect.top + srect.bottom) / 2;
	}
	else {
		CRect srect = getAllComponentsLogicalRectangle();
		route_origin.x = (srect.left + srect.right) / 2;
		route_origin.y = (srect.top + srect.bottom) / 2;
	}
	DrawArrow(route_origin.x, route_origin.y, route_angle);
}

void CNetDesignerView::OnUpdateAutoroute(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !connecting && !selection_box && (placing_component_type == _T("")) && !routing && 
		(move_state != MOVE_MOVING) && (paste_state != PASTE_MOVING)) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnKmeans()
{
	AfxMessageBox(_T("OnKmeans"));
}

void CNetDesignerView::OnUpdateKmeans(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnPca()
{
	AfxMessageBox(_T("OnPca"));
}

void CNetDesignerView::OnUpdatePca(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnNodeAnalysis()
{
#if (RELEASIBILITY > 2) 
	AfxMessageBox(_T("OnNodeAnalysis"));
#else
	AfxMessageBox(_T("Node analysis under construction"));
#endif
}

void CNetDesignerView::OnUpdateNodeAnalysis(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnTest()
{
	CNetDesignerDoc* pDoc = GetDocument();
	//if (pDoc->verifyNet(false, true)) {
	if (!pDoc->new_net) {
		int ninputs = pDoc->nnet.num_inputs;
		int noutputs = pDoc->nnet.num_outputs;
		if ((ninputs == 0) || (noutputs == 0)) {
			AfxMessageBox(_T("Can't evaluate net : num inputs or num outputs = 0"));
			return;
		}
		//CString s = _T("");
		//s.Format(_T("num _inputs = %i : num_outputs = %i"), ninputs, noutputs);
		//AfxMessageBox(s);
		CEvaluateDlg dlg;
		dlg.DoModal();
	}
	else {
		AfxMessageBox(_T("Evaluate : Unable to initialize net"));
	}
}

void CNetDesignerView::OnUpdateTest(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->new_net) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnRandomSeed()
{
	CSeedDlg dlg;
	CNetDesignerDoc* pDoc = GetDocument();
	dlg.m_seed.Format(_T("%i"), pDoc->rand_seed);
	if (dlg.DoModal() == IDOK) {
		pDoc->rand_seed = (int)_tstof((LPCTSTR)dlg.m_seed);
		pDoc->SetModifiedFlag(true);
	}
}

void CNetDesignerView::OnUpdateRandomSeed(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEnableTrain()
{
#if (RELEASIBILITY > 2) 
	CNetDesignerDoc* pDoc = GetDocument();
	if (checkSelectedNodesAreTrainable()) {
		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Hidden")) ||
					(pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Output"))) {
					netDataStruct tdata = pDoc->netData.getNode(selectedComponents[i]);
					tdata.setNodeTrainable(false);
					pDoc->pushEditNodeData(tdata);
				}
			}
		}
		pDoc->processEditData();
	}
	else {
		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Hidden")) ||
					(pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Output"))) {
					netDataStruct tdata = pDoc->netData.getNode(selectedComponents[i]);
					tdata.setNodeTrainable(true);
					pDoc->pushEditNodeData(tdata);
				}
			}
		}
		pDoc->processEditData();
	}
#endif
}

void CNetDesignerView::OnUpdateEnableTrain(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
#if (RELEASIBILITY > 2) 
	if (checkSelectedNodesAreTrainable()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
#endif
}

bool CNetDesignerView::checkSelectedNodesAreTrainable()
{
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < selectedComponents.size(); i++) {
		if (pDoc->netData.isNode(selectedComponents[i])) {
			if ((pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Hidden")) ||
				(pDoc->netData.getNodeMode(selectedComponents[i]) == _T("Output"))) {
				if (!pDoc->netData.getNodeTrainable(selectedComponents[i])) {
					return(false);
				}
			}
		}
	}
	return(true);
}

void CNetDesignerView::On100Percent()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (pMainWnd->m_ribbonSlider.setScale(1.0)) {
		zoomScale = pMainWnd->m_ribbonSlider.getScale();
		initScroll();
	}
}

void CNetDesignerView::OnZoomFit()
{
	showSelectedomponents(true);
}

void CNetDesignerView::OnZoomSelection()
{
	zoomScale += 0.10F;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (zoomScale > (float)(pMainWnd->m_ribbonSlider.GetRangeMax()/100)) {
		zoomScale = (float)(pMainWnd->m_ribbonSlider.GetRangeMax()/100);
	}
	pMainWnd->m_ribbonSlider.setScale(zoomScale);
	zoomScale = pMainWnd->m_ribbonSlider.getScale();
}

void CNetDesignerView::OnZoomOut()
{
	zoomScale -= 0.10F;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (zoomScale < (float)(pMainWnd->m_ribbonSlider.GetRangeMin()/100)) {
		zoomScale = (float)(pMainWnd->m_ribbonSlider.GetRangeMin()/100);
	}
	pMainWnd->m_ribbonSlider.setScale(zoomScale);
	zoomScale = pMainWnd->m_ribbonSlider.getScale();
}

void CNetDesignerView::OnOptions()
{
	COptions dlg;
	CNetDesignerDoc* pDoc = GetDocument();
	dlg.m_metal_nodes = pDoc->metalNodes;
	dlg.m_color_nodes = !pDoc->metalNodes;
	dlg.defaultOptionsDlg.m_train_data = pDoc->show_train_data;
	dlg.defaultOptionsDlg.m_net_diag = pDoc->nnet.diagnostics;
	if (dlg.DoModal() == IDOK) {
		pDoc->show_train_data = dlg.defaultOptionsDlg.m_train_data;
		pDoc->nnet.setDiagnostics(dlg.defaultOptionsDlg.m_net_diag);
		if (dlg.m_metal_nodes != (BOOL)pDoc->metalNodes) {
			pDoc->metalNodes = dlg.m_metal_nodes;
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			if (pDoc->metalNodes) {
				pMainWnd->m_wndNodeView.setTheme(0);
			}
			else {
				pMainWnd->m_wndNodeView.setTheme(1);
			}
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			if (pDoc->metalNodes) {
				pMainWnd->m_wndToolbox.setTheme(0);
			}
			else {
				pMainWnd->m_wndToolbox.setTheme(1);
			}
			pMainWnd->m_wndToolbox.InitToolbox();
			OnDraw();
			CWinApp* pApp = AfxGetApp();
			pApp->WriteProfileInt(_T("Colors"), _T("Metal"), pDoc->metalNodes);
		}
	}
	//CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	//pMainWnd->updateLook();
}

void CNetDesignerView::OnHidden()
{
	startPlaceComponent(_T("Hidden Node"));
}

void CNetDesignerView::OnUpdateHidden(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnOutput()
{
	startPlaceComponent(_T("Output Node"));
}

void CNetDesignerView::OnUpdateOutput(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnConnect()
{
	startPlaceComponent(_T("Connect"));
}

void CNetDesignerView::OnUpdateConnect(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnDataInput()
{
	startPlaceComponent(_T("Input Node"));
}

void CNetDesignerView::OnUpdateDataInput(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnBuffer()
{
	startPlaceComponent(_T("PreProcess"));
}

void CNetDesignerView::OnUpdateBuffer(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnCsvData()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->checkDataSource()) {
		OnViewData();
	}
	else {
		startPlaceComponent(_T("CSV"));
	}
}

void CNetDesignerView::OnUpdateCsvData(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training/* || pDoc->dataSourceTrue*/) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}



// CNetDesignerView drawing functions

void CNetDesignerView::addErrorString(CString err)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.ShowPane(true, false, true);
	pMainWnd->m_wndOutput.setTab(1);
	pMainWnd->m_wndOutput.AddStatusString(err);
}

void CNetDesignerView::addPanel1String(CString msg, CPoint pt)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (pt != CPoint(0, 0)) {
		CString msgpt = _T("");
		msgpt.Format(_T(" (%i, %i)"), pt.x, pt.y);
		pMainWnd->SetStatusBarText1(msg + msgpt);
	}
	else {
		pMainWnd->SetStatusBarText1(msg);
	}
}


void CNetDesignerView::resetZoomAndScrolls(float zoom, int hscroll, int vscroll)
{
	zoomScale = zoom;
	lVertScollPos = vscroll;
	lHorzScollPos = hscroll;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_ribbonSlider.setScale(zoomScale);
	zoomScale = pMainWnd->m_ribbonSlider.getScale();
	EnableVerticalScroll();
	EnableHorizontalScroll();
	OnDraw();
}

void CNetDesignerView::initScroll()
{
	lVertScollPos = 0;
	lHorzScollPos = 0;
	dVertScollPos = 0;
	dHorzScollPos = 0;

	EnableVerticalScroll();
	EnableHorizontalScroll();

}

void CNetDesignerView::setScrollInfo()
{
	if (needVerticalScroll()) {
		EnableVerticalScroll(true);
	}
	else {
		EnableVerticalScroll(false);
	}
	if (needHorizontalScroll()) {
		EnableHorizontalScroll(true);
	}
	else {
		EnableHorizontalScroll(false);
	}
}

bool CNetDesignerView::needVerticalScroll()
{
	return(true);
}

bool CNetDesignerView::needHorizontalScroll()
{
	return(true);
}

void CNetDesignerView::EnableVerticalScroll(bool enable)
{
	if (enable) {
		CRect wrect;
		GetWindowRect(&wrect);
		SCROLLINFO scrollInfo;
		GetScrollInfo(SB_VERT, &scrollInfo);
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		scrollInfo.nMin = -(displayHeight / 2);
		scrollInfo.nMax = displayHeight / 2;
		scrollInfo.nPage = wrect.Height();
		scrollInfo.nPos = lVertScollPos - wrect.Height() / 2;
		dVertScollPos = scrollInfo.nPos;
		SetScrollInfo(SB_VERT, &scrollInfo);
		EnableScrollBarCtrl(SB_VERT, TRUE);
	}
	else {
		EnableScrollBarCtrl(SB_VERT, false);
	}
}

void CNetDesignerView::EnableHorizontalScroll(bool enable)
{
	if (enable) {
		CRect wrect;
		GetWindowRect(&wrect);
		SCROLLINFO scrollInfo;
		GetScrollInfo(SB_HORZ, &scrollInfo);
		scrollInfo.cbSize = sizeof(SCROLLINFO);
		scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		scrollInfo.nMin = -(displayWidth / 2);
		scrollInfo.nMax = displayWidth / 2;
		scrollInfo.nPage = wrect.Width();
		scrollInfo.nPos = lHorzScollPos - wrect.Width()/2;
		dHorzScollPos = scrollInfo.nPos;
		SetScrollInfo(SB_HORZ, &scrollInfo);
		EnableScrollBarCtrl(SB_HORZ, TRUE);
	}
	else {
		EnableScrollBarCtrl(SB_HORZ, false);
	}
}

void CNetDesignerView::showSelectedomponents(bool all)
{
	CRect mrect(0, 0, 0, 0);
	if (all) {
		mrect = getAllComponentsLogicalRectangle();
	}
	else {
		mrect = getSelectedComponentsLogicalRectangle();
	}

	CRect crect;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.GetClipBox(crect);

	float hzoom = 0.70F * ((float)crect.Width() / (float)(mrect.right - mrect.left));
	float vzoom = 0.70F * ((float)crect.Height() / (float)(mrect.bottom - mrect.top));

	zoomScale = hzoom;
	if (zoomScale > vzoom) {
		zoomScale = vzoom;
	}

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_ribbonSlider.setScale(zoomScale);
	zoomScale = pMainWnd->m_ribbonSlider.getScale();
	
	lHorzScollPos = (int)(zoomScale * (float)(mrect.right + mrect.left) / 2);
	lVertScollPos = (int)(zoomScale * (float)(mrect.bottom + mrect.top) / 2);

	CPoint npt = LPtoDP(CPoint(lHorzScollPos, lVertScollPos));

	dVertScollPos = npt.y;
	SetScrollPos(SB_VERT, dVertScollPos);

	dHorzScollPos = npt.x;
	SetScrollPos(SB_HORZ, dHorzScollPos);

	OnDraw();

}

void CNetDesignerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_HORZ, &scrollinfo);
	int& nPosFrom = scrollinfo.nPos;
	dHorzScollPos = nPos;
	bool update = false;
	switch (nSBCode)
	{
	case SB_BOTTOM:
		dHorzScollPos = scrollinfo.nMax;
		update = true;
		break;
	case SB_TOP:
		dHorzScollPos = scrollinfo.nMin;
		update = true;
		break;
	case SB_LINEUP:
		dHorzScollPos = scrollinfo.nPos - logicalWidth / 100;
		if (scrollinfo.nPos == scrollinfo.nMin) break;
		else if (dHorzScollPos < scrollinfo.nMin) dHorzScollPos = scrollinfo.nMin;
		update = true;
		break;
	case SB_LINEDOWN:
		dHorzScollPos = scrollinfo.nPos + logicalWidth / 100;
		if (scrollinfo.nPos == scrollinfo.nMax) break;
		else if (dHorzScollPos > scrollinfo.nMax) dHorzScollPos = scrollinfo.nMax;
		update = true;
		break;
	case SB_PAGEUP:
		dHorzScollPos = scrollinfo.nPos - logicalWidth / 20;
		if (scrollinfo.nPos == scrollinfo.nMin) break;
		else if (dHorzScollPos < scrollinfo.nMin) dHorzScollPos = scrollinfo.nMin;
		update = true;
		break;
	case SB_PAGEDOWN:
		dHorzScollPos = scrollinfo.nPos + logicalWidth / 20;
		if (scrollinfo.nPos == scrollinfo.nMax) break;
		else if (dHorzScollPos > scrollinfo.nMax) dHorzScollPos = scrollinfo.nMax;
		update = true;
		break;
	case SB_THUMBPOSITION:
		if (scrollinfo.nPos == scrollinfo.nMax && (nPos > (UINT)scrollinfo.nMax)) break;
		update = true;
		break;
	case SB_THUMBTRACK:
		if (scrollinfo.nPos == scrollinfo.nMax && (nPos > (UINT)scrollinfo.nMax)) break;
		update = true;
		break;
	case SB_ENDSCROLL:
		break;
	}
	if (update) {
		SetScrollPos(SB_HORZ, dHorzScollPos);
		CRect wrect;
		GetWindowRect(&wrect);
		lHorzScollPos = dHorzScollPos + wrect.Width() / 2;
		OnDraw();
	}
	else {
		dHorzScollPos = scrollinfo.nPos;
	}
}

void CNetDesignerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo);
	int& nPosFrom = scrollinfo.nPos;
	dVertScollPos = nPos;
	bool update = false;
	switch (nSBCode)
	{
	case SB_BOTTOM:
		dVertScollPos = scrollinfo.nMax;
		update = true;
		break;
	case SB_TOP:
		dVertScollPos = scrollinfo.nMin;
		update = true;
		break;
	case SB_LINEUP:
		dVertScollPos = scrollinfo.nPos - logicalHeight/100;
		if (dVertScollPos < scrollinfo.nMin) dVertScollPos = scrollinfo.nMin;
		update = true;
		break;
	case SB_LINEDOWN:
		dVertScollPos = scrollinfo.nPos + logicalHeight/100;
		if (dVertScollPos > scrollinfo.nMax) dVertScollPos = scrollinfo.nMax;
		update = true;
		break;
	case SB_PAGEUP:
		dVertScollPos = scrollinfo.nPos - logicalHeight/20;
		if (dVertScollPos < scrollinfo.nMin) dVertScollPos = scrollinfo.nMin;
		update = true;
		break;
	case SB_PAGEDOWN:
		dVertScollPos = scrollinfo.nPos + logicalHeight/20;
		if (dVertScollPos > scrollinfo.nMax) dVertScollPos = scrollinfo.nMax;
		update = true;
		break;
	case SB_THUMBPOSITION:
		if (dVertScollPos > scrollinfo.nMax) dVertScollPos = scrollinfo.nMax;
		if (dVertScollPos < scrollinfo.nMin) dVertScollPos = scrollinfo.nMin; 
		update = true;
		break;
	case SB_THUMBTRACK:
		if (dVertScollPos > scrollinfo.nMax) dVertScollPos = scrollinfo.nMax;
		if (dVertScollPos < scrollinfo.nMin) dVertScollPos = scrollinfo.nMin;
		update = true;
		break;
	case SB_ENDSCROLL:
		break;
	default:
		break;
	}
	if (update) {
		SetScrollPos(SB_VERT, dVertScollPos);
		CRect wrect;
		GetWindowRect(&wrect);
		lVertScollPos = dVertScollPos + wrect.Height() / 2;
		OnDraw();
	}
	else {
		dVertScollPos = scrollinfo.nPos;
	}
}

CPoint CNetDesignerView::DPtoLP(CPoint dpt)
{
	CPoint lpt = dpt;
	lpt.x = (lpt.x + dHorzScollPos) * logicalWidth / displayWidth;
	lpt.y = (lpt.y + dVertScollPos) * logicalHeight / displayHeight;
	return(lpt);
}

CPoint CNetDesignerView::LPtoDP(CPoint lpt)
{
	CPoint dpt = lpt;
	if (printing) {
		CRect wrect;
		GetWindowRect(&wrect);
		int x_offset = (int)((float)(((wrect.right - wrect.left) / 2) + dHorzScollPos) / zoomScale);
		int y_offset = (int)((float)(((wrect.bottom - wrect.top) / 2) + dVertScollPos) / zoomScale);
		dpt.x = (long)(print_width / 2) + (long)(pZoomScale * (float)(dpt.x - x_offset));
		dpt.y = (long)(print_height / 2) - (long)(pZoomScale * (float)(dpt.y - y_offset));
	}
	else {
		dpt.x = (dpt.x * displayWidth / logicalWidth) - dHorzScollPos;
		dpt.y = (dpt.y * displayHeight / logicalHeight) - dVertScollPos;
	}
	return(dpt);
}

void CNetDesignerView::checkZoom()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	float tzoom = pMainWnd->m_ribbonSlider.getScale();
	if (tzoom <= zoomScale) {
		CRect wrect;
		GetWindowRect(&wrect);
		float fzoom = (float)wrect.Width() / (float)logicalWidth;
		int izoom = (int)((float)logicalWidth * fzoom);
		float tzoom1 = (float)izoom / (float)logicalWidth;
		fzoom = (float)wrect.Height() / (float)logicalHeight;
		izoom = (int)((float)logicalHeight * fzoom);
		float tzoom2 = (float)izoom / (float)logicalHeight;
		if (tzoom2 > tzoom1) {
			tzoom1 = tzoom2;
		}
		if (tzoom1 > zoomScale) {
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			if (tzoom2 > tzoom1) {
				tzoom1 = tzoom2;
			}
			if (pMainWnd->m_ribbonSlider.setScale(tzoom1)) {
				zoomScale = pMainWnd->m_ribbonSlider.getScale();
				initScroll();
			}
		}
	}
}


void CNetDesignerView::OnDraw()
{
	Invalidate();
}

void CNetDesignerView::SetColors()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	int theme = pMainWnd->getTheme();
	if (theme == LIGHT) {
		clr_back = clr_ltback;
		clr_grid = clr_ltgrid;
		clr_outnode = clr_ltoutnode;
		clr_hiddennode1 = clr_lthiddennode1;
		clr_hiddennode2 = clr_lthiddennode2;
		clr_hiddennode3 = clr_lthiddennode3;
		clr_hiddennode4 = clr_lthiddennode4;
		clr_hiddennode5 = clr_lthiddennode5;
		clr_hiddennode6 = clr_lthiddennode6;
		clr_connect = clr_ltconnect;
		clr_input = clr_ltinput;
		clr_buffer = clr_ltbuffer;
		clr_text = clr_lttext;
		clr_inactive = clr_ltinactive;
	}
	else if (theme == COLOR) {
		clr_back = clr_blback;
		clr_grid = clr_blgrid;
		clr_outnode = clr_bloutnode;
		clr_hiddennode1 = clr_blhiddennode1;
		clr_hiddennode2 = clr_blhiddennode2;
		clr_hiddennode3 = clr_blhiddennode3;
		clr_hiddennode4 = clr_blhiddennode4;
		clr_hiddennode5 = clr_blhiddennode5;
		clr_hiddennode6 = clr_blhiddennode6;
		clr_connect = clr_blconnect;
		clr_input = clr_blinput;
		clr_buffer = clr_blbuffer;
		clr_text = clr_bltext;
		clr_inactive = clr_blinactive;
	}
	else if (theme == BLUE) {
		clr_back = adjustColorByPercent(clr_blback, 10);
		clr_grid = clr_blgrid;
		clr_outnode = clr_bloutnode;
		clr_hiddennode1 = clr_blhiddennode1;
		clr_hiddennode2 = clr_blhiddennode2;
		clr_hiddennode3 = clr_blhiddennode3;
		clr_hiddennode4 = clr_blhiddennode4;
		clr_hiddennode5 = clr_blhiddennode5;
		clr_hiddennode6 = clr_blhiddennode6;
		clr_connect = clr_blconnect;
		clr_input = clr_blinput;
		clr_buffer = clr_blbuffer;
		clr_text = clr_bltext;
		clr_inactive = clr_blinactive;
	}
	else if (theme == DARK) {
		clr_back = clr_dkback;
		clr_grid = clr_dkgrid;
		clr_outnode = clr_dkoutnode;
		clr_hiddennode1 = clr_dkhiddennode1;
		clr_hiddennode2 = clr_dkhiddennode2;
		clr_hiddennode3 = clr_dkhiddennode3;
		clr_hiddennode4 = clr_dkhiddennode4;
		clr_hiddennode5 = clr_dkhiddennode5;
		clr_hiddennode6 = clr_dkhiddennode6;
		clr_connect = clr_dkconnect;
		clr_input = clr_dkinput;
		clr_buffer = clr_dkbuffer;
		clr_text = clr_dktext;
		clr_inactive = clr_dkinactive;
	}
	else {
		clr_back = clr_ltback;
		clr_grid = clr_ltgrid;
		clr_outnode = clr_ltoutnode;
		clr_hiddennode1 = clr_lthiddennode1;
		clr_hiddennode2 = clr_lthiddennode2;
		clr_hiddennode3 = clr_lthiddennode3;
		clr_hiddennode4 = clr_lthiddennode4;
		clr_hiddennode5 = clr_lthiddennode5;
		clr_hiddennode6 = clr_lthiddennode6;
		clr_connect = clr_ltconnect;
		clr_input = clr_ltinput;
		clr_buffer = clr_ltbuffer;
		clr_text = clr_lttext;
		clr_inactive = clr_ltinactive;
	}
	if (!pDoc->metalNodes) {
		clr_outnode = clr_mtloutnode;
		clr_hiddennode1 = clr_mtlhiddennode1;
		clr_hiddennode2 = clr_mtlhiddennode2;
		clr_hiddennode3 = clr_mtlhiddennode3;
		clr_hiddennode4 = clr_mtlhiddennode4;
		clr_hiddennode5 = clr_mthiddennode5;
		clr_hiddennode6 = clr_mthiddennode6;
		clr_input = clr_mtlinput;
		clr_buffer = clr_mtlbuffer;
	}
}

void CNetDesignerView::UpdateDrawingData()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();

	SetColors();
	
	zoomScale = pMainWnd->m_ribbonSlider.getScale();

	if (zoomScale != bZoomScale) {

		int tzoom = (int)((float)logicalWidth * zoomScale);
		zoomScale = ((float)(tzoom) / (float)logicalWidth);

		displayWidth = (int)((float)logicalWidth * zoomScale);
		displayHeight = (int)((float)logicalHeight * zoomScale);
		displayCenterX = displayWidth / 2;
		displayCenterY = displayHeight / 2;

		bZoomScale = zoomScale;

	}

	setScrollInfo();

}

void CNetDesignerView::UpdatePrintingData()
{
	SetColors();
	CRect wrect;
	GetWindowRect(&wrect);
	float fzoom1 = print_width / (float)wrect.Width();
	float fzoom2 = -print_height / (float)wrect.Height();
	if (fzoom2 < fzoom1) {
		fzoom1 = fzoom2;
	}
	pZoomScale = 0.9F * fzoom1 * (float)zoomScale;
}

void CNetDesignerView::DrawGrid(CDC* pDC)
{
	CNetDesignerDoc* pDoc = GetDocument();
	CRect crect;
	pDC->GetClipBox(crect);
	CRect wrect;
	GetWindowRect(&wrect);
	CBrush brush;
	brush.CreateSolidBrush(clr_back);
	pDC->FillRect(&crect, &brush);
	if (pDoc->guide_spacing != _T("None")) {
		float ngrid = zoomScale * (float)_tstof((LPCTSTR)pDoc->guide_spacing);
		float tgrid = zoomScale * float(ngrid);
		while (tgrid < 16) {
			ngrid = 2 * ngrid;
			tgrid = zoomScale * float(ngrid);
		}
		 
		int dx1 = -dHorzScollPos;
		int nx1 = (int)((float)dx1 / ngrid);
		int sx1 = dx1 - (int)((float)nx1 * ngrid);
		float fnx1 = (float)dx1 / ngrid;
		float fsx1 = (float)dx1 - ((float)nx1 * ngrid);

		int dx2 = wrect.Width();
		int nx2 = (int)((float)dx2 / ngrid);
		float fnx2 = (float)dx2 / ngrid;
		float fsx2 = fsx1 + ((float)nx2 * ngrid);

		int dy1 = -dVertScollPos;
		int ny1 = (int)((float)dy1 / ngrid);
		int sy1 = dy1 - (int)((float)ny1 * ngrid);
		float fny1 = (float)dy1 / ngrid;
		float fsy1 = (float)dy1 - ((float)ny1 * ngrid);

		int dy2 = wrect.Height();
		int ny2 = (int)((float)dy2 / ngrid);
		float fny2 = (float)dy2 / ngrid;
		float fsy2 = fsy1 + ((float)ny2 * ngrid);

		float fx = fsx1;
		float fy = fsy1;
		while (fy < fsy2) {
			int y = (int)fy;
			while (fx < fsx2) {
				int x = (int)fx;
				pDC->SetPixel(x, y, clr_grid);
				pDC->SetPixel(x - 1, y, clr_grid);
				pDC->SetPixel(x + 1, y, clr_grid);
				pDC->SetPixel(x, y - 1, clr_grid);
				pDC->SetPixel(x, y + 1, clr_grid);
				fx = fx + ngrid;
			}
			fy = fy + ngrid;
			fx = fsx1;
		}

	}
	DeleteObject(brush);
}

void CNetDesignerView::DrawComponents(CDC* pDC)
{
	CNetDesignerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			DrawConnect(pDC, i, zoomScale);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			DrawNode(pDC, i, zoomScale, DRAW_NORMAL, false);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isDataSource(i)) {
			DrawDataSource(pDC, i, zoomScale);
		}
	}
	if (routing) {
		DrawArrow(route_origin.x, route_origin.y, route_angle);
	}
	drawSelectedComponents(DRAW_HIGHLIGHT);
}

void CNetDesignerView::DrawNode(CDC* pDC, int x, int y, CString type, CString func, float scale, int mode, bool gdi_not)
{
	CNetDesignerDoc* pDoc = GetDocument();
	float nradius = (float)pDoc->nradius;
	int myradius = int(scale * nradius);
	COLORREF mclr = 0x000000ff;
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	else if (mode == DRAW_INACTIVE) {
		mclr = clr_inactive;
	}
	else if (type == _T("Input")) {
		mclr = clr_input;
	}
	else if (type == _T("Hidden")) {
		mclr = clr_hiddennode1;
		if (func == _T("Relu")) {
			mclr = clr_hiddennode2;
		}
		else if (func == _T("Tanh")) {
			mclr = clr_hiddennode3;
		}
		else if (func == _T("Shockley")) {
			mclr = clr_hiddennode4;
		}
		else if (func == _T("MaxPool")) {
			mclr = clr_hiddennode5;
		}
		else if (func == _T("Multiply")) {
			mclr = clr_hiddennode6;
		}
	}
	else if (type == _T("Output")) {
		mclr = clr_outnode;
	}
	else if (type == _T("PreProcess")) {
		mclr = clr_buffer;
	}
	else {
		CString msg = _T("DrawNode color error - type = ") + type;
		addErrorString(msg);
	}

	CPoint pt = LPtoDP(CPoint(x, y));
	int l = pt.x - myradius;
	int t = pt.y - myradius;
	int r = pt.x + myradius;
	int b = pt.y + myradius;
	CRect rect;
	rect.SetRect(l, t, r, b);

	if (mode == DRAW_HIGHLIGHT) {
		rect.left = rect.left + 1;
		rect.right = rect.right + 1;
		rect.top = rect.top + 1;
		rect.bottom = rect.bottom + 1;
		drawCircle(pDC, rect, mclr, true, true);
	}
	else if (mode == DRAW_ERASE) {
		drawCircle(pDC, rect, mclr, false, gdi_not);
	}
	else {

		COLORREF clrs[10];

		int rm1 = GetRValue(mclr) - ((26 * GetRValue(mclr)) / 100);
		int gm1 = GetGValue(mclr) - ((26 * GetGValue(mclr)) / 100);
		int bm1 = GetBValue(mclr) - ((26 * GetBValue(mclr)) / 100);
		clrs[0] = RGB(rm1, gm1, bm1);

		rm1 = GetRValue(mclr) - ((18 * GetRValue(mclr)) / 100);
		gm1 = GetGValue(mclr) - ((18 * GetGValue(mclr)) / 100);
		bm1 = GetBValue(mclr) - ((18 * GetBValue(mclr)) / 100);
		clrs[1] = RGB(rm1, gm1, bm1);

		rm1 = GetRValue(mclr) - ((11 * GetRValue(mclr)) / 100);
		gm1 = GetGValue(mclr) - ((11 * GetGValue(mclr)) / 100);
		bm1 = GetBValue(mclr) - ((11 * GetBValue(mclr)) / 100);
		clrs[2] = RGB(rm1, gm1, bm1);

		rm1 = GetRValue(mclr) - ((5 * GetRValue(mclr)) / 100);
		gm1 = GetGValue(mclr) - ((5 * GetGValue(mclr)) / 100);
		bm1 = GetBValue(mclr) - ((5 * GetBValue(mclr)) / 100);
		clrs[3] = RGB(rm1, gm1, bm1);

		clrs[4] = mclr;

		rm1 = GetRValue(mclr) + ((4 * (255 - GetRValue(mclr))) / 100);
		gm1 = GetGValue(mclr) + ((4 * (255 - GetGValue(mclr))) / 100);
		bm1 = GetBValue(mclr) + ((4 * (255 - GetBValue(mclr))) / 100);
		clrs[5] = RGB(rm1, gm1, bm1);

		rm1 = GetRValue(mclr) + ((7 * (255 - GetRValue(mclr))) / 100);
		gm1 = GetGValue(mclr) + ((7 * (255 - GetGValue(mclr))) / 100);
		bm1 = GetBValue(mclr) + ((7 * (255 - GetBValue(mclr))) / 100);
		clrs[6] = RGB(rm1, gm1, bm1);

		rm1 = GetRValue(mclr) + ((9 * (255 - GetRValue(mclr))) / 100);
		gm1 = GetGValue(mclr) + ((9 * (255 - GetGValue(mclr))) / 100);
		bm1 = GetBValue(mclr) + ((9 * (255 - GetBValue(mclr))) / 100);
		clrs[7] = RGB(rm1, gm1, bm1);

		rm1 = GetRValue(mclr) + ((10 * (255 - GetRValue(mclr))) / 100);
		gm1 = GetGValue(mclr) + ((10 * (255 - GetGValue(mclr))) / 100);
		bm1 = GetBValue(mclr) + ((10 * (255 - GetBValue(mclr))) / 100);
		clrs[8] = RGB(rm1, gm1, bm1);

		if (zoomScale >= 1.0) {
			int ml, mt, mr, mb;
			for (int i = 0; i < 9; i++) {
				ml = l + ((i * 2) * myradius) / 20;
				mt = t + ((i * 0) * myradius) / 20;
				mr = r - ((i * 2) * myradius) / 20;
				mb = b - ((i * 4) * myradius) / 20;
				rect.SetRect(ml, mt, mr, mb);
				drawCircle(pDC, rect, clrs[i], false, gdi_not);
			}
		}
		else {
			int ml, mt, mr, mb;
			for (int i = 0; i < 9; i++) {
				ml = l + ((i * 2) * myradius) / 20;
				mt = t + ((i * 2) * myradius) / 20;
				mr = r - ((i * 2) * myradius) / 20;
				mb = b - ((i * 2) * myradius) / 20;
				rect.SetRect(ml, mt, mr, mb);
				drawCircle(pDC, rect, clrs[i], false, gdi_not);
			}
		}

	}

}

void CNetDesignerView::drawCircle(CDC* pDC, CRect mrect, COLORREF mcolor, bool r2_not, bool gdi_not)
{
#if (RELEASIBILITY == 1) 
	int hdc = 0;
	if (r2_not) {
		hdc = pDC->SetROP2(R2_NOT);
	}
	CPen pen(PS_NULL, 1, RGB(0x00, 0x00, 0x00));
	pDC->SelectObject(pen);
	CBrush brush1;
	brush1.CreateSolidBrush(mcolor);
	pDC->SelectObject(&brush1);
	pDC->Ellipse(mrect);
	if (r2_not) {
		pDC->SetROP2(hdc);
	}
	DeleteObject(brush1);
#else
	if (printing || r2_not || gdi_not) {
		int hdc = 0;
		if (r2_not) {
			hdc = pDC->SetROP2(R2_NOT);
		}
		CPen pen(PS_NULL, 1, RGB(0x00, 0x00, 0x00));
		pDC->SelectObject(pen);
		CBrush brush1;
		brush1.CreateSolidBrush(mcolor);
		pDC->SelectObject(&brush1);
		pDC->Ellipse(mrect);
		if (r2_not) {
			pDC->SetROP2(hdc);
		}
		DeleteObject(brush1);
	}
	else {
		int diameter = mrect.right - mrect.left;
		Gdiplus::Graphics graphics(*pDC);
		Gdiplus::Color mclr;
		mclr.SetFromCOLORREF(mcolor);
		Gdiplus::SolidBrush brush2(mclr);
		graphics.FillEllipse(&brush2, mrect.left, mrect.top, diameter, diameter);
	}
#endif
}

void CNetDesignerView::DrawNode(CDC* pDC, int index, float scale, int mode, bool gdi_not)
{
	CNetDesignerDoc* pDoc = GetDocument();
	int x = pDoc->netData.getXDisp(index);
	int y = pDoc->netData.getYDisp(index);
	CString type = pDoc->netData.getNodeMode(index, _T("Output"));
	CString func = pDoc->netData.getNodeFunction(index, _T("Sigmoid"));
	if (!pDoc->netData.getNodeTrainable(index) && (mode == DRAW_NORMAL)) {
		DrawNode(pDC, x, y, type, func, scale, DRAW_INACTIVE, gdi_not);
	}
	else {
		DrawNode(pDC, x, y, type, func, scale, mode, gdi_not);
	}
	//DrawNode(pDC, x, y, type, func, scale, mode);
}

void CNetDesignerView::DrawNode(int index, float scale, int mode, bool gdi_not)
{
	CNetDesignerDoc* pDoc = GetDocument();
	CClientDC dc(this);
	OnPrepareDC(&dc);
	DrawNode(&dc, index, scale, mode, gdi_not);
}

void CNetDesignerView::DrawConnect(CDC* pDC, CPoint pt1, CPoint pt2, float scale, int mode, bool shorten)
{
	CPoint mpt1 = pt1;
	CPoint mpt2 = pt2;
	CNetDesignerDoc* pDoc = GetDocument();
	int mywidth = int(scale * pDoc->lwidth);
	COLORREF mclr = clr_connect;
	bool r2_not = false;
	if (mode == DRAW_HIGHLIGHT) {
		r2_not = true;
		mywidth = -1;
	}
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	if (shorten) {
		float dx = (float)(pt2.x - pt1.x);
		float dy = (float)(pt2.y - pt1.y);
		float l = sqrt((dx * dx) + (dy * dy));
		float myradius = 1.12F * (float)(pDoc->nradius);
		if (l > 0) {
			int drx = (int)((myradius * dx) / l);
			int dry = (int)((myradius * dy) / l);
			mpt1 = CPoint(mpt1.x + drx, mpt1.y + dry);
			mpt2 = CPoint(mpt2.x - drx, mpt2.y - dry);
		}
	}
	drawLine(pDC, mpt1, mpt2, mywidth, mclr, r2_not);
	if ((mode != DRAW_HIGHLIGHT) && pDoc->arrows) {
		float dx = (float)(pt2.x - pt1.x);
		float dy = (float)(pt2.y - pt1.y);
		float l = sqrt((dx * dx) + (dy * dy));
		if (l > 0) {
			float theta = asin(dy / l) - (3.14159F / 2);
			if ((dy > 0) && (dx > 0)) {
				theta = -theta;
			}
			else if ((dy < 0) && (dx > 0)) {
				theta = -theta;
			}
			if ((dy == 0) && (dx > 0)) {
				theta = theta + 3.14159F;

			}
			float theta1 = theta + ((16 * 3.14159F) / 180);
			float theta2 = theta - ((16 * 3.14159F) / 180);
			float myradius = 0.5F * (float)(pDoc->nradius);
			int drx = (int)(myradius * sin(theta1));
			int dry = (int)(myradius * cos(theta1));
			CPoint spt1 = CPoint(mpt2.x - drx, mpt2.y - dry);
			drawLine(pDC, spt1, mpt2, mywidth, mclr);
			drx = (int)(myradius * sin(theta2));
			dry = (int)(myradius * cos(theta2));
			spt1 = CPoint(mpt2.x - drx, mpt2.y - dry);
			drawLine(pDC, spt1, mpt2, mywidth, mclr);
		}
	}
}

void CNetDesignerView::DrawConnect(CDC* pDC, int index, float scale, int mode, bool shorten)
{
	CNetDesignerDoc* pDoc = GetDocument();
	CPoint pt1 = pDoc->netData.getInputNodePoint(index);
	CPoint pt2 = pDoc->netData.getOutputNodePoint(index);
	DrawConnect(pDC, pt1, pt2, scale, mode, shorten);
}

void CNetDesignerView::drawLine(CDC* pDC, CPoint pt1, CPoint pt2, int width, COLORREF mcolor, bool r2_not)
{
	int mywidth = width;
#if (RELEASIBILITY == 1) 
	int hdc = 0;
	if (r2_not) {
		mywidth = -1;
		hdc = pDC->SetROP2(R2_NOT);
	}
	CPen pen(PS_SOLID, mywidth, mcolor);
	pDC->SelectObject(pen);
	pDC->MoveTo(LPtoDP(pt1));
	pDC->LineTo(LPtoDP(pt2));
	if (r2_not) {
		pDC->SetROP2(hdc);
	}
#else
	//if (printing || r2_not) {
		int hdc = 0;
		if (r2_not) {
			mywidth = -1;
			hdc = pDC->SetROP2(R2_NOT);
		}
		CPen pen(PS_SOLID, mywidth, mcolor);
		pDC->SelectObject(pen);
		pDC->MoveTo(LPtoDP(pt1));
		pDC->LineTo(LPtoDP(pt2));
		if (r2_not) {
			pDC->SetROP2(hdc);
		}
	/*}   // mod here to fix of leaving little dots
	else {
		CPoint mpt1 = LPtoDP(pt1);
		CPoint mpt2 = LPtoDP(pt2);
		Gdiplus::Graphics graphics(*pDC);
		Gdiplus::Color mclr;
		mclr.SetFromCOLORREF(mcolor);
		Gdiplus::Pen pen2(mclr, (Gdiplus::REAL)mywidth);
		graphics.DrawLine(&pen2, (INT)mpt1.x, (INT)mpt1.y, (INT)mpt2.x, (INT)mpt2.y);
	}*/
#endif
}

void CNetDesignerView::DrawConnect(int index, float scale, int mode, bool shorten)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	DrawConnect(&dc, index, scale, mode, shorten);
}

void CNetDesignerView::DrawDataSource(CDC* pDC, int x, int y, CString filepath, float scale, int mode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	float w1 = (float)pDoc->dwidth;
	float h1 = w1 * 11.0F / 8.5F;
	int myw1 = int(scale * w1);
	int myh1 = int(scale * h1);
	float nwidth = 2 * (float)pDoc->lwidth;
	int mywidth = int(scale * nwidth);
	int mfont = int(8 * scale);
	CPen pen(PS_SOLID, mywidth, clr_connect);
	CPen pen2(PS_SOLID, mywidth, clr_input);
	CPen pen3(PS_SOLID, mywidth, clr_outnode);
	pDC->SelectObject(pen);
	COLORREF mclr = clr_input;
	CBrush brush1;
	brush1.CreateSolidBrush(clr_connect);
	CBrush brush2;
	brush2.CreateSolidBrush(clr_back);
	CPoint pt = LPtoDP(CPoint(x, y));
	CRect rect;

	if (mode == DRAW_ERASE) {
		int l = pt.x - myw1 / 2;
		int r = pt.x + myw1 / 2;
		int t = pt.y - myh1 / 2;
		int b = pt.y + myh1 / 2;
		rect.SetRect(l, t, r, b);
		pDC->FillRect(rect, &brush2);
		CPen pen4(PS_SOLID, mywidth, clr_back);
		pDC->SelectObject(pen4);
		pDC->MoveTo(l, t);
		pDC->LineTo(r, t);
		pDC->LineTo(r, b);
		pDC->LineTo(l, b);
		pDC->LineTo(l, t);
	}
	else {
		int hdc = 0;
		if (mode == DRAW_HIGHLIGHT) {
			hdc = pDC->SetROP2(R2_NOT);
		}
		//brush1.CreateSolidBrush(clr_connect);
		int dl = 0;
		int dr = 0;
		int dy = 0;
		int div = 12;
		int div2 = 13;
		int l = pt.x - myw1 / 2;
		int r = pt.x + myw1 / 2;
		int t = pt.y - myh1 / 2;
		int b = pt.y + myh1 / 2;
		int dw = (r - l - mywidth) / div;

		pDC->MoveTo(l, t);
		pDC->LineTo(r, t);
		pDC->LineTo(r, b);
		pDC->LineTo(l, b);
		pDC->LineTo(l, t);
		for (int i = 0; i < 6; i++) {
			dy = t + ((2 + (i * 2)) * (b - t - mywidth) / div2);
			pDC->SelectObject(pen2);
			for (int j = 0; j < 3; j++) {
				dl = l + ((2 + (j * 2)) * (r - l - mywidth) / div);
				dr = dl + dw;
				pDC->MoveTo(dl, dy);
				pDC->LineTo(dr, dy);
			}
			pDC->SelectObject(pen3);
			for (int j = 3; j < 5; j++) {
				dl = l + ((2 + (j * 2)) * (r - l - mywidth) / div);
				dr = dl + dw;
				pDC->MoveTo(dl, dy);
				pDC->LineTo(dr, dy);
			}
		}

		if (mode != DRAW_HIGHLIGHT) {

			rect.SetRect(l, t, l + (int)(35 * scale), t + (int)(18 * scale));
			pDC->FillRect(rect, &brush1);

			l = pt.x - myw1 / 2 + 4;
			t = pt.y - myh1 / 2;

			LOGFONT lf;
			memset(&lf, 0, sizeof(lf));
			lf.lfHeight = -MulDiv(mfont, pDC->GetDeviceCaps(LOGPIXELSY), 72);
			lf.lfWeight = FW_BOLD;
			lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;

			CFont newFont;
			if (!newFont.CreateFontIndirect(&lf))
				return;
			CFont* pOldFont = pDC->SelectObject(&newFont);
			pDC->SetTextColor(clr_text);
			pDC->SetBkMode(TRANSPARENT);
			pDC->TextOut(l, t, _T("CSV "));
			pDC->SelectObject(pOldFont);
			DeleteObject(newFont);

		}

		if (mode == DRAW_HIGHLIGHT) {
			pDC->SetROP2(hdc);
		}
	}

	DeleteObject(brush1);
	DeleteObject(brush2);

}

void CNetDesignerView::DrawDataSource(CDC* pDC, int index, float scale, int mode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	int x = pDoc->netData.getXDisp(index);
	int y = pDoc->netData.getYDisp(index);
	CString path = pDoc->netData.getFilePath(index);
	DrawDataSource(pDC, x, y, path, scale, mode);
}

void CNetDesignerView::DrawDataSource(int index, float scale, int mode)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	DrawDataSource(&dc, index, scale, mode);
}

void CNetDesignerView::DrawArrow(CDC* pDC, int x, int y, float angle)
{
	CPoint mpt(x, y);
	CPoint mpts[8];
	CPoint mpts2[8];
	int a = 20;
	int b = 30;
	mpts[0].x = mpt.x + 2 * a;
	mpts[0].y = mpt.y;
	mpts[0] = rotatePoint(mpt, mpts[0], angle);
	mpts[1].x = mpt.x + 1 * a;
	mpts[1].y = mpt.y + 2 * b / 3;
	mpts[1] = rotatePoint(mpt, mpts[1], angle);
	mpts[2].x = mpt.x + 1 * a;
	mpts[2].y = mpt.y + b / 3;
	mpts[2] = rotatePoint(mpt, mpts[2], angle);
	mpts[3].x = mpt.x - 2 * a;
	mpts[3].y = mpt.y + b / 3;
	mpts[3] = rotatePoint(mpt, mpts[3], angle);
	mpts[4].x = mpt.x - 2 * a;
	mpts[4].y = mpt.y - b / 3;
	mpts[4] = rotatePoint(mpt, mpts[4], angle);
	mpts[5].x = mpt.x + 1 * a;
	mpts[5].y = mpt.y - b / 3;
	mpts[5] = rotatePoint(mpt, mpts[5], angle);
	mpts[6].x = mpt.x + 1 * a;
	mpts[6].y = mpt.y - 2 * b / 3;
	mpts[6] = rotatePoint(mpt, mpts[6], angle);

	mpts[0] = LPtoDP(mpts[0]);
	mpts[1] = LPtoDP(mpts[1]);
	mpts[2] = LPtoDP(mpts[2]);
	mpts[3] = LPtoDP(mpts[3]);
	mpts[4] = LPtoDP(mpts[4]);
	mpts[5] = LPtoDP(mpts[5]);
	mpts[6] = LPtoDP(mpts[6]);

	CRgn mrgn;
	mrgn.CreatePolygonRgn(mpts, 7, WINDING);

	//int hdc = pDC->SetROP2(R2_NOT);
	int hdc = pDC->SetROP2(R2_XORPEN);

	CBrush brush1;
	brush1.CreateSolidBrush(aliceblue);
	pDC->FillRgn(&mrgn, &brush1);
	
	pDC->SetROP2(hdc);

	DeleteObject(brush1);
}

void CNetDesignerView::DrawArrow(int x, int y, float angle)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	DrawArrow(&dc, x, y, angle);
}

void CNetDesignerView::updateArrow(CPoint dppoint)
{
	CPoint topoint = DPtoLP(dppoint);
	float dx = (float)(topoint.x - route_origin.x);
	float dy = (float)(topoint.y - route_origin.y);
	float new_angle = (float)(180 * atan2f(dy, dx) / M_PI);
	float dangle = new_angle - route_angle;
	if (dangle < 0) {
		dangle = -dangle;
	}
	if (dangle >= 1) {
		DrawArrow(route_origin.x, route_origin.y, route_angle);
		route_angle = new_angle;
		DrawArrow(route_origin.x, route_origin.y, route_angle);
	}
}

CPoint CNetDesignerView::rotatePoint(CPoint origin, CPoint point, float angle)
{
	CPoint rval(point.x, point.y);
	if (angle != 0) {
		float x = (float)(point.x - origin.x);
		float y = (float)(point.y - origin.y);
		rval.x = (int)((x * cos(M_PI * angle / 180)) - (y * sin(M_PI * angle / 180))) + origin.x;
		rval.y = (int)((x * sin(M_PI * angle / 180)) + (y * cos(M_PI * angle / 180))) + origin.y;
	}
	return(rval);
}

// CNetDesignerView edit functions

bool CNetDesignerView::checkValidSelection(CPoint point)
{
	if (selectedComponents.size() == 1) {
		CNetDesignerDoc* pDoc = GetDocument();
		CPoint mpt = DPtoLP(point);
		int pindex = pDoc->checkComponentPlacement(mpt.x, mpt.y, selection_type);
		if (pindex == selectedComponents[0]) {
			return(true);
		}
	}
	else if (selectedComponents.size() > 1) {
		CPoint mpt = DPtoLP(point);
		CRect mrect = getSelectedComponentsLogicalRectangle();
		if ((mpt.x > mrect.left) && (mpt.x < mrect.right) && (mpt.y > mrect.top) && (mpt.y < mrect.bottom)) {
			return(true);
		}
	}
	return(false);
}

bool CNetDesignerView::checkIfNodeSelected()
{
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < selectedComponents.size(); i++) {
		if (pDoc->netData.isNode(selectedComponents[i])) {
			return(true);
		}
		else if (pDoc->netData.isDataSource(selectedComponents[i])) {
			return(true);
		}
	}
	return(false);
}

int CNetDesignerView::getCenterNodeOfCopiedNodes()
{
	if (copyData.size() == 1) {
		return(0);
	}
	else if (copyData.size() > 1) {
		CNetDesignerDoc* pDoc = GetDocument();
		int minx = logicalWidth;
		int maxx = -logicalWidth;
		int miny = logicalHeight;
		int maxy = -logicalHeight;
		for (int i = 0; i < copyData.size(); i++) {
			if (copyData.isNode(i)) {
				if ((copyData.getXDisp(i) + pDoc->nradius) > maxx) {
					maxx = (copyData.getXDisp(i) + (int)pDoc->nradius);
				}
				if ((copyData.getXDisp(i) - pDoc->nradius) < minx) {
					minx = (copyData.getXDisp(i) - (int)pDoc->nradius);
				}
				if ((copyData.getYDisp(i) + pDoc->nradius) > maxy) {
					maxy = (copyData.getYDisp(i) + (int)pDoc->nradius);
				}
				if ((copyData.getYDisp(i) - pDoc->nradius) < miny) {
					miny = (copyData.getYDisp(i) - (int)pDoc->nradius);
				}
			}
			else if (copyData.isConnect(i)) {
				CPoint pt1 = copyData.getInputNodePoint(i);
				CPoint pt2 = copyData.getOutputNodePoint(i);
				CPoint cpt((pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2);
				if ((cpt.x + pDoc->nradius) > maxx) {
					maxx = (cpt.x + (int)pDoc->nradius);
				}
				if ((cpt.x - pDoc->nradius) < minx) {
					minx = (cpt.x - (int)pDoc->nradius);
				}
				if ((cpt.y + pDoc->nradius) > maxy) {
					maxy = (cpt.y + (int)pDoc->nradius);
				}
				if ((cpt.y - pDoc->nradius) < miny) {
					miny = (cpt.y - (int)pDoc->nradius);
				}
			}
			else if (copyData.isDataSource(i)) {
				if ((copyData.getXDisp(i) + pDoc->dwidth / 2) > maxx) {
					maxx = (copyData.getXDisp(i) + pDoc->dwidth / 2);
				}
				if ((copyData.getXDisp(i) - pDoc->dwidth / 2) < minx) {
					minx = (copyData.getXDisp(i) - pDoc->dwidth / 2);
				}
				if ((copyData.getYDisp(i) + pDoc->dwidth / 2) > maxy) {
					maxy = (copyData.getYDisp(i) + pDoc->dwidth / 2);
				}
				if ((copyData.getYDisp(i) - pDoc->dwidth / 2) < miny) {
					miny = (copyData.getYDisp(i) - pDoc->dwidth / 2);
				}
			}
		}
		int centerx = (maxx + minx) / 2;
		int centery = (maxy + miny) / 2;
		int mindex = -1;
		float mdistance = (float)logicalWidth;
		for (int i = 0; i < copyData.size(); i++) {
			float tdistance = (float)sqrt(pow((float)(centerx - copyData.getXDisp(i)),2) + pow((float)(centery - copyData.getYDisp(i)), 2));
			if (tdistance < mdistance) {
				mdistance = tdistance;
				mindex = i;
			}
		}
		return mindex;
	}
	return(-1);
}

CRect CNetDesignerView::getSelectedComponentsLogicalRectangle(bool nodes_only)
{
	CNetDesignerDoc* pDoc = GetDocument();
	CRect rval(-(int)pDoc->nradius,-(int)pDoc->nradius, (int)pDoc->nradius, (int)pDoc->nradius);
	if (selectedComponents.size() == 1) {
		if (pDoc->netData.size() >= selectedComponents[0]) {
			rval.left = pDoc->netData.getXDisp(selectedComponents[0]) - (int)pDoc->nradius;
			rval.right = pDoc->netData.getXDisp(selectedComponents[0]) + (int)pDoc->nradius;
			rval.top = pDoc->netData.getYDisp(selectedComponents[0]) - (int)pDoc->nradius;
			rval.bottom = pDoc->netData.getYDisp(selectedComponents[0]) + (int)pDoc->nradius;
		}
	}
	else if (selectedComponents.size() > 1) {
		rval.left = logicalWidth;
		rval.right = -logicalWidth;
		rval.top = logicalHeight;
		rval.bottom = -logicalHeight;
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(selectedComponents[i])) {
				if ((pDoc->netData.getXDisp(selectedComponents[i]) + (int)pDoc->nradius) > rval.right) {
					rval.right = (pDoc->netData.getXDisp(selectedComponents[i]) + (int)pDoc->nradius);
				}
				if ((pDoc->netData.getXDisp(selectedComponents[i]) - (int)pDoc->nradius) < rval.left) {
					rval.left = (pDoc->netData.getXDisp(selectedComponents[i]) - (int)pDoc->nradius);
				}
				if ((pDoc->netData.getYDisp(selectedComponents[i]) + (int)pDoc->nradius) > rval.bottom) {
					rval.bottom = (pDoc->netData.getYDisp(selectedComponents[i]) + (int)pDoc->nradius);
				}
				if ((pDoc->netData.getYDisp(selectedComponents[i]) - (int)pDoc->nradius) < rval.top) {
					rval.top = (pDoc->netData.getYDisp(selectedComponents[i]) - (int)pDoc->nradius);
				}
			}
			else if (pDoc->netData.isConnect(selectedComponents[i])) {
				if (!nodes_only) {
					CPoint pt1 = pDoc->netData.getInputNodePoint(i);
					CPoint pt2 = pDoc->netData.getOutputNodePoint(i);
					CPoint cpt((pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2);
					if ((cpt.x + pDoc->nradius) > rval.right) {
						rval.right = (cpt.x + (int)pDoc->nradius);
					}
					if ((cpt.x - pDoc->nradius) < rval.left) {
						rval.left = (cpt.x - (int)pDoc->nradius);
					}
					if ((cpt.y + pDoc->nradius) > rval.bottom) {
						rval.bottom = (cpt.y + (int)pDoc->nradius);
					}
					if ((cpt.y - pDoc->nradius) < rval.top) {
						rval.top = (cpt.y - (int)pDoc->nradius);
					}
				}
			}
			else if (pDoc->netData.isDataSource(selectedComponents[i])) {
				if (!nodes_only) {
					if ((pDoc->netData.getXDisp(selectedComponents[i]) + pDoc->dwidth / 2) > rval.right) {
						rval.right = (pDoc->netData.getXDisp(selectedComponents[i]) + pDoc->dwidth / 2);
					}
					if ((pDoc->netData.getXDisp(selectedComponents[i]) - pDoc->dwidth / 2) < rval.left) {
						rval.left = (pDoc->netData.getXDisp(selectedComponents[i]) - pDoc->dwidth / 2);
					}
					if ((pDoc->netData.getYDisp(selectedComponents[i]) + pDoc->dwidth / 2) > rval.bottom) {
						rval.bottom = (pDoc->netData.getYDisp(selectedComponents[i]) + pDoc->dwidth / 2);
					}
					if ((pDoc->netData.getYDisp(selectedComponents[i]) - pDoc->dwidth / 2) < rval.top) {
						rval.top = (pDoc->netData.getYDisp(selectedComponents[i]) - pDoc->dwidth / 2);
					}
				}
			}
		}
		if ((rval.right - rval.left) < 40) {
			rval.right += 20;
			rval.left -= 20;
		}
		if ((rval.bottom - rval.top) < 40) {
			rval.bottom += 20;
			rval.top -= 20;
		}
	}
	return(rval);
}

CRect CNetDesignerView::getAllComponentsLogicalRectangle()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CRect rval(-(int)pDoc->nradius, -(int)pDoc->nradius, (int)pDoc->nradius, (int)pDoc->nradius);
	if (pDoc->netData.size() == 1) {
		rval.left = pDoc->netData.getXDisp(0) - (int)pDoc->nradius;
		rval.right = pDoc->netData.getXDisp(0) + (int)pDoc->nradius;
		rval.top = pDoc->netData.getYDisp(0) - (int)pDoc->nradius;
		rval.bottom = pDoc->netData.getYDisp(0) + (int)pDoc->nradius;
	}
	else if (pDoc->netData.size() > 1) {
		rval.left = logicalWidth;
		rval.right = -logicalWidth;
		rval.top = logicalHeight;
		rval.bottom = -logicalHeight;
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isNode(i)) {
				if ((pDoc->netData.getXDisp(i) + pDoc->nradius) > rval.right) {
					rval.right = (pDoc->netData.getXDisp(i) + (int)pDoc->nradius);
				}
				if ((pDoc->netData.getXDisp(i) - pDoc->nradius) < rval.left) {
					rval.left = (pDoc->netData.getXDisp(i) - (int)pDoc->nradius);
				}
				if ((pDoc->netData.getYDisp(i) + pDoc->nradius) > rval.bottom) {
					rval.bottom = (pDoc->netData.getYDisp(i) + (int)pDoc->nradius);
				}
				if ((pDoc->netData.getYDisp(i) - pDoc->nradius) < rval.top) {
					rval.top = (pDoc->netData.getYDisp(i) - (int)pDoc->nradius);
				}
			}
			else if (pDoc->netData.isConnect(i)) {
				CPoint pt1 = pDoc->netData.getInputNodePoint(i);
				CPoint pt2 = pDoc->netData.getOutputNodePoint(i);
				CPoint cpt((pt1.x + pt2.x) / 2, (pt1.y + pt2.y) / 2);
				if ((cpt.x + pDoc->nradius) > rval.right) {
					rval.right = (cpt.x + (int)pDoc->nradius);
				}
				if ((cpt.x - pDoc->nradius) < rval.left) {
					rval.left = (cpt.x - (int)pDoc->nradius);
				}
				if ((cpt.y + pDoc->nradius) > rval.bottom) {
					rval.bottom = (cpt.y + (int)pDoc->nradius);
				}
				if ((cpt.y - pDoc->nradius) < rval.top) {
					rval.top = (cpt.y - (int)pDoc->nradius);
				}
			}
			else if (pDoc->netData.isDataSource(i)) {
				if ((pDoc->netData.getXDisp(i) + pDoc->dwidth / 2) > rval.right) {
					rval.right = (pDoc->netData.getXDisp(i) + pDoc->dwidth / 2);
				}
				if ((pDoc->netData.getXDisp(i) - pDoc->dwidth / 2) < rval.left) {
					rval.left = (pDoc->netData.getXDisp(i) - pDoc->dwidth / 2);
				}
				if ((pDoc->netData.getYDisp(i) + pDoc->dwidth / 2) > rval.bottom) {
					rval.bottom = (pDoc->netData.getYDisp(i) + pDoc->dwidth / 2);
				}
				if ((pDoc->netData.getYDisp(i) - pDoc->dwidth / 2) < rval.top) {
					rval.top = (pDoc->netData.getYDisp(i) - pDoc->dwidth / 2);
				}
			}
		}
		if ((rval.right - rval.left) < 40) {
			rval.right += 20;
			rval.left -= 20;
		}
		if ((rval.bottom - rval.top) < 40) {
			rval.bottom += 20;
			rval.top -= 20;
		}
	}
	return(rval);
}

void CNetDesignerView::drawSelectedComponents(int mode)
{
	if (selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		for (int i = 0; i < selectedComponents.size(); i++) {
			int index = selectedComponents[i];
			if (pDoc->netData.isNode(index)) {
				DrawNode(index, zoomScale, mode, false);
			}
			else if (pDoc->netData.isConnect(index)) {
				DrawConnect(index, zoomScale, mode);
			}
			else if (pDoc->netData.isDataSource(index)) {
				DrawDataSource(index, zoomScale, mode);
			}
		}
	}
}

void CNetDesignerView::moveSelectedItems(CPoint frompt, CPoint topt)
{
	if (selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		CClientDC dc(this);
		OnPrepareDC(&dc);
		int x1, y1, x2, y2;
		CString type;
		CString func;
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(selectedComponents[i])) {
				type = pDoc->netData.getNodeMode(selectedComponents[i]);
				func = pDoc->netData.getNodeFunction(selectedComponents[i]);
				x1 = applyGridX(pDoc->netData.getXDisp(selectedComponents[i]) + frompt.x);
				y1 = applyGridY(pDoc->netData.getYDisp(selectedComponents[i]) + frompt.y);
				x2 = applyGridX(pDoc->netData.getXDisp(selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(selectedComponents[i]) + topt.y);
				DrawNode(&dc, x1, y1, type, func, zoomScale, DRAW_HIGHLIGHT, false);
				DrawNode(&dc, x2, y2, type, func, zoomScale, DRAW_HIGHLIGHT, false);
			}
			else if (pDoc->netData.isConnect(selectedComponents[i])) {
				bool fixed_in = true;
				bool fixed_out = true;
				CString in_node = pDoc->netData.getInputNode(selectedComponents[i]);
				CString out_node = pDoc->netData.getOutputNode(selectedComponents[i]);
				for (int j = 0; j < selectedComponents.size(); j++) {
					if (pDoc->netData.isNode(selectedComponents[j])) {
						if (in_node == pDoc->netData.getID(selectedComponents[j])) {
							fixed_in = false;
						}
						if (out_node == pDoc->netData.getID(selectedComponents[j])) {
							fixed_out = false;
						}
						if (!fixed_in && !fixed_out) {
							j = (int)selectedComponents.size();
						}
					}
				}
				CPoint pt1 = pDoc->netData.getInputNodePoint(selectedComponents[i]);
				CPoint pt2 = pDoc->netData.getOutputNodePoint(selectedComponents[i]);
				CPoint mpt1(pt1.x, pt1.y);
				CPoint mpt2(pt2.x, pt2.y);
				CPoint mpt3(pt1.x, pt1.y);
				CPoint mpt4(pt2.x, pt2.y);
				if (!fixed_in) {
					mpt1 = CPoint(pt1.x + frompt.x, pt1.y + frompt.y);
					mpt3 = CPoint(pt1.x + topt.x, pt1.y + topt.y);
				}
				if (!fixed_out) {
					mpt2 = CPoint(pt2.x + frompt.x, pt2.y + frompt.y);
					mpt4 = CPoint(pt2.x + topt.x, pt2.y + topt.y);
				}
				DrawConnect(&dc, applyGrid(mpt1), applyGrid(mpt2), zoomScale, DRAW_HIGHLIGHT);
				DrawConnect(&dc, applyGrid(mpt3), applyGrid(mpt4), zoomScale, DRAW_HIGHLIGHT);
			}
			else if (pDoc->netData.isDataSource(selectedComponents[i])) {
				type = pDoc->netData.getDataMode(selectedComponents[i]);
				x1 = applyGridX(pDoc->netData.getXDisp(selectedComponents[i]) + frompt.x);
				y1 = applyGridY(pDoc->netData.getYDisp(selectedComponents[i]) + frompt.y);
				x2 = applyGridX(pDoc->netData.getXDisp(selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(selectedComponents[i]) + topt.y);
				DrawDataSource(&dc, x1, y1, type, zoomScale, DRAW_HIGHLIGHT);
				DrawDataSource(&dc, x2, y2, type, zoomScale, DRAW_HIGHLIGHT);
			}
		}
	}
}

void CNetDesignerView::finalMoveSelectedItems(CPoint topt)
{
	if (selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		CClientDC dc(this);
		OnPrepareDC(&dc);
		int x, y;
		CString type;
		CString func;
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(selectedComponents[i])) {
				type = pDoc->netData.getNodeMode(selectedComponents[i]);
				func = pDoc->netData.getNodeFunction(selectedComponents[i]);
				x = applyGridX(pDoc->netData.getXDisp(selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(selectedComponents[i]) + topt.y);
				DrawNode(&dc, x, y, type, func, zoomScale, DRAW_HIGHLIGHT, false);
			}
			else if (pDoc->netData.isConnect(selectedComponents[i])) {
				bool fixed_in = true;
				bool fixed_out = true;
				CString in_node = pDoc->netData.getInputNode(selectedComponents[i]);
				CString out_node = pDoc->netData.getOutputNode(selectedComponents[i]);
				for (int j = 0; j < selectedComponents.size(); j++) {
					if (pDoc->netData.isNode(selectedComponents[j])) {
						if (in_node == pDoc->netData.getID(selectedComponents[j])) {
							fixed_in = false;
						}
						if (out_node == pDoc->netData.getID(selectedComponents[j])) {
							fixed_out = false;
						}
						if (!fixed_in && !fixed_out) {
							j = (int)selectedComponents.size();
						}
					}
				}
				CPoint pt1 = pDoc->netData.getInputNodePoint(selectedComponents[i]);
				CPoint pt2 = pDoc->netData.getOutputNodePoint(selectedComponents[i]);
				CPoint mpt3(pt1.x, pt1.y);
				CPoint mpt4(pt2.x, pt2.y);
				if (!fixed_in) {
					mpt3 = CPoint(pt1.x + topt.x, pt1.y + topt.y);
				}
				if (!fixed_out) {
					mpt4 = CPoint(pt2.x + topt.x, pt2.y + topt.y);
				}
				DrawConnect(&dc, applyGrid(mpt3), applyGrid(mpt4), zoomScale, DRAW_HIGHLIGHT);
			}
			else if (pDoc->netData.isDataSource(selectedComponents[i])) {
				type = pDoc->netData.getDataMode(selectedComponents[i]);
				x = applyGridX(pDoc->netData.getXDisp(selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(selectedComponents[i]) + topt.y);
				DrawDataSource(&dc, x, y, type, zoomScale, DRAW_HIGHLIGHT);
			}
		}
	}
}

void CNetDesignerView::drawCopiedComponents(CPoint spt, int snode, int mode)
{
	if (copyData.size() > 0) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		CPoint mpt = DPtoLP(spt);
		mpt = applyGrid(mpt);
		int x = mpt.x - copyData.getXDisp(snode);
		int y = mpt.y - copyData.getYDisp(snode);
		CNetDesignerDoc* pDoc = GetDocument();
		for (int i = 0; i < copyData.size(); i++) {
			if (copyData.isNode(i)) {
				if (!copyData.getNodeTrainable(i) && (mode == DRAW_NORMAL)) {
					DrawNode(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, copyData.getNodeMode(i), copyData.getNodeFunction(i), zoomScale, DRAW_INACTIVE, false);
				}
				else {
					DrawNode(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, copyData.getNodeMode(i), copyData.getNodeFunction(i), zoomScale, mode, false);
				}
			}
			else if (copyData.isConnect(i)) {
				CPoint pt1 = copyData.getInputNodePoint(i);
				CPoint pt2 = copyData.getOutputNodePoint(i);
				CPoint mpt3(pt1.x + x, pt1.y + y);
				CPoint mpt4(pt2.x + x, pt2.y + y);
				DrawConnect(&dc, mpt3, mpt4, zoomScale, mode);
			}
			else if (copyData.isDataSource(i) && !pDoc->dataSourceTrue) {
				DrawDataSource(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, copyData.getDataMode(i), zoomScale, mode);
			}
		}
	}
}

void CNetDesignerView::drawNewGroupComponents(CPoint spt, int snode, int mode)
{
	if (newGroupData.size() > 0) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		CPoint mpt = DPtoLP(spt);
		mpt = applyGrid(mpt);
		int x = mpt.x - newGroupData.getXDisp(snode);
		int y = mpt.y - newGroupData.getYDisp(snode);
		CNetDesignerDoc* pDoc = GetDocument();
		for (int i = 0; i < newGroupData.size(); i++) {
			if (newGroupData.isNode(i)) {
				//CString s = _T("");
				//s.Format(_T("%i : %i"), newGroupData.getXDisp(i) + x, newGroupData.getYDisp(i) + y);
				//addErrorString(s);
				DrawNode(&dc, newGroupData.getXDisp(i) + x, newGroupData.getYDisp(i) + y, newGroupData.getNodeMode(i), newGroupData.getNodeFunction(i), zoomScale, mode, false);
			}
			else if (newGroupData.isConnect(i)) {
				CPoint pt1 = newGroupData.getInputNodePoint(i);
				CPoint pt2 = newGroupData.getOutputNodePoint(i);
				CPoint mpt3(pt1.x + x, pt1.y + y);
				CPoint mpt4(pt2.x + x, pt2.y + y);
				DrawConnect(&dc, mpt3, mpt4, zoomScale, mode);
			}
		}
	}
}


void CNetDesignerView::moveCopiedItems(int snode, CPoint frompt, CPoint topt)
{
	drawCopiedComponents(frompt, snode, DRAW_HIGHLIGHT);
	drawCopiedComponents(topt, snode, DRAW_HIGHLIGHT);
}

void CNetDesignerView::moveNewGroupItems(int snode, CPoint frompt, CPoint topt)
{
	drawNewGroupComponents(frompt, snode, DRAW_HIGHLIGHT);
	drawNewGroupComponents(topt, snode, DRAW_HIGHLIGHT);
}

bool CNetDesignerView::selectComponent(CPoint point, bool add)
{
	if (!add) {
		endSelect();
	}
	CNetDesignerDoc* pDoc = GetDocument();
	CPoint mpt = point;
	mpt = DPtoLP(mpt);
	int pindex = pDoc->checkComponentPlacement(mpt.x,mpt. y, selection_type);
	if (pindex >= 0) {
		if (pDoc->netData.isNode(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			selectedComponents.push_back(pindex);
			//DrawNode(pindex, zoomScale, DRAW_ERASE, false);
			//DrawNode(pindex, zoomScale, DRAW_NORMAL, true);
			DrawNode(pindex, zoomScale, DRAW_HIGHLIGHT, false);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
			return(true);
		}
		else if (pDoc->netData.isConnect(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_CONNECTIONS))) {
			selectedComponents.push_back(pindex);
			DrawConnect(pindex, zoomScale, DRAW_ERASE);
			DrawConnect(pindex, zoomScale, DRAW_HIGHLIGHT);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
			return(true);
		}
		else if (pDoc->netData.isDataSource(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_OTHER))) {
			selectedComponents.push_back(pindex);
			DrawDataSource(pindex, zoomScale, DRAW_HIGHLIGHT);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
			return(true);
		}
	}
	return(false);
}

bool CNetDesignerView::selectComponent(int index, bool add)
{
	if (!add) {
		endSelect();
	}
	CNetDesignerDoc* pDoc = GetDocument();
	if (index >= 0) {
		if (pDoc->netData.isNode(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			selectedComponents.push_back(index);
			//DrawNode(index, zoomScale, DRAW_ERASE, false);
			//DrawNode(index, zoomScale, DRAW_NORMAL, true);
			DrawNode(index, zoomScale, DRAW_HIGHLIGHT, false);
			return(true);
		}
		else if (pDoc->netData.isConnect(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_CONNECTIONS))) {
			selectedComponents.push_back(index);
			DrawConnect(index, zoomScale, DRAW_ERASE);
			DrawConnect(index, zoomScale, DRAW_HIGHLIGHT);
			return(true);
		}
		else if (pDoc->netData.isDataSource(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_OTHER))) {
			selectedComponents.push_back(index);
			DrawDataSource(index, zoomScale, DRAW_HIGHLIGHT);
			return(true);
		}
	}
	return(false);
}

void CNetDesignerView::selectComponent(CString label)
{
	endSelect();
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.getID(i) == label) {
			selectedComponents.push_back(i);
			if (pDoc->netData.isNode(i)) {
				//DrawNode(i, zoomScale, DRAW_ERASE, false);
				//DrawNode(i, zoomScale, DRAW_NORMAL, true);
				DrawNode(i, zoomScale, DRAW_HIGHLIGHT, false);
				CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
				pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
			}
			else if (pDoc->netData.isConnect(i)) {
				DrawConnect(i, zoomScale, DRAW_ERASE);
				DrawConnect(i, zoomScale, DRAW_HIGHLIGHT);
				CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
				pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
			}
			else if (pDoc->netData.isDataSource(i)) {
				DrawDataSource(i, zoomScale, DRAW_HIGHLIGHT);
				CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
				pMainWnd->m_wndProperties.InitPropList(selectedComponents, pDoc->netData, pDoc->trainData.dataSourceHeaders);
			}
			searchItemActive = true;
			return;
		}
	}
}

void CNetDesignerView::endSelect()
{
	if (selectedComponents.size() > 0) {
		updateProperties();
		CNetDesignerDoc* pDoc = GetDocument();
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(selectedComponents[i])) {
				//DrawNode(selectedComponents[i], zoomScale, DRAW_HIGHLIGHT, false);
				DrawNode(selectedComponents[i], zoomScale, DRAW_NORMAL, false);
			}
			else if (pDoc->netData.isConnect(selectedComponents[i])) {
				//DrawConnect(selectedComponents[i], zoomScale, DRAW_HIGHLIGHT, false);
				DrawConnect(selectedComponents[i], zoomScale, DRAW_NORMAL);
			}
			else if (pDoc->netData.isDataSource(selectedComponents[i])) {
				DrawDataSource(selectedComponents[i], zoomScale, DRAW_NORMAL);
			}
		}
		selectedComponents.clear();
	}
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndProperties.InitPropListDefault();
	searchItemActive = false;
}

void CNetDesignerView::updateProperties()
{	
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();

	if (pMainWnd->m_wndProperties.displayType == NODE) {

		CString multiple = pMainWnd->m_wndProperties.multiple;

		CString plbl = pMainWnd->m_wndProperties.getLabel();
		CString ptype = pMainWnd->m_wndProperties.getType();
		CString pfunc = pMainWnd->m_wndProperties.getFunction();
		CString peta = pMainWnd->m_wndProperties.getEta();
		CString palpha = pMainWnd->m_wndProperties.getAlpha();
		CString prand = pMainWnd->m_wndProperties.getRand();
		CString pdata = pMainWnd->m_wndProperties.getDataField();
		CString pindexmode = pMainWnd->m_wndProperties.getDataIndexType();
		int pindex1 = pMainWnd->m_wndProperties.getIndex1();
		int pindex2 = pMainWnd->m_wndProperties.getIndex2();

		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);

		for (int i = 0; i < selectedComponents.size(); i++) {
			int index = selectedComponents[i];
			netDataStruct tdata = pDoc->netData.getNode(index);
			if (pDoc->netData.isNode(index)) {
				if (pMainWnd->m_wndProperties.edit_names) {
					if (plbl != multiple) {
						tdata.setLabel(plbl);
					}
				}
				if (pMainWnd->m_wndProperties.display_functions) {
					if (pfunc != multiple) {
						tdata.setNodeFunction(pfunc);
						pDoc->dfuncn = pfunc;
					}
					if (peta != multiple) {
						tdata.setNodeEta(_tstof((LPCTSTR)peta));
						pDoc->deta = _tstof((LPCTSTR)peta);
					}
					if (palpha != multiple) {
						tdata.setNodeAlpha(_tstof((LPCTSTR)palpha));
						pDoc->dalpha = _tstof((LPCTSTR)palpha);
					}
					if (prand != multiple) {
						tdata.setNodeRand(_tstof((LPCTSTR)prand));
						pDoc->drand = _tstof((LPCTSTR)prand);
					}
				}
				if (pMainWnd->m_wndProperties.display_data_sources) {
					if (pdata != multiple) {
						tdata.setDataField(pdata);
					}
				}
				if (pMainWnd->m_wndProperties.display_input_index) {
					if (pindexmode != multiple) {
						if (pindexmode == _T("Index")) {
							/*if (pDoc->netData.getNodeMode(index) == _T("Output")) {
								if (pindex1 < 0) {
									AfxMessageBox(_T("Time series data indices for output nodes must be greater than or equal to zero"));
									pindex1 = 0;
								}
							}
							else */if (pDoc->netData.getNodeMode(index) == _T("Input")) {
								if (pindex1 > 0) {
									AfxMessageBox(_T("Time series data indices for input nodes must be less than or equal to zero"));
									pindex1 = 0;
								}
							}
						}
						tdata.setNodeIndexMode(pindexmode);
						tdata.setNodeXIndex(pindex1);
						tdata.setNodeYIndex(pindex2);
					}
				}
				pDoc->pushEditNodeData(tdata);
			}
		}

		if (pDoc->processEditData()) {
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
		}

	}

}

void CNetDesignerView::updateMovedProperties(CPoint frompt, CPoint topt)
{
	int dx = (int)((float)(topt.x - frompt.x) / zoomScale);
	int dy = (int)((float)(topt.y - frompt.y) / zoomScale);
	CNetDesignerDoc* pDoc = GetDocument();
	pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
	for (int i = 0; i < selectedComponents.size(); i++) {
		int index = selectedComponents[i];
		if (pDoc->netData.isNode(index)) {
			netDataStruct tdata = pDoc->netData.getNode(index);
			tdata.setXDisp(applyGridX(pDoc->netData.getXDisp(index) + dx));
			tdata.setYDisp(applyGridY(pDoc->netData.getYDisp(index) + dy));
			pDoc->pushEditNodeData(tdata);
		}
		else if (pDoc->netData.isConnect(index)) {
			CString sid = pDoc->netData.getID(index);
			CString slbl = pDoc->netData.getLabel(index);
			CString node_in = pDoc->netData.getInputNode(index);
			CString node_out = pDoc->netData.getOutputNode(index);
			vector<CPoint> points = pDoc->netData.getPoints(index);
			double sweight = pDoc->netData.getWeight(index);
			for (int i = 0; i < points.size(); i++) {
				points[i] = LPtoDP(applyGrid(DPtoLP(CPoint(points[i].x + dx, points[i].y + dy))));
			}
			pDoc->pushEditConnectData(sid, slbl, node_in, node_out, sweight, points);
		}
		else if (pDoc->netData.isDataSource(index)) {
			CString sid = pDoc->netData.getID(index);
			CString slbl = pDoc->netData.getLabel(index);
			CString smode = pDoc->netData.getDataMode(index);
			vector<CString> spaths = pDoc->netData.getFilePaths(index);
			int sx = applyGridX(pDoc->netData.getXDisp(index) + dx);
			int sy = applyGridY(pDoc->netData.getYDisp(index) + dy);
			pDoc->pushEditDataSource(sid, slbl, sx, sy, smode, spaths);
		}
	}
	if (pDoc->processEditData()) {
		//CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		//pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
	}
}

void CNetDesignerView::updatePastedProperties(CPoint spt, int snode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	pDoc->clearAndSetEditParams(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos);
	int nindex_offset = 0;
	vector<CString> fromLabels;
	vector<CString> toLabels;
	CPoint mpt = DPtoLP(spt);
	int x = mpt.x - copyData.getXDisp(snode);
	int y = mpt.y - copyData.getYDisp(snode);
	for (int i = 0; i < copyData.size(); i++) {
		if (copyData.isNode(i)) {
			netDataStruct tdata = copyData.getNode(i);
			fromLabels.push_back(copyData.getID(i));
			CString sid = _T("");
			sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
			toLabels.push_back(sid);
			tdata.setID(sid);
			tdata.setLabel(sid);
			tdata.setXDisp(applyGridX(copyData.getXDisp(i) + x));
			tdata.setYDisp(applyGridY(copyData.getYDisp(i) + y));
			pDoc->pushEditNodeData(tdata);
			nindex_offset++;
		}
		else if (copyData.isDataSource(i) && !pDoc->dataSourceTrue) {
			fromLabels.push_back(copyData.getID(i));
			CString sid = _T("");
			sid.Format(_T("Data%i"), pDoc->getLastDataIndex() + 1 + nindex_offset);
			toLabels.push_back(sid);
			CString smode = copyData.getDataMode(i);
			vector<CString> spaths = copyData.getFilePaths(i);
			int sx = applyGridX(copyData.getXDisp(i) + x);
			int sy = applyGridY(copyData.getYDisp(i) + y);
			pDoc->pushEditDataSource(sid, sid, sx, sy, smode, spaths);
			nindex_offset++;
		}
	}
	if (fromLabels.size() != toLabels.size()) {
		AfxMessageBox(_T("updatePastedProperties : fromLabels.size() != toLabels.size()"));
		return;
	}
	nindex_offset = 0;
	for (int i = 0; i < copyData.size(); i++) {
		if (copyData.isConnect(i)) {
			CString mlabel1 = copyData.getInputNode(i);
			CString mlabel2 = copyData.getOutputNode(i);
			for (int i = 0; i < fromLabels.size(); i++) {
				if (mlabel1 == fromLabels[i]) {
					mlabel1 = toLabels[i];
					i = (int)fromLabels.size();
				}
			}
			for (int i = 0; i < fromLabels.size(); i++) {
				if (mlabel2 == fromLabels[i]) {
					mlabel2 = toLabels[i];
					i = (int)fromLabels.size();
				}
			}

			CString sid = _T("");
			sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
			CString node_in = mlabel1;
			CString node_out = mlabel2;
			vector<CPoint> points = copyData.getPoints(i);
			for (int i = 0; i < points.size(); i++) {
				points[i] = LPtoDP(applyGrid(DPtoLP(CPoint(points[i].x + x, points[i].y + y))));
			}
			double sweight = pDoc->netData.getWeight(i);
			pDoc->pushEditConnectData(sid, sid, node_in, node_out, sweight, points);
			nindex_offset++;
		}
	}
	if (pDoc->processEditData()) {
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
	}
}


void CNetDesignerView::addStragglingConnectionsToSelectedComponents()
{
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			bool cfound = false;
			bool nfound = false;
			CString clabel = pDoc->netData.getID(i);
			for (int j=0; j < selectedComponents.size(); j++) {
				if (pDoc->netData.isConnect(selectedComponents[j])) {
					if (clabel == pDoc->netData.getID(selectedComponents[j])) {
						cfound = true;
						j = (int)selectedComponents.size();
					}
				}
			}
			if (!cfound) {
				CString nlabel1 = pDoc->netData.getInputNode(i);
				CString nlabel2 = pDoc->netData.getOutputNode(i);
				for (int j = 0; j < selectedComponents.size(); j++) {
					if (pDoc->netData.isNode(selectedComponents[j])) {
						if (nlabel1 == pDoc->netData.getID(selectedComponents[j])) {
							nfound = true;
							j = (int)selectedComponents.size();
						}
						if (nlabel2 == pDoc->netData.getID(selectedComponents[j])) {
							nfound = true;
							j = (int)selectedComponents.size();
						}
					}
				}
			}
			if (nfound) {
				selectedComponents.push_back(i);
			}
		}
	}
}

void CNetDesignerView::deleteStragglingConnectionsFromSelectedComponents()
{
	CNetDesignerDoc* pDoc = GetDocument();
	vector<int> keepers;
	for (int i = 0; i < selectedComponents.size(); i++) {
		if (pDoc->netData.isConnect(selectedComponents[i])) {
			bool nfound1 = false;
			bool nfound2 = false;
			CString nlabel1 = pDoc->netData.getInputNode(selectedComponents[i]);
			CString nlabel2 = pDoc->netData.getOutputNode(selectedComponents[i]);
			for (int j = 0; j < selectedComponents.size(); j++) {
				if (nlabel1 == pDoc->netData.getID(selectedComponents[j])) {
					nfound1 = true;
				}
				if (nlabel2 == pDoc->netData.getID(selectedComponents[j])) {
					nfound2 = true;
				}
				if (nfound1 && nfound2) {
					keepers.push_back(selectedComponents[i]);
					j = (int)selectedComponents.size();
				}
			}
		}
		else {
			keepers.push_back(selectedComponents[i]);
		}
	}
	selectedComponents.clear();
	for (int i = 0; i < keepers.size(); i++) {
		selectedComponents.push_back(keepers[i]);
	}
}

void CNetDesignerView::deleteSelectedComponents()
{
	if (selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pDoc->clearAndSetEditParams(UNDO_DELETE, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < selectedComponents.size(); i++) {
			int index = selectedComponents[i];
			if (pDoc->netData.isNode(index)) {
				netDataStruct tdata = pDoc->netData.getNode(index);
				pDoc->pushEditNodeData(tdata);
				DrawNode(index, zoomScale, DRAW_ERASE, false);
			}
			else if (pDoc->netData.isConnect(index)) {
				CString sid = pDoc->netData.getID(index);
				CString slbl = pDoc->netData.getLabel(index);
				CString snode_in = pDoc->netData.getInputNode(index);
				CString snode_out = pDoc->netData.getOutputNode(index);
				vector<CPoint> points = pDoc->netData.getPoints(index);
				double sweight = pDoc->netData.getWeight(index);
				pDoc->pushEditConnectData(sid, slbl, snode_in, snode_out, sweight, points);
				DrawConnect(index, zoomScale, DRAW_ERASE);
			}
			else if (pDoc->netData.isDataSource(index)) {
				CString sid = pDoc->netData.getID(index);
				CString slbl = pDoc->netData.getLabel(index);
				CString smode = pDoc->netData.getDataMode(index);
				vector<CString> spaths = pDoc->netData.getFilePaths(index);
				int sx = pDoc->netData.getXDisp(index);
				int sy = pDoc->netData.getYDisp(index);
				pDoc->pushEditDataSource(sid, slbl, sx, sy, smode, spaths);
				DrawDataSource(index, zoomScale, DRAW_ERASE);
			}
		}
		selectedComponents.clear();
		if (pDoc->processEditData()) {
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			pMainWnd->m_wndProperties.InitPropListDefault();
		}
	}
}

void CNetDesignerView::copySelectedComponents()
{
	if (selectedComponents.size() > 0) {
		copyData.clear();
		CNetDesignerDoc* pDoc = GetDocument();
		for (int i = 0; i < selectedComponents.size(); i++) {
			int index = selectedComponents[i];
			if (pDoc->netData.isNode(index)) {
				copyData.pushNodeData(pDoc->netData.getNode(index));
			}
			else if (pDoc->netData.isConnect(index)) {
				CString sid = pDoc->netData.getID(index);
				CString slbl = pDoc->netData.getLabel(index);
				CString snode_in = pDoc->netData.getInputNode(index);
				CString snode_out = pDoc->netData.getOutputNode(index);
				vector<CPoint> points = pDoc->netData.getPoints(index);
				copyData.pushConnectData(sid, slbl, snode_in, snode_out, 0.0, points);
			}
			else if (pDoc->netData.isDataSource(index)) {
				CString sid = pDoc->netData.getID(index);
				CString slbl = pDoc->netData.getLabel(index);
				CString smode = pDoc->netData.getDataMode(index);
				vector<CString> spaths = pDoc->netData.getFilePaths(index);
				int sx = pDoc->netData.getXDisp(index);
				int sy = pDoc->netData.getYDisp(index);
				copyData.pushDataSource(sid, slbl, sx, sy, smode, spaths);
			}
		}
	}
}

void CNetDesignerView::startPlaceComponent(CString tool)
{
	endSelect();
	if ((tool == _T("Hidden Node")) || (tool == _T("Output Node")) || (tool == _T("Input Node")) || (tool == _T("PreProcess"))) {
		if (tool == _T("Hidden Node")) {
			placing_component_type = _T("Hidden");
		}
		else if (tool == _T("Output Node")) {
			placing_component_type = _T("Output");
		}
		else if (tool == _T("Input Node")) {
			placing_component_type = _T("Input");
		}
		else if (tool == _T("PreProcess")) {
			placing_component_type = _T("PreProcess");
		}
		else {
			placing_component_type = _T("");
		}
		HCURSOR hCurs1 = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_NODE));
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_NODE)));
		SetFocus();
		addPanel1String(_T("Placing Node"));
	}
	else if (tool == _T("Connect")) {
		connecting = true;
		connectingPoints.clear();
		connectingComponents.clear();
		placing_component_type = _T("Connect");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Connecting"));
	}
	else if (tool == _T("CSV")) {
		CNetDesignerDoc* pDoc = GetDocument();
		if (!pDoc->dataSourceTrue) {
			placing_component_type = _T("CSV");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			SetFocus();
			addPanel1String(_T("Placing Data Source"));
		}
	}
	else if (tool == _T("Net")) {
		CNetDesignerDoc* pDoc = GetDocument();
		new_group = 2;
		placing_component_type = _T("Net");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing Net"));
	}
	else if (tool == _T("RNN")) {
		CNetDesignerDoc* pDoc = GetDocument();
		new_group = 2;
		placing_component_type = _T("RNN");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing RNN"));
	}
	else if (tool == _T("AutoEncoder")) {
		CNetDesignerDoc* pDoc = GetDocument();
		new_group = 2;
		placing_component_type = _T("AutoEncoder");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing AutoEncoder"));
	}
	else {
		CString msg = _T("startPlaceComponent error - tool = ") + tool;
		addErrorString(msg);
		addPanel1String(_T(""));
	}
}

void CNetDesignerView::escapePlaceComponent()
{
	if (placing_component_type != _T("")) {
		connectingEraseLine();
		connecting = false;
		connectingPoints.clear();
		connectingComponents.clear();
		selectedComponents.clear();
		newGroupData.clear();
		new_group = 0;
		placing_component_type = _T("");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		addPanel1String(_T(""));
	}
}

bool CNetDesignerView::endPlaceComponent(CPoint point)
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if ((placing_component_type == _T("Hidden")) || (placing_component_type == _T("Output")) ||
		(placing_component_type == _T("Input")) || (placing_component_type == _T("PreProcess"))) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);
			int index = pDoc->addNode(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, placing_component_type);
			selectComponent(index);
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			return(true);
		}
	}
	else if (placing_component_type == _T("Connect")) {
		if ((connectingComponents.size() > 1) && (connectingPoints.size() > 1)) {
			if (!isConnectAlreadyHere(DPtoLP(point))) {
				vector<CPoint> points;
				for (int i = 0; i < connectingPoints.size(); i++) {
					points.push_back(connectingPoints[i]);
				}
				int index = pDoc->addConnect(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos,
					pDoc->netData.getID(connectingComponents[0]),
					pDoc->netData.getID(connectingComponents[1]),
					0.0, points);
				selectComponent(index);
				connectingComponents.clear();
				connectingPoints.clear();
			}
		}
		return(true);
	} 
	else if (placing_component_type == _T("CSV")) {
		if (!pDoc->dataSourceTrue) {
			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);
			vector<CString> filenames;
			int index = pDoc->addDataSource(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, placing_component_type, filenames);
			selectComponent(index); 
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			pDoc->dataSourceTrue = true;
			return(true);
		}
	}
	else if (placing_component_type == _T("Net")) {

		if (new_group == 2) {

			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);

			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);

			CRect rect;
			GetWindowRect(&rect);

			CAutoNetConfigDlg dlg;
			dlg.spt.x = point.x + rect.left;
			dlg.spt.y = point.y + rect.top;

			if (dlg.DoModal() == IDOK) {

				newGroupData = dlg.makeNet(mpt);

				new_group = 1;
				past_mindex = 0;
				CRect wrect;
				GetWindowRect(&wrect);
				pasteLastPoint = applyGrid(mpt);
				drawNewGroupComponents(pasteLastPoint, past_mindex, DRAW_HIGHLIGHT);

			}
			else {
				new_group = 0;
				newGroupData.clear();
				placing_component_type = _T("");
				HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
				SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
				SetCursor(hCurs1);
			}
		}

		else if (new_group == 1) {

			finalizeNetPlacement();

			bool gnormalize = false;
			for (int i = 0; i < newGroupData.size(); i++) {
				if (newGroupData.isNode(i)) {
					if (newGroupData.getNodeMode(i) == _T("PreProcess")) {
						gnormalize = true;
					}
				}
			}

			selectedComponents.clear();
			
			for (int i = 0; i < newGroupData.size(); i++) {
				if (newGroupData.isNode(i)) {
					if (!gnormalize || (newGroupData.getNodeMode(i) != _T("Input"))) {
						selectedComponents.push_back(pDoc->netData.getIndexFromID(newGroupData.getID(i)));
					}
				}
			}

			autoRoute();

			newGroupData.clear();

			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			return(true);

		}
	}

	else if (placing_component_type == _T("RNN")) {

		if (new_group == 2) {

			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);

			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);

			CRect rect;
			GetWindowRect(&rect);

			CAutoRnnConfigDlg dlg;
			dlg.spt.x = point.x + rect.left;
			dlg.spt.y = point.y + rect.top;

			if (dlg.DoModal() == IDOK) {
				
				newGroupData = dlg.makeNet(mpt);

				new_group = 1;		
				past_mindex = 0;
				CRect wrect;
				GetWindowRect(&wrect);
				pasteLastPoint = applyGrid(mpt);
				drawNewGroupComponents(pasteLastPoint, past_mindex, DRAW_HIGHLIGHT);
				
			}
			else {
				new_group = 0;
				newGroupData.clear();
				placing_component_type = _T("");
				HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
				SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
				SetCursor(hCurs1);
			}

		}

		else if (new_group == 1) {

			finalizeNetPlacement();
			selectedComponents.clear();
			newGroupData.clear();

			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			return(true);
			
		}

	}
	else if (placing_component_type == _T("AutoEncoder")) {

		if (new_group == 2) {

			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);

			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);

			CRect rect;
			GetWindowRect(&rect);

			CAutoEncoderConfigDlg dlg;
			dlg.spt.x = point.x + rect.left;
			dlg.spt.y = point.y + rect.top;

			if (dlg.DoModal() == IDOK) {

				newGroupData = dlg.makeNet(mpt);

				new_group = 1;
				past_mindex = 0;
				CRect wrect;
				GetWindowRect(&wrect);
				pasteLastPoint = applyGrid(mpt);
				drawNewGroupComponents(pasteLastPoint, past_mindex, DRAW_HIGHLIGHT);

			}
			else {
				new_group = 0;
				newGroupData.clear();
				placing_component_type = _T("");
				HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
				SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
				SetCursor(hCurs1);
			}

		}

		else if (new_group == 1) {

			finalizeNetPlacement();

			bool gnormalize = false;
			for (int i = 0; i < newGroupData.size(); i++) {
				if (newGroupData.isNode(i)) {
					if (newGroupData.getNodeMode(i) == _T("PreProcess")) {
						gnormalize = true;
					}
				}
			}

			selectedComponents.clear();

			for (int i = 0; i < newGroupData.size(); i++) {
				if (newGroupData.isNode(i)) {
					if (!gnormalize || (newGroupData.getNodeMode(i) != _T("Input"))) {
						selectedComponents.push_back(pDoc->netData.getIndexFromID(newGroupData.getID(i)));
					}
				}
			}

			autoRoute();

			newGroupData.clear();

			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
			return(true);

		}

	}
	
	addPanel1String(_T(""));
	return(false);

}

void CNetDesignerView::finalizeNetPlacement()
{
	CNetDesignerDoc* pDoc = GetDocument();

	new_group = 0;
	placing_component_type = _T("");
	vector<CPoint> points;

	drawNewGroupComponents(pasteLastPoint, past_mindex, DRAW_NORMAL);

	pasteLastPoint = applyGrid(DPtoLP(pasteLastPoint));
	int x = pasteLastPoint.x - newGroupData.getXDisp(0);
	int y = pasteLastPoint.y - newGroupData.getYDisp(0);

	pDoc->clearAndSetEditParams(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos);
	for (int i = 0; i < newGroupData.size(); i++) {
		if (newGroupData.isNode(i)) {
			newGroupData.setXDisp(i, newGroupData.getXDisp(i) + x);
			newGroupData.setYDisp(i, newGroupData.getYDisp(i) + y);
			pDoc->pushEditNodeData(newGroupData.getNode(i));
		}
		if (newGroupData.isConnect(i)) {
			pDoc->pushEditConnectData(newGroupData.getID(i), newGroupData.getID(i), newGroupData.getInputNode(i), newGroupData.getOutputNode(i), 0.0, points);
		}
	}
	pDoc->processEditData();

}

bool CNetDesignerView::isNodeAlreadyHere(CPoint here) 
{
	CNetDesignerDoc* pDoc = GetDocument();
	int index = pDoc->checkComponentPlacement(here.x, here.y, SELECT_ALL);
	if (index >= 0) {
		if (pDoc->netData.isNode(index)) {
			return true;
		}
	}
	return false;
}

bool CNetDesignerView::isConnectAlreadyHere(CPoint here)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((connectingComponents.size() > 1) && (connectingPoints.size() > 1)) {
		CString from = pDoc->netData.getID(connectingComponents[0]);
		CString to = pDoc->netData.getID(connectingComponents[1]);
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConnect(i)) {
				if (((from == pDoc->netData.getInputNode(i)) && (to == pDoc->netData.getOutputNode(i))) || 
					((to == pDoc->netData.getInputNode(i)) && (from == pDoc->netData.getOutputNode(i)))) {
					return(true);
				}
			}
		}
	}
	return false;
}

CPoint CNetDesignerView::applyGrid(CPoint pt)
{
	CPoint rpt = pt;
	rpt.x = applyGridX(rpt.x);
	rpt.y = applyGridX(rpt.y);
	return(rpt);
}

int CNetDesignerView::applyGridX(int x)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->snap_spacing != _T("None")) {
		int ngrid = (int)_tstof((LPCTSTR)pDoc->snap_spacing);
		int g1 = ngrid * (x / ngrid);
		if (x >= 0) {
			int g2 = ngrid * ((x / ngrid) + 1);
			if ((x - g1) < (g2 - x)) {
				return(g1);
			}
			else {
				return(g2);
			}
		}
		else {
			int g2 = ngrid * ((x / ngrid) - 1);
			if ((x - g1) > (g2 - x)) {
				return(g1);
			}
			else {
				return(g2);
			}
		}
	}
	return(x);
}

int CNetDesignerView::applyGridY(int y)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->snap_spacing != _T("None")) {
		int ngrid = (int)_tstof((LPCTSTR)pDoc->snap_spacing);
		int g1 = ngrid * (y / ngrid);
		if (y >= 0) {
			int g2 = ngrid * ((y / ngrid) + 1);
			if ((y - g1) < (g2 - y)) {
				return(g1);
			}
			else {
				return(g2);
			}
		}
		else {
			int g2 = ngrid * ((y / ngrid) - 1);
			if ((y - g1) > (g2 - y)) {
				return(g1);
			}
			else {
				return(g2);
			}
		}
	}
	return(y);
}

void CNetDesignerView::autoRoute()
{
	DrawArrow(route_origin.x, route_origin.y, route_angle);

	typedef struct sort_data {
		int nindex;
		CPoint mpoint;
		int group;
	} SORT_DATA;

	vector<sort_data> rnodes;

	CNetDesignerDoc* pDoc = GetDocument();

	if (selectedComponents.size() > 0) {
		for (int i = 0; i < selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(selectedComponents[i])) {
				sort_data tnode;
				tnode.nindex = selectedComponents[i];
				tnode.mpoint = CPoint(pDoc->netData.getXDisp(tnode.nindex), pDoc->netData.getYDisp(tnode.nindex));
				tnode.group = -1;
				bool added = false;
				for (int j = 0; j < rnodes.size(); j++) {
					if (testNodeRouteOrder(tnode.mpoint, rnodes[j].mpoint)) {
						rnodes.insert(rnodes.begin() + j, tnode);
						j = (int)rnodes.size();
						added = true;
					}
				}
				if (!added) {
					rnodes.push_back(tnode);
				}
			}
		}
	}
	else {
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isNode(i)) {
				sort_data tnode;
				tnode.nindex = i;
				tnode.mpoint = CPoint(pDoc->netData.getXDisp(tnode.nindex), pDoc->netData.getYDisp(tnode.nindex));
				tnode.group = -1;
				bool added = false;
				for (int j = 0; j < rnodes.size(); j++) {
					if (testNodeRouteOrder(tnode.mpoint, rnodes[j].mpoint)) {
						rnodes.insert(rnodes.begin() + j, tnode);
						j = (int)rnodes.size();
						added = true;
					}
				}
				if (!added) {
					rnodes.push_back(tnode);
				}
			}
		}
	}

	int group = 0;
	rnodes[0].group = group;
	for (int i = 1; i < rnodes.size(); i++) {
		if (!testNodeRouteGroup(rnodes[i-1].mpoint, rnodes[i].mpoint)) {
			group++;
		}
		rnodes[i].group = group;
	}

	typedef struct add_connects {
		CString sid;
		CString node_in;
		CString node_out;
	} ADD_CONNECTS;

	vector<add_connects> addconnects;

	int ogroup = 1;
	int nindex_offset = 0;
	vector<int> fromIndices;
	vector<int> toIndices;
	while (ogroup <= group) {
		fromIndices.clear();
		toIndices.clear();
		for (int i = 0; i < rnodes.size(); i++) {
			if (rnodes[i].group == (ogroup-1)) {
				fromIndices.push_back(rnodes[i].nindex);
			}
			else if (rnodes[i].group == ogroup) {
				toIndices.push_back(rnodes[i].nindex);
			}
		}
		for (int i = 0; i < fromIndices.size(); i++) {
			for (int j = 0; j < toIndices.size(); j++) {
				if (!areNodesConnected(fromIndices[i], toIndices[j])) {
					add_connects addconnect;
					addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
					addconnect.node_in = pDoc->netData.getID(fromIndices[i]);
					addconnect.node_out = pDoc->netData.getID(toIndices[j]);
					addconnects.push_back(addconnect);
					//CString s = addconnect.sid + _T(" :  ") + addconnect.node_in + _T(" to ") + addconnect.node_out;
					//addErrorString(s);
					nindex_offset++;
				}
			}
		}
		ogroup++;
	}

	if (addconnects.size() > 0) {
		vector<CPoint> points;
		pDoc->clearAndSetEditParams(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < addconnects.size(); i++) {
			pDoc->pushEditConnectData(addconnects[i].sid, addconnects[i].sid, addconnects[i].node_in, addconnects[i].node_out, 0.0, points);
		}
		if (pDoc->processEditData()) {
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndNodeView.FillFileView(pDoc->netData);
		}
	}

	routing = false;
	//pDoc->arrows = true;
	OnDraw();

}

bool CNetDesignerView::testNodeRouteOrder(CPoint newpoint, CPoint checkpoint)
{
	if ((route_angle >= -45) && (route_angle < 45)) {
		if (newpoint.x < checkpoint.x) {
			return(true);
		}
	}
	else if ((route_angle >= 45) && (route_angle < 135)) {
		if (newpoint.y < checkpoint.y) {
			return(true);
		}
	}
	else if ((route_angle >= 135) || (route_angle < -135)) {
		if (newpoint.x > checkpoint.x) {
			return(true);
		}
	}
	else if ((route_angle >= -135) && (route_angle < -45)) {
		if (newpoint.y > checkpoint.y) {
			return(true);
		}
	}
	return(false);
}

bool CNetDesignerView::testNodeRouteGroup(CPoint point1, CPoint point2)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((route_angle >= -45) && (route_angle < 45)) {
		if ((point2.x - point1.x) < pDoc->nradius) {
			return(true);
		}
	}
	else if ((route_angle >= 45) && (route_angle < 135)) {
		if ((point2.y - point1.y) < pDoc->nradius) {
			return(true);
		}
	}
	else if ((route_angle >= 135) || (route_angle < -135)) {
		if ((point1.x - point2.x) < pDoc->nradius) {
			return(true);
		}
	}
	else if ((route_angle >= -135) && (route_angle < -45)) {
		if ((point1.y - point2.y) < pDoc->nradius) {
			return(true);
		}
	}
	return(false);
}

bool CNetDesignerView::areNodesConnected(int nindex1, int nindex2)
{
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			if ((pDoc->netData.getInputNode(i) == pDoc->netData.getID(nindex1)) && (pDoc->netData.getOutputNode(i) == pDoc->netData.getID(nindex2))) {
				return(true);
			}
			else if ((pDoc->netData.getInputNode(i) == pDoc->netData.getID(nindex2)) && (pDoc->netData.getOutputNode(i) == pDoc->netData.getID(nindex1))) {
				return(true);
			}
		}
	}
	return(false);
}

COLORREF CNetDesignerView::adjustColorByPercent(COLORREF clr, float percent)
{
	COLORREF rval = 0x00000000;

	int r2 = (int)((1 + percent / 100) * (float)GetRValue(clr));
	if (r2 > 255) {
		r2 = 255;
	}
	else if (r2 < 0) {
		r2 = 0;
	}

	int g2 = (int)((1 + percent / 100) * (float)GetGValue(clr));
	if (g2 > 255) {
		g2 = 255;
	}
	else if (g2 < 0) {
		g2 = 0;
	}

	int b2 = (int)((1 + percent / 100) * (float)GetBValue(clr));
	if (b2 > 255) {
		b2 = 255;
	}
	else if (b2 < 0) {
		b2 = 0;
	}

	rval = RGB(r2, g2, b2);

	return(rval);
}




































































































