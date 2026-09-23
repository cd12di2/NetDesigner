#pragma once
#include "CNormalize.h"
#include "CDateTime.h"
#include "CFFT.h"
#include "CDerivative.h"
#include "CFileNameParser.h"
#include "CFieldKeyDlg.h"


// CPreProcessor dialog

class CPreProcessor : public CDialog
{
	DECLARE_DYNAMIC(CPreProcessor)

public:
	CPreProcessor(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPreProcessor();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATH };
#endif

	bool ready;
	bool displayed;
	bool outputnode;
	bool imagefiles;
	bool soundfiles;
	//bool textfiles;
	bool normalize_only;
	bool tanh;
	bool changed;
	CString node_id;
	CString input_node_id;
	CString input_data;
	vector<vector<CString>> stringdata; int here;
	vector<vector<double>> doubledata;

	int currentTab;
	
	CBrush mbrush;

	CButton m_ok_control;
	CTabCtrl m_tab1_control;

	bool normalizeUp;
	bool datetimeUp;
	bool fftUp;
	bool derivativeUp;
	bool fnparserUp;
	//bool dictionaryUp;
	//bool windexUp;
	bool fieldkeyUp;

	CNormalize normalizeDlg;
	CDateTime datetimeDlg;
	CFFT fftDlg;
	CDerivative derivativeDlg;
	CFileNameParser fnparserDlg; 
	//CWordIndex windexDlg;
	CFieldKeyDlg fieldkeyDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void addErrorString(CString err);

	void CreateDlg();
	void DestroyDlg();

	bool isDisplayed() { return(displayed); };

	void setSize();
	void adjustSize();

	CString getMode();
	void setMode(CString mode, int ftype);
	void setfileType(int ftype);
	int getTabFromMode(CString mode);

	void setOutput(bool toutputnode);
	void setDataToDialogs();
	void setTab(CString preprocessor);
	void setTab(int ntab);
	void setCurrentTab();

	double getLowerNormalizationFactor();
	void setLowerNormalizationFactor(double factor);

	double getUpperNormalizationFactor();
	void setUpperNormalizationFactor(double factor);

	double getRealLowerNormalizationFactor();
	void setRealLowerNormalizationFactor(double factor);

	double getRealUpperNormalizationFactor();
	void setRealUpperNormalizationFactor(double factor);

	CString getTime();
	void setTime(CString time);
		
	int getStride();
	void setStride(int stride, bool lock_one);

	int getDownSample();
	void setDownSample(int downsample);

	void setNameKey(CString key);
	CString getNameKey();

	void setOffsetIndex(int mindex);
	int getOffsetIndex();

	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();


	virtual void OnCancel();
	virtual BOOL DestroyWindow();

};
