#pragma once


// CAutoRnnConfigDlg dialog

class CAutoRnnConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CAutoRnnConfigDlg)

public:
	CAutoRnnConfigDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAutoRnnConfigDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTORNN };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	CPoint spt;

	netDataStruct tdata;
	NetData newGroupData;

	BOOL m_normalize;
	CString m_num_sections;
	CString m_num_hidden;

	CString m_activation_function;
	CString m_eta;
	CString m_alpha;
	CString m_rand;

	CComboBox m_activation_function_control;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	NetData makeNet(CPoint mpt, int gindex);

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
