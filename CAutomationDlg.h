#pragma once
#include "CInputOptimization.h"


// CAutomationDlg dialog

class CAutomationDlg : public CDialog
{
	DECLARE_DYNAMIC(CAutomationDlg)

public:
	CAutomationDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAutomationDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTOMATION_DLG };
#endif

	bool ready;
	bool displayed;
	bool changed;

	int currentTab;

	CBrush mbrush;
	CBrush mbrush2;

	bool inputUp;

	//BOOL m_evaluate_inputs;
	//CListBox m_list1;

	CButton m_ok_ctrl;
	CTabCtrl m_tab1_control;

	CInputOptimization inputDlg;

public:
	void CreateDlg();
	void DestroyDlg();

	void initializeTabs();
		
	void adjustSize();
	void setSize();

	void setDataToDialogs();

	void setCurrentTab();
	void setTab(int ntab);

	bool isDisplayed() { return(displayed); };

	void addInputOptimizationLine(CString line);

	void addErrorString(CString err);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL DestroyWindow();
	virtual void OnCancel();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);


};
