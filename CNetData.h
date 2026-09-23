#pragma once


//#define GCC
#define SERIALIZE


#include <vector>
#include <iostream>
#include "Structures.h"
#include "CJSON.h"


using namespace std;


#pragma once

#define	NONE		0
#define	NODE		1
#define CONNECT		2
#define MULTIPLE	3
#define DATAIN		4
#define DATAOUT		5
#define DATASOURCE	6
#define PREPROCESS	7
#define CONVLAYER	8
#define ATTENTION	9
#define SELFATTENTION	10
#define NETCLUSTER	11
#define DROPOUT		12
#define ASSEMBLY	13

#define UNDO_EDIT	1
#define UNDO_ADD	2
#define UNDO_DELETE	3

#define	SIGMOID		0
#define	RELU		1
#define	TANH		2
#define	SHOCKLEY	3
#define	MAXPOOL		4
#define	MULTIPLY	5
#define	ACOS    	6
#define GAUSSIAN    7
#define	ADD			8
#define	SOFTMAX		9
#define	CONNECTOR	10

#define DATA_CONV	1
#define	IMAGE_CONV	2
#define SOUND_CONV	3
#define TEXT_CONV	4
#define HIDDEN_NET	5	
// careful to not use ATTENTION id for any additions here


class netDataStruct
{
public:
	int nodeType;
	string id;
	string label;
	int x_disp;
	int y_disp;
	int group_index;
	vector<int> intParamList;
	vector<string> intParamListLabels;
	vector<double> doubleParamList;
	vector<string> doubleParamListLabels;
	vector<string> stringParamList;
	vector<string> stringParamListLabels;
	vector<string> paths;
	vector<int> convDimensions;
	vector<int> convStrides;
	vector<twoValues> convLocations;
	vector<double> convBiases;
	vector<double> convWeights;

public:

	netDataStruct()
	{
		clear();
	}

	void clear() {
		nodeType = NONE;
		id = "empty";
		label = "";
		x_disp = 0;
		y_disp = 0;
		group_index = -1;
		intParamList.clear();
		intParamListLabels.clear();
		doubleParamList.clear();
		doubleParamListLabels.clear();
		stringParamList.clear();
		stringParamListLabels.clear();
		paths.clear();
		convDimensions.clear();
		convStrides.clear();
		convLocations.clear();
		convBiases.clear();
		convWeights.clear();
	}

	bool isNode()
	{
		if (nodeType == NODE) {
			return(true);
		}
		return(false);
	}

	bool isConnect()
	{
		if (nodeType == CONNECT) {
			return(true);
		}
		return(false);
	}

	bool isDataSource()
	{
		if (nodeType == DATASOURCE) {
			return(true);
		}
		return(false);
	}

	bool isConvLayer()
	{
		if (nodeType == CONVLAYER) {
			return(true);
		}
		return(false);
	}

	bool isAttentionLayer()
	{
		if ((nodeType == ATTENTION) || (nodeType == SELFATTENTION)) {
			return(true);
		}
		return(false);
	}

	bool isSelfAttentionLayer()
	{
		if (nodeType == SELFATTENTION) {
			return(true);
		}
		return(false);
	}

	bool isAssemblyLayer()
	{
		if (nodeType == ASSEMBLY) {
			return(true);
		}
		return(false);
	}

	bool isNetCluster()
	{
		if (nodeType == NETCLUSTER) {
			return(true);
		}
		return(false);
	}

	bool isDropout()
	{
		if (nodeType == DROPOUT) {
			return(true);
		}
		return(false);
	}

	void setNode(netDataStruct node)
	{
		clear();
		nodeType = node.nodeType;
		id = node.id;
		label = node.label; 
		x_disp = node.x_disp;
		y_disp = node.y_disp;
		group_index = node.group_index;

		intParamList = node.intParamList;
		intParamListLabels = node.intParamListLabels;
		doubleParamList = node.doubleParamList;
		doubleParamListLabels = node.doubleParamListLabels;
		stringParamList = node.stringParamList;
		stringParamListLabels = node.stringParamListLabels;
		paths = node.paths;
		convDimensions = node.convDimensions;
		convStrides = node.convStrides;
		convLocations = node.convLocations;
		convBiases = node.convBiases;
		convWeights = node.convWeights;

	}

	void setConnect(string sid, string slbl, string node_in, string node_out, int gindex, double sweight, vector<double> sweights, vector<twoValues> points)
	{
		clear();
		nodeType = CONNECT;
		id = sid;
		label = slbl;
		group_index = gindex;
		stringParamListLabels.push_back("InputNode");
		stringParamList.push_back(node_in);
		stringParamListLabels.push_back("OutputNode");
		stringParamList.push_back(node_out);
		intParamListLabels.push_back("NumPoints");
		intParamList.push_back((int)points.size());
		for (int i = 0; i < (int)points.size(); i++) {
			intParamListLabels.push_back("XPoint");
			intParamList.push_back(points[i].x);
			intParamListLabels.push_back("YPoint");
			intParamList.push_back(points[i].y);
		}
		doubleParamListLabels.push_back("Weight");
		doubleParamList.push_back(sweight);
		convWeights = sweights;
	}

	void setDataSource(string sid, string slbl, int sx, int sy, string smode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filepaths)
	{
		clear();
		nodeType = DATASOURCE;
		id = sid;
		label = slbl;
		x_disp = sx;
		y_disp = sy;
		stringParamListLabels.push_back("Mode");
		stringParamList.push_back(smode);
		setDataImageWidth(width);
		setDataImageHeight(height);
		setFileMinImageWidth(file_min_width);
		setFileImageAspect(aspect);
		setDataImageGray(gray);
		setDataWavMax(gray);
		if (filepaths.size() > 0) {
			stringParamListLabels.push_back("FilePath");
			stringParamList.push_back(filepaths[0]);
		}
		paths = filepaths;
	}

	void setConvolutionalLayer(netDataStruct layer)
	{
		clear();
		nodeType = CONVLAYER;
		id = layer.id;
		label = layer.label;
		x_disp = layer.x_disp;
		y_disp = layer.y_disp;
		group_index = layer.group_index;

		intParamList = layer.intParamList;
		intParamListLabels = layer.intParamListLabels;
		doubleParamList = layer.doubleParamList;
		doubleParamListLabels = layer.doubleParamListLabels;
		stringParamList = layer.stringParamList;
		stringParamListLabels = layer.stringParamListLabels;
		paths = layer.paths;
		convDimensions = layer.convDimensions;
		convStrides = layer.convStrides;
		convLocations = layer.convLocations;
		convBiases = layer.convBiases;
		convWeights = layer.convWeights;

	}

	void setAttentionLayer(netDataStruct layer)
	{
		clear();
		nodeType = layer.nodeType;
		if ((nodeType != ATTENTION) && (nodeType != SELFATTENTION)) {
			nodeType = ATTENTION;
		}
		id = layer.id;
		label = layer.label;
		x_disp = layer.x_disp;
		y_disp = layer.y_disp;
		group_index = layer.group_index;

		intParamList = layer.intParamList;
		intParamListLabels = layer.intParamListLabels;
		doubleParamList = layer.doubleParamList;
		doubleParamListLabels = layer.doubleParamListLabels;
		stringParamList = layer.stringParamList;
		stringParamListLabels = layer.stringParamListLabels;
		paths = layer.paths;
		convDimensions = layer.convDimensions;
		convStrides = layer.convStrides;
		convLocations = layer.convLocations;
		convBiases = layer.convBiases;
		convWeights = layer.convWeights;

	}

	void setAssembly(netDataStruct layer)
	{
		clear();
		nodeType = ASSEMBLY;
		id = layer.id;
		label = layer.label;
		x_disp = layer.x_disp;
		y_disp = layer.y_disp;
		group_index = layer.group_index;

		intParamList = layer.intParamList;
		intParamListLabels = layer.intParamListLabels;
		doubleParamList = layer.doubleParamList;
		doubleParamListLabels = layer.doubleParamListLabels;
		stringParamList = layer.stringParamList;
		stringParamListLabels = layer.stringParamListLabels;
		paths = layer.paths;
		convDimensions = layer.convDimensions;
		convStrides = layer.convStrides;
		convLocations = layer.convLocations;

		//int here;

	}

	void setNetCluster(netDataStruct layer)
	{
		clear();
		nodeType = NETCLUSTER;
		id = layer.id;
		label = layer.label;
		x_disp = layer.x_disp;
		y_disp = layer.y_disp;
		group_index = layer.group_index;

		intParamList = layer.intParamList;
		intParamListLabels = layer.intParamListLabels;
		doubleParamList = layer.doubleParamList;
		doubleParamListLabels = layer.doubleParamListLabels;
		stringParamList = layer.stringParamList;
		stringParamListLabels = layer.stringParamListLabels;
		paths = layer.paths;
		convDimensions = layer.convDimensions;
		convStrides = layer.convStrides;
		convLocations = layer.convLocations;
		convBiases = layer.convBiases;
		convWeights = layer.convWeights;

		setD(1);

	}

	void setDropout(netDataStruct layer)
	{
		clear();
		nodeType = DROPOUT;
		id = layer.id;
		label = layer.label;
		x_disp = layer.x_disp;
		y_disp = layer.y_disp;
		group_index = layer.group_index;

		intParamList = layer.intParamList;
		intParamListLabels = layer.intParamListLabels;
		doubleParamList = layer.doubleParamList;
		doubleParamListLabels = layer.doubleParamListLabels;
		stringParamList = layer.stringParamList;
		stringParamListLabels = layer.stringParamListLabels;

		intParamListLabels.push_back("ConvType");
		intParamList.push_back(DROPOUT);


	}

	void clearMultipleBiases() {
		if (doubleParamListLabels.size() == doubleParamList.size()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if (doubleParamListLabels[i].substr(0, 5) == "Bias:") {
					addErrorString("clearMultipleBiases : deleting " + doubleParamListLabels[i]);
					doubleParamListLabels.erase(doubleParamListLabels.begin() + i);
					doubleParamList.erase(doubleParamList.begin() + i);
				}
			}
		}
		else {
			char s[200]; sprintf_s(s, "clearMultipleBiases : doubleParamListLabels.size (%i) does not match doubleParamListLabels.size (%i)", (int)doubleParamListLabels.size(), (int)doubleParamList.size());
			addErrorString(s);
		}
	}

	void clearMultipleWeights() {
		if (doubleParamListLabels.size() == doubleParamList.size()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if (doubleParamListLabels[i].substr(0, 7) == "Weight:") {
					addErrorString("clearMultipleWeights : deleting " + doubleParamListLabels[i]);
					doubleParamListLabels.erase(doubleParamListLabels.begin() + i);
					doubleParamList.erase(doubleParamList.begin() + i);
				}
			}
		}
		else {
			char s[200]; sprintf_s(s, "clearMultipleWeights : doubleParamListLabels.size (%i) does not match doubleParamListLabels.size (%i)", (int)doubleParamListLabels.size(), (int)doubleParamList.size());
			addErrorString(s);
		}
	}

	int getType() {
		return(nodeType);
	}

	void setType(int ntype) {
		nodeType = ntype;
	}

	string getID() {
		return(id);
	}

	void setID(string nid) {
		id = nid;
	}

	string getLabel() {
		return(label);
	}

	void setLabel(string nlbl) {
		label = nlbl;
	}

	int getXDisp() {
		return(x_disp);
	}

	void setXDisp(int disp) {
		x_disp = disp;
	}

	int getYDisp() {
		return(y_disp);
	}

	void setYDisp(int disp) {
		y_disp = disp;
	}

	int getGroupIndex() {
		return(group_index);
	}

	void setGroupIndex(int index) {
		group_index = index;
	}

	string getNodeMode()
	{
		if (isNode() || isConvLayer() || isNetCluster() || isAttentionLayer() || isDropout() || isAssemblyLayer()) {
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (i < (int)stringParamListLabels.size()) {
					if (stringParamListLabels[i] == "Mode") {
						return(stringParamList[i]);
					}
				}
			}
			return("None1");
		}
		return("Not Node");
	}

	void setNodeMode(string mode)
	{
		if (isNode() || isConvLayer() || isNetCluster() || isAttentionLayer() || isDropout() || isAssemblyLayer()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "Mode") {
					if (i < (int)stringParamList.size()) {
						stringParamList[i] = mode;
					}
					return;
				}
			}
			stringParamListLabels.push_back("Mode");
			stringParamList.push_back(mode);
			return;
		}
	}

	string getDataField() {
		if (isNode()) {
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (i < (int)stringParamListLabels.size()) {
					if (stringParamListLabels[i] == "DataField") {
						return(stringParamList[i]);
					}
				}
			}
			return("None");
		}
		return("Not Node");
	}

	void setDataField(string field) {
		if (isNode()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "DataField") {
					if (i < (int)stringParamList.size()) {
						stringParamList[i] = field;
					}
					return;
				}
			}
			stringParamListLabels.push_back("DataField");
			stringParamList.push_back(field);
		}
	}

	int getNumDataFields() {
		int rval = 0;
		if (isConvLayer()) {
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (stringParamListLabels[i].substr(0, 10) == "DataFields") {
					rval++;
				}
			}
		}
		return(rval);
	}

	vector<string> getDataFields() {
		vector<string> rval;
		if (isConvLayer()) {
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (stringParamListLabels[i].substr(0, 10) == "DataFields") {
					if (i < (int)stringParamListLabels.size()) {
						rval.push_back(stringParamList[i]);
					}
					else {
						addErrorString("getDataFields : Mismatch in stringParamListLabels and stringParamList sizes for " + id);
						return(rval);
					}
				}
			}
		}
		return(rval);
	}

	void setDataFields(vector<string> fields) {
		if (isConvLayer()) {
			int findex = 0;
			int dindex = -1;
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (stringParamListLabels[i].substr(0, 10) == "DataFields") {
					if ((findex < (int)fields.size()) && (i < (int)stringParamListLabels.size())) {
						stringParamList[i] = fields[findex];
						findex++;
					}
					else if (i >= (int)stringParamList.size()) {
						addErrorString("setDataFields : Mismatch in stringParamListLabels and stringParamList sizes for " + id);
						return;
					}
					else {
						if (i < (int)stringParamList.size()) {
							dindex = i;
						}
						i = (int)stringParamList.size();
					}
				}
			}
			if (findex < (int)fields.size()) {
				for (int i = findex; i < (int)fields.size(); i++) {
					stringParamListLabels.push_back("DataFields");
					stringParamList.push_back(fields[i]);
				}
			}
			if (dindex > 0) {
				bool done = false;
				while (!done) {
					done = true;
					for (int i = dindex; i < (int)stringParamList.size(); i++) {
						if (stringParamListLabels[i].substr(0, 10) == "DataFields") {
							stringParamListLabels.erase(stringParamListLabels.begin() + i);
							stringParamList.erase(stringParamList.begin() + i);
							done = false;
						}
					}
				}
			}
			//addErrorString("setDataFields : " + getNodeMode() + " : " + getDataField() + " : " + getNodeFunction());
		}
	}

	string getNodeFunction() {
		if (isNode() || isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "Function") {
					if (i < (int)stringParamList.size()) {
						return(stringParamList[i]);
					}
				}
			}
			return("None1");
		}
		return("Not Node");
	}

	void setNodeFunction(string function) {
		if (isNode() || isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "Function") {
					if (i < (int)stringParamList.size()) {
						stringParamList[i] = function;
					}
					return;
				}
			}
			stringParamListLabels.push_back("Function");
			stringParamList.push_back(function);
		}
		else {
			addErrorString("setNodeFunction : not node or convlayer");
		}
	}

	string getNodePreProcessor() {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "PreProcessor") {
					if (i < (int)stringParamList.size()) {
						return(stringParamList[i]);
					}
				}
			}
			return("NormLinear");
		}
		return("NormLinear");
	}

	void setNodePreProcessor(string function) {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "PreProcessor") {
					if (i < (int)stringParamList.size()) {
						stringParamList[i] = function;
					}
					return;
				}
			}
			stringParamListLabels.push_back("PreProcessor");
			stringParamList.push_back(function);
			return;
		}
		addErrorString("setNodePreProcessor not setting processor for " + id);
	}

	string getNodePreProcessorTime() {
		if (isNode()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "PreProcessorTime") {
					return(stringParamList[i]);
				}
			}
			return("None");
		}
		return("Not Node");
	}

	void setNodePreProcessorTime(string time) {
		if (isNode()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "PreProcessorTime") {
					if ((int)stringParamList.size() > i) {
						stringParamList[i] = time;
					}
					else {
						addErrorString("setNodePreProcessorTime :: err finding stringParamList");
					}
					return;
				}
			}
			stringParamListLabels.push_back("PreProcessorTime");
			stringParamList.push_back(time);
		}
	}

	double getNodeEta() {
		if (isNode() || isConvLayer() || isNetCluster()) {
			if ((doubleParamList.size() > 0) && (doubleParamListLabels.size() > 0)) {
				if (doubleParamListLabels[0] == "Eta") {
					return((double)doubleParamList[0]);
				}
			}
		}
		return(0.0);
	}

	void setNodeEta(double eta) {
		if (isNode() || isConvLayer() || isNetCluster()) {
			if ((doubleParamList.size() > 0) && (doubleParamListLabels.size() > 0)) {
				if (doubleParamListLabels[0] == "Eta") {
					doubleParamList[0] = eta;
				}
			}
			else {
				doubleParamListLabels.push_back("Eta");
				doubleParamList.push_back(eta);
			}
		}
	}

	double getNodeAlpha() {
		if (isNode() || isConvLayer() || isNetCluster()) {
			if ((doubleParamList.size() > 1) && (doubleParamListLabels.size() > 1)) {
				if (doubleParamListLabels[1] == "Alpha") {
					return((double)doubleParamList[1]);
				}
			}
		}
		return(0.0);
	}

	void setNodeAlpha(double alpha) {
		if (isNode() || isConvLayer() || isNetCluster()) {
			if ((doubleParamList.size() > 1) && (doubleParamListLabels.size() > 1)) {
				if (doubleParamListLabels[1] == "Alpha") {
					doubleParamList[1] = alpha;
				}
			}
			else if ((doubleParamList.size() == 1) && (doubleParamListLabels.size() == 1)) {
				doubleParamListLabels.push_back("Alpha");
				doubleParamList.push_back(alpha);
			}
			else {
				char s[200]; sprintf_s(s, "setNodeAlpha : size error (%i/%i)", (int)doubleParamList.size(), (int)doubleParamListLabels.size());
				addErrorString(s);
			}
		}
	}

	double getNodeRand() {
		if (isNode() || isConvLayer() || isNetCluster()) {
			if ((doubleParamList.size() > 2) && (doubleParamListLabels.size() > 2)) {
				if (doubleParamListLabels[2] == "Rand") {
					return((double)doubleParamList[2]);
				}
			}
		}
		return(0.0);
	}

	void setNodeRand(double rand) {
		if (isNode() || isConvLayer() || isNetCluster()) {
			if ((doubleParamList.size() > 2) && (doubleParamListLabels.size() > 2)) {
				if (doubleParamListLabels[2] == "Rand") {
					doubleParamList[2] = rand;
				}
			}
			else if ((doubleParamList.size() == 2) && (doubleParamListLabels.size() == 2)) {
				doubleParamListLabels.push_back("Rand");
				doubleParamList.push_back(rand);
			}
			else {
				char s[200]; sprintf_s(s, "setNodeRand : size error (%i/%i)", (int)doubleParamList.size(), (int)doubleParamListLabels.size());
				addErrorString(s);
			}
		}
	}

	string getResiduals() {
		if (isConvLayer()) {
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (i < (int)stringParamListLabels.size()) {
					if (stringParamListLabels[i] == "Residuals") {
						return(stringParamList[i]);
					}
				}
			}
			return("No");
		}
		return("Not Layer");
	}

	void setResiduals(string residuals) {
		if (isConvLayer()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "Residuals") {
					if (i < (int)stringParamList.size()) {
						stringParamList[i] = residuals;
					}
					return;
				}
			}
			stringParamListLabels.push_back("Residuals");
			stringParamList.push_back(residuals);
			return;
		}
		addErrorString("setResiduals not setting residuals for " + id);
	}

	string getNormalization() {
		if (isConvLayer()) {
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (i < (int)stringParamListLabels.size()) {
					if (stringParamListLabels[i] == "Normalization") {
						return(stringParamList[i]);
					}
				}
			}
			return("No");
		}
		return("Not Layer");
	}

	void setNormalization(string normalization) {
		if (isConvLayer()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "Normalization") {
					if (i < (int)stringParamList.size()) {
						stringParamList[i] = normalization;
					}
					return;
				}
			}
			stringParamListLabels.push_back("Normalization");
			stringParamList.push_back(normalization);
			return;
		}
		addErrorString("setNormalization not setting normalization for " + id);
	}

	double getNodeLowerNormLimit() {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "LowerNormalizationFactor") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0.0);
	}

	void setNodeLowerNormLimit(double limit) {
		if (isNode() || isConvLayer()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "LowerNormalizationFactor") {
						doubleParamList[i] = limit;
						return;
					}
				}
				doubleParamListLabels.push_back("LowerNormalizationFactor");
				doubleParamList.push_back(limit);
			}
			else {
				addErrorString("setNodeLowerNormLimit : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	double getNodeUpperNormLimit() {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "UpperNormalizationFactor") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(1.0);
	}

	void setNodeUpperNormLimit(double limit) {
		if (isNode() || isConvLayer()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "UpperNormalizationFactor") {
						doubleParamList[i] = limit;
						return;
					}
				}
				doubleParamListLabels.push_back("UpperNormalizationFactor");
				doubleParamList.push_back(limit);
			}
			else {
				addErrorString("setNodeUpperNormLimit : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	double getNodeLowerNormalizationFactor() {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "NormalizationFactor1") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0.0);
	}

	void setNodeLowerNormalizationFactor(double factor) {
		if (isNode() || isConvLayer()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "NormalizationFactor1") {
						doubleParamList[i] = factor;
						return;
					}
				}
				doubleParamListLabels.push_back("NormalizationFactor1");
				doubleParamList.push_back(factor);
			}
			else {
				addErrorString("setNodeLowerNormalizationFactor : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	double getNodeUpperNormalizationFactor() {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "NormalizationFactor2") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(1.0);
	}

	void setNodeUpperNormalizationFactor(double factor) {
		if (isNode() || isConvLayer()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "NormalizationFactor2") {
						doubleParamList[i] = factor;
						return;
					}
				}
				doubleParamListLabels.push_back("NormalizationFactor2");
				doubleParamList.push_back(factor);
			}
			else {
				addErrorString("setNodeUpperNormalizationFactor : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	bool areMultipleBias() {
		bool rval = false;
		int rcnt = 0;
		if (isNode()) {
			for (int j = 0; j < (int)doubleParamListLabels.size(); j++) {
				if (doubleParamListLabels[j].substr(0, 5) == "Bias:") {
					if (!rval && (rcnt > 0)) {
						addErrorString("areMultipleBias : different bias types for " + id);
						return(false);
					}
					rval = true;
					rcnt++;
				}
			}
		}
		return(rval);
	}

	int getNumBias() {
		int rcnt = 0;
		if (isNode()) {
			for (int j = 0; j < (int)doubleParamListLabels.size(); j++) {
				if (doubleParamListLabels[j].substr(0, 5) == "Bias:") {
					rcnt++;
				}
			}
		}
		return(rcnt);
	}

	double getNodeBias() {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "Bias") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0.0);
	}

	double getNodeBias(int node_index) {
		if (isNode() || isConvLayer()) {
			//string label = "";  label.Format(_T("Bias:%i"), node_index);
			char label[200]; sprintf_s(label, "Bias:%i", node_index);
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((label == doubleParamListLabels[i]) && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0.0);
	}

	void setNodeBias(double sbias) {
		if (isNode() || isConvLayer()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "Bias") && (i < (int)doubleParamList.size())) {
					doubleParamList[i] = sbias;
					return;
				}
			}
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				doubleParamListLabels.push_back("Bias");
				doubleParamList.push_back((double)sbias);
			}
			else {
				addErrorString("setNodeBias : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	void setNodeBias(int node_index, double sbias) {
		if (isNode() || isConvLayer()) {
			//string label = _T("");  label.Format(_T("Bias:%i"), node_index);
			char label[200]; sprintf_s(label, "Bias:%i", node_index);
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((label == doubleParamListLabels[i]) && (i < (int)doubleParamList.size())) {
					doubleParamList[i] = sbias;
					return;
				}
			}
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				doubleParamListLabels.push_back(label);
				doubleParamList.push_back((double)sbias);
			}
			else {
				addErrorString("setNodeBias : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	int getNodeNormDisallow() {
		if (isNode()) {
			if ((intParamList.size() > 0) && (intParamListLabels.size() > 0)) {
				if (intParamListLabels[0] == "NormDisallow") {
					return((int)intParamList[0]);
				}
			}
		}
		return(0);
	}

	void setNodeNormDisallow(int allow) {
		if (isNode()) {
			if ((intParamList.size() > 0) && (intParamListLabels.size() > 0)) {
				if (intParamListLabels[0] == "NormDisallow") {
					intParamList[0] = allow;
				}
			}
		}
	}

	bool getNodeTrainable() {
		if (isNode() || isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "Trainable") {
					return(intParamList[i]);
				}
			}
		}
		return(true);
	}

	void setNodeTrainable(bool trainable) {
		if (isNode() || isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "Trainable") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = trainable;
					}
					return;
				}
			}
			intParamListLabels.push_back("Trainable");
			intParamList.push_back(trainable);
		}
	}

	bool getNodeZeroOutput() {
		if (isNode() || isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ZeroOutput") {
					return(intParamList[i]);
				}
			}
		}
		return(false);
	}

	void setNodeZeroOutput(bool zero) {
		if (isNode() || isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ZeroOutput") {
					if (i < (int)intParamList.size()) {
						intParamList[i] = zero;
					}
					return;
				}
			}
			intParamListLabels.push_back("ZeroOutput");
			intParamList.push_back(zero);
		}
	}

	string getNodeIndexMode() {
		if (isNode()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "IndexMode") {
					return(stringParamList[i]);
				}
			}
			return("None");
		}
		return("Not Node");
	}

	void setNodeIndexMode(string mode) {
		if (isNode()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "IndexMode") {
					if ((int)stringParamList.size() > i) {
						stringParamList[i] = mode;
					}
					return;
				}
			}
			stringParamListLabels.push_back("IndexMode");
			stringParamList.push_back(mode);
		}
	}

	int getNodeXIndex() {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "XIndex") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setNodeXIndex(int index) {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "XIndex") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = index;
					}
					else {
						addErrorString("setNodeXIndex :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("XIndex");
			intParamList.push_back(index);
		}
	}

	int getNodeYIndex() {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "YIndex") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setNodeYIndex(int index) {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "YIndex") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = index;
					}
					else {
						addErrorString("setNodeYIndex :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("YIndex");
			intParamList.push_back(index);
		}
	}

	int getNodePreProcessorStride() {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "Stride") {
					return(intParamList[i]);
				}
			}
		}
		return(1);
	}

	void setNodePreProcessorStride(int stride) {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "Stride") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = stride;
					}
					else {
						addErrorString("setNodePreProcessorStride :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("Stride");
			intParamList.push_back(stride);
		}
	}

	int getNodePreProcessorDownSample() {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DownSample") {
					return(intParamList[i]);
				}
			}
		}
		return(1);
	}

	void setNodePreProcessorDownSample(int downsample) {
		if (isNode()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DownSample") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = downsample;
					}
					else {
						addErrorString("setNodePreProcessorDownSample :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("DownSample");
			intParamList.push_back(downsample);
		}
	}

	string getInputNode() {
		if (isConnect()) {
			if ((stringParamList.size() > 0) && (stringParamListLabels.size() > 0)) {
				if (stringParamListLabels[0] == "InputNode") {
					return(stringParamList[0]);
				}
			}
		}
		return("Not Connect");
	}

	void setInputNode(string node) {
		if (isConnect()) {
			if ((stringParamList.size() > 0) && (stringParamListLabels.size() > 0)) {
				if (stringParamListLabels[0] == "InputNode") {
					stringParamList[0] = node;
				}
			}
		}
	}

	string getOutputNode() {
		if (isConnect()) {
			if ((stringParamList.size() > 1) && (stringParamListLabels.size() > 1)) {
				if (stringParamListLabels[1] == "OutputNode") {
					return(stringParamList[1]);
				}
			}
		}
		return("Not Connect");
	}

	void setOutputNode(string node) {
		if (isConnect()) {
			if ((stringParamList.size() > 1) && (stringParamListLabels.size() > 1)) {
				if (stringParamListLabels[1] == "OutputNode") {
					stringParamList[1] = node;
				}
			}
		}
	}

	int getNumPoints() {
		if (isConnect()) {
			if ((intParamList.size() > 0) && (intParamListLabels.size() > 0)) {
				if (intParamListLabels[0] == "NumPoints") {
					return((int)intParamList[0]);
				}
			}
		}
		return(0);
	}

	void setNumPoints(int num) {
		if (isConnect()) {
			if ((intParamList.size() > 0) && (intParamListLabels.size() > 0)) {
				if (intParamListLabels[0] == "NumPoints") {
					intParamList[0] = num;
				}
			}
		}
	}

	twoValues getPoint(int pindex) {
		twoValues mpt; mpt.x = mpt.y = 0;
		if (isConnect()) {
			int mindex = (2 * pindex) + 1;
			if (((int)intParamList.size() > (mindex + 1)) && ((int)intParamListLabels.size() > (mindex + 1))) {
				if ((intParamListLabels[mindex] == "XPoint") && (intParamListLabels[(int64_t)mindex+1] == "YPoint")) {
					mpt.x = intParamList[mindex];
					mpt.y = intParamList[(int64_t)mindex + 1];
					return(mpt);
				}
			}
		}
		return(mpt);
	}

	void setPoint(int pindex, twoValues pt) {
		if (isConnect()) {
			int mindex = (2 * pindex) + 1;
			if (((int)intParamList.size() > (mindex + 1)) && ((int)intParamListLabels.size() > (mindex + 1))) {
				if ((intParamListLabels[mindex] == "XPoint") && (intParamListLabels[(int64_t)mindex + 1] == "YPoint")) {
					intParamList[mindex] = pt.x;
					intParamList[(int64_t)mindex + 1] = pt.y;
				}
			}
		}
	}

	void setPoints(vector<twoValues> points) {
		if (isConnect()) {
			intParamList.clear();
			intParamListLabels.clear();
			intParamListLabels.push_back("NumPoints");
			intParamList.push_back((int)points.size());
			for (int i = 0; i < (int)points.size(); i++) {
				intParamListLabels.push_back("XPoint");
				intParamList.push_back(points[i].x);
				intParamListLabels.push_back("YPoint");
				intParamList.push_back(points[i].y);
			}
		}
	}
	 
	vector<twoValues> getPoints() {
		vector<twoValues> points;
		if (isConnect()) {
			for (int j = 0; j < getNumPoints(); j++) {
				points.push_back(getPoint(j));
			}
		}
		return(points);
	}

	bool areMultipleWeights() {
		bool rval = false;
		int rcnt = 0;
		if (isConnect()) {
			for (int j = 0; j < (int)doubleParamListLabels.size(); j++) {
				if (doubleParamListLabels[j].substr(0, 7) == "Weight:") {
					if (!rval && (rcnt > 0)) {
						addErrorString("areMultipleWeights : different weight types for " + id);
						return(false);
					}
					rval = true;
					rcnt++;
				}
			}
		}
		return(rval);
	}

	int getNumWeights() {
		int rcnt = 0;
		if (isConnect()) {
			for (int j = 0; j < (int)doubleParamListLabels.size(); j++) {
				if (doubleParamListLabels[j].substr(0, 7) == "Weight:") {
					rcnt++;
				}
			}
		}
		return(rcnt);
	}

	double getWeight() {
		if (isConnect()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "Weight") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0);
	}

	double getWeight(int node_index) {
		if (isConnect()) {
			char wname[200];  sprintf_s(wname, "Weight:%i", node_index);
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((wname == doubleParamListLabels[i]) && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0);
	}

	void setWeight(double sweight) {
		if (isConnect()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "Weight") && (i < (int)doubleParamList.size())) {
					doubleParamList[i] = sweight;
					return;
				}
			}
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				doubleParamListLabels.push_back("Weight");
				doubleParamList.push_back(sweight);
			}
			else {
				addErrorString("setWeight : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	void setWeight(int node_index, double sweight) {
		if (isConnect()) {
			char wname[200];  sprintf_s(wname, "Weight:%i", node_index);
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((wname == doubleParamListLabels[i]) && (i < (int)doubleParamList.size())) {
					doubleParamList[i] = sweight;
					return;
				}
			}
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				doubleParamListLabels.push_back(wname);
				doubleParamList.push_back(sweight);
			}
			else {
				addErrorString("setWeight : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	void clearConvBiases() {
		convBiases.clear();
		bool erased = true;
		while (erased) {
			erased = false;
			if ((doubleParamListLabels.size() > 0) && (doubleParamList.size() > 0)) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (i < (int)doubleParamList.size()) {
						if (doubleParamListLabels[i].substr(0, 4) == "Bias") {
							doubleParamListLabels.erase(doubleParamListLabels.begin() + i);
							doubleParamList.erase(doubleParamList.begin() + i);
							i = (int)doubleParamListLabels.size();
							erased = true;
						}
					}
				}
			}
		}
	}

	vector<double> getConvBiases() {
		return(convBiases);
	}

	void setConvBiases(vector<double> sbiases) {
		convBiases = sbiases;
	}

	void clearConvWeights() {
		convWeights.clear();
		bool erased = true;
		while (erased) {
			erased = false;
			if ((doubleParamListLabels.size() > 0) && (doubleParamList.size() > 0)) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (i < (int)doubleParamList.size()) {
						if (doubleParamListLabels[i].substr(0, 6) == "Weight") {
							doubleParamListLabels.erase(doubleParamListLabels.begin() + i);
							doubleParamList.erase(doubleParamList.begin() + i);
							i = (int)doubleParamListLabels.size();
							erased = true;
						}
					}
				}
			}
		}
	}

	vector<double> getConvWeights() {
		return(convWeights);
	}

	void setConvWeights(vector<double> sweights) {
		convWeights = sweights;
	}

	string getDataMode()
	{
		if (isDataSource()) {
			if ((stringParamList.size() > 0) && (stringParamListLabels.size() > 0)) {
				if (stringParamListLabels[0] == "Mode") {
					return(stringParamList[0]);
				}
			}
		}
		return("Not Node");
	}

	void setDataMode(string mode) {
		if (isDataSource()) {
			if ((stringParamList.size() > 0) && (stringParamListLabels.size() > 0)) {
				if (stringParamListLabels[0] == "Mode") {
					stringParamList[0] = mode;
				}
			}
		}
	}

	int getDataImageWidth() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ImageWidth") {
					return(intParamList[i]);
				}
			}
		}
		return(100);
	}

	void setDataImageWidth(int width) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ImageWidth") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = width;
					}
					else {
						addErrorString("setDataImageWidth :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("ImageWidth");
			intParamList.push_back(width);
		}
	}

	int getDataImageHeight() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ImageHeight") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setDataImageHeight(int width) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ImageHeight") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = width;
					}
					else {
						addErrorString("setDataImageHeight :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("ImageHeight");
			intParamList.push_back(width);
		}
	}

	int getRawImageWidth() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "RawImageWidth") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setRawImageWidth(int width) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "RawImageWidth") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = width;
					}
					else {
						addErrorString("setRawImageWidth :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("RawImageWidth");
			intParamList.push_back(width);
		}
	}

	int getRawImageHeight() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "RawImageHeight") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setRawImageHeight(int width) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "RawImageHeight") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = width;
					}
					else {
						addErrorString("setRawImageHeight :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("RawImageHeight");
			intParamList.push_back(width);
		}
	}

	int getFileMinImageWidth() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "FileMinImageWidth") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setFileMinImageWidth(int width) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "FileMinImageWidth") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = width;
					}
					else {
						addErrorString("setFileMinImageWidth :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("FileMinImageWidth");
			intParamList.push_back(width);
		}
	}

	float getFileImageAspect() {
		if (isDataSource()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if (doubleParamListLabels[i] == "FileImageAspect") {
					return((float)doubleParamList[i]);
				}
			}
		}
		return(0.0F);
	}

	void setFileImageAspect(float aspect) {
		if (isDataSource()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if (doubleParamListLabels[i] == "FileImageAspect") {
					if ((int)doubleParamList.size() > i) {
						doubleParamList[i] = (double)aspect;
					}
					else {
						addErrorString("setFileImageAspect :: err finding intParamList");
					}
					return;
				}
			}
			doubleParamListLabels.push_back("FileImageAspect");
			doubleParamList.push_back((double)aspect);
		}
	}

	bool getDataImageGray() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ImageGray") {
					return(intParamList[i]);
				}
			}
		}
		return(false);
	}

	void setDataImageGray(bool gray) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ImageGray") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = gray;
					}
					else {
						addErrorString("setDataImageGray :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("ImageGray");
			intParamList.push_back(gray);
		}
	}

	int getDataWavMax() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataMax") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setDataWavMax(int max) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataMax") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = max;
					}
					else {
						addErrorString("setDataWavMax :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("DataMax");
			intParamList.push_back(max);
		}
	}

	int getDataWavMin() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataMin") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setDataWavMin(int min) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataMin") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = min;
					}
					else {
						addErrorString("setDataWavMin :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("DataMin");
			intParamList.push_back(min);
		}
	}

	int getDataWavShortest() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataShortest") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setDataWavShortest(int shortest) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataShortest") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = shortest;
					}
					else {
						addErrorString("setDataWavShortest :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("DataShortest");
			intParamList.push_back(shortest);
		}
	}

	int getDataWavLongest() {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataLongest") {
					return(intParamList[i]);
				}
			}
		}
		return(0);
	}

	void setDataWavLongest(int longest) {
		if (isDataSource()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "DataLongest") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = longest;
					}
					else {
						addErrorString("setDataWavLongest :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("DataLongest");
			intParamList.push_back(longest);
		}
	}

	vector<string> getFilePaths() {
		vector<string> dpaths;
		if (isDataSource()) {
			if (paths.size() > 0) {
				return(paths);
			}
		}
		return(dpaths);
	}

	void setFilePaths(vector<string> mpaths) {
		if (isDataSource()) {
			paths = mpaths;
		}
	}

	int getConvNumLayers() {
		for (int i = 0; i < (int)intParamListLabels.size(); i++) {
			if (intParamListLabels[i] == "CLayers") {
				return(intParamList[i]);
			}
		}
		return(0);
	}

	void setConvNumLayers(int slayers) {
		if (isConvLayer()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "CLayers") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = slayers;
					}
					else {
						addErrorString("setConvNumLayers :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("CLayers");
			intParamList.push_back(slayers);
		}
	}

	int getConvNodesPerLayer() {
		for (int i = 0; i < (int)intParamListLabels.size(); i++) {
			if (intParamListLabels[i] == "CNodesPerLayer") {
				return(intParamList[i]);
			}
		}
		return(0);
	}

	void setConvNodesPerLayer(int snodes) {
		if (isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "CNodesPerLayer") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = snodes;
					}
					else {
						addErrorString("setConvNodesPerLayer :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("CNodesPerLayer");
			intParamList.push_back(snodes);
		}
	}

	int getConvType() {
		for (int i = 0; i < (int)intParamListLabels.size(); i++) {
			if (intParamListLabels[i] == "ConvType") {
				return(intParamList[i]);
			}
		}
		return(NONE);
	}

	void setConvType(int snodes) {
		if (isConvLayer() || isAttentionLayer() || isDropout() || isAssemblyLayer()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if (intParamListLabels[i] == "ConvType") {
					if ((int)intParamList.size() > i) {
						intParamList[i] = snodes;
					}
					else {
						addErrorString("setConvType :: err finding intParamList");
					}
					return;
				}
			}
			intParamListLabels.push_back("ConvType");
			intParamList.push_back(snodes);
		}
	}

	int getDk() {
		if (isAttentionLayer()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if ((intParamListLabels[i] == "KSize") && (i < (int)intParamList.size())) {
					return((int)intParamList[i]);
				}
			}
		}
		return(1);
	}

	void setDk(int size) {
		if (isAttentionLayer()) {
			if (intParamListLabels.size() == intParamList.size()) {
				for (int i = 0; i < (int)intParamListLabels.size(); i++) {
					if (intParamListLabels[i] == "KSize") {
						intParamList[i] = size;
						return;
					}
				}
				intParamListLabels.push_back("KSize");
				intParamList.push_back(size);
			}
			else {
				addErrorString("setDk : problem with intParamList and intParamListLabels sizes");
			}
		}
	}

	int getD() {
		if (isAttentionLayer() || isConvLayer() || isNetCluster()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if ((intParamListLabels[i] == "QSize") && (i < (int)intParamList.size())) {
					return((int)intParamList[i]);
				}
			}
		}
		return(1);
	}

	void setD(int size) {
		if (isAttentionLayer() || isConvLayer() || isNetCluster()) {
			if (intParamListLabels.size() == intParamList.size()) {
				for (int i = 0; i < (int)intParamListLabels.size(); i++) {
					if (intParamListLabels[i] == "QSize") {
						intParamList[i] = size;
						return;
					}
				}
				intParamListLabels.push_back("QSize");
				intParamList.push_back(size);
			}
			else {
				addErrorString("setD : problem with intParamList and intParamListLabels sizes");
			}
		}
	}

	int getDv() {
		if (isAttentionLayer() || isNetCluster()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if ((intParamListLabels[i] == "VSize") && (i < (int)intParamList.size())) {
					return((int)intParamList[i]);
				}
			}
		}
		return(1);
	}

	void setDv(int size) {
		if (isAttentionLayer() || isNetCluster()) {
			if (intParamListLabels.size() == intParamList.size()) {
				for (int i = 0; i < (int)intParamListLabels.size(); i++) {
					if (intParamListLabels[i] == "VSize") {
						intParamList[i] = size;
						return;
					}
				}
				intParamListLabels.push_back("VSize");
				intParamList.push_back(size);
			}
			else {
				addErrorString("setDv : problem with intParamList and intParamListLabels sizes");
			}
		}
	}

	double getClusterUpdateEpochs() {
		if (isNetCluster()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "ClusterUpdateEpochs") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(5.0);
	}

	void setClusterUpdateEpochs(double epochs) {
		if (isNetCluster()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "ClusterUpdateEpochs") {
						doubleParamList[i] = epochs;
						return;
					}
				}
				doubleParamListLabels.push_back("ClusterUpdateEpochs");
				doubleParamList.push_back(epochs);
			}
			else {
				addErrorString("setClusterUpdateEpochs : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	double getClusterTestError() {
		if (isNetCluster()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "ClusterTestError") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0.100);
	}

	void setClusterTestError(double error) {
		if (isNetCluster()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "ClusterTestError") {
						doubleParamList[i] = error;
						return;
					}
				}
				doubleParamListLabels.push_back("ClusterTestError");
				doubleParamList.push_back(error);
			}
			else {
				addErrorString("setClusterTestError : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	double getClusterSplitVariable() {
		if (isNetCluster()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "ClusterTestDeltaError") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(1.0);
	}

	void setClusterSplitVariable(double error) {
		if (isNetCluster()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "ClusterTestDeltaError") {
						doubleParamList[i] = error;
						return;
					}
				}
				doubleParamListLabels.push_back("ClusterTestDeltaError");
				doubleParamList.push_back(error);
			}
			else {
				addErrorString("setClusterSplitVariable : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	string getPatternSplitMethod() {
		if (isNetCluster()) {
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				if (i < (int)stringParamListLabels.size()) {
					if (stringParamListLabels[i] == "SplitMethod") {
						return(stringParamList[i]);
					}
				}
			}
		}
		return("Median");
	}

	void setPatternSlitMethod(string split) {
		if (isNetCluster()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if (stringParamListLabels[i] == "SplitMethod") {
					if (i < (int)stringParamList.size()) {
						stringParamList[i] = split;
					}
					return;
				}
			}
			stringParamListLabels.push_back("SplitMethod");
			stringParamList.push_back(split);
			return;
		}
		addErrorString("setPatternSlitMethod not setting split for " + id);
	}

	double getDropoutPercent() {
		if (isDropout()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "DropoutPercent") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0.02);
	}

	void setDropoutPercent(double error) {
		if (isDropout()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "DropoutPercent") {
						doubleParamList[i] = error;
						return;
					}
				}
				doubleParamListLabels.push_back("DropoutPercent");
				doubleParamList.push_back(error);
			}
			else {
				addErrorString("setDropoutPercent : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	double getSumAddLimit() {
		if (isAttentionLayer()) {
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				if ((doubleParamListLabels[i] == "SumAddLimit") && (i < (int)doubleParamList.size())) {
					return((double)doubleParamList[i]);
				}
			}
		}
		return(0.01);
	}

	void setSumAddLimit(double limit) {
		if (isAttentionLayer()) {
			if (doubleParamListLabels.size() == doubleParamList.size()) {
				for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
					if (doubleParamListLabels[i] == "SumAddLimit") {
						doubleParamList[i] = limit;
						return;
					}
				}
				doubleParamListLabels.push_back("SumAddLimit");
				doubleParamList.push_back(limit);
			}
			else {
				addErrorString("setSumAddLimit : problem with doubleParamList and doubleParamListLabels sizes");
			}
		}
	}

	bool hasSoftmaxProcessing() {
		if (isAttentionLayer()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if ((intParamListLabels[i] == "Softmax") && (i < (int)intParamList.size())) {
					return((bool)intParamList[i]);
				}
			}
		}
		return(true);
	}

	void setSoftmaxProcessing(bool soft) {
		if (isAttentionLayer()) {
			if (intParamListLabels.size() == intParamList.size()) {
				for (int i = 0; i < (int)intParamListLabels.size(); i++) {
					if (intParamListLabels[i] == "Softmax") {
						intParamList[i] = soft;
						return;
					}
				}
				intParamListLabels.push_back("Softmax");
				intParamList.push_back(soft);
			}
			else {
				addErrorString("setSoftmaxProcessing : problem with intParamList and intParamListLabels sizes");
			}
		}
	}

	bool getTransformerModeQuadratic() {
		if (isAttentionLayer()) {
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				if ((intParamListLabels[i] == "Quadratic") && (i < (int)intParamList.size())) {
					return((bool)intParamList[i]);
				}
			}
		}
		return(true);
	}
	void setTransformerModeQuadratic(bool quad) {
		if (isAttentionLayer()) {
			if (intParamListLabels.size() == intParamList.size()) {
				for (int i = 0; i < (int)intParamListLabels.size(); i++) {
					if (intParamListLabels[i] == "Quadratic") {
						intParamList[i] = quad;
						return;
					}
				}
				intParamListLabels.push_back("Quadratic");
				intParamList.push_back(quad);
			}
			else {
				addErrorString("setTransformerModeQuadratic : problem with intParamList and intParamListLabels sizes");
			}
		}
	}

	string getAssemblyReference() {
		if (isAssemblyLayer()) {
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				if ((stringParamListLabels[i] == "AssemblyReference") && (i < (int)stringParamList.size())) {
					return(stringParamList[i]);
				}
			}
		}
		return("");
	}

	void setAssemblyReference(string reference) {
		if (isAssemblyLayer()) {
			if (stringParamListLabels.size() == stringParamList.size()) {
				for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
					if (stringParamListLabels[i] == "AssemblyReference") {
						stringParamList[i] = reference;
						return;
					}
				}
				stringParamListLabels.push_back("AssemblyReference");
				stringParamList.push_back(reference);
			}
			else {
				addErrorString("setAssemblyReference : problem with stringParamList and stringParamListLabels sizes");
			}
		}
	}

	vector<int> getConvDimensions() {
		vector<int> dimensions;
		if (isConvLayer()) {
			return(convDimensions);
		}
		return(dimensions);
	}

	void setConvDimensions(vector<int> dimensions) {
		if (isConvLayer()) {
			convDimensions = dimensions;
		}
	}

	vector<int> getConvStrides() {
		vector<int> strides;
		if (isConvLayer()) {
			return(convStrides);
		}
		return(strides);
	}

	void setConvStrides(vector<int> strides) {
		if (isConvLayer()) {
			convStrides = strides;
		}
	}

	vector<twoValues> getConvLocations() {
		vector<twoValues> locations;
		if (isConvLayer()) {
			return(convLocations);
		}
		return(locations);
	}

	void setConvLocations(vector<twoValues> locations) {
		if (isConvLayer()) {
			convLocations = locations;
		}
	}

