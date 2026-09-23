#pragma once

// CFileTypeSelectDlg dialog

class CFileTypeSelectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileTypeSelectDlg)

public:
	CFileTypeSelectDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFileTypeSelectDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_TYPE };
#endif

	CString type;

	vector<pair<CString, int>> mfile_types;

	CBrush mbrush;
	CBrush mbrush2;

	CListBox m_list_ctrl;
	CString m_list_value;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CString getSelectedType();
	void setFiles(vector<pair<CString, int>> file_types);

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

};
