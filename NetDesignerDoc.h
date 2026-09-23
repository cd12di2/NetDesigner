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

// NetDesignerDoc.h : interface of the CNetDesignerDoc class
//


#pragma once

#include "CNetData.h"
#include "AuxFunctions.h"
#include "Neuron.h"
#include "CStatus.h"
#include "CAutomationDlg.h"
#include "CFileOperations.h"


#define	FILE_TYPE_NONE				0
#define	FILE_TYPE_CSV				1
#define	FILE_TYPE_JPG				2
#define FILE_TYPE_BMP				3
#define FILE_TYPE_WAV				4
#define FILE_TYPE_TXT				5
#define	FILE_TYPE_CSV_WITH_JPG		6
#define FILE_TYPE_RAW				7

//#define HUFFMAN				1
//#define ARITHMETIC			2
//#define ONEHOT				3
//#define WDICTIONARY			4

#define UP					0
#define RIGHT				1
#define DOWN				2
#define LEFT				3
#define UNKNOWN				4


typedef struct placement_data {
	int index = 0;
	CString ID;
	CPoint pt;
} PLACEMENT_DATA;

typedef struct dataIndices {
	bool holdincrement;
	int selectindex;
} DATAINDICES;

/*typedef struct textDictData {
	int count;
	double probability;
	vector<double> hindices;
	CString word;
} TEXTDICTDATA;

typedef struct oneshotParameters {
	CString word;
	vector<CString> related_words;
	vector<double> related_word_counts;
} ONESHITPARAMETERS;*/

typedef struct convParameters {
	int type = 0;
	CString id = _T("");
	CString mode = _T("");
	CString function = _T("");
} CONVPARAMETERS;


class CNetDesignerDoc : public CDocument
{
protected: // create from serialization only
	CNetDesignerDoc() noexcept;
	DECLARE_DYNCREATE(CNetDesignerDoc)

// Attributes
public:

	bool first;
	//bool example;

	int rand_seed;

	CString snap_spacing;
	CString guide_spacing;

	bool arrows;
	bool metalNodes;
	bool dataSourceTrue;
	bool input_data_changed;
	bool show_connect;
	bool has_convolution;
	bool display_conv_images;
	bool select_folder;
	bool nodeview_message_displayed;
	bool cartoon;

	bool auto_save;
	bool train_response;
	bool validation_response;
	int delay_epochs1;
	int delay_epochs2;
	int delta_epochs;
	double previous_archive_error;

	int last_node_index;
	int last_connect_index;
	int last_data_index;
	int last_conv_index;
	int last_attention_index;
	int last_assembly_index;
	int last_net_cluster_index;
	int last_dropout_index;

	int assembly_edit_index;
	int no_sub_assembly_edit_index;
	bool assembly_editable;
	int group_zoom_index = 0;
	int group_zoom_index_max = 3;
	int group_zoom_indices[3];

	int node_limit;
	int num_file_limit;
	int num_data_limit;
	bool flimit_already;
	bool dlimit_already;

	float nradius;
	float nradius2;
	float lwidth;
	int dwidth;

	CString dtype;
	CString dfuncn;
	double deta;
	double dalpha;
	double drand;

	int parallel_processing_test;

	int dclayers;
	int dcnodesperlayer;
	double dcupdateepochs;

	int dkdimension;
	int dvdimension;
	int dmodeldimension;
	double dmsumaddlimit;
	bool dhasSoftmax;

	NetData netData;
	NetData editData;
	EditData backData;
	EditData redoData;

	vector<CString> netInputs;
	vector<CString> netOutputs;

	vector<string> inputStrings;

	int image_width;
	int image_height;
	int default_image_width;
	int default_image_height;
	bool gray_images;

	int conv_type;
	int conv_data_type;

	bool input_image_file_extension_valid;
	CString input_image_file_extension;
	CString defaultCsvJpgPathname;

	vector<int> selectedComponents;

	vector<CString> dataPathnames;
	vector<CString> requestedStringData;
	vector<double> requestedData;

	bool use_validation_percent;
	float validation_percent;

