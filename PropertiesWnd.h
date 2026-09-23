
#pragma once

#include "NetDesignerDoc.h"


class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
	//DECLARE_MESSAGE_MAP()
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

class CMyMFCPropertyGridProperty : public CMFCPropertyGridFileProperty
{
	//DECLARE_DYNCREATE(CMyMFCPropertyGridProperty)

public:

	int lindex = -1;

	CMyMFCPropertyGridProperty(const CString& strName,
		BOOL bOpenFileDialog,
		const CString& strFileName,
		LPCTSTR lpszDefExt = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0) : CMFCPropertyGridFileProperty(strName, bOpenFileDialog, strFileName, lpszDefExt, dwFlags, lpszFilter, lpszDescr, dwData) {}

	BOOL HasButton() { AfxMessageBox(_T("HasButton")); return(true); };
	void setLayerIndex(int index) { lindex = index; };

	virtual void OnClickButton(CPoint point) 
	{ 
		if (lindex >= 0) {
			CWnd* pWndMain = AfxGetMainWnd();
			CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			pDoc->getLayerDataConnections(lindex);
		}
	};

};

class CMyMFCPropertyClickProperty : public CMFCPropertyGridProperty
{
	//DECLARE_DYNCREATE(CMyMFCPropertyGridProperty)

public:

	int lindex = -1;

	CMyMFCPropertyClickProperty(const CString& strName,
		const _variant_t& varValue,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL,
		LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars) {}

	virtual BOOL OnClickValue(UINT uiMsg, CPoint point)
	{
		CString mval = (CString)GetValue();
		if (mval == _T("Yes")) {
			SetValue((_variant_t)_T("No"));
		}
		else {
			SetValue((_variant_t)_T("Yes"));
		}
		return(false);
	};

	/*virtual void OnClickName(CPoint C)
	{
		AfxMessageBox(_T("OnClickName"));
	};*/

};

class CPropertiesWnd : public CDockablePane
{
// Construction
public:
	CPropertiesWnd() noexcept;

	void AdjustLayout();

// Attributes
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

	CBrush mbrush;
	CBrush mbrush2;

	int m_nComboHeight;

	bool display_data_sources;
	bool display_functions;
	bool display_functions2;
	bool display_type;
	bool display_conv;
	bool display_preprocess;
	bool display_input_index;
	bool display_conv_dimensions1;
	bool display_conv_dimensions2;
	bool display_data_button;
	bool display_matrix_dimensions;
	bool display_tranformer_dimensions;
	bool display_update_epochs;
	bool display_dropout_percent;
	bool edit_names;

	bool allow_image_input_size_edit;
	CString image_input_width;
	CString image_input_height;

	int displayType;

	CString multiple;

	CString mid;
	CString mlbl;
	CString mtype;
	CString mdata;
	CString mfuncn;
	CString mindexing;
	CString dmeta;
	CString dmalpha;
	CString dmrand;
	CString dmnumlayers;
	CString dmnodesperlayer;
	CString dmupdateepochs;
	CString dmtesterror;
	CString dmsplitvariable;
	CString dmdropoutpercent;
	CString tfuncn;

	CString mresiduals;
	CString mnormalization;
	CString mpatternsplit;

	int mnumlayers;
	int mnodesperlayer;
	
	double mupdateepochs;
	double mtesterror;
	double msplitvariable;

	int mkdimension;
	int mvdimension;
	int mmodeldimension;
	double msumaddlimit;
	bool mhasSoftmax;

	double meta;
	double malpha;
	double mrand;

	double mdropoutpercent;

	int index1;
	int index2;

	CString mcinput;
	CString mcoutput;

	CString mdatamode;
	CString mfilepath;

	CString conv_type;
	CString layer_type;

	CString dimxin;
	CString dimyin;
	CString dimxout;
	CString dimyout;

	CString stridexin;
	CString strideyin;

	CString kdimension;
	CString vdimension;
	CString modeldimension;
	CString sumaddlimit;
	CString hasSoftmax;

	CString mprocess;


protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

// Implementation
public:
	virtual ~CPropertiesWnd();

	void InitPropList(vector<int> selectedComponents, int subindex = -1);

	void addErrorString(CString err);
	
	void InitPropListDefault();
	void InitPropListNodes(vector<int> selectedComponents, bool update = true);
	void InitPropListConnects(vector<int> selectedComponents);
	void InitPropListConvLayers(vector<int> selectedComponents, bool update = true);
	void InitPropListDataSources(vector<int> selectedComponents);
	void InitPropListMultiple(vector<int> selectedComponents);
	void InitPropListAttentionLayers(vector<int> selectedComponents, bool update = true);
	void InitPropListNetClusters(vector<int> selectedComponents, bool update = true);
	void InitPropListDropouts(vector<int> selectedComponents, bool update = true);
	void InitPropListAssemblies(vector<int> selectedComponents, bool update = true);

	CString getLabel();
	CString getFunction();
	CString getDataField();
	CString getDataIndexType();
	CString getEta();
	CString getAlpha();
	CString getRand();
	
	int getIndex1();
	int getIndex2();

	CString getLayerType();
		
	int getConvType();
	CString getStringConvType();

	int getDimXIn();
	int getDimYIn();

	int getDimXOut();
	int getDimYOut();

	int getStride1();
	int getStride2();

	int getConvLayers();
	int getConvNodesPerLayer();
	
	double getUpdateEpochs();
	double getTestError();
	double getSplitVariable();
	CString getPatternSplitMethod();

	int getKDimension();
	int getVDimension();
	int getModelDimension();
	double getSumAddLimit();
	bool getHasSoftmax();
	CString getTransformerFunction();

	double getDropoutPercent();

	CString getInputImageWidth();
	CString getInputImageHeight();


protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);

	void SetPropListFont();

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);



};

