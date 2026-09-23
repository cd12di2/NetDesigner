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

// NetDesignerDoc.cpp : implementation of the CNetDesignerDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "NetDesigner.h"
#endif

#include "NetDesignerDoc.h"
#include "MainFrm.h"
#include "NetDesignerView.h"
#include "AuxFunctions.h"
#include "NetVerify.h"
#include "CStatus.h"
#include "WaveFile.h"
#include "CLayerDataDlg.h"
#include "CJSON.h"
#include "CDataOperations.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNetDesignerDoc

IMPLEMENT_DYNCREATE(CNetDesignerDoc, CDocument)

BEGIN_MESSAGE_MAP(CNetDesignerDoc, CDocument)
END_MESSAGE_MAP()


// *********************************************************************************************************************************************
// **  CNetDesignerDoc construction/destruction  ***********************************************************************************************
// *********************************************************************************************************************************************

CNetDesignerDoc::CNetDesignerDoc() noexcept
{

	node_limit = 100;
	num_file_limit = 100;
	num_data_limit = 10000;
	flimit_already = false;
	dlimit_already = false;

	first = true;
	has_convolution = false;
	has_image_conv = false;
	has_series_conv = false;
	has_wave_conv = false;
	has_text_conv = false;
	new_net = true;
	changed_net = true;
	net_ready = false;
	snap_spacing = _T("None");
	guide_spacing = _T("None");
	arrows = false;
	metalNodes = false;
	dataSourceTrue = false;
	input_data_changed = true;
	display_conv_images = true;
	select_folder = false;
	nodeview_message_displayed = false;
	cartoon = false;

	auto_save = false;
	train_response = false;
	validation_response = false;
	delay_epochs1 = 100;
	delay_epochs2 = 5;
	delta_epochs = 0;
	previous_archive_error = 1000;

	last_node_index = -1;
	last_connect_index = -1;
	last_data_index = -1;
	last_conv_index = -1;
	last_attention_index = -1;
	last_assembly_index = -1;
	last_net_cluster_index = -1;
	last_dropout_index = -1;
	nradius = 8;// 10;
	nradius2 = 6;
	lwidth = 2.0;
	dwidth = 72;
	dtype = _T("Output");;
	dfuncn = _T("Sigmoid");
	deta = 0.002;
	dalpha = 0.80;
	drand = 0.80;
	dclayers = 1;
	dcnodesperlayer = 16;
	dcupdateepochs = 5.0;
	parallel_processing_test = 100;
	max_series_inputs = 4;

	dkdimension = 4;
	dvdimension = 4;
	dmodeldimension = 4;
	dmsumaddlimit = 0.20;
	dhasSoftmax = true;

	show_connect = true;

	default_image_width = 100;
	default_image_height = 67;
	image_width = default_image_width;
	image_height = default_image_height;
	gray_images = false;
	conv_type = CONV_FULLY;
	conv_data_type = DATA_CONV;

	minimum_sound_level = 20;
	sound_data_process_type = NONE;
	normaiize_file_set = false;
	wresults.files_shortest = 0;
	wresults.files_longest = 0;
	wresults.files_min = 0;
	wresults.files_max = 0;
	wresults.wave_stride = 0;
	wresults.wave_paths.clear();
	wresults.wave_data_lengths.clear();
	wresults.num_copy_iterations.clear();

	word_count = 0;
	sub_sample_control.clear();
	sub_sample_indices.clear();
	//textDictionary.clear();
	textWords.clear();

	//dictionary_type = HUFFMAN;
	sub_sample = false;

	randomize_data = true;

	maxEpochs = 100000;
	minEpochs = 1000;
	trainTime = 12;
	targetError = 0.010F;
	targetDeltaError = 70;
	updateRate = 10;

	sigmoid_leakage = 0.01;
	relu_leakage = 0.001;

	errorfilter = 10000;

	use_validation_percent = false;
	validation_percent = 20;

	use_validation_file = false;
	//validationPathnames.push_back(_T(""));

	train_data_display_index = 0;

	save_sec = 240;
	save_sec2 = 300;
	relative_min_epoch_count = minEpochs;
	error_delta_count = 0;
	error_rising_last = 0;
	training = false;
	verifying = false;
	evaluate_inputs = false;
	optimize_train = false;
	optimize_validation = true;
	randomize_net = true;
	error_rising_sum = 0;
	best_epochs = 0.0;
	best_error = -1.0;
	best_validation_epochs = 0.0;
	best_validation_error = -1.0;

	assembly_edit_index = -1;
	no_sub_assembly_edit_index = -1;
	assembly_editable = false;
	group_zoom_index = 0;
	for (int i = 0; i < group_zoom_index_max; i++) {
		group_zoom_indices[i] = -1;
	}

	forward_direction = RIGHT;

	input_image_file_extension_valid = false;
	input_image_file_extension = _T("");
	defaultCsvJpgPathname = _T("");

	convConnections.clear();

	dataPathnames.clear();
	validationPathnames.clear();		
	combinedPathnames.clear();
	processedCombinedPathnames.clear();

	clustering = false;
	cprocess = 0;
	sortIndex = 0;
	cluster_inc = 10;
	cluster_iteration = 0;
	update_cluster_inc = 10;
	num_cluster_groups = 0;
	clusterDataColumnIndices.clear();
	clusterGroupIndices.clear();
	clusterSumCounts.clear();
	previousClusterSumCounts.clear();
	clusterSums.clear();
	clusterSumSquares.clear();
	clusterDataIndices.clear();
	clusterMeans.clear();
	clustergraphxdata.clear();
	clustergraphydata.clear();

	selectedComponents.clear();

	netInputs.clear();
	netOutputs.clear();

	assembly_input_output_counts.clear();

	rand_seed = 0;

	int seed = (int)time(NULL);
	srand(seed);


}

CNetDesignerDoc::~CNetDesignerDoc()
{
	deallocate();
}


void CNetDesignerDoc::deallocate()
{
	addPanel1String(_T("deallocating nnet.clearAndDeallocate"));
	nnet.clearAndDeallocate();
}

BOOL CNetDesignerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	initData();
	setNewNet();

	assembly_edit_index = -1;
	no_sub_assembly_edit_index = -1;
	assembly_editable = false;
	group_zoom_index = 0;
	for (int i = 0; i < group_zoom_index_max; i++) {
		group_zoom_indices[i] = -1;
	}

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.clear();

	if (training || clustering || verifying) {
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->OnStopTrain();
	}

	UpdateAllViews(NULL);

	//int index = nnet.getNoSubAssemblyIndex("Assembly1");

	return TRUE;
}

BOOL CNetDesignerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	assembly_edit_index = -1;
	no_sub_assembly_edit_index = -1;
	assembly_editable = false;
	group_zoom_index = 0;
	for (int i = 0; i < group_zoom_index_max; i++) {
		group_zoom_indices[i] = -1;
	}

	UpdateAllViews(NULL);

	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();

	if (automationDlg.displayed) {
		automationDlg.DestroyDlg();
	}

	if (training || clustering || verifying) {
		pView->OnStopTrain();
	}

	pView->display = false;

	flimit_already = false;
	dlimit_already = false;

	hasConvolution();

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	//pMainWnd->m_wndOutput.clear();
	//pView->OnClear();
	pMainWnd->m_wndGraph.reset();
	pMainWnd->m_wndClusterGraph.reset(_T(""));

	CString data = _T("");
	data.Format(_T("%.0lf"), maxEpochs);
	pMainWnd->setEpochs(data);
	data.Format(_T("%.0lf"), minEpochs);
	pMainWnd->setMinEpochs(data);
	data.Format(_T("%.1lf"), trainTime);
	pMainWnd->setTrainTime(data);
	data.Format(_T("%.5lf"), targetError);
	pMainWnd->setTargetError(data);
	data.Format(_T("%.0lf"), targetDeltaError);
	pMainWnd->setTargetDeltaError(data);
	data.Format(_T("%.0lf"), updateRate);
	pMainWnd->setUpdateRate(data);

	setNewNet();

	getDataImageWidth();
	getDataImageHeight();
	getDataImageGray();

	updateDataPane();

	pView->processDlg.DestroyDlg();
	pView->clusterdlg.DestroyDlg();

	nodeview_message_displayed = false;

	return TRUE;
}

BOOL CNetDesignerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	return CDocument::OnSaveDocument(lpszPathName);
}

BOOL CNetDesignerDoc::SaveModified()
{
	return CDocument::SaveModified();
}



// *********************************************************************************************************************************************
// **  CNetDesignerDoc serialization  **********************************************************************************************************
// *********************************************************************************************************************************************

void CNetDesignerDoc::Serialize(CArchive& ar)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.clear();

	bool check_only = true;
	if (ar.IsStoring()) {
		CJSON myjson;
		netData.Serialize(&myjson, true);
		CAuxFunctions func;
		CString myfilename = func.removeExtensionFromFilename(GetPathName()) + _T(".json");
		myjson.saveFile(string(CT2CA(myfilename)));
	}
	else {
		CJSON myjson;
		CAuxFunctions func;
		CString myfilename = func.removeExtensionFromFilename(ar.m_strFileName) + _T(".json");
		if (myjson.loadFile(string(CT2CA(myfilename)))) {
			if (!netData.Serialize(&myjson, false)) {
				addErrorString(_T("Unable to process file ") + myfilename + _T(" - reverting to nnd file"));
				check_only = false;
			}
		}
		else {
			addErrorString(_T("Unable to load file ") + myfilename + _T(" - reverting to nnd file"));
			check_only = false;
		}
	}

#ifndef GCC
	CString first = netData.Serialize(ar, check_only);

	if (ar.IsStoring())
	{
		CString save_buffer = _T("");
		save_buffer.Format(_T("%i"), (int)use_validation_percent);
		ar << save_buffer;
		save_buffer.Format(_T("%f"), validation_percent);
		ar << save_buffer;
		save_buffer.Format(_T("%i"), (int)use_validation_file);
		ar << save_buffer;
		ar << (int)validationPathnames.size();
		for (int i = 0; i < validationPathnames.size(); i++) {
			ar << validationPathnames[i];
		}
		save_buffer.Format(_T("%i"), (int)rand_seed);
		ar << save_buffer;

		ar << maxEpochs;
		ar << minEpochs;
		ar << trainTime;
		ar << targetError;
		ar << targetDeltaError;
		ar << updateRate;
		ar << errorfilter;

		ar << randomize_data;

		ar << conv_type;

		ar << dictionary_type;
		ar << sub_sample;

		ar << minimum_sound_level;
		ar << normaiize_file_set;

		ar << defaultCsvJpgPathname;

		ar << auto_save;
		ar << train_response;
		ar << validation_response;
		ar << delay_epochs1;
		ar << delay_epochs2;

	}
	else
	{
		CString save_buffer = _T("");
		if (first != _T("")) {
			save_buffer = first;
			use_validation_percent = (bool)_tstof((LPCTSTR)save_buffer);
		}
		else {
			if (!ar.IsBufferEmpty()) {
				ar >> save_buffer;
				use_validation_percent = (bool)_tstof((LPCTSTR)save_buffer);
			}
		}
		if (!ar.IsBufferEmpty()) {
			ar >> save_buffer;
			validation_percent = (float)_tstof((LPCTSTR)save_buffer);
		}
		if (!ar.IsBufferEmpty()) {
			ar >> save_buffer;
			use_validation_file = (bool)_tstof((LPCTSTR)save_buffer);
		}

		int nvalid = 0;
		if (!ar.IsBufferEmpty()) {
			ar >> nvalid;
		}
		if (nvalid > 0) {
			CString sbuf = _T("");
			validationPathnames.clear();
			for (int i = 0; i < nvalid; i++) {
				if (!ar.IsBufferEmpty()) {
					ar >> sbuf;
					validationPathnames.push_back(sbuf);
				}
			}
		}

		if (!ar.IsBufferEmpty()) {
			ar >> save_buffer;
			rand_seed = (int)_tstof((LPCTSTR)save_buffer);
		}

		if (!ar.IsBufferEmpty()) {
			ar >> maxEpochs;
		}
		if (!ar.IsBufferEmpty()) {
			ar >> minEpochs;
		}
		if (!ar.IsBufferEmpty()) {
			ar >> trainTime;
		}
		if (!ar.IsBufferEmpty()) {
			ar >> targetError;
		}
		if (!ar.IsBufferEmpty()) {
			ar >> targetDeltaError;
		}
		if (!ar.IsBufferEmpty()) {
			ar >> updateRate;
		}
		if (!ar.IsBufferEmpty()) {
			ar >> errorfilter;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> randomize_data;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> conv_type;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> dictionary_type;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> sub_sample;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> minimum_sound_level;
		}
		if (!ar.IsBufferEmpty()) {
			ar >> normaiize_file_set;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> defaultCsvJpgPathname;
		}
		
		if (!ar.IsBufferEmpty()) {
			ar >> auto_save;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> train_response;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> validation_response;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> delay_epochs1;
		}

		if (!ar.IsBufferEmpty()) {
			ar >> delay_epochs2;
		}

	}
#endif

}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CNetDesignerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CNetDesignerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CNetDesignerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CNetDesignerDoc diagnostics

#ifdef _DEBUG
void CNetDesignerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNetDesignerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG




// *********************************************************************************************************************************************
// **  CNetDesignerDoc editor initialization  **************************************************************************************************
// *********************************************************************************************************************************************

void CNetDesignerDoc::setNewNet()
{
	new_net = true;
	checkDataSource();
}

void CNetDesignerDoc::DeleteContents()
{
	initData1();
	CDocument::DeleteContents();
}

void CNetDesignerDoc::initData()
{
	initData1();
	if (first) {
		initData2();
		first = false;
	} else {
		updateDataPane();
	}
}

void CNetDesignerDoc::initData1()
{
	flimit_already = false;
	dlimit_already = false;
	last_node_index = -1;
	last_connect_index = -1;
	graph_update_cnt = 0;
	graph_update_inc = 1;
	graph_update_cnt2 = 0;
	graph_update_inc2 = 2;
	dataSourceTrue = false;
	input_data_changed = true;
	has_convolution = false;
	net_ready = false;    
	has_image_conv = false;
	has_series_conv = false;
	has_wave_conv = false;
	has_text_conv = false;
	conv_data_type = DATA_CONV;
	minimum_sound_level = 20;
	normaiize_file_set = false;
	nodeview_message_displayed = false;
	sound_data_process_type = NONE;
	wresults.files_shortest = 0;
	wresults.files_longest = 0;
	wresults.files_min = 0;
	wresults.files_max = 0;
	wresults.wave_stride = 0;
	wresults.wave_paths.clear();
	wresults.wave_data_lengths.clear();
	wresults.num_copy_iterations.clear();
	num_cluster_groups = 0;
	forward_direction = RIGHT;
	input_image_file_extension_valid = false;
	input_image_file_extension = _T("");
	defaultCsvJpgPathname = _T("");
	auto_save = false;
	train_response = false;
	validation_response = false;
	evaluate_inputs = false;
	optimize_train = false;
	optimize_validation = true;
	delay_epochs1 = 100;
	delay_epochs2 = 5;
	delta_epochs = 0;
	previous_archive_error = 1000;
	netData.clear();
	editData.clear();
	backData.clear();
	redoData.clear();
	trainDataX.clear();
	trainDataY.clear();
	trainDataY2.clear();
	decoderDataX.clear();
	decoderDataY.clear();
	//trainData.clear();
	selectedComponents.clear();
	dataPathnames.clear();
	validationPathnames.clear();
	combinedPathnames.clear();
	processedCombinedPathnames.clear();
	convConnections.clear();
	word_count = 0;
	sub_sample_control.clear();
	sub_sample_indices.clear();
	//textDictionary.clear();
	textWords.clear();
	clusterDataColumnIndices.clear();
	clusterGroupIndices.clear();
	clusterSumCounts.clear();
	previousClusterSumCounts.clear();
	clusterSums.clear();
	clusterSumSquares.clear();
	clusterDataIndices.clear();
	clusterMeans.clear();
	netInputs.clear();
	netOutputs.clear();
	assembly_input_output_counts.clear();
	nnet.clearAndDeallocate();
	if (!first) {
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->processDlg.DestroyDlg();
		pView->clusterdlg.DestroyDlg();
	}
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndData.show_progress = true;
}

void CNetDesignerDoc::initData2()
{
	CWinApp* pApp = AfxGetApp();

	snap_spacing = pApp->GetProfileString(_T("GuideSettings"), _T("SnapGrid"), _T("None"));
	guide_spacing = pApp->GetProfileString(_T("GuideSettings"), _T("GuideGrid"), _T("None"));
	arrows = pApp->GetProfileIntW(_T("GuideSettings"), _T("Arrows"), 1);
	dfuncn = pApp->GetProfileString(_T("NetDefaults"), _T("Function"), _T("Sigmoid"));
	double tdeta = ((double)pApp->GetProfileIntW(_T("NetDefaults"), _T("Eta"), (int)(1e8 * deta)) / 1e8);
	if (tdeta > 0) {
		deta = tdeta;
	}
	double tdalpha = ((double)pApp->GetProfileIntW(_T("NetDefaults"), _T("Alpha"), (int)(1e6 * dalpha)) / 1e6);
	if (tdalpha > 0) {
		dalpha = tdalpha;
	}
	double tdrand = ((double)pApp->GetProfileIntW(_T("NetDefaults"), _T("Rand"), (int)(1e6 * drand)) / 1e6);
	if (tdrand > 0) {
		drand = tdrand;
	}
	int tdclayers = pApp->GetProfileIntW(_T("NetDefaults"), _T("CLayers"), dclayers);
	if (tdclayers > 0) {
		dclayers = tdclayers;
	}
	int tdcnodesperlayer = pApp->GetProfileIntW(_T("NetDefaults"), _T("CNodesPerLayer"), dcnodesperlayer);
	if (tdcnodesperlayer > 0) {
		dcnodesperlayer = tdcnodesperlayer;
	}
	int tdcupdateepochs = pApp->GetProfileIntW(_T("NetDefaults"), _T("CUpdateEpochs"), (int)dcupdateepochs);
	if (tdcupdateepochs > 0) {
		dcupdateepochs = tdcnodesperlayer;
	}

	metalNodes = (int)pApp->GetProfileIntW(_T("Colors"), _T("Metal"), 1);

	maxEpochs = ((float)pApp->GetProfileIntW(_T("NetDefaults"), _T("MaxEpochs"), (int)maxEpochs));
	minEpochs = ((float)pApp->GetProfileIntW(_T("NetDefaults"), _T("MinEpochs"), (int)minEpochs));
	trainTime = ((float)pApp->GetProfileIntW(_T("NetDefaults"), _T("MinTime"), (int)(100*trainTime)) / 100);
	targetError = ((float)pApp->GetProfileIntW(_T("NetDefaults"), _T("ErrorRate"), (int)(1000000*targetError)) / 1000000);
	targetDeltaError = ((float)pApp->GetProfileIntW(_T("NetDefaults"), _T("DeltaErrorRate"), (int)targetDeltaError));
	updateRate = ((float)pApp->GetProfileIntW(_T("NetDefaults"), _T("UpdateRate"), (int)updateRate));

	sigmoid_leakage = ((double)pApp->GetProfileIntW(_T("NetDefaults"), _T("SLeakage"), (int)(1e6 * sigmoid_leakage)) / 1e6);
	relu_leakage = ((double)pApp->GetProfileIntW(_T("NetDefaults"), _T("RLeakage"), (int)(1e6 * relu_leakage)) / 1e6);

	errorfilter = ((double)pApp->GetProfileIntW(_T("NetDefaults"), _T("ErrorFilter"), (int)errorfilter));

	int ptest = (int)pApp->GetProfileIntW(_T("NetDefaults"), _T("ParallelProcessing"), parallel_processing_test);
	if (ptest > 0) {
		parallel_processing_test = ptest;
	}

	relative_min_epoch_count = minEpochs;
	error_rising_last = 0;

	//SetModifiedFlag(false);

}




// *********************************************************************************************************************************************
// **  CNetDesignerDoc netData access functions  ***********************************************************************************************
// *********************************************************************************************************************************************

CString CNetDesignerDoc::getFirstDataPathName()
{
	CString rval = _T("");
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			dataPathnames.clear();// = netData.getFilePaths(i);
			for (int j = 0; j < netData.getFilePaths(i).size(); j++) {
				dataPathnames.push_back((CString)netData.getFilePaths(i)[j].c_str());
			}
			combinedPathnames.clear();
			if (dataPathnames.size() > 0) {
				rval = dataPathnames[0];
			}
		}
	}
	return(rval);
}

BOOL CNetDesignerDoc::getPathnamesFromNet()
{
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			dataPathnames.clear();// = netData.getFilePaths(i);
			for (int j = 0; j < netData.getFilePaths(i).size(); j++) {
				dataPathnames.push_back((CString)netData.getFilePaths(i)[j].c_str());
			}
			combinedPathnames.clear();
			return(true);
		}
	}
	return(false);
}

void CNetDesignerDoc::setDataPathNames(vector<CString> names)
{
	dataPathnames = names;
	combinedPathnames.clear();
	vector<string> snames;
	for (int i = 0; i < names.size(); i++) {
		snames.push_back(string(CT2CA(names[i])));
	}
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			netData.setFilePaths(i, snames);
		}
	}
}

vector<CString> CNetDesignerDoc::getDataPathNames(bool update)
{
	if (update) {
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isDataSource(i)) {
				//dataPathnames = netData.getFilePaths(i);
				dataPathnames.clear();
				for (int j = 0; j < netData.getFilePaths(i).size(); j++) {
					//addErrorString("getDataPathNames : " + netData.getFilePaths(i)[j]);
					dataPathnames.push_back((CString)netData.getFilePaths(i)[j].c_str());
				}
				combinedPathnames.clear();
				i = (int)netData.size();
			}
		}
	}
	return(dataPathnames);
}

CString CNetDesignerDoc::getDataPathName(int index, bool update)
{
	CString rval = _T("");
	if (update) {
		getDataPathNames(update);
	}
	if (index < dataPathnames.size()) {
		rval = dataPathnames[index];
	}
	return(rval);
}

int CNetDesignerDoc::getDataPathNamesSize(bool update)
{
	if (update) {
		getDataPathNames(update);
	}
	int rval = (int)dataPathnames.size();
	return(rval);
}

bool CNetDesignerDoc::loadFileData()
{
	vector<string> tnames;
	vector<CString> tcnames = getDataPathNames(false);
	for (int j = 0; j < tcnames.size(); j++) {
		//addErrorString(_T("loadFileData : ") + tcnames[j]);
		tnames.push_back(string(CT2CA(tcnames[j])));
	}
	nnet.setDataPathnames(tnames);
	tnames.clear();
	if (nnet.use_validation_files) {
		tcnames = getValidationPathNames();
		for (int j = 0; j < tcnames.size(); j++) {
			tnames.push_back(string(CT2CA(tcnames[j])));
		}
	}
	nnet.setValidationPathnames(tnames);
	return(nnet.loadFileData());
}

