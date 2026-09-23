// CAutoNetConfigDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CAutoNetConfigDlg.h"
#include "afxdialogex.h"


// CAutoNetConfigDlg dialog

bool pinputs = true;
bool phidden = true;
bool poutputs = true;
bool pnormalize = true;
int pnum_inputs = 3;
int pnum_hidden = 2;
int pnum_outputs = 3;
int phidden_width = 4;
CString pfunction = _T("Sigmoid");
double peta = 0.002;
double palpha = 0.8;
double prand = 0.2;
bool compact = true;

IMPLEMENT_DYNAMIC(CAutoNetConfigDlg, CDialogEx)

CAutoNetConfigDlg::CAutoNetConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AUTONET, pParent)
{
	spt = CPoint(0, 0);
	m_inputs = pinputs;
	m_hidden = phidden;
	m_outputs = poutputs;
	m_normalize = pnormalize;
	m_compact = compact;
	m_num_inputs.Format(_T("%i"),pnum_inputs);
	m_num_hidden.Format(_T("%i"), pnum_hidden);
	m_num_outputs.Format(_T("%i"), pnum_outputs);
	m_hidden_width.Format(_T("%i"), phidden_width);
	m_activation_function = pfunction;
	m_eta.Format(_T("%.8f"), peta);
	m_alpha.Format(_T("%.6f"), palpha);
	m_rand.Format(_T("%.6f"), prand);
}

CAutoNetConfigDlg::~CAutoNetConfigDlg()
{
}

void CAutoNetConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_INPUT_NODES, m_inputs);
	DDX_Check(pDX, IDC_HIDDEN_NODES, m_hidden);
	DDX_Check(pDX, IDC_OUTPUT_NODE2, m_outputs);
	DDX_Text(pDX, IDC_NUM_INPUTS, m_num_inputs);
	DDX_Text(pDX, IDC_NUM_LAYERS, m_num_hidden);
	DDX_Text(pDX, IDC_NUM_OUTPUTS, m_num_outputs);
	DDX_Text(pDX, IDC_HIDDEN_WIDTH, m_hidden_width);
	DDX_Control(pDX, IDC_COMBO1, m_activation_function_control);
	DDX_CBString(pDX, IDC_COMBO1, m_activation_function);
	DDX_Text(pDX, IDC_ETA, m_eta);
	DDX_Text(pDX, IDC_ALPHA, m_alpha);
	DDX_Text(pDX, IDC_RAND, m_rand);
	DDX_Control(pDX, IDC_NUM_INPUTS, m_num_inputs_ctrl);
	DDX_Control(pDX, IDC_NUM_LAYERS, m_num_layers_ctrl);
	DDX_Control(pDX, IDC_HIDDEN_WIDTH, m_hidden_width_ctrl);
	DDX_Control(pDX, IDC_NUM_OUTPUTS, m_num_outputs_ctrl);
	DDX_Check(pDX, IDC_NORMALIZE, m_normalize);
	DDX_Check(pDX, IDC_COMPACT, m_compact);
}


BEGIN_MESSAGE_MAP(CAutoNetConfigDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_INPUT_NODES, &CAutoNetConfigDlg::OnBnClickedInputNodes)
	ON_BN_CLICKED(IDC_HIDDEN_NODES, &CAutoNetConfigDlg::OnBnClickedHiddenNodes)
	ON_BN_CLICKED(IDC_OUTPUT_NODE2, &CAutoNetConfigDlg::OnBnClickedOutputNode2)
END_MESSAGE_MAP()


// CAutoNetConfigDlg message handlers