#ifdef SERIALIZE
	void Serialize(CArchive& ar, bool check_only) {

		if (ar.IsStoring())
		{
			CString save_buffer = _T("netrev3start");
			ar << save_buffer;

			save_buffer.Format(_T("%i"), nodeType);
			ar << save_buffer;
			ar << (CString)id.c_str();
			ar << (CString)label.c_str();
			save_buffer.Format(_T("%i"), x_disp);
			ar << save_buffer;
			save_buffer.Format(_T("%i"), y_disp);
			ar << save_buffer;

			save_buffer.Format(_T("%i"), (int)intParamList.size());
			ar << save_buffer;
			for (int i = 0; i < (int)intParamList.size(); i++) {
				save_buffer.Format(_T("%i"), intParamList[i]);
				ar << save_buffer;
			}

			save_buffer.Format(_T("%i"), (int)intParamListLabels.size());
			ar << save_buffer;
			for (int i = 0; i < (int)intParamListLabels.size(); i++) {
				ar << (CString)intParamListLabels[i].c_str();
			}

			save_buffer.Format(_T("%i"), (int)doubleParamList.size());
			ar << save_buffer;
			for (int i = 0; i < (int)doubleParamList.size(); i++) {
				save_buffer.Format(_T("%16e"), doubleParamList[i]);
				ar << save_buffer;
			}

			save_buffer.Format(_T("%i"), (int)doubleParamListLabels.size());
			ar << save_buffer;
			for (int i = 0; i < (int)doubleParamListLabels.size(); i++) {
				ar << (CString)doubleParamListLabels[i].c_str();
			}

			save_buffer.Format(_T("%i"), (int)stringParamList.size());
			ar << save_buffer;
			for (int i = 0; i < (int)stringParamList.size(); i++) {
				ar << (CString)stringParamList[i].c_str();
			}

			save_buffer.Format(_T("%i"), (int)stringParamListLabels.size());
			ar << save_buffer;
			for (int i = 0; i < (int)stringParamListLabels.size(); i++) {
				ar << (CString)stringParamListLabels[i].c_str();
			}

			save_buffer.Format(_T("%i"), (int)paths.size());
			ar << save_buffer;
			for (int i = 0; i < (int)paths.size(); i++) {
				ar << (CString)paths[i].c_str();
			}
			
			save_buffer.Format(_T("%i"), (int)convDimensions.size());
			ar << save_buffer;
			for (int i = 0; i < (int)convDimensions.size(); i++) {
				save_buffer.Format(_T("%i"), convDimensions[i]);
				ar << save_buffer;
			}
			
			save_buffer.Format(_T("%i"), (int)convStrides.size());
			ar << save_buffer;
			for (int i = 0; i < (int)convStrides.size(); i++) {
				save_buffer.Format(_T("%i"), convStrides[i]);
				ar << save_buffer;
			}

			save_buffer.Format(_T("%i"), (int)convLocations.size());
			ar << save_buffer;
			for (int i = 0; i < (int)convLocations.size(); i++) {
				save_buffer.Format(_T("%i"), convLocations[i].x);
				ar << save_buffer;
				save_buffer.Format(_T("%i"), convLocations[i].y);
				ar << save_buffer;
			}

			if (convBiases.size() > 0) {
				save_buffer.Format(_T("%i"), (int)convBiases.size());
				ar << save_buffer;
				for (int i = 0; i < (int)convBiases.size(); i++) {
					save_buffer.Format(_T("%16e"), convBiases[i]);
					ar << save_buffer;
				}
			}

			if (convWeights.size() > 0) {
				if (convBiases.size() <= 0) {
					save_buffer.Format(_T("%i"), 0);	// zero for bias count reload
					ar << save_buffer;
				}
				save_buffer.Format(_T("%i"), (int)convWeights.size());
				ar << save_buffer;
				for (int i = 0; i < (int)convWeights.size(); i++) {
					save_buffer.Format(_T("%.16e"), convWeights[i]);
					ar << save_buffer;
				}
			}

			save_buffer.Format(_T("%i"), group_index);
			ar << save_buffer;

			save_buffer = _T("netrev3end");
			ar << save_buffer;

		}
		else {

			if (check_only) {

				CString save_buffer = _T("");

				ar >> save_buffer;
				int rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
				int rv3 = rv2;
				if (rv2 > 0) {
					ar >> save_buffer;
				}
				if (nodeType != (int)_tstof((LPCTSTR)save_buffer)) { addErrorString("Serialize check finding nodeType inequality : " + id); }

				ar >> save_buffer;  if (id != string(CT2CA(save_buffer))) { addErrorString("Serialize check finding id inequality : " + id); }
				ar >> save_buffer;  if (label != string(CT2CA(save_buffer))) { addErrorString("Serialize check finding label inequality : " + id); }
				ar >> save_buffer;  if (x_disp != (int)_tstof((LPCTSTR)save_buffer)) { addErrorString("Serialize check finding x_disp inequality : " + id); }
				ar >> save_buffer;  if (y_disp != (int)_tstof((LPCTSTR)save_buffer)) { addErrorString("Serialize check finding y_disp inequality : " + id); }

				ar >> save_buffer;
				int size = (int)_tstof((LPCTSTR)save_buffer);
				if (size == (int)intParamList.size()) {
					for (int i = 0; i < size; i++) {
						ar >> save_buffer;  if (intParamList[i] != (int)_tstof((LPCTSTR)save_buffer)) { addErrorString("Serialize check finding intParamList inequality : " + id); }
					}
				}
				else {
					addErrorString("Serialize check finding intParamList size inequality : " + id);
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				if (size == (int)intParamListLabels.size()) {
					for (int i = 0; i < size; i++) {
						ar >> save_buffer;  if (intParamListLabels[i] != string(CT2CA(save_buffer))) { addErrorString("Serialize check finding intParamListLabels inequality : " + id); };
					}
				}
				else {
					addErrorString("Serialize check finding intParamListLabels size inequality : " + id);
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				if (size == (int)doubleParamList.size()) {
					for (int i = 0; i < size; i++) {
						ar >> save_buffer;  
						/*if (doubleParamList[i] != (double)_tstof((LPCTSTR)save_buffer)) {
							char s[200];  sprintf_s(s, "doubleParamList[%i] = %f : save_buffer = ", i, doubleParamList[i]);
							addErrorString("Serialize check finding doubleParamList inequality" + string(s) + string(CT2CA(save_buffer)));
						}*/
					}
				}
				else {
					addErrorString("Serialize check finding doubleParamList size inequality : " + id);
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				if (size == (int)doubleParamListLabels.size()) {
					for (int i = 0; i < size; i++) {
						ar >> save_buffer;  if (doubleParamListLabels[i] != string(CT2CA(save_buffer))) { addErrorString("Serialize check finding doubleParamListLabels inequality : " + id); }
					}
				}
				else {
					addErrorString("Serialize check finding doubleParamListLabels size inequality : " + id);
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				if (size == (int)stringParamList.size()) {
					for (int i = 0; i < size; i++) {
						ar >> save_buffer;  if (stringParamList[i] != string(CT2CA(save_buffer))) { addErrorString("Serialize check finding stringParamList inequality : " + id); }
					}
				}
				else {
					addErrorString("Serialize check finding stringParamList size inequality : " + id);
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				if (size == (int)stringParamListLabels.size()) {
					for (int i = 0; i < size; i++) {
						ar >> save_buffer;  if (stringParamListLabels[i] != string(CT2CA(save_buffer))) { addErrorString("Serialize check finding stringParamListLabels inequality : " + id); }
					}
				}
				else {
					addErrorString("Serialize check finding stringParamListLabels size inequality : " + id);
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				if (size == (int)paths.size()) {
					for (int i = 0; i < size; i++) {
						ar >> save_buffer;  if (paths[i] != string(CT2CA(save_buffer))) { addErrorString("Serialize check finding paths inequality"); }
					}
				}
				else {
					addErrorString("Serialize check finding paths size inequality : " + id);
				}

				if (rv2 > 0) {

					ar >> save_buffer;
					rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						if (size == (int)convDimensions.size()) {
							int index = 0;
							while ((index < size) && (rv2 == 0)) {
								ar >> save_buffer;
								rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
								if (rv2 == 0) {
									if (convDimensions[index] != (int)_tstof((LPCTSTR)save_buffer)) { addErrorString("Serialize check finding convDimensions inequality : " + id); }
									index++;
								}
							}
						}
						else {
							addErrorString("Serialize check finding convDimensions size inequality : " + id);
						}
					}

					ar >> save_buffer;
					rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						if (size == (int)convStrides.size()) {
							int index = 0;
							while ((index < size) && (rv2 == 0)) {
								ar >> save_buffer;
								rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
								if (rv2 == 0) {
									if (convStrides[index] != (int)_tstof((LPCTSTR)save_buffer)) { addErrorString("Serialize check finding convStrides inequality : " + id); }
									index++;
								}
							}
						}
						else {
							addErrorString("Serialize check finding convStrides size inequality : " + id);
						}
					}

					ar >> save_buffer;
					rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						if (size == (int)convLocations.size()) {
							int index = 0;
							while ((index < size) && (rv2 == 0)) {
								ar >> save_buffer;
								rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
								if (rv2 == 0) {
									twoValues pt;
									pt.x = (int)_tstof((LPCTSTR)save_buffer);
									ar >> save_buffer;
									rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
									if (rv2 == 0) {
										pt.y = (int)_tstof((LPCTSTR)save_buffer);
										if ((pt.x != convLocations[index].x) || (pt.y != convLocations[index].y)) {
											addErrorString("Serialize check finding convLocations inequality : " + id);
										}
										index++;
									}
								}
							}
						}
						else {
							addErrorString("Serialize check finding convLocations size inequality : " + id);
						}
					}

					bool gfound = false;

					ar >> save_buffer;
					rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						int index = 0;
						while ((index < size) && (rv2 == 0)) {
							ar >> save_buffer;
							rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
							if (rv2 == 0) {
								//convBiases.push_back((double)_tstof((LPCTSTR)save_buffer));
								index++;
							}
						}
						if ((rv3 == 2) && (index == 0)) {
							group_index = size;
							gfound = true;
						}
					}

					if (rv2 == 0) {
						ar >> save_buffer;
						rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
						if (rv2 == 0) {
							size = (int)_tstof((LPCTSTR)save_buffer);
							int index = 0;
							while ((index < size) && (rv2 == 0)) {
								ar >> save_buffer;
								rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
								if (rv2 == 0) {
									//convWeights.push_back((double)_tstof((LPCTSTR)save_buffer));
									index++;
								}
							}
							if ((rv3 == 2) && (index == 0)) {
								group_index = size;
								gfound = true;
							}
						}
					}

					if (rv2 >= 0) {

						if (rv3 == 2) {
							ar >> save_buffer;  if (group_index != (int)_tstof((LPCTSTR)save_buffer)) { addErrorString("Serialize check finding group_index inequality"); }
						}

						while (rv2 >= 0) {
							ar >> save_buffer;
							rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
						}

					}
					else if (rv3 == 2) {
						if (!gfound) {
							addErrorString("Unable to load group_index for " + id);
						}
					}

				}
				else {
					rv2 = -1;
				}

				if (intParamList.size() != intParamListLabels.size()) {
					char s[200]; sprintf_s(s, "netDataStruct : Serialize : intParamList.size (%i) != intParamListLabels.size (%i)", (int)intParamList.size(), (int)intParamListLabels.size());
					addErrorString(s);
				}

				if (doubleParamList.size() != doubleParamListLabels.size()) {
					char s[200]; sprintf_s(s, "netDataStruct : Serialize : doubleParamList.size (%i) != doubleParamListLabels.size (%i)", (int)doubleParamList.size(), (int)doubleParamListLabels.size());
					addErrorString(s);
				}

				if (stringParamList.size() != stringParamListLabels.size()) {
					char s[200]; sprintf_s(s, "netDataStruct : Serialize : stringParamList.size (%i) != stringParamListLabels.size (%i)", (int)stringParamList.size(), (int)stringParamListLabels.size());
					addErrorString(s);
				}

			}
			else {

				clear();

				CString save_buffer = _T("");

				ar >> save_buffer;
				int rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
				int rv3 = rv2;
				if (rv2 > 0) {
					ar >> save_buffer;
				}
				nodeType = (int)_tstof((LPCTSTR)save_buffer);

				ar >> save_buffer;  id = string(CT2CA(save_buffer));
				ar >> save_buffer;  label = string(CT2CA(save_buffer));
				ar >> save_buffer;  x_disp = (int)_tstof((LPCTSTR)save_buffer);
				ar >> save_buffer;  y_disp = (int)_tstof((LPCTSTR)save_buffer);

				ar >> save_buffer;
				int size = (int)_tstof((LPCTSTR)save_buffer);
				for (int i = 0; i < size; i++) {
					ar >> save_buffer;  intParamList.push_back((int)_tstof((LPCTSTR)save_buffer));
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				for (int i = 0; i < size; i++) {
					ar >> save_buffer;  intParamListLabels.push_back(string(CT2CA(save_buffer)));
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				for (int i = 0; i < size; i++) {
					ar >> save_buffer;  doubleParamList.push_back((double)_tstof((LPCTSTR)save_buffer));
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				for (int i = 0; i < size; i++) {
					ar >> save_buffer;  doubleParamListLabels.push_back(string(CT2CA(save_buffer)));
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				for (int i = 0; i < size; i++) {
					ar >> save_buffer;  stringParamList.push_back(string(CT2CA(save_buffer)));
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				for (int i = 0; i < size; i++) {
					ar >> save_buffer;  stringParamListLabels.push_back(string(CT2CA(save_buffer)));
				}

				ar >> save_buffer;
				size = (int)_tstof((LPCTSTR)save_buffer);
				for (int i = 0; i < size; i++) {
					ar >> save_buffer;  paths.push_back(string(CT2CA(save_buffer)));
				}

				if (rv2 > 0) {

					ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						int index = 0;
						while ((index < size) && (rv2 == 0)) {
							ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
							if (rv2 == 0) {
								convDimensions.push_back((int)_tstof((LPCTSTR)save_buffer));
								index++;
							}
						}
					}

					ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						int index = 0;
						while ((index < size) && (rv2 == 0)) {
							ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
							if (rv2 == 0) {
								convStrides.push_back((int)_tstof((LPCTSTR)save_buffer));
								index++;
							}
						}
					}

					ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						int index = 0;
						while ((index < size) && (rv2 == 0)) {
							ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
							if (rv2 == 0) {
								twoValues pt;
								pt.x = (int)_tstof((LPCTSTR)save_buffer);
								ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
								if (rv2 == 0) {
									pt.y = (int)_tstof((LPCTSTR)save_buffer);
									convLocations.push_back(pt);
									index++;
								}
							}
						}
					}

					bool gfound = false;

					ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
					if (rv2 == 0) {
						size = (int)_tstof((LPCTSTR)save_buffer);
						int index = 0;
						while ((index < size) && (rv2 == 0)) {
							ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
							if (rv2 == 0) {
								convBiases.push_back((double)_tstof((LPCTSTR)save_buffer));
								index++;
							}
						}
						if ((rv3 == 2) && (index == 0)) {
							group_index = size;
							gfound = true;
						}
					}

					if (rv2 == 0) {
						ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
						if (rv2 == 0) {
							size = (int)_tstof((LPCTSTR)save_buffer);
							int index = 0;
							while ((index < size) && (rv2 == 0)) {
								ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
								if (rv2 == 0) {
									convWeights.push_back((double)_tstof((LPCTSTR)save_buffer));
									index++;
								}
							}
							if ((rv3 == 2) && (index == 0)) {
								group_index = size;
								gfound = true;
							}
						}
					}

					if (rv2 >= 0) {

						if (rv3 == 2) {
						ar >> save_buffer;  group_index = (int)_tstof((LPCTSTR)save_buffer);
						}

						while (rv2 >= 0) {
							ar >> save_buffer;  rv2 = checkSerializeReadData(string(CT2CA(save_buffer)));
						}

					}
					else if (rv3 == 2) {
						if (!gfound) {
							addErrorString("Unable to load group_index for " + id);
						}
					}

				}
				else {
					rv2 = -1;
				}

				if (intParamList.size() != intParamListLabels.size()) {
					char s[200]; sprintf_s(s, "netDataStruct : Serialize : intParamList.size (%i) != intParamListLabels.size (%i)", (int)intParamList.size(), (int)intParamListLabels.size());
					addErrorString(s);
				}

				if (doubleParamList.size() != doubleParamListLabels.size()) {
					char s[200]; sprintf_s(s, "netDataStruct : Serialize : doubleParamList.size (%i) != doubleParamListLabels.size (%i)", (int)doubleParamList.size(), (int)doubleParamListLabels.size());
					addErrorString(s);
				}

				if (stringParamList.size() != stringParamListLabels.size()) {
					char s[200]; sprintf_s(s, "netDataStruct : Serialize : stringParamList.size (%i) != stringParamListLabels.size (%i)", (int)stringParamList.size(), (int)stringParamListLabels.size());
					addErrorString(s);
				}

			}

		}

	}

	int checkSerializeReadData(string input)
	{
		if (input == "netrev2start") {
			//addErrorString("checkSerializeReadData : netrev2start : " + id);
			return(1);
		}
		else if (input == "netrev2end") {
			//addErrorString("checkSerializeReadData : netrev2end : " + id);
			return(-1);
		}
		else if (input == "netrev3start") {
			//addErrorString("checkSerializeReadData : netrev3start : " + id);
			return(2);
		}
		else if (input == "netrev3end") {
			//addErrorString("checkSerializeReadData : netrev3end : " + id);
			return(-1);
		}
		//addErrorString("checkSerializeReadData : " + input + " : " + id);
		return(0);
	}
#endif

	bool Serialize(CJSON* myjson, bool storing) 
	{
		if (storing) {

			myjson->openCollection(id);

			myjson->addIntKey("nodeType", nodeType, true);
			myjson->addStringKey("id", id, true);
			myjson->addStringKey("label", label, true);
			myjson->addIntKey("x_disp", x_disp, true);
			myjson->addIntKey("y_disp", y_disp, true);
			myjson->addIntKey("group_index", group_index, true);

			if (intParamList.size() == intParamListLabels.size()) {
				myjson->addIntKey("intParamListSize", (int)intParamList.size(), true);
				if (intParamList.size() > 0) {
					myjson->openCollection("intParamList");
					for (int i = 0; i < (int)intParamList.size(); i++) {
						if (i < ((int)intParamList.size() - 1)) {
							myjson->addIntKey(intParamListLabels[i], intParamList[i], true);
						}
						else {
							myjson->addIntKey(intParamListLabels[i], intParamList[i], false);
						}
					}
					myjson->closeCollection(true, true);
				}
			}
			else {
				addErrorString("Serialize for node " + id + " returning due to intParamist size mismatch");
				return(false);
			}

			if (doubleParamList.size() == doubleParamListLabels.size()) {
				myjson->addIntKey("doubleParamListSize", (int)doubleParamList.size(), true);
				if (doubleParamList.size() > 0) {
					myjson->openCollection("doubleParamList");
					for (int i = 0; i < (int)doubleParamList.size(); i++) {
						if (i < ((int)doubleParamList.size() - 1)) {
							myjson->addDoubleKey2(doubleParamListLabels[i], doubleParamList[i], true);
						}
						else {
							myjson->addDoubleKey2(doubleParamListLabels[i], doubleParamList[i], false);
						}
					}
					myjson->closeCollection(true, true);
				}
			}
			else {
				addErrorString("Serialize for node " + id + " returning due to doubleParamist size mismatch");
				return(false);
			}

			if (stringParamList.size() == stringParamListLabels.size()) {
				myjson->addIntKey("stringParamListSize", (int)stringParamList.size(), true);
				if (stringParamList.size() > 0) {
					myjson->openCollection("stringParamList");
					for (int i = 0; i < (int)stringParamList.size(); i++) {
						if (i < ((int)stringParamList.size() - 1)) {
							myjson->addStringKey(stringParamListLabels[i], stringParamList[i], true);
						}
						else {
							myjson->addStringKey(stringParamListLabels[i], stringParamList[i], false);
						}
					}
					myjson->closeCollection(true, true);
				}
			}
			else {
				addErrorString("Serialize for node " + id + " returning due to stringParamist size mismatch");
				return(false);
			}

			myjson->addIntKey("pathsSize", (int)paths.size(), true);
			if (paths.size() > 0) {
				myjson->openArray("paths");
				for (int i = 0; i < (int)paths.size(); i++) {
					if (i < ((int)paths.size() - 1)) {
						myjson->addString(paths[i], true);
					}
					else {
						myjson->addString(paths[i], false);
					}
				}
				myjson->closeArray(true, true);
			}

			myjson->addIntKey("convDimensionsSize", (int)convDimensions.size(), true);
			if (convDimensions.size() > 0) {
				myjson->openArray("convDimensions");
				for (int i = 0; i < (int)convDimensions.size(); i++) {
					if (i < ((int)convDimensions.size() - 1)) {
						myjson->addInt(convDimensions[i], true);
					}
					else {
						myjson->addInt(convDimensions[i], false);
					}
				}
				myjson->closeArray(true, true);
			}

			myjson->addIntKey("convStridesSize", (int)convStrides.size(), true);
			if (convStrides.size() > 0) {
				myjson->openArray("convStrides");
				for (int i = 0; i < (int)convStrides.size(); i++) {
					if (i < ((int)convStrides.size() - 1)) {
						myjson->addInt(convStrides[i], true);
					}
					else {
						myjson->addInt(convStrides[i], false);
					}
				}
				myjson->closeArray(true, true);
			}

			if (convLocations.size() > 0) {
				myjson->addIntKey("convLocationsSize", (int)convLocations.size(), true);
				myjson->openCollection("convLocations");
				for (int i = 0; i < (int)convLocations.size(); i++) {
					myjson->addIntKey("convLocationX", convLocations[i].x, true);
					if (i < ((int)convLocations.size() - 1)) {
						myjson->addIntKey("convLocationY", convLocations[i].y, true);
					}
					else {
						myjson->addIntKey("convLocationY", convLocations[i].y, false);
					}
				}
				myjson->closeCollection(false, true);
			}
			else {
				myjson->addIntKey("convLocationsSize", (int)convLocations.size(), false);
			}

			myjson->closeCollection(false, false);

		}
		else {

			int tint = 0;
			string tid = "";
			string tstring = "";
			double tdouble = 0.0;
			jsonLineData pdata;
			int paramlistsize = 0;

			if (!myjson->parseNextOpenCollection(&id)) { return(false); };

			if (!myjson->parseNextKeyValueInt("nodeType", &nodeType)) { return(false); };

			if (!myjson->parseNextKeyValueString("id", &tid)) { return(false); };
			if (tid != id) {
				addErrorString("Serialize : problem with id match : " + id + " : " + tid);  return(false);
			}

			if (!myjson->parseNextKeyValueString("label", &label)) { return(false); };
			if (!myjson->parseNextKeyValueInt("x_disp", &x_disp)) { return(false); };
			if (!myjson->parseNextKeyValueInt("y_disp", &y_disp)) { return(false); };
			if (!myjson->parseNextKeyValueInt("group_index", &group_index)) { addErrorString("myjson->parseNextKeyValueInt for group_index failed"); return(false); };

			if (!myjson->parseNextKeyValueInt("intParamListSize", &paramlistsize)) { return(false); };
			if (paramlistsize > 0) {
				if (myjson->parseNextOpenCollection(&tid)) {
					if (tid == "intParamList") {
						for (int i = 0; i < paramlistsize; i++) {
							if (!myjson->parseNextKeyValueInt(&tid, &tint)) { return(false); };
							intParamListLabels.push_back(tid);
							intParamList.push_back(tint);
						}
					}
					else {
						addErrorString("Serialize : problem finding intParamList : " + tid);  return(false);
					}
				}
				else {
					addErrorString("Serialize : problem opening collection : " + tid);  return(false);
				}
				if (!myjson->parseNextCloseCollection()) { return(false); };
			}

			if (!myjson->parseNextKeyValueInt("doubleParamListSize", &paramlistsize)) { return(false); };
			if (paramlistsize > 0) {
				if (myjson->parseNextOpenCollection(&tid)) {
					if (tid == "doubleParamList") {
						for (int i = 0; i < paramlistsize; i++) {
							if (!myjson->parseNextKeyValueDouble(&tid, &tdouble)) { return(false); };
							doubleParamListLabels.push_back(tid);
							doubleParamList.push_back(tdouble);
						}
					}
					else {
						addErrorString("Serialize : problem finding doubleParamList : " + tid);  return(false);
					}
				}
				else {
					addErrorString("Serialize : problem opening collection : " + tid);  return(false);
				}
				if (!myjson->parseNextCloseCollection()) { return(false); };
			}

			if (!myjson->parseNextKeyValueInt("stringParamListSize", &paramlistsize)) { return(false); };
			if (paramlistsize > 0) {
				if (myjson->parseNextOpenCollection(&tid)) {
					if (tid == "stringParamList") {
						for (int i = 0; i < paramlistsize; i++) {
							if (!myjson->parseNextKeyValueString(&tid, &tstring)) { return(false); };
							stringParamListLabels.push_back(tid);
							stringParamList.push_back(tstring);
						}
					}
					else {
						addErrorString("Serialize : problem finding stringParamList : " + tid);  return(false);
					}
				}
				else {
					addErrorString("Serialize : problem opening collection : " + tid);  return(false);
				}
				if (!myjson->parseNextCloseCollection()) { return(false); };
			}

			if (!myjson->parseNextKeyValueInt("pathsSize", &paramlistsize)) { return(false); };
			if (paramlistsize > 0) {
				if (myjson->parseNextOpenArray(&tid)) {
					if (tid == "paths") {
						for (int i = 0; i < paramlistsize; i++) {
							if (!myjson->parseNextValueString(&tstring)) { return(false); };
							paths.push_back(tstring);
						}
					}
				}
				else {
					addErrorString("Serialize : problem opening array : " + tid);  return(false);
				}
				if (!myjson->parseNextCloseArray()) { return(false); };
			}

			if (!myjson->parseNextKeyValueInt("convDimensionsSize", &paramlistsize)) { return(false); };
			if (paramlistsize > 0) {
				if (myjson->parseNextOpenArray(&tid)) {
					if (tid == "convDimensions") {
						for (int i = 0; i < paramlistsize; i++) {
							if (!myjson->parseNextValueInt(&tint)) { return(false); };
							convDimensions.push_back(tint);
						}
					}
				}
				else {
					addErrorString("Serialize : problem opening array : " + tid);  return(false);
				}
				if (!myjson->parseNextCloseArray()) { return(false); };
			}

			if (!myjson->parseNextKeyValueInt("convStridesSize", &paramlistsize)) { return(false); };
			if (paramlistsize > 0) {
				if (myjson->parseNextOpenArray(&tid)) {
					if (tid == "convStrides") {
						for (int i = 0; i < paramlistsize; i++) {
							if (!myjson->parseNextValueInt(&tint)) { return(false); };
							convStrides.push_back(tint);
						}
					}
				}
				else {
					addErrorString("Serialize : problem opening array : " + tid);  return(false);
				}
				if (!myjson->parseNextCloseArray()) { return(false); };
			}

			if (!myjson->parseNextKeyValueInt("convLocationsSize", &paramlistsize)) { return(false); };
			if (paramlistsize > 0) {
				if (myjson->parseNextOpenCollection(&tid)) {
					if (tid == "convLocations") {
						for (int i = 0; i < paramlistsize; i++) {
							twoValues mvalues;
							if (!myjson->parseNextKeyValueInt(&tid, &tint)) { return(false); };
							string xlabel = tid;
							if (xlabel == "convLocationX") {
								mvalues.x = tint;
								if (!myjson->parseNextKeyValueInt(&tid, &tint)) { return(false); };
								string ylabel = tid;
								if (ylabel == "convLocationY") {
									mvalues.y = tint;
									convLocations.push_back(mvalues);
								}
								else {
									addErrorString("Serialize : problem parsing convLocation values : " + ylabel);  return(false);
								}
							}
							else {
								addErrorString("Serialize : problem parsing convLocation values : " + xlabel);  return(false);
							}
						}
					}
					else {
						addErrorString("Serialize : problem finding convLocations : " + tid);  return(false);
					}
				}
				else {
					addErrorString("Serialize : problem opening collection : " + tid);  return(false);
				}
				if (!myjson->parseNextCloseCollection()) { return(false); };
			}

			if (!myjson->parseNextCloseCollection()) { return(false); };

		}

		return(true);

	}

	void addErrorString(string err);


};

//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************

class NetData
{
public:

	netDataStruct mydefault;
	vector<netDataStruct> netData;

	netWeightData netweightdata;

	NetData() {
		mydefault.setID("Default");
	}

	int size() {
		return((int)netData.size());
	}

	void clear() {
		if (netData.size() > 0) {
			for (int i = 0; i < (int)netData.size(); i++) {
				netData[i].clear();
			}
		}
		netData.clear();
	}

	void removeItem(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData.erase(netData.begin() + index);
		}
	}

	void removeItem(string rid) {
		for (int i = 0; i < (int)netData.size(); i++) {
			if (rid == netData[i].getID()) {
				netData.erase(netData.begin() + i);
				return;
			}
		}
	}

	void push_back(netDataStruct data) {
		netData.push_back(data);
	}

	void pushNodeData(netDataStruct node) {
		netDataStruct tdata;
		tdata.setNode(node);
		netData.push_back(tdata);
	}

	void updateNode(int index, netDataStruct node)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				setLabel(index, node.getLabel());
				setXDisp(index, node.getXDisp());
				setYDisp(index, node.getYDisp());
				setGroupIndex(index, node.getGroupIndex());
				setNodeMode(index, node.getNodeMode());
				setDataField(index, node.getDataField());
				setNodeFunction(index, node.getNodeFunction());
				setNodePreProcessor(index, node.getNodePreProcessor());
				setNodePreProcessorTime(index, node.getNodePreProcessorTime());
				setNodePreProcessorStride(index, node.getNodePreProcessorStride());
				setNodePreProcessorDownSample(index, node.getNodePreProcessorDownSample());
				setNodeEta(index, node.getNodeEta());
				setNodeAlpha(index, node.getNodeAlpha());
				setNodeRand(index, node.getNodeRand());
				if (node.areMultipleBias()) {
					int num = node.getNumBias();
					for (int i = 0; i < num; i++) {
						setNodeBias(index, i, node.getNodeBias(i));
					}
				}
				else {
					clearMultipleBiases(index);
					setNodeBias(index, node.getNodeBias());
				}
				setNodeLowerNormLimit(index, node.getNodeLowerNormLimit());
				setNodeUpperNormLimit(index, node.getNodeUpperNormLimit());
				setNodeLowerNormalizationFactor(index, node.getNodeLowerNormalizationFactor());
				setNodeUpperNormalizationFactor(index, node.getNodeUpperNormalizationFactor());
				setNodeNormDisallow(index, node.getNodeNormDisallow());
				setNodeTrainable(index, node.getNodeTrainable());
				setNodeZeroOutput(index, node.getNodeZeroOutput());
				setNodeIndexMode(index, node.getNodeIndexMode());
				setNodeXIndex(index, node.getNodeXIndex());
				setNodeYIndex(index, node.getNodeYIndex());
			}
		}
	}

	void pushConnectData(string sid, string slbl, string node_in, string node_out, int gindex, double sweight, vector<double> sweights, vector<twoValues> points) {
		netDataStruct tdata;
		tdata.setConnect(sid, slbl, node_in, node_out, gindex, sweight, sweights, points);
		netData.push_back(tdata);
	}

	void pushDataSource(string sid, string slbl, int sx, int sy, string smode, int width, int height, int file_min_width,
		float aspect, bool gray, int wav_max, vector<string> filepaths) {
		netDataStruct tdata;
		tdata.setDataSource(sid, slbl, sx, sy, smode, width, height, file_min_width, aspect, gray, wav_max, filepaths);
		netData.push_back(tdata);
	}

	void pushConvLayer(netDataStruct layer) {
		netDataStruct tdata;
		tdata.setConvolutionalLayer(layer);
		netData.push_back(tdata);
	}

	void updateConvolution(int index, netDataStruct conv)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				setLabel(index, conv.getLabel());
				setXDisp(index, conv.getXDisp());
				setYDisp(index, conv.getYDisp());
				setGroupIndex(index, conv.getGroupIndex());
				setNodeMode(index, conv.getNodeMode());
				setNodeFunction(index, conv.getNodeFunction());
				setNodeEta(index, conv.getNodeEta());
				setNodeAlpha(index, conv.getNodeAlpha());
				setNodeRand(index, conv.getNodeRand());
				setResiduals(index, conv.getResiduals());
				setNormalization(index, conv.getNormalization());
				setNodeTrainable(index, conv.getNodeTrainable());
				setNodeZeroOutput(index, conv.getNodeZeroOutput());
				setConvNumLayers(index, conv.getConvNumLayers());
				setConvNodesPerLayer(index, conv.getConvNodesPerLayer());
				setConvType(index, conv.getConvType());
				setConvDimensions(index, conv.getConvDimensions());
				setConvStrides(index, conv.getConvStrides());
				setConvLocations(index, conv.getConvLocations());
				setDataFields(index, conv.getDataFields());
				setNodeLowerNormLimit(index, conv.getNodeLowerNormLimit());
				setNodeUpperNormLimit(index, conv.getNodeUpperNormLimit());
				setNodeLowerNormalizationFactor(index, conv.getNodeLowerNormalizationFactor());
				setNodeUpperNormalizationFactor(index, conv.getNodeUpperNormalizationFactor());
				setNodePreProcessor(index, conv.getNodePreProcessor());
				setD(index, conv.getD());
			}
		}
	}

	void pushAttentionLayer(netDataStruct layer) {
		netDataStruct tdata;
		tdata.setAttentionLayer(layer);
		netData.push_back(tdata);
	}

	void pushAssembly(netDataStruct layer) {
		netDataStruct tdata;
		tdata.setAssembly(layer);
		netData.push_back(tdata);
	}

	void pushNetCluster(netDataStruct layer) {
		netDataStruct tdata;
		tdata.setNetCluster(layer);
		netData.push_back(tdata);
	}

	void pushDropout(netDataStruct layer) {
		netDataStruct tdata;
		tdata.setDropout(layer);
		netData.push_back(tdata);
	}

	void updateAttention(int index, netDataStruct atten)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isAttentionLayer(index)) {
				setLabel(index, atten.getLabel());
				setXDisp(index, atten.getXDisp());
				setYDisp(index, atten.getYDisp());
				setGroupIndex(index, atten.getGroupIndex());
				setD(index, atten.getD());
				setDk(index, atten.getDk());
				setDv(index, atten.getDv());
				setSumAddLimit(index, atten.getSumAddLimit());
				setSoftmaxProcessing(index, atten.hasSoftmaxProcessing());
				setTransformerModeQuadratic(index, atten.getTransformerModeQuadratic());
			}
		}
	}

	void updateAssembly(int index, netDataStruct assem)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isAssemblyLayer(index)) {
				setLabel(index, assem.getLabel());
				setXDisp(index, assem.getXDisp());
				setYDisp(index, assem.getYDisp());
				setGroupIndex(index, assem.getGroupIndex());
				setAssemblyReference(index, assem.getAssemblyReference());
				//int here;
			}
		}
	}

	void updateNetCluster(int index, netDataStruct cluster)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNetCluster(index)) {
				setLabel(index, cluster.getLabel());
				setXDisp(index, cluster.getXDisp());
				setYDisp(index, cluster.getYDisp());
				setGroupIndex(index, cluster.getGroupIndex());
				setNodeMode(index, cluster.getNodeMode());
				setNodeFunction(index, cluster.getNodeFunction());
				setNodeEta(index, cluster.getNodeEta());
				setNodeAlpha(index, cluster.getNodeAlpha());
				setNodeRand(index, cluster.getNodeRand());
				setConvNodesPerLayer(index, cluster.getConvNodesPerLayer());
				setConvType(index, cluster.getConvType());
				setClusterUpdateEpochs(index, cluster.getClusterUpdateEpochs());
				setClusterTestError(index, cluster.getClusterTestError());
				setClusterSplitVariable(index, cluster.getClusterSplitVariable());
				setPatternSlitMethod(index, cluster.getPatternSplitMethod());
			}
		}
	}

	void updateDropout(int index, netDataStruct dropout)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDropout(index)) {
				setLabel(index, dropout.getLabel());
				setXDisp(index, dropout.getXDisp());
				setYDisp(index, dropout.getYDisp());
				setGroupIndex(index, dropout.getGroupIndex());
				setDropoutPercent(index, dropout.getDropoutPercent());
			}
		}
	}

	void clearMultipleBiases(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				netData[index].clearConvBiases();
			}
		}
	}

	void clearMultipleWeights(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				netData[index].clearConvWeights();
			}
		}
	}

	bool inputCanHaveMultipleWeights(string input) {
		int index = getIndexFromID(input);
		if ((index >= 0) && (index < (int)netData.size())) {
			if (getNodeMode(index) == "PreProcess") {
				if ((getNodePreProcessor(index) == "WordIndex") || (getNodePreProcessor(index) == "Dictionary")) {
					return(true);
				}
			}
		}
		return(false);
	}

	bool outputCanHaveMultipleWeights(string output) {
		int index = getIndexFromID(output);
		if ((index >= 0) && (index < (int)netData.size())) {
			if (getNodeMode(index) == "Output") {
				if ((getNodePreProcessor(index) == "WordIndex") || (getNodePreProcessor(index) == "Dictionary")) {
					return(true);
				}
			}
		}
		return(false);
	}

	int getIndexFromID(string tid) {
		for (int i = 0; i < (int)netData.size(); i++) {
			if (tid == getID(i)) {
				return(i);
			}
		}
		return(0);
	}

	netDataStruct getNode(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index]);
		}
		return(mydefault);
	}

	bool isNode(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isNode());
		}
		return(false);
	}

	bool isConnect(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isConnect());
		}
		return(false);
	}

	bool isDataSource(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isDataSource());
		}
		return(false);
	}

	bool isConvLayer(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isConvLayer());
		}
		return(false);
	}

	bool isAttentionLayer(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isAttentionLayer());
		}
		return(false);
	}
		
	bool isSelfAttentionLayer(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isSelfAttentionLayer());
		}
		return(false);
	}

	bool isAssemblyLayer(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isAssemblyLayer());
		}
		return(false);
	}

	bool isNetCluster(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isNetCluster());
		}
		return(false);
	}

	bool isDropout(int index)
	{
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].isDropout());
		}
		return(false);
	}

	int getType(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].nodeType);
		}
		return(NONE);
	}

	void setType(int index, int ntype) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].nodeType = ntype;
		}
	}

	string getID(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getID());
		}
		return("None");
	}

	void setID(int index, string nid) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setID(nid);
		}
	}

	string getLabel(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getLabel());
		}
		return("None");
	}

	void setLabel(int index, string nlbl) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setLabel(nlbl);
		}
	}

	int getXDisp(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].x_disp);
		}
		return(0);
	}

	void setXDisp(int index, int disp) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setXDisp(disp);
		}
	}

	int getYDisp(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].y_disp);
		}
		return(0);
	}

	void setYDisp(int index, int disp) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setYDisp(disp);
		}
	}

	int getGroupIndex(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getGroupIndex());
		}
		return(-1);
	}

	void setGroupIndex(int index, int gindex) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setGroupIndex(gindex);
		}
	}

	string getNodeMode(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				return(netData[index].getNodeMode());
			}
		}
		return("None");
	}

	void setNodeMode(int index, string mode) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				netData[index].setNodeMode(mode);
			}
		}
	}

	string getDataField(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getDataField());
			}
		}
		return("None");
	}

	void setDataField(int index, string field) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setDataField(field);
			}
		}
	}

	int getNumDataFields(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				return(netData[index].getNumDataFields());
			}
		}
		return(0);
	}

	vector<string> getDataFields(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				return(netData[index].getDataFields());
			}
		}
		vector<string> rval;
		return(rval);
	}

	void setDataFields(int index, vector<string> fields) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				netData[index].setDataFields(fields);
			}
		}
	}

	string getInputProcessNode(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			for (int i = 0; i < (int)netData.size(); i++) {
				if (isConnect(i)) {
					int pindex = getIndexFromID(getInputNode(i));
					if (getOutputNode(i) == getID(index)) {
						if (getNodeMode(pindex) == "PreProcess") {
							return(getID(pindex));
						}
					}
				}
			}
		}
		return("None");
	}

	string getNodeFunction(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				return(netData[index].getNodeFunction());
			}
		}
		return("None2");
	}

	void setNodeFunction(int index, string function) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				netData[index].setNodeFunction(function);
			}
		}
	}

	string getNodePreProcessor(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				return(netData[index].getNodePreProcessor());
			}
		}
		return("NormLinear");
	}

	void setNodePreProcessor(int index, string processor) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				netData[index].setNodePreProcessor(processor);
			}
		}
	}

	string getNodePreProcessorTime(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getNodePreProcessorTime());
			}
		}
		return("None");
	}

	void setNodePreProcessorTime(int index, string time) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodePreProcessorTime(time);
			}
		}
	}

	double getNodeEta(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				return((double)netData[index].getNodeEta());
			}
		}
		return(0.0);
	}

	void setNodeEta(int index, double eta) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				netData[index].setNodeEta(eta);
			}
		}
	}

	double getNodeAlpha(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				return((double)netData[index].getNodeAlpha());
			}
		}
		return(0.0);
	}

	void setNodeAlpha(int index, double alpha) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				netData[index].setNodeAlpha(alpha);
			}
		}
	}

	double getNodeRand(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				return((double)netData[index].getNodeRand());
			}
		}
		return(0.0);
	}

	void setNodeRand(int index, double rand) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index) || isNetCluster(index)) {
				netData[index].setNodeRand(rand);
			}
		}
	}

	string getResiduals(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				return(netData[index].getResiduals());
			}
		}
		return("No");
	}

	void setResiduals(int index, string residuals) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				netData[index].setResiduals(residuals);
			}
		}
	}

	string getNormalization(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				return(netData[index].getNormalization());
			}
		}
		return("No");
	}

	void setNormalization(int index, string normalization) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				netData[index].setNormalization(normalization);
			}
		}
	}

	double getNodeLowerNormLimit(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				return((double)netData[index].getNodeLowerNormLimit());
			}
		}
		return(0.0);
	}

	void setNodeLowerNormLimit(int index, double limit) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				netData[index].setNodeLowerNormLimit(limit);
			}
		}
	}

	double getNodeUpperNormLimit(int index) {
		if ((index >= 0) && (index < (int)netData.size()) || isConvLayer(index)) {
			if (isNode(index) || isConvLayer(index)) {
				return((double)netData[index].getNodeUpperNormLimit());
			}
		}
		return(1.0);
	}

	void setNodeUpperNormLimit(int index, double limit) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				netData[index].setNodeUpperNormLimit(limit);
			}
		}
	}

	double getNodeLowerNormalizationFactor(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				return((double)netData[index].getNodeLowerNormalizationFactor());
			}
		}
		return(0.0);
	}

	void setNodeLowerNormalizationFactor(int index, double limit) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				netData[index].setNodeLowerNormalizationFactor(limit);
			}
		}
	}

	double getNodeUpperNormalizationFactor(int index) {
		if ((index >= 0) && (index < (int)netData.size()) || isConvLayer(index)) {
			if (isNode(index) || isConvLayer(index)) {
				return((double)netData[index].getNodeUpperNormalizationFactor());
			}
		}
		return(0.0);
	}

	void setNodeUpperNormalizationFactor(int index, double limit) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				netData[index].setNodeUpperNormalizationFactor(limit);
			}
		}
	}

	int getNodeNormDisallow(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return((int)netData[index].getNodeNormDisallow());
			}
		}
		return(0);
	}

	void setNodeNormDisallow(int index, int allow) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodeNormDisallow(allow);
			}
		}
	}

	bool getNodeTrainable(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				return(netData[index].getNodeTrainable());
			}
		}
		return(true);
	}

	void setNodeTrainable(int index, bool trainable) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index) || isConvLayer(index)) {
				netData[index].setNodeTrainable(trainable);
			}
		}
	}

	bool getNodeZeroOutput(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getNodeZeroOutput());
		}
		return(false);
	}

	void setNodeZeroOutput(int index, bool trainable) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setNodeZeroOutput(trainable);
		}
	}

	string getNodeIndexMode(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getNodeIndexMode());
			}
		}
		return("");
	}

	void setNodeIndexMode(int index, string mode) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodeIndexMode(mode);
			}
		}
	}
		
	int getNodeXIndex(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getNodeXIndex());
			}
		}
		return(0);
	}

	void setNodeXIndex(int index, int xindex) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodeXIndex(xindex);
			}
		}
	}

	int getNodeYIndex(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getNodeYIndex());
			}
		}
		return(0);
	}

	void setNodeYIndex(int index, int yindex) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodeYIndex(yindex);
			}
		}
	}

	int getNodePreProcessorStride(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getNodePreProcessorStride());
			}
		}
		return(1);
	}

	void setNodePreProcessorStride(int index, int stride) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodePreProcessorStride(stride);
			}
		}
	}

	int getNodePreProcessorDownSample(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getNodePreProcessorDownSample());
			}
		}
		return(1);
	}

	void setNodePreProcessorDownSample(int index, int downsample) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodePreProcessorDownSample(downsample);
			}
		}
	}

	string getInputNode(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				return(netData[index].getInputNode());
			}
		}
		return("None");
	}

	void setInputNode(int index, string node) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				netData[index].setInputNode(node);
			}
		}
	}

	twoValues getInputNodePoint(int index) {
		twoValues rval; rval.x = rval.y = 0;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				string nname = netData[index].getInputNode();
				for (int i = 0; i < (int)netData.size(); i++) {
					if (isNode(i)) {
						if (nname == getID(i)) {
							rval.x = getXDisp(i);
							rval.y = getYDisp(i);
							return(rval);
						}
					}
					else if (isConvLayer(i)) {
						if (nname == getID(i)) {
							vector<twoValues> locations = getConvLocations(i);
							if (locations.size() > 0) {
								rval.x = getXDisp(i) + locations[0].x;
								rval.y = getYDisp(i) + locations[0].y;
								return(rval);
							}
						}
					}
				}
			}
		}
		return(rval);
	}

	string getOutputNode(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				return(netData[index].getOutputNode());
			}
		}
		return("None");
	}

	void setOutputNode(int index, string node) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				netData[index].setOutputNode(node);
			}
		}
	}

	twoValues getOutputNodePoint(int index) {
		twoValues rval; rval.x = rval.y = 0;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				string nname = netData[index].getOutputNode();
				for (int i = 0; i < (int)netData.size(); i++) {
					if (isNode(i)) {
						if (nname == getID(i)) {
							rval.x = getXDisp(i);
							rval.y = getYDisp(i);
							return(rval);
						}
					}
					else if (isConvLayer(i)) {
						if (nname == getID(i)) {
							rval.x = getXDisp(i);
							rval.y = getYDisp(i);
							return(rval);
						}
					}
				}
			}
		}
		return(rval);
	}

	int getNumPoints(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				return(netData[index].getNumPoints());
			}
		}
		return(0);
	}

	void setNumPoints(int index, int num) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				netData[index].setNumPoints(num);
			}
		}
	}

	twoValues getPoint(int index, int pindex) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				return(netData[index].getPoint(pindex));
			}
		}
		twoValues mpt; mpt.x = mpt.y = 0;
		return(mpt);
	}

	void setPoint(int index, int pindex, twoValues pt) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				netData[index].setPoint(pindex, pt);
			}
		}
	}

	void setPoints(int index, vector<twoValues> points) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				netData[index].setPoints(points);
			}
		}
	}

	vector<twoValues> getPoints(int index) {
		vector<twoValues> points;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				points = netData[index].getPoints();
			}
		}
		return(points);
	}

	bool areMultipleBias(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].areMultipleBias());
			}
		}
		return(false);
	}

	int getNumBias(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return(netData[index].getNumBias());
			}
		}
		return(0);
	}

	double getNodeBias(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				return((double)netData[index].getNodeBias());
			}
		}
		return(0.0);
	}

	double getNodeBias(int net_index, int node_index) {
		if ((net_index >= 0) && (net_index < (int)netData.size())) {
			if (isNode(net_index)) {
				return((double)netData[net_index].getNodeBias(node_index));
			}
		}
		return(0.0);
	}

	void setNodeBias(int index, double sbias) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isNode(index)) {
				netData[index].setNodeBias(sbias);
			}
		}
	}

	void setNodeBias(int net_index, int node_index, double sbias) {
		if ((net_index >= 0) && (net_index < (int)netData.size())) {
			if (isNode(net_index)) {
				netData[net_index].setNodeBias(node_index, sbias);
			}
		}
	}

	bool areMultipleWeights(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				return(netData[index].areMultipleWeights());
			}
		}
		return(false);
	}
		
	int getNumWeights(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				return(netData[index].getNumWeights());
			}
		}
		return(0);
	}
		
	double getWeight(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				return(netData[index].getWeight());
			}
		}
		return(0.0);
	}

	double getWeight(int net_index, int node_index) {
		if ((net_index >= 0) && (net_index < (int)netData.size())) {
			if (isConnect(net_index)) {
				return(netData[net_index].getWeight(node_index));
			}
		}
		return(0.0);
	}

	void setWeight(int index, double sweight) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConnect(index)) {
				netData[index].setWeight(sweight);
			}
		}
	}

	void setWeight(int net_index, int node_index, double sweight) {
		if ((net_index >= 0) && (net_index < (int)netData.size())) {
			if (isConnect(net_index)) {
				netData[net_index].setWeight(node_index, sweight);
			}
		}
	}

	void clearConvBiasesAndWeights(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].clearConvBiases();
			netData[index].clearConvWeights();
		}
	}

	vector<double> getConvNodeBiases(int index) {
		vector<double> rval;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index) || isNode(index)) {
				return(netData[index].getConvBiases());
			}
		}
		return(rval);
	}

	void setConvNodeBiases(int index, vector<double> sbiases) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index) || isNode(index)) {
				netData[index].setConvBiases(sbiases);
			}
		}
	}

	vector<double> getConvWeights(int index) {
		vector<double> rval;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index) || isNode(index)) {
				return(netData[index].getConvWeights());
			}
		}
		return(rval);
	}

	void setConvWeights(int index, vector<double> sweights) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index) || isNode(index)) {
				netData[index].setConvWeights(sweights);
			}
		}
	}

	string getDataMode(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataMode());
			}
		}
		return("None");
	}

	void setDataMode(int index, string mode) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataMode(mode);
			}
		}
	}

	int getDataImageWidth(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataImageWidth());
			}
		}
		return(100);
	}

	void setDataImageWidth(int index, int width) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataImageWidth(width);
			}
		}
	}

	int getDataImageHeight(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataImageHeight());
			}
		}
		return(0);
	}

	void setDataImageHeight(int index, int width) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataImageHeight(width);
			}
		}
	}

	int getRawImageWidth(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getRawImageWidth());
			}
		}
		return(100);
	}

	void setRawImageWidth(int index, int width) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setRawImageWidth(width);
			}
		}
	}

	int getRawImageHeight(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getRawImageHeight());
			}
		}
		return(56);
	}

	void setRawImageHeight(int index, int width) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setRawImageHeight(width);
			}
		}
	}

	int getFileMinImageWidth(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getFileMinImageWidth());
			}
		}
		return(0);
	}

	void setFileMinImageWidth(int index, int width) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setFileMinImageWidth(width);
			}
		}
	}

	float getFileImageAspect(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getFileImageAspect());
			}
		}
		return(0.0F);
	}

	void setFileImageAspect(int index, float aspect) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setFileImageAspect(aspect);
			}
		}
	}

	bool getDataImageGray(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataImageGray());
			}
		}
		return(false);
	}

	void setDataImageGray(int index, bool gray) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataImageGray(gray);
			}
		}
	}

	int getDataWavMax(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataWavMax());
			}
		}
		return(100);
	}

	void setDataWavMax(int index, int max) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataWavMax(max);
			}
		}
	}

	int getDataWavMin(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataWavMin());
			}
		}
		return(100);
	}

	void setDataWavMin(int index, int min) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataWavMin(min);
			}
		}
	}

	int getDataWavShortest(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataWavShortest());
			}
		}
		return(100);
	}

	void setDataWavShortest(int index, int shortest) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataWavShortest(shortest);
			}
		}
	}

	int getDataWavLongest(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getDataWavLongest());
			}
		}
		return(100);
	}

	void setDataWavLongest(int index, int longest) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setDataWavLongest(longest);
			}
		}
	}

	vector<string> getFilePaths(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				return(netData[index].getFilePaths());
			}
		}
		vector<string> dpaths;
		return(dpaths);
	}

	void setFilePaths(int index, vector<string> paths) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isDataSource(index)) {
				netData[index].setFilePaths(paths);
			}
		}
	}

	int getConvNumLayers(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getConvNumLayers());
		}
		return(0);
	}

	void setConvNumLayers(int index, int slayers) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setConvNumLayers(slayers);
		}
	}

	int getConvNodesPerLayer(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getConvNodesPerLayer());
		}
		return(0);
	}

	void setConvNodesPerLayer(int index, int snodes) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setConvNodesPerLayer(snodes);
		}
	}

	int getConvType(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getConvType());
		}
		return(NONE);
	}

	void setConvType(int index, int stype) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setConvType(stype);
		}
	}

	int getD(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getD());
		}
		return(0);
	}

	void setD(int index, int size) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setD(size);
		}
	}

	int getDk(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getDk());
		}
		return(0);
	}

	void setDk(int index, int size) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setDk(size);
		}
	}

	int getDv(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getDv());
		}
		return(0);
	}

	void setDv(int index, int size) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setDv(size);
		}
	}

	double getClusterUpdateEpochs(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getClusterUpdateEpochs());
		}
		return(5.0);
	}

	void setClusterUpdateEpochs(int index, double epochs) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setClusterUpdateEpochs(epochs);
		}
	}

	double getClusterTestError(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getClusterTestError());
		}
		return(0.100);
	}

	void setClusterTestError(int index, double error) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setClusterTestError(error);
		}
	}
		
	double getClusterSplitVariable(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getClusterSplitVariable());
		}
		return(1.0);
	}

	void setClusterSplitVariable(int index, double error) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setClusterSplitVariable(error);
		}
	}

	string getPatternSplitMethod(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getPatternSplitMethod());
		}
		return("Median");
	}

	void setPatternSlitMethod(int index, string split) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setPatternSlitMethod(split);
		}
	}

	double getDropoutPercent(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getDropoutPercent());
		}
		return(0.02);
	}

	void setDropoutPercent(int index, double error) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setDropoutPercent(error);
		}
	}

	void setSumAddLimit(int index, double limit) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setSumAddLimit(limit);
		}
	}

	double getSumAddLimit(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getSumAddLimit());
		}
		return(0.01);
	}

	bool hasSoftmaxProcessing(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].hasSoftmaxProcessing());
		}
		return(false);
	}

	void setSoftmaxProcessing(int index, bool soft) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setSoftmaxProcessing(soft);
		}
	}

	bool getTransformerModeQuadratic(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getTransformerModeQuadratic());
		}
		return(true);
	}

	void setTransformerModeQuadratic(int index, bool quad) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setTransformerModeQuadratic(quad);
		}
	}

	string getAssemblyReference(int index) {
		if ((index >= 0) && (index < (int)netData.size())) {
			return(netData[index].getAssemblyReference());
		}
		return("");
	}

	void setAssemblyReference(int index, string reference) {
		if ((index >= 0) && (index < (int)netData.size())) {
			netData[index].setAssemblyReference(reference);
		}
	}

	vector<int> getConvDimensions(int index) {
		vector<int> blank;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				return(netData[index].getConvDimensions());
			}
		}
		return(blank);
	}

	void setConvDimensions(int index, vector<int> dimensions) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				netData[index].setConvDimensions(dimensions);
			}
		}
	}

	vector<int> getConvStrides(int index) {
		vector<int> blank;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				return(netData[index].getConvStrides());
			}
		}
		return(blank);
	}

	void setConvStrides(int index, vector<int> strides) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				netData[index].setConvStrides(strides);
			}
		}
	}

	vector<twoValues> getConvLocations(int index) {
		vector<twoValues> blank;
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				return(netData[index].getConvLocations());
			}
		}
		return(blank);
	}

	void setConvLocations(int index, vector<twoValues> locations) {
		if ((index >= 0) && (index < (int)netData.size())) {
			if (isConvLayer(index)) {
				netData[index].setConvLocations(locations);
			}
		}
	}

	vector<string> getInputNodeStrings()
	{
		vector<string> inputStrings;
		for (int i = 0; i < size(); i++) {
			if (isNode(i)) {
				if (getNodeMode(i) == "Input") {
					string inputid = getNextPreProcess(i);
					string process = getNodePreProcessor(getIndexFromID(inputid));
					//addErrorString("getInputNodeStrings test1 : " + getID(i) + " : " + inputid + " : " + process);
					if (process.substr(0, 3) == "FFT") {
						//addErrorString("CNetDesignerDoc::getInputNodeStrings : " + process + " : " + process.substr(3));
						//int size = (int)_tstof((LPCTSTR)(CString)process.substr(3).c_str());
						int size = (int)stoi(process.substr(3));
						for (int j = 0; j < size; j++) {
							string fname = "";
							//fname.Format(_T(":%i"), j);
							char s[200];  sprintf_s(s, ":%i", j);
							fname = inputid + s;
							bool found = false;
							for (int k = 0; k < inputStrings.size(); k++) {
								if (fname == inputStrings[k]) {
									k = (int)inputStrings.size();
									found = true;
								}
							}
							if (!found) {
								inputStrings.push_back(fname);
							}
						}
					}
					else {
						bool found = false;
						for (int k = 0; k < inputStrings.size(); k++) {
							if (inputid == inputStrings[k]) {
								k = (int)inputStrings.size();
								found = true;
							}
						}
						if (!found) {
							inputStrings.push_back(inputid);
						}
					}
				}
			}
			else if (isConvLayer(i)) {
				if (getNodeMode(i) == "Input") {
					if (getConvType(i) == SOUND_CONV) {
						string inputid = getNextPreProcess(i);
						string process = getNodePreProcessor(getIndexFromID(inputid));
						//addErrorString("getInputNodeStrings test2 : " + getID(i) + " : " + inputid + " : " + process);
						if (process.substr(0, 3) == "FFT") {
							//addErrorString("CNetDesignerDoc::getInputNodeStrings : " + process + " : " + process.substr(3));
							//int size = (int)_tstof((LPCTSTR)(CString)process.substr(3).c_str());
							int size = stoi(process.substr(3));
							for (int j = 0; j < size; j++) {
								string fname = "";
								//fname.Format(_T(":%i"), j);
								char s[200];  sprintf_s(s, ":%i", j);
								fname = inputid + s;
								bool found = false;
								for (int k = 0; k < inputStrings.size(); k++) {
									if (fname == inputStrings[k]) {
										k = (int)inputStrings.size();
										found = true;
									}
								}
								if (!found) {
									inputStrings.push_back(fname);
								}
							}
						}
					}
					/*else if (tparams.text_data) {
						vector<CString> following = func.getFollowingNodes(getID(i));
						for (int j = 0; j < following.size(); j++) {
							int tindex = getIndexFromID(string(CT2CA(following[j])));
							if (getNodeMode(tindex) == "PreProcess") {
								if ((getNodePreProcessor(tindex) == _T("WordIndex")) || (getNodePreProcessor(tindex) == _T("Dictionary"))) {
									if ((tparams.text_type == HUFFMAN) || (tparams.text_type == ARITHMETIC) || (tparams.text_type == ONEHOT)) {
										for (int k = 0; k < tparams.nword_vector; k++) {
											CString fname = _T("");
											fname.Format(_T(":%i"), k);
											fname = following[j] + fname;
											bool found = false;
											for (int k = 0; k < inputStrings.size(); k++) {
												if (fname == inputStrings[k]) {
													k = (int)inputStrings.size();
													found = true;
												}
											}
											if (!found) {
												inputStrings.push_back(fname);
											}
										}
									}
								}
							}
						}
					}*/
					else if ((getConvType(i) == DATA_CONV) && ((getNodeMode(i) == "Input"))) {
						if (getNumDataFields(i) > 0) {
							vector<string> datafields = getDataFields(i);
							if (hasFollowingPreProcess(i)) {
								for (int j = 0; j < datafields.size(); j++) {
									bool found = false;
									//string nname = "";
									//nname.Format(_T(":%i"), j);
									//char s[200];  sprintf(s, ":%i", j);
									string nname = getNextPreProcess(i) + ":" + datafields[j];
									for (int k = 0; k < inputStrings.size(); k++) {
										if (nname == inputStrings[k]) {
											k = (int)inputStrings.size();
											found = true;
										}
									}
									if (!found) {
										inputStrings.push_back(nname);
									}
								}
							}
							else {
								for (int j = 0; j < datafields.size(); j++) {
									bool found = false;
									for (int k = 0; k < inputStrings.size(); k++) {
										if (datafields[j] == inputStrings[k]) {
											k = (int)inputStrings.size();
											found = true;
										}
									}
									if (!found) {
										inputStrings.push_back(datafields[j]);
									}
								}
							}
						}
					}
				}
			}
			else if (isAttentionLayer(i)) {
				// Attention layers can't be direct inputs - they need to be fed by input/process nodes or nets or convs
			}
		}
		return(inputStrings);
	}
	
	string getNextPreProcess(int index)
	{
		string rval = getID(index);
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				if (getConvNameFromConnectName(getInputNode(i)) == getID(index)) {
					int index2 = getIndexFromID(getOutputNode(i));
					if (isNode(index2)) {
						if (getNodeMode(index2) == "PreProcess") {
							return(getNextPreProcess(index2));
						}
					}
				}
			}
		}
		return(rval);
	}

	bool hasFollowingPreProcess(int index)
	{
		for (int i = 0; i < netData.size(); i++) {
			if (isConnect(i)) {
				if (getConvNameFromConnectName(getInputNode(i)) == getID(index)) {
					int index2 = getIndexFromID(getOutputNode(i));
					if (isNode(index2)) {
						if (getNodeMode(index2) == "PreProcess") {
							return(true);
						}
					}
				}
			}
		}
		return(false);
	}

	string getConvNameFromConnectName(string cname)
	{
		string rval = cname.substr(0, 5);
		if ((cname.substr(0, 9) == "ConvLayer") || (rval == "Layer") || (cname.substr(0, 9) == "ImageData") ||
			(cname.substr(0, 9) == "Attention") || (cname.substr(0, 10) == "PatternNet") || (cname.substr(0, 7) == "Dropout") || 
			(cname.substr(0, 8) == "Assembly")) {
			int k = (int)rval.length();
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				rval += cname[k];
				k++;
			}
			return(rval);
		}
		return(cname);
	}


	int getNoDefaultCount()
	{
		int cnt = 0;
		for (int i = 0; i < (int)netData.size(); i++) {
			if (netData[i].id != "Default") {
				cnt++;
			}
		}
		return(cnt);
	}

