#pragma once


//#define GCC


#include <atlstr.h>
#include "CNetData.h"
#include <iostream>
#include <stdint.h>
#include "Structures.h"


using namespace std;


#define	SIGMOID		0
#define	RELU		1
#define	TANH		2
#define	SHOCKLEY    3
#define	MAXPOOL		4
#define	MULTIPLY	5
#define	ACOS    	6
#define GAUSSIAN    7
#define ADD         8
#define SOFTMAX     9

#define CONV_GENERIC    0
#define CONV_FULLY      1
#define CONV_SEPARATED  2
#define CONV_SHARED     3

#define DEFAULT_NUM_BUFFER_INPUTS    20


class HiddenNode;
class HiddenConvolution;
class HiddenNet;
class NetCluster;
class Attention;
class SelfAttention;
class Dropout;
class Addition;
class Multiplication;



class OutputNode
{
public:
    string id = "";
    bool allocated = false;
    bool trainable = true;
    bool zeroout = false;
    bool inputoverrunmsg = false;
    int num_allocated_inputs = DEFAULT_NUM_BUFFER_INPUTS;
    int num_allocated_backprop_inputs = DEFAULT_NUM_BUFFER_INPUTS;
    int num_inputs = 0;
    int num_backprop_inputs = 0;
    int backprop_set_index = 0;
    int type = SIGMOID;
    int maxpool_index = -1;
    double alpha = 0.2;
    double eta = 0.02;
    double random_scale = 0.02;
    double sum = 0.0;
    double error = 0.0;
    double wrms = 0.0;
    double sigmoid_leakage = 0.010;
    double relu_leakage = 0.001;
    double softmax_error = 0.0;
    double sum_add_limit;

    string dmsg = "";
    string dmsg_sub = "";

    double zero = 0.0;
    double dummy = 0.0;

    double bias = 0.0;
    double* weights;

    bool weights_archived = false;
    double archive_bias = 0.0;
    double* archive_weights;

    double* deltaweights;
    double* backproperrors;

    int64_t* inputpointers;
    int64_t* backupinputpointers;
    string* inputpointernames;
    
    int64_t* backpropinputpointers;
    string* backpropinputpointernames;

    double* testoutputpointer;

    double output;
    double backprop;


public:
    OutputNode() { clear(); };
    ~OutputNode() { deallocate(); };

    void clear();

    void init(int ninputs, int nbinputs, int ntype, double neta, double nalpha, double nrandom_scale);
    void allocate(int ninputs, int nbinputs, int ntype, double neta, double nalpha, double nrandom_scale);
    
    bool addInputPointer(string name, double* tinput);
    bool setBackpropInputPointer(double* tprop, string name);

    string getInputPointerName(int index);
    int getInputIndexFromName(string name);

    void setTestOutputPointer(double* toutput);

    double getOutput();
    double* getOutputPointer();
    void setOutput(double out);

    double* getBackpropOutputPointer(string name);

    // ****************************************************************************
    // ****************************************************************************

    bool join(OutputNode* onode);
    bool join(HiddenNode* hnode);
    bool join(HiddenConvolution* conv);
    bool join(HiddenNet* hnet);
    bool join(NetCluster* ncluster);
    bool join(Attention* attention);
    bool join(SelfAttention* selfattentions);
    bool join(Dropout* dropout);
    bool join(Addition* addition);
    bool join(Multiplication* multiplication);

    // ****************************************************************************
    // ****************************************************************************

    virtual void deallocate();

    virtual void clearWeights();
    virtual void randomizeWeights();
    virtual bool checkValidWeights();

    virtual void forward();

    virtual double getError();
    virtual void backward();

    double getBackpropSum();

    void zeroBackpropErrors();
    void setBackpropError(int index, double berror);
    double getBackpropError(int index);

    virtual double activationFunction(double sum);

    double sigmoid(double data);
    double sigmoidDerivative(double data);
    double relu(double data);
    double reluDerivative(double data);
    double mtanh(double data);
    double mtanh2(double data);
    double mtanhDerivative(double data);
    double shockley(double data);
    double shockleyDerivative(double data);
    double softmax(double data);
    double softmaxDerivative(double data);
    double gaussian(double data);
    double gaussianDerivative(double data);

    double checkError(double toutput);

    nodeWeightData getWeights();
    void setWeights(nodeWeightData nwd);

    void archiveWeights();
    void restoreWeights();

    void rmsWeights();
    double getRMSWeights();

    double squareWeights();
    double maxWeights();

    void setID(string tid) { id = tid; };
    void setTrainable(bool val);
    void setZeroOut(bool val);

    void setParameters(double sleak, double rleak) { 
        sigmoid_leakage = sleak; 
        relu_leakage = rleak;
    };

    void setSoftmaxError(double error) { softmax_error = error; };

    void setInputToZero(string inputid);
    void setInputToData(string inputid);

    void addTrainString(string err);
    void addErrorString(string err);

};


class HiddenNode : public OutputNode
{
public:
    HiddenNode() { clear(); };
    ~HiddenNode() { deallocate(); };

    virtual double getError();
    double getBackprop(int index);

};


class HiddenConvolution
{
public:
    string id = "";
    string mode = "";
    int type = SIGMOID;
    double alpha = 0.2;
    double eta = 0.02;
    double random_scale = 0.02;

    bool layer_on_input = false;

    bool image_output;

    int conv_type = CONV_GENERIC;

    int num_layers = 1;
    int num_nodes_per_layer = 8;
    int num_hidden_nodes = 0;
    int data_type = NONE;

    int num_weights = 0;
    int output_node_offset = 0;
    int num_layer_inputs = 0;