BOOL CAutoNetConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	m_activation_function_control.AddString(_T("Sigmoid"));
	m_activation_function_control.AddString(_T("Relu"));
	m_activation_function_control.AddString(_T("Tanh"));
	m_activation_function_control.AddString(_T("MaxPool"));

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CString dfuncn = pDoc->dfuncn;
	double tdeta = pDoc->deta;
	double tdalpha = pDoc->dalpha;
	double tdrand = pDoc->drand;

	m_activation_function == dfuncn;
	m_activation_function_control.GetLBText(0, m_activation_function);
	m_eta.Format(_T("%.8lf"), tdeta);
	m_alpha.Format(_T("%.6lf"), tdalpha);
	m_rand.Format(_T("%.6lf"), tdrand);

	CRect rect;
	GetWindowRect(&rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	rect.left = spt.x - 45;
	rect.top = spt.y - 55;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MoveWindow(rect, true);

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoNetConfigDlg::OnOK()
{
	UpdateData();

	pinputs = m_inputs;
	phidden = m_hidden;
	poutputs = m_outputs;
	pnormalize = m_normalize;
	compact = m_compact;

	pnum_inputs = (int)_tstof((LPCTSTR)m_num_inputs);
	pnum_hidden = (int)_tstof((LPCTSTR)m_num_hidden);
	pnum_outputs = (int)_tstof((LPCTSTR)m_num_outputs);
	phidden_width = (int)_tstof((LPCTSTR)m_hidden_width);

	pfunction = m_activation_function;
	peta = (double)_tstof((LPCTSTR)m_eta);
	palpha = (double)_tstof((LPCTSTR)m_alpha);
	prand = (double)_tstof((LPCTSTR)m_rand);

	CDialogEx::OnOK();
}

NetData CAutoNetConfigDlg::makeNet(CPoint mpt, int nseparation, int nsnap, int gindex)
{
	int nginputs = pnum_inputs;
	int ngoutputs = pnum_outputs;
	int nghiddenlayers = pnum_hidden;
	int nghiddenwidth = phidden_width;

	string gfunction = string(CT2CA(pfunction));
	double geta = peta;
	double galpha = palpha;
	double grand = prand;

	int nlayers = 0;
	if (pinputs) {
		nlayers++;
	}
	if (phidden) {
		nlayers += nghiddenlayers;
	}
	if (pnormalize) {
		nlayers++;
	}
	if (poutputs) {
		nlayers++;
	}
	int nwidth = nginputs;
	if (ngoutputs > nwidth) {
		nwidth = ngoutputs;
	}
	if (nghiddenwidth > nwidth) {
		nwidth = nghiddenwidth;
	}
	int node_separation = nseparation;// 32;
	int layer_separation = 6 * nsnap;// 48;
	int xoffset = layer_separation * (nlayers / 2);
	int yoffset = node_separation * (nwidth / 2);

	int nindex_offset = 0;
	int ncindex_offset = 0;
	int x = mpt.x;
	int y = mpt.y;

	vector<twoValues> points;

	newGroupData.clear();

	tdata.setType(NODE);
	tdata.stringParamListLabels.push_back("Mode");
	tdata.stringParamList.push_back("Input");
	tdata.stringParamListLabels.push_back("DataField");
	tdata.stringParamList.push_back("");
	tdata.stringParamListLabels.push_back("Function");
	tdata.stringParamList.push_back(gfunction);
	tdata.doubleParamListLabels.push_back("Eta");
	tdata.doubleParamList.push_back(geta);
	tdata.doubleParamListLabels.push_back("Alpha");
	tdata.doubleParamList.push_back(galpha);
	tdata.doubleParamListLabels.push_back("Rand");
	tdata.doubleParamList.push_back(grand);

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	x = x - xoffset;
	if (pinputs) {
		for (int i = 0; i < nginputs; i++) {
			int my = y + (i * node_separation) - ((nginputs * node_separation) / 2);
			//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
			char sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + 1 + nindex_offset);
			tdata.setID(sid);
			tdata.setLabel(sid);
			tdata.setXDisp(x);
			tdata.setYDisp(my);
			tdata.setNodeMode("Input");
			newGroupData.pushNodeData(tdata);
			nindex_offset++;
		}
		if (pnormalize) {
			x = x + (4 * nsnap);// layer_separation / 2;
			for (int i = 0; i < nginputs; i++) {
				int my = y + (i * node_separation) - ((nginputs * node_separation) / 2);
				//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
				char sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + 1 + nindex_offset);
				tdata.setID(sid);
				tdata.setLabel(sid);
				tdata.setXDisp(x);
				tdata.setYDisp(my);
				tdata.setNodeMode("PreProcess");
				newGroupData.pushNodeData(tdata);
				nindex_offset++;
				vector<double> mweights;
				//cid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + ncindex_offset);
				char cid[100]; sprintf_s(sid, "Connect%i", pDoc->getLastConnectIndex() + 1 + ncindex_offset);
				newGroupData.pushConnectData(cid, cid, newGroupData.getID(ncindex_offset), sid, gindex, 0.0, mweights, points);
				ncindex_offset++;
			}
		}
	}
	if (phidden) {
		for (int j = 0; j < nghiddenlayers; j++) {
			x = x + layer_separation;
			for (int i = 0; i < nghiddenwidth; i++) {
				int my = y + (i * node_separation) - ((nghiddenwidth * node_separation) / 2);
				//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
				char sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + 1 + nindex_offset);
				tdata.setID(sid);
				tdata.setLabel(sid);
				tdata.setXDisp(x);
				tdata.setYDisp(my);
				tdata.setNodeMode("Hidden");
				newGroupData.pushNodeData(tdata);
				nindex_offset++;
			}
		}
	}
	if (poutputs) {
		x = x + layer_separation;
		for (int i = 0; i < ngoutputs; i++) {
			int my = y + (i * node_separation) - ((ngoutputs * node_separation) / 2);
			//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
			char sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + 1 + nindex_offset);
			tdata.setID(sid);
			tdata.setLabel(sid);
			tdata.setXDisp(x);
			tdata.setYDisp(my);
			tdata.setNodeMode("Output");
			newGroupData.pushNodeData(tdata);
			nindex_offset++;
		}
	}

	return(newGroupData);

}

HBRUSH CAutoNetConfigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

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

void CAutoNetConfigDlg::OnBnClickedInputNodes()
{
	UpdateData();
	if (m_inputs) {
		m_num_inputs_ctrl.EnableWindow(true);
	}
	else {
		m_num_inputs_ctrl.EnableWindow(false);
	}
}

void CAutoNetConfigDlg::OnBnClickedHiddenNodes()
{
	UpdateData();
	if (m_hidden) {
		m_num_layers_ctrl.EnableWindow(true);
		m_hidden_width_ctrl.EnableWindow(true);
	}
	else {
		m_num_layers_ctrl.EnableWindow(false);
		m_hidden_width_ctrl.EnableWindow(false);
	}
}

void CAutoNetConfigDlg::OnBnClickedOutputNode2()
{
	UpdateData();
	if (m_outputs) {
		m_num_outputs_ctrl.EnableWindow(true);
	}
	else {
		m_num_outputs_ctrl.EnableWindow(false);
	}
}

