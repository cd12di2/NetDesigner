#pragma once


// CEvaluateDlg dialog

class CEvaluateDlg : public CDialog
{
	DECLARE_DYNAMIC(CEvaluateDlg)

public:
	CEvaluateDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEvaluateDlg();

	// Dialog Data

	CBrush mbrush;
	CBrush mbrush2;

	int input_label_id;
	int input_value_id;
	int output_label_id;
	int output_value_id;

	int x_offset;
	int y_offset;
	int x_separation;
	int y_separation;
	int image_border;

	int static_height;
	int static_width;

	int edit_height;
	int edit_width;

	int ninputs;
	int noutputs;

	int image_width;
	int image_height;

	bool allocated; 
	bool use_images_as_outputs;

	CStatic m_vert_ctrl;
	CStatic m_horz1_ctrl;
	CStatic m_horz2_ctrl;
	CStatic m_outline_ctrl;
	CButton m_evaluate_ctrl;

	CString m_label2;
	CStatic m_label1_ctrl;
	CStatic m_label2_ctrl;

	vector<CString> netInputs;
	vector<CString> netOutputs;

	CStatic* input_labels;
	CEdit* input_values;

	CStatic* output_labels;
	CEdit* output_values;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EVALUATE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void updateSize();

	CString getInputField(int index);
	bool doesInputMatch(int index, CString tname);
	
	void saveDataBeforeExit();

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedEvaluate();
	virtual BOOL DestroyWindow();
	virtual void OnCancel();


};