    int numdims = 0;
    int *convins = NULL;
    int *convouts = NULL;
    int *convstrideins = NULL;
    int *inputdims = NULL;
    int *outputdims = NULL;
    int *num_steps = NULL;
    int *sub_indices = NULL;
    int *main_indices = NULL;

    int* convinstrides = NULL;
    int* convoutstrides = NULL;
    int* inputdimstrides = NULL;
    int* inputdimstrides2 = NULL;
    int* outputdimstrides = NULL;

    int input_index = 0;
    int input_data_size = 0;
    int output_data_size = 0;
    int output_connection_index = -1;
    int num_output_connections = 0;
    int conv_input_data_size = 0;
    int conv_output_data_size = 0;
    int preprocess_data_size = 0;
    int backprop_set_index = 0;
    int backprop_index = 0;
    int backproppointerssize = 0;

    double zero = 0.0;
    double dummy = 0.5;
    double dummy2 = 0.0;

    double* conv_data = NULL;
    double* conv_backprop_data = NULL;

    double* output_data = NULL;
    double* backprop_output_data = NULL;

    int64_t* inputpointers = NULL;
    int64_t* backupinputpointers = NULL;
    string* inputpointernames = NULL;

    int* backpropinputindices = NULL;
    string* backpropinputnames = NULL;
    int64_t* backpropinputpointers = NULL;
    double* backpropdatabuffer;

    vector<vector<int>> node_indices;

    HiddenNode* hnodes = NULL;
    HiddenNode* inlayernodes = NULL;

    bool allocated = false;

public:
    HiddenConvolution() { clear(); };
    ~HiddenConvolution() { deallocate(); };

    void clear();

    bool init(string nid, string nmode, int nlayers, int nnodesperlayer, int ntype, double neta, double nalpha, double nrandom_scale, int ndata_type, 
              bool ntrainable, bool nzerooutput, bool nrandomize, int nnumdims, int* nconvins, int* nconvouts, int* nconvstrideins, int* ninputdims, 
              int ctype, bool nlayer_on_input, int nnum_layer_inputs, int noutconnections);

    bool allocate(int nnumdims, int* nconvins, int* nconvouts, int* nconvstrideins, int* ninputdims, bool ntrainable, bool nzerooutput, bool nrandomize);
    
    bool initializeNodes();

    void randomizeWeights();

    bool addHiddenNode(int lindex, int nindex);
    bool connectBackpropPointers(int lindex, int nindex);

    bool addInputLayer();
    bool connectInputLayerBackpropPointers();
    int getInputLayerNumInputs();

    void setInputToZero(string inputid);
    void setInputToData(string inputid);

    void deallocate();

    bool setBackpropInputPointer(int index, double* tprop);
    double* getInputLayerBackpropOutputPointers(int node_index, int input_index);

    double* getOutputPointer(int index);

    bool addInputPointer(string name, double* tinput);
    boolInt addNewBackpropConnection(string name);
    bool setBackpropInputPointer2(int connection_index, double* tprop);

    double* getBackpropOutputPointerConvOnInput(string name);

    // ****************************************************************************
    // ****************************************************************************

    bool join(HiddenNode* hnode);
    bool join(HiddenConvolution* conv);
    bool join(HiddenNet* hnet);
    bool join(NetCluster* ncluster);
    bool join(Attention* attention);
    bool join(SelfAttention* selfattentions);
    bool join(Dropout* dropout);

    // ****************************************************************************
    // ****************************************************************************

    void forward();
    void backward();

    void forward_generic();
    void backward_generic();

    void forward_fully();
    void backward_fully();

    void forward_shared();
    void backward_shared();

    void outputBackpropProcessing();

    int getIndicesSize(int* values);
    int getIndicesIndex(int* indices, int* values);
    int getIndicesIndex(int* indices, int* values, int* strides);
    int getIndicesSubIndex(int* sub_indices, int* main_indices, int* sub_values, int* main_values);
    int getIndicesSubIndex(int* sub_indices, int* main_indices, int* sub_values, int* main_values, int* strides);
    int* clearIndices(int* indices);
    bool incrementIndicesVector(int* indices, int* values);
    string getIndicesLabel(int* indices);

    int getNumWeights();
    double getWeightSquares();
    double maxWeights();

    bool checkValidWeights();

    void setParameters(double sleak, double rleak);

    void setTrainable(bool val);
    void setZeroOut(bool val);

    convWeightData getWeights();
    void setWeights(convWeightData cwd);

    void archiveWeights();
    void restoreWeights();

    string getStringBeforeColon(string cname);

    void addTrainString(string err);
    void addErrorString(string err);


};


class HiddenNet
{
public:
    bool allocated;
    bool has_softmax;

    string id;
    int type;
    double alpha;
    double eta;
    double random_scale;
    double sigmoid_leakage;
    double relu_leakage;

    double softmax_sum;

    int num_inputs;
    int num_layers;
    int num_nodes_layer;
    int num_outputs;

    int num_backprop_inputs;

    int num_nodes;

    int output_start_index;

    int parallel_processing_test;
    
    double dummy;

    HiddenNode* hnodes = NULL;

public:
    HiddenNet() { clear(); };
    ~HiddenNet() { deallocate(); };

    void clear();

    bool init(string nid, int ninputs, int nbinputs, int nnum_layers, int nnum_nodes_layer, int ntype, 
        double neta, double nalpha, double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutput);

    bool allocate(string nid, int ninputs, int nbinputs, int nnum_layers, int nnum_nodes_layer, int ntype, 
        double neta, double nalpha, double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutput);

    void addTrainString(string err);
    void addErrorString(string err);

    bool addInputPointer(string name, double* tinput);
    double* getOutputPointer(int index);

    bool setBackpropInputPointer(int output_index, double* pointer);

    double getBackpropError(int nindex, int eindex);

    // ****************************************************************************
    // ****************************************************************************

