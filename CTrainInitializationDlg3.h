#pragma once


// CTrainInitializationDlg3 dialog

class CTrainInitializationDlg3 : public CDialog
{
	DECLARE_DYNAMIC(CTrainInitializationDlg3)

public:
	CTrainInitializationDlg3(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTrainInitializationDlg3();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INITIALIZATION3 };
#endif

	bool continue_weights = false;
	bool randomize = false;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedContinue();
	afx_msg void OnBnClickedRandomize();

};
