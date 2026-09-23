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
#include "NetVerify.h"
#include "NetOutputDlg.h"
#include "CDataSetDlg.h"
#include "CSeedDlg.h"
#include "CAutoNetConfigDlg.h"
#include "CAutoRnnConfigDlg.h"
#include "CStatus.h"
#include "CEvaluateDlg.h"
#include "CAutoEncoderConfigDlg.h"
#include "CNetInfo.h"
#include "CStatus2.h"
#include "CExportFileTypeDlg.h"
#include "CTrainInitializationDlg1.h"
#include "CTrainInitializationDlg2.h"
#include "CTrainInitializationDlg3.h"
#include "WaveFile.h"
#include "CEvaluateFileDlg.h"
#include "CArchiveWeights.h"
#include "CAssemblyPicker.h"

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
	ON_COMMAND(ID_CONV_LAYER, &CNetDesignerView::OnConvLayer)
	ON_UPDATE_COMMAND_UI(ID_CONV_LAYER, &CNetDesignerView::OnUpdateConvLayer)
		ON_COMMAND(ID_GRAPH_OUTPUTS, &CNetDesignerView::OnGraphOutputs)
		ON_UPDATE_COMMAND_UI(ID_GRAPH_OUTPUTS, &CNetDesignerView::OnUpdateGraphOutputs)
		ON_COMMAND(ID_SHOW_CONNECTS, &CNetDesignerView::OnShowConnects)
		ON_UPDATE_COMMAND_UI(ID_SHOW_CONNECTS, &CNetDesignerView::OnUpdateShowConnects)
		ON_COMMAND(ID_AUTOSAVE, &CNetDesignerView::OnAutosave)
		ON_UPDATE_COMMAND_UI(ID_AUTOSAVE, &CNetDesignerView::OnUpdateAutosave)
		ON_COMMAND(ID_INFO, &CNetDesignerView::OnInfo)
		ON_COMMAND(ID_CLEAR, &CNetDesignerView::OnClear)
		ON_COMMAND(ID_EVALUATE_FILE, &CNetDesignerView::OnEvaluateFile)
		ON_UPDATE_COMMAND_UI(ID_EVALUATE_FILE, &CNetDesignerView::OnUpdateEvaluateFile)
		ON_COMMAND(ID_BUTTON2, &CNetDesignerView::OnInputLayer)
		ON_UPDATE_COMMAND_UI(ID_BUTTON2, &CNetDesignerView::OnUpdateInputLayer)
		ON_COMMAND(ID_BUTTON3, &CNetDesignerView::OnInsertHiddenLayers)
		ON_UPDATE_COMMAND_UI(ID_BUTTON3, &CNetDesignerView::OnUpdateInsertHiddenLayers)
		ON_COMMAND(ID_BUTTON4, &CNetDesignerView::OnInsertOutputLayer)
		ON_UPDATE_COMMAND_UI(ID_BUTTON4, &CNetDesignerView::OnUpdateInsertOutputLayer)
		ON_COMMAND(ID_SELECT_DATA, &CNetDesignerView::OnSelectLayerData)
		ON_UPDATE_COMMAND_UI(ID_SELECT_DATA, &CNetDesignerView::OnUpdateSelectLayerData)
		ON_COMMAND(ID_ATTENTION, &CNetDesignerView::OnAttention)
		ON_UPDATE_COMMAND_UI(ID_ATTENTION, &CNetDesignerView::OnUpdateAttention)
		ON_COMMAND(ID_ZERO_OUTPUT, &CNetDesignerView::OnEnableZeroOutput)
		ON_UPDATE_COMMAND_UI(ID_ZERO_OUTPUT, &CNetDesignerView::OnUpdateEnableZeroOutput)
		ON_COMMAND(ID_AUTOMATION, &CNetDesignerView::OnAutomation)
		ON_UPDATE_COMMAND_UI(ID_AUTOMATION, &CNetDesignerView::OnUpdateAutomation)
		ON_COMMAND(ID_NET_CLUSTER, &CNetDesignerView::OnNetCluster)
		ON_UPDATE_COMMAND_UI(ID_NET_CLUSTER, &CNetDesignerView::OnUpdateNetCluster)
		ON_COMMAND(ID_SAVE_CLUSTER, &CNetDesignerView::OnSaveNetCluster)
		ON_UPDATE_COMMAND_UI(ID_SAVE_CLUSTER, &CNetDesignerView::OnUpdateSaveNetCluster)
		ON_COMMAND(ID_DROPOUT, &CNetDesignerView::OnDropout)
		ON_UPDATE_COMMAND_UI(ID_DROPOUT, &CNetDesignerView::OnUpdateDropout)
		ON_COMMAND(ID_ATTENTION2, &CNetDesignerView::OnAttention2)
		ON_UPDATE_COMMAND_UI(ID_ATTENTION2, &CNetDesignerView::OnUpdateAttention2)
		ON_COMMAND(ID_ASSEMBLY, &CNetDesignerView::OnAssembly)
		ON_UPDATE_COMMAND_UI(ID_ASSEMBLY, &CNetDesignerView::OnUpdateAssembly)
		ON_COMMAND(ID_EDIT_ASSEMBLY, &CNetDesignerView::OnEditAssembly)
		ON_UPDATE_COMMAND_UI(ID_EDIT_ASSEMBLY, &CNetDesignerView::OnUpdateEditAssembly)
		ON_COMMAND(ID_CLOSE_ASSEMBLY, &CNetDesignerView::OnCloseAssembly)
		ON_UPDATE_COMMAND_UI(ID_CLOSE_ASSEMBLY, &CNetDesignerView::OnUpdateCloseAssembly)
		ON_WM_LBUTTONDBLCLK()
		ON_WM_RBUTTONDBLCLK()
		END_MESSAGE_MAP()



// CNetDesignerView construction/destruction

CNetDesignerView::CNetDesignerView() noexcept
{
	display = false;
	netLoaded = false;
	set_initial_scroll = true;
	node_limit_message_displayed = false;
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
	pMainWnd->m_wndNodeView.FillFileView();

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
	display = true;
	OnZoomFit();

	addPanel1String(_T(""));

	CString data = _T("");

	data.Format(_T("%.0lf"), pDoc->maxEpochs);
	pMainWnd->setEpochs(data);
	data.Format(_T("%.0lf"), pDoc->minEpochs);
	pMainWnd->setMinEpochs(data);
	data.Format(_T("%.1lf"), pDoc->trainTime);
	pMainWnd->setTrainTime(data);
	data.Format(_T("%.6lf"), pDoc->targetError);
	pMainWnd->setTargetError(data);
	data.Format(_T("%.0lf"), pDoc->targetDeltaError);
	pMainWnd->setTargetDeltaError(data);
	data.Format(_T("%.0lf"), pDoc->updateRate);
	pMainWnd->setUpdateRate(data);

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
	if (!pDoc) {
		return;
	}
	if (display) {
		checkZoom();
		UpdateDrawingData();
		DrawGrid(pDC);
		DrawComponents(pDC);
	}
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
		if (pDoc->netData.isConvLayer(i)) {
			DrawConvolution(pDC, i, pZoomScale, DRAW_NORMAL, true);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isAttentionLayer(i)) {
			DrawAttention(pDC, i, pZoomScale, DRAW_NORMAL, true);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isAssemblyLayer(i)) {
			assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
			DrawAssembly(pDC, i, pZoomScale, DRAW_NORMAL, asub, true);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNetCluster(i)) {
			DrawNetCluster(pDC, i, pZoomScale, DRAW_NORMAL, true);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isDropout(i)) {
			DrawDropout(pDC, i, pZoomScale, DRAW_NORMAL, true);
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
		addErrorString(_T("OnRButtonUp : tp1"));
		escapePlaceComponent();
	}
	else if (pDoc->selectedComponents.size() > 0) {
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
	assembly_sub_component asssub = checkSelectionAssemblyNode(point);
	if (connecting && (placing_component_type == _T("Connect"))) {
		CNetDesignerDoc* pDoc = GetDocument();
		CPoint mpt = DPtoLP(point);
		if (connectingPoints.size() == 0) {
			placement_data place = pDoc->checkComponentPlacementReturnString(mpt.x, mpt.y, SELECT_NODES, true, pDoc->assembly_edit_index);
			if (place.index >= 0) {
				connecting_erase = false;
				connectingComponents.push_back(place.ID);
				CPoint npt(place.pt.x, place.pt.y);
				npt = LPtoDP(npt);
				connectingPoints.push_back(npt);
			}
		}
		else if (connectingPoints.size() > 0) {
			CPoint npt = LPtoDP(applyGrid(DPtoLP(point)));
			placement_data place = pDoc->checkComponentPlacementReturnString(mpt.x, mpt.y, SELECT_NODES, false, pDoc->assembly_edit_index);
			if ((place.index >= 0) && (place.ID != connectingComponents[connectingComponents.size()-1])) {
				connectingComponents.push_back(place.ID);
				CPoint npt2(place.pt.x, place.pt.y);
				npt2 = LPtoDP(npt2);
				connectingEraseLine();
				connectingPoints.push_back(npt2);
				endPlaceComponent(point);
				CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
				pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				pMainWnd->m_wndNodeView.FillFileView();
			}
			else if (npt != connectingPoints[connectingPoints.size()-1]) {
				connectingDrawLine(npt);
				connectingPoints.push_back(npt);
			}
		}
	}
	else if (checkIfNodeSelected() && (move_state == MOVE_CAPTURE) && checkValidSelection(point)) {
		moveStartPoint = point;// applyGrid(point);
		moveLastPoint = moveStartPoint;
		move_state = MOVE_MOVING;
		//addErrorString(_T("setting to moving (1)"));
		addStragglingConnectionsToSelectedComponents();
		drawSelectedComponents(DRAW_ERASE);
		drawSelectedComponents(DRAW_HIGHLIGHT);
		addPanel1String(_T("Moving"));
	}
	else if ((checkIfNodeSelected() && (asssub.net_index < 0)) && (move_state == MOVE_DONE) && checkValidSelection(point) && !(nFlags & MK_CONTROL)) {
		moveStartPoint = point;// applyGrid(point);
		moveLastPoint = moveStartPoint;
		move_state = MOVE_CHECKING;
	}
	else if (move_state == MOVE_MOVING) {
		move_state = MOVE_DONE;
		CPoint mpt = applyGrid(point);
		CPoint pt2;
		pt2.x = mpt.x - moveStartPoint.x;
		pt2.y = mpt.y - moveStartPoint.y;
		pt2.x = (int)((float)pt2.x / zoomScale);
		pt2.y = (int)((float)pt2.y / zoomScale);
		finalMoveSelectedItems(pt2);
		moveLastPoint = mpt;
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
		//addErrorString(_T("OnLButtonDown : selecting component"));
		bool add = false;
		if (nFlags & MK_CONTROL) {
			add = true;
		}
		if (!selectComponent(point, add, asssub)) {
			selection_box = true;
			selection_box_erase = false;
			selection_pt1 = point;
			CNetDesignerDoc* pDoc = GetDocument();
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
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
		pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
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
				CPoint lpt1 = pDoc->getInputNodePoint(i);
				CPoint lpt2 = pDoc->getOutputNodePoint(i);
				mpt.x = (lpt1.x + lpt2.x) / 2;
				mpt.y = (lpt1.y + lpt2.y) / 2;
				found = true;
			}
			else if (pDoc->netData.isConvLayer(i)) {
				CPoint pt = pDoc->getConvCenter(i);
				mpt.x = pt.x;
				mpt.y = pt.y;
				found = true;
			}
			else if (pDoc->netData.isAttentionLayer(i)) {
				CPoint pt = pDoc->getAttentionCenter(i);
				mpt.x = pt.x;
				mpt.y = pt.y;
				found = true;
			}
			else if (pDoc->netData.isAssemblyLayer(i)) {
				CPoint pt = pDoc->getAssemblyCenter(i);
				mpt.x = pt.x;
				mpt.y = pt.y;
				found = true;
			}
			else if (pDoc->netData.isNetCluster(i)) {
				CPoint pt = pDoc->getNetClusterCenter(i);
				mpt.x = pt.x;
				mpt.y = pt.y;
				found = true;
			}
			else if (pDoc->netData.isDropout(i)) {
				CPoint pt = pDoc->getDropoutCenter(i);
				mpt.x = pt.x;
				mpt.y = pt.y;
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
		pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);

	}
}

void CNetDesignerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CNetDesignerDoc* pDoc = GetDocument();
	CPoint mpt = DPtoLP(point);
	int pindex = pDoc->checkComponentPlacementReturnIndex(mpt.x, mpt.y, SELECT_NODES, pDoc->assembly_edit_index);
	if (pindex >= 0) {
		if (pDoc->netData.isAssemblyLayer(pindex)) {
			OnEditAssembly();
		}
	}
	CView::OnLButtonDblClk(nFlags, point);
}


void CNetDesignerView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CView::OnRButtonDblClk(nFlags, point);
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
			//addErrorString(_T("setting to moving (2)"));
			addStragglingConnectionsToSelectedComponents();
			drawSelectedComponents(DRAW_ERASE);
			drawSelectedComponents(DRAW_HIGHLIGHT);
			addPanel1String(_T("Moving"), point);
		}
	}
	else if (move_state == MOVE_MOVING) {	
		CPoint pt1 = moveLastPoint - moveStartPoint;
		pt1.x = (int)((float)pt1.x / zoomScale);
		pt1.y = (int)((float)pt1.y / zoomScale);
		CPoint mpt = applyGrid(point);
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
			deleteSelectedComponents();
		}
		else if (key == VK_BACK) {
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
	bool preprocess = true;
	bool inputlayer = false;
	bool outputlayer = false;
	bool learnable_node = false;
	bool learnable_conv = false;
	bool output_node = false;
	bool netcluster = false;
	bool assembly = false;
	if (pDoc->selectedComponents.size() == 1) {
		int sel1 = pDoc->selectedComponents[0];
		string mode1 = pDoc->netData.getNodeMode(sel1);
		if ((pDoc->getDataConvType() == DATA_CONV) && pDoc->netData.isConvLayer(sel1)) {
			if (mode1 == "Input") {
				inputlayer = true;
			}
			else if (mode1 == "Output") {
				outputlayer = true;
			}
		}
		if (pDoc->netData.isNetCluster(sel1)) {
			netcluster = true;
		}
		if (pDoc->netData.isAssemblyLayer(sel1)) {
			assembly = true;
		}
		if ((mode1 != "PreProcess") && (mode1 != "Input")) {
			preprocess = false;
		}
	}
	for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
		if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
			if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") {
				if ((pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]) != "MaxPool") &&
					(pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]) != "Multiply") &&
					(pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]) != "Add")) {
					learnable_node = true;
				}
				preprocess = false;
			}
			else if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Output") {
				learnable_node = true;
				output_node = true;
			}
		}
		else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
			if (!assembly) {
				if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Output")) {
					learnable_conv = true;
				}
			}
			preprocess = false;
		}
	}
	if (pDoc->selectedComponents.size() == 0) {
		preprocess = false;
	}
	if (inputlayer) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT6, point.x, point.y, this, TRUE);
	}
	else if (netcluster) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT10, point.x, point.y, this, TRUE);
	}
	else if (outputlayer) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT7, point.x, point.y, this, TRUE);
	}
	else if ((learnable_node || learnable_conv) && !preprocess && !output_node) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT4, point.x, point.y, this, TRUE);
	}
	else if ((learnable_node || learnable_conv) && preprocess) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT5, point.x, point.y, this, TRUE);
	}
	else if (preprocess) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT8, point.x, point.y, this, TRUE);
	}
	else if (learnable_node && output_node) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT9, point.x, point.y, this, TRUE);
	}
	else if (assembly) {
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT11, point.x, point.y, this, TRUE);
	}
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

void CNetDesignerView::OnInfo()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CNetInfo dlg;
	bool show_dialog = false;
	int color_scale = pDoc->getDataConvType();
	dlg.m_notes = (CString)getInfo(color_scale).c_str();
	if (pDoc->getValidationPathNamesSize() > 0) {
		CString s = _T("");
		s.Format(_T("Num Validation Files = %i\n  Num Validation Data = %i\n"), (int)pDoc->getValidationPathNamesSize(), pDoc->nnet.num_validationdata_rows);
		dlg.m_notes = s + dlg.m_notes;
		show_dialog = true;
	}
	if (pDoc->getDataPathNamesSize(false) > 0) {
		CString s = _T("");
		s.Format(_T("Num Data Files = %i\n  Num Train Data = %i\n"), pDoc->getDataPathNamesSize(false), pDoc->nnet.num_filedata_rows);
		dlg.m_notes = s + dlg.m_notes;
		show_dialog = true;
	}
	if (!pDoc->training && !pDoc->verifying && !pDoc->clustering && (pDoc->nnet.train_count > 0)) {
		CStatus2 sdlg;
		sdlg.setLabel(_T("Forward Pass Timing Analysis . . ."));
		sdlg.CreateDlg();
		double time = pDoc-> timingAnalysis();
		if (time < 0.001) {
			time = time * 1000000;
			CString s = _T("");
			if (time > 100) {
				s.Format(_T("Timing Analysis:  %.1fus per forward pass\n"), time);
			}
			else if (time > 10) {
				s.Format(_T("Timing Analysis:  %.2fus per forward pass\n"), time);
			}
			else {
				s.Format(_T("Timing Analysis:  %.3fus per forward pass\n"), time);
			}
			dlg.m_notes = dlg.m_notes + _T("\n") + s;
		}
		else if (time < 1.0) {
			time = time * 1000;
			CString s = _T("");
			if (time > 100) {
				s.Format(_T("Timing Analysis:  %.1fms per forward pass\n"), time);
			}
			else if (time > 10) {
				s.Format(_T("Timing Analysis:  %.2fms per forward pass\n"), time);
			}
			else {
				s.Format(_T("Timing Analysis:  %.3fms per forward pass\n"), time);
			}
			dlg.m_notes = dlg.m_notes + _T("\n") + s;
		}
		else {
			CString s = _T("");
			if (time > 100) {
				s.Format(_T("Timing Analysis:  %.1fs per forward pass\n"), time);
			}
			else if (time > 10) {
				s.Format(_T("Timing Analysis:  %.2fs per forward pass\n"), time);
			}
			else {
				s.Format(_T("Timing Analysis:  %.3fs per forward pass\n"), time);
			}
			dlg.m_notes = dlg.m_notes + _T("\n") + s;
		}
		sdlg.DestroyDlg();
		show_dialog = true;
	}
	if (show_dialog) {
		dlg.DoModal();
	}

	/*static int tcnt = 0;
	CString s = _T("");
	s.Format(_T(" : count = %i"), tcnt);
	addErrorString(_T("OnInfo : Checking HiddenNet") + s);
	HiddenNet hnet;
	for (int i = 0; i < 100; i++) {

		hnet.init(_T("Net0"), 10+i, 10+i, 4+i, 20+i, SIGMOID, pDoc->deta, pDoc->dalpha, pDoc->drand);

		hnet.clearWeights();
		hnet.randomizeWeights();
		hnet.checkValidWeights();
		hnet.forward();
		hnet.deallocate();

	}
	addErrorString(_T("OnInfo : Checking HiddenNet Complete"));
	tcnt++;*/

}

string CNetDesignerView::getInfo(int color_scale)
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();
	string info = "";
	int num_nodes = 0;
	int num_connects = 0;
	int num_convs = 0;
	for (int i = 0; i < (int)pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			if (pDoc->netData.areMultipleBias(i)) {
				num_nodes += pDoc->netData.getNumBias(i);
			}
			else {
				num_nodes++;
			}
		}
		else if (pDoc->netData.isConnect(i)) {
			if (func.getConvFromConnectName(pDoc->netData.getInputNode(i)) && !func.getConvFromConnectName(pDoc->netData.getOutputNode(i))) {
				for (int j = 0; j < (int)pDoc->netData.size(); j++) {
					if (pDoc->netData.isConvLayer(i)) {
						if (pDoc->netData.getID(j) == func.getConvNameFromConnectName(pDoc->netData.getInputNode(i))) {
							imageSizeData imageSize = func.getImageSizeData(pDoc->netData.getID(j));
							int outwidth = imageSize.output_width;
							int outheight = imageSize.output_height;
							int tconnects = outwidth * outheight;
							num_connects += tconnects;
							j = (int)pDoc->netData.size();
						}
					}
				}
			}
			else {
				if (pDoc->netData.areMultipleWeights(i)) {
					num_connects += pDoc->netData.getNumWeights(i);
				}
				else {
					num_connects++;
				}
			}
		}
		else if (pDoc->netData.isConvLayer(i)) {
			num_convs++;
			if ((pDoc->netData.getNodeMode(i) == "Hidden") || (pDoc->netData.getNodeMode(i) == "Output")) {
				vector<int> dims = pDoc->netData.getConvDimensions(i);
				if (dims.size() == 4) {
					int nlayers = pDoc->netData.getConvNumLayers(i);
					if (nlayers > 1) {
						int nplayer = pDoc->netData.getConvNodesPerLayer(i);
						if (nlayers == 2) {
							num_connects += (color_scale * dims[0] * dims[1] * nplayer);
							num_connects += (color_scale * nplayer * dims[2] * dims[3]);
							num_nodes += (dims[2] * dims[3]);
							num_nodes += nplayer;
						}
						else {
							num_connects += (color_scale * dims[0] * dims[1] * nplayer);
							for (int k = 1; k < (nlayers - 1); k++) {
								num_connects += (nplayer * nplayer);
								num_nodes += nplayer;
							}
							num_connects += (color_scale * nplayer * dims[2] * dims[3]);
							num_nodes += (dims[2] * dims[3]);
							num_nodes += nplayer;
						}
					}
					else {
						num_connects += (color_scale * dims[0] * dims[1] * dims[2] * dims[3]);
						num_nodes += (dims[2] * dims[3]);
					}
				}
			}
		}
	}
	if (num_nodes > 0) {
		char s[200];  sprintf_s(s, "Num Nodes = %i\n", num_nodes);
		info += s;
	}
	if (num_convs > 0) {
		char s[200];  sprintf_s(s, "Num Convolutions = %i\n", num_convs);
		info += s;
	}
	if (num_connects > 0) {
		char s[200];  sprintf_s(s, "Num Connects = %i\n", num_connects);
		info += s;
	}
	return(info);
}

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
	drawSelectedComponents(DRAW_NORMAL);
	deleteStragglingConnectionsFromSelectedComponents();
	copySelectedComponents();
}

void CNetDesignerView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((pDoc->selectedComponents.size() > 0) && !pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEditCut()
{
	drawSelectedComponents(DRAW_NORMAL);
	deleteStragglingConnectionsFromSelectedComponents();
	copySelectedComponents();
	deleteSelectedComponents();
	OnEditPaste();
}

void CNetDesignerView::OnEditCut(CPoint pt)
{
	drawSelectedComponents(DRAW_NORMAL);
	deleteStragglingConnectionsFromSelectedComponents();
	copySelectedComponents();
	deleteSelectedComponents();
	OnEditPaste(pt);
}

void CNetDesignerView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((pDoc->selectedComponents.size() > 0) && !pDoc->training && !pDoc->clustering && !pDoc->verifying) {
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
	if ((copyData.size() > 0) && !pDoc->training && !pDoc->clustering && !pDoc->verifying) {
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
	if (checkIfNodeSelected() && !pDoc->training && !pDoc->clustering && !pDoc->verifying) {
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
		if (pDoc->netData.getGroupIndex(i) == pDoc->assembly_edit_index) {
			selectComponent(i, add);
			add = true;
		}
	}
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
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

void CNetDesignerView::OnShowConnects()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->show_connect) {
		pDoc->show_connect = false;
	}
	else {
		pDoc->show_connect = true;
	}
	OnDraw();
}

void CNetDesignerView::OnUpdateShowConnects(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->show_connect == true) {
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
	bool cancel = false;
	if ((pDoc->nnet.train_count > 0)/* && !pDoc->IsModified()*/ && !pDoc->new_net) {
		CTrainInitializationDlg1 dlg;
		if (dlg.DoModal() == IDOK) {
			if (dlg.continue_weights) {
				reset = false;
				randomize = false;
			}
			else if (dlg.saved_weights) {
				reset = true;
				randomize = false;
			}
			else if (dlg.randomize) {
				reset = true;
				randomize = true;
			}
			else {
				cancel = true;
			}
		}
		else {
			cancel = true;
		}
	}
	else if (pDoc->new_net || pDoc->IsModified()) {
		if (pDoc->IsModified()) {
			pDoc->DoFileSave();
		}
		CTrainInitializationDlg2 dlg;
		if (dlg.DoModal() == IDOK) {
			if (dlg.saved_weights) {
				reset = true;
				randomize = false;
			}
			else if (dlg.randomize) {
				reset = true;
				randomize = true;
			}
			else {
				cancel = true;
			}
		}
		else {
			cancel = true;
		}
	}
	if (!cancel) {
		if (reset) {
			if (pDoc->verifyNet(randomize)) {
				pMainWnd->m_wndOutput.ShowPane(true, false, true);
				pMainWnd->m_wndOutput.setTab(0);
				pDoc->training = true;
				pDoc->train_timer.Start();
			}
		}
		else {
			pDoc->softRestart();
			pDoc->training = true;
		}
	}
}

void CNetDesignerView::OnUpdateTrain(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnAutomation()
{
	CNetDesignerDoc* pDoc = GetDocument();
	pDoc->automationDlg.CreateDlg();
}

void CNetDesignerView::OnUpdateAutomation(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnAutosave()
{
	CArchiveWeights dlg;
	if (dlg.DoModal() == IDOK) {
		if (dlg.modified) {
			CNetDesignerDoc* pDoc = GetDocument();
			pDoc->SetModifiedFlag();
		}
	}

}

void CNetDesignerView::OnUpdateAutosave(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->auto_save == true) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnVerifyNet()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pMainWnd->m_wndOutput.ShowPane(true, false, true);
		pMainWnd->m_wndOutput.setTab(1);
		pDoc->verifyNet(false);
	}
}

void CNetDesignerView::OnUpdateVerifyNet(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->clustering || pDoc->verifying) {
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
	pDoc->stopClustering();
	clusterdlg.setButtonOff();
}

void CNetDesignerView::OnUpdateStopTrain(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->clustering) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnViewData()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->OnDataView();
}

void CNetDesignerView::OnUpdateViewData(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
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
	dlg.mfilenames = pDoc->getValidationPathNames();
	pDoc->combinedPathnames.clear();
	if (dlg.DoModal() == IDOK) {
		if (dlg.m_no_validation == 0) {
			pDoc->use_validation_percent = false;
			pDoc->use_validation_file = false;
			pDoc->clearValidationPathNames();
		}
		else if (dlg.m_no_validation == 1) {
			pDoc->use_validation_percent = true;
			pDoc->use_validation_file = false;
			pDoc->validation_percent = (float)_tstof((LPCTSTR)dlg.m_percent);
			pDoc->clearValidationPathNames();
		}
		else if (dlg.m_no_validation == 2) {
			pDoc->use_validation_percent = false;
			pDoc->use_validation_file = true;
			pDoc->clearValidationPathNames();
			pDoc->setValidationPathNames(dlg.mfilenames);
		}
		int ftype = pDoc->getDataPathType();
		if (ftype == FILE_TYPE_JPG) {
			pDoc->getCombinedImages();
		}
		else if (ftype == FILE_TYPE_WAV) {
			pDoc->getCombinedImages();
			pDoc->getMaxWaveMagnitude(true);
		}
		else if (ftype == FILE_TYPE_TXT) {
			pDoc->getCombinedImages();
		}
		pDoc->SetModifiedFlag(true);
	}
}

void CNetDesignerView::OnUpdateValidationSet(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnEquation()
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();
	processDlg.setfileType(pDoc->getDataPathType());
	processDlg.CreateDlg();
	updateDataToEquation();
}

void CNetDesignerView::updateDataToEquation()
{
	if (processDlg.isDisplayed()) {
		CNetDesignerDoc* pDoc = GetDocument();
		if (pDoc->selectedComponents.size() == 1) {
			CNetDesignerDoc* pDoc = GetDocument();
			if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "PreProcess") || (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
				CAuxFunctions func;
				processDlg.node_id = (CString)pDoc->netData.getID(pDoc->selectedComponents[0]).c_str();
				processDlg.normalize_only = false;
				if (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "PreProcess") {
					CAuxFunctions func;
					processDlg.input_data = _T("");
					for (int i = 0; i < pDoc->netData.size(); i++) {
						if (pDoc->netData.isConnect(i)) {
							if (pDoc->netData.getOutputNode(i) == string(CT2CA(processDlg.node_id))) {
								bool mwave = false;
								bool mtext = false;
								bool mseries = false;
								processDlg.input_node_id = (CString)func.getConvNameFromConnectName(pDoc->netData.getInputNode(i)).c_str();
								int index = pDoc->netData.getIndexFromID(string(CT2CA(processDlg.input_node_id)));
								if (pDoc->netData.isConvLayer(index) && (pDoc->netData.getNodeMode(index) == "Input") && (pDoc->netData.getConvType(index) == SOUND_CONV)) {
									processDlg.input_data = _T("Wave_Data");
									mwave = true;
								}
								else if (pDoc->netData.isConvLayer(index) && (pDoc->netData.getNodeMode(index) == "Input") && (pDoc->netData.getConvType(index) == TEXT_CONV)) {
									processDlg.input_data = _T("Text_Data");
									mtext = true;
								}
								else if (pDoc->netData.isConvLayer(index) && (pDoc->netData.getNodeMode(index) == "Input") && (pDoc->netData.getConvType(index) == DATA_CONV)) {
									processDlg.input_data = _T("Series_Data");
									processDlg.normalize_only = true;
									mseries = true;
								}
								else {
									processDlg.input_data = (CString)pDoc->netData.getDataField(index).c_str();
									if (processDlg.input_data == _T("")) {
										processDlg.input_data = (CString)pDoc->netData.getInputProcessNode(pDoc->selectedComponents[0]).c_str();
									}
								}
								bool lock_one = true;
								if (pDoc->getDataPathType() == FILE_TYPE_WAV) {
									lock_one = false;
								}
								processDlg.setLowerNormalizationFactor(pDoc->netData.getNodeLowerNormLimit(pDoc->selectedComponents[0]));
								processDlg.setUpperNormalizationFactor(pDoc->netData.getNodeUpperNormLimit(pDoc->selectedComponents[0]));
								processDlg.setRealLowerNormalizationFactor(pDoc->getNodeLowerNormalizationFactor(pDoc->selectedComponents[0]));
								processDlg.setRealUpperNormalizationFactor(pDoc->getNodeUpperNormalizationFactor(pDoc->selectedComponents[0]));
								processDlg.setTime((CString)pDoc->netData.getNodePreProcessorTime(pDoc->selectedComponents[0]).c_str());
								if ((processDlg.getMode().Left(3) == _T("FFT"))) {

								}
								processDlg.setStride(pDoc->netData.getNodePreProcessorStride(pDoc->selectedComponents[0]), lock_one);
								processDlg.setDownSample(pDoc->netData.getNodePreProcessorDownSample(pDoc->selectedComponents[0]));
								processDlg.setOffsetIndex(pDoc->netData.getNodeXIndex(pDoc->selectedComponents[0]));
								//processDlg.setWordKey(pDoc->netData.getDataField(pDoc->selectedComponents[0]));
								processDlg.setNameKey((CString)pDoc->netData.getDataField(pDoc->selectedComponents[0]).c_str());
								processDlg.setOutput(false);
								processDlg.setMode((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str(), pDoc->getDataPathType());
								processDlg.setTab((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str());
								processDlg.stringdata.clear();
								processDlg.doubledata.clear();
								if (mwave) {
									if (pDoc->getDataPathNamesSize(false) > 0) {
										vector<double> ddata;
										wave_data wdata = func.getWaveDataFromFile(pDoc->getDataPathName(0, false));
										if (wdata.length > 0) {
											int data_length = wdata.length;
											if (data_length > 0) {
												for (int j = 0; j < wdata.length; j++) {
													ddata.push_back(wdata.data[j]);
												}
											}
										}
										processDlg.doubledata.push_back(ddata);
									}
								}
								else if (mtext) {
								}
								else if (mseries) {
									vector<string> data_fields = pDoc->netData.getDataFields(index);
									if (data_fields.size() == 1) {
										processDlg.input_data = (CString)data_fields[0].c_str();
										func.getStackedColumnDataFromDataSources((CString)data_fields[0].c_str(), &processDlg.stringdata, &processDlg.doubledata);
										CString s = _T("");
										s.Format(_T("stringdata size = %i : doubledata size = %i (1)"), (int)processDlg.stringdata.size(), (int)processDlg.doubledata.size());
										addErrorString(s);
									}
									else if (data_fields.size() > 1) {
										processDlg.input_data = (CString)(data_fields[0] + " - " + data_fields[data_fields.size() - 1]).c_str();
										for (int k = 0; k < data_fields.size(); k++) {
											vector<vector<double>> ddata;
											vector<vector<CString>> sdata;
											func.getStackedColumnDataFromDataSources((CString)data_fields[k].c_str(), &sdata, &ddata);
											if (sdata.size() == ddata.size()) {
												for (int m = 0; m < sdata.size(); m++) {
													processDlg.doubledata.push_back(ddata[m]);
													processDlg.stringdata.push_back(sdata[m]);
												}
											}
											else {
												addErrorString(_T("updateDataToEquation : data sizemismatch"));
											}
										}
									}
								}
								else {
									func.getStackedColumnDataFromDataSources(processDlg.input_data, &processDlg.stringdata, &processDlg.doubledata);
								}
								processDlg.setDataToDialogs();
								if (pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]) == "FieldKey") {
									processDlg.setNameKey((CString)pDoc->netData.getNodePreProcessorTime(pDoc->selectedComponents[0]).c_str());
								}
								else if (pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]) == "FNParser") {
									processDlg.setNameKey((CString)pDoc->netData.getDataField(pDoc->selectedComponents[0]).c_str());
								}
								i = pDoc->netData.size();
							}
						}
					}
					if (processDlg.input_data == _T("")) {
						addErrorString(_T("PreProcess node needs connection to input node with data reference : ") + processDlg.node_id);
					}
				}
				else if (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output") {
					bool ok = true;
					bool mtextfiles = false;
					bool mimagefiles = false;
					if ((pDoc->getDataPathType() == FILE_TYPE_JPG) || (pDoc->getDataPathType() == FILE_TYPE_WAV)) {
						mimagefiles = true;
					}
					else if (pDoc->getDataPathType() == FILE_TYPE_TXT) {
						mtextfiles = true;
					}
					else {
						processDlg.input_node_id = (CString)pDoc->netData.getID(pDoc->selectedComponents[0]).c_str();
						if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV)) {
							processDlg.normalize_only = true;
							vector<string> data_fields = pDoc->netData.getDataFields(pDoc->selectedComponents[0]);
							if (data_fields.size() == 1) {
								processDlg.input_data = (CString)data_fields[0].c_str();
							}
							else if (data_fields.size() > 1) {
								processDlg.input_data = (CString)(data_fields[0] + " - " + data_fields[data_fields.size() - 1]).c_str();
							}
							else {
								addErrorString(_T("Output PreProcess needs data reference : ") + processDlg.node_id);
								ok = false;
							}
						}
						else {
							processDlg.input_data = (CString)pDoc->netData.getDataField(pDoc->selectedComponents[0]).c_str();
						}
						if (processDlg.input_data == _T("")) {
							addErrorString(_T("Output PreProcess needs data reference : ") + processDlg.node_id);
							ok = false;
						}
					}
					if (mimagefiles) {
						processDlg.setRealLowerNormalizationFactor(pDoc->getNodeLowerNormalizationFactor(pDoc->selectedComponents[0]));
						processDlg.setRealUpperNormalizationFactor(pDoc->getNodeUpperNormalizationFactor(pDoc->selectedComponents[0]));
						processDlg.setOutput(true);
						processDlg.setMode((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str(), pDoc->getDataPathType());
						processDlg.setTab((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str());
						processDlg.setDataToDialogs();
						processDlg.setNameKey((CString)pDoc->netData.getDataField(pDoc->selectedComponents[0]).c_str());
					}
					else if (mtextfiles) {
						processDlg.setOffsetIndex(pDoc->netData.getNodeXIndex(pDoc->selectedComponents[0]));
						processDlg.setOutput(true);
						processDlg.setMode((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str(), pDoc->getDataPathType());
						processDlg.setTab((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str());
						processDlg.setDataToDialogs();
						processDlg.setNameKey((CString)pDoc->netData.getDataField(pDoc->selectedComponents[0]).c_str());
					}
					else {
						if (ok) {
							processDlg.setLowerNormalizationFactor(pDoc->netData.getNodeLowerNormLimit(pDoc->selectedComponents[0]));
							processDlg.setUpperNormalizationFactor(pDoc->netData.getNodeUpperNormLimit(pDoc->selectedComponents[0]));
							processDlg.setRealLowerNormalizationFactor(pDoc->getNodeLowerNormalizationFactor(pDoc->selectedComponents[0]));
							processDlg.setRealUpperNormalizationFactor(pDoc->getNodeUpperNormalizationFactor(pDoc->selectedComponents[0]));
							processDlg.setStride(pDoc->netData.getNodePreProcessorStride(pDoc->selectedComponents[0]), true);
							processDlg.setDownSample(pDoc->netData.getNodePreProcessorDownSample(pDoc->selectedComponents[0]));
							processDlg.setOutput(true);
							processDlg.setMode((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str(), pDoc->getDataPathType(pDoc->getDataPathNames(false)));
							processDlg.setTab((CString)pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]).c_str());
							processDlg.stringdata.clear();
							processDlg.doubledata.clear();
							if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV)) {
								vector<string> data_fields = pDoc->netData.getDataFields(pDoc->selectedComponents[0]);
								if (data_fields.size() == 1) {
									processDlg.input_data = (CString)data_fields[0].c_str();
									func.getStackedColumnDataFromDataSources((CString)data_fields[0].c_str(), &processDlg.stringdata, &processDlg.doubledata);
									CString s = _T("");
									s.Format(_T("stringdata size = %i : doubledata size = %i (2)"), (int)processDlg.stringdata.size(), (int)processDlg.doubledata.size());
									addErrorString(s);
								}
								else if (data_fields.size() > 1) {
									processDlg.input_data = (CString)(data_fields[0] + " - " + data_fields[data_fields.size() - 1]).c_str();
									for (int k = 0; k < data_fields.size(); k++) {
										vector<vector<double>> ddata;
										vector<vector<CString>> sdata;
										func.getStackedColumnDataFromDataSources((CString)data_fields[k].c_str(), &sdata, &ddata);
										if (sdata.size() == ddata.size()) {
											for (int m = 0; m < sdata.size(); m++) {
												processDlg.doubledata.push_back(ddata[m]);
												processDlg.stringdata.push_back(sdata[m]);
											}
										}
										else {
											addErrorString(_T("updateDataToEquation : data sizemismatch"));
										}
									}
								}
							}
							else {
								func.getStackedColumnDataFromDataSources(processDlg.input_data, &processDlg.stringdata, &processDlg.doubledata);
							}
							if (pDoc->netData.getNodeFunction(pDoc->selectedComponents[0]) == "Tanh") {
								processDlg.tanh = true;
							}
							else {
								processDlg.tanh = false;
							}
							processDlg.setDataToDialogs();
							if (pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]) == "FieldKey") {
								processDlg.setNameKey((CString)pDoc->netData.getNodePreProcessorTime(pDoc->selectedComponents[0]).c_str());
							}
							else if (pDoc->netData.getNodePreProcessor(pDoc->selectedComponents[0]) == "FNParser") {
								processDlg.setNameKey((CString)pDoc->netData.getDataField(pDoc->selectedComponents[0]).c_str());
							}
						}
					}
				}
			}
			else {
				clearEquation();
			}
		}
		else {
			clearEquation();
		}
	}
}

