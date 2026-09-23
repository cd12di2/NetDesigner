// CNormalize.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CNormalize.h"
#include "NormalizationFactor.h"
#include "afxdialogex.h"
#include "COptions.h"


// CNormalize dialog

IMPLEMENT_DYNAMIC(CNormalize, CDialog)

CNormalize::CNormalize(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_NORMALIZE, pParent)
{
	data_updated = false;
	data_label = _T("");
	node_label = _T("");
	m_linear = true;
	m_logarithmic = false;
	output = false;
	tanh = false;
	m_lower = _T("0.0000");
	m_upper = _T("0.0000");
	ynot = 0;
	slope = 0;
	nfactormin = 0;
	nfactormax = 1;
}

CNormalize::~CNormalize()
{
}

void CNormalize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_LINEAR, m_logarithmic);
	DDX_Radio(pDX, IDC_LOGARITHMIC, m_linear);
	DDX_Text(pDX, IDC_LOWER_NORM_FACTOR, m_lower);
	DDX_Text(pDX, IDC_UPPER_NORM_FACTER, m_upper);
	DDX_Control(pDX, IDC_STATIC_BOX, m_static_box_ctrl);
}


BEGIN_MESSAGE_MAP(CNormalize, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_LINEAR, &CNormalize::OnBnClickedLinear)
	ON_BN_CLICKED(IDC_LOGARITHMIC, &CNormalize::OnBnClickedLogarithmic)
	ON_BN_CLICKED(IDC_RESET_FACTORS, &CNormalize::OnBnClickedResetFactors)
	ON_BN_CLICKED(IDC_NORM_OPTIONS, &CNormalize::OnBnClickedNormOptions)
END_MESSAGE_MAP()


// CNormalize message handlers

void CNormalize::setSize(CRect mrect)
{
	int t0 = 18;
	int t1 = 24;
	int t4 = 180;
	CRect rect = mrect;
	MoveWindow(rect, true);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	CRect rect13;
	m_static_box_ctrl.GetWindowRect(&rect13);
	rect13.left = t0 - 12;
	rect13.top = t1 - 24;
	rect13.right = t4 - 9;
	rect13.bottom = (rect.bottom - rect.top) - 5;
	m_static_box_ctrl.MoveWindow(rect13);
	CRect rect3;
	rect3.left = t4;
	rect3.top = 5;
	rect3.right = (rect.right - rect.left) - 5;
	rect3.bottom = ((rect.bottom - rect.top) / 2) - 2;
	mgraph1.setSize(rect3);
	rect3.top = ((rect.bottom - rect.top) / 2) + 2;
	rect3.bottom = (rect.bottom - rect.top) - 5;
	mgraph2.setSize(rect3);
}

void CNormalize::setData(vector<vector<double>> ddata)
{
	CAuxFunctions func;
	if (!data_updated/* && func.areDataNumbers(sdata)*/) {	
		data.clear();
		for (int i = 0; i < ddata.size(); i++) {
			for (int j = 0; j < ddata[i].size(); j++) {
				//double mdata = _tstof((LPCTSTR)sdata[i][j]);
				data.push_back(ddata[i][j]);
			}
		}
		calc_limits = true;
		mgraph1.setData(data);
	}
}

void CNormalize::setNormalizedData()
{
	nfactormin = 0;
	nfactormax = 1;
	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label);
	mgraph2.setXLabel(node_label + _T(" : ") + data_label + _T(" - Normalized"));
	if (m_linear) {
		setNormalizedLinearData();
	}
	else {
		setNormalizedLogData();
	}
	CClientDC dc(this);
	mgraph1.drawFrame(&dc);
	mgraph1.drawXAxis(&dc);
	mgraph1.drawData(&dc);
	mgraph2.drawGraph(&dc);
}

void CNormalize::setNormalizedLinearData()
{
	normalized_data.clear();
	if (data.size() > 0) {
		if (slope == 0) {
			ynot = 0;
			slope = 1;
		}
		for (int i = 0; i < data.size(); i++) {
			double tdata = ynot + ((double)data[i] * slope);
			normalized_data.push_back(tdata);
		}
	}
	mgraph2.setData(normalized_data);
}

void CNormalize::setNormalizedLogData()
{
	if (!checkLogOk()) {
		OnBnClickedLinear();
		return;
	}
	normalized_data.clear();
	if (data.size() > 0) {
		if (slope == 0) {
			ynot = 0;
			slope = 1;
		}
		for (int i = 0; i < data.size(); i++) {
			double tdata = ynot + ((double)log(data[i]) * slope);
			normalized_data.push_back(tdata);
		}
	}
	mgraph2.setData(normalized_data);
}

bool CNormalize::checkLogOk()
{
	if (data.size() > 0) {
		for (int i = 0; i < data.size(); i++) {
			if (data[i] <= 0) {
				if (!zero_log_message) {
					AfxMessageBox(_T("All data must be greater than zero to select Log"));
				}
				zero_log_message = true;
				return(false);
			}
		}
		return(true);
	}
	return(false);
}