vector<CString> CNetDesignerDoc::getDataFields(vector<int> mySelectedComponents)
{
	loadFileData();
	vector<CString> data_fields;
	addPanel1String(_T("Getting data fields"));
	CStatus myProgressDlg;
	myProgressDlg.CreateDlg();
	myProgressDlg.setLabel(_T("Getting data fields . . ."));
	myProgressDlg.setProgress(0);
	if (nnet.csvFileData.size() > 0) {
		for (int i = 0; i < (int)nnet.csvFileData.size(); i++) {
			if (nnet.csvFileData[i].set_data.size() > 0) {
				for (int j = 0; j < (int)nnet.csvFileData[i].set_data.size(); j++) {
					CString ts = (CString)nnet.csvFileData[i].set_data[j].header.c_str();
					if (data_fields.size() > 0) {
						bool found = false;
						for (int k = 0; k < (int)data_fields.size(); k++) {
							if (ts == data_fields[k]) {
								found = true;
							}
						}
						if (!found) {
							data_fields.push_back(ts);
						}
					}
					else {
						data_fields.push_back(ts);
					}
				}
			}
			myProgressDlg.setProgress((i * 100) / (int)nnet.csvFileData.size());
		}
	}
	if ((getDataPathType() != FILE_TYPE_JPG) && (getDataPathType() != FILE_TYPE_WAV)) {
		bool all_inputs = true;
		for (int i = 0; i < mySelectedComponents.size(); i++) {
			if (netData.isNode(mySelectedComponents[i])) {
				if (netData.getNodeMode(mySelectedComponents[i]) != "Input") {
					all_inputs = false;
					i = (int)mySelectedComponents.size();
				}
			}
		}
		if (all_inputs) {
			for (int i = 0; i < netData.size(); i++) {
				if (netData.isNode(i)) {
					if (netData.getNodeMode(i) == "Output") {
						CString label = _T("Output:") + (CString)netData.getID(i).c_str();
						data_fields.push_back(label);
					}
				}
			}
		}
	}
	myProgressDlg.DestroyDlg();
	addPanel1String(_T(""));
	return(data_fields);
}

void CNetDesignerDoc::setValidationPathNames(vector<CString> names)
{
	validationPathnames = names;
}

vector<CString> CNetDesignerDoc::getValidationPathNames()
{
	return(validationPathnames);
}

CString CNetDesignerDoc::getValidationPathName(int index)
{
	CString rval = _T("");
	if (index < validationPathnames.size()) {
		rval = validationPathnames[index];
	}
	return(rval);
}

int CNetDesignerDoc::getValidationPathNamesSize()
{
	int rval = (int)validationPathnames.size();
	return(rval);
}

void CNetDesignerDoc::pushbackValidationPathName(CString pname)
{
	validationPathnames.push_back(pname);
}

void CNetDesignerDoc::clearValidationPathNames()
{
	validationPathnames.clear();
}

int CNetDesignerDoc::getDataPathType()
{
	return(getDataPathType(dataPathnames));
}

CString CNetDesignerDoc::getDefaultCSVPathname()
{
	CString first = getFirstDataPathName();
	if (first != _T("")) {
		CAuxFunctions func;
		CString ext = (func.getFileExtension(first)).MakeLower();
		if (ext == _T("csv")) {
			// this seems more effort than its worth
			// you would have to scan each csv file when opened
			// and pick one to use.  without this, though, no
			// image is displayed in any image input boxes
			// until training is started
		}
	}
	return(defaultCsvJpgPathname);
}

int CNetDesignerDoc::getDataPathType(vector<CString> mpaths)
{
	if (mpaths.size() > 0) {
		CAuxFunctions func;
		CString ext = func.getFileExtension(mpaths[0]);
		ext = ext.MakeUpper();
		if (ext == _T("CSV")) {
			return(FILE_TYPE_CSV);
		}
		else if (ext == _T("JPG")) {
			return(FILE_TYPE_JPG);
		}
		else if (ext == _T("WAV")) {
			return(FILE_TYPE_WAV);
		}
		else if (ext == _T("TXT")) {
			return(FILE_TYPE_TXT);
		}
		else if (ext == _T("RAW")) {
			return(FILE_TYPE_RAW);
		}
	}
	return(FILE_TYPE_NONE);
}

/*int CNetDesignerDoc::getImageWidth()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getDataImageWidth(index));
	}
	return(0);
}

int CNetDesignerDoc::getImageHeight()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getDataImageHeight(index));
	}
	return(0);
}*/

int CNetDesignerDoc::getMinimumImageWidth()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getFileMinImageWidth(index));
	}
	return(0);
}

float CNetDesignerDoc::getImageAspectRatio()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getFileImageAspect(index));
	}
	return(0.0F);
}

int CNetDesignerDoc::getDataWavMax()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getDataWavMax(index));
	}
	return(0);
}

void CNetDesignerDoc::setDataWavMax(int mmax)
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		if (mmax != netData.getDataWavMax(index)) {
			netData.setDataWavMax(index, mmax);
			SetModifiedFlag();
		}
	}
}

int CNetDesignerDoc::getDataWavMin()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getDataWavMin(index));
	}
	return(0);
}

void CNetDesignerDoc::setDataWavMin(int mmin)
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		if (mmin != netData.getDataWavMin(index)) {
			netData.setDataWavMin(index, mmin);
			SetModifiedFlag();
		}
	}
}

int CNetDesignerDoc::getDataWavShortest()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getDataWavShortest(index));
	}
	return(0);
}

void CNetDesignerDoc::setDataWavShortest(int mshortest)
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		if (mshortest != netData.getDataWavShortest(index)) {
			netData.setDataWavShortest(index, mshortest);
			SetModifiedFlag();
		}
	}
}

int CNetDesignerDoc::getDataWavLongest()
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		return(netData.getDataWavLongest(index));
	}
	return(0);
}

void CNetDesignerDoc::setDataWavLongest(int mlongest)
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	if (index >= 0) {
		if (mlongest != netData.getDataWavLongest(index)) {
			netData.setDataWavLongest(index, mlongest);
			SetModifiedFlag();
		}
	}
}

void CNetDesignerDoc::updateDataPane()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndData.updateFromNet();
}

void CNetDesignerDoc::setDataImageWidth(int width)
{
	//CString s = _T(""); s.Format(_T("setDataImageWidth : image_width = %i"), image_width); addErrorString(s);
	image_width = width;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			netData.setDataImageWidth(i, width);
		}
	}
}

int CNetDesignerDoc::getDataImageWidth()
{
	CString pathext = _T("");
	if (dataPathnames.size() > 0) {
		CAuxFunctions func;
		pathext = func.getFileExtension(dataPathnames[0]).MakeLower();
	}
	CString imageext = getImageFileExtension();
	if (((imageext == _T("jpg")) || (imageext == _T("raw"))) && (pathext == _T("csv"))) {
		image_width = 100;
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isDataSource(i)) {
				netData.setDataImageWidth(i, image_width);
				i = netData.size();
			}
		}
		//CString s = _T(""); s.Format(_T("getDataImageWidth (3) : image_width = %i"), image_width); addErrorString(s);
		return(image_width);
	}
	else {
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isDataSource(i)) {
				image_width = netData.getDataImageWidth(i);
				//CString s = _T(""); s.Format(_T("getDataImageWidth (2) : image_width = %i"), image_width); addErrorString(s);
				return(image_width);
			}
		}
	}
	//CString s = _T(""); s.Format(_T("getDataImageWidth (1) : image_width = %i"), image_width); addErrorString(s);
	return(image_width);
}

void CNetDesignerDoc::setDataImageHeight(int height)
{
	image_height = height;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			netData.setDataImageHeight(i, height);
		}
	}
}

int CNetDesignerDoc::getDataImageHeight()
{
	CString pathext = _T("");
	if (dataPathnames.size() > 0) {
		CAuxFunctions func;
		pathext = func.getFileExtension(dataPathnames[0]).MakeLower();
	}
	CString imageext = getImageFileExtension();
	if (((imageext == _T("raw"))) && (pathext == _T("csv"))) {
		int rawwidth = -1;
		int rawheight = -1;
		for (int i = 0; i < (int)netData.size(); i++) {
			if (netData.isDataSource(i)) {
				rawwidth = netData.getRawImageWidth(i);
				rawheight = netData.getRawImageHeight(i);
				i = (int)netData.size();
			}
		}
		if ((rawwidth > 0) && (rawheight > 0)) {
			image_width = 100;
			image_height = image_width * rawheight / rawwidth;
		}
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isDataSource(i)) {
				netData.setDataImageWidth(i, image_width);
				netData.setDataImageHeight(i, image_height);
				i = netData.size();
			}
		}
		//CString s = _T(""); s.Format(_T("getDataImageHeight : image_height = %i"), image_height); addErrorString(s);
		return(image_height);
	}
	else {
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isDataSource(i)) {
				image_height = netData.getDataImageHeight(i);
				return(image_height);
			}
		}
		return(image_height);
	}
	return(image_height);
}

void CNetDesignerDoc::setDataImageGray(bool gray)
{
	gray_images = gray;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			netData.setDataImageGray(i, gray);
		}
	}
}

bool CNetDesignerDoc::getDataImageGray()
{
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			gray_images = netData.getDataImageGray(i);
			return(gray_images);
		}
	}
	return(gray_images);
}

void CNetDesignerDoc::setDataConvType(BOOL type1, BOOL type2, BOOL type3)
{
	if (type1) {
		setDataConvType(CONV_FULLY);
	}
	else if (type2) {
		setDataConvType(CONV_SEPARATED);
	}
	else if (type3) {
		setDataConvType(CONV_SHARED);
	}
	else {
		setDataConvType(CONV_GENERIC);
	}
}

void CNetDesignerDoc::setDataConvType(int type)
{
	conv_type = type;
	CAuxFunctions func;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			int nodes = func.getNodesPerLayer(i);
			netData.setConvNodesPerLayer(i, nodes);
		}
	}
}

int CNetDesignerDoc::getDataConvType()
{
	return(conv_type);
}

int CNetDesignerDoc::getOutputImageSize()
{
	getDataImageWidth();
	getDataImageHeight();
	int imagewidth = image_width;
	int imageheight = image_height;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			imagewidth = netData.getDataImageWidth(i);
			imageheight = netData.getDataImageHeight(i);
		}
	}
	int color = 3;
	if (gray_images) {
		color = 1;
	}
	return(color * imagewidth * imageheight);
}

CString CNetDesignerDoc::getNewNodeName()
{
	CString rval = _T("");
	rval.Format(_T("Node%i"), getLastNodeIndex() + 1);
	return rval;
}

CString CNetDesignerDoc::getNewConnectName()
{
	CString rval = _T("");
	rval.Format(_T("Connect%i"), getLastConnectIndex() + 1);
	return rval;
}

CString CNetDesignerDoc::getNewDataName()
{
	CString rval = _T("");
	rval.Format(_T("Data%i"), getLastDataIndex() + 1);
	return rval;
}

CString CNetDesignerDoc::getNewConvLayerName()
{
	CString rval = _T("");
	//rval.Format(_T("ConvLayer%i"), getLastConvIndex() + 1);
	rval.Format(_T("Layer%i"), getLastConvIndex() + 1);
	return rval;
}

CString CNetDesignerDoc::getNewAttentionLayerName()
{
	CString rval = _T("");
	rval.Format(_T("Attention%i"), getLastAttentionIndex() + 1);
	return rval;
}

CString CNetDesignerDoc::getNewAssemblyName()
{
	CString rval = _T("");
	rval.Format(_T("Assembly%i"), getLastAssemblyIndex() + 1);
	return rval;
}

CString CNetDesignerDoc::getNewClusterName()
{
	CString rval = _T("");
	rval.Format(_T("PatternNet%i"), getLastNetClusterIndex() + 1);
	return rval;
}

CString CNetDesignerDoc::getNewDropoutName()
{
	CString rval = _T("");
	rval.Format(_T("Dropout%i"), getLastDropoutIndex() + 1);
	return rval;
}


int CNetDesignerDoc::checkComponentPlacementReturnIndex(int x, int y, int selection_type, int gindex)
{
	int i_min = -1;
	float r = 0;
	float r_min = 0;
	float min_test = (float)nradius;
	//addErrorString(_T("one"));
	if (((selection_type == SELECT_ALL) || (selection_type == SELECT_NODES))) {
		for (int i = 0; i < netData.size(); i++) {
			if (netData.getGroupIndex(i) == gindex) {
				if (netData.isNode(i)) {
					r = float(pow((x - netData.getXDisp(i)), 2) + pow((y - netData.getYDisp(i)), 2));
					r = sqrt(r);
					if (r < nradius) {
						return(i);
					}
					if (i_min < 0) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
					else if (r < r_min) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
				}
				else if (netData.isConvLayer(i)) {
					CRect crect = getConvRect(getConvParameters(i), 1.0F);
					int tx1 = netData.getXDisp(i) - crect.left - (int)nradius2;
					int tx2 = netData.getXDisp(i) + crect.right - (2 * crect.left) + (int)nradius2;
					int ty1 = netData.getYDisp(i) + crect.top - (int)nradius2;
					int ty2 = netData.getYDisp(i) + crect.bottom + (int)nradius2;
					r = 10000;
					if ((x >= tx1) && (x <= tx2) && (y >= ty1) && (y <= ty2)) {
						return(i);
					}
					if (i_min < 0) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
					else if (r < r_min) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
				}
				else if (netData.isAttentionLayer(i)) {
					CRect crect = getAttentionRect(1.0F);
					int tx1 = netData.getXDisp(i) - crect.left - (int)nradius2;
					int tx2 = netData.getXDisp(i) + crect.right - (2 * crect.left) + (int)nradius2;
					int ty1 = netData.getYDisp(i) + crect.top - (int)nradius2;
					int ty2 = netData.getYDisp(i) + crect.bottom + (int)nradius2;
					r = 10000;
					if ((x >= tx1) && (x <= tx2) && (y >= ty1) && (y <= ty2)) {
						return(i);
					}
					if (i_min < 0) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
					else if (r < r_min) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
				}
				else if (netData.isAssemblyLayer(i)) {
					CRect crect = getAssemblyRect(i, 1.0F);
					int tx1 = netData.getXDisp(i) - crect.left - (int)nradius2;
					int tx2 = netData.getXDisp(i) + crect.right - (2 * crect.left) + (int)nradius2;
					int ty1 = netData.getYDisp(i) + crect.top - (int)nradius2;
					int ty2 = netData.getYDisp(i) + crect.bottom + (int)nradius2;
					r = 10000;
					if ((x >= tx1) && (x <= tx2) && (y >= ty1) && (y <= ty2)) {
						return(i);
					}
					if (i_min < 0) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
					else if (r < r_min) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
				}
				else if (netData.isNetCluster(i)) {
					CRect crect = getNetClusterRect(1.0F);
					int tx1 = netData.getXDisp(i) - crect.left - (int)nradius2;
					int tx2 = netData.getXDisp(i) + crect.right - (2 * crect.left) + (int)nradius2;
					int ty1 = netData.getYDisp(i) + crect.top - (int)nradius2;
					int ty2 = netData.getYDisp(i) + crect.bottom + (int)nradius2;
					r = 10000;
					if ((x >= tx1) && (x <= tx2) && (y >= ty1) && (y <= ty2)) {
						return(i);
					}
					if (i_min < 0) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
					else if (r < r_min) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
				}
				else if (netData.isDropout(i)) {
					CRect crect = getNetClusterRect(1.0F);
					int tx1 = netData.getXDisp(i) - crect.left - (int)nradius2;
					int tx2 = netData.getXDisp(i) + crect.right - (2 * crect.left) + (int)nradius2;
					int ty1 = netData.getYDisp(i) + crect.top - (int)nradius2;
					int ty2 = netData.getYDisp(i) + crect.bottom + (int)nradius2;
					r = 10000;
					if ((x >= tx1) && (x <= tx2) && (y >= ty1) && (y <= ty2)) {
						return(i);
					}
					if (i_min < 0) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
					else if (r < r_min) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
				}
			}
		}
	}
	//addErrorString(_T("two"));
	if ((selection_type == SELECT_ALL) || (selection_type == SELECT_CONNECTIONS)) {
		for (int i = 0; i < netData.size(); i++) {
			if (netData.getGroupIndex(i) == gindex) {
				if (netData.isConnect(i)) {
					CPoint pt1 = getInputNodePoint(i);
					CPoint pt2 = getOutputNodePoint(i);
					r = getDistanceToLine(CPoint(x, y), pt1, pt2);
					if (i_min < 0) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
					else if (r < r_min) {
						r_min = r;
						i_min = i;
						min_test = (float)nradius;
					}
				}
			}
		}
	}
	//addErrorString(_T("three"));
	if (r_min < min_test) {
		return(i_min);
	}
	return(-1);
}

placement_data CNetDesignerDoc::checkComponentPlacementReturnString(int x, int y, int selection_type, bool start, int gindex)
{
	CAuxFunctions func;
	placement_data rval;
	rval.ID = _T("");
	rval.pt = CPoint(0, 0);	
	rval.index = checkComponentPlacementReturnIndex(x, y, selection_type, gindex);
	if (rval.index >= 0) {
		rval.ID = (CString)netData.getID(rval.index).c_str();
		rval.pt.x = netData.getXDisp(rval.index);
		rval.pt.y = netData.getYDisp(rval.index);
		if (netData.isConvLayer(rval.index)) {
			float r = 0;
			int mx = 0;
			int my = 0;
			string type = netData.getNodeMode(rval.index);
			if (start) {
				if ((type == "Input") || (type == "Hidden")) {
					convParameters params = getConvParameters(rval.index);
					CRect brect = getConvRect(params, 1.0F);
					int inputwoffset = getConvInputOffset(brect, 1.0F);
					vector<CPoint> pts = func.getLayerPoints2((int)nradius2, params.type);
					if (pts.size() > 0) {
						mx = netData.getXDisp(rval.index) + pts[0].x + inputwoffset;
						my = netData.getYDisp(rval.index) + pts[0].y;
						r = float(pow((x - mx), 2) + pow((y - my), 2));
						r = sqrt(r);
						if (r < nradius2) {
							rval.ID += _T(":i:0");
							rval.pt.x = mx;
							rval.pt.y = my;
							return(rval);
						}
					}
					bool is_image_conv = isConvImageType(rval.index);
					if (is_image_conv && !gray_images) {
						if (pts.size() > 1) {
							mx = netData.getXDisp(rval.index) + pts[1].x + inputwoffset;
							my = netData.getYDisp(rval.index) + pts[1].y;
							r = float(pow((x - mx), 2) + pow((y - my), 2));
							r = sqrt(r);
							if (r < nradius2) {
								rval.ID += _T(":i:1");
								rval.pt.x = mx;
								rval.pt.y = my;
								return(rval);
							}
						}
						if (pts.size() > 2) {
							mx = netData.getXDisp(rval.index) + pts[2].x + inputwoffset;
							my = netData.getYDisp(rval.index) + pts[2].y;
							r = float(pow((x - mx), 2) + pow((y - my), 2));
							r = sqrt(r);
							if (r < nradius2) {
								rval.ID += _T(":i:2");
								rval.pt.x = mx;
								rval.pt.y = my;
								return(rval);
							}
						}
					}
				}
			}
			else {
				if ((type == "Output") || (type == "Hidden")) {
					vector<CPoint> pts = func.getLayerPoints2((int)nradius2, netData.getConvType(rval.index));
					if (pts.size() > 0) {
						mx = netData.getXDisp(rval.index) + pts[0].x;
						my = netData.getYDisp(rval.index) + pts[0].y;
						r = float(pow((x - mx), 2) + pow((y - my), 2));
						r = sqrt(r);
						if (r < nradius2) {
							//if (!((type == _T("Output")) && (netData.getConvType(rval.index) == DATA_CONV))) {
								rval.ID += _T(":o:0");
							//}
							rval.pt.x = mx;
							rval.pt.y = my;
							return(rval);
						}
					}
					bool is_image_conv = isConvImageType(rval.index);
					if (is_image_conv && !gray_images) {
						if (pts.size() > 1) {
							mx = netData.getXDisp(rval.index) + pts[1].x;
							my = netData.getYDisp(rval.index) + pts[1].y;
							r = float(pow((x - mx), 2) + pow((y - my), 2));
							r = sqrt(r);
							if (r < nradius2) {
								rval.ID += _T(":o:1");
								rval.pt.x = mx;
								rval.pt.y = my;
								return(rval);
							}
						}
						if (pts.size() > 2) {
							mx = netData.getXDisp(rval.index) + pts[2].x;
							my = netData.getYDisp(rval.index) + pts[2].y;
							r = float(pow((x - mx), 2) + pow((y - my), 2));
							r = sqrt(r);
							if (r < nradius2) {
								rval.ID += _T(":o:2");
								rval.pt.x = mx;
								rval.pt.y = my;
								return(rval);
							}
						}
					}
				}
			}
			rval.index = -1;
		}
		else if (netData.isAttentionLayer(rval.index)) {
			float r = 0;
			int mx = 0;
			int my = 0;
			CRect brect = getAttentionRect(1.0F);
			if (start) {
				int inputwoffset = getConvInputOffset(brect, 1.0F);
				mx = netData.getXDisp(rval.index) + inputwoffset;
				my = netData.getYDisp(rval.index);
				r = float(pow((x - mx), 2) + pow((y - my), 2));
				r = sqrt(r);
				if (r < nradius2) {
					rval.ID += _T(":i:0");
					rval.pt.x = mx;
					rval.pt.y = my;
					return(rval);
				}
			}
			else {
				vector<CPoint> pts = func.getAttentionPoints((int)nradius2);
				if (pts.size() > 0) {
					mx = netData.getXDisp(rval.index) + pts[0].x;
					my = netData.getYDisp(rval.index) + pts[0].y;
					r = float(pow((x - mx), 2) + pow((y - my), 2));
					r = sqrt(r);
					if (r < nradius2) {
						rval.ID += _T(":o:0");
						rval.pt.x = mx;
						rval.pt.y = my;
						return(rval);
					}
				}
				if (!netData.isSelfAttentionLayerOnly(rval.index)) {
					if (pts.size() > 1) {
						mx = netData.getXDisp(rval.index) + pts[1].x;
						my = netData.getYDisp(rval.index) + pts[1].y;
						r = float(pow((x - mx), 2) + pow((y - my), 2));
						r = sqrt(r);
						if (r < nradius2) {
							rval.ID += _T(":o:1");
							rval.pt.x = mx;
							rval.pt.y = my;
							return(rval);
						}
					}
					if (pts.size() > 2) {
						mx = netData.getXDisp(rval.index) + pts[2].x;
						my = netData.getYDisp(rval.index) + pts[2].y;
						r = float(pow((x - mx), 2) + pow((y - my), 2));
						r = sqrt(r);
						if (r < nradius2) {
							rval.ID += _T(":o:2");
							rval.pt.x = mx;
							rval.pt.y = my;
							return(rval);
						}
					}
				}
			}
			rval.index = -1;
		}
		else if (netData.isAssemblyLayer(rval.index)) {
			float r = 0;
			int mx = 0;
			int my = 0;
			CRect brect = getAssemblyRect(rval.index, 1.0F);
			if (start) {
				vector<CPoint> pts = getAssemblyOutputPoints(rval.index, (int)nradius2);
				if (pts.size() > 0) {
					for (int k = 0; k < (int)pts.size(); k++) {
						mx = netData.getXDisp(rval.index) + pts[k].x;
						my = netData.getYDisp(rval.index) + pts[k].y;
						r = float(pow((x - mx), 2) + pow((y - my), 2));
						r = sqrt(r);
						if (r < nradius2) {
							CString s5 = _T("");  s5.Format(_T(":i:%i"), k);
							rval.ID += s5;
							rval.pt.x = mx;
							rval.pt.y = my;
							return(rval);
						}
					}
				}
			}
			else {
				vector<CPoint> pts = getAssemblyInputPoints(rval.index, (int)nradius2);
				if (pts.size() > 0) {
					for (int  k= 0; k < (int)pts.size(); k++) {
						mx = netData.getXDisp(rval.index) + pts[k].x;
						my = netData.getYDisp(rval.index) + pts[k].y;
						r = float(pow((x - mx), 2) + pow((y - my), 2));
						r = sqrt(r);
						if (r < nradius2) {
							CString s5 = _T("");  s5.Format(_T(":o:%i"), k);
							rval.ID += s5; // _T(":o:0");
							rval.pt.x = mx;
							rval.pt.y = my;
							return(rval);
						}
					}
				}
			}
			rval.index = -1;
		}
		else if (netData.isNetCluster(rval.index)) {
			float r = 0;
			int mx = 0;
			int my = 0;
			CRect brect = getNetClusterRect(1.0F);
			if (start) {
				if (netData.getNodeMode(rval.index) == "Hidden") {
					int inputwoffset = getConvInputOffset(brect, 1.0F);
					vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
					if (pts.size() > 0) {
						mx = netData.getXDisp(rval.index) + pts[0].x + inputwoffset;
						my = netData.getYDisp(rval.index) + pts[0].y;
						r = float(pow((x - mx), 2) + pow((y - my), 2));
						r = sqrt(r);
						if (r < nradius2) {
							rval.ID += _T(":i:0");
							rval.pt.x = mx;
							rval.pt.y = my;
							//addErrorString(rval.ID);
							return(rval);
						}
					}
				}
			}
			else {
				vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
				if (pts.size() > 0) {
					mx = netData.getXDisp(rval.index) + pts[0].x;
					my = netData.getYDisp(rval.index) + pts[0].y;
					r = float(pow((x - mx), 2) + pow((y - my), 2));
					r = sqrt(r);
					if (r < nradius2) {
						rval.ID += _T(":o:0");
						rval.pt.x = mx;
						rval.pt.y = my;
						//addErrorString(rval.ID);
						return(rval);
					}
				}
			}
			rval.index = -1;
		}
		else if (netData.isDropout(rval.index)) {
			float r = 0;
			int mx = 0;
			int my = 0;
			CRect brect = getNetClusterRect(1.0F);
			if (start) {
				int inputwoffset = getConvInputOffset(brect, 1.0F);
				vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
				if (pts.size() > 0) {
					mx = netData.getXDisp(rval.index) + pts[0].x + inputwoffset;
					my = netData.getYDisp(rval.index) + pts[0].y;
					r = float(pow((x - mx), 2) + pow((y - my), 2));
					r = sqrt(r);
					if (r < nradius2) {
						rval.ID += _T(":i:0");
						rval.pt.x = mx;
						rval.pt.y = my;
						//addErrorString(rval.ID);
						return(rval);
					}
				}
			}
			else {
				vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
				if (pts.size() > 0) {
					mx = netData.getXDisp(rval.index) + pts[0].x;
					my = netData.getYDisp(rval.index) + pts[0].y;
					r = float(pow((x - mx), 2) + pow((y - my), 2));
					r = sqrt(r);
					if (r < nradius2) {
						rval.ID += _T(":o:0");
						rval.pt.x = mx;
						rval.pt.y = my;
						//addErrorString(rval.ID);
						return(rval);
					}
				}
			}
			rval.index = -1;
		}
	}
	return(rval);
}

