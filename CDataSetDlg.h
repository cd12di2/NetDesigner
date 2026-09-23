#pragma once


// CDataSetDlg dialog

class CDataSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataSetDlg)

public:
	CDataSetDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDataSetDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_SETS };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	CString m_percent;

	int m_no_validation;

	int data_type;

	vector<CString> mfilenames;

	CEdit m_percent_control;

	CComboBox m_combo_ctrl;
	CString m_combo;

	CButton m_select_control;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void addErrorString(CString err);

	void setFilesnamesToList();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();

	afx_msg void OnBnClickedFileOpen();
	afx_msg void OnBnClickedNoValidation();
	afx_msg void OnBnClickedRandomized();
	afx_msg void OnBnClickedFromFile();

	afx_msg void OnBnClickedSelectFile2();


};