void CNetDesignerView::updateDataFromEquation()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (processDlg.isDisplayed() && !pDoc->training && !pDoc->verifying) {
		if (pDoc->selectedComponents.size() == 1) {
			if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "PreProcess") || (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
				CString pmode = processDlg.getMode();
				CString pmode2 = pmode.Left(10);
				CString pmode3 = pmode.Left(3);
				CString pmode8 = pmode.Left(8);
				CString pmode7 = pmode.Left(7);
				if (((pmode == _T("NormLinear")) || (pmode == _T("NormLog"))) && (pDoc->selectedComponents.size() == 1)) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodeLowerNormLimit(processDlg.getLowerNormalizationFactor());
						tdata.setNodeUpperNormLimit(processDlg.getUpperNormalizationFactor());
						tdata.setNodeLowerNormalizationFactor(processDlg.getRealLowerNormalizationFactor());
						tdata.setNodeUpperNormalizationFactor(processDlg.getRealUpperNormalizationFactor());
						tdata.setNodePreProcessorTime(string(CT2CA(processDlg.getTime())));
						pDoc->pushEditConvLayer(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
					else {
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodeLowerNormLimit(processDlg.getLowerNormalizationFactor());
						tdata.setNodeUpperNormLimit(processDlg.getUpperNormalizationFactor());
						tdata.setNodeLowerNormalizationFactor(processDlg.getRealLowerNormalizationFactor());
						tdata.setNodeUpperNormalizationFactor(processDlg.getRealUpperNormalizationFactor());
						tdata.setNodePreProcessorTime(string(CT2CA(processDlg.getTime())));
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
				else if ((pDoc->selectedComponents.size() == 1) && ((pmode == _T("NormSeconds")) || (pmode == _T("NormMinutes")) ||
					(pmode == _T("NormHours")) || (pmode == _T("NormDays")) || (pmode == _T("Seconds")) || (pmode == _T("Minutes")) ||
					(pmode == _T("Hours")) || (pmode == _T("Days")))) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						addErrorString(_T("updateDataFromEquation : Unable to process time for output layer"));
					}
					else {
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodeLowerNormLimit(processDlg.getLowerNormalizationFactor());
						tdata.setNodeUpperNormLimit(processDlg.getUpperNormalizationFactor());
						tdata.setNodeLowerNormalizationFactor(processDlg.getRealLowerNormalizationFactor());
						tdata.setNodeUpperNormalizationFactor(processDlg.getRealUpperNormalizationFactor());
						tdata.setNodePreProcessorTime(string(CT2CA(processDlg.getTime())));
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
				else if ((pDoc->selectedComponents.size() == 1) && (pmode2 == _T("Derivative"))) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						addErrorString(_T("updateDataFromEquation : Unable to process time for derivative layer"));
					}
					else {
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodeLowerNormLimit(processDlg.getLowerNormalizationFactor());
						tdata.setNodeUpperNormLimit(processDlg.getUpperNormalizationFactor());
						tdata.setNodeLowerNormalizationFactor(processDlg.getRealLowerNormalizationFactor());
						tdata.setNodeUpperNormalizationFactor(processDlg.getRealUpperNormalizationFactor());
						tdata.setNodePreProcessorTime(string(CT2CA(processDlg.getTime())));
						tdata.setNodePreProcessorStride(processDlg.getStride());
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
				else if ((pDoc->selectedComponents.size() == 1) && (pmode3 == _T("FFT"))) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						addErrorString(_T("updateDataFromEquation : Unable to process FFT for output layer"));
					}
					else {
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodeLowerNormLimit(processDlg.getLowerNormalizationFactor());
						tdata.setNodeUpperNormLimit(processDlg.getUpperNormalizationFactor());
						tdata.setNodeLowerNormalizationFactor(processDlg.getRealLowerNormalizationFactor());
						tdata.setNodeUpperNormalizationFactor(processDlg.getRealUpperNormalizationFactor());
						tdata.setNodePreProcessorTime(string(CT2CA(processDlg.getTime())));
						tdata.setNodePreProcessorStride(processDlg.getStride());
						tdata.setNodePreProcessorDownSample(processDlg.getDownSample());
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
				else if ((pDoc->selectedComponents.size() == 1) && (pmode8 == _T("FieldKey"))) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						addErrorString(_T("updateDataFromEquation : Unable to process FieldKey for output layer"));
					}
					else {
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodePreProcessorTime(string(CT2CA(processDlg.getNameKey())));
						tdata.setNodeLowerNormLimit(processDlg.getLowerNormalizationFactor());
						tdata.setNodeUpperNormLimit(processDlg.getUpperNormalizationFactor());
						tdata.setNodeLowerNormalizationFactor(processDlg.getRealLowerNormalizationFactor());
						tdata.setNodeUpperNormalizationFactor(processDlg.getRealUpperNormalizationFactor());
						//tdata.setDataField(processDlg.getNameKey());
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
				else if ((pDoc->selectedComponents.size() == 1) && (pmode8 == _T("FNParser"))) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						addErrorString(_T("updateDataFromEquation : Unable to process FNParser for output layer"));
					}
					else {
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setDataField(string(CT2CA(processDlg.getNameKey())));
						tdata.setNodeLowerNormLimit(processDlg.getLowerNormalizationFactor());
						tdata.setNodeUpperNormLimit(processDlg.getUpperNormalizationFactor());
						tdata.setNodeLowerNormalizationFactor(processDlg.getRealLowerNormalizationFactor());
						tdata.setNodeUpperNormalizationFactor(processDlg.getRealUpperNormalizationFactor());
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
				else if ((pDoc->selectedComponents.size() == 1) && (pmode8 == _T("WordInde"))) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						addErrorString(_T("updateDataFromEquation : Unable to process WordIndex for output layer"));
					}
					else {
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodeXIndex(processDlg.getOffsetIndex());
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
				/*else if ((pDoc->selectedComponents.size() == 1) && (pmode == _T("WordKey"))) {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
					tdata.setNodePreProcessor(pmode);
					tdata.setDataField(processDlg.getWordKey());
					tdata.setNodeXIndex(processDlg.getOffsetIndex());
					pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
					pDoc->pushEditNodeData(tdata);
					pDoc->processEditData();
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
					pMainWnd->m_wndNodeView.FillFileView();
				}*/
				else if ((pDoc->selectedComponents.size() == 1) && (pmode == _T("Dictionary"))) {
					if (pDoc->netData.isConvLayer(pDoc->selectedComponents[0]) && (pDoc->netData.getConvType(pDoc->selectedComponents[0]) == DATA_CONV) && (pDoc->netData.getNodeMode(pDoc->selectedComponents[0]) == "Output")) {
						addErrorString(_T("updateDataFromEquation : Unable to process Dictionary for output layer"));
					}
					else {
						netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[0]);
						tdata.setNodePreProcessor(string(CT2CA(pmode)));
						tdata.setNodeXIndex(0);
						pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
						pDoc->pushEditNodeData(tdata);
						pDoc->processEditData();
						CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
						pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
						pMainWnd->m_wndNodeView.FillFileView();
					}
				}
			}
		}
	}
}

void CNetDesignerView::clearEquation()
{
	if (processDlg.isDisplayed()) {
		CNetDesignerDoc* pDoc = GetDocument();
		processDlg.setMode(_T("NormLinear"), pDoc->getDataPathType());
		processDlg.input_node_id = _T("");
		processDlg.input_node_id = _T("");
		processDlg.input_data = _T("");
		processDlg.setLowerNormalizationFactor(0);
		processDlg.setUpperNormalizationFactor(1);
		processDlg.setRealLowerNormalizationFactor(pDoc->sigmoid_leakage);
		processDlg.setRealUpperNormalizationFactor(1.0 - pDoc->sigmoid_leakage);
		processDlg.setTime(_T(""));
		processDlg.setStride(1, true);
		processDlg.setDownSample(1);
		processDlg.stringdata.clear();
		vector<CString> sdata;
		processDlg.stringdata.push_back(sdata);
		processDlg.doubledata.clear();
		vector<double> ddata;
		processDlg.doubledata.push_back(ddata);
		processDlg.setDataToDialogs();
	}
}

void CNetDesignerView::OnUpdateEquation(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnEditUndo()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->undoLast(zoomScale, lHorzScollPos, lVertScollPos)) {
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndNodeView.FillFileView();
	}
}

void CNetDesignerView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((pDoc->backData.size() > 0) && !pDoc->training && !pDoc->clustering && !pDoc->verifying) {
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
		pMainWnd->m_wndNodeView.FillFileView();
	}
}

void CNetDesignerView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((pDoc->redoData.size() > 0) && !pDoc->training && !pDoc->clustering && !pDoc->verifying) {
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
	if (!pDoc->training && !pDoc->verifying) {
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
	bool yes = false;
	bool outputconv = false;
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNode(i)) {
			if ((pDoc->netData.getNodeMode(i) == "PreProcess") || (pDoc->netData.getNodeMode(i) == "Output")) {
				if ((pDoc->netData.getNodePreProcessor(i) == "FNParser") || (pDoc->netData.getNodePreProcessor(i) == "FieldKey")) {
					yes = true;;
				}
			}
		}
		else if (pDoc->netData.isConvLayer(i)) {
			if (pDoc->netData.getNodeMode(i) == "Output") {
				if (pDoc->netData.getConvType(i) == IMAGE_CONV) {
					if (pDoc->getDataPathType() == FILE_TYPE_JPG) {
						outputconv = true;
					}
				}
			}
		}
	}
	if (outputconv) {
		AfxMessageBox(_T("Unable to show data output for image outputs"));
	}
	else {
		NetOutputDlg dlg;
		dlg.setClassification(yes);
		dlg.getData();
		dlg.DoModal();
	}
}

void CNetDesignerView::OnUpdateOutputData(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnAutoroute()
{
	routing = true;
	route_angle = 0;
	route_origin.x = 0;
	route_origin.y = 0;
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() > 0) {
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
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying && !connecting && !selection_box && (placing_component_type == _T("")) && !routing &&
		(move_state != MOVE_MOVING) && (paste_state != PASTE_MOVING)) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnKmeans()
{
	clusterdlg.CreateDlg();
}

void CNetDesignerView::OnUpdateKmeans(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnPca()
{
	AfxMessageBox(_T("Node cluster analysis under construction"));
}

void CNetDesignerView::OnUpdatePca(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnNodeAnalysis()
{
	AfxMessageBox(_T("Node analysis under construction"));
}

void CNetDesignerView::OnUpdateNodeAnalysis(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnGraphOutputs()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->display_conv_images) {
		pDoc->display_conv_images = false;
	}
	else {
		pDoc->display_conv_images = true;
	}
}

void CNetDesignerView::OnUpdateGraphOutputs(CCmdUI* pCmdUI)
{
	//CNetDesignerDoc* pDoc = GetDocument();
	//if (!pDoc->training) {
		pCmdUI->Enable(true);
	//}
	//else {
		//pCmdUI->Enable(false);
	//}
}


void CNetDesignerView::OnTest()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->new_net) {
		int ninputs = pDoc->nnet.num_inputs;
		int noutputs = pDoc->nnet.num_outputs;
		if ((ninputs == 0) || (noutputs == 0)) {
			AfxMessageBox(_T("Can't evaluate net : num inputs or num outputs = 0"));
			return;
		}
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
	if (!pDoc->training && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEvaluateFile()
{
	CEvaluateFileDlg dlg;
	dlg.DoModal();
}

void CNetDesignerView::OnUpdateEvaluateFile(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->verifying) {
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
	if (!pDoc->training && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnEnableTrain()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (checkSelectedNodesAreTrainable()) {
		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Output")) {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeTrainable(false);
					pDoc->pushEditNodeData(tdata);
				}
			}
			else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Output")) {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeTrainable(false);
					pDoc->pushEditConvLayer(tdata);
				}
			}
		}
		pDoc->processEditData();
	}
	else {
		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Output")) {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeTrainable(true);
					pDoc->pushEditNodeData(tdata);
				}
			}
			else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Output")) {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeTrainable(true);
					pDoc->pushEditNodeData(tdata);
					pDoc->pushEditConvLayer(tdata);
				}
			}
		}
		pDoc->processEditData();
	}
}

void CNetDesignerView::OnUpdateEnableTrain(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
	if (checkSelectedNodesAreTrainable()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnEnableZeroOutput()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (checkSelectedNodesAreZeroizable()) {
		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Input") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "PreProcess")) {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeZeroOutput(false);
					pDoc->pushEditNodeData(tdata);
				}
			}
			else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
				if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeZeroOutput(false);
					pDoc->pushEditConvLayer(tdata);
				}
			}
		}
		pDoc->processEditData();
	}
	else {
		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
				if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Input") || 
					(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "PreProcess")) {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeZeroOutput(true);
					pDoc->pushEditNodeData(tdata);
				}
			}
			else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
				if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") {
					netDataStruct tdata = pDoc->netData.getNode(pDoc->selectedComponents[i]);
					tdata.setNodeZeroOutput(true);
					pDoc->pushEditNodeData(tdata);
					pDoc->pushEditConvLayer(tdata);
				}
			}
		}
		pDoc->processEditData();
	}
}

void CNetDesignerView::OnUpdateEnableZeroOutput(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->clustering && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
	if (checkSelectedNodesAreZeroizable()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CNetDesignerView::OnSelectLayerData()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() == 1) {
		CNetDesignerDoc* pDoc = GetDocument();
		pDoc->getLayerDataConnections(pDoc->selectedComponents[0]);
	}
}

void CNetDesignerView::OnUpdateSelectLayerData(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->verifying) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

bool CNetDesignerView::checkSelectedNodesAreTrainable()
{
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
		if (pDoc->netData.isNode(pDoc->selectedComponents[i]) || pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
			if ((pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") ||
				(pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Output")) {
				if (!pDoc->netData.getNodeTrainable(pDoc->selectedComponents[i])) {
					return(false);
				}
			}
		}
	}
	return(true);
}

bool CNetDesignerView::checkSelectedNodesAreZeroizable()
{
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
		if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
			if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") {
				if ((pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]) != "MaxPool") &&
					(pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]) != "Multiply") &&
					(pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]) != "Add")) {
					if (pDoc->netData.getNodeZeroOutput(pDoc->selectedComponents[i])) {
						return(true);
					}
				}
			}
			else if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Input") {
				if (pDoc->netData.getNodeZeroOutput(pDoc->selectedComponents[i])) {
					return(true);
				}
			}
			else if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "PreProcess") {
				if (pDoc->netData.getNodeZeroOutput(pDoc->selectedComponents[i])) {
					return(true);
				}
			}
		}
		else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
			if (pDoc->netData.getNodeMode(pDoc->selectedComponents[i]) == "Hidden") {
				if (pDoc->netData.getNodeZeroOutput(pDoc->selectedComponents[i])) {
					return(true);
				}
			}
		}
	}
	return(false);
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
	zoomScale += 0.20F;
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (zoomScale > (float)(pMainWnd->m_ribbonSlider.GetRangeMax()/100)) {
		zoomScale = (float)(pMainWnd->m_ribbonSlider.GetRangeMax()/100);
	}
	pMainWnd->m_ribbonSlider.setScale(zoomScale);
	zoomScale = pMainWnd->m_ribbonSlider.getScale();
}

void CNetDesignerView::OnZoomOut()
{
	zoomScale -= 0.20F;
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
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (dlg.DoModal() == IDOK) {
	}
}

void CNetDesignerView::OnHidden()
{
	startPlaceComponent(_T("Hidden Node"));
}

void CNetDesignerView::OnUpdateHidden(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
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
	if (pDoc->training || pDoc->verifying) {
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
	if (pDoc->training || pDoc->verifying) {
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
	if (pDoc->training || pDoc->clustering || pDoc->verifying) {
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
	if (pDoc->training || pDoc->verifying || (pDoc->assembly_edit_index >= 0)) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnConvLayer()
{
	startPlaceComponent(_T("ConvLayer"));
}

void CNetDesignerView::OnUpdateConvLayer(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnAttention()
{
	startPlaceComponent(_T("Attention"));
}

void CNetDesignerView::OnUpdateAttention(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnAttention2()
{
	startPlaceComponent(_T("SelfAttention"));
}

void CNetDesignerView::OnUpdateAttention2(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnAssembly()
{
	startPlaceComponent(_T("Assembly"));
}

void CNetDesignerView::OnUpdateAssembly(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnNetCluster()
{
	startPlaceComponent(_T("NetCluster"));
}

void CNetDesignerView::OnUpdateNetCluster(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnDropout()
{
	startPlaceComponent(_T("DropOut"));
}

void CNetDesignerView::OnUpdateDropout(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnSaveNetCluster()
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();
	if (!pDoc->training && !pDoc->verifying && ((int)pDoc->selectedComponents.size() == 1)) {

		CString cid = (CString)pDoc->netData.getID(pDoc->selectedComponents[0]).c_str();

		if (pDoc->dataPathnames.size() == 1) {

			if (pDoc->getDataPathType() == FILE_TYPE_CSV) {
				CString fname = pDoc->GetPathName();  //addErrorString(_T("fname = ") + fname);
				CString fname2 = func.removeExtensionFromFilename(fname) + _T("_") + cid + _T("_NetPatterns.csv");
				CFileDialog dlg2(FALSE, _T("csv"), fname2, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("CSV Files (*.csv)|*.csv||"));
				if (dlg2.DoModal() == IDOK) {
					fname2 = dlg2.GetPathName();  //addErrorString(_T("save filename = ") + fname2);
					int csvsize = (int)pDoc->nnet.csvFileData.size();
					if (csvsize > 0) {
						CString s = _T("");
						for (int i = 0; i < csvsize; i++) {
							int dsize = (int)pDoc->nnet.csvFileData[i].set_data.size();
							//s.Format(_T(" : set_data size = %i"), dsize); addErrorString(_T("original filename = ") + (CString)pDoc->nnet.csvFileData[i].file_path.c_str() + s);
							if (dsize > 0) {
								vector<int> clusters = pDoc->nnet.getClustersFromID(string(CT2CA(cid)));
								if ((int)clusters.size() == (int)pDoc->nnet.csvFileData[i].set_data[0].column_string_data.size()) {
									vector<CString> lines;
									CString line = _T("");
									for (int j = 0; j < dsize; j++) {
										if (j > 0) {
											line += _T(",");
										}
										line += (CString)pDoc->nnet.csvFileData[i].set_data[j].header.c_str();
									}
									line += _T(",group");  //addErrorString(line);
									line += _T("\n");
									lines.push_back(line);
									for (int m = 0; m < (int)pDoc->nnet.csvFileData[i].set_data[0].column_string_data.size(); m++) {
										line = _T("");
										for (int j = 0; j < dsize; j++) {
											if (j > 0) {
												line += _T(",");
											}
											line += (CString)pDoc->nnet.csvFileData[i].set_data[j].column_string_data[m].c_str();
										}
										s.Format(_T(",%i"), clusters[m]);
										line += s;  //addErrorString(line);
										line += _T("\n");
										lines.push_back(line);
									}
									func.OnSaveCSV(fname2, lines);
								}
								else {
									AfxMessageBox(_T("Must train patterns to save"));
								}
							}
						}
					}
				}
			}
			else {
				AfxMessageBox(_T("Unable to save net pattern results - need to set up single file names for non csv inputs"));
			}

		}

		else if (pDoc->dataPathnames.size() > 1) {

			if (pDoc->getDataPathType() == FILE_TYPE_JPG) {

				int nclusters = pDoc->nnet.getNumClustersFromID(string(CT2CA(cid)));
				vector<int> clusters = pDoc->nnet.getClustersFromID(string(CT2CA(cid)));
				if ((nclusters > 0) && ((int)clusters.size() > 0)) {

					/*if (index < (int)imageTrainIndices.size()) {
						int mindex = imageTrainIndices[index];
						if (mindex < num_train_images) {
							setInputImageData(image_train_data + ((int64_t)mindex * 3 * image_single));
							data_file_index = mindex;
						}
						else {
							char s[200];  sprintf_s(s, "loadTrainImageToNet : mindex = %i : num_train_images = %i", mindex, num_train_images);
							addErrorString(s);
						}
					}*/


					if (pDoc->nnet.imageTrainIndices.size() == clusters.size()) {

						CString mfolder = _T("");
						vector<CString> mpnames = pDoc->getDataPathNames();
						if (mpnames.size() > 0) {
							mfolder = mpnames[0];
						}
						mfolder = func.getPathFromFilename(mfolder);
						mfolder = func.getDataFolder(mfolder);
						if (mfolder != _T("")) {

							mfolder += _T("\\Patterns\\");

							if (func.directoryExists(mfolder)) {
								if (AfxMessageBox(_T("Directory ") + mfolder + _T(" exists - delete?"), MB_YESNO) == IDYES) {
									func.RecursiveDelete(mfolder);
								}
							}

							bool fcreated = false;
							if (func.createFolder(mfolder)) {
								fcreated = true;
								for (int i = 0; i < nclusters; i++) {
									CString s = _T(""); s.Format(_T("\\Pattern%i"), i);
									if (!func.createFolder(mfolder + s)) {
										fcreated = false;
										i = nclusters;
									}
								}
							}
							if (!fcreated) {
								addErrorString(_T("OnSaveNetCluster : unable to create folders for ") + cid);
								return;
							}

							int update = 10;
							CStatus myProgressDlg;
							myProgressDlg.CreateDlg();
							myProgressDlg.setLabel(_T("Copying patterned files . . ."));
							for (int i = 0; i < (int)pDoc->nnet.imageTrainIndices.size(); i++) {

								CString s = _T(""); s.Format(_T("Pattern%i\\"), clusters[i]);  //addErrorString(s);
								CString to = mfolder + s + func.removePathFromFilename(pDoc->dataPathnames[pDoc->nnet.imageTrainIndices[i]]);

								if (!func.copyFile(pDoc->dataPathnames[pDoc->nnet.imageTrainIndices[i]], to)) {
									addErrorString(_T("Unable to copy ") + pDoc->dataPathnames[i] + _T(" to ") + to);
									myProgressDlg.DestroyDlg();
									return;
								}

								if (i >= update) {
									int per = (int)((100 * (float)i) / (float)pDoc->dataPathnames.size());
									myProgressDlg.setProgress(per);
									update += 10;
								}

							}
							myProgressDlg.DestroyDlg();

							addErrorString(_T("Saving pattern files complete"));

						}
						else {
							addErrorString(_T("OnSaveNetCluster : num files must equal num pattern indices"));
						}

					}
					else {
						AfxMessageBox(_T("imageTrainIndices size and clusters size don't match"));
					}


				}

			}
			else {
				AfxMessageBox(_T("Unable to save net pattern results - need to set up multiple files names other than jpg"));
			}

		}

		else {
			AfxMessageBox(_T("Unable to save net pattern results - no data files"));
		}

	}
}

void CNetDesignerView::OnUpdateSaveNetCluster(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnEditAssembly()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->group_zoom_index < (pDoc->group_zoom_index_max - 1)) {
		getAssemblyIndex();
		if (pDoc->assembly_edit_index >= 0) {
			showSelectedomponents(true);
		}
		pDoc->group_zoom_index++;
		pDoc->group_zoom_indices[pDoc->group_zoom_index] = pDoc->assembly_edit_index;
		if (pDoc->assembly_edit_index >= 0) {
			CString s = _T("");
			s.Format(_T("Assy%i"), pDoc->no_sub_assembly_edit_index);
			addPanel2String(s);
		}
		else {
			addPanel2String(_T(""));
		}
	}
}

void CNetDesignerView::OnUpdateEditAssembly(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying || !pDoc->assembly_editable) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnCloseAssembly()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->assembly_edit_index >= 0) {
		pDoc->clearAssemblyInputOutputCounts(pDoc->assembly_edit_index);
	}
	if (pDoc->group_zoom_index > 0) {
		pDoc->group_zoom_index--;
	}
	pDoc->assembly_edit_index = pDoc->group_zoom_indices[pDoc->group_zoom_index];
	if (pDoc->assembly_edit_index >= 0) {
		CString s = _T("");
		s.Format(_T("Assy%i"), pDoc->no_sub_assembly_edit_index);
		addPanel2String(s);
	}
	else {
		addPanel2String(_T(""));
	}
	showSelectedomponents(true);
}

void CNetDesignerView::OnUpdateCloseAssembly(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->assembly_edit_index >= 0) {
		pCmdUI->Enable(true);
	}
	else {
		pCmdUI->Enable(false);
	}
}

void CNetDesignerView::OnInputLayer()
{
	startPlaceComponent(_T("InputLayer"));
}

void CNetDesignerView::OnUpdateInsertHiddenLayers(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnInsertHiddenLayers()
{
	startPlaceComponent(_T("HiddenLayers"));
}

void CNetDesignerView::OnUpdateInputLayer(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnInsertOutputLayer()
{
	startPlaceComponent(_T("OutputLayer"));
}

void CNetDesignerView::OnUpdateInsertOutputLayer(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnCsvData()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->OnDataView();
}

void CNetDesignerView::OnUpdateCsvData(CCmdUI* pCmdUI)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->training || pDoc->verifying) {
		pCmdUI->Enable(false);
	}
	else {
		pCmdUI->Enable(true);
	}
}

void CNetDesignerView::OnClear()
{
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.clear();
	pMainWnd->m_wndGraph.reset();
	//pDoc->trainDataX.clear();
	//pDoc->trainDataY.clear();
	//pDoc->trainDataY2.clear();
	//pDoc->updateTrainGraph();
}



// CNetDesignerView drawing functions

void CNetDesignerView::addErrorString(string err)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.ShowPane(true, false, true);
	pMainWnd->m_wndOutput.setTab(1);
	pMainWnd->m_wndOutput.AddStatusString((CString)err.c_str());
}

void CNetDesignerView::addPanel1String(string msg, CPoint pt)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (pt != CPoint(0, 0)) {
		CString msgpt = _T("");
		msgpt.Format(_T(" (%i, %i)"), pt.x, pt.y);
		pMainWnd->SetStatusBarText1((CString)msg.c_str() + msgpt);
	}
	else {
		pMainWnd->SetStatusBarText1((CString)msg.c_str());
	}
}

void CNetDesignerView::addPanel2String(string msg, CPoint pt)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (pt != CPoint(0, 0)) {
		CString msgpt = _T("");
		msgpt.Format(_T(" (%i, %i)"), pt.x, pt.y);
		pMainWnd->SetStatusBarText2((CString)msg.c_str() + msgpt);
	}
	else {
		pMainWnd->SetStatusBarText2((CString)msg.c_str());
	}
}

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

void CNetDesignerView::addPanel2String(CString msg, CPoint pt)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (pt != CPoint(0, 0)) {
		CString msgpt = _T("");
		msgpt.Format(_T(" (%i, %i)"), pt.x, pt.y);
		pMainWnd->SetStatusBarText2(msg + msgpt);
	}
	else {
		pMainWnd->SetStatusBarText2(msg);
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
	CNetDesignerDoc* pDoc = GetDocument();

	CRect mrect(-(int)pDoc->nradius, -(int)pDoc->nradius, (int)pDoc->nradius, (int)pDoc->nradius);

	int cnt = 0;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.getGroupIndex(i) == pDoc->assembly_edit_index) {
			cnt++;
		}
	}
	if (cnt > 0) {
		if (all) {
			mrect = getAllComponentsLogicalRectangle();
		}
		else {
			mrect = getSelectedComponentsLogicalRectangle();
		}
	}

	CRect crect;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.GetClipBox(crect);

	float hzoom = 0.90F * ((float)crect.Width() / (float)(mrect.right - mrect.left));
	float vzoom = 0.90F * ((float)crect.Height() / (float)(mrect.bottom - mrect.top));

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
		clr_hiddennode7 = clr_lthiddennode7;
		clr_hiddennode8 = clr_lthiddennode8;
		clr_hiddennode9 = clr_lthiddennode9;
		clr_hiddennode10 = clr_lthiddennode10;
		clr_hiddennode11 = clr_lthiddennode11;
		clr_hiddennode12 = clr_lthiddennode12;
		clr_hiddennode13 = clr_lthiddennode13;
		clr_connect = clr_ltconnect;
		clr_input = clr_ltinput;
		clr_buffer = clr_ltbuffer;
		clr_text = clr_lttext;
		clr_inactive = clr_ltinactive;
		clr_zero = clr_ltzero;
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
		clr_hiddennode7 = clr_blhiddennode7;
		clr_hiddennode8 = clr_blhiddennode8;
		clr_hiddennode9 = clr_blhiddennode9;
		clr_hiddennode10 = clr_blhiddennode10;
		clr_hiddennode11 = clr_blhiddennode11;
		clr_hiddennode12 = clr_blhiddennode12;
		clr_hiddennode13 = clr_blhiddennode13;
		clr_connect = clr_blconnect;
		clr_input = clr_blinput;
		clr_buffer = clr_blbuffer;
		clr_text = clr_bltext;
		clr_inactive = clr_blinactive;
		clr_zero = clr_blzero;
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
		clr_hiddennode7 = clr_blhiddennode7;
		clr_hiddennode8 = clr_blhiddennode8;
		clr_hiddennode9 = clr_blhiddennode9;
		clr_hiddennode10 = clr_blhiddennode10;
		clr_hiddennode11 = clr_blhiddennode11;
		clr_hiddennode12 = clr_blhiddennode12;
		clr_hiddennode13 = clr_blhiddennode13;
		clr_connect = clr_blconnect;
		clr_input = clr_blinput;
		clr_buffer = clr_blbuffer;
		clr_text = clr_bltext;
		clr_inactive = clr_blinactive;
		clr_zero = clr_blzero;
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
		clr_hiddennode7 = clr_dkhiddennode7;
		clr_hiddennode8 = clr_dkhiddennode8;
		clr_hiddennode9 = clr_dkhiddennode9;
		clr_hiddennode10 = clr_dkhiddennode10;
		clr_hiddennode11 = clr_dkhiddennode11;
		clr_hiddennode12 = clr_dkhiddennode12;
		clr_hiddennode13 = clr_dkhiddennode13;
		clr_connect = clr_dkconnect;
		clr_input = clr_dkinput;
		clr_buffer = clr_dkbuffer;
		clr_text = clr_dktext;
		clr_inactive = clr_dkinactive;
		clr_zero = clr_dkzero;
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
		clr_hiddennode7 = clr_lthiddennode7;
		clr_hiddennode8 = clr_lthiddennode8;
		clr_hiddennode9 = clr_lthiddennode9;
		clr_hiddennode10 = clr_lthiddennode10;
		clr_hiddennode11 = clr_lthiddennode11;
		clr_hiddennode12 = clr_lthiddennode12;
		clr_hiddennode13 = clr_lthiddennode13;
		clr_connect = clr_ltconnect;
		clr_input = clr_ltinput;
		clr_buffer = clr_ltbuffer;
		clr_text = clr_lttext;
		clr_inactive = clr_ltinactive;
		clr_zero = clr_ltzero;
	}
	if (!pDoc->metalNodes) {
		clr_outnode = clr_mtloutnode;
		clr_hiddennode1 = clr_mtlhiddennode1;
		clr_hiddennode2 = clr_mtlhiddennode2;
		clr_hiddennode3 = clr_mtlhiddennode3;
		clr_hiddennode4 = clr_mtlhiddennode4;
		clr_hiddennode5 = clr_mthiddennode5;
		clr_hiddennode6 = clr_mthiddennode6;
		clr_hiddennode7 = clr_mthiddennode7;
		clr_hiddennode8 = clr_mthiddennode8;
		clr_hiddennode9 = clr_mthiddennode9;
		clr_hiddennode10 = clr_mthiddennode10;
		clr_hiddennode11 = clr_mthiddennode11;
		clr_hiddennode12 = clr_mthiddennode12;
		clr_hiddennode13 = clr_mthiddennode13;
		clr_input = clr_mtlinput;
		clr_buffer = clr_mtlbuffer;
		clr_inactive = clr_ltinactive;
		clr_zero = clr_ltzero;
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
	pDoc->hasConvolution();
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
		if (pDoc->netData.isConvLayer(i)) {
			DrawConvolution(pDC, i, zoomScale, DRAW_NORMAL);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isAttentionLayer(i)) {
			DrawAttention(pDC, i, zoomScale, DRAW_NORMAL);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isAssemblyLayer(i)) {
			assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
			DrawAssembly(pDC, i, zoomScale, DRAW_NORMAL, asub);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isNetCluster(i)) {
			DrawNetCluster(pDC, i, zoomScale, DRAW_NORMAL);
		}
	}
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isDropout(i)) {
			DrawDropout(pDC, i, zoomScale, DRAW_NORMAL);
		}
	}
	if (routing) {
		DrawArrow(route_origin.x, route_origin.y, route_angle);
	}
	drawSelectedComponents(DRAW_HIGHLIGHT);
}

void CNetDesignerView::DrawConvolutions(bool redraw_spheres)
{
	CNetDesignerDoc* pDoc = GetDocument();
	CClientDC dc(this);
	OnPrepareDC(&dc);
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConvLayer(i)) {
			if (!(((pDoc->netData.getNodeMode(i) == "Input") || (pDoc->netData.getNodeMode(i) == "Output")) && (pDoc->netData.getConvType(i) == DATA_CONV))) {
				DrawConvolution(&dc, i, zoomScale, DRAW_NORMAL, redraw_spheres);
			}
		}
	}
}

