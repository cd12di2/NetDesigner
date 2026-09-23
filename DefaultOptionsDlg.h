#pragma once


// DefaultOptionsDlg dialog

class DefaultOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(DefaultOptionsDlg)

public:
	DefaultOptionsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DefaultOptionsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEFAULT_OPTIONS };
#endif
	CBrush mbrush;
	CBrush mbrush2;

	CComboBox m_activation_function_control;
	CString m_activation_function;
	CString m_eta;
	CString m_alpha;
	CString m_rand;
	CString m_sleakage;
	CString m_rleakage;
	CString m_filter;
	CString m_pprocessing;
	BOOL m_normalize_fileset;
	BOOL m_normalize_files;

	CString m_minimum_sound_level;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void setSize(CRect rect);

	void saveOptions();

	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedSaveOptions();

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	virtual void OnCancel();

	afx_msg void OnBnClickedNormalizeFileSet();
	afx_msg void OnBnClickedNormalizePerFile();


	afx_msg void OnBnClickedCartoon();
	BOOL m_cartoon;
};
