// CFieldKeyDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CFieldKeyDlg.h"
#include "AuxFunctions.h"
#include "NormalizationFactor.h"
#include "afxdialogex.h"


// CFieldKeyDlg dialog

IMPLEMENT_DYNAMIC(CFieldKeyDlg, CDialog)

CFieldKeyDlg::CFieldKeyDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FIELD_KEY, pParent)
{
	data_updated = false;
	tanh = false;
	m_edit1 = _T("");
	nfactormin = 0.2;
	nfactormax = 0.8;
}

CFieldKeyDlg::~CFieldKeyDlg()
{
}

void CFieldKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit1_ctrl);
	DDX_Text(pDX, IDC_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_LIST3, m_list3_ctrl);
	DDX_Control(pDX, IDC_NAME3, m_name3_ctrl);
	DDX_Control(pDX, IDC_NAME2, m_name2_ctrl);
	DDX_Control(pDX, IDC_LINE1, m_line1_ctrl);
	DDX_Control(pDX, IDC_NORM_OPTIONS, m_scale_ctrl);
}


BEGIN_MESSAGE_MAP(CFieldKeyDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST3, &CFieldKeyDlg::OnLbnSelchangeList3)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_NORM_OPTIONS, &CFieldKeyDlg::OnBnClickedNormOptions)
END_MESSAGE_MAP()


// CFieldKeyDlg message handlers

void CFieldKeyDlg::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

BOOL CFieldKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CFieldKeyDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages
}

void CFieldKeyDlg::setSize(CRect mrect)
{
	CRect rect = mrect;

	MoveWindow(rect, true);

	int t0 = 18;
	int t1 = 18;

	CRect rect3;
	rect3.left = t0 + 3;
	rect3.top = t1;
	rect3.right = rect3.left + 60;
	rect3.bottom = rect3.top + 17;
	m_name3_ctrl.MoveWindow(rect3);

	CRect rect4;
	rect4.left = rect3.right + 10;
	rect4.top = rect3.top - 2;
	rect4.right = rect.right - 100;
	rect4.bottom = rect3.bottom + 2;
	m_edit1_ctrl.MoveWindow(rect4);

	CRect rect8;
	rect8.right = rect.right - 20;
	rect8.left = rect8.right - 60;
	rect8.top = rect3.top - 6;
	rect8.bottom = rect3.bottom + 6;
	m_scale_ctrl.MoveWindow(rect8);

	CRect rect7;
	rect7.left = t0 - 2;
	rect7.top = rect3.bottom + 10;
	rect7.right = rect.right - 18;
	rect7.bottom = rect7.top + 1;
	m_line1_ctrl.MoveWindow(rect7);

	CRect rect5;
	rect5.left = t0 + 3;
	rect5.top = rect3.bottom + 20;
	rect5.right = rect5.left + 120;
	rect5.bottom = rect5.top + 17;
	m_name2_ctrl.MoveWindow(rect5);

	CRect rect6;
	rect6.left = t0 + 1;
	rect6.top = rect5.bottom + 4;
	rect6.right = rect.right - 20;
	rect6.bottom = rect.bottom - 46;
	m_list3_ctrl.MoveWindow(rect6);

}

CString CFieldKeyDlg::getMode()
{
	return(_T("FieldKey"));
}

void CFieldKeyDlg::setMode(CString mode)
{
}

void CFieldKeyDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

void CFieldKeyDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

BOOL CFieldKeyDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::DestroyWindow();
}

void CFieldKeyDlg::OnLbnSelchangeList3()
{
	int sel = m_list3_ctrl.GetCurSel();
	if (sel < 0) {
		sel = 0;
	}
	m_list3_ctrl.GetText(sel, m_edit1);
	UpdateData(false);
}

void CFieldKeyDlg::setNameKey(CString key)
{
	if (::IsWindow(m_list3_ctrl.m_hWnd)) {
		m_edit1 = key;
		m_list3_ctrl.SelectString(-1, m_edit1);
		UpdateData(false);
	}
	/*int index = m_list3_ctrl.FindString(0, m_edit1);
	if (index >= 0) {
		//m_list3_ctrl.SetSel(index);
		CString s = _T("");
		s.Format(_T("setNameKey index = %i : "), index);
		addErrorString(s + m_edit1);
	}*/
}

CString CFieldKeyDlg::getNameKey()
{
	//addErrorString(_T("CFieldKeyDlg::getNameKey : ") + m_edit1);
	return(m_edit1);
}

double CFieldKeyDlg::getRealLowerNormalizationFactor()
{
	return(nfactormin);
}

void CFieldKeyDlg::setRealLowerNormalizationFactor(double factor)
{
	nfactormin = factor;
}

double CFieldKeyDlg::getRealUpperNormalizationFactor()
{
	return(nfactormax);
}

void CFieldKeyDlg::setRealUpperNormalizationFactor(double factor)
{
	nfactormax = factor;
}

void CFieldKeyDlg::setData(vector<vector<CString>> stringdata)
{
	CAuxFunctions func;
	if (!data_updated && !func.areDataNumbers(stringdata)) {
		vector<CString> keys;
		for (int i = 0; i < stringdata.size(); i++) {
			for (int j = 0; j < stringdata[i].size(); j++) {
				bool found = false;
				for (int k = 0; k < keys.size(); k++) {
					if (keys[k] == stringdata[i][j]) {
						found = true;
						k = (int)keys.size();
					}
				}
				if (!found) {
					keys.push_back(stringdata[i][j]);
				}
			}
		}
		m_list3_ctrl.ResetContent();
		for (int i = 0; i < keys.size(); i++) {
			m_list3_ctrl.AddString(keys[i]);
		}
	}
}

void CFieldKeyDlg::OnBnClickedNormOptions()
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
