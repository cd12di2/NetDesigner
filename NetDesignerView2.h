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

// NetDesignerView.h : interface of the CNetDesignerView class
//

#pragma once

#define LIGHT		0
#define DARK		1
#define BLUE		3
#define COLOR		4

#define	SELECT_ALL			0
#define	SELECT_NODES		1
#define	SELECT_CONNECTIONS	2
#define	SELECT_OTHER		3

#define	DRAW_NORMAL			0
#define	DRAW_HIGHLIGHT		1
#define	DRAW_ERASE			2
#define	DRAW_INACTIVE		3

#define	MOVE_CAPTURE		0
#define	MOVE_MOVING			1
#define MOVE_CHECKING		2
#define	MOVE_DONE			3

#define	PASTE_WAITING		0
#define	PASTE_MOVING		1
#define	PASTE_DONE			2

# define M_PI				3.14159265358979323846  /* pi */

class CViewCustom : public CView
{

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

class CNetDesignerView : public CView
{
protected: // create from serialization only
	CNetDesignerView() noexcept;
	DECLARE_DYNCREATE(CNetDesignerView)

// Attributes
public:
	CNetDesignerDoc* GetDocument() const;

	bool netLoaded;
	bool set_initial_scroll;
	CString placing_component_type;

	vector<int> selectedComponents;

	NetData copyData;
	NetData newGroupData;

	int displayWidth;
	int displayHeight;
	int displayCenterX;
	int displayCenterY;

	float zoomScale;
	float bZoomScale;
	float pZoomScale;

	float print_width;
	float print_height;

	bool searchItemActive;

	int lVertScollPos;
	int lHorzScollPos;
	int dVertScollPos;
	int dHorzScollPos;
	int logicalWidth;
	int logicalHeight;

	int selection_type;

	int move_state;
	CPoint moveStartPoint;
	CPoint moveLastPoint;

	int paste_state;
	int past_mindex;
	CPoint pasteLastPoint;

	bool selection_box;
	bool selection_box_erase;
	CPoint selection_pt1;
	CPoint selection_pt2;

	bool connecting;
	bool connecting_erase;
	CPoint connecting_pt2;
	vector<CPoint> connectingPoints;
	vector<int> connectingComponents;

	bool routing;
	CPoint route_origin;
	float route_angle;

	int new_group;

	bool printing;

	// Colors
	COLORREF clr_solarized_base03 = 0x00362b00;
	COLORREF clr_solarized_base02 = 0x00423607;
	COLORREF clr_solarized_base01 = 0x00756e58;
	COLORREF clr_solarized_base00 = 0x00837b65;
	COLORREF clr_solarized_base0 = 0x00969483;
	COLORREF clr_solarized_base1 = 0x00a1a193;
	COLORREF clr_solarized_base2 = 0x00d5e8ee;
	COLORREF clr_solarized_base3 = 0x00e3f6fd;
	COLORREF clr_solarized_yellow = 0x000089b5;
	COLORREF clr_solarized_orange = 0x00164bcb;
	COLORREF clr_solarized_orange2 = 0x000944ac;
	COLORREF clr_solarized_red = 0x002f32dc;
	COLORREF clr_solarized_magenta = 0x008236d3;
	COLORREF clr_solarized_violet = 0x00c4716c;
	COLORREF clr_solarized_blue = 0x00d28b26;
	COLORREF clr_solarized_cyan = 0x0098a12a;
	COLORREF clr_solarized_green = 0x00009985;

	COLORREF yamahablue = 0x00822d0a;
	COLORREF bluejeans = 0x00edcc58;
	COLORREF alcroeggshell = 0x00a7c4d2;
	COLORREF grayinactivecaption = 0x00b8b8b8;
	COLORREF grayinactivecaption2 = 0x00484848;
	COLORREF aliceblue = 0x00fef9f0;
	COLORREF antiflashwhite = 0x00f4f3f2;

