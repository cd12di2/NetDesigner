

#include "pch.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "Graph.h"



Graph::Graph() 
{ 
	theme = LIGHT;
	mfont = 8;
	label_x = label_y = 0;
	xaxistext_x1 = xaxistext_x2 = xaxistext_y = 0;
	yaxistext_x = yaxistext_y1 = yaxistext_y2 = 0;
	axis_x1 = axis_x2 = axis_y1 = axis_y2 = 0;
	xlabel = _T("xlabel");
	ylabel = _T("ylabel");
	minx = 0;
	maxx = 1;
	miny = 0;
	maxy = 10;
	use_custom_miny = false;
	use_custom_maxy = false;
	custom_miny = miny;
	custom_maxy = maxy;
	dx = 0;
	dy = 0;
	zero_x_index = 0;
	zero_y_index = 0;
	zero_x_line = false;
	zero_y_line = false;
	mrect = CRect(10, 10, 20, 20);
	show_background = true;
	show_background = true;
	message_displayed = false;
	setColors();
}

Graph::~Graph() 
{
}

void Graph::setSize(CRect srect)
{ 
	mrect = srect; 
}

void Graph::setColors(bool dark)
{
	//clr_back = 0x00e3f6fd; //0x00d5e8ee;
	//clr_grid = 0x00756e58;
	//clr_text = 0x00362b00;
}

void Graph::drawGraph(CDC* pDC)
{
	if (show_background) {
		drawFrame(pDC);
	}
	drawXAxis(pDC);
	drawData(pDC);
	drawDataN(pDC);
}

void Graph::eraseGraph(CDC* pDC, CBrush *brush1)
{
	pDC->FillRect(mrect, brush1);
	if (show_border) {
		CPen pen1(PS_SOLID, 2, GetGlobalData()->clrBarFace);
		pDC->SelectObject(pen1);
		pDC->MoveTo(mrect.left, mrect.top);
		pDC->LineTo(mrect.right, mrect.top);
		pDC->LineTo(mrect.right, mrect.bottom);
		pDC->LineTo(mrect.left, mrect.bottom);
		pDC->LineTo(mrect.left, mrect.top);
	}
}

void Graph::drawFrame(CDC* pDC)
{
	CBrush brush1;
	//clr_back = adjustColorByPercent(vsl_background2, 0);
	if (theme == LIGHT) {
		//clr_back = vsl_background2;// adjustColorByPercent(clr_solarized_base2, 15);
		clr_back = adjustColorByPercent(net_background3, 80);
		clr_grid = adjustColorByPercent(clr_back, -40);
		clr_grid2 = adjustColorByPercent(clr_back, -20);
	}
	else if (theme == COLOR) {
	}
	else if (theme == BLUE) {
	}
	else if (theme == DARK) {
		//clr_back = vsl_background4;// adjustColorByPercent(clr_solarized_base2, 0);
		clr_back = adjustColorByPercent(net_background3, 40);
		clr_grid = adjustColorByPercent(clr_back, -40);
		clr_grid2 = adjustColorByPercent(clr_back, -20);
	}
	brush1.CreateSolidBrush(clr_back);
	pDC->FillRect(mrect, &brush1);
	if (show_border) {
		CPen pen1(PS_SOLID, 2, clr_grid);
		pDC->SelectObject(pen1);
		pDC->MoveTo(mrect.left, mrect.top);
		pDC->LineTo(mrect.right, mrect.top);
		pDC->LineTo(mrect.right, mrect.bottom);
		pDC->LineTo(mrect.left, mrect.bottom);
		pDC->LineTo(mrect.left, mrect.top);
	}
	DeleteObject(brush1);
}

void Graph::setGridCorners()
{
	label_x = (mrect.right + mrect.left) / 2;
	label_y = mrect.top + 4;
	axis_x1 = mrect.left + 24 + (3 * mfont);
	axis_x2 = mrect.right - 10 - (2 * mfont);
	axis_y1 = mrect.bottom - (2 * mfont) - 8;
	axis_y2 = mrect.top + 20;
	xaxistext_y = axis_y1 + mfont - 2;
	xaxistext_x1 = axis_x1;
	xaxistext_x2 = axis_x2;
	yaxistext_x = axis_x1 - (3 * mfont);// 16;
	yaxistext_y1 = axis_y1;
	yaxistext_y2 = axis_y2 + 5;
	dx = (float)(axis_x2 - axis_x1);
	dy = (float)(axis_y2 - axis_y1);
}

void Graph::drawXAxis(CDC* pDC)
{
	setGridCorners();

	float scale = (float)mrect.Width() / 500;
	if (scale > ((float)mrect.Height() / 300)) {
		scale = (float)mrect.Height() / 300;
	}

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = -MulDiv(mfont, pDC->GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;

	CFont newFont;
	if (!newFont.CreateFontIndirect(&lf))
		return;
	CFont* pOldFont = pDC->SelectObject(&newFont);
	pDC->SetTextColor(clr_text);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_CENTER);

	CString lbl = _T("");

	lbl.Format(_T("%.1f"), minx);
	pDC->TextOut(xaxistext_x1, xaxistext_y, lbl);

	lbl.Format(_T("%.1f"), maxx);
	pDC->TextOut(xaxistext_x2, xaxistext_y, lbl);

	lbl.Format(_T("%.1f"), (minx + maxx) / 2);
	pDC->TextOut((xaxistext_x1 + xaxistext_x2) / 2, xaxistext_y, lbl);

	pDC->TextOut(yaxistext_x, yaxistext_y1 - mfont, convertNumber(miny));

	pDC->TextOut(yaxistext_x, yaxistext_y2 - mfont, convertNumber(maxy));

	if (xlabel != _T("")) {
		pDC->TextOut(label_x, label_y, xlabel);
	}

	CPen pen1(PS_SOLID, 2, clr_grid);
	pDC->SelectObject(pen1);
	pDC->MoveTo(axis_x1, axis_y1);
	pDC->LineTo(axis_x2, axis_y1);

	pDC->MoveTo(xaxistext_x1, axis_y1);
	pDC->LineTo(xaxistext_x1, axis_y1+4);
	pDC->MoveTo(xaxistext_x2, axis_y1);
	pDC->LineTo(xaxistext_x2, axis_y1+4);
	pDC->MoveTo((xaxistext_x1 + xaxistext_x2)/2, axis_y1);
	pDC->LineTo((xaxistext_x1 + xaxistext_x2)/2, axis_y1+4);

	pDC->MoveTo(axis_x1, axis_y1);
	pDC->LineTo(axis_x1, axis_y2);

	pDC->MoveTo(axis_x1, yaxistext_y1);
	pDC->LineTo(axis_x1 - 4, yaxistext_y1);
	pDC->MoveTo(axis_x1, yaxistext_y2);
	pDC->LineTo(axis_x1 - 4, yaxistext_y2);

	pDC->SelectObject(pOldFont);
	DeleteObject(newFont);


}

