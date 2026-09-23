#pragma once

#include "Graph.h"

// CFFT dialog

class CFFT : public CDialog
{
	DECLARE_DYNAMIC(CFFT)

public:
	CFFT(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFFT();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FFT };
#endif

	bool data_updated;
	bool lock_one_stride;

	CString data_label;
	CString node_label;

	Linegraph mgraph1;
	Histogram mgraph2;

	double nmin;
	double nmax;

	vector<double> data;
	vector<double> ddata;
	vector<double> xdata;
	vector<double> ddata2;
	vector<double> normalized_data;

	CString m_n;
	CString m_stride;
	CString m_downsample;
	CString m_intercept;
	CString m_slope;

	CComboBox m_n_ctrl;
	CEdit m_stride_ctrl;

	CStatic m_static_box_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void setSize(CRect rect);
	void setDataLabel(CString label) { data_label = label; };
	void setNodeLabel(CString label) { node_label = label; };

	void setData(vector<double> ddata);

	afx_msg void OnCbnSelchangeCombo1();

	void calcNormalizationFactors();
	void setNormalizedData();

	CString getMode();
	void setMode(CString mode);

	double getLowerNormalizationFactor();
	void setLowerNormalizationFactor(double factor);

	double getUpperNormalizationFactor();
	void setUpperNormalizationFactor(double factor);

	int getStride();
	void setStride(int stride, bool lock_one);

	int getDownSample();
	void setDownSample(int downsample);

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL DestroyWindow();

	afx_msg void OnEnChangeSleak();
	afx_msg void OnEnChangeSleak2();


};