	COLORREF clr_back = clr_solarized_base2;
	COLORREF clr_grid = clr_solarized_base01;
	COLORREF clr_outnode = clr_solarized_blue;
	COLORREF clr_hiddennode1 = clr_solarized_cyan;
	COLORREF clr_hiddennode4 = clr_solarized_green;
	int r1 = GetRValue(clr_hiddennode1) + (1 * (GetRValue(clr_hiddennode4) - GetRValue(clr_hiddennode1))) / 3;
	int g1 = GetGValue(clr_hiddennode1) + (1 * (GetGValue(clr_hiddennode4) - GetGValue(clr_hiddennode1))) / 3;
	int b1 = GetBValue(clr_hiddennode1) + (1 * (GetBValue(clr_hiddennode4) - GetBValue(clr_hiddennode1))) / 3;
	COLORREF clr_hiddennode2 = RGB(r1,g1,b1);
	int r2 = GetRValue(clr_hiddennode1) + (2 * (GetRValue(clr_hiddennode4) - GetRValue(clr_hiddennode1))) / 3;
	int g2 = GetGValue(clr_hiddennode1) + (2 * (GetGValue(clr_hiddennode4) - GetGValue(clr_hiddennode1))) / 3;
	int b2 = GetBValue(clr_hiddennode1) + (2 * (GetBValue(clr_hiddennode4) - GetBValue(clr_hiddennode1))) / 3;
	COLORREF clr_hiddennode3 = RGB(r2, g2, b2);
	//int r3 = GetRValue(clr_hiddennode1) + (3 * (GetRValue(clr_hiddennode5) - GetRValue(clr_hiddennode1))) / 4;
	//int g3 = GetGValue(clr_hiddennode1) + (3 * (GetGValue(clr_hiddennode5) - GetGValue(clr_hiddennode1))) / 4;
	//int b3 = GetBValue(clr_hiddennode1) + (3 * (GetBValue(clr_hiddennode5) - GetBValue(clr_hiddennode1))) / 4;
	COLORREF clr_hiddennode5 = clr_solarized_violet;
	COLORREF clr_hiddennode6 = adjustColorByPercent(clr_solarized_red, -30);
	COLORREF clr_connect = clr_solarized_yellow;
	COLORREF clr_input = clr_solarized_orange2;
	COLORREF clr_buffer = clr_solarized_yellow;
	//COLORREF clr_data = clr_solarized_orange;
	COLORREF clr_text = clr_solarized_base03;
	COLORREF clr_inactive = grayinactivecaption;

	// Light colors
	COLORREF clr_ltback = clr_solarized_base2;
	COLORREF clr_ltgrid = clr_solarized_base01;
	COLORREF clr_ltoutnode = clr_solarized_blue;
	COLORREF clr_lthiddennode1 = adjustColorByPercent(clr_solarized_cyan, 0);// clr_solarized_cyan;
	COLORREF clr_lthiddennode4 = adjustColorByPercent(clr_solarized_green, -40);// clr_solarized_green;
	int rl1 = GetRValue(clr_lthiddennode1) + (1 * (GetRValue(clr_lthiddennode4) - GetRValue(clr_lthiddennode1))) / 3;
	int gl1 = GetGValue(clr_lthiddennode1) + (1 * (GetGValue(clr_lthiddennode4) - GetGValue(clr_lthiddennode1))) / 3;
	int bl1 = GetBValue(clr_lthiddennode1) + (1 * (GetBValue(clr_lthiddennode4) - GetBValue(clr_lthiddennode1))) / 3;
	COLORREF clr_lthiddennode2 = RGB(rl1, gl1, bl1);
	int rl2 = GetRValue(clr_lthiddennode1) + (2 * (GetRValue(clr_lthiddennode4) - GetRValue(clr_lthiddennode1))) / 3;
	int gl2 = GetGValue(clr_lthiddennode1) + (2 * (GetGValue(clr_lthiddennode4) - GetGValue(clr_lthiddennode1))) / 3;
	int bl2 = GetBValue(clr_lthiddennode1) + (2 * (GetBValue(clr_lthiddennode4) - GetBValue(clr_lthiddennode1))) / 3;
	COLORREF clr_lthiddennode3 = RGB(rl2, gl2, bl2);
	//int rl3 = GetRValue(clr_lthiddennode1) + (3 * (GetRValue(clr_lthiddennode5) - GetRValue(clr_lthiddennode1))) / 4;
	//int gl3 = GetGValue(clr_lthiddennode1) + (3 * (GetGValue(clr_lthiddennode5) - GetGValue(clr_lthiddennode1))) / 4;
	//int bl3 = GetBValue(clr_lthiddennode1) + (3 * (GetBValue(clr_lthiddennode5) - GetBValue(clr_lthiddennode1))) / 4;
	//COLORREF clr_lthiddennode4 = RGB(rl3, gl3, bl3);
	COLORREF clr_lthiddennode5 = clr_solarized_violet;
	COLORREF clr_lthiddennode6 = adjustColorByPercent(clr_solarized_red, -30);
	COLORREF clr_ltconnect = clr_solarized_base1;
	COLORREF clr_ltinput = clr_solarized_orange2;
	COLORREF clr_ltbuffer = clr_solarized_yellow;
	COLORREF clr_lttext = clr_solarized_base03;
	COLORREF clr_ltinactive = grayinactivecaption;

