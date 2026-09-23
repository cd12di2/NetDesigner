
#include "pch.h"
#include "CMFCVisualManagerCustom.h"


CMFCVisualManagerCustom::CMFCVisualManagerCustom()
{
    blue = false;
    dark = false;
    color = false;
}

CMFCVisualManagerCustom::~CMFCVisualManagerCustom()
{
}

void CMFCVisualManagerCustom::OnDrawRibbonCategory(CDC* pDC, CMFCRibbonCategory* pCategory, CRect rectCategory)
{
    CBrush brush(GeColorInactiveBorder());
    pDC->FillRect(rectCategory, &brush);

    CPen pen(PS_SOLID, 1, GeColor3dlight());
    pDC->SelectObject(pen);
    pDC->MoveTo(CPoint(rectCategory.left, rectCategory.top));
    pDC->LineTo(CPoint(rectCategory.right - 1, rectCategory.top));

}

COLORREF CMFCVisualManagerCustom::OnDrawRibbonTabsFrame(CDC* pDC, CMFCRibbonBar* pWndRibbonBar, CRect rectTab)
{
    CBrush brush(GeColorWindow2());
    pDC->FillRect(rectTab, &brush);

    // color return is reserved by Microsoft
    return -1;
}

COLORREF CMFCVisualManagerCustom::OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel, CRect rectCaption)
{
    // the color of the ribbon
    CBrush brush(GeColorInactiveBorder());
    pDC->FillRect(rectPanel, &brush);

    CPen pen(PS_SOLID, 1, GeColorActiveBorder());
    pDC->SelectObject(pen);
    pDC->MoveTo(CPoint(rectPanel.right-1, rectPanel.top));
    pDC->LineTo(CPoint(rectPanel.right - 1, rectPanel.bottom));

    // This is the color of controls on the ribbon
    return GeColorGrayText();
}

COLORREF CMFCVisualManagerCustom::OnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive)
{
    // the background of the category tab
    if (bIsActive) {
        
        CBrush brush(GeColorInactiveBorder());
        pDC->FillRect(pTab->GetRect(), &brush);

        CPen pen(PS_SOLID, 1, GeColor3dlight());
        pDC->SelectObject(pen);
        pDC->MoveTo(CPoint(pTab->GetRect().left, pTab->GetRect().bottom));
        pDC->LineTo(CPoint(pTab->GetRect().left, pTab->GetRect().top));
        pDC->LineTo(CPoint(pTab->GetRect().right - 1, pTab->GetRect().top));
        pDC->LineTo(CPoint(pTab->GetRect().right - 1, pTab->GetRect().bottom));

        CPen pen2(PS_SOLID, 1, GeColorInactiveBorder());
        pDC->SelectObject(pen2);
        pDC->LineTo(CPoint(pTab->GetRect().left, pTab->GetRect().bottom));
    
        return GeColorGrayText();
    
    } else {

        CBrush brush(GeColorWindow2());
        pDC->FillRect(pTab->GetRect(), &brush);

        CPen pen(PS_SOLID, 1, GeColor3dlight());
        pDC->SelectObject(pen);
        pDC->MoveTo(CPoint(pTab->GetRect().left, pTab->GetRect().bottom));
        pDC->LineTo(CPoint(pTab->GetRect().right - 1, pTab->GetRect().bottom));
        pDC->SelectStockObject(BLACK_PEN);

    }
    return GeColorHighlightText();
}