    bool join(HiddenNode* hnode);
    bool join(HiddenConvolution* conv);
    bool join(HiddenNet* hnet);
    bool join(NetCluster* ncluster);
    bool join(Attention* attention);
    bool join(SelfAttention* selfattentions);
    bool join(Dropout* dropout);
    bool join(Addition* addition);
    bool join(Multiplication* multiplication);

    // ****************************************************************************
    // ****************************************************************************

    void deallocate();

    void clearWeights();
    void randomizeWeights();
    bool checkValidWeights();

    void forward();
    virtual void backward();

    void softmaxNormalizeOutputLayers();

    void softmaxOutputLayerBackpropProcessing();

    int getNumWeights();
    double getWeightSquares();
    double maxWeights();
        
    void setInputToZero(string inputid);
    void setInputToData(string inputid);
        
    void setParameters(double sleak, double rleak);

    void setTrainable(bool val);
    void setZeroOut(bool val);

    hiddenWeightData getWeights();
    void setWeights(hiddenWeightData cwd);

    void archiveWeights();
    void restoreWeights();

    void setParallelProcessingTest(int test);

    void addPanel1String(string msg);

};


class Attention
{
public:
    bool allocated;

    string id;

    bool oninput;
    bool processsoftmax;

    int input_index;
    int backprop_set_index;
    
    int num_kinputs;
    int num_qinputs;
    int num_vinputs;

    int kdimension;
    int vdimension;
    int modeldimension;
    int input_size;
    int offset;

    int kstart_index;
    int qstart_index;
    int vstart_index;

    int intermedate_size;
    int output_size;

    int num_backprop_inputs;
    int* backprop_input_indices;
    int backprop_size;

    int output_connection_index;

    double sum_add_limit;

    string* inputpointernames;

    int64_t* inputpointers;
    int64_t* backpropinputpointers;

    double* intermediatebuffer;
    double* backpropinputs;
    double* outputbuffer;

    double* intermediatebackpropoutputs;
    double* intermediatesoftmaxbackpropoutputs;
    double* backpropoutputs;

    double dummy;
    double zero;
    double one;

public:
    Attention() { clear(); };
    ~Attention() { deallocate(); };

    void clear();

    bool init(string nid, int ndk, int ndv, int ndm, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool input);

    bool allocate(string nid, int ndk, int ndv, int ndm, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool ninput);

    void addTrainString(string err);
    void addErrorString(string err);

    void setInputPointer(int index, double* tinput, string name);
    bool addKInputPointer(string name, double* tinput);
    bool addQInputPointer(string name, double* tinput);
    bool addVInputPointer(string name, double* tinput);
    double* getOutputPointer(int index);

    boolInt addNewBackpropConnection(string name);
    bool setBackpropInputPointer(int output_index, double* pointer);
    double* getBackpropOutputPointer(int input_index);
    double* getKBackpropOutputPointer(string name);
    double* getQBackpropOutputPointer(string name);
    double* getVBackpropOutputPointer(string name);

    bool addInputPointer(string name, double* tinput);
    double* getBackpropOutputPointer(string name);

    // ****************************************************************************
    // ****************************************************************************

    bool joink(HiddenNode* hnode);
    bool joinq(HiddenNode* hnode);
    bool joinv(HiddenNode* hnode);

    bool joinv(HiddenConvolution* conv);

    boolInt joink(HiddenNet* hnet);
    boolInt joinq(HiddenNet* hnet, int offset);
    boolInt joinv(HiddenNet* hnet, int offset);

    bool joink(NetCluster* ncluster);
    bool joinq(NetCluster* ncluster);

    bool join(Dropout* dropout);

    // ****************************************************************************
    // ****************************************************************************

    void deallocate();

    void forward();
    void backward();

    void qktDotProduct();
    void vDotProduct();
    void qdtSoftMax();

    void calculateBackpropInputs();
    void calculateVBackprops();
    void calculateIntermediateBackprops();
    void calculateIntermediateBackSoftMax();
    void calculateQBackprops();
    void calculateKBackprops();

    double softmaxDerivative(double data);

    void addPanel1String(string msg);

};


class SelfAttention : public Attention
{
public:
    SelfAttention() { clear(); };
    ~SelfAttention() { deallocate(); };

    void clear();

    bool init(string nid, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool ninput);

    bool allocate(string nid, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool ninput);

    // ****************************************************************************
    // ****************************************************************************

    bool join(HiddenNode* hnode);

    bool join(HiddenConvolution* conv);

    bool join(HiddenNet* hnet);

    bool join(NetCluster* ncluster);

    bool join(Dropout* dropout);

    // ****************************************************************************
    // ****************************************************************************

};


class SubCluster
{
public:
    bool allocated;

    string id;
    string mode;
    int type;
    double alpha;
    double eta;
    double random_scale;
    double sigmoid_leakage;
    double relu_leakage;

    bool adaptive_split;
    int64_t index2percent;
    int64_t index98percent;

    int num_inputs;

    double filter_alpha;
    double filtered_error;
    double delta_error;

    int num_history = 5;
    double error_history[5];

    double dummy;

    double testoutput;

    OutputNode onode;

    bool dataset_allocated;
    int64_t num_dataset;

    int64_t* cluster_nodes;
    int64_t* cluster_sort_indices;
    double* last_net_outputs;
    double* output_test_values;


public:
    SubCluster() { clear(); };
    ~SubCluster() { deallocate(); };

    void clear();

    bool init(string nid, string nmode, int ninputs, double neta, double nalpha, double nrandom_scale);

    bool allocate(string nid, string nmode, int ninputs, double neta, double nalpha, double nrandom_scale);

    bool initializeNetSubCluster(int64_t tnum);
    bool allocateDataset(int64_t tnum);
    void randomizeClusterIndices();
    void setOutputTestValuesSorting();