void Graph::drawData(CDC* pDC)
{

}

void Graph::drawDataN(CDC* pDC)
{

}

CString Graph::convertNumber(float num)
{
	int cnt = 0;
	float tnum = num;
	while (tnum >= 1000) {
		tnum = tnum / 1000;
		cnt++;
	}
	float anum = num;
	if (anum < 0) {
		anum = -1 * anum;
	}
	CString rval = _T("");
	if (cnt == 1) {
		rval.Format(_T("%.1fk"), tnum);
	}
	else if (cnt == 2) {
		rval.Format(_T("%.1fM"), tnum);
	}
	else if (cnt == 3) {
		rval.Format(_T("%.1fB"), tnum);
	}
	else if (cnt == 4) {
		rval.Format(_T("%.1fT"), tnum);
	}
	else if (cnt == 5) {
		rval.Format(_T("%.1fQ"), tnum);
	}
	else if (cnt == 6) {
		rval.Format(_T("%.1fMT"), tnum);
	}
	else if (anum < 0.1F) {
		rval.Format(_T("%.3f"), tnum);
	}
	else if (anum < 1.0F) {
		rval.Format(_T("%.2f"), tnum);
	}
	else {
		rval.Format(_T("%.1f"), tnum);
	}
	return(rval);
}

void Graph::addErrorString(CString msg)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(msg);
}

COLORREF Graph::adjustColorByPercent(COLORREF clr, float percent)
{
	COLORREF rval = 0x00000000;

	int r2 = (int)((1 + percent / 100) * (float)GetRValue(clr));
	if (r2 > 255) {
		r2 = 255;
	}
	else if (r2 < 0) {
		r2 = 0;
	}

	int g2 = (int)((1 + percent / 100) * (float)GetGValue(clr));
	if (g2 > 255) {
		g2 = 255;
	}
	else if (g2 < 0) {
		g2 = 0;
	}

	int b2 = (int)((1 + percent / 100) * (float)GetBValue(clr));
	if (b2 > 255) {
		b2 = 255;
	}
	else if (b2 < 0) {
		b2 = 0;
	}

	rval = RGB(r2, g2, b2);

	return(rval);
}



//**************************************************************************************
//**************************************************************************************

void Histogram::setGridCorners()
{
	Graph::setGridCorners();
	barwidth = ((axis_x2 - axis_x1) / numbars) -1;
	if (barwidth <= 0) {
		barwidth = 1;
	}
	xaxistext_x1 += (barwidth / 2);
	xaxistext_x2 -= (barwidth / 2);
}

void Histogram::setSize(CRect srect)
{
	mrect = srect;
	numbars = mrect.Width() / 15; 
	if (numbars < 21) {
		numbars = 21;
	}
}

void Histogram::setData(vector<double> sdata) 
{ 
	data = sdata; 
	maxy = minx = maxx = 0;
	if (data.size() > 0) {
		int* mdata = new int[(int)(numbars + 2)];
		for (int i = 0; i < numbars; i++) {
			mdata[i] = 0;
		}
		minx = maxx = (float)data[0];
		for (int i = 1; i < data.size(); i++) {
			if (data[i] < minx) {
				minx = (float)data[i];
			}
			if (data[i] > maxx) {
				maxx = (float)data[i];
			}
		}
		if (maxx > minx) {
			for (int i = 0; i < data.size(); i++) {
				int index = (int)((float)numbars * ((float)data[i] - minx) / (maxx - minx));
				if (index >= numbars) {
					index = numbars - 1;
				}
				mdata[index]++;
				float tdata = (float)mdata[index];
				if (tdata > maxy) {
					maxy = tdata;
				}
			}
		}
		delete[] mdata;
	}
}

void Histogram::drawData(CDC* pDC)
{
	if (maxx > minx) {
		CBrush brush1;
		brush1.CreateSolidBrush(clr_data1);
		int* mdata = new int[(int)(numbars + 2)];
		for (int i = 0; i < numbars; i++) {
			mdata[i] = 0;
		}
		for (int i = 0; i < data.size(); i++) {
			int index = (int)((float)numbars * ((float)data[i] - minx) / (maxx - minx));
			if (index >= numbars) {
				index = numbars - 1;
			}
			mdata[index]++;
		}
		for (int i = 0; i < numbars; i++) {
			float tdata = (float)mdata[i];
			int x = xaxistext_x1 + ((i * (xaxistext_x2 - xaxistext_x1)) / (numbars - 1));
			int y = axis_y1 - (int)((tdata * (float)(axis_y1 - axis_y2)) / maxy);
			CRect rect;
			rect.left =  x - barwidth / 2;
			rect.top = y;
			rect.right = x + barwidth / 2;
			rect.bottom = axis_y1 - 1;
			pDC->FillRect(rect, &brush1);
		}
		delete[] mdata;
		DeleteObject(brush1);

	}
}