CPoint CNetDesignerDoc::getInputNodePoint(int index)
{
	CPoint pt(0, 0);
	if (index < netData.size()) {
		if (netData.isConnect(index)) {
			CAuxFunctions func;
			string nname = netData.getInputNode(index);
			for (int i = 0; i < netData.size(); i++) {
				if (netData.isNode(i)) {
					if (nname == netData.getID(i)) {
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isConvLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (input) {
								convParameters params = getConvParameters(i);
								CRect brect = getConvRect(params, 1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								vector<CPoint> pts = func.getLayerPoints2((int)nradius2, params.type);
								if (pts.size() > cnode) {
									pt.x = netData.getXDisp(i) + pts[cnode].x + inputwoffset;
									pt.y = netData.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isAttentionLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (input) {
								CRect brect = getAttentionRect(1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								pt.x = netData.getXDisp(i) + inputwoffset;
								pt.y = netData.getYDisp(i);
								return(pt);
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isAssemblyLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (input) {
								CRect brect = getAssemblyRect(i, 1.0F);
								vector<CPoint> pts = getAssemblyOutputPoints(i, (int)nradius2);
								if (pts.size() > cnode) {
									pt.x = netData.getXDisp(i) + pts[cnode].x;
									pt.y = netData.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isNetCluster(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (input) {
								CRect brect = getNetClusterRect(1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								pt.x = netData.getXDisp(i) + inputwoffset;
								pt.y = netData.getYDisp(i);
								return(pt);
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isDropout(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (input) {
								CRect brect = getNetClusterRect(1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								pt.x = netData.getXDisp(i) + inputwoffset;
								pt.y = netData.getYDisp(i);
								return(pt);
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
			}
		}
	}
	return(CPoint(0, 0));
}

CPoint CNetDesignerDoc::getInputNodePoint(int index, NetData mdata)
{
	CPoint pt(0, 0);
	if (index < mdata.size()) {
		if (mdata.isConnect(index)) {
			CAuxFunctions func;
			CString nname = (CString)mdata.getInputNode(index).c_str();
			for (int i = 0; i < mdata.size(); i++) {
				if (mdata.isNode(i)) {
					if (nname == (CString)mdata.getID(i).c_str()) {
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isConvLayer(i)) {
					CString nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == (CString)mdata.getID(i).c_str()) {
						int cindex = mdata.getIndexFromID(string(CT2CA(nname2)));
						int cnode = func.getConvNodeFromConnectName(nname);
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName(nname2);
							if (input) {
								convParameters params;
								params.type = mdata.getConvType(i);
								params.mode = (CString)mdata.getNodeMode(i).c_str();
								params.function = (CString)mdata.getNodeFunction(i).c_str();
								params.id = (CString)mdata.getID(i).c_str();
								CRect brect = getConvRect(params, 1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								vector<CPoint> pts = func.getLayerPoints2((int)nradius2, params.type);
								if (pts.size() > cnode) {
									pt.x = mdata.getXDisp(i) + pts[cnode].x + inputwoffset;
									pt.y = mdata.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isAttentionLayer(i)) {
					CString nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == (CString)mdata.getID(i).c_str()) {
						int cindex = mdata.getIndexFromID(string(CT2CA(nname2)));
						int cnode = func.getConvNodeFromConnectName(nname);
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName(nname);
							if (input) {
								CRect brect = getAttentionRect(1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								pt.x = mdata.getXDisp(i) + inputwoffset;
								pt.y = mdata.getYDisp(i);
								return(pt);
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isAssemblyLayer(i)) {
					CString nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == (CString)mdata.getID(i).c_str()) {
						int cindex = mdata.getIndexFromID(string(CT2CA(nname2)));
						int cnode = func.getConvNodeFromConnectName(nname);
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName(nname);
							if (input) {
								CRect brect = getAssemblyRect(i, 1.0F);
								vector<CPoint> pts = getAssemblyOutputPoints(i, (int)nradius2);
								if (pts.size() > cnode) {
									pt.x = mdata.getXDisp(i) + pts[cnode].x;
									pt.y = mdata.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isNetCluster(i)) {
					CString nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == (CString)mdata.getID(i).c_str()) {
						int cindex = mdata.getIndexFromID(string(CT2CA(nname2)));
						int cnode = func.getConvNodeFromConnectName(nname);
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName(nname);
							if (input) {
								CRect brect = getNetClusterRect(1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								pt.x = mdata.getXDisp(i) + inputwoffset;
								pt.y = mdata.getYDisp(i);
								return(pt);
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isDropout(i)) {
					CString nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == (CString)mdata.getID(i).c_str()) {
						int cindex = mdata.getIndexFromID(string(CT2CA(nname2)));
						int cnode = func.getConvNodeFromConnectName(nname);
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName(nname);
							if (input) {
								CRect brect = getNetClusterRect(1.0F);
								int inputwoffset = getConvInputOffset(brect, 1.0F);
								pt.x = mdata.getXDisp(i) + inputwoffset;
								pt.y = mdata.getYDisp(i);
								return(pt);
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
			}
		}
	}
	return(CPoint(0, 0));
}

CPoint CNetDesignerDoc::getOutputNodePoint(int index)
{
	CPoint pt(0, 0);
	if (index < netData.size()) {
		if (netData.isConnect(index)) {
			CAuxFunctions func;
			string nname = netData.getOutputNode(index);
			for (int i = 0; i < netData.size(); i++) {
				if (netData.isNode(i)) {
					if (nname == netData.getID(i)) {
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isConvLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								vector<CPoint> pts = func.getLayerPoints2((int)nradius2, netData.getConvType(cindex));
								if (pts.size() > cnode) {
									pt.x = netData.getXDisp(i) + pts[cnode].x;
									pt.y = netData.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isAttentionLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getAttentionRect(1.0F);
								vector<CPoint> pts = func.getAttentionPoints((int)nradius2);
								if (pts.size() > cnode) {
									pt.x = netData.getXDisp(i) + pts[cnode].x;
									pt.y = netData.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isAssemblyLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getAssemblyRect(i, 1.0F);
								vector<CPoint> pts = getAssemblyInputPoints(i, (int)nradius2);
								if (pts.size() > cnode) {
									pt.x = netData.getXDisp(i) + pts[cnode].x;
									pt.y = netData.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isNetCluster(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getNetClusterRect(1.0F);
								vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
								if (pts.size() > cnode) {
									pt.x = netData.getXDisp(i) + pts[cnode].x;
									pt.y = netData.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
				else if (netData.isDropout(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == netData.getID(i)) {
						int cindex = netData.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getNetClusterRect(1.0F);
								vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
								if (pts.size() > cnode) {
									pt.x = netData.getXDisp(i) + pts[cnode].x;
									pt.y = netData.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(netData.getXDisp(i), netData.getYDisp(i)));
					}
				}
			}
		}
	}
	return(CPoint(0, 0));
}

CPoint CNetDesignerDoc::getOutputNodePoint(int index, NetData mdata)
{
	CPoint pt(0, 0);
	if (index < mdata.size()) {
		if (mdata.isConnect(index)) {
			CAuxFunctions func;
			string nname = mdata.getOutputNode(index);
			for (int i = 0; i < mdata.size(); i++) {
				if (mdata.isNode(i)) {
					if (nname == mdata.getID(i)) {
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isConvLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == mdata.getID(i)) {
						int cindex = mdata.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								vector<CPoint> pts = func.getLayerPoints2((int)nradius2, mdata.getConvType(cindex));
								if (pts.size() > cnode) {
									pt.x = mdata.getXDisp(i) + pts[cnode].x;
									pt.y = mdata.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isAttentionLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == mdata.getID(i)) {
						int cindex = mdata.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getAttentionRect(1.0F);
								vector<CPoint> pts = func.getAttentionPoints((int)nradius2);
								if (pts.size() > cnode) {
									pt.x = mdata.getXDisp(i) + pts[cnode].x;
									pt.y = mdata.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isAssemblyLayer(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == mdata.getID(i)) {
						int cindex = mdata.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getAssemblyRect(i, 1.0F);
								vector<CPoint> pts = getAssemblyInputPoints(i, (int)nradius2);
								if (pts.size() > cnode) {
									pt.x = mdata.getXDisp(i) + pts[cnode].x;
									pt.y = mdata.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isNetCluster(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == mdata.getID(i)) {
						int cindex = mdata.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getNetClusterRect(1.0F);
								vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
								if (pts.size() > cnode) {
									pt.x = mdata.getXDisp(i) + pts[cnode].x;
									pt.y = mdata.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
				else if (mdata.isDropout(i)) {
					string nname2 = func.getConvNameFromConnectName(nname);
					if (nname2 == mdata.getID(i)) {
						int cindex = mdata.getIndexFromID(nname2);
						int cnode = func.getConvNodeFromConnectName((CString)nname.c_str());
						if (cnode >= 0) {
							bool input = func.getConvInputFromConnectName((CString)nname.c_str());
							if (!input) {
								CRect brect = getNetClusterRect(1.0F);
								vector<CPoint> pts = func.getNetClusterPoints((int)nradius2);
								if (pts.size() > cnode) {
									pt.x = mdata.getXDisp(i) + pts[cnode].x;
									pt.y = mdata.getYDisp(i) + pts[cnode].y;
									return(pt);
								}
							}
						}
						return(CPoint(mdata.getXDisp(i), mdata.getYDisp(i)));
					}
				}
			}
		}
	}
	return(CPoint(0, 0));
}

bool CNetDesignerDoc::hasConvolution()
{
	has_convolution = false;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			has_convolution = true;
			return(has_convolution);
		}
	}
	return(has_convolution);
}

bool CNetDesignerDoc::alreadyHasInputLayer(int exception)
{
	for (int i = 0; i < netData.size(); i++) {
		if ((netData.isConvLayer(i)) && (netData.getNodeMode(i) == "Input") && (i != exception)) {
			return(true);
		}
	}
	return(false);
}

bool CNetDesignerDoc::alreadyHasOutputLayer(int exception)
{
	for (int i = 0; i < netData.size(); i++) {
		if ((netData.isConvLayer(i)) && (netData.getNodeMode(i) == "Output") && (i != exception)) {
			return(true);
		}
	}
	return(false);
}

bool CNetDesignerDoc::checkInputSeriesLayerCount(int exception)
{
	int count = 0;
	for (int i = 0; i < netData.size(); i++) {
		if ((netData.isConvLayer(i)) && (netData.getNodeMode(i) == "Input") && (netData.getConvType(i) == DATA_CONV) && (i != exception)) {
			count++;
			if (count > max_series_inputs) {
				return(false);
			}
		}
	}
	return(true);
}

bool CNetDesignerDoc::alreadyHasInputImageLayer(int exception)
{
	for (int i = 0; i < netData.size(); i++) {
		if ((netData.isConvLayer(i)) && (netData.getNodeMode(i) == "Input") && (netData.getConvType(i) == IMAGE_CONV) && (i != exception)) {
			return(true);
		}
	}
	return(false);
}

CString CNetDesignerDoc::getImageFileExtension()
{
	if (!input_image_file_extension_valid) {
		input_image_file_extension = _T("");
		if (dataPathnames.size() > 0) {
			CAuxFunctions func;
			CString ext = func.getFileExtension(dataPathnames[0]).MakeLower();
			if (ext == _T("jpg")) {
				input_image_file_extension == ext;
			}
			else if (ext == _T("raw")) {
				input_image_file_extension = ext;
			}
			else if (ext == _T("csv")) {
				CDataOperations dops;
				vector<dataset2> fileData;
				dops.getDataFromCsv(string(CT2CA(dataPathnames[0])), &fileData);
				input_image_file_extension = (CString)dops.checkFilenamesInCsv(&fileData).c_str();
			}
			input_image_file_extension_valid = true;
		}
	}
	return(input_image_file_extension);
}

CPoint CNetDesignerDoc::getConvCenter(int index)
{
	CPoint pt = CPoint(0, 0);
	if (netData.isConvLayer(index)) {
		CRect crect = getConvRect(getConvParameters(index), 1.0F);
		int tx1 = netData.getXDisp(index) - crect.left;
		int tx2 = netData.getXDisp(index) + crect.right;
		int ty1 = netData.getYDisp(index) - crect.top;
		int ty2 = netData.getYDisp(index) + crect.bottom;
		pt.x = (tx1 + tx2) / 2;
		pt.y = (ty1 + ty2) / 2;
	}
	return(pt);
}

CRect CNetDesignerDoc::getConvRect(convParameters params, float scale)
{
	CRect brect;
	int ny = 3;
	int halfheight = (int)((float)(ny * 16) * scale / 2);
	brect.top = -halfheight;
	brect.left = 0;
	brect.bottom = halfheight;
	brect.right = (int)((float)(ny * 16) * scale);
	getDataImageWidth();
	getDataImageHeight();
	//if (netData.isConvLayer(nindex)) {
		if ((params.type == SOUND_CONV) && (params.mode == _T("Hidden"))) {
			CRect brect;
			int ny = 3;
			halfheight = (int)((float)(ny * 16) * scale / 2);
			brect.top = -halfheight;
			brect.left = 0;
			brect.bottom = halfheight;
			brect.right = (int)((float)(ny * 16) * scale);
		}
		else if (params.type == HIDDEN_NET) {
			int nx = 3;// 2;// 3;
			int ny = 7;// 5;
			halfheight = (int)((float)((ny * 8/* + 6*/)/* * nradius2*/) * scale / 2);
			brect.top = -halfheight;
			brect.left = 0;
			brect.right = (int)((float)((nx * 8/* + 5*/)/* * nradius2*/) * scale);
			brect.bottom = halfheight;
		}
		else if ((params.type == DATA_CONV) && ((params.mode == _T("Input")) || (params.mode == _T("Output")))) {
			int nx = 2;
			int ny = 3;
			halfheight = (int)((float)(ny * 16) * scale / 2);
			brect.top = -halfheight;
			brect.left = 0;
			brect.bottom = halfheight;
			brect.right = (int)((float)(nx * 16) * scale);
		}
		else {
			int ny = 3;
			halfheight = (int)((float)(ny * 16) * scale / 2);
			brect.top = -halfheight;
			brect.left = 0;
			brect.bottom = halfheight;
			brect.right = (int)((float)(ny * 16) * scale);
			if (params.type == IMAGE_CONV) {
				getDataImageWidth();
				getDataImageHeight();
				int width = (int)((float)(6 * 12) * scale);
				halfheight = (int)(((float)width * (float)image_height) / (2 * (float)image_width));
				brect.right = brect.left + width;
				brect.top = -halfheight;
				brect.bottom = halfheight;
				//CString s = _T(""); s.Format(_T("getConvRect : image_width = %i : image_height = %i"), image_width, image_height); addErrorString(s);
			}
			else if (params.type == SOUND_CONV) {
				brect.right = 2 * brect.right;
			}
		}
	//}
	//CString s = _T("");
	//s.Format(_T(" : top = %i : bottom = %i"), brect.top, brect.bottom);
	//addErrorString(_T("getConvRect : ") + netData.getID(nindex) + s);
	return(brect);
}

CPoint CNetDesignerDoc::getAttentionCenter(int index)
{
	CPoint pt = CPoint(0, 0);
	if (netData.isAttentionLayer(index)) {
		CRect crect = getAttentionRect(1.0F);
		int tx1 = netData.getXDisp(index) - crect.left;
		int tx2 = netData.getXDisp(index) + crect.right;
		int ty1 = netData.getYDisp(index) - crect.top;
		int ty2 = netData.getYDisp(index) + crect.bottom;
		pt.x = (tx1 + tx2) / 2;
		pt.y = (ty1 + ty2) / 2;
	}
	return(pt);
}

CPoint CNetDesignerDoc::getAssemblyCenter(int index)
{
	CPoint pt = CPoint(0, 0);
	if (netData.isAssemblyLayer(index)) {
		CRect crect = getAssemblyRect(index, 1.0F);
		int tx1 = netData.getXDisp(index) - crect.left;
		int tx2 = netData.getXDisp(index) + crect.right;
		int ty1 = netData.getYDisp(index) - crect.top;
		int ty2 = netData.getYDisp(index) + crect.bottom;
		pt.x = (tx1 + tx2) / 2;
		pt.y = (ty1 + ty2) / 2;
	}
	return(pt);
}

CPoint CNetDesignerDoc::getNetClusterCenter(int index)
{
	CPoint pt = CPoint(0, 0);
	if (netData.isNetCluster(index)) {
		CRect crect = getNetClusterRect(1.0F);
		int tx1 = netData.getXDisp(index) - crect.left;
		int tx2 = netData.getXDisp(index) + crect.right;
		int ty1 = netData.getYDisp(index) - crect.top;
		int ty2 = netData.getYDisp(index) + crect.bottom;
		pt.x = (tx1 + tx2) / 2;
		pt.y = (ty1 + ty2) / 2;
	}
	return(pt);
}

CPoint CNetDesignerDoc::getDropoutCenter(int index)
{
	CPoint pt = CPoint(0, 0);
	if (netData.isDropout(index)) {
		CRect crect = getNetClusterRect(1.0F);
		int tx1 = netData.getXDisp(index) - crect.left;
		int tx2 = netData.getXDisp(index) + crect.right;
		int ty1 = netData.getYDisp(index) - crect.top;
		int ty2 = netData.getYDisp(index) + crect.bottom;
		pt.x = (tx1 + tx2) / 2;
		pt.y = (ty1 + ty2) / 2;
	}
	return(pt);
}

CRect CNetDesignerDoc::getAttentionRect(float scale)
{
	CRect brect;
	int nx = 3;
	int ny = 7;
	int halfheight = (int)((float)((ny * 8)) * scale / 2);
	brect.top = -halfheight;
	brect.left = 0;
	brect.right = (int)((float)((nx * 8)) * scale);
	brect.bottom = halfheight;
	return(brect);
}

CRect CNetDesignerDoc::getAssemblyRect(int nindex, float scale)
{
	CRect brect;
	int nx = 3;
	int ny = 8;
	int nnodes = getNumAssemblyInputs(nindex);
	int noutputs = getNumAssemblyOutputs(nindex);
	if (noutputs > nnodes) {
		nnodes = noutputs;
	}
	if (nnodes > 4) {
		ny += 4;
	}
	else if (nnodes > 3) {
		ny += 2;
	}
	int halfheight = (int)((float)((ny * 8)) * scale / 2);
	brect.top = -halfheight;
	brect.left = 0;
	brect.right = (int)((float)((nx * 8)) * scale);
	brect.bottom = halfheight;
	return(brect);
}

CRect CNetDesignerDoc::getNetClusterRect(float scale)
{
	CRect brect;
	int nx = 3;
	int ny = 7;
	int halfheight = (int)((float)((ny * 8)) * scale / 2);
	brect.top = -halfheight;
	brect.left = 0;
	brect.right = (int)((float)((nx * 8)) * scale);
	brect.bottom = halfheight;
	return(brect);
}

int CNetDesignerDoc::getAssemblyIndex(int nindex)
{
	int rval = -1;
	if (netData.isAssemblyLayer(nindex)) {
		int mindex = nindex;
		if (netData.getAssemblyReference(mindex) != "") {
			for (int i = 0; i < netData.size(); i++) {
				if (netData.isAssemblyLayer(i)) {
					if (netData.getID(i) == netData.getAssemblyReference(mindex)) {
						mindex = i;
						i = netData.size();
					}
				}
			}
		}
		rval = getNoSubAssemblyIndex(mindex);
	}
	return(rval);
}

int CNetDesignerDoc::getNoSubAssemblyIndex(int nindex)
{
	int rval = -1;
	string aname = netData.getID(nindex);
	if (aname.substr(0, 8) == "Assembly") {
		string t3 = aname.substr(8);
		if (netData.isWordNumber(t3)) {
			rval = netData.myStoi(t3);
		}
	}
	return(rval);
}

void CNetDesignerDoc::clearAssemblyInputOutputCounts(int nindex)
{
	if (assembly_input_output_counts.size() > 0) {
		for (int i = 0; i < (int)assembly_input_output_counts.size(); i++) {
			if (netData.getID(nindex) == assembly_input_output_counts[i].assembly_id) {
				assembly_input_output_counts[i].input_count = 0;
				assembly_input_output_counts[i].output_count = 0;
			}
		}
	}
}

int CNetDesignerDoc::getNumAssemblyInputs(int nindex)
{
	int rval = 0;
	if (netData.isAssemblyLayer(nindex)) {
		int aindex = -1;
		if (assembly_input_output_counts.size() > 0) {
			for (int i = 0; i < (int)assembly_input_output_counts.size(); i++) {
				if (netData.getID(nindex) == assembly_input_output_counts[i].assembly_id) {
					if (assembly_input_output_counts[i].input_count > 0) {
						return(assembly_input_output_counts[i].input_count);
					}
					aindex = i;
					i = (int)assembly_input_output_counts.size();
				}
			}
		}
		int asmindex = getAssemblyIndex(nindex);
		for (int i = 0; i < netData.size(); i++) {
			if ((netData.getNodeMode(i) == "Input") && (netData.getGroupIndex(i) == asmindex)) {
				rval++;
			}
		}
		if (aindex >= 0) {
			assembly_input_output_counts[aindex].input_count = rval;
		}
	}
	return(rval);
}

int CNetDesignerDoc::getNumAssemblyOutputs(int nindex)
{
	int rval = 0;
	if (netData.isAssemblyLayer(nindex)) {
		int aindex = -1;
		if (assembly_input_output_counts.size() > 0) {
			for (int i = 0; i < (int)assembly_input_output_counts.size(); i++) {
				if (netData.getID(nindex) == assembly_input_output_counts[i].assembly_id) {
					if (assembly_input_output_counts[i].output_count > 0) {
						return(assembly_input_output_counts[i].output_count);
					}
					aindex = i;
					i = (int)assembly_input_output_counts.size();
				}
			}
		}
		int asmindex = getAssemblyIndex(nindex);
		for (int i = 0; i < netData.size(); i++) {
			if ((netData.getNodeMode(i) == "Output") && (netData.getGroupIndex(i) == asmindex)) {
				rval++;
			}
		}
		if (aindex >= 0) {
			assembly_input_output_counts[aindex].output_count = rval;
		}
	}
	return(rval);
}

vector<CPoint> CNetDesignerDoc::getAssemblyInputPoints(int nindex, int radius)
{
	vector<CPoint> rpoints;
	int num = getNumAssemblyInputs(nindex);
	if (num > 0) {
		if ((num % 2) == 0) {
			for (int i = 0; i < (num / 2); i++) {
				rpoints.push_back(CPoint(0, -(2 + (4 * i)) * radius));
				rpoints.push_back(CPoint(0, (2 + (4 * i)) * radius));
			}
		}
		else {
			rpoints.push_back(CPoint(0, 0));
			for (int i = 0; i < (num / 2); i++) {
				rpoints.push_back(CPoint(0, -4 * (i + 1) * radius));
				rpoints.push_back(CPoint(0, 4 * (i + 1) * radius));
			}
		}
	}
	else {
		rpoints.push_back(CPoint(0, 0));
	}
	return(rpoints);
}

vector<CPoint> CNetDesignerDoc::getAssemblyOutputPoints(int nindex, int radius)
{
	vector<CPoint> rpoints;
	int num = getNumAssemblyOutputs(nindex);
	CRect brect = getAssemblyRect(nindex, 1.0F);
	int inputwoffset = getConvInputOffset(brect, 1.0F);
	if (num > 0) {
		if ((num % 2) == 0) {
			for (int i = 0; i < (num / 2); i++) {
				rpoints.push_back(CPoint(inputwoffset, -(2 + (4 * i)) * radius));
				rpoints.push_back(CPoint(inputwoffset, (2 + (4 * i)) * radius));
			}
		}
		else {
			rpoints.push_back(CPoint(inputwoffset, 0));
			for (int i = 0; i < (num / 2); i++) {
				rpoints.push_back(CPoint(inputwoffset, -4 * (i + 1) * radius));
				rpoints.push_back(CPoint(inputwoffset, 4 * (i + 1) * radius));
			}
		}
	}
	else {
		rpoints.push_back(CPoint(inputwoffset, 0));
	}
	return(rpoints);
}

convParameters CNetDesignerDoc::getConvParameters(int index)
{
	convParameters params;
	params.type = netData.getConvType(index);
	params.mode = (CString)netData.getNodeMode(index).c_str();
	params.function = (CString)netData.getNodeFunction(index).c_str();
	params.id = (CString)netData.getID(index).c_str();
	return(params);
}

bool CNetDesignerDoc::isConvImageType(int cindex)
{
	if (getDataPathType() == FILE_TYPE_JPG) {
		conv_data_type = IMAGE_CONV;
		return(true);
	}
	else if (getDataPathType() == FILE_TYPE_CSV) {
		conv_data_type = netData.getConvType(cindex);
		if (conv_data_type == IMAGE_CONV) {
			return(true);
		}
		conv_data_type = DATA_CONV;
		return(false);
	}
	else if (getDataPathType() == FILE_TYPE_WAV) {
		conv_data_type = SOUND_CONV;
		return(false);
	}
	else if (getDataPathType() == FILE_TYPE_TXT) {
		conv_data_type = TEXT_CONV;
		return(false);
	}
	else if (conv_data_type == DATA_CONV) {
		return(false);
	}
	else if (conv_data_type == IMAGE_CONV) {
		return(true);
	}
	else if (conv_data_type == SOUND_CONV) {
		return(false);
	}
	else if (conv_data_type == TEXT_CONV) {
		return(false);
	}
	conv_data_type = DATA_CONV;
	return(false);
}

bool CNetDesignerDoc::isConvSoundType(int cindex)
{
	if (getDataPathType() == FILE_TYPE_JPG) {
		conv_data_type = IMAGE_CONV;
		return(false);
	}
	else if (getDataPathType() == FILE_TYPE_CSV) {
		conv_data_type = netData.getConvType(cindex);
		if (conv_data_type == FILE_TYPE_WAV) {
			return(true);
		}
		conv_data_type = DATA_CONV;
		return(false);
	}
	else if (getDataPathType() == FILE_TYPE_WAV) {
		conv_data_type = SOUND_CONV;
		return(true);
	}
	else if (getDataPathType() == FILE_TYPE_TXT) {
		conv_data_type = TEXT_CONV;
		return(false);
	}
	else if (conv_data_type == DATA_CONV) {
		return(false);
	}
	else if (conv_data_type == IMAGE_CONV) {
		return(false);
	}
	else if (conv_data_type == SOUND_CONV) {
		return(true);
	}
	else if (conv_data_type == TEXT_CONV) {
		return(false);
	}
	conv_data_type = DATA_CONV;
	return(false);
}

int CNetDesignerDoc::getConvInputOffset(CRect crect, float scale) {
	int inputwoffset = 0;
	if (scale > 0) {
		inputwoffset = (int)((float)(crect.Width()) / scale);
	}
	return(inputwoffset);
}

float CNetDesignerDoc::getDistanceToLine(CPoint pt, CPoint lpt1, CPoint lpt2)
{
	float r = 0;
	float dx = (float)(lpt2.x - lpt1.x);
	float dy = (float)(lpt2.y - lpt1.y);
	float l0 = (float)pow((float)(lpt2.x - lpt1.x), 2) + (float)pow((float)(lpt2.y - lpt1.y), 2);
	float l1 = (float)pow((float)(lpt1.x - pt.x), 2) + (float)pow((float)(lpt1.y - pt.y), 2);
	float l2 = (float)pow((float)(lpt2.x - pt.x), 2) + (float)pow((float)(lpt2.y - pt.y), 2);
	float dnom = (float)sqrt((dy * dy) + (dx * dx));
	float num = (dy * (float)pt.x) - (dx * (float)pt.y) + (float)(lpt2.x * lpt1.y) - (float)(lpt2.y * lpt1.x);
	if (num < 0) {
		num = -1 * num;
	}
	if ((dx == 0) && (dy == 0)) {
		dx = (float)(pt.x - lpt1.x);
		dy = (float)(pt.y - lpt1.y);
		return sqrt((dx * dx) + (dy * dy));
	}
	else if (l1 > l0) {
		return(sqrt(l2));
	}
	else if (l2 > l0) {
		return(sqrt(l1));
	}
	else if (dnom > 0) {
		r = num / dnom;
	} 
	return(r);
}

int CNetDesignerDoc::getLastNodeIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			CString lbl = (CString)netData.getID(i).c_str();
			CString num = lbl.Right(lbl.GetLength() - 4);
			int tval = (int)_tstof((LPCTSTR)num);
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	for (int i = 0; i < redoData.size(); i++) {
		for (int j = 0; j < redoData.editData.size(); j++) {
			if (redoData.isNode(i, j)) {
				string lbl = redoData.getID(i, j);
				string num = lbl.substr(4);
				int tval = (int)_tstof((LPCTSTR)(CString)num.c_str());
				if (tval > rval) {
					rval = tval;
				}
			}
		}
	}
	if (last_node_index < rval) {
		last_node_index = rval;
	}
	else if (rval < last_node_index) {
		rval = last_node_index;
	}
	last_node_index = rval;
	return(rval);
}

int CNetDesignerDoc::getLastConnectIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConnect(i)) {
			CString lbl = (CString)netData.getID(i).c_str();
			CString num = lbl.Right(lbl.GetLength() - 7);
			int tval = (int)_tstof((LPCTSTR)num);
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	for (int i = 0; i < redoData.size(); i++) {
		for (int j = 0; j < redoData.editData.size(); j++) {
			if (redoData.isConnect(i, j)) {
				string lbl = redoData.getID(i, j);
				string num = lbl.substr(7);
				int tval = (int)_tstof((LPCTSTR)(CString)num.c_str());
				if (tval > rval) {
					rval = tval;
				}
			}
		}
	}
	if (last_connect_index < rval) {
		last_connect_index = rval;
	}
	else if (rval < last_connect_index) {
		rval = last_connect_index;
	}
	last_connect_index = rval;
	return(rval);
}

int CNetDesignerDoc::getLastDataIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			CString lbl = (CString)netData.getID(i).c_str();
			CString num = lbl.Right(lbl.GetLength() - 4);
			int tval = (int)_tstof((LPCTSTR)num);
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	if (last_data_index < rval) {
		last_data_index = rval;
	}
	else if (rval < last_data_index) {
		rval = last_data_index;
	}
	last_data_index = rval;
	return(rval);
}

int CNetDesignerDoc::getLastConvIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			int tval = 0;
			CString lbl = (CString)netData.getID(i).c_str();
			if (lbl.Left(5) == _T("Layer")) {
				CString num = lbl.Right(lbl.GetLength() - 5);
				tval = (int)_tstof((LPCTSTR)num);
			}
			else if (lbl.Left(9) == _T("ConvLayer")) {
				CString num = lbl.Right(lbl.GetLength() - 9);
				tval = (int)_tstof((LPCTSTR)num);
			}
			else {
				addErrorString(_T("getLastConvIndex unable to resolve layer type"));
			}
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	if (last_conv_index < rval) {
		last_conv_index = rval;
	}
	else if (rval < last_conv_index) {
		rval = last_conv_index;
	}
	last_conv_index = rval;
	return(rval);
}

int CNetDesignerDoc::getLastAttentionIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isAttentionLayer(i)) {
			int tval = 0;
			CString lbl = (CString)netData.getID(i).c_str();
			if (lbl.Left(9) == _T("Attention")) {
				CString num = lbl.Right(lbl.GetLength() - 9);
				tval = (int)_tstof((LPCTSTR)num);
			}
			else {
				addErrorString(_T("getLastAttentionIndex unable to resolve layer type"));
			}
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	if (last_attention_index < rval) {
		last_attention_index = rval;
	}
	else if (rval < last_attention_index) {
		rval = last_attention_index;
	}
	last_attention_index = rval;
	return(rval);
}

int CNetDesignerDoc::getLastAssemblyIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isAssemblyLayer(i)) {
			int tval = 0;
			CString lbl = (CString)netData.getID(i).c_str();
			if (lbl.Left(8) == _T("Assembly")) {
				CString num = lbl.Right(lbl.GetLength() - 8);
				tval = (int)_tstof((LPCTSTR)num);
			}
			else {
				addErrorString(_T("getLastAssemblyIndex unable to resolve layer type"));
			}
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	if (last_assembly_index < rval) {
		last_assembly_index = rval;
	}
	else if (rval < last_assembly_index) {
		rval = last_assembly_index;
	}
	last_assembly_index = rval;
	return(rval);
}

int CNetDesignerDoc::getLastNetClusterIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNetCluster(i)) {
			int tval = 0;
			CString lbl = (CString)netData.getID(i).c_str();
			if (lbl.Left(10) == _T("NetCluster")) {
				CString num = lbl.Right(lbl.GetLength() - 10);
				tval = (int)_tstof((LPCTSTR)num);
			}
			else {
				addErrorString(_T("getLastNetClusterIndex unable to resolve layer type"));
			}
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	if (last_net_cluster_index < rval) {
		last_net_cluster_index = rval;
	}
	else if (rval < last_net_cluster_index) {
		rval = last_net_cluster_index;
	}
	last_net_cluster_index = rval;
	return(rval);
}

int CNetDesignerDoc::getLastDropoutIndex()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDropout(i)) {
			int tval = 0;
			CString lbl = (CString)netData.getID(i).c_str();
			if (lbl.Left(7) == _T("Dropout")) {
				CString num = lbl.Right(lbl.GetLength() - 7);
				tval = (int)_tstof((LPCTSTR)num);
			}
			else {
				addErrorString(_T("getLastDropoutIndex unable to resolve layer type"));
			}
			if (tval > rval) {
				rval = tval;
			}
		}
	}
	if (last_dropout_index < rval) {
		last_dropout_index = rval;
	}
	else if (rval < last_dropout_index) {
		rval = last_dropout_index;
	}
	last_dropout_index = rval;
	return(rval);
}

bool CNetDesignerDoc::checkDataSource()
{
	dataSourceTrue = false;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			dataSourceTrue = true;
			return(dataSourceTrue);
		}
	}
	return(dataSourceTrue);
}

void CNetDesignerDoc::updateConvNetConnections()
{
	convConnections.clear();
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			if (netData.getNodeMode(i) == "Input") {
				conv_order_struct cos;
				cos.netindex = i;
				cos.prevconvnets.push_back(-1);
				convConnections.push_back(cos);
			}
		}
	}
}

double CNetDesignerDoc::getNodeLowerNormalizationFactor(int index)
{
	double rval = netData.getNodeLowerNormalizationFactor(index);
	if (rval < sigmoid_leakage) {
		rval = sigmoid_leakage;
	}
	else if (rval > 0.40) {
		rval = 0.40;
	}
	return(rval);
}

double CNetDesignerDoc::getNodeUpperNormalizationFactor(int index)
{
	double rval = netData.getNodeUpperNormalizationFactor(index);
	if (rval > (1.0 - sigmoid_leakage)) {
		rval = 1.0 - sigmoid_leakage;
	}
	else if (rval < 0.60) {
		rval = 0.60;
	}
	return(rval);
}

void CNetDesignerDoc::getProjectedMemoryUsage()
{
	long available = 0;
	CString s2 = _T("");
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if (GlobalMemoryStatusEx(&statex)) {
		available = (long)((statex.ullAvailPhys / 1024) / 1024);
		s2.Format(_T("Available memory = %iMB"), available);
		addErrorString(s2);
		unsigned long long physicalMemory = 0;
		if (GetPhysicallyInstalledSystemMemory(&physicalMemory)) {
			long mbytes = (long)(physicalMemory / 1024);
			CString s2 = _T("");
			s2.Format(_T("System memory = %iMB"), mbytes);
			addErrorString(s2);
		}
		addErrorString(_T(""));
	}
	return;
}

bool CNetDesignerDoc::checkAvailableMemory(bool ask)
{
	long available = 0;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if (GlobalMemoryStatusEx(&statex)) {
		available = (long)((statex.ullAvailPhys / 1024) / 1024);
		if (available < 1000) {
			if (ask) {
				CString s = _T("");
				s.Format(_T("Available computer memory down to %iMB - continue?"), available);
				if (AfxMessageBox(s, MB_YESNO) == IDNO) {
					return(false);
				}
			}
			else {
				return(false);
			}
		}
	}
	return(true);
}

void CNetDesignerDoc::showAvailableMemory()
{
	static int mycount = 0;
	long available = 0;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if (GlobalMemoryStatusEx(&statex)) {
		available = (long)((statex.ullAvailPhys / 1024) / 1024);
		CString s = _T("");
		s.Format(_T("Available computer memory down to %iMB : %i"), available, mycount);
		addErrorString(s);
		mycount++;
	}
}

void CNetDesignerDoc::getLayerDataConnections(int nindex)
{
	if (nindex >= 0) {
		if (netData.isConvLayer(nindex) && (getDataConvType() == DATA_CONV) &&
			((netData.getNodeMode(nindex) == "Input") || (netData.getNodeMode(nindex) == "Output"))) {
			CLayerDataDlg ldlg;
			ldlg.setComponentIndex(nindex);
			if (ldlg.DoModal() == IDOK) {

			}
		}
		else {
			addErrorString(_T("Unable to get layer data connections - invalid component type"));
		}
	}
	else {
		addErrorString(_T("Unable to get layer data connections - invalid layer index"));
	}
}

int CNetDesignerDoc::getNumNodes()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			rval++;
		}
	}
	return(rval);
}

int CNetDesignerDoc::getNumConvolutions()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			if ((netData.getNodeMode(i) != "Input") && (netData.getNodeMode(i) != "Output")) {
				rval++;
			}
		}
	}
	return(rval);
}

int CNetDesignerDoc::getNumInputLayers()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			if (netData.getNodeMode(i) == "Input") {
				rval++;
			}
		}
	}
	return(rval);
}

int CNetDesignerDoc::getNumOutputLayers()
{
	int rval = 0;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			if (netData.getNodeMode(i) == "Output") {
				rval++;
			}
		}
	}
	return(rval);
}




// *********************************************************************************************************************************************
// **  CNetDesignerDoc netData access functions for weight data  *******************************************************************************
// *********************************************************************************************************************************************

double CNetDesignerDoc::getBiasFromNode(CString node)
{
	double rval = 0;
	if (net_ready) {
		rval = nnet.getBias(string(CT2CA(node)));
	}
	return(rval);
}

double CNetDesignerDoc::getRMSWeightsFromNode(CString node)
{
	double rval = 0;
	if (net_ready) {
		rval = nnet.rmsWeights(string(CT2CA(node)));
	}
	return(rval);
}

double CNetDesignerDoc::getWeightFromConnection(CString connect)
{
	if (net_ready) {
		int cindex = netData.getIndexFromID(string(CT2CA(connect)));
		string lbl = netData.getOutputNode(cindex);
		int input = 0;
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isConnect(i)) {
				if (lbl == netData.getOutputNode(i)) {
					if (connect == (CString)netData.getID(i).c_str()) {
						return(nnet.getWeight(lbl, input));
					}
					else {
						input++;
					}
				}
			}
		}
	}
	return(0);
}

// Load weights from netData to nnet 
bool CNetDesignerDoc::transferWeightsAndBiases(bool netdata_to_nnet)
{
	if (net_ready) {
		if (netdata_to_nnet) {
			nnet.setWeights(netData.netweightdata);
		}
		else {
			netData.netweightdata = nnet.getWeights();
		}
	}
	return(true);
}

bool CNetDesignerDoc::activeWeights()
{
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			double bias = netData.getNodeBias(i);
			if (!isnan(bias) && !isinf(bias)) {
				if (bias != 0) {
					return(true);
				}
			}
			vector<double> weights = netData.getConvWeights(i);
			if (weights.size() > 0) {
				for (int j = 0; j < weights.size(); j++) {
					double weight = weights[j];
					if (!isnan(weight) && !isinf(weight)) {
						if (weight != 0) {
							return(true);
						}
					}
				}
			}
		}
		else if (netData.isConnect(i)) {
			double weight = netData.getWeight(i);
			if (!isnan(weight) && !isinf(weight)) {
				if (weight != 0) {
					return(true);
				}
			}
		}
		else if (netData.isConvLayer(i)) {
			vector<double> biases = netData.getConvNodeBiases(i);
			if (biases.size() > 0) {
				for (int j = 0; j < biases.size(); j++) {
					double bias = biases[j];
					if (!isnan(bias) && !isinf(bias)) {
						if (bias != 0) {
							return(true);
						}
					}
				}
			}
			vector<double> weights = netData.getConvWeights(i);
			if (weights.size() > 0) {
				for (int j = 0; j < weights.size(); j++) {
					double weight = weights[j];
					if (!isnan(weight) && !isinf(weight)) {
						if (weight != 0) {
							return(true);
						}
					}
				}
			}
		}
	}
	return(false);
}




// *********************************************************************************************************************************************
// **  CNetDesignerDoc netData edit functions  *************************************************************************************************
// *********************************************************************************************************************************************

int CNetDesignerDoc::addNode(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, int gindex)
{
	string sid = string(CT2CA(getNewNodeName()));
	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	netDataStruct tdata;
	tdata.setType(NODE);
	tdata.setID(sid);
	tdata.setLabel(sid);
	tdata.setXDisp(x);
	tdata.setYDisp(y);
	tdata.setGroupIndex(gindex);
	tdata.stringParamListLabels.push_back("Mode");
	tdata.stringParamList.push_back(string(CT2CA(mode)));
	tdata.stringParamListLabels.push_back("DataField");
	tdata.stringParamList.push_back("");
	tdata.stringParamListLabels.push_back("Function");
	if ((gindex >= 0) && ((mode == _T("Input")) || (mode == _T("Output")))) {
		tdata.stringParamList.push_back("Connector");
	}
	else {
		tdata.stringParamList.push_back(string(CT2CA(dfuncn)));
	}
	tdata.doubleParamListLabels.push_back("Eta");
	tdata.doubleParamList.push_back(deta);
	tdata.doubleParamListLabels.push_back("Alpha");
	tdata.doubleParamList.push_back(dalpha);
	tdata.doubleParamListLabels.push_back("Rand");
	tdata.doubleParamList.push_back(drand);
	pushEditNodeData(tdata);
	processEditData();
	return(netData.getIndexFromID(sid));
}

int CNetDesignerDoc::addNode(int x, int y, CString mode, int gindex)
{
	netDataStruct tdata;
	tdata.nodeType = NODE;
	tdata.id = string(CT2CA(getNewNodeName()));
	tdata.label = tdata.id;
	tdata.x_disp = x;
	tdata.y_disp = y;
	tdata.group_index = gindex;
	tdata.stringParamListLabels.push_back("Mode");
	tdata.stringParamList.push_back(string(CT2CA(mode)));
	tdata.stringParamListLabels.push_back("DataField");
	tdata.stringParamList.push_back("");
	tdata.stringParamListLabels.push_back("Function");
	tdata.stringParamList.push_back(string(CT2CA(dfuncn)));
	tdata.doubleParamListLabels.push_back("Eta");
	tdata.doubleParamList.push_back(deta);
	tdata.doubleParamListLabels.push_back("Alpha");
	tdata.doubleParamList.push_back(dalpha);
	tdata.doubleParamListLabels.push_back("Rand");
	tdata.doubleParamList.push_back(drand);
	tdata.doubleParamListLabels.push_back("Bias");
	tdata.doubleParamList.push_back(0.0);
	tdata.intParamListLabels.push_back("Trainable");
	tdata.intParamList.push_back((int)true);
	netData.pushNodeData(tdata);
	SetModifiedFlag();
	return(netData.getIndexFromID(tdata.id));
}

int CNetDesignerDoc::addConnect(int sop, float szoom, int shscroll, int svscroll,
	CString node_in, CString node_out, int gindex, double sweight, vector<double> sweights, vector<CPoint> points)
{
	CString sid = getNewConnectName();
	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	pushEditConnectData(sid, sid, node_in, node_out, gindex, sweight, sweights, points);
	processEditData();
	return(netData.getIndexFromID(string(CT2CA(sid))));
}

int CNetDesignerDoc::addConnect(CString node_in, CString node_out, int gindex, double sweight, vector<double> sweights, vector<CPoint> points)
{
	netDataStruct tdata;
	tdata.nodeType = CONNECT;
	string sid = string(CT2CA(getNewConnectName()));
	vector<twoValues> mpoints;  // passing in no points since we don't use them
	netData.pushConnectData(sid, sid, string(CT2CA(node_in)), string(CT2CA(node_out)), gindex, sweight, sweights, mpoints);
	SetModifiedFlag();
	return(netData.getIndexFromID(sid));
}

int CNetDesignerDoc::addDataSource(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filenames)
{
	CString sid = getNewDataName();
	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	pushEditDataSource(sid, sid, x, y, mode, width, height, file_min_width, aspect, gray, wav_max, filenames);
	processEditData();
	return(netData.getIndexFromID(string(CT2CA(sid))));
}

int CNetDesignerDoc::addConvLayer(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, CString sfunc, double seta, double salpha, double srand, string processor, double lower, double upper, bool strainable, int slayers, int snodesperlayer, int sconvtype, vector<int> dimensions, vector<int> strides, vector<CPoint> locations, vector<string> fields, int gindex)
{
	string sid = string(CT2CA(getNewConvLayerName()));
	netDataStruct tdata;

	vector<twoValues> mlocations;
	if (locations.size() > 0) {
		for (int i = 0; i < locations.size(); i++) {
			twoValues mpt;
			mpt.x = locations[i].x;
			mpt.y = locations[i].y;
			mlocations.push_back(mpt);
		}
	}

	tdata.setType(CONVLAYER);
	tdata.setID(sid);
	tdata.setLabel(sid);
	tdata.setXDisp(x);
	tdata.setYDisp(y);
	tdata.setGroupIndex(gindex);

	tdata.stringParamListLabels.push_back("Mode");
	tdata.stringParamList.push_back(string(CT2CA(mode)));
	tdata.stringParamListLabels.push_back("DataField");
	tdata.stringParamList.push_back("");
	tdata.stringParamListLabels.push_back("Function");
	tdata.stringParamList.push_back(string(CT2CA(dfuncn)));
	tdata.doubleParamListLabels.push_back("Eta");
	tdata.doubleParamList.push_back(deta);
	tdata.doubleParamListLabels.push_back("Alpha");
	tdata.doubleParamList.push_back(dalpha);
	tdata.doubleParamListLabels.push_back("Rand");
	tdata.doubleParamList.push_back(drand);
	tdata.intParamListLabels.push_back("CLayers");
	tdata.intParamList.push_back(slayers);
	tdata.intParamListLabels.push_back("CNodesPerLayer");
	tdata.intParamList.push_back(snodesperlayer);
	tdata.intParamListLabels.push_back("ConvType");
	tdata.intParamList.push_back(sconvtype);

	tdata.setNodePreProcessor(processor);
	tdata.setNodeLowerNormLimit(lower);
	tdata.setNodeUpperNormLimit(upper);
	tdata.setNodeTrainable(strainable);
	tdata.setDataFields(fields);
	tdata.setConvDimensions(dimensions);
	tdata.setConvStrides(strides);
	tdata.setConvLocations(mlocations);

	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	pushEditConvLayer(tdata);// sid, sid, x, y, mode, sfunc, seta, salpha, srand, processor, lower, upper, strainable, slayers, snodesperlayer, sconvtype, dimensions, strides, locations, fields);
	processEditData();
	return(netData.getIndexFromID(sid));
}

int CNetDesignerDoc::addAttentionLayer(int sop, float szoom, int shscroll, int svscroll, int x, int y, int dsize, int ksize, int vsize, double slimit, bool self, int gindex)
{
	string sid = string(CT2CA(getNewAttentionLayerName()));
	netDataStruct tdata;

	tdata.setType(ATTENTION);
	if (self) {
		tdata.setType(SELFATTENTION);
	}
	tdata.setID(sid);
	tdata.setLabel(sid);
	tdata.setXDisp(x);
	tdata.setYDisp(y);
	tdata.setGroupIndex(gindex);

	tdata.intParamListLabels.push_back("ConvType");
	if (self) {
		tdata.intParamList.push_back(SELFATTENTION);
	}
	else {
		tdata.intParamList.push_back(ATTENTION);
	}

	tdata.setD(dsize);
	tdata.setDk(ksize);
	tdata.setDv(vsize);
	tdata.setSumAddLimit(slimit);

	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	pushEditAttentionLayer(tdata);
	processEditData();
	return(netData.getIndexFromID(sid));
}

int CNetDesignerDoc::addAssembly(int sop, float szoom, int shscroll, int svscroll, int x, int y, int gindex, string reference)
{
	string sid = string(CT2CA(getNewAssemblyName()));
	netDataStruct tdata;

	tdata.setType(ASSEMBLY);
	tdata.setID(sid);
	tdata.setLabel(sid);
	tdata.setXDisp(x);
	tdata.setYDisp(y);
	tdata.setGroupIndex(gindex);
	tdata.setAssemblyReference(reference);

	tdata.intParamListLabels.push_back("ConvType");
	tdata.intParamList.push_back(ASSEMBLY);

	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	pushEditAssembly(tdata);
	processEditData();
	return(netData.getIndexFromID(sid));
}

int CNetDesignerDoc::addNetCluster(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, CString sfunc, double seta, double salpha, double srand, int snodesperlayer, int sconvtype, int gindex)
{
	string sid = string(CT2CA(getNewClusterName()));
	netDataStruct tdata;

	tdata.setType(NETCLUSTER);
	tdata.setID(sid);
	tdata.setLabel(sid);
	tdata.setXDisp(x);
	tdata.setYDisp(y);
	tdata.setGroupIndex(gindex);

	tdata.stringParamListLabels.push_back("Mode");
	tdata.stringParamList.push_back(string(CT2CA(mode)));
	tdata.stringParamListLabels.push_back("Function");
	tdata.stringParamList.push_back(string(CT2CA(dfuncn)));
	tdata.doubleParamListLabels.push_back("Eta");
	tdata.doubleParamList.push_back(deta);
	tdata.doubleParamListLabels.push_back("Alpha");
	tdata.doubleParamList.push_back(dalpha);
	tdata.doubleParamListLabels.push_back("Rand");
	tdata.doubleParamList.push_back(drand);
	tdata.intParamListLabels.push_back("CNodesPerLayer");
	tdata.intParamList.push_back(snodesperlayer);
	tdata.intParamListLabels.push_back("ConvType");
	tdata.intParamList.push_back(sconvtype);

	tdata.setD(1);
	tdata.setClusterUpdateEpochs(dcupdateepochs);
	//tdata.setMinTestEpochs(100);
	tdata.setClusterTestError(0.020);
	tdata.setClusterSplitVariable(1.0);
	tdata.setPatternSlitMethod("Median");

	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	pushEditNetCluster(tdata);
	processEditData();
	return(netData.getIndexFromID(sid));
}

int CNetDesignerDoc::addDropout(int sop, float szoom, int shscroll, int svscroll, int x, int y, double spercent, int gindex)
{
	string sid = string(CT2CA(getNewDropoutName()));
	netDataStruct tdata;

	tdata.setType(DROPOUT);
	tdata.setID(sid);
	tdata.setLabel(sid);
	tdata.setXDisp(x);
	tdata.setYDisp(y);
	tdata.setGroupIndex(gindex);

	tdata.doubleParamListLabels.push_back("Eta");
	tdata.doubleParamList.push_back(deta);

	tdata.setDropoutPercent(spercent);

	clearAndSetEditParams(sop, szoom, shscroll, svscroll);
	pushEditDropout(tdata);
	processEditData();
	return(netData.getIndexFromID(sid));
}

int CNetDesignerDoc::updateDataSource(CString mode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filepaths)
{
	int index = -1;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			index = i;
			i = netData.size();
		}
	}
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	if (index >= 0) {
		CString sid = (CString)netData.getID(index).c_str();
		CString slbl = (CString)netData.getLabel(index).c_str();
		int sx = netData.getXDisp(index);
		int sy = netData.getYDisp(index);
		clearAndSetEditParams(UNDO_EDIT, pView->zoomScale, pView->lHorzScollPos, pView->lVertScollPos);
		pushEditDataSource(sid, slbl, sx, sy, mode, width, height, file_min_width, aspect, gray, wav_max, filepaths);
		processEditData();
	}
	else {
		index = addDataSource(UNDO_ADD, pView->zoomScale, pView->lHorzScollPos, pView->lVertScollPos, 0, 0, mode, width, height, file_min_width, aspect, gray, wav_max, filepaths);
	}
	vector<CString> tfilenames;
	for (int j = 0; j < filepaths.size(); j++) {
		tfilenames.push_back((CString)filepaths[j].c_str());
	}
	//CString s = _T(""); s.Format(_T("updateDataSource : tfilenames size = %i : " + mode), (int)tfilenames.size()); addErrorString(s);
	setDataPathNames(tfilenames);
	if (getDataPathType() == FILE_TYPE_CSV) {
		CAuxFunctions func;
		setDataPathNames(func.checkHeadersFromDataSources(getDataPathNames(false)));
		//trainData.getHeadersFromProcessDataSources(getDataPathNames(false));
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isConvLayer(i)) {
				if (netData.getNodeMode(i) == "Input") {
					netData.setConvType(i, DATA_CONV);
				}
			}
		}
	}
	else if ((getDataPathType() == FILE_TYPE_JPG) || (getDataPathType() == FILE_TYPE_RAW)) {
		vector<vector<CString>> pathnames;
		pathnames.push_back(getDataPathNames(false));
		pathnames.push_back(getValidationPathNames());
		//trainData.getHeadersFromJpgDataSources(pathnames);
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isConvLayer(i)) {
				if (netData.getConvType(i) != HIDDEN_NET) {
					netData.setConvType(i, IMAGE_CONV);
				}
			}
		}
	}
	else if (getDataPathType() == FILE_TYPE_WAV) {
		vector<vector<CString>> pathnames;
		pathnames.push_back(getDataPathNames(false));
		pathnames.push_back(getValidationPathNames());
		//trainData.getHeadersFromWavDataSources(pathnames);
		for (int i = 0; i < netData.size(); i++) {
			if (netData.isConvLayer(i)) {
				if (netData.getNodeMode(i) == "Input") {
					netData.setConvType(i, SOUND_CONV);
				}
			}
		}
	}
	return(index);
}

void CNetDesignerDoc::clearAndSetEditParams(int sop, float szoom, int shscroll, int svscroll)
{
	editData.clear();
	backData.clearCurrent();
	backData.setOpParams(sop, szoom, shscroll, svscroll);
}

void CNetDesignerDoc::pushEditNodeData(netDataStruct node)
{
	editData.pushNodeData(node);
}

void CNetDesignerDoc::pushEditConnectData(CString sid, CString slbl, CString node_in, CString node_out, int gindex, double sweight, vector<double> sweights, vector<CPoint> points)
{
	vector<twoValues> mpoints;  // passing in no points since we don't use them
	editData.pushConnectData(string(CT2CA(sid)), string(CT2CA(slbl)), string(CT2CA(node_in)), string(CT2CA(node_out)), gindex, sweight, sweights, mpoints); 
}

void CNetDesignerDoc::pushEditDataSource(CString sid, CString slbl, int sx, int sy, CString smode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filepaths)
{
	editData.pushDataSource(string(CT2CA(sid)), string(CT2CA(slbl)), sx, sy, string(CT2CA(smode)), width, height, file_min_width, aspect, gray, wav_max, filepaths);
}

void CNetDesignerDoc::pushEditConvLayer(netDataStruct layer)
{
	editData.pushConvLayer(layer);
}

void CNetDesignerDoc::pushEditAttentionLayer(netDataStruct layer)
{
	editData.pushAttentionLayer(layer);
}

void CNetDesignerDoc::pushEditAssembly(netDataStruct layer)
{
	editData.pushAssembly(layer);
}

void CNetDesignerDoc::pushEditNetCluster(netDataStruct layer)
{
	editData.pushNetCluster(layer);
}

void CNetDesignerDoc::pushEditDropout(netDataStruct layer)
{
	editData.pushDropout(layer);
}

bool CNetDesignerDoc::processEditData()
{
	bool changed = false;
	if (editData.size() > 0) {
		if (backData.isCurrentEdit()) {
			netDataStruct tdata;
			for (int i = 0; i < editData.size(); i++) {
				tdata = editData.getNode(i);
				//addErrorString(_T("processEditData"));
				if (isDataChanged(tdata)) {
					changed = true;
				}
			}
			if (changed) {
				//addErrorString(_T("processEditData changed for ") + tdata.id);
				backData.push_back();
				redoData.clear();
			}
			/*else {
				addErrorString(_T("processEditData not changed for ") + tdata.id);
			}*/
		}
		else if (backData.isCurrentAdd()) {
			netDataStruct tdata;
			for (int i = 0; i < editData.size(); i++) {
				if (editData.isNode(i)) {
					tdata = editData.getNode(i);
					netData.pushNodeData(tdata);
					backData.pushNodeData(tdata);
				}
				else if (editData.isConnect(i)) {
					tdata = editData.getNode(i);
					vector<twoValues> mpoints;
					netData.pushConnectData(tdata.getID(), tdata.getLabel(), tdata.getInputNode(), tdata.getOutputNode(), tdata.getGroupIndex(), tdata.getWeight(), tdata.getConvWeights(), mpoints);// tdata.getPoints());
					backData.pushConnectData(tdata.getID(), tdata.getLabel(), tdata.getInputNode(), tdata.getOutputNode(), tdata.getGroupIndex(), tdata.getWeight(), tdata.getConvWeights(), mpoints);// tdata.getPoints());
				}
				else if (editData.isDataSource(i)) {
					tdata = editData.getNode(i);
					netData.pushDataSource(tdata.getID(), tdata.getLabel(), tdata.getXDisp(), tdata.getYDisp(), tdata.getNodeMode(), tdata.getDataImageWidth(), tdata.getDataImageHeight(), tdata.getFileMinImageWidth(), tdata.getFileImageAspect(), tdata.getDataImageGray(), tdata.getDataWavMax(), tdata.getFilePaths());
					backData.pushDataSource(tdata.getID(), tdata.getLabel(), tdata.getXDisp(), tdata.getYDisp(), tdata.getNodeMode(), tdata.getDataImageWidth(), tdata.getDataImageHeight(), tdata.getFileMinImageWidth(), tdata.getFileImageAspect(), tdata.getDataImageGray(), tdata.getDataWavMax(), tdata.getFilePaths());
				}
				else if (editData.isConvLayer(i)) {
					tdata = editData.getNode(i);
					netData.pushConvLayer(tdata);
					backData.pushConvLayer(tdata);
				}
				else if (editData.isAttentionLayer(i)) {
					tdata = editData.getNode(i);
					netData.pushAttentionLayer(tdata);
					backData.pushAttentionLayer(tdata);
				}
				else if (editData.isAssemblyLayer(i)) {
					tdata = editData.getNode(i);
					netData.pushAssembly(tdata);
					backData.pushAssembly(tdata);
				}
				else if (editData.isNetCluster(i)) {
					tdata = editData.getNode(i);
					netData.pushNetCluster(tdata);
					backData.pushNetCluster(tdata);
				}
				else if (editData.isDropout(i)) {
					tdata = editData.getNode(i);
					netData.pushDropout(tdata);
					backData.pushDropout(tdata);
				}
			}
			backData.push_back();
			redoData.clear();
			changed = true;
		}
		else if (backData.isCurrentDelete()) {
			netDataStruct tdata;
			for (int i = 0; i < editData.size(); i++) {
				tdata = editData.getNode(i);
				for (int j = 0; j < netData.size(); j++) {
					if (tdata.id == netData.getID(j)) {
						if (tdata.isNode()) {
							backData.pushNodeData(tdata);
							netData.removeItem(tdata.id);
							j = netData.size();
						}
						else if (tdata.isConnect()) {
							vector<twoValues> mpoints;
							backData.pushConnectData(tdata.getID(), tdata.getLabel(), tdata.getInputNode(), tdata.getOutputNode(), tdata.getGroupIndex(), tdata.getWeight(), tdata.getConvWeights(), mpoints);// tdata.getPoints());
							netData.removeItem(tdata.id);
							j = netData.size();
						}
						else if (tdata.isDataSource()) {
							backData.pushDataSource(tdata.getID(), tdata.getLabel(), tdata.getXDisp(), tdata.getYDisp(), tdata.getNodeMode(), tdata.getDataImageWidth(), tdata.getDataImageHeight(), tdata.getFileMinImageWidth(), tdata.getFileImageAspect(), tdata.getDataImageGray(), tdata.getDataWavMax(), tdata.getFilePaths());
							netData.removeItem(tdata.id);
							j = netData.size();
						}
						else if (tdata.isConvLayer()) {
							backData.pushConvLayer(tdata);
							netData.removeItem(tdata.id);
							j = netData.size();
						}
						else if (tdata.isAttentionLayer()) {
							backData.pushAttentionLayer(tdata);
							netData.removeItem(tdata.id);
							j = netData.size();
						}
						else if (tdata.isAssemblyLayer()) {
							backData.pushAssembly(tdata);
							netData.removeItem(tdata.id);
							j = netData.size();
						}
						else if (tdata.isNetCluster()) {
							backData.pushNetCluster(tdata);
							netData.removeItem(tdata.id);
							j = netData.size();
						}
						else if (tdata.isDropout()) {
							backData.pushDropout(tdata);
							netData.removeItem(tdata.id);
							j = netData.size();
						}
					}
				}
			}
			backData.push_back();
			redoData.clear();
			changed = true;
		}
		if (changed) {
			SetModifiedFlag();
		}
	}
	checkDataSource();
	hasConvolution();
	return(changed);
}

bool CNetDesignerDoc::isDataChanged(netDataStruct tdata)
{
	for (int i = 0; i < netData.size(); i++) {
		if (tdata.id == netData.getID(i)) {
			if (tdata.getType() == netData.getType(i)) {
				if (netData.isNode(i)) {
					bool changed = didParametListChanged(i, tdata);
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getNodeMode() != netData.getNodeMode(i)) ||
						(tdata.getXDisp() != netData.getXDisp(i)) ||
						(tdata.getYDisp() != netData.getYDisp(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i)) || changed) {
						backData.pushNodeData(netData.getNode(i));
						netData.updateNode(i, tdata);
						return(true);
					}
				}
				else if (netData.isConnect(i)) {
					/*bool changed = false;
					for (int j = 0; j < tdata.getNumPoints(); j++) {
						if ((tdata.getPoint(j).x != netData.getPoint(i, j).x) || (tdata.getPoint(j).y != netData.getPoint(i, j).y)) {
							changed = true;
							j = tdata.getNumPoints();
						}
					}*/
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getInputNode() != netData.getInputNode(i)) ||
						(tdata.getOutputNode() != netData.getOutputNode(i)) || 
						(tdata.getNumPoints() != netData.getNumPoints(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i))/* || changed*/) {
						vector<twoValues> mpoints;
						backData.pushConnectData(netData.getID(i), netData.getLabel(i), netData.getInputNode(i),
							netData.getOutputNode(i), netData.getGroupIndex(i), netData.getWeight(i), netData.getConvWeights(i), mpoints);
						netData.setInputNode(i, tdata.getInputNode());
						netData.setOutputNode(i, tdata.getOutputNode());
						netData.setPoints(i, netData.getPoints(i));// mpoints);
						return(true);
					}
				}
				else if (netData.isDataSource(i)) {
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getDataMode() != netData.getDataMode(i)) ||
						(tdata.getNodeMode() != netData.getNodeMode(i)) ||
						(tdata.getFilePaths() != netData.getFilePaths(i)) ||
						(tdata.getXDisp() != netData.getXDisp(i)) ||
						(tdata.getYDisp() != netData.getYDisp(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i)) ||
						(tdata.getDataImageWidth() != netData.getDataImageWidth(i)) ||
						(tdata.getDataImageHeight() != netData.getDataImageHeight(i)) ||
						(tdata.getFileMinImageWidth() != netData.getFileMinImageWidth(i)) ||
						(tdata.getFileImageAspect() != netData.getFileImageAspect(i)) ||
						(tdata.getDataImageGray() != netData.getDataImageGray(i)) ||
						(tdata.getDataWavMax() != netData.getDataWavMax(i))) {
						backData.pushDataSource(netData.getID(i), netData.getLabel(i), netData.getXDisp(i), netData.getYDisp(i), netData.getNodeMode(i), netData.getDataImageWidth(i), netData.getDataImageHeight(i), netData.getFileMinImageWidth(i), netData.getFileImageAspect(i), netData.getDataImageGray(i), netData.getDataWavMax(i), netData.getFilePaths(i));
						netData.setXDisp(i, tdata.getXDisp());
						netData.setYDisp(i, tdata.getYDisp());
						netData.setDataMode(i, tdata.getDataMode());
						netData.setNodeMode(i, tdata.getNodeMode());
						netData.setFilePaths(i, tdata.getFilePaths());
						netData.setDataImageWidth(i, tdata.getDataImageWidth());
						netData.setDataImageHeight(i, tdata.getDataImageHeight());
						netData.setFileMinImageWidth(i, tdata.getFileMinImageWidth());
						netData.setFileImageAspect(i, tdata.getFileImageAspect());
						netData.setDataImageGray(i, tdata.getDataImageGray());
						return(true);
					}
				}
				else if (netData.isConvLayer(i)) {
					bool changed = didParametListChanged(i, tdata);
					vector<twoValues> cl1 = tdata.getConvLocations();
					vector<twoValues> cl2 = netData.getConvLocations(i);
					if (cl1.size() != cl2.size()) {
						changed = true;
					}
					else if (cl1.size() > 0) {
						for (int j = 0; j < cl1.size(); j++) {
							if ((cl1[j].x != cl2[j].x) || (cl1[j].y != cl2[j].y)) {
								j = (int)cl1.size();
								changed = true;
							}
						}
					}
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getNodeMode() != netData.getNodeMode(i)) ||
						(tdata.getXDisp() != netData.getXDisp(i)) ||
						(tdata.getYDisp() != netData.getYDisp(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i)) ||
						(tdata.getD() != netData.getD(i)) ||
						(tdata.getConvDimensions() != netData.getConvDimensions(i)) ||
						(tdata.getConvStrides() != netData.getConvStrides(i)) || changed) {
						backData.pushConvLayer(netData.getNode(i));
						netData.updateConvolution(i, tdata);
						return(true);
					}
				}
				else if (netData.isAttentionLayer(i)) {
					bool changed = didParametListChanged(i, tdata);
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getNodeMode() != netData.getNodeMode(i)) ||
						(tdata.getXDisp() != netData.getXDisp(i)) ||
						(tdata.getYDisp() != netData.getYDisp(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i)) ||
						(tdata.getD() != netData.getD(i)) ||
						(tdata.getDk() != netData.getDk(i)) ||
						(tdata.getDv() != netData.getDv(i)) || 
						(tdata.getSumAddLimit() != netData.getSumAddLimit(i)) ||
						(tdata.hasSoftmaxProcessing() != netData.hasSoftmaxProcessing(i)) ||
						(tdata.getTransformerModeQuadratic() != netData.getTransformerModeQuadratic(i)) || changed) {
						backData.pushAttentionLayer(netData.getNode(i));
						netData.updateAttention(i, tdata);
						return(true);
					}
				}
				else if (netData.isAssemblyLayer(i)) {
					bool changed = didParametListChanged(i, tdata);
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getNodeMode() != netData.getNodeMode(i)) ||
						(tdata.getXDisp() != netData.getXDisp(i)) ||
						(tdata.getYDisp() != netData.getYDisp(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i)) ||
						(tdata.getAssemblyReference() != netData.getAssemblyReference(i)) || changed) {
						backData.pushAssembly(netData.getNode(i));
						netData.updateAssembly(i, tdata);
						return(true);
					}
				}
				else if (netData.isNetCluster(i)) {
					bool changed = didParametListChanged(i, tdata);
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getNodeMode() != netData.getNodeMode(i)) ||
						(tdata.getXDisp() != netData.getXDisp(i)) ||
						(tdata.getYDisp() != netData.getYDisp(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i)) ||
						(tdata.getConvDimensions() != netData.getConvDimensions(i)) ||
						(tdata.getConvDimensions() != netData.getConvDimensions(i)) || 
						(tdata.getDv() != netData.getDv(i)) || 
						(tdata.getClusterTestError() != netData.getClusterTestError(i)) ||
						(tdata.getClusterSplitVariable() != netData.getClusterSplitVariable(i)) ||
						(tdata.getPatternSplitMethod() != netData.getPatternSplitMethod(i)) || changed) {
						backData.pushNetCluster(netData.getNode(i));
						netData.updateNetCluster(i, tdata);
						return(true);
					}
				}
				else if (netData.isDropout(i)) {
					bool changed = didParametListChanged(i, tdata);
					if ((tdata.getLabel() != netData.getLabel(i)) ||
						(tdata.getNodeMode() != netData.getNodeMode(i)) ||
						(tdata.getXDisp() != netData.getXDisp(i)) ||
						(tdata.getYDisp() != netData.getYDisp(i)) ||
						(tdata.getGroupIndex() != netData.getGroupIndex(i)) ||
						(tdata.getDropoutPercent() != netData.getDropoutPercent(i))  || changed) {
						backData.pushDropout(netData.getNode(i));
						netData.updateDropout(i, tdata);
						return(true);
					}
				}
			}
			i = netData.size();
		}
	}
	return(false);
}

