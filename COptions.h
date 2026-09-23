#pragma once

#include "DefaultOptionsDlg.h"
#include "CopyrightOptions.h"
#include "CActivation.h"

// COptions dialog

class COptions : public CDialog
{
	DECLARE_DYNAMIC(COptions)

public:
	COptions(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COptions();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS };
#endif

	bool ready;

	int currentTab;

	CBrush mbrush;
	CBrush mbrush2;

	CTabCtrl m_tab1_control;

	DefaultOptionsDlg defaultOptionsDlg;
	CopyrightOptions copyrightOptionsDlg;
	CActivation activationFunctionsDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void setSize();

	virtual BOOL OnInitDialog();

	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);

};
