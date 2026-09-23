// CInputOptimization.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CInputOptimization.h"
#include "afxdialogex.h"
#include "AuxFunctions.h"


// CInputOptimization dialog

IMPLEMENT_DYNAMIC(CInputOptimization, CDialog)

CInputOptimization::CInputOptimization(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INPUT_OPTIMIZATION, pParent)
{
	m_optimize_inputs = false;
	m_train_error = 0;
	m_list1 = _T("");
}

CInputOptimization::~CInputOptimization()
{
}

void CInputOptimization::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVALUATE_INPUTS, m_optimize_inputs_ctrl);
	DDX_Check(pDX, IDC_EVALUATE_INPUTS, m_optimize_inputs);
	DDX_Control(pDX, IDC_OPTIMIZE_TRAIN, m_train_error_ctrl);
	DDX_Control(pDX, IDC_OPTIMIZE_VALIDATION, m_validation_error_ctrl);
	DDX_Radio(pDX, IDC_OPTIMIZE_TRAIN, m_train_error);
	DDX_Control(pDX, IDC_LIST1, m_list1_ctrl);
	DDX_LBString(pDX, IDC_LIST1, m_list1);
}


BEGIN_MESSAGE_MAP(CInputOptimization, CDialog)
	ON_BN_CLICKED(IDC_EVALUATE_INPUTS, &CInputOptimization::OnBnClickedEvaluateInputs)
	ON_BN_CLICKED(IDC_OPTIMIZE_TRAIN, &CInputOptimization::OnBnClickedOptimizeTrain)
	ON_BN_CLICKED(IDC_OPTIMIZE_VALIDATION, &CInputOptimization::OnBnClickedOptimizeValidation)
END_MESSAGE_MAP()


// CInputOptimization message handlers


BOOL CInputOptimization::OnInitDialog()
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CDialog::OnInitDialog();

	m_optimize_inputs = pDoc->evaluate_inputs;

	if (pDoc->optimize_validation && ((pDoc->use_validation_percent && (pDoc->validation_percent > 0)) || pDoc->use_validation_file)) {
		m_train_error = 1;
	}
	else {
		m_train_error = 0;
	}

	if (!((pDoc->use_validation_percent && (pDoc->validation_percent > 0)) || pDoc->use_validation_file)) {
		m_validation_error_ctrl.EnableWindow(false);
	}

	displaySetupData();

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputOptimization::setSize(CRect mrect)
{
	int t0 = 156;
	int t1 = 10;
	CRect rect = mrect;
	MoveWindow(rect, true);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	CRect rect1;
	m_list1_ctrl.GetWindowRect(&rect1);
	rect1.left = t0;
	rect1.top = t1;
	rect1.right = rect.right - 10;
	rect1.bottom = (rect.bottom - rect.top) - 10;
	m_list1_ctrl.MoveWindow(rect1);
}

void CInputOptimization::displaySetupData()
{
	CAuxFunctions func;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CString s = _T("");

	float train_time = pDoc->trainTime * 60 * 60;
	s.Format(_T("train time = %i seconds"), (int)train_time);
	addLine1(s);

	vector<string> inputs = func.getNetInputNodes();
	int connected_to_output = 0;
	if (inputs.size() > 0) {
		for (int i = 0; i < (int)inputs.size(); i++) {
			string input = func.getInputNodeName(inputs[i]);
			string input_field = func.getInputFieldFromNodeName(input);
			if (input_field.substr(0, 7) == "Output:") {
				connected_to_output++;
			}
		}
	}
	s.Format(_T("num inputs = %i"), (int)inputs.size());
	if (connected_to_output > 0) {
		s.Format(_T("num inputs = %i (num connected to outputs = %i)"), (int)inputs.size(), connected_to_output);
	}
	addLine1(s);

}

void CInputOptimization::addLine1(CString line)
{
	m_list1_ctrl.InsertString(0, line);
	while (m_list1_ctrl.GetCount() > 40) {
		m_list1_ctrl.DeleteString(m_list1_ctrl.GetCount() - 1);
	}
}

void CInputOptimization::OnBnClickedEvaluateInputs()
{
	UpdateData();
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->evaluate_inputs = m_optimize_inputs;
}

void CInputOptimization::OnBnClickedOptimizeTrain()
{
	setOptimization();
}

void CInputOptimization::OnBnClickedOptimizeValidation()
{
	setOptimization();
}

void CInputOptimization::setOptimization()
{
	UpdateData();
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (m_train_error == 0) {
		pDoc->optimize_train = true;
		pDoc->optimize_validation = false;
	}
	else {
		pDoc->optimize_train = false;
		pDoc->optimize_validation = true;
	}
}