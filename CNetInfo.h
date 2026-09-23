#pragma once


// CNetInfo dialog

class CNetInfo : public CDialog
{
	DECLARE_DYNAMIC(CNetInfo)

public:
	CNetInfo(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CNetInfo();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INFO };
#endif

	CBrush mbrush;
	CBrush mbrush2;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	CString m_notes;
};
