#pragma once


#include "Graph.h"


// CNormalize dialog

class CNormalize : public CDialog
{
	DECLARE_DYNAMIC(CNormalize)

public:
	CNormalize(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CNormalize();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NORMALIZE };
#endif

	bool data_updated;

	bool calc_limits = true;
	bool zero_log_message = false;

	bool output = false;
	bool tanh = false;

	CString data_label;
	CString node_label;

	Histogram mgraph1;
	Histogram mgraph2;

	double ynot;
	double slope;

	double nfactormin;
	double nfactormax;

	vector<double> data;
	vector<double> normalized_data;

	BOOL m_linear;
	BOOL m_logarithmic;

	CString m_lower;
	CString m_upper;

	CStatic m_static_box_ctrl;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void addErrorString(CString err);

	void setSize(CRect rect);
	void setData(vector<vector<double>> ddata);
	void setNormalizedData();
	void setNormalizedLinearData();
	void setNormalizedLogData();
	void setDataLabel(CString label) { data_label = label; };
	void setNodeLabel(CString label) { node_label = label; };
	void setLinear() { m_linear = true; m_logarithmic = false; };
	void setOutput(bool out) { output = out; };
	void setTanh(bool tnh) { tanh = tnh; };
	bool checkLogOk();

	CString getMode();
	void setMode(CString mode);

	double getLowerNormalizationFactor();
	void setLowerNormalizationFactor(double factor);

	double getUpperNormalizationFactor();
	void setUpperNormalizationFactor(double factor);

	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedLinear();
	afx_msg void OnBnClickedLogarithmic();

	//afx_msg void OnEnChangeLowerNormFactor();
	//afx_msg void OnEnChangeUpperNormFacter();

	virtual BOOL DestroyWindow();
	virtual void OnCancel();
	virtual void OnOK();

	afx_msg void OnBnClickedResetFactors();
	afx_msg void OnBnClickedNormOptions();

};
