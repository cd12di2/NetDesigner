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

#pragma once

class CTreeCtrlCustom : public CTreeCtrl
{
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

class CToolboxWnd : public CDockablePane
{
// Construction
public:
	CToolboxWnd() noexcept;

	void AdjustLayout();

// Attributes
protected:
	int theme = 0;

	CFont m_fntPropList;
	CTreeCtrlCustom m_wndToolList2;
	CImageList m_TreeViewImages;

// Implementation
public:
	virtual ~CToolboxWnd();

	void setTheme(int ntheme) { theme = ntheme; };
	int getTheme() { return(theme); };

	void InitToolbox();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	DECLARE_MESSAGE_MAP()

	void SetToolboxFont();

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

};