void CNetDesignerView::DrawNode(CDC* pDC, int x, int y, CString type, CString func, float scale, int mode, bool gdi_not)
{
	CNetDesignerDoc* pDoc = GetDocument();
	float nradius = (float)pDoc->nradius;
	int myradius = int(7.0 * scale * nradius / 8.0);	
	COLORREF mclr = 0x000000ff;
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	else if (mode == DRAW_ZERO) {
		mclr = clr_zero;
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
			mclr = clr_hiddennode9;
		}
		else if (func == _T("ACos")) {
			mclr = clr_hiddennode6;
		}
		else if (func == _T("SoftMax")) {
			mclr = clr_hiddennode10;
		}
		else if (func == _T("Gaussian")) {
			mclr = clr_hiddennode8;
		}
		else if (func == _T("Add")) {
			mclr = clr_hiddennode9;
		}
	}
	else if (type == _T("Output")) {
		if (func == _T("SoftMax")) {
			mclr = adjustColorBetween(clr_outnode, clr_hiddennode10, 0.30F);
		}
		else {
			mclr = clr_outnode;
		}
	}
	else if (type == _T("PreProcess")) {
		//addErrorString(_T("CNetDesignerView::DrawNode : func = ") + func);
		mclr = clr_buffer;
		if (func.Left(3) == _T("FFT")) {
			//nradius = 1.1F * nradius;
			//myradius = int(scale * nradius);
			mclr = adjustColorBetween(clr_buffer, clr_solarized_orange2, 0.7F);
		}
		else if ((func.Left(8) == _T("NormDays")) || (func.Left(4) == _T("Days"))) {
			mclr = adjustColorBetween(clr_buffer, clr_solarized_red, 0.7F);
		}
		else if (func.Left(5) == _T("Deriv")) {
			mclr = adjustColorBetween(clr_buffer, clr_solarized_orange2, 0.85F);
		}
	}
	else {
		//CString msg = _T("DrawNode color error - type = ") + type;
		//addErrorString(msg);
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
		drawCircle(pDC, rect, mclr, false, gdi_not, true);
	}
	else {
		draw3dCircle(pDC, x, y, myradius, mclr, gdi_not);
		if ((type == _T("Hidden")) || (type == _T("Output"))) {
			if (func == _T("Multiply")) {
				CPoint pt = LPtoDP(CPoint(x, y));
				int l = pt.x - (11 * myradius) / 32;
				int t = pt.y - (11 * myradius) / 32;
				int r = pt.x + (11 * myradius) / 32;
				int b = pt.y + (11 * myradius) / 32;
				int mywidth = int(scale * pDoc->lwidth);
				CPen pen(PS_SOLID, mywidth, adjustColorByPercent(mclr, -20));
				pDC->SelectObject(pen);
				pDC->MoveTo(l, t);
				pDC->LineTo(r, b);
				pDC->MoveTo(r, t);
				pDC->LineTo(l, b);
			}
			else if (func == _T("Add")) {
				CPoint pt = LPtoDP(CPoint(x, y));
				int l = pt.x - (13 * myradius) / 32;
				int t = pt.y - (13 * myradius) / 32;
				int r = pt.x + (13 * myradius) / 32;
				int b = pt.y + (13 * myradius) / 32;
				int mywidth = int(scale * pDoc->lwidth);
				CPen pen(PS_SOLID, mywidth, adjustColorByPercent(mclr, -20));
				pDC->SelectObject(pen);
				pDC->MoveTo(l, pt.y);
				pDC->LineTo(r, pt.y);
				pDC->MoveTo(pt.x, t);
				pDC->LineTo(pt.x, b);
			}
		}
	}

}

void CNetDesignerView::drawCircle(CDC* pDC, CRect mrect, COLORREF mcolor, bool r2_not, bool gdi_not, bool cartoon_anyway)
{
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
		CNetDesignerDoc* pDoc = GetDocument();
		int diameter = mrect.right - mrect.left;
		Gdiplus::Graphics graphics(*pDC);
		Gdiplus::Color mclr;
		mclr.SetFromCOLORREF(mcolor);
		if (pDoc->cartoon || cartoon_anyway) {
			Gdiplus::SolidBrush brush2(mclr);
			graphics.FillEllipse(&brush2, mrect.left, mrect.top, diameter, diameter);
		}
		else {
			Gdiplus::Pen mpen(mclr, -1);
			graphics.DrawEllipse(&mpen, mrect.left, mrect.top, diameter, diameter);
		}
	}
}

void CNetDesignerView::drawOpenCircle(CDC* pDC, int x, int y, int myradius, COLORREF mclr, bool gdi_not)
{
	CPoint pt = LPtoDP(CPoint(x, y));
	CRect rect(pt.x - myradius, pt.y - myradius, pt.x + myradius, pt.y + myradius);
	drawCircle(pDC, rect, mclr, false, gdi_not);
	/*int diameter = rect.right - rect.left;
	Gdiplus::Graphics graphics(*pDC);
	Gdiplus::Color mclr2;
	mclr2.SetFromCOLORREF(mclr);
	Gdiplus::Pen bluePen(mclr2, 2);
	graphics.DrawEllipse(&bluePen, rect.left, rect.top, diameter, diameter);*/
}

void CNetDesignerView::draw3dCircle(CDC* pDC, int x, int y, int myradius, COLORREF mclr, bool gdi_not)
{
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

	CPoint pt = LPtoDP(CPoint(x, y));
	int l = pt.x - myradius;
	int t = pt.y - myradius;
	int r = pt.x + myradius;
	int b = pt.y + myradius;
	CRect rect;

	if (zoomScale >= 1.0) {
		int ml, mt, mr, mb;
		for (int i = 0; i < 9; i++) {
			ml = l + ((i * 2) * myradius) / 20;
			mt = t + ((i * 0) * myradius) / 20;
			mr = r - ((i * 2) * myradius) / 20;
			mb = b - ((i * 4) * myradius) / 20;
			rect.SetRect(ml, mt, mr, mb);
			drawCircle(pDC, rect, clrs[i], false, gdi_not, true);
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
			drawCircle(pDC, rect, clrs[i], false, gdi_not, true);
		}
	}
}

void CNetDesignerView::drawSquare(CDC* pDC, CRect mrect, COLORREF mcolor, bool r2_not, bool gdi_not)
{
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
		pDC->Rectangle(mrect);
		if (r2_not) {
			pDC->SetROP2(hdc);
		}
		DeleteObject(brush1);
	}
	else {
		CNetDesignerDoc* pDoc = GetDocument();
		if (pDoc->cartoon) {
			int diameter = mrect.right - mrect.left;
			Gdiplus::Graphics graphics(*pDC);
			Gdiplus::Color mclr;
			mclr.SetFromCOLORREF(mcolor);
			Gdiplus::SolidBrush brush2(mclr);
			graphics.FillRectangle(&brush2, mrect.left, mrect.top, diameter, diameter);
		}
		else {
			int diameter = mrect.right - mrect.left;
			Gdiplus::Graphics graphics(*pDC);
			Gdiplus::Color mclr;
			mclr.SetFromCOLORREF(mcolor);
			Gdiplus::Pen mpen(mclr, -1);
			graphics.DrawRectangle(&mpen, mrect.left, mrect.top, diameter, diameter);
		}
	}
}

void CNetDesignerView::DrawNode(CDC* pDC, int index, float scale, int mode, bool gdi_not)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		int x = pDoc->netData.getXDisp(index);
		int y = pDoc->netData.getYDisp(index);
		string type = pDoc->netData.getNodeMode(index);
		string func = pDoc->netData.getNodeFunction(index);
		if (type == "PreProcess") {
			func = pDoc->netData.getNodePreProcessor(index);
			//addErrorString(_T("PreProcessor = ") + func);
		}
		if (pDoc->netData.getNodeZeroOutput(index) && (mode == DRAW_NORMAL)) {
			DrawNode(pDC, x, y, (CString)type.c_str(), (CString)func.c_str(), scale, DRAW_ZERO, gdi_not);
		}
		else if (!pDoc->netData.getNodeTrainable(index) && (mode == DRAW_NORMAL)) {
			DrawNode(pDC, x, y, (CString)type.c_str(), (CString)func.c_str(), scale, DRAW_INACTIVE, gdi_not);
		}
		else {
			DrawNode(pDC, x, y, (CString)type.c_str(), (CString)func.c_str(), scale, mode, gdi_not);
		}
	}
}

void CNetDesignerView::DrawNode(int index, float scale, int mode, bool gdi_not)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		DrawNode(&dc, index, scale, mode, gdi_not);
	}
}

void CNetDesignerView::DrawConnect(CDC* pDC, CPoint pt1, CPoint pt2, float scale, int mode, bool shorten)
{
	CPoint mpt1 = pt1;
	CPoint mpt2 = pt2;
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->show_connect) {
		int mywidth = -1;
		if ((pDoc->netData.size() < 200) && pDoc->cartoon) {
			mywidth = int(scale * pDoc->lwidth);
		}
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
}

void CNetDesignerView::DrawConnect(CDC* pDC, int index, float scale, int mode, bool shorten)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->show_connect) {
		if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
			CPoint pt1 = pDoc->getInputNodePoint(index);
			CPoint pt2 = pDoc->getOutputNodePoint(index);
			DrawConnect(pDC, pt1, pt2, scale, mode, shorten);
		}
	}
}

void CNetDesignerView::DrawConnect(int index, float scale, int mode, bool shorten)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		DrawConnect(&dc, index, scale, mode, shorten);
	}
}

void CNetDesignerView::drawLine(CDC* pDC, CPoint pt1, CPoint pt2, int width, COLORREF mcolor, bool r2_not)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->show_connect) {
		int mywidth = width;
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
	}
}

void CNetDesignerView::removeNonGrayConvConnections()
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool found = false;
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			CAuxFunctions func;
			CString iname = (CString)pDoc->netData.getInputNode(i).c_str();
			CString iname2 = func.getConvNameFromConnectName(iname);
			int inode = func.getConvNodeFromConnectName(iname);
			string oname = pDoc->netData.getOutputNode(i);
			string oname2 = func.getConvNameFromConnectName(oname);
			int onode = func.getConvNodeFromConnectName((CString)oname.c_str());
			for (int j = 0; j < pDoc->netData.size(); j++) {
				if (pDoc->netData.isConvLayer(j)) {
					if (iname2 == (CString)pDoc->netData.getID(j).c_str()) {
						if (inode > 0) {
							if (!found) {
								pDoc->clearAndSetEditParams(UNDO_DELETE, zoomScale, lHorzScollPos, lVertScollPos);
								found = true;
							}
							CString sid = (CString)pDoc->netData.getID(i).c_str();
							CString slbl = (CString)pDoc->netData.getLabel(i).c_str();
							CString snode_in = (CString)pDoc->netData.getInputNode(i).c_str();
							CString snode_out = (CString)pDoc->netData.getOutputNode(i).c_str();
							vector<CPoint> points;
							double sweight = pDoc->netData.getWeight(i);
							vector<double> sweights = pDoc->netData.getConvWeights(i);
							pDoc->pushEditConnectData(sid, slbl, snode_in, snode_out, pDoc->netData.getGroupIndex(i), sweight, sweights, points);
						}
					}
					if (oname2 == pDoc->netData.getID(j)) {
						if (onode > 0) {
							if (!found) {
								pDoc->clearAndSetEditParams(UNDO_DELETE, zoomScale, lHorzScollPos, lVertScollPos);
								found = true;
							}
							CString sid = (CString)pDoc->netData.getID(i).c_str();
							CString slbl = (CString)pDoc->netData.getLabel(i).c_str();
							CString snode_in = (CString)pDoc->netData.getInputNode(i).c_str();
							CString snode_out = (CString)pDoc->netData.getOutputNode(i).c_str();
							vector<CPoint> points;
							double sweight = pDoc->netData.getWeight(i);
							vector<double> sweights = pDoc->netData.getConvWeights(i);
							pDoc->pushEditConnectData(sid, slbl, snode_in, snode_out, pDoc->netData.getGroupIndex(i), sweight, sweights, points);
						}
					}
				}
			}
		}
	}
	if (found) {
		pDoc->processEditData();
	}
}

void CNetDesignerView::DrawConvolution(CDC* pDC, int x, int y, bool gray, convParameters params, vector<int> dimensions, vector<CPoint> pts, float scale, int mode, bool redraw_spheres, bool print)
{
	if (dimensions.size() > 1) {
		CNetDesignerDoc* pDoc = GetDocument();
		bool gdi_not = false;
		float nradius = (float)pDoc->nradius2;
		int myradius = int(scale * nradius);
		float fcorner = scale * nradius / 2;
		int corner = int(fcorner);
		CRect brect = pDoc->getConvRect(params, scale);
		int inputwoffset = pDoc->getConvInputOffset(brect, scale);
		float nwidth = 1 * (float)pDoc->lwidth;
		int mywidth = -1;
		if (pDoc->cartoon) {
			mywidth = int(scale * nwidth);
		}
		bool is_image_conv = false; // pDoc->isConvImageType(index);
		if (params.type == IMAGE_CONV) {
			is_image_conv = true;
		}
		COLORREF mclr2 = clr_connect;
		if (!is_image_conv) {
			if (params.mode == _T("Hidden")) {
				mclr2 = clr_hiddennode1;
				if (params.function == _T("Relu")) {
					mclr2 = clr_hiddennode2;
				}
				else if (params.function == _T("Tanh")) {
					mclr2 = clr_hiddennode3;
				}
				else if (params.function == _T("Shockley")) {
					mclr2 = clr_hiddennode4;
				}
				else if (params.function == _T("MaxPool")) {
					mclr2 = clr_hiddennode5;
				}
				else if (params.function == _T("ACos")) {
					mclr2 = clr_hiddennode6;
				}
				else if (params.function == _T("SoftMax")) {
					mclr2 = clr_hiddennode10;
				}
				else if (params.function == _T("Gaussian")) {
					mclr2 = clr_hiddennode8;
				}
			}
			else if (params.mode == _T("Output")) {
				mclr2 = clr_outnode;
			}
			else if (params.mode == _T("Input")) {
				mclr2 = clr_input;
			}
		}
		COLORREF mclr = mclr2;// adjustColorBetween(adjustColorByPercent(clr_connect, 20), mclr2, 0.6F); //adjustColorByPercent(clr_connect, 20);
		COLORREF mclrgray = RGB(127, 127, 127);
		COLORREF mclrred = RGB(180, 30, 7);
		COLORREF mclrgreen = RGB(32, 180, 7);
		COLORREF mclrblue = RGB(10, 45, 255);
		if (mode == DRAW_ERASE) {
			mclr = clr_back;
		}
		else if (mode == DRAW_ZERO) {
			mclr = mclr2 = mclrgray = mclrred = mclrgreen = mclrblue = clr_zero;
		}
		else if (mode == DRAW_INACTIVE) {
			mclr = mclr2 = mclrgray = mclrred = mclrgreen = mclrblue = clr_inactive;
		}
		CPen pen(PS_SOLID, mywidth, mclr);
		pDC->SelectObject(pen);
		CBrush brush2;
		brush2.CreateSolidBrush(clr_back);
		CBrush brush3;
		brush3.CreateSolidBrush(RGB(180, 180, 180));
		CPoint pt = LPtoDP(CPoint(x, y));
		CRect rect;

		int hdc = 0;
		if (mode == DRAW_HIGHLIGHT) {
			hdc = pDC->SetROP2(R2_NOT);
		}

		//int data_type = pDoc->getDataPathType();
		if (mode != DRAW_HIGHLIGHT) {
			if (mode == DRAW_ERASE) {
				int l = pt.x - brect.left;
				int r = l + brect.Width();
				int t = pt.y + brect.top;
				int b = t + brect.Height();
				if (print) {
					b = t - brect.Height();
				}
				rect.SetRect(l, t, r, b);
				pDC->FillRect(rect, &brush2);
			}
			if (pDoc->display_conv_images) {
				if (mode != DRAW_ERASE) {
					if (params.type == IMAGE_CONV) {
						if ((params.mode == _T("Input")) || ((params.mode == _T("Output")) && !pDoc->nnet.image_data_allocated)) {
							DrawConvolutionInputOutputImage(pDC, x, y, params, scale, gray, print);
							//addErrorString(_T("DrawConvolutionInputOutputImage"));
						}
						else {
							if ((params.mode == _T("Output")) && pDoc->nnet.use_images_as_outputs) {
								DrawOutputLayerFromNodes(pDC, x, y, params, scale, gray, print);
								//addErrorString(_T("DrawOutputLayerFromNodes"));
							}
							else {
								DrawConvolutionHiddenImage(pDC, x, y, scale, gray, params, print);
								//addErrorString(_T("DrawConvolutionHiddenImage"));
							}
						}
					}
					else if (params.type == SOUND_CONV) {
						if (params.mode == _T("Input")) {
							DrawConvolutionInputOutputSound(pDC, x, y, params, scale, print);
							//addErrorString(_T("DrawConvolutionInputOutputSound"));
						}
						else if (params.mode == _T("Hidden")) {
							DrawDataConvolution(pDC, x, y, scale, params, mclr2, print);
							//addErrorString(_T("DrawDataConvolution"));
						}
					}
					else if (params.type == DATA_CONV) {
						DrawDataConvolution(pDC, x, y, scale, params, mclr2, print);
						//addErrorString(_T("DrawDataConvolution"));
					}
					/*else if (mctype == HIDDEN_NET) {
						if (redraw_spheres) {
							DrawHiddenNet(pDC, x, y, scale, index, mclr2, print);
						}
					}*/
				}
			}
			if (mode != DRAW_ERASE) {
				if (params.type == HIDDEN_NET) {
					if (redraw_spheres) {
						DrawHiddenNet(pDC, x, y, scale, params, mclr2, print);
						//addErrorString(_T("DrawHiddenNet"));
					}
				}
			}
		}

		if (mode == DRAW_ERASE) {
			int l = pt.x - brect.left;
			int r = l + brect.Width();
			int t = pt.y + brect.top;
			int b = t + brect.Height();
			if (print) {
				b = t - brect.Height();
			}
			CPen pen4(PS_SOLID, mywidth, clr_back);
			pDC->SelectObject(pen4);
			if (params.type == HIDDEN_NET) {
				int f4 = (int)(fcorner * 0.707);
				int f5 = (int)(fcorner * (1.0 - 0.707));
				POINT pts[] = {
					{l + corner, t},
					{r - corner, t},
					{r - corner + f4, t + f5},
					{r, t + corner},
					{r, b - corner},
					{r - corner + f4, b - f5},
					{r - corner, b},
					{l + corner, b},
					{l + corner - f4, b - f5},
					{l, b - corner},
					{l, t + corner},
					{l + corner - f4, t + f5},
					{l + corner, t}
				};
				pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
			}
			else {
				POINT pts[] = {
					{l, t},
					{r, t},
					{r, b},
					{l, b},
					{l, t}
				};
				pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
			}
		}
		else {
			if (redraw_spheres) {
				//CString s = _T(""); s.Format(_T(" : left = %i : top = %i : width = %i : height = %i"), brect.left, brect.top, brect.Width(), brect.Height());
				//addErrorString(_T("DrawConvolution : drawbox" + s));
				int l = pt.x - brect.left;
				int r = l + brect.Width();
				int t = pt.y + brect.top;
				int b = t + brect.Height();
				if (print) {
					b = t - brect.Height();
				}
				if (params.type == HIDDEN_NET || params.type == DATA_CONV) {
					int f4 = (int)(fcorner * 0.707);
					int f5 = (int)(fcorner * (1.0 - 0.707));
					POINT pts[] = {
						{l + corner, t},
						{r - corner, t},
						{r - corner + f4, t + f5},
						{r, t + corner},
						{r, b - corner},
						{r - corner + f4, b - f5},
						{r - corner, b},
						{l + corner, b},
						{l + corner - f4, b - f5},
						{l, b - corner},
						{l, t + corner},
						{l + corner - f4, t + f5},
						{l + corner, t}
					};
					pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
				}
				else {
					POINT pts[] = {
						{l, t},
						{r, t},
						{r, b},
						{l, b},
						{l, t}
					};
					pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
				}
			}
		}

		if (redraw_spheres) {

			CAuxFunctions func;

			if ((params.mode == _T("Input")) || (params.mode == _T("Hidden"))) {

				vector<CPoint> pts1 = func.getLayerPoints2((int)pDoc->nradius2, params.type);

				for (int i = 0; i < pts1.size(); i++) {

					int mx = x + pts1[i].x + inputwoffset;
					int my = y + pts1[i].y;

					CPoint pt = LPtoDP(CPoint(mx, my));
					int l = pt.x - myradius;
					int t = pt.y - myradius;
					int r = pt.x + myradius;
					int b = pt.y + myradius;
					rect.SetRect(l, t, r, b);

					if (mode == DRAW_HIGHLIGHT) {
						if ((i == 0) || (is_image_conv && !gray && (i == 1)) || (is_image_conv && !gray && (i == 2))) {
							rect.left = rect.left + 1;
							rect.right = rect.right + 1;
							rect.top = rect.top + 1;
							rect.bottom = rect.bottom + 1;
							drawCircle(pDC, rect, mclr, true, true);
						}
					}
					else if (mode == DRAW_ERASE) {
						drawCircle(pDC, rect, mclr, false, gdi_not, true);
					}
					else {
						if (i == 0) {
							if (!is_image_conv) {
								draw3dCircle(pDC, mx, my, myradius, mclr2, gdi_not);
							}
							else if (gray) {
								draw3dCircle(pDC, mx, my, myradius, mclrgray, gdi_not);
							}
							else {
								draw3dCircle(pDC, mx, my, myradius, mclrred, gdi_not);
							}
						}
						else if (is_image_conv && !gray && (i == 1)) {
							draw3dCircle(pDC, mx, my, myradius, mclrgreen, gdi_not);
						}
						else if (is_image_conv && !gray && (i == 2)) {
							draw3dCircle(pDC, mx, my, myradius, mclrblue, gdi_not);
						}
					}

				}

			}

			if ((params.mode == _T("Output")) || (params.mode == _T("Hidden"))) {

				vector<CPoint> pts1 = func.getLayerPoints2((int)pDoc->nradius2, params.type);

				for (int i = 0; i < pts1.size(); i++) {

					int mx = x + pts1[i].x;
					int my = y + pts1[i].y;

					CPoint pt = LPtoDP(CPoint(mx, my));
					int l = pt.x - myradius;
					int t = pt.y - myradius;
					int r = pt.x + myradius;
					int b = pt.y + myradius;
					rect.SetRect(l, t, r, b);

					if (mode == DRAW_HIGHLIGHT) {
						if ((i == 0) || (is_image_conv && !gray && (i == 1)) || (is_image_conv && !gray && (i == 2))) {
							rect.left = rect.left + 1;
							rect.right = rect.right + 1;
							rect.top = rect.top + 1;
							rect.bottom = rect.bottom + 1;
							drawCircle(pDC, rect, mclr, true, true);
						}
					}
					else if (mode == DRAW_ERASE) {
						drawCircle(pDC, rect, mclr, false, gdi_not, true);
					}
					else {
						if (i == 0) {
							if (!is_image_conv) {
								draw3dCircle(pDC, mx, my, myradius, mclr2, gdi_not);
							}
							else if (gray) {
								draw3dCircle(pDC, mx, my, myradius, mclrgray, gdi_not);
							}
							else {
								draw3dCircle(pDC, mx, my, myradius, mclrred, gdi_not);
							}
						}
						else if (is_image_conv && !gray && (i == 1)) {
							draw3dCircle(pDC, mx, my, myradius, mclrgreen, gdi_not);
						}
						else if (is_image_conv && !gray && (i == 2)) {
							draw3dCircle(pDC, mx, my, myradius, mclrblue, gdi_not);
						}
					}

				}

			}

		}

		if (mode == DRAW_HIGHLIGHT) {
			pDC->SetROP2(hdc);
		}

		DeleteObject(brush2);
		DeleteObject(brush3);

	}

}

void CNetDesignerView::DrawConvolution(CDC* pDC, int index, float scale, int mode, bool redraw_spheres, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		//addErrorString(_T("DrawConvolution : ") + (CString)pDoc->netData.getID(index).c_str());
		int x = pDoc->netData.getXDisp(index);
		int y = pDoc->netData.getYDisp(index);
		bool gray = pDoc->getDataImageGray();
		convParameters params = pDoc->getConvParameters(index);
		vector<int> dimensions = pDoc->netData.getConvDimensions(index);
		vector<CPoint> locations = convertValuesToPoints(pDoc->netData.getConvLocations(index));
		if (pDoc->netData.getNodeZeroOutput(index) && (mode == DRAW_NORMAL)) {
			DrawConvolution(pDC, x, y, gray, params, dimensions, locations, scale, DRAW_ZERO, redraw_spheres, print);
		}
		else if (!pDoc->netData.getNodeTrainable(index) && (mode == DRAW_NORMAL)) {
			DrawConvolution(pDC, x, y, gray, params, dimensions, locations, scale, DRAW_INACTIVE, redraw_spheres, print);
		}
		else {
			DrawConvolution(pDC, x, y, gray, params, dimensions, locations, scale, mode, redraw_spheres, print);
		}
	}
}

vector<CPoint> CNetDesignerView::convertValuesToPoints(vector<twoValues> locations)
{
	vector<CPoint> mlocations;
	for (int i = 0; i < locations.size(); i++) {
		CPoint mpt;
		mpt.x = locations[i].x;
		mpt.y = locations[i].y;
		mlocations.push_back(mpt);
	}
	return(mlocations);
}

void CNetDesignerView::DrawConvolution(int index, float scale, int mode, bool redraw_spheres)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		DrawConvolution(&dc, index, scale, mode, redraw_spheres);
	}
}

void CNetDesignerView::DrawAttention(CDC* pDC, int x, int y, float scale, int mode, bool self, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool gdi_not = false;
	float nradius = (float)pDoc->nradius2;
	int myradius = int(scale * nradius);
	float fcorner = scale * nradius / 2;
	int corner = int(fcorner);
	CRect brect = pDoc->getAttentionRect(scale);
	int inputwoffset = pDoc->getConvInputOffset(brect, scale);
	float nwidth = 1 * (float)pDoc->lwidth;
	int mywidth = -1;
	if (pDoc->cartoon) {
		mywidth = int(scale * nwidth);
	}
	COLORREF mclr = clr_hiddennode9;// adjustColorByPercent(clr_hiddennode9, -20);
	if (self) {
		mclr = oxidizedcopper;
	}
	COLORREF mclr2 = adjustColorByPercent(mclr, -40);
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	CPen pen(PS_SOLID, mywidth, mclr);
	pDC->SelectObject(pen);
	CBrush brush2;
	brush2.CreateSolidBrush(clr_back);
	CBrush brush3;
	brush3.CreateSolidBrush(RGB(180, 180, 180));
	CPoint pt = LPtoDP(CPoint(x, y));
	CRect rect;

	int hdc = 0;
	if (mode == DRAW_HIGHLIGHT) {
		hdc = pDC->SetROP2(R2_NOT);
	}

	if (mode != DRAW_HIGHLIGHT) {
		if (mode == DRAW_ERASE) {
			int l = pt.x - brect.left;
			int r = l + brect.Width();
			int t = pt.y + brect.top;
			int b = t + brect.Height();
			if (print) {
				b = t - brect.Height();
			}
			rect.SetRect(l, t, r, b);
			pDC->FillRect(rect, &brush2);
		}
		else {
			int woff = 0;
			int hoff = 0;
			int mwidth = 0;
			int mheight = 0;
			CPen pen(PS_SOLID, mywidth, mclr2);
			CPen* oldpen = pDC->SelectObject(&pen);
			CPoint pt = LPtoDP(CPoint(x, y));
			int xoffset = (int)(0.34 * (double)brect.Width());
			int l = pt.x - brect.left + xoffset;
			int r = l + brect.Width() - 2 * xoffset;
			int num_lines = 5;
			int y1 = pt.y + brect.top + (brect.Height() / (num_lines + 1));
			int y2 = pt.y + brect.top + (num_lines * brect.Height()) / (num_lines + 1);
			pDC->MoveTo(l, y1);
			pDC->LineTo(l, y2);
			pDC->MoveTo((l + r) / 2, y1);
			pDC->LineTo((l + r) / 2, y2);
			pDC->MoveTo(r, y1);
			pDC->LineTo(r, y2);
			pDC->SelectObject(oldpen);
		}
	}

	if (mode == DRAW_ERASE) {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		CPen pen4(PS_SOLID, mywidth, clr_back);
		pDC->SelectObject(pen4);
		pDC->MoveTo(l, t);
		pDC->LineTo(r, t);
		pDC->LineTo(r, b);
		pDC->LineTo(l, b);
		pDC->LineTo(l, t);
	}
	else {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		int f4 = (int)(fcorner * 0.707);
		int f5 = (int)(fcorner * (1.0 - 0.707));
		POINT pts[] = {
			{l + corner, t},
			{r - corner, t},
			{r - corner + f4, t + f5},
			{r, t + corner},
			{r, b - corner},
			{r - corner + f4, b - f5},
			{r - corner, b},
			{l + corner, b},
			{l + corner - f4, b - f5},
			{l, b - corner},
			{l, t + corner},
			{l + corner - f4, t + f5},
			{l + corner, t}
		};
		pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
	}

	CAuxFunctions func;

	vector<CPoint> pts1 = func.getAttentionPoints((int)pDoc->nradius2);

	for (int i = 0; i < pts1.size(); i++) {

		if (!self || (i == 0)) {

			int mx = x + pts1[i].x;// +inputwoffset;
			int my = y + pts1[i].y;

			CPoint pt = LPtoDP(CPoint(mx, my));
			int l = pt.x - myradius;
			int t = pt.y - myradius;
			int r = pt.x + myradius;
			int b = pt.y + myradius;
			rect.SetRect(l, t, r, b);

			if (mode == DRAW_HIGHLIGHT) {
				rect.left = rect.left + 1;
				rect.right = rect.right + 1;
				rect.top = rect.top + 1;
				rect.bottom = rect.bottom + 1;
				drawCircle(pDC, rect, mclr, true, true);
			}
			else if (mode == DRAW_ERASE) {
				drawCircle(pDC, rect, mclr, false, gdi_not, true);
			}
			else {
				draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
				if (!self) {
					int delta = (int)(10 * scale * nradius / 16);
					l += delta;
					t += delta;
					r -= delta;
					b -= delta;
					CPen dpen(PS_SOLID, mywidth, mclr2);
					pDC->SelectObject(dpen);
					if (i == 0) {
						// Draw Q
						int delta1 = (int)(2 * scale * nradius / 16);
						int delta2 = (int)(2 * scale * nradius / 16);
						l -= delta2;
						t -= delta1;
						r += delta2;
						b += delta1;
						pDC->Arc(l, t, r, b, l, t, l, t);
						delta1 = (int)(2 * scale * nradius / 16);
						delta2 = (int)(2 * scale * nradius / 16);
						r -= 5 * delta2;
						b -= 5 * delta1;
						pDC->MoveTo(r, b);
						r += 3 * delta2;
						b += 3 * delta1;
						pDC->LineTo(r, b);
					}
					else if (i == 1) {
						// Draw K
						delta = (int)(3 * scale * nradius / 16);
						l += delta;
						int c = (int)(0.55 * (double)t + 0.45 * (double)b);
						pDC->MoveTo(l, t);
						pDC->LineTo(l, b);
						pDC->MoveTo(r, b);
						pDC->LineTo(l, c);
						r -= delta;
						pDC->MoveTo(r, t);
						pDC->LineTo(l, c);
					}
					else if (i == 2) {
						// Draw V
						delta = (int)(2 * scale * nradius / 16);
						l += delta;
						r -= delta;
						pDC->MoveTo(l, t);
						pDC->LineTo((l + r) / 2, b);
						pDC->LineTo(r, t);
					}
				}
			}
		}

	}

	int mx = x + inputwoffset;
	int my = y;

	pt = LPtoDP(CPoint(mx, my));
	int l = pt.x - myradius;
	int t = pt.y - myradius;
	int r = pt.x + myradius;
	int b = pt.y + myradius;
	rect.SetRect(l, t, r, b);

	if (mode == DRAW_HIGHLIGHT) {
		rect.left = rect.left + 1;
		rect.right = rect.right + 1;
		rect.top = rect.top + 1;
		rect.bottom = rect.bottom + 1;
		drawCircle(pDC, rect, mclr, true, true);
	}
	else if (mode == DRAW_ERASE) {
		drawCircle(pDC, rect, mclr, false, gdi_not, true);
	}
	else {
		draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
	}

	if (mode == DRAW_HIGHLIGHT) {
		pDC->SetROP2(hdc);
	}

	DeleteObject(brush2);
	DeleteObject(brush3);

}

void CNetDesignerView::DrawAttention(CDC* pDC, int index, float scale, int mode, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		int x = pDoc->netData.getXDisp(index);
		int y = pDoc->netData.getYDisp(index);
		DrawAttention(pDC, x, y, scale, mode, pDoc->netData.isSelfAttentionLayer(index), print);
	}
}

void CNetDesignerView::DrawAttention(int index, float scale, int mode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		DrawAttention(&dc, index, scale, mode);
	}
}

void CNetDesignerView::DrawAssembly2(CDC* pDC, int x, int y, int index, float scale, int mode, assembly_sub_component asub, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool gdi_not = false;
	float nradius = (float)pDoc->nradius2;
	int myradius = int(scale * nradius);
	float fcorner = scale * nradius / 2;
	int corner = int(fcorner);
	CRect brect = pDoc->getAssemblyRect(index, scale);
	float nwidth = 1 * (float)pDoc->lwidth;
	int mywidth = -1;
	if (pDoc->cartoon) {
		mywidth = int(scale * nwidth);
	}
	COLORREF mclr = clr_hiddennode13;
	COLORREF mclr2 = adjustColorByPercent(mclr, -40);
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	CPen pen(PS_SOLID, mywidth, mclr);
	pDC->SelectObject(pen);
	CBrush brush2;
	brush2.CreateSolidBrush(clr_back);
	CBrush brush3;
	brush3.CreateSolidBrush(RGB(180, 180, 180));
	CPoint pt = LPtoDP(CPoint(x, y));
	CRect rect;

	int hdc = 0;
	if (mode == DRAW_HIGHLIGHT) {
		hdc = pDC->SetROP2(R2_NOT);
	}

	if (mode != DRAW_HIGHLIGHT) {
		if (mode == DRAW_ERASE) {
			int l = pt.x - brect.left;
			int r = l + brect.Width();
			int t = pt.y + brect.top;
			int b = t + brect.Height();
			if (print) {
				b = t - brect.Height();
			}
			rect.SetRect(l, t, r, b);
			pDC->FillRect(rect, &brush2);
		}
		else {
			DrawInternalDropout(pDC, x, y, scale, brect, mclr2, print);
			int diameter = rect.right - rect.left;
			Gdiplus::Graphics graphics(*pDC);
			Gdiplus::Color mclr2;
			mclr2.SetFromCOLORREF(mclr);
			Gdiplus::Pen bluePen(mclr2, 2);
			graphics.DrawEllipse(&bluePen, rect.left, rect.top, diameter, diameter);
		}
	}

	if (mode == DRAW_ERASE) {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		CPen pen4(PS_SOLID, mywidth, clr_back);
		pDC->SelectObject(pen4);
		pDC->MoveTo(l, t);
		pDC->LineTo(r, t);
		pDC->LineTo(r, b);
		pDC->LineTo(l, b);
		pDC->LineTo(l, t);
	}
	else {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		int f4 = (int)(fcorner * 0.707);
		int f5 = (int)(fcorner * (1.0 - 0.707));
		POINT pts[] = {
			{l + corner, t},
			{r - corner, t},
			{r - corner + f4, t + f5},
			{r, t + corner},
			{r, b - corner},
			{r - corner + f4, b - f5},
			{r - corner, b},
			{l + corner, b},
			{l + corner - f4, b - f5},
			{l, b - corner},
			{l, t + corner},
			{l + corner - f4, t + f5},
			{l + corner, t}
		};
		pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
	}

	drawAssemblyInputOutputs(pDC, x, y, index, scale, mode, asub);

	if (mode == DRAW_HIGHLIGHT) {
		pDC->SetROP2(hdc);
	}

	/*if (asub.net_index >= 0) {
		selectedComponents.clear();
		selectedComponents.push_back(asub.net_index);
	}*/

	DeleteObject(brush2);
	DeleteObject(brush3);

}

