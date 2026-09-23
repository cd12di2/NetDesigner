
#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "NetDesigner.h"
#include "NetDesignerView.h"
#include "AuxFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{
	m_nComboHeight = 0;
	
	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = false;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = false;

	allow_image_input_size_edit = false;
	image_input_width = _T("100");
	image_input_height = _T("100");;

	displayType = NONE;

	multiple = _T("Multiple Values");

	mid = _T("");
	mlbl = _T("");
	dmeta = _T("");
	dmalpha = _T("");
	dmrand = _T("");
	mtype = _T("");
	mdata = _T("");
	mfuncn = _T("");
	mindexing = _T("");
	dmnumlayers = _T("");
	dmnodesperlayer = _T("");
	dmupdateepochs = _T("");
	dmtesterror = _T("");
	dmsplitvariable = _T("");
	dmdropoutpercent = _T("");
	tfuncn = _T("");

	mresiduals = _T("");
	mnormalization = _T("");
	mpatternsplit = _T("");

	mnumlayers = 0;
	mnodesperlayer = 0;
	mupdateepochs = 5.0;
	mtesterror = 0.020;
	msplitvariable = 1.0;
	mdropoutpercent = 0.02;

	mkdimension = 4;
	mvdimension = 4;
	mmodeldimension = 4;
	msumaddlimit = 0.20;
	mhasSoftmax = true;

	meta = 0;
	malpha = 0;
	mrand = 0;

	index1 = 0;
	index2 = 0;

	conv_type = _T("DATA_CONV");
	layer_type = _T("Input");

	dimxin = _T("6");
	dimyin = _T("6");

	dimxout = _T("4");
	dimyout = _T("4");

	stridexin = _T("3");
	strideyin = _T("3");

	kdimension.Format(_T("%i"), mkdimension);
	vdimension.Format(_T("%i"), mvdimension);
	modeldimension.Format(_T("%i"), mmodeldimension);
	sumaddlimit.Format(_T("%.4f"), msumaddlimit);
	if (mhasSoftmax) {
		hasSoftmax = _T("Yes");
	}
	else {
		hasSoftmax = _T("No");
	}

	mcinput = _T("");
	mcoutput = _T("");

}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::addErrorString(CString err) {
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString(err);
}

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, ID_PROPCTRL_CLASS))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropListDefault();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();	
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	AdjustLayout();

	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->updateProperties();
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropListDefault()
{
	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = false;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = false;

}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

LRESULT CPropertiesWnd::OnPropertyChanged(__in WPARAM wparam, __in LPARAM lparam)
{

	CMFCPropertyGridProperty* pProperty = (CMFCPropertyGridProperty*)lparam;

	switch (wparam)
	{
	case(ID_PROPCTRL_CLASS):

		if (display_input_index) {
			if (mindexing != getDataIndexType()) {
				mindexing = getDataIndexType();
				mlbl = getLabel();
				mtype = getLayerType();
				mdata = getDataField();
				CWnd* pWndMain = AfxGetMainWnd();
				CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
				InitPropListNodes(pDoc->selectedComponents, false);
			}
		}
		else if (display_conv) {
			if (conv_type != getStringConvType()) {
				mlbl = getLabel();
				conv_type = getStringConvType();
				if (conv_type == _T("Series Data")) {
					CWnd* pWndMain = AfxGetMainWnd();
					CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
					int cindex = -1;
					if (pDoc->selectedComponents.size() == 1) {
						cindex = pDoc->selectedComponents[0];
					}
					if ((layer_type == _T("Input")) && (pDoc->alreadyHasInputLayer(cindex))) {
						layer_type = _T("Hidden");
					}
					else if ((layer_type == _T("Output")) && (pDoc->alreadyHasOutputLayer(cindex))) {
						layer_type = _T("Hidden");
					}
				}
				else if (conv_type == _T("Wave Data")) {
					layer_type = _T("Input");
				}
				else if (conv_type == _T("Text Data")) {
					if (layer_type == _T("Output")) {
						layer_type = _T("Hidden");
					}
				}
				else if (conv_type == _T("None")) {
					//layer_type = _T("Hidden");
				}
				CWnd* pWndMain = AfxGetMainWnd();
				CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
				InitPropListConvLayers(pDoc->selectedComponents, false);
			}
			else if ((layer_type != getLayerType()) && (conv_type != _T("Pattern Net"))) {
				mlbl = getLabel();
				layer_type = getLayerType();
				CWnd* pWndMain = AfxGetMainWnd();
				CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
				InitPropListConvLayers(pDoc->selectedComponents, false);
			}
			else if (((int)_tstof((LPCTSTR)(CString)dimxin) != getDimXIn()) || ((int)_tstof((LPCTSTR)(CString)dimyin) != getDimYIn()) ||
				((int)_tstof((LPCTSTR)(CString)dimxout) != getDimXOut()) || ((int)_tstof((LPCTSTR)(CString)dimyout) != getDimYOut()) ||
				((int)_tstof((LPCTSTR)(CString)stridexin) != getStride1()) || ((int)_tstof((LPCTSTR)(CString)strideyin) != getStride2())) {
				mlbl = getLabel();
				layer_type = getLayerType();
				dimxin.Format(_T("%i"), getDimXIn());
				dimyin.Format(_T("%i"), getDimYIn());
				dimxout.Format(_T("%i"), getDimXOut());
				dimyout.Format(_T("%i"), getDimYOut());
				stridexin.Format(_T("%i"), getStride1());
				strideyin.Format(_T("%i"), getStride2());
				CWnd* pWndMain = AfxGetMainWnd();
				CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
				InitPropListConvLayers(pDoc->selectedComponents, false);
			}
		}

	}

	return(0);
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);

}

void CPropertiesWnd::InitPropList(vector<int> selectedComponents, int subindex)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	if (selectedComponents.size() == 0) {
		InitPropListDefault();
		return;
	}
	bool allNodes = true;
	bool allConnects = true;
	bool allConvLayers = true;
	bool allAttentionLayers = true;
	bool allNetClusters = true;
	bool allDataSources = true;
	bool allDropouts = true;
	bool allAssemblies = true;
	CString ttype = _T(""); 
	for (int i = 0; i < selectedComponents.size(); i++) {
		int index = selectedComponents[i];
		if (pDoc->netData.isNode(index)) {
			allConnects = false;
			allConvLayers = false;
			allDataSources = false;
			allAttentionLayers = false;
			allNetClusters = false;
			allDropouts = false;
			allAssemblies = false;
			if (i > 0) {
				if (ttype != (CString)pDoc->netData.getNodeMode(index).c_str()) {
					allNodes = false;
				}
			}
			else {
				ttype = (CString)pDoc->netData.getNodeMode(index).c_str();
			}
		}
		else if (pDoc->netData.isConnect(index)) {
			allNodes = false;
			allConvLayers = false;
			allAttentionLayers = false;
			allDataSources = false;
			allNetClusters = false;
			allDropouts = false;
			allAssemblies = false;
		}
		else if (pDoc->netData.isDataSource(index)) {
			allConnects = false;
			allConvLayers = false;
			allAttentionLayers = false;
			allNetClusters = false;
			allDropouts = false;
			allNodes = false;
			allAssemblies = false;
		}
		else if (pDoc->netData.isConvLayer(index)) {
			allNodes = false;
			allConnects = false;
			allAttentionLayers = false;
			allDataSources = false;
			allNetClusters = false;
			allDropouts = false;
			allAssemblies = false;
		}
		else if (pDoc->netData.isAttentionLayer(index)) {
			allNodes = false;
			allConnects = false;
			allConvLayers = false;
			allDataSources = false;
			allNetClusters = false;
			allDropouts = false;
			allAssemblies = false;
		}
		else if (pDoc->netData.isNetCluster(index)) {
			allNodes = false;
			allConnects = false;
			allConvLayers = false;
			allDataSources = false;
			allAttentionLayers = false;
			allDropouts = false;
			allAssemblies = false;
		}
		else if (pDoc->netData.isDropout(index)) {
			allNodes = false;
			allConnects = false;
			allConvLayers = false;
			allDataSources = false;
			allAttentionLayers = false;
			allNetClusters = false;
			allAssemblies = false;
		}
		else if (pDoc->netData.isAssemblyLayer(index)) {
			allNodes = false;
			allConnects = false;
			allConvLayers = false;
			allDataSources = false;
			allAttentionLayers = false;
			allNetClusters = false;
			allDropouts = false;
		}
	}
	if (allNodes) {
		InitPropListNodes(selectedComponents);
	}
	else if (allConnects) {
		InitPropListConnects(selectedComponents);
	}
	else if (allDataSources) {
		InitPropListDataSources(selectedComponents);
	}
	else if (allConvLayers) {
		InitPropListConvLayers(selectedComponents);
	}
	else if (allAttentionLayers) {
		InitPropListAttentionLayers(selectedComponents);
	}
	else if (allNetClusters) {
		InitPropListNetClusters(selectedComponents);
	}
	else if (allDropouts) {
		InitPropListDropouts(selectedComponents);
	}
	else if (allAssemblies) {
		if (subindex >= 0) {
			vector<int> subindices;
			subindices.push_back(subindex);
			InitPropListNodes(subindices);
		}
		else {
			InitPropListAssemblies(selectedComponents);
		}
	}
	else {
		InitPropListMultiple(selectedComponents);
	}
}

