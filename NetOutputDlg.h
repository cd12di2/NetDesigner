#pragma once


// NetOutputDlg dialog

class NetOutputDlg : public CDialog
{
	DECLARE_DYNAMIC(NetOutputDlg)

public:
	NetOutputDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~NetOutputDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OUTPUT_VIEW };
#endif

	int ncols;

	bool dialog_up;

	bool extend_last_column_width;
	int extend_last_column_characters;

	bool classification_matrix;

	vector<CString> dinputheaders;
	vector<CString> doutputheaders;

	int max_train_data;
	int max_validation_data;
	int max_display_data;

	int trows;
	int vrows;

	int dcols;
	int drows;
	vector<CString> ddata;

	double trainClassificationAccuracy;
	double validationClassificationAccuracy;
	vector<vector<int>> trainClassificationCounts;
	vector<vector<int>> validationClassificationCounts;

	CBrush m_brush;

	CListCtrl m_list_control;
	CButton m_save_control;
	CButton m_ok_button_control;
	CButton m_show_ctrl;
	CProgressCtrl m_progress_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void adjustSize();
	void fillHeaders();
	void fillData();
	void getData(bool all_rows = false, bool include_inputs = true);
	//CString getWordError(textParameters tparams, vector<int> windices, int windex, int wfindex);
	CString findFirstWord(CString word);
	CString findSecondWord(CString word);

	void setClassification(bool yes) { classification_matrix = yes; };
	double getClassificationMitrices();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedSaveCsv();
	afx_msg void OnBnClickedShowAll();


};