bool CNetDesignerDoc::didParametListChanged(int netindex, netDataStruct tdata)
{
	if ((tdata.stringParamList.size() != netData.getNode(netindex).stringParamList.size()) ||
		(tdata.stringParamListLabels.size() != netData.getNode(netindex).stringParamListLabels.size())) {
		return(true);
	}
	else if ((tdata.doubleParamList.size() != netData.getNode(netindex).doubleParamList.size()) ||
		(tdata.doubleParamListLabels.size() != netData.getNode(netindex).doubleParamListLabels.size())) {
		return(true);
	}
	else if ((tdata.intParamList.size() != netData.getNode(netindex).intParamList.size()) ||
		(tdata.intParamListLabels.size() != netData.getNode(netindex).intParamListLabels.size())) {
		return(true);
	}
	for (int j = 0; j < tdata.stringParamListLabels.size(); j++) {
		if ((tdata.stringParamList.size() > j) &&
			(netData.getNode(netindex).stringParamList.size() > j) &&
			(netData.getNode(netindex).stringParamListLabels.size() > j)) {
			if (tdata.stringParamListLabels[j] == netData.getNode(netindex).stringParamListLabels[j]) {
				if (tdata.stringParamList[j] != netData.getNode(netindex).stringParamList[j]) {
					return(true);
				}
			}
		}
	}
	for (int j = 0; j < tdata.intParamListLabels.size(); j++) {
		if ((tdata.intParamList.size() > j) &&
			(netData.getNode(netindex).intParamList.size() > j) &&
			(netData.getNode(netindex).intParamListLabels.size() > j)) {
			if (tdata.intParamListLabels[j] == netData.getNode(netindex).intParamListLabels[j]) {
				if (tdata.intParamList[j] != netData.getNode(netindex).intParamList[j]) {
					return(true);
				}
			}
		}
	}
	for (int j = 0; j < tdata.doubleParamListLabels.size(); j++) {
		for (int k = 0; k < netData.getNode(netindex).doubleParamListLabels.size(); k++) {
			if (tdata.doubleParamListLabels[j] == netData.getNode(netindex).doubleParamListLabels[k]) {
				if ((j < tdata.doubleParamList.size()) && (k < netData.getNode(netindex).doubleParamList.size())) {
					if (tdata.doubleParamList[j] != netData.getNode(netindex).doubleParamList[k]) {
						return(true);
					}
				}
			}
		}
	}
	return(false);
}