    void testSort();
    void sortNetOutputs(int64_t size);
    void quickSortOutputs(int64_t lo, int64_t hi);
    int64_t partitionOutputs(int64_t lo, int64_t hi);

    void getClusteredOutputs(vector<vector<double>>* data);

    void addTrainString(string err);
    void addErrorString(string err);

    bool setInputPointer(string name, double* tinput);

    double* getBackpropOutputPointer(int backprop_index);

    double getBackpropError(int eindex);

    void deallocate();

    void clearWeights();
    void randomizeWeights();
    bool checkValidWeights();

    void setOutputTestData(int64_t dindex);
    double getFilteredError();

    double getError();

    void forward();
    virtual void backward(int64_t dindex);

    void zeroBackpropErrors();

    void updateNetCluster();

    int64_t splitCluster();

    int getNumWeights();
    double getWeightSquares();
    double maxWeights();

    void setParameters(double sleak, double rleak);

    void addPanel1String(string msg);

};


class NetCluster
{
public:
    bool allocated;

    string id;
    string mode;
    int type;
    double alpha;
    double eta;
    double random_scale;
    double sigmoid_leakage;
    double relu_leakage;

    bool has_softmax;

    int num_inputs;
    int num_outputs;

    int num_active_nodes;

    double filter_alpha;
    double filtered_error;
    double delta_error;

    double my_filtered_error;

    int num_history = 5;
    double error_history[5];

    double update_epochs;
    double num_epochs;
    double num_test_epochs;
    int64_t num_iterations;
    int64_t num_sub_iterations;
    int64_t num_update_iterations;

    double test_error;
    double split_variable;

    bool separating_data;
    bool learning_nodes;
    bool last_realign;

    double dummy;

    double* testoutputs;

    SubCluster subcluster;

    OutputNode* onodes = NULL;

    bool dataset_allocated;
    int64_t num_dataset;
    int64_t data_allocation_size;

    int test_cluster_index;
    int64_t num_test_dataset;

    int64_t* cluster_nodes;
    int64_t* cluster_sort_indices;
    double* last_net_outputs;
    double* output_test_values;

    int64_t* index_cluster_to_sub;
    int64_t* index_sub_to_cluster;

    vector<double> cluster_rms_errors;


public:
    NetCluster() { clear(); };
    ~NetCluster() { deallocate(); };

    void clear();

    bool init(string nid, string nmode, int ninputs, int noutputs, int ntype, double neta, double nalpha,
        double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutputs, double nepochs);

    bool allocate(string nid, string nmode, int ninputs, int noutputs, int ntype, double neta, double nalpha,
        double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutput, double nepochs);

    bool initializeNetCluster(int64_t tnum);
    bool allocateDataset(int64_t tnum);
    int64_t setupSubClusterIndices(int cindex);
    void setOutputTestValuesLearning();

    void testSort();
    void sortNetOutputsByCluster();
    void quickSortOutputsByCluster(int64_t lo, int64_t hi);
    int64_t partitionOutputsByCluster(int64_t lo, int64_t hi);

    void getClusteredOutputs(vector<vector<double>> *data);

    void addTrainString(string err);
    void addErrorString(string err);

    bool addInputPointer(string name, double* tinput);
    double* getOutputPointer(int index);

    double* getBackpropOutputPointer(int input_index, int backprop_index);

    double getBackpropError(int nindex, int eindex);

    // ****************************************************************************
   // ****************************************************************************

    bool join(HiddenNode* hnode);
    bool join(HiddenConvolution* conv);
    bool join(HiddenNet* hnet);
    bool join(NetCluster* ncluster);
    bool join(Attention* attention);
    bool join(SelfAttention* selfattention);
    bool join(Dropout* dropout);
    bool join(Addition* addition);
    bool join(Multiplication* multiplication);

    // ****************************************************************************
    // ****************************************************************************

    void deallocate();

    void clearWeights();
    void randomizeWeights();
    bool checkValidWeights();

    void setOutputTestData(int64_t dindex);
    double getFilteredError();

    void forward();
    virtual void backward(int64_t dindex);

    void softmaxNormalizeOutputLayers();
    void softmaxOutputLayerBackpropProcessing();

    void realignClusters();

    double getClusterRMSError(int cindex);
    double getClusterStdError(int cindex);

    void copySubClusterBackpropErrors(int64_t sindex);

    void zeroBackpropErrors();

    void updateNetCluster();
    void setupNextSubCluster();

    string getErrorString();

    double getCumulativeError(int64_t dindex);

    void updateGraphLabel();

    bool finished();

    int getNumWeights();
    double getWeightSquares();
    double maxWeights();

    void setParameters(double sleak, double rleak);
    void setParameters2(double testerror, double splitvariable, string splitmethod);

    void setTrainable(bool val);
    void setZeroOut(bool val);

    void setInputToZero(string inputid);
    void setInputToData(string inputid);

    hiddenWeightData getWeights();
    void setWeights(hiddenWeightData cwd);

    void archiveWeights();
    void restoreWeights();

    void addPanel1String(string msg);

};


class Dropout
{
public:
    bool allocated = false;

    string id;
    double zero;
    double dummy;
    double percent;
    int num_inputs;
    int input_index;
    int num_output_connections;
    int output_connection_index;
    int backproppointerssize;
    int backprop_set_index;

    int drop_point;
    int num_drops;
    int64_t* drop_indices;

    int64_t* inputpointers;
    int64_t* backupinputpointers;
    string* inputpointernames;

    double* outputdata;

    int* backpropinputindices;
    int64_t* backpropinputpointers;
    double* backpropdatabuffer;

public:
    Dropout() { clear(); };
    ~Dropout() { deallocate(); };

