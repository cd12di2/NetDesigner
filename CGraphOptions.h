#pragma once


// CGraphOptions dialog

class CGraphOptions : public CDialog
{
	DECLARE_DYNAMIC(CGraphOptions)

public:
	CGraphOptions(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CGraphOptions();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRAPH_OPTIONS };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	bool yminauto;
	bool ymaxauto;

	CString default_ymin;
	CString default_ymax;

	CString m_ymin;
	CString m_ymax;
	CButton m_ymin_button_ctrl;
	CButton m_ymax_button_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void setButtons();

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedYminButton();
	afx_msg void OnBnClickedYmaxButton();
	afx_msg void OnEnChangeYmin();
	afx_msg void OnEnChangeYmax();
};
