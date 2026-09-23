// CFFT.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CFFT.h"
#include "afxdialogex.h"


// CFFT dialog

IMPLEMENT_DYNAMIC(CFFT, CDialog)

CFFT::CFFT(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FFT, pParent)
{
	data_updated = false;
	lock_one_stride = true;
	data_label = _T("");
	node_label = _T("");
	m_downsample = _T("");
	m_n = _T("8");
	m_stride = _T("1");
	m_downsample = _T("");
	m_intercept = _T("0");
	m_slope = _T("0");
}

CFFT::~CFFT()
{
}

void CFFT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_n_ctrl);
	DDX_CBString(pDX, IDC_COMBO1, m_n);
	DDX_Text(pDX, IDC_SLEAK, m_stride);
	DDX_Text(pDX, IDC_LOWER_NORM_FACTOR, m_intercept);
	DDX_Text(pDX, IDC_UPPER_NORM_FACTER, m_slope);
	DDX_Control(pDX, IDC_STATIC_BOX, m_static_box_ctrl);
	DDX_Text(pDX, IDC_SLEAK2, m_downsample);
	DDX_Control(pDX, IDC_SLEAK, m_stride_ctrl);
}


BEGIN_MESSAGE_MAP(CFFT, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CFFT::OnCbnSelchangeCombo1)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_SLEAK, &CFFT::OnEnChangeSleak)
	ON_EN_CHANGE(IDC_SLEAK2, &CFFT::OnEnChangeSleak2)
END_MESSAGE_MAP()


// CFFT message handlers

void CFFT::setSize(CRect rect)
{
	int t0 = 18;
	int t1 = 24;
	int t4 = 180;
	MoveWindow(rect, true);
	CRect rect13;
	m_static_box_ctrl.GetWindowRect(&rect13);
	rect13.left = t0 - 12;
	rect13.top = t1 - 24;
	rect13.right = t4 - 9;
	rect13.bottom = (rect.bottom - rect.top) - 5;
	m_static_box_ctrl.MoveWindow(rect13);
	CRect rect3;
	rect3.left = t4;// rect13.Width() + 18;// 180;
	rect3.top = 5;
	rect3.right = (rect.right - rect.left) - 5;
	rect3.bottom = ((rect.bottom - rect.top) / 2) - 2;
	mgraph1.setSize(rect3);
	rect3.top = ((rect.bottom - rect.top) / 2) + 2;
	rect3.bottom = (rect.bottom - rect.top) - 5;
	mgraph2.setSize(rect3);
}

void CFFT::setData(vector<double> ddata)
{
	CAuxFunctions func;
	if (!data_updated/* && func.areDataNumbers(sdata)*/) {	
		data.clear();
		if (ddata.size() > 0) {
			for (int i = 0; i < ddata.size(); i++) {
				//double mdata = _tstof((LPCTSTR)sdata[i]);
				data.push_back(ddata[i]);
			}
		}
		//data_updated = true;
	}
}

void CFFT::OnCbnSelchangeCombo1()
{
	CAuxFunctions aux;
	UpdateData();
	xdata.clear();
	ddata.clear();
	ddata2.clear();
	int mstride = (int)_tstof((LPCTSTR)m_stride);
	if (mstride <= 0) {
		mstride = 1;
	}
	int mdownsample = (int)_tstof((LPCTSTR)m_downsample);
	if (mdownsample <= 0) {
		mdownsample = 1;
	}
	int myN = 2 * (int)_tstof((LPCTSTR)m_n);
	vector<double> mdata = aux.getFFT(1, myN, mdownsample, true, data);
	for (int j = 0; j < mdata.size(); j++) {
		ddata.push_back(mdata[j]);
		xdata.push_back((double)j + 1);
	}
	for (int i = 2; i < (int)data.size(); i++) {
		mdata = aux.getFFT(i, myN, mdownsample, true, data);
		for (int j = 0; j < mdata.size(); j++) {
			//ddata[j] = ddata[j] + mdata[j];
			if (mdata[j] > ddata[j]) {
				ddata[j] = mdata[j];
			}
		}
	}
	//if (((int)ddata.size() > 0) && ((int)data.size() > 0)) {
		//for (int j = 0; j < mdata.size(); j++) {
			//ddata[j] = ddata[j] / (double)data.size();
		//}
	//}
	calcNormalizationFactors();
}

