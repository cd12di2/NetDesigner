#pragma once

#include "Graph.h"

class ClusterGraphWnd : public CDockablePane
{
	DECLARE_DYNAMIC(ClusterGraphWnd)

public:

	CBrush mbrush;

	Linegraph mgraph;


public:
	ClusterGraphWnd();
	virtual ~ClusterGraphWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:

	void reset(CString title);

	void setTitle(CString ntitle);

	void setTheme(int ntheme) { mgraph.setTheme(ntheme); };

	void adjustSize();
	void updateGraph(vector<double>* sxdata, vector<vector<double>>* sydata);

	//afx_msg void OnSaveCSV();
	//afx_msg void OnAdjustScale();

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	afx_msg void OnSelectDecoder();


};

