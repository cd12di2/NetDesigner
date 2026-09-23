// CLayerDataDlg.cpp : implementation file
//

#include "pch.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerView.h"
#include "AuxFunctions.h"
#include "CLayerDataDlg.h"
#include "afxdialogex.h"


// CLayerDataDlg dialog

bool already6 = false;
CRect lastRect6(0, 0, 0, 0);

IMPLEMENT_DYNAMIC(CLayerDataDlg, CDialog)

CLayerDataDlg::CLayerDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LAYER_DATA, pParent)
{
	lastfieldindex = -1;
	lastdataindex = -1;
	//m_list1 = _T("");
	//m_list2 = _T("");
}

CLayerDataDlg::~CLayerDataDlg()
{
}

void CLayerDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1_ctrl);
	//DDX_LBString(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2_ctrl);
	//DDX_LBString(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDOK, m_ok_ctrl);
	DDX_Control(pDX, IDC_STATIC1, m_static1_ctrl);
	DDX_Control(pDX, IDC_STATIC2, m_static2_ctrl);
	DDX_Control(pDX, IDC_ADD, m_add_ctrl);
	DDX_Control(pDX, IDC_REMOVE, m_remove_ctrl);
}


BEGIN_MESSAGE_MAP(CLayerDataDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADD, &CLayerDataDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_REMOVE, &CLayerDataDlg::OnBnClickedRemove)
	ON_LBN_SELCHANGE(IDC_LIST1, &CLayerDataDlg::OnLbnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, &CLayerDataDlg::OnLbnDblclkList1)
	ON_LBN_SELCANCEL(IDC_LIST1, &CLayerDataDlg::OnLbnSelcancelList1)
	ON_LBN_SELCHANGE(IDC_LIST2, &CLayerDataDlg::OnLbnSelchangeList2)
	ON_LBN_DBLCLK(IDC_LIST2, &CLayerDataDlg::OnLbnDblclkList2)
	ON_LBN_SELCANCEL(IDC_LIST2, &CLayerDataDlg::OnLbnSelcancelList2)
END_MESSAGE_MAP()


// CLayerDataDlg message handlers


BOOL CLayerDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	adjustSize();

	SetWindowText(_T("Select Layer Data - ") + (CString)pDoc->netData.getID(dindex).c_str());

	CAuxFunctions func;
	vector<CString> mfnames = func.checkHeadersFromDataSources(pDoc->getDataPathNames(false));
	if (mfnames.size() > 0) {
		filesheaders = func.getHeadersFromDataSources();
	}
	listheaders.clear();
	listinputs.clear();

	lastfieldindex = -1;
	lastdataindex = -1;

	getNetInputFields();

	loadListBoxes();

	if (already6) {
		MoveWindow(lastRect6);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CLayerDataDlg::OnOK()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->netData.setDataFields(dindex, listinputs);
	GetWindowRect(lastRect6);
	already6 = true;
	MoveWindow(lastRect6);
	CDialog::OnOK();
}

HBRUSH CLayerDataDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr2 = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

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
		hbr = hbr2;// mbrush2;
		//pDC->SetTextColor(GetGlobalData()->clrGrayedText);
		//pDC->SetBkMode(TRANSPARENT);
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

void CLayerDataDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	adjustSize();
}

