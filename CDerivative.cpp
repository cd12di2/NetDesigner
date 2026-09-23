// CDerivative.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CDerivative.h"
#include "AuxFunctions.h"
#include "afxdialogex.h"


// CDerivative dialog

IMPLEMENT_DYNAMIC(CDerivative, CDialog)

CDerivative::CDerivative(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DERIVATIVE, pParent)
{
	data_updated = false;
	data_label = _T("");
	node_label = _T("");
	m_derivative = 0;
	m_stride = _T("1");
	m_intercept = _T("0");
	m_slope = _T("0");
	m_relative = false;
	output = false;
	tanh = false;
}

CDerivative::~CDerivative()
{
}

void CDerivative::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_LINEAR, m_derivative);
	DDX_Text(pDX, IDC_SLEAK, m_stride);
	DDX_Text(pDX, IDC_LOWER_NORM_FACTOR2, m_intercept);
	DDX_Text(pDX, IDC_UPPER_NORM_FACTER2, m_slope);
	DDX_Check(pDX, IDC_RELATIVE, m_relative);
	DDX_Control(pDX, IDC_STATIC_BOX, m_static_box_ctrl);
}


BEGIN_MESSAGE_MAP(CDerivative, CDialog)
	ON_BN_CLICKED(IDC_LINEAR, &CDerivative::OnBnClickedLinear)
	ON_BN_CLICKED(IDC_SECOND, &CDerivative::OnBnClickedSecond)
	ON_BN_CLICKED(IDC_THIRD, &CDerivative::OnBnClickedThird)
	ON_BN_CLICKED(IDC_FOURTH, &CDerivative::OnBnClickedFourth)
	ON_BN_CLICKED(IDC_RELATIVE, &CDerivative::OnBnClickedRelative)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_SLEAK, &CDerivative::OnEnChangeSleak)
END_MESSAGE_MAP()


// CDerivative message handlers

void CDerivative::addErrorString(CString err) {
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString(err);
}

void CDerivative::setSize(CRect rect)
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
	rect3.left = t4;// rect14.Width() + 18;// 180;
	rect3.top = 5;
	rect3.right = (rect.right - rect.left) - 5;
	rect3.bottom = ((rect.bottom - rect.top) / 2) - 2;
	mgraph1.setSize(rect3);
	rect3.top = ((rect.bottom - rect.top) / 2) + 2;
	rect3.bottom = (rect.bottom - rect.top) - 5;
	mgraph2.setSize(rect3);
}

void CDerivative::setData(vector<vector<double>> ddata)
{
	CAuxFunctions func;
	if (!data_updated/* && func.areDataNumbers(sdata)*/) {	
		data.clear();
		xdata.clear();
		int xindex = 1;
		for (int i = 0; i < ddata.size(); i++) {
			vector<double> adata;
			data.push_back(adata);
			for (int j = 0; j < ddata[i].size(); j++) {
				//double mdata = _tstof((LPCTSTR)sdata[i][j]);
				data[i].push_back(ddata[i][j]);
				xdata.push_back((double)xindex);
				xindex++;
			}
		}
		//data_updated = true;
	}
}

void CDerivative::OnBnClickedLinear()
{
	//CStatus myProgressDlg;
	//myProgressDlg.CreateDlg();
	//myProgressDlg.setLabel(_T("Calculating first derivatives . . ."));
	//myProgressDlg.setProgress(0);
	CAuxFunctions aux;
	UpdateData();
	ddata.clear();
	xdata.clear();
	int xindex = 1;
	int mstride = (int)_tstof((LPCTSTR)m_stride);
	if (mstride == 0) {
		mstride = 1;
	}
	if (data.size() > 0) {
		for (int i = 0; i < (int)data.size(); i++) {
			aux.setDerivativeData(data[i]);
			for (int j = 0; j < data[i].size(); j++) {
				double der = aux.firstDerivativeBackward(j, mstride, m_relative/*, data[i]*/);
				ddata.push_back(der);
				xdata.push_back((double)xindex);
				xindex++;
			}
			//myProgressDlg.setProgress((100 * i) / (int)data.size());
		}
	}
	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label + _T(" - Derivative"));
	calcNormalizationFactors();
	//myProgressDlg.DestroyDlg();
}

