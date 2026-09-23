#pragma once


// CLayerDataDlg dialog

class CLayerDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CLayerDataDlg)

public:
	CLayerDataDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLayerDataDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAYER_DATA };
#endif
	int dindex = -1;

	int lastfieldindex = -1;
	int lastdataindex = -1;

	vector<CString> filesheaders;
	vector<CString> listheaders;
	vector<string> listinputs;

	CBrush mbrush;
	CBrush mbrush2;

	//CString m_list1;
	//CString m_list2;

	CButton m_ok_ctrl;
	CButton m_add_ctrl;
	CButton m_remove_ctrl;
	CListBox m_list1_ctrl;
	CListBox m_list2_ctrl;
	CStatic m_static1_ctrl;
	CStatic m_static2_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void addErrorString(CString err);

	void adjustSize();
	void getNetInputFields();
	void loadListBoxes();

	void setComponentIndex(int nindex) { dindex = nindex; };

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);


	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnLbnSelcancelList1();
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnLbnDblclkList2();
	afx_msg void OnLbnSelcancelList2();
};