void CMFCVisualManagerCustom::OnDrawRibbonPanelCaption(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectCaption)
{
    CBrush brush(GeColorInactiveBorder());
    pDC->FillRect(rectCaption, &brush);

    CPen pen(PS_SOLID, 1, GeColorActiveBorder());
    pDC->SelectObject(pen);
    pDC->MoveTo(CPoint(rectCaption.right - 1, rectCaption.top));
    pDC->LineTo(CPoint(rectCaption.right - 1, rectCaption.bottom));

    CString str = pPanel->GetName();

    if (!str.IsEmpty())
    {
        rectCaption.DeflateRect(1, 1);

        if ((rectCaption.Width() % 2) == 0)
        {
            rectCaption.right--;
        }

        rectCaption.OffsetRect(0, -1);

        COLORREF clrTextOld = pDC->SetTextColor(GeColorGrayText());
        pDC->DrawText(str, rectCaption, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        pDC->SetTextColor(clrTextOld);
    }
}

void CMFCVisualManagerCustom::OnDrawRibbonApplicationButton(CDC* pDC, CMFCRibbonButton* pButton)
{
    CBrush brush(GeColorHighlight());
    pDC->FillRect(pButton->GetRect(), &brush);
}

COLORREF CMFCVisualManagerCustom::OnDrawRibbonStatusBarPane(CDC* pDC, CMFCRibbonStatusBar* pBar, CMFCRibbonStatusBarPane* pPane)
{
    // this is the color of the names
    CBrush brush(GeColorWindow());
    pDC->FillRect(pPane->GetRect(), &brush);

    // this is the text color
    return GeColorBtnText();
}

void CMFCVisualManagerCustom::OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea)
{
    CBrush brush(GeColorWindow());
    pDC->FillRect(rectClient, &brush);
}
/*
void CMFCVisualManagerCustom::OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd)
{
    pDC->FillSolidRect(rect, RGB(0x0, 0xFF, 0));
}
*/
void CMFCVisualManagerCustom::OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd)
{
    int delta = 4;
    CRect myrect = CRect(rectTab.left+delta, rectTab.top, rectTab.right-delta, rectTab.bottom);

    if (bIsActive) {
        CBrush brush(GeColorWindow2());
        pDC->FillRect(myrect, &brush);
    }
    else {
        pDC->FillSolidRect(myrect, GeColorInactiveBorder());
    }

    CWnd* pTabWndCtrl = pTabWnd->GetTabWnd(iTab);

    CPen pen(PS_SOLID, 1, GeColor3dlight());
    pDC->SelectObject(pen);

    if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
    {
        pDC->MoveTo(CPoint(myrect.left, myrect.bottom));
        pDC->LineTo(CPoint(myrect.right - 1, myrect.bottom));
        pDC->MoveTo(CPoint(myrect.left, myrect.bottom-1));
        pDC->LineTo(CPoint(myrect.right - 1, myrect.bottom-1));
    }
    else {
        pDC->MoveTo(CPoint(myrect.left, myrect.top));
        pDC->LineTo(CPoint(myrect.right - 1, myrect.top));
        pDC->MoveTo(CPoint(myrect.left, myrect.top+1));
        pDC->LineTo(CPoint(myrect.right - 1, myrect.top+1));
    }

    // the right hand side of the tab
    pDC->MoveTo(CPoint(myrect.right - 1, myrect.top));
    pDC->LineTo(CPoint(myrect.right - 1, myrect.bottom));
    pDC->MoveTo(CPoint(myrect.left + 1, myrect.top));
    pDC->LineTo(CPoint(myrect.left + 1, myrect.bottom));


    CString str;
    //pTabWndCtrl->GetWindowTextW(str);
    pTabWnd->GetTabLabel(iTab, str);

    if (!str.IsEmpty())
    {
        rectTab.DeflateRect(1, 1);

        if ((rectTab.Width() % 2) == 0)
        {
            rectTab.right--;
        }

        rectTab.OffsetRect(0, -1);

        if (bIsActive) {
            COLORREF clrTextOld = pDC->SetTextColor(GeColorHighlightText());
            pDC->DrawText(str, rectTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
            pDC->SetTextColor(clrTextOld);
        }
        else {
            COLORREF clrTextOld = pDC->SetTextColor(GeColorGrayText());
            pDC->DrawText(str, rectTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
            pDC->SetTextColor(clrTextOld);
        }

    }
}
/*
void CMFCVisualManagerCustom::OnDrawRibbonMainPanelButtonBorder(CDC* pDC, CMFCRibbonButton* pButton)
{
    pDC->SelectStockObject(BLACK_PEN);
    pDC->SelectStockObject(NULL_BRUSH);
    pDC->Rectangle(pButton->GetRect());
}
*/
void  CMFCVisualManagerCustom::OnDrawRibbonMainPanelFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect)
{
    pDC->FillSolidRect(rect, GeColorInactiveBorder());
    CBrush brush(GeColorInactiveBorder());
    pDC->FrameRect(rect,&brush);
}
/*
COLORREF CMFCVisualManagerCustom::OnFillRibbonMainPanelButton(CDC* pDC, CMFCRibbonButton* pButton)
{
    pDC->FillSolidRect(pButton->GetRect(), GeColorHighlight());
    return RGB(0x00, 0xFF, 00);
}
*/
void CMFCVisualManagerCustom::OnFillRibbonMenuFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect)
{
    pDC->FillSolidRect(pPanel->GetRect(), GeColorWindow2());
}

