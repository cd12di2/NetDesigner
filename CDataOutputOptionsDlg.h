#pragma once


// CDataOutputOptionsDlg dialog

class CDataOutputOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataOutputOptionsDlg)

public:
	CDataOutputOptionsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDataOutputOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAVE_DATA_OPTIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBrush mbrush;
	CBrush mbrush2;

	CString m_notes;
	BOOL m_net_output;
	BOOL m_train_output;
	BOOL m_train_input;
	BOOL m_classification;
	CButton m_net_output_ctrl;
	CButton m_train_output_ctrl;
	CButton m_train_input_ctrl;
	CButton m_classification_ctrl;
	CStatic m_static1_ctrl;
	CStatic m_static2_ctrl;
	CStatic m_static3_ctrl;
	CStatic m_static4_ctrl;

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

};