void CNetDesignerView::drawAssemblyInputOutputs(CDC* pDC, int x, int y, int index, float scale, int mode, assembly_sub_component asub)
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool gdi_not = false;
	float nradius = (float)pDoc->nradius2;
	int myradius = int(scale * nradius);
	int mywidth = -1;
	if (pDoc->cartoon) {
		mywidth = int(scale * (float)pDoc->lwidth);
	}
	COLORREF mclr = clr_hiddennode13;
	COLORREF mclr2 = adjustColorByPercent(mclr, -40);
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	CRect rect;

	int hdc = 0;
	if (mode == DRAW_HIGHLIGHT) {
		hdc = pDC->SetROP2(R2_NOT);
	}

	// Draw input connectors
	vector<CPoint> pts1 = pDoc->getAssemblyInputPoints(index, (int)pDoc->nradius2);
	if (pts1.size() > 0) {
		for (int i = 0; i < pts1.size(); i++) {
			int mx = x + pts1[i].x;
			int my = y + pts1[i].y;
			CPoint pt = LPtoDP(CPoint(mx, my));
			int l = pt.x - myradius;
			int t = pt.y - myradius;
			int r = pt.x + myradius;
			int b = pt.y + myradius;
			rect.SetRect(l, t, r, b);
			if (mode == DRAW_HIGHLIGHT) {
				rect.left = rect.left + 1;
				rect.right = rect.right + 1;
				rect.top = rect.top + 1;
				rect.bottom = rect.bottom + 1;
				drawCircle(pDC, rect, mclr, true, true);
			}
			else if (mode == DRAW_ERASE) {
				drawCircle(pDC, rect, mclr, false, gdi_not, true);
			}
			else {
				draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
				int delta = (int)(10 * scale * nradius / 16);
				l += delta;
				t += delta;
				r -= delta;
				b -= delta;
				CPen dpen(PS_SOLID, mywidth, mclr2);
				pDC->SelectObject(dpen);
			}
		}
	}

	// Draw Output connectors
	vector<CPoint> pts2 = pDoc->getAssemblyOutputPoints(index, (int)pDoc->nradius2);
	if (pts2.size() > 0) {
		for (int i = 0; i < pts2.size(); i++) {
			int mx = x + pts2[i].x;
			int my = y + pts2[i].y;
			CPoint pt = LPtoDP(CPoint(mx, my));
			int l = pt.x - myradius;
			int t = pt.y - myradius;
			int r = pt.x + myradius;
			int b = pt.y + myradius;
			rect.SetRect(l, t, r, b);
			if (mode == DRAW_HIGHLIGHT) {
				rect.left = rect.left + 1;
				rect.right = rect.right + 1;
				rect.top = rect.top + 1;
				rect.bottom = rect.bottom + 1;
				drawCircle(pDC, rect, mclr, true, true);
			}
			else if (mode == DRAW_ERASE) {
				drawCircle(pDC, rect, mclr, false, gdi_not, true);
			}
			else {
				draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
				int delta = (int)(10 * scale * nradius / 16);
				l += delta;
				t += delta;
				r -= delta;
				b -= delta;
				CPen dpen(PS_SOLID, mywidth, mclr2);
				pDC->SelectObject(dpen);
			}
		}
	}

	if (mode == DRAW_HIGHLIGHT) {
		pDC->SetROP2(hdc);
	}

}

void CNetDesignerView::DrawAssembly(CDC* pDC, int index, float scale, int mode, assembly_sub_component asub, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		int x = pDoc->netData.getXDisp(index);
		int y = pDoc->netData.getYDisp(index);
		DrawAssembly2(pDC, x, y, index, scale, mode, asub, print);
	}
}

void CNetDesignerView::DrawAssembly(int index, float scale, int mode, assembly_sub_component asub)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		DrawAssembly(&dc, index, scale, mode, asub);
	}
}

void CNetDesignerView::DrawNetCluster(CDC* pDC, int x, int y, float scale, int mode, string omode, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool gdi_not = false;
	float nradius = (float)pDoc->nradius2;
	int myradius = int(scale * nradius);
	float fcorner = scale * nradius / 2;
	int corner = int(fcorner);
	CRect brect = pDoc->getNetClusterRect(scale);
	int inputwoffset = pDoc->getConvInputOffset(brect, scale);
	float nwidth = 1 * (float)pDoc->lwidth;
	int mywidth = -1;
	if (pDoc->cartoon) {
		mywidth = int(scale * nwidth);
	}
	COLORREF mclr = clr_hiddennode11;// adjustColorByPercent(platinum, -20);// potassium;// adjustColorByPercent(potassium, -5);
	COLORREF mclr2 = adjustColorByPercent(mclr, -40);
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	CPen pen(PS_SOLID, mywidth, mclr);
	pDC->SelectObject(pen);
	CBrush brush2;
	brush2.CreateSolidBrush(clr_back);
	CBrush brush3;
	brush3.CreateSolidBrush(RGB(180, 180, 180));
	CPoint pt = LPtoDP(CPoint(x, y));
	CRect rect;

	int hdc = 0;
	if (mode == DRAW_HIGHLIGHT) {
		hdc = pDC->SetROP2(R2_NOT);
	}

	if (mode != DRAW_HIGHLIGHT) {
		if (mode == DRAW_ERASE) {
			int l = pt.x - brect.left;
			int r = l + brect.Width();
			int t = pt.y + brect.top;
			int b = t + brect.Height();
			if (print) {
				b = t - brect.Height();
			}
			rect.SetRect(l, t, r, b);
			pDC->FillRect(rect, &brush2);
		}
		else {
			DrawInternalNetCluster(pDC, x, y, scale, brect, mclr2, print);
			int diameter = rect.right - rect.left;
			Gdiplus::Graphics graphics(*pDC);
			Gdiplus::Color mclr2;
			mclr2.SetFromCOLORREF(mclr);
			Gdiplus::Pen bluePen(mclr2, 2);
			graphics.DrawEllipse(&bluePen, rect.left, rect.top, diameter, diameter);
		}
	}

	if (mode == DRAW_ERASE) {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		CPen pen4(PS_SOLID, mywidth, clr_back);
		pDC->SelectObject(pen4);
		pDC->MoveTo(l, t);
		pDC->LineTo(r, t);
		pDC->LineTo(r, b);
		pDC->LineTo(l, b);
		pDC->LineTo(l, t);
	}
	else {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		int f4 = (int)(fcorner * 0.707);
		int f5 = (int)(fcorner * (1.0 - 0.707));
		POINT pts[] = {
			{l + corner, t},
			{r - corner, t},
			{r - corner + f4, t + f5},
			{r, t + corner},
			{r, b - corner},
			{r - corner + f4, b - f5},
			{r - corner, b},
			{l + corner, b},
			{l + corner - f4, b - f5},
			{l, b - corner},
			{l, t + corner},
			{l + corner - f4, t + f5},
			{l + corner, t}
		};
		pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
	}

	CAuxFunctions func;

	vector<CPoint> pts1 = func.getNetClusterPoints((int)pDoc->nradius2);

	for (int i = 0; i < pts1.size(); i++) {

		int mx = x + pts1[i].x;
		int my = y + pts1[i].y;

		CPoint pt = LPtoDP(CPoint(mx, my));
		int l = pt.x - myradius;
		int t = pt.y - myradius;
		int r = pt.x + myradius;
		int b = pt.y + myradius;
		rect.SetRect(l, t, r, b);

		if (mode == DRAW_HIGHLIGHT) {
			rect.left = rect.left + 1;
			rect.right = rect.right + 1;
			rect.top = rect.top + 1;
			rect.bottom = rect.bottom + 1;
			drawCircle(pDC, rect, mclr, true, true);
		}
		else if (mode == DRAW_ERASE) {
			drawCircle(pDC, rect, mclr, false, gdi_not, true);
		}
		else {
			draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
		}

	}

	if (omode == "Hidden") {
		for (int i = 0; i < pts1.size(); i++) {

			int mx = x + pts1[i].x + inputwoffset;
			int my = y + pts1[i].y;

			CPoint pt = LPtoDP(CPoint(mx, my));
			int l = pt.x - myradius;
			int t = pt.y - myradius;
			int r = pt.x + myradius;
			int b = pt.y + myradius;
			rect.SetRect(l, t, r, b);

			if (mode == DRAW_HIGHLIGHT) {
				rect.left = rect.left + 1;
				rect.right = rect.right + 1;
				rect.top = rect.top + 1;
				rect.bottom = rect.bottom + 1;
				drawCircle(pDC, rect, mclr, true, true);
			}
			else if (mode == DRAW_ERASE) {
				drawCircle(pDC, rect, mclr, false, gdi_not, true);
			}
			else {
				draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
			}

		}
	}

	if (mode == DRAW_HIGHLIGHT) {
		pDC->SetROP2(hdc);
	}

	DeleteObject(brush2);
	DeleteObject(brush3);

}

void CNetDesignerView::DrawNetCluster(CDC* pDC, int index, float scale, int mode, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		int x = pDoc->netData.getXDisp(index);
		int y = pDoc->netData.getYDisp(index);
		string omode = pDoc->netData.getNodeMode(index);
		DrawNetCluster(pDC, x, y, scale, mode, omode, print);
	}
}

void CNetDesignerView::DrawNetCluster(int index, float scale, int mode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		DrawNetCluster(&dc, index, scale, mode);
	}
}

void CNetDesignerView::DrawDropout(CDC* pDC, int x, int y, float scale, int mode, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool gdi_not = false;
	float nradius = (float)pDoc->nradius2;
	int myradius = int(scale * nradius);
	float fcorner = scale * nradius / 2;
	int corner = int(fcorner);
	CRect brect = pDoc->getNetClusterRect(scale);
	int inputwoffset = pDoc->getConvInputOffset(brect, scale);
	float nwidth = 1 * (float)pDoc->lwidth;
	int mywidth = -1;
	if (pDoc->cartoon) {
		mywidth = int(scale * nwidth);
	}
	COLORREF mclr = clr_hiddennode12;
	COLORREF mclr2 = adjustColorByPercent(mclr, -40);
	if (mode == DRAW_ERASE) {
		mclr = clr_back;
	}
	CPen pen(PS_SOLID, mywidth, mclr);
	pDC->SelectObject(pen);
	CBrush brush2;
	brush2.CreateSolidBrush(clr_back);
	CBrush brush3;
	brush3.CreateSolidBrush(RGB(180, 180, 180));
	CPoint pt = LPtoDP(CPoint(x, y));
	CRect rect;

	int hdc = 0;
	if (mode == DRAW_HIGHLIGHT) {
		hdc = pDC->SetROP2(R2_NOT);
	}

	if (mode != DRAW_HIGHLIGHT) {
		if (mode == DRAW_ERASE) {
			int l = pt.x - brect.left;
			int r = l + brect.Width();
			int t = pt.y + brect.top;
			int b = t + brect.Height();
			if (print) {
				b = t - brect.Height();
			}
			rect.SetRect(l, t, r, b);
			pDC->FillRect(rect, &brush2);
		}
		else {
			DrawInternalDropout(pDC, x, y, scale, brect, mclr2, print);
			int diameter = rect.right - rect.left;
			Gdiplus::Graphics graphics(*pDC);
			Gdiplus::Color mclr2;
			mclr2.SetFromCOLORREF(mclr);
			Gdiplus::Pen bluePen(mclr2, 2);
			graphics.DrawEllipse(&bluePen, rect.left, rect.top, diameter, diameter);
		}
	}

	if (mode == DRAW_ERASE) {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		CPen pen4(PS_SOLID, mywidth, clr_back);
		pDC->SelectObject(pen4);
		pDC->MoveTo(l, t);
		pDC->LineTo(r, t);
		pDC->LineTo(r, b);
		pDC->LineTo(l, b);
		pDC->LineTo(l, t);
	}
	else {
		int l = pt.x - brect.left;
		int r = l + brect.Width();
		int t = pt.y + brect.top;
		int b = t + brect.Height();
		if (print) {
			b = t - brect.Height();
		}
		int f4 = (int)(fcorner * 0.707);
		int f5 = (int)(fcorner * (1.0 - 0.707));
		POINT pts[] = {
			{l + corner, t},
			{r - corner, t},
			{r - corner + f4, t + f5},
			{r, t + corner},
			{r, b - corner},
			{r - corner + f4, b - f5},
			{r - corner, b},
			{l + corner, b},
			{l + corner - f4, b - f5},
			{l, b - corner},
			{l, t + corner},
			{l + corner - f4, t + f5},
			{l + corner, t}
		};
		pDC->Polyline(pts, sizeof(pts) / sizeof(pts[0]));
	}

	CAuxFunctions func;

	vector<CPoint> pts1 = func.getNetClusterPoints((int)pDoc->nradius2);

	for (int i = 0; i < pts1.size(); i++) {

		int mx = x + pts1[i].x;
		int my = y + pts1[i].y;

		CPoint pt = LPtoDP(CPoint(mx, my));
		int l = pt.x - myradius;
		int t = pt.y - myradius;
		int r = pt.x + myradius;
		int b = pt.y + myradius;
		rect.SetRect(l, t, r, b);

		if (mode == DRAW_HIGHLIGHT) {
			rect.left = rect.left + 1;
			rect.right = rect.right + 1;
			rect.top = rect.top + 1;
			rect.bottom = rect.bottom + 1;
			drawCircle(pDC, rect, mclr, true, true);
		}
		else if (mode == DRAW_ERASE) {
			drawCircle(pDC, rect, mclr, false, gdi_not, true);
		}
		else {
			draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
		}

	}

	for (int i = 0; i < pts1.size(); i++) {

		int mx = x + pts1[i].x + inputwoffset;
		int my = y + pts1[i].y;

		CPoint pt = LPtoDP(CPoint(mx, my));
		int l = pt.x - myradius;
		int t = pt.y - myradius;
		int r = pt.x + myradius;
		int b = pt.y + myradius;
		rect.SetRect(l, t, r, b);

		if (mode == DRAW_HIGHLIGHT) {
			rect.left = rect.left + 1;
			rect.right = rect.right + 1;
			rect.top = rect.top + 1;
			rect.bottom = rect.bottom + 1;
			drawCircle(pDC, rect, mclr, true, true);
		}
		else if (mode == DRAW_ERASE) {
			drawCircle(pDC, rect, mclr, false, gdi_not, true);
		}
		else {
			draw3dCircle(pDC, mx, my, myradius, mclr, gdi_not);
		}

	}

	if (mode == DRAW_HIGHLIGHT) {
		pDC->SetROP2(hdc);
	}

	DeleteObject(brush2);
	DeleteObject(brush3);

}

void CNetDesignerView::DrawDropout(CDC* pDC, int index, float scale, int mode, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		int x = pDoc->netData.getXDisp(index);
		int y = pDoc->netData.getYDisp(index);
		DrawDropout(pDC, x, y, scale, mode, print);
	}
}

void CNetDesignerView::DrawDropout(int index, float scale, int mode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		DrawDropout(&dc, index, scale, mode);
	}
}

void CNetDesignerView::DrawConvolutionInputOutputImage(CDC* pDC, int x, int y, convParameters params, float scale, bool gray, bool print)
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();

	bool ok_to_draw = false;
	bool delete_mbits = false;
	int bmHeight = 0;
	int bmWidth = 0;
	int bmWidthBytes = 0;
	int imsize2 = 0;

	int image_width = pDoc->getDataImageWidth();
	int image_height = pDoc->getDataImageHeight();

	CPoint pt = LPtoDP(CPoint(x, y));
	CRect brect = pDoc->getConvRect(params, scale);

	CImage image;
	CBitmap bitmap;
	BITMAP pBitMap;
	unsigned char* mbits = NULL;

	if (pDoc->nnet.image_data_allocated && (pDoc->nnet.convs.size() > 0)) {

		string cname = pDoc->nnet.input_image_conv_name;
		if (cname == "") {
			cname = pDoc->nnet.newConvs[0].id;
		}
		//addErrorString(cname);

		int size2 = pDoc->nnet.getOutputImageDataSize(cname);

		double* cimage_data = new double[size2];
		if (cimage_data == 0) {
			addErrorString(_T("DrawConvolution : unable to allocate cimage_data"));
			return;
		}

		imageData idata = pDoc->nnet.getOutputImageData(cname, cimage_data); 

		bmWidth = idata.numx;
		bmHeight = idata.numy;
		bmWidthBytes = idata.xstride;

		image_width = bmWidth;
		image_height = bmHeight;

		imsize2 = bmWidthBytes * bmHeight;
		if (size2 > imsize2) {
			imsize2 = size2;
		}
		mbits = new unsigned char[imsize2];
		if (mbits == 0) {
			addErrorString(_T("DrawConvolution : unable to allocate mbits"));
			return;
		}
		delete_mbits = true;

		int index2 = 0;
		for (int j = 0; j < bmHeight; j++) {
			for (int i = 0; i < bmWidth; i++) {
				index2 = (j * bmWidthBytes) + (i * 3);
				mbits[index2 + 2] = (int)(255 * cimage_data[index2 + 2]);
				mbits[index2 + 1] = (int)(255 * cimage_data[index2 + 1]);
				mbits[index2 + 0] = (int)(255 * cimage_data[index2 + 0]);
			}
		}

		delete[] cimage_data;

		ok_to_draw = true;

	}
	else {

		int sindex = pDoc->nnet.data_file_index;
		if ((sindex < 0) || (sindex >= pDoc->getDataPathNamesSize(false))) {
			sindex = 0;
		}
		CString cname = pDoc->getDataPathName(sindex, false);
		if (pDoc->getDataPathType() == FILE_TYPE_CSV) {
			// Get filename for csv jpg
			if (pDoc->defaultCsvJpgPathname != _T("")) {
				cname = pDoc->defaultCsvJpgPathname;
			}
		}

		CString ext = pDoc->getImageFileExtension();

		if (ext == _T("jpg")) {
			HRESULT rslt = image.Load(cname);
			if (rslt == S_OK) {
				bitmap.Attach(image.Detach());
				if (bitmap.GetBitmap(&pBitMap)) {
					bmHeight = pBitMap.bmHeight;
					bmWidth = pBitMap.bmWidth;
					bmWidthBytes = pBitMap.bmWidthBytes;
					mbits = (unsigned char*)pBitMap.bmBits;
					ok_to_draw = true;
				}
			}
		}
		else if (ext == _T("raw")) {

			// do we want to display images for raw images on initial display?
			/*int rawwidth = -1;
			int rawheight = -1;
			if ((rawwidth < 0) || (rawheight < 0)) {
				CWnd* pWndMain = AfxGetMainWnd();
				CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
				for (int i = 0; i < pDoc->netData.size(); i++) {
					if (pDoc->netData.isDataSource(i)) {
						rawwidth = pDoc->netData.getRawImageWidth(i);
						rawheight = pDoc->netData.getRawImageHeight(i);
						i = pDoc->netData.size();
					}
				}
			}

			if ((rawwidth > 0) && (rawheight > 0)) {

				int test_size = 3 * rawwidth * rawheight;
				unsigned char* filedata = new unsigned char[3 * (int64_t)rawwidth * (int64_t)rawheight];
				CFile file;
				int here_we_need_the_filename_for_the_raw_filename_embedded_in_the_csv_file;
				if (file.Open(cname, CFile::modeRead | CFile::shareDenyWrite)) {
					unsigned long filelength = (unsigned long)file.GetLength();
					if (filelength == test_size) {
						file.Read(filedata, filelength);
						file.Close();
					}
				}
				delete[] filedata;

			}*/

		}

	}

	if (ok_to_draw) {

		int imsize = 3 * (brect.Width() + 0) * (brect.Height() + 2);
		if (imsize2 > imsize) {
			imsize = imsize2;
		}
		unsigned char* image_data = new unsigned char[imsize];

		if (image_data != 0) {

			int woff = 0;
			int hoff = 0;
			int mwidth = brect.Width();
			int mheight = brect.Height();
			if ((mwidth > image_width) || (mheight > image_height)) {
				woff = (mwidth - image_width) / 2;
				hoff = (mheight - image_height) / 2;
				mwidth = image_width;
				mheight = image_height;
			}

			if ((bmWidth > mwidth) && (bmHeight > mheight)) {
				image_data = func.downscaleImageData(bmHeight, bmWidth, bmWidthBytes, mbits, mheight, mwidth, image_data);
				bmWidth = mwidth;
				bmHeight = mheight;
				bmWidthBytes = 3 * mwidth;
			}
			else {
				for (int i = 0; i < imsize; i++) {
					image_data[i] = 80;
				}
				int index2 = 0;
				for (int j = 0; j < bmHeight; j++) {
					for (int i = 0; i < bmWidth; i++) {
						index2 = (j * bmWidthBytes) + (i * 3);
						image_data[index2 + 2] = mbits[index2 + 2];
						image_data[index2 + 1] = mbits[index2 + 1];
						image_data[index2 + 0] = mbits[index2 + 0];
					}
				}
			}

			CImage mimage;
			mimage.Create(mwidth, mheight, 24);

			if (gray) {
				int gclr = 0;
				int index = 0;
				for (int j = 0; j < mheight; j++) {
					for (int i = 0; i < mwidth; i++) {
						index = (j * bmWidthBytes) + (i * 3);
						gclr = (image_data[index] + image_data[index + 1] + image_data[index + 2]) / 3;
						if (((index + 2) < imsize) && (i < bmWidth) && (j < mheight)) {
							if (print) {
								mimage.SetPixel(i, j, RGB(gclr, gclr, gclr));
							}
							else {
								mimage.SetPixel(i, mheight - j - 1, RGB(gclr, gclr, gclr));
							}
						}
					}
				}
			}
			else {
				int index = 0;
				for (int j = 0; j < mheight; j++) {
					for (int i = 0; i < mwidth; i++) {
						index = (j * bmWidthBytes) + (i * 3);
						if (((index + 2) < imsize) && (i < bmWidth) && (j < mheight)) {
							if (print) {
								mimage.SetPixel(i, j + 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
							}
							else {
								mimage.SetPixel(i, mheight - j - 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
							}
						}
					}
				}
			}

			delete[] image_data;

			if (print) {
				mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top - brect.Height() + hoff);
			}
			else {
				mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top + hoff);
			}

		}
		else {
			addErrorString(_T("DrawConvolution : Unable to allocate memory for image"));
		}

	}

	if (delete_mbits) {
		delete[] mbits;
	}

}

void CNetDesignerView::DrawOutputLayerFromNodes(CDC* pDC, int x, int y, convParameters params, float scale, bool gray, bool print)
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();

	bool ok_to_draw = false;
	bool delete_mbits = false;
	int imsize2 = pDoc->getOutputImageSize();
	int bmHeight = pDoc->getDataImageHeight();
	int bmWidth = pDoc->getDataImageWidth();
	int bmWidthBytes = bmWidth;
	if (!gray) {
		bmWidthBytes = 3 * bmWidthBytes;
	}

	CPoint pt = LPtoDP(CPoint(x, y));
	CRect brect = pDoc->getConvRect(params, scale);

	CImage image;
	CBitmap bitmap;
	BITMAP pBitMap;
	unsigned char* mbits = NULL;

	if (pDoc->nnet.num_outputs > 0) {

		mbits = new unsigned char[imsize2];
		if (mbits == 0) {
			addErrorString(_T("DrawOutputLayer : unable to allocate mbits"));
			return;
		}
		delete_mbits = true; 

		for (int i = 0; i < imsize2; i++) {
			mbits[i] = (int)(255 * pDoc->nnet.getOutput(i));
		}

		ok_to_draw = true;

	}
	else {
		int sindex = pDoc->nnet.data_file_index;
		if ((sindex < 0) || (sindex >= pDoc->getDataPathNamesSize(false))) {
			CString s = _T("");
			s.Format(_T("DrawOutputLayer : setting index to zero : sindex = %i : dataPathnames size = %i"), sindex, pDoc->getDataPathNamesSize(false));
			addErrorString(s);
			sindex = 0;
		}
		HRESULT rslt = image.Load(pDoc->getDataPathName(sindex, false));
		if (rslt == S_OK) {
			bitmap.Attach(image.Detach());
			if (bitmap.GetBitmap(&pBitMap)) {
				bmHeight = pBitMap.bmHeight;
				bmWidth = pBitMap.bmWidth;
				bmWidthBytes = pBitMap.bmWidthBytes;
				mbits = (unsigned char*)pBitMap.bmBits;
				ok_to_draw = true;
			}
		}
	}

	if (ok_to_draw) {

		int mwidth = brect.Width();
		int mheight = brect.Height();
		int imsize = 3 * (mwidth + 0) * (mheight + 2);
		if (imsize2 > imsize) {
			imsize = imsize2;
		}
		unsigned char* image_data = new unsigned char[imsize];

		if (image_data != 0) {

			int woff = 0;
			int hoff = 0;
			if ((mwidth > bmWidth) || (mheight > bmHeight)) {
				woff = (mwidth - bmWidth) / 2;
				hoff = (mheight - bmHeight) / 2;
				mwidth = bmWidth;
				mheight = bmHeight;
			}

			if ((bmWidth > mwidth) && (bmHeight > mheight)) {
				image_data = func.downscaleImageData(bmHeight, bmWidth, bmWidthBytes, mbits, mheight, mwidth, image_data);
				bmWidth = mwidth;
				bmHeight = mheight;
				bmWidthBytes = 3 * mwidth;
			}
			else {
				for (int i = 0; i < imsize; i++) {
					image_data[i] = 80;
				}
				int index2 = 0;
				for (int j = 0; j < bmHeight; j++) {
					for (int i = 0; i < bmWidth; i++) {
						index2 = (j * bmWidthBytes) + (i * 3);
						image_data[index2 + 2] = mbits[index2 + 2];
						image_data[index2 + 1] = mbits[index2 + 1];
						image_data[index2 + 0] = mbits[index2 + 0];
					}
				}
			}

			CImage mimage;
			mimage.Create(mwidth, mheight, 24);

			if (gray) {
				int gclr = 0;
				int index = 0;
				for (int j = 0; j < mheight; j++) {
					for (int i = 0; i < mwidth; i++) {
						index = (j * bmWidthBytes) + (i * 3);
						gclr = (image_data[index] + image_data[index + 1] + image_data[index + 2]) / 3;
						if (((index + 2) < imsize) && (i < bmWidth) && (j < mheight)) {
							if (print) {
								mimage.SetPixel(i, j, RGB(gclr, gclr, gclr));
							}
							else {
								mimage.SetPixel(i, mheight - j - 1, RGB(gclr, gclr, gclr));
							}
						}
					}
				}
			}
			else {
				int index = 0;
				for (int j = 0; j < mheight; j++) {
					for (int i = 0; i < mwidth; i++) {
						index = (j * bmWidthBytes) + (i * 3);
						if (((index + 2) < imsize) && (i < bmWidth) && (j < mheight)) {
							if (print) {
								mimage.SetPixel(i, j + 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
							}
							else {
								mimage.SetPixel(i, mheight - j - 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
							}
						}
					}
				}
			}

			delete[] image_data;

			if (print) {
				mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top - brect.Height() + hoff);
			}
			else {
				mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top + hoff);
			}

		}
		else {
			addErrorString(_T("DrawOutputLayer : Unable to allocate memory for image"));
		}

	}

	if (delete_mbits) {
		delete[] mbits;
	}

}

void CNetDesignerView::DrawConvolutionHiddenImage(CDC* pDC, int x, int y, float scale, bool gray, convParameters params, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();

	int woff = 0;
	int hoff = 0;
	int mwidth = 0;
	int mheight = 0;

	int image_width = pDoc->getDataImageWidth();
	int image_height = pDoc->getDataImageHeight();

	CRect rect;
	CBrush brush3;
	brush3.CreateSolidBrush(RGB(180, 180, 180));
	CPoint pt = LPtoDP(CPoint(x, y));
	CRect brect = pDoc->getConvRect(params, scale);

	if ((image_width > 0) && (image_height > 0)) {

		CAuxFunctions func;

		string cname = string(CT2CA(params.id));

		if (!pDoc->nnet.image_data_allocated) {

			imageSizeData imageSize = func.getImageSizeData(cname, pDoc->netData);

			mwidth = imageSize.output_width;
			mheight = imageSize.output_height;

			if (mwidth > brect.Width()) {
				mwidth = brect.Width();
			}
			if (mheight > brect.Height()) {
				mheight = brect.Height();
			}
			if ((mwidth < brect.Width()) || (mheight < brect.Height())) {
				woff = (brect.Width() - mwidth) / 2;
				hoff = (brect.Height() - mheight) / 2;
			}

			//CString s = _T("");
			//s.Format(_T(" : brect.Height() = %i : brect.top = %i : mheight = %i : hoff = %i"), brect.Height(), brect.top, mheight, hoff);
			//addErrorString(_T("DrawConvolutionHiddenImage : ") + pDoc->netData.getID(index) + s);

			int l = pt.x + woff;
			int r = l + mwidth;
			int t = pt.y + brect.top + hoff;
			int b = t + mheight;
			if (print) {
				b = t - mheight;
			}
			rect.SetRect(l, t, r, b);
			pDC->FillRect(rect, &brush3);

		}
		else {

			int size2 = pDoc->nnet.getOutputImageDataSize(cname);

			double* cimage_data = new double[size2];
			if (cimage_data == 0) {
				addErrorString(_T("DrawConvolution : unable to allocate cimage_data"));
				return;
			}

			imageData idata = pDoc->nnet.getOutputImageData(cname, cimage_data);

			if ((idata.numx > 0) && (idata.numy > 0) && (idata.xstride > 0)) {

				int bmWidth = idata.numx;
				int bmHeight = idata.numy;
				int bmWidthBytes = idata.xstride;

				image_width = bmWidth;
				image_height = bmHeight;

				int imsize2 = size2;
				unsigned char* mbits = new unsigned char[imsize2];
				if (mbits == 0) {
					addErrorString(_T("DrawConvolution : unable to allocate mbits"));
					return;
				}

				int index2 = 0;
				for (int j = 0; j < bmHeight; j++) {
					for (int i = 0; i < bmWidth; i++) {
						index2 = (j * bmWidthBytes) + (i * 3);
						mbits[index2 + 2] = (int)(255 * cimage_data[index2 + 2]);
						mbits[index2 + 1] = (int)(255 * cimage_data[index2 + 1]);
						mbits[index2 + 0] = (int)(255 * cimage_data[index2 + 0]);
					}
				}

				delete[] cimage_data;

				int imsize = 3 * (brect.Width() + 0) * (brect.Height() + 2);
				if (imsize2 > imsize) {
					imsize = imsize2;
				}
				unsigned char* image_data = new unsigned char[imsize];

				if (image_data != 0) {

					int woff = 0;
					int hoff = 0;
					int mwidth = brect.Width();
					int mheight = brect.Height();
					if (mwidth > image_width) {
						woff = (mwidth - image_width) / 2;
						mwidth = image_width;
					}
					if (mheight > image_height) {
						hoff = (mheight - image_height) / 2;
						mheight = image_height;
					}

					if ((bmWidth > mwidth) || (bmHeight > mheight)) {
						image_data = func.downscaleImageData(bmHeight, bmWidth, bmWidthBytes, mbits, mheight, mwidth, image_data);
						bmWidth = mwidth;
						bmHeight = mheight;
						bmWidthBytes = 3 * mwidth;
					}
					else {
						for (int i = 0; i < imsize; i++) {
							image_data[i] = 80;
						}
						int index2 = 0;
						for (int j = 0; j < bmHeight; j++) {
							for (int i = 0; i < bmWidth; i++) {
								index2 = (j * bmWidthBytes) + (i * 3);
								image_data[index2 + 2] = mbits[index2 + 2];
								image_data[index2 + 1] = mbits[index2 + 1];
								image_data[index2 + 0] = mbits[index2 + 0];
							}
						}
					}

					CImage mimage;
					mimage.Create(mwidth, mheight, 24);

					if (gray) {
						int gclr = 0;
						int index = 0;
						for (int j = 0; j < mheight; j++) {
							for (int i = 0; i < mwidth; i++) {
								index = (j * bmWidthBytes) + (i * 3);
								gclr = (image_data[index] + image_data[index + 1] + image_data[index + 2]) / 3;
								if (((index + 2) < imsize) && (i < bmWidth) && (j < mheight)) {
									if (print) {
										mimage.SetPixel(i, j, RGB(gclr, gclr, gclr));
									}
									else {
										mimage.SetPixel(i, mheight - j - 1, RGB(gclr, gclr, gclr));
									}
								}
							}
						}
					}
					else {
						int index = 0;
						for (int j = 0; j < mheight; j++) {
							for (int i = 0; i < mwidth; i++) {
								index = (j * bmWidthBytes) + (i * 3);
								if (((index + 2) < imsize) && (i < bmWidth) && (j < mheight)) {
									if (print) {
										mimage.SetPixel(i, j + 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
									}
									else {
										mimage.SetPixel(i, mheight - j - 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
									}
								}
							}
						}
					}

					delete[] image_data;

					if (print) {
						mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top - brect.Height() + hoff);
					}
					else {
						mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top + hoff);
					}

				}

				delete[] mbits;

			}
			else {
				delete[] cimage_data;
			}

		}

	}

}

void CNetDesignerView::DrawConvolutionInputOutputSound(CDC* pDC, int x, int y, convParameters params, float scale, bool print)
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();

	CPoint pt = LPtoDP(CPoint(x, y));
	CRect brect = pDoc->getConvRect(params, scale);

	int dxmax = brect.right;
	int dymax = (10 * brect.bottom) / 8;

	if (pDoc->nnet.wave_data_allocated && (pDoc->nnet.wave_mem_size > 20)) {

		Linegraph mgraph;

		float nradius = (float)pDoc->nradius2;
		int myradius = int(scale * nradius);

		CRect rect3;
		rect3.left = pt.x + brect.left + myradius;
		rect3.top = pt.y + brect.top + myradius;
		rect3.right = pt.x + brect.right - myradius;
		rect3.bottom = pt.y + brect.bottom - myradius;
		mgraph.setSize(rect3);

		mgraph.setXLabel(_T(""));
		mgraph.xlabel2 = _T("");

		vector<double> xdata;
		vector<double> ddata1;
		vector<double> ddata2;

		double mymax = 0;
		for (int i = 0; i < pDoc->nnet.wave_mem_size; i++) {
			xdata.push_back((double)i);
			ddata1.push_back(pDoc->nnet.wave_data[i]);
			if (pDoc->nnet.wave_data[i] > mymax) {
				mymax = pDoc->nnet.wave_data[i];
			}
		}
		mymax = 1.8 * mymax;

		mgraph.setData(xdata, ddata1, ddata2);

		mgraph.minx = 0;
		mgraph.maxx = (float)ddata1.size();
		mgraph.maxy = (float)mymax;
		mgraph.miny = -1 * mgraph.maxy;

		CClientDC dc(this);
		mgraph.drawFrameNoBackgound(&dc, clr_back);
		mgraph.drawXAxisNoLabels(&dc);
		mgraph.drawData(&dc);

	}
	else {

		int sindex = pDoc->nnet.data_file_index;
		if ((sindex < 0) || (sindex >= pDoc->getDataPathNamesSize(false))) {
			//CString s = _T("");
			//s.Format(_T("DrawConvolutionInputOutputSound : setting index to zero : sindex = %i : dataPathnames size = %i"), sindex, pDoc->getDataPathNamesSize(false));
			//addErrorString(s);
			sindex = 0;	
		}

		wave_data wdata = func.getWaveDataFromFile(pDoc->getDataPathName(sindex, false));
		if (wdata.length > 0) {

			Linegraph mgraph;

			float nradius = (float)pDoc->nradius2;
			int myradius = int(scale * nradius);

			CRect rect3;
			rect3.left = pt.x + brect.left + myradius;
			rect3.top = pt.y + brect.top + myradius;
			rect3.right = pt.x + brect.right - myradius;
			rect3.bottom = pt.y + brect.bottom - myradius;
			mgraph.setSize(rect3);

			mgraph.setXLabel(_T(""));
			mgraph.xlabel2 = _T("");

			vector<double> xdata;
			vector<double> ddata1;
			vector<double> ddata2;

			for (int i = 0; i < wdata.length; i++) {
				xdata.push_back((double)i);
				ddata1.push_back(wdata.data[i]);
			}

			mgraph.setData(xdata, ddata1, ddata2);

			mgraph.minx = 0;
			mgraph.maxx = (float)ddata1.size();
			mgraph.miny = -1 * mgraph.maxy;

			CClientDC dc(this);
			mgraph.drawFrameNoBackgound(&dc, clr_back);
			mgraph.drawXAxisNoLabels(&dc);
			mgraph.drawData(&dc);

		}

	}

}

