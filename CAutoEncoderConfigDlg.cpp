// CAutoEncoderConfigDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CAutoEncoderConfigDlg.h"
#include "afxdialogex.h"


// CAutoEncoderConfigDlg dialog

bool pnormalize3 = true;
int pnum_inputs3 = 3;
int pnum_hidden3 = 2;
int phidden_width3 = 4;
CString pfunction3 = _T("Sigmoid");
double peta3 = 0.002;
double palpha3 = 0.8;
double prand3 = 0.2;

IMPLEMENT_DYNAMIC(CAutoEncoderConfigDlg, CDialog)

CAutoEncoderConfigDlg::CAutoEncoderConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_AUTOENCODER, pParent)
{
	spt = CPoint(0, 0);
	m_normalize = pnormalize3;
	m_num_inputs.Format(_T("%i"), pnum_inputs3);
	m_num_hidden.Format(_T("%i"), pnum_hidden3);
	m_hidden_width.Format(_T("%i"), phidden_width3);
	m_activation_function = pfunction3;
	m_eta.Format(_T("%.8f"), peta3);
	m_alpha.Format(_T("%.6f"), palpha3);
	m_rand.Format(_T("%.6f"), prand3);
}

CAutoEncoderConfigDlg::~CAutoEncoderConfigDlg()
{
}

void CAutoEncoderConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_NORMALIZE, m_normalize);
	DDX_Text(pDX, IDC_NUM_INPUTS, m_num_inputs);
	DDX_Text(pDX, IDC_NUM_LAYERS, m_num_hidden);
	DDX_Text(pDX, IDC_HIDDEN_WIDTH, m_hidden_width);
	DDX_CBString(pDX, IDC_COMBO1, m_activation_function);
	DDX_Text(pDX, IDC_ETA, m_eta);
	DDX_Text(pDX, IDC_ALPHA, m_alpha);
	DDX_Text(pDX, IDC_RAND, m_rand);
	DDX_Control(pDX, IDC_COMBO1, m_activation_function_control);
}


BEGIN_MESSAGE_MAP(CAutoEncoderConfigDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAutoEncoderConfigDlg message handlers

BOOL CAutoEncoderConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

void CAutoEncoderConfigDlg::OnOK()
{
	UpdateData();

	pnormalize3 = m_normalize;

	pnum_inputs3 = (int)_tstof((LPCTSTR)m_num_inputs);
	pnum_hidden3 = (int)_tstof((LPCTSTR)m_num_hidden);
	phidden_width3 = (int)_tstof((LPCTSTR)m_hidden_width);

	pfunction3 = m_activation_function;
	peta3 = (double)_tstof((LPCTSTR)m_eta);
	palpha3 = (double)_tstof((LPCTSTR)m_alpha);
	prand3 = (double)_tstof((LPCTSTR)m_rand);

	CDialog::OnOK();
}

HBRUSH CAutoEncoderConfigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

NetData CAutoEncoderConfigDlg::makeNet(CPoint mpt, int gindex)
{
	int nginputs = pnum_inputs3;
	int ngoutputs = pnum_inputs3;
	int nghiddenlayers = pnum_hidden3;
	int nghiddenwidth = phidden_width3;

	string gfunction = string(CT2CA(pfunction3));
	double geta = peta3;
	double galpha = palpha3;
	double grand = prand3;

	int nlayers = 0;
	nlayers++;
	nlayers += nghiddenlayers;
	if (pnormalize3) {
		nlayers++;
	}
	nlayers++;

	int nwidth = nginputs;
	if (ngoutputs > nwidth) {
		nwidth = ngoutputs;
	}
	if (nghiddenwidth > nwidth) {
		nwidth = nghiddenwidth;
	}
	int node_separation = 32;
	int layer_separation = 48;
	int xoffset = layer_separation * (nlayers / 2);
	int yoffset = node_separation * (nwidth / 2);

	CString sid = _T("");
	CString cid = _T("");
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
	for (int i = 0; i < nginputs; i++) {
		int my = y + (i * node_separation) - ((nginputs * node_separation) / 2);
		sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
		tdata.setID(string(CT2CA(sid)));
		tdata.setLabel(string(CT2CA(sid)));
		tdata.setXDisp(x);
		tdata.setYDisp(my);
		tdata.setNodeMode("Input");
		newGroupData.pushNodeData(tdata);
		nindex_offset++;
	}
	if (pnormalize3) {
		x = x + layer_separation;
		for (int i = 0; i < nginputs; i++) {
			int my = y + (i * node_separation) - ((nginputs * node_separation) / 2);
			sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
			tdata.setID(string(CT2CA(sid)));
			tdata.setLabel(string(CT2CA(sid)));
			tdata.setXDisp(x);
			tdata.setYDisp(my);
			tdata.setNodeMode("PreProcess");
			newGroupData.pushNodeData(tdata);
			nindex_offset++;
			vector<double> mweights;
			cid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + 1 + ncindex_offset);
			newGroupData.pushConnectData(string(CT2CA(cid)), string(CT2CA(cid)), newGroupData.getID(ncindex_offset), string(CT2CA(sid)), gindex, 0.0, mweights, points);
			ncindex_offset++;
		}
	}
	for (int j = 0; j < nghiddenlayers; j++) {
		x = x + layer_separation;
		for (int i = 0; i < nghiddenwidth; i++) {
			int my = y + (i * node_separation) - ((nghiddenwidth * node_separation) / 2);
			sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
			tdata.setID(string(CT2CA(sid)));
			tdata.setLabel(string(CT2CA(sid)));
			tdata.setXDisp(x);
			tdata.setYDisp(my);
			tdata.setNodeMode("Hidden");
			newGroupData.pushNodeData(tdata);
			nindex_offset++;
		}
	}
	x = x + layer_separation;
	for (int i = 0; i < ngoutputs; i++) {
		int my = y + (i * node_separation) - ((ngoutputs * node_separation) / 2);
		sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + 1 + nindex_offset);
		tdata.setID(string(CT2CA(sid)));
		tdata.setLabel(string(CT2CA(sid)));
		tdata.setXDisp(x);
		tdata.setYDisp(my);
		tdata.setNodeMode("Output");
		newGroupData.pushNodeData(tdata);
		nindex_offset++;
	}

	return(newGroupData);

}