//**************************************************************************************
//**************************************************************************************

Linegraph::Linegraph()
{
	mfont = 8;
	label_x = label_y = 0;
	xaxistext_x1 = xaxistext_x2 = xaxistext_y = 0;
	yaxistext_x = yaxistext_y1 = yaxistext_y2 = 0;
	axis_x1 = axis_x2 = axis_y1 = axis_y2 = 0;
	xlabel = _T("xlabel");
	xlabel2 = _T("xlabel2");
	ylabel = _T("ylabel");
	mrect = CRect(10, 10, 20, 20);
	reset();
	setColors();

	// https://www.simplifiedsciencepublishing.com/resources/best-color-palettes-for-scientific-figures-and-data-visualizations
	clr_data[0] = 0x007d3a00;	// 003a7d dark blue
	clr_data[1] = 0x003430d8;	// d83034 red
	clr_data[2] = 0x00ff8d00;	// 008dff med blue
	clr_data[3] = 0x00b673ff;	// ff73b6 pink
	clr_data[4] = 0x008dcb4e;	// 4ecb8d green
	clr_data[5] = 0x00ff01c7;	// c701ff purple
	clr_data[6] = 0x003a9dff;	// ff9d3a orange
	clr_data[7] = 0x00238eba;	// f9e858 yellow
	clr_data[8] = 0x00888888;	// c8c8c8 gray
	clr_data[9] = 0x0071c5f0;	// f0c571 gold
	clr_data[10] = 0x009ca859;	// 59a89c teal
	clr_data[11] = 0x005957e2;	// e25759 red
	clr_data[12] = 0x00a2810b;	// 0b81a2 blue
	clr_data[13] = 0x00002c9d;	// 9d2c00 dark red
	clr_data[14] = 0x0000b736;	// 36b700 green
	clr_data[15] = 0x0094477e;	// 7e4794 purple

}

Linegraph::~Linegraph()
{
}

void Linegraph::reset()
{
	div = 1;
	minx = 0;
	miny = 0;
	maxx = 0.5F;
	maxy = 0.5F;
	use_custom_miny = false;
	use_custom_maxy = false;
	message_displayed = false;
	custom_miny = miny;
	custom_maxy = maxy;
	xdata.clear();
	ydata.clear();
	ydata2.clear();
	ydataN.clear();
}

void Linegraph::setGridCorners()
{
	Graph::setGridCorners();
	axis_y2 += 5;
	yaxistext_x -= 2;
	yaxistext_y2 = axis_y2;
}

void Linegraph::updateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2, CDC* pDC)
{
	float mymaxx = maxx;
	float mymaxy = maxy;
	setData(sxdata, sydata, sydata2);
	if ((mymaxx != maxx) || (mymaxy != maxy)) {
		if (show_background) {
			drawFrame(pDC);
		}
		drawXAxis(pDC);
	}
	drawData(pDC);
}

void Linegraph::updateGraphN(vector<double> *sxdata, vector<vector<double>> *sydata, CDC* pDC)
{
	float mymaxx = maxx;
	float mymaxy = maxy;
	setData(sxdata, sydata);
	if ((mymaxx != maxx) || (mymaxy != maxy)) {
		if (show_background) {
			drawFrame(pDC);
		}
		drawXAxis(pDC);
	}
	drawDataN(/*sxdata, sydata, */pDC);
}

void Linegraph::clearAndUpdateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2, CDC* pDC)
{
	setData(sxdata, sydata, sydata2);
	maxx = (float)sxdata.size() - 1;
	maxy = 1.0F;
	//reset();
	if (show_background) {
		drawFrame(pDC);
	}
	drawXAxis(pDC);
	drawData(pDC);
}

void Linegraph::clearAndUpdateGraphN(vector<double>* sxdata, vector<vector<double>>* sydata, CDC* pDC)
{
	setData(sxdata, sydata);
	maxx = (float)sxdata->size() - 1;
	maxy = 1.0F;
	if (show_background) {
		drawFrame(pDC);
	}
	drawXAxis(pDC);
	drawDataN(/*sxdata, sydata, */pDC);
}

void Linegraph::setData(vector<double> sxdata, vector<double> sydata, vector<double> sydata2)
{
	xdata = sxdata;
	ydata = sydata;
	ydata2 = sydata2;
	miny = minx = 0;
	if ((xdata.size() == 0) || (ydata.size() == 0) || (xdata.size() != ydata.size())) {
		CString s = _T("");
		s.Format(_T("Linegraph::setData::setData : problem with data : %i : %i"), (int)xdata.size(), (int)ydata.size());
		if (!message_displayed) {
			message_displayed = true;
		}
		return;
	}
	float tmaxx = (float)xdata[0];
	for (int i = 1; i < xdata.size(); i++) {
		if (xdata[i] > tmaxx) {
			tmaxx = (float)xdata[i];
		}
	}
	float tmaxy = (float)ydata[0];
	for (int i = 1; i < ydata.size(); i++) {
		if (ydata[i] > tmaxy) {
			tmaxy = (float)ydata[i];
		}
		if (ydata[i] < -tmaxy) {
			tmaxy = -1 * (float)ydata[i];
		}
	}
	for (int i = 1; i < ydata2.size(); i++) {
		if (ydata2[i] > tmaxy) {
			tmaxy = (float)ydata2[i];
		}
		if (ydata2[i] < -tmaxy) {
			tmaxy = -1 * (float)ydata2[i];
		}
	}
	maxy = 0.5;
	if (tmaxy > maxy) {
		int ind = (int)(tmaxy / 0.5);
		maxy = (float)(ind + 1) * 0.5F;
	}
	if (tmaxx > maxx) {
		div = 2 * div;
		maxx = div;
	}
	if (use_custom_miny) {
		miny = custom_miny;
	}
	else {
		custom_miny = miny;
	}
	if (use_custom_maxy) {
		maxy = custom_maxy;
	}
	else {
		custom_maxy = maxy;
	}
	if (xdata.size() > 2000) {
		if ((ydata.size() == ydata2.size()) || (ydata2.size() == 0)) {
			int skip = (int)xdata.size() / 1000;
			if (skip > 1) {
				if (ydata2.size() > 0) {
					vector<double> txdata = xdata;
					vector<double> tydata = ydata;
					vector<double> tydata2 = ydata2;
					xdata.clear();
					ydata.clear();
					ydata2.clear();
					for (int i = 0; i < txdata.size(); i += skip) {
						xdata.push_back(txdata[i]);
						ydata.push_back(tydata[i]);
						ydata2.push_back(tydata2[i]);
					}
				}
				else {
					vector<double> txdata = xdata;
					vector<double> tydata = ydata;
					xdata.clear();
					ydata.clear();
					for (int i = 0; i < txdata.size(); i += skip) {
						xdata.push_back(txdata[i]);
						ydata.push_back(tydata[i]);
					}
				}
			}
		}
	}
}

