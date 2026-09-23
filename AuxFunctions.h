
#pragma once


#include "CStatus.h"
#include "Neuron.h"
#include "CFileOperations.h"
#include "CJSON.h"


#define pi 3.14159265358979323846264338327950288

#define MAXCOLS		18000

typedef double real;
typedef struct { real Re; real Im; } complex;


class Timer
{
private:
	double start_time;
	double hold_time;
public:
	Timer::Timer() { start_time = 0; hold_time = 0; };
	void Start();
	void Hold();
	void Restart();
	double GetDelay();
};


typedef struct layer_point {
	float scale = 0.0;
	CPoint point = CPoint(0, 0);
} LAYER_POINT;

typedef struct imageCheckData {
	int min_width = 0;
	int min_height = 0;
	float optimum_aspect = 0.0;
	vector<CString> paths;
} IMAGECHECKDATA;

typedef struct softmax_layer {
	double centerx = 0;
	double centery = 0;
	vector<int> netIndices;
	vector<CString> nodeIDs;
	bool output = true;
} SOFTMAX_LAYER;


class CAuxFunctions : public CLineParser
{
public:
	bool errordisplayed;

	int num_ones = 4;

	vector<double> derivative_data;

	vector<CString> last_wave_fnames;
	wave_check_results last_wave_check_results;

	CStatus myProgressDlg;

public:
	CAuxFunctions(void);
	~CAuxFunctions(void) {};

	void addErrorString(CString err);
	void addPanel1String(CString msg);
	void addErrorString(string err);

	CString getFileExtension(CString path);
	CString getFileExtension2(CString path);
	CString removePathFromFilename(CString path);
	CString getPathFromFilename(CString path);
	CString removeExtensionFromFilename(CString path);
	bool directoryExists(CString path);
	void RecursiveDelete(CString szPath);
	bool createFolder(CString mfolder);
	bool copyFile(CString from, CString to);

	vector<CString> getCsvFileName(CString filename);
	vector<CString> getDataFileName(CString filename, int ftype);
	CString getDataFolder(CString folder);

	unsigned long getCSVFile(CString filename, unsigned char* filedata, unsigned long maxfilelength, bool display_error = true);
	vector<CString> parseCSVHeaders(unsigned char* filedata, unsigned long filelength);

	long findEndOfLine(long start, unsigned char* filedata, unsigned long filelength);
	long findAfterEndOfLine(long start, unsigned char* filedata, unsigned long filelength);
	long parseLine(long start, long end, unsigned char* filedata, CString* dataset, long maxcols);
	vector<CString> parseLine(long start, long end, unsigned char* filedata, long maxcols = MAXCOLS);
	vector<CString> parseLine(CString line);
	vector<CString> parseLine2(CString line);

	vector<CString> getDataNamesFromFilenames(vector<CString> dataPaths);
	bool isDataInFilenames(CString label, CString dataPathname);
	vector<CString> parseKeyNamesFromFilename(CString dataPathname);

	long getDirectoryFiles(CString path, CString extension, CString* filenames, long maxfiles);
	vector<CString> getDirectoryFiles(CString path, CString extension = _T("\\*.*"));
	vector<CString> getDirectoryAndSubdirectoryFiles(CString path, CString extension = _T("\\*.*"));
	vector<CString> getSelectFileType(vector<CString> files, CString folder);

	bool areAllNumbers(vector<CString> testwords);
	bool areAnyNumbers(vector<CString> testwords);

	CString cleanHeader(CString inheader);
	vector<CString> getHeadersFromDataSource(CString pname, bool display_error = false);
	vector<CString> checkHeadersFromDataSources(vector<CString> pnames);
	vector<CString> getHeadersFromDataSources();
	vector<vector<vector<CString>>> getDataFromDataSources(vector<CString> headers);
	void getStackedColumnDataFromDataSources(CString header, vector<vector<CString>>* sdata, vector<vector<double>>* ddata);

	vector<CString> getHeadersFromJpgSources(vector<vector<CString>> pnames);

	COleDateTime getTimeFromString(CString sdate);

	void setDerivativeData(vector<double> data) { derivative_data = data; };
	double firstDerivativeBackward(int index, int stride, bool relative);
	double secondDerivativeBackward(int index, int stride, bool relative);
	double thirdDerivativeBackward(int index, int stride, bool relative);
	double fourthDerivativeBackward(int index, int stride, bool relative);

	vector<double> getFFT(int index, int N, int downsample, bool normalize, vector<double> data);
	void fft(complex* v, int n);
	void fft(complex* v, int n, complex* tmp);
	void ifft(complex* v, int n, complex* tmp);
	void rebuildWaveform(unsigned long n, unsigned long nn, complex* datain, complex* dataout);
	void OnSaveCSV(complex* data, unsigned long nn);

	void OnSaveCSV(CString fname, vector<CString> lines);

	CString convertNumber(double data, int precision);

	CRect getJpgSize(CString fname);
	imageCheckData checkJpgSources(vector<CString> fnames);
	bool checkRawSources(vector<CString> fnames);

	wave_data getWaveDataFromFile(CString fname);
	wave_check_results getMaxWaveMagnitude(vector<CString> fnames, bool anyway);

	vector<CString> checkWavSources(vector<CString> mpaths);

	vector<CPoint> getLayerPoints2(int radius, int convtype);
	vector<CPoint> getLayerPoints3(int radius, int convtype = -1);
	vector<twoValues> getLayerPoints4(int radius, int convtype = -1);
	vector<CPoint> getAttentionPoints(int radius);
	//vector<CPoint> getAssemblyPoints(int radius);
	vector<CPoint> getNetClusterPoints(int radius);