	// Blue colors
	COLORREF clr_blback = clr_solarized_base3;
	COLORREF clr_blgrid = clr_solarized_base01;
	COLORREF clr_bloutnode = clr_solarized_blue;
	COLORREF clr_blhiddennode1 = adjustColorByPercent(clr_solarized_cyan, 0);// clr_solarized_cyan;
	COLORREF clr_blhiddennode4 = adjustColorByPercent(clr_solarized_green, -40);// clr_solarized_green;
	int rb1 = GetRValue(clr_blhiddennode1) + (1 * (GetRValue(clr_blhiddennode4) - GetRValue(clr_blhiddennode1))) / 3;
	int gb1 = GetGValue(clr_blhiddennode1) + (1 * (GetGValue(clr_blhiddennode4) - GetGValue(clr_blhiddennode1))) / 3;
	int bb1 = GetBValue(clr_blhiddennode1) + (1 * (GetBValue(clr_blhiddennode4) - GetBValue(clr_blhiddennode1))) / 3;
	COLORREF clr_blhiddennode2 = RGB(rb1, gb1, bb1);
	int rb2 = GetRValue(clr_blhiddennode1) + (2 * (GetRValue(clr_blhiddennode4) - GetRValue(clr_blhiddennode1))) / 3;
	int gb2 = GetGValue(clr_blhiddennode1) + (2 * (GetGValue(clr_blhiddennode4) - GetGValue(clr_blhiddennode1))) / 3;
	int bb2 = GetBValue(clr_blhiddennode1) + (2 * (GetBValue(clr_blhiddennode4) - GetBValue(clr_blhiddennode1))) / 3;
	COLORREF clr_blhiddennode3 = RGB(rb2, gb2, bb2);
	//int rb3 = GetRValue(clr_blhiddennode1) + (3 * (GetRValue(clr_blhiddennode5) - GetRValue(clr_blhiddennode1))) / 4;
	//int gb3 = GetGValue(clr_blhiddennode1) + (3 * (GetGValue(clr_blhiddennode5) - GetGValue(clr_blhiddennode1))) / 4;
	//int bb3 = GetBValue(clr_blhiddennode1) + (3 * (GetBValue(clr_blhiddennode5) - GetBValue(clr_blhiddennode1))) / 4;
	//COLORREF clr_blhiddennode4 = RGB(rb3, gb3, bb3);
	COLORREF clr_blhiddennode5 = clr_solarized_violet;
	COLORREF clr_blhiddennode6 = adjustColorByPercent(clr_solarized_red, -30);
	COLORREF clr_blconnect = clr_solarized_base1;
	COLORREF clr_blinput = clr_solarized_orange2;
	COLORREF clr_blbuffer = clr_solarized_yellow;
	COLORREF clr_bltext = clr_solarized_base03;
	COLORREF clr_blinactive = grayinactivecaption;