bool CNetDesignerDoc::undoLast(float szoom, int shscroll, int svscroll)
{
	if (backData.size() > 0) {
		backData.loadLast();
		editStruct bdata = backData.meditdata;
		editStruct rdata = processDataToNet(bdata);
		int op = UNDO_EDIT;
		if (bdata.getOperation() == UNDO_ADD) {
			op = UNDO_DELETE;
		}
		else if (bdata.getOperation() == UNDO_DELETE) {
			op = UNDO_ADD;
		}
		addToRedo(op, szoom, shscroll, svscroll, rdata);
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->resetZoomAndScrolls(bdata.getZoom(), bdata.getHScroll(), bdata.getVScroll());
		backData.pop_back();
		SetModifiedFlag();
		checkDataSource();
		return(true);
	}
	return(false);
}

bool CNetDesignerDoc::redoLast(float szoom, int shscroll, int svscroll)
{
	if (redoData.size() > 0) {
		redoData.loadLast();
		editStruct rdata = redoData.meditdata;
		editStruct bdata = processDataToNet(rdata);
		int op = UNDO_EDIT;
		if (rdata.getOperation() == UNDO_ADD) {
			op = UNDO_DELETE;
		}
		else if (rdata.getOperation() == UNDO_DELETE) {
			op = UNDO_ADD;
		}
		addToBack(op, szoom, shscroll, svscroll, bdata);
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->resetZoomAndScrolls(rdata.getZoom(), rdata.getHScroll(), rdata.getVScroll());
		redoData.pop_back();
		SetModifiedFlag();
		checkDataSource();
		return(true);
	}
	return(false);
}