void CNetDesignerView::DrawHiddenNet(CDC* pDC, int x, int y, float scale, convParameters params, COLORREF clr, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();

	int half_num_speres_wide = 1;
	int half_num_speres_high = 2;

	CRect brect = pDoc->getConvRect(params, scale);

	COLORREF mclr = adjustColorByPercent(clr, -40);

	float nradius = (float)pDoc->nradius2;
	int myradius = int(7.0 * scale * nradius / 8.0);

	int centerx = x + (int)((float)brect.Width() / (2 * scale));
	int centery = y;

	bool gdi_not = false;

	int my = centery;
	for (int j = 0; j < half_num_speres_high; j++) {
		int my = centery - (int)(((float)j * 2.0 * nradius) + nradius);
		for (int i = 0; i < half_num_speres_wide; i++) {
			int mx = centerx;
			drawOpenCircle(pDC, mx, my, myradius, mclr, gdi_not);
		}
		my = centery + (int)(((float)j * 2.0 * nradius) + nradius);
		for (int i = 0; i < half_num_speres_wide; i++) {
			int mx = centerx;
			drawOpenCircle(pDC, mx, my, myradius, mclr, gdi_not);
		}
	}
	
}

void CNetDesignerView::DrawInternalNetCluster(CDC* pDC, int x, int y, float scale, CRect brect, COLORREF clr, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();

	int half_num_speres_wide = 1;
	int half_num_speres_high = 2;

	COLORREF mclr = adjustColorByPercent(clr, -10);

	float nradius = (float)pDoc->nradius2;
	int myradius = int(7.0 * scale * nradius / 8.0);

	int centerx = x + (int)((float)brect.Width() / (2 * scale));
	int centery = y;

	bool gdi_not = false;

	int my = centery;
	for (int j = 0; j < half_num_speres_high; j++) {
		int my = centery - (int)(((float)j * 2.0 * nradius) + nradius);
		for (int i = 0; i < half_num_speres_wide; i++) {
			int mx = centerx;
			CPoint pt = LPtoDP(CPoint(mx, my));
			CRect rect(pt.x - myradius, pt.y - myradius, pt.x + myradius, pt.y + myradius);
			drawCircle(pDC, rect, mclr, false, gdi_not);
		}
		my = centery + (int)(((float)j * 2.0 * nradius) + nradius);
		for (int i = 0; i < half_num_speres_wide; i++) {
			int mx = centerx;
			CPoint pt = LPtoDP(CPoint(mx, my));
			CRect rect(pt.x - myradius, pt.y - myradius, pt.x + myradius, pt.y + myradius);
			drawCircle(pDC, rect, mclr, false, gdi_not);
		}
	}

}

void CNetDesignerView::DrawInternalDropout(CDC* pDC, int x, int y, float scale, CRect brect, COLORREF clr, bool print)
{
	CNetDesignerDoc* pDoc = GetDocument();

	COLORREF mclr = adjustColorByPercent(clr, -10);

	float nradius = (float)pDoc->nradius2;
	int myradius = int(2.0 * scale * (float)pDoc->nradius2 / 3.0F);

	int half_num_spheres_wide = 1;
	int half_num_spheres_high = 2;

	int div = brect.Height() / myradius;
	if (div > 21) {
		half_num_spheres_high += 1;
	}
	//CString s2 = _T("");  s2.Format(_T("DrawInternalDropout : div = %i"), div); addErrorString(s2);

	int centerx = x + (int)((float)brect.Width() / (2 * scale));
	int centery = y;

	bool gdi_not = false;

	int my = centery;
	float offset = 0.0F;
	if ((((div / 4) % 2) != 0) && (div > 15)) {
		offset = nradius;
		for (int i = 0; i < half_num_spheres_wide; i++) {
			int mx = centerx;
			CPoint pt = LPtoDP(CPoint(mx, my));
			CRect rect(pt.x - myradius, pt.y - myradius, pt.x + myradius, pt.y + myradius);
			drawSquare(pDC, rect, mclr, false, gdi_not);
		}
	}
	for (int j = 0; j < half_num_spheres_high; j++) {
		int my = centery - (int)(((float)j * 2.0 * nradius) + nradius + offset);
		for (int i = 0; i < half_num_spheres_wide; i++) {
			int mx = centerx;
			CPoint pt = LPtoDP(CPoint(mx, my));
			CRect rect(pt.x - myradius, pt.y - myradius, pt.x + myradius, pt.y + myradius);
			drawSquare(pDC, rect, mclr, false, gdi_not);
		}
		my = centery + (int)(((float)j * 2.0 * nradius) + nradius + offset);
		for (int i = 0; i < half_num_spheres_wide; i++) {
			int mx = centerx;
			CPoint pt = LPtoDP(CPoint(mx, my));
			CRect rect(pt.x - myradius, pt.y - myradius, pt.x + myradius, pt.y + myradius);
			drawSquare(pDC, rect, mclr, false, gdi_not);
		}
	}

}

void CNetDesignerView::DrawDataConvolution(CDC* pDC, int x, int y, float scale, convParameters params, COLORREF clr, bool print)
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();

	int woff = 0;
	int hoff = 0;
	int mwidth = 0;
	int mheight = 0;

	bool special = false;
	if (params.function == _T("Tanh")) {
		special = true;
	}

	int cindex = -1;
	CString cname = params.id;
	if (pDoc->nnet.allocated) {
		for (int i = 0; i < pDoc->nnet.num_active_convs; i++) {
			if (pDoc->nnet.newConvs[i].id == string(CT2CA(cname))) {
				cindex = i;
				i = pDoc->nnet.num_active_convs;
			}
		}
	}

	if (cindex >= 0) {

		if (pDoc->nnet.newConvs[cindex].allocated) {

			int image_width = pDoc->nnet.newConvs[cindex].outputdims[0];
			int image_height = pDoc->nnet.newConvs[cindex].outputdims[1];

			CRect rect;
			CBrush brush3;
			brush3.CreateSolidBrush(RGB(180, 180, 180));
			CPoint pt = LPtoDP(CPoint(x, y));
			CRect brect = pDoc->getConvRect(params, scale);

			if ((image_width > 0) && (image_height > 0)) {

				if (!pDoc->nnet.newConvs[cindex].allocated) {

					mwidth = image_width;
					mheight = image_height;

					if (mwidth > brect.Width()) {
						mwidth = brect.Width();
					}
					if (mheight > brect.Height()) {
						mheight = brect.Height();
					}
					if ((mwidth < brect.Width()) || (mheight < brect.Height())) {
						woff = (brect.Width() - mwidth) / 2;
						hoff = (brect.Height() - mheight) / 2;
					}

					int l = pt.x + woff;
					int r = l + mwidth;
					int t = pt.y + hoff;
					int b = t + mheight;
					if (print) {
						b = t - mheight;
					}
					rect.SetRect(l, t, r, b);
					pDC->FillRect(rect, &brush3);

					}

				else if (pDoc->nnet.newConvs[cindex].numdims == 2) {

					int bmWidth = pDoc->nnet.newConvs[cindex].outputdims[0];
					int bmHeight = pDoc->nnet.newConvs[cindex].outputdims[1];
					int size2 = pDoc->nnet.newConvs[cindex].output_data_size;
					int bmWidthBytes = 3 * bmWidth;

					double* cimage_data = pDoc->nnet.newConvs[cindex].output_data;

					int imsize2 = 3 * size2;
					unsigned char* mbits = new unsigned char[imsize2];
					if (mbits == 0) {
						addErrorString(_T("DrawConvolution : unable to allocate mbits"));
						return;
					}

					int index1 = 0;
					int index2 = 0;
					for (int j = 0; j < bmHeight; j++) {
						for (int i = 0; i < bmWidth; i++) {
							index1 = (j * bmWidth) + i;
							index2 = (j * 3 * bmWidth) + (i * 3);

							COLORREF mcolor = getColor(cimage_data[index1], special);

							mbits[index2 + 2] = GetRValue(mcolor);
							mbits[index2 + 1] = GetGValue(mcolor);
							mbits[index2 + 0] = GetBValue(mcolor);
						}
					}

					int imsize = 3 * (brect.Width() + 0) * (brect.Height() + 2);
					if (imsize2 > imsize) {
						imsize = imsize2;
					}
					unsigned char* image_data = new unsigned char[imsize];

					if (image_data != 0) {

						int woff = 0;
						int hoff = 0;
						int mwidth = brect.Width();
						int mheight = brect.Height();
						if (mwidth > image_width) {
							woff = (mwidth - image_width) / 2;
							mwidth = image_width;
						}
						if (mheight > image_height) {
							hoff = (mheight - image_height) / 2;
							mheight = image_height;
						}

						if ((bmWidth > mwidth) || (bmHeight > mheight)) {
							image_data = func.downscaleImageData(bmHeight, bmWidth, bmWidthBytes, mbits, mheight, mwidth, image_data);
							bmWidth = mwidth;
							bmHeight = mheight;
							bmWidthBytes = 3 * mwidth;
						}
						else {
							for (int i = 0; i < imsize; i++) {
								image_data[i] = 80;
							}
							int index2 = 0;
							for (int j = 0; j < bmHeight; j++) {
								for (int i = 0; i < bmWidth; i++) {
									index2 = (j * bmWidthBytes) + (i * 3);
									image_data[index2 + 2] = mbits[index2 + 2];
									image_data[index2 + 1] = mbits[index2 + 1];
									image_data[index2 + 0] = mbits[index2 + 0];
								}
							}
						}

						CImage mimage;
						mimage.Create(mwidth, mheight, 24);

						int index = 0;
						for (int j = 0; j < mheight; j++) {
							for (int i = 0; i < mwidth; i++) {
								index = (j * bmWidthBytes) + (i * 3);
								if (((index + 2) < imsize) && (i < bmWidth) && (j < mheight)) {
									if (print) {
										mimage.SetPixel(i, j + 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
									}
									else {
										mimage.SetPixel(i, mheight - j - 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
									}
								}
							}
						}

						delete[] image_data;
						delete[] mbits;

						if (print) {
							mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top - brect.Height() + hoff);
						}
						else {
							mimage.Draw(*pDC, pt.x - brect.left + woff, pt.y + brect.top + hoff);
						}

					}

				}

			}

		}

	}
	else {

		float nwidth = 1 * (float)pDoc->lwidth;
		int mywidth = int(scale * nwidth);

		COLORREF mclr = adjustColorByPercent(clr, -40);

		CPen pen(PS_SOLID, mywidth, mclr);
		CPen* oldpen = pDC->SelectObject(&pen);
		CPoint pt = LPtoDP(CPoint(x, y));
		CRect brect = pDoc->getConvRect(params, scale);

		int xoffset = brect.Width() / 4;

		int l = pt.x - brect.left + xoffset;
		int r = l + brect.Width() - 2 * xoffset;

		int num_lines = 4;
		for (int i = 0; i < num_lines; i++) {		
			int y = pt.y + brect.top + ((i + 1) * brect.Height()) / (num_lines + 1);
			if (print) {
				y = pt.y + brect.top - ((i + 1) * brect.Height()) / (num_lines + 1);
			}
			pDC->MoveTo(l, y);
			pDC->LineTo(r, y);
		}

		pDC->SelectObject(oldpen);

	}

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

	int hdc = pDC->SetROP2(R2_XORPEN);

	CBrush brush1;
	brush1.CreateSolidBrush(aliceblue);
	pDC->FillRgn(&mrgn, &brush1);
	
	pDC->SetROP2(hdc);

	DeleteObject(brush1);
}

COLORREF CNetDesignerView::getColor(double value, bool special)
{
	COLORREF rval = 0x00000000;

	int red = 255;
	int green = 255;
	int blue = 255;

	double mvmin = 0.0;
	if (special) {
		mvmin = -1.0;
	}
	double mvmax = 1.0;
	double dv = mvmax - mvmin;

	if (value > mvmax) {  
		value = mvmax;  
	}
	else if (value < mvmin) {  
		value = mvmin;  
	}

	if (value < (mvmin + (0.25*dv))) {
		red = 0;
		green = (int)(255.0 * (4.0 * (value - mvmin) / dv));
	}
	else if (value < (mvmin + (0.5 * dv))) {
		red = 0;
		blue = (int)(255.0 * (1.0 + 4.0 * (mvmin + (0.25 * dv) - value) / dv));
	}
	else if (value < (mvmin + (0.75 * dv))) {
		red = (int)(255.0 * (4.0 * (value - mvmin - (0.5 * dv)) / dv));
		blue = 0;
	}
	else {
		green = (int)(255.0 * (1.0 + 4.0 * (mvmin + (0.75 * dv) - value) / dv));
		blue = 0;
	}

	if (red > 255) {
		red = 255;
	}
	else if (red < 0) {
		red = 0;
	}
	if (green > 255) {
		green = 255;
	}
	else if (green < 0) {
		green = 0;
	}
	if (blue > 255) {
		blue = 255;
	}
	else if (blue < 0) {
		blue = 0;
	}

	rval = RGB(red, green, blue);

	return(rval);
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
	float new_angle = (float)(180 * (float)atan2f(dy, dx) / M_PI);
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
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() == 1) {
		CNetDesignerDoc* pDoc = GetDocument();
		CPoint mpt = DPtoLP(point);
		int pindex = pDoc->checkComponentPlacementReturnIndex(mpt.x, mpt.y, selection_type, pDoc->assembly_edit_index);
		if (pindex == pDoc->selectedComponents[0]) {
			return(true);
		}
	}
	else if (pDoc->selectedComponents.size() > 1) {
		CPoint mpt = DPtoLP(point);
		CRect mrect = getSelectedComponentsLogicalRectangle();
		if ((mpt.x > mrect.left) && (mpt.x < mrect.right) && (mpt.y > mrect.top) && (mpt.y < mrect.bottom)) {
			return(true);
		}
	}
	return(false);
}

assembly_sub_component CNetDesignerView::checkSelectionAssemblyNode(CPoint point)
{
	assembly_sub_component rval;
	rval.net_index = -1;
	rval.input_sub_index = -1;
	rval.output_sub_index = -1;
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() == 1) {
		CNetDesignerDoc* pDoc = GetDocument();
		if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[0])) {
			CPoint mpt = DPtoLP(point);
			int pindex = pDoc->checkComponentPlacementReturnIndex(mpt.x, mpt.y, selection_type, pDoc->assembly_edit_index);
			if (pindex == pDoc->selectedComponents[0]) {
				int gindex = pDoc->getAssemblyIndex(pindex);
				vector<CPoint> pts1 = pDoc->getAssemblyInputPoints(pindex, (int)pDoc->nradius2);
				if (pts1.size() > 0) {
					for (int k = 0; k < (int)pts1.size(); k++) {
						int mx = pDoc->netData.getXDisp(pindex) + pts1[k].x;
						int my = pDoc->netData.getYDisp(pindex) + pts1[k].y;
						float r = float(pow((mpt.x - mx), 2) + pow((mpt.y - my), 2));
						r = sqrt(r);
						if (r < pDoc->nradius2) {
							int cnt = 0;
							for (int m = 0; m < pDoc->netData.size(); m++) {
								if (pDoc->netData.isNode(m) && (pDoc->netData.getNodeMode(m) == "Input")) {
									//CString s2 = _T("");  s2.Format(_T("testing : getGroupIndex = %i : gindex = %i : "), pDoc->netData.getGroupIndex(m), gindex);  addErrorString(s2 + (CString)pDoc->netData.getID(m).c_str());
									if (pDoc->netData.getGroupIndex(m) == gindex) {
										//CString s2 = _T("");  s2.Format(_T("testing : k = %i : cnt = %i : "), k, cnt);  addErrorString(s2 + (CString)pDoc->netData.getID(m).c_str());
										if (cnt == k) {
											//addErrorString(_T("checkSelectionAssemblyNode : found input ") + (CString)pDoc->netData.getID(m).c_str());
											rval.net_index = m;
											rval.input_sub_index = k;
											return(rval);
										}
										cnt++;
									}
								}
							}
							AfxMessageBox(_T("checkSelectionAssemblyNode : input not found"));
						}
					}
				}
				vector<CPoint> pts2 = pDoc->getAssemblyOutputPoints(pindex, (int)pDoc->nradius2);
				if (pts2.size() > 0) {
					for (int k = 0; k < (int)pts2.size(); k++) {
						int mx = pDoc->netData.getXDisp(pindex) + pts2[k].x;
						int my = pDoc->netData.getYDisp(pindex) + pts2[k].y;
						float r = float(pow((mpt.x - mx), 2) + pow((mpt.y - my), 2));
						r = sqrt(r);
						if (r < pDoc->nradius2) {
							int cnt = 0;
							for (int m = 0; m < pDoc->netData.size(); m++) {
								if (pDoc->netData.isNode(m) && (pDoc->netData.getNodeMode(m) == "Output")) {
									//CString s2 = _T("");  s2.Format(_T("testing : getGroupIndex = %i : gindex = %i : "), pDoc->netData.getGroupIndex(m), gindex);  addErrorString(s2 + (CString)pDoc->netData.getID(m).c_str());
									if (pDoc->netData.getGroupIndex(m) == gindex) {
										//CString s2 = _T("");  s2.Format(_T("testing : k = %i : cnt = %i : "), k, cnt);  addErrorString(s2 + (CString)pDoc->netData.getID(m).c_str());
										if (cnt == k) {
											//addErrorString(_T("checkSelectionAssemblyNode : found output ") + (CString)pDoc->netData.getID(m).c_str());
											rval.net_index = m;
											rval.output_sub_index = k;
											return(rval);
										}
										cnt++;
									}
								}
							}
							AfxMessageBox(_T("checkSelectionAssemblyNode : output not found"));
						}
					}
				}
				return(rval);
			}
		}
	}
	return(rval);
}

bool CNetDesignerView::checkIfNodeSelected()
{
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
		if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
			return(true);
		}
		else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
			return(true);
		}
		else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[i])) {
			return(true);
		}
		else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[i])) {
			return(true);
		}
		else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[i])) {
			return(true);
		}
		else if (pDoc->netData.isDropout(pDoc->selectedComponents[i])) {
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
				CPoint pt1 = pDoc->getInputNodePoint(i, copyData);
				CPoint pt2 = pDoc->getOutputNodePoint(i, copyData);
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
			else if (copyData.isConvLayer(i)) {
				convParameters params;
				params.type = copyData.getConvType(i);
				params.mode = (CString)copyData.getNodeMode(i).c_str();
				params.function = (CString)copyData.getNodeFunction(i).c_str();
				params.id = (CString)copyData.getID(i).c_str();
				CRect crect = pDoc->getConvRect(params, 1.0F);
				int tx1 = copyData.getXDisp(i) - crect.left;
				int tx2 = copyData.getXDisp(i) + crect.right - (2 * crect.left);
				int ty1 = copyData.getYDisp(i) - crect.top;
				int ty2 = copyData.getYDisp(i) + crect.bottom + (2 * crect.top);
				if (tx2 > maxx) {
					maxx = tx2;
				}
				if (tx1 < minx) {
					minx = tx1;
				}
				if (ty2 > maxy) {
					maxy = ty2;
				}
				if (ty1 < miny) {
					miny = ty1;
				}
			}
			else if (copyData.isAttentionLayer(i)) {
				CRect crect = pDoc->getAttentionRect(1.0F);
				int tx1 = copyData.getXDisp(i) - crect.left;
				int tx2 = copyData.getXDisp(i) + crect.right - (2 * crect.left);
				int ty1 = copyData.getYDisp(i) - crect.top;
				int ty2 = copyData.getYDisp(i) + crect.bottom + (2 * crect.top);
				if (tx2 > maxx) {
					maxx = tx2;
				}
				if (tx1 < minx) {
					minx = tx1;
				}
				if (ty2 > maxy) {
					maxy = ty2;
				}
				if (ty1 < miny) {
					miny = ty1;
				}
			}
			else if (copyData.isAssemblyLayer(i)) {
				CRect crect = pDoc->getAssemblyRect(i, 1.0F);
				int tx1 = copyData.getXDisp(i) - crect.left;
				int tx2 = copyData.getXDisp(i) + crect.right - (2 * crect.left);
				int ty1 = copyData.getYDisp(i) - crect.top;
				int ty2 = copyData.getYDisp(i) + crect.bottom + (2 * crect.top);
				if (tx2 > maxx) {
					maxx = tx2;
				}
				if (tx1 < minx) {
					minx = tx1;
				}
				if (ty2 > maxy) {
					maxy = ty2;
				}
				if (ty1 < miny) {
					miny = ty1;
				}
			}
			else if (copyData.isNetCluster(i)) {
				CRect crect = pDoc->getNetClusterRect(1.0F);
				int tx1 = copyData.getXDisp(i) - crect.left;
				int tx2 = copyData.getXDisp(i) + crect.right - (2 * crect.left);
				int ty1 = copyData.getYDisp(i) - crect.top;
				int ty2 = copyData.getYDisp(i) + crect.bottom + (2 * crect.top);
				if (tx2 > maxx) {
					maxx = tx2;
				}
				if (tx1 < minx) {
					minx = tx1;
				}
				if (ty2 > maxy) {
					maxy = ty2;
				}
				if (ty1 < miny) {
					miny = ty1;
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
	if (pDoc->selectedComponents.size() == 1) {
		if (pDoc->netData.getGroupIndex(0) == pDoc->assembly_edit_index) {
			if (pDoc->netData.size() >= pDoc->selectedComponents[0]) {
				rval.left = pDoc->netData.getXDisp(pDoc->selectedComponents[0]) - (int)pDoc->nradius;
				rval.right = pDoc->netData.getXDisp(pDoc->selectedComponents[0]) + (int)pDoc->nradius;
				rval.top = pDoc->netData.getYDisp(pDoc->selectedComponents[0]) - (int)pDoc->nradius;
				rval.bottom = pDoc->netData.getYDisp(pDoc->selectedComponents[0]) + (int)pDoc->nradius;
			}
		}
	}
	else if (pDoc->selectedComponents.size() > 1) {
		rval.left = logicalWidth;
		rval.right = -logicalWidth;
		rval.top = logicalHeight;
		rval.bottom = -logicalHeight;
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.getGroupIndex(i) == pDoc->assembly_edit_index) {
				if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
					if ((pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + (int)pDoc->nradius) > rval.right) {
						rval.right = (pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + (int)pDoc->nradius);
					}
					if ((pDoc->netData.getXDisp(pDoc->selectedComponents[i]) - (int)pDoc->nradius) < rval.left) {
						rval.left = (pDoc->netData.getXDisp(pDoc->selectedComponents[i]) - (int)pDoc->nradius);
					}
					if ((pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + (int)pDoc->nradius) > rval.bottom) {
						rval.bottom = (pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + (int)pDoc->nradius);
					}
					if ((pDoc->netData.getYDisp(pDoc->selectedComponents[i]) - (int)pDoc->nradius) < rval.top) {
						rval.top = (pDoc->netData.getYDisp(pDoc->selectedComponents[i]) - (int)pDoc->nradius);
					}
				}
				else if (pDoc->netData.isConnect(pDoc->selectedComponents[i])) {
					if (!nodes_only) {
						CPoint pt1 = pDoc->getInputNodePoint(i);
						CPoint pt2 = pDoc->getOutputNodePoint(i);
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
				else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
					CRect crect = pDoc->getConvRect(pDoc->getConvParameters(pDoc->selectedComponents[i]), 1.0F);
					int tx1 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) - crect.left;
					int tx2 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) - crect.top;
					int ty2 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[i])) {
					CRect crect = pDoc->getAttentionRect(1.0F);
					int tx1 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) - crect.left;
					int tx2 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) - crect.top;
					int ty2 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[i])) {
					CRect crect = pDoc->getAssemblyRect(pDoc->selectedComponents[i], 1.0F);
					int tx1 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) - crect.left;
					int tx2 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) - crect.top;
					int ty2 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[i])) {
					CRect crect = pDoc->getNetClusterRect(1.0F);
					int tx1 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) - crect.left;
					int tx2 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) - crect.top;
					int ty2 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isDropout(pDoc->selectedComponents[i])) {
					CRect crect = pDoc->getNetClusterRect(1.0F);
					int tx1 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) - crect.left;
					int tx2 = pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) - crect.top;
					int ty2 = pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
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
		if (pDoc->netData.getGroupIndex(0) == pDoc->assembly_edit_index) {
			rval.left = pDoc->netData.getXDisp(0) - (int)pDoc->nradius;
			rval.right = pDoc->netData.getXDisp(0) + (int)pDoc->nradius;
			rval.top = pDoc->netData.getYDisp(0) - (int)pDoc->nradius;
			rval.bottom = pDoc->netData.getYDisp(0) + (int)pDoc->nradius;
		}
	}
	else if (pDoc->netData.size() > 1) {
		rval.left = logicalWidth;
		rval.right = -logicalWidth;
		rval.top = logicalHeight;
		rval.bottom = -logicalHeight;
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.getGroupIndex(i) == pDoc->assembly_edit_index) {
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
					CPoint pt1 = pDoc->getInputNodePoint(i);
					CPoint pt2 = pDoc->getOutputNodePoint(i);
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
				else if (pDoc->netData.isConvLayer(i)) {
					CRect crect = pDoc->getConvRect(pDoc->getConvParameters(i), 1.0F);
					int tx1 = pDoc->netData.getXDisp(i) - crect.left;
					int tx2 = pDoc->netData.getXDisp(i) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(i) - crect.top;
					int ty2 = pDoc->netData.getYDisp(i) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isAttentionLayer(i)) {
					CRect crect = pDoc->getAttentionRect(1.0F);
					int tx1 = pDoc->netData.getXDisp(i) - crect.left;
					int tx2 = pDoc->netData.getXDisp(i) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(i) - crect.top;
					int ty2 = pDoc->netData.getYDisp(i) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isAssemblyLayer(i)) {
					CRect crect = pDoc->getAssemblyRect(i, 1.0F);
					int tx1 = pDoc->netData.getXDisp(i) - crect.left;
					int tx2 = pDoc->netData.getXDisp(i) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(i) - crect.top;
					int ty2 = pDoc->netData.getYDisp(i) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isNetCluster(i)) {
					CRect crect = pDoc->getNetClusterRect(1.0F);
					int tx1 = pDoc->netData.getXDisp(i) - crect.left;
					int tx2 = pDoc->netData.getXDisp(i) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(i) - crect.top;
					int ty2 = pDoc->netData.getYDisp(i) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
					}
				}
				else if (pDoc->netData.isDropout(i)) {
					CRect crect = pDoc->getNetClusterRect(1.0F);
					int tx1 = pDoc->netData.getXDisp(i) - crect.left;
					int tx2 = pDoc->netData.getXDisp(i) + crect.right - (2 * crect.left);
					int ty1 = pDoc->netData.getYDisp(i) - crect.top;
					int ty2 = pDoc->netData.getYDisp(i) + crect.bottom + (2 * crect.top);
					if (tx2 > rval.right) {
						rval.right = tx2;
					}
					if (tx1 < rval.left) {
						rval.left = tx1;
					}
					if (ty2 > rval.bottom) {
						rval.bottom = ty2;
					}
					if (ty1 < rval.top) {
						rval.top = ty1;
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

void CNetDesignerView::drawSelectedComponents(int mode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			int index = pDoc->selectedComponents[i];
			if (pDoc->netData.isNode(index)) {
				DrawNode(index, zoomScale, mode, false);
			}
			else if (pDoc->netData.isConnect(index)) {
				DrawConnect(index, zoomScale, mode);
			}
			else if (pDoc->netData.isConvLayer(index)) {
				DrawConvolution(index, zoomScale, mode);
			}
			else if (pDoc->netData.isAttentionLayer(index)) {
				DrawAttention(index, zoomScale, mode);
			}
			else if (pDoc->netData.isAssemblyLayer(index)) {
				assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
				DrawAssembly(index, zoomScale, mode, asub);
			}
			else if (pDoc->netData.isNetCluster(index)) {
				DrawNetCluster(index, zoomScale, mode);
			}
			else if (pDoc->netData.isDropout(index)) {
				DrawDropout(index, zoomScale, mode);
			}
		}
	}
}

void CNetDesignerView::moveSelectedItems(CPoint frompt, CPoint topt)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		CClientDC dc(this);
		OnPrepareDC(&dc);
		CAuxFunctions func;
		int x1, y1, x2, y2, ctype;
		CString type;
		CString function;
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isConnect(pDoc->selectedComponents[i])) {
				string input_node = func.getConvNameFromConnectName(pDoc->netData.getInputNode(pDoc->selectedComponents[i]));
				string output_node = func.getConvNameFromConnectName(pDoc->netData.getOutputNode(pDoc->selectedComponents[i]));
				if ((func.getConvNodeFromConnectName((CString)input_node.c_str()) < 1) && (func.getConvNodeFromConnectName((CString)output_node.c_str()) < 1)) {
					CPoint pt1 = pDoc->getInputNodePoint(pDoc->selectedComponents[i]);
					CPoint pt2 = pDoc->getOutputNodePoint(pDoc->selectedComponents[i]);
					CPoint pt3 = pt1;
					CPoint pt4 = pt2;
					bool fixed_input = true;
					bool fixed_output = true;
					for (int j = 0; j < pDoc->selectedComponents.size(); j++) {
						if (pDoc->netData.isNode(pDoc->selectedComponents[j]) || pDoc->netData.isConvLayer(pDoc->selectedComponents[j])) {
							if (input_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt3 = applyGrid(CPoint(pt1.x + topt.x, pt1.y + topt.y));
								pt1 = selectedComponentsPoints1[i];
								fixed_input = false;
							}
							if (output_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt4 = applyGrid(CPoint(pt2.x + topt.x, pt2.y + topt.y));
								pt2 = selectedComponentsPoints2[i];
								fixed_output = false;
							}
							if (!fixed_input && !fixed_output) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[j])) {
							if (input_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt3 = applyGrid(CPoint(pt1.x + topt.x, pt1.y + topt.y));
								pt1 = selectedComponentsPoints1[i];
								fixed_input = false;
							}
							if (output_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt4 = applyGrid(CPoint(pt2.x + topt.x, pt2.y + topt.y));
								pt2 = selectedComponentsPoints2[i];
								fixed_output = false;
							}
							if (!fixed_input && !fixed_output) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[j])) {
							if (input_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt3 = applyGrid(CPoint(pt1.x + topt.x, pt1.y + topt.y));
								pt1 = selectedComponentsPoints1[i];
								fixed_input = false;
							}
							if (output_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt4 = applyGrid(CPoint(pt2.x + topt.x, pt2.y + topt.y));
								pt2 = selectedComponentsPoints2[i];
								fixed_output = false;
							}
							if (!fixed_input && !fixed_output) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[j])) {
							if (input_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt3 = applyGrid(CPoint(pt1.x + topt.x, pt1.y + topt.y));
								pt1 = selectedComponentsPoints1[i];
								fixed_input = false;
							}
							if (output_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt4 = applyGrid(CPoint(pt2.x + topt.x, pt2.y + topt.y));
								pt2 = selectedComponentsPoints2[i];
								fixed_output = false;
							}
							if (!fixed_input && !fixed_output) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isDropout(pDoc->selectedComponents[j])) {
							if (input_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt3 = applyGrid(CPoint(pt1.x + topt.x, pt1.y + topt.y));
								pt1 = selectedComponentsPoints1[i];
								fixed_input = false;
							}
							if (output_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								pt4 = applyGrid(CPoint(pt2.x + topt.x, pt2.y + topt.y));
								pt2 = selectedComponentsPoints2[i];
								fixed_output = false;
							}
							if (!fixed_input && !fixed_output) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
					}
					DrawConnect(&dc, pt1, pt2, zoomScale, DRAW_HIGHLIGHT);
					DrawConnect(&dc, pt3, pt4, zoomScale, DRAW_HIGHLIGHT);
					if (i < selectedComponentsPoints1.size()) {
						selectedComponentsPoints1[i] = pt3;
					}
					if (i < selectedComponentsPoints2.size()) {
						selectedComponentsPoints2[i] = pt4;
					}
				}
			}
		}
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
				function = (CString)pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]).c_str();
				x1 = selectedComponentsPoints1[i].x;
				y1 = selectedComponentsPoints1[i].y;
				x2 = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				if (i < selectedComponentsPoints1.size()) {
					selectedComponentsPoints1[i] = CPoint(x2, y2);
				}
				else {
					addErrorString(_T("moveSelectedItems : index greater than moveSelectedItems size"));
				}
				DrawNode(&dc, x1, y1, type, function, zoomScale, DRAW_HIGHLIGHT, false);
				DrawNode(&dc, x2, y2, type, function, zoomScale, DRAW_HIGHLIGHT, false);
			}
			else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
				ctype = pDoc->netData.getConvType(pDoc->selectedComponents[i]);
				x1 = selectedComponentsPoints1[i].x;
				y1 = selectedComponentsPoints1[i].y;
				x2 = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				if (i < selectedComponentsPoints1.size()) {
					selectedComponentsPoints1[i] = CPoint(x2, y2);
				}
				else {
					addErrorString(_T("moveSelectedItems : index greater than moveSelectedItems size"));
				}
				bool gray = pDoc->getDataImageGray();
				vector<int> dimensions = pDoc->netData.getConvDimensions(pDoc->selectedComponents[i]);
				vector<CPoint> locations = convertValuesToPoints(pDoc->netData.getConvLocations(pDoc->selectedComponents[i]));
				convParameters params = pDoc->getConvParameters(pDoc->selectedComponents[i]);
				DrawConvolution(&dc, x1, y1, gray, params, dimensions, locations, zoomScale, DRAW_HIGHLIGHT, ctype);
				DrawConvolution(&dc, x2, y2, gray, params, dimensions, locations, zoomScale, DRAW_HIGHLIGHT, ctype);
			}
			else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[i])) {
				x1 = selectedComponentsPoints1[i].x;
				y1 = selectedComponentsPoints1[i].y;
				x2 = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				if (i < selectedComponentsPoints1.size()) {
					selectedComponentsPoints1[i] = CPoint(x2, y2);
				}
				else {
					addErrorString(_T("moveSelectedItems : index greater than moveSelectedItems size"));
				}
				DrawAttention(&dc, x1, y1, zoomScale, DRAW_HIGHLIGHT, pDoc->netData.isSelfAttentionLayer(pDoc->selectedComponents[i]));
				DrawAttention(&dc, x2, y2, zoomScale, DRAW_HIGHLIGHT, pDoc->netData.isSelfAttentionLayer(pDoc->selectedComponents[i]));
			}
			else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[i])) {
				x1 = selectedComponentsPoints1[i].x;
				y1 = selectedComponentsPoints1[i].y;
				x2 = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				if (i < selectedComponentsPoints1.size()) {
					selectedComponentsPoints1[i] = CPoint(x2, y2);
				}
				else {
					addErrorString(_T("moveSelectedItems : index greater than moveSelectedItems size"));
				}
				assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
				DrawAssembly2(&dc, x1, y1, pDoc->selectedComponents[i], zoomScale, DRAW_HIGHLIGHT, asub);
				DrawAssembly2(&dc, x2, y2, pDoc->selectedComponents[i], zoomScale, DRAW_HIGHLIGHT, asub);
			}
			else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[i])) {
				x1 = selectedComponentsPoints1[i].x;
				y1 = selectedComponentsPoints1[i].y;
				x2 = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				if (i < selectedComponentsPoints1.size()) {
					selectedComponentsPoints1[i] = CPoint(x2, y2);
				}
				else {
					addErrorString(_T("moveSelectedItems : index greater than moveSelectedItems size"));
				}
				DrawNetCluster(&dc, x1, y1, zoomScale, DRAW_HIGHLIGHT, pDoc->netData.getNodeMode(pDoc->selectedComponents[i]));
				DrawNetCluster(&dc, x2, y2, zoomScale, DRAW_HIGHLIGHT, pDoc->netData.getNodeMode(pDoc->selectedComponents[i]));
			}
			else if (pDoc->netData.isDropout(pDoc->selectedComponents[i])) {
				x1 = selectedComponentsPoints1[i].x;
				y1 = selectedComponentsPoints1[i].y;
				x2 = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y2 = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				if (i < selectedComponentsPoints1.size()) {
					selectedComponentsPoints1[i] = CPoint(x2, y2);
				}
				else {
					addErrorString(_T("moveSelectedItems : index greater than moveSelectedItems size"));
				}
				DrawDropout(&dc, x1, y1, zoomScale, DRAW_HIGHLIGHT);
				DrawDropout(&dc, x2, y2, zoomScale, DRAW_HIGHLIGHT);
			}
		}
	}
}

