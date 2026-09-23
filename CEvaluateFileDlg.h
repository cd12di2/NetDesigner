#pragma once


// CEvaluateFileDlg dialog

class CEvaluateFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CEvaluateFileDlg)

public:
	CEvaluateFileDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEvaluateFileDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EVALUATE_FILE };
#endif

	CBrush m_brush;

	int ncols;

	CString filename;

	vector<CString> headers;
	vector<vector<CString>> data;

	vector<CString> outputLabels;
	vector<vector<CString>> outputData;

	vector<int> inputIndices;

	CListCtrl m_list_control;
	CButton m_save_control;
	CButton m_file_ctrl;
	CProgressCtrl m_progress_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void adjustSize();
	void fillHeaders();
	void fillData();

	bool doesInputMatch(CString mname, CString tname);
	CString getOutputString(CString nname);

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	afx_msg void OnBnClickedLoadData();
	afx_msg void OnBnClickedSaveCsv();


	virtual void OnCancel();
};
