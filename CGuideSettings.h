#pragma once


// CGuideSettings dialog

class CGuideSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CGuideSettings)

public:
	CGuideSettings(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CGuideSettings();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GUIDESETTINGS };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	CString m_spacing;
	CString m_spacing2;
	CComboBox m_spacing_control;
	CComboBox m_spacing2_control;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();


	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
