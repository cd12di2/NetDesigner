
// NNetDesigner App for drawing, training, and testing neural networks

// NetDesigner.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "NetDesigner.h"
#include "MainFrm.h"

//#include <winnls.h>
#include <gdiplus.h>

#include "NetDesignerDoc.h"
#include "NetDesignerView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetDesignerApp

BEGIN_MESSAGE_MAP(CNetDesignerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CNetDesignerApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CNetDesignerApp construction

CNetDesignerApp::CNetDesignerApp() noexcept
{
	m_bHiColorIcons = TRUE;
	SetAppID(_T("NetDesigner.AppID.NoVersion"));
}

// The one and only CNetDesignerApp object
CNetDesignerApp theApp;

// CNetDesignerApp initialization

BOOL CNetDesignerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	CAuxFunctions func;
	if (!func.checkWindowsVersion()) {
		return(false);
	}
	if (!func.checkCountry()) {
		return(false);
	}

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(6);  // Load standard INI file options (including MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNetDesignerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CNetDesignerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	return TRUE;
}

int CNetDesignerApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CNetDesignerApp message handlers


// App command to run the dialog
void CNetDesignerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CNetDesignerApp customization load/save methods

void CNetDesignerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT2);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT3);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT4);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT5);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT6);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT7);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT8);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT9);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT10);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT11);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_GRAPH);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_CLUSTER_GRAPH);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CNetDesignerApp::LoadCustomState()
{
}

void CNetDesignerApp::SaveCustomState()
{
}

// CNetDesignerApp message handlers

BOOL CNetDesignerApp::OnIdle(LONG lCount)
{
	CNetDesignerDoc* pMyDoc = (CNetDesignerDoc*)((CFrameWnd*)AfxGetApp()->GetMainWnd())->GetActiveDocument();
	if (pMyDoc->training) {
		pMyDoc->iterateTrain();
		return CWinApp::OnIdle(0);
	}
	else if (pMyDoc->clustering) {
		pMyDoc->iterateClusterAnalysis();
		return CWinApp::OnIdle(0);
	}
	return CWinAppEx::OnIdle(lCount);
}



// **************************************************************************************************
// **************************************************************************************************
// **************************************************************************************************


// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
	// Messages displayed when user clicks Help Tab and then About
	CNetDesignerDoc* pMyDoc = (CNetDesignerDoc*)((CFrameWnd*)AfxGetApp()->GetMainWnd())->GetActiveDocument();
	label1 = _T("NNetDesigner, Version ") + pMyDoc->getVersion();
	label2 = _T("Copyright \xa9 2022 GradientN");
	label3 = _T("All rights reserved.");
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LABEL1, label1);
	DDX_Text(pDX, IDC_LABEL2, label2);
	DDX_Text(pDX, IDC_LABEL3, label3);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	return TRUE;  
				  
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
		hbr = mbrush2;
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