	// Dark colors
	COLORREF clr_dkback = clr_solarized_base03;
	COLORREF clr_dkgrid = clr_solarized_base00;
	COLORREF clr_dkoutnode = clr_solarized_blue;
	COLORREF clr_dkhiddennode1 = adjustColorByPercent(clr_solarized_cyan, 0);// clr_solarized_cyan;
	COLORREF clr_dkhiddennode4 = adjustColorByPercent(clr_solarized_green, -40);// clr_solarized_green;
	int rd1 = GetRValue(clr_dkhiddennode1) + (1 * (GetRValue(clr_dkhiddennode4) - GetRValue(clr_dkhiddennode1))) / 3;
	int gd1 = GetGValue(clr_dkhiddennode1) + (1 * (GetGValue(clr_dkhiddennode4) - GetGValue(clr_dkhiddennode1))) / 3;
	int bd1 = GetBValue(clr_dkhiddennode1) + (1 * (GetBValue(clr_dkhiddennode4) - GetBValue(clr_dkhiddennode1))) / 3;
	COLORREF clr_dkhiddennode2 = RGB(rd1, gd1, bd1);
	int rd2 = GetRValue(clr_dkhiddennode1) + (2 * (GetRValue(clr_dkhiddennode4) - GetRValue(clr_dkhiddennode1))) / 3;
	int gd2 = GetGValue(clr_dkhiddennode1) + (2 * (GetGValue(clr_dkhiddennode4) - GetGValue(clr_dkhiddennode1))) / 3;
	int bd2 = GetBValue(clr_dkhiddennode1) + (2 * (GetBValue(clr_dkhiddennode4) - GetBValue(clr_dkhiddennode1))) / 3;
	COLORREF clr_dkhiddennode3 = RGB(rd2, gd2, bd2);
	//int rd3 = GetRValue(clr_dkhiddennode1) + (3 * (GetRValue(clr_dkhiddennode5) - GetRValue(clr_dkhiddennode1))) / 4;
	//int gd3 = GetGValue(clr_dkhiddennode1) + (3 * (GetGValue(clr_dkhiddennode5) - GetGValue(clr_dkhiddennode1))) / 4;
	//int bd3 = GetBValue(clr_dkhiddennode1) + (3 * (GetBValue(clr_dkhiddennode5) - GetBValue(clr_dkhiddennode1))) / 4;
	//COLORREF clr_dkhiddennode4 = RGB(rd3, gd3, bd3);
	COLORREF clr_dkhiddennode5 = clr_solarized_violet;
	COLORREF clr_dkhiddennode6 = adjustColorByPercent(clr_solarized_red, -50);
	COLORREF clr_dkconnect = clr_solarized_base01;
	COLORREF clr_dkinput = clr_solarized_orange2;
	COLORREF clr_dkbuffer = clr_solarized_yellow;
	COLORREF clr_dktext = clr_solarized_base2;
	COLORREF clr_dkinactive = grayinactivecaption2;

	// Metal colors
	COLORREF clr_mtloutnode = 0x00857060;
	COLORREF clr_mtlhiddennode1 = adjustColorByPercent(0x00b5ceb2, 0);// 0x00b5ceb2;
	COLORREF clr_mtlhiddennode4 = adjustColorByPercent(0x00758060, -40);// 0x00758060;
	int rm1 = GetRValue(clr_mtlhiddennode1) + (1 * (GetRValue(clr_mtlhiddennode4) - GetRValue(clr_mtlhiddennode1))) / 3;
	int gm1 = GetGValue(clr_mtlhiddennode1) + (1 * (GetGValue(clr_mtlhiddennode4) - GetGValue(clr_mtlhiddennode1))) / 3;
	int bm1 = GetBValue(clr_mtlhiddennode1) + (1 * (GetBValue(clr_mtlhiddennode4) - GetBValue(clr_mtlhiddennode1))) / 3;
	COLORREF clr_mtlhiddennode2 = RGB(rm1, gm1, bm1);
	int rm2 = GetRValue(clr_mtlhiddennode1) + (2 * (GetRValue(clr_mtlhiddennode4) - GetRValue(clr_mtlhiddennode1))) / 3;
	int gm2 = GetGValue(clr_mtlhiddennode1) + (2 * (GetGValue(clr_mtlhiddennode4) - GetGValue(clr_mtlhiddennode1))) / 3;
	int bm2 = GetBValue(clr_mtlhiddennode1) + (2 * (GetBValue(clr_mtlhiddennode4) - GetBValue(clr_mtlhiddennode1))) / 3;
	COLORREF clr_mtlhiddennode3 = RGB(rm2, gm2, bm2);
	//int rm3 = GetRValue(clr_mtlhiddennode1) + (3 * (GetRValue(clr_mtlhiddennode5) - GetRValue(clr_mtlhiddennode1))) / 4;
	//int gm3 = GetGValue(clr_mtlhiddennode1) + (3 * (GetGValue(clr_mtlhiddennode5) - GetGValue(clr_mtlhiddennode1))) / 4;
	//int bm3 = GetBValue(clr_mtlhiddennode1) + (3 * (GetBValue(clr_mtlhiddennode5) - GetBValue(clr_mtlhiddennode1))) / 4;
	//COLORREF clr_mtlhiddennode4 = RGB(rm3, gm3, bm3);
	COLORREF clr_mthiddennode5 = clr_solarized_base2;
	COLORREF clr_mthiddennode6 = clr_solarized_base3;
	COLORREF clr_mtlinput = 0x003373b8;
	COLORREF clr_mtlbuffer = 0x0058b3c5;


	// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CNetDesignerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void initScroll();
	void setScrollInfo();
	bool needVerticalScroll();
	bool needHorizontalScroll();
	void EnableVerticalScroll(bool enable = true);
	void EnableHorizontalScroll(bool enable = true);
	void showSelectedomponents(bool all = false);

	void addErrorString(CString err);
	void addPanel1String(CString msg, CPoint pt = CPoint(0, 0));

	void clearEditor();
		
	void OnEditCut(CPoint pt);
	void OnEditPaste(CPoint pt);
	bool isPtOnScreen(CPoint pt);
	bool isPtOnScreen2(CPoint pt);

	CPoint DPtoLP(CPoint dpt);
	CPoint LPtoDP(CPoint lpt);

	void connectingDrawLine(CPoint point);
	void connectingEraseLine();

	void checkZoom();

	void OnDraw();
	void SetColors();
	void UpdateDrawingData();
	void UpdatePrintingData();
	void DrawGrid(CDC* pDC);
	void DrawComponents(CDC* pDC);
	void DrawNode(CDC* pDC, int x, int y, CString type, CString func, float scale, int mode, bool gdi_not);
	void DrawNode(CDC* pDC, int index, float scale, int mode, bool gdi_not);
	void DrawNode(int index, float scale, int mode, bool gdi_not);
	void DrawConnect(CDC* pDC, CPoint pt1, CPoint pt2, float scale, int mode = DRAW_NORMAL, bool shorten = true);
	void DrawConnect(CDC* pDC, int index, float scale, int mode = DRAW_NORMAL, bool shorten = true);
	void DrawConnect(int index, float scale, int mode = DRAW_NORMAL, bool shorten = true);
	void DrawDataSource(CDC* pDC, int x, int y, CString filepath, float scale, int mode);
	void DrawDataSource(CDC* pDC, int index, float scale, int mode = DRAW_NORMAL);
	void DrawDataSource(int index, float scale, int mode = DRAW_NORMAL);
	void DrawArrow(CDC* pDC, int x, int y, float angle);
	void DrawArrow(int x, int y, float angle);
	void updateArrow(CPoint dppoint);
	CPoint rotatePoint(CPoint origin, CPoint point, float angle);

	void drawCircle(CDC* pDC, CRect mrect, COLORREF mcolor, bool r2_not, bool gdi_not);
	void drawLine(CDC* pDC, CPoint pt1, CPoint pt2, int width, COLORREF mcolor, bool r2_not = false);

	bool checkValidSelection(CPoint point);
	bool checkIfNodeSelected();
	int getCenterNodeOfCopiedNodes();
	CRect getSelectedComponentsLogicalRectangle(bool nodes_only = false);
	CRect getAllComponentsLogicalRectangle();
	void drawSelectedComponents(int mode = DRAW_NORMAL);
	void moveSelectedItems(CPoint frompt, CPoint topt);
	void finalMoveSelectedItems(CPoint topt);
	void moveCopiedItems(int snode, CPoint frompt, CPoint topt);
	void drawCopiedComponents(CPoint spt, int snode, int mode);
	bool selectComponent(CPoint point, bool add = false);
	bool selectComponent(int index, bool add = false);
	void selectComponent(CString label);
	void endSelect();
	void updateProperties();
	void updateMovedProperties(CPoint frompt, CPoint topt);
	void updatePastedProperties(CPoint spt, int snode);
	void startPlaceComponent(CString tool);
	void escapePlaceComponent();
	bool endPlaceComponent(CPoint point);
	void finalizeNetPlacement();

	void drawNewGroupComponents(CPoint spt, int snode, int mode);
	void moveNewGroupItems(int snode, CPoint frompt, CPoint topt);

	bool isNodeAlreadyHere(CPoint here);
	bool isConnectAlreadyHere(CPoint here);

	CPoint applyGrid(CPoint pt);
	int applyGridX(int x);
	int applyGridY(int y);

	void resetZoomAndScrolls(float zoom, int hscroll, int vscroll);

	void addStragglingConnectionsToSelectedComponents();
	void deleteStragglingConnectionsFromSelectedComponents();

	void deleteSelectedComponents();
	void copySelectedComponents();

