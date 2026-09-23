#pragma once


// CAutoNetConfigDlg dialog

class CAutoNetConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAutoNetConfigDlg)

public:
	CAutoNetConfigDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAutoNetConfigDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTONET };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	CPoint spt;

	netDataStruct tdata;
	NetData newGroupData;

	BOOL m_inputs;
	BOOL m_hidden;
	BOOL m_outputs;
	BOOL m_normalize;
	BOOL m_compact;
	CString m_num_inputs;
	CString m_num_hidden;
	CString m_num_outputs;
	CString m_hidden_width;

	CString m_activation_function;
	CString m_eta;
	CString m_alpha;
	CString m_rand;

	CComboBox m_activation_function_control;

	CEdit m_num_inputs_ctrl;
	CEdit m_num_layers_ctrl;
	CEdit m_hidden_width_ctrl;
	CEdit m_num_outputs_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	NetData makeNet(CPoint mpt, int nseparation, int nsnap, int gindex);

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedInputNodes();
	afx_msg void OnBnClickedHiddenNodes();
	afx_msg void OnBnClickedOutputNode2();


};