COLORREF CMFCVisualManagerCustom::OnFillRibbonButton(CDC* pDC, CMFCRibbonButton* pButton)
{
    if (pButton->GetParentPanel() && pButton->GetParentPanel()->IsKindOf(RUNTIME_CLASS(CMFCRibbonMainPanel)))
    {
        if (pButton->IsHighlighted()) {
            pDC->FillSolidRect(pButton->GetRect(), GeColorHighlight());
            return GeColorHighlightText2(); 
        }
        else {
            pDC->FillSolidRect(pButton->GetRect(), GeColorWindow2());
            return GeColorHighlightText();
        }
    }
    else if (!pButton->GetParentPanel()) {
        if (pButton->IsHighlighted()) {
            pDC->FillSolidRect(pButton->GetRect(), GeColorHighlight());
            return GeColorHighlightText2(); 
        }
        else {
            pDC->FillSolidRect(pButton->GetRect(), GeColorWindow2());
            return GeColorHighlightText(); 
        }
    }
    return GeColorGrayText();
}

COLORREF CMFCVisualManagerCustom::OnDrawMenuLabel(CDC* pDC, CRect rect)
{
    pDC->FillSolidRect(rect, GeColorWindow2());
    return GeColorHighlightText();
}

COLORREF CMFCVisualManagerCustom::GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList)
{
    return GeColorInactiveBorder();
}

// Color for popup menu text
COLORREF CMFCVisualManagerCustom::GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled)
{
    if (bHighlighted) {
        return GeColorHighlightText();
    }
    return GeColorBtnText();
}

BOOL CMFCVisualManagerCustom::DrawTextOnGlass(CDC* pDC, CString strText, CRect rect, DWORD dwFlags, int nGlowSize, COLORREF clrText)
{
    return(__super::DrawTextOnGlass(pDC, strText, rect, dwFlags, nGlowSize, smokyblack));
}