	imageSizeData getImageSizeData(string id);
	imageSizeData getImageSizeData(int cindex);
	imageSizeData getImageSizeData(string id, NetData ndata);
	imageSizeData getImageSizeData(int sindex, NetData ndata);
	imageSizeData getImageSizeDataFromOutputImage(int sindex, NetData ndata);
	imageSizeData getInputImageSizeWithNoSupportingImage(int sindex, NetData ndata);

	CPoint getInputImageSize();
	bool getImageGray();
	int getNodesPerLayer(int cindex);
	double getUpdateEpochs(int cindex);
	int getNumLayerInputs(int cindex);

	bool displayModelDimensionsForHiddenNet(int cindex);

	int getAttentionDk(int cindex);
	int getAttentionDv(int cindex);
	int getAttentionDm(int cindex);
	double getAttentionSumAddLimit(int cindex);
	int getAttentionInputSize(int cindex);
	int getAttentionOutputSize(int cindex);
	bool hasSoftmaxProcessing(int cindex);

	vector<string> getSupportingConvNets(string id);
	vector<int> getSupportingConvNets(int sindex, NetData ndata);
	vector<string> getImmediateSupportingConvNets(string id, bool from_any = false);
	vector<string> getImmediateSupportingConvNets(int sindex);
	vector<string> getImmediateSupportingLayers(int sindex);
	vector<string> getImmediateSupportingLayersReturnStrings(int sindex);
	vector<int> getImmediateSupportingConvNets(int sindex, NetData ndata, bool from_any = false);
	vector<string> getFollowingConvNets(string id);
	vector<int> getFollowingConvNets(int sindex);
	vector<string> getImmediateFollowingConvNets(string id);
	vector<string> getImmediateFollowingConvNets(int sindex);
	vector<int> getImmediateFollowingConvNets(int sindex, NetData ndata);
	vector<string> getImmediateFollowingAttentionLayers(int sindex);
	vector<string> getFollowingNodes(string id, bool nodes_only = false);
	vector<int> getFollowingNodes(int sindex, NetData ndata, bool nodes_only = false);
	vector<string> getImmediateFollowingNodes(int sindex);
	vector<string> getImmediateFollowingNodesReturnStrings(int sindex);  
	vector<string> getImmediateSupportingNodes(string id);
	vector<string> getImmediateSupportingNodes(int sindex);
	vector<string> getImmediateSupportingNodesReturnStrings(int sindex);	
	vector<int> getImmediateSupportingNodes(int sindex, NetData ndata);

	vector<string> getConvOrder();

	int getConvInputX(string id);
	int getConvInputX(int sindex);
	int getConvInputX(int sindex, NetData ndata);
	int getConvInputY(string id);
	int getConvInputY(int sindex);
	int getConvInputY(int sindex, NetData ndata);
	int getConvOutputX(string id);
	int getConvOutputX(int sindex);
	int getConvOutputX(int sindex, NetData ndata);
	int getConvOutputY(string id);
	int getConvOutputY(int sindex);
	int getConvOutputY(int sindex, NetData ndata);
	int getConvOutputStrideX(string id);
	int getConvOutputStrideX(int sindex);
	int getConvOutputStrideX(int sindex, NetData ndata);
	int getConvOutputStrideY(string id);
	int getConvOutputStrideY(int sindex);
	int getConvOutputStrideY(int sindex, NetData ndata);

	int getImageConvOutputWidth(int cindex);
	int getImageConvOutputHeight(int cindex);
	int getDataConvOutputSize(int cindex);

	int countOutputNodes(int sindex, NetData ndata);
	int countInputNodes(int sindex, NetData ndata);

	bool getConvFromConnectName(string cname);
	bool getConvFromConnectName(CString cname);
	string getConvNameFromConnectName(string cname);
	CString getConvNameFromConnectName(CString cname);
	int getConvNodeFromConnectName(CString cname);
	bool getConvInputFromConnectName(CString cname);
	bool getConvGrayFromConnectName(CString cname);

	bool hasSingleColon(CString cname);
	string getStringBeforeColon(string cname);
	CString getStringBeforeColon(CString cname);
	string getStringAfterColon(string cname);
	CString getStringAfterColon(CString cname);
	int getFirstNumberAfterColon(CString cname);
	int getSecondNumberAfterColon(string cname);
	int getSecondNumberAfterColon(CString cname);
	vector<string> getInputFieldsFromNodeName(int index);
	vector<string> getInputFieldsFromNodeName(string cname);
	vector<string> getInputFieldsFromLayerName(int index);
	vector<string> getInputFieldsFromLayerName(string cname);
	vector<string> getInputFieldsFromLayerNameReturnString(int index);
	vector<string> getOutputFieldsFromLayerName(string cname);
	int getInputWidthFromLayerName(string cname);

	CString getTimeString(float dsec);

	bool areDataNumbers(vector<CString> sdata);
	bool areDataNumbers(vector<vector<CString>> sdata);

	//bool isStringFilename(CString string);
	//void saveLines(CString fname, vector<CString> lines);

	bool checkWindowsVersion();
	bool checkCountry();

	int checkForwardDirection();


	//****************************************************************************
	// netData helper functions

	int getIndexFromID(string mid);
	bool isInputConnectedToOutputNode(CString id);

	string getIDFromNetData(int index);

	int getConvType(int index);
	int getConvType(string id);

	vector<string> getNetInputNodes();
	string getInputNodeName(string node);
	string getInputFieldFromNodeName(string cname);


};













