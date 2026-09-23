

#include "pch.h"
#include "framework.h"

#include "DataView.h"
#include "Resource.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "WaveFile.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


DataView::DataView() noexcept
{
	m_notes = _T("");
	m_text = _T("");
	m_combo = _T("");
	ncols = 0;

	m_gray = FALSE;
	m_conv1 = FALSE;
	m_conv2 = TRUE;
	m_conv3 = FALSE;
	optimum_aspect = 1.5F;
	image_rect.left = 0;
	image_rect.top = 0;
	image_rect.right = 100;
	image_rect.bottom = (int)((float)image_rect.right / optimum_aspect);
	min_width = 0;
	awidth = 0;
	aheight = 0;
	image_left = 14;
	image_top = 14;
	allocated = false;
	conv_options = true;
	ready = false;
	waiting = false;
	folder = false;
	images_combined = true;

	show_progress = true;
	data_type = FILE_TYPE_NONE;
}

DataView::~DataView()
{
	delete m_select_control;
	delete m_folder_control;
	delete m_combo_ctrl;
	delete m_update_control;
	delete m_label1;
	delete m_label2;
	delete m_label3;
	delete m_edit1;
	delete m_edit2;
	deallocate();
}

void DataView::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_COMBO, m_combo_ctrl);
	DDX_CBString(pDX, IDC_FILE_COMBO, m_combo);
	DDX_Text(pDX, IDC_NOTES, m_notes);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Text(pDX, IDC_WIDTH, m_width);
	DDX_Text(pDX, IDC_HEIGHT, m_height);
	DDX_Check(pDX, IDC_GREY_SCALE, m_gray);
	DDX_Check(pDX, IDC_CONV1_OPTION, m_conv1);
	DDX_Check(pDX, IDC_CONV2_OPTION, m_conv2);
	DDX_Check(pDX, IDC_CONV3_OPTION, m_conv3);
}

BEGIN_MESSAGE_MAP(DataView, CDockablePane)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(IDC_SELECT_FILE, &DataView::OnBnClickedSelectFile)
	ON_UPDATE_COMMAND_UI(IDC_SELECT_FILE, &DataView::OnUpdateSelectFile)
	ON_COMMAND(IDC_SELECT_FOLDER, &DataView::OnBnClickedSelectFolder)
	ON_UPDATE_COMMAND_UI(IDC_SELECT_FOLDER, &DataView::OnUpdateSelectFolder)
	ON_COMMAND(IDC_GREY_SCALE, &DataView::OnBnClickedGreyScale)
	ON_UPDATE_COMMAND_UI(IDC_GREY_SCALE, &DataView::OnUpdateClickedGreyScale)
	ON_COMMAND(IDC_CONV1_OPTION, &DataView::OnBnClickedConv1)
	ON_UPDATE_COMMAND_UI(IDC_CONV1_OPTION, &DataView::OnUpdateClickedConv1)
	ON_COMMAND(IDC_CONV2_OPTION, &DataView::OnBnClickedConv2)
	ON_UPDATE_COMMAND_UI(IDC_CONV2_OPTION, &DataView::OnUpdateClickedConv2)
	ON_COMMAND(IDC_CONV3_OPTION, &DataView::OnBnClickedConv3)
	ON_UPDATE_COMMAND_UI(IDC_CONV3_OPTION, &DataView::OnUpdateClickedConv3)
	ON_COMMAND(IDC_UPDATE, &DataView::OnBnClickedUpdate)
	ON_UPDATE_COMMAND_UI(IDC_UPDATE, &DataView::OnUpdateUpdate)	
	ON_EN_CHANGE(IDC_WIDTH, &DataView::OnEnChangeWidth)
	ON_CBN_SELCHANGE(IDC_FILE_COMBO, &DataView::OnCbnSelchangeCombo)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void DataView::addErrorString(CString err) {
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString(err);
}

void DataView::addPanel1String(CString msg)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->SetStatusBarText1(msg);
}

int DataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_select_control.Create(_T("Select File(s)"), WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, 98, 26), this, IDC_SELECT_FILE);
	m_folder_control.Create(_T("Source"), WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0,68, 26), this, IDC_SELECT_FOLDER);
	m_combo_ctrl.Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE, CRect(0, 0, 100, 26), this, IDC_FILE_COMBO);
	
	m_list_control.Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | LVS_REPORT | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP | BS_DEFPUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_FILE_DATA);
	m_list_control.SetExtendedStyle(m_list_control.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_label1.Create(_T("Process Width:"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
	m_label2.Create(_T("Process Height:"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
	m_label3.Create(_T("Gray Scale:"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
	m_notes_ctrl.Create(_T("Notes"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_NOTES);
	m_text_ctrl.Create(_T("Text"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_TEXT);

	m_edit1.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_NOHIDESEL, CRect(0, 0, 0, 0), this, IDC_WIDTH);
	m_edit2.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_NOHIDESEL | ES_READONLY, CRect(0, 0, 0, 0), this, IDC_HEIGHT);

	m_update_control.Create(_T("Update"), WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_UPDATE);
	m_gray_control.Create(_T(""), WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_CHECKBOX, CRect(0, 0, 0, 0), this, IDC_GREY_SCALE);

	m_label4.Create(_T("Conv fully connected:"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
	m_label5.Create(_T("Conv separated by color:"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
	m_label6.Create(_T("Conv shared over colors:"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);

	m_conv1_control.Create(_T(""), WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_RADIOBUTTON, CRect(0, 0, 0, 0), this, IDC_CONV1_OPTION);
	m_conv2_control.Create(_T(""), WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_RADIOBUTTON, CRect(0, 0, 0, 0), this, IDC_CONV2_OPTION);
	m_conv3_control.Create(_T(""), WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_RADIOBUTTON, CRect(0, 0, 0, 0), this, IDC_CONV3_OPTION);

	if (image_rect.Width() == 0) {
		image_rect.left = 0;
		image_rect.top = 0;
		image_rect.right = 100;
	}
	image_rect.bottom = (int)((float)image_rect.right / optimum_aspect);
	m_width.Format(_T("%i"), image_rect.Width());
	m_height.Format(_T("%i"), image_rect.Height());
	UpdateData(false);

	SetFont();

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	int theme = pMainWnd->getTheme();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	clrGrayedText = GetGlobalData()->clrGrayedText;

	AdjustLayout();

	ready = true;

	return 0;
}

void DataView::setColors(int theme)
{
	COLORREF clr1 = GetGlobalData()->clrBarFace;
	COLORREF clr2 = GetGlobalData()->clrActiveCaption;

	clrGrayedText = GetGlobalData()->clrGrayedText;

	if (theme == LIGHT) {
		COLORREF antiflashwhite = 0x00f4f3f2;
		COLORREF aliceblue = 0x00fef9f0;
		clr1 = adjustColorByPercent(antiflashwhite, 0);
		clr2 = adjustColorByPercent(aliceblue, -5);
		clrGrayedText = RGB(0x36, 0x2b, 0x00);
	}
	else if (theme == DARK) {
		COLORREF charlestongreen = 0x002d2d2d;
		COLORREF grayactivecaption = 0x00555555;
		COLORREF graytext = 0x00f4f4f4;
		clr1 = adjustColorByPercent(charlestongreen, 30);
		clr2 = adjustColorByPercent(grayactivecaption, 10);
		clrGrayedText = adjustColorByPercent(graytext, -10);
	}
	else if (theme == BLUE) {
		COLORREF eggshell = 0x00d6eaf0;
		COLORREF tuftsblue = 0x00d39538;
		COLORREF coolblack = 0x005f2f07;
		clr1 = adjustColorByPercent(eggshell, 0);
		clr2 = adjustColorByPercent(tuftsblue, 0);
		clrGrayedText = coolblack;
	}
	else if (theme == COLOR) {
		COLORREF gardeneggshell = 0x00ceeeee;
		COLORREF russiangreen = 0x00579165;
		COLORREF pinetree = 0x00222b2e;
		clr1 = adjustColorByPercent(gardeneggshell, 0);
		clr2 = adjustColorByPercent(russiangreen, 20);
		clrGrayedText = pinetree;
	}

	mbrush.CreateSolidBrush(clr1);
	mbrush2.CreateSolidBrush(clr2);

	Invalidate(TRUE);
	UpdateWindow();

}

COLORREF DataView::adjustColorByPercent(COLORREF clr, float percent)
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

void DataView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic())) {
		return;
	}
	
	int edge = 5;
	int edge2 = 20;
	int separation = 5;

	CRect rectClient;
	GetClientRect(&rectClient);

	//CRect rect1 = CRect(edge, edge-1, edge + 98, edge + 27);
	CRect rect1 = CRect(edge, edge - 1, edge + 98, edge + 27);
	CRect rect3 = CRect(rect1.right + separation, edge - 1, rect1.right + separation + 68, edge + 27);// +300);
	CRect rect2 = CRect(rect3.right + separation, edge, rectClient.Width() - edge, edge + 26);// +300);

	m_select_control.MoveWindow(rect1);
	m_folder_control.MoveWindow(rect3);
	m_combo_ctrl.MoveWindow(rect2);
	
	if ((data_type == FILE_TYPE_CSV) || (data_type == FILE_TYPE_CSV_WITH_JPG)) {
		CRect rect0 = CRect(0, 0, 0, 0);
		m_label1.MoveWindow(rect0);
		m_label2.MoveWindow(rect0);
		m_label3.MoveWindow(rect0);
		m_label4.MoveWindow(rect0);
		m_label5.MoveWindow(rect0);
		m_label6.MoveWindow(rect0);
		m_edit1.MoveWindow(rect0);
		m_edit2.MoveWindow(rect0);
		m_update_control.MoveWindow(rect0);
		m_gray_control.MoveWindow(rect0);
		m_conv1_control.MoveWindow(rect0);
		m_conv2_control.MoveWindow(rect0);
		m_conv3_control.MoveWindow(rect0);
		m_notes_ctrl.MoveWindow(rect0);
		m_text_ctrl.MoveWindow(rect0);
		CRect rect3 = CRect(edge, rect1.bottom + separation, rectClient.Width() - edge, rectClient.bottom - edge);
		m_list_control.MoveWindow(rect3);
	}
	else if (data_type == FILE_TYPE_JPG) {
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		bool hasconv = false;
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConvLayer(i)) {
				if (pDoc->netData.getNodeMode(i) != "Input") {
					hasconv = true;
					i = pDoc->netData.size();
				}
			}
		}
		setConv(pDoc->getDataConvType());
		if (hasconv && !m_gray) {
			conv_options = true;
		}
		else {
			conv_options = false;
		}
		CRect rect0 = CRect(0, 0, 0, 0);
		m_list_control.MoveWindow(rect0);
		m_text_ctrl.MoveWindow(rect0);
		CRect crect;
		CClientDC dc(this);
		dc.GetClipBox(&crect);
		dc.FillRect(&crect, &mbrush);
		CRect rect3 = CRect(edge2, rect1.bottom + edge2 + separation, edge + 110, rect1.bottom + edge2 + 24);
		m_label1.MoveWindow(rect3);
		CRect rect4 = CRect(rect3.left, rect3.bottom + separation + separation, rect3.right, rect3.bottom + separation + 24);
		m_label2.MoveWindow(rect4);
		CRect rect5 = CRect(rect3.right + separation, rect1.bottom + edge2 + edge - 3, rect3.right + separation + 60, rect1.bottom + edge2 + edge + 21);
		m_edit1.MoveWindow(rect5);
		CRect rect6 = CRect(rect5.left, rect5.bottom + separation, rect5.right, rect5.bottom + separation + 24);
		m_edit2.MoveWindow(rect6);
		CRect rect7 = CRect(rect5.right + 3 * separation, rect1.bottom + edge2 + edge - 4, rect5.right + 3 * separation + 60, rect1.bottom + edge2 + edge + 20);
		m_update_control.MoveWindow(rect7);
		if (conv_options) {
			int label_width = 155;
			CRect rect8 = CRect(rect3.left, rect4.bottom + separation + separation + separation, rect3.right, rect4.bottom + separation + separation + 24);
			m_label3.MoveWindow(rect8);
			CRect rect9 = CRect(rect3.left + label_width + separation, rect6.bottom + separation + separation + separation, rect3.left + label_width + separation + 16, rect6.bottom + separation + separation + separation + 24 - 8);
			m_gray_control.MoveWindow(rect9);
			CRect rect11 = CRect(rect3.left + label_width + separation, rect9.bottom + separation + separation + separation, rect3.left + label_width + separation + 16, rect9.bottom + separation + separation + separation + 24 - 8);
			m_conv1_control.MoveWindow(rect11);
			CRect rect12 = CRect(rect3.left + label_width + separation, rect11.bottom + separation + separation / 2, rect3.left + label_width + separation + 16, rect11.bottom + separation + separation / 2 + 24 - 8);
			m_conv2_control.MoveWindow(rect12);
			CRect rect13 = CRect(rect3.left + label_width + separation, rect12.bottom + separation + separation / 2, rect3.left + label_width + separation + 16, rect12.bottom + separation + separation / 2 + 24 - 8);
			m_conv3_control.MoveWindow(rect13);
			CRect rect14 = CRect(rect3.left, rect11.top - 1, rect3.left + label_width, rect11.bottom + 1);
			m_label4.MoveWindow(rect14);
			CRect rect15 = CRect(rect3.left, rect12.top - 1, rect3.left + label_width, rect12.bottom + 1);
			m_label5.MoveWindow(rect15);
			CRect rect16 = CRect(rect3.left, rect13.top - 1, rect3.left + label_width, rect13.bottom + 1);
			m_label6.MoveWindow(rect16);
			CRect rect10 = CRect(rect8.left, rect13.bottom + separation + separation + separation, rect7.right, rectClient.bottom - edge2 - edge);
			m_notes_ctrl.MoveWindow(rect10);
		}
		else {
			m_label4.MoveWindow(rect0);
			m_label5.MoveWindow(rect0);
			m_label6.MoveWindow(rect0);
			m_conv1_control.MoveWindow(rect0);
			m_conv2_control.MoveWindow(rect0);
			m_conv3_control.MoveWindow(rect0);
			CRect rect8 = CRect(rect3.left, rect4.bottom + separation + separation, rect3.right, rect4.bottom + separation + 24);
			m_label3.MoveWindow(rect8);
			CRect rect9 = CRect(rect6.left, rect6.bottom + separation + separation, rect6.left + 16, rect6.bottom + separation + separation + 24 - 8);
			m_gray_control.MoveWindow(rect9);
			CRect rect10 = CRect(rect8.left, rect8.bottom + separation + separation, rect7.right, rectClient.bottom - edge2 - edge);
			m_notes_ctrl.MoveWindow(rect10);
		}
		image_left = rect7.right + 3 * separation;
		image_top = rect1.bottom + edge2 + separation - 4;
		adjustSize();
	}
	else if (data_type == FILE_TYPE_RAW) {
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		bool hasconv = false;
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConvLayer(i)) {
				if (pDoc->netData.getNodeMode(i) != "Input") {
					hasconv = true;
					i = pDoc->netData.size();
				}
			}
		}
		setConv(pDoc->getDataConvType());
		if (hasconv && !m_gray) {
			conv_options = true;
		}
		else {
			conv_options = false;
		}
		CRect rect0 = CRect(0, 0, 0, 0);
		m_list_control.MoveWindow(rect0);
		m_text_ctrl.MoveWindow(rect0);
		CRect crect;
		CClientDC dc(this);
		dc.GetClipBox(&crect);
		dc.FillRect(&crect, &mbrush);
		CRect rect3 = CRect(edge2, rect1.bottom + edge2 + separation, edge + 110, rect1.bottom + edge2 + 24);
		m_label1.MoveWindow(rect3);
		CRect rect4 = CRect(rect3.left, rect3.bottom + separation + separation, rect3.right, rect3.bottom + separation + 24);
		m_label2.MoveWindow(rect4);
		CRect rect5 = CRect(rect3.right + separation, rect1.bottom + edge2 + edge - 3, rect3.right + separation + 60, rect1.bottom + edge2 + edge + 21);
		m_edit1.MoveWindow(rect5);
		CRect rect6 = CRect(rect5.left, rect5.bottom + separation, rect5.right, rect5.bottom + separation + 24);
		m_edit2.MoveWindow(rect6);
		CRect rect7 = CRect(rect5.right + 3 * separation, rect1.bottom + edge2 + edge - 4, rect5.right + 3 * separation + 60, rect1.bottom + edge2 + edge + 20);
		m_update_control.MoveWindow(rect7);
		if (conv_options) {
			int label_width = 155;
			CRect rect8 = CRect(rect3.left, rect4.bottom + separation + separation + separation, rect3.right, rect4.bottom + separation + separation + 24);
			m_label3.MoveWindow(rect8);
			CRect rect9 = CRect(rect3.left + label_width + separation, rect6.bottom + separation + separation + separation, rect3.left + label_width + separation + 16, rect6.bottom + separation + separation + separation + 24 - 8);
			m_gray_control.MoveWindow(rect9);
			CRect rect11 = CRect(rect3.left + label_width + separation, rect9.bottom + separation + separation + separation, rect3.left + label_width + separation + 16, rect9.bottom + separation + separation + separation + 24 - 8);
			m_conv1_control.MoveWindow(rect11);
			CRect rect12 = CRect(rect3.left + label_width + separation, rect11.bottom + separation + separation / 2, rect3.left + label_width + separation + 16, rect11.bottom + separation + separation / 2 + 24 - 8);
			m_conv2_control.MoveWindow(rect12);
			CRect rect13 = CRect(rect3.left + label_width + separation, rect12.bottom + separation + separation / 2, rect3.left + label_width + separation + 16, rect12.bottom + separation + separation / 2 + 24 - 8);
			m_conv3_control.MoveWindow(rect13);
			CRect rect14 = CRect(rect3.left, rect11.top - 1, rect3.left + label_width, rect11.bottom + 1);
			m_label4.MoveWindow(rect14);
			CRect rect15 = CRect(rect3.left, rect12.top - 1, rect3.left + label_width, rect12.bottom + 1);
			m_label5.MoveWindow(rect15);
			CRect rect16 = CRect(rect3.left, rect13.top - 1, rect3.left + label_width, rect13.bottom + 1);
			m_label6.MoveWindow(rect16);
			CRect rect10 = CRect(rect8.left, rect13.bottom + separation + separation + separation, rect7.right, rectClient.bottom - edge2 - edge);
			m_notes_ctrl.MoveWindow(rect10);
		}
		else {
			m_label4.MoveWindow(rect0);
			m_label5.MoveWindow(rect0);
			m_label6.MoveWindow(rect0);
			m_conv1_control.MoveWindow(rect0);
			m_conv2_control.MoveWindow(rect0);
			m_conv3_control.MoveWindow(rect0);
			CRect rect8 = CRect(rect3.left, rect4.bottom + separation + separation, rect3.right, rect4.bottom + separation + 24);
			m_label3.MoveWindow(rect8);
			CRect rect9 = CRect(rect6.left, rect6.bottom + separation + separation, rect6.left + 16, rect6.bottom + separation + separation + 24 - 8);
			m_gray_control.MoveWindow(rect9);
			CRect rect10 = CRect(rect8.left, rect8.bottom + separation + separation, rect7.right, rectClient.bottom - edge2 - edge);
			m_notes_ctrl.MoveWindow(rect10);
		}
		image_left = rect7.right + 3 * separation;
		image_top = rect1.bottom + edge2 + separation - 4;
		adjustSize();
	}
	else if (data_type == FILE_TYPE_WAV) {
		int image_height = 200;
		CRect rect0 = CRect(0, 0, 0, 0);
		m_label1.MoveWindow(rect0);
		m_label2.MoveWindow(rect0);
		m_label3.MoveWindow(rect0);
		m_label4.MoveWindow(rect0);
		m_label5.MoveWindow(rect0);
		m_label6.MoveWindow(rect0);
		m_edit1.MoveWindow(rect0);
		m_edit2.MoveWindow(rect0);
		m_update_control.MoveWindow(rect0);
		m_gray_control.MoveWindow(rect0);
		m_conv1_control.MoveWindow(rect0);
		m_conv2_control.MoveWindow(rect0);
		m_conv3_control.MoveWindow(rect0);
		m_notes_ctrl.MoveWindow(rect0);
		m_text_ctrl.MoveWindow(rect0);
		m_list_control.MoveWindow(rect0);
		CRect crect;
		CClientDC dc(this);
		dc.GetClipBox(&crect);
		dc.FillRect(&crect, &mbrush);
		CRect rect10 = CRect(edge + separation, rect1.bottom + 3 * separation + image_height, rectClient.Width() - edge, rectClient.bottom - edge2 - edge);
		m_notes_ctrl.MoveWindow(rect10);
		CRect rect3;
		rect3.left = edge;
		rect3.top = rect1.bottom + separation;
		rect3.right = rectClient.Width() - edge;
		rect3.bottom = rect1.bottom + separation + image_height;
		mgraph.setSize(rect3);
		//mgraph.drawGraph(&dc);
	}
	else if (data_type == FILE_TYPE_TXT) {
		CRect rect0 = CRect(0, 0, 0, 0);
		m_label1.MoveWindow(rect0);
		m_label2.MoveWindow(rect0);
		m_label3.MoveWindow(rect0);
		m_label4.MoveWindow(rect0);
		m_label5.MoveWindow(rect0);
		m_label6.MoveWindow(rect0);
		m_edit1.MoveWindow(rect0);
		m_edit2.MoveWindow(rect0);
		m_update_control.MoveWindow(rect0);
		m_gray_control.MoveWindow(rect0);
		m_conv1_control.MoveWindow(rect0);
		m_conv2_control.MoveWindow(rect0);
		m_conv3_control.MoveWindow(rect0);
		m_list_control.MoveWindow(rect0);
		int mheight = 80;
		CRect rect3 = CRect((2 * edge), rect1.bottom + (2 * separation), rectClient.Width() - (2 * edge), rect1.bottom + separation + mheight);
		m_notes_ctrl.MoveWindow(rect3);
		CRect rect4 = CRect(rect3.left, rect3.bottom + separation, rect3.right, rectClient.bottom - (2 * edge));
		m_text_ctrl.MoveWindow(rect4);
	}
	else {
		CRect rect0 = CRect(0, 0, 0, 0);
		m_list_control.MoveWindow(rect0);
		m_label1.MoveWindow(rect0);
		m_label2.MoveWindow(rect0);
		m_label3.MoveWindow(rect0);
		m_label4.MoveWindow(rect0);
		m_label5.MoveWindow(rect0);
		m_label6.MoveWindow(rect0);
		m_edit1.MoveWindow(rect0);
		m_edit2.MoveWindow(rect0);
		m_update_control.MoveWindow(rect0);
		m_gray_control.MoveWindow(rect0);
		m_conv1_control.MoveWindow(rect0);
		m_conv2_control.MoveWindow(rect0);
		m_conv3_control.MoveWindow(rect0);
		m_notes_ctrl.MoveWindow(rect0);
		m_text_ctrl.MoveWindow(rect0);
	}

}

void DataView::OnPaint()
{
	CPaintDC dc(this); 
	CRect crect;
	dc.GetClipBox(&crect);
	dc.FillRect(&crect, &mbrush);
	if (!waiting) {
		updateImage();
	}
	updateWave();
}

HBRUSH DataView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDockablePane::OnCtlColor(pDC, pWnd, nCtlColor);

	hbr = mbrush;

	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		pDC->SetTextColor(clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		//hbr = mbrush2;
		break;
	case CTLCOLOR_BTN:
		pDC->SetTextColor(RGB(255, 0, 0));
		//pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_DLG:
		pDC->SetTextColor(RGB(255, 255, 0));
		//pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_EDIT:
		hbr = mbrush2;
		pDC->SetTextColor(clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_LISTBOX:
		hbr = mbrush2;
		pDC->SetTextColor(clrGrayedText);
		//pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_MSGBOX:
		pDC->SetTextColor(RGB(255, 0, 255));
		break;
	case CTLCOLOR_SCROLLBAR:
		pDC->SetTextColor(RGB(0, 0, 255));
		break;
	default:
		pDC->SetTextColor(RGB(255, 0, 0)); //GetGlobalData()->clrGrayedText);
		//pDC->SetBkMode(TRANSPARENT);
	}

	return hbr;
}

void DataView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if ((data_type == FILE_TYPE_JPG) || (data_type == FILE_TYPE_WAV)) {
		if (!waiting) {
			waiting = true;
			if (data_type == FILE_TYPE_JPG) {
				SetTimer(100, 500, NULL);
			}
			else if (data_type == FILE_TYPE_WAV) {
				//SetTimer(100, 200, NULL);
			}
		}
	}
	AdjustLayout();
}

void DataView::SetFont()
{
	::DeleteObject(m_fnt.Detach());
	
	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = 18;// info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fnt.CreateFontIndirect(&lf);
	
	m_select_control.SetFont(&m_fnt);
	m_folder_control.SetFont(&m_fnt);
	m_combo_ctrl.SetFont(&m_fnt);
	m_list_control.SetFont(&m_fnt);
	m_label1.SetFont(&m_fnt);
	m_label2.SetFont(&m_fnt);
	m_label3.SetFont(&m_fnt);
	m_edit1.SetFont(&m_fnt);
	m_edit2.SetFont(&m_fnt);
	m_update_control.SetFont(&m_fnt);
	m_gray_control.SetFont(&m_fnt);
	m_conv1_control.SetFont(&m_fnt);
	m_conv2_control.SetFont(&m_fnt);
	m_conv3_control.SetFont(&m_fnt);
	m_notes_ctrl.SetFont(&m_fnt);
	m_text_ctrl.SetFont(&m_fnt);
	m_label4.SetFont(&m_fnt);
	m_label5.SetFont(&m_fnt);
	m_label6.SetFont(&m_fnt);

}

void DataView::OnBnClickedSelectFile()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	CAuxFunctions func;
	vector<CString> mpaths;
	if (folder) {
		CString mfolder = _T("");
		vector<CString> mpnames = pDoc->getDataPathNames();
		if (mpnames.size() > 0) {
			mfolder = mpnames[0];
		}
		mfolder = func.getDataFolder(mfolder);
		if (mfolder != _T("")) {
			mpaths = func.getDirectoryAndSubdirectoryFiles(mfolder);
			if (mpaths.size() > 0) {
				mpaths = func.getSelectFileType(mpaths, mfolder);
			}
		}
	}
	else {
		mpaths = pDoc->getDataPathNames();
		int ftype = pDoc->getDataPathType();
		if (mpaths.size() == 0) {
			mpaths = func.getDataFileName(_T(""), ftype);
		}
		else {
			mpaths = func.getDataFileName(pDoc->getFirstDataPathName(), ftype);
		}
	}
	//CString s = _T(""); s.Format(_T("OnBnClickedSelectFile : mpaths size = %i"), (int)mpaths.size()); addErrorString(s);
	if (mpaths.size() > 0) {
		data_type = pDoc->getDataPathType(mpaths);
		if ((data_type == FILE_TYPE_CSV) || (data_type == FILE_TYPE_CSV_WITH_JPG)) {
			if (mpaths[0] != _T("")) {
				UpdateData();
				mfilenames = mpaths;
				mfilenames = func.checkHeadersFromDataSources(mfilenames);
				vector<string> tfilenames;
				for (int j = 0; j < mfilenames.size(); j++) {
					tfilenames.push_back(string(CT2CA(mfilenames[j])));
				}
				int width = (int)_tstof((LPCTSTR)m_width);
				int height = (int)_tstof((LPCTSTR)m_height);
				pDoc->updateDataSource(_T("CSV"), width, height, pDoc->getMinimumImageWidth(), pDoc->getImageAspectRatio(), m_gray, pDoc->getDataWavMax(), tfilenames);
			}
		}
		else if (data_type == FILE_TYPE_JPG) {
			UpdateData();
			imageCheckData idata = func.checkJpgSources(mpaths);
			mfilenames = idata.paths;
			if (mfilenames.size() > 0) {
				vector<string> tfilenames;
				for (int j = 0; j < mfilenames.size(); j++) {
					tfilenames.push_back(string(CT2CA(mfilenames[j])));
				}
				int width = (int)_tstof((LPCTSTR)m_width);
				int height = (int)_tstof((LPCTSTR)m_height);
				min_width = idata.min_width;
				optimum_aspect = idata.optimum_aspect;
				if (optimum_aspect <= 0) {
					optimum_aspect = 1.5F;
				}
				pDoc->updateDataSource(_T("JPG"), width, height, min_width, optimum_aspect, m_gray, pDoc->getDataWavMax(), tfilenames);
				OnBnClickedUpdate();
			}
		}
		else if (data_type == FILE_TYPE_RAW) {
			if (func.checkRawSources(mpaths)) {
				mfilenames = mpaths;
				vector<string> tfilenames;
				for (int j = 0; j < (int)mpaths.size(); j++) {
					tfilenames.push_back(string(CT2CA(mpaths[j])));
				}
				int width = (int)_tstof((LPCTSTR)m_width);
				int height = (int)_tstof((LPCTSTR)m_height);
				min_width = pDoc->image_width;
				for (int i = 0; i < pDoc->netData.size(); i++) {
					if (pDoc->netData.isDataSource(i)) {
						min_width = pDoc->netData.getRawImageWidth(i);
						i = pDoc->netData.size();
					}
				}
				optimum_aspect = 1.0;
				if (height > 0) {
					optimum_aspect = (float)width / (float)height;
				}
				//CString s = _T("");  s.Format(_T("OnBnClickedSelectFile for raw : width = %i : height = %i"), width, height);  addErrorString(s);
				pDoc->updateDataSource(_T("RAW"), width, height, min_width, optimum_aspect, m_gray, pDoc->getDataWavMax(), tfilenames);
				OnBnClickedUpdate();
			}
		}
		else if (data_type == FILE_TYPE_WAV) {
			UpdateData();
			mfilenames = mpaths;
			if (mfilenames.size() > 0) {
				vector<string> tfilenames;
				for (int j = 0; j < mfilenames.size(); j++) {
					tfilenames.push_back(string(CT2CA(mfilenames[j])));
				}
				int width = (int)_tstof((LPCTSTR)m_width);
				int height = (int)_tstof((LPCTSTR)m_height);
				pDoc->updateDataSource(_T("WAV"), width, height, pDoc->getMinimumImageWidth(), pDoc->getImageAspectRatio(), m_gray, pDoc->getDataWavMax(), tfilenames);
				pDoc->getCombinedImages();
				images_combined = true;
				pDoc->getMaxWaveMagnitude(true);
				pDoc->setDataWavMax((int)pDoc->wresults.files_max);
			}
		}
		else if (data_type == FILE_TYPE_TXT) {
			UpdateData();
			mfilenames = mpaths;
			if (mfilenames.size() > 0) {
				vector<string> tfilenames;
				for (int j = 0; j < mfilenames.size(); j++) {
					tfilenames.push_back(string(CT2CA(mfilenames[j])));
				}
				pDoc->updateDataSource(_T("TXT"), pDoc->getDataImageWidth(), pDoc->getDataImageHeight(), pDoc->getMinimumImageWidth(), pDoc->getImageAspectRatio(), m_gray, pDoc->getDataWavMax(), tfilenames);
				pDoc->getCombinedImages();
				images_combined = true;
			}
		}
		updateFromNet();
	}
}

void DataView::OnUpdateSelectFile(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void DataView::OnBnClickedSelectFolder()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (folder) {
		folder = false;
		pDoc->select_folder = false;
		m_select_control.SetWindowText(_T("Select File(s)"));
	}
	else {
		folder = true;
		pDoc->select_folder = true;
		m_select_control.SetWindowText(_T("Select Folder"));
	}
}

void DataView::OnUpdateSelectFolder(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void DataView::updateFromNet()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	CString mfilename = _T("");

	mfilenames = pDoc->getDataPathNames();
	data_type = pDoc->getDataPathType(mfilenames);

	m_width.Format(_T("%i"), pDoc->getDataImageWidth());
	m_gray = pDoc->getDataImageGray();

	setFilesnamesToList();
	
	if ((data_type == FILE_TYPE_CSV) || (data_type == FILE_TYPE_CSV_WITH_JPG)) {
		setCsvData();
	}
	else if (data_type == FILE_TYPE_JPG) {
		setJpgData();
	}
	else if (data_type == FILE_TYPE_RAW) {
		setRawData();
	}
	else if (data_type == FILE_TYPE_WAV) {
		setWavData();
	}
	else if (data_type == FILE_TYPE_TXT) {
		setTextData();
	}
	else {
		SetWindowText(_T("  Select Data Source"));
		AdjustLayout();
	}

}

void DataView::setFilesnamesToList()
{
	m_combo = _T("");
	m_combo_ctrl.ResetContent();
	if (mfilenames.size() > 0) {
		for (int i = 0; i < mfilenames.size(); i++) {
			m_combo_ctrl.AddString(mfilenames[i]);
		}
		m_combo = mfilenames[0];
	}
	UpdateData(false);
}

void DataView::setCsvData()
{
	if (mfilenames.size() > 0) {
		CAuxFunctions func;
		SetWindowText(_T("  CSV File Data"));
		headers = func.getHeadersFromDataSources();
		dataSourceData = func.getDataFromDataSources(headers);
		AdjustLayout();
		updateCSV();
	}
}

void DataView::setJpgData()
{
	if (mfilenames.size() > 0) {
		if (data_type == FILE_TYPE_JPG) {
			SetWindowText(_T("  JPG File Data"));
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			min_width = pDoc->getMinimumImageWidth();
			optimum_aspect = pDoc->getImageAspectRatio();
			if ((min_width <= 0) || (optimum_aspect <= 0)) {
				CAuxFunctions func;
				imageCheckData idata = func.checkJpgSources(mfilenames);
				min_width = idata.min_width;
				optimum_aspect = idata.optimum_aspect;
				if (optimum_aspect <= 0) {
					optimum_aspect = 1.5F;
				}
			}
			OnEnChangeWidth();
			AdjustLayout();
			allocate();
			updateImage();
		}
	}
}

void DataView::setRawData()
{
	if (mfilenames.size() > 0) {
		if (data_type == FILE_TYPE_RAW) {
			SetWindowText(_T("  RAW File Data"));
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			int width = pDoc->image_width;
			int height = pDoc->image_height;
			for (int i = 0; i < pDoc->netData.size(); i++) {
				if (pDoc->netData.isDataSource(i)) {
					width = pDoc->netData.getRawImageWidth(i);
					height = pDoc->netData.getRawImageHeight(i);
					i = pDoc->netData.size();
				}
			}
			min_width = width;
			optimum_aspect = 1.0;
			if (height > 0) {
				optimum_aspect = (float)width / (float)height;
			}
			OnEnChangeWidth();
			AdjustLayout();
			allocate();
			updateImage();
		}
	}
}

void DataView::setWavData()
{
	if (mfilenames.size() > 0) {
		if (data_type == FILE_TYPE_WAV) {
			SetWindowText(_T("  Sound File Data"));
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			/*min_width = pDoc->getMinimumImageWidth();
			optimum_aspect = pDoc->getImageAspectRatio();
			if ((min_width <= 0) || (optimum_aspect <= 0)) {
				CAuxFunctions func;
				imageCheckData idata = func.checkJpgSources(mfilenames);
				min_width = idata.min_width;
				optimum_aspect = idata.optimum_aspect;
				if (optimum_aspect <= 0) {
					optimum_aspect = 1.5F;
				}
			}*/
			AdjustLayout();
			updateWave();
		}
	}
}

void DataView::updateCSV()
{
	if ((data_type == FILE_TYPE_CSV) || (data_type == FILE_TYPE_CSV_WITH_JPG)) {
		fillHeaders();
		fillData();
	}
}

void DataView::fillHeaders()
{
	for (int i = 0; i < ncols; i++) {
		m_list_control.DeleteColumn(0);
	}
	m_list_control.DeleteAllItems();
	ncols = 0;
	m_list_control.InsertColumn(1, _T("Index"), LVCFMT_CENTER, 50);
	ncols++;
	int mcols = (int)headers.size();
	if (mcols > 100) {
		mcols = 100;
	}
	for (int i = 0; i < mcols; i++) {
		int width = 20 + (7 * headers[i].GetLength());
		if (width < 80) {
			width = 80;
		}
		m_list_control.InsertColumn(i + 2, headers[i], LVCFMT_CENTER, width);
		ncols++;
	}
}

void DataView::fillData()
{
	CAuxFunctions func;
	CStatus progress;
	if (show_progress) {
		progress.CreateDlg();
		progress.setLabel(_T("Settig fileview data . . ."));
		progress.setProgress(0);
	}
	addPanel1String(_T("Setting fileview data"));
	int findex = m_combo_ctrl.GetCurSel();
	int numcols = (long)headers.size();
	if ((headers.size() > 0) && (findex < (int)dataSourceData.size())) {
		if (dataSourceData[findex].size() > 0) {
			long numrows = (int)dataSourceData[findex][0].size();
			if (numrows > 1000) {
				numrows = 1000;
			}
			if (numrows > 0) {
				int mcnt = 0;
				int ttotal = 1 + numrows;
				CString sbuf = _T("");
				for (int i = 0; i < numrows; i++) {
					sbuf.Format(_T("%i"), i + 1);
					int nIndex = m_list_control.InsertItem(i + 1, sbuf);
					for (int j = 0; j < (ncols - 1)/*numcols*/; j++) {
						m_list_control.SetItemText(nIndex, j + 1, dataSourceData[findex][j][i]);
					}
					if (show_progress) {
						mcnt++;
						if (mcnt > 100) {
							progress.setProgress((100 * (i + 1)) / (numrows - 1));
							if (progress.cancel) {
								progress.DestroyDlg();
								return;
							}
							mcnt = 0;
						}
					}
				}
			}
		}
	}
	if (show_progress) {
		progress.DestroyDlg();
	}
	addPanel1String(_T(""));
	show_progress = false;
}

void DataView::OnBnClickedGreyScale()
{
	if (data_type == FILE_TYPE_JPG) {
		UpdateData();
		if (m_gray) {
			m_gray = false;
		}
		else {
			m_gray = true;
		}
		UpdateData(false);
		updateImage();
		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		CString mode = _T("JPG");
		int width = (int)_tstof((LPCTSTR)m_width);
		if (width < 10) {
			width = 10;
			m_width.Format(_T("%i"), width);
		}
		int height = (int)((float)width / optimum_aspect);
		m_height.Format(_T("%i"), height);
		vector<string> tfilenames;
		for (int j = 0; j < mfilenames.size(); j++) {
			tfilenames.push_back(string(CT2CA(mfilenames[j])));
		}
		pDoc->updateDataSource(mode, width, height, min_width, optimum_aspect, m_gray, pDoc->getDataWavMax(), tfilenames);
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		if (m_gray) {
			pView->removeNonGrayConvConnections();
		}
		pView->Invalidate(TRUE);
		pView->UpdateWindow();
		AdjustLayout();
	}
}

void DataView::OnUpdateClickedGreyScale(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void DataView::setConv(int conv)
{
	UpdateData();
	if (conv == 0) {
		m_conv1 = FALSE;
		m_conv2 = FALSE;
		m_conv3 = FALSE;
	}
	else if (conv == 1) {
		m_conv1 = TRUE;
		m_conv2 = FALSE;
		m_conv3 = FALSE;
	}
	else if (conv == 2) {
		m_conv1 = FALSE;
		m_conv2 = TRUE;
		m_conv3 = FALSE;
	}
	else if (conv == 3) {
		m_conv1 = FALSE;
		m_conv2 = FALSE;
		m_conv3 = TRUE;
	}
	UpdateData(false);
}

void DataView::OnBnClickedConv1()
{
	UpdateData();
	if (m_conv1) {
		m_conv1 = FALSE;
	}
	else {
		m_conv1 = TRUE;
	}
	m_conv2 = FALSE;
	m_conv3 = FALSE;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->setDataConvType(m_conv1, m_conv2, m_conv3);
	UpdateData(false);
}

void DataView::OnUpdateClickedConv1(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void DataView::OnBnClickedConv2()
{
	UpdateData();
	if (m_conv2) {
		m_conv2 = FALSE;
	}
	else {
		m_conv2 = TRUE;
	}
	m_conv1 = FALSE;
	m_conv3 = FALSE;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->setDataConvType(m_conv1, m_conv2, m_conv3);
	UpdateData(false);
}

void DataView::OnUpdateClickedConv2(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void DataView::OnBnClickedConv3()
{
	UpdateData();
	if (m_conv3) {
		m_conv3 = FALSE;
	}
	else {
		m_conv3 = TRUE;
	}
	m_conv1 = FALSE;
	m_conv2 = FALSE;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->setDataConvType(m_conv1, m_conv2, m_conv3);
	UpdateData(false);
}

void DataView::OnUpdateClickedConv3(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void DataView::OnEnChangeWidth()
{
	UpdateData();
	if (optimum_aspect > 0) {
		int width = (int)_tstof((LPCTSTR)m_width);
		if (width > min_width) {
			width = min_width;
			m_width.Format(_T("%i"), width);
		}
		int height = (int)((float)width / optimum_aspect);
		m_height.Format(_T("%i"), height);
		UpdateData(false);
	}
}

void DataView::OnBnClickedUpdate()
{
	if ((data_type == FILE_TYPE_JPG) || (data_type == FILE_TYPE_RAW)) {
		UpdateData();
		if (optimum_aspect > 0) {
			int width = (int)_tstof((LPCTSTR)m_width);
			if (width < 10) {
				width = 10;
				m_width.Format(_T("%i"), width);
			}
			int height = (int)((float)width / optimum_aspect);
			m_height.Format(_T("%i"), height);
			allocate();
			adjustSize();
			Invalidate(TRUE);
			UpdateWindow();
			updateImage();
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			CString mode = _T("");
			/*if ((data_type == FILE_TYPE_CSV) || (data_type == FILE_TYPE_CSV_WITH_JPG)) {
				mode = _T("CSV");
			}
			else */
			if (data_type == FILE_TYPE_JPG) {
				mode = _T("JPG");
			}
			else if (data_type == FILE_TYPE_RAW) {
				mode = _T("RAW");
			}
			vector<string> tfilenames;
			for (int j = 0; j < mfilenames.size(); j++) {
				tfilenames.push_back(string(CT2CA(mfilenames[j])));
			}
			pDoc->updateDataSource(mode, width, height, min_width, optimum_aspect, m_gray, pDoc->getDataWavMax(), tfilenames);

			for (int i = 0; i < pDoc->netData.size(); i++) {
				if (pDoc->netData.isConvLayer(i)) {
					if (pDoc->netData.getNodeMode(i) == "Output") {
						int nodes_per_layer = pDoc->getOutputImageSize();

						CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
						CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
						CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();

						netDataStruct tdata = pDoc->netData.getNode(i);
						tdata.setConvNodesPerLayer(nodes_per_layer);

						pDoc->clearAndSetEditParams(UNDO_EDIT, pView->zoomScale, pView->lHorzScollPos, pView->lVertScollPos);
						pDoc->pushEditConvLayer(tdata);
						pDoc->processEditData();

					}
				}
			}

		}
	}
}

void DataView::OnUpdateUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void DataView::allocate()
{
	deallocate();
	int width = (int)_tstof((LPCTSTR)m_width);
	int height = (int)_tstof((LPCTSTR)m_height);
	image_data = new unsigned char[3 * (width + 0) * (height + 2)];
	if (image_data == 0) {
		AfxMessageBox(_T("DataView : Unable to allocate memory for image"));
	}
	else {
		awidth = width;
		aheight = height;
		allocated = true;
	}
}

void DataView::deallocate()
{
	if (allocated) {
		delete[] image_data;
		allocated = false;
	}
}

void DataView::updateImage()
{
	if (data_type == FILE_TYPE_JPG) {

		int findex = m_combo_ctrl.GetCurSel();

		if ((findex >= 0) && (findex < mfilenames.size()) && ready) {

			CAuxFunctions func;

			HRESULT rslt = image.Load(mfilenames[findex]);

			if (rslt == S_OK) {

				CBitmap bitmap;
				bitmap.Attach(image.Detach());

				if (bitmap.GetBitmap(&pBitMap)) {

					UpdateData();
					float aspect = 0;
					if (pBitMap.bmHeight != 0) {
						aspect = (float)pBitMap.bmWidth / (float)pBitMap.bmHeight;
					}
					CString s = _T("");
					s.Format(_T("Num Images = %i"), (int)mfilenames.size());
					m_notes = s;
					s.Format(_T("\nImage Size : %iW x %iH"), pBitMap.bmWidth, pBitMap.bmHeight);
					m_notes += s;
					s.Format(_T("\nImage Aspect Ratio : %.4f"), aspect);
					m_notes += s;
					s.Format(_T("\nProcess Aspect Ratio : %.4f"), optimum_aspect);
					m_notes += s;
					UpdateData(false);

					if (allocated) {

						if ((awidth != image_rect.Width()) || (aheight != image_rect.Height())) {
							OnBnClickedUpdate();
						}

						if ((awidth == image_rect.Width()) && (aheight == image_rect.Height())) {

							unsigned char* mbits = (unsigned char*)pBitMap.bmBits;
							image_data = func.downscaleImageData(pBitMap.bmHeight, pBitMap.bmWidth, pBitMap.bmWidthBytes, mbits, image_rect.Height(), image_rect.Width(), image_data);

							CClientDC dc(this);

							CImage mimage;

							if (m_gray) {

								mimage.Create(image_rect.Width(), image_rect.Height(), 24);

								int mclr = 0;
								int index = 0;
								for (int j = 0; j < image_rect.Height() - 0; j++) {
									for (int i = 0; i < image_rect.Width() - 0; i++) {
										index = (j * (image_rect.Width() * 3)) + (i * 3);
										mclr = (image_data[index] + image_data[index + 1] + image_data[index + 2]) / 3;
										mimage.SetPixel(i, image_rect.Height() - j - 1, RGB(mclr, mclr, mclr));
									}
								}

							}
							else {

								mimage.Create(image_rect.Width(), image_rect.Height(), 24);

								int index = 0;
								for (int j = 0; j < image_rect.Height() - 0; j++) {
									for (int i = 0; i < image_rect.Width() - 0; i++) {
										index = (j * (image_rect.Width() * 3)) + (i * 3);
										mimage.SetPixel(i, image_rect.Height() - j - 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
									}
								}

							}

							mimage.Draw(dc, image_rect.left, image_rect.top);

						}
						else {
							s.Format(_T("DataView::updateImage size error :: %i : %i :: %i : %i"), awidth, image_rect.Width(), aheight, image_rect.Height());
							addErrorString(s);
						}

					}

				}

			}

		}

	}
	
}

void DataView::updateWave()
{
	if (data_type == FILE_TYPE_WAV) {

		int findex = m_combo_ctrl.GetCurSel();

		if ((findex >= 0) && (findex < mfilenames.size()) && ready) {

			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

			if (!images_combined) {
				pDoc->getCombinedImages();
				images_combined = true;
			}

			int min = pDoc->getDataWavMin();
			int max = pDoc->getDataWavMax();
			int shortest = pDoc->getDataWavShortest();
			int longest = pDoc->getDataWavLongest();
			if ((max == 0) || (longest == 0)) {
				pDoc->getMaxWaveMagnitude();
				min = pDoc->getDataWavMin();
				max = pDoc->getDataWavMax();
				shortest = pDoc->getDataWavShortest();
				longest = pDoc->getDataWavLongest();
			}

			CAuxFunctions func;

			wave_data wdata = func.getWaveDataFromFile(mfilenames[findex]);
			if (wdata.length > 0) {

			
				int filelength = wdata.file_length;
				int data_length = wdata.length;
				int sample_rate = wdata.sample_rate;

				UpdateData();

				mgraph.setXLabel(_T(""));
				mgraph.xlabel2 = _T("");

				vector<double> xdata;
				vector<double> ddata1;
				vector<double> ddata2;

				double mymax = 0;
				if ((filelength > 0) && (data_length > 0)) {
					for (int i = 0; i < data_length; i++) {
						xdata.push_back((double)i);
						ddata1.push_back(wdata.data[i]);
					}
				}

				CString s = _T("");
				m_notes = _T("\nFile Data:");
				s.Format(_T("\n File Length : %i"), filelength);
				m_notes += s;
				s.Format(_T("\n Data Length : %i"), data_length);
				m_notes += s;
				s.Format(_T("\n Sample Rate : %i"), sample_rate);
				m_notes += s;
				double wmax = pDoc->getDataWavMax();
				if (pDoc->wresults.files_max > wmax) {
					wmax = pDoc->wresults.files_max;
				}
				s.Format(_T("\n Wave Peak Amplitude : %.0f"), wmax);
				m_notes += s;
				if (sample_rate > 0) {
					s.Format(_T("\n Duration : %.3f sec"), (float)data_length / (float)sample_rate);
					m_notes += s;
				}
				UpdateData(false);

				mgraph.setData(xdata, ddata1, ddata2);

				mgraph.minx = 0;
				mgraph.maxx = (float)ddata1.size();
				mgraph.miny = -1 * mgraph.maxy;

				CClientDC dc(this);
				mgraph.drawFrame(&dc);
				mgraph.drawXAxis(&dc);
				mgraph.drawData(&dc);
				mgraph.drawGraph(&dc);

			}

		}

	}

}

void DataView::setTextData()
{
	if ((data_type == FILE_TYPE_TXT) && (mfilenames.size() > 0)) {

		int findex = m_combo_ctrl.GetCurSel();

		if ((findex >= 0) && (findex < mfilenames.size()) && ready) {

			CAuxFunctions func;
			SetWindowText(_T("  TXT File Data"));

			CString mtext = _T("");// func.getTextDataFromDataSource(mfilenames[findex]);

			m_text = _T("");
			int mlength = 4000;
			if (mtext.GetLength() < mlength) {
				mlength = mtext.GetLength();
			}
			for (int i = 0; i < mlength; i++) {
				m_text += mtext[i];
			}

			CString s = _T("");
			m_notes = _T("\nFile Data:");
			s.Format(_T("\n File Length : %i"), mtext.GetLength());
			m_notes += s;

			UpdateData(false);

			AdjustLayout();

		}

	}
}

void DataView::OnCbnSelchangeCombo()
{
	AdjustLayout();
	// Only updating text file type here
	// Image and sound types are updated in AdjustLayout
	// All CSV files are loaded into one array and so don't need to be updated 
	// This should be cleaned up and made more consistent
	if ((data_type == FILE_TYPE_TXT) && (mfilenames.size() > 0)) {
		UpdateData();
		int findex = m_combo_ctrl.GetCurSel();
		if ((findex >= 0) && (findex < mfilenames.size()) && ready) {
			CAuxFunctions func;
			CString mtext = _T("");// func.getTextDataFromDataSource(mfilenames[findex]);
			m_text = _T("");
			int mlength = 4000;
			if (mtext.GetLength() < mlength) {
				mlength = mtext.GetLength();
			}
			for (int i = 0; i < mlength; i++) {
				m_text += mtext[i];
			}
			CString s = _T("");
			m_notes = _T("\nFile Data:");
			s.Format(_T("\n File Length : %i"), mtext.GetLength());
			m_notes += s;
			UpdateData(false);
		}
	}
	//updateImage();
	//updateWave();
	updateCSV();
}

void DataView::adjustSize()
{
	int width = (int)_tstof((LPCTSTR)m_width);
	int height = (int)_tstof((LPCTSTR)m_height);
	image_rect.top = image_top;
	image_rect.left = image_left;
	image_rect.bottom = image_rect.top + height;
	image_rect.right = image_rect.left + width;
}

void DataView::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(100);
	updateImage();
	//updateWave();
	waiting = false;
	CDockablePane::OnTimer(nIDEvent);
}

BOOL DataView::PreTranslateMessage(MSG* pMsg)
{
	int key = (int)pMsg->wParam;
	if (pMsg->message == WM_KEYDOWN) {
		if (key == VK_DELETE) {
			if (m_combo_ctrl.GetDroppedState()) {
				int sel = m_combo_ctrl.GetCurSel();
				CString s = _T("");
				m_combo_ctrl.GetLBText(sel, s);
				if (AfxMessageBox(_T("Delete file ") + s, MB_YESNO) == IDYES) {
					for (int i = 0; i < mfilenames.size(); i++) {
						if (s == mfilenames[i]) {
							mfilenames.erase(mfilenames.begin() + i);
							i = (int)mfilenames.size();
						}
					}
					setFilesnamesToList();
					OnBnClickedUpdate();
				}
			}
		}
	}
	return CDockablePane::PreTranslateMessage(pMsg);
}
