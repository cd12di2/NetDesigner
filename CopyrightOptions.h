#pragma once


// CopyrightOptions dialog

class CopyrightOptions : public CDialog
{
	DECLARE_DYNAMIC(CopyrightOptions)

public:
	CopyrightOptions(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CopyrightOptions();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COPYRIGHT_OPTIONS };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	CListBox m_list2_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void setSize(CRect rect);

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	virtual void OnOK();
	virtual void OnCancel();
};