BOOL CFFT::OnInitDialog()
{
	CDialog::OnInitDialog();

	mgraph1.xlabel2 = _T("");
	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label + _T(" - FFT"));
	mgraph2.setXLabel(node_label + _T(" : ") + data_label + _T(" - Normalized"));

	m_n_ctrl.AddString(_T("4"));
	m_n_ctrl.AddString(_T("8"));
	m_n_ctrl.AddString(_T("16"));
	m_n_ctrl.AddString(_T("32"));
	m_n_ctrl.AddString(_T("64"));
	m_n_ctrl.AddString(_T("128"));
	m_n_ctrl.AddString(_T("256"));
	m_n_ctrl.SetCurSel(m_n_ctrl.FindString(0, m_n));

	if (lock_one_stride) {
		m_stride_ctrl.EnableWindow(false);
	} 
	else {
		m_stride_ctrl.EnableWindow(true);
	}

	OnCbnSelchangeCombo1();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

CString CFFT::getMode()
{
	CString rval = _T("FFT") + m_n;
	return(rval);
}

void CFFT::setMode(CString mode)
{
	if (mode.Left(3) == _T("FFT")) {
		m_n = mode.Right(mode.GetLength() - 3);
	}
}

double CFFT::getLowerNormalizationFactor()
{
	return(nmin);
}

void CFFT::setLowerNormalizationFactor(double factor)
{
	nmin = factor;
	m_intercept.Format(_T("%.6f"), nmin);
}

double CFFT::getUpperNormalizationFactor()
{
	return(nmax);
}

void CFFT::setUpperNormalizationFactor(double factor)
{
	nmax = factor;
	m_slope.Format(_T("%.6f"), nmax);
}

int CFFT::getStride()
{
	int mstride = (int)_tstof((LPCTSTR)m_stride);
	return(mstride);
}

void CFFT::setStride(int stride, bool lock_one)
{
	if (lock_one) {
		lock_one_stride = true;
		m_stride = _T("1");
		m_stride_ctrl.EnableWindow(false);
	}
	else {
		lock_one_stride = false;
		m_stride.Format(_T("%i"), stride);
		m_stride_ctrl.EnableWindow(true);
	}
}

int CFFT::getDownSample()
{
	int mdownsample = (int)_tstof((LPCTSTR)m_downsample);
	return(mdownsample);
}

void CFFT::setDownSample(int downsample)
{
	m_downsample.Format(_T("%i"), downsample);
}

void CFFT::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages

	mgraph1.drawGraph(&dc);
	mgraph2.drawGraph(&dc);

}

void CFFT::OnOK()
{
	//CDialog::OnOK();
}

void CFFT::OnCancel()
{
	//CDialog::OnCancel();
}

BOOL CFFT::DestroyWindow()
{
	return CDialog::DestroyWindow();
}

void CFFT::calcNormalizationFactors()
{
	if (ddata.size() > 0) {
		double mmin = 0;
		double mmax = 0;
		mmin = ddata[0];
		mmax = ddata[0];
		for (int i = 1; i < ddata.size(); i++) {
			if (ddata[i] < mmin) {
				mmin = ddata[i];
			}
			if (ddata[i] > mmax) {
				mmax = ddata[i];
			}
		}
		double nfactormin = 0;
		double nfactormax = 1;
		if (mmax != mmin) {
			nmin = (nfactormin * mmax - nfactormax * mmin) / (mmax - mmin);
			nmax = (nfactormax - nfactormin) / (mmax - mmin);
		}
		else {
			nmin = 0;
			nmax = 0;
		}
		setNormalizedData();
		CClientDC dc(this);
		mgraph1.minx = 1;
		mgraph1.maxx = (float)ddata.size();
		//mgraph1.maxy = 1.1F * mgraph1.maxy;
		//mgraph1.miny = 0;// -1 * mgraph1.maxy;
		mgraph1.drawFrame(&dc);
		mgraph1.drawXAxis(&dc);
		mgraph1.drawData(&dc);
		//mgraph1.drawGraph(&dc);
		mgraph2.drawGraph(&dc);
		m_intercept.Format(_T("%.6f"), nmin);
		m_slope.Format(_T("%.6f"), nmax);
		UpdateData(false);
	}
	else {
		nmin = 0;
		nmax = 0;
	}
}

void CFFT::setNormalizedData()
{
	if (ddata.size() > 0) {
		normalized_data.clear();
		if (nmax == 0) {
			nmin = 0;
			nmax = 1;
		}
		for (int i = 0; i < ddata.size(); i++) {
			double tdata = nmin + ((double)ddata[i] * nmax);
			normalized_data.push_back(tdata);
		}
		mgraph1.setData(xdata, ddata, ddata2);
		mgraph2.setData(normalized_data);
	}
}

void CFFT::OnEnChangeSleak()
{
	UpdateData();
	//OnCbnSelchangeCombo1();
}

void CFFT::OnEnChangeSleak2()
{
	UpdateData();
	OnCbnSelchangeCombo1();
}