    void clear();

    bool init(string nid, int ninputs, double npercent, int noutconnections);

    bool allocate(string nid, int ninputs, double npercent, int noutconnections);

    void deallocate();

    bool setNextDropPoint();

    bool addInputPointer(string name, double* tinput);
    double* getOutputPointer(int index);

    bool setBackpropInputPointer(int output_index, double* pointer);
    double* getBackpropOutputPointer(string name);

    boolInt addNewBackpropConnection();

    // ****************************************************************************
    // ****************************************************************************

    bool join(HiddenNode* hnode);
    bool join(HiddenConvolution* conv);
    bool join(HiddenNet* hnet);
    bool join(NetCluster* ncluster);
    bool join(Attention* attention);
    bool join(Addition* addition);
    bool join(Multiplication* multiplication);

    // ****************************************************************************
    // ****************************************************************************

    void forward(bool train);
    void backward();

    void outputBackpropProcessing();

    void setInputToZero(string inputid);
    void setInputToData(string inputid);

    string getStringBeforeColon(string cname);

    void addTrainString(string err);
    void addErrorString(string err);
    void addPanel1String(string msg);

};


class Addition
{
public:
    bool allocated = false;

    string id;
    double zero;
    double dummy;
    double percent;
    int num_input_nodes;
    int num_output_nodes;
    int model_dimension;
    int backproppointerssize;
    int nodebackpropoutputpointer;
    bool input_nodes_only;

    boolInt input_node_connection;

    int input_connection_index;
    int output_connection_index;

    int64_t* inputindices;
    int64_t* inputpointers;
    int64_t* backupinputpointers;
    string* inputpointernames;

    double* outputdata;

    int* backpropinputindices;
    int64_t* backpropinputpointers;
    double* backpropdatabuffer;

public:
    Addition() { clear(); };
    ~Addition() { deallocate(); };

    void clear();

    bool init(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly);

    bool allocate(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly);

    void deallocate();

    boolInt addNewInputConnection(string name);
    bool addInputPointer(int nindex, double* tinput);
    bool addInputPointerForInputNode(string name, double* tinput);
    double* getOutputPointer(int index);

    boolInt addNewBackpropConnection();
    bool setBackpropInputPointer(int output_index, double* pointer);
    double* getBackpropOutputPointer(int index);

    // ****************************************************************************
    // ****************************************************************************

    bool join(HiddenNode* hnode);
    bool join(HiddenConvolution* conv);
    bool join(HiddenNet* hnet);
    bool join(NetCluster* ncluster);
    bool join(Attention* attention);
    bool join(Dropout* dropout);

    // ****************************************************************************
    // ****************************************************************************

    void forward();
    void backward();

    void setInputToZero(string inputid);
    void setInputToData(string inputid);

    void addTrainString(string err);
    void addErrorString(string err);
    void addPanel1String(string msg);

};


class Multiplication : public Addition
{
public:

    double sum_add_limit;

public:
    Multiplication() { clear(); };
    ~Multiplication() { deallocate(); };

    void clear();

    bool init(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly);

    bool allocate(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly);

    double* getBackpropOutputPointer(int inputindex, int modelindex);

    // ****************************************************************************
    // ****************************************************************************

    bool join(HiddenNode* hnode);
    bool join(HiddenConvolution* conv);
    bool join(HiddenNet* hnet);
    bool join(NetCluster* ncluster);
    bool join(Attention* attention);
    bool join(Dropout* dropout);

    // ****************************************************************************
    // ****************************************************************************

    void forward();
    void backward();

};


class NetCore {

public:
    bool allocated = false;

    bool conv_variables_initialized = false;

    int num_inputs = 0;
    int num_outputs = 0;
    int num_hidden_nodes = 0;
    int num_active_convs = 0;
    int num_hidden_nets = 0;
    int num_attention_layers = 0;
    int num_self_attention_layers = 0;
    int num_net_clusters = 0;
    int num_dropouts = 0;
    int num_additions = 0;
    int num_multiplications = 0;

    int parallel_processing_test = 100;

    bool conv_gray = false;
    int conv_numx = 0;
    int conv_numy = 0;
    int conv_single = 0;
    int conv_max_size = 0;

    int conv_output_test_index;

    bool randomize_data = true;
    bool use_validation_index = false;
    int data_file_index = 0;
    
    train_index data_index;
    train_index error_index;
    train_index validation_index;

    long num_validationdata_rows = 0;

    long train_inc = 10;
    long error_inc = 2;
    long update_inc = 100;

    _int64 train_count = 0;
    _int64 err_count = 0;

    float updateRate = 10;

    long num_filedata_rows = 0;
    vector<int> fileTrainIndicies;
    vector<int> fileTrainSizes;

    vector<int> fileValidationIndicies;
    vector<int> fileValidationSizes;

    bool use_validation_files = false;
    bool use_percent_validation = false;
    bool dovalidation = false;
    float validation_percent = 0.0F;

    vector<string> dataPathnames;
    vector<string> validationPathnames;
    vector<string> combinedPathnames;

    vector<string> previousDataPathnames;
    vector<string> previousValidationPathnames;

    vector<int> hiddenNodesWithInputsIndices;
    vector<int> activeConvsWithInputsIndices;
    vector<int> activeNetsWithInputsIndices;
    vector<int> activeNetClustersWithInputsIndices;
    vector<int> activeDropoutsWithInputsIndices;
    vector<int> activeAdditionsWithInputsIndices;
    vector<int> activeMultiplicationsWithInputsIndices;

    double train_error;
    double train_error_delta;
    double validation_error;

