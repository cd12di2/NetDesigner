#pragma once


// NormalizationFactor dialog

class NormalizationFactor : public CDialog
{
	DECLARE_DYNAMIC(NormalizationFactor)

public:
	NormalizationFactor(CWnd* pParent = nullptr);   // standard constructor
	virtual ~NormalizationFactor();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NORM_SCALE };
#endif

	CString m_lower;
	CString m_upper;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:


};