void CPropertiesWnd::InitPropListNodes(vector<int> selectedComponents, bool update)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	vector<CString> mfields = pDoc->getDataFields(selectedComponents);

	display_data_sources = true;
	display_functions = true;
	display_functions2 = false;
	display_type = true;
	display_conv = false;
	display_preprocess = true;
	display_input_index = true;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = true;

	bool show_label = false;
	
	displayType = NODE;

	//int ftype = pDoc->getDataPathType();

	if (update) {

		mid = _T("");
		mlbl = _T("");

		mtype = pDoc->dtype;
		mdata = _T("");
		mfuncn = pDoc->dfuncn;
		mprocess = _T("None");
		mindexing = _T("None");

		meta = pDoc->deta;
		malpha = pDoc->dalpha;
		mrand = pDoc->drand;

		index1 = 0;
		index2 = 0;

		dmeta.Format(_T("%.6f"), meta);
		dmalpha.Format(_T("%.6f"), malpha);
		dmrand.Format(_T("%.6f"), mrand);

	}

	bool all_inputs = true;
	bool all_outputs = true;
	bool any_outputs = false;

	for (int i = 0; i < selectedComponents.size(); i++) {

		int index = selectedComponents[i];

		string ttype = pDoc->netData.getNodeMode(index);
		if (ttype == "Input") {
			display_functions = false;
			display_functions2 = false;
			display_preprocess = false;
			all_outputs = false;
		}
		else if (ttype == "Hidden") {
			display_data_sources = false;
			display_preprocess = false;
			display_input_index = false;
			all_inputs = false;
			all_outputs = false;
		}
		else if (ttype == "Output") {
			//display_input_index = false;
			any_outputs = true;
			all_inputs = false;
		}
		else if (ttype == "PreProcess") {
			display_data_sources = false;
			display_functions = false;
			display_functions2 = false;
			display_input_index = false;
			all_inputs = false;
			all_outputs = false;
		}

		if (i == 0) {
			if (pDoc->netData.isNode(index)) {
				if (update) {

					mid = (CString)pDoc->netData.getID(index).c_str();
					mlbl = (CString)pDoc->netData.getLabel(index).c_str();
					mtype = (CString)pDoc->netData.getNodeMode(index).c_str();
					mfuncn = (CString)pDoc->netData.getNodeFunction(index).c_str();
					meta = pDoc->netData.getNodeEta(index);
					malpha = pDoc->netData.getNodeAlpha(index);
					mrand = pDoc->netData.getNodeRand(index);
					dmeta.Format(_T("%.6f"), meta);
					dmalpha.Format(_T("%.6f"), malpha);
					dmrand.Format(_T("%.6f"), mrand);
					mdata = (CString)pDoc->netData.getDataField(index).c_str();
					mprocess = (CString)pDoc->netData.getNodePreProcessor(index).c_str();
					mindexing = (CString)pDoc->netData.getNodeIndexMode(index).c_str();
					index1 = pDoc->netData.getNodeXIndex(index);
					index2 = pDoc->netData.getNodeYIndex(index);

					if (mfuncn == _T("Connector")) {
						display_data_sources = false;
						display_functions = false;
						display_functions2 = false;
						display_type = false;
						display_conv = false;
						display_preprocess = false;
						display_input_index = false;
						display_conv_dimensions1 = false;
						display_conv_dimensions2 = false;
						display_data_button = false;
						display_matrix_dimensions = false;
						display_tranformer_dimensions = false;
						display_update_epochs = false;
						display_dropout_percent = false;
						show_label = true;
						edit_names = true;
					}

				}
			}
			else {
				displayType = NONE;
			}
		}
		else {
			if (pDoc->netData.isNode(index)) {
				mid = multiple;
				mlbl = multiple;
				if (mtype != (CString)pDoc->netData.getNodeMode(index).c_str()) {
					mtype = multiple;
				}
				if (mfuncn != (CString)pDoc->netData.getNodeFunction(index).c_str()) {
					mfuncn = multiple;
				}
				if (meta != pDoc->netData.getNodeEta(index)) {
					dmeta = multiple;
				}
				if (malpha != pDoc->netData.getNodeAlpha(index)) {
					dmalpha = multiple;
				}
				if (mrand != pDoc->netData.getNodeRand(index)) {
					dmrand = multiple;
				}
				if (mrand != pDoc->netData.getNodeRand(index)) {
					dmrand = multiple;
				}
				if (mindexing != (CString)pDoc->netData.getNodeIndexMode(index).c_str()) {
					mindexing = multiple;
					index1 = 0;
					index2 = 0;
				}
				mdata = multiple;
				edit_names = false;
			}
			else {
				displayType = NONE;
			}
		}

	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Node Settings"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	if (show_label) {
		pProp = new CMFCPropertyGridProperty(_T("Item Label"), mlbl, _T("User defined lebel"));
		pProp->AllowEdit(true);
		pGroup1->AddSubItem(pProp);
	}

	pProp = new CMFCPropertyGridProperty(_T("Node"), mtype, _T("Node Type"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	if (display_functions) {
		
		pProp = new CMFCPropertyGridProperty(_T("Function"), mfuncn, _T("Select activation function"));
		pProp->AddOption(_T("Sigmoid"));
		pProp->AddOption(_T("Relu"));
		pProp->AddOption(_T("Tanh"));
		pProp->AddOption(_T("Shockley"));
		pProp->AddOption(_T("Gaussian"));
		pProp->AddOption(_T("SoftMax"));
		if (!any_outputs) {
			pProp->AddOption(_T("MaxPool"));
		}
		if (!any_outputs) {
			pProp->AddOption(_T("Multiply"));
			pProp->AddOption(_T("Add"));
		}
		pProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Eta"), dmeta, _T("Specifies learning rate"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Alpha"), dmalpha, _T("Specifies learning momentum"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Random"), dmrand, _T("Specifies the weight random initialization range"));
		pGroup1->AddSubItem(pProp);

	}

	if (display_data_sources) {
		CString dlabel = _T("Data");
		if (all_inputs) {
			dlabel = _T("Input Data");
		}
		else if (all_outputs) {
			dlabel = _T("Output Data");
		}
		pProp = new CMFCPropertyGridProperty(dlabel, mdata, _T("Select data source"));
		if (mdata != multiple) {
			//if (ftype == FILE_TYPE_WAV) {
				//pProp->AddOption(_T("SoundFile"));
			//}
			for (int i = 0; i < mfields.size(); i++) {
				pProp->AddOption(mfields[i]);
			}
		}
		pProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pProp);
	}

	if (display_preprocess) {
		pProp = new CMFCPropertyGridProperty(_T("PreProcess Mode"), mprocess, _T("Specifies PreProcess type"));
		pProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pProp);
	}

	if (display_input_index) {
		pProp = new CMFCPropertyGridProperty(_T("Data Indexing"), mindexing, _T("Select indexing mode"));
		pProp->AddOption(_T("None"));
		pProp->AddOption(_T("Index"));
		//pProp->AddOption(_T("Convolution"));
		pProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pProp);
		if (mindexing == _T("Index")) {
			CString s = _T("");
			s.Format(_T("%i"), index1);
			if (display_functions) {
				pProp = new CMFCPropertyGridProperty(_T("  Index"), s, _T("Offset index from current - Must be greater than or equal to zero for output nodes"));
			}
			else {
				pProp = new CMFCPropertyGridProperty(_T("  Index"), s, _T("Offset index from current - Must be less than or equal to zero for input nodes"));
			}
			pGroup1->AddSubItem(pProp);
		}
	}

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListConnects(vector<int> selectedComponents)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = false;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = true;

	displayType = CONNECT;

	mid = _T("");
	mlbl = _T("");

	mcinput = _T("");
	mcoutput = _T("");

	if (selectedComponents.size() == 1) {
		mid = (CString)pDoc->netData.getID(selectedComponents[0]).c_str();
		mlbl = (CString)pDoc->netData.getLabel(selectedComponents[0]).c_str();
		mcinput = (CString)pDoc->netData.getInputNode(selectedComponents[0]).c_str();
		mcoutput = (CString)pDoc->netData.getOutputNode(selectedComponents[0]).c_str();
	}
	else {
		mid = multiple;
		mlbl = multiple;
		mcinput = multiple;
		mcoutput = multiple;
		edit_names = false;
	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Connect Settings"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Input Node"), mcinput, _T("Input Node"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Output Node"), mcoutput, _T("Output Node"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListConvLayers(vector<int> selectedComponents, bool update)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CAuxFunctions func;

	//addErrorString(_T("InitPropListConvLayers"));

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = true;
	display_conv = true;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = true;
	display_conv_dimensions2 = true;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = true;

	displayType = CONVLAYER;

	if (update) {

		mid = _T("");
		mlbl = _T("");

		int ctype = pDoc->getDataConvType();
		if (ctype == NONE) {
			conv_type = _T("None");
		}
		else if (ctype == DATA_CONV) {
			conv_type = _T("Series Data");
		}
		else if (ctype == IMAGE_CONV) {
			conv_type = _T("Image Data");
		}
		else if (ctype == SOUND_CONV) {
			conv_type = _T("Wave Data");
		}
		else if (ctype == TEXT_CONV) {
			conv_type = _T("Text Data");
		}
		else if (ctype == HIDDEN_NET) {
			conv_type = _T("Hidden Net");
		}

		layer_type = pDoc->dtype;
		mfuncn = pDoc->dfuncn;

		mnumlayers = pDoc->dclayers;
		mnodesperlayer = pDoc->dcnodesperlayer;

		meta = pDoc->deta;
		malpha = pDoc->dalpha;
		mrand = pDoc->drand;
		mprocess = _T("");

		dmnumlayers.Format(_T("%i"), mnumlayers);
		dmnodesperlayer.Format(_T("%i"), mnodesperlayer);

		dmeta.Format(_T("%.6f"), meta);
		dmalpha.Format(_T("%.6f"), malpha);
		dmrand.Format(_T("%.6f"), mrand);

		mresiduals = _T("No");
		mnormalization = _T("No");

		mmodeldimension = pDoc->dmodeldimension;

	}

	int mxin = 2;
	int myin = 16;
	int mxout = 2;
	int myout = 4;
	int stridex = 16;
	int stridey = 64;

	bool disallowedit1 = true;
	bool edit_residuals = false;
	bool edit_normalization = false;

	int cindex = -1;
	if (selectedComponents.size() == 1) {
		cindex = selectedComponents[0];
		if (update) {
			mid = (CString)pDoc->netData.getID(selectedComponents[0]).c_str();
			mlbl = (CString)pDoc->netData.getLabel(selectedComponents[0]).c_str();
			int ctype = pDoc->netData.getConvType(selectedComponents[0]);
			if (ctype == NONE) {
				conv_type = _T("None");
			}
			else if (ctype == DATA_CONV) {
				conv_type = _T("Series Data");
			}
			else if (ctype == IMAGE_CONV) {
				conv_type = _T("Image Data");
			}
			else if (ctype == SOUND_CONV) {
				conv_type = _T("Wave Data");
			}
			else if (ctype == TEXT_CONV) {
				conv_type = _T("Text Data");
			}
			else if (ctype == HIDDEN_NET) {
				conv_type = _T("Hidden Net");
			}
			layer_type = (CString)pDoc->netData.getNodeMode(selectedComponents[0]).c_str();
			mfuncn = (CString)pDoc->netData.getNodeFunction(selectedComponents[0]).c_str();
			meta = pDoc->netData.getNodeEta(selectedComponents[0]);
			malpha = pDoc->netData.getNodeAlpha(selectedComponents[0]);
			mrand = pDoc->netData.getNodeRand(selectedComponents[0]);
			mprocess = (CString)pDoc->netData.getNodePreProcessor(selectedComponents[0]).c_str();
			mnumlayers = pDoc->netData.getConvNumLayers(selectedComponents[0]);
			mnodesperlayer = func.getNodesPerLayer(selectedComponents[0]); // pDoc->netData.getConvNodesPerLayer(selectedComponents[0]);
			dmnumlayers.Format(_T("%i"), mnumlayers);
			dmnodesperlayer.Format(_T("%i"), mnodesperlayer);
			dmeta.Format(_T("%.6f"), meta);
			dmalpha.Format(_T("%.6f"), malpha);
			dmrand.Format(_T("%.6f"), mrand);
			if (func.getNodesPerLayer(selectedComponents[0]) == func.getNumLayerInputs(selectedComponents[0])) {
				edit_residuals = true;
				mresiduals = (CString)pDoc->netData.getResiduals(selectedComponents[0]).c_str();
			}
			if ((mfuncn == _T("Relu")) || (mfuncn == _T("Shockley"))) {
				edit_normalization = true;
				mnormalization = (CString)pDoc->netData.getNormalization(selectedComponents[0]).c_str();
			}
			CAuxFunctions func;
			if (ctype == IMAGE_CONV) {
				mxin = func.getConvInputX(string(CT2CA(mid)));
				mxout = func.getConvOutputX(string(CT2CA(mid)));
			}
			else if (ctype == DATA_CONV) {
				mxin = func.getInputWidthFromLayerName(string(CT2CA(mid)));
				mxout = mxin;
				disallowedit1 = false;
			}
			else {
				mxin = func.getConvInputX(string(CT2CA(mid)));
				mxout = func.getConvOutputX(string(CT2CA(mid)));
			}
			myin = func.getConvInputY(string(CT2CA(mid)));
			myout = func.getConvOutputY(string(CT2CA(mid)));
			stridex = func.getConvOutputStrideX(string(CT2CA(mid)));
			stridey = func.getConvOutputStrideY(string(CT2CA(mid)));
			mmodeldimension = pDoc->netData.getD(selectedComponents[0]);
			dimxin.Format(_T("%i"), mxin);
			dimyin.Format(_T("%i"), myin);
			dimxout.Format(_T("%i"), mxout);
			dimyout.Format(_T("%i"), myout);
			stridexin.Format(_T("%i"), stridex);
			strideyin.Format(_T("%i"), stridey);
			modeldimension.Format(_T("%i"), mmodeldimension);
			display_matrix_dimensions = false; // func.displayModelDimensionsForHiddenNet(selectedComponents[0]);
		}
	}
	else {
		mid = multiple;
		mlbl = multiple;
		conv_type = multiple;
		layer_type = multiple;
		mfuncn = multiple;
		dmeta = multiple;
		dmalpha = multiple;
		dmrand = multiple;
		mprocess = _T("");
		dmnumlayers = multiple;
		dmnodesperlayer = multiple;
		dimxin = multiple;
		dimyin = multiple;
		dimxout = multiple;
		dimyout = multiple;
		stridexin = multiple;
		strideyin = multiple;
		edit_names = false;
	}

	if (conv_type == _T("Series Data")) {
		if ((layer_type == _T("Input")) && (pDoc->alreadyHasInputLayer(cindex))) {
			if (!pDoc->checkInputSeriesLayerCount(cindex) && !pDoc->alreadyHasInputImageLayer(cindex)) {
				conv_type = _T("Image Data");
			}
			else if (!pDoc->checkInputSeriesLayerCount(cindex)/* && pDoc->alreadyHasInputImageLayer(cindex)*/) {
				//conv_type = _T("Series Data");
			//}
			//else {
				layer_type = _T("Hidden");
			}
		}
		else if ((layer_type == _T("Output")) && (pDoc->alreadyHasOutputLayer(cindex))) {
			layer_type = _T("Hidden");
		}
	}
	else if (conv_type == _T("Wave Data")) {
		layer_type = _T("Input");
	}
	else if (conv_type == _T("Text Data")) {
		layer_type = _T("Input");
	}
	else if (conv_type == _T("Hidden Net")) {
		layer_type = _T("Hidden");
	}
	else if (conv_type == _T("None")) {
		//layer_type = _T("Hidden");
	}

	if ((layer_type == _T("Output")) || (layer_type == _T("Hidden"))) {
		display_functions = true;
		display_functions2 = true;
		if ((layer_type == _T("Output")) && (conv_type == _T("Series Data"))) {
			display_functions2 = false;
			display_preprocess = true;
		}
	}

	if (((layer_type == _T("Input")) || (layer_type == _T("Output"))) && (conv_type == _T("Series Data"))) {
		display_data_button = true;
	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Layer Settings"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Data Type"), conv_type, _T("Select data type"));
	if (conv_type == _T("Hidden Net")) {
		//pProp->AddOption(_T("Hidden Net"));
	}
	else {
		//pProp->AddOption(_T("None"));
		pProp->AddOption(_T("Series Data"));
		pProp->AddOption(_T("Image Data"));
		if (layer_type == _T("Input")) {
			pProp->AddOption(_T("Wave Data"));
		}
		//pProp->AddOption(_T("Text Data"));
	}
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Layer Type"), layer_type, _T("Layer type"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	vector<string> supporting = func.getImmediateSupportingConvNets(string(CT2CA(mid)));// .getSupportingConvNets(mid);
	vector<string> following = func.getFollowingConvNets(string(CT2CA(mid)));

	if (display_functions) {

		pProp = new CMFCPropertyGridProperty(_T("Function"), mfuncn, _T("Select activation function"));
		pProp->AddOption(_T("Sigmoid"));
		pProp->AddOption(_T("Relu"));
		pProp->AddOption(_T("Tanh"));
		pProp->AddOption(_T("Shockley"));
		pProp->AddOption(_T("Gaussian"));
		if (conv_type == _T("Hidden Net")) {		// haven't been able to get softmax to work in hidden nodes 
			pProp->AddOption(_T("SoftMax"));
		}
		if (conv_type != _T("Hidden Net")) {
			pProp->AddOption(_T("MaxPool"));
		}
		pProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Eta"), dmeta, _T("Specifies learning rate"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Alpha"), dmalpha, _T("Specifies learning momentum"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Random"), dmrand, _T("Specifies the weight random initialization range"));
		pGroup1->AddSubItem(pProp);

		if (display_functions2) {

			pProp = new CMFCPropertyGridProperty(_T("Num Layers"), dmnumlayers, _T("Specifies the number of layers in the convolution"));
			pGroup1->AddSubItem(pProp);

			pProp = new CMFCPropertyGridProperty(_T("Nodes per Layer"), dmnodesperlayer, _T("Specifies the number of nodes per layer in the convolution"));
			pGroup1->AddSubItem(pProp);

		}

	}

	if ((conv_type == _T("Series Data")) && (layer_type == _T("Input"))) {
		display_matrix_dimensions = true;
	}

	if (display_matrix_dimensions) {
		pProp = new CMFCPropertyGridProperty(_T("Model Dimension"), modeldimension, _T("Specifies the model dimension"));
		pGroup1->AddSubItem(pProp);
	}

	CString dlabel = _T("Data");
	if (layer_type == _T("Input")) {
		dlabel = _T("Input Data");
	}
	else if (layer_type == _T("Output")) {
		dlabel = _T("Output Data");
	}
	CMyMFCPropertyGridProperty* pMyProp = new CMyMFCPropertyGridProperty(dlabel, false, _T("Select Data"));
	if (display_data_button) {
		if (selectedComponents.size() == 1) {
			pMyProp->setLayerIndex(selectedComponents[0]);
		}
		else {
			pMyProp->setLayerIndex(-1);
		}
		pMyProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pMyProp);
	}

	if (display_preprocess) {
		pProp = new CMFCPropertyGridProperty(_T("PreProcess Mode"), mprocess, _T("Specifies PreProcess type"));
		pProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pProp);
	}

	if (conv_type == _T("Image Data")) {
		if (((supporting.size() == 0) && (following.size() == 0)) || (layer_type == _T("Input"))) {
			display_conv_dimensions1 = false;
			display_conv_dimensions2 = false;
		}
		else if (((supporting.size() > 0) && (following.size() == 0)) && (layer_type != _T("Input"))) {
			bool net = false;
			for (int i = 0; i < supporting.size(); i++) {
				if (func.getConvType(supporting[i]) == HIDDEN_NET) {
					i = (int)supporting.size();
					net = true;
				}
			}
			if (net) {
				display_conv_dimensions1 = false;
				display_conv_dimensions2 = false;
			}
		}
	}
	if ((conv_type == _T("Series Data")) && (layer_type != _T("Hidden"))) {
		display_conv_dimensions1 = false;
	}
	if (conv_type == _T("Wave Data")) {
		display_conv_dimensions1 = false;
	}
	if (conv_type == _T("Text Data")) {
		display_conv_dimensions1 = false;
	}
	if (conv_type == _T("Hidden Net")) {
		display_conv_dimensions1 = false;
	}
	if (conv_type == _T("None")) {
		display_conv_dimensions1 = false;
	}

	if (display_conv_dimensions1) {

		if ((layer_type == _T("Output")) || (layer_type == _T("Hidden"))) {

			pProp = new CMFCPropertyGridProperty(_T("Conv Input Dims"), _T(""), _T("Convolution input characteristics"));
			pProp->AllowEdit(false);
			pGroup1->AddSubItem(pProp);

			if (conv_type == _T("Image Data")) {

				pProp = new CMFCPropertyGridProperty(_T("   Width"), dimxin, _T("   Image or subimage input convolution width"));
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   Height"), dimyin, _T("   Image or subimage input convolution height"));
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   StrideX"), stridexin, _T("   Image or subimage output convolution width stride"));
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   StrideY"), strideyin, _T("   Image or subimage output convolution width stride"));
				pGroup1->AddSubItem(pProp);

			}
			else {

				pProp = new CMFCPropertyGridProperty(_T("   Num Input Data"), dimxin, _T("   Number of data inputs"));
				if (disallowedit1) {
					pProp->AllowEdit(false);
				}
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   Input Conv Size"), dimyin, _T("   Series input size of convolution"));
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   Input Conv Stride"), stridexin, _T("   Input convolution stride"));
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   Input Frame Size"), strideyin, _T("   Size of input convolution frame"));
				pGroup1->AddSubItem(pProp);

			}

		}

		if ((layer_type == _T("Hidden")) || (layer_type == _T("Output"))) {

			pProp = new CMFCPropertyGridProperty(_T("Conv Output Dims"), _T(""), _T("Convolution output characteristics"));
			pProp->AllowEdit(false);
			pGroup1->AddSubItem(pProp);

			if (conv_type == _T("Image Data")) {

				pProp = new CMFCPropertyGridProperty(_T("   Width"), dimxout, _T("   Image or subimage output convolution width"));
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   Height"), dimyout, _T("   Image or subimage output convolution height"));
				pGroup1->AddSubItem(pProp);

			}
			else {

				pProp = new CMFCPropertyGridProperty(_T("   Num Output Data"), dimxout, _T("   Number of data outputs"));
				pProp->AllowEdit(false);
				pGroup1->AddSubItem(pProp);

				pProp = new CMFCPropertyGridProperty(_T("   Output Conv Size"), dimyout, _T("   Series size of output convolution"));
				pGroup1->AddSubItem(pProp);

			}

		}

	}

	if (selectedComponents.size() == 1) {

		if (conv_type == _T("Image Data")) {

			if (layer_type == _T("Input")) {

				allow_image_input_size_edit = false;
				CString ext = pDoc->getImageFileExtension();
				if (ext == _T("raw")) {
					allow_image_input_size_edit = true;
				}

				int inwidth = pDoc->image_width;
				int inheight = pDoc->image_height;

				if (allow_image_input_size_edit) {
					for (int i = 0; i < pDoc->netData.size(); i++) {
						if (pDoc->netData.isDataSource(i)) {
							inwidth = pDoc->netData.getRawImageWidth(i);
							inheight = pDoc->netData.getRawImageHeight(i);
							i = pDoc->netData.size();
						}
					}
				}
				else {
					for (int i = 0; i < pDoc->netData.size(); i++) {
						if (pDoc->netData.isDataSource(i)) {
							inwidth = pDoc->netData.getDataImageWidth(i);
							inheight = pDoc->netData.getDataImageHeight(i);
							i = pDoc->netData.size();
						}
					}
				}

				image_input_width.Format(_T("%i"), inwidth);
				pProp = new CMFCPropertyGridProperty(_T("File image width:"), image_input_width, _T("Width of file image"));
				pProp->AllowEdit(allow_image_input_size_edit);
				pGroup1->AddSubItem(pProp);

				image_input_height.Format(_T("%i"), inheight);
				pProp = new CMFCPropertyGridProperty(_T("File image height:"), image_input_height, _T("Height of file image"));
				pProp->AllowEdit(allow_image_input_size_edit);
				pGroup1->AddSubItem(pProp);

			}
			else if (layer_type == _T("Hidden")) {

				imageSizeData imageSize = func.getImageSizeData(pDoc->netData.getID(selectedComponents[0]), pDoc->netData);

				int inwidth = imageSize.input_width;
				int inheight = imageSize.input_height;

				int outwidth = imageSize.output_width;
				int outheight = imageSize.output_height;

				if ((following.size() > 0) && (supporting.size() > 0)) {
					pProp = new CMFCPropertyGridProperty(_T("Hidden Convolution"), _T(""), _T("Convolution image sizes"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
				}
				else if (following.size() > 0) {
					pProp = new CMFCPropertyGridProperty(_T("Input Image"), _T(""), _T("Image size"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
				}
				else if (supporting.size() > 0) {
					pProp = new CMFCPropertyGridProperty(_T("Output Image"), _T(""), _T("Image size"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
				}

				CString s = _T("");
				if ((supporting.size() > 0) || (following.size() > 0)) {
					s.Format(_T("%iw x %ih"), inwidth, inheight);
					pProp = new CMFCPropertyGridProperty(_T("   Input image:"), s, _T("   Width and height of convolution input image"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
					s.Format(_T("%iw x %ih"), outwidth, outheight);
					pProp->AllowEdit(false);
					pProp = new CMFCPropertyGridProperty(_T("   Output image:"), s, _T("   Width and height of convolution output image"));
					pGroup1->AddSubItem(pProp);
				}

			}
			else if (layer_type == _T("Output")) {

				int imagewidth = pDoc->image_width;
				int imageheight = pDoc->image_height;
				for (int i = 0; i < pDoc->netData.size(); i++) {
					if (pDoc->netData.isDataSource(i)) {
						imagewidth = pDoc->netData.getDataImageWidth(i);
						imageheight = pDoc->netData.getDataImageHeight(i);
					}
				}

				imageSizeData imageSize = func.getImageSizeData(pDoc->netData.getID(selectedComponents[0]), pDoc->netData);

				int inwidth = imageSize.input_width;
				int inheight = imageSize.input_height;

				int outwidth = imageSize.output_width;
				int outheight = imageSize.output_height;

				if (display_conv_dimensions2) {
					pProp = new CMFCPropertyGridProperty(_T("Output Convolution"), _T(""), _T("Convolution image sizes"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
				}
				else {
					pProp = new CMFCPropertyGridProperty(_T("Output Image"), _T(""), _T("Image size"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
				}

				CString s = _T("");
				if (display_conv_dimensions2) {
					s.Format(_T("%iw x %ih"), inwidth, inheight);
					pProp = new CMFCPropertyGridProperty(_T("   Input image:"), s, _T("   Width and height of convolution input image"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
					s.Format(_T("%iw x %ih"), outwidth, outheight);
					pProp = new CMFCPropertyGridProperty(_T("   Output image:"), s, _T("   Width and height of convolution output image"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);
				}

				s.Format(_T("%iw x %ih"), imagewidth, imageheight);
				pProp = new CMFCPropertyGridProperty(_T("   File image:"), s, _T("   Width and height of file image"));
				pProp->AllowEdit(false);
				pGroup1->AddSubItem(pProp);

			}

		}
		else if (conv_type == _T("Wave Data")) {

			if (layer_type == _T("Input")) {

				pProp = new CMFCPropertyGridProperty(_T("Wave Input"), _T(""), _T("Wave iiles"));
				pProp->AllowEdit(false);
				pGroup1->AddSubItem(pProp);

				CString s = _T("");
				s.Format(_T("%i"), (int)pDoc->getDataPathNamesSize(false));
				pProp = new CMFCPropertyGridProperty(_T("   Num Wave Files"), s, _T("   Numbeer of wav files in train set"));
				pProp->AllowEdit(false);
				pGroup1->AddSubItem(pProp);

				if (pDoc->wresults.wave_data_lengths.size() > 0) {

					s.Format(_T("%i"), pDoc->wresults.files_longest);
					pProp = new CMFCPropertyGridProperty(_T("   File Data Length"), s, _T("   Max data length of wave file(s)"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);

					s.Format(_T("%.0f"), pDoc->wresults.files_max);
					pProp = new CMFCPropertyGridProperty(_T("   Peak Data Value"), s, _T("   Peak data size of wave file(s)"));
					pProp->AllowEdit(false);
					pGroup1->AddSubItem(pProp);

				}

			}

		}
		else if (conv_type == _T("Text Data")) {

			if (layer_type == _T("Input")) {

				pProp = new CMFCPropertyGridProperty(_T("Text Input"), _T(""), _T("Text iiles"));
				pProp->AllowEdit(false);
				pGroup1->AddSubItem(pProp);

				CString s = _T("");
				s.Format(_T("%i"), (int)pDoc->getDataPathNamesSize(false));
				pProp = new CMFCPropertyGridProperty(_T("   Num Text Files"), s, _T("   Numbeer of txt files in train set"));
				pProp->AllowEdit(false);
				pGroup1->AddSubItem(pProp);

			}

		}
		else if ((conv_type == _T("Series Data")) && (layer_type == _T("Hidden"))) {

			pProp = new CMFCPropertyGridProperty(_T("Series Convolution"), _T(""), _T("Convolution sizes"));
			pProp->AllowEdit(false);
			pGroup1->AddSubItem(pProp);

			int outwidth = 0;
			if ((int)_tstof((LPCTSTR)stridexin) > 0) {
				outwidth = (int)_tstof((LPCTSTR)dimyout) * (1 + ((int)_tstof((LPCTSTR)strideyin) - ((int)_tstof((LPCTSTR)dimyin))) / (int)_tstof((LPCTSTR)stridexin));
			}
			if (outwidth < 0) {
				outwidth = 0;
			}
			CString s = _T("");
			s.Format(_T("%i"), outwidth);

			pProp = new CMFCPropertyGridProperty(_T("   Output Frame Size"), s, _T("   Size of output convolution frame"));
			pProp->AllowEdit(false);
			pGroup1->AddSubItem(pProp);

			s.Format(_T("%i"), (int)_tstof((LPCTSTR)dimxin) * (int)_tstof((LPCTSTR)strideyin));
			pProp = new CMFCPropertyGridProperty(_T("   Input Data Size"), s, _T("   Size of input data"));
			pProp->AllowEdit(false);
			pGroup1->AddSubItem(pProp);

			s.Format(_T("%i"), (int)_tstof((LPCTSTR)dimxin) * outwidth);
			pProp = new CMFCPropertyGridProperty(_T("   Output Data Size"), s, _T("   Size of output data"));
			pProp->AllowEdit(false);
			pGroup1->AddSubItem(pProp);

		}

	}

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListAttentionLayers(vector<int> selectedComponents, bool update)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CAuxFunctions func;

	//addErrorString(_T("InitPropListAttentionLayers"));

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = false;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = true;
	display_tranformer_dimensions = true;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = true;

	displayType = ATTENTION;

	if (update) {
		mid = _T("");
		mlbl = _T("");
		mkdimension = pDoc->dkdimension;
		mvdimension = pDoc->dvdimension;
		mmodeldimension = pDoc->dmodeldimension;
		msumaddlimit = pDoc->dmsumaddlimit;
		mhasSoftmax = pDoc->dhasSoftmax;
		tfuncn = _T("Quadratic");
	}

	bool disallowedit1 = true;
	
	int minput_size = 0;
	CString input_size = _T("");

	bool self = false;;

	int cindex = -1;
	if (selectedComponents.size() == 1) {
		cindex = selectedComponents[0];
		if (update) {			
			mid = (CString)pDoc->netData.getID(selectedComponents[0]).c_str();
			mlbl = (CString)pDoc->netData.getLabel(selectedComponents[0]).c_str();
			self = pDoc->netData.isSelfAttentionLayer(selectedComponents[0]);
			mkdimension = pDoc->netData.getDk(selectedComponents[0]);
			mvdimension = pDoc->netData.getDv(selectedComponents[0]);
			mmodeldimension = pDoc->netData.getD(selectedComponents[0]);
			msumaddlimit = pDoc->netData.getSumAddLimit(selectedComponents[0]);
			mhasSoftmax = pDoc->netData.hasSoftmaxProcessing(selectedComponents[0]);
			kdimension.Format(_T("%i"), mkdimension);
			vdimension.Format(_T("%i"), mvdimension);
			sumaddlimit.Format(_T("%.4f"), msumaddlimit);
			modeldimension.Format(_T("%i"), mmodeldimension);
			if (mhasSoftmax) {
				hasSoftmax = _T("Yes");
			}
			else {
				hasSoftmax = _T("No");
			}
			tfuncn = _T("Quadratic");
			if (!pDoc->netData.getTransformerModeQuadratic(selectedComponents[0])) {
				tfuncn = _T("Linear");
			}
			minput_size = func.getAttentionInputSize(func.getIndexFromID(string(CT2CA(mid))));
			input_size.Format(_T("%i"), minput_size);
		}
	}
	else {
		mid = multiple;
		mlbl = multiple;
		kdimension = multiple;
		vdimension = multiple;
		modeldimension = multiple;
		edit_names = false;
	}

	if (self) {
		display_tranformer_dimensions = false;
	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Attention Layer Settings"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	if (display_tranformer_dimensions) {

		pProp = new CMFCPropertyGridProperty(_T("K Dimension"), kdimension);// , _T("Specifies the dimension for K and Q matrices"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("V Dimension"), vdimension, _T("Specifies the dimension for the V matrix"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Model Dimension"), modeldimension, _T("Specifies the model dimension"));
		pGroup1->AddSubItem(pProp);

	}

	pProp = new CMFCPropertyGridProperty(_T("Sum Add Limit"), sumaddlimit, _T("Division limit when backpropagating matrices"));
	pGroup1->AddSubItem(pProp);

	if (display_tranformer_dimensions) {

		pProp = new CMFCPropertyGridProperty(_T("Input Dimension"), input_size, _T("The calculated input dimension"));
		pProp->AllowEdit(false);
		pGroup1->AddSubItem(pProp);

	}

	pProp = new CMyMFCPropertyClickProperty(_T("SoftMax Processing"), (_variant_t)hasSoftmax, _T("Use softmax processing on KQ matrix multiplication"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Mode"), tfuncn, _T("Select attention matrix processing order"));
	pProp->AddOption(_T("Quadradic"));
	pProp->AddOption(_T("Linear"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListNetClusters(vector<int> selectedComponents, bool update)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CAuxFunctions func;

	//addErrorString(_T("InitPropListNetClusters"));

	display_data_sources = false;
	display_functions = true;
	display_functions2 = true;
	display_type = true;
	display_conv = true;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = true;
	display_dropout_percent = false;
	edit_names = true;

	displayType = NETCLUSTER;

	if (update) {

		mid = _T("");
		mlbl = _T("");

		conv_type = _T("Pattern Net");

		layer_type = pDoc->dtype;
		mfuncn = pDoc->dfuncn;

		mnumlayers = 1;
		mnodesperlayer = pDoc->dcnodesperlayer;
		mupdateepochs = pDoc->dcupdateepochs;

		meta = pDoc->deta;
		malpha = pDoc->dalpha;
		mrand = pDoc->drand;

		mpatternsplit = _T("Median");

		dmnumlayers.Format(_T("%i"), mnumlayers);
		dmnodesperlayer.Format(_T("%i"), mnodesperlayer);
		dmupdateepochs.Format(_T("%.1f"), mupdateepochs);
		dmtesterror.Format(_T("%.3f"), mtesterror);
		dmsplitvariable.Format(_T("%.2f"), msplitvariable);

		dmeta.Format(_T("%.6f"), meta);
		dmalpha.Format(_T("%.6f"), malpha);
		dmrand.Format(_T("%.6f"), mrand);

	}

	if (selectedComponents.size() == 1) {
		if (update) {
			mid = (CString)pDoc->netData.getID(selectedComponents[0]).c_str();
			mlbl = (CString)pDoc->netData.getLabel(selectedComponents[0]).c_str();
			conv_type = _T("Pattern Net");
			layer_type = (CString)pDoc->netData.getNodeMode(selectedComponents[0]).c_str();
			mfuncn = (CString)pDoc->netData.getNodeFunction(selectedComponents[0]).c_str();
			meta = pDoc->netData.getNodeEta(selectedComponents[0]);
			malpha = pDoc->netData.getNodeAlpha(selectedComponents[0]);
			mrand = pDoc->netData.getNodeRand(selectedComponents[0]);
			mnumlayers = 1;
			mnodesperlayer = func.getNodesPerLayer(selectedComponents[0]); 
			mupdateepochs = func.getUpdateEpochs(selectedComponents[0]);
			mtesterror = pDoc->netData.getClusterTestError(selectedComponents[0]);
			msplitvariable = pDoc->netData.getClusterSplitVariable(selectedComponents[0]);
			mpatternsplit = (CString)pDoc->netData.getPatternSplitMethod(selectedComponents[0]).c_str();
			dmnumlayers.Format(_T("%i"), mnumlayers);
			dmnodesperlayer.Format(_T("%i"), mnodesperlayer);
			dmupdateepochs.Format(_T("%.1f"), mupdateepochs);
			dmtesterror.Format(_T("%.3f"), mtesterror);
			dmsplitvariable.Format(_T("%.2f"), msplitvariable);
			dmeta.Format(_T("%.6f"), meta);
			dmalpha.Format(_T("%.6f"), malpha);
			dmrand.Format(_T("%.6f"), mrand);
			CAuxFunctions func;
		}
	}
	else {
		mid = multiple;
		mlbl = multiple;
		conv_type = multiple;
		layer_type = multiple;
		mfuncn = multiple;
		dmeta = multiple;
		dmalpha = multiple;
		dmrand = multiple;
		dmnumlayers = multiple;
		dmnodesperlayer = multiple;
		dmupdateepochs = multiple;
		dmtesterror = multiple;
		dmsplitvariable = multiple;
		edit_names = false;
	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Pattern Net Settings"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Process Type"), conv_type, _T(""));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Layer Type"), layer_type, _T("Layer type"));
	pProp->AddOption(_T("Hidden"));
	pProp->AddOption(_T("Output"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	if (display_functions) {

		pProp = new CMFCPropertyGridProperty(_T("Function"), mfuncn, _T("Select activation function"));
		pProp->AddOption(_T("Sigmoid"));
		pProp->AddOption(_T("Relu"));
		pProp->AddOption(_T("Tanh"));
		pProp->AddOption(_T("SoftMax"));
		pProp->AllowEdit(FALSE);
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Eta"), dmeta, _T("Specifies learning rate"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Alpha"), dmalpha, _T("Specifies learning momentum"));
		pGroup1->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty(_T("Random"), dmrand, _T("Specifies the weight random initialization range"));
		pGroup1->AddSubItem(pProp);

		if (display_functions2) {

			pProp = new CMFCPropertyGridProperty(_T("Num Layers"), dmnumlayers, _T("Limited to one layer in pattern net"));
			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);

			pProp = new CMFCPropertyGridProperty(_T("Num Patterns"), dmnodesperlayer, _T("Specifies the number of patterns to sort"));
			pGroup1->AddSubItem(pProp);

			pProp = new CMFCPropertyGridProperty(_T("Update Epochs"), dmupdateepochs, _T("Specifies the number of epochs to update cluster algorithms"));
			pGroup1->AddSubItem(pProp);

			pProp = new CMFCPropertyGridProperty(_T("Max Process Error"), dmtesterror, _T("Specifies the maximum process error per update"));
			pGroup1->AddSubItem(pProp);

			pProp = new CMFCPropertyGridProperty(_T("Split Variable"), dmsplitvariable, _T("Choose Next Cluster to Split by Standard Deviation - 0.0 <==> 1.0 - Chose by Cluster Size"));
			pGroup1->AddSubItem(pProp);

			pProp = new CMyMFCPropertyClickProperty(_T("Pattern Split Method"), (_variant_t)mpatternsplit, _T("Pattern search method"));
			pProp->AddOption(_T("Median"));
			pProp->AddOption(_T("Adaptive"));
			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);

		}

	}

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListDropouts(vector<int> selectedComponents, bool update)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CAuxFunctions func;

	//addErrorString(_T("InitPropListDropouts"));

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = false;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = true;
	edit_names = true;

	displayType = DROPOUT;

	if (update) {
		mid = _T("");
		mlbl = _T("");
		conv_type = _T("Dropout");
		mdropoutpercent = 0.02;
		dmdropoutpercent.Format(_T("%.3f"), mdropoutpercent);
	}

	if (selectedComponents.size() == 1) {
		if (update) {
			mid = (CString)pDoc->netData.getID(selectedComponents[0]).c_str();
			mlbl = (CString)pDoc->netData.getLabel(selectedComponents[0]).c_str();
			conv_type = _T("Dropout");
			mdropoutpercent = pDoc->netData.getDropoutPercent(selectedComponents[0]);
			dmdropoutpercent.Format(_T("%.3f"), mdropoutpercent);
		}
	}
	else {
		mid = multiple;
		mlbl = multiple;
		dmdropoutpercent = multiple;
		edit_names = false;
	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Dropout Settings"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Dropout Rate"), dmdropoutpercent, _T("Set data dropout rate"));
	pGroup1->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListAssemblies(vector<int> selectedComponents, bool update)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	CAuxFunctions func;

	//addErrorString(_T("InitPropListAssemblies"));

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = false;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = true;

	displayType = ASSEMBLY;

	CString reference = _T("");

	if (update) {
		mid = _T("");
		mlbl = _T("");
		conv_type = _T("Assembly");
	}

	if (selectedComponents.size() == 1) {
		if (update) {
			mid = (CString)pDoc->netData.getID(selectedComponents[0]).c_str();
			mlbl = (CString)pDoc->netData.getLabel(selectedComponents[0]).c_str();
			reference = (CString)pDoc->netData.getAssemblyReference(selectedComponents[0]).c_str();
			conv_type = _T("Assembly");
		}
	}
	else {
		mid = multiple;
		mlbl = multiple;
		reference = multiple;
		edit_names = false;
	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Assembly Properties"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Item Label"), mlbl, _T("User defined lebel"));
	pProp->AllowEdit(true);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Assembly Reference"), reference, _T("Reference to other assembly"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListDataSources(vector<int> selectedComponents)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = true;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = true;

	displayType = DATASOURCE;

	CAuxFunctions func;

	mid = _T("");
	mlbl = _T("");

	mdatamode = _T("");
	mfilepath = _T("");

	if (selectedComponents.size() == 1) {
		mid = (CString)pDoc->netData.getID(selectedComponents[0]).c_str();
		mlbl = (CString)pDoc->netData.getLabel(selectedComponents[0]).c_str();
		mdatamode = (CString)pDoc->netData.getDataMode(selectedComponents[0]).c_str();
		vector<string> paths = pDoc->netData.getFilePaths(selectedComponents[0]);
		if (paths.size() > 0) {
			mfilepath = (CString)paths[0].c_str();
		}
		if (paths.size() > 1) {
			mfilepath += _T(" (multiple)");
		}
		mfilepath = func.removePathFromFilename(mfilepath);
	}
	else {
		mid = multiple;
		mlbl = multiple;
		mdatamode = multiple;
		mfilepath = multiple;
		edit_names = false;
	}

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Data Source"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Source"), mdatamode, _T("Data Source Type"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("File Path"), mfilepath, _T("File Path"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup1);

}

void CPropertiesWnd::InitPropListMultiple(vector<int> selectedComponents)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	display_data_sources = false;
	display_functions = false;
	display_functions2 = false;
	display_type = false;
	display_conv = false;
	display_preprocess = false;
	display_input_index = false;
	display_conv_dimensions1 = false;
	display_conv_dimensions2 = false;
	display_data_button = false;
	display_matrix_dimensions = false;
	display_tranformer_dimensions = false;
	display_update_epochs = false;
	display_dropout_percent = false;
	edit_names = false;

	displayType = MULTIPLE;

	mid = multiple;
	mlbl = multiple;
	mtype = multiple;

	SetPropListFont();

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Multiple Settings"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Item ID"), mid, _T("Unique item identifier"));
	pProp->AllowEdit(false);
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Component Type"), mtype, _T("Component type"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup1);

}

CString CPropertiesWnd::getLabel()
{
	CString rval = _T("");
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (edit_names) {
			rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(1)->GetValue();
		}
	}
	return(rval);
}

int CPropertiesWnd::getConvType()
{
	int rval = NONE;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv) {
			int index = 1;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				if (s == _T("Series Data")) {
					rval = DATA_CONV;
				}
				else if (s == _T("Image Data")) {
					rval = IMAGE_CONV;
				}
				else if (s == _T("Wave Data")) {
					rval = SOUND_CONV;
				}
				else if (s == _T("Text Data")) {
					rval = TEXT_CONV;
				}
				else if (s == _T("Hidden Net")) {
					rval = HIDDEN_NET;
				}
				else if (s == _T("Pattern Net")) {
					rval = NETCLUSTER;
				}
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getStringConvType()
{
	CString rval = _T("");
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv) {
			int index = 1;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getFunction()
{
	CString rval = mfuncn;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_functions) {
			int index = 2;
			if (display_conv) {
				index += 1;
			}
			if (display_data_sources) {
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getDataField()
{
	CString rval = _T("");
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_data_sources) {
			int index = 2;
			if (display_conv) {
				index += 1;
			}
			if (display_functions) {
				index += 4;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getDataIndexType()
{
	CString rval = _T("");
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_input_index) {
			int index = 3;
			if (display_conv) {
				index += 1;
			}
			if (display_functions) {
				index += 5;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getEta()
{
	CString rval = dmeta;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_functions) {
			int index = 3;
			if (display_conv) {
				index += 1;
			}
			if (display_data_sources) {
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getAlpha()
{
	CString rval = dmalpha;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_functions) {
			int index = 4;
			if (display_conv) {
				index += 1;
			}
			if (display_data_sources) {
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getRand()
{
	CString rval = dmrand;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_functions) {
			int index = 5;
			if (display_conv) {
				index += 1;
			}
			if (display_data_sources) {
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getConvLayers()
{
	int rval = mnumlayers;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_functions && display_functions2) {
			int index = 6;
			if (display_conv) {
				index += 1;
			}
			if (display_data_sources) {
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				rval = (int)_tstof((LPCTSTR)s);
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getConvNodesPerLayer()
{
	int rval = mnodesperlayer;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_functions && display_functions2) {
			int index = 7;
			if (display_conv) {
				index += 1;
			}
			if (display_data_sources) {
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				rval = (int)_tstof((LPCTSTR)s);
			}
		}
	}
	return(rval);
}

double CPropertiesWnd::getUpdateEpochs()
{
	double rval = mupdateepochs;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_update_epochs) {
			int index = 8;
			if (display_conv) {
				index += 1;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				rval = (double)_tstof((LPCTSTR)s);
			}
		}
	}
	return(rval);
}

double CPropertiesWnd::getTestError()
{
	double rval = mtesterror;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_update_epochs) {
			int index = 9;
			if (display_conv) {
				index += 1;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				rval = (double)_tstof((LPCTSTR)s);
			}
		}
	}
	return(rval);
}

double CPropertiesWnd::getSplitVariable()
{
	double rval = msplitvariable;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_update_epochs) {
			int index = 10;
			if (display_conv) {
				index += 1;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				rval = (double)_tstof((LPCTSTR)s);
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getPatternSplitMethod()
{
	CString rval = mpatternsplit;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_update_epochs) {
			int index = 11;
			if (display_conv) {
				index += 1;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

double CPropertiesWnd::getDropoutPercent()
{
	double rval = mdropoutpercent;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_dropout_percent) {
			int index = 1;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				rval = (double)_tstof((LPCTSTR)s);
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getIndex1()
{
	int rval = index1;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_input_index) {
			if ((getDataIndexType() == _T("Index")) || (getDataIndexType() == _T("Convolution"))) {
				int index = 4;
				if (display_conv) {
					index += 1;
				}
				if (display_functions) {
					index += 5;
				}
				if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
					CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
					rval = (int)_tstof((LPCTSTR)s);
				}
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getIndex2()
{
	int rval = index2;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_input_index) {
			if (getDataIndexType() == _T("Convolution")) {
				int index = 5;
				if (display_conv) {
					index += 1;
				}
				if (display_functions) {
					index += 5;
				}
				if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
					CString s = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
					rval = (int)_tstof((LPCTSTR)s);
				}
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getLayerType()
{
	CString rval = layer_type; 
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_type) {
			int index = 1;
			if (display_conv) {
				index += 1;
			}
			if (index <= m_wndPropList.GetProperty(0)->GetSubItemsCount()) {
				rval = m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getDimXIn()
{
	int rval = (int)_tstof((LPCTSTR)dimxin);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv_dimensions1) {
			int index = 3;
			if (display_conv) {
				index += 1;
			}
			if (display_functions) {
				index += 6;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getDimYIn()
{
	int rval = (int)_tstof((LPCTSTR)dimyin);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv_dimensions1) {
			int index = 4;
			if (display_conv) {
				index += 1;
			}
			if (display_functions) {
				index += 6;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getStride1()
{
	int rval = (int)_tstof((LPCTSTR)stridexin);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv_dimensions1) {
			int index = 5;
			if (display_conv) {
				index += 1;
			}
			if (display_functions) {
				index += 6;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getStride2()
{
	int rval = (int)_tstof((LPCTSTR)strideyin);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv_dimensions1) {
			int index = 6;
			if (display_conv) {
				index += 1;
			}
			if (display_functions) {
				index += 6;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getDimXOut()
{
	int rval = (int)_tstof((LPCTSTR)dimxout);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv_dimensions1) {
			if (display_functions) {
				if ((getLayerType() == _T("Hidden")) || (getLayerType() == _T("Output"))) {
					int index = 14;
					if (display_conv) {
						index += 1;
					}
					if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
						rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
					}
				}
			}
		}
	}
	//CString s = _T(""); s.Format(_T("getDimXOut = %i"), rval); addErrorString(s);
	return(rval);
}

int CPropertiesWnd::getDimYOut()
{
	int rval = (int)_tstof((LPCTSTR)dimyout);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_conv_dimensions1) {
			if (display_functions) {
				if ((getLayerType() == _T("Hidden")) || (getLayerType() == _T("Output"))) {
					int index = 15;
					if (display_conv) {
						index += 1;
					}
					if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
						rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
					}
				}
			}
		}
	}
	//CString s = _T(""); s.Format(_T("getDimYOut = %i"), rval); addErrorString(s);
	return(rval);
}

int CPropertiesWnd::getKDimension()
{
	int rval = (int)_tstof((LPCTSTR)kdimension);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_matrix_dimensions && display_tranformer_dimensions) {
			int index = 1;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getVDimension()
{
	int rval = (int)_tstof((LPCTSTR)kdimension);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_matrix_dimensions && display_tranformer_dimensions) {
			int index = 2;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
		}
	}
	return(rval);
}

int CPropertiesWnd::getModelDimension()
{
	int rval = (int)_tstof((LPCTSTR)modeldimension);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_matrix_dimensions || display_tranformer_dimensions) {
			int index = 11;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
			else {
				index = 3;
				if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
					rval = (int)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
				}
			}
		}
	}
	return(rval);
}

double CPropertiesWnd::getSumAddLimit()
{
	double rval = (double)_tstof((LPCTSTR)sumaddlimit);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_matrix_dimensions) {
			int index = 1;
			if (display_tranformer_dimensions) {
				index += 3;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (double)_tstof((LPCTSTR)(CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue());
			}
		}
	}
	return(rval);
}

bool CPropertiesWnd::getHasSoftmax()
{
	bool rval = (bool)_tstof((LPCTSTR)hasSoftmax);
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_matrix_dimensions) {
			int index = 2;
			if (display_tranformer_dimensions) {
				index += 4;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				CString tval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
				if (tval == _T("No")) {
					rval = false;
				}
				else {
					rval = true;
				}
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getTransformerFunction()
{
	CString rval = tfuncn;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (display_matrix_dimensions) {
			int index = 3;
			if (display_tranformer_dimensions) {
				index += 4;
			}
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getInputImageWidth()
{
	CString rval = image_input_width;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (allow_image_input_size_edit) {
			int index = 3;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

CString CPropertiesWnd::getInputImageHeight()
{
	CString rval = image_input_height;
	if (m_wndPropList.GetPropertyCount() > 0) {
		if (allow_image_input_size_edit) {
			int index = 4;
			if (m_wndPropList.GetProperty(0)->GetSubItemsCount() >= index) {
				rval = (CString)m_wndPropList.GetProperty(0)->GetSubItem(index)->GetValue();
			}
		}
	}
	return(rval);
}

HBRUSH CPropertiesWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDockablePane::OnCtlColor(pDC, pWnd, nCtlColor);

	hbr = mbrush;

	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		pDC->SetTextColor(RGB(255, 0, 0)); //GetGlobalData()->clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_BTN:
		pDC->SetTextColor(RGB(255, 0, 0));
		break;
	case CTLCOLOR_DLG:
		pDC->SetTextColor(RGB(255, 255, 0));
		break;
	case CTLCOLOR_EDIT:
		hbr = mbrush2;
		pDC->SetTextColor(RGB(255, 0, 0)); //GetGlobalData()->clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_LISTBOX:
		hbr = mbrush2;
		pDC->SetTextColor(RGB(255, 0, 0)); //GetGlobalData()->clrGrayedText);
		//pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_MSGBOX:
		pDC->SetTextColor(RGB(255, 0, 255));
		break;
	case CTLCOLOR_SCROLLBAR:
		pDC->SetTextColor(RGB(0, 0, 255));
		break;
	default:
		pDC->SetTextColor(RGB(255, 0, 0)); //GetGlobalData()->clrGrayedText);
	}

	return hbr;
}


