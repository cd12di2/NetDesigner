#pragma once

#include "Graph.h"

// CActivation dialog

class CActivation : public CDialog
{
	DECLARE_DYNAMIC(CActivation)

public:
	CActivation(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CActivation();

	CBrush mbrush;
	CBrush mbrush2;

	vector<double> sxdata;
	vector<double> sydata1;
	vector<double> sydata2;

	Linegraph2 mgraph;

	CComboBox m_activation_function_control;
	CString m_activation_function;

	CString m_leakage;

	CString m_bias_text;
	CStatic m_bias_text_ctrl;

	CSliderCtrl m_slider_ctrl;


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACTIVATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	//void reset();
	void setSize(CRect rect);
	void updateGraph();
	void updateBias();

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEnChangeSleak();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	virtual void OnOK();
	virtual void OnCancel();

};
