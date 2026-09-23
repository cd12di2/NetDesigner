// CAutoRnnConfigDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CAutoRnnConfigDlg.h"
#include "afxdialogex.h"


// CAutoRnnConfigDlg dialog

bool pnormalize2 = true;
int pnum_sections = 4;
int pnum_hidden2 = 1;
CString pfunction2 = _T("Sigmoid");
double peta2 = 0.002;
double palpha2 = 0.8;
double prand2 = 0.2;

IMPLEMENT_DYNAMIC(CAutoRnnConfigDlg, CDialog)

CAutoRnnConfigDlg::CAutoRnnConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_AUTORNN, pParent)
{
	spt = CPoint(0, 0);
	m_normalize = pnormalize2;
	m_num_sections.Format(_T("%i"), pnum_sections);
	m_num_hidden.Format(_T("%i"), pnum_hidden2);
	m_activation_function = pfunction2;
	m_eta.Format(_T("%.8f"), peta2);// = _T("0.00000000");
	m_alpha.Format(_T("%.6f"), palpha2);// =_T("0.000000");
	m_rand.Format(_T("%.6f"), prand2);// = _T("0.000000");
}

CAutoRnnConfigDlg::~CAutoRnnConfigDlg()
{
}

void CAutoRnnConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_NORMALIZE, m_normalize);
	DDX_Text(pDX, IDC_NUM_INPUTS, m_num_sections);
	DDX_Text(pDX, IDC_NUM_LAYERS, m_num_hidden);
	DDX_CBString(pDX, IDC_COMBO1, m_activation_function);
	DDX_Text(pDX, IDC_ETA, m_eta);
	DDX_Text(pDX, IDC_ALPHA, m_alpha);
	DDX_Text(pDX, IDC_RAND, m_rand);
	DDX_Control(pDX, IDC_COMBO1, m_activation_function_control);
}


BEGIN_MESSAGE_MAP(CAutoRnnConfigDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAutoRnnConfigDlg message handlers


BOOL CAutoRnnConfigDlg::OnInitDialog()
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

void CAutoRnnConfigDlg::OnOK()
{
	UpdateData();

	pnormalize2 = m_normalize;

	pnum_sections = (int)_tstof((LPCTSTR)m_num_sections);
	pnum_hidden2 = (int)_tstof((LPCTSTR)m_num_hidden);

	pfunction2 = m_activation_function;
	peta2 = (double)_tstof((LPCTSTR)m_eta);
	palpha2 = (double)_tstof((LPCTSTR)m_alpha);
	prand2 = (double)_tstof((LPCTSTR)m_rand);

	CDialog::OnOK();
}

NetData CAutoRnnConfigDlg::makeNet(CPoint mpt, int gindex)
{
	bool gnormalize = pnormalize2;
	int nsections = pnum_sections;
	int nghiddendepth = pnum_hidden2;
	string gfunction = string(CT2CA(pfunction2));
	double geta = peta2;
	double galpha = palpha2;
	double grand = prand2;

	int ndepth = nghiddendepth + 2;
	if (gnormalize) {
		ndepth++;
	}
	int node_separation = 32;
	int layer_separation = 48;
	int xoffset = layer_separation * (nsections / 2);
	int yoffset = node_separation * (ndepth / 2);

	newGroupData.clear();

	//CString sid = _T("");
	//CString cid = _T("");
	string lid = "";
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

	int addindex = 1;
	int laddindex = 1;
	x = x - xoffset;

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	for (int i = 0; i < nsections; i++) {

		nindex_offset = 0;

		int my = y - (nindex_offset * node_separation) + ((ndepth * node_separation) / 2) + (ndepth * node_separation);
		//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + addindex);
		char sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + addindex);
		tdata.setID(sid);
		tdata.setLabel(sid);
		tdata.setXDisp(x);
		tdata.setYDisp(my);
		tdata.setNodeMode("Input");
		newGroupData.pushNodeData(tdata);
		addindex++;
		nindex_offset++;
		lid = sid;

		if (gnormalize) {
			my = y - (nindex_offset * node_separation) + ((ndepth * node_separation) / 2) + (ndepth * node_separation);
			//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + addindex);
			char sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + addindex);
			tdata.setID(sid);
			tdata.setLabel(sid);
			tdata.setXDisp(x);
			tdata.setYDisp(my);
			tdata.setNodeMode("PreProcess");
			newGroupData.pushNodeData(tdata);
			addindex++;
			vector<double> mweights;
			//cid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + laddindex);
			char cid[100]; sprintf_s(cid, "Connect%i", pDoc->getLastConnectIndex() + laddindex);
			newGroupData.pushConnectData(cid, cid, lid, sid, gindex, 0.0, mweights, points);
			laddindex++;
			nindex_offset++;
			lid = sid;
		}

		for (int j = 0; j < nghiddendepth; j++) {
			my = y - (nindex_offset * node_separation) + ((ndepth * node_separation) / 2) + (ndepth * node_separation);
			//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + addindex);
			char sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + addindex);
			tdata.setID(sid);
			tdata.setLabel(sid);
			tdata.setXDisp(x);
			tdata.setYDisp(my);
			tdata.setNodeMode("Hidden");
			newGroupData.pushNodeData(tdata);
			addindex++;
			vector<double> mweights;
			//cid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + laddindex);
			char cid[100]; sprintf_s(cid, "Connect%i", pDoc->getLastConnectIndex() + laddindex);
			newGroupData.pushConnectData(cid, cid, lid, sid, gindex, 0.0, mweights, points);
			laddindex++;
			int pindex = (2 * ndepth - 1);
			if (i > 0) {
				if (i == 1) {
					pindex = addindex + laddindex - (3 + (nghiddendepth * 2) + 4 + j);
				}
				else {
					pindex = addindex + laddindex - (3 + (nghiddendepth * 3) + 3);
				}
				if (i > 1) {
					pindex--;
				}
				if (gnormalize) {
					pindex -= 2;
				}
				vector<double> mweights2;
				//cid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + laddindex);
				char cid[100]; sprintf_s(cid, "Connect%i", pDoc->getLastConnectIndex() + laddindex);
				newGroupData.pushConnectData(cid, cid, newGroupData.getID(pindex), sid, gindex, 0.0, mweights2, points);
				laddindex++;
			}
			nindex_offset++;
			lid = sid;
		}

		my = y - (nindex_offset * node_separation) + ((ndepth * node_separation) / 2) + (ndepth * node_separation);
		//sid.Format(_T("Node%i"), pDoc->getLastNodeIndex() + addindex);
		sid[100]; sprintf_s(sid, "Node%i", pDoc->getLastNodeIndex() + addindex);
		tdata.setID(sid);
		tdata.setLabel(sid);
		tdata.setXDisp(x);
		tdata.setYDisp(my);
		tdata.setNodeMode("Output");
		newGroupData.pushNodeData(tdata);
		addindex++;
		vector<double> mweights;
		//cid.Format(_T("Connect%i"), pDoc->getLastConnectIndex() + laddindex);
		char cid[100]; sprintf_s(cid, "Connect%i", pDoc->getLastConnectIndex() + laddindex);
		newGroupData.pushConnectData(cid, cid, lid, sid, gindex, 0.0, mweights, points);
		laddindex++;
		nindex_offset++;
		lid = sid;

		x = x + layer_separation;

	}

	return(newGroupData);

}

HBRUSH CAutoRnnConfigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