editStruct CNetDesignerDoc::processDataToNet(editStruct data)
{
	editStruct meditdata;
	if (data.isEdit()) {
		for (int i = 0; i < data.editData.size(); i++) {
			string id = data.editData.getID(i);
			for (int j = 0; j < netData.size(); j++) {
				if (id == netData.getID(j)) {
					if (netData.isNode(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.updateNode(j, data.editData.getNode(i));
						j = netData.size();
					}
					else if (netData.isConnect(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.setLabel(j, data.editData.getLabel(i));
						netData.setInputNode(j, data.editData.getInputNode(i));
						netData.setOutputNode(j, data.editData.getOutputNode(i));
						netData.setGroupIndex(j, data.editData.getGroupIndex(i));
						vector<twoValues> mpoints;
						if (data.editData.getNumPoints(i) > 0) {
							for (int k = 0; k < data.editData.getNumPoints(i); k++) {
								twoValues mpt;  mpt.x = data.editData.getPoint(i, k).x; mpt.y = data.editData.getPoint(i, k).y;
								mpoints.push_back(mpt);
							}
						}
						netData.setPoints(j, data.editData.getPoints(i));
					} 
					else if (netData.isDataSource(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.setLabel(j, data.editData.getLabel(i));
						netData.setDataMode(j, data.editData.getDataMode(i));
						netData.setXDisp(j, data.editData.getXDisp(i));
						netData.setYDisp(j, data.editData.getYDisp(i));
						j = netData.size();
					}
					else if (netData.isConvLayer(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.updateConvolution(j, data.editData.getNode(i));
						j = netData.size();
					}
					else if (netData.isAttentionLayer(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.updateAttention(j, data.editData.getNode(i));
						j = netData.size();
					}
					else if (netData.isAssemblyLayer(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.updateAssembly(j, data.editData.getNode(i));
						j = netData.size();
					}
					else if (netData.isNetCluster(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.updateNetCluster(j, data.editData.getNode(i));
						j = netData.size();
					}
					else if (netData.isDropout(j)) {
						meditdata.pushData(netData.getNode(j));
						netData.updateDropout(j, data.editData.getNode(i));
						j = netData.size();
					}
				}
			}
		}
	}
	else if (data.isAdd()) {
		for (int i = 0; i < data.editData.size(); i++) {
			string id = data.editData.getID(i);
			for (int j = 0; j < netData.size(); j++) {
				if (id == netData.getID(j)) {
					meditdata.pushData(netData.getNode(j));
					netData.removeItem(j);
					j = netData.size();
				}
			}
		}
	}
	else if (data.isDelete()) {
		for (int i = 0; i < data.editData.size(); i++) {
			if (data.editData.isNode(i)) {
				netData.pushNodeData(data.editData.getNode(i));
				meditdata.pushNodeData(data.editData.getNode(i));
			} 
			else if (data.editData.isConnect(i)) {
				vector<twoValues> mpoints;
				netData.pushConnectData(data.editData.getID(i), data.editData.getLabel(i), data.editData.getInputNode(i),
					data.editData.getOutputNode(i), data.editData.getGroupIndex(i), data.editData.getWeight(i), data.editData.getConvWeights(i), mpoints);// data.editData.getPoints(i));
				meditdata.pushConnectData(data.editData.getID(i), data.editData.getLabel(i), data.editData.getInputNode(i),
					data.editData.getOutputNode(i), data.editData.getGroupIndex(i), data.editData.getWeight(i), data.editData.getConvWeights(i), mpoints);// data.editData.getPoints(i));
			}
			else if (data.editData.isDataSource(i)) {
				netData.pushDataSource(data.editData.getID(i), data.editData.getLabel(i), data.editData.getXDisp(i),
					data.editData.getYDisp(i), data.editData.getNodeMode(i), data.editData.getDataImageWidth(i), data.editData.getDataImageHeight(i), data.editData.getFileMinImageWidth(i), data.editData.getFileImageAspect(i), data.editData.getDataImageGray(i), data.editData.getDataWavMax(i), data.editData.getFilePaths(i));
				meditdata.pushDataSource(data.editData.getID(i), data.editData.getLabel(i), data.editData.getXDisp(i),
					data.editData.getYDisp(i), data.editData.getNodeMode(i), data.editData.getDataImageWidth(i), data.editData.getDataImageHeight(i), data.editData.getFileMinImageWidth(i), data.editData.getFileImageAspect(i), data.editData.getDataImageGray(i), data.editData.getDataWavMax(i), data.editData.getFilePaths(i));
			}
			else if (data.editData.isConvLayer(i)) {
				netData.pushConvLayer(data.editData.getNode(i));
				meditdata.pushConvLayer(data.editData.getNode(i));
			}
			else if (data.editData.isAttentionLayer(i)) {
				netData.pushAttentionLayer(data.editData.getNode(i));
				meditdata.pushAttentionLayer(data.editData.getNode(i));
			}
			else if (data.editData.isAssemblyLayer(i)) {
				netData.pushAssembly(data.editData.getNode(i));
				meditdata.pushAssembly(data.editData.getNode(i));
			}
			else if (data.editData.isNetCluster(i)) {
				netData.pushNetCluster(data.editData.getNode(i));
				meditdata.pushNetCluster(data.editData.getNode(i));
			}
			else if (data.editData.isDropout(i)) {
				netData.pushDropout(data.editData.getNode(i));
				meditdata.pushDropout(data.editData.getNode(i));
			}
		}
	}
	changed_net = true;
	return(meditdata);
}

void CNetDesignerDoc::addToRedo(int sop, float szoom, int shscroll, int svscroll, editStruct data)
{
	redoData.clearCurrent();
	redoData.setOpParams(sop, szoom, shscroll, svscroll);
	for (int i = 0; i < data.editData.size(); i++) {
		redoData.pushData(data.editData.getNode(i));
	}
	redoData.push_back();
}

void CNetDesignerDoc::addToBack(int sop, float szoom, int shscroll, int svscroll, editStruct data)
{
	backData.clearCurrent();
	backData.setOpParams(sop, szoom, shscroll, svscroll);
	for (int i = 0; i < data.editData.size(); i++) {
		backData.pushData(data.editData.getNode(i));
	}
	backData.push_back();
}

/*bool CNetDesignerDoc::needProcessInputDataUpdate()
{
	if (trainData.checkIfDataSourceChanged(getDataPathNames(false)) ||
		!trainData.data_up_to_date ||
		input_data_changed) {
		return(true);
	}
	return(false);
}*/




// *********************************************************************************************************************************************
// **  CNetDesignerDoc netData data initialization functions  **********************************************************************************
// *********************************************************************************************************************************************

bool CNetDesignerDoc::loadNet(bool randomize)
{
	bool rval = true;

	addErrorString(_T(""));
	addPanel1String(_T("Loading net . . ."));

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndGraph.reset();
	pMainWnd->m_wndClusterGraph.reset(_T(""));

	CAuxFunctions func;

	CStatus progressDlg;

	train_data_display_index = 0;

	atepochs = 10;
	atseconds = 60;

	if (rand_seed != 0) {
		srand(rand_seed);
	}

	deallocate();

	// Initial processing for softmax functions
	forward_direction = func.checkForwardDirection();

	if (rval) {

		rval = checkAvailableMemory();

		if (rval) {

			addPanel1String(_T("Getting node strings . . ."));
			inputStrings = netData.getInputNodeStrings();

			// Initialize net data
			nnet.clearAndDeallocate();
			nnet.setUsePercentValidation(performValidationPercent(), validation_percent);
			nnet.setUseValidationFiles(performValidation() && !performValidationPercent());
			// Initialize data pathnames
			vector<string> tnames;
			vector<CString> tcnames = combinedPathnames;
			for (int j = 0; j < tcnames.size(); j++) {
				tnames.push_back(string(CT2CA(tcnames[j])));
			}
			nnet.setCombinedPathnames(tnames);
			// Load and check file data
			rval = loadFileData();
			// Initialize net
			if (rval) {
				getDataImageWidth();
				getDataImageHeight();
				//CString s = _T(""); s.Format(_T("loadNet : image_width = %i : image_height = %i"), image_width, image_height); addErrorString(s);
				nnet.setImageParametersData(image_width, image_height, gray_images);
				nnet.preProcessWaveParameters();
				rval = nnet.initialize(netData, conv_type, randomize, randomize_data, updateRate);
			}
			for (int j = 0; j < nnet.verifyStrings.size(); j++) {
				addErrorString(nnet.verifyStrings[j]);
			}
			if (rval) {
				nnet.setParameters(sigmoid_leakage, relu_leakage);
				nnet.setClusterParameter();
				nnet.setErrorFilter(errorfilter);
				nnet.setWaveFileNormalization(normaiize_file_set);
				nnet.preProcessWaveParameters();
				nnet.getConvInputSize();
				rval = nnet.getTrainDataFromFileData();
				if (rval) {
					rval = checkAvailableMemory();
				}
				rval = nnet.initializeNetClusters();
			}


			// Converting from nnet string to Doc CString
			netInputs.clear();
			vector<inputData> tinputs = nnet.netInputs;
			for (int j = 0; j < tinputs.size(); j++) {
				netInputs.push_back((CString)tinputs[j].nodeId.c_str());
			}

			// Converting from nnet string to Doc CString
			netOutputs.clear();
			vector<outputData> toutputs = nnet.netOutputs;
			for (int j = 0; j < toutputs.size(); j++) {
				netOutputs.push_back((CString)toutputs[j].nodeId.c_str());
			}

			if (rval) {
				// Set image parameters
				has_image_conv = nnet.has_image_conv;
				has_series_conv = nnet.has_series_conv;
				has_wave_conv = nnet.has_wave_conv;
				// Initialize memory for image/sound/text data
				rval = nnet.checkAndAllocateTrainImageMemory();
				if (rval) {
					rval = nnet.checkAndAllocateTrainSoundMemory();
				}
				if (rval) {
					rval = checkAvailableMemory();
				}
			}

			if (rval) {
				rval = nnet.dataInitializationErrorCheck1();
				if (rval) {
					rval = checkAvailableMemory();
				}
			}

			if (rval) {
				rval = nnet.getImageAndWaveData();
			}

		}
	}

	if (rval) {

		addPanel1String(_T("Allocating train data memory . . ."));

		int filetype = getDataPathType();

		// Do some image output layer initialization
		rval = nnet.checkAndGetImageOutputLayers();

		if (rval) {
			rval = checkAvailableMemory();
		}

		bool no_column_data = false;

		if (rval) {

			// Size and allocate input/output memory
			addPanel1String(_T("Allocating train data arrays . . ."));
			if (!nnet.allocateInputTrainData((int64_t)nnet.num_filedata_rows)) {
				rval = false;
			}
			if (!nnet.allocateOutputTrainData((int64_t)nnet.num_filedata_rows)) {
				rval = false;
			}
			if (rval && (nnet.num_validationdata_rows > 0)) {
				if (!nnet.allocateInputValidationData((int64_t)nnet.num_validationdata_rows)) {
					rval = false;
				}
				if (!nnet.allocateOutputValidationData((int64_t)nnet.num_validationdata_rows)) {
					rval = false;
				}
			}

			if (!checkAvailableMemory()) {
				deallocate();
				rval = false;
			}

			rval = nnet.dataInitializationErrorCheck2b();

		}

		if (rval) {
			// Check memory
			if (!checkAvailableMemory()) {
				deallocate();
				rval = false;
			}
		}

		// Check memory
		if (rval) {
			if (!checkAvailableMemory()) {
				deallocate();
				rval = false;
			}
		}

		// Set validation files for validation percent
		if ((filetype == FILE_TYPE_JPG) || ((filetype == FILE_TYPE_WAV) && (sound_data_process_type != FFT_FOLLOWING) && (sound_data_process_type != CONV_FOLLOWING_FFT_FOLLOWING))) {
			if (no_column_data) {
				rval = nnet.dataInitializationErrorCheck3();
			}
		}

	}

	nnet.setupCopyOutputIndices();
	nnet.copyProcessedDataToNetTrainArrays();
	nnet.copyProcessedDataToNetValidationArrays();
	nnet.copyImageOutputDataToNetTrainArrays();
	nnet.copyImageOutputDataToNetValidationArrays();

	nnet.setParallelProcessingTest(parallel_processing_test);

	if (!checkAvailableMemory()) {
		deallocate();
		rval = false;
	}

	if (rval) {
		net_ready = true;
	}

	new_net = false;
	changed_net = false;

	addPanel1String(_T(""));

	UpdateAllViews(NULL);

	return(rval);

}

double CNetDesignerDoc::processData(CString dname, double pdata)
{
	int dindex = netData.getIndexFromID(string(CT2CA(dname)));
	return(nnet.processData(dindex, pdata));
}

double CNetDesignerDoc::reProcessData(CString dname, double pdata)
{
	//addErrorString(_T("reProcessData : processing data for ") + dname);
	double rval = pdata;
	static bool reprocess_error_shown = false;
	int dindex = netData.getIndexFromID(string(CT2CA(dname)));
	if (dindex >= 0) {
		if (netData.getNodePreProcessor(dindex) == "NormLinear") {
			double lfactor = netData.getNodeLowerNormLimit(dindex);
			double ufactor = netData.getNodeUpperNormLimit(dindex);
			if (ufactor != 0.0) {
				rval = (pdata - lfactor) / ufactor;
			}
		}
		else if (netData.getNodePreProcessor(dindex) == "NormLog") {
			double lfactor = netData.getNodeLowerNormLimit(dindex);
			double ufactor = netData.getNodeUpperNormLimit(dindex);
			if ((ufactor != 0.0)) {
				rval = exp((pdata - lfactor) / ufactor);
			}
		}
		else if (netData.getNodePreProcessor(dindex) == "FNParser") {
			/*if (netData.getNodeFunction(dindex) != "SoftMax") {
				double delta = 1.0 - (2 * sigmoid_leakage);
				if (delta > 0.0) {
					double nfactormin = getNodeLowerNormalizationFactor(dindex);
					double nfactormax = getNodeUpperNormalizationFactor(dindex);
					double lfactor = (nfactormin * (1.0 - sigmoid_leakage) - nfactormax * sigmoid_leakage) / delta;
					double ufactor = (nfactormax - nfactormin) / delta;
					if (ufactor != 0.0) {
						rval = (pdata - lfactor) / ufactor;
					}
				}
			}*/
			rval = pdata;
		}
		else if (netData.getNodePreProcessor(dindex) == "FieldKey") {
			//if (netData.getNodeFunction(dindex) != "SoftMax") {
				/*double delta = 1.0 - (2 * sigmoid_leakage);
				if (delta > 0.0) {
					double nfactormin = getNodeLowerNormalizationFactor(dindex);
					double nfactormax = getNodeUpperNormalizationFactor(dindex);
					double lfactor = (nfactormin * (1.0 - sigmoid_leakage) - nfactormax * sigmoid_leakage) / delta;
					double ufactor = (nfactormax - nfactormin) / delta;
					if (ufactor != 0.0) {
						rval = (pdata - lfactor) / ufactor;
					}
				}*/
			//}
			rval = pdata;
		}
		else {
			//addErrorString(_T("reProcessData : unable to process ") + dname);
		}
	}
	else if (!reprocess_error_shown) {
		addErrorString(_T("reProcessData : unable to process data for ") + dname);
		reprocess_error_shown = true;
	}
	//CString s = _T("");
	//s.Format(_T("processData : processing %.4f to %.4f for "), pdata, rval);
	//addErrorString(s + dname);
	return(rval);
}

double CNetDesignerDoc::reScaleData(CString dname, double pdata)
{
	double rval = pdata;
	int dindex = netData.getIndexFromID(string(CT2CA(dname)));
	if (dindex >= 0) {
		if (netData.getNodePreProcessor(dindex) == "NormLinear") {
			double ufactor = netData.getNodeUpperNormLimit(dindex);
			if (ufactor != 0) {
				rval = pdata / ufactor;
			}
		}
		else if (netData.getNodePreProcessor(dindex) == "NormLog") {
			double ufactor = netData.getNodeUpperNormLimit(dindex);
			if ((ufactor != 0)) {
				rval = exp(pdata / ufactor);
			}
		}
	}
	return(rval);
}

bool CNetDesignerDoc::performValidation()
{
	return(performValidationPercent() || use_validation_file);
}

bool CNetDesignerDoc::performValidationPercent()
{
	return(use_validation_percent && (validation_percent > 0));
}

bool CNetDesignerDoc::verifyNet(bool randomize)
{
	bool rval = true;
	NetVerify verify;

	verifying = true;

	randomize_net = randomize;

	addPanel1String(_T("Starting net verification"));

	nnet.train_count = 0;
	nnet.err_count = 0;
	save_sec = 240;
	save_sec2 = 300;
	relative_min_epoch_count = minEpochs;
	error_delta_count = 0;
	nnet.resetUpdateInc();
	error_rising_last = 0;
	error_rising_sum = 0;
	best_epochs = 0.0;
	best_error = -1.0;
	best_validation_epochs = 0.0;
	best_validation_error = -1.0;
	train_data_display_index = 0;
	delta_epochs = 0;
	previous_archive_error = 1000;
	net_ready = false;

	cprocess = 0;
	sortIndex = 0;
	cluster_inc = 10;
	cluster_iteration = 0;
	update_cluster_inc = 10;

	graph_update_cnt = 0;
	graph_update_inc = 1;
	graph_update_cnt2 = 0;
	graph_update_inc2 = 2;

	trainDataX.clear();
	trainDataY.clear();
	trainDataY2.clear();
	decoderDataX.clear();
	decoderDataY.clear();

	getTrainExitCriteria();

	if (!checkAvailableMemory()) {
		rval = false;
	}

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.clear();

	CString msg = _T("");
	vector<CString> verifyStrings;

	verifyStrings.clear();

	CAuxFunctions func;

	int filetype = FILE_TYPE_NONE;

	addPanel1String(_T("Checking net data source . . ."));

	if (rval && checkDataSource()) {

		// Checking that data pathnames are valid
		bool valid = false;
		int numfiles = getDataPathNamesSize(false);
		if (numfiles > 0) {
			valid = true;
			for (int i = 0; i < numfiles; i++) {
				if ((getDataPathName(i, false) == _T("")) || (getDataPathName(i, false) == _T("Null Filename"))) {
					verifyStrings.push_back(_T("Error : Invalid Data Filename"));
					i = numfiles;
					valid = false;
				}
			}
		}

		if (valid) {

			filetype = getDataPathType();

			if (filetype == FILE_TYPE_CSV) {

				if (numfiles > 1) {
					msg.Format(_T("Pass : Num Data Filenames = %i"), numfiles);
					verifyStrings.push_back(msg);
				}
				else {
					verifyStrings.push_back(_T("Pass : Data Filename - ") + getDataPathName(0, false));
				}

				setDataPathNames(func.checkHeadersFromDataSources(getDataPathNames(false)));

			}
			else if (filetype == FILE_TYPE_JPG) {

				if (numfiles > 1) {
					msg.Format(_T("Pass : Num Data Filenames = %i"), numfiles);
					verifyStrings.push_back(msg);
				}
				else {
					verifyStrings.push_back(_T("Warning : More than one image is needed to train - ") + getDataPathName(0, false));
				}

			}
			else if (filetype == FILE_TYPE_RAW) {

				if (numfiles > 1) {
					msg.Format(_T("Pass : Num Data Filenames = %i"), numfiles);
					verifyStrings.push_back(msg);
				}
				else {
					verifyStrings.push_back(_T("Warning : More than one image is needed to train - ") + getDataPathName(0, false));
				}

			}
			else if (filetype == FILE_TYPE_WAV) {

				if (numfiles > 1) {
					msg.Format(_T("Pass : Num Data Filenames = %i"), numfiles);
					verifyStrings.push_back(msg);
				}
				else {
					verifyStrings.push_back(_T("Warning : More than one sound file is needed to train - ") + getDataPathName(0, false));
				}

			}
			else {
				verifyStrings.push_back(_T("Error : Unable to process file type - ") + getDataPathName(0, false));
				rval = false;
			}

		}

	}

	if (rval) {

		CStatus myProgressDlg;
		myProgressDlg.CreateDlg();
		myProgressDlg.setLabel(_T("Verifying Net . . ."));

		myProgressDlg.setProgress(30);

		if (use_validation_file) {
			myProgressDlg.setLabel(_T("Checking validation source . . ."));
			addPanel1String(_T("Checking validation source . . ."));
			if (getValidationPathNamesSize() > 0) {
				if ((getValidationPathName(0) == _T("")) || (getValidationPathName(0) == _T("Null Filename"))) {
					verifyStrings.push_back(_T("Error : Invalid Validation Filename"));
				}
			}
		}
		for (int i = (int)(verifyStrings.size() - 1); i >= 0; i--) {
			msg = verifyStrings[i];
			addErrorString(msg);
		}

		myProgressDlg.setLabel(_T("Checking net connections . . ."));
		addPanel1String(_T("Checking net connections . . ."));
		if (verify.verifyConnects(netData)) {
			if (verify.verifyStrings.size() > 0) {
				for (int i = 0; i < verify.verifyStrings.size(); i++) {
					msg = verify.verifyStrings[i];
					addErrorString(msg);
				}
			}
			else {
				msg = _T("Bug Issue : Connection Check Error");
				addErrorString(msg);
			}
		}
		else {
			for (int i = 0; i < verify.verifyStrings.size(); i++) {
				msg = verify.verifyStrings[i];
				addErrorString(msg);
			}
			rval = false;
		}

		myProgressDlg.setProgress(40);

		if (!checkAvailableMemory()) {
			rval = false;
		}

		if (rval) {
			myProgressDlg.setLabel(_T("Checking net output nodes . . ."));
			addPanel1String(_T("Checking net output nodes . . ."));

			myProgressDlg.setProgress(50);

			if (!checkAvailableMemory()) {
				rval = false;
			}

			if (rval) {

				myProgressDlg.setLabel(_T("Checking net hidden nodes . . ."));
				addPanel1String(_T("Checking net hidden nodes . . ."));
				if (verify.verifyHiddenNodes(netData)) {
					if (verify.verifyStrings.size() > 0) {
						for (int i = 0; i < verify.verifyStrings.size(); i++) {
							msg = verify.verifyStrings[i];
							addErrorString(msg);
						}
					}
				}
				else {
					for (int i = 0; i < verify.verifyStrings.size(); i++) {
						msg = verify.verifyStrings[i];
						addErrorString(msg);
					}
					rval = false;
				}

				if (!checkAvailableMemory()) {
					rval = false;
				}

				if (rval) {

					if (hasConvolution()) {
						myProgressDlg.setProgress(60);
						myProgressDlg.setLabel(_T("Checking convolutional nets . . ."));
						addPanel1String(_T("Checking convolutional nets . . ."));
					}

					myProgressDlg.setProgress(70);

					if (!checkAvailableMemory()) {
						rval = false;
					}

					if (rval) {

						myProgressDlg.setLabel(_T("Checking net process nodes . . ."));
						addPanel1String(_T("Checking net process nodes . . ."));

						myProgressDlg.setProgress(80);

						if (!checkAvailableMemory()) {
							rval = false;
						}

						if (rval) {

							myProgressDlg.setLabel(_T("Checking net input nodes . . ."));
							addPanel1String(_T("Checking net input nodes . . ."));

							//addErrorString(_T("----------------"));
							addErrorString(_T("--- Net Verification ---"));

						}

					}

				}

			}

		}

		myProgressDlg.DestroyDlg();

	}

	if (rval) {

		if (!checkAvailableMemory()) {
			rval = false;
		}
		if (rval) {
			rval = loadNet(randomize);
			if (rval) {
				if (!checkAvailableMemory(false)) {
					rval = false;
				}
			}
		}
	}

	if (rval) {

		if (randomize) {
			for (int i = 0; i < netData.size(); i++) {
				if (netData.isNode(i)) {
					if (netData.getNodeTrainable(i)) {
						nnet.randomizeNode(netData.getID(i));
					}
				}
				else if (netData.isConvLayer(i)) {
					if (netData.getNodeTrainable(i)) {
						nnet.randomizeConvLayer(netData.getID(i));
					}
				}
			}
		}
		else {
			transferWeightsAndBiases();
		}
		pMainWnd->m_wndNodeView.FillFileView();
		nnet.checkValidWeights();
	}

	if (rval) {
		getProjectedMemoryUsage();
	}

	if (rval) {
		//addErrorString(_T(""));
		addErrorString(_T("Net Verification Complete : Ready"));
		addErrorString(_T(""));
	}
	else {
		addErrorString(_T(""));
		addErrorString(_T("Net Verification Complete : Not Ready"));
		addErrorString(_T(""));
	}

	addPanel1String(_T(""));

	verifying = false;

	return(rval);

}




// *********************************************************************************************************************************************
// **  CNetDesignerDoc auxiliary fuctions for netData data initialization  *********************************************************************
// *********************************************************************************************************************************************

int CNetDesignerDoc::getCombinedImages()
{
	//int added_following_check_to_keep_from_repeatedly_processing___will_need_to_be_monitored;
	if (combinedPathnames.size() == 0) {	
		CStatus myProgressDlg;
		myProgressDlg.CreateDlg();
		myProgressDlg.setLabel(_T("Combining train files . . ."));
		myProgressDlg.setProgress(0);
		combinedPathnames.clear();
		int numfiles = getDataPathNamesSize(false);
		int numvalfiles = getValidationPathNamesSize();
		//CString s = _T("");
		//s.Format(_T("getCombinedImages : numfiles = %i"), numfiles);
		//addErrorString(s);
		if (numfiles > 0) {
			int pupdatecnt = 0;
			int pupdatelimit = numfiles / 10;
			for (int j = 0; j < numfiles; j++) {
				if (combinedPathnames.size() > 0) {
					bool found = false;
					for (int k = 0; k < combinedPathnames.size(); k++) {
						if (getDataPathName(j, false) == combinedPathnames[k]) {
							found = true;
							k = (int)combinedPathnames.size();
						}
					}
					if (!found && (getDataPathName(j, false) != _T(""))) {
						combinedPathnames.push_back(getDataPathName(j, false));
					}
				}
				else {
					if (getDataPathName(j, false) != _T("")) {
						combinedPathnames.push_back(getDataPathName(j, false));
					}
				}
				pupdatecnt++;
				if (pupdatecnt >= pupdatelimit) {
					myProgressDlg.setProgress((100 * j) / numfiles);
					pupdatecnt = 0;
				}
			}
		}
		if (performValidation()) {
			if (numvalfiles > 0) {
				if ((numvalfiles > 1) || (getValidationPathName(0) != _T(""))) {
					int pupdatecnt = 0;
					int pupdatelimit = numvalfiles / 10;
					myProgressDlg.setLabel(_T("Combining validation files . . ."));
					for (int j = 0; j < numvalfiles; j++) {
						if (combinedPathnames.size() > 0) {
							bool found = false;
							for (int k = 0; k < combinedPathnames.size(); k++) {
								if (getValidationPathName(j) == combinedPathnames[k]) {
									found = true;
									k = (int)combinedPathnames.size();
								}
							}
							if (!found && (getValidationPathName(j) != _T(""))) {
								combinedPathnames.push_back(getValidationPathName(j));
							}
						}
						else {
							if (getValidationPathName(j) != _T("")) {
								combinedPathnames.push_back(getValidationPathName(j));
							}
						}
						pupdatecnt++;
						if (pupdatecnt >= pupdatelimit) {
							myProgressDlg.setProgress((100 * j) / numvalfiles);
							pupdatecnt = 0;
						}
					}
				}
			}
		}
		myProgressDlg.DestroyDlg();
	}
	return((int)combinedPathnames.size());
}

int CNetDesignerDoc::getCombinedIndex(CString fname)
{
	if (combinedPathnames.size() > 0) {
		for (int i = 0; i < combinedPathnames.size(); i++) {
			if (fname == combinedPathnames[i]) {
				return(i);
			}
		}
	}
	addErrorString(_T("getCombinedIndex unable to find ") + fname);
	return(-1);
}



// *********************************************************************************************************************************************
// **  CNetDesignerDoc nnet train functions  ***************************************************************************************************
// *********************************************************************************************************************************************

void CNetDesignerDoc::iterateTrain()
{
	if (training) {
		if (nnet.update_inc > 0) {
			double delay = 0;
			Timer mtimer;
			mtimer.Start();
			train();
			delay = mtimer.GetDelay();
			if (delay < 0.5) {
				nnet.train_inc = 2 * nnet.train_inc;
				nnet.error_inc = nnet.train_inc / 4;
				if (nnet.error_inc < 1) {
					nnet.error_inc = 1;
				}
				nnet.update_inc--;
			}
			else {
				nnet.update_inc = 0;
			}
		}
		else {
			train();
		}
		updateTrainDisplay();
	}
}

void CNetDesignerDoc::train()
{
	if (nnet.train() < 0) {
		AfxMessageBox(_T("Training stopped due to numerical over/underflow"));
		stopTrain();
	}
}

void CNetDesignerDoc::updateTrainDisplay()
{
	CString tmsg = _T("");
	if ((isnan(nnet.train_error) || isinf(nnet.train_error)) && training) {
		AfxMessageBox(_T("Training stopped due to numerical over/underflow"));
		stopTrain();
		return;
	}

	float nepochs = 0;
	if (nnet.num_filedata_rows > 0) {
		nepochs = (float)nnet.train_count / ((float)nnet.num_filedata_rows);
	}
	if ((best_error < 0) || (nnet.train_error < best_error) || (nepochs < 5.0)) {
		best_epochs = nepochs;
		best_error = nnet.train_error;
	}
	if ((best_validation_error < 0) || (nnet.validation_error < best_validation_error) || (nepochs < 5.0)) {
		best_validation_epochs = nepochs;
		best_validation_error = nnet.validation_error;
	}
	float dsec = (float)train_timer.GetDelay();
	float hrs = (float)(long)(dsec / 3600);
	float min = (float)(long)((dsec / 60) - (hrs * 60));
	float sec = dsec - (hrs * 3600) - (min * 60);
	CString smin = _T("");
	if ((int)min > 9) {
		smin.Format(_T(" : Time %i:%i"), (int)hrs, (int)min);
	}
	else {
		smin.Format(_T(" : Time %i:0%i"), (int)hrs, (int)min);
	}
	CString ssec = _T("");
	if ((int)sec > 9) {
		ssec.Format(_T(":%i"), (int)sec);
	}
	else {
		ssec.Format(_T(":0%i"), (int)sec);
	}
	CString serr = _T("");
	serr.Format(_T(" : RMS Error = %.4lf (%.4lf/%.1f)"), nnet.train_error, best_error, best_epochs);
	CString verr = _T("");
	if (performValidation()) {
		verr.Format(_T(" : RMS Validation Error = %.4lf (%.4lf/%.1f)"), nnet.validation_error, best_validation_error, best_validation_epochs);
	}
	CString serrdelta = _T("");
	if (nnet.train_error_delta == 0) {
		serrdelta = _T(" : Delta Error = 0");
	}
	else if (nnet.train_error_delta < 0) {
		double db = 10 * log10(-nnet.train_error_delta);
		serrdelta.Format(_T(" : Delta Error = (-)%.2lf dB"), db);
	}
	else {
		double db = 10 * log10(nnet.train_error_delta);
		serrdelta.Format(_T(" : Delta Error = (+)%.2lf dB"), db);
	}
	CString sepoch = _T("");
	if (nepochs < 1000) {
		sepoch.Format(_T("Epochs %.2lf"), nepochs);
	}
	else if (nepochs < 1000000) {
		sepoch.Format(_T("Epochs %.2lfk"), nepochs / 1000);
	}
	else if (nepochs < 1000000000) {
		sepoch.Format(_T("Epochs %.2lfM"), nepochs / 1000000);
	}
	else {
		sepoch.Format(_T("Epochs %.2lfB"), nepochs / 1000000000);
	}
	tmsg = _T("Train Count ") + convertNumber(nnet.train_count) + _T(" : ");
	CString weightdata = _T("");
	double mweight = nnet.maxWeights();
	weightdata.Format(_T(" : Weight Max %.2f, RMS %.2f"), mweight, nnet.rmsWeights());
	tmsg = tmsg + sepoch + smin + ssec + serr;
	tmsg += verr + serrdelta + weightdata;
	CString clusterstring = (CString)nnet.getClustersErrorString().c_str();
	if (clusterstring == _T("")) {
		if (mweight > 4.0) {
			tmsg += (CString)nnet.maxWeightString().c_str();
		}
	}
	else {
		tmsg += clusterstring;
	}
	addTrainString(tmsg);

	graph_update_cnt++;
	if (graph_update_cnt >= graph_update_inc) {
		trainDataX.push_back(nepochs);
		trainDataY.push_back(nnet.train_error);
		if (performValidation()) {
			trainDataY2.push_back(nnet.validation_error);
		}
		if (trainDataX.size() > 200) {
			if (trainDataX.size() != trainDataY.size()) {
				addTrainString(_T("Problem with train data not being equal (1)"));
			}
			else {
				vector<double> temptrainDataX = trainDataX;
				vector<double> temptrainDataY = trainDataY;
				trainDataX.clear();
				trainDataY.clear();
				for (int i = 0; i < temptrainDataX.size(); i += 2) {
					trainDataX.push_back(temptrainDataX[i]);
					trainDataY.push_back(temptrainDataY[i]);
				}
				if (performValidation()) {
					if (temptrainDataX.size() != trainDataY2.size()) {
						addTrainString(_T("Problem with train data not being equal (2)"));
					}
					else {
						vector<double> temptrainDataY2 = trainDataY2;
						trainDataY2.clear();
						for (int i = 0; i < temptrainDataX.size(); i += 2) {
							trainDataY2.push_back(temptrainDataY2[i]);
						}
					}
				}
				graph_update_inc = 2 * graph_update_inc;
			}
		}
		updateTrainGraph();
		graph_update_cnt = 0;
	}

	graph_update_cnt2++;
	if (graph_update_cnt2 >= graph_update_inc2) {
		updateDecoderGraph();
		graph_update_cnt2 = 0;
	}

	float tdelta = targetDeltaError;
	if (tdelta < 0) {
		tdelta = -1 * tdelta;
	}
	if ((nnet.train_error_delta > tdelta) || (nnet.train_error_delta < -tdelta)) {
		error_delta_count++;
	}
	else {
		error_delta_count = 0;
	}

	double frate = 0;
	if (dsec > 0) {
		CString srate = _T("");
		CString units = _T(" epochs per min");
		frate = 60 * nepochs / dsec;
		double mrate = frate;
		if (frate > 1000000) {
			mrate = frate / 1000000;
			units = _T("M epochs per min");
		}
		else if (frate > 1000) {
			mrate = frate / 1000;
			units = _T("k epochs per min");
		}
		if (mrate > 100) {
			srate.Format(_T("Train Rate = %.0f"), mrate);
		}
		else if (mrate > 10) {
			srate.Format(_T("Train Rate = %.1f"), mrate);
		}
		else if (mrate > 1) {
			srate.Format(_T("Train Rate = %.2f"), mrate);
		}
		else {
			srate.Format(_T("Train Rate = %.3f"), mrate);
		}
		addPanel1String(srate + units);
	}

	if (nepochs > relative_min_epoch_count) {
		if (nepochs > maxEpochs) {
			AfxMessageBox(_T("Training stopped due to epoch limit"));
			stopTrain();
		}
		else if (dsec > (3600 * trainTime)) {
			AfxMessageBox(_T("Training stopped due to time limit"));
			stopTrain();
		}
		else if (nnet.train_error < targetError) {
			AfxMessageBox(_T("Training stopped - min error limit met"));
			stopTrain();
		}
		else if (checkErrorRising(nnet.train_error_delta, frate)) {
			AfxMessageBox(_T("Training stopped - error increasing"));
			stopTrain();
		}
		else if (error_delta_count > 20) {
			AfxMessageBox(_T("Training stopped - delta error limit met"));
			stopTrain();
		}
		else if (nnet.num_net_clusters > 0) {
			bool cfinished = true;
			for (int i = 0; i < nnet.num_net_clusters; i++) {
				if (!nnet.nclusters[i].finished()) {
					i = nnet.num_net_clusters;
					cfinished = false;
				}
			}
			if (cfinished) {
				AfxMessageBox(_T("Training stopped - decoder net(s) finished"));
				stopTrain();
			}
		}
	}

	if (has_convolution && display_conv_images) {
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->DrawConvolutions(false);
	}

	// Auto save weights
	if (training && auto_save) {
		if ((int)nepochs > delay_epochs1) {
			double terror = 0.0;
			bool validation_response_valid = false;
			if (performValidation()) {
				validation_response_valid = true;
			}
			if (train_response && validation_response && validation_response_valid) {
				terror = (nnet.train_error * nnet.train_error) + (nnet.validation_error * nnet.validation_error);
				if (terror > 0.0) {
					terror = sqrt(terror);
				}
			}
			else if (train_response) {
				terror = nnet.train_error;
			}
			else if (validation_response && validation_response_valid) {
				terror = nnet.validation_error;
			}
			if (terror > 0.0) {
				if ((((int)nepochs > delta_epochs) && (terror < previous_archive_error)) || (terror < (0.8 * previous_archive_error))) {
					previous_archive_error = terror;
					delta_epochs = (int)nepochs + delay_epochs2;
					nnet.archiveWeights();
				}
			}
		}
	}

	if (automationDlg.displayed) {
		if ((nepochs > atepochs) || (dsec > atseconds)) {
			if (evaluate_inputs) {
				if (optimize_train) {
					CString s = _T("");
					s.Format(_T(" : RMS Error = %.4lf (%.4lf)"), nnet.train_error, best_error);
					s = sepoch + smin + ssec + s;
					automationDlg.addInputOptimizationLine(s);
				}
				else if (optimize_validation) {
					CString s = _T("");
					s.Format(_T(" : RMS Validation Error = %.4lf (%.4lf)"), nnet.validation_error, best_validation_error);
					s = sepoch + smin + ssec + s;
					automationDlg.addInputOptimizationLine(s);
				}
			}
			atseconds += 60;
			atepochs += 10;
		}
	}

}

bool CNetDesignerDoc::checkErrorRising(double error_delta, double rate)
{
	double mrate = rate;
	if (mrate < 100) {
		mrate = 100;
	}
	if (error_delta > 0) {
		error_rising_sum = (1.0 + (mrate - 1) * error_rising_sum) / mrate;
	}
	else if (error_delta < 0) {
		error_rising_sum = (-1.0 + (mrate - 1) * error_rising_sum) / mrate;
	}
	if (error_rising_sum > 1.0) {
		return(true);
	}
	return(false);
}

void CNetDesignerDoc::setNetTrainables()
{
	for (int i = 0; i < (int)netData.size(); i++) {
		if (netData.isNode(i)) {
			bool t = netData.getNodeTrainable(i);
			if (netData.getNodeMode(i) == "Output") {
				for (int j = 0; j < nnet.num_outputs; j++) {
					if (netData.getID(i) == nnet.onodes[j].id) {
						nnet.onodes[j].setTrainable(t);
					}
				}
			}
			else if (netData.getNodeMode(i) == "Hidden") {
				for (int j = 0; j < nnet.num_hidden_nodes; j++) {
					if (netData.getID(i) == nnet.hnodes[j].id) {
						nnet.hnodes[j].setTrainable(t);
					}
				}
			}
		}
		else if (netData.isConvLayerOnly(i)) {
			bool t = netData.getNodeTrainable(i);
			for (int j = 0; j < nnet.num_active_convs; j++) {
				if (netData.getID(i) == nnet.newConvs[j].id) {
					nnet.newConvs[j].setTrainable(t);
				}
			}
		}
		else if (netData.isHiddenNetOnly(i)) {
			bool t = netData.getNodeTrainable(i);
			for (int j = 0; j < nnet.num_hidden_nets; j++) {
				if (netData.getID(i) == nnet.hnets[j].id) {
					nnet.hnets[j].setTrainable(t);
				}
			}
		}
		else if (netData.isNetClusterOnly(i)) {
			bool t = netData.getNodeTrainable(i);
			for (int j = 0; j < nnet.num_net_clusters; j++) {
				if (netData.getID(i) == nnet.nclusters[j].id) {
					nnet.nclusters[j].setTrainable(t);
				}
			}
		}
	}
}

void CNetDesignerDoc::softRestart()
{
	if (nnet.softRestart()) {
		nnet.resetUpdateInc();
		setNetTrainables();
		train_timer.Restart();
		float nepochs = 0;
		if (nnet.num_filedata_rows > 0) {
			nepochs = (float)nnet.train_count / ((float)nnet.num_filedata_rows);
		}
		relative_min_epoch_count = nepochs + minEpochs;
		error_rising_sum = 0;
		error_rising_last = 0;
	}
	else {
		addErrorString(_T(""));
		for (int j = 0; j < nnet.verifyStrings.size(); j++) {
			addErrorString(nnet.verifyStrings[j]);
		}
		addErrorString(_T("Net Initialization Not Ready"));
	}
	nnet.checkValidWeights();
}

void CNetDesignerDoc::stopTrain()
{
	train_timer.Hold();
	training = false;
	verifying = false;
	clustering = false;
	nnet.restoreWeights();
	transferWeightsAndBiases(false);
	SetModifiedFlag();
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndNodeView.FillFileView();
}

void CNetDesignerDoc::getTrainExitCriteria()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CWinApp* pApp = AfxGetApp();

	float data = (float)_tstof((LPCTSTR)pMainWnd->getTrainTime());
	if (data >= 0.01) {
		trainTime = data;
		pApp->WriteProfileInt(_T("NetDefaults"), _T("MinTime"), (int)(100 * data));
	}
	else {
		trainTime = 0.01F;
	}

	data = (float)_tstof((LPCTSTR)pMainWnd->getEpochs());
	if (data >= 1) {
		maxEpochs = data;
		pApp->WriteProfileInt(_T("NetDefaults"), _T("MaxEpochs"), (int)data);
	}
	else {
		maxEpochs = 1;
	}

	data = (float)_tstof((LPCTSTR)pMainWnd->getMinEpochs());
	if (data >= 1) {
		minEpochs = data;
		pApp->WriteProfileInt(_T("NetDefaults"), _T("MinEpochs"), (int)data);
	}
	else {
		minEpochs = 1;
	}
	relative_min_epoch_count = minEpochs;

	data = (float)_tstof((LPCTSTR)pMainWnd->getTargetError());
	if (data < 0.000001) {
		data = 0.000001F;
		targetError = data;
	}
	pApp->WriteProfileInt(_T("NetDefaults"), _T("ErrorRate"), (int)(1000000 * data));

	data = (float)_tstof((LPCTSTR)pMainWnd->getTargetDeltaError());
	if (data < 0) {
		data = -1 * data;
	}
	targetDeltaError = data;
	pApp->WriteProfileInt(_T("NetDefaults"), _T("DeltaErrorRate"), (int)data);

	data = (float)_tstof((LPCTSTR)pMainWnd->getUpdateRate());
	if (data >= 1) {
		updateRate = data;
		pApp->WriteProfileInt(_T("NetDefaults"), _T("UpdateRate"), (int)data);
	}
	else {
		updateRate = 1;
	}

}

void CNetDesignerDoc::updateTrainGraph()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (pMainWnd->m_wndGraph.IsVisible()) {
		pMainWnd->m_wndGraph.updateGraph(trainDataX, trainDataY, trainDataY2);
		pMainWnd->m_wndGraph.UpdateWindow();
	}
}

void CNetDesignerDoc::updateDecoderGraph()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (nnet.num_net_clusters > 0) {
		if (pMainWnd->m_wndClusterGraph.IsVisible()) {
			decoderDataX.clear();
			decoderDataY.clear();
			nnet.nclusters[0].getClusteredOutputs(&decoderDataY);
			if (decoderDataY.size() > 0) {
				if (decoderDataY[0].size() > 0) {
					for (int i = 0; i < (int)decoderDataY[0].size(); i++) {
						decoderDataX.push_back((double)i);
					}
					pMainWnd->m_wndClusterGraph.updateGraph(&decoderDataX, &decoderDataY);
					pMainWnd->m_wndClusterGraph.UpdateWindow();
				}
			}
		}
	}
}

double CNetDesignerDoc::timingAnalysis()
{
	if (!training && (nnet.train_count > 0)) {
		int index1 = 0;
		int iterations = 100;
		double time = 0.0;
		train_index mrindex;
		mrindex.dindex = 0;
		mrindex.findex = 0;
		mrindex.sindex = 0;
		Timer mtimer;
		while ((time <= 0) && (iterations < 1000000)) {
			mtimer.Start();
			for (int i = 0; i < iterations; i++) {
				index1 = mrindex.dindex * nnet.num_inputs;
				nnet.loadExternalDataToNet(mrindex);
				nnet.forward(index1, false);
				nnet.copyOutputsToInputs(mrindex.dindex);
				mrindex = nnet.updateTrainIndex(mrindex);
				/*mrindex++;
				if (mrindex >= nnet.num_filedata_rows) {
					mrindex = 0;
				}*/
			}
			time = mtimer.GetDelay();
			if (time <= 0.0) {
				iterations = 10 * iterations;
			}
		}
		if (time > 0) {
			iterations = (int)(2.0 * (double)iterations / time);
			if (iterations > 0) {
				mtimer.Start();
				for (int i = 0; i < iterations; i++) {
					index1 = mrindex.dindex * nnet.num_inputs;
					nnet.loadExternalDataToNet(mrindex);
					nnet.forward(index1, false);
					nnet.copyOutputsToInputs(mrindex.dindex);
					mrindex = nnet.updateTrainIndex(mrindex);
					/*mrindex++;
					if (mrindex >= nnet.num_filedata_rows) {
						mrindex = 0;
					}*/
				}
				time = mtimer.GetDelay();
				double dtime = time / (double)iterations;
				return(dtime);
			}
		}
	}
	return(0.0);
}



// *********************************************************************************************************************************************
// **  CNetDesignerDoc k-means clustering functions  *******************************************************************************************
// *********************************************************************************************************************************************

vector<CString> CNetDesignerDoc::getClusterInputs()
{
	CAuxFunctions func;
	inputStrings = netData.getInputNodeStrings();
	vector<CString> clusterInputs;
	for (int i = 0; i < inputStrings.size(); i++) {
		int nindex = netData.getIndexFromID(inputStrings[i]);
		vector<string> supporting = func.getImmediateSupportingNodes(inputStrings[i]);
		for (int j = 0; j < supporting.size(); j++) {
			int sindex = netData.getIndexFromID(supporting[j]);
			CString lbl = (CString)(inputStrings[i] + " : " + netData.getDataField(sindex) + ", " + netData.getNodePreProcessor(nindex)).c_str();
			clusterInputs.push_back(lbl);
		}
	}
	return(clusterInputs);
}

bool CNetDesignerDoc::setupClusterData(int num_groups)
{
	if (!training && !clustering) {

		CAuxFunctions func;
		clustering = true;
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		pMainWnd->m_wndOutput.ShowPane(true, false, true);
		pMainWnd->m_wndOutput.setTab(1);
		cprocess = 0;
		sortIndex = 0;
		num_cluster_groups = 0;
		cluster_inc = 0;
		cluster_iteration = 0;
		update_cluster_inc = 10;
		clusterDataColumnIndices.clear();
		clusterGroupIndices.clear();
		clusterSumCounts.clear();
		previousClusterSumCounts.clear();
		clusterSums.clear();
		clusterSumSquares.clear();
		clusterSortData.clear();
		clusterMeans.clear();
		clusterDataIndices.clear();
		clustergraphxdata.clear();
		clustergraphydata.clear();

		if (verifyNet(false)) {
			if (nnet.processedData.size() > 0) {

				num_cluster_groups = num_groups;

				for (int i = 0; i < num_cluster_groups; i++) {
					vector<double> tdata;
					clustergraphydata.push_back(tdata);
				}

				// Get indices to data columns
				for (int j = 0; j < inputStrings.size(); j++) {
					int nindex = netData.getIndexFromID(inputStrings[j]);
					vector<string> supporting = func.getImmediateSupportingNodes(inputStrings[j]);
					if ((int)supporting.size() == 1) {
						int sindex = netData.getIndexFromID(supporting[0]);
						string dfield = netData.getDataField(sindex);
						bool found = false;
						for (int i = 0; i < nnet.processedData[0].set_data.size(); i++) {
							//addErrorString(dfield + " :: " + nnet.processedData[0].set_data[i].header);
							if (dfield == nnet.processedData[0].set_data[i].header) {
								clusterDataColumnIndices.push_back(i);
								i = (int)nnet.processedData[0].set_data.size();
								found = true;
							}
						}
						if (!found) {
							addErrorString("setupClusterData : Unable to find input data match for " + inputStrings[j]);
							clustering = false;
							return(false);
						}
					}
					else {

					}
				}

				// Verify that data column indices match number of input strings
				if (inputStrings.size() != clusterDataColumnIndices.size()) {
					char s[200];  sprintf_s(s, "setupClusterData : mismatch between inputString size (%i) and clusterDataColumnIndices size (%i)", (int)inputStrings.size(), (int)clusterDataColumnIndices.size());  
					addErrorString(s);
					clustering = false;
					return(false);
				}

				// Verify that processed columns all have same indexed headers
				if (nnet.processedData.size() > 1) {
					for (int i = 0; i < (nnet.processedData.size()-1); i++) {
						if ((int)nnet.processedData[i].set_data.size() == (int)nnet.processedData[(int64_t)i+1].set_data.size()) {
							for (int j = 0; j < clusterDataColumnIndices.size(); j++) {
								if (nnet.processedData[i].set_data[clusterDataColumnIndices[j]].header != nnet.processedData[(int64_t)i+1].set_data[clusterDataColumnIndices[j]].header) {
									addErrorString("setupClusterData : trainData.set_data headers don't match between files");
									clustering = false;
									return(false);
								}
							}
						}
						else {
							char s[200];  sprintf_s(s, " : %i : %i", (int)nnet.processedData[i].set_data.size(), (int)nnet.processedData[(int64_t)i + 1].set_data.size());
							addErrorString("setupClusterData : trainData.set_data sizes not equal for " + nnet.processedData[i].file_path + " and " + nnet.processedData[(int64_t)i+1].file_path + s);
							clustering = false;
							return(false);
						}
					}
				}

				// Fill clusterDataIndices
				for (int i = 0; i < nnet.processedData.size(); i++) {
					for (int j = 0; j < (clusterDataColumnIndices.size() - 1); j++) {
						if (nnet.processedData[i].set_data[clusterDataColumnIndices[j]].column_data.size() != nnet.processedData[i].set_data[clusterDataColumnIndices[(int64_t)j + 1]].column_data.size()) {
							addErrorString("setupClusterData : trainData.set_data sizes don't match between columns");
							clustering = false;
							return(false);
						}
					}
					for (int j = 0; j < nnet.processedData[i].set_data[clusterDataColumnIndices[0]].column_data.size(); j++) {
						cluster_data_index_struct tstruct;
						tstruct.dataindex = j;
						tstruct.fileindex = i;
						clusterDataIndices.push_back(tstruct);
					}
				}

				//CString s = _T("");  s.Format(_T("setupClusterData : num_cluster_groups = %i"), num_cluster_groups);  AfxMessageBox(s);

				// Initialize clusterGroupIndices to pseudo random values
				for (int i = 0; i < num_cluster_groups; i++) {
					vector<int> tindices;
					clusterGroupIndices.push_back(tindices);
				}

				//s.Format(_T("setupClusterData : clusterDataIndices size = %i : clusterGroupIndices size = %i"), (int)clusterDataIndices.size(), (int)clusterGroupIndices.size());  AfxMessageBox(s);

				int cindex = 0;
				for (int i = 0; i < clusterDataIndices.size(); i++) {
					cindex = (int)(((_int64)num_cluster_groups * (_int64)rand()) / (_int64)RAND_MAX);
					if (cindex >= num_cluster_groups) {
						cindex = 0;
					}
					clusterGroupIndices[cindex].push_back(i);

				}

				getClusterSums();
				resetClusterSortData();


				return(true);
			}
			else {
				addErrorString("setupClusterData : No processed train data");
				clustering = false;
				return(false);
			}
		}
		else {
			addErrorString("setupClusterData : Net verify failure");
			clustering = false;
			return(false);
		}
	}
	else if (training) {
		addErrorString("setupClusterData : Currently training");
	}
	else if (clustering) {
		addErrorString("setupClusterData : Currently running cluster analysis");
	}
	return(false);
}

void CNetDesignerDoc::resetClusterSortData()
{
	clusterSortData.clear();
}

void CNetDesignerDoc::getClusterSums()
{
	previousClusterSumCounts = clusterSumCounts;
	clusterSumCounts.clear();
	clusterSums.clear();
	clusterSumSquares.clear();
	clusterMeans.clear();
	for (int i = 0; i < num_cluster_groups; i++) {
		vector<double> tsums;
		vector<double> tsumsquares;
		vector<double> tmeans;
		for (int j = 0; j < clusterDataColumnIndices.size(); j++) {
			tsums.push_back(0.0);
			tsumsquares.push_back(0.0);
			tmeans.push_back(0.0);
		}
		clusterSumCounts.push_back(0);
		clusterSums.push_back(tsums);
		clusterSumSquares.push_back(tsumsquares);
		clusterMeans.push_back(tmeans);
	}
	for (int i = 0; i < num_cluster_groups; i++) {
		for (int k = 0; k < clusterGroupIndices[i].size(); k++) {
			for (int m = 0; m < clusterDataColumnIndices.size(); m++) {
				int findex = clusterDataIndices[clusterGroupIndices[i][k]].fileindex;
				int dindex = clusterDataIndices[clusterGroupIndices[i][k]].dataindex;
				double data = nnet.processedData[findex].set_data[clusterDataColumnIndices[m]].column_data[dindex];
				clusterSums[i][m] = clusterSums[i][m] + data;
				clusterSumSquares[i][m] = clusterSumSquares[i][m] + (data * data);
			}
			clusterSumCounts[i] = clusterSumCounts[i] + 1;
		}
		if (clusterSumCounts[i] > 0) {
			for (int m = 0; m < clusterDataColumnIndices.size(); m++) {
				clusterMeans[i][m] = clusterSums[i][m] / (double)clusterSumCounts[i];
			}
		}
	}
}

double CNetDesignerDoc::getClusterRMS(int cluster)
{
	double std = 0;
	if (cluster < num_cluster_groups) {
		if ((clusterSumCounts[cluster] > 0) && (clusterDataColumnIndices.size() > 0)) {
			for (int m = 0; m < clusterDataColumnIndices.size(); m++) {
				std += ((clusterSumSquares[cluster][m] - (clusterSums[cluster][m] * clusterSums[cluster][m]) / (double)clusterSumCounts[cluster]) / (double)clusterSumCounts[cluster]);
			}
			std = std / (double)clusterDataColumnIndices.size();
			std = sqrt(std);
		}
	}
	return(std);
}

double CNetDesignerDoc::getClusterSilhouetteScore(int cluster)
{
	// https://en.wikipedia.org/wiki/Silhouette_(clustering)#:~:text=The%20silhouette%20score%20is%20specialized,or%20are%20of%20varying%20sizes
	double sc = 0.0;
	if (cluster < num_cluster_groups) {
		if (clusterGroupIndices[cluster].size() > 0) {
			for (int i = 0; i < clusterGroupIndices[cluster].size(); i++) {
				double a = 0.0;
				double b = 0.0;
				for (int c = 0; c < num_cluster_groups; c++) {
					double d = 0.0;
					double data = 0.0;
					double mean = 0.0;
					double delta = 0.0;
					bool first = true;
					for (int m = 0; m < clusterDataColumnIndices.size(); m++) {
						int findex = clusterDataIndices[clusterGroupIndices[cluster][i]].fileindex;
						int dindex = clusterDataIndices[clusterGroupIndices[cluster][i]].dataindex;
						data = nnet.processedData[findex].set_data[clusterDataColumnIndices[m]].column_data[dindex];	
						mean = clusterMeans[c][m];
						delta = data - mean;
						d += (delta * delta);
					}
					d = sqrt(d);
					if (c == cluster) {
						a = d;
					}
					else {
						if (first) {
							b = d;
							first = false;
						}
						else {
							if (d < b) {
								b = d;
							}
						}
					}
				}
				if (a > b) {
					if (a != 0.0) {
						sc += ((b / a) - 1);
					}
				}
				else if (a < b) {
					if (b != 0.0) {
						sc += (1 - (a / b));
					}
				}
			}
			sc = sc / (double)clusterGroupIndices[cluster].size();
		}
	}
	return(sc);
}

int CNetDesignerDoc::getClusterCount(int cluster)
{
	int rval = 0;
	if (cluster < num_cluster_groups) {
		rval = clusterSumCounts[cluster];
	}
	return(rval);
}

double CNetDesignerDoc::getClusterCountChange()
{
	if (clusterSumCounts.size() == previousClusterSumCounts.size()) {
		int total = 0;
		int difference = 0;
		for (int i = 0; i < num_cluster_groups; i++) {
			total += clusterSumCounts[i];
			int delta = clusterSumCounts[i] - previousClusterSumCounts[i];
			if (delta > 0) {
				difference += delta;
			}
			else {
				difference -= delta;
			}
		}
		if (total > 0) {
			return(100 * (double)difference / (double)total);
		}
	}
	return(0.0);
}

void CNetDesignerDoc::addDistancesFromClusters(int index)
{
	cluster_sort_struct sortData;
	sortData.dataindex = index;
	sortData.bestclusterindex = 0;
	sortData.bestdistance = 0.0;
	for (int i = 0; i < num_cluster_groups; i++) {
		double distance = getRMSDistanceFromCluster(i, index);
		if ((i == 0) || (distance < sortData.bestdistance)) {
			sortData.bestclusterindex = i;
			sortData.bestdistance = distance;
		}
	}
	clusterSortData.push_back(sortData);
}

double CNetDesignerDoc::getRMSDistanceFromCluster(int cluster, int index)
{
	double rval = 0.0;
	vector<double> distances = getDistancesFromCluster(cluster, index);
	if (distances.size() > 0) {
		rval = 0;
		for (int i = 0; i < distances.size(); i++) {
			rval += (distances[i] * distances[i]);
		}
		rval = rval / (double)distances.size();
		rval = sqrt(rval);
	}
	else {
		CString s = _T("");
		s.Format(_T("getRMSDistanceFromCluster : error receiving distances from cluster %i and index %i"), cluster, index);
		updateClusterDisplay(s);
	}
	return(rval);
}

vector<double> CNetDesignerDoc::getDistancesFromCluster(int cluster, int index)
{
	vector<double> rval;
	if (cluster < num_cluster_groups) {
		if (index < clusterDataIndices.size()) {
			if ((clusterMeans[cluster].size() > 0) && (clusterDataColumnIndices.size() > 0)) {
				for (int m = 0; m < clusterDataColumnIndices.size(); m++) {
					int findex = clusterDataIndices[index].fileindex;
					int dindex = clusterDataIndices[index].dataindex;
					double data = nnet.processedData[findex].set_data[clusterDataColumnIndices[m]].column_data[dindex];
					double delta = clusterMeans[cluster][m] - data;
					rval.push_back(delta);
				}
			}
		}
	}
	return(rval);
}

void CNetDesignerDoc::iterateClusterAnalysis()
{
	if (clustering) {
		if (update_cluster_inc > 0) {
			double delay = 0;
			Timer mtimer;
			mtimer.Start();
			cluster();
			delay = mtimer.GetDelay();
			if ((delay < 0.5) && (cluster_inc < 100000)) {
				cluster_inc = 2 * cluster_inc;
				update_cluster_inc--;
			}
			else {
				update_cluster_inc = 0;
			}
		}
		else {
			cluster();
		}
	}
}

void CNetDesignerDoc::cluster()
{
	if (cprocess == 0) {
		if (sortIndex == 0) {
			resetClusterSortData();
		}
		int end = 10;
		for (int i = 0; i < cluster_inc; i++) {
			for (int j = 0; j < end; j++) {
				addDistancesFromClusters(sortIndex);
				sortIndex++;
				if (sortIndex >= clusterDataIndices.size()) {
					cprocess = 1;
					i = cluster_inc;
					j = end;
				}
			}
		}
	}
	else {
		reOrderClusters();
		updateClusterDisplay();
		updateClusterGraph();
		if (!checkClusteringEndCondition()) {
			cluster_iteration++;
		}
		sortIndex = 0;
		cprocess = 0;
	}
}

void CNetDesignerDoc::updateClusterDisplay()
{
	bool colon = false;
	CString s = _T("");
	s.Format(_T("Iteration %i : Change from Previous %.3f%% : "), cluster_iteration, getClusterCountChange());
	for (int i = 0; i < num_cluster_groups; i++) {
		CString s2 = _T("");
		if (getClusterRMS(i) > 0) {
			double density = (double)getClusterCount(i) / getClusterRMS(i);
			s2.Format(_T("Cluster%i density "), i);
			s2 += convertNumber(density);
			s2 += _T(" (");
			s2 += convertNumber((_int64)getClusterCount(i));
			s2 += _T(")");
			if (colon) {
				s += _T(" : ");
			}
			colon = true;
		}
		s += s2;
	}
	updateClusterDisplay(s);
	colon = false;
	s.Format(_T("Iteration %i : Change from Previous %.3f%% : "), cluster_iteration, getClusterCountChange());
	for (int i = 0; i < num_cluster_groups; i++) {
		double sc = getClusterSilhouetteScore(i);
		CString s2 = _T("");
		s2.Format(_T("Cluster%i sc %.3f"), i, sc);
		if (colon) {
			s += _T(" : ");
		}
		colon = true;
		s += s2;
	}
	updateClusterDisplay(s);
}

void CNetDesignerDoc::updateClusterDisplay(CString msg)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->clusterdlg.addMyErrorString(msg);
}

void CNetDesignerDoc::updateClusterGraph()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->clusterdlg.updateGraph(clustergraphxdata, clustergraphydata);
}

void CNetDesignerDoc::stopClustering()
{
	clustering = false;
}

void CNetDesignerDoc::reOrderClusters()
{
	clusterGroupIndices.clear();
	for (int i = 0; i < num_cluster_groups; i++) {
		vector<int> tgroupIndices;
		clusterGroupIndices.push_back(tgroupIndices);
	}
	for (int i = 0; i < (int)clusterSortData.size(); i++) {
		int tcluster = clusterSortData[i].bestclusterindex;
		if (tcluster < (int)clusterGroupIndices.size()) {
			clusterGroupIndices[tcluster].push_back(clusterSortData[i].dataindex);
		}
		else {
			CString s = _T("");
			s.Format(_T("reOrderClusters : reference to invalid bestclusterindex : %i"), tcluster);
			updateClusterDisplay(s);
			return;
		}
	}
	clustergraphxdata.push_back(cluster_iteration);
	for (int i = 0; i < num_cluster_groups; i++) {
		if (i < clustergraphydata.size()) {
			if (getClusterRMS(i) > 0) {
				double density = (double)getClusterCount(i) / getClusterRMS(i);
				clustergraphydata[i].push_back(density);
			}
			else {
				clustergraphydata[i].push_back(0.0);
			}
		}
	}
	getClusterSums();
}

bool CNetDesignerDoc::checkClusteringEndCondition()
{
	if (getClusterCountChange() <= 0.0) {
		clustering = false;
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
		CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
		pView->clusterdlg.setButtonOff();
		updateClusterDisplay(_T("Clustering finished"));
		return(true);
	}
	return(false);
}





// *********************************************************************************************************************************************
// **  CNetDesignerDoc auxiliary functions  ****************************************************************************************************
// *********************************************************************************************************************************************

CString CNetDesignerDoc::getVersion()
{
	CString mversion = _T("1.1.0.");
	mversion += _T("0");
	return(mversion);
}

void CNetDesignerDoc::addErrorString(CString err)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.ShowPane(true, false, true);
	pMainWnd->m_wndOutput.setTab(1);
	pMainWnd->m_wndOutput.AddStatusString(err);
}

void CNetDesignerDoc::addTrainString(CString err)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.ShowPane(true, false, true);
	pMainWnd->m_wndOutput.setTab(0);
	pMainWnd->m_wndOutput.AddTrainString(err);
}