void CMFCVisualManagerCustom::OnUpdateSystemColors()
{

    CMFCVisualManagerWindows7::OnUpdateSystemColors();

    GetGlobalData()->clrBarFace = GeColorBtnFace();
    GetGlobalData()->clrBtnFace = GeColorBtnFace();

    GetGlobalData()->clrBarShadow = GeColorBtnShadow();
    GetGlobalData()->clrBtnShadow = GeColorBtnShadow();

    GetGlobalData()->clrBarDkShadow = GeColor3ddkShadow();
    GetGlobalData()->clrBtnDkShadow = GeColor3ddkShadow();

    GetGlobalData()->clrBarLight = GeColor3dlight();
    GetGlobalData()->clrBtnLight = GeColor3dlight();

    GetGlobalData()->clrBarHilite = GeColorBtnHighlight();
    GetGlobalData()->clrBtnHilite = GeColorBtnHighlight();

    GetGlobalData()->clrBarText = GeColorBtnText();
    GetGlobalData()->clrBtnText = GeColorBtnText();
    GetGlobalData()->clrGrayedText = GeColorGrayText();
    GetGlobalData()->clrWindowFrame = GeColorWindowframe();

    GetGlobalData()->clrHilite = GeColorHighlight();
    GetGlobalData()->clrTextHilite = GeColorHighlightText();

    GetGlobalData()->clrBarWindow = GeColorWindow();
    GetGlobalData()->clrWindow = GeColorWindow();

    GetGlobalData()->clrWindowText = GeColorWindowText();

    GetGlobalData()->clrCaptionText = GeColorCaptionText();
    GetGlobalData()->clrMenuText = GeColorMenuText();

    GetGlobalData()->clrActiveCaption = GeColorActiveCaption();
    GetGlobalData()->clrInactiveCaption = GeColorInactiveCaption();

    GetGlobalData()->clrActiveCaptionGradient = GeColorGradientActiveCaption();
    GetGlobalData()->clrInactiveCaptionGradient = GeColorGradientInactiveCaption();

    GetGlobalData()->clrActiveBorder = GeColorActiveBorder();
    GetGlobalData()->clrInactiveBorder = GeColorInactiveBorder();

    GetGlobalData()->clrInactiveCaptionText = GeColorInactiveCaptionText();

    //GetGlobalData()->clrHotLinkHoveredText = RGB(0xff, 0, 0);
    //GetGlobalData()->clrHotLinkNormalText = RGB(0, 0xff, 0);
    //GetGlobalData()->clrHotLinkVisitedText = RGB(0, 0, 0xff);

    GetGlobalData()->brBtnFace.DeleteObject();
    GetGlobalData()->brBtnFace.CreateSolidBrush(GetGlobalData()->clrBtnFace);

    GetGlobalData()->brBarFace.DeleteObject();
    GetGlobalData()->brBarFace.CreateSolidBrush(GetGlobalData()->clrBarFace);

    GetGlobalData()->brActiveCaption.DeleteObject();
    GetGlobalData()->brActiveCaption.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

    GetGlobalData()->brInactiveCaption.DeleteObject();
    GetGlobalData()->brInactiveCaption.CreateSolidBrush(GetGlobalData()->clrInactiveCaption);

    GetGlobalData()->brHilite.DeleteObject();
    GetGlobalData()->brHilite.CreateSolidBrush(GetGlobalData()->clrHilite);

    GetGlobalData()->brBlack.DeleteObject();
    GetGlobalData()->brBlack.CreateSolidBrush(GetGlobalData()->clrBtnDkShadow);

    GetGlobalData()->brWindow.DeleteObject();
    GetGlobalData()->brWindow.CreateSolidBrush(GetGlobalData()->clrWindow);

    //GetGlobalData()->brLight.DeleteObject();
    //GetGlobalData()->brLight.CreateSolidBrush(RGB(0xff, 0xff, 0));

    GetGlobalData()->penHilite.DeleteObject();
    GetGlobalData()->penHilite.CreatePen(PS_SOLID, 1, RGB(0xff,0,0)/*afxGlobalData.clrHilite*/);

    GetGlobalData()->penBarFace.DeleteObject();
    GetGlobalData()->penBarFace.CreatePen(PS_SOLID, 1, RGB(0xff, 0, 0)/*afxGlobalData.clrBarFace*/);

    GetGlobalData()->penBarShadow.DeleteObject();
    GetGlobalData()->penBarShadow.CreatePen(PS_SOLID, 1, RGB(0xff, 0, 0)/*afxGlobalData.clrBarShadow*/);

    //GetGlobalData()->clrActiveBorder;
    //GetGlobalData()->clrActiveCaption;
    //GetGlobalData()->clrActiveCaptionGradient;
    //GetGlobalData()->clrBarDkShadow;
    //GetGlobalData()->clrBarFace;
    //GetGlobalData()->clrBarHilite;
    //GetGlobalData()->clrBarLight;
    //GetGlobalData()->clrBarShadow;
    //GetGlobalData()->clrBarText;
    //GetGlobalData()->clrBarWindow;
    //GetGlobalData()->clrBtnDkShadow;
    //GetGlobalData()->clrBtnFace;
    //GetGlobalData()->clrBtnHilite;
    //GetGlobalData()->clrBtnLight;
    //GetGlobalData()->clrBtnShadow;
    //GetGlobalData()->clrBtnText;
    //GetGlobalData()->clrCaptionText;
    //GetGlobalData()->clrGrayedText;
    //GetGlobalData()->clrHilite;
    GetGlobalData()->clrHotLinkHoveredText;
    GetGlobalData()->clrHotLinkNormalText;
    GetGlobalData()->clrHotLinkVisitedText;
    //GetGlobalData()->clrInactiveBorder;
    //GetGlobalData()->clrInactiveCaption;
    //GetGlobalData()->clrInactiveCaptionGradient;
    //GetGlobalData()->clrMenuText;
    //GetGlobalData()->clrTextHilite;
    //GetGlobalData()->clrWindow;
    //GetGlobalData()->clrWindowFrame;
    //GetGlobalData()->clrWindowText;
    //GetGlobalData()->brActiveCaption;
    //GetGlobalData()->brBarFace;
    //GetGlobalData()->brBlack;
    //GetGlobalData()->brBtnFace;
    //GetGlobalData()->brHilite;
    //GetGlobalData()->brInactiveCaption;
    GetGlobalData()->brLight;
    //GetGlobalData()->brWindow;

}

