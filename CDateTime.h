#pragma once


// CDateTime dialog

class CDateTime : public CDialog
{
	DECLARE_DYNAMIC(CDateTime)

public:
	CDateTime(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDateTime();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATE_TIME };
#endif
	bool data_updated;

	double nmin;
	double nmax;

	vector<CString> stringdata;
	vector<double> doubledata;

	CListCtrl m_list_ctrl;
	COleDateTime m_date;
	BOOL m_selection;
	BOOL m_normalize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void addErrorString(CString err);

	void setSize(CRect rect);
	void setData(vector<vector<CString>> sdata);
	
	CString getMode();
	void setMode(CString mode);

	double getLowerNormalizationFactor();
	void setLowerNormalizationFactor(double factor);
	double getUpperNormalizationFactor();
	void setUpperNormalizationFactor(double factor);

	CString getTime();
	void setTime(CString time);

	void setListBox();

	afx_msg void OnBnClickedSeconds();
	afx_msg void OnBnClickedMinutes();
	afx_msg void OnBnClickedHours();
	afx_msg void OnBnClickedDays();

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedNormalize();


};