void Linegraph::setData(vector<double> *sxdata, vector<vector<double>> *sydata)
{
	xdata = *sxdata;
	ydataN = *sydata;
	miny = minx = 0;
	if ((xdata.size() == 0) || (ydataN.size() == 0)) {
		CString s = _T("");
		s.Format(_T("Linegraph::setData::setData : problem with data : %i : %i"), (int)sxdata->size(), (int)sydata->size());
		if (!message_displayed) {
			message_displayed = true;
		}
		return;
	}
	float tmaxx = (float)xdata[0];
	for (int i = 1; i < xdata.size(); i++) {
		if (xdata[i] > tmaxx) {
			tmaxx = (float)xdata[i];
		}
	}
	float tmaxy = (float)ydataN[0][0];
	for (int i = 1; i < ydataN[0].size(); i++) {
		if (ydataN[0][i] > tmaxy) {
			tmaxy = (float)ydataN[0][i];
		}
		if (ydataN[0][i] < -tmaxy) {
			tmaxy = -1 * (float)ydataN[0][i];
		}
	}
	maxy = 0.5;
	if (tmaxy > maxy) {
		int ind = (int)(tmaxy / 0.5);
		maxy = (float)(ind + 1) * 0.5F;
	}
	if (tmaxx > maxx) {
		div = 2 * div;
		maxx = div;
	}
}

void Linegraph::drawFrameNoBackgound(CDC* pDC, COLORREF back)
{
	CBrush brush1;
	brush1.CreateSolidBrush(back);
	pDC->FillRect(mrect, &brush1);
	/*if (show_border) {
		CPen pen1(PS_SOLID, 2, clr_grid);
		pDC->SelectObject(pen1);
		pDC->MoveTo(mrect.left, mrect.top);
		pDC->LineTo(mrect.right, mrect.top);
		pDC->LineTo(mrect.right, mrect.bottom);
		pDC->LineTo(mrect.left, mrect.bottom);
		pDC->LineTo(mrect.left, mrect.top);
	}*/
	DeleteObject(brush1);
}

void Linegraph::drawXAxis(CDC* pDC)
{
	setGridCorners();

	float scale = (float)mrect.Width() / 500;
	if (scale > ((float)mrect.Height() / 300)) {
		scale = (float)mrect.Height() / 300;
	}

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = -MulDiv(mfont, pDC->GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;

	CFont newFont;
	if (!newFont.CreateFontIndirect(&lf))
		return;
	CFont* pOldFont = pDC->SelectObject(&newFont);
	pDC->SetTextColor(clr_text);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_CENTER);

	CString lbl = _T("");

	lbl.Format(_T("%.0f"), minx);
	pDC->TextOut(xaxistext_x1, xaxistext_y, lbl);

	lbl = convertNumber(maxx);
	pDC->TextOut(xaxistext_x2, xaxistext_y, lbl);


	lbl.Format(_T("%.1f"), miny);
	pDC->TextOut(yaxistext_x, yaxistext_y1 - mfont, lbl);

	lbl = convertNumber(maxy);
	pDC->TextOut(yaxistext_x, yaxistext_y2 - mfont, lbl);

	if (zero_y_line) {
		lbl = _T("0.0");
		pDC->TextOut(yaxistext_x, axis_y1 - (1 * mfont) - ((zero_y_index) * (axis_y2 - axis_y1)) / (numygrid + 1), lbl);
	}
	if (xlabel != _T("")) {
		pDC->TextOut(label_x, label_y, xlabel);
	}
	if (xlabel2 != _T("")) {
		pDC->TextOut((xaxistext_x1 + xaxistext_x2) / 2, xaxistext_y - 2, xlabel2);
	}

	CPen pen1(PS_SOLID, 2, clr_grid2);
	pDC->SelectObject(pen1);
	for (int i = 0; i < numxgrid; i++) {
		int x = axis_x1 + ((i + 1) * (axis_x2 - axis_x1)) / (numxgrid + 1);
		pDC->MoveTo(x, axis_y1);
		pDC->LineTo(x, axis_y2);
	}
	for (int i = 0; i < numygrid; i++) {
		int y = axis_y1 + ((i + 1) * (axis_y2 - axis_y1)) / (numygrid + 1);
		pDC->MoveTo(axis_x1, y);
		pDC->LineTo(axis_x2, y);
	}

	CPen pen2(PS_SOLID, 2, clr_grid);
	pDC->SelectObject(pen2);
	pDC->MoveTo(axis_x1, axis_y1);
	pDC->LineTo(axis_x2, axis_y1);
	pDC->LineTo(axis_x2, axis_y2);
	pDC->LineTo(axis_x1, axis_y2);
	pDC->LineTo(axis_x1, axis_y1);


	pDC->MoveTo(xaxistext_x1, axis_y1);
	pDC->LineTo(xaxistext_x1, axis_y1 + 4);
	pDC->MoveTo(xaxistext_x2, axis_y1);
	pDC->LineTo(xaxistext_x2, axis_y1 + 4);

	pDC->MoveTo(axis_x1, yaxistext_y1);
	pDC->LineTo(axis_x1 - 4, yaxistext_y1);
	pDC->MoveTo(axis_x1, yaxistext_y2);
	pDC->LineTo(axis_x1 - 4, yaxistext_y2);

	if (zero_x_line) {
		int x = axis_x1 - ((zero_x_index) * (axis_x2 - axis_x1)) / (numxgrid + 1);
		pDC->MoveTo(x, axis_y1);
		pDC->LineTo(x, axis_y2);
	}
	if (zero_y_line) {
		int y = axis_y1 - ((zero_y_index) * (axis_y2 - axis_y1)) / (numygrid + 1);
		pDC->MoveTo(axis_x1, y);
		pDC->LineTo(axis_x2, y);
	}

	pDC->SelectObject(pOldFont);
	DeleteObject(newFont);

}