// Color for main frame thick border, thick panel borders, popup and File backgrounds, output window tabs
COLORREF CMFCVisualManagerCustom::GeColorWindow2()
{
    if (dark) {
        return(vsd_background2);// charlestongreen);
    }
    else if (blue) {
        return(mediumpersianblue);
    }
    else if (color) {
        return(lavender);
    }
    //return(adjustColorByPercent(antiflashwhite, 0));
    return(adjustColorByPercent(vsl_background2/*softwhite*/, 0));
}

// Color for ribbonbar, deselected output window tabs, property window thick frame
COLORREF CMFCVisualManagerCustom::GeColorInactiveBorder()
{
    if (dark) {
        return(adjustColorByPercent(vsd_background2, 20));
    }
    else if (blue) {
        return(eggshell);
    }
    else if (color) {
        return(olivine);
    }
    //return(adjustColorByPercent(white, 10));
    return(adjustColorByPercent(vsl_background2/*softwhite*/, -5));
}

// Color for property background, output window tab tops, and property window shadow
COLORREF CMFCVisualManagerCustom::GeColorWindow()
{
    if (dark) {
        return(adjustColorByPercent(vsd_background2, 20));
    }
    else if (blue) {
        return(adjustColorByPercent(eggshell, 10));// GetSysColor(COLOR_WINDOW);
    }
    else if (color) {
        return(adjustColorByPercent(alcroeggshell, 0));
    }
    //return(adjustColorByPercent(ghostwhite, 10));
    return(adjustColorByPercent(vsl_background2/*antiflashwhite*/, 5));
}

// Color for pinned panel tabs, output window frame, property description background
COLORREF CMFCVisualManagerCustom::GeColorBtnFace()
{
    if (dark) {
        return(adjustColorByPercent(vsd_background2, 30));
    }
    else if (blue) {
        return(adjustColorByPercent(eggshell, 0));
    }
    else if (color) {
        return(gardeneggshell);
    }
    return(adjustColorByPercent(vsl_background2/*antiflashwhite*/, -5));
}

// Color for upper portion of highlighted frame top frame
COLORREF CMFCVisualManagerCustom::GeColorActiveCaption()
{
    if (dark) {
        return(adjustColorByPercent(grayactivecaption, 10));
    }
    else if (blue) {
        return(adjustColorByPercent(tuftsblue, 0));
    }
    else if (color) {
        return(adjustColorByPercent(russiangreen, 20));
    }
    return(adjustColorByPercent(whitesmoke, -5));
}

// Color for lower portion of highlighted frame top frame
COLORREF CMFCVisualManagerCustom::GeColorGradientActiveCaption()
{
    if (dark) {
        return(adjustColorByPercent(grayactivecaption, -10));
    }
    else if (blue) {
        return(adjustColorByPercent(tuftsblue, -20));
    }
    else if (color) {
        return(adjustColorByPercent(russiangreen, 0));
    }
    return(adjustColorByPercent(whitesmoke, -15));
}

// Color for upper portion of un-highlighted frame top frame
COLORREF CMFCVisualManagerCustom::GeColorInactiveCaption()
{
    if (dark) {
        return(adjustColorByPercent(grayinactivecaption, 10));
    }
    else if (blue) {
        return(adjustColorByPercent(tuftsblue, -20));
    }
    else if (color) {
        return(adjustColorByPercent(russiangreen, 0));
    }
    return(adjustColorByPercent(whitesmoke, 0));
}

// Color for lower portion of un-highlighted frame top frame
COLORREF CMFCVisualManagerCustom::GeColorGradientInactiveCaption()
{
    if (dark) {
        return(adjustColorByPercent(grayinactivecaption, -10));
    }
    else if (blue) {
        return(adjustColorByPercent(tuftsblue, -40));
    }
    else if (color) {
        return(adjustColorByPercent(russiangreen, -20));
    }
    return(adjustColorByPercent(whitesmoke, -10));
}

// Color for ribbonbar panel dividers
COLORREF CMFCVisualManagerCustom::GeColorActiveBorder()
{
    if (dark) {
        return(grayborder);
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_ACTIVEBORDER);
}