	void autoRoute();
	bool testNodeRouteOrder(CPoint newpoint, CPoint checkpoint);
	bool testNodeRouteGroup(CPoint point1, CPoint point2);
	bool areNodesConnected(int nindex1, int nindex2);

	bool checkSelectedNodesAreTrainable();

	COLORREF adjustColorByPercent(COLORREF clr, float percent);


protected:

// Generated message map functions
protected:
	virtual void OnInitialUpdate();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:

	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

	afx_msg void OnGuideSettings();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	afx_msg void OnSnap();
	afx_msg void OnGrid();
	afx_msg void OnUpdateSnap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGrid(CCmdUI* pCmdUI);

	afx_msg void On100Percent();
	afx_msg void OnZoomFit();
	afx_msg void OnZoomSelection();

	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnRedraw();

	afx_msg void OnTrain();
	afx_msg void OnUpdateTrain(CCmdUI* pCmdUI);
	afx_msg void OnStopTrain();
	afx_msg void OnUpdateStopTrain(CCmdUI* pCmdUI);
	afx_msg void OnVerifyNet();

	afx_msg void OnEditFind();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnZoomOut();

	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);

	afx_msg void OnEditMove();
	afx_msg void OnUpdateEditMove(CCmdUI* pCmdUI);

	afx_msg void OnSelectAll();
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnSelectNodes();
	afx_msg void OnUpdateSelectNodes(CCmdUI* pCmdUI);
	afx_msg void OnSelectConnects();
	afx_msg void OnUpdateSelectConnects(CCmdUI* pCmdUI);
	afx_msg void OnSelectOther();
	afx_msg void OnUpdateSelectOther(CCmdUI* pCmdUI);

	afx_msg void OnOptions();

	afx_msg void OnHidden();
	afx_msg void OnOutput();
	afx_msg void OnConnect();
	afx_msg void OnDataInput();
	afx_msg void OnBuffer();
	afx_msg void OnCsvData();

	afx_msg void OnUpdateHidden(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDataInput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBuffer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCsvData(CCmdUI* pCmdUI);

	afx_msg void OnViewData();
	afx_msg void OnUpdateViewData(CCmdUI* pCmdUI);
	afx_msg void OnValidationSet();
	afx_msg void OnUpdateValidationSet(CCmdUI* pCmdUI);
	afx_msg void OnEquation();
	afx_msg void OnUpdateEquation(CCmdUI* pCmdUI);

	afx_msg void OnArrows();

	afx_msg void OnTimeHrs();

	afx_msg void OnEpochs();
	afx_msg void OnMinEpochs();
	afx_msg void OnErrorRate();
	afx_msg void OnDeltaError();
	afx_msg void OnUpdateRate();
	afx_msg void OnRandomize();
	afx_msg void OnUpdateRandomize(CCmdUI* pCmdUI);

	afx_msg void OnUpdateVerifyNet(CCmdUI* pCmdUI);

	afx_msg void OnOutputData();
	afx_msg void OnUpdateOutputData(CCmdUI* pCmdUI);

	afx_msg void OnL1norm();
	afx_msg void OnUpdateL1norm(CCmdUI* pCmdUI);
	afx_msg void OnL2norm();
	afx_msg void OnUpdateL2norm(CCmdUI* pCmdUI);

	afx_msg void OnDestroy();

	afx_msg void OnAutoroute();
	afx_msg void OnUpdateAutoroute(CCmdUI* pCmdUI);
	afx_msg void OnKmeans();
	afx_msg void OnUpdateKmeans(CCmdUI* pCmdUI);
	afx_msg void OnPca();
	afx_msg void OnUpdatePca(CCmdUI* pCmdUI);
	afx_msg void OnRandomSeed();
	afx_msg void OnUpdateRandomSeed(CCmdUI* pCmdUI);

	afx_msg void OnEnableTrain();
	afx_msg void OnUpdateEnableTrain(CCmdUI* pCmdUI);



	afx_msg void OnNodeAnalysis();
	afx_msg void OnUpdateNodeAnalysis(CCmdUI* pCmdUI);
	afx_msg void OnTest();
	afx_msg void OnUpdateTest(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // debug version in NetDesignerView.cpp
inline CNetDesignerDoc* CNetDesignerView::GetDocument() const
   { return reinterpret_cast<CNetDesignerDoc*>(m_pDocument); }
#endif