void CNetDesignerDoc::addPanel1String(CString msg)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->SetStatusBarText1(msg);
}

void CNetDesignerDoc::addErrorString(string err)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.ShowPane(true, false, true);
	pMainWnd->m_wndOutput.setTab(1);
	pMainWnd->m_wndOutput.AddStatusString((CString)err.c_str());
}

void CNetDesignerDoc::addTrainString(string err)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->m_wndOutput.ShowPane(true, false, true);
	pMainWnd->m_wndOutput.setTab(0);
	pMainWnd->m_wndOutput.AddTrainString((CString)err.c_str());
}

void CNetDesignerDoc::addPanel1String(string msg)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	pMainWnd->SetStatusBarText1((CString)msg.c_str());
}

wave_check_results CNetDesignerDoc::getMaxWaveMagnitude(bool anyway)
{
	if ((getDataPathType(combinedPathnames) == FILE_TYPE_WAV) && ((processedCombinedPathnames != combinedPathnames) || anyway)) {
		CAuxFunctions func;
		wresults = func.getMaxWaveMagnitude(combinedPathnames, anyway);
		setDataWavMax((int)wresults.files_max);
		setDataWavMin((int)wresults.files_min);
		setDataWavShortest(wresults.files_shortest);
		setDataWavLongest(wresults.files_longest);
		processedCombinedPathnames = combinedPathnames;
		//CString s = _T("");
		//s.Format(_T("CNetDesignerDoc::getMaxWaveMagnitude : max = %.0f"), wresults.files_max);
		//AfxMessageBox(s);
	}
	return(wresults);
}