// Color for file button, highlighted style button, and highlighted property label background
COLORREF CMFCVisualManagerCustom::GeColorHighlight()
{
    if (dark) {
        return(adjustColorByPercent(yamahablue, -15));
    }
    else if (blue) {
        return(yamahablue);
    }
    else if (color) {
        return(paradisepink);
    }
    return(adjustColorByPercent(aliceblue, -15));
}

// Color for ribbon panel text
COLORREF CMFCVisualManagerCustom::GeColorGrayText()
{
    if (dark) {
        return(adjustColorByPercent(vsd_foreground1, -10));
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return adjustColorByPercent(vsl_foreground1, -10);// RGB(0x36, 0x2b, 0x00);
}

// Color for shadow borders for pinned panel tabs, output window frame, property description 
COLORREF CMFCVisualManagerCustom::GeColorBtnShadow()
{
    if (dark) {
        return(adjustColorByPercent(graytext, -40));
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_BTNSHADOW);
}

// Color for highlighted property label text
COLORREF CMFCVisualManagerCustom::GeColorHighlightText()
{
    if (dark) {
        return(vsd_foreground1);// adjustColorByPercent(graytext, 10));
    }
    else if (blue) {
        return(white);
    }
    else if (color) {
        return(pinetree);
    }
    int gray = 10;
    return vsl_foreground1;// RGB(gray, gray, gray);
}

// Color for output window shadow, popup menu text, and zoom text
COLORREF CMFCVisualManagerCustom::GeColorBtnText()
{
    if (dark) {
        return(graytext);
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_BTNTEXT);
}

// Color for pinned pannel tab text
COLORREF CMFCVisualManagerCustom::GeColor3ddkShadow()
{
    if (dark) {
        return(adjustColorByPercent(graytext, -40));
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_3DDKSHADOW);
}

// Color for remaining ribbonbar text, file and style menu text, property window text
COLORREF CMFCVisualManagerCustom::GeColorWindowText()
{
    if (dark) {
        return(graytext);
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_WINDOWTEXT);
}

// Color for highlighted panel label text
COLORREF CMFCVisualManagerCustom::GeColorCaptionText()
{
    if (dark) {
        return(adjustColorByPercent(graytext, 10));
    }
    else if (blue) {
        return(white);
    }
    else if (color) {
        return(white);
    }
    return GetSysColor(COLOR_CAPTIONTEXT);
}

// Color for deselected panel main text
COLORREF CMFCVisualManagerCustom::GeColorInactiveCaptionText()
{
    if (dark) {
        return(adjustColorByPercent(graytext, -10));
    }
    else if (blue) {
        return(white);
    }
    else if (color) {
        return(white);
    }
    return GetSysColor(COLOR_INACTIVECAPTIONTEXT);
}

// Color for highlighted text in main menu
COLORREF CMFCVisualManagerCustom::GeColorHighlightText2()
{
    if (dark) {
        return(adjustColorByPercent(graytext, 10));
    }
    else if (blue) {
        return(white);
    }
    else if (color) {
        return(white);
    }
    //int gray = 254;
    int gray = 10;
    return RGB(gray, gray, gray);
}

// Color for highlighted popup menu text
COLORREF CMFCVisualManagerCustom::GeColorMenuText()
{
    if (dark) {
        return(adjustColorByPercent(vsd_background2, 0));
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return vsl_background2;// GetSysColor(COLOR_MENUTEXT);
}

// Color for ribbon shadow, output window tab shadow, and popup menu shadow
COLORREF CMFCVisualManagerCustom::GeColor3dlight()
{
    if (dark) {
        return(adjustColorByPercent(graytext, -40));
        return(grayborder);
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_3DLIGHT);
}

// Color for pinned panel shadow and popup menu shadow
COLORREF CMFCVisualManagerCustom::GeColorBtnHighlight()
{
    if (dark) {
        return(adjustColorByPercent(graytext, -40));
        return(grayborder);
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_BTNHIGHLIGHT);
}

// Color for ribbon tab text, output window tab text, and ribbon checkbox text
COLORREF CMFCVisualManagerCustom::GeColorWindowframe()
{
    if (dark) {
        return(adjustColorByPercent(graytext, -40));
        return(grayborder);
    }
    else if (blue) {
        return(coolblack);
    }
    else if (color) {
        return(pinetree);
    }
    return GetSysColor(COLOR_WINDOWFRAME);
}

COLORREF CMFCVisualManagerCustom::adjustColorByPercent(COLORREF clr, float percent)
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