#ifdef SERIALIZE
	CString Serialize(CArchive& ar, bool check_only) {
		if (ar.IsStoring())
		{
			CString save_buffer = _T("");
			int cnt = getNoDefaultCount();
			save_buffer.Format(_T("%i"), cnt); // (int)netData.size());
			ar << save_buffer;
			for (int i = 0; i < (int)netData.size(); i++) {
				if (netData[i].id != "Default") {
					netData[i].Serialize(ar, check_only);
				}
			}
			return(serializeWeightData(ar, check_only));
		}
		else
		{
			CString save_buffer = _T("");
			ar >> save_buffer;
			int size = (int)_tstof((LPCTSTR)save_buffer);
			for (int i = 0; i < size; i++) {
				if (!check_only) {
					push_back(mydefault);
				}
				netData[i].Serialize(ar, check_only);
				if (netData[i].getID() != netData[i].getLabel()) {
					if (!netData[i].isAssemblyLayer() && !(netData[i].isNode() && netData[i].getNodeFunction() == "Connector")) {
						string s = "Observation: ID " + netData[i].getID() + " not equal to label " + netData[i].getLabel();
						netData[i].setLabel(netData[i].getID());
						addErrorString(s);
					}
				}
			}
			return(serializeWeightData(ar, check_only));
		}
		return(_T(""));
	}

	CString serializeWeightData(CArchive& ar, bool check_only)
	{
		if (ar.IsStoring()) {
			
			CString save_buffer = _T("netweightsstart");
			ar << save_buffer;

			// Archive hidden weight data
			save_buffer = _T("numhiddennodes");
			ar << save_buffer;
			ar << (int)netweightdata.hiddennodeweightdata.size();
			if (netweightdata.hiddennodeweightdata.size() > 0) {
				for (int i = 0; i < (int)netweightdata.hiddennodeweightdata.size(); i++) {
					ar << (CString)netweightdata.hiddennodeweightdata[i].id.c_str();
					ar << netweightdata.hiddennodeweightdata[i].bias;
					save_buffer = _T("numweights");
					ar << save_buffer;
					ar << (int)netweightdata.hiddennodeweightdata[i].weightdata.size();
					if (netweightdata.hiddennodeweightdata[i].weightdata.size() > 0) {
						for (int j = 0; j < (int)netweightdata.hiddennodeweightdata[i].weightdata.size(); j++) {
							ar << (CString)netweightdata.hiddennodeweightdata[i].weightdata[j].label.c_str();
							ar << netweightdata.hiddennodeweightdata[i].weightdata[j].weight;
						}
					}
				}
			}

			// Archive output weight data
			save_buffer = _T("numoutputnodes");
			ar << save_buffer;
			ar << (int)netweightdata.outputnodeweightdata.size();
			if (netweightdata.outputnodeweightdata.size() > 0) {
				for (int i = 0; i < (int)netweightdata.outputnodeweightdata.size(); i++) {
					ar << (CString)netweightdata.outputnodeweightdata[i].id.c_str();
					ar << netweightdata.outputnodeweightdata[i].bias;
					save_buffer = _T("numweights");
					ar << save_buffer;
					ar << (int)netweightdata.outputnodeweightdata[i].weightdata.size();
					if (netweightdata.outputnodeweightdata[i].weightdata.size() > 0) {
						for (int j = 0; j < (int)netweightdata.outputnodeweightdata[i].weightdata.size(); j++) {
							ar << (CString)netweightdata.outputnodeweightdata[i].weightdata[j].label.c_str();
							ar << netweightdata.outputnodeweightdata[i].weightdata[j].weight;
						}
					}
				}
			}

			// Archive conv weight data
			save_buffer = _T("numconvs");
			ar << save_buffer;
			ar << (int)netweightdata.convweightdata.size();
			if (netweightdata.convweightdata.size() > 0) {
				for (int i = 0; i < (int)netweightdata.convweightdata.size(); i++) {
					ar << (CString)netweightdata.convweightdata[i].id.c_str();
					save_buffer = _T("numnodes");
					ar << save_buffer;
					ar << (int)netweightdata.convweightdata[i].nodeweightdata.size();
					if (netweightdata.convweightdata[i].nodeweightdata.size() > 0) {
						for (int j = 0; j < (int)netweightdata.convweightdata[i].nodeweightdata.size(); j++) {
							ar << (CString)netweightdata.convweightdata[i].nodeweightdata[j].id.c_str();
							ar << netweightdata.convweightdata[i].nodeweightdata[j].bias;
							save_buffer = _T("numweights");
							ar << save_buffer;
							ar << (int)netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size();
							if (netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size() > 0) {
								for (int k = 0; k < (int)netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size(); k++) {
									ar << (CString)netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].label.c_str();
									ar << netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].weight;
								}
							}
						}
					}
					save_buffer = _T("numinputlayernodes");
					ar << save_buffer;
					ar << (int)netweightdata.convweightdata[i].inputlayerweightdata.size();
					if (netweightdata.convweightdata[i].inputlayerweightdata.size() > 0) {
						for (int j = 0; j < (int)netweightdata.convweightdata[i].inputlayerweightdata.size(); j++) {
							ar << (CString)netweightdata.convweightdata[i].inputlayerweightdata[j].id.c_str();
							ar << netweightdata.convweightdata[i].inputlayerweightdata[j].bias;
							save_buffer = _T("numweights");
							ar << save_buffer;
							ar << (int)netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size();
							if (netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size() > 0) {
								for (int k = 0; k < (int)netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size(); k++) {
									ar << (CString)netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].label.c_str();
									ar << netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].weight;
								}
							}
						}
					}
				}
			}

			// Archive hidden net weight data
			save_buffer = _T("numhiddennets");
			ar << save_buffer;
			ar << (int)netweightdata.hiddenweightdata.size();
			if (netweightdata.hiddenweightdata.size() > 0) {
				for (int i = 0; i < (int)netweightdata.hiddenweightdata.size(); i++) {
					ar << (CString)netweightdata.hiddenweightdata[i].id.c_str();
					save_buffer = _T("numnodes");
					ar << save_buffer;
					ar << (int)netweightdata.hiddenweightdata[i].nodeweightdata.size();
					if (netweightdata.hiddenweightdata[i].nodeweightdata.size() > 0) {
						for (int j = 0; j < (int)netweightdata.hiddenweightdata[i].nodeweightdata.size(); j++) {
							ar << (CString)netweightdata.hiddenweightdata[i].nodeweightdata[j].id.c_str();
							ar << netweightdata.hiddenweightdata[i].nodeweightdata[j].bias;
							save_buffer = _T("numweights");
							ar << save_buffer;
							ar << (int)netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size();
							if (netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size() > 0) {
								for (int k = 0; k < (int)netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size(); k++) {
									ar << (CString)netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].label.c_str();
									ar << netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].weight;
								}
							}
						}
					}
				}
			}

			save_buffer = _T("netweightsend");
			ar << save_buffer;

		}
		else {

			CString s = _T("");
			CString save_buffer = _T("");

			ar >> save_buffer;
			if (save_buffer != _T("netweightsstart")) {
				//AfxMessageBox(_T("serializeWeightData problem finding netweightsstart"));
				return(save_buffer);
			}

			netWeightData mynetweightdata;
			netWeightData *pnetweightdata;

			if (check_only) {
				pnetweightdata = &mynetweightdata;
			}
			else {
				netweightdata.hiddennodeweightdata.clear();
				netweightdata.outputnodeweightdata.clear();
				netweightdata.convweightdata.clear();
				netweightdata.hiddenweightdata.clear();
				pnetweightdata = &netweightdata;
			}

			int datasize = 0;
			int numlayers = 0;
			int weightdatasize = 0;

			// Get hidden node weights
			ar >> save_buffer;
			if (save_buffer != _T("numhiddennodes")) {
				AfxMessageBox(_T("serializeWeightData problem finding numhiddennodes : ") + save_buffer);
				return(save_buffer);
			}
			ar >> datasize;
			if (datasize > 0) {
				for (int i = 0; i < datasize; i++) {
					nodeWeightData tdata;
					ar >> s;  tdata.id = string(CT2CA(s));
					ar >> tdata.bias;
					ar >> save_buffer;
					if (save_buffer != _T("numweights")) {
						AfxMessageBox(_T("serializeWeightData problem finding hidden node numweights"));
						return(save_buffer);
					}
					ar >> weightdatasize;
					if (weightdatasize > 0) {
						for (int j = 0; j < weightdatasize; j++) {
							weightData wd;
							ar >> s;  wd.label = string(CT2CA(s));
							ar >> wd.weight;
							tdata.weightdata.push_back(wd);
						}
					}
					pnetweightdata->hiddennodeweightdata.push_back(tdata);
				}
			}

			// Get output node weights
			ar >> save_buffer;
			if (save_buffer != _T("numoutputnodes")) {
				AfxMessageBox(_T("serializeWeightData problem finding numoutputnodes"));
				return(save_buffer);
			}
			ar >> datasize;
			if (datasize > 0) {
				for (int i = 0; i < datasize; i++) {
					nodeWeightData tdata;
					ar >> s;  tdata.id = string(CT2CA(s));
					ar >> tdata.bias;
					ar >> save_buffer;
					if (save_buffer != _T("numweights")) {
						AfxMessageBox(_T("serializeWeightData problem finding output node numweights"));
						return(save_buffer);
					}
					ar >> weightdatasize;
					if (weightdatasize > 0) {
						for (int j = 0; j < weightdatasize; j++) {
							weightData wd;
							ar >> s;  wd.label = string(CT2CA(s));
							ar >> wd.weight;
							tdata.weightdata.push_back(wd);
						}
					}
					pnetweightdata->outputnodeweightdata.push_back(tdata);
				}
			}

			// Get conv layer weights
			ar >> save_buffer;
			if (save_buffer != _T("numconvs")) {
				AfxMessageBox(_T("serializeWeightData problem finding num conv layers"));
				return(save_buffer);
			}
			ar >> numlayers;
			if (numlayers > 0) {
				for (int k = 0; k < numlayers; k++) {
					convWeightData cd;
					ar >> s;  cd.id = string(CT2CA(s));
					// Get conv node data
					ar >> save_buffer;
					if (save_buffer != _T("numnodes")) {
						AfxMessageBox(_T("serializeWeightData problem finding conv numnodes"));
						return(save_buffer);
					}
					ar >> datasize;
					if (datasize > 0) {
						for (int i = 0; i < datasize; i++) {
							nodeWeightData tdata;
							ar >> s;  tdata.id = string(CT2CA(s));
							ar >> tdata.bias;
							ar >> save_buffer;
							if (save_buffer != _T("numweights")) {
								AfxMessageBox(_T("serializeWeightData problem finding conv node numweights"));
								return(save_buffer);
							}
							ar >> weightdatasize;
							if (weightdatasize > 0) {
								for (int j = 0; j < weightdatasize; j++) {
									weightData wd;
									ar >> s;  wd.label = string(CT2CA(s));
									ar >> wd.weight;
									tdata.weightdata.push_back(wd);
								}
							}
							cd.nodeweightdata.push_back(tdata);
						}
					}
					// Get conv input layer data
					ar >> save_buffer;
					if (save_buffer != _T("numinputlayernodes")) {
						AfxMessageBox(_T("serializeWeightData problem finding conv input layer"));
						return(save_buffer);
					}
					ar >> datasize;
					if (datasize > 0) {
						for (int i = 0; i < datasize; i++) {
							nodeWeightData tdata;
							ar >> s;  tdata.id = string(CT2CA(s));
							ar >> tdata.bias;
							ar >> save_buffer;
							if (save_buffer != _T("numweights")) {
								AfxMessageBox(_T("serializeWeightData problem finding conv input layer numweights"));
								return(save_buffer);
							}
							ar >> weightdatasize;
							if (weightdatasize > 0) {
								for (int j = 0; j < weightdatasize; j++) {
									weightData wd;
									ar >> s;  wd.label = string(CT2CA(s));
									ar >> wd.weight;
									tdata.weightdata.push_back(wd);
								}
							}
							cd.inputlayerweightdata.push_back(tdata);
						}
					}
					pnetweightdata->convweightdata.push_back(cd);
				}
			}

			// Get net layer weights
			ar >> save_buffer;
			if (save_buffer != _T("numhiddennets")) {
				AfxMessageBox(_T("serializeWeightData problem finding num net layers"));
				return(save_buffer);
			}
			ar >> numlayers;
			if (numlayers > 0) {
				for (int k = 0; k < numlayers; k++) {
					hiddenWeightData nd;
					ar >> s;  nd.id = string(CT2CA(s));
					ar >> save_buffer;
					// Get net node data
					if (save_buffer != _T("numnodes")) {
						AfxMessageBox(_T("serializeWeightData problem finding net numnodes"));
						return(save_buffer);
					}
					ar >> datasize;
					if (datasize > 0) {
						for (int i = 0; i < datasize; i++) {
							nodeWeightData tdata;
							ar >> s;  tdata.id = string(CT2CA(s));
							ar >> tdata.bias;
							ar >> save_buffer;
							if (save_buffer != _T("numweights")) {
								AfxMessageBox(_T("serializeWeightData problem finding hidden net node numweights"));
								return(save_buffer);
							}
							ar >> weightdatasize;
							if (weightdatasize > 0) {
								for (int j = 0; j < weightdatasize; j++) {
									weightData wd;
									ar >> s;  wd.label = string(CT2CA(s));
									ar >> wd.weight;
									tdata.weightdata.push_back(wd);
								}
							}
							nd.nodeweightdata.push_back(tdata);
						}
					}
					pnetweightdata->hiddenweightdata.push_back(nd);
				}
			}

			ar >> save_buffer;
			if (save_buffer != _T("netweightsend")) {
				AfxMessageBox(_T("serializeWeightData problem finding netweightsstart"));
				return(save_buffer);
			}

			if (check_only) {
				/*
				typedef struct weightData {
					string label = "";
					double weight = 0.0;
				} weightData;

				typedef struct nodeWeightData {
					string id = "";
					double bias = 0.0;
					vector<weightData> weightdata;
				} nodeWeightData;

				typedef struct convWeightData {
					string id = "";
					vector<nodeWeightData> nodeweightdata;
					vector<nodeWeightData> inputlayerweightdata;
				} convWeightData;

				typedef struct hiddenWeightData {
					string id = "";
					vector<nodeWeightData> nodeweightdata;
				} hiddenWeightData;

				typedef struct netWeightData {
					vector<nodeWeightData> hiddennodeweightdata;
					vector<nodeWeightData> outputnodeweightdata;
					vector<convWeightData> convweightdata;
					vector<hiddenWeightData> hiddenweightdata;
				} netWeightData;
				*/
				if (netweightdata.hiddennodeweightdata.size() != mynetweightdata.hiddennodeweightdata.size()) {
					addErrorString("serializeWeightData : netweightdata.hiddennodeweightdata size missmatch");
				}
				else {
					for (int i = 0; i < (int)netweightdata.hiddennodeweightdata.size(); i++) {
						if (netweightdata.hiddennodeweightdata[i].id != mynetweightdata.hiddennodeweightdata[i].id) {
							addErrorString("serializeWeightData : netweightdata.hiddennodeweightdata id missmatch");
						}
						else if (netweightdata.hiddennodeweightdata[i].bias != mynetweightdata.hiddennodeweightdata[i].bias) {
							addErrorString("serializeWeightData : netweightdata.hiddennodeweightdata bias missmatch");
						}
						if (netweightdata.hiddennodeweightdata[i].weightdata.size() == mynetweightdata.hiddennodeweightdata[i].weightdata.size()) {
							for (int j = 0; j < (int)netweightdata.hiddennodeweightdata[i].weightdata.size(); j++) {
								if (netweightdata.hiddennodeweightdata[i].weightdata[j].label != mynetweightdata.hiddennodeweightdata[i].weightdata[j].label) {
									addErrorString("serializeWeightData : netweightdata.hiddennodeweightdata weight label missmatch");
								}
								else if (netweightdata.hiddennodeweightdata[i].weightdata[j].weight != mynetweightdata.hiddennodeweightdata[i].weightdata[j].weight) {
									addErrorString("serializeWeightData : netweightdata.hiddennodeweightdata weight missmatch");
								}
							}
						}
						else {
							addErrorString("serializeWeightData : netweightdata.hiddennodeweightdata.weightdata size missmatch");
						}
					}
				}

				if (netweightdata.outputnodeweightdata.size() != mynetweightdata.outputnodeweightdata.size()) {
					addErrorString("serializeWeightData : netweightdata.outputnodeweightdata size missmatch");
				}
				else {
					for (int i = 0; i < (int)netweightdata.outputnodeweightdata.size(); i++) {
						if (netweightdata.outputnodeweightdata[i].id != mynetweightdata.outputnodeweightdata[i].id) {
							addErrorString("serializeWeightData : netweightdata.outputnodeweightdata id missmatch");
						}
						else if (netweightdata.outputnodeweightdata[i].bias != mynetweightdata.outputnodeweightdata[i].bias) {
							addErrorString("serializeWeightData : netweightdata.outputnodeweightdata bias missmatch");
						}
						if (netweightdata.outputnodeweightdata[i].weightdata.size() == mynetweightdata.outputnodeweightdata[i].weightdata.size()) {
							for (int j = 0; j < (int)netweightdata.outputnodeweightdata[i].weightdata.size(); j++) {
								if (netweightdata.outputnodeweightdata[i].weightdata[j].label != mynetweightdata.outputnodeweightdata[i].weightdata[j].label) {
									addErrorString("serializeWeightData : netweightdata.outputnodeweightdata weight label missmatch");
								}
								else if (netweightdata.outputnodeweightdata[i].weightdata[j].weight != mynetweightdata.outputnodeweightdata[i].weightdata[j].weight) {
									addErrorString("serializeWeightData : netweightdata.outputnodeweightdata weight missmatch");
								}
							}
						}
						else {
							addErrorString("serializeWeightData : netweightdata.outputnodeweightdata.weightdata size missmatch");
						}
					}
				}

				if (netweightdata.convweightdata.size() != mynetweightdata.convweightdata.size()) {
					addErrorString("serializeWeightData : netweightdata.convweightdata size missmatch");
				}
				else {
					for (int i = 0; i < (int)netweightdata.convweightdata.size(); i++) {
						if (netweightdata.convweightdata[i].id != mynetweightdata.convweightdata[i].id) {
							addErrorString("serializeWeightData : netweightdata.convweightdata id missmatch");
						}
						if (netweightdata.convweightdata[i].nodeweightdata.size() == mynetweightdata.convweightdata[i].nodeweightdata.size()) {
							for (int j = 0; j < (int)netweightdata.convweightdata[i].nodeweightdata.size(); j++) {
								if (netweightdata.convweightdata[i].nodeweightdata[j].id != mynetweightdata.convweightdata[i].nodeweightdata[j].id) {
									addErrorString("serializeWeightData : netweightdata.convweightdata.nodeweightdata id missmatch");
								}
								else if (netweightdata.convweightdata[i].nodeweightdata[j].bias != mynetweightdata.convweightdata[i].nodeweightdata[j].bias) {
									addErrorString("serializeWeightData : netweightdata.convweightdata.nodeweightdata bias missmatch");
								}
								if (netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size() == mynetweightdata.convweightdata[i].nodeweightdata[j].weightdata.size()) {
									for (int k = 0; k < netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size(); k++) {
										if (netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].label != mynetweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].label) {
											addErrorString("serializeWeightData : netweightdata.convweightdata.nodeweightdata weight label missmatch");
										}
										else if (netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].weight != mynetweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].weight) {
											addErrorString("serializeWeightData : netweightdata.convweightdata.nodeweightdata weight missmatch");
										}
									}
								}
								else {
									addErrorString("serializeWeightData : netweightdata.convweightdata.nodeweightdata.weightdata size missmatch");
								}
							}
						}
						else {
							addErrorString("serializeWeightData : netweightdata.convweightdata.nodeweightdata size missmatch");
						}
						if (netweightdata.convweightdata[i].inputlayerweightdata.size() == mynetweightdata.convweightdata[i].inputlayerweightdata.size()) {
							for (int j = 0; j < (int)netweightdata.convweightdata[i].inputlayerweightdata.size(); j++) {
								if (netweightdata.convweightdata[i].inputlayerweightdata[j].id != mynetweightdata.convweightdata[i].inputlayerweightdata[j].id) {
									addErrorString("serializeWeightData : netweightdata.convweightdata.inputlayerweightdata id missmatch");
								}
								else if (netweightdata.convweightdata[i].inputlayerweightdata[j].bias != mynetweightdata.convweightdata[i].inputlayerweightdata[j].bias) {
									addErrorString("serializeWeightData : netweightdata.convweightdata.inputlayerweightdata bias missmatch");
								}
								if (netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size() == mynetweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size()) {
									for (int k = 0; k < netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size(); k++) {
										if (netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].label != mynetweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].label) {
											addErrorString("serializeWeightData : netweightdata.convweightdata.inputlayerweightdata weight label missmatch");
										}
										else if (netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].weight != mynetweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].weight) {
											addErrorString("serializeWeightData : netweightdata.convweightdata.inputlayerweightdata weight missmatch");
										}
									}
								}
								else {
									addErrorString("serializeWeightData : netweightdata.convweightdata.nodeweightdata.inputlayerweightdata size missmatch");
								}
							}
						}
						else {
							addErrorString("serializeWeightData : netweightdata.convweightdata.inputlayerweightdata size missmatch");
						}
					}
				}

				if (netweightdata.hiddenweightdata.size() != mynetweightdata.hiddenweightdata.size()) {
					addErrorString("serializeWeightData : netweightdata.hiddenweightdata size missmatch");
				}
				else {
					for (int i = 0; i < (int)netweightdata.hiddenweightdata.size(); i++) {
						if (netweightdata.hiddenweightdata[i].id != mynetweightdata.hiddenweightdata[i].id) {
							addErrorString("serializeWeightData : netweightdata.hiddenweightdata id missmatch");
						}
						if (netweightdata.hiddenweightdata[i].nodeweightdata.size() == mynetweightdata.hiddenweightdata[i].nodeweightdata.size()) {
							for (int j = 0; j < (int)netweightdata.hiddenweightdata[i].nodeweightdata.size(); j++) {
								if (netweightdata.hiddenweightdata[i].nodeweightdata[j].id != mynetweightdata.hiddenweightdata[i].nodeweightdata[j].id) {
									addErrorString("serializeWeightData : netweightdata.hiddenweightdata.nodeweightdata id missmatch");
								}
								else if (netweightdata.hiddenweightdata[i].nodeweightdata[j].bias != mynetweightdata.hiddenweightdata[i].nodeweightdata[j].bias) {
									addErrorString("serializeWeightData : netweightdata.hiddenweightdata.nodeweightdata bias missmatch");
								}
								if (netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size() == mynetweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size()) {
									for (int k = 0; k < netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size(); k++) {
										if (netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].label != mynetweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].label) {
											addErrorString("serializeWeightData : netweightdata.hiddenweightdata.nodeweightdata weight label missmatch");
										}
										else if (netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].weight != mynetweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].weight) {
											addErrorString("serializeWeightData : netweightdata.hiddenweightdata.nodeweightdata weight missmatch");
										}
									}
								}
								else {
									addErrorString("serializeWeightData : netweightdata.hiddenweightdata.nodeweightdata.weightdata size missmatch");
								}
							}
						}
						else {
							addErrorString("serializeWeightData : netweightdata.hiddenweightdata.nodeweightdata size missmatch");
						}
					}
				}


				if (netweightdata.hiddenweightdata.size() != mynetweightdata.hiddenweightdata.size()) {
					addErrorString("serializeWeightData : netweightdata.hiddenweightdata size missmatch");
				}
				else {

				}




			}

		}
		return(_T(""));
	}