void CDerivative::OnBnClickedSecond()
{
	//CStatus myProgressDlg;
	//myProgressDlg.CreateDlg();
	//myProgressDlg.setLabel(_T("Calculating second derivatives . . ."));
	//myProgressDlg.setProgress(0);
	CAuxFunctions aux;
	UpdateData();
	ddata.clear();
	xdata.clear();
	int xindex = 1;
	int mstride = (int)_tstof((LPCTSTR)m_stride);
	if (mstride == 0) {
		mstride = 1;
	}
	if (data.size() > 0) {
		for (int i = 0; i < (int)data.size(); i++) {
			aux.setDerivativeData(data[i]);
			for (int j = 0; j < data[i].size(); j++) {
				double der = aux.secondDerivativeBackward(j, mstride, m_relative);
				ddata.push_back(der);
				xdata.push_back((double)xindex);
				xindex++;
			}
			//myProgressDlg.setProgress((100 * i) / (int)data.size());
		}
	}
	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label + _T(" - Second Derivative"));
	calcNormalizationFactors();
	//myProgressDlg.DestroyDlg();
}

void CDerivative::OnBnClickedThird()
{
	//CStatus myProgressDlg;
	//myProgressDlg.CreateDlg();
	//myProgressDlg.setLabel(_T("Calculating third derivatives . . ."));
	//myProgressDlg.setProgress(0);
	CAuxFunctions aux;
	UpdateData();
	ddata.clear();
	xdata.clear();
	int xindex = 1;
	int mstride = (int)_tstof((LPCTSTR)m_stride);
	if (mstride == 0) {
		mstride = 1;
	}
	if (data.size() > 0) {
		for (int i = 0; i < (int)data.size(); i++) {
			aux.setDerivativeData(data[i]);
			for (int j = 0; j < data[i].size(); j++) {
				double der = aux.thirdDerivativeBackward(j, mstride, m_relative);
				ddata.push_back(der);
				xdata.push_back((double)xindex);
				xindex++;
			}
			//myProgressDlg.setProgress((100 * i) / (int)data.size());
		}
	}
	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label + _T(" - Third Derivative"));
	calcNormalizationFactors();
	//myProgressDlg.DestroyDlg();
}

void CDerivative::OnBnClickedFourth()
{
	//CStatus myProgressDlg;
	//myProgressDlg.CreateDlg();
	//myProgressDlg.setLabel(_T("Calculating fourth derivatives . . ."));
	//myProgressDlg.setProgress(0);
	CAuxFunctions aux;
	UpdateData();
	ddata.clear();
	xdata.clear();
	int xindex = 1;
	int mstride = (int)_tstof((LPCTSTR)m_stride);
	if (mstride == 0) {
		mstride = 1;
	}
	if (data.size() > 0) {
		for (int i = 0; i < (int)data.size(); i++) {
			aux.setDerivativeData(data[i]);
			for (int j = 0; j < data[i].size(); j++) {
				double der = aux.fourthDerivativeBackward(j, mstride, m_relative);
				ddata.push_back(der);
				xdata.push_back((double)xindex);
				xindex++;
			}
			//myProgressDlg.setProgress((100 * i) / (int)data.size());
		}
	}
	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label + _T(" - Fourth Derivative"));
	calcNormalizationFactors();
	//myProgressDlg.DestroyDlg();
}

