// CActivation.cpp : implementation file
//

#include "pch.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CActivation.h"
#include "Neuron.h"
#include "afxdialogex.h"


// CActivation dialog

IMPLEMENT_DYNAMIC(CActivation, CDialog)

CActivation::CActivation(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ACTIVATION, pParent)
{
	//mbrush.CreateSolidBrush(RGB(255, 0, 0));// GetGlobalData()->clrBarFace);

	mgraph.xlabel = _T("");
	mgraph.xlabel2 = _T("Sum Inputs x Weights");

	m_activation_function == _T("Sigmoid");
	m_leakage = _T("0.010000");
	m_bias_text = _T("0.00");
		
	mgraph.show_background = true;
	mgraph.minmaxy = 1;
	mgraph.maxminy = 0;
}

CActivation::~CActivation()
{
}

void CActivation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slider_ctrl);
	DDX_Control(pDX, IDC_BIAS_TEXT, m_bias_text_ctrl);
	DDX_Text(pDX, IDC_BIAS_TEXT, m_bias_text);
	DDX_Control(pDX, IDC_COMBO1, m_activation_function_control);
	DDX_CBString(pDX, IDC_COMBO1, m_activation_function);
	DDX_Text(pDX, IDC_SLEAK, m_leakage);
}


BEGIN_MESSAGE_MAP(CActivation, CDialog)
	//ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &CActivation::OnTRBNThumbPosChangingSlider1)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CActivation::OnNMReleasedcaptureSlider1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CActivation::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_SLEAK, &CActivation::OnEnChangeSleak)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CActivation message handlers

void CActivation::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages
}

BOOL CActivation::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	m_activation_function_control.AddString(_T("Sigmoid"));
	m_activation_function_control.AddString(_T("Tanh"));
	m_activation_function_control.AddString(_T("Relu"));
	m_activation_function_control.AddString(_T("Shockley"));
	//m_activation_function_control.AddString(_T("MaxPool"));
	//m_activation_function_control.AddString(_T("Multiply"));

	m_activation_function == _T("Sigmoid");
	m_activation_function_control.GetLBText(0, m_activation_function);

	m_slider_ctrl.SetRange(-1000, 1000);
	
	updateBias();

	updateGraph();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CActivation::updateGraph()
{
	UpdateData();

	int sel = m_activation_function_control.GetCurSel();
	if (sel < 0) {
		sel = 0;
	}
	m_activation_function_control.GetLBText(sel, m_activation_function);

	sxdata.clear();
	sydata1.clear();
	sydata2.clear();

	OutputNode node;

	double bias = (double)m_slider_ctrl.GetPos() / 100;
	double leakage = (double)_tstof((LPCTSTR)m_leakage);
	node.sigmoid_leakage = leakage;
	node.relu_leakage = leakage;

	int inc = 500;
	double xmin = -10;
	double xmax = 10;
	for (int i = 0; i <= inc; i++) {
		double x = xmin + ((double)i * (xmax - xmin)) / (double)inc;
		sxdata.push_back(x);
		if (m_activation_function == _T("Sigmoid")) {
			double y = node.sigmoid(x + bias);
			sydata1.push_back(y);
			sydata2.push_back(node.sigmoidDerivative(y)*x);
		}
		else if (m_activation_function == _T("Relu")) {
			double y = node.relu(x + bias);
			sydata1.push_back(y);
			sydata2.push_back(node.reluDerivative(y)*x);
		}
		else if (m_activation_function == _T("Tanh")) {
			double y = node.mtanh(x + bias);
			sydata1.push_back(y);
			sydata2.push_back(node.mtanhDerivative(y)*x);
		}
		else if (m_activation_function == _T("MaxPool")) {
			sydata1.push_back(x);
			sydata2.push_back(1);
		}
		else if (m_activation_function == _T("Shockley")) {
			double y = node.shockley(x + bias);
			sydata1.push_back(y);
			sydata2.push_back(node.shockleyDerivative(y)*x);
		}
	}

	mgraph.minmaxy = 1;
	mgraph.maxminy = 0;
	if (m_activation_function == _T("Tanh")) {
		mgraph.minmaxy = 1;
		mgraph.maxminy = -1;
	}
	else if (m_activation_function == _T("Relu")) {
		mgraph.minmaxy = 12;
		mgraph.maxminy = -4;
	}
	else if (m_activation_function == _T("Shockley")) {
		mgraph.minmaxy = 12;
		mgraph.maxminy = -4;
	}


	CClientDC dc(this);

	mgraph.setData(sxdata, sydata1, sydata2);
	mgraph.minx = -10;
	mgraph.maxx = 10;
	mgraph.drawFrame(&dc);
	mgraph.drawXAxis(&dc);
	mgraph.drawData(&dc);
	mgraph.drawData2(&dc);

}

void CActivation::setSize(CRect rect)
{
	MoveWindow(rect, true);
	CRect rect1;
	GetWindowRect(&rect1);
	
	CRect rect3;
	rect3.left = 200;
	rect3.top = 15;
	rect3.right = (rect1.right - rect1.left) - 5;
	rect3.bottom = (rect1.bottom - rect1.top) - 2 - 40;
	mgraph.setSize(rect3);

	int xoffset1 = 30;
	int xoffset2 = 30;

	CRect rect4;
	m_bias_text_ctrl.GetWindowRect(&rect4);
	m_bias_text_ctrl.MoveWindow(rect3.left + xoffset1 + 6, rect3.bottom + rect4.Height() + 0, rect4.Width(), rect4.Height(), true);

	CRect rect2;
	m_slider_ctrl.GetWindowRect(&rect2);
	m_slider_ctrl.MoveWindow(rect3.left + xoffset1 + rect4.Width(), rect3.bottom + rect2.Height() - 20, rect3.Width() - (2 * xoffset2) - (2 * rect4.Width()), rect2.Height(), true);

	updateGraph();

}

void CActivation::OnNMReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	updateBias();
	*pResult = 0;
}

void CActivation::updateBias()
{
	float pos = (float)m_slider_ctrl.GetPos()/100;
	m_bias_text.Format(_T("Bias = %.2f"), pos);
	UpdateData(false);
	updateGraph();
}

void CActivation::OnCbnSelchangeCombo1()
{
	updateGraph();
}

void CActivation::OnEnChangeSleak()
{
	updateGraph();
}

HBRUSH CActivation::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	HBRUSH hbr = mbrush;

	if (pWnd->GetDlgCtrlID() == IDC_TAB1) {
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	else {

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
			pDC->SetBkMode(TRANSPARENT);
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

	}

	return hbr;
}

void CActivation::OnOK()
{
	//CDialog::OnOK();
}

void CActivation::OnCancel()
{
	//CDialog::OnCancel();
}