void Linegraph::drawXAxisNoLabels(CDC* pDC)
{
	label_x = 0;
	label_y = 0;
	axis_x1 = mrect.left + 1;
	axis_x2 = mrect.right - 1;
	axis_y1 = mrect.bottom - 1;
	axis_y2 = mrect.top + 1;
	xaxistext_y = 0;
	xaxistext_x1 = 0;
	xaxistext_x2 = 0;
	yaxistext_x = 0;
	yaxistext_y1 = 0;
	yaxistext_y2 = 0;
	dx = (float)(axis_x2 - axis_x1);
	dy = (float)(axis_y2 - axis_y1);
}

void Linegraph::drawData(CDC* pDC)
{
	if (((maxx - minx) > 0) && ((maxy - miny) > 0)) {
		if ((xdata.size() > 0) && (ydata.size() > 0) && (xdata.size() == ydata.size())) {
			//int inc = 1 + (int)(xdata.size() / (float)xdata.size());
			CPen pen(PS_SOLID, 2, clr_data1);
			pDC->SelectObject(pen);
			double x = (double)axis_x1 + ((xdata[0] - minx) * dx) / (maxx - minx);
			if (x > (double)axis_x2) {
				x = (double)axis_x2;
			}
			double y = (double)axis_y1 + ((ydata[0] - miny) * dy) / (1.02 * (maxy - miny));
			if (y > (double)axis_y1) {
				y = (double)axis_y1;
			}
			else if (y < (double)axis_y2) {
				y = (double)axis_y2;
			}
			pDC->MoveTo((int)x, (int)y);
			for (int i = 1; i < xdata.size(); i++) {
				x = (double)axis_x1 + ((xdata[i] - minx) * dx) / (maxx - minx);
				if (x > (double)axis_x2) {
					x = (double)axis_x2;
				}
				y = (double)axis_y1 + ((ydata[i] - miny) * dy) / (1.02 * (maxy - miny));
				if (y > (double)axis_y1) {
					y = (double)axis_y1;
				}
				else if (y < (double)axis_y2) {
					y = (double)axis_y2;
				}
				pDC->LineTo((int)x, (int)y);
			}
			if ((ydata2.size() > 0) && (xdata.size() == ydata2.size())) {
				CPen pen2(PS_SOLID, 2, clr_data2);
				pDC->SelectObject(pen2);
				x = (double)axis_x1 + ((xdata[0] - minx) * dx) / (maxx - minx);
				if (x > (double)axis_x2) {
					x = (double)axis_x2;
				}
				y = (double)axis_y1 + ((ydata2[0] - miny) * dy) / (1.02 * (maxy - miny));
				if (y > (double)axis_y1) {
					y = (double)axis_y1;
				}
				else if (y < (double)axis_y2) {
					y = (double)axis_y2;
				}
				pDC->MoveTo((int)x, (int)y);
				for (int i = 1; i < xdata.size(); i++) {
					x = (double)axis_x1 + ((xdata[i] - minx) * dx) / (maxx - minx);
					if (x > (double)axis_x2) {
						x = (double)axis_x2;
					}
					y = (double)axis_y1 + ((ydata2[i] - miny) * dy) / (1.02 * (maxy - miny));
					if (y > (double)axis_y1) {
						y = (double)axis_y1;
					}
					else if (y < (double)axis_y2) {
						y = (double)axis_y2;
					}
					pDC->LineTo((int)x, (int)y);
				}
			}
		}
	}
}