void CNetDesignerView::finalMoveSelectedItems(CPoint topt)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		CClientDC dc(this);
		OnPrepareDC(&dc);
		int x, y, ctype;
		string type;
		CString func;
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
				type = pDoc->netData.getNodeMode(pDoc->selectedComponents[i]);
				func = (CString)pDoc->netData.getNodeFunction(pDoc->selectedComponents[i]).c_str();
				x = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				DrawNode(&dc, x, y, (CString)type.c_str(), func, zoomScale, DRAW_HIGHLIGHT, false);
			}
			else if (pDoc->netData.isConnect(pDoc->selectedComponents[i])) {
				CAuxFunctions func;
				bool fixed_in = true;
				bool fixed_out = true;
				string in_node = pDoc->netData.getInputNode(pDoc->selectedComponents[i]);
				string out_node = pDoc->netData.getOutputNode(pDoc->selectedComponents[i]);
				int cnode1 = func.getConvNodeFromConnectName((CString)in_node.c_str());
				int cnode2 = func.getConvNodeFromConnectName((CString)out_node.c_str());
				in_node = func.getConvNameFromConnectName(in_node);
				out_node = func.getConvNameFromConnectName(out_node);
				if ((cnode1 < 1) && (cnode2 < 1)) {
					for (int j = 0; j < pDoc->selectedComponents.size(); j++) {
						if (pDoc->netData.isNode(pDoc->selectedComponents[j]) || pDoc->netData.isConvLayer(pDoc->selectedComponents[j])) {
							if (in_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_in = false;
							}
							if (out_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_out = false;
							}
							if (!fixed_in && !fixed_out) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[j])) {
							if (in_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_in = false;
							}
							if (out_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_out = false;
							}
							if (!fixed_in && !fixed_out) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[j])) {
							if (in_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_in = false;
							}
							if (out_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_out = false;
							}
							if (!fixed_in && !fixed_out) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[j])) {
							if (in_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_in = false;
							}
							if (out_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_out = false;
							}
							if (!fixed_in && !fixed_out) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isDropout(pDoc->selectedComponents[j])) {
							if (in_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_in = false;
							}
							if (out_node == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								fixed_out = false;
							}
							if (!fixed_in && !fixed_out) {
								j = (int)pDoc->selectedComponents.size();
							}
						}
					}
					CPoint pt1 = pDoc->getInputNodePoint(pDoc->selectedComponents[i]);
					CPoint pt2 = pDoc->getOutputNodePoint(pDoc->selectedComponents[i]);
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
			}
			else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
				type = pDoc->netData.getNodeMode(pDoc->selectedComponents[i]);
				ctype = pDoc->netData.getConvType(pDoc->selectedComponents[i]);
				x = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				bool gray = pDoc->getDataImageGray();
				vector<int> dimensions = pDoc->netData.getConvDimensions(pDoc->selectedComponents[i]);
				vector<CPoint> locations = convertValuesToPoints(pDoc->netData.getConvLocations(pDoc->selectedComponents[i]));
				convParameters params = pDoc->getConvParameters(pDoc->selectedComponents[i]);
				DrawConvolution(&dc, x, y, gray, params, dimensions, locations, zoomScale, DRAW_HIGHLIGHT, ctype);
			}
			else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[i])) {
				x = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				DrawAttention(&dc, x, y, zoomScale, DRAW_HIGHLIGHT, pDoc->netData.isSelfAttentionLayer(pDoc->selectedComponents[i]));
			}
			else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[i])) {
				x = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
				DrawAssembly2(&dc, x, y, pDoc->selectedComponents[i], zoomScale, DRAW_HIGHLIGHT, asub);
			}
			else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[i])) {
				x = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				DrawNetCluster(&dc, x, y, zoomScale, DRAW_HIGHLIGHT, pDoc->netData.getNodeMode(pDoc->selectedComponents[i]));
			}
			else if (pDoc->netData.isDropout(pDoc->selectedComponents[i])) {
				x = applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i]) + topt.x);
				y = applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]) + topt.y);
				DrawDropout(&dc, x, y, zoomScale, DRAW_HIGHLIGHT);
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
				if (copyData.getNodeZeroOutput(i) && (mode == DRAW_NORMAL)) {
					DrawNode(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, (CString)copyData.getNodeMode(i).c_str(), (CString)copyData.getNodeFunction(i).c_str(), zoomScale, DRAW_ZERO, false);
				}
				else if (!copyData.getNodeTrainable(i) && (mode == DRAW_NORMAL)) {
					DrawNode(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, (CString)copyData.getNodeMode(i).c_str(), (CString)copyData.getNodeFunction(i).c_str(), zoomScale, DRAW_INACTIVE, false);
				}
				else {
					DrawNode(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, (CString)copyData.getNodeMode(i).c_str(), (CString)copyData.getNodeFunction(i).c_str(), zoomScale, mode, false);
				}
			}
			else if (copyData.isConnect(i)) {
				CPoint pt1 = pDoc->getInputNodePoint(i, copyData);
				CPoint pt2 = pDoc->getOutputNodePoint(i, copyData);
				CPoint mpt3(pt1.x + x, pt1.y + y);
				CPoint mpt4(pt2.x + x, pt2.y + y);
				DrawConnect(&dc, mpt3, mpt4, zoomScale, mode);
			}
			else if (copyData.isConvLayer(i)) {
				bool gray = pDoc->getDataImageGray();
				int ctype = copyData.getConvType(i);
				convParameters params;
				params.type = copyData.getConvType(i);
				params.mode = (CString)copyData.getNodeMode(i).c_str();
				params.function = (CString)copyData.getNodeFunction(i).c_str();
				params.id = (CString)copyData.getID(i).c_str();
				vector<CPoint> locations = convertValuesToPoints(copyData.getConvLocations(i));
				if (copyData.getNodeZeroOutput(i) && (mode == DRAW_NORMAL)) {
					DrawConvolution(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, gray, params, copyData.getConvDimensions(i), locations, zoomScale, DRAW_ZERO, ctype);
				}
				else if (!copyData.getNodeTrainable(i) && (mode == DRAW_NORMAL)) {
					DrawConvolution(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, gray, params, copyData.getConvDimensions(i), locations, zoomScale, DRAW_INACTIVE, ctype);
				}
				else {
					DrawConvolution(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, gray, params, copyData.getConvDimensions(i), locations, zoomScale, mode, ctype);
				}
			}
			else if (copyData.isAttentionLayer(i)) {
				DrawAttention(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, zoomScale, mode, copyData.isSelfAttentionLayer(i));
			}
			else if (copyData.isAssemblyLayer(i)) {
				assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
				DrawAssembly2(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, i, zoomScale, mode, asub);
			}
			else if (copyData.isNetCluster(i)) {
				DrawNetCluster(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, zoomScale, mode, copyData.getNodeMode(i));
			}
			else if (copyData.isDropout(i)) {
				DrawDropout(&dc, copyData.getXDisp(i) + x, copyData.getYDisp(i) + y, zoomScale, mode);
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
				DrawNode(&dc, newGroupData.getXDisp(i) + x, newGroupData.getYDisp(i) + y, (CString)newGroupData.getNodeMode(i).c_str(), (CString)newGroupData.getNodeFunction(i).c_str(), zoomScale, mode, false);
			}
			else if (newGroupData.isConnect(i)) {
				CPoint pt1 = pDoc->getInputNodePoint(i, newGroupData);
				CPoint pt2 = pDoc->getOutputNodePoint(i, newGroupData);
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

bool CNetDesignerView::selectComponent(CPoint point, bool add, assembly_sub_component asub)
{
	if (!add) {
		endSelect();
	}
	CNetDesignerDoc* pDoc = GetDocument();
	CPoint mpt = point;
	mpt = DPtoLP(mpt);
	int pindex = pDoc->checkComponentPlacementReturnIndex(mpt.x, mpt.y, selection_type, pDoc->assembly_edit_index);
	if (pindex >= 0) {
		if (pDoc->netData.isNode(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			pDoc->selectedComponents.push_back(pindex);
			selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
			selectedComponentsPoints2.push_back(CPoint(0, 0));
			DrawNode(pindex, zoomScale, DRAW_HIGHLIGHT, false);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
			updateDataToEquation();
			checkAssemblyEditable();
			return(true);
		}
		else if (pDoc->netData.isConnect(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_CONNECTIONS))) {
			pDoc->selectedComponents.push_back(pindex);
			selectedComponentsPoints1.push_back(pDoc->getInputNodePoint(pindex));
			selectedComponentsPoints2.push_back(pDoc->getOutputNodePoint(pindex));
			DrawConnect(pindex, zoomScale, DRAW_ERASE);
			DrawConnect(pindex, zoomScale, DRAW_HIGHLIGHT);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
			updateDataToEquation();
			checkAssemblyEditable();
			return(true);
		}
		else if (pDoc->netData.isConvLayer(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			pDoc->selectedComponents.push_back(pindex);
			selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
			selectedComponentsPoints2.push_back(CPoint(0, 0));
			DrawConvolution(pindex, zoomScale, DRAW_HIGHLIGHT);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
			updateDataToEquation();
			checkAssemblyEditable();
			return(true);
		}
		else if (pDoc->netData.isAttentionLayer(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			pDoc->selectedComponents.push_back(pindex);
			selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
			selectedComponentsPoints2.push_back(CPoint(0, 0));
			DrawAttention(pindex, zoomScale, DRAW_HIGHLIGHT);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
			updateDataToEquation();
			return(true);
		}
		else if (pDoc->netData.isAssemblyLayer(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			pDoc->selectedComponents.push_back(pindex);
			selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
			selectedComponentsPoints2.push_back(CPoint(0, 0));
			DrawAssembly(pindex, zoomScale, DRAW_HIGHLIGHT, asub);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents, asub.net_index);
			updateDataToEquation();
			checkAssemblyEditable();
			return(true);
		}
		else if (pDoc->netData.isNetCluster(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			pDoc->selectedComponents.push_back(pindex);
			selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
			selectedComponentsPoints2.push_back(CPoint(0, 0));
			DrawNetCluster(pindex, zoomScale, DRAW_HIGHLIGHT);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
			updateDataToEquation();
			checkAssemblyEditable();
			return(true);
		}
		else if (pDoc->netData.isDropout(pindex) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
			pDoc->selectedComponents.push_back(pindex);
			selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
			selectedComponentsPoints2.push_back(CPoint(0, 0));
			DrawDropout(pindex, zoomScale, DRAW_HIGHLIGHT);
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
			checkAssemblyEditable();
			updateDataToEquation();
			return(true);
		}
		else {
			addErrorString("selectComponent : unable to process " + pDoc->netData.getID(pindex));
		}
	}
	checkAssemblyEditable();
	return(false);
}

bool CNetDesignerView::selectComponent(int index, bool add)
{
	if (!add) {
		endSelect();
	}
	CNetDesignerDoc* pDoc = GetDocument();
	if (index >= 0) {
		if (pDoc->netData.getGroupIndex(index) == pDoc->assembly_edit_index) {
			if (pDoc->netData.isNode(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
				pDoc->selectedComponents.push_back(index);
				selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(index), pDoc->netData.getYDisp(index)));
				selectedComponentsPoints2.push_back(CPoint(0, 0));
				DrawNode(index, zoomScale, DRAW_HIGHLIGHT, false);
				checkAssemblyEditable();
				updateDataToEquation();
				return(true);
			}
			else if (pDoc->netData.isConnect(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_CONNECTIONS))) {
				pDoc->selectedComponents.push_back(index);
				selectedComponentsPoints1.push_back(pDoc->getInputNodePoint(index));
				selectedComponentsPoints2.push_back(pDoc->getOutputNodePoint(index));
				DrawConnect(index, zoomScale, DRAW_ERASE);
				DrawConnect(index, zoomScale, DRAW_HIGHLIGHT);
				checkAssemblyEditable();
				updateDataToEquation();
				return(true);
			}
			else if (pDoc->netData.isConvLayer(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_OTHER))) {
				pDoc->selectedComponents.push_back(index);
				selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(index), pDoc->netData.getYDisp(index)));
				selectedComponentsPoints2.push_back(CPoint(0, 0));
				DrawConvolution(index, zoomScale, DRAW_HIGHLIGHT);
				checkAssemblyEditable();
				updateDataToEquation();
				return(true);
			}
			else if (pDoc->netData.isAttentionLayer(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_OTHER))) {
				pDoc->selectedComponents.push_back(index);
				selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(index), pDoc->netData.getYDisp(index)));
				selectedComponentsPoints2.push_back(CPoint(0, 0));
				DrawAttention(index, zoomScale, DRAW_HIGHLIGHT);
				checkAssemblyEditable();
				updateDataToEquation();
				return(true);
			}
			else if (pDoc->netData.isAssemblyLayer(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_OTHER))) {
				pDoc->selectedComponents.push_back(index);
				selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(index), pDoc->netData.getYDisp(index)));
				selectedComponentsPoints2.push_back(CPoint(0, 0));
				assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
				DrawAssembly(index, zoomScale, DRAW_HIGHLIGHT, asub);
				checkAssemblyEditable();
				updateDataToEquation();
				return(true);
			}
			else if (pDoc->netData.isNetCluster(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_OTHER))) {
				pDoc->selectedComponents.push_back(index);
				selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(index), pDoc->netData.getYDisp(index)));
				selectedComponentsPoints2.push_back(CPoint(0, 0));
				DrawNetCluster(index, zoomScale, DRAW_HIGHLIGHT);
				checkAssemblyEditable();
				updateDataToEquation();
				return(true);
			}
			else if (pDoc->netData.isDropout(index) && ((selection_type == SELECT_ALL) || (selection_type == SELECT_OTHER))) {
				pDoc->selectedComponents.push_back(index);
				selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(index), pDoc->netData.getYDisp(index)));
				selectedComponentsPoints2.push_back(CPoint(0, 0));
				DrawDropout(index, zoomScale, DRAW_HIGHLIGHT);
				checkAssemblyEditable();
				updateDataToEquation();
				return(true);
			}
		}
	}
	checkAssemblyEditable();
	return(false);
}

void CNetDesignerView::selectComponent(CString label)
{
	endSelect();
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.getGroupIndex(i) == pDoc->assembly_edit_index) {
			if (pDoc->netData.getID(i) == string(CT2CA(label))) {
				pDoc->selectedComponents.push_back(i);
				if (pDoc->netData.isNode(i)) {
					selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(i), pDoc->netData.getYDisp(i)));
					selectedComponentsPoints2.push_back(CPoint(0, 0));
					DrawNode(i, zoomScale, DRAW_HIGHLIGHT, false);
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				}
				else if (pDoc->netData.isConnect(i)) {
					selectedComponentsPoints1.push_back(pDoc->getInputNodePoint(i));
					selectedComponentsPoints2.push_back(pDoc->getOutputNodePoint(i));
					DrawConnect(i, zoomScale, DRAW_ERASE);
					DrawConnect(i, zoomScale, DRAW_HIGHLIGHT);
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				}
				else if (pDoc->netData.isConvLayer(i)) {
					selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(i), pDoc->netData.getYDisp(i)));
					selectedComponentsPoints2.push_back(CPoint(0, 0));
					DrawConvolution(i, zoomScale, DRAW_HIGHLIGHT);
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				}
				else if (pDoc->netData.isAttentionLayer(i)) {
					selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(i), pDoc->netData.getYDisp(i)));
					selectedComponentsPoints2.push_back(CPoint(0, 0));
					DrawAttention(i, zoomScale, DRAW_HIGHLIGHT);
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				}
				else if (pDoc->netData.isAssemblyLayer(i)) {
					selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(i), pDoc->netData.getYDisp(i)));
					selectedComponentsPoints2.push_back(CPoint(0, 0));
					assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
					DrawAssembly(i, zoomScale, DRAW_HIGHLIGHT, asub);
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				}
				else if (pDoc->netData.isNetCluster(i)) {
					selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(i), pDoc->netData.getYDisp(i)));
					selectedComponentsPoints2.push_back(CPoint(0, 0));
					DrawNetCluster(i, zoomScale, DRAW_HIGHLIGHT);
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				}
				else if (pDoc->netData.isDropout(i)) {
					selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(i), pDoc->netData.getYDisp(i)));
					selectedComponentsPoints2.push_back(CPoint(0, 0));
					DrawDropout(i, zoomScale, DRAW_HIGHLIGHT);
					CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
					pMainWnd->m_wndProperties.InitPropList(pDoc->selectedComponents);
				}
				checkAssemblyEditable();
				updateDataToEquation();
				searchItemActive = true;
				return;
			}
		}
	}
	checkAssemblyEditable();
}

void CNetDesignerView::endSelect()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		updateProperties();
		updateDataFromEquation();
		clearEquation();
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
				DrawNode(pDoc->selectedComponents[i], zoomScale, DRAW_NORMAL, false);
			}
			else if (pDoc->netData.isConnect(pDoc->selectedComponents[i])) {
				DrawConnect(pDoc->selectedComponents[i], zoomScale, DRAW_NORMAL);
			}
			else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
				DrawConvolution(pDoc->selectedComponents[i], zoomScale, DRAW_NORMAL);
			}
			else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[i])) {
				DrawAttention(pDoc->selectedComponents[i], zoomScale, DRAW_NORMAL);
			}
			else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[i])) {
				assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
				DrawAssembly(pDoc->selectedComponents[i], zoomScale, DRAW_NORMAL, asub);
			}
			else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[i])) {
				DrawNetCluster(pDoc->selectedComponents[i], zoomScale, DRAW_NORMAL);
			}
			else if (pDoc->netData.isDropout(pDoc->selectedComponents[i])) {
				DrawDropout(pDoc->selectedComponents[i], zoomScale, DRAW_NORMAL);
			}
		}
		pDoc->selectedComponents.clear();
		selectedComponentsPoints1.clear();
		selectedComponentsPoints2.clear();
		checkAssemblyEditable();
	}
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndProperties.InitPropListDefault();
	searchItemActive = false;
}

void CNetDesignerView::updateProperties()
{	
	//AfxMessageBox(_T("updateProperties start"));
	CNetDesignerDoc* pDoc = GetDocument();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();

	if (pMainWnd->m_wndProperties.displayType == NODE) {

		CString multiple = pMainWnd->m_wndProperties.multiple;

		CString plbl = pMainWnd->m_wndProperties.getLabel();
		CString ptype = pMainWnd->m_wndProperties.getLayerType();
		CString pfunc = pMainWnd->m_wndProperties.getFunction();
		CString peta = pMainWnd->m_wndProperties.getEta();
		CString palpha = pMainWnd->m_wndProperties.getAlpha();
		CString prand = pMainWnd->m_wndProperties.getRand();
		CString pdata = pMainWnd->m_wndProperties.getDataField();
		CString pindexmode = pMainWnd->m_wndProperties.getDataIndexType();
		int pindex1 = pMainWnd->m_wndProperties.getIndex1();
		int pindex2 = pMainWnd->m_wndProperties.getIndex2();

		//CString s = _T(""); s.Format(_T(" : pindex1 = %i : pindex2 = %i"), pindex1, pindex2); addErrorString(_T("updateProperties : ") + plbl + s);

		pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);

		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			int index = pDoc->selectedComponents[i];
			addPanel1String("updateProperties : " + pDoc->netData.getID(index));
			netDataStruct tdata = pDoc->netData.getNode(index);
			if (pDoc->netData.isNode(index)) {
				if (ptype == multiple) {
					pindex1 = pDoc->netData.getNodeXIndex(index);
					pindex2 = pDoc->netData.getNodeYIndex(index);
				}
				if (pMainWnd->m_wndProperties.edit_names) {
					if (plbl != multiple) {
						if (tdata.getNodeFunction() == "Connector") {
							tdata.setLabel(string(CT2CA(plbl)));
						}
					}
				}
				if (pMainWnd->m_wndProperties.display_functions) {
					if (pfunc != multiple) {
						tdata.setNodeFunction(string(CT2CA(pfunc)));
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
						tdata.setDataField(string(CT2CA(pdata)));
					}
				}
				if (pMainWnd->m_wndProperties.display_input_index) {
					if (pindexmode != multiple) {
						if (pindexmode == _T("Index")) {
							if (pDoc->netData.getNodeMode(index) == "Input") {
								if (pindex1 > 0) {
									AfxMessageBox(_T("Time series data indices for input nodes must be less than or equal to zero"));
									pindex1 = 0;
								}
							}
						}
						tdata.setNodeIndexMode(string(CT2CA(pindexmode)));
						tdata.setNodeXIndex(pindex1);
						tdata.setNodeYIndex(pindex2);
						//addErrorString(_T("updateProperties : updating indices"));
					}
				}
				pDoc->pushEditNodeData(tdata);
			}
		}
		if (pDoc->processEditData()) {
			pMainWnd->m_wndNodeView.FillFileView();
		}

	}
	else if (pMainWnd->m_wndProperties.displayType == CONVLAYER) {

		CString multiple = pMainWnd->m_wndProperties.multiple;

		if (pDoc->selectedComponents.size() == 1) {

			CAuxFunctions func;

			//AfxMessageBox(_T("getLayerType = ") + pMainWnd->m_wndProperties.getLayerType());
			CString ptype = pMainWnd->m_wndProperties.getLayerType();
			//AfxMessageBox(_T("getFunction"));
			CString pfunc = pMainWnd->m_wndProperties.getFunction();
			//AfxMessageBox(_T("getEta"));
			CString peta = pMainWnd->m_wndProperties.getEta();
			//AfxMessageBox(_T("getAlpha"));
			CString palpha = pMainWnd->m_wndProperties.getAlpha();
			//AfxMessageBox(_T("getRand"));
			CString prand = pMainWnd->m_wndProperties.getRand();
			//AfxMessageBox(_T("getDimXIn"));
			int dim1 = pMainWnd->m_wndProperties.getDimXIn();
			//AfxMessageBox(_T("getDimYIn"));
			int dim2 = pMainWnd->m_wndProperties.getDimYIn();
			//AfxMessageBox(_T("getDimXOut"));
			int dim3 = pMainWnd->m_wndProperties.getDimXOut();
			//AfxMessageBox(_T("getDimYOut"));
			int dim4 = pMainWnd->m_wndProperties.getDimYOut();
			//AfxMessageBox(_T("getStride1"));
			int stride1 = pMainWnd->m_wndProperties.getStride1();
			//AfxMessageBox(_T("getStride2"));
			int stride2 = pMainWnd->m_wndProperties.getStride2();
			//AfxMessageBox(_T("getConvType"));
			int nconvtype = pMainWnd->m_wndProperties.getConvType();
			//AfxMessageBox(_T("getConvLayers"));
			int nlayers = pMainWnd->m_wndProperties.getConvLayers();
			//AfxMessageBox(_T("getConvNodesPerLayer"));
			int nnodesperlayer = pMainWnd->m_wndProperties.getConvNodesPerLayer();
			CString input_image_width = pMainWnd->m_wndProperties.getInputImageWidth();
			CString input_image_height = pMainWnd->m_wndProperties.getInputImageHeight();
			int model_dim = pMainWnd->m_wndProperties.getModelDimension();

			if (ptype == _T("Input")) {
				pfunc = pMainWnd->m_wndProperties.getFunction();
				peta = pMainWnd->m_wndProperties.getEta();
				palpha = pMainWnd->m_wndProperties.getAlpha();
				prand = pMainWnd->m_wndProperties.getRand();
			}

			if (dim1 < 1) {
				dim1 = 1;
			}
			if (nconvtype == DATA_CONV) {
				if (dim1 > 256) {
					dim1 = 256;
				}
			}
			else {
				if (dim1 > 12) {
					dim1 = 12;
				}
			}

			if (dim2 < 1) {
				dim2 = 1;
			}
			if (nconvtype == DATA_CONV) {
				if (dim2 > 64) {
					dim2 = 64;
				}
			}
			else {
				if (dim2 > 12) {
					dim2 = 12;
				}
			}

			if (dim3 < 1) {
				dim3 = 1;
			}
			if (nconvtype == DATA_CONV) {
				if (dim3 > 512) {
					dim3 = 512;
				}
			}
			else {
				if (dim3 > 12) {
					dim3 = 12;
				}
			}

			if (dim4 < 1) {
				dim4 = 1;
			}
			if (nconvtype == DATA_CONV) {
				if (dim4 > 512) {
					dim4 = 512;
				}
			}
			else {
				if (dim4 > 12) {
					dim4 = 12;
				}
			}

			if (stride1 < 1) {
				stride1 = 1;
			}
			if (nconvtype == DATA_CONV) {
				if (stride1 > 64) {
					stride1 = 64;
				}
			}
			else {
				if (stride1 > 12) {
					stride1 = 12;
				}
			}

			if (stride2 < 1) {
				stride2 = 1;
			}
			if (nconvtype == DATA_CONV) {
				if (stride1 > 512) {
					stride1 = 512;
				}
			}
			else {
				if (stride2 > 12) {
					stride2 = 12;
				}
			}

			if (nlayers < 1) {
				nlayers = 1;
			}
			if (nlayers > 6) {
				nlayers = 6;
			}
			if (nnodesperlayer < 1) {
				nnodesperlayer = 1;
			}
			if (nnodesperlayer > 102) {
				if (nconvtype == DATA_CONV) {
					nnodesperlayer = 102;
				}
			}

			if (model_dim< 0) {
				model_dim = 0;
			}

			pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);

			int index = pDoc->selectedComponents[0];
			addPanel1String("updateProperties : " + pDoc->netData.getID(index));
			netDataStruct tdata = pDoc->netData.getNode(index);
			if (pDoc->netData.isConvLayer(index)) {
				if (pMainWnd->m_wndProperties.edit_names) {
					tdata.setNodeMode(string(CT2CA(ptype)));
					tdata.setNodeFunction(string(CT2CA(pfunc)));
					tdata.setNodeEta(_tstof((LPCTSTR)peta));
					tdata.setNodeAlpha(_tstof((LPCTSTR)palpha));
					tdata.setNodeRand(_tstof((LPCTSTR)prand));
					tdata.setConvNumLayers(nlayers);
					tdata.setConvNodesPerLayer(nnodesperlayer);
					tdata.setConvType(nconvtype);
					vector<int> mdimensions;
					mdimensions.push_back(dim1);
					mdimensions.push_back(dim2);
					mdimensions.push_back(dim3);
					mdimensions.push_back(dim4);
					tdata.setConvDimensions(mdimensions);
					vector<int> mstrides;
					mstrides.push_back(stride1);
					mstrides.push_back(stride2);
					tdata.setConvStrides(mstrides);
					tdata.setConvLocations(func.getLayerPoints4((int)pDoc->nradius2, nconvtype));
					tdata.setDataFields(pDoc->netData.getDataFields(index));
					tdata.setD(model_dim);
					//CString s = _T(""); s.Format(_T("updateProperties : d1 = %i : d2 = %i : "), pMainWnd->m_wndProperties.getModelDimension(), tdata.getD()); addErrorString(s + (CString)tdata.id.c_str());
					addPanel1String("updateProperties : pushEditConvLayer : " + pDoc->netData.getID(index));
					pDoc->pushEditConvLayer(tdata);

					CString ext = pDoc->getImageFileExtension();
					if ((nconvtype == IMAGE_CONV) && (tdata.getNodeMode() == "Input") && ext == _T("raw")) {
						//addErrorString(_T("updateProperties : updating image width and height"));
						for (int j = 0; j < pDoc->netData.size(); j++) {
							if (pDoc->netData.isDataSource(j)) {
								pDoc->netData.setRawImageWidth(j, (int)_tstof((LPCTSTR)input_image_width));
								pDoc->netData.setRawImageHeight(j, (int)_tstof((LPCTSTR)input_image_height));
								j = pDoc->netData.size();
							}
						}
					}

				}
			}

			if (pDoc->processEditData()) {
				addPanel1String("updateProperties : FillFileView : " + pDoc->netData.getID(index));
				pMainWnd->m_wndNodeView.FillFileView();
			}

		}

	}
	else if (pMainWnd->m_wndProperties.displayType == ATTENTION) {

		CString multiple = pMainWnd->m_wndProperties.multiple;

		if (pDoc->selectedComponents.size() == 1) {

			pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);

			int index = pDoc->selectedComponents[0];
			addPanel1String("updateProperties : " + pDoc->netData.getID(index));
			netDataStruct tdata = pDoc->netData.getNode(index);
			if (pDoc->netData.isAttentionLayer(index)) {
				if (pMainWnd->m_wndProperties.edit_names) {
					tdata.setDk(pMainWnd->m_wndProperties.getKDimension());
					tdata.setDv(pMainWnd->m_wndProperties.getVDimension());
					tdata.setD(pMainWnd->m_wndProperties.getModelDimension());
					tdata.setSumAddLimit(pMainWnd->m_wndProperties.getSumAddLimit());
					tdata.setSoftmaxProcessing(pMainWnd->m_wndProperties.getHasSoftmax());
					tdata.setTransformerModeQuadratic(true);
					CString tmode = pMainWnd->m_wndProperties.getTransformerFunction();
					if (tmode == _T("Linear")) {
						tdata.setTransformerModeQuadratic(false);
					}
					tdata.setConvType(ATTENTION);
					addPanel1String("updateProperties : pushEditAttentionLayer : " + pDoc->netData.getID(index));
					pDoc->pushEditAttentionLayer(tdata);
				}
			}

			if (pDoc->processEditData()) {
				addPanel1String("updateProperties : FillFileView : " + pDoc->netData.getID(index));
				pMainWnd->m_wndNodeView.FillFileView();
			}

		}

	}
	else if (pMainWnd->m_wndProperties.displayType == ASSEMBLY) {

		CString multiple = pMainWnd->m_wndProperties.multiple;

		CString plbl = pMainWnd->m_wndProperties.getLabel();

		if (pDoc->selectedComponents.size() == 1) {

			pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);

			int index = pDoc->selectedComponents[0];
			addPanel1String("updateProperties : " + pDoc->netData.getID(index));
			netDataStruct tdata = pDoc->netData.getNode(index);
			if (pDoc->netData.isAssemblyLayer(index)) {
				if (pMainWnd->m_wndProperties.edit_names) {
					tdata.setLabel(string(CT2CA(plbl)));
					tdata.setConvType(ASSEMBLY);
					addPanel1String("updateProperties : pushEditAssemblyLayer : " + pDoc->netData.getID(index));
					pDoc->pushEditAssembly(tdata);
				}
			}

			if (pDoc->processEditData()) {
				addPanel1String("updateProperties : FillFileView : " + pDoc->netData.getID(index));
				pMainWnd->m_wndNodeView.FillFileView();
			}

		}

	}
	else if (pMainWnd->m_wndProperties.displayType == NETCLUSTER) {

		CString multiple = pMainWnd->m_wndProperties.multiple;


		if (pDoc->selectedComponents.size() == 1) {

			pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);

			int index = pDoc->selectedComponents[0];
			addPanel1String("updateProperties : " + pDoc->netData.getID(index));
			netDataStruct tdata = pDoc->netData.getNode(index);

			CString ptype = pMainWnd->m_wndProperties.getLayerType();
			CString pfunc = pMainWnd->m_wndProperties.getFunction();
			CString peta = pMainWnd->m_wndProperties.getEta();
			CString palpha = pMainWnd->m_wndProperties.getAlpha();
			CString prand = pMainWnd->m_wndProperties.getRand();
			int nnodesperlayer = pMainWnd->m_wndProperties.getConvNodesPerLayer();
			double nupdateepochs = pMainWnd->m_wndProperties.getUpdateEpochs();
			double ntesterror = pMainWnd->m_wndProperties.getTestError();
			double nsplitvariable = pMainWnd->m_wndProperties.getSplitVariable();
			if (nsplitvariable < 0.0) {
				nsplitvariable = 0.0;
			}
			else if (nsplitvariable > 1.0) {
				nsplitvariable = 1.0;
			}
			CString npatternsplit = pMainWnd->m_wndProperties.getPatternSplitMethod();

			if (pDoc->netData.isNetCluster(index)) {
				if (pMainWnd->m_wndProperties.edit_names) {
					tdata.setNodeMode(string(CT2CA(ptype)));
					tdata.setNodeFunction(string(CT2CA(pfunc)));
					tdata.setNodeEta(_tstof((LPCTSTR)peta));
					tdata.setNodeAlpha(_tstof((LPCTSTR)palpha));
					tdata.setNodeRand(_tstof((LPCTSTR)prand));
					tdata.setConvNodesPerLayer(nnodesperlayer);
					tdata.setConvType(NETCLUSTER);
					tdata.setClusterUpdateEpochs(nupdateepochs);
					tdata.setClusterTestError(ntesterror);
					tdata.setClusterSplitVariable(nsplitvariable);
					tdata.setPatternSlitMethod(string(CT2CA(npatternsplit)));
					addPanel1String("updateProperties : pushEditNetCluster : " + pDoc->netData.getID(index));
					pDoc->pushEditNetCluster(tdata);
				}
			}

			if (pDoc->processEditData()) {
				addPanel1String("updateProperties : FillFileView : " + pDoc->netData.getID(index));
				pMainWnd->m_wndNodeView.FillFileView();
			}

		}

	}
	else if (pMainWnd->m_wndProperties.displayType == DROPOUT) {

		CString multiple = pMainWnd->m_wndProperties.multiple;

		if (pDoc->selectedComponents.size() == 1) {

			pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);

			int index = pDoc->selectedComponents[0];
			addPanel1String("updateProperties : " + pDoc->netData.getID(index));
			netDataStruct tdata = pDoc->netData.getNode(index);

			double npercent = pMainWnd->m_wndProperties.getDropoutPercent();
			if (npercent < 0.0) {
				addErrorString("Dropout min percent = 0.0");
				npercent = 0.0;
			}
			else if (npercent > 1.0) {
				addErrorString("Dropout max percent = 1.0");
				npercent = 1.0;
			}

			if (pDoc->netData.isDropout(index)) {
				if (pMainWnd->m_wndProperties.edit_names) {
					tdata.setConvType(DROPOUT);
					tdata.setDropoutPercent(npercent);
					addPanel1String("updateProperties : pushEditDropout : " + pDoc->netData.getID(index));
					pDoc->pushEditDropout(tdata);
				}
			}

			if (pDoc->processEditData()) {
				addPanel1String("updateProperties : FillFileView : " + pDoc->netData.getID(index));
				pMainWnd->m_wndNodeView.FillFileView();
			}

		}

	}
	//AfxMessageBox(_T("updateProperties end"));
	addPanel1String(_T(""));
}