    double filter = 0.01;
    double errorfilter = 100;
    double errorFiltercount = 1;
    double validationfiltercount = 1;
    double accumulative_error = 0.0;
    double accumulative_error_slope = 0;
    double accumulative_validation_error = 0;
    double avgneterrors[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    bool normaiize_file_set = false;

    vector<inputData> netInputs;
    vector<outputData> netOutputs;

    vector<dataset2> csvFileData;

    vector<dataset2> processedData;
    vector<dataset2> processedValidationData;

    int num_copy_outputs_to_inputs = 0;
    int* copy_outputs_offsets;
    int* copy_to_input_indices;
    int* copy_from_output_indices;

    double* inputs;
    double* testoutputs;

    bool test_inputs_allocated;
    double* test_inputs;

    bool has_softmax;
    double softmax_sum;

    int set_data_count = 0;

    typedef struct loopControlData {
        string id = "";
        string mode = "";
        int start_index = 0;
        int end_index = 0;
        bool isconv = false;
        bool ishiddennet = false;
        bool isattention = false;
        bool isselfattention = false;
        bool isnetcluster = false;
        bool hasinput = false;
        bool isconvoutput = false;
        bool isdropout = false;
        bool isaddition = false;
        bool ismultiplication = false;
        int hidden_conv_index = 0;
    } LOOPCONTROLDATA;
    vector<loopControlData> loopControl;

    double sigmoid_leakage = 0.01;

    double zero = 0.0;
    double dummy = 0.0;

    OutputNode* onodes;
    HiddenNode* hnodes;
    HiddenConvolution* newConvs;
    HiddenNet* hnets;
    Attention* attentions;
    SelfAttention* selfattentions;
    NetCluster* nclusters;
    Dropout* dropouts;
    Addition* additions;
    Multiplication* multiplications;

    bool progress_dialog_up = false;
    CStatus progressDlg;

public:
    NetCore();
    ~NetCore();

    // ----------------------------------------------------
    // train memory management start ----------------------

    int64_t num_train_data = 0;
    int64_t num_validation_data = 0;

    int64_t inputtraindatasize = 0;
    int64_t inputvalidationdatasize = 0;
    int64_t outputtraindatasize = 0;
    int64_t outputvalidationdatasize = 0;

    double* inputtraindata;
    double* outputtraindata;
    double* inputvalidationdata;
    double* outputvalidationdata;

    bool has_series_conv = false;
    bool series_data_allocated = false;
    int series_input_width = 0;
    int series_input_height = 0;
    int series_mem_size = 0;
    int series_backprop_size = 0;
    double* series_data;
    double* series_backprop_data;

    bool csv_with_filename = false;
    string csv_filename_header = "";
    vector<string> csv_image_filenames;

    bool has_image_conv = false;
    bool image_data_allocated = false;
    bool use_images_as_outputs = false;
    int image_mem_size = 0;
    int image_backprop_size = 0;
    double* image_data;
    double* image_backprop_data;
    string input_image_conv_name = "";

    _int64 image_train_data_size = 0;
    bool image_train_data_allocated = false;
    bool image_gray = false;
    int image_width = 0;
    int image_height = 0;
    int image_single = 0;
    int image_max_size = 0;
    int num_train_images = 0;
    double* image_train_data;

    vector<int> imageTrainIndices;
    vector<int> imageValidationIndices;

    bool has_wave_conv = false;
    bool has_wave_fft_conv = false;
    bool wave_data_allocated = false;
    int wave_mem_size = 0;
    double* wave_data;

    int sound_data_process_type = NONE;
    int sound_process_stride = 1;
    int sound_down_sample = 1;
    int sound_data_stride = 1;
    bool process_sound_ffts = false;
    vector<string> soundfftnames;
    string soundfftprocess = "";
    int soundfftstride = 1;
    int soundfftdownsample = 1;

    bool wave_train_data_allocated = false;
    int wave_allocation_size = 0;
    int wave_files_longest = 0;
    wave_check_results wresults;
    double* wave_train_data;

    bool index_conv = false;
    bool sound_conv_only = false;
    bool processed_data_conv = false;
    int conv_frame_size = 1;
    int conv_input_size = 1;

    bool has_text_conv = false;

    void clearAndDeallocate();

    void getIndicesWithInputs();
    void setInputToZero(string inputid);
    void setInputToData(string inputid);

    bool allocateInputTrainData(int64_t num_data);
    bool allocateOutputTrainData(int64_t num_data);
    bool allocateInputValidationData(int64_t num_data);
    bool allocateOutputValidationData(int64_t num_data);
    bool checkAndAllocateSeriesMemory(int mem_size, int backprop_size);
    bool checkAndAllocateImageMemory(int mem_size, int backprop_size);
    bool checkAndAllocateTrainImageMemory(int mem_size, int num_images);
    bool checkAndAllocateSoundMemory(int mem_size);
    bool checkAndAllocateTrainSoundMemory(int mem_size);

    double getInputTrainData(int index);
    bool setInputTrainData(int index, double data);

    double getOutputTrainData(int index);
    bool setOutputTrainData(int index, double data);

    double getInputValidationData(int index);
    bool setInputValidationData(int index, double data);

    double getOutputValidationData(int index);
    bool setOutputValidationData(int index, double data);

    void loadExternalDataToNet(train_index tindex);
    void loadExternalValidationDataToNet(train_index tindex);

    void loadTrainSeriesDataToNet(int index);
    void loadValidationSeriesToNet(int index);

    void setImageParametersData(int width, int height, bool gray);
    void setInputImageData(double* pointer);
    void loadTrainImageToNet(int index);
    void loadValidationImageToNet(int index);

    bool loadTrainWaveDataToNet(train_index tindex);
    bool loadValidationWaveDataToNet(train_index tindex);

    bool dataInitializationErrorCheck1();
    bool dataInitializationErrorCheck2();
    bool dataInitializationErrorCheck2b();
    bool dataInitializationErrorCheck3();

    // train memory management end ------------------------
    // ----------------------------------------------------

    // ----------------------------------------------------
    // file and data management start ---------------------

    void setUseValidationFiles(bool use);
    void setUsePercentValidation(bool use, float percent);

    void setDataPathnames(vector<string> names) { dataPathnames = names; };
    void setValidationPathnames(vector<string> names) { validationPathnames = names; };
    void setCombinedPathnames(vector<string> names) { combinedPathnames = names; };

    bool loadFileData();

    void clearProcessedData();
    bool getTrainDataFromFileData();
    bool getProcessedDataFromFileData(vector<string> fnames, vector<string> fields, vector<dataset2>* processedData, vector<dataset2>* fileData);

    int getSoundConvNumStepsThroughTrainWaveFile(int pindex);
    int getSoundConvNumStepsThroughValidationWaveFile(int pindex);

    bool getImageAndWaveData();
    bool checkForSoundFFTs();
    bool getMaxWaveMagnitude();
    bool getImages();
    bool getSoundData();
    int getCombinedImageCount();

    bool copyProcessedDataToNetTrainArrays();
    bool copyProcessedDataToNetValidationArrays();
    void copyImageOutputDataToNetTrainArrays();
    void copyImageOutputDataToNetValidationArrays();

    void setupCopyOutputIndices();

    // file and data management end -----------------------
    // ----------------------------------------------------

    // ----------------------------------------------------
    // train and backpropagation start --------------------

    bool addLoopControlData(string dependency, int hnode_index, bool hidden_net, bool attention, bool selfattention, bool netcluster, bool dropout, bool addition, bool multiplication);

    void setRandomize(bool rand);
    void setParallelProcessingTest(int test);

    _int64 train();
    bool updateError();

    void forward(int index, bool training);
    void backward(int index1, int index2, int index3);

    void forward(double* tinputs, bool training);
    void backward(double* tinputs, double* toutputs, int input_index);

    bool initializeNetClusters();
    
    void resetUpdateInc();
    void setUpdateRate(float rate);
    train_index updateTrainIndex(train_index tindex);
    train_index updateTrainIndexNoRandom(train_index tindex);
    train_index updateValidationIndex(train_index tindex);

    void checkError(int index1, int index2, int input_index);
    void checkValidationError(int index1, int index2);

    void copyOutputsToInputs(int index);
    void copyValidationOutputsToInputs(int index);

    void softmaxNormalizeOutputLayers();
    void softmaxOutputLayerBackpropProcessing();

    double checkError1(double* tinputs, double* toutputs, int input_index);
    double checkError2(double* tinputs, double* toutputs, int input_index);
    double checkValidationError1(double* tinputs, double* toutputs);

    void archiveWeights();
    void restoreWeights();

    double getOutput(int index);

    // train and backpropagation end ----------------------
    // ----------------------------------------------------




    bool getConvFromConnectName(string cname);

    void addErrorString(string err);
    void addTrainString(string err);
    void addPanel1String(string msg);

    void showPopUpMessage(string msg, int progress);
    void closePopUp();


};


class NetBuilder : public NetCore {

public:
    int nactive_index = 0;
    int nhidden_net_index = 0;
    int nattention_layer_index = 0;
    int nself_attention_layer_index = 0;
    int nnet_cluster_index = 0;
    int ndropout_index = 0;
    int naddition_index = 0;
    int nmultiplication_index = 0;
    double wrms = 0.0;
    double zero = 0.0;