#endif

	bool Serialize(CJSON* myjson, bool storing)
	{
		if (storing) {

			myjson->clear();

			myjson->addLineFeed();
			myjson->openCollection("");
			int cnt = getNoDefaultCount();
			myjson->addIntKey("netDataSize", cnt, true);// (int)netData.size(), true);
			if (netData.size() > 0) {
				for (int i = 0; i < (int)netData.size(); i++) {
					if (netData[i].id != "Default") {
						netData[i].Serialize(myjson, storing);
						myjson->addComma();
						myjson->addLineFeed();
					}
				}
			}
			serializeWeightData(myjson, storing);
			myjson->closeCollection(false, true);
			myjson->addLineFeed();

		}
		else {

			jsonLineData pdata;
			int netdatasize = 0;

			if (!myjson->parseNextOpenCollection()) { return(false); };

			if (!myjson->parseNextKeyValueInt("netDataSize", &netdatasize)) { return(false); };

			for (int i = 0; i < netdatasize; i++) {
				push_back(mydefault);
				if (netData[i].Serialize(myjson, storing)) {
					if (netData[i].getID() != netData[i].getLabel()) {
						if (!netData[i].isAssemblyLayer() && !(netData[i].isNode() && netData[i].getNodeFunction() == "Connector")) {
							string s = "Observation: ID " + netData[i].getID() + " not equal to label " + netData[i].getLabel();  addErrorString(s);
							netData[i].setLabel(netData[i].getID());
						}
					}
				}
				else {
					char s[200];  sprintf_s(s, "i = %i", i);  string msg = "Serialize : cancelling : ";  addErrorString(msg + s);  return(false);
				}
			}

			if (!serializeWeightData(myjson, storing)) {
				return(false);
			}

			addErrorString("Loading net complete");

		}

		return(true);

	}

	bool serializeWeightData(CJSON* myjson, bool storing)
	{

		if (storing) {

			myjson->addLineFeed();
			myjson->addComment("Hidden Node Weight Data ****************************************", true);
			myjson->openCollection("hiddenNodeWeightData");
			if (netweightdata.hiddennodeweightdata.size() > 0) {
				addErrorString("Setting hidden node weights");
				myjson->addIntKey("numhiddennodes", (int)netweightdata.hiddennodeweightdata.size(), true);
				for (int i = 0; i < (int)netweightdata.hiddennodeweightdata.size(); i++) {
					myjson->openCollection(netweightdata.hiddennodeweightdata[i].id);
					myjson->addDoubleKey2("bias", netweightdata.hiddennodeweightdata[i].bias, true);
					myjson->addIntKey("numweights", (int)netweightdata.hiddennodeweightdata[i].weightdata.size(), true);
					if (netweightdata.hiddennodeweightdata[i].weightdata.size() > 0) {
						myjson->openCollection("weights");
						for (int j = 0; j < (int)netweightdata.hiddennodeweightdata[i].weightdata.size(); j++) {
							if (j < ((int)netweightdata.hiddennodeweightdata[i].weightdata.size() - 1)) {
								myjson->addDoubleKey2(netweightdata.hiddennodeweightdata[i].weightdata[j].label, netweightdata.hiddennodeweightdata[i].weightdata[j].weight, true);
							}
							else {
								myjson->addDoubleKey2(netweightdata.hiddennodeweightdata[i].weightdata[j].label, netweightdata.hiddennodeweightdata[i].weightdata[j].weight, false);
							}
						}
						myjson->closeCollection(false, true);
					}
					if (i < ((int)netweightdata.hiddennodeweightdata.size() - 1)) {
						myjson->closeCollection(true, true);
					}
					else {
						myjson->closeCollection(false, true);
					}
				}
			}
			else {
				myjson->addIntKey("numhiddennodes", (int)netweightdata.hiddennodeweightdata.size(), false);
			}
			myjson->closeCollection(true, true);


			myjson->addLineFeed();
			myjson->addComment("Output Node Weight Data ****************************************", true);
			myjson->openCollection("outputNodeWeightData");
			if (netweightdata.outputnodeweightdata.size() > 0) {
				addErrorString("Setting output node weights");
				myjson->addIntKey("numoutputnodes", (int)netweightdata.outputnodeweightdata.size(), true);
				for (int i = 0; i < (int)netweightdata.outputnodeweightdata.size(); i++) {
					myjson->openCollection(netweightdata.outputnodeweightdata[i].id);
					myjson->addDoubleKey2("bias", netweightdata.outputnodeweightdata[i].bias, true);
					myjson->addIntKey("numweights", (int)netweightdata.outputnodeweightdata[i].weightdata.size(), true);
					if (netweightdata.outputnodeweightdata[i].weightdata.size() > 0) {
						myjson->openCollection("weights");
						for (int j = 0; j < (int)netweightdata.outputnodeweightdata[i].weightdata.size(); j++) {
							if (j < ((int)netweightdata.outputnodeweightdata[i].weightdata.size() - 1)) {
								myjson->addDoubleKey2(netweightdata.outputnodeweightdata[i].weightdata[j].label, netweightdata.outputnodeweightdata[i].weightdata[j].weight, true);
							}
							else {
								myjson->addDoubleKey2(netweightdata.outputnodeweightdata[i].weightdata[j].label, netweightdata.outputnodeweightdata[i].weightdata[j].weight, false);
							}
						}
						myjson->closeCollection(false, true);
					}
					if (i < ((int)netweightdata.outputnodeweightdata.size() - 1)) {
						myjson->closeCollection(true, true);
					}
					else {
						myjson->closeCollection(false, true);
					}
				}
			}
			else {
				myjson->addIntKey("numoutputnodes", (int)netweightdata.outputnodeweightdata.size(), false);
			}
			myjson->closeCollection(true, true);


			myjson->addLineFeed();
			myjson->addComment("Convolution Weight Data ****************************************", true);
			myjson->openCollection("convolutionWeightData");
			if (netweightdata.convweightdata.size() > 0) {
				addErrorString("Setting conv layer weights");
				myjson->addIntKey("numconvolutions", (int)netweightdata.convweightdata.size(), true);
				for (int i = 0; i < (int)netweightdata.convweightdata.size(); i++) {
					myjson->openCollection(netweightdata.convweightdata[i].id);
					myjson->addIntKey("numnodes", (int)netweightdata.convweightdata[i].nodeweightdata.size(), true);
					if (netweightdata.convweightdata[i].nodeweightdata.size() > 0) {
						myjson->openCollection("nodes");
						for (int j = 0; j < (int)netweightdata.convweightdata[i].nodeweightdata.size(); j++) {
							myjson->openCollection(netweightdata.convweightdata[i].nodeweightdata[j].id);
							myjson->addDoubleKey2("bias", netweightdata.convweightdata[i].nodeweightdata[j].bias, true);
							myjson->addIntKey("numweights", (int)netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size(), true);
							if ((int)netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size() > 0) {
								myjson->openCollection("weights");
								for (int k = 0; k < (int)netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size(); k++) {
									if (k < ((int)netweightdata.convweightdata[i].nodeweightdata[j].weightdata.size() - 1)) {
										myjson->addDoubleKey2(netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].label, netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].weight, true);
									}
									else {
										myjson->addDoubleKey2(netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].label, netweightdata.convweightdata[i].nodeweightdata[j].weightdata[k].weight, false);
									}
								}
								myjson->closeCollection(false, true);
							}
							if (j < ((int)netweightdata.convweightdata[i].nodeweightdata.size() - 1)) {
								myjson->closeCollection(true, true);
							}
							else {
								myjson->closeCollection(false, true);
							}
						}
						myjson->closeCollection(true, true);
					}
					if (netweightdata.convweightdata[i].inputlayerweightdata.size() > 0) {
						myjson->addIntKey("numinputlayernodes", (int)netweightdata.convweightdata[i].inputlayerweightdata.size(), true);
						for (int j = 0; j < (int)netweightdata.convweightdata[i].inputlayerweightdata.size(); j++) {
							myjson->openCollection(netweightdata.convweightdata[i].inputlayerweightdata[j].id);
							myjson->addDoubleKey2("bias", netweightdata.convweightdata[i].inputlayerweightdata[j].bias, true);
							myjson->addIntKey("numweights", (int)netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size(), true);
							if ((int)netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size() > 0) {
								myjson->openCollection("weights");
								for (int k = 0; k < (int)netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size(); k++) {
									if (k < ((int)netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata.size() - 1)) {
										myjson->addDoubleKey2(netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].label, netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].weight, true);
									}
									else {
										myjson->addDoubleKey2(netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].label, netweightdata.convweightdata[i].inputlayerweightdata[j].weightdata[k].weight, false);
									}
								}
								myjson->closeCollection(false, true);
							}
							if (j < ((int)netweightdata.convweightdata[i].inputlayerweightdata.size() - 1)) {
								myjson->closeCollection(true, true);
							}
							else {
								myjson->closeCollection(false, true);
							}
						}
					}
					else {
						myjson->addIntKey("numinputlayernodes", (int)netweightdata.convweightdata[i].inputlayerweightdata.size(), false);
					}
					if (i < ((int)netweightdata.convweightdata.size() - 1)) {
						myjson->closeCollection(true, true);
					}
					else {
						myjson->closeCollection(false, true);
					}
				}
			}
			else {
				myjson->addIntKey("numconvolutions", (int)netweightdata.convweightdata.size(), false);
			}
			myjson->closeCollection(true, true);


			myjson->addLineFeed();
			myjson->addComment("Hidden Net Weight Data ****************************************", true);
			myjson->openCollection("hiddenNetWeightData");
			if (netweightdata.hiddenweightdata.size() > 0) {
				addErrorString("Setting hidden net weights");
				myjson->addIntKey("numhiddennets", (int)netweightdata.hiddenweightdata.size(), true);
				for (int i = 0; i < (int)netweightdata.hiddenweightdata.size(); i++) {
					myjson->openCollection(netweightdata.hiddenweightdata[i].id);
					myjson->addIntKey("numnodes", (int)netweightdata.hiddenweightdata[i].nodeweightdata.size(), true);
					if (netweightdata.hiddenweightdata[i].nodeweightdata.size() > 0) {
						myjson->openCollection("nodes");
						for (int j = 0; j < (int)netweightdata.hiddenweightdata[i].nodeweightdata.size(); j++) {
							myjson->openCollection(netweightdata.hiddenweightdata[i].nodeweightdata[j].id);
							myjson->addDoubleKey2("bias", netweightdata.hiddenweightdata[i].nodeweightdata[j].bias, true);
							myjson->addIntKey("numweights", (int)netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size(), true);
							if ((int)netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size() > 0) {
								myjson->openCollection("weights");
								for (int k = 0; k < (int)netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size(); k++) {
									if (k < ((int)netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata.size() - 1)) {
										myjson->addDoubleKey2(netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].label, netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].weight, true);
									}
									else {
										myjson->addDoubleKey2(netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].label, netweightdata.hiddenweightdata[i].nodeweightdata[j].weightdata[k].weight, false);
									}
								}
								myjson->closeCollection(false, true);
							}
							if (j < ((int)netweightdata.hiddenweightdata[i].nodeweightdata.size() - 1)) {
								myjson->closeCollection(true, true);
							}
							else {
								myjson->closeCollection(false, true);
							}
						}
						myjson->closeCollection(false, true);
					}
					if (i < ((int)netweightdata.hiddenweightdata.size() - 1)) {
						myjson->closeCollection(true, true);
					}
					else {
						myjson->closeCollection(false, true);
					}
				}
			}
			else {
				myjson->addIntKey("numhiddennets", (int)netweightdata.hiddenweightdata.size(), false);
			}
			myjson->closeCollection(false, true);

			myjson->addLineFeed();

		}
		else {

			int tint = 0;
			int numnodes = 0;
			int numconvs = 0;
			int numnets = 0;
			int numweights = 0;
			int numinputlayernodes = 0;
			string cid = "";
			string tstring = "";

			netweightdata.hiddennodeweightdata.clear();
			netweightdata.outputnodeweightdata.clear();
			netweightdata.convweightdata.clear();
			netweightdata.hiddenweightdata.clear();


			// Getting hidden node weights
			if (!myjson->parseNextKeyValueString(&cid, &tstring)) { return(false); };
			if (cid.substr(0, 7) != "Comment") { addErrorString("serializeWeightData : looking for Comment : " + cid);  return(false); }
			if (!myjson->parseNextOpenCollection("hiddenNodeWeightData")) { return(false); }
			if (!myjson->parseNextKeyValueInt("numhiddennodes", &numnodes)) { return(false); };
			if (numnodes > 0) {
				addErrorString("Getting hidden node weights");
				for (int i = 0; i < numnodes; i++) {
					nodeWeightData tdata;
					if (myjson->parseNextOpenCollection(&tdata.id)) {
						if (!myjson->parseNextKeyValueDouble("bias", &tdata.bias)) { return(false); };
						//char s[200]; sprintf_s(s, " : bias = %.16e", tdata.bias);  addErrorString("hidden node : " + tdata.id + s);
						if (!myjson->parseNextKeyValueInt("numweights", &numweights)) { return(false); };
						if (numweights > 0) {
							if (!myjson->parseNextOpenCollection("weights")) { return(false); }
							for (int j = 0; j < numweights; j++) {
								weightData wd;
								if (!myjson->parseNextKeyValueDouble(&wd.label, &wd.weight)) { return(false); };
								//sprintf_s(s, " : weight = %.16e", wd.weight);  addErrorString("hidden node : " + tdata.id + " : " + wd.label + s);
								tdata.weightdata.push_back(wd);
							}
							if (!myjson->parseNextCloseCollection()) { return(false); }
						}
						netweightdata.hiddennodeweightdata.push_back(tdata);
					}
					else {
						addErrorString("serializeWeightData : problem opening collection : " + tdata.id);  return(false);
					}
					if (!myjson->parseNextCloseCollection()) { return(false); }
				}
			}
			if (!myjson->parseNextCloseCollection()) { return(false); }


			// Getting output node weights
			if (!myjson->parseNextKeyValueString(&cid, &tstring)) { return(false); };
			if (cid.substr(0, 7) != "Comment") { addErrorString("serializeWeightData : looking for Comment : " + cid);  return(false); }
			if (!myjson->parseNextOpenCollection("outputNodeWeightData")) { return(false); }
			if (!myjson->parseNextKeyValueInt("numoutputnodes", &numnodes)) { return(false); };
			if (numnodes > 0) {
				addErrorString("Getting output node weights");
				for (int i = 0; i < numnodes; i++) {
					nodeWeightData tdata;
					if (myjson->parseNextOpenCollection(&tdata.id)) {
						if (!myjson->parseNextKeyValueDouble("bias", &tdata.bias)) { return(false); };
						//char s[200]; sprintf_s(s, " : bias = %.16e", tdata.bias);  addErrorString("output node : " + tdata.id + s);
						if (!myjson->parseNextKeyValueInt("numweights", &numweights)) { return(false); };
						if (numweights > 0) {
							if (!myjson->parseNextOpenCollection("weights")) { return(false); }
							for (int j = 0; j < numweights; j++) {
								weightData wd;
								if (!myjson->parseNextKeyValueDouble(&wd.label, &wd.weight)) { return(false); };
								//sprintf_s(s, " : weight = %.16e", wd.weight);  addErrorString("output node : " + tdata.id + " : " + wd.label + s);
								tdata.weightdata.push_back(wd);
							}
							if (!myjson->parseNextCloseCollection()) { return(false); }
						}
						netweightdata.outputnodeweightdata.push_back(tdata);
					}
					else {
						addErrorString("serializeWeightData : problem opening collection : " + tdata.id);  return(false);
					}
					if (!myjson->parseNextCloseCollection()) { return(false); }
				}
			}
			if (!myjson->parseNextCloseCollection()) { return(false); }


			// Get conv layer weights
			if (!myjson->parseNextKeyValueString(&cid, &tstring)) { return(false); };
			if (cid.substr(0, 7) != "Comment") { addErrorString("serializeWeightData : looking for Comment : " + cid);  return(false); }
			if (!myjson->parseNextOpenCollection("convolutionWeightData")) { return(false); }
			if (!myjson->parseNextKeyValueInt("numconvolutions", &numconvs)) { return(false); };
			if (numconvs > 0) {
				addErrorString("Getting conv layer weights");
				for (int i = 0; i < numconvs; i++) {
					convWeightData cd;
					if (myjson->parseNextOpenCollection(&cd.id)) {
						if (!myjson->parseNextKeyValueInt("numnodes", &numnodes)) { return(false); };
						if (numnodes > 0) {
							if (!myjson->parseNextOpenCollection("nodes")) { return(false); }
							for (int j = 0; j < numnodes; j++) {
								nodeWeightData tdata;
								if (!myjson->parseNextOpenCollection(&tdata.id)) { return(false); }
								if (!myjson->parseNextKeyValueDouble("bias", &tdata.bias)) { return(false); };
								if (!myjson->parseNextKeyValueInt("numweights", &numweights)) { return(false); };
								if (numweights > 0) {
									if (!myjson->parseNextOpenCollection("weights")) { return(false); }
									for (int k = 0; k < numweights; k++) {
										weightData wd;
										if (!myjson->parseNextKeyValueDouble(&wd.label, &wd.weight)) { return(false); };
										//sprintf_s(s, " : weight = %.16e", wd.weight);  addErrorString("output node : " + tdata.id + " : " + wd.label + s);
										tdata.weightdata.push_back(wd);
									}
									if (!myjson->parseNextCloseCollection()) { return(false); }
								}
								if (!myjson->parseNextCloseCollection()) { return(false); }
								cd.nodeweightdata.push_back(tdata);
							}
							if (!myjson->parseNextCloseCollection()) { return(false); }
						}
						if (!myjson->parseNextKeyValueInt("numinputlayernodes", &numinputlayernodes)) { return(false); };
						if (numinputlayernodes > 0) {
							if (!myjson->parseNextOpenCollection("nodes")) { return(false); }
							for (int j = 0; j < numnodes; j++) {
								nodeWeightData tdata;
								if (!myjson->parseNextOpenCollection(&tdata.id)) { return(false); }
								if (!myjson->parseNextKeyValueDouble("bias", &tdata.bias)) { return(false); };
								if (!myjson->parseNextKeyValueInt("numweights", &numweights)) { return(false); };
								if (numweights > 0) {
									if (!myjson->parseNextOpenCollection("weights")) { return(false); }
									for (int k = 0; k < numweights; k++) {
										weightData wd;
										if (!myjson->parseNextKeyValueDouble(&wd.label, &wd.weight)) { return(false); };
										//sprintf_s(s, " : weight = %.16e", wd.weight);  addErrorString("output node : " + tdata.id + " : " + wd.label + s);
										tdata.weightdata.push_back(wd);
									}
									if (!myjson->parseNextCloseCollection()) { return(false); }
								}
								if (!myjson->parseNextCloseCollection()) { return(false); }
								cd.nodeweightdata.push_back(tdata);
							}
							if (!myjson->parseNextCloseCollection()) { return(false); }
						}
					}
					if (!myjson->parseNextCloseCollection()) { return(false); }
					netweightdata.convweightdata.push_back(cd);
				}
			}
			if (!myjson->parseNextCloseCollection()) { return(false); }


			// Get net layer weights
			if (!myjson->parseNextKeyValueString(&cid, &tstring)) { return(false); };
			if (cid.substr(0, 7) != "Comment") { addErrorString("serializeWeightData : looking for Comment : " + cid);  return(false); }
			if (!myjson->parseNextOpenCollection("hiddenNetWeightData")) { return(false); }
			if (!myjson->parseNextKeyValueInt("numhiddennets", &numnets)) { return(false); };
			if (numnets > 0) {
				addErrorString("Getting net layer weights");
				for (int i = 0; i < numnets; i++) {
					hiddenWeightData nd;
					if (myjson->parseNextOpenCollection(&nd.id)) {
						if (!myjson->parseNextKeyValueInt("numnodes", &numnodes)) { return(false); };
						if (numnodes > 0) {
							if (!myjson->parseNextOpenCollection("nodes")) { return(false); }
							for (int j = 0; j < numnodes; j++) {
								nodeWeightData tdata;
								if (!myjson->parseNextOpenCollection(&tdata.id)) { return(false); }
								if (!myjson->parseNextKeyValueDouble("bias", &tdata.bias)) { return(false); };
								if (!myjson->parseNextKeyValueInt("numweights", &numweights)) { return(false); };
								if (numweights > 0) {
									if (!myjson->parseNextOpenCollection("weights")) { return(false); }
									for (int k = 0; k < numweights; k++) {
										weightData wd;
										if (!myjson->parseNextKeyValueDouble(&wd.label, &wd.weight)) { return(false); };
										//sprintf_s(s, " : weight = %.16e", wd.weight);  addErrorString("output node : " + tdata.id + " : " + wd.label + s);
										tdata.weightdata.push_back(wd);
									}
									if (!myjson->parseNextCloseCollection()) { return(false); }
								}
								if (!myjson->parseNextCloseCollection()) { return(false); }
								nd.nodeweightdata.push_back(tdata);
							}
							if (!myjson->parseNextCloseCollection()) { return(false); }
						}
					}
					if (!myjson->parseNextCloseCollection()) { return(false); }
					netweightdata.hiddenweightdata.push_back(nd);
				}
			}
			if (!myjson->parseNextCloseCollection()) { return(false); }


		}

		return(true);

	}

	/*void saveAsCSV(string fname) {

		//AfxMessageBox(fname);
		
		CFile file;

		// Open csv file
		if (!file.Open((CString)fname.c_str(), CFile::modeCreate | CFile::modeReadWrite)) {
			string msg = "saveAsCSV : Unable to open " + fname;
			addErrorString(msg);
			return;
		}

		// Allocate memory used to parse lines of data
		unsigned long max_data = 50000;
		char* cdata = new char[max_data];
		if (cdata == 0) {
			addErrorString("saveAsCSV : Unable to allocate memory to save csv file");
			return;
		}

		string sline = "";

		for (int i = 0; i < netData.size(); i++) {

			sline = "";

			if (isNode(i)) {

				char s[200];

				sline += "Node=";
				sline += getID(i);

				if (getNodeMode(i) == "Input") {
					sline += ",Mode=Input";
					sline += ",DataField=";
					sline += getDataField(i);
					sline += ",DataIndexMode=";
					sline += getNodeIndexMode(i);
					sline += ",DataIndex1=";
					sprintf_s(s, "%i", getNodeXIndex(i));
					sline += s;
					sline += ",DataIndex2=";
					sprintf_s(s, "%i", getNodeYIndex(i));
					sline += s;
					sline += "\n";
				}
				else if (getNodeMode(i) == "Output") {
					sline += ",Mode=Output";
					sline += ",Function=";
					sline += getNodeFunction(i);
					sline += ",Eta=";
					sprintf_s(s, "%16e", getNodeEta(i));
					sline += s;
					sline += ",Alpha=";
					sprintf_s(s, "%16e", getNodeAlpha(i));
					sline += s;
					sline += ",Rand=";
					sprintf_s(s, "%16e", getNodeRand(i));
					sline += s;
					if (areMultipleBias(i)) {
						int num = getNumBias(i);
						for (int j = 0; j < num; j++) {
							if (j == 0) {
								sline += ",Biases=";
							}
							else {
								sline += ",";
							}
							sprintf_s(s, "%16e", getNodeBias(i, j));
							sline += s;
						}
					}
					else {
						sline += ",Bias=";
						sprintf_s(s, "%16e", getNodeBias(i));
						sline += s;
					}
					sline += ",Trainable=";
					sprintf_s(s, "%i", getNodeTrainable(i));
					sline += s;
					sline += ",DataField=";
					sline += getDataField(i);
					sline += ",DataIndexMode=";
					sline += getNodeIndexMode(i);
					sline += ",DataIndex1=";
					sprintf_s(s, "%i", getNodeXIndex(i));
					sline += s;
					sline += ",DataIndex2=";
					sprintf_s(s, "%i", getNodeYIndex(i));
					sline += s;
					sline += ",PreProcessor=";
					sline += getNodePreProcessor(i);
					sline += ",PreProcessorLowerNormLimit=";
					sprintf_s(s, "%16e", getNodeLowerNormLimit(i));
					sline += s;
					sline += ",PreProcessorUpperNormLimit=";
					sprintf_s(s, "%16e", getNodeUpperNormLimit(i));
					sline += s;
					int cindex = 0;
					for (int j = 0; j < netData.size(); j++) {
						if (isConnect(j)) {
							string id = getID(i);
							if (id == getOutputNode(j)) {
								sprintf_s(s, "%i", cindex);
								sline += ",InputNodeID";
								sline += s;
								sline += "=";
								sline += getInputNode(j);
								if (areMultipleWeights(j)) {
									int wnum = getNumWeights(j);
									for (int k = 0; k < wnum; k++) {
										if (k == 0) {
											sline += ",Weights=";
										}
										else {
											sline += ",";
										}
										sprintf_s(s, "%16e", getWeight(j, k));
										sline += s;
									}
								}
								else {
									sline += ",Weight=";
									sprintf_s(s, "%16e", getWeight(j));
									sline += s;
								}

								cindex++;
							}
						}
					}
					sline += "\n";
				}
				else if (getNodeMode(i) == "Hidden") {
					sline += ",Mode=Hidden";
					sline += ",Function=";
					sline += getNodeFunction(i);
					sline += ",Eta=";
					sprintf_s(s, "%16e", getNodeEta(i));
					sline += s;
					sline += ",Alpha=";
					sprintf_s(s, "%16e", getNodeAlpha(i));
					sline += s;
					sline += ",Rand=";
					sprintf_s(s, "%16e", getNodeRand(i));
					sline += s;
					if (areMultipleBias(i)) {
						int num = getNumBias(i);
						for (int j = 0; j < num; j++) {
							if (j == 0) {
								sline += ",Biases=";
							}
							else {
								sline += ",";
							}
							sprintf_s(s, "%16e", getNodeBias(i, j));
							sline += s;
						}
					}
					else {
						sline += ",Bias=";
						sprintf_s(s, "%16e", getNodeBias(i));
						sline += s;
					}
					sline += ",Trainable=";
					sprintf_s(s, "%i", getNodeTrainable(i));
					sline += s;
					int cindex = 0;
					for (int j = 0; j < netData.size(); j++) {
						if (isConnect(j)) {
							string id = getID(i);
							if (id == getOutputNode(j)) {
								sprintf_s(s, "%i", cindex);
								sline += ",InputNodeID";
								sline += s;
								sline += "=";
								sline += getInputNode(j);
								if (areMultipleWeights(j)) {
									int wnum = getNumWeights(j);
									for (int k = 0; k < wnum; k++) {
										if (k == 0) {
											sline += ",Weights=";
										}
										else {
											sline += ",";
										}
										sprintf_s(s, "%16e", getWeight(j, k));
										sline += s;
									}
								}
								else {
									sline += ",Weight=";
									sprintf_s(s, "%16e", getWeight(j));
									sline += s;
								}
								cindex++;
							}
						}
					}
					sline += "\n";
				}
				else if (getNodeMode(i) == "PreProcess") {
					sline += ",Mode=PreProcess";
					sline += ",PreProcessor=";
					sline += getNodePreProcessor(i);
					sline += ",PreProcessorTime=";
					sline += getNodePreProcessorTime(i);
					sline += ",PreProcessorLowerNormLimit=";
					sprintf_s(s, "%16e", getNodeLowerNormLimit(i));
					sline += s;
					sline += ",PreProcessorUpperNormLimit=";
					sprintf_s(s, "%16e", getNodeUpperNormLimit(i));
					sline += s;
					sline += ",PreProcessorNormDisallow=";
					sprintf_s(s, "%i", getNodeNormDisallow(i));
					sline += s;
					sline += ",PreProcessorStride=";
					sprintf_s(s, "%i", getNodePreProcessorStride(i));
					sline += s;
					int cindex = 0;
					for (int j = 0; j < netData.size(); j++) {
						if (isConnect(j)) {
							string id = getID(i);
							if (id == getOutputNode(j)) {
								sprintf_s(s, "%i", cindex);
								sline += ",InputNodeID";
								sline += s;
								sline += "=";
								sline += getInputNode(j);
								cindex++;
							}
						}
					}
					sline += "\n";
				}
				else {
					sprintf_s(s, "saveAsCSV : node %i is ", i);
					addErrorString(s + getNodeMode(i) + " unknown");
				}
			}

			if (sline.length() < int(max_data)) {
				for (int j = 0; j < sline.length(); j++) {
					cdata[j] = (char)sline[j];
				}
			}
			file.Write(cdata, (int)sline.length());

		}

		file.Flush();
		file.Close();

		delete[] cdata;

		addErrorString("Finished saving net as CSV");

	}*/

	void addErrorString(string err);



	//****************************************************************************
	// netData helper functions

	bool getIsOutput(int node_index)
	{
		if (getNodeMode(node_index) == "Output") {
			return(true);
		}
		return(false);
	}

	double getWeightFromNetData(string id, int windex)
	{
		if (hasSingleColon(id)) {
			static bool s4 = false;
			if (!s4) {
				addErrorString("getWeightFromNetData - need processing for layer output");
				s4 = true;
			}
		}
		return(getWeight(windex, getIndexFromID(id)));
	}

	double getBiasFromNetData(string id)
	{
		if (hasSingleColon(id)) {
			static bool s5 = false;
			if (!s5) {
				addErrorString("getBiasFromNetData - need processing for layer output");
				s5 = true;
			}
		}
		return(getNodeBias(getIndexFromID(id)));
	}

	int getConvType(string id)
	{
		for (int i = 0; i < size(); i++) {
			if (isConvLayer(i) || isAttentionLayer(i) || isDropout(i) || isAssemblyLayer(i)) {
				if (getID(i) == id) {
					return(getConvType(i));
				}
			}
		}
		return(NONE);
	}

	bool isProcessNode(string nname)
	{
		for (int i = 0; i < size(); i++) {
			if (isNode(i)) {
				if (getID(i) == nname) {
					if (getNodeMode(i) == "PreProcess") {
						return(true);
					}
				}
			}
		}
		return(false);
	}

	bool isProcessNode(int index)
	{
		if (isNode(index)) {
			if (getNodeMode(index) == "PreProcess") {
				return(true);
			}
		}
		return(false);
	}

	int getNodesPerLayerFromHiddenNet(string cname)
	{
		for (int i = 0; i < size(); i++) {
			if (isConvLayer(i)) {
				if (getID(i) == cname) {
					if (getConvType(i) == HIDDEN_NET) {
						return(getConvNodesPerLayer(i));
					}
				}
			}
		}
		return(0);
	}

	int getAttentionDk(int cindex)
	{
		if (isAttentionLayer(cindex)) {
			return(getDk(cindex));
		}
		return(0);
	}

	int getAttentionDv(int cindex)
	{
		if (isAttentionLayer(cindex) || isNetCluster(cindex)) {
			return(getDv(cindex));
		}
		return(0);
	}

	int getAttentionDm(int cindex)
	{
		if (isAttentionLayer(cindex) || isConvLayer(cindex) || isNetCluster(cindex)) {
			return(getD(cindex));
		}
		return(0);
	}

	double getAttentionSumAddLimit(int cindex)
	{
		if (isAttentionLayer(cindex) || isConvLayer(cindex)) {
			return(getSumAddLimit(cindex));
		}
		return(0.01);
	}

	int getAttentionOutputSize(int cindex)
	{
		if (isAttentionLayer(cindex)) {
			int dv = getDv(cindex);
			int dm = getD(cindex);
			int output_size = dv * dm;
			return(output_size);
		}
		return(0);
	}

	string getAttentionKConnection(string attentionID)
	{
		string rval = "";
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string input = getStringBeforeColon(getInputNode(i));
				string output = getStringBeforeColon(getOutputNode(i));
				if (output == attentionID) {
					int cnumber = getSecondNumberAfterColon(getOutputNode(i));
					if (cnumber == 1) {
						return(input);
					}
				}
			}
		}
		return(rval);
	}

	string getAttentionQConnection(string attentionID)
	{
		string rval = "";
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string input = getStringBeforeColon(getInputNode(i));
				string output = getStringBeforeColon(getOutputNode(i));
				if (output == attentionID) {
					int cnumber = getSecondNumberAfterColon(getOutputNode(i));
					if (cnumber == 0) {
						return(input);
					}
				}
			}
		}
		return(rval);
	}

	string getAttentionVConnection(string attentionID)
	{
		string rval = "";
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string input = getStringBeforeColon(getInputNode(i));
				string output = getStringBeforeColon(getOutputNode(i));
				if (output == attentionID) {
					int cnumber = getSecondNumberAfterColon(getOutputNode(i));
					if (cnumber == 2) {
						return(input);
					}
				}
			}
		}
		return(rval);
	}

	bool isConvLayerOnly(int cindex)
	{
		if (isConvLayer(cindex) && (getConvType(cindex) != HIDDEN_NET) && (getConvType(cindex) != ATTENTION)) {
			return(true);
		}
		return(false);
	}

	bool isHiddenNetOnly(int cindex)
	{
		if (isConvLayer(cindex) && (getConvType(cindex) == HIDDEN_NET)) {
			return(true);
		}
		return(false);
	}

	bool isAttentionLayerOnly(int cindex)
	{
		if (isAttentionLayer(cindex)) {
			return(true);
		}
		return(false);
	}

	bool isSelfAttentionLayerOnly(int cindex)
	{
		if (isAttentionLayer(cindex)) {
			if (getConvType(cindex) == SELFATTENTION) {
				return(true);
			}
		}
		return(false);
	}

	bool isNetClusterOnly(int cindex)
	{
		if (isNetCluster(cindex)) {
			if (getConvType(cindex) == NETCLUSTER) {
				return(true);
			}
		}
		return(false);
	}

	bool isDropoutOnly(int cindex)
	{
		if (isDropout(cindex)) {
			if (getConvType(cindex) == DROPOUT) {
				return(true);
			}
		}
		return(false);
	}

	bool getLayerResiduals(int cindex)
	{
		if (isConvLayer(cindex)) {
			if (getResiduals(cindex) == "Yes") {
				return(true);
			}
		}
		return(false);
	}

	bool getLayerNormalization(int cindex)
	{
		if (isConvLayer(cindex)) {
			if (getNormalization(cindex) == "Yes") {
				return(true);
			}
		}
		return(false);
	}

	vector<string> getInputFieldsFromNodeName(string cname)
	{
		vector<string> mfields;
		for (int i = 0; i < size(); i++) {
			if (isNode(i)) {
				if (getNodeMode(i) == "Input") {
					if (cname == getID(i)) {
						if (getNodeIndexMode(i) == "Index") {
							int mindex = getNodeXIndex(i);
							if (mindex < 0) {
								char cindex[200];  sprintf_s(cindex, ":%i", -mindex);
								if (getNumDataFields(i) > 0) {
									vector<string> tfields = getDataFields(i);
									for (int k = 0; k < tfields.size(); k++) {
										mfields[k] = tfields[k] + cindex;
									}
								}
								else {
									mfields.push_back(getDataField(i) + cindex);
								}
							}
						}
					}
				}
			}
		}
		return(mfields);
	}

	vector<string> getInputFieldsFromLayerName(int index)
	{
		vector<string> mfields;
		if (isConvLayer(index)) {
			if ((getNodeMode(index) == "Input") && (getConvType(index) == DATA_CONV)) {
				if (getNumDataFields(index) > 0) {
					mfields = getDataFields(index);
				}
				else {
					mfields.push_back(getDataField(index));
				}
			}
		}
		return(mfields);
	}

	vector<string> getInputFieldsFromLayerName(string cname)
	{
		vector<string> mfields;
		for (int i = 0; i < (int)netData.size(); i++) {
			if (isConvLayer(i)) {
				if ((getNodeMode(i) == "Input") && (getConvType(i) == DATA_CONV)) {
					if (getID(i) == cname) {
						if (getNumDataFields(i) > 0) {
							vector<string> tfields = getDataFields(i);
							for (int i = 0; i < (int)tfields.size(); i++) {
								mfields.push_back(tfields[i]);
							}
						}
						else {
							mfields.push_back(getDataField(i));
						}
					}
				}
			}
		}
		return(mfields);
	}

	bool getConvFromConnectName(string cname)
	{
		if ((cname.substr(0, 9) == "ConvLayer") || (cname.substr(0, 5) == "Layer") || (cname.substr(0, 9) == "ImageData") ||
			(cname.substr(0, 9) == "Attention") || (cname.substr(0, 10) == "PatternNet") || (cname.substr(0, 7) == "Dropout")) {
			return(true);
		}
		return(false);
	}

	int getConvXFromConnectName(string cname)
	{
		int rval = -1;
		string name = cname.substr(0, 5);
		if ((cname.substr(0, 9) == "ConvLayer") || (cname.substr(0, 5) == "Layer") || (cname.substr(0, 9) == "ImageData") || (cname.substr(0, 9) == "Attention")) {
			int k = (int)name.length();
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			k++;
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			k++;
			int l = k;
			while ((l < (int)cname.length()) && (cname[l] != ':')) {
				l++;
			}
			if (k < (int)cname.length()) {
				string t3 = cname.substr(k, ((int64_t)l - k));
				//addErrorString("getConvXFromConnectName : " + cname + " : t3 = " + t3);
				rval = myStoi(t3);
#ifndef GCC
				if (rval != (int)_tstof((LPCTSTR)(CString)t3.c_str())) {
					addErrorString("integer conversion error 9");
				}
#endif
			}
		}
		return(rval);
	}

	int getConvYFromConnectName(string cname)
	{
		int rval = -1;
		string name = cname.substr(0, 5);
		if ((cname.substr(0, 9) == "ConvLayer") || (cname.substr(0, 5) == "Layer") || (cname.substr(0, 9) == "ImageData") || (cname.substr(0, 9) == "Attention")) {
			int k = (int)name.length();
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			k++;
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			k++;
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			k++;
			if (k < (int)cname.length()) {
				string t3 = cname.substr(k);
				//addErrorString("getConvYFromConnectName : " + cname + " : t3 = " + t3);
				rval = myStoi(t3);
#ifndef GCC
				if (rval != (int)_tstof((LPCTSTR)(CString)t3.c_str())) {
					addErrorString("integer conversion error 1");
				}
#endif
			}
		}
		return(rval);
	}

	int getConvPreXFromConnectName(string cname)
	{
		int rval = -1;
		string name = cname.substr(0, 5);
		if ((cname.substr(0, 9) == "ConvLayer") || (cname.substr(0, 5) == "Layer") || (cname.substr(0, 9) == "ImageData") || (cname.substr(0, 9) == "Attention")) {
			int k = (int)name.length();
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			k++;
			if (k < (int)cname.length()) {
				string t3 = cname.substr(k);
				//addErrorString("getConvPreXFromConnectName : " + cname + " : t3 = " + t3);
				rval = stoi(t3);
#ifndef GCC
				if (rval != (int)_tstof((LPCTSTR)(CString)t3.c_str())) {
					char s[100];  sprintf_s(s, " : rval = %i : tstof = %i", rval, (int)_tstof((LPCTSTR)(CString)t3.c_str()));
					addErrorString("integer conversion error 2 : t3 : " + cname + s);
				}
#endif
			}
		}
		return(rval);
	}

	char getConvColorFromConnectName(string cname)
	{
		char rval = ' ';
		string name = cname.substr(0, 5);
		if ((cname.substr(0, 9) == "ConvLayer") || (cname.substr(0, 5) == "Layer") || (cname.substr(0, 9) == "ImageData")) {
			int k = (int)name.length();
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			k++;
			while ((k < (int)cname.length()) && (cname[k] != ':')) {
				k++;
			}
			if (k <= (int)cname.length()) {
				rval = (char)cname[(int64_t)k - 1];
			}
		}
		return(rval);
	}

















	
	



	
	
	
	bool hasSingleColon(string cname)
	{
		string mname = cname;
		for (int i = 0; i < (int)mname.length(); i++) {
			if (mname[i] == ':') {
				if (i < ((int)mname.length() - 1)) {
					if (mname[(int64_t)i + 1] != ':') {
						return(true);
					}
				}
				else {
					return(true);
				}
			}
		}
		return(false);
	}

	string getStringBeforeColon(string cname)
	{
		string mname = cname;
		for (int i = 0; i < (int)mname.length(); i++) {
			if (mname[i] == ':') {
				return(mname.substr(0, i));
			}
		}
		return(cname);
	}

	string getStringAfterColon(string cname)
	{
		string mname = cname;
		for (int i = 0; i < (int)mname.length(); i++) {
			if (mname[i] == ':') {
				//addErrorString("getStringAfterColon : " + cname + " : " + mname.substr(i + 1));
				return(mname.substr((int64_t)i + 1));
			}
		}
		return(cname);
	}

	int getFirstNumberAfterColon(string cname)
	{
		int k = 0;
		string name = cname;
		while ((k < (int)cname.length()) && (cname[k] != ':')) {
			k++;
		}
		if (k <= (int)cname.length()) {
			string t3 = cname.substr((int64_t)k + 1);
			//addErrorString("getSecondNumberAfterColon : " + cname + " : " + t3);
			if (isWordNumber(t3)) {
				int rval = myStoi(t3);
#ifndef GCC
				if (rval != (int)_tstof((LPCTSTR)(CString)t3.c_str())) {
					addErrorString("integer conversion error 4");
				}
#endif
				return(rval);
			}
		}
		return(-1);
	}

	int getSecondNumberAfterColon(string cname)
	{
		int k = 0;
		string name = cname;
		while ((k < (int)cname.length()) && (cname[k] != ':')) {
			k++;
		}
		k++;
		while ((k < (int)cname.length()) && (cname[k] != ':')) {
			k++;
		}
		if (k <= (int)cname.length()) {
			string t3 = cname.substr((int64_t)k + 1);
			//addErrorString("getSecondNumberAfterColon : " + cname + " : " + t3);
			if (isWordNumber(t3)) {
				int rval = myStoi(t3);
#ifndef GCC
				if (rval != (int)_tstof((LPCTSTR)(CString)t3.c_str())) {
					addErrorString("integer conversion error 4");
				}
#endif
				return(rval);
			}
		}
		return(-1);
	}

	int myStoi(string s)
	{
		if (isWordNumber(s)) {
			return(stoi(s));
		}
		addErrorString(s);
		return(0);
	}

	bool isWordNumber(string word)
	{
		int ecnt = 0;
		bool leading_space = true;
		for (int i = 0; i < (int)word.length(); i++) {
			if (!isNumber((char)word[i]) && ((char)word[i] != 'e') && ((char)word[i] != 'E') && ((char)word[i] != ' ')) {
				return(false);
			}
			else if (((char)word[i] == 'e') || ((char)word[i] == 'E')) {
				ecnt++;
				leading_space = false;
			}
			else if ((char)word[i] == ' ') {
				if (!leading_space) {
					return(false);
				}
			}
			else {
				leading_space = false;
			}
		}
		if (ecnt < 2) {
			return(true);
		}
		return(false);
	}

	bool isNumber(char data)
	{
		if ((data >= 48) && (data <= 57)) {
			return(true);
		}
		else if ((data == 46) || (data == 43) || (data == 45)) {
			return(true);
		}
		return(false);
	}

	string sortNodesAll(vector<string> node_names, bool sort)
	{
		string rval = sortNodes(node_names, sort);
		if (rval.length() > 0) {
			rval = " : " + rval;
		}
		string tval = sortConvs(node_names, sort);
		if (tval.length() > 0) {
			rval += (" : " + tval);
		}
		return(rval);
	}

	string sortNodes(vector<string> node_names, bool sort)
	{
		string rval = "";

		vector<string> mnodes;
		vector<int> mnums;

		for (int i = 0; i < (int)node_names.size(); i++) {
			if (!getConvFromConnectName(node_names[i])) {
				mnodes.push_back(node_names[i]);
				mnums.push_back(getNumFromString(node_names[i]));
			}
		}

		if (sort) {
			for (int i = 0; i < ((int)mnodes.size() - 1); i++) {
				for (int j = i; j < (int)mnodes.size(); j++) {
					if (mnums[j] < mnums[i]) {
						string tnode = mnodes[i];
						int tnum = mnums[i];
						mnodes[i] = mnodes[j];
						mnums[i] = mnums[j];
						mnodes[j] = tnode;
						mnums[j] = tnum;
					}
				}
			}
		}

		int cnt = 0;
		int next = -1;
		string prev = "";
		vector<string> mnodes2;
		for (int i = 0; i < (int)mnodes.size(); i++) {
			if (mnums[i] != next) {
				if (cnt > 0) {
					mnodes2[(int)mnodes2.size() - 1] += "-";
					mnodes2[(int64_t)mnodes2.size() - 1] += prev;
				}
				mnodes2.push_back(mnodes[i]);
				cnt = 0;
			}
			else {
				cnt++;
			}
			prev = mnodes[i];
			next = mnums[i] + 1;
		}

		if (cnt > 0) {
			mnodes2[(int)mnodes2.size() - 1] += "-";
			mnodes2[(int64_t)mnodes2.size() - 1] += prev;
		}

		for (int i = 0; i < (int)mnodes2.size(); i++) {
			if (i > 0) {
				rval += ", ";
			}
			rval += mnodes2[i];
		}

		return(rval);
	}

	string sortConvs(vector<string> nconv_names, bool sort)
	{
		string rval = "";

		vector<string> mconvs;
		vector<int> mnums;
		vector<int> mnums2;

		vector<string> mconv_names = nconv_names;

		int stride = 0;
		for (int i = 0; i < (int)mconv_names.size(); i++) {
			if (getConvFromConnectName(mconv_names[i])) {
				int y = getConvYFromConnectName(mconv_names[i]);
				if (y > stride) {
					stride = y;
				}
			}
		}
		stride++;

		for (int i = 0; i < (int)mconv_names.size(); i++) {
			if (getConvFromConnectName(mconv_names[i])) {
				mconvs.push_back(mconv_names[i]);
				mnums.push_back(getNumFromString(mconv_names[i]));
				char clr = getConvColorFromConnectName(mconv_names[i]);
				int x = getConvXFromConnectName(mconv_names[i]);
				int y = getConvYFromConnectName(mconv_names[i]);
				int tnum2 = y + (x * stride);
				if (clr == 'g') {
					tnum2 += 100;
				}
				else if (clr == 'b') {
					tnum2 += 10000;
				}
				mnums2.push_back(tnum2);
			}
		}

		if (sort) {
			for (int i = 0; i < ((int)mconvs.size() - 1); i++) {
				for (int j = i; j < (int)mconvs.size(); j++) {
					if (getConvNameFromConnectName(mconvs[i]) == getConvNameFromConnectName(mconvs[j])) {
						if (mnums[j] < mnums[i]) {
							string tnode = mconvs[i];
							int tnum = mnums[i];
							int tnum2 = mnums2[i];
							mconvs[i] = mconvs[j];
							mnums[i] = mnums[j];
							mnums2[i] = mnums2[j];
							mconvs[j] = tnode;
							mnums[j] = tnum;
							mnums2[j] = tnum2;
						}
					}
				}
			}
			for (int i = 0; i < ((int)mconvs.size() - 1); i++) {
				for (int j = i; j < (int)mconvs.size(); j++) {
					if (getConvNameFromConnectName(mconvs[i]) == getConvNameFromConnectName(mconvs[j])) {
						if (mnums2[j] < mnums2[i]) {
							string tnode = mconvs[i];
							int tnum = mnums[i];
							int tnum2 = mnums2[i];
							mconvs[i] = mconvs[j];
							mnums[i] = mnums[j];
							mnums2[i] = mnums2[j];
							mconvs[j] = tnode;
							mnums[j] = tnum;
							mnums2[j] = tnum2;
						}
					}
				}
			}
		}

		int cnt = 0;
		int next2 = -1;
		string prev = "";
		vector<string> mconvs2;
		for (int i = 0; i < (int)mconvs.size(); i++) {
			if ((mnums2[i] != next2)) {
				if (cnt > 1) {
					mconvs2[(int)mconvs2.size() - 1] += "-";
					mconvs2[(int)mconvs2.size() - 1] += prev;
				}
				mconvs2.push_back(mconvs[i]);
				cnt = 0;
			}
			else {
				cnt++;
			}
			prev = mconvs[i];
			next2 = mnums2[i] + 1;
		}

		if (cnt > 1) {
			mconvs2[(int)mconvs2.size() - 1] += "-";
			mconvs2[(int)mconvs2.size() - 1] += prev;
		}

		for (int i = 0; i < (int)mconvs2.size(); i++) {
			if (i > 0) {
				rval += ", ";
			}
			rval += mconvs2[i];
		}

		return(rval);
	}

	int getNumFromString(string mstring)
	{
		int rval = 0;
		int i = 0;
		//addErrorString("mstring = " + mstring);
		while ((i < (int)mstring.length()) && !isNumber(mstring[i])) {
			i++;
		}
		if (i < (int)mstring.length()) {
			string num = mstring.substr(i);
			//char s[100]; sprintf_s(s, " : %i : %i", i, (int)mstring.length());  addErrorString("getNumFromString : " + mstring + " : num = " + num + s);
			rval = myStoi(num);
#ifndef GCC
			if (rval != (int)_tstof((LPCTSTR)(CString)num.c_str())) {
				addErrorString("integer conversion error 3");
			}
#endif
		}
		return(rval);
	}

	vector<string> getFollowingNodes(string id, bool nodes_only = false)
	{
		vector<string> rval;
		int sindex = getIndexFromID(id);
		if ((sindex >= 0) && (sindex < (int)netData.size())) {
			vector<int> following = getFollowingNodes(sindex, nodes_only);
			for (int i = 0; i < (int)following.size(); i++) {
				rval.push_back(getID(following[i]));
			}
		}
		return(rval);
	}

	vector<int> getFollowingNodes(int sindex, bool nodes_only = false)
	{
		vector<int> followingNodes;
		string fname = getID(sindex);
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname = getInputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((fname == nname2) || (fname == nname)) {
					string nname10 = getOutputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex1 = getIndexFromID(nname12);
					int cindex2 = getIndexFromID(nname10);
					if (isConvLayer(cindex1)) {
						if (!nodes_only) {
							bool found = false;
							for (int k = 0; k < (int)followingNodes.size(); k++) {
								if (cindex1 == followingNodes[k]) {
									found = true;
								}
							}
							if (!found) {
								followingNodes.push_back(cindex1);
							}
						}
					}
					else if (isNode(cindex2)) {
						bool found = false;
						for (int k = 0; k < (int)followingNodes.size(); k++) {
							if (followingNodes[k] == cindex2) {
								found = true;
							}
						}
						if (!found) {
							followingNodes.push_back(cindex2);
						}
					}
				}
			}
		}
		return(followingNodes);
	}

	vector<string> getFollowingConvNets(string id)
	{
		vector<string> rval;
		int sindex = getIndexFromID(id);
		if ((sindex >= 0) && (sindex < (int)netData.size())) {
			vector<int> following = getFollowingConvNets(sindex);
			for (int i = 0; i < (int)following.size(); i++) {
				rval.push_back(getID(following[i]));
			}
		}
		return(rval);
	}

	vector<int> getFollowingConvNets(int sindex)
	{
		vector<int> followingConvNets;

		if (!isConvLayer(sindex) && !isAttentionLayer(sindex) && !isNetCluster(sindex) && !isDropout(sindex) || isAssemblyLayer(sindex)) {
			return(followingConvNets);
		}

		string nconvname = getID(sindex);

		vector<vector<int>> nodeconnections;

		vector<int> nodeset0;
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname = getInputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((nconvname == nname2) || (nconvname == nname)) {
					string nname10 = getOutputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex1 = getIndexFromID(nname12);
					int cindex2 = getIndexFromID(nname10);
					if (isConvLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)followingConvNets.size(); k++) {
							if (cindex1 == followingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							followingConvNets.push_back(cindex1);
						}
					}
					else if (isAttentionLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)followingConvNets.size(); k++) {
							if (cindex1 == followingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							followingConvNets.push_back(cindex1);
						}
					}
					else if (isNetCluster(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)followingConvNets.size(); k++) {
							if (cindex1 == followingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							followingConvNets.push_back(cindex1);
						}
					}
					else if (isDropout(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)followingConvNets.size(); k++) {
							if (cindex1 == followingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							followingConvNets.push_back(cindex1);
						}
					}
					else if (isNode(cindex2)) {
						if (getNodeMode(cindex2) == "Hidden") {
							bool found = false;
							for (int k = 0; k < (int)nodeset0.size(); k++) {
								if (nodeset0[k] == cindex2) {
									found = true;
								}
							}
							if (!found) {
								nodeset0.push_back(cindex2);
							}
						}
					}
				}
			}
		}
		nodeconnections.push_back(nodeset0);

		bool done = false;
		while (!done) {
			vector<int> nodeset1;
			for (int i = 0; i < size(); i++) {
				if (isConnect(i)) {
					for (int j = 0; j < (int)nodeset0.size(); j++) {
						if (getID(nodeset0[j]) == getInputNode(i)) {
							string nname = getOutputNode(i);
							string nname2 = getConvNameFromConnectName(nname);
							int cindex1 = getIndexFromID(nname2);
							int cindex2 = getIndexFromID(nname);
							if (isConvLayer(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)followingConvNets.size(); k++) {
									if (cindex1 == followingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									followingConvNets.push_back(cindex1);
								}
							}
							if (isAttentionLayer(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)followingConvNets.size(); k++) {
									if (cindex1 == followingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									followingConvNets.push_back(cindex1);
								}
							}
							if (isNetCluster(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)followingConvNets.size(); k++) {
									if (cindex1 == followingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									followingConvNets.push_back(cindex1);
								}
							}
							if (isDropout(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)followingConvNets.size(); k++) {
									if (cindex1 == followingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									followingConvNets.push_back(cindex1);
								}
							}
							else if (isNode(cindex2)) {
								if (getNodeMode(cindex2) == "Hidden") {
									bool found = false;
									for (int k = 0; k < (int)nodeset1.size(); k++) {
										if (nodeset1[k] == cindex2) {
											found = true;
										}
									}
									if (!found) {
										nodeset1.push_back(cindex2);
									}
								}
							}
						}
					}
				}
			}
			if (nodeset1.size() == 0) {
				done = true;
			}
			else {
				nodeconnections.push_back(nodeset1);
				nodeset0 = nodeset1;
			}
		}

		return(followingConvNets);

	}

	vector<twoStrings> getImmediateSupportingConnections(int sindex)
	{
		vector<twoStrings> supportingConnections;
		string fname = getID(sindex);
		for (int i = 0; i < (int)netData.size(); i++) {
			if (isConnect(i)) {
				string nname = getOutputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((fname == nname2) || (fname == nname)) {
					string nname10 = getInputNode(i);
					bool found = false;
					for (int k = 0; k < (int)supportingConnections.size(); k++) {
						if ((supportingConnections[k].s1 == nname10) && (supportingConnections[k].s2 == nname)) {
							found = true;
						}
					}
					if (!found) {
						twoStrings inputoutput;
						inputoutput.s1 = nname10;
						inputoutput.s2 = nname;
						supportingConnections.push_back(inputoutput);
					}
				}
			}
		}
		return(supportingConnections);
	}

	vector<string> getImmediateSupportingNodes(int sindex)
	{
		vector<string> supportingNodes;
		string fname = getID(sindex);
		for (int i = 0; i < (int)netData.size(); i++) {
			if (isConnect(i)) {
				string nname = getOutputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				//addErrorString("getImmediateSupportingNodes : comparing " + nname + " against " + fname);
				if ((fname == nname2) || (fname == nname)) {
					string nname10 = getInputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex2 = getIndexFromID(nname12);
					if (isNode(cindex2)) {
						//addErrorString("getImmediateSupportingNodes : " + nname + " : " + nname2 + " : " + nname10 + " : " + nname12);
						bool found = false;
						for (int k = 0; k < (int)supportingNodes.size(); k++) {
							if (supportingNodes[k] == nname10) {
								found = true;
							}
						}
						if (!found) {
							supportingNodes.push_back(nname10);
						}
					}
				}
			}
		}
		return(supportingNodes);
	}

	vector<int> getImmediateSupportingNodesReturnIndices(int sindex)
	{
		vector<int> supportingNodes;
		string fname = getID(sindex);
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname = getOutputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((fname == nname2) || (fname == nname)) {
					string nname10 = getInputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex2 = getIndexFromID(nname12);
					if (isNode(cindex2)) {
						bool found = false;
						for (int k = 0; k < (int)supportingNodes.size(); k++) {
							if (supportingNodes[k] == cindex2) {
								found = true;
							}
						}
						if (!found) {
							//CString s = _T(""); s.Format(_T(" : cindex2 = %i"), cindex2);
							//AfxMessageBox(_T("tp20 : ") + (CString)fname.c_str() + _T(" : ") + (CString)nname.c_str() + _T(" : ") + (CString)nname2.c_str() + _T(" : ") + (CString)nname10.c_str() + _T(" : ") + (CString)nname12.c_str() + s);
							supportingNodes.push_back(cindex2);
						}
					}
				}
			}
		}
		return(supportingNodes);
	}

	vector<string> getImmediateSupportingLayers(int sindex)
	{
		vector<string> supportingConvNets;
		string nconvname = getID(sindex);
		for (int i = 0; i < (int)size(); i++) {
			if (isConnect(i)) {
				string nname = getOutputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((nconvname == nname2) || (nconvname == nname)) {
					string nname10 = getInputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex1 = getIndexFromID(nname12);
					if (isConvLayer(cindex1) || isAttentionLayer(cindex1) || isNetCluster(cindex1) || isDropout(cindex1) || isAssemblyLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)supportingConvNets.size(); k++) {
							if (nname12 == supportingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							supportingConvNets.push_back(nname12);
						}
					}
				}
			}
		}
		return(supportingConvNets);
	}

	vector<string> getImmediateFollowingNodes(int sindex)
	{
		vector<string> followingNodes;
		string fname = getID(sindex);
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname = getInputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((fname == nname2) || (fname == nname)) {
					string nname10 = getOutputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex2 = getIndexFromID(nname12);
					if (isNode(cindex2)) {
						bool found = false;
						for (int k = 0; k < (int)followingNodes.size(); k++) {
							if (followingNodes[k] == nname10) {
								found = true;
							}
						}
						if (!found) {
							followingNodes.push_back(nname10);
						}
					}
				}
			}
		}
		return(followingNodes);
	}

	vector<string> getImmediateSupportingConvNets(string id, bool from_any = false)
	{
		vector<string> rval;
		int sindex = getIndexFromID(id);
		if ((sindex >= 0) && (sindex < (int)netData.size())) {
			//addErrorString("getImmediateSupportingConvNets : tp1");
			vector<int> following = getImmediateSupportingConvNetsReturnIndices(sindex, from_any);
			for (int i = 0; i < (int)following.size(); i++) {
				rval.push_back(getID(following[i]));
			}
		}
		return(rval);
	}

	vector<string> getImmediateSupportingConvNets(int sindex)
	{
		vector<string> supportingConvNets;
		string nconvname = getID(sindex);
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname = getOutputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((nconvname == nname2) || (nconvname == nname)) {
					string nname10 = getInputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex1 = getIndexFromID(nname12);
					if (isConvLayer(cindex1) || isAttentionLayer(cindex1) || isNetCluster(cindex1) || isDropout(cindex1) || isAssemblyLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)supportingConvNets.size(); k++) {
							if (nname12 == supportingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							supportingConvNets.push_back(nname12);
						}
					}
				}
			}
		}
		return(supportingConvNets);
	}

	vector<int> getImmediateSupportingConvNetsReturnIndices(int sindex, bool from_any = false)
	{
		vector<int> supportingConvNets;
		if (!from_any) {
			if (!(isConvLayer(sindex) || isAttentionLayer(sindex) || isNetCluster(sindex) || isDropout(sindex) || isAssemblyLayer(sindex))) {
				return(supportingConvNets);
			}
		}
		string nconvname = getID(sindex);
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname = getOutputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((nconvname == nname2) || (nconvname == nname)) {
					string nname10 = getInputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex1 = getIndexFromID(nname12);
					if (isConvLayer(cindex1) || isAttentionLayer(cindex1) || isNetCluster(cindex1) || isDropout(cindex1) || isAssemblyLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)supportingConvNets.size(); k++) {
							if (cindex1 == supportingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							supportingConvNets.push_back(cindex1);
						}
					}
				}
			}
		}
		return(supportingConvNets);
	}

	vector<string> getImmediateFollowingConvNets(string id)
	{
		vector<string> rval;
		int sindex = getIndexFromID(id);
		if ((sindex >= 0) && (sindex < (int)netData.size())) {
			vector<int> following = getImmediateFollowingConvNets(sindex);
			for (int i = 0; i < (int)following.size(); i++) {
				rval.push_back(getID(following[i]));
			}
		}
		return(rval);
	}

	vector<int> getImmediateFollowingConvNets(int sindex)
	{
		vector<int> followingNodes;
		string fname = getID(sindex);
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname2 = getConvNameFromConnectName(getInputNode(i));
				if (fname == nname2) {
					int cindex1 = getIndexFromID(getConvNameFromConnectName(getOutputNode(i)));
					if (isConvLayer(cindex1) || isAttentionLayer(cindex1) || isNetCluster(cindex1) || isDropout(cindex1) || isAssemblyLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)followingNodes.size(); k++) {
							if (cindex1 == followingNodes[k]) {
								found = true;
							}
						}
						if (!found) {
							followingNodes.push_back(cindex1);
						}
					}
				}
			}
		}
		return(followingNodes);
	}

	vector<string> getSupportingConvNets(string id)
	{
		vector<string> rval;
		int sindex = getIndexFromID(id);
		if ((sindex >= 0) && (sindex < (int)netData.size())) {
			vector<int> following = getSupportingConvNets(sindex);
			for (int i = 0; i < (int)following.size(); i++) {
				rval.push_back(getID(following[i]));
			}
		}
		return(rval);
	}

	vector<int> getSupportingConvNetsRecursive(string id) 
	{
		vector<int> rval;
		vector<int> rsupporting = getSupportingRecursive(id);
		if (rsupporting.size() > 0) {
			for (int i = 0; i < (int)rsupporting.size(); i++) {
				if (isConvLayer(rsupporting[i]) || isAttentionLayer(rsupporting[i]) || isNetCluster(rsupporting[i]) || isDropout(rsupporting[i]) || isAssemblyLayer(rsupporting[i])) {
					rval.push_back(rsupporting[i]);
				}
			}
		}
		return(rval);
	}

	vector<int> getSupportingRecursive(string id)
	{
		vector<int> rval;
		int sindex = getIndexFromID(id);
		if ((sindex >= 0) && (sindex < (int)netData.size())) {
			vector<int> supporting = getImmediateSupportingConvNetsReturnIndices(sindex, true);
			vector<int> snodes = getImmediateSupportingNodesReturnIndices(sindex);
			rval = supporting;
			if (snodes.size() > 0) {
				for (int i = 0; i < (int)snodes.size(); i++) {
					if (supporting.size() > 0) {
						bool found = false;
						for (int j = 0; j < (int)supporting.size(); j++) {
							if (supporting[j] == snodes[i]) {
								j = (int)supporting.size();
								found = true;
							}
						}
						if (!found) {
							supporting.push_back(snodes[i]);
							rval.push_back(snodes[i]);
						}
					}
					else {
						supporting.push_back(snodes[i]);
						rval.push_back(snodes[i]);
					}
				}
			}
			//char s[100]; sprintf_s(s, "getSupportingRecursive : supporting size = %i for ", (int)supporting.size());  addErrorString(s + id);
			if (supporting.size() > 0) {
				for (int i = 0; i < (int)supporting.size(); i++) {
					vector<int> rsupporting = getSupportingRecursive(getID(supporting[i]));
					//char s[100]; sprintf_s(s, "getSupportingRecursive : rsupporting size = %i for ", (int)rsupporting.size());  addErrorString(s + getID(supporting[i]));
					if (rsupporting.size() > 0) {
						for (int j = 0; j < (int)rsupporting.size(); j++) {
							//AfxMessageBox(_T("tp4 : ") + (CString)getID(supporting[i]).c_str() + _T(" : ") + (CString)getID(rsupporting[j]).c_str());
							if (rval.size() > 0) {
								bool found = false;
								for (int k = 0; k < (int)rval.size(); k++) {
									if (rsupporting[j] == rval[k]) {
										k = (int)rval.size();
										found = true;
									}
								}
								if (!found) {
									rval.push_back(rsupporting[j]);
								}
							}
							else {
								rval.push_back(rsupporting[j]);
							}
							//AfxMessageBox(_T("tp5"));
						}
					}
				}
			}
		}
		return(rval);
	}

	vector<int> getSupportingConvNets(int sindex)
	{
		vector<int> supportingConvNets;

		if (!(isConvLayer(sindex) || isAttentionLayer(sindex) || isNetCluster(sindex) || isDropout(sindex) || isAssemblyLayer(sindex))) {
			return(supportingConvNets);
		}

		string nconvname = getID(sindex);

		vector<vector<int>> nodeconnections;

		vector<int> nodeset0;
		for (int i = 0; i < size(); i++) {
			if (isConnect(i)) {
				string nname = getOutputNode(i);
				string nname2 = getConvNameFromConnectName(nname);
				if ((nconvname == nname2) || (nconvname == nname)) {
					string nname10 = getInputNode(i);
					string nname12 = getConvNameFromConnectName(nname10);
					int cindex1 = getIndexFromID(nname12);
					int cindex2 = getIndexFromID(nname10);
					if (isConvLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)supportingConvNets.size(); k++) {
							if (cindex1 == supportingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							supportingConvNets.push_back(cindex1);
						}
					}
					else if (isAttentionLayer(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)supportingConvNets.size(); k++) {
							if (cindex1 == supportingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							supportingConvNets.push_back(cindex1);
						}
					}
					else if (isNetCluster(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)supportingConvNets.size(); k++) {
							if (cindex1 == supportingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							supportingConvNets.push_back(cindex1);
						}
					}
					else if (isDropout(cindex1)) {
						bool found = false;
						for (int k = 0; k < (int)supportingConvNets.size(); k++) {
							if (cindex1 == supportingConvNets[k]) {
								found = true;
							}
						}
						if (!found) {
							supportingConvNets.push_back(cindex1);
						}
					}
					else if (isNode(cindex2)) {
						if ((getNodeMode(cindex2) == "Hidden") || (getNodeMode(cindex2) == "PreProcess")) {
							bool found = false;
							for (int k = 0; k < (int)nodeset0.size(); k++) {
								if (nodeset0[k] == cindex2) {
									found = true;
								}
							}
							if (!found) {
								nodeset0.push_back(cindex2);
							}
						}
					}
				}
			}
		}
		nodeconnections.push_back(nodeset0);

		bool done = false;
		while (!done) {
			vector<int> nodeset1;
			for (int i = 0; i < (int)size(); i++) {
				if (isConnect(i)) {
					for (int j = 0; j < (int)nodeset0.size(); j++) {
						if (getID(nodeset0[j]) == getOutputNode(i)) {
							string nname = getInputNode(i);
							string nname2 = getConvNameFromConnectName(nname);
							int cindex1 = getIndexFromID(nname2);
							int cindex2 = getIndexFromID(nname);
							if (isConvLayer(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)supportingConvNets.size(); k++) {
									if (cindex1 == supportingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									supportingConvNets.push_back(cindex1);
								}
							}
							else if (isAttentionLayer(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)supportingConvNets.size(); k++) {
									if (cindex1 == supportingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									supportingConvNets.push_back(cindex1);
								}
							}
							else if (isNetCluster(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)supportingConvNets.size(); k++) {
									if (cindex1 == supportingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									supportingConvNets.push_back(cindex1);
								}
							}
							else if (isDropout(cindex1)) {
								bool found = false;
								for (int k = 0; k < (int)supportingConvNets.size(); k++) {
									if (cindex1 == supportingConvNets[k]) {
										found = true;
									}
								}
								if (!found) {
									supportingConvNets.push_back(cindex1);
								}
							}
							else if (isNode(cindex2)) {
								if ((getNodeMode(cindex2) == "Hidden") || (getNodeMode(cindex2) == "PreProcess")) {
									bool found = false;
									for (int k = 0; k < (int)nodeset1.size(); k++) {
										if (nodeset1[k] == cindex2) {
											found = true;
										}
									}
									if (!found) {
										nodeset1.push_back(cindex2);
									}
								}
							}
						}
					}
				}
			}
			if (nodeset1.size() == 0) {
				done = true;
			}
			else {
				nodeconnections.push_back(nodeset1);
				nodeset0 = nodeset1;
			}
		}

		return(supportingConvNets);

	}

	twoValues getInputImageSize()
	{
		twoValues rval;
		int image_width = 0;
		int image_height = 0;
		for (int i = 0; i < size(); i++) {
			if (isDataSource(i)) {
				image_width = getDataImageWidth(i);
				image_height = getDataImageHeight(i);
				i = size();
			}
		}
		if ((image_width <= 0) || (image_height <= 0)) {
			addErrorString("getInputImageSize : image_width or image_height equal to zero");
		}
		rval.x = image_width;
		rval.y = image_height;
		return(rval);
	}

	bool getImageGray()
	{
		bool rval = false;
		for (int i = 0; i < size(); i++) {
			if (isDataSource(i)) {
				rval = getDataImageGray(i);
				i = size();
			}
		}
		return(rval);
	}

	int getSoundConvDataProcessType()
	{
		int rval = NONE;
		if (size() > 0) {
			for (int i = 0; i < size(); i++) {
				if (isConvLayer(i)) {
					if (getConvType(i) == SOUND_CONV) {
						if (getNodeMode(i) == "Input") {
							vector<string> fnodes1;
							vector<string> fnodes2 = getFollowingNodes(getConvNameFromConnectName(getID(i)));
							if (fnodes2.size() > 0) {
								for (int j = 0; j < (int)fnodes2.size(); j++) {
									if (!isConvLayer(getIndexFromID(getConvNameFromConnectName(fnodes2[j])))) {
										fnodes1.push_back(fnodes2[j]);
									}
								}
							}
							if (fnodes1.size() > 0) {
								for (int j = 0; j < (int)fnodes1.size(); j++) {
									if (getNodeMode(getIndexFromID(fnodes1[j])) == "PreProcess") {
										string processor = getNodePreProcessor(getIndexFromID(fnodes1[j]));
										if (processor.substr(0, 3) == "FFT") {
											rval = FFT_FOLLOWING;
											j = (int)fnodes1.size();
											i = (int)size();
										}
										if (rval == NONE) {
											vector<string> fconvs = getFollowingConvNets(fnodes1[j]);
											if (fconvs.size() > 0) {
												for (int k = 0; k < (int)fconvs.size(); k++) {
													if (getConvType(getIndexFromID(fconvs[k])) == DATA_CONV) {
														//AfxMessageBox(_T("*** conv following fft following"));
														addErrorString("*** conv following fft following");
														rval = CONV_FOLLOWING_FFT_FOLLOWING;
														k = (int)fconvs.size();
														j = (int)fnodes1.size();
														i = (int)size();
													}
												}
											}
										}
									}
								}
								if (rval == NONE) {
									rval = FULLY_CONNECTED_FOLLOWING;
								}
								i = (int)size();
							}
							else {
								vector<string> fconvs = getFollowingConvNets(getConvNameFromConnectName(getID(i)));
								if (fconvs.size() > 0) {
									for (int k = 0; k < (int)fconvs.size(); k++) {
										if (getConvType(getIndexFromID(fconvs[k])) == DATA_CONV) {
											rval = CONV_FOLLOWING;
											k = (int)fconvs.size();
											i = (int)size();
										}
										else if (getConvType(getIndexFromID(fconvs[k])) == HIDDEN_NET) {
											rval = HIDDEN_NET_FOLLOWING;
											k = (int)fconvs.size();
											i = (int)size();
										}
										else if (isNetCluster(getIndexFromID(fconvs[k]))) {
											rval = HIDDEN_NET_FOLLOWING;
											k = (int)fconvs.size();
											i = (int)size();
										}
										else if (isDropout(getIndexFromID(fconvs[k]))) {
											rval = HIDDEN_NET_FOLLOWING;
											k = (int)fconvs.size();
											i = (int)size();
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return(rval);
	}

	int getOutputImageSize()
	{
		int imagewidth = 0;
		int imageheight = 0;
		for (int i = 0; i < size(); i++) {
			if (isDataSource(i)) {
				imagewidth = getDataImageWidth(i);
				imageheight = getDataImageHeight(i);
			}
		}
		int color = 3;
		if (getImageGray()) {
			color = 1;
		}
		return(color * imagewidth * imageheight);
	}

	int getDataConvOutputSize(int cindex)
	{
		int outsize = 0;
		int mxin = getConvInputX(cindex);
		int dimyin = getConvInputY(cindex);
		int dimyout = getConvOutputY(cindex);
		int stridexin = getConvOutputStrideX(cindex);
		int strideyin = getConvOutputStrideY(cindex);
		if (stridexin > 0) {
			outsize = dimyout * (1 + (strideyin - dimyin) / stridexin);
		}
		if (outsize < 0) {
			outsize = 0;
		}
		outsize = mxin * outsize;
		return(outsize);
	}

	int getConvInputX(int sindex)
	{
		vector<int> dimensions = getConvDimensions(sindex);
		if (getConvType(sindex) == DATA_CONV) {
			return(getInputWidthFromLayerName(getID(sindex)));
		}
		else {
			if (dimensions.size() > 0) {
				return(dimensions[0]);
			}
		}
		return(0);
	}

	int getConvInputY(int sindex)
	{
		vector<int> dimensions = getConvDimensions(sindex);
		if (dimensions.size() > 1) {
			return(dimensions[1]);
		}
		return(0);
	}

	int getConvOutputX(int sindex)
	{
		vector<int> dimensions = getConvDimensions(sindex);
		if (dimensions.size() > 2) {
			//char s[200]; sprintf_s(s, " : %i", dimensions[2]);	addErrorString("getConvOutputX for " + getID(sindex) + s);
			return(dimensions[2]);
		}
		//addErrorString("getConvOutputX for " + getID(sindex) + " : returning zero");
		return(0);
	}

	int getConvOutputY(int sindex)
	{
		vector<int> dimensions = getConvDimensions(sindex);
		if (dimensions.size() > 3) {
			//char s[200]; sprintf_s(s, " : %i", dimensions[3]);	addErrorString("getConvOutputY for " + getID(sindex) + s);
			return(dimensions[3]);
		}
		//addErrorString("getConvOutputY for " + getID(sindex) + " : returning zero");
		return(0);
	}

	int getConvOutputStrideX(int sindex)
	{
		vector<int> strides = getConvStrides(sindex);
		if (strides.size() > 0) {
			return(strides[0]);
		}
		return(0);
	}

	int getConvOutputStrideY(int sindex)
	{
		vector<int> strides = getConvStrides(sindex);
		if (strides.size() > 1) {
			//char s[200];  sprintf_s(s, "getConvOutputStrideY = %i : ", strides[1]); addErrorString(s + getID(sindex));
			return(strides[1]);
		}
		return(0);
	}

	int getInputWidthFromLayerName(string cname)
	{
		int supporting_count = 0;
		int cindex = getIndexFromID(cname);
		if ((cindex >= 0) && (cindex < (int)netData.size())) {
			vector<string> supporting = getImmediateSupportingNodes(cindex);
			if (supporting.size() > 0) {
				for (int k = 0; k < supporting.size(); k++) {
					int sindex = getIndexFromID(supporting[k]);
					string process = getNodePreProcessor(sindex);
					if (isNode(sindex) && (getNodeMode(sindex) == "PreProcess") && (process.substr(0, 3) == "FFT")) {
						//addErrorString("getInputWidthFromLayerName : " + process + " : substr = " + process.substr(3));
						int size = myStoi(process.substr(3));
#ifndef GCC
						if (size != (int)_tstof((LPCTSTR)(CString)process.substr(3).c_str())) {
							addErrorString("integer conversion error 5");
						}
#endif
						//char p[200]; sprintf_s(p, "%i", size);  addErrorString("getInputWidthFromLayerName : " + process + " : " + process.substr(3) + " : " + p);
						supporting_count += size;
					}
					else if (isNode(sindex) && (getNodeMode(sindex) == "PreProcess")) {
						int nfields = (int)getInputFieldsFromNodeName(supporting[k]).size();
						if (nfields > 0) {
							supporting_count += nfields;
						}
						else {
							vector<string> supporting3 = getSupportingConvNets(cname);
							if (supporting3.size() > 0) {
								nfields = (int)getInputFieldsFromLayerName(supporting3[0]).size();
								if (nfields > 0) {
									supporting_count += nfields;
								}
								else {
									supporting_count++;
								}
							}
							else {
								supporting_count++;
							}
						}
					}
					else {
						supporting_count++;
					}
				}
			}
			else {
				vector<string> supporting2 = getImmediateSupportingConvNets(cname);
				if (supporting2.size() == 1) {
					int cindex = getIndexFromID(supporting2[0]);
					if (getNodeMode(cindex) == "Input") {
						if (getConvType(cindex) == SOUND_CONV) {
							supporting_count = 1;
						}
						else {
							supporting_count = getConvInputX(cindex);
						}
					}
					else {
						supporting_count = getInputWidthFromLayerName(supporting2[0]);
					}
				}
				else if (supporting2.size() > 1) {
					//addErrorString(_T("Error 6783412"));
					for (int i = 0; i < supporting2.size(); i++) {
						supporting_count += getInputWidthFromLayerName(supporting2[i]);
					}
				}
				else {
					//addErrorString(_T("Error 6783413"));
				}
			}
		}
		return(supporting_count);
	}

	imageSizeData getImageSizeData(int cindex)
	{
		imageSizeData rval;
		rval.input_width = 0;
		rval.input_height = 0;
		rval.output_width = 0;
		rval.output_height = 0;
		rval.num_width_output_steps = 0;
		rval.num_height_output_steps = 0;
		if ((cindex >= 0) && (cindex < (int)netData.size())) {
			rval = getImageSizeData2(cindex);
			if ((rval.output_width == 0) || (rval.output_height == 0)) {
				rval = getImageSizeDataFromOutputImage(cindex);
				if ((rval.output_width == 0) || (rval.output_height == 0)) {
					rval = getInputImageSizeWithNoSupportingImage(cindex);
				}
			}
		}
		return(rval);
	}

	imageSizeData getImageSizeData2(int sindex)
	{
		imageSizeData rval;
		rval.input_width = 0;
		rval.input_height = 0;
		rval.output_width = 0;
		rval.output_height = 0;
		rval.num_width_output_steps = 0;
		rval.num_height_output_steps = 0;

		if (isConvLayer(sindex)) {

			if (getNodeMode(sindex) == "Input") {

				rval.input_width = 0;
				rval.input_height = 0;
				for (int i = 0; i < size(); i++) {
					if (isDataSource(i)) {
						rval.input_width = getDataImageWidth(i);
						rval.input_height = getDataImageHeight(i);
						i = size();
					}
				}
				if ((rval.input_width == 0) || (rval.input_height == 0)) {
					addErrorString("getImageSizeData2 : image height or width = 0");
				}

				rval.num_width_output_steps = 1;		// num out steps wide
				rval.output_width = rval.input_width;	// out width
				rval.num_height_output_steps = 1;		// num steps high
				rval.output_height = rval.input_height;	// out height

			}
			else {

				int mxin = getConvInputX(sindex);
				int myin = getConvInputY(sindex);
				int mxout = getConvOutputX(sindex);
				int myout = getConvOutputY(sindex);
				int stridex = getConvOutputStrideX(sindex);
				int stridey = getConvOutputStrideY(sindex);

				vector<int> sn0 = getImmediateSupportingConvNetsReturnIndices(sindex);

				bool dropout = false;
				bool hidden_net = false;
				if (sn0.size() > 0) {
					for (int i = 0; i < (int)sn0.size(); i++) {
						if (getConvType(getID(sn0[i])) == HIDDEN_NET) {
							i = (int)sn0.size();
							hidden_net = true;
						}
						if (getConvType(getID(sn0[i])) == DROPOUT) {
							i = (int)sn0.size();
							dropout = true;
						}
					}
				}

				if ((sn0.size() > 0) && !hidden_net && !dropout) {

					imageSizeData tval1 = getImageSizeData(sn0[0]);

					for (int i = 1; i < (int)sn0.size(); i++) {
						imageSizeData tval2 = getImageSizeData(sn0[i]);
						if ((tval1.input_width != tval2.input_width) || (tval1.input_height != tval2.input_height) ||
							(tval1.output_width != tval2.output_height) || (tval1.output_height != tval2.output_height) ||
							(tval1.num_width_output_steps != tval2.num_width_output_steps) || (tval1.num_height_output_steps != tval2.num_height_output_steps)) {
							addErrorString("Error : getImageSizeData found different sizes for " + getID(sn0[0]) + " and " + getID(sn0[i]));
						}
					}

					rval.input_width = tval1.output_width;
					if (stridex > 0) {
						rval.num_width_output_steps = rval.input_width / stridex;		// num out steps wide
						if (stridex < mxin) {
							rval.num_width_output_steps = (rval.input_width - mxin) / stridex;
						}
					}
					rval.output_width = rval.num_width_output_steps * mxout;

					rval.input_height = tval1.output_height;
					if (stridey > 0) {
						rval.num_height_output_steps = rval.input_height / stridey;		// num out steps high
						if (stridey < myin) {
							rval.num_height_output_steps = (rval.input_height - myin) / stridey;
						}
					}
					rval.output_height = rval.num_height_output_steps * myout;

				}
				else if (((int)sn0.size() == 1) && dropout) {

					vector<int> sn1 = getImmediateSupportingConvNetsReturnIndices(sn0[0]);
					if ((int)sn1.size() == 1) {
						if (getConvType(getID(sn1[0])) == IMAGE_CONV) {
							return(getImageSizeData2(sn1[0]));
						}
					}

				}

			}

		}

		return(rval);
	}

	imageSizeData getImageSizeDataFromOutputImage(int sindex)
	{
		imageSizeData rval;
		rval.input_width = 0;
		rval.input_height = 0;
		rval.output_width = 0;
		rval.output_height = 0;
		rval.num_width_output_steps = 0;
		rval.num_height_output_steps = 0;

		if (isConvLayer(sindex)) {

			int mxin = getConvInputX(sindex);
			int myin = getConvInputY(sindex);
			int mxout = getConvOutputX(sindex);
			int myout = getConvOutputY(sindex);

			if (getNodeMode(sindex) == "Output") {

				rval.output_width = 0;
				rval.output_height = 0;
				for (int i = 0; i < size(); i++) {
					if (isDataSource(i)) {
						rval.output_width = getDataImageWidth(i);
						rval.output_height = getDataImageHeight(i);
						i = size();
					}
				}

				if ((rval.output_width == 0) || (rval.output_height == 0)) {
					addErrorString("getImageSizeDataFromOutputImage : image height or width = 0");
				}

				if (mxin > 0) {
					rval.num_width_output_steps = rval.output_width / mxout;		// num steps wide
					rval.input_width = rval.num_width_output_steps * mxin;		// in width
				}
				if (myin > 0) {
					rval.num_height_output_steps = rval.output_height / myout;	// num steps high
					rval.input_height = rval.num_height_output_steps * myin;	// in height
				}

			}
			else if (getNodeMode(sindex) == "Hidden") {

				vector<int> sn0 = getImmediateFollowingConvNets(sindex);

				if (sn0.size() > 0) {

					imageSizeData tval1 = getImageSizeDataFromOutputImage(sn0[0]);

					for (int i = 1; i < (int)sn0.size(); i++) {
						imageSizeData tval2 = getImageSizeDataFromOutputImage(sn0[i]);
						if ((tval1.input_width != tval2.input_width) || (tval1.input_height != tval2.input_height) ||
							(tval1.output_width != tval2.output_height) || (tval1.output_height != tval2.output_height) ||
							(tval1.num_width_output_steps != tval2.num_width_output_steps) || (tval1.num_height_output_steps != tval2.num_height_output_steps)) {
							addErrorString("Error : getImageSizeDataFromOutputImage found different sizes for " + getID(sn0[0]) + " and " + getID(sn0[i]));
						}
					}

					rval.output_width = tval1.input_width;
					if (mxout > 0) {
						rval.num_width_output_steps = rval.output_width / mxout;	// num steps wide
						rval.input_width = rval.num_width_output_steps * mxin;		// in width
					}

					rval.output_height = tval1.input_height;
					if (myout > 0) {
						rval.num_height_output_steps = rval.output_height / myout;	// num steps high
						rval.input_height = rval.num_height_output_steps * myin;	// in height
					}

				}

			}

		}

		return(rval);
	}

	imageSizeData getInputImageSizeWithNoSupportingImage(int sindex)
	{
		imageSizeData rval;
		rval.input_width = 0;
		rval.input_height = 0;
		rval.output_width = 0;
		rval.output_height = 0;
		rval.num_width_output_steps = 0;
		rval.num_height_output_steps = 0;

		if (isConvLayer(sindex)) {

			string mname = getID(sindex);

			int nxsteps = 0;
			int nysteps = 0;
			int mwidth = 0;
			int mheight = 0;

			bool mgray = getImageGray();
			int mxin = getConvInputX(sindex);
			int myin = getConvInputY(sindex);
			int stridex = getConvOutputStrideX(sindex);

			int min_cinput_size = mxin * myin;
			if (!mgray) {
				min_cinput_size = 3 * min_cinput_size;
			}
			vector<string> supportingNodes = getImmediateSupportingNodes(getIndexFromID(mname));
			if ((int)supportingNodes.size() >= min_cinput_size) {
				if ((myin > 0) && (mxin > 0) && (stridex > 0)) {
					nysteps = 1;
					mheight = myin;
					int inlength = (int)supportingNodes.size() / mheight;
					if (!mgray) {
						inlength = inlength / 3;
					}
					mwidth = inlength;
					if ((mwidth > 0) && (mheight > 0)) {
						if (mgray) {
							float ft = (float)supportingNodes.size() / (float)(mwidth * mheight);
							int it = (int)ft;
							if (ft != (float)it) {
								char s[200];  sprintf_s(s, " : mwidth = %i : mheight = %i", mwidth, mheight);
								addErrorString("Error for " + mname + " : For convs supported by nodes and not using color images, num supporting nodes must be increment of (conv input width * conv input height)" + s);
							}
						}
						else {
							float ft = (float)supportingNodes.size() / (float)(3 * mwidth * mheight);
							int it = (int)ft;
							if (ft != (float)it) {
								char s[200];  sprintf_s(s, " : mwidth = %i : mheight = %i", mwidth, mheight);
								addErrorString("Error for " + mname + " : For convs supported by nodes and using color images, num supporting nodes must be increment of (3 * conv input width * conv input height)" + s);
							}
						}
					}
					else {
						char s[200];  sprintf_s(s, " : mwidth = %i : mheight = %i", mwidth, mheight);
						addErrorString("Error for " + mname + " : image width and height must be > zero" + s);
					}
					if (stridex >= mxin) {
						nxsteps = mwidth / stridex;		// num out steps wide
					}
					else {
						nxsteps = (mwidth - mxin) / stridex;
					}
					rval.output_width = mwidth;				// out width
					rval.num_width_output_steps = nxsteps;	// num out steps wide
					rval.output_height = mheight;			// out height
					rval.num_height_output_steps = nysteps;	// num steps high
				}
			}

		}

		return(rval);
	}

	int getImageConvOutputWidth(int cindex)
	{
		int numxsteps = 0;
		int nxin = getConvInputX(cindex);
		int nxout = getConvOutputX(cindex);
		int nxstride = getConvOutputStrideX(cindex);
		imageSizeData imageSize = getImageSizeData(cindex);
		int input_width = imageSize.input_width;
		if (nxstride > 0) {
			if (nxstride > nxin) {
				numxsteps = (input_width - nxin) / nxstride;
			}
			else {
				numxsteps = input_width / nxstride;
			}
			if (numxsteps <= 0) {
				char s[200]; sprintf_s(s, " : nxin = %i : nxstride = %i : input_width = %i", nxin, nxstride, input_width);
				addErrorString("getImageConvOutputWidth for " + getID(cindex) + " zero x steps not allowed (1)" + s);
			}
		}
		else {
			char s[200]; sprintf_s(s, " : nxin = %i : nxstride = %i : input_width = %i", nxin, nxstride, input_width);
			addErrorString("getImageConvOutputWidth for " + getID(cindex) + " zero x stride not allowed (2)" + s);
		}
		int output_width = numxsteps * nxout;
		return(output_width);
	}

	int getImageConvOutputHeight(int cindex)
	{
		int numysteps = 0;
		int nyin = getConvInputY(cindex);
		int nyout = getConvOutputY(cindex);
		int nystride = getConvOutputStrideY(cindex);
		imageSizeData imageSize = getImageSizeData(cindex);
		int input_height = imageSize.input_height;
		if (nystride > 0) {
			if (nystride > nyin) {
				numysteps = (input_height - nyin) / nystride;
			}
			else {
				numysteps = input_height / nystride;
			}
			if (numysteps <= 0) {
				char s[200]; sprintf_s(s, " : nyin = %i : nystride = %i : input_height = %i", nyin, nystride, input_height);
				addErrorString("getImageConvOutputHeight for " + getID(cindex) + " zero y steps not allowed (1)" + s);
			}
		}
		else {
			char s[200]; sprintf_s(s, " : nyin = %i : nystride = %i : input_height = %i", nyin, nystride, input_height);
			addErrorString("getConvOutputHeight for " + getID(cindex) + " zero y stride not allowed (2)" + s);
		}
		int output_height = numysteps * nyout;
		return(output_height);
	}

	int getNumOutputsFromConv(string id)
	{
		int sindex = getIndexFromID(id);
		if (isConvLayer(sindex)) {
			if (getConvType(sindex) == HIDDEN_NET) {
				return(getConvNodesPerLayer(sindex));
			}
		}
		return(0);
	}














};



//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************

class editStruct
{
public:
	int operation;
	int hScrollData;
	int vScrollData;
	float zoomData;
	NetData editData;

	editStruct()
	{
		clear();
	}

	void clear() {
		operation = NONE;
		zoomData = 0.0;
		hScrollData = 0;
		vScrollData = 0;
		editData.clear();
	}

	void setOpParams(int op, float zoom, int hscroll, int vscroll)
	{
		operation = op;
		zoomData = zoom;
		hScrollData = hscroll;
		vScrollData = vscroll;
	}

	void pushData(netDataStruct data)
	{
		editData.push_back(data);
	}

	void pushNodeData(netDataStruct node)
	{
		editData.pushNodeData(node);
	}

	void pushConnectData(string sid, string slbl, string node_in, string node_out, int gindex, double sweight, vector<double> sweights, vector<twoValues> points)
	{
		editData.pushConnectData(sid, slbl, node_in, node_out, gindex, sweight, sweights, points);
	}

	void pushDataSource(string sid, string slbl, int sx, int sy, string smode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filepaths)
	{
		editData.pushDataSource(sid, slbl, sx, sy, smode, width, height, file_min_width, aspect, gray, wav_max, filepaths);
	}

	void pushConvLayer(netDataStruct layer)
	{
		editData.pushConvLayer(layer); 
	}

	void pushAttentionLayer(netDataStruct layer)
	{
		editData.pushAttentionLayer(layer);
	}

	void pushAssembly(netDataStruct layer)
	{
		editData.pushAssembly(layer);
	}

	void pushNetCluster(netDataStruct layer)
	{
		editData.pushNetCluster(layer);
	}

	void pushDropout(netDataStruct layer)
	{
		editData.pushDropout(layer);
	}

	bool isNode(int index)
	{
		if (index < (int)editData.size()) {
			return(editData.isNode(index));
		}
		return(false);
	}

	bool isConnect(int index)
	{
		if (index < (int)editData.size()) {
			return(editData.isConnect(index));
		}
		return(false);
	}

	string getID(int index) {
		if (index < (int)editData.size()) {
			return(editData.getID(index));
		}
		return("None");
	}

	bool isEdit() {
		if (operation == UNDO_EDIT) {
			return(true);
		}
		return(false);
	}

	bool isAdd() {
		if (operation == UNDO_ADD) {
			return(true);
		}
		return(false);
	}

	bool isDelete() {
		if (operation == UNDO_DELETE) {
			return(true);
		}
		return(false);
	}

	int getOperation() { return(operation); };
	int getHScroll() { return(hScrollData); };
	int getVScroll() { return(vScrollData); };
	float getZoom() { return(zoomData); };
	NetData getData() { return(editData); };

};

//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************

class EditData
{
public:
	editStruct meditdata;

	vector<editStruct> editData;

	int size() {
		return((int)editData.size());
	}

	void clear() {
		editData.clear();
	}

	void clearCurrent()
	{
		meditdata.clear();
	}

	void setOpParams(int sop, float szoom, int shscroll, int svscroll)
	{
		meditdata.setOpParams(sop, szoom, shscroll, svscroll);
	}

	void pushData(netDataStruct data)
	{
		meditdata.pushData(data);
	}

	void pushNodeData(netDataStruct node)
	{
		meditdata.pushNodeData(node);
	}

	void pushConnectData(string sid, string slbl, string node_in, string node_out, int gindex, double sweight, vector<double> sweights, vector<twoValues> points)
	{
		meditdata.pushConnectData(sid, slbl, node_in, node_out, gindex, sweight, sweights, points);
	}

	void pushDataSource(string sid, string slbl, int sx, int sy, string smode, int width, int height, int file_min_width, float aspect, bool gray, int wav_max, vector<string> filepaths)
	{
		meditdata.pushDataSource(sid, slbl, sx, sy, smode, width, height, file_min_width, aspect, gray, wav_max, filepaths);
	}

	void pushConvLayer(netDataStruct layer)
	{
		meditdata.pushConvLayer(layer); 
	}

	void pushAttentionLayer(netDataStruct layer)
	{
		meditdata.pushAttentionLayer(layer);
	}

	void pushAssembly(netDataStruct layer)
	{
		meditdata.pushAssembly(layer);
	}

	void pushNetCluster(netDataStruct layer)
	{
		meditdata.pushNetCluster(layer);
	}

	void pushDropout(netDataStruct layer)
	{
		meditdata.pushDropout(layer);
	}

	void push_back() {
		if (editData.size() > 100) {
			editData.erase(editData.begin());
		}
		editData.push_back(meditdata);
	}

	bool isNode(int index1, int index2)
	{
		if (index1 < (int)editData.size()) {
			return(editData[index1].isNode(index2));
		}
		return(false);
	}

	bool isConnect(int index1, int index2)
	{
		if (index1 < (int)editData.size()) {
			return(editData[index1].isConnect(index2));
		}
		return(false);
	}

	string getID(int index1, int index2) {
		if (index1 < (int)editData.size()) {
			return(editData[index1].getID(index2));
		}
		return("None");
	}

	editStruct back()
	{
		meditdata = editData.back();
		return(meditdata);
	}

	void loadLast()
	{
		meditdata = editData[editData.size() - 1];
	}

	void pop_back()
	{
		if (editData.size() > 0) {
			editData.pop_back();
		}
	}

	bool isCurrentEdit() {
		return(meditdata.isEdit());
	}

	bool isCurrentAdd() {
		return(meditdata.isAdd());
	}

	bool isCurrentDelete() {
		return(meditdata.isDelete());
	}



};

//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************

class node
{
public:
	string id;
	string function;
	bool isoutput;
	bool trainable;
	double eta;
	double alpha;
	double rand;
	double bias;
	vector<string> inputNodeIDs;
	vector<string> inputConvIDs;
	vector<double> weights;

public:
	node() { clear(); };

	void clear() {
		id = "";
		function = "";
		isoutput = false;
		trainable = true;
		eta = 0;
		alpha = 0;
		rand = 0;
		bias = 0;
		inputNodeIDs.clear();
		inputConvIDs.clear();
		weights.clear();
	}

	void setData(string tid, string tfunction, bool tisoutput, double teta, double talpha, double trand,
		double tbias, bool ttrainable, vector<string> tinputNodeIDs, vector<string> tinputConvIDs, vector<double> tweights)
	{
		clear();
		id = tid;
		function = tfunction;
		isoutput = tisoutput;
		trainable = ttrainable;
		eta = teta;
		alpha = talpha;
		rand = trand;
		bias = tbias;
		if (tinputNodeIDs.size() == tweights.size()) {
			for (int i = 0; i < (int)tinputNodeIDs.size(); i++) {
				inputNodeIDs.push_back(tinputNodeIDs[i]);
				weights.push_back(tweights[i]);
			}
		}
		else {
			for (int i = 0; i < (int)tinputNodeIDs.size(); i++) {
				addErrorString("setData : " + id + " : input " + tinputNodeIDs[i]);
			}
			char s[200];  sprintf_s(s, "setData : inputNodeIDs size (%i) doesn't equal weights size (%i) for ", (int)tinputNodeIDs.size(), (int)tweights.size());
			addErrorString(s + tid);
		}
		for (int i = 0; i < (int)tinputConvIDs.size(); i++) {
			inputConvIDs.push_back(tinputConvIDs[i]);
		}
	}

	string getID() { return (id); };
	string getFunction() { return (function); };
	bool getIsOutput() { return(isoutput); };
	double getEta() { return(eta); };
	double getAlpha() { return(alpha); };
	double getRand() { return(rand); };
	double getBias() { return(bias); };
	bool getTrainable() { return(trainable); };
	int getNumInputNodes() { return((int)inputNodeIDs.size()); };
	int getNumInputConvs() { return((int)inputConvIDs.size()); };

	string getInputNodeID(int index) {
		if (index < (int)inputNodeIDs.size()) {
			return((string)inputNodeIDs[index]);
		}
		addErrorString("node::getInputNodeID : index out of bounds");
		return("");
	}

	string getInputConvID(int index) {
		if (index < (int)inputConvIDs.size()) {
			return((string)inputConvIDs[index]);
		}
		addErrorString("node::getInputConvID : index out of bounds");
		return("");
	}

	double getWeight(int index) {
		if ((index >= 0) && (index < (int)weights.size())) {
			return((double)weights[index]);
		}
		addErrorString("node::getWeight : index out of bounds");
		return(0);
	}

	string getCsvString() {
		string rval = "";
		return(rval);
	}

	void parseCsvString(string data) {
	}

	/*void Serialize(CArchive& ar) {
		if (ar.IsStoring())
		{
		}
		else
		{
		}
	}*/

	void addErrorString(string err);

};

//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************

class conv : public node
{
public:
	string mode;
	int nxin;
	int nyin;
	int nxout;
	int nyout;
	int nxstride;
	int nystride;
	int input_width;
	int input_height;
	int data_type;
	bool gray;

	int layers;
	int nodes;

	int numxsteps;
	int numysteps;
	int output_width;
	int output_height;

	int d;
	int dk;
	int dv;

	bool used;

	vector<string> supportingConvIDs;
	vector<string> followingConvIDs;

public:
	conv() { clear(); };

	void clear() {

		node::clear();

		mode = "";
		nxin = 0;
		nyin = 0;
		nxout = 0;
		nyout = 0;
		nxstride = 0;
		nystride = 0;
		input_width = 0;
		input_height = 0;
		data_type = NONE;
		gray = false;

		layers = 0;
		nodes = 0;

		numxsteps = 0;
		numysteps = 0;
		output_width = 0;
		output_height = 0;

		used = false;

		supportingConvIDs.clear();
		followingConvIDs.clear();

	}

	void setData(string tid, string tmode, string tfunction, double teta, double talpha, double trand, 
		int tlayers, int tnodes, double tbias, vector<double> tweights, bool ttrainable,
		int tnxin, int tnyin, int tnxout, int tnyout, int tnxstride, int tnystride, int twidth, int theight, bool tgray, int tdata_type,
		int td, int tdk, int tdv,
		vector<string> tsupportingNodeIDs, vector<string> tsupportingConvIDs, vector<string> tfollowingConvIDs)
	{
		clear();

		id = tid;
		mode = tmode;
		function = tfunction;
		eta = teta;
		alpha = talpha;
		rand = trand;
		bias = tbias;
		weights = tweights;
		trainable = ttrainable;

		nxin = tnxin;
		nyin = tnyin;
		nxout = tnxout;
		nyout = tnyout;
		nxstride = tnxstride;
		nystride = tnystride;
		input_width = twidth;
		input_height = theight;
		gray = tgray;

		data_type = tdata_type;
		if ((data_type != DATA_CONV) && (data_type != IMAGE_CONV) && (data_type != SOUND_CONV) && (data_type != TEXT_CONV) && 
			(data_type != HIDDEN_NET) && (data_type != ATTENTION) && (data_type != SELFATTENTION) && (data_type != NETCLUSTER) && 
			(data_type != DROPOUT)) {
			addErrorString("conv::setData for " + id + " : data type must be series, image, sound, text, or must be hidden net");
		}

		layers = tlayers;
		nodes = tnodes;

		d = td;
		dk = tdk;
		dv = tdv;

		if (mode == "Input") {		
			numxsteps = 1;
			output_width = input_width;
			numysteps = 1;
			output_height = input_height;
		}
		else if ((data_type != ATTENTION) && (data_type != SELFATTENTION) && (data_type != NETCLUSTER) && (data_type != DROPOUT)) {

			numxsteps = 0;
			if (nxstride > 0) {
				if (nxstride > nxin) {
					numxsteps = (input_width - nxin) / nxstride;
				}
				else {
					numxsteps = input_width / nxstride;
				}
				if (numxsteps <= 0) {
					char s[200];  sprintf_s(s, " : nxin = %i : nxstride = %i : input_width = %i", nxin, nxstride, input_width);
					addErrorString("conv::setData for " + id + " zero x steps not allowed (1)" + s);
				}
			}
			else {
				char s[200];  sprintf_s(s, " : nxin = %i : nxstride = %i : input_width = %i", nxin, nxstride, input_width);
				addErrorString("conv::setData for " + id + " zero x stride not allowed (2)" + s);
			}

			numysteps = 0;
			if (nystride > 0) {
				if (nystride > nyin) {
					numysteps = (input_height - nyin) / nystride;
				}
				else {
					numysteps = input_height / nystride;
				}
				if ((numysteps <= 0) && (data_type != HIDDEN_NET)) {
					char s[200];  sprintf_s(s, " : nyin = %i : nystride = %i : input_height = %i", nyin, nystride, input_height);
					addErrorString("conv::setData for " + id + " zero y steps not allowed (1)" + s);
				}
			}
			else if (data_type != HIDDEN_NET) {
				char s[200];  sprintf_s(s, " : nyin = %i : nystride = %i : input_height = %i", nyin, nystride, input_height);
				addErrorString("conv::setData for " + id + " zero y stride not allowed (2)" + s);
			}

			output_width = numxsteps * nxout;
			output_height = numysteps * nyout;

		}

		inputNodeIDs = tsupportingNodeIDs;
		supportingConvIDs = tsupportingConvIDs;
		followingConvIDs = tfollowingConvIDs;

	}

	string getMode() { return(mode); };
	int getNxIn() { return(nxin); };
	int getNyIn() { return(nyin); };
	int getNxOut() { return(nxout); };
	int getNyOut() { return(nxout); };
	int getNxStride() { return(nxstride); };
	int getNyStride() { return(nystride); };
	int getNumLayers() { return(layers); };
	int getNumNodes() { return(nodes); };


};



