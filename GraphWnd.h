#pragma once

#include "Graph.h"

// GraphWnd

class GraphWnd : public CDockablePane
{
	DECLARE_DYNAMIC(GraphWnd)

public:

	CBrush mbrush;

	Linegraph mgraph;


public:
	GraphWnd();
	virtual ~GraphWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:

	void reset();

	void setTheme(int ntheme) { mgraph.setTheme(ntheme); };
		
	//void setSize();
	void adjustSize();
	void updateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2);

	afx_msg void OnSaveCSV();
	afx_msg void OnAdjustScale();

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);


};


