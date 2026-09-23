// CAssemblyPicker.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CAssemblyPicker.h"
#include "afxdialogex.h"


// CAssemblyPicker dialog

IMPLEMENT_DYNAMIC(CAssemblyPicker, CDialog)

bool a_new = false;
bool a_select = true;
CString a_assembly = _T("");


CAssemblyPicker::CAssemblyPicker(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ASSEMBLY_PICKER_DLG, pParent)
{
	spt = CPoint(0, 0);
	m_new = false;
	m_select = true;
	m_assembly = _T("");
}

CAssemblyPicker::~CAssemblyPicker()
{
}

void CAssemblyPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_NEW_ASSEMBLY, m_new);
	DDX_Control(pDX, IDC_COMBO1, m_assemblies_ctrl);
	DDX_Radio(pDX, IDC_EXISTING_ASSEMBLY, m_select);
	DDX_CBString(pDX, IDC_COMBO1, m_assembly);
}


BEGIN_MESSAGE_MAP(CAssemblyPicker, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_NEW_ASSEMBLY, &CAssemblyPicker::OnBnClickedNewAssembly)
	ON_BN_CLICKED(IDC_EXISTING_ASSEMBLY, &CAssemblyPicker::OnBnClickedExistingAssembly)
END_MESSAGE_MAP()


// CAssemblyPicker message handlers


BOOL CAssemblyPicker::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);
	
	if (assemblies.size() > 0) {
		bool found = false;
		for (int i = 0; i < (int)assemblies.size(); i++) {
			m_assemblies_ctrl.AddString((CString)assemblies[i].c_str());
			if (a_assembly == (CString)assemblies[i].c_str()) {
				found = true;
			}
		}
		if (found) {
			m_assembly = a_assembly;
		}
		else {
			m_assembly = (CString)assemblies[0].c_str();
		}
		m_new = a_new;
		m_select = a_select;
	}
	else {
		m_new = false;
		m_select = true;
	}
	if (m_select) {
		m_assemblies_ctrl.EnableWindow(false);
	}

	CRect rect;
	GetWindowRect(&rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	rect.left = spt.x - width / 2;
	rect.top = spt.y - height / 2;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MoveWindow(rect, true);

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CAssemblyPicker::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	hbr = mbrush;

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

void CAssemblyPicker::OnOK()
{
	UpdateData();
	a_new = m_new;
	a_select = m_select;
	a_assembly = m_assembly;
	CDialog::OnOK();
}

void CAssemblyPicker::OnBnClickedNewAssembly()
{
	m_assemblies_ctrl.EnableWindow(false);
}

void CAssemblyPicker::OnBnClickedExistingAssembly()
{
	m_assemblies_ctrl.EnableWindow(true);
}