    bool image_variables_initialized = false;

    vector<bool> nodes_used;

    vector<conv> convs;
    vector<int> newConvToConv;
    vector<int> convToNewConv;

    vector<int> nodeIndices;
    vector<string> nodeIDs;
    vector<vector<string>> nodeInputs;

    vector<string> convInputs;
    vector<conv> netConvs;

    int outputconnectioncnt;
    vector<int> input_test_indices;
    vector<int> input_test_to_ndata_indices;
    vector<int> input_from_output_test_indices;
    vector<int> output_to_input_test_indices;

    vector<dataInput> inputPointers;

    vector<string> verifyStrings;

    NetData netData;


public:
    NetBuilder();
    ~NetBuilder();

    void setNetData(NetData netdata) { netData = netdata; };
    void getNetData();

    void expandAssemblies();
    vector<string> getAndSortAssemblies();
    void populateAssemblyWithReference(string fromname, string toname);
    vector<string> getNumAssemblyInputs(int aindex);
    vector<string> getNumAssemblyOutputs(int aindex);
    vector<assembly_input_connections> getAssemblyInputConnections(string aname, vector<string> inputs);
    vector<assembly_output_connections> getAssemblyOutputConnections(string aname, vector<string> outputs);

    int getLastNodeIndex();
    int getLastConvIndex();
    int getLastAttentionIndex();
    int getLastNetClusterIndex();
    int getLastDropoutIndex();
    int getLastConnectIndex();
    int getAssemblyIndex(string aname);
    int getNoSubAssemblyIndex(string aname);

    int getOutputImageDataSize(string id);
    imageData getOutputImageData(string id, double* output_image_pointer);

    bool checkAndGetImageOutputLayers();
    bool checkAndAllocateTrainImageMemory();
    bool checkAndAllocateTrainSoundMemory();

    bool checkAndAllocateSeriesMemory();
    bool checkAndAllocateImageMemory();
    bool checkAndAllocateSoundMemory();

    bool initialize(NetData netdata, int ctype, bool randomize_weights, bool trandomize_data, float urate);