CString CNormalize::getMode()
{
	if (m_logarithmic) {
		return(_T("NormLog"));
	}
	return(_T("NormLinear"));
}

void CNormalize::setMode(CString mode)
{
	if (mode == _T("NormLinear")) {
		m_linear = true;
		m_logarithmic = false;
	}
	else if (mode == _T("NormLog")) {
		m_linear = false;
		m_logarithmic = true;
	}
}

double CNormalize::getLowerNormalizationFactor()
{
	return(ynot);
}

void CNormalize::setLowerNormalizationFactor(double factor)
{
	ynot = factor;
	m_lower.Format(_T("%.6f"), ynot);
}

double CNormalize::getUpperNormalizationFactor()
{
	return(slope);
}

void CNormalize::setUpperNormalizationFactor(double factor)
{
	slope = factor;
	m_upper.Format(_T("%.6f"), slope);
}

BOOL CNormalize::OnInitDialog()
{
	CDialog::OnInitDialog();

	data.clear();
	normalized_data.clear();

	mgraph1.setData(data);
	mgraph2.setData(normalized_data);

	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label);
	mgraph2.setXLabel(node_label + _T(" : ") + data_label + _T(" - Normalized"));

	if (ynot == slope) {
		OnBnClickedResetFactors();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CNormalize::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages

	mgraph1.drawGraph(&dc);
	mgraph2.drawGraph(&dc);

}

void CNormalize::OnBnClickedLinear()
{
	m_linear = true;
	m_logarithmic = false;
	calc_limits = true;
	UpdateData(false);
	setNormalizedLinearData();
	CClientDC dc(this);
	mgraph2.drawGraph(&dc);
}

void CNormalize::OnBnClickedLogarithmic()
{
	m_linear = false;
	m_logarithmic = true;
	calc_limits = true;
	UpdateData(false);
	setNormalizedLogData();
	CClientDC dc(this);
	mgraph2.drawGraph(&dc);
}

BOOL CNormalize::DestroyWindow()
{
	return CDialog::DestroyWindow();
}

void CNormalize::OnCancel()
{
	//CDialog::OnCancel();
}

void CNormalize::OnOK()
{
	//CDialog::OnOK();
}

void CNormalize::OnBnClickedResetFactors()
{
	if (data.size() > 0) {
		double mmin = 0;
		double mmax = 0;
		if (m_logarithmic) {
			if (checkLogOk()) {
				mmin = log(data[0]);
				mmax = log(data[0]);
				for (int i = 1; i < data.size(); i++) {
					if (log(data[i]) < mmin) {
						mmin = log(data[i]);
					}
					if (log(data[i]) > mmax) {
						mmax = log(data[i]);
					}
					if (output && tanh) {
						if (-mmin > mmax) {
							mmax = -mmin;
						}
						else {
							mmin = -mmax;
						}
					}
				}
			}
			else {
				setLinear();
			}
		}
		if (m_linear) {
			mmin = data[0];
			mmax = data[0];
			for (int i = 1; i < data.size(); i++) {
				if (data[i] < mmin) {
					mmin = data[i];
				}
				if (data[i] > mmax) {
					mmax = data[i];
				}
				if (output && tanh) {
					if (-mmin > mmax) {
						mmax = -mmin;
					}
					else {
						mmin = -mmax;
					}
				}
			}
		}
		if (mmax != mmin) {
			if (output && tanh) {
				ynot = 0;
				slope = 2 * nfactormax / (mmax - mmin);
			}
			else {
				ynot = (nfactormin * mmax - nfactormax * mmin) / (mmax - mmin);
				slope = (nfactormax - nfactormin) / (mmax - mmin);
			}
		}
		else {
			ynot = 0.0;
			slope = 1.0;
		}
		setNormalizedData();
		CClientDC dc(this);
		mgraph2.drawGraph(&dc);
		m_lower.Format(_T("%.6f"), ynot);
		m_upper.Format(_T("%.6f"), slope);
		UpdateData(false);
	}
	else {
		ynot = 0.0;
		slope = 1.0;
	}
}

void CNormalize::OnBnClickedNormOptions()
{
	NormalizationFactor dlg;
	if (output && tanh) {
		dlg.m_lower.Format(_T("%.6f"), -0.8);
	}
	else {
		dlg.m_lower.Format(_T("%.6f"), 0.2);
	}
	dlg.m_upper.Format(_T("%.6f"), 0.8);
	if (dlg.DoModal() == IDOK) {
		nfactormin = (double)_tstof((LPCTSTR)dlg.m_lower);
		nfactormax = (double)_tstof((LPCTSTR)dlg.m_upper);
		OnBnClickedResetFactors();
	}
}

void CNormalize::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}



