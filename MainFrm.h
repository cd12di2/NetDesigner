// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "NodeView.h"
#include "OutputWnd.h"
#include "ToolboxWnd.h"
#include "PropertiesWnd.h"
#include "GraphWnd.h"
#include "ClusterGraphWnd.h"
#include "DataView.h"


#define RELEASIBILITY	3
#define ERROR_CHECKING	0


class CMFCRibbonApplicationButtonCustom : public CMFCRibbonApplicationButton
{
public:
	CMFCRibbonApplicationButtonCustom() {
		SetText(L"File");
	}
	void OnDraw(CDC* pDC) {
		CString str = GetText();
		if (!str.IsEmpty()) {
			COLORREF clrTextOld = pDC->SetTextColor(GetGlobalData()->clrTextHilite/*GetSysColor(COLOR_HIGHLIGHTTEXT)*/);
			pDC->DrawText(str, GetRect(), DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			pDC->SetTextColor(clrTextOld);
		}
	}
};

class CMFCRibbonSliderCustom : public CMFCRibbonSlider
{
	int pos = 0;
	int previous = 0;
public:
	CMFCRibbonSliderCustom();
	void OnLButtonUp(CPoint point);
	void OnMouseMove(CPoint point);
	void OnDraw(CDC* dDc);
	void updatePosition();
	float getScale();
	bool setScale(float newscale);
	int setZoomOnFive(int pos);
};

class CRibbonBar : public CMFCRibbonBar
{
public:
	void delQAT()
	{
		m_QAToolbar.RemoveAll(); //m_QAToolbar is a protected member of CMFCRibbon
	}
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	int theme = 1;

// Operations
public:
	void setTheme(int ntheme) { theme = ntheme; };
	int getTheme() { return(theme); };

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CMFCRibbonSliderCustom  m_ribbonSlider;
	CMFCRibbonProgressBar	m_progress_ctrl;
	CNodeView         m_wndNodeView;
	CPropertiesWnd    m_wndProperties;
	COutputWnd        m_wndOutput;
	CToolboxWnd		  m_wndToolbox;
	GraphWnd		  m_wndGraph;
	ClusterGraphWnd	  m_wndClusterGraph;
	DataView		  m_wndData;

protected:  // control bar embedded members
	CRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButtonCustom m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;

public:
	void SetStatusBarText1(CString text)
	{
		m_wndStatusBar.GetElement(0)->SetText(text);
		m_wndStatusBar.GetElement(0)->NotifyCommand();
	}

	void SetStatusBarText2(CString text)
	{
		m_wndStatusBar.GetExElement(1)->SetText(text);
		m_wndStatusBar.GetExElement(1)->NotifyCommand();
	}

	void updateLook();

	void setEpochs(CString data);
	CString getEpochs();
	void setMinEpochs(CString data);
	CString getMinEpochs();
	void setTrainTime(CString data);
	CString getTrainTime();
	void setTargetError(CString data);
	CString getTargetError();
	void setTargetDeltaError(CString data);
	CString getTargetDeltaError();
	void setUpdateRate(CString data);
	CString getUpdateRate();

	void enableProgress(bool enable);
	void setProgress(int progress);

	void OnDataView();


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnToolsPanel();
	afx_msg void OnUpdateToolsPanel(CCmdUI* pCmdUI);
	afx_msg void OnOutputPanel();
	afx_msg void OnUpdateOutputPanel(CCmdUI* pCmdUI);
	afx_msg void OnNodeView();
	afx_msg void OnUpdateNodeView(CCmdUI* pCmdUI);
	afx_msg void OnProperties();
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnGraphTrainError();
	afx_msg void OnUpdateGraphTrainError(CCmdUI* pCmdUI);
	afx_msg void OnGraphOutputs();
	afx_msg void OnUpdateGraphOutputs(CCmdUI* pCmdUI);
	afx_msg void OnClusterGraph();
	afx_msg void OnUpdateClusterGraph(CCmdUI* pCmdUI);


};