	bool use_validation_file;
	vector<CString> validationPathnames;

	vector<CString> combinedPathnames;
	vector<CString> processedCombinedPathnames;

	float maxEpochs;
	float minEpochs;
	float trainTime;
	float targetError;
	float targetDeltaError;
	float updateRate;

	double sigmoid_leakage;
	double relu_leakage;

	double errorfilter;

	bool training;
	bool verifying;
	bool randomize_net;
	float save_sec;
	float save_sec2;
	float relative_min_epoch_count;
	double error_rising_last;
	int error_delta_count;
	double error_rising_sum;

	double best_epochs;
	double best_error;
	double best_validation_epochs;
	double best_validation_error;

	Timer train_timer;

	bool has_image_conv;
	bool has_series_conv;
	bool has_wave_conv;
	bool has_text_conv;

	int max_series_inputs;

	int minimum_sound_level;
	int sound_data_process_type;
	bool normaiize_file_set;
	wave_check_results wresults;

	int word_count;
	vector<vector<int>> sub_sample_control;
	vector<vector<int>> sub_sample_indices;
	vector<vector<CString>> textWords;

	int dictionary_type;
	bool sub_sample;

	bool randomize_data;

	int train_data_display_index;

	int display_load_step;

	int graph_update_cnt;
	int graph_update_inc;

	int graph_update_cnt2;
	int graph_update_inc2;

	vector<double> trainDataX;
	vector<double> trainDataY;
	vector<double> trainDataY2;

	vector<double> decoderDataX;
	vector<vector<double>> decoderDataY;

	typedef struct conv_order_struct {
		int netindex = 0;
		vector<int> prevconvnets;
	} CONV_ORDER_STRUCT;

	vector<conv_order_struct> convConnections;

	bool new_net;
	bool changed_net;
	bool net_ready;

	int cprocess;
	int sortIndex;
	long cluster_iteration;
	long update_cluster_inc;	
	long cluster_inc;
	bool clustering;
	int num_cluster_groups;
	vector<int> clusterDataColumnIndices;
	vector<int> clusterSumCounts;
	vector<int> previousClusterSumCounts;
	vector<vector<double>> clusterSums;
	vector<vector<double>> clusterSumSquares;
	vector<vector<double>> clusterMeans;
	vector<vector<int>> clusterGroupIndices;
	vector<double> clustergraphxdata;
	vector<vector<double>> clustergraphydata;
	
	typedef struct cluster_data_index_struct {
		int dataindex;
		int fileindex;
	} CLUSTER_DATA_INDEX_STRUCT;
	vector<cluster_data_index_struct> clusterDataIndices;

	typedef struct cluster_sort_struct {
		int dataindex;
		int bestclusterindex;
		double bestdistance;
	} CLUSTER_SORT_STRUCT;
	vector<cluster_sort_struct> clusterSortData;

	int forward_direction;

	bool evaluate_inputs;
	bool optimize_train;
	bool optimize_validation;
	float atepochs;
	float atseconds;
	CAutomationDlg automationDlg;

	vector<assembly_input_counts> assembly_input_output_counts;

	NetBuilder nnet;


// Operations
public:
	void deallocate();

	CString getVersion();

	void addErrorString(CString err);
	void addTrainString(CString err);
	void addPanel1String(CString msg);

	void addErrorString(string err);
	void addTrainString(string err);
	void addPanel1String(string msg);

	void initData();
	void initData1();
	void initData2();

	int addNode(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, int gindex);
	int addNode(int x, int y, CString mode, int gindex);

	int addConnect(int sop, float szoom, int shscroll, int svscroll, CString node_in, CString node_out, int gindex, double sweight, vector<double> sweights, vector<CPoint> points);
	int addConnect(CString node_in, CString node_out, int gindex, double sweight, vector<double> sweights, vector<CPoint> points);

	int addDataSource(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filenames);

	int addConvLayer(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, CString sfunc, double seta, double salpha, double srand, string processor, double lower, double upper, bool strainable, int slayers, int snodesperlayer, int sconvtype, vector<int> dimensions, vector<int> strides, vector<CPoint> locations, vector<string> fields, int gindex);

