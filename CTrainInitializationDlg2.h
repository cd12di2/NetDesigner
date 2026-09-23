#pragma once


// CTrainInitializationDlg2 dialog

class CTrainInitializationDlg2 : public CDialog
{
	DECLARE_DYNAMIC(CTrainInitializationDlg2)

public:
	CTrainInitializationDlg2(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTrainInitializationDlg2();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INITIALIZATION2 };
#endif

	bool saved_weights = false;
	bool randomize = false;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSavedWeights();
	afx_msg void OnBnClickedRandomize();
};