CString CNetDesignerDoc::convertNumber(_int64 num)
{
	int cnt = 0;
	_int64 tnum = num;
	while (tnum >= 10000) {
		tnum = tnum / 1000;
		cnt++;
	}
	CString rval = _T("");
	rval.Format(_T("%i"), (int)tnum);
	if (cnt == 1) {
		rval += _T("k");
	}
	else if (cnt == 2) {
		rval += _T("M");
	}
	else if (cnt == 3) {
		rval += _T("B");
	}
	else if (cnt == 4) {
		rval += _T("T");
	}
	else if (cnt == 5) {
		rval += _T("Q");
	}
	else if (cnt == 6) {
		rval += _T("MT");
	}
	return(rval);
}

CString CNetDesignerDoc::convertNumber(double num)
{
	int cnt = 0;
	double tnum = num;
	while (tnum >= 1000) {
		tnum = tnum / 1000;
		cnt++;
	}
	CString rval = _T("");
	if (tnum > 1000) {
		rval.Format(_T("%.0f"), tnum);
	}
	else if (tnum > 100) {
		rval.Format(_T("%.1f"), tnum);
	}
	else if (tnum > 10) {
		rval.Format(_T("%.2f"), tnum);
	}
	else if (tnum > 1) {
		rval.Format(_T("%.3f"), tnum);
	}
	else {
		rval.Format(_T("%.4f"), tnum);
	}
	if (cnt == 1) {
		rval += _T("k");
	}
	else if (cnt == 2) {
		rval += _T("M");
	}
	else if (cnt == 3) {
		rval += _T("B");
	}
	else if (cnt == 4) {
		rval += _T("T");
	}
	else if (cnt == 5) {
		rval += _T("Q");
	}
	else if (cnt == 6) {
		rval += _T("MT");
	}
	return(rval);
}






