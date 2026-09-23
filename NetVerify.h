#pragma once

typedef struct conv_size {
	int index;
	int nx;
	int ny;
} CONV_SIZE;

typedef struct conv_forward_parameters {
	int index = 0;
	CString mode = _T("");
	int nxin = 0;
	int nyin = 0;
	int nxout = 0;
	int nyout = 0;
	vector<int> forwardconvnets;
} CONV_FORWARD_PARAMETERS;

typedef struct conv_forward_return_values {
	bool good;
	vector<conv_forward_parameters> rvals;
} CONV_FORWARD_RETURN_VALUES;

typedef struct dims_and_strides {
	int idimxin = 0;
	int idimyin = 0;
	int idimxout = 0;
	int idimyout = 0;
	int istridexin = 0;
	int istrideyin = 0;
} DIMS_AND_STRIDES;


class NetVerify
{
public:
	bool allocated;

	long numcols;
	long numrows;

	CString* data;
	vector<CString> verifyStrings;

	vector<conv_size> conv_sizes;

public:
	NetVerify() {
		numcols = 0;
		numrows = 0;
		data = NULL;
		allocated = false; 
	};

	~NetVerify() { if (allocated) { delete[] data; }; };

	void addErrorString(CString err);

	bool verifyInputNodes(NetData netData, CString fname, vector<CString> dataSourceHeaders);
	bool verifyHiddenNodes(NetData netData);
	bool verifyOutputNodes(NetData netData, CString fname, vector<CString> dataSourceHeaders, vector<CString> dataPaths);
	bool verifyConvNets(NetData netData, CString fname, vector<CString> dataSourceHeaders);
	bool verifyProcessNodes(NetData netData, CString fname, vector<CString> dataSourceHeaders);
	bool verifyConnects(NetData netData);

	CString verifyDataField(CString field, CString fname, vector<CString> dataSourceHeaders);
	bool verifyDataFieldNotZero(CString field, CString fname, vector<CString> dataSourceHeaders);

	bool verifyProcessNode(int index, NetData netData, CString fname, vector<CString> dataSourceHeaders);


	conv_forward_return_values getConvolutionFeedForwardSetup(NetData ndata);

	dims_and_strides getConvDimensions(int index, NetData ndata);



};

