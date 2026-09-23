#pragma once


// CAssemblyPicker dialog

class CAssemblyPicker : public CDialog
{
	DECLARE_DYNAMIC(CAssemblyPicker)

public:
	CAssemblyPicker(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAssemblyPicker();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASSEMBLY_PICKER_DLG };
#endif

	CBrush mbrush;
	CBrush mbrush2;

	CPoint spt;

	vector<string> assemblies;

	BOOL m_new;
	BOOL m_select;
	CString m_assembly;

	CComboBox m_assemblies_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedNewAssembly();
	afx_msg void OnBnClickedExistingAssembly();

};
