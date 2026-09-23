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


#include "CPreProcessor.h"
#include "CClusterAnalysisDlg.h"
#include "CAutomationDlg.h"


#define LIGHT		0
#define DARK		1
#define BLUE		3
#define COLOR		4

#define	SELECT_ALL			0
#define	SELECT_NODES		1
#define	SELECT_CONNECTIONS	2
#define	SELECT_OTHER		3
#define	SELECT_CONV_INPUT	4
#define	SELECT_CONV_OUTPUT	5

#define	DRAW_NORMAL			0
#define	DRAW_HIGHLIGHT		1
#define	DRAW_ERASE			2
#define	DRAW_INACTIVE		3
#define	DRAW_ZERO			4

#define	MOVE_CAPTURE		0
#define	MOVE_MOVING			1
#define MOVE_CHECKING		2
#define	MOVE_DONE			3

#define	PASTE_WAITING		0
#define	PASTE_MOVING		1
#define	PASTE_DONE			2

#define M_PI				3.14159265358979323846  /* pi */


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

	bool display;
	bool netLoaded;
	bool set_initial_scroll;
	CString placing_component_type;

	bool node_limit_message_displayed;

	//vector<int> selectedComponents;
	vector<CPoint> selectedComponentsPoints1;
	vector<CPoint> selectedComponentsPoints2;

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
	vector<CString> connectingComponents;

	bool routing;
	CPoint route_origin;
	float route_angle;

	typedef struct connectData {
		CString input_node;
		vector<CString> output_nodes;
	} CONNECTDATA;
	vector <connectData> connections;

	int new_group;

	bool printing;

	typedef struct sort_data {
		int nindex;
		int type;
		bool convinput;
		bool convoutput;
		bool attentioninput;
		bool attentionoutput;
		bool assemblyinput;
		bool assemblyoutput;
		bool netclusterinput;
		bool netclusteroutput;
		bool netdropoutinput;
		bool netdropoutoutput;
		CPoint mpoint;
		int group;
	} SORT_DATA;
	vector<sort_data> rnodes;

	CPreProcessor processDlg;
	CClusterAnalysisDlg clusterdlg;


	// Solarized
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

	// Gruvbox
	COLORREF dark0_hard = 0x0021201d; // #161819
	COLORREF dark0 = 0x00282828; // #1e1e1e
	COLORREF dark0_soft = 0x002f3032; // #262423
	COLORREF dark1 = 0x0036383c; // #2e2a29
	COLORREF dark2 = 0x00454950; // #3f3935
	COLORREF dark3 = 0x00545c66; // #534a42
	COLORREF dark4 = 0x00647c6f7c; // #685c51

	COLORREF gray_245 = 0x00748392;// #7f7061
	COLORREF gray_244 = 0x00928374;// #7f7061

	COLORREF light0_hard = 0x00d7f5f9;// #f8f4cd
	COLORREF light0 = 0x00c7f1fb;// #faeebb
	COLORREF light0_soft = 0x00bce5f2;// #efdfae
	COLORREF light1 = 0x00b2dbeb;// #e6d4a3
	COLORREF light2 = 0x00a1c4d5;// #cbb890
	COLORREF light3 = 0x0093aebd;// #af9f81
	COLORREF light4 = 0x008499a8;// #978771

	COLORREF bright_red = 0x003449fb;		// #f73028
	COLORREF bright_green = 0x00b8bb26;		// #aab01e
	COLORREF bright_yellow = 0x00fabd2f;	// #f7b125
	COLORREF bright_blue = 0x0083a598;		// #719586
	COLORREF bright_purple = 0x00d3869b;	// #c77089
	COLORREF bright_aqua = 0x008ec07c;		// #7db669
	COLORREF bright_orange = 0x00fe8019;	// #fb6a16

	COLORREF neutral_red = 0x00cc241d;		// #be0f17;
	COLORREF neutral_green = 0x001a9798;	// #868715;
	COLORREF neutral_yellow = 0x002199d7;	// #cc881a;
	COLORREF neutral_blue = 0x00458588;		// #377375;
	COLORREF neutral_purple = 0x00b16286;	// #a04b73;
	COLORREF neutral_aqua = 0x006a9d68;		// #578e57;
	COLORREF neutral_orange = 0x000e5dd6;	// #ca480e;

	COLORREF faded_red = 0x009d0006;		// #890009;
	COLORREF faded_green = 0x0079740e;		// #66620d;
	COLORREF faded_yellow = 0x00b57614;		// #a56311;
	COLORREF faded_blue = 0x00076678;		// #0e5365;
	COLORREF faded_purple = 0x008f3f71;		// #7b2b5e;
	COLORREF faded_aqua = 0x00427b58;		// #356a46;
	COLORREF faded_orange = 0x00af3a03;		// #9d2807;

	// Colors
	COLORREF yamahablue = 0x00822d0a;
	COLORREF bluejeans = 0x00edcc58;
	COLORREF alcroeggshell = 0x00a7c4d2;
	COLORREF grayinactivecaption = 0x00b8b8b8;
	COLORREF grayinactivecaption2 = 0x00484848;
	COLORREF aliceblue = 0x00fef9f0;
	COLORREF antiflashwhite = 0x00f4f3f2;

	COLORREF magenta = 0x00ff00a2;
	COLORREF metro5 = 0x000996f0;

	COLORREF chocolate = 0x001e69d2;
	COLORREF goldenrod = 0x0020a5da;

	COLORREF copper = 0x003373b8;
	COLORREF oxidizedcopper = 0x004c67a4;
	COLORREF potassium = 0x00acd4fb;
	COLORREF platinum = 0x00e2e4e5;

	// Visual Studio Dark Theme  -  https://learn.microsoft.com/en-us/visualstudio/extensibility/ux-guidelines/color-value-reference-for-visual-studio?view=vs-2022
	COLORREF vsd_default = 0x00302D2D;	
	COLORREF vsd_foreground1 = 0x00F1F1F1;
	COLORREF vsd_foreground2 = 0x00CC7A00;
	COLORREF vsd_background1 = 0x001C1B1B;
	COLORREF vsd_background2 = 0x00262525;
	COLORREF vsd_background3 = 0x00292929;
	COLORREF vsd_background4 = 0x00302D2D;
	COLORREF vsd_background5 = 0x00373333;
	COLORREF vsd_border1 = 0x001C1B1B;
	COLORREF vsd_border2 = 0x00262525;
	COLORREF vsd_border3 = 0x00403E3E;
	COLORREF vsd_border4 = 0x00463F3F;

	// Visual Studio Light Theme
	COLORREF vsl_default = 0x00F2EEEE;
	COLORREF vsl_foreground1 = 0x001E1E1E;
	COLORREF vsl_background1 = 0x00FCFCFC;
	COLORREF vsl_background2 = 0x00F5F5F5;
	COLORREF vsl_background3 = 0x00F5DEC9;
	COLORREF vsl_background4 = 0x00F2EEEE;
	COLORREF vsl_background5 = 0x00ECE8E7;
	COLORREF vsl_border1 = 0x00F5F5F5;
	COLORREF vsl_border2 = 0x00ECE8E7;
	COLORREF vsl_border3 = 0x00D8CECC;


	// Base colors
	COLORREF clr_back = adjustColorByPercent(clr_solarized_base2, -2);
	COLORREF clr_grid = adjustColorByPercent(clr_solarized_base01, -2);
	COLORREF clr_outnode = adjustColorByPercent(clr_solarized_blue, 5);
	COLORREF clr_hiddennode1 = adjustColorByPercent(clr_solarized_cyan, 5);
	COLORREF clr_hiddennode4 = adjustColorByPercent(clr_solarized_green, 5);
	COLORREF clr_hiddennode2 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.80F);
	COLORREF clr_hiddennode3 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.60F);
	COLORREF clr_hiddennode7 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.40F);
	COLORREF clr_hiddennode8 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.20F);
	COLORREF clr_hiddennode5 = adjustColorByPercent(clr_solarized_violet, 5);
	COLORREF clr_hiddennode6 = adjustColorByPercent(clr_solarized_red, -30);
	COLORREF clr_hiddennode9 = adjustColorByPercent(gray_244, -10);
	COLORREF clr_hiddennode10 = adjustColorByPercent(adjustColorBetween(clr_solarized_blue, clr_solarized_cyan, 0.40F), -10);
	COLORREF clr_hiddennode11 = adjustColorByPercent(metro5/*platinum*/, 0);
	COLORREF clr_hiddennode12 = adjustColorByPercent(bluejeans, 0);
	COLORREF clr_hiddennode13 = adjustColorByPercent(goldenrod, 0);
	COLORREF clr_connect = adjustColorByPercent(clr_solarized_yellow, 20);
	COLORREF clr_input = adjustColorByPercent(clr_solarized_orange2, 5);
	COLORREF clr_buffer = adjustColorByPercent(clr_solarized_yellow, 5);
	COLORREF clr_text = adjustColorByPercent(clr_solarized_base03, -5);
	COLORREF clr_inactive = grayinactivecaption;
	COLORREF clr_zero = adjustColorByPercent(aliceblue, -30);// light3;// light4;;

	// Light colors
	COLORREF clr_ltback = adjustColorByPercent(vsl_background1, 0);// adjustColorByPercent(clr_solarized_base3, 10);
	COLORREF clr_ltgrid = adjustColorByPercent(clr_ltback, -60); //adjustColorByPercent(clr_solarized_base01, -2);
	COLORREF clr_ltoutnode = adjustColorByPercent(clr_solarized_blue, 5);
	COLORREF clr_lthiddennode1 = adjustColorByPercent(clr_solarized_cyan, 10);
	COLORREF clr_lthiddennode4 = adjustColorByPercent(clr_solarized_green, 5);
	COLORREF clr_lthiddennode2 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.80F), 5);
	COLORREF clr_lthiddennode3 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.60F), 5);
	COLORREF clr_lthiddennode7 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.40F), 5);
	COLORREF clr_lthiddennode8 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.20F), 5);
	COLORREF clr_lthiddennode5 = adjustColorByPercent(clr_solarized_violet, 5);
	COLORREF clr_lthiddennode6 = adjustColorByPercent(clr_solarized_red, -10);
	COLORREF clr_lthiddennode9 = adjustColorByPercent(gray_244, -10);
	COLORREF clr_lthiddennode10 = adjustColorByPercent(adjustColorBetween(clr_solarized_blue, clr_solarized_cyan, 0.40F), -10);
	COLORREF clr_lthiddennode11 = adjustColorByPercent(bluejeans/*platinum*/, 10);
	COLORREF clr_lthiddennode12 = adjustColorByPercent(aliceblue, -30);
	COLORREF clr_lthiddennode13 = adjustColorByPercent(goldenrod, 0);
	COLORREF clr_ltconnect = adjustColorByPercent(clr_solarized_base1, 10);
	COLORREF clr_ltinput = adjustColorByPercent(neutral_orange, 20);
	COLORREF clr_ltbuffer = adjustColorByPercent(neutral_yellow, 20);
	COLORREF clr_lttext = adjustColorByPercent(clr_solarized_base03, -2);
	COLORREF clr_ltinactive = grayinactivecaption;
	COLORREF clr_ltzero = adjustColorByPercent(aliceblue, 0);// light3;// light4;;

	// Blue colors
	COLORREF clr_blback = adjustColorByPercent(clr_solarized_base3, -5);
	COLORREF clr_blgrid = adjustColorByPercent(clr_solarized_base01, -5);
	COLORREF clr_bloutnode = adjustColorByPercent(clr_solarized_blue, 5);
	COLORREF clr_blhiddennode1 = adjustColorByPercent(clr_solarized_cyan, 10);
	COLORREF clr_blhiddennode4 = adjustColorByPercent(clr_solarized_green, 5);
	COLORREF clr_blhiddennode2 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.80F);
	COLORREF clr_blhiddennode3 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.60F);
	COLORREF clr_blhiddennode7 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.40F);
	COLORREF clr_blhiddennode8 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.20F);
	COLORREF clr_blhiddennode5 = adjustColorByPercent(clr_solarized_violet, 5);
	COLORREF clr_blhiddennode6 = adjustColorByPercent(clr_solarized_red, -10);
	COLORREF clr_blhiddennode9 = adjustColorByPercent(gray_244, -10);
	COLORREF clr_blhiddennode10 = adjustColorByPercent(adjustColorBetween(clr_solarized_blue, clr_solarized_cyan, 0.40F), -10);
	COLORREF clr_blhiddennode11 = adjustColorByPercent(metro5/*bluejeans*//*platinum*/, -20);
	COLORREF clr_blhiddennode12 = adjustColorByPercent(aliceblue, 0);
	COLORREF clr_blhiddennode13 = adjustColorByPercent(goldenrod, 0);
	COLORREF clr_blconnect = adjustColorByPercent(clr_solarized_base1, 10);
	COLORREF clr_blinput = adjustColorByPercent(neutral_orange, 20);
	COLORREF clr_blbuffer = adjustColorByPercent(neutral_yellow, 20);
	COLORREF clr_bltext = adjustColorByPercent(clr_solarized_base03, -2);
	COLORREF clr_blinactive = grayinactivecaption;
	COLORREF clr_blzero = adjustColorByPercent(aliceblue, -30);// light3;// light4;;

	// Dark colors
	COLORREF clr_dkback = adjustColorByPercent(vsd_background1, 10); //adjustColorByPercent(clr_solarized_base03, -5);
	COLORREF clr_dkgrid = adjustColorByPercent(clr_dkback, 280); //adjustColorByPercent(clr_solarized_base00, -2);
	COLORREF clr_dkoutnode = adjustColorByPercent(clr_solarized_blue, -10);
	COLORREF clr_dkhiddennode1 = adjustColorByPercent(clr_solarized_cyan, -10);
	COLORREF clr_dkhiddennode4 = adjustColorByPercent(clr_solarized_green, -20);
	COLORREF clr_dkhiddennode2 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.80F), 10);
	COLORREF clr_dkhiddennode3 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.60F), 10);
	COLORREF clr_dkhiddennode7 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.40F), 10);
	COLORREF clr_dkhiddennode8 = adjustColorByPercent(adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.20F), 10);
	COLORREF clr_dkhiddennode5 = adjustColorByPercent(clr_solarized_violet, -10);
	COLORREF clr_dkhiddennode6 = adjustColorByPercent(adjustColorByPercent(clr_solarized_red, -40), 10);
	COLORREF clr_dkhiddennode9 = adjustColorByPercent(gray_244, -10);
	COLORREF clr_dkhiddennode10 = adjustColorByPercent(adjustColorBetween(clr_solarized_blue, clr_solarized_cyan, 0.40F), -10);
	COLORREF clr_dkhiddennode11 = adjustColorByPercent(bluejeans/*platinum*/, -10);
	COLORREF clr_dkhiddennode12 = adjustColorByPercent(aliceblue, -30);
	COLORREF clr_dkhiddennode13 = adjustColorByPercent(goldenrod, -20);
	COLORREF clr_dkconnect = adjustColorByPercent(clr_solarized_base01, -10);
	COLORREF clr_dkinput = adjustColorByPercent(neutral_orange, -10);
	COLORREF clr_dkbuffer = adjustColorByPercent(clr_solarized_yellow, -10);
	COLORREF clr_dktext = adjustColorByPercent(clr_solarized_base2, 2);
	COLORREF clr_dkinactive = grayinactivecaption2;
	COLORREF clr_dkzero = adjustColorByPercent(aliceblue, -30);// light3;// light4;

	// Metal colors
	COLORREF clr_mtloutnode = 0x00857060;
	COLORREF clr_mtlhiddennode1 = adjustColorByPercent(0x00b5ceb2, 0);
	COLORREF clr_mtlhiddennode4 = adjustColorByPercent(oxidizedcopper, 0);
	COLORREF clr_mtlhiddennode2 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.80F);// RGB(rm1, gm1, bm1);
	COLORREF clr_mtlhiddennode3 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.60F);// RGB(rm2, gm2, bm2);
	COLORREF clr_mthiddennode7 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.40F);// RGB(rm2, gm2, bm2);
	COLORREF clr_mthiddennode8 = adjustColorBetween(clr_hiddennode1, clr_hiddennode4, 0.20F);// RGB(rm2, gm2, bm2);
	COLORREF clr_mthiddennode5 = adjustColorByPercent(potassium, -20);
	COLORREF clr_mthiddennode6 = adjustColorByPercent(copper, -30);
	COLORREF clr_mthiddennode9 = adjustColorByPercent(gray_244, -20);
	COLORREF clr_mthiddennode10 = adjustColorByPercent(adjustColorBetween(clr_solarized_blue, clr_solarized_cyan, 0.40F), -10);
	COLORREF clr_mthiddennode11 = adjustColorByPercent(metro5/*platinum*/, -10);
	COLORREF clr_mthiddennode12 = adjustColorByPercent(platinum, 0);
	COLORREF clr_mthiddennode13 = adjustColorByPercent(goldenrod, 0);
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

	void addErrorString(string err);
	void addPanel1String(string msg, CPoint pt = CPoint(0, 0));
	void addPanel2String(string msg, CPoint pt = CPoint(0, 0));

	void addErrorString(CString err);
	void addPanel1String(CString msg, CPoint pt = CPoint(0, 0));
	void addPanel2String(CString msg, CPoint pt = CPoint(0, 0));

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
	void DrawConvolutions(bool redraw_spheres = true);

	void DrawNode(CDC* pDC, int x, int y, CString type, CString func, float scale, int mode, bool gdi_not);
	void DrawNode(CDC* pDC, int index, float scale, int mode, bool gdi_not);
	void DrawNode(int index, float scale, int mode, bool gdi_not);
	
	void DrawConnect(CDC* pDC, CPoint pt1, CPoint pt2, float scale, int mode = DRAW_NORMAL, bool shorten = true);
	void DrawConnect(CDC* pDC, int index, float scale, int mode = DRAW_NORMAL, bool shorten = true);
	void DrawConnect(int index, float scale, int mode = DRAW_NORMAL, bool shorten = true);
	void removeNonGrayConvConnections();

	void DrawConvolution(CDC* pDC, int x, int y, bool gray, convParameters params, vector<int> dimensions, vector<CPoint> pts, float scale, int mode, bool redraw_spheres = true, bool print = false);
	void DrawConvolution(CDC* pDC, int index, float scale, int mode, bool redraw_spheres = true, bool print = false);
	void DrawConvolution(int index, float scale, int mode, bool redraw_spheres = true);
	void DrawConvolutionInputOutputImage(CDC* pDC, int x, int y, convParameters params, float scale, bool gray, bool print);
	void DrawOutputLayerFromNodes(CDC* pDC, int x, int y, convParameters params, float scale, bool gray, bool print);
	void DrawConvolutionHiddenImage(CDC* pDC, int x, int y, float scale, bool gray, convParameters params, bool print);
	void DrawConvolutionInputOutputSound(CDC* pDC, int x, int y, convParameters params, float scale, bool print);
	void DrawDataConvolution(CDC* pDC, int x, int y, float scale, convParameters params, COLORREF clr, bool print);
	void DrawHiddenNet(CDC* pDC, int x, int y, float scale, convParameters params, COLORREF clr, bool print);
	void DrawInternalNetCluster(CDC* pDC, int x, int y, float scale, CRect brect, COLORREF clr, bool print);
	void DrawInternalDropout(CDC* pDC, int x, int y, float scale, CRect brect, COLORREF clr, bool print);
	COLORREF getColor(double value, bool special = false);

	void DrawAttention(CDC* pDC, int x, int y, float scale, int mode, bool self, bool print = false);
	void DrawAttention(CDC* pDC, int index, float scale, int mode, bool print = false);
	void DrawAttention(int index, float scale, int mode);

	void DrawAssembly2(CDC* pDC, int x, int y, int index, float scale, int mode, assembly_sub_component asub, bool print = false);
	void DrawAssembly(CDC* pDC, int index, float scale, int mode, assembly_sub_component asub, bool print = false);
	void DrawAssembly(int index, float scale, int mode, assembly_sub_component asub);
	void drawAssemblyInputOutputs(CDC* pDC, int x, int y, int index, float scale, int mode, assembly_sub_component asub);

	void DrawNetCluster(CDC* pDC, int x, int y, float scale, int mode, string omode, bool print = false);
	void DrawNetCluster(CDC* pDC, int index, float scale, int mode, bool print = false);
	void DrawNetCluster(int index, float scale, int mode);

	void DrawDropout(CDC* pDC, int x, int y, float scale, int mode, bool print = false);
	void DrawDropout(CDC* pDC, int index, float scale, int mode, bool print = false);
	void DrawDropout(int index, float scale, int mode);

	void DrawArrow(CDC* pDC, int x, int y, float angle);
	void DrawArrow(int x, int y, float angle);
	void updateArrow(CPoint dppoint);
	CPoint rotatePoint(CPoint origin, CPoint point, float angle);

	void drawCircle(CDC* pDC, CRect mrect, COLORREF mcolor, bool r2_not, bool gdi_not, bool cartoon_anyway = false);
	void drawOpenCircle(CDC* pDC, int x, int y, int myradius, COLORREF mclr, bool gdi_not);
	void draw3dCircle(CDC* pDC, int x, int y, int myradius, COLORREF mclr, bool gdi_not);
	void drawSquare(CDC* pDC, CRect mrect, COLORREF mcolor, bool r2_not, bool gdi_not);
	void drawLine(CDC* pDC, CPoint pt1, CPoint pt2, int width, COLORREF mcolor, bool r2_not = false);

	bool checkValidSelection(CPoint point);
	bool checkIfNodeSelected();
	int getCenterNodeOfCopiedNodes();
	CRect getSelectedComponentsLogicalRectangle(bool nodes_only = false);
	CRect getAllComponentsLogicalRectangle();
	void drawSelectedComponents(int mode);
	void moveSelectedItems(CPoint frompt, CPoint topt);
	void finalMoveSelectedItems(CPoint topt);
	void moveCopiedItems(int snode, CPoint frompt, CPoint topt);
	void drawCopiedComponents(CPoint spt, int snode, int mode);
	bool selectComponent(CPoint point, bool add, assembly_sub_component asub);
	bool selectComponent(int index, bool add);
	void selectComponent(CString label);
	void endSelect();
	void updateProperties();
	void updateMovedProperties(CPoint frompt, CPoint topt);
	void updatePastedProperties(CPoint spt, int snode);
	void startPlaceComponent(CString tool);
	void escapePlaceComponent();
	bool endPlaceComponent(CPoint point);
	void finalizeNetPlacement();

	void checkAssemblyEditable();
	int getAssemblyIndex();
	vector<string> getAssemblyNames();
	vector<int> getAssemblySubComponents(int sindex);
	assembly_sub_component checkSelectionAssemblyNode(CPoint point);

	void updateDataToEquation();
	void updateDataFromEquation();
	void clearEquation();

	void drawNewGroupComponents(CPoint spt, int snode, int mode);
	void moveNewGroupItems(int snode, CPoint frompt, CPoint topt);

	bool isNodeAlreadyHere(CPoint here);
	bool isConnectAlreadyHere(CPoint here);

	CPoint applyGrid(CPoint pt);
	int applyGridX(int x);
	int applyGridY(int y);

	vector<CPoint> convertValuesToPoints(vector<twoValues> locations);

	void resetZoomAndScrolls(float zoom, int hscroll, int vscroll);

	void addStragglingConnectionsToSelectedComponents();
	void deleteStragglingConnectionsFromSelectedComponents();

	void deleteSelectedComponents();
	void copySelectedComponents();

	void autoRoute();
	void addComponentToRouteOrder(int index);
	bool testNodeRouteOrder(CPoint newpoint, CPoint checkpoint);
	bool testNodeRouteGroup(CPoint point1, CPoint point2);
	bool areNodesConnected(int nindex1, int nindex2);

	bool checkSelectedNodesAreTrainable();
	bool checkSelectedNodesAreZeroizable();

	COLORREF adjustColorByPercent(COLORREF clr, float percent);
	COLORREF adjustColorBetween(COLORREF clr1, COLORREF clr2, float percent);

	string getInfo(int color_scale);


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

	afx_msg void OnShowConnects();
	afx_msg void OnUpdateShowConnects(CCmdUI* pCmdUI);

	afx_msg void OnOptions();

	afx_msg void OnHidden();
	afx_msg void OnOutput();
	afx_msg void OnConnect();
	afx_msg void OnDataInput();
	afx_msg void OnBuffer();
	afx_msg void OnCsvData();
	afx_msg void OnConvLayer();
	afx_msg void OnAttention();
	afx_msg void OnAttention2();
	afx_msg void OnNetCluster();
	afx_msg void OnSaveNetCluster();
	afx_msg void OnDropout();

	afx_msg void OnUpdateHidden(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDataInput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBuffer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCsvData(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvLayer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAttention(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAttention2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNetCluster(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveNetCluster(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDropout(CCmdUI* pCmdUI);

	afx_msg void OnViewData();
	afx_msg void OnUpdateViewData(CCmdUI* pCmdUI);
	afx_msg void OnValidationSet();
	afx_msg void OnUpdateValidationSet(CCmdUI* pCmdUI);
	afx_msg void OnEquation();
	afx_msg void OnUpdateEquation(CCmdUI* pCmdUI);

	afx_msg void OnEvaluateFile();
	afx_msg void OnUpdateEvaluateFile(CCmdUI* pCmdUI);

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

	afx_msg void OnEnableZeroOutput();
	afx_msg void OnUpdateEnableZeroOutput(CCmdUI* pCmdUI);

	afx_msg void OnSelectLayerData();
	afx_msg void OnUpdateSelectLayerData(CCmdUI* pCmdUI);

	afx_msg void OnNodeAnalysis();
	afx_msg void OnUpdateNodeAnalysis(CCmdUI* pCmdUI);
	afx_msg void OnTest();
	afx_msg void OnUpdateTest(CCmdUI* pCmdUI);

	afx_msg void OnGraphOutputs();
	afx_msg void OnUpdateGraphOutputs(CCmdUI* pCmdUI);

	afx_msg void OnAutosave();
	afx_msg void OnUpdateAutosave(CCmdUI* pCmdUI);

	afx_msg void OnInfo();

	afx_msg void OnClear();

	afx_msg void OnInputLayer();
	afx_msg void OnUpdateInputLayer(CCmdUI* pCmdUI);
	afx_msg void OnInsertHiddenLayers();
	afx_msg void OnUpdateInsertHiddenLayers(CCmdUI* pCmdUI);
	afx_msg void OnInsertOutputLayer();
	afx_msg void OnUpdateInsertOutputLayer(CCmdUI* pCmdUI);

	afx_msg void OnAutomation();
	afx_msg void OnUpdateAutomation(CCmdUI* pCmdUI);


	afx_msg void OnAssembly();
	afx_msg void OnUpdateAssembly(CCmdUI* pCmdUI);
	afx_msg void OnEditAssembly();
	afx_msg void OnUpdateEditAssembly(CCmdUI* pCmdUI);
	afx_msg void OnCloseAssembly();
	afx_msg void OnUpdateCloseAssembly(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in NetDesignerView.cpp
inline CNetDesignerDoc* CNetDesignerView::GetDocument() const
   { return reinterpret_cast<CNetDesignerDoc*>(m_pDocument); }
#endif