BOOL CDerivative::OnInitDialog()
{
	CDialog::OnInitDialog();

	xdata.clear();
	data.clear();
	ddata.clear();
	normalized_data.clear();

	mgraph1.setData(xdata, ddata, ddata);
	mgraph2.setData(normalized_data);

	if (data_label.GetLength() > 20) {
		data_label = data_label.Left(20);
	}
	mgraph1.setXLabel(node_label + _T(" : ") + data_label + _T(" - Derivative"));
	mgraph1.xlabel2 = _T("");
	mgraph2.setXLabel(node_label + _T(" : ") + data_label + _T(" - Normalized"));

	getDerivative();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDerivative::getDerivative()
{
	if (m_derivative == 0) {
		OnBnClickedLinear();
	}
	else if (m_derivative == 1) {
		OnBnClickedSecond();
	}
	else if (m_derivative == 2) {
		OnBnClickedThird();
	}
	else if (m_derivative == 3) {
		OnBnClickedFourth();
	}
}

CString CDerivative::getMode()
{
	CString rval = _T("Derivative1");
	if (m_derivative == 1) {
		rval = _T("Derivative2");
	}
	else if (m_derivative == 2) {
		rval = _T("Derivative3");
	}
	else if (m_derivative == 3) {
		rval = _T("Derivative4");
	}
	if (m_relative) {
		rval += _T("R");
	}
	else {
		rval += _T("N");
	}
	return(rval);
}

void CDerivative::setMode(CString mode)
{
	CString mmode = mode;
	if (mmode[mmode.GetLength()-1] == 'R') {
		m_relative = true;
	}
	else {
		m_relative = false;
	}
	mmode = mmode.Left(11);
	m_derivative = 0;
	if (mmode == _T("Derivative2")) {
		m_derivative = 1;
	}
	else if (mmode == _T("Derivative3")) {
		m_derivative = 2;
	}
	else if (mmode == _T("Derivative4")) {
		m_derivative = 3;
	}
	else {
	}
}

double CDerivative::getLowerNormalizationFactor()
{
	return(nmin);
}

void CDerivative::setLowerNormalizationFactor(double factor)
{
	nmin = factor;
	m_intercept.Format(_T("%.6f"), nmin);
}

double CDerivative::getUpperNormalizationFactor()
{
	return(nmax);
}

void CDerivative::setUpperNormalizationFactor(double factor)
{
	nmax = factor;
	m_slope.Format(_T("%.6f"), nmax);
}

int CDerivative::getStride()
{
	int mstride = (int)_tstof((LPCTSTR)m_stride);
	return(mstride);
}

void CDerivative::setStride(int stride)
{
	m_stride.Format(_T("%i"), stride);
}

void CDerivative::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages

	mgraph1.drawGraph(&dc);
	mgraph2.drawGraph(&dc);


}

void CDerivative::OnOK()
{
	//CDialog::OnOK();
}

void CDerivative::OnCancel()
{
	//CDialog::OnCancel();
}

BOOL CDerivative::DestroyWindow()
{
	return CDialog::DestroyWindow();
}

void CDerivative::calcNormalizationFactors()
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
		double nfactormin = -1.0F;
		double nfactormax = 1.0F;
		if (output && tanh) {
			nfactormin = -0.8F;
			nfactormax = 0.8F;
		}
		else if (output) {
			nfactormin = 0.2F;
			nfactormax = 0.8F;
		}
		//CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		//CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		//CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		//CString s = _T("");
		//s.Format(_T("CDerivative : min = %.6f : max = %.6f"), mmin, mmax);
		//pView->addErrorString(s);
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
		mgraph1.minx = 0;
		mgraph1.maxx = (float)ddata.size();
		mgraph1.miny = -1 * mgraph1.maxy;
		mgraph2.minx = -1.0F;
		mgraph2.maxx = 1.0F;
		if (output && tanh) {
			mgraph2.minx = -0.8F;
			mgraph2.maxx = 0.8F;
		}
		else if (output) {
			mgraph2.minx = 0.2F;
			mgraph2.maxx = 0.8F;
		}
		mgraph1.drawFrame(&dc);
		mgraph1.drawXAxis(&dc);
		mgraph1.drawData(&dc);
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

void CDerivative::setNormalizedData()
{
	if (ddata.size() > 0) {
		normalized_data.clear();
		if (nmax == 0) {
			nmin = 0;
			nmax = 1;
		}
		vector<double> ddata2;
		if (ddata.size() > 19999) {
			vector<double> mxdata;
			vector<double> mydata;
			int factor = (int)ddata.size() / 10000;
			for (int i = 0; i < ddata.size(); i+=factor) {
				double tdata = (double)ddata[i];
				double tdata2 = nmin + (tdata * nmax);
				mxdata.push_back((double)i);
				mydata.push_back(tdata);
				normalized_data.push_back(tdata2);
			}
			mgraph1.setData(mxdata, mydata, ddata2);
			mgraph2.setData(normalized_data);
		}
		else {
			for (int i = 0; i < ddata.size(); i++) {
				double tdata = nmin + ((double)ddata[i] * nmax);
				normalized_data.push_back(tdata);
			}
			mgraph1.setData(xdata, ddata, ddata2);
			mgraph2.setData(normalized_data);
		}
	}
}

void CDerivative::OnBnClickedRelative()
{
	getDerivative();
}

void CDerivative::OnEnChangeSleak()
{
	UpdateData();
	getDerivative();
}