void CLayerDataDlg::adjustSize()
{
	CRect rect1;
	CRect rect2;
	CRect rect3;
	CRect rect4;
	CRect rect5;
	CRect rect6;
	CRect rect7;
	CRect rect8;

	GetWindowRect(&rect1);

	m_ok_ctrl.GetWindowRect(&rect2);
	m_add_ctrl.GetWindowRect(&rect3);
	m_remove_ctrl.GetWindowRect(&rect4);
	m_static1_ctrl.GetWindowRect(&rect5);
	m_static2_ctrl.GetWindowRect(&rect6);
	m_list1_ctrl.GetWindowRect(&rect7);
	m_list2_ctrl.GetWindowRect(&rect8);

	int spacex = 6;
	int spacey = 4;
	int borderx = 10;
	int bordery = 8;
	int listwidth = (rect1.Width() - ((2 * borderx) + (2 * spacex) + rect3.Width() + 20)) / 2;
	int listheight = rect1.Height() - ((2*bordery) + rect3.Height() + rect5.Height() + spacey + 55);

	m_ok_ctrl.MoveWindow(rect1.Width() - (rect2.Width() + borderx + 25), rect1.Height() - (rect2.Height() + bordery + 48), rect2.Width(), rect2.Height(), true);
	m_static1_ctrl.MoveWindow(borderx + 4, bordery, rect5.Width(), rect5.Height(), true);
	m_static2_ctrl.MoveWindow(borderx + 8 + listwidth + (2* spacex) + rect3.Width(), bordery, rect6.Width(), rect6.Height(), true);
	m_list1_ctrl.MoveWindow(borderx, bordery + spacey + rect5.Height(), listwidth, listheight, true);
	

	m_add_ctrl.MoveWindow(borderx + spacex + listwidth, bordery + spacey + rect5.Height() + spacey, rect3.Width(), rect3.Height(), true);
	m_remove_ctrl.MoveWindow(borderx + spacex + listwidth, bordery + (2*spacey) + rect3.Height() + rect5.Height() + spacey, rect3.Width(), rect3.Height(), true);

	m_list2_ctrl.ShowWindow(false);
	m_list2_ctrl.MoveWindow(borderx + listwidth + (2 * spacex) + rect3.Width(), bordery + spacey + rect5.Height(), listwidth, listheight, true);
	m_list2_ctrl.ShowWindow(true);

}

void CLayerDataDlg::getNetInputFields()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	listinputs = pDoc->netData.getDataFields(dindex);
	vector<CString> mheaders = filesheaders;
	listheaders.clear();
	for (int i = 0; i < mheaders.size(); i++) {
		bool found = false;
		for (int j = 0; j < listinputs.size(); j++) {
			if (mheaders[i] == (CString)listinputs[j].c_str()) {
				j = (int)listinputs.size();
				found = true;
			}
		}
		if (!found) {
			listheaders.push_back(mheaders[i]);
		}
	}
}

void CLayerDataDlg::loadListBoxes()
{
	m_list1_ctrl.ResetContent();
	if (listheaders.size() > 0) {
		for (int i = 0; i < listheaders.size(); i++) {
			m_list1_ctrl.AddString(listheaders[i]);
		}
	}
	m_list2_ctrl.ResetContent();
	if (listinputs.size() > 0) {
		for (int i = 0; i < listinputs.size(); i++) {
			m_list2_ctrl.AddString((CString)listinputs[i].c_str());
		}
	}
}

void CLayerDataDlg::OnBnClickedAdd()
{
	vector<string> selections;
	int nCount = m_list1_ctrl.GetCount();
	for (int i = 0; i < nCount; i++) {
		if (m_list1_ctrl.GetSel(i)) {
			CString selection = _T("");
			m_list1_ctrl.GetText(i, selection);
			selections.push_back(string(CT2CA(selection)));
		}
	}
	if (selections.size() > 0) {
		vector<CString> newlistitems;
		for (int j = 0; j < listheaders.size(); j++) {
			bool found1 = false;
			for (int i = 0; i < selections.size(); i++) {
				if (selections[i] == string(CT2CA(listheaders[j]))) {
					bool found2 = false;
					for (int k = 0; k < listinputs.size(); k++) {
						if (selections[i] == listinputs[k]) {
							k = (int)listinputs.size();
							found2 = true;
						}
					}
					if (!found2) {
						listinputs.push_back(selections[i]);
					}
					i = (int)selections.size();
					found1 = true;
				} 
			}
			if (!found1) {
				newlistitems.push_back(listheaders[j]);
			}
		}
		listheaders = newlistitems;
		loadListBoxes();
	}
}

void CLayerDataDlg::OnBnClickedRemove()
{
	vector<string> selections;
	int nCount = m_list2_ctrl.GetCount();
	for (int i = 0; i < nCount; i++) {
		if (m_list2_ctrl.GetSel(i)) {
			CString selection = _T("");
			m_list2_ctrl.GetText(i, selection);
			selections.push_back(string(CT2CA(selection)));
		}
	}
	if (selections.size() > 0) {
		vector<string> newlistitems;
		for (int j = 0; j < listinputs.size(); j++) {
			bool found1 = false;
			for (int i = 0; i < selections.size(); i++) {
				if (selections[i] == listinputs[j]) {
					bool found2 = false;
					for (int k = 0; k < listheaders.size(); k++) {
						if (selections[i] == string(CT2CA(listheaders[k]))) {
							k = (int)listheaders.size();
							found2 = true;
						}
					}
					if (!found2) {
						listheaders.push_back((CString)selections[i].c_str());
					}
					i = (int)selections.size();
					found1 = true;
				}
			}
			if (!found1) {
				newlistitems.push_back(listinputs[j]);
			}
		}
		listinputs = newlistitems;
		loadListBoxes();
	}
}

