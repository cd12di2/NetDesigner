#pragma once


// CSeedDlg dialog

class CSeedDlg : public CDialog
{
	DECLARE_DYNAMIC(CSeedDlg)

public:
	CSeedDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSeedDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RANDOM_SEED };
#endif
	
	CBrush mbrush;
	CBrush mbrush2;

	CString m_seed;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);



};