void CNetDesignerView::updateMovedProperties(CPoint frompt, CPoint topt)
{
	int dx = (int)((float)(topt.x - frompt.x) / zoomScale);
	int dy = (int)((float)(topt.y - frompt.y) / zoomScale);
	CNetDesignerDoc* pDoc = GetDocument();
	pDoc->clearAndSetEditParams(UNDO_EDIT, zoomScale, lHorzScollPos, lVertScollPos);
	for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
		int index = pDoc->selectedComponents[i];
		if (pDoc->netData.isNode(index)) {
			netDataStruct tdata = pDoc->netData.getNode(index);
			tdata.setXDisp(applyGridX(pDoc->netData.getXDisp(index) + dx));
			tdata.setYDisp(applyGridY(pDoc->netData.getYDisp(index) + dy));
			pDoc->pushEditNodeData(tdata);
		}
		else if (pDoc->netData.isConnect(index)) {
			CString sid = (CString)pDoc->netData.getID(index).c_str();
			CString slbl = (CString)pDoc->netData.getLabel(index).c_str();
			CString node_in = (CString)pDoc->netData.getInputNode(index).c_str();
			CString node_out = (CString)pDoc->netData.getOutputNode(index).c_str();
			vector<CPoint> points;
			double sweight = pDoc->netData.getWeight(index);
			vector<double> sweights = pDoc->netData.getConvWeights(index);
			for (int j = 0; j < points.size(); j++) {
				points[j] = LPtoDP(applyGrid(DPtoLP(CPoint(points[j].x + dx, points[j].y + dy))));
			}
			pDoc->pushEditConnectData(sid, slbl, node_in, node_out, pDoc->netData.getGroupIndex(index), sweight, sweights, points);
		}
		else if (pDoc->netData.isConvLayer(index)) {
			netDataStruct tdata = pDoc->netData.getNode(index);
			tdata.setXDisp(applyGridX(pDoc->netData.getXDisp(index) + dx));
			tdata.setYDisp(applyGridY(pDoc->netData.getYDisp(index) + dy));
			pDoc->pushEditConvLayer(tdata);
		}
		else if (pDoc->netData.isAttentionLayer(index)) {
			netDataStruct tdata = pDoc->netData.getNode(index);
			tdata.setXDisp(applyGridX(pDoc->netData.getXDisp(index) + dx));
			tdata.setYDisp(applyGridY(pDoc->netData.getYDisp(index) + dy));
			pDoc->pushEditAttentionLayer(tdata);
		}
		else if (pDoc->netData.isAssemblyLayer(index)) {
			netDataStruct tdata = pDoc->netData.getNode(index);
			tdata.setXDisp(applyGridX(pDoc->netData.getXDisp(index) + dx));
			tdata.setYDisp(applyGridY(pDoc->netData.getYDisp(index) + dy));
			pDoc->pushEditAssembly(tdata);
		}
		else if (pDoc->netData.isNetCluster(index)) {
			netDataStruct tdata = pDoc->netData.getNode(index);
			tdata.setXDisp(applyGridX(pDoc->netData.getXDisp(index) + dx));
			tdata.setYDisp(applyGridY(pDoc->netData.getYDisp(index) + dy));
			pDoc->pushEditNetCluster(tdata);
		}
		else if (pDoc->netData.isDropout(index)) {
			netDataStruct tdata = pDoc->netData.getNode(index);
			tdata.setXDisp(applyGridX(pDoc->netData.getXDisp(index) + dx));
			tdata.setYDisp(applyGridY(pDoc->netData.getYDisp(index) + dy));
			pDoc->pushEditDropout(tdata);
		}
	}
	if (pDoc->processEditData()) {
		//CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		//pMainWnd->m_wndNodeView.FillFileView();
	}
}

void CNetDesignerView::updatePastedProperties(CPoint spt, int snode)
{
	CNetDesignerDoc* pDoc = GetDocument();
	pDoc->clearAndSetEditParams(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos);
	bool redraw = false;
	int nindex_offset = 0;
	int cindex_offset = 0;
	int vindex_offset = 0;
	int aindex_offset = 0;
	int ncindex_offset = 0;
	int dindex_offset = 0;
	vector<CString> fromLabels;
	vector<CString> toLabels;
	CPoint mpt = DPtoLP(spt);
	int x = mpt.x - copyData.getXDisp(snode);
	int y = mpt.y - copyData.getYDisp(snode);
	for (int i = 0; i < copyData.size(); i++) {
		if (copyData.isNode(i)) {
			bool place = true;
			if (place) {
				netDataStruct tdata = copyData.getNode(i);
				fromLabels.push_back((CString)copyData.getID(i).c_str());
				CString sid = _T("");
				sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
				toLabels.push_back(sid);
				tdata.setID(string(CT2CA(sid)));
				tdata.setLabel(string(CT2CA(sid)));
				tdata.setXDisp(applyGridX(copyData.getXDisp(i) + x));
				tdata.setYDisp(applyGridY(copyData.getYDisp(i) + y));
				tdata.setGroupIndex(pDoc->assembly_edit_index);
				pDoc->pushEditNodeData(tdata);
				nindex_offset++;
			}
		}
		else if (copyData.isConvLayer(i)) {
			bool place = true;
			if (place) {
				fromLabels.push_back((CString)copyData.getID(i).c_str());
				CString sid = _T("");
				sid.Format(_T("Layer%i"), pDoc->getLastConvIndex() + 1 + cindex_offset);
				toLabels.push_back(sid);
				netDataStruct tdata = copyData.getNode(i);
				tdata.setID(string(CT2CA(sid)));
				tdata.setLabel(string(CT2CA(sid)));
				tdata.setXDisp(applyGridX(copyData.getXDisp(i) + x));
				tdata.setYDisp(applyGridY(copyData.getYDisp(i) + y));
				tdata.setGroupIndex(pDoc->assembly_edit_index);
				pDoc->pushEditConvLayer(tdata);
				cindex_offset++;
			}
		}
		else if (copyData.isAttentionLayer(i)) {
			bool place = true;
			if (place) {
				fromLabels.push_back((CString)copyData.getID(i).c_str());
				CString sid = _T("");
				sid.Format(_T("Attention%i"), pDoc->getLastAttentionIndex() + 1 + vindex_offset);
				toLabels.push_back(sid);
				netDataStruct tdata = copyData.getNode(i);
				tdata.setID(string(CT2CA(sid)));
				tdata.setLabel(string(CT2CA(sid)));
				tdata.setXDisp(applyGridX(copyData.getXDisp(i) + x));
				tdata.setYDisp(applyGridY(copyData.getYDisp(i) + y));
				tdata.setConvType(ATTENTION);
				tdata.setGroupIndex(pDoc->assembly_edit_index);
				pDoc->pushEditAttentionLayer(tdata);
				vindex_offset++;
			}
		}
		else if (copyData.isAssemblyLayer(i)) {
			bool place = true;
			if (place) {
				fromLabels.push_back((CString)copyData.getID(i).c_str());
				CString sid = _T("");
				sid.Format(_T("Assembly%i"), pDoc->getLastAssemblyIndex() + 1 + aindex_offset); 
				toLabels.push_back(sid);
				netDataStruct tdata = copyData.getNode(i);
				tdata.setID(string(CT2CA(sid)));
				tdata.setLabel(string(CT2CA(sid)));
				tdata.setXDisp(applyGridX(copyData.getXDisp(i) + x));
				tdata.setYDisp(applyGridY(copyData.getYDisp(i) + y));
				tdata.setConvType(ASSEMBLY);
				tdata.setGroupIndex(pDoc->assembly_edit_index);
				pDoc->pushEditAssembly(tdata);
				aindex_offset++;
			}
		}
		else if (copyData.isNetCluster(i)) {
			bool place = true;
			if (place) {
				fromLabels.push_back((CString)copyData.getID(i).c_str());
				CString sid = _T("");
				sid.Format(_T("NetCluster%i"), pDoc->getLastNetClusterIndex() + 1 + ncindex_offset);
				toLabels.push_back(sid);
				netDataStruct tdata = copyData.getNode(i);
				tdata.setID(string(CT2CA(sid)));
				tdata.setLabel(string(CT2CA(sid)));
				tdata.setXDisp(applyGridX(copyData.getXDisp(i) + x));
				tdata.setYDisp(applyGridY(copyData.getYDisp(i) + y));
				tdata.setConvType(NETCLUSTER);
				tdata.setGroupIndex(pDoc->assembly_edit_index);
				pDoc->pushEditNetCluster(tdata);
				ncindex_offset++;
			}
		}
		else if (copyData.isDropout(i)) {
			bool place = true;
			if (place) {
				fromLabels.push_back((CString)copyData.getID(i).c_str());
				CString sid = _T("");
				sid.Format(_T("Dropout%i"), pDoc->getLastDropoutIndex() + 1 + dindex_offset);
				toLabels.push_back(sid);
				netDataStruct tdata = copyData.getNode(i);
				tdata.setID(string(CT2CA(sid)));
				tdata.setLabel(string(CT2CA(sid)));
				tdata.setXDisp(applyGridX(copyData.getXDisp(i) + x));
				tdata.setYDisp(applyGridY(copyData.getYDisp(i) + y));
				tdata.setConvType(DROPOUT);
				tdata.setGroupIndex(pDoc->assembly_edit_index);
				pDoc->pushEditDropout(tdata);
				dindex_offset++;
			}
		}
	}
	if (fromLabels.size() != toLabels.size()) {
		AfxMessageBox(_T("updatePastedProperties : fromLabels.size() != toLabels.size()"));
		return;
	}
	nindex_offset = 0;
	for (int i = 0; i < copyData.size(); i++) {
		if (copyData.isConnect(i)) {
			CString add1 = _T("");
			CString mlabel1 = (CString)copyData.getInputNode(i).c_str();
			if ((mlabel1.Left(9) == _T("ConvLayer")) || (mlabel1.Left(5) == _T("Layer"))) {
				CString tlabel1 = mlabel1.Left(5);
				int j = tlabel1.GetLength();
				while ((j < mlabel1.GetLength()) && (mlabel1[j] != ':')) {
					tlabel1 += mlabel1[j];
					j++;
				}
				add1 = mlabel1.Right(mlabel1.GetLength() - j);
				mlabel1 = tlabel1;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel1 == fromLabels[j]) {
						mlabel1 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel1.Left(9) == _T("Attention")) {
				CString tlabel1 = mlabel1.Left(9);
				int j = tlabel1.GetLength();
				while ((j < mlabel1.GetLength()) && (mlabel1[j] != ':')) {
					tlabel1 += mlabel1[j];
					j++;
				}
				add1 = mlabel1.Right(mlabel1.GetLength() - j);
				mlabel1 = tlabel1;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel1 == fromLabels[j]) {
						mlabel1 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel1.Left(10) == _T("NetCluster")) {
				CString tlabel1 = mlabel1.Left(10);
				int j = tlabel1.GetLength();
				while ((j < mlabel1.GetLength()) && (mlabel1[j] != ':')) {
					tlabel1 += mlabel1[j];
					j++;
				}
				add1 = mlabel1.Right(mlabel1.GetLength() - j);
				mlabel1 = tlabel1;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel1 == fromLabels[j]) {
						mlabel1 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel1.Left(7) == _T("Dropout")) {
				CString tlabel1 = mlabel1.Left(7);
				int j = tlabel1.GetLength();
				while ((j < mlabel1.GetLength()) && (mlabel1[j] != ':')) {
					tlabel1 += mlabel1[j];
					j++;
				}
				add1 = mlabel1.Right(mlabel1.GetLength() - j);
				mlabel1 = tlabel1;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel1 == fromLabels[j]) {
						mlabel1 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel1.Left(8) == _T("Assembly")) {
				CString tlabel1 = mlabel1.Left(8);
				int j = tlabel1.GetLength();
				while ((j < mlabel1.GetLength()) && (mlabel1[j] != ':')) {
					tlabel1 += mlabel1[j];
					j++;
				}
				add1 = mlabel1.Right(mlabel1.GetLength() - j);
				mlabel1 = tlabel1;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel1 == fromLabels[j]) {
						mlabel1 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else {
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel1 == fromLabels[j]) {
						mlabel1 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			CString add2 = _T("");
			CString mlabel2 = (CString)copyData.getOutputNode(i).c_str();
			if ((mlabel2.Left(9) == _T("ConvLayer")) || (mlabel2.Left(5) == _T("Layer"))) {
				CString tlabel2 = mlabel2.Left(5);
				int j = tlabel2.GetLength();
				while ((j < mlabel2.GetLength()) && (mlabel2[j] != ':')) {
					tlabel2 += mlabel2[j];
					j++;
				}
				add2 = mlabel2.Right(mlabel2.GetLength() - j);
				mlabel2 = tlabel2;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel2 == fromLabels[j]) {
						mlabel2 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel2.Left(9) == _T("Attention")) {
				CString tlabel2 = mlabel2.Left(9);
				int j = tlabel2.GetLength();
				while ((j < mlabel2.GetLength()) && (mlabel2[j] != ':')) {
					tlabel2 += mlabel2[j];
					j++;
				}
				add2 = mlabel2.Right(mlabel2.GetLength() - j);
				mlabel2 = tlabel2;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel2 == fromLabels[j]) {
						mlabel2 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel2.Left(10) == _T("NetCluster")) {
				CString tlabel2 = mlabel2.Left(10);
				int j = tlabel2.GetLength();
				while ((j < mlabel2.GetLength()) && (mlabel2[j] != ':')) {
					tlabel2 += mlabel2[j];
					j++;
				}
				add2 = mlabel2.Right(mlabel2.GetLength() - j);
				mlabel2 = tlabel2;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel2 == fromLabels[j]) {
						mlabel2 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel2.Left(7) == _T("Dropout")) {
				CString tlabel2 = mlabel2.Left(7);
				int j = tlabel2.GetLength();
				while ((j < mlabel2.GetLength()) && (mlabel2[j] != ':')) {
					tlabel2 += mlabel2[j];
					j++;
				}
				add2 = mlabel2.Right(mlabel2.GetLength() - j);
				mlabel2 = tlabel2;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel2 == fromLabels[j]) {
						mlabel2 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else if (mlabel2.Left(8) == _T("Assembly")) {
				CString tlabel2 = mlabel2.Left(8);
				int j = tlabel2.GetLength();
				while ((j < mlabel2.GetLength()) && (mlabel2[j] != ':')) {
					tlabel2 += mlabel2[j];
					j++;
				}
				add2 = mlabel2.Right(mlabel2.GetLength() - j);
				mlabel2 = tlabel2;
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel2 == fromLabels[j]) {
						mlabel2 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			else {
				for (int j = 0; j < fromLabels.size(); j++) {
					if (mlabel2 == fromLabels[j]) {
						mlabel2 = toLabels[j];
						j = (int)fromLabels.size();
					}
				}
			}
			CString sid = _T("");
			sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + cindex_offset);
			CString node_in = mlabel1 + add1;
			CString node_out = mlabel2 + add2;
			vector<CPoint> points;
			for (int j = 0; j < points.size(); j++) {
				points[j] = LPtoDP(applyGrid(DPtoLP(CPoint(points[j].x + x, points[j].y + y))));
			}
			double sweight = copyData.getWeight(i);
			vector<double> sweights = copyData.getConvWeights(i);
			pDoc->pushEditConnectData(sid, sid, node_in, node_out, pDoc->assembly_edit_index, sweight, sweights, points);
			cindex_offset++;
		}
	}
	if (pDoc->processEditData()) {
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndNodeView.FillFileView();
	}
	if (redraw) {
		OnRedraw();
	}
}

void CNetDesignerView::addStragglingConnectionsToSelectedComponents()
{
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			bool cfound = false;
			bool nfound = false;
			string clabel = pDoc->netData.getID(i);
			for (int j = 0; j < pDoc->selectedComponents.size(); j++) {
				if (pDoc->selectedComponents[j] < (int)pDoc->netData.size()) {
					if (pDoc->netData.isConnect(pDoc->selectedComponents[j])) {
						if (clabel == pDoc->netData.getID(pDoc->selectedComponents[j])) {
							cfound = true;
							j = (int)pDoc->selectedComponents.size();
						}
					}
				}
				else {
					AfxMessageBox(_T("error12345"));
				}
			}
			if (!cfound) {
				string nlabel1 = func.getConvNameFromConnectName(pDoc->netData.getInputNode(i));
				string nlabel2 = func.getConvNameFromConnectName(pDoc->netData.getOutputNode(i));
				for (int j = 0; j < pDoc->selectedComponents.size(); j++) {
					if (pDoc->selectedComponents[j] < (int)pDoc->netData.size()) {
						if (pDoc->netData.isNode(pDoc->selectedComponents[j]) || pDoc->netData.isConvLayer(pDoc->selectedComponents[j])) {
							if (nlabel1 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
							else if (nlabel2 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[j])) {
							if (nlabel1 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
							else if (nlabel2 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[j])) {
							if (nlabel1 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
							else if (nlabel2 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[j])) {
							if (nlabel1 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
							else if (nlabel2 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
						}
						else if (pDoc->netData.isDropout(pDoc->selectedComponents[j])) {
							if (nlabel1 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
							else if (nlabel2 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
								nfound = true;
								j = (int)pDoc->selectedComponents.size();
							}
						}
					}
					else {
						AfxMessageBox(_T("error12346"));
					}
				}
			}
			if (nfound) {
				//AfxMessageBox((CString)clabel.c_str());
				pDoc->selectedComponents.push_back(i);
				selectedComponentsPoints1.push_back(pDoc->getInputNodePoint(i));
				selectedComponentsPoints2.push_back(pDoc->getOutputNodePoint(i));
			}
		}
	}
	checkAssemblyEditable();
	//AfxMessageBox(_T("done"));
}

void CNetDesignerView::deleteStragglingConnectionsFromSelectedComponents()
{
	CAuxFunctions func;
	vector<int> keepers;
	vector<CPoint> keeperPoints1;
	vector<CPoint> keeperPoints2;
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
		if (pDoc->netData.getGroupIndex(pDoc->selectedComponents[i]) == pDoc->assembly_edit_index) {
			if (pDoc->netData.isConnect(pDoc->selectedComponents[i])) {
				bool nfound1 = false;
				bool nfound2 = false;
				string nlabel1 = func.getConvNameFromConnectName(pDoc->netData.getInputNode(pDoc->selectedComponents[i]));
				string nlabel2 = func.getConvNameFromConnectName(pDoc->netData.getOutputNode(pDoc->selectedComponents[i]));
				for (int j = 0; j < pDoc->selectedComponents.size(); j++) {
					if (nlabel1 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
						nfound1 = true;
					}
					if (nlabel2 == pDoc->netData.getID(pDoc->selectedComponents[j])) {
						nfound2 = true;
					}
					if (nfound1 && nfound2) {
						keepers.push_back(pDoc->selectedComponents[i]);
						keeperPoints1.push_back(pDoc->getInputNodePoint(pDoc->selectedComponents[i]));
						keeperPoints2.push_back(pDoc->getOutputNodePoint(pDoc->selectedComponents[i]));
						j = (int)pDoc->selectedComponents.size();
					}
				}
			}
			else {
				keepers.push_back(pDoc->selectedComponents[i]);
				keeperPoints1.push_back(CPoint(applyGridX(pDoc->netData.getXDisp(pDoc->selectedComponents[i])), applyGridY(pDoc->netData.getYDisp(pDoc->selectedComponents[i]))));
				keeperPoints2.push_back(CPoint(0, 0));
			}
		}
	}
	pDoc->selectedComponents.clear();
	selectedComponentsPoints1.clear();
	selectedComponentsPoints2.clear();
	for (int i = 0; i < keepers.size(); i++) {
		pDoc->selectedComponents.push_back(keepers[i]);
		selectedComponentsPoints1.push_back(keeperPoints1[i]);
		selectedComponentsPoints2.push_back(keeperPoints2[i]);
	}
	checkAssemblyEditable();
}

void CNetDesignerView::deleteSelectedComponents()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() > 0) {
		CNetDesignerDoc* pDoc = GetDocument();
		
		// Check for assemblies and add subcomponents if existing
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			int index = pDoc->selectedComponents[i];
			if (pDoc->netData.isAssemblyLayer(index) && (pDoc->netData.getAssemblyReference(index) == "")) {
				vector<int> subcomponents = getAssemblySubComponents(index);
				if (subcomponents.size() > 0) {
					//if (AfxMessageBox(_T("Assembly ") + (CString)pDoc->netData.getID(index).c_str() + _T(" has subcomponents - delete anyway?"), MB_YESNO) == IDYES) {
						for (int j = 0; j < (int)subcomponents.size(); j++) {
							bool found = false;
							for (int k = 0; k < (int)pDoc->selectedComponents.size(); k++) {
								if (subcomponents[j] == pDoc->selectedComponents[k]) {
									k = (int)pDoc->selectedComponents.size();
									found = true;
								}
							}
							if (!found) {
								pDoc->selectedComponents.push_back(subcomponents[j]);
							}
						}
					//}
					/*else {    can'y delete assembly without deleting sub components
						pDoc->selectedComponents.erase(pDoc->selectedComponents.begin() + index);
						i--;
					}*/
				}
			}
		}
		addStragglingConnectionsToSelectedComponents();
		pDoc->clearAndSetEditParams(UNDO_DELETE, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			int index = pDoc->selectedComponents[i];
			if (pDoc->netData.isNode(index)) {
				netDataStruct tdata = pDoc->netData.getNode(index);
				pDoc->pushEditNodeData(tdata);
				DrawNode(index, zoomScale, DRAW_ERASE, false);
			}
			else if (pDoc->netData.isConnect(index)) {
				string sid = pDoc->netData.getID(index);
				string slbl = pDoc->netData.getLabel(index);
				string snode_in = pDoc->netData.getInputNode(index);
				string snode_out = pDoc->netData.getOutputNode(index);
				vector<CPoint> points;
				double sweight = pDoc->netData.getWeight(index);
				vector<double> sweights = pDoc->netData.getConvWeights(index);
				pDoc->pushEditConnectData((CString)sid.c_str(), (CString)slbl.c_str(), (CString)snode_in.c_str(), (CString)snode_out.c_str(), pDoc->netData.getGroupIndex(index), sweight, sweights, points);
				DrawConnect(index, zoomScale, DRAW_ERASE);
			}
			else if (pDoc->netData.isConvLayer(index)) {
				netDataStruct tdata = pDoc->netData.getNode(index);
				pDoc->pushEditConvLayer(tdata);
				DrawConvolution(index, zoomScale, DRAW_ERASE);
			}
			else if (pDoc->netData.isAttentionLayer(index)) {
				netDataStruct tdata = pDoc->netData.getNode(index);
				pDoc->pushEditAttentionLayer(tdata);
				DrawAttention(index, zoomScale, DRAW_ERASE);
			}
			else if (pDoc->netData.isAssemblyLayer(index)) {
				netDataStruct tdata = pDoc->netData.getNode(index);
				pDoc->pushEditAssembly(tdata);
				assembly_sub_component asub; asub.net_index = -1; asub.input_sub_index = -1; asub.output_sub_index = -1;
				DrawAssembly(index, zoomScale, DRAW_ERASE, asub);
			}
			else if (pDoc->netData.isNetCluster(index)) {
				netDataStruct tdata = pDoc->netData.getNode(index);
				pDoc->pushEditNetCluster(tdata);
				DrawNetCluster(index, zoomScale, DRAW_ERASE);
			}
			else if (pDoc->netData.isDropout(index)) {
				netDataStruct tdata = pDoc->netData.getNode(index);
				pDoc->pushEditDropout(tdata);
				DrawDropout(index, zoomScale, DRAW_ERASE);
			}
		}
		pDoc->selectedComponents.clear();
		selectedComponentsPoints1.clear();
		selectedComponentsPoints2.clear();
		clearEquation();
		if (pDoc->processEditData()) {
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndNodeView.FillFileView();
			pMainWnd->m_wndProperties.InitPropListDefault();
		}
		checkAssemblyEditable();
	}
}

void CNetDesignerView::copySelectedComponents()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->selectedComponents.size() > 0) {
		copyData.clear();
		CNetDesignerDoc* pDoc = GetDocument();
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			int index = pDoc->selectedComponents[i];
			if (pDoc->netData.isNode(index)) {
				copyData.pushNodeData(pDoc->netData.getNode(index));
			}
			else if (pDoc->netData.isConnect(index)) {
				string sid = pDoc->netData.getID(index);
				string slbl = pDoc->netData.getLabel(index);
				string snode_in = pDoc->netData.getInputNode(index);
				string snode_out = pDoc->netData.getOutputNode(index);
				vector<twoValues> points;
				double sweight = pDoc->netData.getWeight(index);
				vector<double> sweights = pDoc->netData.getConvWeights(index);
				copyData.pushConnectData(sid, slbl, snode_in, snode_out, pDoc->assembly_edit_index, sweight, sweights, points);
			}
			else if (pDoc->netData.isConvLayer(index)) {
				copyData.pushConvLayer(pDoc->netData.getNode(index));
			}
			else if (pDoc->netData.isAttentionLayer(index)) {
				copyData.pushAttentionLayer(pDoc->netData.getNode(index));
			}
			else if (pDoc->netData.isAssemblyLayer(index)) {
				copyData.pushAssembly(pDoc->netData.getNode(index));
			}
			else if (pDoc->netData.isNetCluster(index)) {
				copyData.pushNetCluster(pDoc->netData.getNode(index));
			}
			else if (pDoc->netData.isDropout(index)) {
				copyData.pushDropout(pDoc->netData.getNode(index));
			}
			else {
				addErrorString("copySelectedComponents : unable to process " + pDoc->netData.getID(index));
			}
		}
	}
}

void CNetDesignerView::startPlaceComponent(CString tool)
{
	CNetDesignerDoc* pDoc = GetDocument();
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
	else if (tool == _T("ConvLayer") || tool == _T("CNN")) {
		CNetDesignerDoc* pDoc = GetDocument();
		//new_group = 2;
		placing_component_type = _T("ConvLayer");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing ConvLayer"));
	}
	else if (tool == _T("InputLayer")) {
		bool add = true;
		if (pDoc->getNumInputLayers() > pDoc->max_series_inputs) {
			add = false;
		}
		if (add) {
			//new_group = 2;
			placing_component_type = _T("InputLayer");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			SetFocus();
			addPanel1String(_T("Placing Input Layer"));
		}

	}
	else if (tool == _T("OutputLayer")) {
		bool add = true;
		if (pDoc->getNumOutputLayers() > 1) {
			add = false;
		}
		if (add) {
			//new_group = 2;
			placing_component_type = _T("OutputLayer");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			SetFocus();
			addPanel1String(_T("Placing Output Layer"));
		}

	}
	else if (tool == _T("HiddenLayers")) {
		CNetDesignerDoc* pDoc = GetDocument();
		//new_group = 2;
		placing_component_type = _T("HiddenLayers");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing Hidden Layers"));
	}
	else if (tool == _T("Attention")) {
		CNetDesignerDoc* pDoc = GetDocument();
		placing_component_type = _T("Attention");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing Attention Layer"));
	}
	else if (tool == _T("SelfAttention")) {
		CNetDesignerDoc* pDoc = GetDocument();
		placing_component_type = _T("SelfAttention");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing Self Attention Layer"));
	}
	else if (tool == _T("Assembly")) {
		CNetDesignerDoc* pDoc = GetDocument();
		placing_component_type = _T("Assembly");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing Self Assembly"));
	}
	else if (tool == _T("NetCluster")) {
		CNetDesignerDoc* pDoc = GetDocument();
		placing_component_type = _T("NetCluster");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing Net Pattern Output"));
	}
	else if (tool == _T("DropOut")) {
		CNetDesignerDoc* pDoc = GetDocument();
		placing_component_type = _T("DropOut");
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS);
		SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
		SetCursor(hCurs1);
		SetFocus();
		addPanel1String(_T("Placing Dropout"));
	}
	else {
		CString msg = _T("startPlaceComponent error - tool = ") + tool;
		addErrorString(msg);
		addPanel1String(_T(""));
	}
}

void CNetDesignerView::escapePlaceComponent()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (placing_component_type != _T("")) {
		connectingEraseLine();
		connecting = false;
		connectingPoints.clear();
		connectingComponents.clear();
		pDoc->selectedComponents.clear();
		selectedComponentsPoints1.clear();
		selectedComponentsPoints2.clear();
		newGroupData.clear();
		checkAssemblyEditable();
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
			bool place = true;
			if (pDoc->assembly_edit_index >= 0) {
				int cnt = 0;
				for (int k = 0; k < pDoc->netData.size(); k++) {
					if (pDoc->netData.getGroupIndex(k) == pDoc->assembly_edit_index) {
						if (pDoc->netData.isNode(k)) {
							if (((pDoc->netData.getNodeMode(k) == "Input") && (placing_component_type == _T("Input"))) || 
								((pDoc->netData.getNodeMode(k) == "Output") && (placing_component_type == _T("Output")))) {
								cnt++;
							}
						}
					}
				}
				if (cnt >= 5) {
					if (placing_component_type == _T("Input")) {
						addErrorString(_T("Unable to place more than five input nodes for assembly"));
					}
					else if (placing_component_type == _T("Output")) {
						addErrorString(_T("Unable to place more than five output nodes for assembly"));
					}
					place = false;
				}
			}
			if (place) {
				CPoint mpt = point;
				mpt = DPtoLP(mpt);
				mpt = applyGrid(mpt);
				int index = pDoc->addNode(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, placing_component_type, pDoc->assembly_edit_index);
				selectComponent(index, false);
				pMainWnd->m_wndNodeView.FillFileView();
				return(true);
			}
			return(false);
		}
	}
	else if (placing_component_type == _T("Connect")) {
		if ((connectingComponents.size() > 1) && (connectingPoints.size() > 1)) {
			if (!isConnectAlreadyHere(DPtoLP(point))) {
				vector<CPoint> points;
				for (int i = 0; i < connectingPoints.size(); i++) {
					points.push_back(connectingPoints[i]);
				}
				vector<double> sweights;
				//addErrorString(_T("addConnect : ") + connectingComponents[0] + _T(" : ") + connectingComponents[1]);
				int index = pDoc->addConnect(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos,
					connectingComponents[0],
					connectingComponents[1],
					pDoc->assembly_edit_index, 0.0, sweights, points);
				selectComponent(index, false);
				connectingComponents.clear();
				connectingPoints.clear();
			}
		}
		return(true);
	} 
	else if (placing_component_type == _T("ConvLayer")) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			bool place = true;
			if (place) {
				CPoint mpt = point;
				mpt = DPtoLP(mpt);
				mpt = applyGrid(mpt);
				vector<int> dimensions;
				dimensions.push_back(2);
				dimensions.push_back(2);
				dimensions.push_back(2);
				dimensions.push_back(2);
				vector<int> strides;
				strides.push_back(2);
				strides.push_back(2);
				CString mode = _T("Hidden");
				int data_type = pDoc->conv_data_type;
				if (pDoc->alreadyHasInputLayer(-1) && (data_type == SOUND_CONV)) {
					data_type = DATA_CONV;
				}
				if (data_type == DATA_CONV) {
					dimensions[0] = 1;
					dimensions[1] = 2;
					dimensions[2] = 2;
					dimensions[3] = 32;
					strides[0] = 2;
					strides[1] = 32;
				}
				CAuxFunctions func;
				vector<CPoint> locations = func.getLayerPoints3((int)pDoc->nradius2, data_type);
				vector<string> fields;
				int index = pDoc->addConvLayer(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, mode, pDoc->dfuncn, pDoc->deta, pDoc->dalpha, pDoc->drand, "", 0.0, 0.0, true, pDoc->dclayers, pDoc->dcnodesperlayer, data_type, dimensions, strides, locations, fields, pDoc->assembly_edit_index);
				selectComponent(index, false);
				placing_component_type = _T("");
				HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
				SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
				SetCursor(hCurs1);
				addPanel1String(_T(""));
				pMainWnd->m_wndNodeView.FillFileView();
				return(true);
			}
		}
		return(false);
	}
	else if (placing_component_type == _T("InputLayer")) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			bool place = true;
			CPoint mpt = point;
			if (pDoc->conv_data_type == IMAGE_CONV) {
				for (int i = 0; i < pDoc->netData.size(); i++) {
					if (pDoc->netData.isConvLayer(i)) {
						if (pDoc->netData.getNodeMode(i) == "Input") {
							addErrorString(_T("Unable to place more than one input image layers"));
							i = pDoc->netData.size();
							place = false;
						}
					}
				}
			}
			if (place) {
				mpt = DPtoLP(mpt);
				mpt = applyGrid(mpt);
				vector<int> dimensions;
				dimensions.push_back(4);
				dimensions.push_back(4);
				dimensions.push_back(2);
				dimensions.push_back(2);
				vector<int> strides;
				strides.push_back(4);
				strides.push_back(4);
				CString mode = _T("Input");
				int data_type = pDoc->getDataConvType();
				int ptype = pDoc->getDataPathType();
				if (ptype == FILE_TYPE_WAV) {
					data_type = SOUND_CONV;
				}
				else if (ptype == FILE_TYPE_JPG) {
					data_type = IMAGE_CONV;
				}
				CAuxFunctions func;
				vector<CPoint> locations = func.getLayerPoints3((int)pDoc->nradius2, pDoc->conv_data_type);	
				vector<string> fields;
				int index = pDoc->addConvLayer(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, mode,
					pDoc->dfuncn, pDoc->deta, pDoc->dalpha, pDoc->drand, "", 0.0, 0.0, true, pDoc->dclayers, pDoc->dcnodesperlayer,
					data_type, dimensions, strides, locations, fields, pDoc->assembly_edit_index);
				selectComponent(index, false);
				pMainWnd->m_wndNodeView.FillFileView();
			}
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
			return(true);
		}
		return(false);
	}
	else if (placing_component_type == _T("OutputLayer")) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			bool place = true;
			CPoint mpt = point;
			if (pDoc->conv_data_type == IMAGE_CONV) {
				for (int i = 0; i < pDoc->netData.size(); i++) {
					if (pDoc->netData.isConvLayer(i)) {
						if (pDoc->netData.getNodeMode(i) == "Output") {
							addErrorString(_T("Unable to place more than one output image layers"));
							i = pDoc->netData.size();
							place = false;
						}
					}
				}
			}
			if (place) {
				mpt = DPtoLP(mpt);
				mpt = applyGrid(mpt);
				vector<int> dimensions;
				dimensions.push_back(4);
				dimensions.push_back(4);
				dimensions.push_back(2);
				dimensions.push_back(2);
				vector<int> strides;
				strides.push_back(4);
				strides.push_back(4);
				CString mode = _T("Output");
				int data_type = pDoc->getDataConvType();
				int ptype = pDoc->getDataPathType();
				if (ptype == FILE_TYPE_WAV) {
					data_type = SOUND_CONV;
				}
				else if (ptype == FILE_TYPE_JPG) {
					data_type = IMAGE_CONV;
				}
				int nodes_per_layer = pDoc->dcnodesperlayer;
				CAuxFunctions func;
				vector<CPoint> locations = func.getLayerPoints3((int)pDoc->nradius2, pDoc->conv_data_type);
				vector<string> fields;
				if ((mode == _T("Output")) && (pDoc->conv_data_type == IMAGE_CONV)) {
					nodes_per_layer = pDoc->getOutputImageSize();
				}
				int index = pDoc->addConvLayer(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, mode, 
					pDoc->dfuncn, pDoc->deta, pDoc->dalpha, pDoc->drand, "", 0.0, 0.0, true, pDoc->dclayers, nodes_per_layer, 
					data_type, dimensions, strides, locations, fields, pDoc->assembly_edit_index);
				selectComponent(index, false);
			}
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
			return(true);
		}
		return(false);
	}
	else if (placing_component_type == _T("HiddenLayers")) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);
			vector<int> dimensions;
			dimensions.push_back(4);
			dimensions.push_back(4);
			dimensions.push_back(2);
			dimensions.push_back(2);
			vector<int> strides;
			strides.push_back(4);
			strides.push_back(4);
			CString mode = _T("Hidden");
			int my_conv_data_type = HIDDEN_NET;
			CAuxFunctions func;
			vector<CPoint> locations = func.getLayerPoints3((int)pDoc->nradius2, my_conv_data_type);
			vector<string> fields;
			int my_num_layers = 2;
			int my_nodes_per_layer = 8;
			int index = pDoc->addConvLayer(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, mode,
				pDoc->dfuncn, pDoc->deta, pDoc->dalpha, pDoc->drand, "", 0.0, 0.0, true, my_num_layers, my_nodes_per_layer,
				my_conv_data_type, dimensions, strides, locations, fields, pDoc->assembly_edit_index);
			selectComponent(index, false);
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
			return(true);
		}
	}
	else if ((placing_component_type == _T("Attention")) || (placing_component_type == _T("SelfAttention"))) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);
			int my_ksize = 8;
			int my_qsize = 8;
			int my_vsize = 8;
			double my_slimit = 0.01;
			bool self = false;
			if (placing_component_type == _T("SelfAttention")) {
				self = true;
			}
			int index = pDoc->addAttentionLayer(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, my_ksize, my_qsize, my_vsize, my_slimit, self, pDoc->assembly_edit_index);
			selectComponent(index, false);
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
			return(true);
		}
	}
	else if (placing_component_type == _T("Assembly")) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			bool add = true;
			string assembly_reference = "";
			vector<string> assemblynames = getAssemblyNames();
			if (assemblynames.size() > 0) {
				CAssemblyPicker adlg;
				CRect rect;  GetWindowRect(&rect);
				adlg.spt.x = point.x + rect.left;
				adlg.spt.y = point.y + rect.top;
				adlg.assemblies = assemblynames;
				if (adlg.DoModal() == IDOK) {
					if (adlg.m_new) {
						assembly_reference = string(CT2CA(adlg.m_assembly));
					}
				} 
				else {
					add = false;
				}
			}
			if (add) {
				CPoint mpt = point;
				mpt = DPtoLP(mpt);
				mpt = applyGrid(mpt);
				int index = pDoc->addAssembly(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, pDoc->assembly_edit_index, assembly_reference);
				selectComponent(index, false);
			}
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
			return(true);
		}
	}
	else if (placing_component_type == _T("NetCluster")) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);
			CString mode = _T("Output");
			int my_conv_data_type = NETCLUSTER;
			CAuxFunctions func;
			int my_nodes_per_layer = 8;
			int index = pDoc->addNetCluster(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, mode,
				pDoc->dfuncn, pDoc->deta, pDoc->dalpha, pDoc->drand, my_nodes_per_layer, my_conv_data_type, pDoc->assembly_edit_index);
			selectComponent(index, false);
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
			return(true);
		}
	}
	else if (placing_component_type == _T("DropOut")) {
		if (!isNodeAlreadyHere(DPtoLP(point))) {
			CPoint mpt = point;
			mpt = DPtoLP(mpt);
			mpt = applyGrid(mpt);
			double percent = 0.02;
			int index = pDoc->addDropout(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos, mpt.x, mpt.y, percent, pDoc->assembly_edit_index);
			selectComponent(index, false);
			placing_component_type = _T("");
			HCURSOR hCurs1 = LoadCursor(NULL, IDC_ARROW);
			SetClassLongPtrA(m_hWnd, GCLP_HCURSOR, (LONG_PTR)hCurs1);
			SetCursor(hCurs1);
			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
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

				int mradius = (int)pDoc->nradius;
				int msnap = (int)_tstof((LPCTSTR)pDoc->snap_spacing);
				int mseparation = 3 * msnap;

				int snap_times = 2;
				if (dlg.m_compact) {
					snap_times = 1;
				}

				if (msnap > 0) {
					mseparation = ((mradius / msnap) * msnap) + (snap_times * msnap);
					if ((mradius % msnap) > (msnap / 2)) {
						mseparation += msnap;
					}
				}
				else {
					mseparation = (3 * mradius) / 2;
				}
				newGroupData = dlg.makeNet(mpt, mseparation, msnap, pDoc->assembly_edit_index);

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
					if (newGroupData.getNodeMode(i) == "PreProcess") {
						gnormalize = true;
					}
				}
			}

			pDoc->selectedComponents.clear();
			selectedComponentsPoints1.clear();
			selectedComponentsPoints2.clear();

			for (int i = 0; i < newGroupData.size(); i++) {
				if (newGroupData.isNode(i)) {
					if (!gnormalize || (newGroupData.getNodeMode(i) != "Input")) {
						int pindex = pDoc->netData.getIndexFromID(newGroupData.getID(i));
						pDoc->selectedComponents.push_back(pindex);
						selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
						selectedComponentsPoints2.push_back(CPoint(0, 0));
					}
				}
			}
			checkAssemblyEditable();

			autoRoute();

			newGroupData.clear();

			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
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

				newGroupData = dlg.makeNet(mpt, pDoc->assembly_edit_index);

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
			pDoc->selectedComponents.clear();
			selectedComponentsPoints1.clear();
			selectedComponentsPoints2.clear();
			newGroupData.clear();
			checkAssemblyEditable();

			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
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

				newGroupData = dlg.makeNet(mpt, pDoc->assembly_edit_index);

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
					if (newGroupData.getNodeMode(i) == "PreProcess") {
						gnormalize = true;
					}
				}
			}

			pDoc->selectedComponents.clear();
			selectedComponentsPoints1.clear();
			selectedComponentsPoints2.clear();

			for (int i = 0; i < newGroupData.size(); i++) {
				if (newGroupData.isNode(i)) {
					if (!gnormalize || (newGroupData.getNodeMode(i) != "Input")) {
						int pindex = pDoc->netData.getIndexFromID(newGroupData.getID(i));
						pDoc->selectedComponents.push_back(pindex);
						selectedComponentsPoints1.push_back(CPoint(pDoc->netData.getXDisp(pindex), pDoc->netData.getYDisp(pindex)));
						selectedComponentsPoints2.push_back(CPoint(0, 0));
					}
				}
			}
			checkAssemblyEditable();

			autoRoute();

			newGroupData.clear();

			addPanel1String(_T(""));
			pMainWnd->m_wndNodeView.FillFileView();
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
			vector<double> sweights;
			pDoc->pushEditConnectData((CString)newGroupData.getID(i).c_str(), (CString)newGroupData.getID(i).c_str(), (CString)newGroupData.getInputNode(i).c_str(), (CString)newGroupData.getOutputNode(i).c_str(), pDoc->assembly_edit_index, 0.0, sweights, points);
		}
	}
	pDoc->processEditData();

}