	int addAttentionLayer(int sop, float szoom, int shscroll, int svscroll, int x, int y, int ksize, int qsize, int vsize, double slimit, bool self, int gindex);

	int addAssembly(int sop, float szoom, int shscroll, int svscroll, int x, int y, int gindex, string reference);

	int addNetCluster(int sop, float szoom, int shscroll, int svscroll, int x, int y, CString mode, CString sfunc, double seta, double salpha, double srand, int snodesperlayer, int sconvtype, int gindex);

	int addDropout(int sop, float szoom, int shscroll, int svscroll, int x, int y, double spercent, int gindex);

	int updateDataSource(CString mode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filepaths);
	
	//int getImageWidth();
	//int getImageHeight();
	int getMinimumImageWidth();
	float getImageAspectRatio();
	int getDataWavMax();
	void setDataWavMax(int mmax);
	int getDataWavMin();
	void setDataWavMin(int mmin);
	int getDataWavShortest();
	void setDataWavShortest(int mshortest);
	int getDataWavLongest();
	void setDataWavLongest(int mlongrst);

	void updateDataPane();
	void setDataImageWidth(int width);
	int getDataImageWidth();
	void setDataImageHeight(int height);
	int getDataImageHeight();
	void setDataImageGray(bool gray);
	bool getDataImageGray();
	void setDataConvType(int type);
	void setDataConvType(BOOL type1, BOOL type2, BOOL type3);
	int getDataConvType();
	int getOutputImageSize();

	CString getNewNodeName();
	CString getNewConnectName();
	CString getNewDataName();
	CString getNewConvLayerName();
	CString getNewAttentionLayerName();
	CString getNewAssemblyName();
	CString getNewClusterName();
	CString getNewDropoutName();

	CString getFirstDataPathName();
		
	void setDataPathNames(vector<CString> name);
	vector<CString> getDataPathNames(bool update = true);
	CString getDataPathName(int index, bool update = true);
	int getDataPathNamesSize(bool update = true);
	void setValidationPathNames(vector<CString> names);
	vector<CString> getValidationPathNames();
	CString getValidationPathName(int index);
	int getValidationPathNamesSize();
	void pushbackValidationPathName(CString pname);
	void clearValidationPathNames();

	int getDataPathType();
	int getDataPathType(vector<CString> mpaths);
	CString getDefaultCSVPathname();

	bool loadFileData();
	vector<CString> getDataFields(vector<int> selectedComponents);

	int getLastNodeIndex();
	int getLastConnectIndex();
	int getLastDataIndex();
	int getLastConvIndex();
	int getLastAttentionIndex();
	int getLastAssemblyIndex();
	int getLastNetClusterIndex();
	int getLastDropoutIndex();

	int checkComponentPlacementReturnIndex(int x, int y, int selection_type, int gindex);
	placement_data checkComponentPlacementReturnString(int x, int y, int selection_type, bool start, int gindex);
	CPoint getInputNodePoint(int index);
	CPoint getInputNodePoint(int index, NetData mdata);
	CPoint getOutputNodePoint(int index);
	CPoint getOutputNodePoint(int index, NetData mdata);
	bool hasConvolution();
	bool alreadyHasInputLayer(int exception);
	bool alreadyHasOutputLayer(int exception);
	bool checkInputSeriesLayerCount(int exception);
	bool alreadyHasInputImageLayer(int exception);
	CString getImageFileExtension();
	CPoint getConvCenter(int index);
	CRect getConvRect(convParameters params, float scale);
	CPoint getAttentionCenter(int index);
	CPoint getAssemblyCenter(int index);
	CPoint getNetClusterCenter(int index);
	CPoint getDropoutCenter(int index);
	CRect getAttentionRect(float scale);
	CRect getAssemblyRect(int nindex, float scale);
	CRect getNetClusterRect(float scale);
	bool isConvImageType(int cindex);
	bool isConvSoundType(int cindex);
	int getConvInputOffset(CRect crect, float scale);
	float getDistanceToLine(CPoint pt, CPoint lpt1, CPoint lpt2);
	convParameters getConvParameters(int index);

