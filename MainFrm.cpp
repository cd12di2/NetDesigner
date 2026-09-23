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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"

#include "MainFrm.h"
#include "CMFCVisualManagerCustom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)
IMPLEMENT_DYNCREATE(CMFCVisualManagerCustom, CMFCVisualManagerOffice2007);


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_COLORFUL, ID_VIEW_APPLOOK_COLORFUL, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_COLORFUL, ID_VIEW_APPLOOK_COLORFUL, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_TOOLS_PANEL, &CMainFrame::OnToolsPanel)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PANEL, &CMainFrame::OnUpdateToolsPanel)
	ON_COMMAND(ID_OUTPUT_PANEL, &CMainFrame::OnOutputPanel)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_PANEL, &CMainFrame::OnUpdateOutputPanel)
	ON_COMMAND(ID_NODE_VIEW, &CMainFrame::OnNodeView)
	ON_UPDATE_COMMAND_UI(ID_NODE_VIEW, &CMainFrame::OnUpdateNodeView)
	ON_COMMAND(ID_PROPERTIES, &CMainFrame::OnProperties)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CMainFrame::OnUpdateProperties)
	ON_COMMAND(ID_GRAPH_TRAIN_ERROR, &CMainFrame::OnGraphTrainError)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TRAIN_ERROR, &CMainFrame::OnUpdateGraphTrainError)
	ON_COMMAND(ID_GRAPH_OUTPUTS, &CMainFrame::OnGraphOutputs)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_OUTPUTS, &CMainFrame::OnUpdateGraphOutputs)
	ON_COMMAND(ID_CLUSTER_GRAPH, &CMainFrame::OnClusterGraph)
	ON_UPDATE_COMMAND_UI(ID_CLUSTER_GRAPH, &CMainFrame::OnUpdateClusterGraph)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
	//m_wndRibbonBar.delQAT();
	CSize buttonSize(45, 48);
	m_wndRibbonBar.SetApplicationButton(&m_MainButton, buttonSize);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1 = _T("Pane11234567890123456789012345678901234567890123456789012345678901234567890");
	//CString strTitlePane1 = _T("123456789012345678901234567890123567890");
	CString strTitlePane2 = _T("Panel12345");
	m_ribbonSlider.SetZoomButtons(true);
	m_ribbonSlider.SetPos(100, TRUE);
	m_ribbonSlider.SetRange(10, 200);

	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddElement(&m_progress_ctrl, _T(""), true);
	m_wndStatusBar.AddExtendedElement(&m_ribbonSlider, _T(""), true);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	m_progress_ctrl.SetVisible(false);
	m_progress_ctrl.SetRange(0, 100);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndNodeView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndNodeView);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndToolbox.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolbox);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	m_wndGraph.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndGraph);
	m_wndClusterGraph.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndClusterGraph);
	m_wndData.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndData);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);


	return 0;
}

