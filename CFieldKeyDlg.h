#pragma once


// CFieldKeyDlg dialog

class CFieldKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CFieldKeyDlg)

public:
	CFieldKeyDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFieldKeyDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIELD_KEY };
#endif

	bool data_updated;

	bool tanh = false;

	double nfactormin;
	double nfactormax;

	CEdit m_edit1_ctrl;
	CString m_edit1;
	CListBox m_list3_ctrl;
	CStatic m_name3_ctrl;
	CStatic m_name2_ctrl;
	CStatic m_line1_ctrl;
	CButton m_scale_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void addErrorString(CString err);

	void setSize(CRect mrect);
	void setData(vector<vector<CString>> stringdata);

	void setTanh(bool tnh) { tanh = tnh; };

	CString getMode();
	void setMode(CString mode);

	void setNameKey(CString key);
	CString getNameKey();

	double getRealLowerNormalizationFactor();
	void setRealLowerNormalizationFactor(double factor);
	double getRealUpperNormalizationFactor();
	void setRealUpperNormalizationFactor(double factor);

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL DestroyWindow();

	afx_msg void OnLbnSelchangeList3();
	afx_msg void OnBnClickedNormOptions();



};