    bool joinHiddenNodesToPrevious();
    bool joinOutputNodesToPrevious();
    bool joinConvolutionsToPrevious();
    bool joinHiddenNetsToPrevious();
    bool joinAttentionsToPrevious();
    bool joinSelfAttentionsToPrevious();
    bool joinClustersToPrevious();
    bool joinDropoutsToPrevious();
    bool joinAdditionsToPrevious();
    bool joinMultiplicationsToPrevious();

    int addNextHiddenNodes(int* hnode_index, vector<int>* hnode_to_net_indices, bool randomize);
    int addNextConvs(int ctype, bool randomize);
    int addNextConvs2(int ctype, bool randomize);
    int addNextHiddenNet(bool randomize);
    int addNextAttentionLayer();
    int addNextSelfAttentionLayer();
    int addNextNetCluster(bool randomize);
    int addNextDropout();
    int addNextAddition();
    int addNextMultiplication();

    void getConvInputSize();
    int getNumInputsFromConvs(int cindex);
    int getNumInputsFromNetData(int nindex);
    int getSelfAttentionOutputSize(int ninputs);
    int getConvIndexFromName(string name);

    void initializeConvImageErrorVariables();
    bool setupConvolutionToImageOutputLayerBackpropPointers(int index);

    string displayInitData(int total_num_connects);
    int areNodesRemainingToProcess();
    int areConvsRemainingToProcess();
    int areHiddenNetsRemainingToProcess();
    int areAttentionsRemainingToProcess();
    int areSelfAttentionsRemainingToProcess();
    int areNetClustersRemainingToProcess();
    int areDropoutsRemainingToProcess();
    int areAdditionsRemainingToProcess();
    int areMultiplicationsRemainingToProcess();
    bool addHiddenNode(int index, int hnode_index, bool randomize);
    bool addOutputNode(int index, int onode_index, bool randomize);
    bool areInputsCompleted(int index);
    bool areConvInputsCompleted(int index);
    bool areOutputsCompleted(int index);
    double* getInputPointer(string input);
    boolIntDoublePointer getImageNodePointerFromInputs(string input);
    boolIntDoublePointer getSoundNodePointerFromInputs(string input);
    boolIntVectorStringDoublePointer isInputProcessNode(string input);
    boolDoublePointer getInputPointerFromInput(string input);
    vector<string> getSortedConvolutions();
    bool getConvGroups();
    int getActiveConvs();
    bool isActiveConvolution(int cindex);

    int getHiddenNets();
    int getAttentionLayers();
    int getSelfAttentionLayers();
    int getNetClusters();
    int getDropouts();
    int getAdditions();
    int getMultiplications();

    int convertConvConnectionToIndex(string name, int hindex);

    int getConvImageMemorySize(string id);
    int getConvBackpropMemorySize();

    bool getConvGray(string id);

    bool softRestart();

    void randomizeNode(string id);
    void randomizeConvLayer(string id);

    int getIndexFromHiddenID(string tid);
    int getIndexFromOutputID(string tid);
    int getConvIndexFromID(string id);

    void clearAndDeallocate();

    double getBias(string lbl);
    double getWeight(string lbl, int input);

    netWeightData getWeights();
    void setWeights(netWeightData nwd);

    double rmsWeights(string lbl);
    double rmsWeights();
    double maxWeights();
    string maxWeightString();

    bool checkValidWeights();

    void setParameters(double sleak, double rleak);
    void setClusterParameter();
    void setErrorFilter(double alphafilter) { errorfilter = alphafilter; };
    void setWaveFileNormalization(bool fileset) {};

    int getNumClustersFromID(string id);
    vector<int> getClustersFromID(string id);
    string getClustersErrorString();



    //****************************************************************************
    // netData helper functions

    bool isNumber(char data);
    bool isWordNumber(string word);


    //****************************************************************************
    // netData initialization functions (from CNetInitialization)

    vector<int> inputNodeIndices;
    vector<int> processNodeIndices;
    vector<int> hiddenNodeIndices;
    vector<int> outputNodeIndices;
    vector<int> connectIndices;
    vector<int> convLayerIndices;

    vector<int> convertHiddenToAll;
    vector<int> convertOutputToAll;

    vector<double> tweights;
    vector<string> tinputNodeIDs;
    vector<string> tinputConvIDs;
    vector<string> tprecedingConvIDs;
    vector<string> tfollowingConvIDs;

    vector<vector<int>> immediateSupportingNodes;
    vector<vector<int>> immediateFollowingNodes;
    vector<vector<int>> immediateSupportingConvs;
    vector<vector<int>> immediateFollowingConvs;

    void netDataInitializations();
    void getNetConnections();
    nodeStructure getNodeIndices();
    vector<string> getInputsFromNetData(int index);
    vector<inputData> getNetInputs();
    void preProcessWaveParameters();
    vector<string> getConvInputs();
    vector<string> getConvOrder();
    int getNumOutputs(int index);
    vector<conv> getConvs();
    int getNumberOfSelftAttentionLayerInputs(int index);


    bool inInputNodes(string minput);
    bool inPreceedingConvs(string mname);
    bool inFollowingConvs(string mname);
    bool addToInputs(string minput);
    bool addToOutputs(string moutput);
    bool addIndexToInput(string mname, int index);

    bool isInArray(string test, vector<string> marray);
    void addFieldToInput(int dataindex, int inputindex);
    void addFftFieldToInput(int dataindex, int inputindex, int fftindex);
    void addProcessToInput(int dataindex, int inputindex);
    void addFieldToOutput(int dataindex, int inputindex);


    int myStoi(string s);

    double processData(int dindex, double pdata);

    vector<string> getInputDataLabels();
    vector<doubleKvPair> testForward(vector<doubleKvPair> inputs, bool use_previous_align);

    int getDataPathType();
    int getDataPathType(vector<string> mpaths);
    int getCombinedIndex(string fname);
    string getDataPathName(int index);
    string getValidationPathName(int index);




};








