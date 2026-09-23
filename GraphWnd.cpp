// GraphWnd.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "GraphWnd.h"
#include "CGraphOptions.h"


// GraphWnd

IMPLEMENT_DYNAMIC(GraphWnd, CDockablePane)

GraphWnd::GraphWnd()
{

	//GetGlobalData()->clrBarFace = RGB(0, 255, 0);
	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);

	mgraph.xlabel = _T("RMS Train Error");
	mgraph.xlabel2 = _T("Training Epochs");

}

GraphWnd::~GraphWnd()
{
}


BEGIN_MESSAGE_MAP(GraphWnd, CDockablePane)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_GRAPH_DATA, &GraphWnd::OnSaveCSV)
	ON_COMMAND(ID_GRAPH_SCALE, &GraphWnd::OnAdjustScale)
END_MESSAGE_MAP()


// GraphWnd message handlers

void GraphWnd::reset()
{
	mgraph.reset();
	CClientDC dc(this);
	mgraph.drawGraph(&dc);
}


void GraphWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDockablePane::OnPaint() for painting messages

	CRect crect;
	dc.GetClipBox(crect);
	dc.FillRect(&crect, &mbrush);

	mgraph.drawGraph(&dc);

}

void GraphWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	CRect rect2;
	GetWindowRect(&rect2);
	CRect rect3;
	rect3.left = 1;
	rect3.top = 1;
	rect3.right = cx - 1;
	rect3.bottom = cy - 1;
	CClientDC dc(this);
	mgraph.setSize(rect3);
	mgraph.drawGraph(&dc);

}

void GraphWnd::adjustSize()
{
	CRect rect1;
	GetWindowRect(&rect1);
}

BOOL GraphWnd::OnEraseBkgnd(CDC* pDC)
{
	return(true);
	return CDockablePane::OnEraseBkgnd(pDC);
}

void GraphWnd::updateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2)
{
	CClientDC dc(this);
	mgraph.updateGraph(sxdata, sydata, sydata2, &dc);
}

void GraphWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_GRAPH, point.x, point.y, this, TRUE);
#endif
}

void GraphWnd::OnSaveCSV()
{
	if (mgraph.ydata.size() > 0) {

		CWnd* pWndMain = AfxGetMainWnd();
		CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

		if (pDoc->getDataPathNamesSize(false)) {

			CString myfilename = pDoc->getDataPathName(0, false) + _T("_TrainGraphData.csv");

			CFileDialog dlg2(FALSE, _T("csv"), myfilename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Comma Separated (*.csv)|*.csv||"));

			if (dlg2.DoModal() == IDOK) {

				CFile file;

				myfilename = dlg2.GetPathName();

				// Open csv file
				if (!file.Open(myfilename, CFile::modeCreate | CFile::modeReadWrite)) {
					CString msg = _T("Unable to open ") + myfilename;
					AfxMessageBox(msg);
					return;
				}

				// Allocate memory used to parse lines of data
				unsigned long max_data = 50000;
				char* cdata = new char[max_data];
				if (cdata == 0) {
					AfxMessageBox(_T("Unable to allocate memory to save csv file"));
					return;
				}


				CString sline = _T("");

				sline = _T("Thousand Epochs,RMS Training Error");
				if (mgraph.ydata2.size() > 0) {
					sline += _T(",RMS Validation Error");
				}
				sline += _T("\n");
				if (sline.GetLength() < int(max_data)) {
					for (int j = 0; j < sline.GetLength(); j++) {
						cdata[j] = (char)sline.GetAt(j);
					}
				}
				file.Write(cdata, sline.GetLength());

				for (int i = 0; i < mgraph.xdata.size(); i++) {

					if ((i < mgraph.ydata.size()) && (i < mgraph.ydata2.size())) {
						sline.Format(_T("%f,%f,%f"), mgraph.xdata[i], mgraph.ydata[i], mgraph.ydata2[i]);
					}
					else if (i < mgraph.ydata.size()) {
						sline.Format(_T("%f,%f"), mgraph.xdata[i], mgraph.ydata[i]);
					}
					else {
						sline.Format(_T("%f"), mgraph.xdata[i]);
					}
					sline += _T("\n");
					if (sline.GetLength() < int(max_data)) {
						for (int j = 0; j < sline.GetLength(); j++) {
							cdata[j] = (char)sline.GetAt(j);
						}
					}
					file.Write(cdata, sline.GetLength());
				}

				file.Flush();
				file.Close();

				delete[] cdata;

			}

		}

	}
	else {
		AfxMessageBox(_T("No data to save!"));
	}

}

void GraphWnd::OnAdjustScale()
{
	CGraphOptions dlg;

	dlg.default_ymax.Format(_T("%.2f"), mgraph.maxy);
	dlg.default_ymin.Format(_T("%.2f"), mgraph.miny);
	dlg.m_ymax.Format(_T("%.2f"), mgraph.maxy);
	dlg.m_ymin.Format(_T("%.2f"), mgraph.miny);

	if (dlg.DoModal() == IDOK) {

		mgraph.use_custom_miny = dlg.yminauto;
		mgraph.use_custom_maxy = dlg.ymaxauto;

		mgraph.custom_miny = (float)_tstof((LPCTSTR)dlg.m_ymin);
		mgraph.custom_maxy = (float)_tstof((LPCTSTR)dlg.m_ymax);

		CClientDC dc(this);
		mgraph.updateGraph(mgraph.xdata, mgraph.ydata, mgraph.ydata2, &dc);
		//mgraph.drawGraph(&dc);


	}

}
