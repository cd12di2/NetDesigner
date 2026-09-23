#pragma once


// CStatus2 dialog

class CStatus2 : public CDialog
{
	DECLARE_DYNAMIC(CStatus2)

public:
	CStatus2(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStatus2();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STATUS2 };
#endif

	bool displayed;

	CBrush mbrush;
	CBrush mbrush2;

	CString m_label;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void CreateDlg();
	void DestroyDlg();

	void setLabel(CString label);

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


};