	int getAssemblyIndex(int nindex);
	int getNoSubAssemblyIndex(int nindex);
	int getNumAssemblyInputs(int nindex);
	int getNumAssemblyOutputs(int nindex);
	vector<CPoint> getAssemblyInputPoints(int nindex, int radius);
	vector<CPoint> getAssemblyOutputPoints(int nindex, int radius);
	void clearAssemblyInputOutputCounts(int nindex);

	void clearAndSetEditParams(int sop, float szoom, int shscroll, int svscroll);
	void pushEditNodeData(netDataStruct node);
	void pushEditConnectData(CString slabel, CString slbl, CString node_in, CString node_out, int gindex, double sweight, vector<double> sweights, vector<CPoint> points);
	void pushEditDataSource(CString slabel, CString slbl, int sx, int sy, CString smode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filepaths);
	void pushEditConvLayer(netDataStruct layer);
	void pushEditAttentionLayer(netDataStruct layer);
	void pushEditAssembly(netDataStruct layer);
	void pushEditNetCluster(netDataStruct layer);
	void pushEditDropout(netDataStruct layer);

	bool processEditData();
	bool isDataChanged(netDataStruct tdata);
	bool didParametListChanged(int netindex, netDataStruct tdata);

	bool undoLast(float szoom, int shscroll, int svscroll);
	bool redoLast(float szoom, int shscroll, int svscroll);
	editStruct processDataToNet(editStruct data);

	void addToRedo(int sop, float szoom, int shscroll, int svscroll, editStruct data);
	void addToBack(int sop, float szoom, int shscroll, int svscroll, editStruct data);

	bool checkDataSource();

	void updateConvNetConnections();

	double processData(CString dname, double pdata);
	double reProcessData(CString dname, double pdata);
	double reScaleData(CString dname, double pdata);
	double getNodeLowerNormalizationFactor(int index);
	double getNodeUpperNormalizationFactor(int index);

	void getTrainExitCriteria();

	void setNewNet();
	wave_check_results getMaxWaveMagnitude(bool anyway = false);
	bool loadNet(bool randomize);
	int getCombinedImages();
	int getCombinedIndex(CString fname);

	bool verifyNet(bool randomize);
	bool performValidation();
	bool performValidationPercent();
	void getProjectedMemoryUsage();
	bool checkAvailableMemory(bool ask = true);
	void showAvailableMemory();

	void setNetTrainables();

	void stopTrain();
	void softRestart();

	void iterateTrain();
	void train();
	void updateTrainDisplay();
	bool checkErrorRising(double error_delta, double rate);
		
	CString convertNumber(_int64 num);
	CString convertNumber(double num);
	void updateTrainGraph();
	void updateDecoderGraph();
	double timingAnalysis();

	double getBiasFromNode(CString node);
	double getRMSWeightsFromNode(CString node);
	double getWeightFromConnection(CString connect);
	bool activeWeights();

	bool transferWeightsAndBiases(bool netdata_to_nnet = true);

	vector<CString> getClusterInputs();
	bool setupClusterData(int num_groups);
	void resetClusterSortData();
	void getClusterSums();
	double getClusterRMS(int cluster);
	double getClusterSilhouetteScore(int cluster);
	int getClusterCount(int cluster);
	double getClusterCountChange();
	double getRMSDistanceFromCluster(int cluster, int index);
	vector<double> getDistancesFromCluster(int cluster, int index);
	void addDistancesFromClusters(int index);
	void iterateClusterAnalysis();
	void cluster();
	void updateClusterDisplay();
	void updateClusterGraph();
	void updateClusterDisplay(CString msg);
	void stopClustering();
	void reOrderClusters();
	bool checkClusteringEndCondition();

	void getLayerDataConnections(int nindex);

	int getNumNodes();
	int getNumConvolutions();
	int getNumInputLayers();
	int getNumOutputLayers();


	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CNetDesignerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL getPathnamesFromNet();

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL SaveModified();
	virtual void DeleteContents();



};
