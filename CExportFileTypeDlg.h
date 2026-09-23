#pragma once


// CExportFileTypeDlg dialog

class CExportFileTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CExportFileTypeDlg)

public:
	CExportFileTypeDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CExportFileTypeDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPORT };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	BOOL m_c_file;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();


};
