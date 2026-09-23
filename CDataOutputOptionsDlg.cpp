// CDataOutputOptionsDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "CDataOutputOptionsDlg.h"
#include "afxdialogex.h"


// CDataOutputOptionsDlg dialog

IMPLEMENT_DYNAMIC(CDataOutputOptionsDlg, CDialog)

bool net_output = true;
bool train_output = true;
bool train_input = true;
bool classification = true;

CDataOutputOptionsDlg::CDataOutputOptionsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SAVE_DATA_OPTIONS, pParent)
{
	m_notes = _T("");
	m_net_output = net_output;
	m_train_output = train_output;
	m_train_input = train_input;
	m_classification = classification;
}

CDataOutputOptionsDlg::~CDataOutputOptionsDlg()
{
}

void CDataOutputOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_NET_OUTPUT_DATA, m_net_output);
	DDX_Control(pDX, IDC_NET_OUTPUT_DATA, m_net_output_ctrl);
	DDX_Check(pDX, IDC_TRAIN_OUTPUT_DATA, m_train_output);
	DDX_Control(pDX, IDC_TRAIN_OUTPUT_DATA, m_train_output_ctrl);
	DDX_Check(pDX, IDC_TRAIN_INPUT_DATA, m_train_input);
	DDX_Control(pDX, IDC_TRAIN_INPUT_DATA, m_train_input_ctrl);
	DDX_Check(pDX, IDC_CLASSIFICATION_MATRICES, m_classification);
	DDX_Control(pDX, IDC_CLASSIFICATION_MATRICES, m_classification_ctrl);
	DDX_Control(pDX, IDC_STATIC2, m_static1_ctrl);
	DDX_Control(pDX, IDC_STATIC3, m_static2_ctrl);
	DDX_Control(pDX, IDC_STATIC4, m_static3_ctrl);
	DDX_Control(pDX, IDC_STATIC1, m_static4_ctrl);
	DDX_Text(pDX, IDC_NOTES, m_notes);
}


BEGIN_MESSAGE_MAP(CDataOutputOptionsDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDataOutputOptionsDlg message handlers

void CDataOutputOptionsDlg::OnOK()
{
	UpdateData();
	net_output = m_net_output;
	train_output = m_train_output;
	train_input = m_train_input;
	classification = m_classification;
	CDialog::OnOK();
}

BOOL CDataOutputOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDataOutputOptionsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