void CLayerDataDlg::OnLbnSelchangeList1()
{
	int sel = m_list1_ctrl.GetCurSel();
	if (sel >= 0) {
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			if (m_list1_ctrl.GetSel(sel)) {
				if (lastfieldindex >= 0) {
					m_list1_ctrl.SetSel(sel, false);
					if ((sel - lastfieldindex) > 0) {
						for (int i = lastfieldindex; i <= sel; i++) {
							m_list1_ctrl.SetSel(i, true);
						}
					}
					else if ((sel - lastfieldindex) < 0) {
						for (int i = sel; i <= lastfieldindex; i++) {
							m_list1_ctrl.SetSel(i, true);
						}
					}
				}
			}
		}
		if (m_list1_ctrl.GetSel(sel)) {
			lastfieldindex = sel;
		}
		else {
			lastfieldindex = -1;
		}
	}
}

void CLayerDataDlg::OnLbnDblclkList1()
{
	int sel = m_list1_ctrl.GetCurSel();
	if (sel >= 0) {
		//if (m_list1_ctrl.GetSel(sel)) {
			vector<CString> newlistitems;
			CString selection = _T("");
			m_list1_ctrl.GetText(sel, selection);
			for (int j = 0; j < listheaders.size(); j++) {
				bool found1 = false;
				if (selection == listheaders[j]) {
					bool found2 = false;
					for (int k = 0; k < listinputs.size(); k++) {
						if (selection == (CString)listinputs[k].c_str()) {
							k = (int)listinputs.size();
							found2 = true;
						}
					}
					if (!found2) {
						listinputs.push_back(string(CT2CA(selection)));
					}
					found1 = true;
				}
				if (!found1) {
					newlistitems.push_back(listheaders[j]);
				}
			}
			listheaders = newlistitems;
			loadListBoxes();
		//}
	}
}

void CLayerDataDlg::OnLbnSelcancelList1()
{
	// TODO: Add your control notification handler code here
}

void CLayerDataDlg::OnLbnSelchangeList2()
{
	int sel = m_list2_ctrl.GetCurSel();
	if (sel >= 0) {
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			if (m_list2_ctrl.GetSel(sel)) {
				if (lastdataindex >= 0) {
					m_list2_ctrl.SetSel(sel, true);
					if ((sel - lastdataindex) > 0) {
						for (int i = lastdataindex; i <= sel; i++) {
							m_list2_ctrl.SetSel(i, true);
						}
					}
					else if ((sel - lastdataindex) < 0) {
						for (int i = sel; i <= lastdataindex; i++) {
							m_list2_ctrl.SetSel(i, true);
						}
					}
				}
			}
		}
		if (m_list2_ctrl.GetSel(sel)) {
			lastdataindex = sel;
		}
		else {
			lastdataindex = -1;
		}
	}
}

void CLayerDataDlg::OnLbnDblclkList2()
{
	int sel = m_list2_ctrl.GetCurSel();
	if (sel >= 0) {
		vector<string> newlistitems;
		CString selection = _T("");
		m_list2_ctrl.GetText(sel, selection);
		for (int j = 0; j < listinputs.size(); j++) {
			bool found1 = false;
			if (selection == (CString)listinputs[j].c_str()) {
				bool found2 = false;
				for (int k = 0; k < listheaders.size(); k++) {
					if (selection == listheaders[k]) {
						k = (int)listheaders.size();
						found2 = true;
					}
				}
				if (!found2) {
					listheaders.push_back(selection);
				}
				found1 = true;
			}
			if (!found1) {
				newlistitems.push_back(listinputs[j]);
			}
		}
		listinputs = newlistitems;
		loadListBoxes();
	}
}

void CLayerDataDlg::OnLbnSelcancelList2()
{
	// TODO: Add your control notification handler code here
}

void CLayerDataDlg::addErrorString(CString err) {
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString(err);
}

