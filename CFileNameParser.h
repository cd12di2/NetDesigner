#pragma once


// CFileNameParser dialog

class CFileNameParser : public CDialog
{
	DECLARE_DYNAMIC(CFileNameParser)

public:
	CFileNameParser(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFileNameParser();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGE_NAME_PARSER };
#endif
	
	bool data_updated;

	bool tanh = false;

	double nfactormin;
	double nfactormax;

	CString m_key;

	CStatic m_name1_ctrl;
	CStatic m_name2_ctrl;
	CStatic m_name3_ctrl;
	CListBox m_list_ctrl2;
	CListBox m_list_ctrl3;
	CEdit m_key_ctrl;
	CStatic m_line1_ctrl;
	CButton m_scale_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void addErrorString(CString err);
		
	void setSize(CRect rect);

	void setTanh(bool tnh) { tanh = tnh; };

	CString getMode();
	void setMode(CString mode);

	void setNameKey(CString key);
	CString getNameKey();

	double getRealLowerNormalizationFactor();
	void setRealLowerNormalizationFactor(double factor);
	double getRealUpperNormalizationFactor();
	void setRealUpperNormalizationFactor(double factor);

	void setListBox();

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnLbnSelchangeList3();
	afx_msg void OnBnClickedNormOptions();


};
