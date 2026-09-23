#pragma once


// CStatus dialog

class CStatus : public CDialog
{
	DECLARE_DYNAMIC(CStatus)

public:
	CStatus(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStatus();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STATUS };
#endif

	bool displayed;
	bool cancel;
	bool cancellable;

	CBrush mbrush;
	CBrush mbrush2;

	CString m_label;
	CButton m_cancel_ctrl;
	CProgressCtrl m_progress_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void CreateDlg();
	void DestroyDlg();

	void setLabel(string label);
	void setLabel(CString label);
	void addErrorString(CString err);
	void setCancellable(bool value) { cancellable = value; };

	//void updateProgress(int percent);
	void setProgress(int percent);

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedStatusCancel();


};
