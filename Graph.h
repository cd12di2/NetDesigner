#pragma once


class Graph
{
public:

	int theme = 0;

	CString xlabel;
	CString ylabel;

	COLORREF bright_red = 0x003449fb;
	COLORREF clr_solarized_base2 = 0x00d5e8ee;

	COLORREF vsl_foreground1 = 0x001E1E1E;
	COLORREF vsd_background1 = 0x001C1B1B;
	COLORREF vsd_background2 = 0x00262525;
	COLORREF vsd_background3 = 0x00292929;
	COLORREF vsd_background4 = 0x00302D2D;
	COLORREF vsd_background5 = 0x00373333;

	COLORREF vsl_background1 = 0x00FCFCFC;
	COLORREF vsl_background2 = 0x00F5F5F5;
	COLORREF vsl_background4 = 0x00F2EEEE;
	COLORREF vsl_background5 = 0x00ECE8E7;

	COLORREF net_background1 = 0x004368CB;
	COLORREF net_background2 = 0x004040CB;
	COLORREF net_background3 = 0x00ABBCC8;
	COLORREF net_background4 = 0x00F5D09B;

	//COLORREF clr_back = adjustColorByPercent(vsl_background2, 0);
	COLORREF clr_back = adjustColorByPercent(net_background3, 40);
	COLORREF clr_grid = adjustColorByPercent(clr_back, -40);
	COLORREF clr_grid2 = adjustColorByPercent(clr_back, -20);
	//COLORREF clr_grid = adjustColorByPercent(vsd_background5, 240);
	//COLORREF clr_grid2 = adjustColorByPercent(vsd_background5, 280);
	COLORREF clr_text = 0x00362b00;
	COLORREF clr_data1 = adjustColorByPercent(0x00d39538, -10);
	COLORREF clr_data2 = 0x002222b2;

	bool show_background = true;
	bool show_border = true;
	bool message_displayed = false;

	int mfont;

	int label_x;
	int label_y;

	int xaxistext_x1;
	int xaxistext_x2;
	int xaxistext_y;
	
	int yaxistext_x;
	int yaxistext_y1;
	int yaxistext_y2;

	int axis_x1;
	int axis_x2;
	int axis_y1;
	int axis_y2;

	float dx;
	float dy;

	float minx;
	float maxx;

	float miny;
	float maxy;

	bool use_custom_miny;
	bool use_custom_maxy;
	float custom_miny;
	float custom_maxy;

	int zero_x_index = 0;
	int zero_y_index = 0;
	bool zero_x_line = false;
	bool zero_y_line = false;

	CRect mrect;

public:
	Graph();
	~Graph();

	void init(CRect srect) { mrect = srect; };

	void setColors(bool dark = false);
	void setXLabel(CString label) { xlabel = label; };
	void setBackground(bool show = true) { show_background = show; };

	void setTheme(int ntheme) { theme = ntheme; };
	
	void setSize(CRect srect); 
	virtual void setGridCorners();

	virtual void drawGraph(CDC* pDC);
	virtual void drawFrame(CDC *pDC);
	virtual void drawXAxis(CDC* pDC);
	virtual void drawData(CDC* pDC);
	virtual void drawDataN(CDC* pDC);
	virtual void eraseGraph(CDC* pDC, CBrush* brush1);

	CString convertNumber(float num);

	void addErrorString(CString msg);

	COLORREF adjustColorByPercent(COLORREF clr, float percent);


};


class Histogram : public Graph
{
public:
	int numbars = 21;
	int barwidth;
	vector<double> data;

public:
	//Histogram();

	void setSize(CRect srect);
		
	void setNumBars(int num) { numbars = num; };
	void setData(vector<double> sdata);

	void setGridCorners();
	void drawData(CDC* pDC);


};


class Linegraph : public Graph
{
public:
	CString xlabel2;
	int numxgrid = 3;
	int numygrid = 4;
	float div = 100;

	vector<double> xdata;
	vector<double> ydata;
	vector<double> ydata2;
	vector<vector<double>> ydataN;

	COLORREF clr_data[16];

public:
	Linegraph();
	~Linegraph();

	virtual void reset();
	
	void updateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2, CDC* pDC);
	void updateGraphN(vector<double> *sxdata, vector<vector<double>> *sydata, CDC* pDC);
	void clearAndUpdateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2, CDC* pDC);
	void clearAndUpdateGraphN(vector<double>* sxdata, vector<vector<double>>* sydata, CDC* pDC);

	void setData(vector<double> sxdata, vector<double> sydata, vector<double> sydata2);
	void setData(vector<double>* sxdata, vector<vector<double>>* sydata);

	void drawFrameNoBackgound(CDC* pDC, COLORREF back);
	void drawXAxisNoLabels(CDC* pDC);

	virtual void setGridCorners();
	virtual void drawXAxis(CDC* pDC);
	virtual void drawData(CDC* pDC);
	void drawDataN(/*vector<double>* mxdata, vector<vector<double>>* mydata, */CDC* pDC);

};


class Linegraph2 : public Linegraph
{
public:
	float divx = 1;
	float divy = 1;
	float minmaxy = 1;
	float maxminy = 1;
	vector<double> ydata2;

public:
	Linegraph2();
	~Linegraph2();

	void reset();

	void updateGraph(vector<double> sxdata, vector<double> sydata, vector<double> sydata2, CDC* pDC);

	void setData(vector<double> sxdata, vector<double> sydata, vector<double> sydata2);

	void drawData2(CDC* pDC);


};


class LinegraphMulti : public Linegraph
{
public:
	float divx = 1;
	float divy = 1;
	float minmaxy = 1;
	float maxminy = 1;
	vector<COLORREF> colors;
	vector<vector<double>> multiydata;

public:
	LinegraphMulti();
	~LinegraphMulti();

	void reset();

	void updateGraph(vector<double> sxdata, vector<vector<double>> sydata, CDC* pDC);

	void setData(vector<double> sxdata, vector<vector<double>> sydata);

	void drawMultiData(CDC* pDC);


};

