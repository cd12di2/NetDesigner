#pragma once


// CInputOptimization dialog

class CInputOptimization : public CDialog
{
	DECLARE_DYNAMIC(CInputOptimization)

public:
	CInputOptimization(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInputOptimization();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_OPTIMIZATION };
#endif

	BOOL m_optimize_inputs;
	int m_train_error;
	CString m_list1;

	CButton m_optimize_inputs_ctrl;
	CButton m_train_error_ctrl;
	CButton m_validation_error_ctrl;
	CListBox m_list1_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void setSize(CRect rect);
	void displaySetupData();
	void addLine1(CString line);
	void setOptimization();


	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedEvaluateInputs();
	afx_msg void OnBnClickedOptimizeTrain();
	afx_msg void OnBnClickedOptimizeValidation();



};
