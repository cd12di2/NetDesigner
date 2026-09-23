#pragma once

#include "Graph.h"


// CClusterAnalysisDlg dialog

class CClusterAnalysisDlg : public CDialog
{
	DECLARE_DYNAMIC(CClusterAnalysisDlg)

public:
	CClusterAnalysisDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CClusterAnalysisDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLUSTER_DLG };
#endif

	bool ready;
	bool dataOk;
	bool displayed;

	bool validGraphBox;
	CRect graphBox;

	CBrush mbrush;
	CBrush mbrush2;

	LinegraphMulti mgraph;

	BOOL m_check;
	CString m_nclusters;

	CEdit m_nclusters_ctrl;
	CButton m_check_ctrl;
	CButton m_button_ctrl;
	CButton m_save_ctrl;
	CListBox m_list1_ctrl;
	CListBox m_list2_ctrl;
	CStatic m_static1_ctrl;
	CStatic m_static2_ctrl;
	CStatic m_static3_ctrl;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void addErrorString(CString err);
	void addMyErrorString(CString err);
	void clearMyErrorString();

	void CreateDlg();
	void DestroyDlg();

	void setSize();

	void setTheme(int ntheme) { mgraph.setTheme(ntheme); };
	void updateGraph(vector<double> sxdata, vector<vector<double>> sydata);

	void displayClusterStandardDeviations();

	void setButtonOff();

	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedSave();



};
