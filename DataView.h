#pragma once

//#include <afxdockablepane.h>

#include "NetDesignerDoc.h"
#include "Graph.h"

class DataView : public CDockablePane
{
// Construction
public:
	DataView() noexcept;

	void AdjustLayout();

public:

	CBrush mbrush;
	CBrush mbrush2;

	COLORREF clrGrayedText;

	int data_type;

	vector<CString> mfilenames;

	int ncols;
	bool show_progress;
	bool waiting;
	bool folder;

	bool images_combined;

	vector<CString> headers;
	vector<vector<vector<CString>>> dataSourceData;


	bool ready;
	bool conv_options;
	bool allocated;
	int awidth;
	int aheight;
	CRect image_rect;
	int min_width;
	float optimum_aspect;
	unsigned char* image_data;
	int image_left;
	int image_top;
	CImage image;
	BITMAP pBitMap;

	Linegraph mgraph;

	CButton m_select_control;
	CButton m_folder_control;

	CString m_combo;
	CComboBox m_combo_ctrl;
	
	CListCtrl m_list_control;

	CString m_notes;
	CString m_text;
	CString m_width;
	CString m_height;
	BOOL m_gray;

	BOOL m_conv1;
	BOOL m_conv2;
	BOOL m_conv3;

protected:
	CFont m_fnt;

	CButton m_update_control;
	CButton m_gray_control;
	CButton m_conv1_control;
	CButton m_conv2_control;
	CButton m_conv3_control;
	CStatic m_label1;
	CStatic m_label2;
	CStatic m_label3;
	CStatic m_label4;
	CStatic m_label5;
	CStatic m_label6;
	CStatic m_notes_ctrl;
	CStatic m_text_ctrl;
	CEdit m_edit1;
	CEdit m_edit2;

	// Implementation
public:
	virtual ~DataView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void addErrorString(CString err);
	void addPanel1String(CString msg);

	void allocate();
	void deallocate();

	void SetFont();

	void setColors(int theme);
	COLORREF adjustColorByPercent(COLORREF clr, float percent);

	void updateFromNet();
	void setFilesnamesToList();

	void setCsvData();
	void updateCSV();
	void fillHeaders();
	void fillData();

	void setJpgData();
	void updateImage();
	void adjustSize();

	void setRawData();

	void setWavData();
	void updateWave();

	void setTextData();

	void setConv(int conv);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnBnClickedSelectFile();
	afx_msg void OnUpdateSelectFile(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedSelectFolder();
	afx_msg void OnUpdateSelectFolder(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedGreyScale();
	afx_msg void OnUpdateClickedGreyScale(CCmdUI* pCmdUI);

	afx_msg void OnBnClickedConv1();
	afx_msg void OnUpdateClickedConv1(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedConv2();
	afx_msg void OnUpdateClickedConv2(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedConv3();
	afx_msg void OnUpdateClickedConv3(CCmdUI* pCmdUI);

	afx_msg void OnEnChangeWidth();

	afx_msg void OnBnClickedUpdate();
	afx_msg void OnUpdateUpdate(CCmdUI* pCmdUI);

	afx_msg void OnCbnSelchangeCombo();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