void CMainFrame::updateLook()
{
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerCustom));
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerCustom));
	CMFCVisualManagerCustom* pVisMan = (CMFCVisualManagerCustom*)CMFCVisualManager::GetInstance();
	if (theme == DARK) {
		//pVisMan->setDark(true);
		theApp.m_nAppLook = ID_VIEW_APPLOOK_OFF_2007_SILVER;
	}
	else if (theme == BLUE) {
		//pVisMan->setBlue(true);
		theApp.m_nAppLook = ID_VIEW_APPLOOK_WINDOWS_7;
	}
	else if (theme == COLOR) {
		//pVisMan->setColor(true);
		theApp.m_nAppLook = ID_VIEW_APPLOOK_COLORFUL;
	}
	else {
		//pVisMan->setDark(false);
		theApp.m_nAppLook = ID_VIEW_APPLOOK_LIGHT;
	}
	//pVisMan->OnUpdateSystemColors();
	
	OnApplicationLook(theApp.m_nAppLook);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create file view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndNodeView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Node View window\n");
		return FALSE; // failed to create
	}
	m_wndNodeView.m_bDisableAnimation = true;

	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}
	m_wndOutput.m_bDisableAnimation = true;

	// Create toolbar window
	CString strToolboxWnd;
	bNameValid = strToolboxWnd.LoadString(IDS_TOOLBOX_WND);
	ASSERT(bNameValid);
	if (!m_wndToolbox.Create(strToolboxWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_TOOLBARWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Toolbox\n");
		return FALSE; // failed to create
	}
	m_wndToolbox.m_bDisableAnimation = true;

	// Create graph window
	CString strGraphWnd = _T("Train Graph");
	if (!m_wndGraph.Create(strGraphWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_GRAPHWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Graph Window\n");
		return FALSE; // failed to create
	}
	m_wndGraph.m_bDisableAnimation = true;

	// Create cluster graph window
	CString strClusterGraphWnd = _T("Pattern Graph");
	if (!m_wndClusterGraph.Create(strClusterGraphWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLUSTERGRAPHWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Pattern Graph Window\n");
		return FALSE; // failed to create
	}
	m_wndClusterGraph.m_bDisableAnimation = true;

	// Create input data window
	int min_input_data_wnd_width = 500;
	int min_input_data_wnd_height = 340;
	CString strInputDataWnd = _T("  Select Input Data");
	if (!m_wndData.Create(strInputDataWnd, this, CRect(0, 0, min_input_data_wnd_width, min_input_data_wnd_height), TRUE, ID_VIEW_INPUT_DATAWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Input Data Window\n");
		return FALSE; // failed to create
	}
	m_wndData.m_bDisableAnimation = true;
	m_wndData.SetMinSize(CPoint(min_input_data_wnd_width, min_input_data_wnd_height));

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}
	m_wndProperties.m_bDisableAnimation = true;

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndNodeView.SetIcon(hFileViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hToolboxBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_TOOLBOX_VIEW_HC : IDI_TOOLBOX_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndToolbox.SetIcon(hToolboxBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	HICON hGraphBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_GRAPH_VIEW_HC : IDI_GRAPH_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndGraph.SetIcon(hGraphBarIcon, FALSE);

	HICON hClusterGraphBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_GRAPH_VIEW_HC : IDI_GRAPH_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClusterGraph.SetIcon(hClusterGraphBarIcon, FALSE);

	HICON hInputDataBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndData.SetIcon(hInputDataBarIcon, FALSE);

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	//setTheme(BLUE);

	/*switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:*/
		//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		/*break;

	//case ID_VIEW_APPLOOK_WINDOWS_7:
		//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		//CDockingManager::SetDockingMode(DT_SMART);
		//m_wndRibbonBar.SetWindows7Look(TRUE);
		//break;

	default:*/

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerCustom));
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerCustom));
		CMFCVisualManagerCustom* pVisMan = (CMFCVisualManagerCustom*)CMFCVisualManager::GetInstance();

		if (theApp.m_nAppLook == ID_VIEW_APPLOOK_OFF_2007_SILVER) {
			pVisMan->setDark(true);
			setTheme(DARK);
			m_wndGraph.setTheme(DARK);
			m_wndClusterGraph.setTheme(DARK);
		}
		else if (theApp.m_nAppLook == ID_VIEW_APPLOOK_WINDOWS_7) {
			pVisMan->setBlue(true);
			setTheme(BLUE);
			m_wndGraph.setTheme(BLUE);
			m_wndClusterGraph.setTheme(BLUE);
		}
		else if (theApp.m_nAppLook == ID_VIEW_APPLOOK_COLORFUL) {
			pVisMan->setColor(true);
			setTheme(COLOR);
			m_wndGraph.setTheme(COLOR);
			m_wndClusterGraph.setTheme(COLOR);
		}
		else {
			pVisMan->setDark(false);
			setTheme(LIGHT);
			m_wndGraph.setTheme(LIGHT);
			m_wndClusterGraph.setTheme(LIGHT);
		}
		pVisMan->OnUpdateSystemColors();
		
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(true);

		m_wndData.setColors(theme);

	//}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);

}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnToolsPanel()
{
	if (m_wndToolbox.IsVisible()) {
		m_wndToolbox.ShowPane(false, false, false);
	}
	else {
		m_wndToolbox.ShowPane(true, false, true);
	}
}

void CMainFrame::OnUpdateToolsPanel(CCmdUI* pCmdUI)
{
	if (m_wndToolbox.IsVisible() || m_wndToolbox.IsWindowVisible()) {
		pCmdUI->SetCheck(true);
	} else {
		pCmdUI->SetCheck(false);
	}
}

void CMainFrame::OnOutputPanel()
{
	if (m_wndOutput.IsVisible()) {
		m_wndOutput.ShowPane(false, false, false);
	}
	else {
		m_wndOutput.ShowPane(true, false, true);
	}
}