void Linegraph::drawDataN(/*vector<double>* mxdata, vector<vector<double>>* mydata, */CDC* pDC)
{
	if (((maxx - minx) > 0) && ((maxy - miny) > 0)) {
		if ((xdata.size() > 0) && (ydataN.size() > 0)) {
			int cindex = 0;
			int skip = 1;
			float xsize = 2 * (float)(axis_x2 - axis_x1);
			if (xsize > 1) {
				skip = (int)((float)xdata.size() / xsize);
				if (skip < 1) {
					skip = 1;
				}
			}
			//CString s = _T(""); s.Format(_T("xsize = %.1f : xdata.size() = %i : skip = %i"), xsize, (int)xdata.size(), skip); addErrorString(s);
			bool first = true;
			vector<double> txdata;
			vector<vector<double>> tydata;
			for (int m = 0; m < (int)ydataN.size(); m++) {
				if ((ydataN[m].size() > 0) && (xdata.size() == ydataN[m].size())) {
					if (first) {
						double x = (double)axis_x1 + ((xdata[0] - minx) * dx) / (maxx - minx);
						if (x > (double)axis_x2) {
							x = (double)axis_x2;
						}
						txdata.push_back(x);
					}
					vector<double> tempy;
					tydata.push_back(tempy);
					int cnt = 0;
					double sum = 0.0;
					for (int n = 0; n < skip; n++) {
						if (n < (int)xdata.size()) {
							sum += ydataN[m][n];
							cnt++;
						}
					}
					double ty = ydataN[m][0];
					if (cnt > 0) {
						ty = sum / (double)cnt;
					}
					double y = (double)axis_y1 + ((ty - miny) * dy) / (1.02 * (maxy - miny));
					if (y > (double)axis_y1) {
						y = (double)axis_y1;
					}
					else if (y < (double)axis_y2) {
						y = (double)axis_y2;
					}
					tydata[(int)tydata.size() - 1].push_back(y);
					for (int i = 1; i < xdata.size(); i += skip) {
						if (first) {
							double x = (double)axis_x1 + ((xdata[i] - minx) * dx) / (maxx - minx);
							if (x > (double)axis_x2) {
								x = (double)axis_x2;
							}
							txdata.push_back(x);
						}
						cnt = 0;
						sum = 0.0;
						for (int n = 0; n < skip; n++) {
							if ((i + n) < (int)xdata.size()) {
								sum += ydataN[m][i + n];
								cnt++;
							}
						}
						ty = ydataN[m][i];
						if (cnt > 0) {
							ty = sum / (double)cnt;
						}
						y = (double)axis_y1 + ((ty - miny) * dy) / (1.02 * (maxy - miny));
						if (y > (double)axis_y1) {
							y = (double)axis_y1;
						}
						else if (y < (double)axis_y2) {
							y = (double)axis_y2;
						}
						tydata[(int)tydata.size() - 1].push_back(y);
					}
					first = false;
				}
			}
			for (int m = 0; m < (int)tydata.size(); m++) {
				if ((tydata[m].size() > 0) && (txdata.size() == tydata[m].size())) {
					CPen pen(PS_SOLID, 2, clr_data[cindex]);
					cindex++;  if (cindex >= 16) { cindex = 0; }
					pDC->SelectObject(pen);
					pDC->MoveTo((int)txdata[0], (int)tydata[m][0]);
					for (int i = 1; i < (int)txdata.size(); i++) {
						pDC->LineTo((int)txdata[i], (int)tydata[m][i]);
					}
				}
			}
		}
	}
}



//**************************************************************************************
//**************************************************************************************

Linegraph2::Linegraph2()
{
	mfont = 8;
	label_x = label_y = 0;
	xaxistext_x1 = xaxistext_x2 = xaxistext_y = 0;
	yaxistext_x = yaxistext_y1 = yaxistext_y2 = 0;
	axis_x1 = axis_x2 = axis_y1 = axis_y2 = 0;
	xlabel = _T("xlabel");
	xlabel2 = _T("xlabel2");
	ylabel = _T("ylabel");
	mrect = CRect(10, 10, 20, 20);
	reset();
	setColors();
}

Linegraph2::~Linegraph2()
{
}

void Linegraph2::reset()
{
	div = 0.1F;
	minx = 0;
	miny = 0;
	maxx = 0.5F;
	maxy = 0.5F;
	message_displayed = false;
	xdata.clear();
	ydata.clear();
	ydata2.clear();
}

void Linegraph2::setData(vector<double> sxdata, vector<double> sydata, vector<double> sydata2)
{
	xdata = sxdata;
	ydata = sydata;
	ydata2 = sydata2;
	miny = minx = 0;
	maxx = 0.5F;
	maxy = 0.5F;
	if ((xdata.size() == 0) || (ydata.size() == 0) || (xdata.size() != ydata.size())) {
		if (!message_displayed) {
			message_displayed = true;
		}
		return;
	}
	float tminx = 0;
	float tmaxx = 0.5F;
	for (int i = 0; i < xdata.size(); i++) {
		if (xdata[i] > tmaxx) {
			tmaxx = (float)xdata[i];
		}
		if (xdata[i] < tminx) {
			tminx = (float)xdata[i];
		}
	}
	float tminy = 0;
	float tmaxy = 0.5;
	for (int i = 0; i < ydata.size(); i++) {
		if (ydata[i] > tmaxy) {
			tmaxy = (float)ydata[i];
		}
		if (ydata2[i] > tmaxy) {
			tmaxy = (float)ydata2[i];
		}
		if (ydata[i] < tminy) {
			tminy = (float)ydata[i];
		}
		if (ydata2[i] < tminy) {
			tminy = (float)ydata2[i];
		}
	}

	if (tmaxy < minmaxy) {
		tmaxy = minmaxy;
	}
	if (tminy > maxminy) {
		tminy = maxminy;
	}

	float tmax = tmaxy;
	if (tmax < -tminy) {
		tmax = -tminy;
	}
	int numinc = 4;
	divy = tmax / (float)numinc;
	if (divy < 1) {
		float scale = 1;
		float tdiv = divy;
		while ((tdiv < 1) && (scale > 0.0000001)) {
			tdiv = tdiv * 10;
			scale = scale / 10;
		}
		int idivy = (int)tdiv;
		divy = (float)idivy * scale;
	}
	else if (divy > 10) {
		float scale = 1;
		float tdiv = divy;
		while ((tdiv > 10) && (scale < 1000000)) {
			tdiv = tdiv / 10;
			scale = scale * 10;
		}
		int idivy = (int)tdiv;
		divy = (float)idivy * scale;
	}
	else {
		int idivy = (int)divy;
		divy = (float)idivy;
	}
	if (tminy < 0) {
		miny = divy * (float)((int)(tminy / divy));
		if (tminy < miny) {
			miny = miny - divy;
		}
	}
	else {
		miny = 0;
	}
	if (tmaxy > 0) {
		maxy = divy * (float)((int)(tmaxy / divy));
		if (tmaxy > maxy) {
			maxy = maxy + divy;
		}
	}
	else {
		maxy = 0;
	}
	numygrid = (int)((maxy - miny) / divy) - 1;
	if ((miny != 0) && (maxy != 0) && (divy != 0)) {
		zero_y_index = (int)(miny / divy);
		zero_y_line = true;
	}
	else {
		zero_y_index = 0;
		zero_y_line = false;
	}


	tmax = tmaxx;
	if (tmax < -tminx) {
		tmax = -tminx;
	}
	numinc = 4;
	divx = tmax / (float)numinc;
	if (divx < 1) {
		float scale = 1;
		float tdiv = divx;
		while ((tdiv < 1) && (scale > 0.0000001)) {
			tdiv = tdiv * 10;
			scale = scale / 10;
		}
		int idivx = (int)tdiv;
		divx = (float)idivx * scale;
	}
	else if (divx > 10) {
		float scale = 1;
		float tdiv = divx;
		while ((tdiv > 10) && (scale < 1000000)) {
			tdiv = tdiv / 10;
			scale = scale * 10;
		}
		int idivx = (int)tdiv;
		divx = (float)idivx * scale;
	}
	else {
		int idivx = (int)divx;
		divx = (float)idivx;
	}
	if (tminx < 0) {
		minx = divx * (float)((int)(tminx / divx));
		if (tminx < minx) {
			minx = minx - divx;
		}
	}
	else {
		minx = 0;
	}
	if (tmaxx > 0) {
		maxx = divx * (float)((int)(tmaxx / divx));
		if (tmaxx > maxx) {
			maxx = maxx + divx;
		}
	}
	else {
		maxx = 0;
	}
	numxgrid = (int)((maxx - minx) / divx) - 1;
	if ((minx != 0) && (maxx != 0) && (divx != 0)) {
		zero_x_index = (int)(minx / divx);
		zero_x_line = true;
	}
	else {
		zero_x_index = 0;
		zero_x_line = false;
	}

}

