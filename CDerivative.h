#pragma once


#include "Graph.h"

// CDerivative dialog

class CDerivative : public CDialog
{
	DECLARE_DYNAMIC(CDerivative)

public:
	CDerivative(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDerivative();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DERIVATIVE };
#endif

	bool data_updated;

	bool output = false;
	bool tanh = false;

	CString data_label;
	CString node_label;

	Linegraph mgraph1;
	Histogram mgraph2;

	double nmin;
	double nmax;

	vector<vector<double>> data;
	vector<double> ddata;
	vector<double> xdata;
	vector<double> normalized_data;

	int m_derivative;
	CString m_stride;
	CString m_intercept;
	CString m_slope;
	BOOL m_relative;

	CStatic m_static_box_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void addErrorString(CString err);

	void setSize(CRect rect);
	void setDataLabel(CString label) { data_label = label; };
	void setNodeLabel(CString label) { node_label = label; };
	void setOutput(bool out) { output = out; };
	void setTanh(bool tnh) { tanh = tnh; };

	void setData(vector<vector<double>> ddata);
	void calcNormalizationFactors();
	void setNormalizedData();
	void getDerivative();

	CString getMode();
	void setMode(CString mode);

	double getLowerNormalizationFactor();
	void setLowerNormalizationFactor(double factor);

	double getUpperNormalizationFactor();
	void setUpperNormalizationFactor(double factor);

	int getStride();
	void setStride(int stride);

	afx_msg void OnBnClickedLinear();
	afx_msg void OnBnClickedSecond();
	afx_msg void OnBnClickedThird();
	afx_msg void OnBnClickedFourth();

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL DestroyWindow();

	afx_msg void OnBnClickedRelative();
	afx_msg void OnEnChangeSleak();


};
