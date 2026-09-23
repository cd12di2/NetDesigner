// CFileNameParser.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CFileNameParser.h"
#include "AuxFunctions.h"
#include "NormalizationFactor.h"
#include "afxdialogex.h"


// CFileNameParser dialog

IMPLEMENT_DYNAMIC(CFileNameParser, CDialog)

CFileNameParser::CFileNameParser(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IMAGE_NAME_PARSER, pParent)
{
	data_updated = false;
	tanh = false;
	m_key = _T("");
	nfactormin = 0.2;
	nfactormax = 0.8;
}

CFileNameParser::~CFileNameParser()
{
}

void CFileNameParser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAME1, m_name1_ctrl);
	DDX_Control(pDX, IDC_LIST2, m_list_ctrl2);
	DDX_Control(pDX, IDC_LIST3, m_list_ctrl3);
	DDX_Control(pDX, IDC_NAME2, m_name2_ctrl);
	DDX_Control(pDX, IDC_NAME3, m_name3_ctrl);
	DDX_Control(pDX, IDC_EDIT1, m_key_ctrl);
	DDX_Text(pDX, IDC_EDIT1, m_key);
	DDX_Control(pDX, IDC_LINE1, m_line1_ctrl);
	DDX_Control(pDX, IDC_NORM_OPTIONS, m_scale_ctrl);
}


BEGIN_MESSAGE_MAP(CFileNameParser, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST3, &CFileNameParser::OnLbnSelchangeList3)
	ON_BN_CLICKED(IDC_NORM_OPTIONS, &CFileNameParser::OnBnClickedNormOptions)
END_MESSAGE_MAP()


// CFileNameParser message handlers

void CFileNameParser::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

void CFileNameParser::setSize(CRect rect)
{
	int conv_width = rect.Width() / 3;//  160;
	CRect rect3;

	MoveWindow(rect, true);

	int t1 = 8;
	int t2 = 44;
	int t3 = 33;

	rect3.left = 13;
	rect3.top = t1;
	rect3.right = rect3.left + 70;
	rect3.bottom = rect3.top + 17;
	m_name3_ctrl.MoveWindow(rect3);

	rect3.left = rect3.right + 10;
	rect3.top = rect3.top - 2;
	rect3.right = rect.right - 12 - conv_width - 5;
	rect3.bottom = rect3.bottom + 2;
	m_key_ctrl.MoveWindow(rect3);

	CRect rect8;
	rect8.right = rect.right - 20;
	rect8.left = rect8.right - 60;
	rect8.top = rect3.top - 4;
	rect8.bottom = rect3.bottom + 4;
	m_scale_ctrl.MoveWindow(rect8);

	rect3.left = 10;
	rect3.top = t3;
	rect3.right = rect.right - 12;
	rect3.bottom = rect3.top + 1;
	m_line1_ctrl.MoveWindow(rect3);

	rect3.left = 13;
	rect3.top = t2;
	rect3.right = rect3.left + 60;
	rect3.bottom = rect3.top + 17;
	m_name1_ctrl.MoveWindow(rect3);
	
	rect3.left = 10;
	rect3.top = rect3.top + 19;
	rect3.right = rect.right - 12 - conv_width - 5;
	rect3.bottom = rect.bottom - 37;
	m_list_ctrl2.MoveWindow(rect3);

	rect3.left = rect.right - 12 - conv_width + 3;
	rect3.top = t2;
	rect3.right = rect3.left + 60;
	rect3.bottom = rect3.top + 17;
	m_name2_ctrl.MoveWindow(rect3);

	rect3.left = rect.right - 12 - conv_width;
	rect3.top = rect3.top + 19;
	rect3.right = rect.right - 12;
	rect3.bottom = rect.bottom - 37;
	m_list_ctrl3.MoveWindow(rect3);

}

CString CFileNameParser::getMode()
{
	return(_T("FNParser"));
}

void CFileNameParser::setMode(CString mode)
{
}

void CFileNameParser::setNameKey(CString key)
{
	if (::IsWindow(m_list_ctrl3.m_hWnd)) {
		m_key = key;
		m_list_ctrl3.SelectString(-1, m_key);
		UpdateData(false);
	}
	/*int index = m_list_ctrl3.FindString(0, m_key);
	if (index >= 0) {
		CString s = _T("");
		s.Format(_T("setNameKey index = %i"), index);
		addErrorString(s);
	}*/
}

CString CFileNameParser::getNameKey()
{
	//addErrorString(_T("CFileNameParser::getNameKey : ") + m_key);
	return(m_key);
}

double CFileNameParser::getRealLowerNormalizationFactor()
{
	return(nfactormin);
}

void CFileNameParser::setRealLowerNormalizationFactor(double factor)
{
	nfactormin = factor;
}

double CFileNameParser::getRealUpperNormalizationFactor()
{
	return(nfactormax);
}

void CFileNameParser::setRealUpperNormalizationFactor(double factor)
{
	nfactormax = factor;
}

void CFileNameParser::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

BOOL CFileNameParser::OnInitDialog()
{
	CDialog::OnInitDialog();

	setListBox();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileNameParser::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CFileNameParser::setListBox()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	int npnames = (int)pDoc->getDataPathNamesSize(false);
	if (npnames > 100) {
		npnames = 100;
	}
	m_list_ctrl2.ResetContent();
	for (int i = 0; i < npnames; i++) {
		m_list_ctrl2.AddString(pDoc->getDataPathName(i, false));
	}

	CAuxFunctions func;

	vector<CString> datanames = func.getDataNamesFromFilenames(pDoc->getDataPathNames(false));

	m_list_ctrl3.ResetContent();
	for (int i = 0; i < datanames.size(); i++) {
		m_list_ctrl3.AddString(datanames[i]);
	}

}

void CFileNameParser::OnLbnSelchangeList3()
{
	int sel = m_list_ctrl3.GetCurSel();
	if (sel < 0) {
		sel = 0;
	}
	m_list_ctrl3.GetText(sel, m_key);
	UpdateData(false);
}

void CFileNameParser::OnBnClickedNormOptions()
{
	NormalizationFactor dlg;
	if (tanh) {
		dlg.m_lower.Format(_T("%.3f"), nfactormin);
	}
	else {
		dlg.m_lower.Format(_T("%.3f"), nfactormin);
	}
	dlg.m_upper.Format(_T("%.3f"), nfactormax);
	if (dlg.DoModal() == IDOK) {
		nfactormin = (double)_tstof((LPCTSTR)dlg.m_lower);
		nfactormax = (double)_tstof((LPCTSTR)dlg.m_upper);
	}
}