void Linegraph2::drawData2(CDC* pDC)
{
	if (((maxx - minx) > 0) && ((maxy - miny) > 0)) {
		if ((xdata.size() > 0) && (ydata2.size() > 0) && (xdata.size() == ydata2.size())) {
			int inc = 1 + (int)(xdata.size() / (float)xdata.size());
			CPen pen(PS_SOLID, 2, clr_data2);
			pDC->SelectObject(pen);
			double x = (double)axis_x1 + ((xdata[0] - minx) * dx) / (maxx - minx);
			if (x > (double)axis_x2) {
				x = (double)axis_x2;
			}
			double y = (double)axis_y1 + ((ydata2[0] - miny) * dy) / (1.02 * (maxy - miny));
			if (y > (double)axis_y1) {
				y = (double)axis_y1;
			}
			else if (y < (double)axis_y2) {
				y = (double)axis_y2;
			}
			pDC->MoveTo((int)x, (int)y);
			for (int i = 1; i < xdata.size(); i++) {
				x = (double)axis_x1 + ((xdata[i] - minx) * dx) / (maxx - minx);
				if (x > (double)axis_x2) {
					x = (double)axis_x2;
				}
				y = (double)axis_y1 + ((ydata2[i] - miny) * dy) / (1.02 * (maxy - miny));
				if (y > (double)axis_y1) {
					y = (double)axis_y1;
				}
				else if (y < (double)axis_y2) {
					y = (double)axis_y2;
				}
				pDC->LineTo((int)x, (int)y);
			}
		}
	}
}

void Linegraph2::updateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2, CDC* pDC)
{
	float mymaxx = maxx;
	setData(sxdata, sydata, sydata2);
	if (show_background) {
		drawFrame(pDC);
	}
	drawXAxis(pDC);
	drawData(pDC);
	drawData2(pDC);
}



//**************************************************************************************
//**************************************************************************************

LinegraphMulti::LinegraphMulti()
{
	mfont = 8;
	label_x = label_y = 0;
	xaxistext_x1 = xaxistext_x2 = xaxistext_y = 0;
	yaxistext_x = yaxistext_y1 = yaxistext_y2 = 0;
	axis_x1 = axis_x2 = axis_y1 = axis_y2 = 0;
	xlabel = _T("xlabel");
	xlabel2 = _T("xlabel2");
	ylabel = _T("ylabel");
	mrect = CRect(10, 10, 20, 20);
	colors.push_back(COLORREF(0x000000b3));
	colors.push_back(COLORREF(0x0058117c));
	colors.push_back(COLORREF(0x00af2144));
	colors.push_back(COLORREF(0x00ff531a));
	colors.push_back(COLORREF(0x00e6880d));
	colors.push_back(COLORREF(0x00c7b700));
	colors.push_back(COLORREF(0x005ad45a));
	colors.push_back(COLORREF(0x004ee08b));
	reset();
	setColors();
}

LinegraphMulti::~LinegraphMulti()
{
}

void LinegraphMulti::reset()
{
	div = 0.1F;
	minx = 0;
	miny = 0;
	maxx = 0.5F;
	maxy = 0.5F;
	message_displayed = false;
	xdata.clear();
	ydata.clear();
	ydata2.clear();
	multiydata.clear();
}