void CMainFrame::OnUpdateOutputPanel(CCmdUI* pCmdUI)
{
	if (m_wndOutput.IsVisible()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CMainFrame::OnNodeView()
{
	if (m_wndNodeView.IsVisible()) {
		m_wndNodeView.ShowPane(false, false, false);
	}
	else {
		m_wndNodeView.ShowPane(true, false, true);
	}
}

void CMainFrame::OnUpdateNodeView(CCmdUI* pCmdUI)
{
	if (m_wndNodeView.IsVisible()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CMainFrame::OnProperties()
{
	if (m_wndProperties.IsVisible()) {
		m_wndProperties.ShowPane(false, false, false);
	}
	else {
		m_wndProperties.ShowPane(true, false, true);
	}
}

void CMainFrame::OnUpdateProperties(CCmdUI* pCmdUI)
{
	if (m_wndProperties.IsVisible() || m_wndProperties.IsWindowVisible()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CMainFrame::OnGraphTrainError()
{
	if (m_wndGraph.IsVisible()) {
		m_wndGraph.ShowPane(false, false, false);
	}
	else {
		m_wndGraph.ShowPane(true, false, true);
	}
}

void CMainFrame::OnUpdateGraphTrainError(CCmdUI* pCmdUI)
{
	if (m_wndGraph.IsVisible() || m_wndGraph.IsWindowVisible()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CMainFrame::OnClusterGraph()
{
	if (m_wndClusterGraph.IsVisible()) {
		m_wndClusterGraph.ShowPane(false, false, false);
	}
	else {
		m_wndClusterGraph.ShowPane(true, false, true);
	}
}

void CMainFrame::OnUpdateClusterGraph(CCmdUI* pCmdUI)
{
	if (m_wndClusterGraph.IsVisible() || m_wndClusterGraph.IsWindowVisible()) {
		pCmdUI->SetCheck(true);
	}
	else {
		pCmdUI->SetCheck(false);
	}
}

void CMainFrame::OnDataView()
{
	m_wndData.ShowPane(true, false, true);
}

void CMainFrame::OnGraphOutputs()
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnUpdateGraphOutputs(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CMainFrame::setEpochs(CString data)
{
	CMFCRibbonEdit*medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EPOCHS));
	medit->SetEditText(data);
}

CString CMainFrame::getEpochs()
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EPOCHS));
	return(medit->GetEditText());
}

void CMainFrame::setMinEpochs(CString data)
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_MIN_EPOCHS));
	medit->SetEditText(data);
}

CString CMainFrame::getMinEpochs()
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_MIN_EPOCHS));
	return(medit->GetEditText());
}

void CMainFrame::setTrainTime(CString data)
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_TIME_HRS));
	medit->SetEditText(data);
}

CString CMainFrame::getTrainTime()
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_TIME_HRS));
	return(medit->GetEditText());
}

void CMainFrame::setTargetError(CString data)
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_ERROR_RATE));
	medit->SetEditText(data);
}

CString CMainFrame::getTargetError()
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_ERROR_RATE));
	return(medit->GetEditText());
}

void CMainFrame::setTargetDeltaError(CString data)
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_DELTA_ERROR));
	medit->SetEditText(data);
}

CString CMainFrame::getTargetDeltaError()
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_DELTA_ERROR));
	return(medit->GetEditText());
}

void CMainFrame::setUpdateRate(CString data)
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_UPDATE_RATE));
	medit->SetEditText(data);
}

CString CMainFrame::getUpdateRate()
{
	CMFCRibbonEdit* medit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_UPDATE_RATE));
	return(medit->GetEditText());
}

void CMainFrame::enableProgress(bool enable)
{
	if (enable) {
		m_progress_ctrl.SetVisible(true);
		SetStatusBarText1(_T("enable"));
	}
	else {
		m_progress_ctrl.SetVisible(false);
		SetStatusBarText1(_T("not enable"));
	}
	//m_wndStatusBar.GetElement(1)->SetVisible(enable);
	//m_progress_ctrl.SetVisible(enable);
	//m_wndStatusBar.GetElement(1)->NotifyCommand();
}

void CMainFrame::setProgress(int progress)
{
	m_progress_ctrl.SetPos(progress);
}


//*********************************************************************************
//*********************************************************************************

CMFCRibbonSliderCustom::CMFCRibbonSliderCustom() {
}

