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

#include "ViewTree.h"
#include "NetDesignerDoc.h"


class CViewTreeCustom : public CViewTree
{
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

class CNodeView : public CDockablePane
{
// Construction
public:
	CNodeView() noexcept;

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
protected:

	int theme = 0;

	int size_limit = 1000;

	HTREEITEM lastSelectedItem;

	CViewTreeCustom m_wndNodeView;
	CImageList m_NodeViewImages;

public:
	void FillFileView();
	void selectItem();

	void setTheme(int ntheme) { theme = ntheme; };
	int getTheme() { return(theme); };

	void addPanel1String(CString msg);

// Implementation
public:
	virtual ~CNodeView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

};