void LinegraphMulti::setData(vector<double> sxdata, vector<vector<double>> sydata)
{
	xdata = sxdata;
	multiydata = sydata;
	miny = minx = 0;
	maxx = 0.5F;
	maxy = 0.5F;
	if ((xdata.size() == 0) || (multiydata.size() == 0)) {
		return;
	}
	/*else {
		for (int i = 0; i < multiydata.size(); i++) {
			if (xdata.size() != multiydata[i].size()) {
				return;
			}
		}
	}*/
	float tminx = 0;
	float tmaxx = 0.5F;
	for (int i = 0; i < xdata.size(); i++) {
		if (xdata[i] > tmaxx) {
			tmaxx = (float)xdata[i];
		}
		if (xdata[i] < tminx) {
			tminx = (float)xdata[i];
		}
	}
	float tminy = 0;
	float tmaxy = 0.5;
	for (int i = 0; i < multiydata.size(); i++) {
		for (int j = 0; j < multiydata[i].size(); j++) {
			if (multiydata[i][j] > tmaxy) {
				tmaxy = (float)multiydata[i][j];
			}
			if (multiydata[i][j] < tminy) {
				tminy = (float)multiydata[i][j];
			}
		}
	}

	if (tmaxy < minmaxy) {
		tmaxy = minmaxy;
	}
	if (tminy > maxminy) {
		tminy = maxminy;
	}

	float tmax = tmaxy;
	if (tmax < -tminy) {
		tmax = -tminy;
	}
	int numinc = 4;
	divy = tmax / (float)numinc;
	if (divy < 1) {
		float scale = 1;
		float tdiv = divy;
		while ((tdiv < 1) && (scale > 0.0000001)) {
			tdiv = tdiv * 10;
			scale = scale / 10;
		}
		int idivy = (int)tdiv;
		divy = (float)idivy * scale;
	}
	else if (divy > 10) {
		float scale = 1;
		float tdiv = divy;
		while ((tdiv > 10) && (scale < 1000000)) {
			tdiv = tdiv / 10;
			scale = scale * 10;
		}
		int idivy = (int)tdiv;
		divy = (float)idivy * scale;
	}
	else {
		int idivy = (int)divy;
		divy = (float)idivy;
	}
	if (tminy < 0) {
		miny = divy * (float)((int)(tminy / divy));
		if (tminy < miny) {
			miny = miny - divy;
		}
	}
	else {
		miny = 0;
	}
	if (tmaxy > 0) {
		maxy = divy * (float)((int)(tmaxy / divy));
		if (tmaxy > maxy) {
			maxy = maxy + divy;
		}
	}
	else {
		maxy = 0;
	}
	numygrid = (int)((maxy - miny) / divy) - 1;
	if ((miny != 0) && (maxy != 0) && (divy != 0)) {
		zero_y_index = (int)(miny / divy);
		zero_y_line = true;
	}
	else {
		zero_y_index = 0;
		zero_y_line = false;
	}


	tmax = tmaxx;
	if (tmax < -tminx) {
		tmax = -tminx;
	}
	numinc = 4;
	divx = tmax / (float)numinc;
	if (divx < 1) {
		float scale = 1;
		float tdiv = divx;
		while ((tdiv < 1) && (scale > 0.0000001)) {
			tdiv = tdiv * 10;
			scale = scale / 10;
		}
		int idivx = (int)tdiv;
		divx = (float)idivx * scale;
	}
	else if (divx > 10) {
		float scale = 1;
		float tdiv = divx;
		while ((tdiv > 10) && (scale < 1000000)) {
			tdiv = tdiv / 10;
			scale = scale * 10;
		}
		int idivx = (int)tdiv;
		divx = (float)idivx * scale;
	}
	else {
		int idivx = (int)divx;
		divx = (float)idivx;
	}
	if (tminx < 0) {
		minx = divx * (float)((int)(tminx / divx));
		if (tminx < minx) {
			minx = minx - divx;
		}
	}
	else {
		minx = 0;
	}
	if (tmaxx > 0) {
		maxx = divx * (float)((int)(tmaxx / divx));
		if (tmaxx > maxx) {
			maxx = maxx + divx;
		}
	}
	else {
		maxx = 0;
	}
	numxgrid = (int)((maxx - minx) / divx) - 1;
	if ((minx != 0) && (maxx != 0) && (divx != 0)) {
		zero_x_index = (int)(minx / divx);
		zero_x_line = true;
	}
	else {
		zero_x_index = 0;
		zero_x_line = false;
	}

}

void LinegraphMulti::drawMultiData(CDC* pDC)
{
	if (((maxx - minx) > 0) && ((maxy - miny) > 0)) {
		if ((xdata.size() > 0) && (multiydata.size() > 0)) {
			int cindex = 0;
			int inc = 1 + (int)(xdata.size() / (float)xdata.size());
			for (int k = 0; k < multiydata.size(); k++) {
				if (multiydata[k].size() == xdata.size()) {
					CPen pen(PS_SOLID, 2, colors[cindex]);
					pDC->SelectObject(pen);
					double x = (double)axis_x1 + ((xdata[0] - minx) * dx) / (maxx - minx);
					if (x > (double)axis_x2) {
						x = (double)axis_x2;
					}
					double y = (double)axis_y1 + ((multiydata[k][0] - miny) * dy) / (1.02 * (maxy - miny));
					if (y > (double)axis_y1) {
						y = (double)axis_y1;
					}
					else if (y < (double)axis_y2) {
						y = (double)axis_y2;
					}
					pDC->MoveTo((int)x, (int)y);
					for (int i = 1; i < xdata.size(); i++) {
						x = (double)axis_x1 + ((xdata[i] - minx) * dx) / (maxx - minx);
						if (x > (double)axis_x2) {
							x = (double)axis_x2;
						}
						y = (double)axis_y1 + ((multiydata[k][i] - miny) * dy) / (1.02 * (maxy - miny));
						if (y > (double)axis_y1) {
							y = (double)axis_y1;
						}
						else if (y < (double)axis_y2) {
							y = (double)axis_y2;
						}
						pDC->LineTo((int)x, (int)y);
					}
					cindex++;
					if (cindex >= colors.size()) {
						cindex = 0;
					}
				}
			}
		}
	}
}

void LinegraphMulti::updateGraph(vector<double> sxdata, vector<vector<double>> sydata, CDC* pDC)
{
	float mymaxx = maxx;
	setData(sxdata, sydata);
	if (show_background) {
		drawFrame(pDC);
	}
	drawXAxis(pDC);
	drawMultiData(pDC);
}



