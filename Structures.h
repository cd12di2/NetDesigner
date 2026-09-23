#pragma once

#include <stdint.h>
#include <vector>

using namespace std;


#define FFT_FOLLOWING                   1
#define CONV_FOLLOWING                  2
#define FULLY_CONNECTED_FOLLOWING       3
#define CONV_FOLLOWING_FFT_FOLLOWING    4
#define HIDDEN_NET_FOLLOWING			5


typedef struct dataInput {
	string label = "";
	double* inputpointer;
} dataInput;

typedef struct imageData {
    double* pointer = NULL;
    int numx = 0;
    int numy = 0;
    int xstride = 0;
    bool gray = false;
} imageData;

typedef struct imageSizeData {
    int input_width;
    int input_height;
    int output_width;
    int output_height;
    int num_width_output_steps;
    int num_height_output_steps;
} IMAGESIZEDATA;

typedef struct nodeStructure {
    vector<int> indices;
    vector<string> ids;
} NODESTRUCTURE;

typedef struct wave_check_results {
	int files_shortest = 0;
	int files_longest = 0;
	double files_max = 0.0;
	double files_min = 0.0;
	int wave_stride = 0;
	vector<string> wave_paths;
	vector<int> wave_data_lengths;
	vector<_int64> wave_start_indices;
	vector<int> num_copy_iterations;
} WAVE_CHECK_RESULTS;

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

typedef struct twoValues {
	long x = 0;
	long y = 0;
} twoValues;

typedef struct int64Double {
	int64_t index = 0;
	double err = 0.0;
} int64Double;

typedef struct doubleKvPair {
	string key = "";
	double value = 0.0;
} doubleKvPair;

typedef struct boolInt {
	bool valid = false;
	int value = -1;
} boolInt;

typedef struct boolDoublePointer {
	bool valid = false;
	double* input = NULL;
} boolDoublePointer;

typedef struct boolIntDoublePointer {
	bool valid = false;
	int count = 0;
	double* input = NULL;
} boolIntDoublePointer;

typedef struct boolIntVectorStringDoublePointer {
	bool valid = false;
	int count = 0;
	vector<string> inputs;
	double* pointer = NULL;
} boolIntVectorStringDoublePointer;

typedef struct twoStrings {
	string s1 = "";
	string s2 = "";
} twoStrings;

typedef struct attentionConnection {
	string input = "";
	bool k = false;
	bool q = false;
	bool v = false;
} attentionConnection;

typedef struct data_column {
	CString header = _T("");
	vector<double> column_data;
	vector<CString> column_string_data;
} DATA_COLUMN;

typedef struct dataset {
	CString file_path = _T("");
	vector<data_column> set_data;
} DATASET;

typedef struct data_column2 {
	string header = "";
	vector<double> column_data;
	vector<string> column_string_data;
} DATA_COLUMN2;

typedef struct dataset2 {
	string file_path = "";
	vector<data_column2> set_data;
} DATASET2;

typedef struct processData {
	string key1 = "";
	string key2 = "";
	string key3 = "";
	string process = "";
	double lfactor = 0.0;
	double ufactor = 0.0;
	int stride = 1;
	int downsample = 1;
} PROCESSDATA;

typedef struct inputData {
	string nodeId = "";
	vector<string> inputFields;
	processData processdata;
	int indexOffset = 0;
	bool from_data_conv = false;
} INPUTDATA;

typedef struct outputData {
	string nodeId = "";
	string outputField = "";
	string outputFieldAlias = "";
	processData processdata;
	bool imageOutput = false;
	int indexOffset = 0;
} OUTPUTDATA;

typedef struct fftIndices {
	int index = 0;
	int fftnum = 0;
} FFTINDICES;

typedef struct rgb_value {
	float red = 0.0;
	float green = 0.0;
	float blue = 0.0;
} RGB_VALUE;

typedef struct wave_data {
	int length = 0;
	int file_length = 0;
	int sample_rate = 0;
	double max = 0.0;
	vector<double> data;
} WAVE_DATA;

typedef struct train_index {
	int dindex = 0;
	int findex = 0;
	int sindex = 0;
} TRAIN_INDEX;

typedef struct assembly_input_counts {
	string assembly_id = "";
	int input_count = 0;
	int output_count = 0;
} ASSEMBLY_INPUT_COUNTS;

typedef struct assembly_sub_component {
	int net_index = -1;
	int input_sub_index = -1;
	int output_sub_index = -1;
} ASSEMBLY_SUB_COMPONENT;

typedef struct port_parameters {
	string name = "";
	char inputoutput = '-';
	int port_number = -1;
	bool converted = false;
} PORT_PARAMETERS;

typedef struct assembly_input_connections {
	string input_node_name = "";
	vector<string> old_connection_names;
	vector<string> new_input_node_names;
	vector<string> new_output_node_names;
} ASSEMBLY_INPUT_CONNECTIONS;

typedef struct assembly_output_connections {
	string output_node_name = "";
	vector<string> old_connection_names;
	vector<string> new_input_node_names;
	vector<string> new_output_node_names;
} ASSEMBLY_OUTPUT_CONNECTIONS;