void CNetDesignerView::checkAssemblyEditable()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((int)pDoc->selectedComponents.size() == 1) {
		if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[0])) {
			pDoc->assembly_editable = true;
			return;
		}
	}
	pDoc->assembly_editable = false;
}

int CNetDesignerView::getAssemblyIndex()
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((int)pDoc->selectedComponents.size() == 1) {
		CNetDesignerDoc* pDoc = GetDocument();
		pDoc->assembly_edit_index = pDoc->getAssemblyIndex(pDoc->selectedComponents[0]);
		pDoc->no_sub_assembly_edit_index = pDoc->getNoSubAssemblyIndex(pDoc->selectedComponents[0]);
		return(pDoc->assembly_edit_index);
	}
	return(-1);
}

vector<string> CNetDesignerView::getAssemblyNames()
{
	vector<string> rval;
	CNetDesignerDoc* pDoc = GetDocument();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isAssemblyLayer(i)) {
			rval.push_back(pDoc->netData.getID(i));
		}
	}
	return(rval);
}

vector<int> CNetDesignerView::getAssemblySubComponents(int sindex)
{
	vector<int> rval;
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->netData.isAssemblyLayer(sindex)) {
		int gindex = pDoc->getAssemblyIndex(sindex);
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.getGroupIndex(i) == gindex) {
				rval.push_back(i);
				if (pDoc->netData.isAssemblyLayer(i)) {
					vector<int> acomps = getAssemblySubComponents(i);
					if (acomps.size() > 0) {
						for (int j = 0; j < (int)acomps.size(); j++) {
							bool found = false;
							for (int k = 0; k < (int)rval.size(); k++) {
								if (acomps[j] == rval[k]) {
									k = (int)rval.size();
									found = true;
								}
							}
							if (!found) {
								rval.push_back(acomps[j]);
							}
						}
					}
				}
			}
		}
	}
	return(rval);
}

bool CNetDesignerView::isNodeAlreadyHere(CPoint here)
{
	CNetDesignerDoc* pDoc = GetDocument();
	int index = pDoc->checkComponentPlacementReturnIndex(here.x, here.y, SELECT_ALL, pDoc->assembly_edit_index);
	if (index >= 0) {
		if (pDoc->netData.isNode(index) || pDoc->netData.isConvLayer(index) || pDoc->netData.isAttentionLayer(index) || 
			pDoc->netData.isAssemblyLayer(index) || pDoc->netData.isNetCluster(index) || pDoc->netData.isDropout(index)) {
			return true;
		}
	}
	return false;
}

bool CNetDesignerView::isConnectAlreadyHere(CPoint here)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if ((connectingComponents.size() > 1) && (connectingPoints.size() > 1)) {
		CString from = connectingComponents[0];
		CString to = connectingComponents[0];
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.getGroupIndex(i) == pDoc->assembly_edit_index) {
				if (pDoc->netData.isConnect(i)) {
					if (((from == (CString)pDoc->netData.getInputNode(i).c_str()) && (to == (CString)pDoc->netData.getOutputNode(i).c_str())) ||
						((to == (CString)pDoc->netData.getInputNode(i).c_str()) && (from == (CString)pDoc->netData.getOutputNode(i).c_str()))) {
						return(true);
					}
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
	rpt.y = applyGridY(rpt.y);
	return(rpt);
}

int CNetDesignerView::applyGridX(int x)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->snap_spacing != _T("None")) {
		int ngrid = (int)_tstof((LPCTSTR)pDoc->snap_spacing);
		int myx = x;
		if (myx < 0) {
			myx -= ngrid / 2;
		}
		int div = myx / ngrid;
		int g1 = ngrid * div;			// (x / ngrid);
		int g2 = ngrid * (div + 1);		// (x / ngrid) + 1);
		int d1 = myx - g1;
		int d2 = g2 - myx;
		//CString s = _T("");
		//s.Format(_T("x = %i : d1 = %i : d2 = %i : div = %i : g1 = %i : g2 = %i"), x, d1, d2, div, g1, g2);
		//addErrorString(s);
		if (d1 < d2) {
			return(g1);
		}
		else {
			return(g2);
		}
	}
	return(x);
}

int CNetDesignerView::applyGridY(int y)
{
	CNetDesignerDoc* pDoc = GetDocument();
	if (pDoc->snap_spacing != _T("None")) {
		int ngrid = (int)_tstof((LPCTSTR)pDoc->snap_spacing);
		int myy = y;
		if (myy < 0) {
			myy -= ngrid / 2;
		}
		int div = myy / ngrid;
		int g1 = ngrid * div;			// (y / ngrid);
		int g2 = ngrid * (div + 1);		// (y / ngrid) + 1);
		int d1 = myy - g1;
		int d2 = g2 - myy;
		//CString s = _T("");
		//s.Format(_T("y = %i : d1 = %i : d2 = %i : g1 = %i : g2 = %i"), x, d1, d2, g1, g2);
		//addErrorString(s);
		if (d1 < d2) {
			return(g1);
		}
		else {
			return(g2);
		}
	}
	return(y);
}

void CNetDesignerView::autoRoute()
{
	DrawArrow(route_origin.x, route_origin.y, route_angle);

	rnodes.clear();

	CAuxFunctions func;

	CNetDesignerDoc* pDoc = GetDocument();

	if (pDoc->selectedComponents.size() > 0) {
		for (int i = 0; i < pDoc->selectedComponents.size(); i++) {
			if (pDoc->netData.getGroupIndex(pDoc->selectedComponents[i]) == pDoc->assembly_edit_index) {
				if (pDoc->netData.isNode(pDoc->selectedComponents[i])) {
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
				}
				else if (pDoc->netData.isConvLayer(pDoc->selectedComponents[i])) {
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
				}
				else if (pDoc->netData.isAttentionLayer(pDoc->selectedComponents[i])) {
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
				}
				else if (pDoc->netData.isAssemblyLayer(pDoc->selectedComponents[i])) {
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
				}
				else if (pDoc->netData.isNetCluster(pDoc->selectedComponents[i])) {
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
				}
				else if (pDoc->netData.isDropout(pDoc->selectedComponents[i])) {
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
					addComponentToRouteOrder(pDoc->selectedComponents[i]);
				}
			}
		}
	}
	else {
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.getGroupIndex(i) == pDoc->assembly_edit_index) {
				if (pDoc->netData.isNode(i)) {
					addComponentToRouteOrder(i);
				}
				else if (pDoc->netData.isConvLayer(i)) {
					addComponentToRouteOrder(i);
					addComponentToRouteOrder(i);
				}
				else if (pDoc->netData.isAttentionLayer(i)) {
					addComponentToRouteOrder(i);
					addComponentToRouteOrder(i);
				}
				else if (pDoc->netData.isAssemblyLayer(i)) {
					addComponentToRouteOrder(i);
					addComponentToRouteOrder(i);
				}
				else if (pDoc->netData.isNetCluster(i)) {
					addComponentToRouteOrder(i);
					addComponentToRouteOrder(i);
				}
				else if (pDoc->netData.isDropout(i)) {
					addComponentToRouteOrder(i);
					addComponentToRouteOrder(i);
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

	// Find existing connections
	connections.clear();
	for (int i = 0; i < pDoc->netData.size(); i++) {
		if (pDoc->netData.isConnect(i)) {
			string idin = func.getConvNameFromConnectName(pDoc->netData.getInputNode(i));
			string idout = func.getConvNameFromConnectName(pDoc->netData.getOutputNode(i));
			bool found = false;
			for (int j = 0; j < connections.size(); j++) {
				if (idin == string(CT2CA(connections[j].input_node))) {
					connections[j].output_nodes.push_back((CString)idout.c_str());
					j = (int)connections.size();
					found = true;
				}
			}
			if (!found) {
				connectData tconnect;
				tconnect.input_node = (CString)idin.c_str();
				tconnect.output_nodes.push_back((CString)idout.c_str());
				connections.push_back(tconnect);
			}
		}
	}

	// Get connections to add
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
			if (rnodes[i].group == (ogroup - 1)) {
				if ((rnodes[i].type == NODE) ||
					((rnodes[i].type == CONVLAYER) && rnodes[i].convinput) ||
					((rnodes[i].type == ATTENTION) && rnodes[i].attentioninput) ||
					((rnodes[i].type == ASSEMBLY) && rnodes[i].assemblyinput) ||
					((rnodes[i].type == NETCLUSTER) && rnodes[i].netclusterinput) ||
					((rnodes[i].type == DROPOUT) && rnodes[i].netdropoutinput)) {
					//addErrorString(_T("adding ") + pDoc->netData.getID(rnodes[i].nindex) + _T(" to fromIndices"));
					fromIndices.push_back(rnodes[i].nindex);
				}
			}
			else if (rnodes[i].group == ogroup) {
				if ((rnodes[i].type == NODE) ||
					((rnodes[i].type == CONVLAYER) && rnodes[i].convoutput) ||
					((rnodes[i].type == ATTENTION) && rnodes[i].attentionoutput) ||
					((rnodes[i].type == ASSEMBLY) && rnodes[i].assemblyoutput) ||
					((rnodes[i].type == NETCLUSTER) && rnodes[i].netclusteroutput) ||
					((rnodes[i].type == DROPOUT) && rnodes[i].netdropoutoutput)) {
					//addErrorString(_T("adding ") + pDoc->netData.getID(rnodes[i].nindex) + _T(" to toIndices"));
					toIndices.push_back(rnodes[i].nindex);
				}
			}
		}
		for (int i = 0; i < fromIndices.size(); i++) {
			CString idin = (CString)pDoc->netData.getID(fromIndices[i]).c_str();
			for (int j = 0; j < toIndices.size(); j++) {
				CString idout = (CString)pDoc->netData.getID(toIndices[j]).c_str();
				if (!areNodesConnected(fromIndices[i], toIndices[j])) {

					//addErrorString(idin + _T(" is connected to ") + idout);
					int indexin = pDoc->netData.getIndexFromID(string(CT2CA(idin)));
					int indexout = pDoc->netData.getIndexFromID(string(CT2CA(idout)));

					if (pDoc->netData.isConvLayer(indexout)) {
						
						if (pDoc->netData.isConvLayer(indexin)) {
							bool image_conv_in = true;
							if (pDoc->netData.getConvType(indexin) != IMAGE_CONV) {
								image_conv_in = false;
							}
							bool image_conv_out = true;
							if (pDoc->netData.getConvType(indexout) != IMAGE_CONV) {
								image_conv_out = false;
							}
							add_connects addconnect;
							if (pDoc->getDataImageGray() || (!image_conv_in && !image_conv_out)) {
								addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect.node_in = idin + _T(":i:0");
								addconnect.node_out = idout + _T(":o:0");
								bool found = false;
								for (int q = 0; q < addconnects.size(); q++) {
									if ((addconnect.node_in == addconnects[q].node_in) && (addconnect.node_out == addconnects[q].node_out)) {
										q = (int)addconnects.size();
										found = true;
									}
								}
								if (!found) {
									//addErrorString(_T("addconnect1 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
									addconnects.push_back(addconnect);
									nindex_offset++;
								}
							}
							else {
								CString s = _T("");
								for (int m = 0; m < 3; m++) {
									for (int n = 0; n < 3; n++) {
										add_connects addconnect2;
										addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
										if (image_conv_in) {
											s.Format(_T(":i:%i"), m);
										}
										else {
											s = _T(":i:0");
										}
										addconnect2.node_in = idin + s;
										if (image_conv_out) {
											s.Format(_T(":o:%i"), n);
										}
										else {
											s = _T(":o:0");
										}
										addconnect2.node_out = idout + s;
										bool found = false;
										for (int q = 0; q < addconnects.size(); q++) {
											if ((addconnect2.node_in == addconnects[q].node_in) && (addconnect2.node_out == addconnects[q].node_out)) {
												q = (int)addconnects.size();
												found = true;
											}
										}
										if (!found) {
											//addErrorString(_T("addconnect2 : ") + addconnect2.node_in + _T(" to ") + addconnect2.node_out);
											addconnects.push_back(addconnect2);
											nindex_offset++;
										}
									}
								}
							}
						}
						else if (pDoc->netData.isAttentionLayer(indexin)) {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin + _T(":i:0");
							addconnect.node_out = idout + _T(":o:0");
							//addErrorString(_T("addconnect8 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
							addconnects.push_back(addconnect);
							nindex_offset++;
							if (!pDoc->getDataImageGray() && (pDoc->netData.getConvType(indexout) == IMAGE_CONV)) {
								add_connects addconnect2;
								addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect2.node_in = idin + _T(":i:0");
								addconnect2.node_out = idout + _T(":o:1");
								//addErrorString(_T("addconnect9 : ") + addconnect2.node_in + _T(" to ") + addconnect2.node_out);
								addconnects.push_back(addconnect2);
								nindex_offset++;
								add_connects addconnect3;
								addconnect3.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect3.node_in = idin + _T(":i:0");
								addconnect3.node_out = idout + _T(":o:2");
								//addErrorString(_T("addconnect11 : ") + addconnect3.node_in + _T(" to ") + addconnect3.node_out);
								addconnects.push_back(addconnect3);
								nindex_offset++;
							}
						}
						else {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin;
							if ((pDoc->netData.isNetCluster(indexin)) || (pDoc->netData.isDropout(indexin)) || (pDoc->netData.isAssemblyLayer(indexin))) {
								addconnect.node_in += _T(":i:0");
							}
							addconnect.node_out = idout + _T(":o:0");
							//addErrorString(_T("addconnect15 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
							addconnects.push_back(addconnect);
							nindex_offset++;
							if (!pDoc->getDataImageGray() && (pDoc->netData.getConvType(indexout) == IMAGE_CONV)) {
								add_connects addconnect2;
								addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect2.node_in = idin;
								addconnect2.node_out = idout + _T(":o:1");
								//addErrorString(_T("addconnect16 : ") + addconnect2.node_in + _T(" to ") + addconnect2.node_out);
								addconnects.push_back(addconnect2);
								nindex_offset++;
								add_connects addconnect3;
								addconnect3.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect3.node_in = idin;
								addconnect3.node_out = idout + _T(":o:2");
								//addErrorString(_T("addconnect17 : ") + addconnect3.node_in + _T(" to ") + addconnect3.node_out);
								addconnects.push_back(addconnect3);
								nindex_offset++;
							}
						}

					}

					else if (pDoc->netData.isAttentionLayer(indexout)) {

						if (pDoc->netData.isAttentionLayer(indexin)) {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin + _T(":i:0");
							addconnect.node_out = idout + _T(":o:0");
							//addErrorString(_T("addconnect3 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
							addconnects.push_back(addconnect);
							nindex_offset++;
							if (!pDoc->netData.isSelfAttentionLayerOnly(indexout)) {
								add_connects addconnect2;
								addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect2.node_in = idin + _T(":i:0");
								addconnect2.node_out = idout + _T(":o:1");
								//addErrorString(_T("addconnect4 : ") + addconnect2.node_in + _T(" to ") + addconnect2.node_out);
								addconnects.push_back(addconnect2);
								nindex_offset++;
								add_connects addconnect3;
								addconnect3.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect3.node_in = idin + _T(":i:0");
								addconnect3.node_out = idout + _T(":o:2");
								//addErrorString(_T("addconnect5 : ") + addconnect3.node_in + _T(" to ") + addconnect3.node_out);
								addconnects.push_back(addconnect3);
								nindex_offset++;
							}
						}
						else if (pDoc->netData.isConvLayer(indexin)) {
							CString s = _T("");
							for (int m = 0; m < 3; m++) {
								if (!pDoc->netData.isSelfAttentionLayerOnly(indexout) || (m == 0)) {
									s.Format(_T(":o:%i"), m);
									add_connects addconnect;
									addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
									addconnect.node_in = idin + _T(":i:0");
									addconnect.node_out = idout + s;
									bool found = false;
									for (int q = 0; q < addconnects.size(); q++) {
										if ((addconnect.node_in == addconnects[q].node_in) && (addconnect.node_out == addconnects[q].node_out)) {
											q = (int)addconnects.size();
											found = true;
										}
									}
									if (!found) {
										//addErrorString(_T("addconnect6 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
										addconnects.push_back(addconnect);
										nindex_offset++;
									}
								}
							}
							if (!pDoc->getDataImageGray() && (pDoc->netData.getConvType(indexin) == IMAGE_CONV)) {
								for (int m = 0; m < 3; m++) {
									s.Format(_T(":o:%i"), m);
									add_connects addconnect2;
									addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
									addconnect2.node_in = idin + _T(":i:1");
									addconnect2.node_out = idout + s;
									//addErrorString(_T("addconnect7a : ") + addconnect2.node_in + _T(" to ") + addconnect2.node_out);
									addconnects.push_back(addconnect2);
									nindex_offset++;
								}
								for (int m = 0; m < 3; m++) {
									s.Format(_T(":o:%i"), m);
									add_connects addconnect3;
									addconnect3.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
									addconnect3.node_in = idin + _T(":i:2");
									addconnect3.node_out = idout + s;
									//addErrorString(_T("addconnect7b : ") + addconnect3.node_in + _T(" to ") + addconnect3.node_out);
									addconnects.push_back(addconnect3);
									nindex_offset++;
								}
							}
						}
						else if (pDoc->netData.isNetCluster(indexin)) {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin + _T(":i:0");
							addconnect.node_out = idout + _T(":o:1");
							//addErrorString(_T("addconnect19 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
							addconnects.push_back(addconnect);
							nindex_offset++;
						}
						else {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin;
							if (pDoc->netData.isDropout(indexin) || pDoc->netData.isAssemblyLayer(indexin)) {
								addconnect.node_in += _T(":i:0");
							}
							addconnect.node_out = idout + _T(":o:0");
							//addErrorString(_T("addconnect19 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
							addconnects.push_back(addconnect);
							nindex_offset++;
							add_connects addconnect2;
							addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect2.node_in = idin;
							if (pDoc->netData.isDropout(indexin) || pDoc->netData.isAssemblyLayer(indexin)) {
								addconnect2.node_in += _T(":i:0");
							}
							if (!pDoc->netData.isSelfAttentionLayerOnly(indexout)) {
								addconnect2.node_out = idout + _T(":o:1");
								//addErrorString(_T("addconnect20 : ") + addconnect2.node_in + _T(" to ") + addconnect2.node_out);
								addconnects.push_back(addconnect2);
								nindex_offset++;
								add_connects addconnect3;
								addconnect3.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
								addconnect3.node_in = idin;
								if (pDoc->netData.isDropout(indexin) || pDoc->netData.isAssemblyLayer(indexin)) {
									addconnect3.node_in += _T(":i:0");
								}
								addconnect3.node_out = idout + _T(":o:2");
								//addErrorString(_T("addconnect21 : ") + addconnect3.node_in + _T(" to ") + addconnect3.node_out);
								addconnects.push_back(addconnect3);
								nindex_offset++;
							}
						}

					}

					else if (pDoc->netData.isNetCluster(indexout)) {
						if (pDoc->netData.isConvLayer(indexin)) {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin + _T(":i:0");
							addconnect.node_out = idout;// +_T(":o:0");
							addconnects.push_back(addconnect);
							nindex_offset++;
							if (!pDoc->getDataImageGray() && (pDoc->netData.getConvType(indexin) == IMAGE_CONV)) {
									add_connects addconnect2;
									addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
									addconnect2.node_in = idin + _T(":i:1");
									addconnect2.node_out = idout;// +_T(":o:0");
									addconnects.push_back(addconnect2);
									nindex_offset++;
									add_connects addconnect3;
									addconnect3.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
									addconnect3.node_in = idin + _T(":i:2");
									addconnect3.node_out = idout;// +_T(":o:0");
									addconnects.push_back(addconnect3);
									nindex_offset++;
							}
						}
						else if (pDoc->netData.isAttentionLayer(indexin)) {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin + _T(":i:0");
							addconnect.node_out = idout;
							addconnects.push_back(addconnect);
							nindex_offset++;
						}
						else {
							add_connects addconnect;
							addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect.node_in = idin;
							if (pDoc->netData.isDropout(indexin) || pDoc->netData.isAssemblyLayer(indexin)) {
								addconnect.node_in += _T(":i:0");
							}
							addconnect.node_out = idout;
							addconnects.push_back(addconnect);
							nindex_offset++;
						}
					}

					else if (pDoc->netData.isConvLayer(indexin)) {
						add_connects addconnect;
						addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
						addconnect.node_in = idin + _T(":i:0");
						addconnect.node_out = idout;
						if (pDoc->netData.isAssemblyLayer(indexout)) {
							addconnect.node_out += _T(":o:0");
						}
						bool found = false;
						for (int q = 0; q < addconnects.size(); q++) {
							if ((addconnect.node_in == addconnects[q].node_in) && (addconnect.node_out == addconnects[q].node_out)) {
								q = (int)addconnects.size();
								found = true;
							}
						}
						if (!found) {
							//addErrorString(_T("addconnect12 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
							addconnects.push_back(addconnect);
							nindex_offset++;
						}
						if (!pDoc->getDataImageGray() && (pDoc->netData.getConvType(indexin) == IMAGE_CONV)) {
							add_connects addconnect2;
							addconnect2.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect2.node_in = idin + _T(":i:1");
							addconnect2.node_out = idout;
							//addErrorString(_T("addconnect13 : ") + addconnect2.node_in + _T(" to ") + addconnect2.node_out);
							addconnects.push_back(addconnect2);
							nindex_offset++;
							add_connects addconnect3;
							addconnect3.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
							addconnect3.node_in = idin + _T(":i:2");
							addconnect3.node_out = idout;
							//addErrorString(_T("addconnect14 : ") + addconnect3.node_in + _T(" to ") + addconnect3.node_out);
							addconnects.push_back(addconnect3);
							nindex_offset++;
						}
					}

					else if (pDoc->netData.isAttentionLayer(indexin)) {
						add_connects addconnect;
						addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
						addconnect.node_in = idin + _T(":i:0");
						addconnect.node_out = idout;
						if (pDoc->netData.isAssemblyLayer(indexout)) {
							addconnect.node_out += _T(":o:0");
						}
						bool found = false;
						for (int q = 0; q < addconnects.size(); q++) {
							if ((addconnect.node_in == addconnects[q].node_in) && (addconnect.node_out == addconnects[q].node_out)) {
								q = (int)addconnects.size();
								found = true;
							}
						}
						if (!found) {
							//addErrorString(_T("addconnect18 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
							addconnects.push_back(addconnect);
							nindex_offset++;
						}
					}

					else {
						//addErrorString("autoRoute : tp22 : " + pDoc->netData.getID(indexin) + " : " + pDoc->netData.getID(indexout));
						add_connects addconnect;
						addconnect.sid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + nindex_offset);
						addconnect.node_in = idin;
						if (pDoc->netData.isNetCluster(indexin) || pDoc->netData.isDropout(indexin) || pDoc->netData.isAssemblyLayer(indexin)) {
							//addErrorString("autoRoute : setting node_in : " + pDoc->netData.getID(indexin) + " : " + pDoc->netData.getID(indexout));
							addconnect.node_in += _T(":i:0");
						}
						addconnect.node_out = idout;
						if (pDoc->netData.isAssemblyLayer(indexout)) {
							//addErrorString("autoRoute : setting node_out : " + pDoc->netData.getID(indexin) + " : " + pDoc->netData.getID(indexout));
							addconnect.node_out += _T(":o:0");
						}
						//addErrorString(_T("addconnect22 : ") + addconnect.node_in + _T(" to ") + addconnect.node_out);
						addconnects.push_back(addconnect);
						nindex_offset++;
					}

				}
			}
		}
		ogroup++;
	}

	/*for (int i = 0; i < addconnects.size(); i++) {
		CString s = _T("");
		s.Format(_T("%i"), i);
		addErrorString(_T("addconnects[") + s + _T("] : ") + addconnects[i].node_in + _T(" to ") + addconnects[i].node_out);
	}*/

	if (addconnects.size() > 0) {
		//int change_point_in_CNetData_pushConnectData_change_to_not_clearing_weights_to_make_routing_faster;
		vector<CPoint> points;
		pDoc->clearAndSetEditParams(UNDO_ADD, zoomScale, lHorzScollPos, lVertScollPos);
		for (int i = 0; i < addconnects.size(); i++) {
			vector<double> sweights;
			pDoc->pushEditConnectData(addconnects[i].sid, addconnects[i].sid, addconnects[i].node_in, addconnects[i].node_out, pDoc->assembly_edit_index, 0.0, sweights, points);
		}
		if (pDoc->processEditData()) {
			CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
			pMainWnd->m_wndNodeView.FillFileView();
		}
	}

	rnodes.clear();

	routing = false;
	OnDraw();

}

void CNetDesignerView::addComponentToRouteOrder(int index)
{
	CNetDesignerDoc* pDoc = GetDocument();
	bool route = true;
	sort_data tnode;
	tnode.nindex = index;
	tnode.type = NODE;
	tnode.convinput = false;
	tnode.convoutput = false;
	tnode.attentioninput = false;
	tnode.attentionoutput = false;
	tnode.assemblyinput = false;
	tnode.assemblyoutput = false;
	tnode.netclusterinput = false;
	tnode.netclusteroutput = false;
	tnode.netdropoutinput = false;
	tnode.netdropoutoutput = false;
	int inputwoffset = 0;
	if (pDoc->netData.isConvLayer(index)) {
		route = false;
		CRect brect = pDoc->getConvRect(pDoc->getConvParameters(index), zoomScale);
		tnode.type = CONVLAYER;
		bool already = false;
		for (int i = 0; i < rnodes.size(); i++) {
			if (tnode.nindex == rnodes[i].nindex) {
				already = true;
				i = (int)rnodes.size();
			}
		}
		string mmode = pDoc->netData.getNodeMode(index);
		if (mmode == "Input" && !already) {
			tnode.convinput = true;
			inputwoffset = pDoc->getConvInputOffset(brect, zoomScale);
			route = true;
		}
		else if (mmode == "Hidden") {
			if (!already) {
				tnode.convinput = true;
				inputwoffset = pDoc->getConvInputOffset(brect, zoomScale);
				route = true;
			}
			else {
				tnode.convoutput = true;
				route = true;
			}
		}
		else if (mmode == "Output" && !already) {
			tnode.convoutput = true;
			route = true;
		}
	}
	else if (pDoc->netData.isAttentionLayer(index)) {
		route = false;
		CRect brect = pDoc->getAttentionRect(zoomScale);
		tnode.type = ATTENTION;
		bool already = false;
		for (int i = 0; i < rnodes.size(); i++) {
			if (tnode.nindex == rnodes[i].nindex) {
				already = true;
				i = (int)rnodes.size();
			}
		}
		if (!already) {
			tnode.attentioninput = true;
			inputwoffset = pDoc->getConvInputOffset(brect, zoomScale);
			route = true;
		}
		else {
			tnode.attentionoutput = true;
			route = true;
		}
	}
	else if (pDoc->netData.isAssemblyLayer(index)) {
		route = false;
		CRect brect = pDoc->getAssemblyRect(index, zoomScale);
		tnode.type = ASSEMBLY;
		bool already = false;
		for (int i = 0; i < rnodes.size(); i++) {
			if (tnode.nindex == rnodes[i].nindex) {
				already = true;
				i = (int)rnodes.size();
			}
		}
		if (!already) {
			tnode.assemblyinput = true;
			inputwoffset = pDoc->getConvInputOffset(brect, zoomScale);
			route = true;
		}
		else {
			tnode.assemblyoutput = true;
			route = true;
		}
	}
	else if (pDoc->netData.isNetCluster(index)) {
		route = false;
		CRect brect = pDoc->getNetClusterRect(zoomScale);
		tnode.type = NETCLUSTER;
		bool already = false;
		for (int i = 0; i < rnodes.size(); i++) {
			if (tnode.nindex == rnodes[i].nindex) {
				already = true;
				i = (int)rnodes.size();
			}
		}
		if (!already) {
			tnode.netclusterinput = true;
			inputwoffset = pDoc->getConvInputOffset(brect, zoomScale);
			route = true;
		}
		else {
			tnode.netclusteroutput = true;
			route = true;
		}
	}
	else if (pDoc->netData.isDropout(index)) {
		route = false;
		CRect brect = pDoc->getNetClusterRect(zoomScale);
		tnode.type = DROPOUT;
		bool already = false;
		for (int i = 0; i < rnodes.size(); i++) {
			if (tnode.nindex == rnodes[i].nindex) {
				already = true;
				i = (int)rnodes.size();
			}
		}
		if (!already) {
			tnode.netdropoutinput = true;
			inputwoffset = pDoc->getConvInputOffset(brect, zoomScale);
			route = true;
		}
		else {
			tnode.netdropoutoutput = true;
			route = true;
		}
	}
	if (route) {
		tnode.mpoint = CPoint(pDoc->netData.getXDisp(tnode.nindex) + inputwoffset, pDoc->netData.getYDisp(tnode.nindex));
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
			//addErrorString(_T("adding ") + pDoc->netData.getID(tnode.nindex) + _T(" to rnodes"));
			rnodes.push_back(tnode);
		}
	}
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
	CAuxFunctions func;
	CNetDesignerDoc* pDoc = GetDocument();
	CString id1 = (CString)pDoc->netData.getID(nindex1).c_str();
	if (pDoc->netData.isConvLayer(nindex1)) {
		id1 = func.getConvNameFromConnectName(id1);
	}
	else if (pDoc->netData.isAttentionLayer(nindex1)) {
		id1 = func.getConvNameFromConnectName(id1);
	}
	else if (pDoc->netData.isAssemblyLayer(nindex1)) {
		id1 = func.getConvNameFromConnectName(id1);
	}
	else if (pDoc->netData.isNetCluster(nindex1)) {
		id1 = func.getConvNameFromConnectName(id1);
	}
	else if (pDoc->netData.isDropout(nindex1)) {
		id1 = func.getConvNameFromConnectName(id1);
	}
	//addErrorString(_T("id1 = ") + id1);
	CString id2 = (CString)pDoc->netData.getID(nindex2).c_str();
	if (pDoc->netData.isConvLayer(nindex2)) {
		id2 = func.getConvNameFromConnectName(id2);
	}
	else if (pDoc->netData.isAttentionLayer(nindex2)) {
		id2 = func.getConvNameFromConnectName(id2);
	}
	else if (pDoc->netData.isAssemblyLayer(nindex2)) {
		id2 = func.getConvNameFromConnectName(id2);
	}
	else if (pDoc->netData.isNetCluster(nindex2)) {
		id2 = func.getConvNameFromConnectName(id2);
	}
	else if (pDoc->netData.isDropout(nindex2)) {
		id2 = func.getConvNameFromConnectName(id2);
	}
	//addErrorString(_T("id2 = ") + id2);
	if (connections.size() > 0) {
		bool ck1 = false;
		bool ck2 = false;
		for (int i = 0; i < connections.size(); i++) {
			if (id1 == connections[i].input_node) {
				for (int j = 0; j < connections[i].output_nodes.size(); j++) {
					if (id2 == connections[i].output_nodes[j]) {
						//addErrorString(_T("connected"));
						return(true);
					}
				}
				ck1 = true;
			}
			else if (id2 == connections[i].input_node) {
				for (int j = 0; j < connections[i].output_nodes.size(); j++) {
					if (id1 == connections[i].output_nodes[j]) {
						//addErrorString(_T("connected"));
						return(true);
					}
				}
				ck2 = true;
			}
			if (ck1 && ck2) {
				//addErrorString(_T("not connected"));
				return(false);
			}
		}
	}
	//addErrorString(_T("not connected"));
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

COLORREF CNetDesignerView::adjustColorBetween(COLORREF clr1, COLORREF clr2, float percent)
{
	int r2 = (int)(percent * (float)GetRValue(clr1) + ((1.0F - percent) * (float)GetRValue(clr2)));
	int g2 = (int)(percent * (float)GetGValue(clr1) + ((1.0F - percent) * (float)GetGValue(clr2)));
	int b2 = (int)(percent * (float)GetBValue(clr1) + ((1.0F - percent) * (float)GetBValue(clr2)));
	COLORREF rval = RGB(r2, g2, b2);
	return(rval);
}












































































































































