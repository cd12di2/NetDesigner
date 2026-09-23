#pragma once


// CArchiveWeights dialog

class CArchiveWeights : public CDialog
{
	DECLARE_DYNAMIC(CArchiveWeights)

public:
	CArchiveWeights(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CArchiveWeights();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAVE_WEIGHTS_DLG };
#endif
	CBrush mbrush;
	CBrush mbrush2;

	bool modified;

	BOOL m_save_weights;
	BOOL m_train_response;
	BOOL m_validation_response;
	CString m_delay_epochs;
	CString m_repeat_epochs;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRestoreWeights();

};
