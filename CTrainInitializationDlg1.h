#pragma once


// CTrainInitializationDlg1 dialog

class CTrainInitializationDlg1 : public CDialog
{
	DECLARE_DYNAMIC(CTrainInitializationDlg1)

public:
	CTrainInitializationDlg1(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTrainInitializationDlg1();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INITIALIZATION1 };
#endif

	bool continue_weights = false;
	bool saved_weights = false;
	bool randomize = false;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedContinue();
	afx_msg void OnBnClickedSavedWeights();
	afx_msg void OnBnClickedRandomize();

};
