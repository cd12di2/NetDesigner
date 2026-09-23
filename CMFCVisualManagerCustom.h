
#pragma once
#include <afxvisualmanageroffice2007.h>


class CMFCVisualManagerCustom : public CMFCVisualManagerWindows7
{
	DECLARE_DYNCREATE(CMFCVisualManagerCustom)

	bool blue;
	bool dark;
	bool color;

public:
	CMFCVisualManagerCustom();
	~CMFCVisualManagerCustom();

	void setBlue(bool val) { blue = val; dark = false; color = false; };
	void setDark(bool val) { dark = val; blue = false; color = false; };
	void setColor(bool val) { color = val; blue = false; dark = false; };

	COLORREF white = 0x00fefefe;
	COLORREF eggshell = 0x00d6eaf0;
	COLORREF alcroeggshell = 0x00a7c4d2;
	COLORREF yamahablue = 0x00822d0a;
	COLORREF softwhite = 0x00fbfafb;
	COLORREF softwhite2 = 0x00e9e0c9;
	COLORREF warmwhite = 0x00dcf4fd;
	COLORREF whitesmoke = 0x00f5f5f5;

	COLORREF red = 0x000000FF;
	COLORREF bright_red = 0x003449fb;		

	COLORREF grayactivecaption = 0x00555555;
	COLORREF grayinactivecaption = 0x00484848;
	COLORREF grayborder = 0x00606060;
	COLORREF graytext = 0x00f4f4f4;

	// www.schemecolor.com - cleanest whites
	COLORREF aliceblue = 0x00fef9f0;
	COLORREF ghostwhite = 0x00fff8f8;
	COLORREF antiflashwhite = 0x00f4f3f2;

	// www.schemecolor.com - natural flower
	COLORREF russiangreen = 0x00579165;
	COLORREF olivine = 0x0084c0a1;
	COLORREF gardeneggshell = 0x00ceeeee;
	COLORREF paradisepink = 0x006352e0;

	// www.schemecolor.com - lavender flower
	COLORREF lightbrown = 0x001b5dbd;
	COLORREF eminence = 0x00892863;
	COLORREF cadmiumviolet = 0x009f3c78;
	COLORREF richlavender = 0x00ce69a5;
	COLORREF lavender = 0x00df7eb6;

	// www.schemecolor.com - sunflower fields
	COLORREF orange = 0x00029df8;
	COLORREF selectiveyellow = 0x0000bbf7;
	COLORREF philippineyellow = 0x0000caf9;
	COLORREF pinetree = 0x00222b2e;

	// www.schemecolor.com - shades of blue
	COLORREF coolblack = 0x005f2f07;
	COLORREF mediumpersianblue = 0x00a06112;
	COLORREF tuftsblue = 0x00d39538;
	COLORREF bluejeans = 0x00edcc58;

	// www.schemecolor.com - dark matte
	COLORREF smokyblack = 0x00121412;
	COLORREF eerieblack = 0x00191919;
	COLORREF charlestongreen = 0x002d2d2d;
	COLORREF arsenic = 0x004f443b;
	COLORREF japaneseindigo = 0x00483a30;
	COLORREF gunmetal = 0x003b2d25;

	// https://colorhunt.co/palettes/blue
	COLORREF blue1 = 0x0070381c;
	COLORREF blue2 = 0x00aa7e60;
	COLORREF tan1 = 0x00d2e3ea;
	COLORREF tan2 = 0x00f8f5f9;

	// Visual Studio Dark Theme  -  https://learn.microsoft.com/en-us/visualstudio/extensibility/ux-guidelines/color-value-reference-for-visual-studio?view=vs-2022
	COLORREF vsd_default = 0x00302D2D;
	COLORREF vsd_foreground1 = 0x00F1F1F1;
	COLORREF vsd_foreground2 = 0x00CC7A00;
	COLORREF vsd_background1 = 0x001C1B1B;
	COLORREF vsd_background2 = 0x00262525;
	COLORREF vsd_background3 = 0x00292929;
	COLORREF vsd_background4 = 0x00302D2D;
	COLORREF vsd_background5 = 0x00373333;
	COLORREF vsd_border1 = 0x001C1B1B;
	COLORREF vsd_border2 = 0x00262525;
	COLORREF vsd_border3 = 0x00403E3E;
	COLORREF vsd_border4 = 0x00463F3F;

	// Visual Studio Light Theme
	COLORREF vsl_default = 0x00F2EEEE;
	COLORREF vsl_foreground1 = 0x001E1E1E;
	COLORREF vsl_background1 = 0x00FCFCFC;
	COLORREF vsl_background2 = 0x00F5F5F5;
	COLORREF vsl_background3 = 0x00F5DEC9;
	COLORREF vsl_background4 = 0x00F2EEEE;
	COLORREF vsl_background5 = 0x00ECE8E7;
	COLORREF vsl_border1 = 0x00F5F5F5;
	COLORREF vsl_border2 = 0x00ECE8E7;
	COLORREF vsl_border3 = 0x00D8CECC;


	virtual void OnDrawRibbonCategory(CDC* pDC, CMFCRibbonCategory* pCategory, CRect rectCategory);
	virtual COLORREF OnDrawRibbonTabsFrame(CDC* pDC, CMFCRibbonBar* pWndRibbonBar, CRect rectTab);
	virtual COLORREF OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel, CRect rectCaption);
	virtual COLORREF OnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive);
	virtual void OnDrawRibbonPanelCaption(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectCaption);
	virtual void OnDrawRibbonApplicationButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual COLORREF OnDrawRibbonStatusBarPane(CDC* pDC, CMFCRibbonStatusBar* pBar, CMFCRibbonStatusBarPane* pPane);
	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	//virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual COLORREF GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList);
	//virtual void OnDrawRibbonMainPanelButtonBorder(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonMainPanelFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	//virtual COLORREF OnFillRibbonMainPanelButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnFillRibbonMenuFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	virtual COLORREF OnFillRibbonButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual COLORREF OnDrawMenuLabel(CDC* pDC, CRect rect);
	virtual COLORREF GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled);
	virtual BOOL DrawTextOnGlass(CDC* pDC, CString strText, CRect rect, DWORD dwFlags, int nGlowSize = 0, COLORREF clrText = (COLORREF)-1);	
	void OnUpdateSystemColors() override;

	COLORREF GeColorBtnFace();
	COLORREF GeColorBtnShadow();
	COLORREF GeColor3ddkShadow();
	COLORREF GeColor3dlight();
	COLORREF GeColorBtnHighlight();
	COLORREF GeColorBtnText();
	COLORREF GeColorGrayText();
	COLORREF GeColorWindowframe();
	COLORREF GeColorHighlight();
	COLORREF GeColorHighlightText();
	COLORREF GeColorHighlightText2();
	COLORREF GeColorWindow();
	COLORREF GeColorWindow2();
	COLORREF GeColorWindowText();
	COLORREF GeColorCaptionText();
	COLORREF GeColorMenuText();
	COLORREF GeColorActiveCaption();
	COLORREF GeColorInactiveCaption();
	COLORREF GeColorGradientActiveCaption();
	COLORREF GeColorGradientInactiveCaption();
	COLORREF GeColorActiveBorder();
	COLORREF GeColorInactiveBorder();
	COLORREF GeColorInactiveCaptionText();

	COLORREF adjustColorByPercent(COLORREF clr, float percent);

};