void CMFCRibbonSliderCustom::OnLButtonUp(CPoint point)
{
	CRect rect = GetRect();
	pos = GetPos();
	int x = point.x - rect.left;
	bool changed = false;
	if (x < 25) {
		pos-=5;
		if (pos < 10) {
			pos = 10;
		}
		changed = true;
	}
	else if (x > 115) {
		pos+=5;
		if (pos > 200) {
			pos = 200;
		}
		changed = true;
	}
	if (changed) {
		pos = setZoomOnFive(pos);
		SetPos(pos, true);
		updatePosition();
	}
}

void CMFCRibbonSliderCustom::OnMouseMove(CPoint point)
{
	CMFCRibbonSlider::OnMouseMove(point);
	pos = GetPos();
	pos = setZoomOnFive(pos);
	updatePosition();
}

void CMFCRibbonSliderCustom::OnDraw(CDC* dDc)
{
	//CMFCRibbonSlider::OnDraw(dDc);
	CRect rect = GetRect();
	int offset = 5;
	int midy = ((rect.top + rect.bottom) / 2) - 1;
	int buttonsize = (rect.right - rect.left) / 7;
	int halfbuttonsize = buttonsize / 2;
	int halfbuttonlinesize = halfbuttonsize - offset;
	int barstart = rect.left + buttonsize + offset;
	int barend = rect.right - buttonsize - offset;

	CPen pen(PS_SOLID, 1, GetGlobalData()->clrBarShadow);
	dDc->SelectObject(pen);
	dDc->MoveTo(CPoint(barstart, midy));
	dDc->LineTo(CPoint(barend, midy));
	dDc->MoveTo(CPoint((barend + barstart) / 2, midy - 3));
	dDc->LineTo(CPoint((barend + barstart) / 2, midy + 3));

	CPen pen2(PS_SOLID, 2, GetGlobalData()->clrBarShadow);
	dDc->SelectObject(pen2);
	dDc->MoveTo(CPoint(rect.left + halfbuttonsize - halfbuttonlinesize, midy));
	dDc->LineTo(CPoint(rect.left + halfbuttonsize + halfbuttonlinesize, midy));
	dDc->MoveTo(CPoint(rect.right - halfbuttonsize - halfbuttonlinesize, midy));
	dDc->LineTo(CPoint(rect.right - halfbuttonsize + halfbuttonlinesize, midy));
	dDc->MoveTo(CPoint(rect.right - halfbuttonsize + 1, midy + halfbuttonlinesize - 1));
	dDc->LineTo(CPoint(rect.right - halfbuttonsize + 1, midy - halfbuttonlinesize - 1));

	int cheight = (rect.top - rect.bottom) / 2;
	int cwidth = 2;
	pos = GetPos();
	int min = GetRangeMin();
	int max = GetRangeMax();
	float percent = float(pos - min) / float(max - min);
	int ipercent = barstart + 2 + (int)(percent * float(barend - barstart - 3));
	CRect rect2(ipercent-cwidth,midy+cheight/2,ipercent+cwidth, midy - cheight / 2);
	CBrush brush(GetGlobalData()->clrBarShadow);
	dDc->FillRect(rect2, &brush);
}

void CMFCRibbonSliderCustom::updatePosition()
{
	if (pos != previous) {
		previous = pos;
		CString msg = _T("");
		msg.Format(_T("%i%%"), pos);
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->SetStatusBarText2(msg);
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->OnDraw();
	}
}

float CMFCRibbonSliderCustom::getScale()
{
	pos = GetPos();
	float rval = float(pos) / 100;
	return(rval);
}

bool CMFCRibbonSliderCustom::setScale(float newscale)
{
	float mscale = newscale;
	float min = (float)(GetRangeMin() / 100);
	float max = (float)(GetRangeMax() / 100);
	if (mscale < min) {
		mscale = min;
	} 
	else if (mscale > max) {
		mscale = max;
	}
	pos = (int)(100 * mscale);
	pos = setZoomOnFive(pos);
	SetPos(pos);
	updatePosition();
	return(true);
}

int CMFCRibbonSliderCustom::setZoomOnFive(int pos)
{
	int tpos = 5 * (pos / 5);
	if ((pos - tpos) > 2.5) {
		tpos = tpos + 5;
	}
	return(tpos);
}


//*********************************************************************************
//*********************************************************************************

BOOL CRibbonBar::PreTranslateMessage(MSG* pMsg)
{
	int key = (int)pMsg->wParam;
	if (pMsg->message == WM_KEYDOWN) {
		if (key == VK_ESCAPE) {
			AfxMessageBox(_T("CRibbonBar::PreTranslateMessage"));
		}
	}
	return CMFCRibbonBar::PreTranslateMessage(pMsg);
}






