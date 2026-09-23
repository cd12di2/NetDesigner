

//#define GCC


#include "pch.h"
#include "MainFrm.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CStatus2.h"
#include "CDataOperations.h"
#include <ppl.h>
#include "CNetData.h"
#include "Neuron.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;
#ifndef GCC
using namespace concurrency;
#endif



//******************************************************************************************************

void OutputNode::clear() {
    allocated = false;
    weights_archived = false;
    bool trainable = true;
    bool zeroout = false;
    bool inputoverrunmsg = false;
    id = "";
    type = SIGMOID;
    num_allocated_inputs = DEFAULT_NUM_BUFFER_INPUTS;
    num_allocated_backprop_inputs = DEFAULT_NUM_BUFFER_INPUTS;
    num_inputs = 0;
    num_backprop_inputs = 0;
    backprop_set_index = 0;
    maxpool_index = -1;
    alpha = 0.2;
    eta = 0.02;
    random_scale = 0.2;
    bias = 0.0;
    archive_bias = 0.0;
    sum = 0.0;
    error = 0.0;
    wrms = 0.0;
    zero = 0.0;
    dummy = 0.0;
    sigmoid_leakage = 0.01;
    relu_leakage = 0.001;
    softmax_error = 0.0;
    sum_add_limit = 0.10;
    weights = NULL;
    archive_weights = NULL;
    deltaweights = NULL;
    backproperrors = NULL;
    inputpointers = NULL;
    backupinputpointers = NULL;
    backpropinputpointers = NULL;
    inputpointernames = NULL;
    testoutputpointer = NULL;
    backprop = 0.0;
    output = 0.0;
    dmsg = "";
    dmsg_sub = "";
}

void OutputNode::init(int ninputs, int nbinputs, int ntype, double neta, double nalpha, double nrandom_scale) {
    allocate(ninputs, nbinputs, ntype, neta, nalpha, nrandom_scale);
}

void OutputNode::allocate(int ninputs, int nbinputs, int ntype, double neta, double nalpha, double nrandom_scale) {
    deallocate();
    num_allocated_inputs = DEFAULT_NUM_BUFFER_INPUTS + ninputs;
    num_allocated_backprop_inputs = DEFAULT_NUM_BUFFER_INPUTS + nbinputs;
    num_inputs = 0;// ninputs;
    num_backprop_inputs = 0;// nbinputs;
    type = ntype;
    eta = neta;
    alpha = nalpha;
    random_scale = nrandom_scale;
    weights = new double[num_allocated_inputs];
    archive_weights = new double[num_allocated_inputs];
    deltaweights = new double[num_allocated_inputs];
    backproperrors = new double[num_allocated_inputs];
    inputpointers = new int64_t[num_allocated_inputs];
    backupinputpointers = new int64_t[num_allocated_inputs];
    inputpointernames = new string[num_allocated_inputs];
    backpropinputpointers = new int64_t[num_allocated_backprop_inputs];
    backpropinputpointernames = new string[num_allocated_backprop_inputs];
    for (int i = 0; i < num_allocated_inputs; i++) {
        weights[i] = 0.0;
        archive_weights[i] = 0.0;
        deltaweights[i] = 0.0;
        backproperrors[i] = 0.0;
        inputpointers[i] = 0;
        backupinputpointers[i] = 0;
        inputpointernames[i] = "";
    }
    for (int i = 0; i < num_allocated_backprop_inputs; i++) {
        backpropinputpointers[i] = (int64_t)&dummy;
        backpropinputpointernames[i] = "";
    }
    backprop = 0.0;
    output = 0.0;
    clearWeights();
    randomizeWeights();
    allocated = true;
}

void OutputNode::deallocate() {
    if (allocated) {
        delete[] weights;
        delete[] archive_weights;
        delete[] deltaweights;
        delete[] backproperrors;
        delete[] inputpointers;
        delete[] backupinputpointers;
        delete[] inputpointernames;
        delete[] backpropinputpointers;
        delete[] backpropinputpointernames;
        allocated = false;
    }
}

bool OutputNode::addInputPointer(string name, double* tinput) {
    if (allocated) {
        if (num_inputs < num_allocated_inputs) {
            inputpointers[num_inputs] = (int64_t)tinput;
            backupinputpointers[num_inputs] = (int64_t)tinput;
            inputpointernames[num_inputs] = name;
            num_inputs++;
            //char s[200];  sprintf_s(s, " (%i) : ", num_allocated_inputs); addErrorString("OutputNode::addInputPointer for " + id + " to " + name + s);
            return(true);
        }
        char s[200];  sprintf_s(s, " : num_inputs (%i) >= num_allocated_inputs (%i) : ", num_inputs, num_allocated_inputs);
        addErrorString("OutputNode::addInputPointer for " + id + s + name);
    }
    else {
        addErrorString("OutputNode::addInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

string OutputNode::getInputPointerName(int index)
{
    if ((index >= 0) && (index < num_inputs)) {
        return(inputpointernames[index]);
    }
    return("");
}

int OutputNode::getInputIndexFromName(string name)
{
    int rval = -1;
    for (int i = 0; i < num_inputs; i++) {
        if (name == inputpointernames[i]) {
            return(i);
        }
    }
    return(rval);
}

void OutputNode::setTestOutputPointer(double* toutput) {
    testoutputpointer = toutput;
}

double OutputNode::getOutput()
{
    return(output);
}

double* OutputNode::getOutputPointer()
{
    return(&output);
}

void OutputNode::setOutput(double out)
{
    output = out;
}

bool OutputNode::setBackpropInputPointer(double* tprop, string name) {
    if (allocated) {
        if (num_backprop_inputs < num_allocated_backprop_inputs) {
            backpropinputpointers[num_backprop_inputs] = (int64_t)tprop;
            backpropinputpointernames[num_backprop_inputs] = name;
            num_backprop_inputs++;
            return(true);
        }
        char s[200];  sprintf_s(s, " (%i) : ", num_allocated_backprop_inputs);
        addErrorString("OutputNode::setBackpropInputPointer for " + id + " : index > num_allocated_backprop_inputs" + s + name);
    }
    else {
        addErrorString("OutputNode::setBackpropInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* OutputNode::getBackpropOutputPointer(string name) {
    if (allocated) {
        //char s[200];  sprintf_s(s, " : backprop_set_index = %i : num_inputs = %i", backprop_set_index, num_inputs);
        //addErrorString("OutputNode::getBackpropOutputPointer : " + name + " : " + inputpointernames[backprop_set_index] + s);
        if (backprop_set_index < num_inputs) {
            if (name == inputpointernames[backprop_set_index]) {
                double* rval = &backproperrors[backprop_set_index];
                backprop_set_index++;
                if (backprop_set_index > num_inputs) {
                    backprop_set_index = 0;
                }
                return(rval);
            }
        }
        for (int i = 0; i < num_inputs; i++) {
            if (name == inputpointernames[i]) {
                backprop_set_index = i + 1;
                if (backprop_set_index > num_inputs) {
                    backprop_set_index = 0;
                }
                return(&backproperrors[i]);
            }
        }
        addErrorString("OutputNode::getBackpropOutputPointer for " + id + " : unable to find " + name);
    }
    else {
        addErrorString("OutputNode::getBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

// ****************************************************************************
// ****************************************************************************

bool OutputNode::join(OutputNode* onode)
{
    // set onode input pointer
    if (!addInputPointer(onode->id, &onode->output)) {
        addErrorString("Node::join for " + id + " for input node " + onode->id + " unable to set input pointer");
        return(false);
    }
    // set self backprop input
    if (!onode->setBackpropInputPointer(getBackpropOutputPointer(onode->id), id)) {
        addErrorString("Node::join for " + id + " for input node " + onode->id + " unable to set backprop pointer");
        return(false);
    }
    return(true);
}

bool OutputNode::join(HiddenNode* hnode)
{
    // set hnode input pointer
    if (!addInputPointer(hnode->id, &hnode->output)) {
        addErrorString("Node::join for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
        return(false);
    }
    // set self backprop inputs
    if (!hnode->setBackpropInputPointer(getBackpropOutputPointer(hnode->id), id)) {
        addErrorString("Node::join for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
        return(false);
    }
    return(true);
}

bool OutputNode::join(HiddenConvolution* conv)
{
    if (conv->allocated && (conv->output_data_size > 0)) {
        boolInt tval = conv->addNewBackpropConnection(id);
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < conv->output_data_size; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(conv->id + s, conv->getOutputPointer(i))) {
                    addErrorString("Node::join for " + id + " for convolution " + conv->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(conv->id + s);
                if (!conv->setBackpropInputPointer2(tval.value, pointer)) {  
                    addErrorString("Node::join for " + id + " for conv " + conv->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("Node::join for " + id + " for conv " + conv->id + " problem with conv backprop index");
        }
    }
    return(true);
}

bool OutputNode::join(HiddenNet* hnet)
{
    if (hnet->num_nodes_layer > 0) {
        for (int i = 0; i < hnet->num_nodes_layer; i++) {
            // set hnet input pointers
            char s[100];  sprintf_s(s, ":%i", i);
            if (!addInputPointer(hnet->id + s, hnet->getOutputPointer(i))) {
                addErrorString("Node::join for " + id + " for hidden net " + hnet->id + " unable to set input pointers");
                return(false);
            }
            // set self backprop inputs
            double* pointer = getBackpropOutputPointer(hnet->id + s);
            if (!hnet->setBackpropInputPointer(i, pointer)) {
                addErrorString("Node::join for " + id + " for hidden net " + hnet->id + " unable to set backprop pointer");
                return(false);
            }
        }
    }
    else {
        addErrorString("Node::join for " + id + " for hidden net " + hnet->id + " - no hnet outputs");
        return(false);
    }
    return(true);
}

bool OutputNode::join(NetCluster* ncluster)
{
    if (ncluster->num_outputs) {
        // set cluster input pointers
        for (int i = 0; i < ncluster->num_outputs; i++) {
            char s[100];  sprintf_s(s, ":%i", i);
            if (!addInputPointer(ncluster->id + s, ncluster->getOutputPointer(i))) {
                addErrorString("Node::join for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
                return(false);
            }
        }
        // don't self backprop inputs for cluster
    }
    else {
        addErrorString("Node::join for " + id + " for cluster " + ncluster->id + " - no cluster outputs");
        return(false);
    }
    return(true);
}

bool OutputNode::join(Attention* attention)
{
    if (attention->output_size > 0) {
        boolInt tval = attention->addNewBackpropConnection(id);
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < attention->output_size; i++) {
                // set attention input pointers
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(attention->id + s, attention->getOutputPointer(i))) {
                    addErrorString("Node::join for " + id + " for attention " + attention->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(attention->id + s);
                if (!attention->setBackpropInputPointer(tval.value, pointer)) {  
                    addErrorString("Node::join for " + id + " for attention " + attention->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("Node::join for " + id + " for conv " + attention->id + " problem with attention backprop index");
        }
    }
    return(true);
}

bool OutputNode::join(SelfAttention* selfattentions)
{
    return(join((Attention*)selfattentions));
}

bool OutputNode::join(Dropout* dropout)
{
    if (dropout->allocated && (dropout->num_inputs > 0)) {
        boolInt tval = dropout->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < dropout->num_inputs; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(dropout->id + s, dropout->getOutputPointer(i))) {
                    addErrorString("Node::join for " + id + " for dropout " + dropout->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(dropout->id + s);
                if (!dropout->setBackpropInputPointer(tval.value, pointer)) {  
                    addErrorString("Node::join for " + id + " for dropout " + dropout->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("Node::join for " + id + " for dropout " + dropout->id + " problem with dropout backprop index");
        }
    }
    return(true);
}

bool OutputNode::join(Addition* addition)
{
    if (addition->allocated && (addition->model_dimension > 0)) {
        boolInt tval = addition->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < addition->model_dimension; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(addition->id + s, addition->getOutputPointer(i))) {
                    addErrorString("Node::join for " + id + " for addition " + addition->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(addition->id + s);
                if (!addition->setBackpropInputPointer(tval.value, pointer)) {
                    addErrorString("Node::join for " + id + " for addition " + addition->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("Node::join for " + id + " for addition " + addition->id + " problem with addition backprop index");
        }
    }
    return(true);
}

bool OutputNode::join(Multiplication* multiplication)
{
    if (multiplication->allocated && (multiplication->model_dimension > 0)) {
        boolInt tval = multiplication->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < multiplication->model_dimension; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(multiplication->id + s, multiplication->getOutputPointer(i))) {
                    addErrorString("Node::join for " + id + " for multiplication " + multiplication->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(multiplication->id + s);
                if (!multiplication->setBackpropInputPointer(tval.value, pointer)) {
                    addErrorString("Node::join for " + id + " for multiplication " + multiplication->id + " unable to set backprop pointer");
                    return(false);
                }
                //char s2[100]; sprintf_s(s2, " : tval.value = %i", tval.value); addErrorString("OutputNode::join : " + id + " to " + multiplication->id + s + s2 + "");
            }
            return(true);
        }
        else {
            addErrorString("Node::join for " + id + " for multiplication " + multiplication->id + " problem with multiplication backprop index");
        }
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void OutputNode::clearWeights() {
    for (int j = 0; j < num_inputs; j++) {
        weights[j] = 0.0;
        archive_weights[j] = 0.0;
        deltaweights[j] = 0.0;
        backproperrors[j] = 0.0;
    }
    backprop = 0.0;
    output = 0.0;
    error = 0.0;
    bias = 0.0;
    archive_bias = 0.0;
    sum = 0.0;
    weights_archived = false;
}

void OutputNode::randomizeWeights() {
    if ((type == SIGMOID) || (type == TANH) || (type == SOFTMAX) || (type == GAUSSIAN)) {
        double scale = random_scale;
        bias = scale * ((2.0 * (double)rand() / RAND_MAX) - 1.0);
        if (num_inputs > 0) {
            scale = random_scale * sqrt(1.0/(double)num_inputs);
        }
        for (long j = 0; j < num_inputs; j++) {
            weights[j] = scale * ((2.0 * (double)rand() / RAND_MAX) - 1.0);
        }
    }
    else if ((type == RELU) || (type == SHOCKLEY)) {
        double scale = random_scale;
        bias = scale * ((2.0 * (double)rand() / RAND_MAX) - 1.0);
        if (num_inputs > 0) {
            scale = random_scale * sqrt(2.0/(double)num_inputs);
        }
        for (long j = 0; j < num_inputs; j++) {
            weights[j] = scale * ((1.0 * (double)rand() / RAND_MAX) - 0.0);
        }
    }
}

bool OutputNode::checkValidWeights()
{
    if (bias != 0.0) {
        return(true);
    }
    for (int i = 0; i < num_inputs; i++) {
        if (weights[i] != 0.0) {
            return(true);
        }
    }
    return(false);
}

void OutputNode::forward() {
    if (zeroout) {
        output = 0.0;
    }
    else {
        string s2 = "";
        sum = bias;
        //char s[200];  sprintf_s(s, " : bias = %.3f : ", bias); s2 += s;
        for (int j = 0; j < num_inputs; j++) {
            sum += (*(double*)inputpointers[j] * weights[j]);
            //sprintf_s(s, " : input = %.3f : ", (*(double*)inputpointers[j])); s2 += s;
        }
        output = activationFunction(sum);
        //addErrorString(id + s2);
    }
}

double OutputNode::getError()
{
    //char s[200];  sprintf_s(s, "getError : output = %.6f : testoutput = %6f : ", output, *(double*)testoutputpointer);  addTrainString(s + id);
    switch (type) {
        case SIGMOID:
        {
            error = (*(double*)testoutputpointer - output) * sigmoidDerivative(output);
            break;
        }
        case RELU:
        {
            error = (*(double*)testoutputpointer - output) * reluDerivative(output);
            break;
        }
        case SHOCKLEY:
        {
            error = (*(double*)testoutputpointer - output) * shockleyDerivative(output);
            break;
        }
        case TANH:
        {
            error = (*(double*)testoutputpointer - output) * mtanhDerivative(output);
            break;
        }
        case SOFTMAX:
        {
            error = softmax_error;
            break;
        }
        case GAUSSIAN:
        {
            error = (*(double*)testoutputpointer - output) * gaussianDerivative(output);
            break;
        }
        case MAXPOOL:
        {
            error = (*(double*)testoutputpointer - output);
            break;
        }
        case MULTIPLY:
        {
            error = (*(double*)testoutputpointer - output);
            break;
        }
        case ADD:
        {
            error = (*(double*)testoutputpointer - output);
            break;
        }
    }
    return(error);
}

void OutputNode::backward()
{
    if (zeroout) {
        for (int j = 0; j < num_inputs; j++) {
            backproperrors[j] = 0.0;
        }
    }
    else {
        getError();
        if (type == MAXPOOL) {
            for (int j = 0; j < num_inputs; j++) {
                if (maxpool_index == j) {
                    backproperrors[j] = error;
                }
                else {
                    backproperrors[j] = 0.0;
                }
            }
        }
        else if (type == MULTIPLY) {
            for (int j = 0; j < num_inputs; j++) {
                double val = 1.0;
                for (int k = 0; k < num_inputs; k++) {
                    if (k != j) {
                        val *= (*(double*)inputpointers[k]);
                    }
                }
                if (val != 0.0) {
                    val = 1 / val;
                    if (val > sum_add_limit) {
                        val = sum_add_limit;
                    }
                    else if (val < -sum_add_limit) {
                        val = -sum_add_limit;
                    }
                    backproperrors[j] = val * error;
                }
                else {
                    backproperrors[j] = 0.0;
                }
            }
        }
        else if (type == ADD) {
            for (int j = 0; j < num_inputs; j++) {
                backproperrors[j] = error;  // 1.0 times error
            }
        }
        else {
            if (trainable) {
                bias += (0.1 * eta * error);
                for (int j = 0; j < num_inputs; j++) {
                    deltaweights[j] = (alpha * deltaweights[j]) + (eta * error * (*(double*)inputpointers[j]));
                    weights[j] += deltaweights[j];
                }
                for (int j = 0; j < num_inputs; j++) {
                    backproperrors[j] = (error * weights[j]);
                }
            }
        }
    }
}

double OutputNode::getBackpropSum()
{
    backprop = 0.0;
    for (int i = 0; i < num_backprop_inputs; i++) {
        backprop += *(double*)backpropinputpointers[i];
    }
    return(backprop);
}

void OutputNode::zeroBackpropErrors()
{
    for (int j = 0; j < num_inputs; j++) {
        backproperrors[j] = 0.0;
    }
}

void OutputNode::setBackpropError(int index, double berror)
{
    if (index < num_inputs) {
        backproperrors[index] = berror;
    }
}

double OutputNode::getBackpropError(int index)
{
    if (index < num_inputs) {
        return(backproperrors[index]);
    }
    return(0.0);
}

double OutputNode::activationFunction(double msum) {
    double rval = 0;
    switch (type) {
        case SIGMOID:
        {
            //addErrorString("activationFunction : sigmoid : " + id);
            rval = sigmoid(msum);
            break;
        }
        case RELU :
        {
            //addErrorString("activationFunction : relu : " + id);
            rval = relu(msum);
            break;
        }
        case SHOCKLEY:
        {
            //addErrorString("activationFunction : shockley : " + id);
            rval = shockley(msum);
            break;
        }
        case TANH:
        {
            //addErrorString("activationFunction : tanh : " + id);
            rval = mtanh(msum);
            break;
        }
        case SOFTMAX:
        {
            //addErrorString("activationFunction : softmax : " + id);
            rval = softmax(msum);
            break;
        }
        case GAUSSIAN:
        {
            //addErrorString("activationFunction : gaussian : " + id);
            rval = gaussian(msum);
            break;
        }
        case MAXPOOL:
        {
            //addErrorString("activationFunction : maxpool : " + id);
            maxpool_index = 0;
            rval = *(double*)inputpointers[0];
            for (int j = 1; j < num_inputs; j++) {
                if (*(double*)inputpointers[j] > rval) {
                    maxpool_index = j;
                    rval = *(double*)inputpointers[j];
                }
            }
            break;
        }
        case MULTIPLY:
        {
            //addErrorString("activationFunction : multiply : " + id);
            rval = 1;
            for (int j = 0; j < num_inputs; j++) {
                rval = rval * *(double*)inputpointers[j];
            }
            break;
        }
        case ADD:
        {
            //addErrorString("activationFunction : add : " + id);
            rval = 0;
            for (int j = 0; j < num_inputs; j++) {
                rval = rval + *(double*)inputpointers[j];
            }
            break;
        }
        default:
        {
            addErrorString(id + " : invalid activation function");
        }
    }
    return(rval);
}

double OutputNode::sigmoid(double data)
{
    return(sigmoid_leakage + (1.0 - (2.0 * sigmoid_leakage)) * (1.0 / (1.0 + exp(-1.0 * data))));
}

double OutputNode::sigmoidDerivative(double data)
{
    return(data * (1 - data));
}

double OutputNode::relu(double data)
{
    if (data < 0.0) {
        return(data * relu_leakage);
    }
    return(data);
}

double OutputNode::reluDerivative(double data)
{
    if (data < 0) {
        return(relu_leakage);
    }
    return(1.0);
}

double OutputNode::mtanh(double data)
{
    return((1.0 - sigmoid_leakage) * ((2.0 / (1.0 + exp(-2.0 * data))) - 1));
}

double OutputNode::mtanh2(double data)
{
    return((exp(data) - exp(-data)) / (exp(data) + exp(-data)));
}

double OutputNode::mtanhDerivative(double data)
{
    return(1 - (data * data));
}

double OutputNode::shockley(double data)
{
    if (data != 0) {
        return(data / (1 + data / (exp(data) - 1)));
    }
    return(0);
}

double OutputNode::shockleyDerivative(double data)
{
    if (data != 0) {
        double mexp = exp(data);
        double mexpm = mexp - 1;
        double mexpm2 = mexpm * mexpm;
        double derivative = ((1 / (data * data)) + (mexp / mexpm2)) / pow(((1 / data) + (1 / mexpm)), 2.0);
        return(derivative);
    }
    return(0);
}

double OutputNode::softmax(double data)
{
    return(exp(data));
}

double OutputNode::softmaxDerivative(double data)
{
    if (data > (1.0 - sigmoid_leakage)) {
        return(sigmoid_leakage);
    }
    else if (data < sigmoid_leakage) {
        return(sigmoid_leakage);
    }
    return(data * (1 - data));
}

double OutputNode::gaussian(double data)
{
    return(exp(-pow(data, 2)));
}

double OutputNode::gaussianDerivative(double data)
{
    return(-2 * data * exp(-pow(data, 2)));
}

double OutputNode::checkError(double toutput) {
    forward();
    double merror = (toutput - output);
    if (merror < 0) {
        merror = -1 * merror;
    }
    return(merror);
}

nodeWeightData OutputNode::getWeights()
{
    nodeWeightData rval;
    rval.id = id;
    if (!isnan(bias) && !isinf(bias)) {
        rval.bias = bias;
    }
    else {
        rval.bias = 0.0;
    }
    for (int i = 0; i < num_inputs; i++) {
        weightData wd;
        wd.label = inputpointernames[i];
        if (!isnan(weights[i]) && !isinf(weights[i])) {
            wd.weight = weights[i];
        }
        else {
            wd.weight = 0.0;
        }
        rval.weightdata.push_back(wd);
    }
    return(rval);
}

void OutputNode::setWeights(nodeWeightData nwd)
{
    if (id == nwd.id) {
        bias = nwd.bias;
        int i = 0;
        bool done = false;
        bool name_search = false;
        while (!done) {
            if ((i < num_inputs) && (i < (int)nwd.weightdata.size())) {
                if (nwd.weightdata[i].label == inputpointernames[i]) {
                    weights[i] = nwd.weightdata[i].weight;
                }
                else {
                    i = num_inputs;
                    name_search = true;
                    done = true;
                }
                i++;
            }
            else {
                done = true;
            }
        }
        if (name_search) {
            for (i=0; i < (int)nwd.weightdata.size(); i++) {
                for (int j = 0; j < num_inputs; j++) {
                    if (nwd.weightdata[i].label == inputpointernames[j]) {
                        weights[j] = nwd.weightdata[i].weight;
                        j = num_inputs;
                    }
                }
            }
        }
    }
}

void OutputNode::archiveWeights()
{
    if (!isnan(bias) && !isinf(bias)) {
        archive_bias = bias;
    }
    else {
        archive_bias = 0.0;
    }
    for (int i = 0; i < num_inputs; i++) {
        if (!isnan(weights[i]) && !isinf(weights[i])) {
            archive_weights[i] = weights[i];
        }
        else {
            archive_weights[i] = 0.0;
        }
    }
    weights_archived = true;
}

void OutputNode::restoreWeights()
{
    if (weights_archived) {
        bias = archive_bias;
        for (int i = 0; i < num_inputs; i++) {
            weights[i] = archive_weights[i];
        }
    }
}

void OutputNode::rmsWeights() {
    int cnt = 0;
    double sum2 = 0;
    for (int j = 0; j < num_inputs; j++) {
        sum2 += (weights[j] * weights[j]);
        cnt++;
    }
    wrms = sqrt(sum2 / (double)cnt);
}

double OutputNode::getRMSWeights() {
    rmsWeights();
    return(wrms);
}

double OutputNode::squareWeights() {
    double msum = (bias * bias);
    for (int j = 0; j < num_inputs; j++) {
        msum += (weights[j] * weights[j]);
    }
    return(msum);
}

double OutputNode::maxWeights() {
    double max = bias;
    if (max < 0) {
        max = -1 * max;
    }
    for (int j = 0; j < num_inputs; j++) {
        if (weights[j] > max) {
            max = weights[j];
        }
        else if (weights[j] < -max) {
            max = -weights[j];
        }
    }
    return(max);
}

void OutputNode::setInputToZero(string inputid)
{
    if (allocated) {
        for (int i = 0; i < num_inputs; i++) {
            if (inputpointernames[i] == inputid) {
                inputpointers[i] = (int64_t)&zero;
                return;
            }
        }
    }
}

void OutputNode::setInputToData(string inputid)
{
    if (allocated) {
        for (int i = 0; i < num_inputs; i++) {
            if (inputpointernames[i] == inputid) {
                inputpointers[i] = backupinputpointers[i];
                return;
            }
        }
    }
}

void OutputNode::setTrainable(bool val)
{
    trainable = val;
};

void OutputNode::setZeroOut(bool val) 
{ 
    zeroout = val; 
};

void OutputNode::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void OutputNode::addErrorString(string err) {
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}



//******************************************************************************************************

double HiddenNode::getError()
{
    getBackpropSum();
    switch (type) {
        case SIGMOID:
        {
            error = backprop * sigmoidDerivative(output); 
            break;
        }
        case RELU:
        {    
            error = backprop * reluDerivative(output);
            break;
        }
        case SHOCKLEY:
        {
            error = backprop * shockleyDerivative(output);
            break;
        }
        case TANH:
        {
            error = backprop * mtanhDerivative(output);
            break;
        }
        case SOFTMAX:
        {
            error = softmax_error;// backprop* softmaxDerivative(output);
            break;
        }
        case GAUSSIAN:
        {
            error = backprop * gaussianDerivative(output);
            break;
        }
        case MAXPOOL:
        {
            error = backprop;
            break;
        }
        case MULTIPLY:
        {
            error = backprop;
            break;
        }
        case ADD:
        {
            error = backprop;
            break;
        }
    }
    return(error);
}

double HiddenNode::getBackprop(int index)
{
    backprop = 0.0;
    if (index < num_backprop_inputs) {
        return(*(double*)backpropinputpointers[index]);
    }
    addErrorString("HiddenNode::getBackprop : index >= num_backprop_inputs");
    return(0.0);
}


//******************************************************************************************************

void HiddenConvolution::clear()
{
    id = "";
    mode = "";
    type = SIGMOID;
    conv_type = CONV_GENERIC;
    layer_on_input = false;
    image_output = false;
    alpha = 0.2;
    eta = 0.02;
    data_type = NONE;
    random_scale = 0.02;
    dummy = 0.5;
    dummy2 = 0.0;
    num_layers = 1;
    num_nodes_per_layer = 8;
    num_hidden_nodes = 0;
    num_weights = 0;
    output_node_offset = 0;
    numdims = 0;
    input_index = 0;
    input_data_size = 0;
    output_data_size = 0;
    num_output_connections = 0;
    output_connection_index = -1;
    conv_input_data_size = 0;
    conv_output_data_size = 0;
    preprocess_data_size = 0;
    backprop_set_index = 0;
    backprop_index = 0;
    num_layer_inputs = 0;
    if (node_indices.size() > 0) {
        for (int i = 0; i < (int)node_indices.size(); i++) {
            node_indices[i].clear();
        }
    }
    node_indices.clear();
}

bool HiddenConvolution::init(string nid, string nmode, int nlayers, int nnodesperlayer, int ntype, double neta, double nalpha, 
                            double nrandom_scale, int ndata_type, bool ntrainable, bool nzerooutput, bool nrandomize, int nnumdims, int* nconvins,
                            int* nconvouts, int* nconvstrideins, int* ninputdims, int ctype, bool nlayer_on_input, int nnum_layer_inputs,
                            int noutconnections)
{
    clear();

    id = nid;
    mode = nmode;
    type = ntype;
    eta = neta;
    alpha = nalpha;
    data_type = ndata_type;
    random_scale = nrandom_scale;

    layer_on_input = nlayer_on_input;
    num_layer_inputs = nnum_layer_inputs;

    num_output_connections = noutconnections;
    //CString s = _T(""); s.Format(_T(" : num_output_connections = %i"), num_output_connections);  AfxMessageBox((CString)id.c_str() + s);

    conv_type = ctype;

    numdims = nnumdims;
    if (numdims < 1) {
        addErrorString("Convolution::init for " + id + " num dimensions must be greater than zero");
        return(false);
    }
    else if (numdims > 6) {
        addErrorString("Convolution::init for " + id + " num dimensions must be less or equal to six");
        return(false);
    }
    if ((numdims != 3) || (conv_type == CONV_GENERIC)) {
        conv_type = CONV_GENERIC;
    }
    else if (nconvins[2] == 1) {
        conv_type = CONV_FULLY;
        addErrorString("Setting gray image convolutions to fully connected");
    }
    else if (conv_type == MAXPOOL) {
        conv_type = CONV_SEPARATED;
    }

    num_layers = nlayers;
    if (num_layers < 1) {
        addErrorString("Convolution::init for " + id + " num_layers must be greater than zero");
        return(false);
    }
    else if (num_layers > 10) {
        addErrorString("Convolution::init for " + id + " num_layers must be less or equal to ten");
        return(false);
    }

    num_nodes_per_layer = nnodesperlayer;
    if (num_nodes_per_layer < 1) {
        addErrorString("Convolution::init for " + id + " nodes per layer must be greater than zero");
        return(false);
    }
    else if (num_nodes_per_layer > 4000) {
        if ((nmode != "Output") || (ndata_type != IMAGE_CONV)) {
            addErrorString("Convolution::init for " + id + " nodes per layer must be less or equal to four thousand");
            return(false);
        }
    }
    if (conv_type == CONV_SEPARATED) {
        if (num_layers > 1) {
            int third = num_nodes_per_layer / 3;
            if ((3 * third) < num_nodes_per_layer) {
                num_nodes_per_layer = 3 * (third + 1);
                char s[200];  sprintf_s(s, " resetting num hidden nodes to %i for convolution with separate color connections", num_nodes_per_layer);
                addErrorString("Init HiddenConvolution for " + id + s);
            }
        }
    }

    if ((nmode == "Output") && (ndata_type == IMAGE_CONV)) {
        image_output = true;
    }

    return(allocate(numdims, nconvins, nconvouts, nconvstrideins, ninputdims, ntrainable, nzerooutput, nrandomize));

}

bool HiddenConvolution::allocate(int nnumdims, int* nconvins, int* nconvouts, int* nconvstrideins, int* ninputdims, bool ntrainable, bool nzerooutput, bool nrandomize)
{
    deallocate();

    convins = new int[numdims];
    convouts = new int[numdims];
    convstrideins = new int[numdims];
    inputdims = new int[numdims];
    outputdims = new int[numdims];
    num_steps = new int[numdims];
    sub_indices = new int[numdims];
    main_indices = new int[numdims];
    convinstrides = new int[numdims];
    convoutstrides = new int[numdims];
    inputdimstrides = new int[numdims];
    inputdimstrides2 = new int[numdims];
    outputdimstrides = new int[numdims];
    if ((convins == 0) || (convouts == 0) || (convstrideins == 0) || (inputdims == 0) || (outputdims == 0) || (num_steps == 0) || (sub_indices == 0) || (main_indices == 0) || (convinstrides == 0) || (convoutstrides == 0) || (inputdimstrides == 0) || (inputdimstrides2 == 0) || (outputdimstrides == 0)) {
        addErrorString("Convolution::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < numdims; i++) {
        convins[i] = nconvins[i];
        //char s[200];  sprintf_s(s, " : i = %i : convins = %i", i, convins[i]);  addErrorString(id + s);
        convouts[i] = nconvouts[i];
        convstrideins[i] = nconvstrideins[i];
        inputdims[i] = ninputdims[i];
        num_steps[i] = 0;
        sub_indices[i] = 0;
        main_indices[i] = 0;
    }

    for (int i = 0; i < numdims; i++) {
        if (convstrideins[i] > 0) {
            if (convstrideins[i] > convins[i]) {
                num_steps[i] = (inputdims[i] - convins[i]) / convstrideins[i];
            }
            else {
                num_steps[i] = (inputdims[i] / convstrideins[i]);
            }
            if (num_steps[i] <= 0) {
                addErrorString("Convolution::init for " + id + " zero steps not allowed");
                return(false);
            }
        }
        else {
            addErrorString("Convolution::init for " + id + " zero stride not allowed");
            return(false);
        }
    }

    for (int i = 0; i < numdims; i++) {
        outputdims[i] = (num_steps[i] * convouts[i]);
    }

    convinstrides[0] = 1;
    convoutstrides[0] = 1;
    inputdimstrides[0] = 1;
    inputdimstrides2[0] = 1;
    outputdimstrides[0] = 1;
    for (int i = 1; i < numdims; i++) {
        convinstrides[i] = convins[i - 1] * convinstrides[i - 1];
        convoutstrides[i] = convouts[i - 1] * convoutstrides[i - 1];
        inputdimstrides[i] = inputdims[i - 1] * inputdimstrides[i - 1];
        inputdimstrides2[i] = (num_steps[i - 1] * convins[i - 1]) * inputdimstrides2[i - 1];
        outputdimstrides[i] = outputdims[i - 1] * outputdimstrides[i - 1];
    }

    input_index = 0;
    input_data_size = 1;
    output_data_size = 1;
    conv_input_data_size = 1;
    conv_output_data_size = 1;
    preprocess_data_size = 1;
    for (int i = 0; i < numdims; i++) {
        input_data_size *= inputdims[i];
        output_data_size *= outputdims[i];
        conv_input_data_size *= convins[i];
        conv_output_data_size *= convouts[i];
        preprocess_data_size *= (num_steps[i] * convins[i]);
        //char s[200];  sprintf_s(s, " : i = %i : convouts = %i", i, convouts[i]); addErrorString(id + s);
    }

    if (conv_type == CONV_SHARED) {
        convins[2] = 1;
        convouts[2] = 1;
        conv_input_data_size = convins[0] * convins[1];
        conv_output_data_size = convouts[0] * convouts[1];
    }

    conv_data = new double[conv_input_data_size];
    conv_backprop_data = new double[conv_output_data_size];
    backprop_output_data = new double[input_data_size];
    inputpointers = new int64_t[input_data_size];
    backupinputpointers = new int64_t[input_data_size];
    inputpointernames = new string[input_data_size];
    output_data = new double[output_data_size];
    backpropdatabuffer = new double[output_data_size];
    if ((conv_data == 0) || (conv_backprop_data == 0) || (output_data == 0) || (backprop_output_data == 0) ||
        (inputpointers == 0) || (backupinputpointers == 0) || (inputpointernames == 0) || (backpropdatabuffer == 0)) {
        addErrorString("Convolution::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < conv_input_data_size; i++) {
        conv_data[i] = 0.5;
    }
    for (int i = 0; i < conv_output_data_size; i++) {
        conv_backprop_data[i] = 0.0;
    }
    dummy2 = 0.0;
    for (int i = 0; i < output_data_size; i++) {
        output_data[i] = 0.5;
        backpropdatabuffer[i] = 0.0;
    }
    dummy = 0.5;
    for (int i = 0; i < input_data_size; i++) {
        backprop_output_data[i] = 0.0;
        inputpointers[i] = (int64_t)&dummy;
        backupinputpointers[i] = (int64_t)&dummy;
        inputpointernames[i] = "";
    }

    backproppointerssize = num_output_connections * output_data_size;
    backpropinputpointers = new int64_t[backproppointerssize];
    if (backpropinputpointers == 0) {
        addErrorString("Convolution::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < backproppointerssize; i++) {
        backpropinputpointers[i] = (int64_t)&dummy2;
    }

    backpropinputindices = new int[num_output_connections];
    backpropinputnames = new string[num_output_connections];
    if ((backpropinputindices == 0) || (backpropinputnames == 0)) {
        addErrorString("Convolution::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < num_output_connections; i++) {
        backpropinputindices[i] = 0;
        backpropinputnames[i] = "";
    }


    int ncnt = 0;
    vector<int> tindices;
    for (int i = 0; i < (num_layers - 1); i++) {
        node_indices.push_back(tindices);
        for (int j = 0; j < num_nodes_per_layer; j++) {
            node_indices[i].push_back(ncnt);
            ncnt++;
        }
    }
    output_node_offset = ncnt;
    node_indices.push_back(tindices);
    for (int j = 0; j < conv_output_data_size; j++) {
        node_indices[(int64_t)node_indices.size() - 1].push_back(ncnt);
        ncnt++;
    }

    if (num_layers != (int)node_indices.size()) {
        addErrorString("Convolution::init for " + id + " num_layers / node_indices.size mismatch");
        return(false);
    }

    num_hidden_nodes = 0;
    for (int i = 0; i < (int)node_indices.size(); i++) {
        num_hidden_nodes += (int)node_indices[i].size();
    }

    if (ncnt != num_hidden_nodes) {
        addErrorString("Convolution::init for " + id + " ncnt / num_hidden_nodes mismatch");
        return(false);
    }

#ifndef GCC
    CAuxFunctions func;
    int new_num_hidden_nodes = func.getNodesPerLayer(func.getIndexFromID(id));
    if ((num_layers * new_num_hidden_nodes) != num_hidden_nodes) {
        char s[200];  sprintf_s(s, " : num_hidden_nodes (%i) not equal to new_num_hidden_nodes (%i)", num_hidden_nodes, (num_layers * new_num_hidden_nodes));
        addErrorString(id + s);
    }
#endif

    hnodes = new HiddenNode[num_hidden_nodes];
    if (hnodes == 0) {
        addErrorString("Convolution::init for " + id + " unable to allocate nodes");
        return(false);
    }

    if (layer_on_input) {
        inlayernodes = new HiddenNode[input_data_size];
        if (inlayernodes == 0) {
            addErrorString("Convolution::init for " + id + " unable to allocate input layer nodes");
            return(false);
        }
    }

    if (!initializeNodes()) {
        return(false);
    }

    randomizeWeights();

    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].setTrainable(ntrainable);
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].setTrainable(ntrainable);
        }
    }

    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].setZeroOut(nzerooutput);
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].setZeroOut(nzerooutput);
        }
    }

    allocated = true;

    return(true);

}

void HiddenConvolution::randomizeWeights()
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].randomizeWeights();
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].randomizeWeights();
        }
    }
}

bool HiddenConvolution::initializeNodes()
{
    for (int i = 0; i < num_layers; i++) {
        for (int j = 0; j < (int)node_indices[i].size(); j++) {
            if (!addHiddenNode(i, j)) {
                return(false);
            }
        }
    }
    for (int i = 0; i < num_layers; i++) {
        for (int j = 0; j < (int)node_indices[i].size(); j++) {
            if (!connectBackpropPointers(i, j)) {
                return(false);
            }
        }
    }
    if (layer_on_input) {
        if (addInputLayer()) {
            if (!connectInputLayerBackpropPointers()) {
                return(false);
            }
        }
        else {
            return(false);
        }
    }
    return(true);
}

bool HiddenConvolution::addHiddenNode(int lindex, int nindex)   
{
    // Get node index
    int index = 0;
    if ((lindex > 0) && (lindex < (int)node_indices.size())) {
        for (int i = 0; i < lindex; i++) {
            index += (int)node_indices[i].size();
        }
    }
    index += nindex;
    // Check if index within bounds
    if ((index < num_hidden_nodes) && (lindex < num_layers)) {
        // Allocate node to net
        int num_inputs = conv_input_data_size;
        if (lindex > 0) {
            num_inputs = num_nodes_per_layer;
        }
        int num_outputs = num_nodes_per_layer;
        if (lindex == (num_layers - 2)) {
            num_outputs = conv_output_data_size;
        }
        else if (lindex == (num_layers - 1)) {
            num_outputs = 1;
        }
        if (conv_type == CONV_SEPARATED) {
            int third = num_inputs / 3;
            if ((3 * third) != num_inputs) {
                char s[200];  sprintf_s(s, " num_inputs (%i) not increment of three for separated color convolution", num_inputs);
                addErrorString("HiddenConvolution addHiddenNode for " + id + s);
                return(false);
            }
            else {
                num_inputs = third;
            }
            if (lindex < (num_layers - 1)) {
                third = num_outputs / 3;
                if ((3 * third) != num_outputs) {
                    char s[200];  sprintf_s(s, " num_outputs (%i) not increment of three for separated color convolution", num_outputs);
                    addErrorString("HiddenConvolution addHiddenNode for " + id + s);
                    return(false);
                }
                else {
                    num_outputs = third;
                }
            }
        }
        hnodes[index].allocate(num_inputs, num_outputs, type, eta, alpha, random_scale);
        char s[200];  sprintf_s(s, "Layer%iNode%i", lindex, nindex);
        hnodes[index].id = s; 
        // Set pointers to input data
        string input = "";
        if (lindex == 0) {
            if (conv_type == CONV_SEPARATED) {
                if (num_inputs != (getIndicesSize(convins)) / 3) {
                    char s[200];  sprintf_s(s, "addHiddenNode : input size mismatch : num_inputs = %i : indicesSize = %i", num_inputs, (getIndicesSize(convins) / 3));
                    addErrorString(s);
                    return(false);
                } 
                else if ((int)node_indices[lindex].size() < 1) {
                    char s[200];  sprintf_s(s, "addHiddenNode : incorrect number of nodes for layer %i : num nodes = %i", lindex, (int)node_indices[lindex].size());
                    addErrorString(s);
                    return(false);
                }
                int c = (3 * nindex) / (int)node_indices[lindex].size();
                int cindex1 = (c * convins[0] * convins[1]);
                for (int q = 0; q < convins[1]; q++) {
                    for (int p = 0; p < convins[0]; p++) {
                        int sub_index = p + (q * convinstrides[1]);
                        if (sub_index >= num_inputs) {
                            char s[200];  sprintf_s(s, "addHiddenNode : sub index out of bounds : %i : %i", sub_index, num_inputs);
                            addErrorString(s);
                            return(false);
                        }
                        else if ((sub_index + cindex1) >= conv_input_data_size) {
                            char s[200];  sprintf_s(s, "addHiddenNode : sub index out of bounds (2) : %i : %i", (sub_index + cindex1), conv_input_data_size);
                            addErrorString(s);
                            return(false);
                        }
                        char input[200];  sprintf_s(input, "ConvInput:%i:%i:%i", lindex, sub_index, c);
                        double* pointer = conv_data + sub_index + cindex1;
                        hnodes[index].addInputPointer(input, pointer);
                    }
                }
            }
            else {
                sub_indices = clearIndices(sub_indices);
                if (num_inputs != getIndicesSize(convins)) {
                    char s[200];  sprintf_s(s, "addHiddenNode : input size mismatch : num_inputs = %i : indicesSize = %i", num_inputs, getIndicesSize(convins));
                    addErrorString(s);
                    return(false);
                }
                do {
                    int sub_index = getIndicesIndex(sub_indices, convins);
                    if (sub_index >= num_inputs) {
                        char s[200];  sprintf_s(s, "addHiddenNode : sub index out of bounds : %i : %i", sub_index, num_inputs);
                        addErrorString(s);
                        return(false);
                    }
                    input = "ConvInput" + getIndicesLabel(sub_indices);
                    if (sub_index >= conv_input_data_size) {
                        char s[200];  sprintf_s(s, "addHiddenNode : input index out of bounds : %i : %i", sub_index, conv_input_data_size);
                        addErrorString(s);
                        return(false);
                    }
                    double* pointer = conv_data + sub_index;
                    hnodes[index].addInputPointer(input, pointer);
                } while (incrementIndicesVector(sub_indices, convins));
            }
        }
        // Set some extra net parameters
        hnodes[index].bias = 0;
        return(true);
    }
    else {
        char s[200];  sprintf_s(s, "addHiddenNode : indices out of bounds : lindex = %i : nindex = %i : index = %i", lindex, nindex, index);
        addErrorString(s);
        return(false);
    }
}

bool HiddenConvolution::addInputLayer()
{
    if (layer_on_input) {
        int num_inputs = num_layer_inputs;
#ifndef GCC
        CAuxFunctions func;
        vector<string> snodes = func.getImmediateSupportingNodes(id);
        if (num_layer_inputs != (int)snodes.size()) {
            AfxMessageBox(_T("HiddenConvolution::addInputLayer : problem with num_layer_inputs"));
        }
#endif
        if (num_inputs == 0) {
            addErrorString("Convolution " + id + " unable to initialize input layer - no inputs");
            return(false);
        }
        int num_outputs = 1;
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].allocate(num_inputs, num_outputs, type, eta, alpha, random_scale);
            char s[200];  sprintf_s(s, "InputLayerNode%i", i);
            inlayernodes[i].id = s; 
            inlayernodes[i].bias = 0;
        }
    }
    return(true);
}

void HiddenConvolution::setInputToZero(string inputid)
{
    if (allocated) {
        string tname = getStringBeforeColon(inputid);
        if (!layer_on_input) {
            bool found = false;
            for (int i = 0; i < input_data_size; i++) {
                if (tname == getStringBeforeColon(inputpointernames[i])) {
                    inputpointers[i] = (int64_t)&zero;
                    return;
                }
            }
        }
        else {
            addErrorString("HiddenConvolution::setInputToZero unable to process input pointer for " + inputid + " for " + id);
        }
    }
    else {
        addErrorString("HiddenConvolution::setInputToZero for " + id + " : memory allocation error");
    }
}

void HiddenConvolution::setInputToData(string inputid)
{
    if (allocated) {
        string tname = getStringBeforeColon(inputid);
        if (!layer_on_input) {
            bool found = false;
            for (int i = 0; i < input_data_size; i++) {
                if (tname == getStringBeforeColon(inputpointernames[i])) {
                    inputpointers[i] = backupinputpointers[i];
                    return;
                }
            }
        }
        else {
            addErrorString("HiddenConvolution::setInputToData unable to process input pointer for " + inputid + " for " + id);
        }
    }
    else {
        addErrorString("HiddenConvolution::setInputToData for " + id + " : memory allocation error");
    }
}

int HiddenConvolution::getInputLayerNumInputs()
{
    if (layer_on_input) {
        if (input_data_size > 0) {
            return(inlayernodes[0].num_inputs);
        }
    }
    return(0);
}

bool HiddenConvolution::connectBackpropPointers(int lindex, int nindex)
{
    if ((lindex >= 0) && (lindex < num_layers) && (lindex < (int)node_indices.size())) {

        // Get node index
        int index = 0;
        for (int i = 0; i < lindex; i++) {
            index += (int)node_indices[i].size();
        }
        index += nindex;

        if (index < num_hidden_nodes) {
            if (lindex == (num_layers - 1)) {
                if (nindex < conv_output_data_size) {
                    if ((int)node_indices[lindex].size() != conv_output_data_size) {
                        char s[200];  sprintf_s(s, " : %i : %i", (int)node_indices[lindex].size(), conv_output_data_size);
                        addErrorString("connectBackpropPointers for " + hnodes[index].id + " num backprops not equal to nodes" + s);
                        return(false);
                    }
                    char s[100];  sprintf_s(s, ":%i", index);
                    double* pointer = conv_backprop_data + nindex;
                    hnodes[index].setBackpropInputPointer(pointer, id + s);
                }
                else {
                    char s[200];  sprintf_s(s, " : lindex = %i : nindex = %i : conv_output_data_size = %i", lindex, nindex, conv_output_data_size);
                    addErrorString("connectBackpropPointers error for " + id + s);
                    return(false);
                }
            }
            else if (num_layers > 0) { 
                int index2 = 0;
                for (int i = 0; i <= lindex; i++) {
                    index2 += (int)node_indices[i].size();
                }
                if (conv_type == CONV_SEPARATED) {
                    int num_backprop_inputs = (int)node_indices[(int64_t)lindex + 1].size() / 3;
                    int c1 = (3 * nindex) / (int)node_indices[lindex].size();
                    int c2 = (c1 * (int)node_indices[(int64_t)lindex + 1].size()) / 3;
                    int c3 = (c1 * (int)node_indices[lindex].size()) / 3;
                    for (int j = 0; j < num_backprop_inputs; j++) {
                        if (((c2 + j) < (int)node_indices[(int64_t)lindex + 1].size()) && ((nindex - c3) >= 0) && ((nindex - c3) < hnodes[index2 + c2 + j].num_inputs)) {
                            hnodes[index2 + c2 + j].join(&hnodes[index]);
                        }
                        else {
                            char s[200];  sprintf_s(s, "connectBackpropPointers : problem with node_indices[%i].size for j = %i : %i : %i", lindex, (c2 + j), (nindex - c3), hnodes[index2 + c2 + j].num_inputs);
                            addErrorString(s);
                            return(false);
                        }
                    }
                }
                else {
                    for (int j = 0; j < (int)node_indices[(int64_t)lindex + 1].size(); j++) {
                        hnodes[index2 + j].join(&hnodes[index]);
                    }
                }
            }
            else {
                addErrorString("connectBackpropPointers : returning false for " + id + " - no convolutional layers");
            }
        }
        else {
            char s[200];  sprintf_s(s, "connectBackpropPointers : indices out of bounds : lindex = %i : nindex = %i : index = %i : num_hidden_nodes = %i", lindex, nindex, index, num_hidden_nodes);
            addErrorString(s);
            return(false);
        }

    }
    else {
        char s[200];  sprintf_s(s, "connectBackpropPointers : lindex out of bounds : lindex = %i : nindex = %i", lindex, nindex);
        addErrorString(s);
        return(false);
    }

    return(true);

}

bool HiddenConvolution::connectInputLayerBackpropPointers()
{
    if (layer_on_input) {
        if (num_layers > 0) {
            for (int i = 0; i < input_data_size; i++) {
                double* pointer = backprop_output_data + i;
                char s[100];  sprintf_s(s, ":%i", i);
                inlayernodes[i].setBackpropInputPointer(pointer, id + s);
            }
        }
        else {
            addErrorString("connectInputLayerBackpropPointers : returning false for " + id + " - no following convolutional layers");
        }
    }
    return(true);
}

void HiddenConvolution::deallocate()
{
    if (allocated) {

        delete[] convins;
        delete[] convouts;
        delete[] convstrideins;
        delete[] inputdims;
        delete[] outputdims;
        delete[] num_steps;
        delete[] sub_indices;
        delete[] main_indices;

        delete[] convinstrides;
        delete[] convoutstrides;
        delete[] inputdimstrides;
        delete[] inputdimstrides2;
        delete[] outputdimstrides;

        delete[] conv_data;
        delete[] conv_backprop_data;
        delete[] output_data;
        delete[] backprop_output_data;

        delete[] inputpointers;
        delete[] backupinputpointers;
        delete[] inputpointernames;
        delete[] backpropinputpointers;

        delete[] backpropdatabuffer;

        delete[] backpropinputindices;
        delete[] backpropinputnames;

        for (int i = 0; i < num_hidden_nodes; i++) {
            hnodes[i].deallocate();
            hnodes[i].clear();
        }
        delete[] hnodes;

        if (layer_on_input) {
            for (int i = 0; i < input_data_size; i++) {
                inlayernodes[i].deallocate();
                inlayernodes[i].clear();
            }
            delete[] inlayernodes;
        }

        numdims = 0;
        input_index = 0;
        input_data_size = 0;
        num_hidden_nodes = 0;
        output_data_size = 0;
        conv_input_data_size = 0;
        conv_output_data_size = 0;
        output_node_offset = 0;
        num_weights = 0;
        num_layer_inputs = 0;
        backprop_set_index = 0;
        backprop_index = 0;

        layer_on_input = false;
        image_output = false;

        allocated = false;
    }
}

bool HiddenConvolution::setBackpropInputPointer(int index, double* tprop)
{
    if (allocated) {
        if (index < output_data_size) {
            backpropinputpointers[index] = (int64_t)tprop;
            return(true);
        }
        else {
            char s[200];  sprintf_s(s, " : %i : %i", index, output_data_size);
            addErrorString("HiddenConvolution::setBackpropInputPointer for " + id + " : index >= output_data_size" + s);
        }
    }
    return(false);
}

double* HiddenConvolution::getInputLayerBackpropOutputPointers(int node_index, int input_index)
{
    if (layer_on_input) {
        if ((input_data_size > 0) && (node_index < input_data_size)) {
            if (input_index < inlayernodes[node_index].num_inputs) {
                return(inlayernodes[node_index].getBackpropOutputPointer(inlayernodes[node_index].getInputPointerName(input_index)));
            }
            else {
                char s[200];  sprintf_s(s, " input_index (%i) exceeds num_inputs (%i)", input_index, inlayernodes[node_index].num_inputs);
                addErrorString("getInputLayerBackpropOutputPointers for " + id + s);
            }
        }
        else {
            char s[200];  sprintf_s(s, " problem with node indices : node_index = %i : input_data_size = %i", node_index, input_data_size);
            addErrorString("getInputLayerBackpropOutputPointers for " + id + s);
        }
    }
    return(&dummy2);
}

double* HiddenConvolution::getOutputPointer(int index)
{
    if (allocated && (index < output_data_size)) {
        return(&output_data[index]);
    }
    addErrorString("HiddenConvolution::getOutputPointer unable to get output pointer");
    return(&dummy);
}

bool HiddenConvolution::addInputPointer(string name, double* tinput) 
{
    if (allocated) {
        //string tname = getStringBeforeColon(name);
        if (layer_on_input) {
            for (int j = 0; j < input_data_size; j++) {
                inlayernodes[j].addInputPointer(name, tinput);
            }
        }
        else {
            if (input_index < input_data_size) {
                //char s[200];  sprintf_s(s, " : input_index = %i", input_index);  addErrorString("adding input pointer to " + id + " from " + name + s);
                inputpointers[input_index] = (int64_t)tinput;
                backupinputpointers[input_index] = (int64_t)tinput;
                inputpointernames[input_index] = name;
                input_index++;
                return(true);
            }
            char s[200];  sprintf_s(s, " : input_index = %i : input_data_size = %i", input_index, input_data_size);
            addErrorString("HiddenConvolution::addInputPointer for " + id + " from " + name + s);
        }
    }
    else {
        addErrorString("HiddenConvolution::addInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

boolInt HiddenConvolution::addNewBackpropConnection(string name)
{
    boolInt rval;
    rval.valid = false;
    rval.value = -1;
    if (output_connection_index < (num_output_connections - 1)) {
        output_connection_index++;
        backpropinputnames[output_connection_index] = name;
        rval.value = output_connection_index;
        rval.valid = true;
    }
    return(rval);
}

bool HiddenConvolution::setBackpropInputPointer2(int connection_index, double* tprop) {
    if (allocated) {
        if (connection_index < num_output_connections) {
            if (backpropinputindices[connection_index] < output_data_size) {
                int index = (connection_index * output_data_size) + backpropinputindices[connection_index];
                backpropinputpointers[index] = (int64_t)tprop;
                backpropinputindices[connection_index]++;
                return(true);
            }
        }
        char s[200];  sprintf_s(s, " (%i) : ", output_data_size);
        addErrorString("HiddenConvolution::setBackpropInputPointer for " + id + " : index > num_allocated_backprop_inputs" + s);
    }
    else {
        addErrorString("HiddenConvolution::setBackpropInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* HiddenConvolution::getBackpropOutputPointerConvOnInput(string name) {
    if (allocated) {
        if (backprop_set_index < input_data_size) {
            if (name == inputpointernames[backprop_set_index]) {
                double* rval = &backprop_output_data[backprop_set_index];
                backprop_set_index++;
                if (backprop_set_index > num_output_connections) {
                    backprop_set_index = 0;
                }
                return(rval);
            }
        }
        for (int i = 0; i < input_data_size; i++) {
            if (name == inputpointernames[i]) {
                double* rval = &backprop_output_data[i];
                backprop_set_index = i + 1;
                if (backprop_set_index > input_data_size) {
                    backprop_set_index = 0;
                }
                return(rval);
            }
        }
        addErrorString("HiddenConvolution::getBackpropOutputPointer for " + id + " : unable to find " + name + " : example " + inputpointernames[0]);
    }
    else {
        addErrorString("HiddenConvolution::getBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

// ****************************************************************************
// ****************************************************************************

bool HiddenConvolution::join(HiddenNode* hnode)
{
    if (layer_on_input) {
        for (int j = 0; j < input_data_size; j++) {
            inlayernodes[j].join(hnode);
        }
    }
    else {
        // set hnode input pointer
        if (!addInputPointer(hnode->id, &hnode->output)) {
            addErrorString("HiddenConvolution::join for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
            return(false);
        }
        // set self backprop inputs
        if (!hnode->setBackpropInputPointer(getBackpropOutputPointerConvOnInput(hnode->id), id)) {
            addErrorString("HiddenConvolution::join for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
            return(false);
        }
    }
    return(true);
}

bool HiddenConvolution::join(HiddenConvolution* conv)
{
    if (conv->allocated && (input_data_size > 0) && (conv->output_data_size > 0)) {
        if (layer_on_input) {
            for (int j = 0; j < input_data_size; j++) {
                inlayernodes[j].join(conv);
            }
        }
        else {
            if (input_data_size == conv->output_data_size) {
                boolInt tval = conv->addNewBackpropConnection(id);
                if (tval.valid) {
                    char s[100];
                    for (int i = 0; i < conv->output_data_size; i++) {
                        // Set inputs
                        sprintf_s(s, ":%i", i);
                        if (!addInputPointer(conv->id + s, conv->getOutputPointer(i))) {
                            addErrorString("HiddenConvolution::join for " + id + " for convolution " + conv->id + " unable to set input pointers");
                            return(false);
                        }
                        // set self backprop inputs
                        double* pointer = getBackpropOutputPointerConvOnInput(conv->id + s);
                        if (!conv->setBackpropInputPointer2(tval.value, pointer)) {   
                            addErrorString("HiddenConvolution::join for " + id + " for conv " + conv->id + " unable to set backprop pointer");
                            return(false);
                        }
                    }
                    return(true);
                }
                else {
                    addErrorString("HiddenConvolution::join for " + id + " for conv " + conv->id + " problem with conv backprop index");
                }
            }
            else {
                char s[200];  sprintf_s(s, " input_data_size (%i) : output_data_size (%i)", input_data_size, conv->output_data_size);
                addErrorString("HiddenConvolution::join for " + id + " : input_data_size != output_data_size" + s);
            }
        }
    }
    return(true);
}

bool HiddenConvolution::join(HiddenNet* hnet)
{
    if (layer_on_input) {
        for (int i = 0; i < hnet->num_nodes_layer; i++) {
            for (int j = 0; j < input_data_size; j++) {
                inlayernodes[j].join(hnet->hnodes);
            }
        }
    }
    else {
        for (int i = 0; i < hnet->num_nodes_layer; i++) {
            // set hnode input pointer
            if (!addInputPointer(hnet->hnodes[i].id, &hnet->hnodes[i].output)) {
                addErrorString("HiddenConvolution::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set input pointer");
                return(false);
            }
            // set self backprop inputs
            if (!hnet->hnodes[i].setBackpropInputPointer(getBackpropOutputPointerConvOnInput(hnet->hnodes[i].id), id)) {
                addErrorString("HiddenConvolution::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set backprop pointer");
                return(false);
            }
        }
    }
    return(true);
}

bool HiddenConvolution::join(NetCluster* ncluster)
{
    if (ncluster->num_outputs) {
        // set cluster input pointers
        if (layer_on_input) {
            for (int j = 0; j < input_data_size; j++) {
                inlayernodes[j].join(ncluster);
            }
        }
        else {
            for (int i = 0; i < ncluster->num_outputs; i++) {
                char s[100];  sprintf_s(s, ":%i", i);
                if (!addInputPointer(ncluster->id + s, ncluster->getOutputPointer(i))) {
                    addErrorString("HiddenConvolution::join for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
                    return(false);
                }
            }
        }
        // don't self backprop inputs for cluster
    }
    else {
        addErrorString("HiddenConvolution::join for " + id + " for cluster " + ncluster->id + " - no cluster outputs");
        return(false);
    }
    return(true);
}

bool HiddenConvolution::join(Attention* attention)
{
    if (attention->output_size > 0) {
        if (layer_on_input) {
            for (int i = 0; i < attention->output_size; i++) {
                for (int j = 0; j < input_data_size; j++) {
                    inlayernodes[j].join(attention);
                }
            }
        }
        else {
            boolInt tval = attention->addNewBackpropConnection(id);
            if (tval.valid) {
                char s[100];  
                for (int i = 0; i < attention->output_size; i++) {
                    // set attention input pointers
                    sprintf_s(s, ":%i", i);
                    if (!addInputPointer(attention->id + s, attention->getOutputPointer(i))) {
                        addErrorString("HiddenConvolution::join for " + id + " for attention " + attention->id + " unable to set input pointers");
                        return(false);
                    }
                    // set self backprop inputs
                    double* pointer = getBackpropOutputPointerConvOnInput(attention->id + s);
                    if (!attention->setBackpropInputPointer(tval.value, pointer)) {  
                        addErrorString("HiddenConvolution::join for " + id + " for attention " + attention->id + " unable to set backprop pointer");
                        return(false);
                    }
                }
            }
            else {
                addErrorString("HiddenConvolution::join for " + id + " for attention " + attention->id + " problem with backprop index");
            }
        }
    }
    else {
        addErrorString("HiddenConvolution::join for " + id + " for attention " + attention->id + " - no attention outputs");
        return(false);
    }
    return(true);
}

bool HiddenConvolution::join(SelfAttention* selfattentions)
{
    return(join((Attention*)selfattentions));
}

bool HiddenConvolution::join(Dropout* dropout)
{
    if (input_data_size == dropout->num_inputs) {
        boolInt tval = dropout->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < dropout->num_inputs; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(dropout->id + s, dropout->getOutputPointer(i))) {
                    addErrorString("HiddenConvolution::join for " + id + " for dropout " + dropout->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointerConvOnInput(dropout->id + s);
                if (!dropout->setBackpropInputPointer(tval.value, pointer)) {  
                    addErrorString("HiddenConvolution::join for " + id + " for dropout " + dropout->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("HiddenConvolution::join for " + id + " for dropout " + dropout->id + " problem with dropout backprop index");
        }
    }
    else {
        char s[200];  sprintf_s(s, " input_data_size (%i) : num_inputs (%i)", input_data_size, dropout->num_inputs);
        addErrorString("HiddenConvolution::join for " + id + " : input_data_size != output_data_size" + s);
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void HiddenConvolution::forward()
{
    if (conv_type == CONV_GENERIC) {
        forward_generic();
    }
    else if (conv_type == CONV_FULLY) {
        forward_fully();
    }
    else if (conv_type == CONV_SEPARATED) {
        forward_fully();
    }
    else if (conv_type == CONV_SHARED) {
        forward_shared();
    }
}

void HiddenConvolution::backward()
{
    if (conv_type == CONV_GENERIC) {
        backward_generic();
    }
    else if (conv_type == CONV_FULLY) {
        backward_fully();
    }
    else if (conv_type == CONV_SEPARATED) {
        backward_fully();
    }
    else if (conv_type == CONV_SHARED) {
        backward_shared();
    }
}

void HiddenConvolution::forward_generic()
{
    sub_indices = clearIndices(sub_indices);
    main_indices = clearIndices(main_indices);
    if (input_data_size != getIndicesSize(inputdims)) {
        char s[200];  sprintf_s(s, "forward : input_data_size (%i) not equal to getIndicesSize (%i)", input_data_size, getIndicesSize(inputdims));
        addTrainString(s);
        return;
    }
    if (conv_input_data_size != getIndicesSize(convins)) {
        char s[200];  sprintf_s(s, "forward : conv_input_data_size (%i) not equal to getIndicesSize (%i)", conv_input_data_size, getIndicesSize(convins));
        addTrainString(s);
        return;
    }

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].forward();
        }
    }

    int sub_index = 0;
    int main_index = 0;

    do {

        sub_indices = clearIndices(sub_indices);
        do {
            sub_index = getIndicesIndex(sub_indices, convins, convinstrides);
            main_index = getIndicesSubIndex(sub_indices, main_indices, convstrideins, inputdims, inputdimstrides);
            if (sub_index >= conv_input_data_size) {
                char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : conv_input_data_size = %i", sub_index, conv_input_data_size);
                addTrainString(s);
                return;
            }
            if (main_index < input_data_size) {
                if (layer_on_input) {
                    conv_data[sub_index] = inlayernodes[main_index].getOutput();
                }
                else {
                    conv_data[sub_index] = *(double*)inputpointers[main_index];
                }
            }
        } while (incrementIndicesVector(sub_indices, convins));

        for (int i = 0; i < num_hidden_nodes; i++) {
            hnodes[i].forward();
        }

        sub_indices = clearIndices(sub_indices);
        do {
            sub_index = output_node_offset + getIndicesIndex(sub_indices, convouts, convoutstrides);
            main_index = getIndicesSubIndex(sub_indices, main_indices, convouts, outputdims, outputdimstrides);
            if (sub_index >= num_hidden_nodes) {
                char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : num_hidden_nodes = %i", sub_index, num_hidden_nodes);
                addTrainString(s);
                return;
            }
            if (main_index >= output_data_size) {
                char s[200];  sprintf_s(s, "forward : main index out of bounds : main_index = %i : output_data_size = %i", main_index, output_data_size);
                addTrainString(s);
                return;
            }
            output_data[main_index] = hnodes[sub_index].getOutput();
        } while (incrementIndicesVector(sub_indices, convouts));

    } while (incrementIndicesVector(main_indices, num_steps));

}

void HiddenConvolution::backward_generic()
{
    int sub_index = 0;
    int main_index = 0;

    for (int i = 0; i < input_data_size; i++) {
        backprop_output_data[i] = 0.0;
    }

    outputBackpropProcessing();

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].forward();
        }
    }

    do {

        sub_indices = clearIndices(sub_indices);
        do {
            sub_index = getIndicesIndex(sub_indices, convins, convinstrides);
            main_index = getIndicesSubIndex(sub_indices, main_indices, convstrideins, inputdims, inputdimstrides);
            if (sub_index >= conv_input_data_size) {
                char s[200];  sprintf_s(s, "backward : sub index out of bounds : sub_index = %i : conv_input_data_size = %i", sub_index, conv_input_data_size);
                addTrainString(s);
                return;
            }
            if (main_index < input_data_size) {
                if (layer_on_input) {
                    conv_data[sub_index] = inlayernodes[main_index].getOutput();
                }
                else {
                    conv_data[sub_index] = *(double*)inputpointers[main_index];
                }
            }
        } while (incrementIndicesVector(sub_indices, convins));

        for (int i = 0; i < num_hidden_nodes; i++) {
            hnodes[i].forward();
        }

        sub_indices = clearIndices(sub_indices);
        do {
            sub_index = getIndicesIndex(sub_indices, convouts, convoutstrides);
            main_index = getIndicesSubIndex(sub_indices, main_indices, convouts, outputdims, outputdimstrides);
            if (sub_index >= conv_output_data_size) {
                char s[200];  sprintf_s(s, "backward : sub index out of bounds : sub_index = %i : conv_output_data_size = %i", sub_index, conv_output_data_size);
                addTrainString(s);
                return;
            }
            if (main_index >= output_data_size) {
                char s[200];  sprintf_s(s, "backward : main index out of bounds : main_index = %i : output_data_size = %i", main_index, output_data_size);
                addTrainString(s);
                return;
            }
            conv_backprop_data[sub_index] = backpropdatabuffer[main_index];
        } while (incrementIndicesVector(sub_indices, convouts));

        for (int i = (num_hidden_nodes - 1); i >= 0; i--) {
            hnodes[i].backward();
        }

        int num_input_nodes = (int)node_indices[0].size();
        sub_indices = clearIndices(sub_indices);
        do {
            sub_index = getIndicesIndex(sub_indices, convins, convinstrides);
            main_index = getIndicesSubIndex(sub_indices, main_indices, convstrideins, inputdims, inputdimstrides);
            if (sub_index >= conv_input_data_size) {
                char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : conv_input_data_size = %i", sub_index, conv_input_data_size);
                addTrainString(s);
                return;
            }
            if (main_index < input_data_size) {
                for (int i = 0; i < num_input_nodes; i++) {
                    backprop_output_data[main_index] += hnodes[i].backproperrors[sub_index];
                }
            }
        } while (incrementIndicesVector(sub_indices, convins));

    } while (incrementIndicesVector(main_indices, num_steps));

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].backward();
        }
    }

}

void HiddenConvolution::forward_fully()
{
    int mindex1 = 0;
    int mindex2 = 0;
    int nindex1 = 0;
    int nindex2 = 0;
    int cindex1 = 0;
    int cindex2 = 0;
    int qindex1 = 0;
    int qindex2 = 0;
    int sub_index = 0;
    int main_index = 0;
    
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].forward();
        }
    }

    for (int m = 0; m < num_steps[1]; m++) {

        mindex1 = (m * convstrideins[1]);
        mindex2 = (m * convouts[1]);
        for (int n = 0; n < num_steps[0]; n++) {

            nindex1 = (n * convstrideins[0]);
            nindex2 = (n * convouts[0]);
            for (int c = 0; c < convins[2]; c++) {
                cindex1 = (c * convinstrides[2]);
                cindex2 = (c * inputdimstrides[2]);
                for (int q = 0; q < convins[1]; q++) {
                    qindex1 = (q * convinstrides[1]);
                    qindex2 = ((q + mindex1) * inputdimstrides[1]);
                    for (int p = 0; p < convins[0]; p++) {
                        sub_index = p + qindex1 + cindex1;
                        main_index = p + nindex1 + qindex2 + cindex2;
                        if (sub_index >= conv_input_data_size) {
                            char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : conv_input_data_size = %i", sub_index, conv_input_data_size);
                            addErrorString(s);
                            return;
                        }
                        if (main_index < input_data_size) {
                            if (layer_on_input) {
                                conv_data[sub_index] = inlayernodes[main_index].getOutput();
                            }
                            else {
                                conv_data[sub_index] = *(double*)inputpointers[main_index];
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < num_hidden_nodes; i++) {
                hnodes[i].forward();
            }

            for (int c = 0; c < convouts[2]; c++) {
                cindex1 = (c * convoutstrides[2]);
                cindex2 = (c * outputdimstrides[2]);
                for (int q = 0; q < convouts[1]; q++) {
                    qindex1 = (q * convoutstrides[1]);
                    qindex2 = ((q + mindex2) * outputdimstrides[1]);
                    for (int p = 0; p < convouts[0]; p++) {
                        sub_index = p + qindex1 + cindex1 + output_node_offset;
                        main_index = p + nindex2 + qindex2 + cindex2;
                        if (sub_index >= num_hidden_nodes) {
                            char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : num_hidden_nodes = %i", sub_index, num_hidden_nodes);
                            addErrorString(s);
                            return;
                        }
                        if (main_index >= output_data_size) {
                            char s[200];  sprintf_s(s, "forward : main index out of bounds : main_index = %i : output_data_size = %i", main_index, output_data_size);
                            addErrorString(s);
                            return;
                        }
                        output_data[main_index] = hnodes[sub_index].getOutput();
                    }
                }
            }

        }
    }

}

void HiddenConvolution::backward_fully()
{
    int mindex1 = 0;
    int mindex2 = 0;
    int nindex1 = 0;
    int nindex2 = 0;
    int cindex1 = 0;
    int cindex2 = 0;
    int qindex1 = 0;
    int qindex2 = 0;
    int sub_index = 0;
    int main_index = 0;

    for (int i = 0; i < input_data_size; i++) {
        backprop_output_data[i] = 0.0;
    }

    outputBackpropProcessing();

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].forward();
        }
    }

    for (int m = 0; m < num_steps[1]; m++) {

        mindex1 = (m * convstrideins[1]);
        mindex2 = (m * convouts[1]);
        for (int n = 0; n < num_steps[0]; n++) {

            nindex1 = (n * convstrideins[0]);
            nindex2 = (n * convouts[0]);
            for (int c = 0; c < convins[2]; c++) {
                cindex1 = (c * convinstrides[2]);
                cindex2 = (c * inputdimstrides[2]);
                for (int q = 0; q < convins[1]; q++) {
                    qindex1 = (q * convinstrides[1]);
                    qindex2 = ((q + mindex1) * inputdimstrides[1]);
                    for (int p = 0; p < convins[0]; p++) {
                        sub_index = p + qindex1 + cindex1;
                        main_index = p + nindex1 + qindex2 + cindex2;
                        if (sub_index >= conv_input_data_size) {
                            char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : conv_input_data_size = %i", sub_index, conv_input_data_size);
                            addErrorString(s);
                            return;
                        }
                        if (main_index < input_data_size) {
                            if (layer_on_input) {
                                conv_data[sub_index] = inlayernodes[main_index].getOutput();
                            }
                            else {
                                conv_data[sub_index] = *(double*)inputpointers[main_index];
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < num_hidden_nodes; i++) {
                hnodes[i].forward();
            }

            for (int c = 0; c < convouts[2]; c++) {
                cindex1 = (c * convoutstrides[2]);
                cindex2 = (c * outputdimstrides[2]);
                for (int q = 0; q < convouts[1]; q++) {
                    qindex1 = (q * convoutstrides[1]);
                    qindex2 = ((q + mindex2) * outputdimstrides[1]);
                    for (int p = 0; p < convouts[0]; p++) {
                        sub_index = p + qindex1 + cindex1;
                        main_index = p + nindex2 + qindex2 + cindex2;
                        if (sub_index >= conv_output_data_size) {
                            char s[200];  sprintf_s(s, "backward : sub index out of bounds : sub_index = %i : conv_output_data_size = %i", sub_index, conv_output_data_size);
                            addErrorString(s);
                            return;
                        }
                        if (main_index >= output_data_size) {
                            char s[200];  sprintf_s(s, "backward : main index out of bounds : main_index = %i : output_data_size = %i", main_index, output_data_size);
                            addErrorString(s);
                            return;
                        }
                        conv_backprop_data[sub_index] = backpropdatabuffer[main_index];
                    }
                }
            }

            for (int i = (num_hidden_nodes - 1); i >= 0; i--) {
                hnodes[i].backward();
            }

            int num_input_nodes = (int)node_indices[0].size();
            if (conv_type == CONV_SEPARATED) {
                num_input_nodes = num_input_nodes / 3;
            }
            for (int c = 0; c < convins[2]; c++) {
                cindex1 = (c * convinstrides[2]);
                cindex2 = (c * inputdimstrides[2]);
                for (int q = 0; q < convins[1]; q++) {
                    qindex1 = (q * convinstrides[1]);
                    qindex2 = ((q + mindex1) * inputdimstrides[1]);
                    for (int p = 0; p < convins[0]; p++) {
                        sub_index = p + qindex1 + cindex1;
                        main_index = p + nindex1 + qindex2 + cindex2;
                        if (main_index < input_data_size) {
                            if (conv_type == CONV_SEPARATED) {
                                for (int i = 0; i < num_input_nodes; i++) {
                                    backprop_output_data[main_index] += hnodes[i + (c * num_input_nodes)].backproperrors[sub_index - cindex1];
                                }
                            }
                            else {
                                for (int i = 0; i < num_input_nodes; i++) {
                                    backprop_output_data[main_index] += hnodes[i].backproperrors[sub_index];
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].backward();
        }
    }

}

void HiddenConvolution::forward_shared()
{
    int mindex1 = 0;
    int mindex2 = 0;
    int nindex1 = 0;
    int nindex2 = 0;
    int cindex2 = 0;
    int cindex3 = 0;
    int qindex1 = 0;
    int qindex2 = 0;
    int sub_index = 0;
    int main_index = 0;

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].forward();
        }
    }

    for (int c = 0; c < inputdims[2]; c++) {

        cindex2 = (c * inputdimstrides[2]);
        cindex3 = (c * outputdimstrides[2]);
        for (int m = 0; m < num_steps[1]; m++) {

            mindex1 = (m * convstrideins[1]);
            mindex2 = (m * convouts[1]);
            for (int n = 0; n < num_steps[0]; n++) {

                nindex1 = (n * convstrideins[0]);
                nindex2 = (n * convouts[0]);
                for (int q = 0; q < convins[1]; q++) {
                    qindex1 = (q * convinstrides[1]);
                    qindex2 = ((q + mindex1) * inputdimstrides[1]);
                    for (int p = 0; p < convins[0]; p++) {
                        sub_index = p + qindex1;
                        main_index = p + nindex1 + qindex2 + cindex2;
                        if (sub_index >= conv_input_data_size) {
                            char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : conv_input_data_size = %i", sub_index, conv_input_data_size);
                            addErrorString(s);
                            return;
                        }
                        if (main_index < input_data_size) {
                            if (layer_on_input) {
                                conv_data[sub_index] = inlayernodes[main_index].getOutput();
                            }
                            else {
                                conv_data[sub_index] = *(double*)inputpointers[main_index];
                            }
                        }
                    }
                }

                for (int i = 0; i < num_hidden_nodes; i++) {
                    hnodes[i].forward();
                }

                for (int q = 0; q < convouts[1]; q++) {
                    qindex1 = (q * convoutstrides[1]);
                    qindex2 = ((q + mindex2) * outputdimstrides[1]);
                    for (int p = 0; p < convouts[0]; p++) {
                        sub_index = p + qindex1 + output_node_offset;
                        main_index = p + nindex2 + qindex2 + cindex3;
                        if (sub_index >= num_hidden_nodes) {
                            char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : num_hidden_nodes = %i", sub_index, num_hidden_nodes);
                            addErrorString(s);
                            return;
                        }
                        if (main_index >= output_data_size) {
                            char s[200];  sprintf_s(s, "forward : main index out of bounds : main_index = %i : output_data_size = %i", main_index, output_data_size);
                            addErrorString(s);
                            return;
                        }
                        output_data[main_index] = hnodes[sub_index].getOutput();
                    }
                }

            }

        }

    }

}

void HiddenConvolution::backward_shared()
{
    int mindex1 = 0;
    int mindex2 = 0;
    int nindex1 = 0;
    int nindex2 = 0;
    int cindex2 = 0;
    int cindex3 = 0;
    int qindex1 = 0;
    int qindex2 = 0;
    int sub_index = 0;
    int main_index = 0;

    for (int i = 0; i < input_data_size; i++) {
        backprop_output_data[i] = 0.0;
    }

    outputBackpropProcessing();

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].forward();
        }
    }

    for (int c = 0; c < inputdims[2]; c++) {

        cindex2 = (c * inputdimstrides[2]);
        cindex3 = (c * outputdimstrides[2]);
        for (int m = 0; m < num_steps[1]; m++) {

            mindex1 = (m * convstrideins[1]);
            mindex2 = (m * convouts[1]);
            for (int n = 0; n < num_steps[0]; n++) {

                nindex1 = (n * convstrideins[0]);
                nindex2 = (n * convouts[0]);
                for (int q = 0; q < convins[1]; q++) {
                    qindex1 = (q * convinstrides[1]);
                    qindex2 = ((q + mindex1) * inputdimstrides[1]);
                    for (int p = 0; p < convins[0]; p++) {
                        sub_index = p + qindex1;
                        main_index = p + nindex1 + qindex2 + cindex2;
                        if (sub_index >= conv_input_data_size) {
                            char s[200];  sprintf_s(s, "forward : sub index out of bounds : sub_index = %i : conv_input_data_size = %i", sub_index, conv_input_data_size);
                            addErrorString(s);
                            return;
                        }
                        if (main_index < input_data_size) {
                            if (layer_on_input) {
                                conv_data[sub_index] = inlayernodes[main_index].getOutput();
                            }
                            else {
                                conv_data[sub_index] = *(double*)inputpointers[main_index];
                            }
                        }
                    }
                }

                for (int i = 0; i < num_hidden_nodes; i++) {
                    hnodes[i].forward();
                }

                for (int q = 0; q < convouts[1]; q++) {
                    qindex1 = (q * convoutstrides[1]);
                    qindex2 = ((q + mindex2) * outputdimstrides[1]);
                    for (int p = 0; p < convouts[0]; p++) {
                        sub_index = p + qindex1;
                        main_index = p + nindex2 + qindex2 + cindex3;
                        if (sub_index >= conv_output_data_size) {
                            char s[200];  sprintf_s(s, "backward : sub index out of bounds : sub_index = %i : conv_output_data_size = %i", sub_index, conv_output_data_size);
                            addErrorString(s);
                            return;
                        }
                        if (main_index >= output_data_size) {
                            char s[200];  sprintf_s(s, "backward : main index out of bounds : main_index = %i : output_data_size = %i", main_index, output_data_size);
                            addErrorString(s);
                            return;
                        }
                        conv_backprop_data[sub_index] = backpropdatabuffer[main_index];
                    }
                }

                for (int i = (num_hidden_nodes - 1); i >= 0; i--) {
                    hnodes[i].backward();
                }

                int num_input_nodes = (int)node_indices[0].size();
                for (int q = 0; q < convins[1]; q++) {
                    qindex1 = (q * convinstrides[1]);
                    qindex2 = ((q + mindex1) * inputdimstrides[1]);
                    for (int p = 0; p < convins[0]; p++) {
                        sub_index = p + qindex1;
                        main_index = p + nindex1 + qindex2 + cindex2;
                        if (main_index < input_data_size) {
                            for (int i = 0; i < (num_input_nodes); i++) {
                                backprop_output_data[main_index] += hnodes[i].backproperrors[sub_index];
                            }
                        }
                    }
                }

            }

        }

    }

    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].backward();
        }
    }

}

void HiddenConvolution::outputBackpropProcessing()
{
    if (image_output) {
        for (int i = 0; i < output_data_size; i++) {
            backpropdatabuffer[i] = *(double*)backpropinputpointers[i] - output_data[i];
        }
    }
    else {
        for (int i = 0; i < output_data_size; i++) {
            backpropdatabuffer[i] = 0.0;
            for (int j = 0; j < num_output_connections; j++) {
                int index = (j * output_data_size) + i;
                backpropdatabuffer[i] += *(double*)backpropinputpointers[index];
            }
        }
    }
}

int* HiddenConvolution::clearIndices(int *indices)
{
    for (int i = 0; i < numdims; i++) {
        indices[i] = 0;
    }
    return(indices);
}

bool HiddenConvolution::incrementIndicesVector(int* indices, int* values)
{
    for (int i = 0; i < numdims; i++) {
        indices[i]++;
        if (indices[i] >= values[i]) {
            indices[i] = 0;
            if (i == (numdims - 1)) {
                return(false);
            }
        }
        else {
            i = numdims;
        }
    }
    return(true);
}

int HiddenConvolution::getIndicesSize(int* values)
{
    int size = 1;
    for (int i = 0; i < numdims; i++) {
        size *= values[i];
    }
    return(size);
}

string HiddenConvolution::getIndicesLabel(int *indices)
{
    string label = "";
    for (int i = 0; i < numdims; i++) {
        char s[200];  sprintf_s(s, ":%i", indices[i]);
        label += s;
    }
    return(label);
}

int HiddenConvolution::getIndicesIndex(int* indices, int* values)
{
    int index = 0;
    int stride = 1;
    for (int i = 0; i < numdims; i++) {
        index += (indices[i] * stride);
        stride *= values[i];
    }
    return(index);
}

int HiddenConvolution::getIndicesIndex(int* indices, int* values, int* strides)
{
    int index = 0;
    for (int i = 0; i < numdims; i++) {
        index += (indices[i] * strides[i]);
    }
    return(index);
}

int HiddenConvolution::getIndicesSubIndex(int* sub_indices, int* main_indices, int* sub_values, int* main_values)
{
    int index = 0;
    int stride = 1;
    for (int i = 0; i < numdims; i++) {
        index += ((sub_indices[i] + (main_indices[i] * sub_values[i])) * stride);
        stride *= main_values[i];
    }
    return(index);
}

int HiddenConvolution::getIndicesSubIndex(int* sub_indices, int* main_indices, int* sub_values, int* main_values, int* strides)
{
    int index = 0;
    for (int i = 0; i < numdims; i++) {
        index += ((sub_indices[i] + (main_indices[i] * sub_values[i])) * strides[i]);
    }
    return(index);
}

void HiddenConvolution::setParameters(double sleak, double rleak) {
    if (allocated) {
        for (int i = 0; i < num_hidden_nodes; i++) {
            hnodes[i].sigmoid_leakage = sleak;
            hnodes[i].relu_leakage = rleak;
        }
        if (layer_on_input) {
            for (int i = 0; i < input_data_size; i++) {
                inlayernodes[i].sigmoid_leakage = sleak;
                inlayernodes[i].relu_leakage = rleak;
            }
        }
    }
}

int HiddenConvolution::getNumWeights()
{
    if (num_weights <= 0) {
        num_weights = 0;
        for (int i = 0; i < num_hidden_nodes; i++) {
            num_weights += hnodes[i].num_inputs;
        }
        if (layer_on_input) {
            for (int i = 0; i < input_data_size; i++) {
                num_weights += inlayernodes[i].num_inputs;
            }
        }
    }
    return(num_weights);
}

double HiddenConvolution::getWeightSquares()
{
    double square_weights = 0;
    for (int i = 0; i < num_hidden_nodes; i++) {
        square_weights += hnodes[i].squareWeights();
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            square_weights += inlayernodes[i].squareWeights();
        }
    }
    return(square_weights);
}

double HiddenConvolution::maxWeights()
{
    double max = 0;
    for (int i = 0; i < num_hidden_nodes; i++) {
        double tmax = hnodes[i].maxWeights();
        if (tmax > max) {
            max = tmax;
        }
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            double tmax = inlayernodes[i].maxWeights();
            if (tmax > max) {
                max = tmax;
            }
        }
    }
    return(max);
}

bool HiddenConvolution::checkValidWeights()
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (!hnodes[i].checkValidWeights()) {
            return(false);
        }
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            if (!inlayernodes[i].checkValidWeights()) {
                return(false);
            }
        }
    }
    return(true);
}

void HiddenConvolution::setTrainable(bool val)
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].setTrainable(val);
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].setTrainable(val);
        }
    }
}

void HiddenConvolution::setZeroOut(bool val)
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].setZeroOut(val);
    }
    if (layer_on_input) {
        for (int i = 0; i < input_data_size; i++) {
            inlayernodes[i].setZeroOut(val);
        }
    }
}

convWeightData HiddenConvolution::getWeights()
{
    convWeightData rval;
    if (allocated) {
        rval.id = id;
        for (int j = 0; j < num_hidden_nodes; j++) {
            rval.nodeweightdata.push_back(hnodes[j].getWeights());
        }
        if (layer_on_input) {
            for (int j = 0; j < input_data_size; j++) {
                rval.inputlayerweightdata.push_back(inlayernodes[j].getWeights());
            }
        }
    }
    return(rval);
}

void HiddenConvolution::setWeights(convWeightData cwd)
{
    if (allocated) {
        if (cwd.id == id) {
            int i = 0;
            bool done = false;
            bool name_search = false;
            while (!done) {
                if ((i < num_hidden_nodes) && (i < (int)cwd.nodeweightdata.size())) {
                    if (cwd.nodeweightdata[i].id == hnodes[i].id) {
                        hnodes[i].setWeights(cwd.nodeweightdata[i]);
                    }
                    else {
                        i = num_hidden_nodes;
                        name_search = true;
                        done = true;
                    }
                    i++;
                }
                else {
                    done = true;
                }
            }
            if (name_search) {
                for (i = 0; i < (int)cwd.nodeweightdata.size(); i++) {
                    for (int j = 0; j < num_hidden_nodes; j++) {
                        if (cwd.nodeweightdata[i].id == hnodes[j].id) {
                            hnodes[j].setWeights(cwd.nodeweightdata[i]);
                            j = num_hidden_nodes;
                        }
                    }
                }
            }
            if (layer_on_input && (cwd.inputlayerweightdata.size() > 0)) {
                i = 0;
                done = false;
                name_search = false;
                while (!done) {
                    if ((i < input_data_size) && (i < (int)cwd.inputlayerweightdata.size())) {
                        if (cwd.inputlayerweightdata[i].id == inlayernodes[i].id) {
                            inlayernodes[i].setWeights(cwd.inputlayerweightdata[i]);
                        }
                        else {
                            i = input_data_size;
                            name_search = true;
                            done = true;
                        }
                        i++;
                    }
                    else {
                        done = true;
                    }
                }
                if (name_search) {
                    for (i = 0; i < (int)cwd.inputlayerweightdata.size(); i++) {
                        for (int j = 0; j < input_data_size; j++) {
                            if (cwd.inputlayerweightdata[i].id == inlayernodes[j].id) {
                                inlayernodes[j].setWeights(cwd.inputlayerweightdata[i]);
                                j = input_data_size;
                            }
                        }
                    }
                }
            }
        }
    }
}

void HiddenConvolution::archiveWeights()
{
    if (allocated) {
        for (int j = 0; j < num_hidden_nodes; j++) {
            hnodes[j].archiveWeights();
        }
        if (layer_on_input) {
            for (int j = 0; j < input_data_size; j++) {
                inlayernodes[j].archiveWeights();
            }
        }
    }
}

void HiddenConvolution::restoreWeights()
{
    if (allocated) {
        for (int j = 0; j < num_hidden_nodes; j++) {
            hnodes[j].restoreWeights();
        }
        if (layer_on_input) {
            for (int j = 0; j < input_data_size; j++) {
                inlayernodes[j].restoreWeights();
            }
        }
    }
}

string HiddenConvolution::getStringBeforeColon(string cname)
{
    string mname = cname;
    for (int i = 0; i < mname.length(); i++) {
        if (mname[i] == ':') {
            return(mname.substr(0, i));
        }
}
    return(cname);
}

void HiddenConvolution::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void HiddenConvolution::addErrorString(string err) {
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}



//******************************************************************************************************

void HiddenNet::clear() {
    allocated = false;
    id = "";
    type = SIGMOID;
    has_softmax = false;
    num_inputs = 0;
    num_layers = 0;
    num_nodes_layer = 0;
    num_outputs = 0;
    num_backprop_inputs = 0;
    num_nodes = 0;
    alpha = 0.2;
    eta = 0.02;
    random_scale = 0.2;
    sigmoid_leakage = 0.01;
    relu_leakage = 0.001;
    softmax_sum = 0.0;
    parallel_processing_test = 100;
    dummy = 0.0;
}

void HiddenNet::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void HiddenNet::addErrorString(string err) {
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}

bool HiddenNet::init(string nid, int ninputs, int noutputs, int nnum_layers, int nnum_nodes_layer, int ntype, 
    double neta, double nalpha, double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutput) {
    return(allocate(nid, ninputs, noutputs, nnum_layers, nnum_nodes_layer, ntype, neta, nalpha, nrandom_scale, 
        nrandomize, ntrainable, nzerooutput));
}

bool HiddenNet::allocate(string nid, int ninputs, int noutputs, int nnum_layers, int nnum_nodes_layer, int ntype, 
    double neta, double nalpha, double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutput)
{
    deallocate();

    clear();

    // Set net parameters
    id = nid;
    num_inputs = ninputs;
    num_layers = nnum_layers;
    num_nodes_layer = nnum_nodes_layer;
    num_outputs = noutputs;
    num_backprop_inputs = 0;

    if ((num_inputs == 0) || (num_layers == 0) || (num_nodes_layer == 0) || (num_outputs == 0)) {
        char s[200];  sprintf_s(s, " : num_inputs = %i : num_layers = %i : num_nodes_layer = %i : num_outputs = %i : ", num_inputs, num_layers, num_nodes_layer, num_outputs);
        addErrorString((string)("Error : HiddenNet input parameters equal to zero") + s + id);
        return(false);
    }

    //char s[200];  sprintf_s(s, " : num_inputs = %i : num_layers = %i : num_nodes_layer = %i : num_outputs = %i : ", num_inputs, num_layers, num_nodes_layer, num_outputs);
    //addErrorString((string)("HiddenNet input parameters") + s + id);

    type = ntype;
    if (type == SOFTMAX) {
        type = SIGMOID;
        has_softmax = true;
    }
    if (type == MAXPOOL) {
        addErrorString("Question : Do we really need a fully connected maxpool hidden net for " + id + "?");
    }
    else if (type == MULTIPLY) {
        addErrorString("Question : Do we really need a fully connected multiply hidden net for " + id + "?");
    }
    else if (type == ADD) {
        addErrorString("Question : Do we really need a fully connected add hidden net for " + id + "?");
    }

    eta = neta;
    alpha = nalpha;
    random_scale = nrandom_scale;

    // Allocate nodes
    num_nodes = num_layers * num_nodes_layer;
    hnodes = new HiddenNode[num_nodes];
    if (hnodes == 0) {
        addErrorString("Error : HiddenNet unable to allocate nodes");
        return(false);
    }

    addPanel1String("Adding net " + id + " - initializing nodes");

    if (num_layers == 1) {
        if (num_nodes == num_nodes_layer) {
            // Initialize for one layer - running parallel processing here may cause crashes
            int mytype = type;
            if (has_softmax) {
                mytype = SOFTMAX;
            }
            for (int i = 0; i < num_nodes_layer; i++) {
                if (i < num_nodes) {
                    hnodes[i].allocate(num_inputs, num_outputs, mytype, eta, alpha, random_scale);
                    char tid[200];  sprintf_s(tid, "layer%iindex%i", 0, i);
                    hnodes[i].setID(id + tid);
                    hnodes[i].trainable = ntrainable;
                    hnodes[i].zeroout = nzerooutput;
                }
                else {
                    addErrorString("Error : HiddenNet index i out of bounds initializing nodes for layer zero");
                }
            }
        }
        else {
            addErrorString("Error : HiddenNet num_nodes not equal to num_nodes_layer for num_layers equal to one");
            return(false);
        }
    }
    else if (num_layers > 1) {

        // Initialize input layer
#ifdef GCC
        for (int i = 0; i < num_nodes_layer; i++) {
#else 
        parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
#endif
            if (i < num_nodes) {
                hnodes[i].allocate(num_inputs, num_nodes_layer, type, eta, alpha, random_scale);
                char tid[200];  sprintf_s(tid, "layer%iindex%i", 0, i);
                hnodes[i].setID(id + tid);
                hnodes[i].trainable = ntrainable;
                hnodes[i].zeroout = nzerooutput;
            }
            else {
                addErrorString("Error : HiddenNet index i out of bounds initializing nodes for layer zero");
            }
#ifdef GCC
        }
#else
        });
#endif

        // Initialize tweener layers
        if (num_layers > 2) {
            for (int k = 1; k < (num_layers - 1); k++) {
#ifdef GCC
                for (int i = 0; i < num_nodes_layer; i++) {
#else
                parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
#endif
                    int nindex = k * num_nodes_layer + i;
                    if (nindex < num_nodes) {
                        hnodes[nindex].allocate(num_nodes_layer, num_nodes_layer, type, eta, alpha, random_scale);
                        char tid[200];  sprintf_s(tid, "layer%iindex%i", k, i);
                        hnodes[nindex].setID(id + tid);
                        hnodes[nindex].trainable = ntrainable;
                        hnodes[nindex].zeroout = nzerooutput;
                    }
                    else {
                        addErrorString("Error : HiddenNet nindex out of bounds initializing nodes for layers");
                    }
#ifdef GCC
                }
#else
                });
#endif
            }
        }

        // Initialize output layer
#ifdef GCC
        for (int i = 0; i < num_nodes_layer; i++) {
#else
        parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
#endif
            int nindex = (num_layers - 1) * num_nodes_layer + i;
            if (nindex < num_nodes) {
                int mytype = type;
                if (has_softmax) {
                    mytype = SOFTMAX;
                }
                hnodes[nindex].allocate(num_nodes_layer, num_outputs, mytype, eta, alpha, random_scale);
                 char tid[200];  sprintf_s(tid, "layer%iindex%i", (num_layers - 1), i);
                hnodes[nindex].setID(id + tid);
                hnodes[nindex].trainable = ntrainable;
                hnodes[nindex].zeroout = nzerooutput;
            }
            else {
                addErrorString("Error : HiddenNet nindex out of bounds initializing nodes for layers");
            }
#ifdef GCC
        }
#else
        });
#endif
    }
    else {
        addErrorString("Error : HiddenNet num_layers equal to zero");
        return(false);
    }

    // Join nodes (need to skip last layer and set it's pointers from outside)
    addPanel1String("Adding net " + id + " - joining nodes");
    if (num_layers > 1) {
        // For all layers except output layer
        for (int k = 0; k < (num_layers - 1); k++) {
            // For all nodes in layer
            for (int i = 0; i < num_nodes_layer; i++) {
                // Make an index for the node
                int nindex = k * num_nodes_layer + i;
                // If nindex is good
                if (nindex < num_nodes) {
                    // For all nodes connected to the output of our nindex node
                    for (int m = 0; m < num_nodes_layer; m++) {
                        // Get pindex for this output connected node
                        int pindex = (k + 1) * num_nodes_layer + m;
                        // If pindex is good
                        if ((pindex >= 0) && (pindex < num_nodes)) {
                            // Join nodes
                            hnodes[pindex].join(&hnodes[nindex]);
                        }
                        else {
                            // pindex error
                            addErrorString("Error : HiddenNet pindex out of bounds joining nodes");
                        }
                    }
                }
                else {
                    // nindex error
                    addErrorString("Error : HiddenNet nindex out of bounds joining nodes");
                }
            }
        }
    }

    output_start_index = num_nodes - num_nodes_layer;

    addPanel1String("Adding net " + id + " - randomizing weights");

    randomizeWeights();

    allocated = true;

    addPanel1String("");

    return(true);

}

void HiddenNet::deallocate() {
    if (allocated) {
        for (int i = 0; i < num_nodes; i++) {
            hnodes[i].deallocate();
            hnodes[i].clear();
        }
        delete[] hnodes;
        allocated = false;
    }
}

bool HiddenNet::addInputPointer(string name, double* tinput)
{
    if (allocated) {
        for (int i = 0; i < num_nodes_layer; i++) {
            if (!hnodes[i].addInputPointer(name, tinput)) {
                addErrorString("HiddenNet::addInputPointer for " + id + " : error setting input " + name + " for node " + hnodes[i].id);
                return(false);
            }
        }
        return(true);
    }
    else {
        addErrorString("HiddenNet::addInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* HiddenNet::getOutputPointer(int index)
{
    if (allocated) {
        int nindex = output_start_index + index;
        if ((nindex >= 0) && (nindex < num_nodes)) {
            return(hnodes[nindex].getOutputPointer());
        }
        else {
            char s[200];  sprintf_s(s, "getOutputPointer index out of bounds : %i : %i", index, num_nodes);
            addErrorString(s);
        }
    }
    return(&dummy);
}

bool HiddenNet::setBackpropInputPointer(int output_index, double* pointer)
{
    if (allocated) {
        int node_index = output_start_index + output_index;
        if ((node_index >= 0) && (node_index < num_nodes)) {
            char s[100];  sprintf_s(s, ":%i", hnodes[node_index].num_backprop_inputs);
            hnodes[node_index].setBackpropInputPointer(pointer, id + s);
            return(true);
        }
        else {
            char s[200];  sprintf_s(s, " : output_index = %i : num_nodes_layer = %i", output_index, num_nodes_layer);
            addErrorString("setBackpropInputPointer for " + id + s);
        }
    }
    return(true);
}

double HiddenNet::getBackpropError(int nindex, int eindex)
{
    if ((nindex >= 0) && (nindex < num_nodes)) {
        return(hnodes[nindex].getBackpropError(eindex));
    }
    return(0.0);
}

// ****************************************************************************
// ****************************************************************************

bool HiddenNet::join(HiddenNode* hnode)
{
    for (int i = 0; i < num_nodes_layer; i++) {
        if (!hnodes[i].join(hnode)) {
            return(false);
        }
    }
    return(true);
}

bool HiddenNet::join(HiddenConvolution* conv)
{
    if (conv->allocated) {
        for (int i = 0; i < num_nodes_layer; i++) {
            if (!hnodes[i].join(conv)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("HiddenNet::join for " + id + " for hidden net " + conv->id + " - conv not allocated");
        return(false);
    }
    return(true);
}

bool HiddenNet::join(HiddenNet* hnet)
{
    if (hnet->num_nodes_layer > 0) {
        for (int i = 0; i < num_nodes_layer; i++) {
            if (!hnodes[i].join(hnet)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("HiddenNet::join for " + id + " for hidden net " + hnet->id + " - no hnet outputs");
        return(false);
    }
    return(true);
}

bool HiddenNet::join(NetCluster* ncluster)
{
    if (ncluster->num_outputs > 0) {
        for (int i = 0; i < num_nodes_layer; i++) {
            if (!hnodes[i].join(ncluster)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("HiddenNet::join for " + id + " for cluster " + ncluster->id + " - no cluster outputs");
        return(false);
    }
    return(true);
}

bool HiddenNet::join(Attention* attention)
{
    if (attention->output_size > 0) {
        for (int i = 0; i < num_nodes_layer; i++) {
            if (!hnodes[i].join(attention)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("HiddenNet::join for " + id + " for attention " + attention->id + " - no ttention outputs");
        return(false);
    }
    return(true);
}

bool HiddenNet::join(SelfAttention* selfattentions)
{
    return(join((Attention*)selfattentions));
}

bool HiddenNet::join(Dropout* dropout)
{
    if (dropout->allocated) {
        for (int i = 0; i < num_nodes_layer; i++) {
            if (!hnodes[i].join(dropout)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("HiddenNet::join for " + id + " for dropout " + dropout->id + " - not allocated");
        return(false);
    }
    return(true);
}

bool HiddenNet::join(Addition* addition)
{
    if (addition->allocated && (addition->model_dimension > 0) && (num_nodes_layer <= addition->model_dimension)) {
        boolInt tval = addition->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < num_nodes_layer; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(addition->id + s, addition->getOutputPointer(i))) {
                    addErrorString("HiddenNet::join for " + id + " for addition " + addition->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = hnodes[i].getBackpropOutputPointer(addition->id + s);
                if (!addition->setBackpropInputPointer(tval.value, pointer)) {
                    addErrorString("HiddenNet::join for " + id + " for addition " + addition->id + " unable to set backprop pointer");
                    return(false);
                }
                //addErrorString("HiddenNet::join : Addition : " + (string)s);
            }
            return(true);
        }
        else {
            addErrorString("HiddenNet::join for " + id + " for addition " + addition->id + " problem with addition backprop index");
        }
    }
    else if (num_nodes_layer > addition->model_dimension) {
        char s[200];  sprintf_s(s, " : num_nodes_layer (%i) > model_dimension (%i)", num_nodes_layer, addition->model_dimension);
        addErrorString("Node::join for " + id + " for addition " + addition->id + " problem with addition backprop index");
    }
    else {
        addErrorString("HiddenNet::join for " + id + " for addition " + addition->id + " - no addition model size");
        return(false);
    }
    return(true);
}

bool HiddenNet::join(Multiplication* multiplication)
{
    if (multiplication->allocated && (multiplication->model_dimension > 0) && (num_nodes_layer <= multiplication->model_dimension)) {
        boolInt tval = multiplication->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < num_nodes_layer; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(multiplication->id + s, multiplication->getOutputPointer(i))) {
                    addErrorString("HiddenNet::join for " + id + " for multiplication " + multiplication->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = hnodes[i].getBackpropOutputPointer(multiplication->id + s);
                if (!multiplication->setBackpropInputPointer(tval.value, pointer)) {
                    addErrorString("HiddenNet::join for " + id + " for multiplication " + multiplication->id + " unable to set backprop pointer");
                    return(false);
                }
                //addErrorString("HiddenNet::join : Multiplication : " + (string)s);
            }
            return(true);
        }
        else {
            addErrorString("HiddenNet::join for " + id + " for multiplication " + multiplication->id + " problem with multiplication backprop index");
        }
    }
    else if (num_nodes_layer > multiplication->model_dimension) {
        char s[200];  sprintf_s(s, " : num_nodes_layer (%i) > model_dimension (%i)", num_nodes_layer, multiplication->model_dimension);
        addErrorString("Node::join for " + id + " for multiplication " + multiplication->id + " problem with addition backprop index");
    }
    else {
        addErrorString("HiddenNet::join for " + id + " for multiplication " + multiplication->id + " - no multiplication model size");
        return(false);
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void HiddenNet::clearWeights() {
    for (int i = 0; i < num_nodes; i++) {
        hnodes[i].clearWeights();
    }
}

void HiddenNet::randomizeWeights() {
    for (int i = 0; i < num_nodes; i++) {
        hnodes[i].randomizeWeights();
    }
}

bool HiddenNet::checkValidWeights()
{
    for (int i = 0; i < num_nodes; i++) {
        if (!hnodes[i].checkValidWeights()) {
            return(false);
        }
    }
    return(true);
}

void HiddenNet::forward()
{
    if (num_nodes_layer >= parallel_processing_test) {
        if (num_layers == 1) {
#ifdef GCC
            for (int i = 0; i < num_nodes_layer; i++) {
                hnodes[i].forward();
            }
#else 
            parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                hnodes[i].forward();
            });
#endif
        }
        else if (num_layers > 1) {
            int pre = 0;
#ifdef GCC
            for (int i = 0; i < num_nodes_layer; i++) {
                hnodes[i].forward();
            }
#else
            parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                hnodes[i].forward();
            });
#endif
            if (num_layers > 2) {
                for (int k = 1; k < (num_layers - 1); k++) {
                    pre = k * num_nodes_layer;
#ifdef GCC
                    for (int i = 0; i < num_nodes_layer; i++) {
                        hnodes[pre + i].forward();
                    }
#else
                    parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                        hnodes[pre + i].forward();
                    });
#endif
                }
            }
            pre = (num_layers - 1) * num_nodes_layer;
#ifdef GCC
            for (int i = 0; i < num_nodes_layer; i++) {
                hnodes[pre + i].forward();
            }
#else
            parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                hnodes[pre + i].forward();
            });
#endif
        }
    }
    else {
        for (int i = 0; i < num_nodes; i++) {
            hnodes[i].forward();
        }
    }
    softmaxNormalizeOutputLayers();
}

void HiddenNet::backward()
{
    softmaxOutputLayerBackpropProcessing();
    if (num_nodes_layer >= parallel_processing_test) {
        if (num_layers == 1) {
#ifdef GCC
            for (int i = 0; i < num_nodes_layer; i++) {
                hnodes[i].backward();
            }
#else
            parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                hnodes[i].backward();
            });
#endif
        }
        else if (num_layers > 1) {
            int pre = (num_layers - 1) * num_nodes_layer;
#ifdef GCC
            for (int i = 0; i < num_nodes_layer; i++) {
                hnodes[pre + i].backward();
            }
#else
            parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                hnodes[pre + i].backward();
            });
#endif
            if (num_layers > 2) {
                for (int k = (num_layers - 2); k > 0; k--) {
                    pre = k * num_nodes_layer;
#ifdef GCC
                    for (int i = 0; i < num_nodes_layer; i++) {
                        hnodes[pre + i].backward();
                    }
#else
                    parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                        hnodes[pre + i].backward();
                    });
#endif
                }
            }
#ifdef GCC
            for (int i = 0; i < num_nodes_layer; i++) {
                hnodes[i].backward();
            }
#else
            parallel_for(long(0), (long)num_nodes_layer, [&](long i) {
                hnodes[i].backward();
            });
#endif
        }
    }
    else {
        for (int i = (num_nodes - 1); i >= 0; i--) {
            hnodes[i].backward();
        }
    }
}

void HiddenNet::softmaxNormalizeOutputLayers()
{
    if (has_softmax) {
        softmax_sum = 0.0;
        for (int i = output_start_index; i < num_nodes; i++) {
            softmax_sum += hnodes[i].getOutput();
        }
        if (softmax_sum < 0.0) {
            softmax_sum = -1 * softmax_sum;
        }
        if (softmax_sum > 0.0) {
            for (int i = output_start_index; i < num_nodes; i++) {
                hnodes[i].setOutput(hnodes[i].getOutput() / softmax_sum);
            }
        }
    }
}

void HiddenNet::softmaxOutputLayerBackpropProcessing()
{
    if (has_softmax) {
        double gi = 0.0;
        double gj = 0.0;
        double dij = 0.0;
        double error = 0.0;
        double delta = 0.0;
        double derivative = 0.0;
        for (int i = 0; i < num_nodes_layer; i++) {
            error = 0.0;
            gi = hnodes[output_start_index + i].getOutput();
            for (int j = 0; j < num_nodes_layer; j++) {
                gj = hnodes[output_start_index + j].getOutput();
                if (i == j) {
                    dij = 1.0;
                }
                else {
                    dij = 0.0;
                }
                derivative = (gi * (dij - gj));
                delta = hnodes[output_start_index + j].getBackpropSum();
                error += (delta * derivative);
            }
            hnodes[output_start_index + i].setSoftmaxError(error);
        }
    }
}

void HiddenNet::setParameters(double sleak, double rleak) {
    if (allocated) {
        for (int i = 0; i < num_nodes; i++) {
            hnodes[i].sigmoid_leakage = sleak;
            hnodes[i].relu_leakage = rleak;
        }
    }
}

int HiddenNet::getNumWeights()
{
    int num_weights = 0;
    for (int i = 0; i < num_nodes; i++) {
        num_weights += hnodes[i].num_inputs;
    }
    return(num_weights);
}

double HiddenNet::getWeightSquares()
{
    double square_weights = 0;
    for (int i = 0; i < num_nodes; i++) {
        square_weights += hnodes[i].squareWeights();
    }
    return(square_weights);
}

double HiddenNet::maxWeights()
{
    double max = 0;
    for (int i = 0; i < num_nodes; i++) {
        double tmax = hnodes[i].maxWeights();
        if (tmax > max) {
            max = tmax;
        }
    }
    return(max);
}

void HiddenNet::setInputToZero(string inputid)
{
    if (allocated) {
        for (int i = 0; i < num_nodes_layer; i++) {
            hnodes[i].setInputToZero(inputid);
        }
    }
}

void HiddenNet::setInputToData(string inputid)
{
    if (allocated) {
        for (int i = 0; i < num_nodes_layer; i++) {
            hnodes[i].setInputToData(inputid);
        }
    }
}

void HiddenNet::setTrainable(bool val)
{
    for (int i = 0; i < num_nodes; i++) {
        hnodes[i].setTrainable(val);
    }
}

void HiddenNet::setZeroOut(bool val)
{
    for (int i = 0; i < num_nodes; i++) {
        hnodes[i].setZeroOut(val);
    }
}

hiddenWeightData HiddenNet::getWeights()
{
    hiddenWeightData rval;
    if (allocated) {
        rval.id = id;
        for (int j = 0; j < num_nodes; j++) {
            rval.nodeweightdata.push_back(hnodes[j].getWeights());
        }
    }
    return(rval);
}

void HiddenNet::setWeights(hiddenWeightData cwd)
{
    if (allocated) {
        if (cwd.id == id) {
            int i = 0;
            bool done = false;
            bool name_search = false;
            while (!done) {
                if ((i < num_nodes) && (i < (int)cwd.nodeweightdata.size())) {
                    if (cwd.nodeweightdata[i].id == hnodes[i].id) {
                        hnodes[i].setWeights(cwd.nodeweightdata[i]);
                    }
                    else {
                        i = num_nodes;
                        name_search = true;
                        done = true;
                    }
                    i++;
                }
                else {
                    done = true;
                }
            }
            if (name_search) {
                for (i = 0; i < (int)cwd.nodeweightdata.size(); i++) {
                    for (int j = 0; j < num_nodes; j++) {
                        if (cwd.nodeweightdata[i].id == hnodes[j].id) {
                            hnodes[j].setWeights(cwd.nodeweightdata[i]);
                            j = num_nodes;
                        }
                    }
                }
            }
        }
    }
}

void HiddenNet::archiveWeights()
{
    if (allocated) {
        for (int j = 0; j < num_nodes; j++) {
            hnodes[j].archiveWeights();
        }
    }
}

void HiddenNet::restoreWeights()
{
    if (allocated) {
        for (int j = 0; j < num_nodes; j++) {
            hnodes[j].restoreWeights();
        }
    }
}

void HiddenNet::setParallelProcessingTest(int test)
{
    if (test > 0) {
        parallel_processing_test = test;
    }
}

void HiddenNet::addPanel1String(string msg)
{
#ifdef GCC
    cout << msg << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addPanel1String((CString)msg.c_str());
#endif
}





//******************************************************************************************************

void Attention::clear() {
    allocated = false;
    oninput = false;
    processsoftmax = true;
    id = "";
    
    input_index = 0;
    backprop_set_index = 0;

    num_kinputs = 0;
    num_qinputs = 0;
    num_vinputs = 0;

    kdimension = 0;
    vdimension = 0;
    modeldimension = 0;
    input_size = 0;
    offset = 0;

    kstart_index = 0;
    qstart_index = 0;
    vstart_index = 0;

    intermedate_size = 0;
    output_size = 0;

    num_backprop_inputs = 0;
    backprop_input_indices = NULL;
    backprop_size = 0;

    output_connection_index = -1;

    sum_add_limit = 0.10;

    dummy = 0.0;
    zero = 0.0;
    one = 1.0;

}

void Attention::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void Attention::addErrorString(string err) {
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}

bool Attention::init(string nid, int ndk, int ndv, int ndm, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool ninput)
{
    return(allocate(nid, ndk, ndv, ndm, naddsum, nsoft, ninputs, nbackpropinputs, ninput));
}

bool Attention::allocate(string nid, int ndk, int ndv, int ndm, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool ninput)
{
    addPanel1String("Adding attention layer " + id);

    deallocate();

    clear();

    // Set net parameters
    id = nid;
    oninput = ninput;
    kdimension = ndk;
    vdimension = ndv;
    modeldimension = ndm;
    sum_add_limit = naddsum;
    processsoftmax = nsoft;
    num_backprop_inputs = nbackpropinputs;
    backprop_input_indices = NULL;

    input_size = (2 * kdimension * modeldimension) + (vdimension * modeldimension);
    if (input_size != ninputs) {
        char s[200];  sprintf_s(s, " : input_size = %i : ninputs = %i : ", input_size, ninputs);
        addErrorString((string)("Error : Attention input size mismatch") + s + id);
        return(false);
    }

    output_size = vdimension * modeldimension;

    //char s[200];  sprintf_s(s, "Attention::allocate : num_backprop_inputs = %i : output size = %i", num_backprop_inputs, output_size); addErrorString(s);

    kstart_index = 0;
    qstart_index = kdimension * modeldimension;
    vstart_index = 2 * kdimension * modeldimension;

    if ((kdimension == 0) || (vdimension == 0) || (modeldimension == 0)) {
        char s[200];  sprintf_s(s, " : kdimension = %i : vdimension = %i : modeldimension = %i : ", kdimension, vdimension, modeldimension);
        addErrorString((string)("Error : Attention input parameters equal to zero") + s + id);
        return(false);
    }

    addPanel1String("Allocaing inputpointers for " + id);

    inputpointers = new int64_t[input_size];
    inputpointernames = new string[input_size];
    backpropoutputs = new double[input_size];
    if ((inputpointers == 0) || (backpropoutputs == 0) || (inputpointernames == 0)) {
        addErrorString("Error : Attention unable to allocate inputpointers");
        return(false);
    }
    for (int i = 0; i < input_size; i++) {
        inputpointers[i] = (int64_t)&dummy;
        inputpointernames[i] = "";
        backpropoutputs[i] = 0.0;
    }

    addPanel1String("Allocaing intermediatebuffer for " + id);

    intermedate_size = modeldimension * modeldimension;
    intermediatebuffer = new double[intermedate_size];
    intermediatebackpropoutputs = new double[intermedate_size];
    intermediatesoftmaxbackpropoutputs = new double[intermedate_size];
    if ((intermediatebuffer == 0) || (intermediatebackpropoutputs == 0) || (intermediatesoftmaxbackpropoutputs == 0)) {
        addErrorString("Error : Attention unable to allocate intermediatebuffer");
        return(false);
    }
    for (int i = 0; i < intermedate_size; i++) {
        intermediatebuffer[i] = 0.0;
        intermediatesoftmaxbackpropoutputs[i] = 0.0;
        intermediatebackpropoutputs[i] = 0.0;
    }

    addPanel1String("Allocaing backpropinputs for " + id);

    backpropinputs = new double[output_size];
    outputbuffer = new double[output_size];
    if ((outputbuffer == 0) || (backpropinputs == 0)) {
        addErrorString("Error : Attention unable to allocate outputbuffer");
        return(false);
    }
    for (int i = 0; i < output_size; i++) {
        backpropinputs[i] = 0.0;
        outputbuffer[i] = 0.0;
    }

    addPanel1String("Allocaing backprop_input_indices for " + id);

    backprop_input_indices = new int[num_backprop_inputs];
    if (backprop_input_indices == 0) {
        addErrorString("Error : Attention unable to allocate outputbuffer");
        return(false);
    }
    for (int i = 0; i < num_backprop_inputs; i++) {
        backprop_input_indices[i] = 0;
    }

    addPanel1String("Allocaing backpropinputpointers for " + id);

    backprop_size = output_size * num_backprop_inputs;
    backpropinputpointers = new int64_t[backprop_size];
    if (backpropinputpointers == 0) {
        addErrorString("Error : Attention unable to allocate backpropinputpointers");
        return(false);
    }
    for (int i = 0; i < backprop_size; i++) {
        backpropinputpointers[i] = (int64_t)&dummy;
    }

    allocated = true;

    addPanel1String("");

    return(true);

}

void Attention::deallocate() {
    if (allocated) {
        delete[] inputpointers;
        delete[] inputpointernames;
        delete[] backpropinputpointers;
        delete[] intermediatebuffer;
        delete[] intermediatebackpropoutputs;
        delete[] intermediatesoftmaxbackpropoutputs;
        delete[] backprop_input_indices;
        delete[] backpropoutputs;
        delete[] backpropinputs;
        delete[] outputbuffer;
        allocated = false;
    }
}

void Attention::setInputPointer(int index, double* tinput, string name)
{
    if (allocated) {
        if (index < input_size) {
            inputpointers[index] = (int64_t)tinput;
        }
        else {
            char s[200];  sprintf_s(s, " : %i (%i)", index, input_size);
            addErrorString("Attention::setInputPointer for " + id + " : index >= input_size" + s);
        }
    }
}

bool Attention::addKInputPointer(string name, double* tinput)
{
    if (allocated) {
        if (num_kinputs < (kdimension * modeldimension)) {
            inputpointers[num_kinputs] = (int64_t)tinput;
            inputpointernames[num_kinputs] = name;
            num_kinputs++;
            return(true);
        }
    }
    else {
        addErrorString("Attention::addKInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

bool Attention::addQInputPointer(string name, double* tinput)
{
    if (allocated) {
        if (num_qinputs < (kdimension * modeldimension)) {
            inputpointers[qstart_index + num_qinputs] = (int64_t)tinput;
            inputpointernames[qstart_index + num_qinputs] = name;
            num_qinputs++;
            return(true);
        }
    }
    else {
        addErrorString("Attention::addQInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

bool Attention::addVInputPointer(string name, double* tinput)
{
    if (allocated) {
        if (num_vinputs < (vdimension * modeldimension)) {
            inputpointers[vstart_index + num_vinputs] = (int64_t)tinput;
            inputpointernames[vstart_index + num_vinputs] = name;
            num_vinputs++;
            return(true);
        }
    }
    else {
        addErrorString("Attention::addVInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* Attention::getBackpropOutputPointer(int input_index)
{
    if (allocated) {
        if (input_index < input_size) {
            return(&backpropoutputs[input_index]);
        }
    }
    return(&dummy);
}

double* Attention::getKBackpropOutputPointer(string name) {
    if (allocated) {
        for (int i = 0; i < (kdimension * modeldimension); i++) {
            if (name == inputpointernames[i]) {
                return(&backpropoutputs[i]);
            }
        }
        addErrorString("Attention::getKBackpropOutputPointer for " + id + " : unable to find " + name);
    }
    else {
        addErrorString("Attention::getKBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

double* Attention::getQBackpropOutputPointer(string name) {
    if (allocated) {
        for (int i = 0; i < (kdimension * modeldimension); i++) {
            if (name == inputpointernames[qstart_index + i]) {
                return(&backpropoutputs[qstart_index + i]);
            }
        }
        addErrorString("Attention::getQBackpropOutputPointer for " + id + " : unable to find " + name);
    }
    else {
        addErrorString("Attention::getQBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

double* Attention::getVBackpropOutputPointer(string name) {
    if (allocated) {
        for (int i = 0; i < (vdimension * modeldimension); i++) {
            if (name == inputpointernames[vstart_index + i]) {
                return(&backpropoutputs[vstart_index + i]);
            }
        }
        addErrorString("Attention::getVBackpropOutputPointer for " + id + " : unable to find " + name);
    }
    else {
        addErrorString("Attention::getVBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

boolInt Attention::addNewBackpropConnection(string name)
{
    boolInt rval;
    rval.valid = false;
    rval.value = -1;
    if (output_connection_index < (num_backprop_inputs - 1)) {
        output_connection_index++;
        rval.value = output_connection_index;
        rval.valid = true;
    }
    else {
        char s[200];  sprintf_s(s, "Attention::addNewBackpropConnection : output_connection_index (%i) >= num_backprop_inputs - 1 (%i) for ", output_connection_index, (num_backprop_inputs - 1));
        addErrorString(s + id);
    }
    return(rval);
}

bool Attention::setBackpropInputPointer(int output_index, double* pointer)
{
    if (allocated) {
        if (output_index < num_backprop_inputs) {
            if (backprop_input_indices[output_index] < output_size) {
                int index = (output_index * output_size) + backprop_input_indices[output_index];
                if (index < backprop_size) {
                    backpropinputpointers[index] = (int64_t)pointer;
                    backprop_input_indices[output_index]++;
                    return(true);
                }
                else {
                    addErrorString("backprop_size error for " + id);
                    return(false);
                }
            }
            else {
                char s[200];  sprintf_s(s, " : output_index = %i : backprop_input_indices[output_index] = %i : num_backprop_inputs = %i", output_index, backprop_input_indices[output_index], num_backprop_inputs);
                addErrorString("backprop_input_indices error for " + id + s);
                return(false);
            }
        }
        else {
            addErrorString("Unable to process backprop connection for " + id + " - problem with index (1)");
            return(false);
        }
    }
    return(true);
}

double* Attention::getOutputPointer(int index)
{
    if (allocated) {
        if ((index >= 0) && (index < output_size)) {
            return(&outputbuffer[index]);
        }
        else {
            char s[200];  sprintf_s(s, "Attention::getOutputPointer index out of bounds : %i : %i", index, output_size);
            addErrorString(s);
        }
    }
    return(&dummy);
}

bool Attention::addInputPointer(string name, double* tinput)
{
    if (allocated) {
        int mindex = offset + input_index;
        if (mindex < input_size) {
            inputpointers[mindex] = (int64_t)tinput;
            inputpointernames[mindex] = name;
            input_index++;
            return(true);
        }
        char s[200];  sprintf_s(s, " : input_index = %i : num_inputs = %i : offset = %i", input_index, input_size, offset);
        addErrorString("Attention::addInputPointer for " + id + " from " + name + s);
    }
    else {
        addErrorString("Attention::addInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* Attention::getBackpropOutputPointer(string name)
{
    if (allocated) {
        if (backprop_set_index < input_size) {
            if (name == inputpointernames[backprop_set_index]) {
                double* rval = &backpropoutputs[backprop_set_index];
                backprop_set_index++;
                if (backprop_set_index > input_size) {
                    backprop_set_index = 0;
                }
                return(rval);
            }
        }
        for (int i = 0; i < input_size; i++) {
            if (name == inputpointernames[i]) {
                double* rval = &backpropoutputs[i];
                backprop_set_index = i + 1;
                if (backprop_set_index > input_size) {
                    backprop_set_index = 0;
                }
                return(rval);
            }
        }
        addErrorString("Attention::getBackpropOutputPointer for " + id + " : unable to find " + name + " : example " + inputpointernames[0]);
    }
    else {
        addErrorString("Attention::getBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

// ****************************************************************************
// ****************************************************************************

bool Attention::joink(HiddenNode* hnode)
{
    // set hnode input pointer
    if (!addKInputPointer(hnode->id, &hnode->output)) {
        //addErrorString("Attention::joink for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
        return(false);
    }
    // set self backprop inputs
    if (!hnode->setBackpropInputPointer(getKBackpropOutputPointer(hnode->id), id)) {
        //addErrorString("Attention::joink for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
        return(false);
    }
    return(true);
}

bool Attention::joinq(HiddenNode* hnode)
{
    // set hnode input pointer
    if (!addQInputPointer(hnode->id, &hnode->output)) {
        //addErrorString("Attention::joinq for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
        return(false);
    }
    // set self backprop inputs
    if (!hnode->setBackpropInputPointer(getQBackpropOutputPointer(hnode->id), id)) {
        //addErrorString("Attention::joinq for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
        return(false);
    }
    return(true);
}

bool Attention::joinv(HiddenNode* hnode)
{
    // set hnode input pointer
    if (!addVInputPointer(hnode->id, &hnode->output)) {
        //addErrorString("Attention::joinv for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
        return(false);
    }
    // set self backprop inputs
    if (!hnode->setBackpropInputPointer(getVBackpropOutputPointer(hnode->id), id)) {
        //addErrorString("Attention::joinv for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
        return(false);
    }
    return(true);
}

bool Attention::joinv(HiddenConvolution* conv)
{
    boolInt tval = conv->addNewBackpropConnection(id);
    if (tval.valid) {
        char s[100];
        for (int i = 0; i < conv->output_data_size; i++) {
            // Set inputs
            sprintf_s(s, ":%i", i);
            if (!addVInputPointer(conv->id + s, conv->getOutputPointer(i))) {
                addErrorString("Attention::joinv for " + id + " for convolution " + conv->id + " unable to set input pointers");
                return(false);
            }
            // set self backprop inputs
            double* pointer = getVBackpropOutputPointer(conv->id + s);
            if (!conv->setBackpropInputPointer2(tval.value, pointer)) {  
                addErrorString("Attention::joinv for " + id + " for conv " + conv->id + " unable to set backprop pointer");
                return(false);
            }
        }
        return(true);
    }
    else {
        addErrorString("Attention::joinv for " + id + " for conv " + conv->id + " problem with conv backprop index");
    }
    return(true);
}

boolInt Attention::joink(HiddenNet* hnet)
{
    boolInt rval;
    rval.value = 0;
    rval.valid = false;
    while (rval.value < hnet->num_nodes_layer) {  
        //char s[200];  sprintf_s(s, "joink : rval.value = %i : qstart_index = %i : num_nodes_layer = %i", rval.value, qstart_index, hnet->num_nodes_layer);  addErrorString(s);
        // set net hnode input pointer
        if (!addKInputPointer(hnet->hnodes[rval.value].id, &hnet->hnodes[rval.value].output)) {
            //addErrorString("Attention::joink for " + id + " for hidden net node " + hnet->hnodes[rval.value].id + " unable to set input pointer");
            return(rval);
        }
        // set self backprop inputs
        if (!hnet->hnodes[rval.value].setBackpropInputPointer(getKBackpropOutputPointer(hnet->hnodes[rval.value].id), id)) {
            //addErrorString("Attention::joink for " + id + " for hidden net node " + hnet->hnodes[rval.value].id + " unable to set backprop pointer");
            return(rval);
        }
        rval.value++;
    }
    rval.valid = true;
    return(rval);
}

boolInt Attention::joinq(HiddenNet* hnet, int offset)
{
    boolInt rval;
    rval.value = offset;
    rval.valid = false;
    while (rval.value < hnet->num_nodes_layer) {
        //char s[200];  sprintf_s(s, "joinq : rval.value = %i : vstart_index = %i : num_nodes_layer = %i", rval.value, vstart_index, hnet->num_nodes_layer);  addErrorString(s);
        // set net hnode input pointer
        if (!addQInputPointer(hnet->hnodes[rval.value].id, &hnet->hnodes[rval.value].output)) {
            //addErrorString("Attention::joinq for " + id + " for hidden net node " + hnet->hnodes[rval.value].id + " unable to set input pointer");
            return(rval);
        }
        // set self backprop inputs
        if (!hnet->hnodes[rval.value].setBackpropInputPointer(getQBackpropOutputPointer(hnet->hnodes[rval.value].id), id)) {
            //addErrorString("Attention::joinq for " + id + " for hidden net node " + hnet->hnodes[rval.value].id + " unable to set backprop pointer");
            return(rval);
        }
        rval.value++;
    }
    rval.valid = true;
    return(rval);
}

boolInt Attention::joinv(HiddenNet* hnet, int offset)
{
    boolInt rval;
    rval.value = offset;
    rval.valid = false;
    while (rval.value < hnet->num_nodes_layer) {
        //char s[200];  sprintf_s(s, "joinv : rval.value = %i : input_size = %i : num_nodes_layer = %i", rval.value, input_size, hnet->num_nodes_layer);  addErrorString(s);
        // set net hnode input pointer
        if (!addVInputPointer(hnet->hnodes[rval.value].id, &hnet->hnodes[rval.value].output)) {
            addErrorString("Attention::joinv for " + id + " for hidden net node " + hnet->hnodes[rval.value].id + " unable to set input pointer");
            return(rval);
        }
        // set self backprop inputs
        if (!hnet->hnodes[rval.value].setBackpropInputPointer(getVBackpropOutputPointer(hnet->hnodes[rval.value].id), id)) {
            addErrorString("Attention::joinv for " + id + " for hidden net node " + hnet->hnodes[rval.value].id + " unable to set backprop pointer");
            return(rval);
        }
        rval.value++;
    }
    rval.valid = true;
    return(rval);
}

bool Attention::joink(NetCluster* ncluster)
{
    for (int i = 0; i < ncluster->num_outputs; i++) {
        char s[100];  sprintf_s(s, ":%i", i);
        if (!addKInputPointer(ncluster->id + s, ncluster->getOutputPointer(i))) {
            addErrorString("Attention::joink for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
            return(false);
        }
    }
    return(true);
}

bool Attention::joinq(NetCluster* ncluster)
{
    for (int i = 0; i < ncluster->num_outputs; i++) {
        char s[100];  sprintf_s(s, ":%i", i);
        if (!addQInputPointer(ncluster->id + s, ncluster->getOutputPointer(i))) {
            addErrorString("Attention::joinq for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
            return(false);
        }
    }
    return(true);
}

bool Attention::join(Dropout* dropout)
{
    if (input_size == dropout->num_inputs) {
        boolInt tval = dropout->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < dropout->num_inputs; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(dropout->id + s, dropout->getOutputPointer(i))) {
                    addErrorString("Attention::join for " + id + " for dropout " + dropout->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(dropout->id + s);
                if (!dropout->setBackpropInputPointer(tval.value, pointer)) {  
                    addErrorString("Attention::join for " + id + " for dropout " + dropout->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("Attention::join for " + id + " for dropout " + dropout->id + " problem with dropout backprop index");
        }
    }
    else {
        char s[200];  sprintf_s(s, " input_data_size (%i) : num_inputs (%i)", input_size, dropout->num_inputs);
        addErrorString("Attention::join for " + id + " : input_data_size != output_data_size" + s);
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void Attention::forward()
{
    qktDotProduct();
    qdtSoftMax();
    vDotProduct();
}

void Attention::backward()
{
    if (!oninput) {
        calculateBackpropInputs();
        calculateVBackprops();
        calculateIntermediateBackprops();
        calculateIntermediateBackSoftMax();
        calculateKBackprops();
        calculateQBackprops();
    }
}

void Attention::qktDotProduct()
{
    int kindex1 = 0;
    int oindex1 = 0;
    int qindex1 = qstart_index;
    double sum = 0.0;
    double invdk = 1 / sqrt((double)kdimension);
    for (int i = 0; i < modeldimension; i++) {
        kindex1 = 0;
        for (int j = 0; j < modeldimension; j++) {
            sum = 0.0;
            for (int k = 0; k < kdimension; k++) {
                sum += (*(double*)inputpointers[kindex1 + k] * *(double*)inputpointers[qindex1 + k]);
            }
            intermediatebuffer[oindex1 + j] = sum * invdk;
            kindex1 += kdimension;
        }
        oindex1 += modeldimension;
        qindex1 += kdimension;
    }
}

void Attention::qdtSoftMax()
{
    if (processsoftmax) {
        double softmax_sum = 0.0;
        for (int i = 0; i < intermedate_size; i++) {
            softmax_sum += exp(intermediatebuffer[i]);
        }
        if (softmax_sum < 0.0) {
            softmax_sum = -1 * softmax_sum;
        }
        if (softmax_sum > 0.0) {
            for (int i = 0; i < intermedate_size; i++) {
                intermediatebuffer[i] = exp(intermediatebuffer[i]) / softmax_sum;
            }
        }
        else {
            addErrorString("qdtSoftMax : negative sum error for " + id);
        }
    }
}

void Attention::vDotProduct()
{
    int vindex1 = 0;
    int sindex1 = 0;
    int oindex1 = 0;
    double sum = 0.0;
    for (int i = 0; i < modeldimension; i++) {
        for (int j = 0; j < vdimension; j++) {
            sum = 0.0;
            vindex1 = vstart_index + j;
            for (int k = 0; k < modeldimension; k++) {
                sum += (intermediatebuffer[sindex1 + k] * *(double*)inputpointers[vindex1]);
                vindex1 += vdimension;
            }
            outputbuffer[oindex1 + j] = sum;
        }
        sindex1 += modeldimension;
        oindex1 += vdimension;
    }
}

void Attention::calculateBackpropInputs()
{
    for (int i = 0; i < output_size; i++) {
        double sum = 0.0;
        for (int j = 0; j < num_backprop_inputs; j++) {
            int index = (i * num_backprop_inputs) + j;
            sum += *(double*)backpropinputpointers[index];
        }
        backpropinputs[i] = sum;
    }
}

void Attention::calculateVBackprops()
{
    int vindex1 = 0;
    int sindex1 = 0;
    int oindex1 = vstart_index;
    double sum = 0.0;
    double val = 0.0;
    for (int i = 0; i < modeldimension; i++) {
        for (int j = 0; j < vdimension; j++) {
            sum = 0.0;
            sindex1 = i;
            vindex1 = j;
            for (int k = 0; k < modeldimension; k++) {
                if (intermediatebuffer[sindex1] != 0.0) {
                    val = backpropinputs[vindex1] / intermediatebuffer[sindex1];
                    if (val > sum_add_limit) {
                        val = sum_add_limit;
                    }
                    else if (val < -sum_add_limit) {
                        val = -sum_add_limit;
                    }
                    sum += val;
                }
                sindex1 += modeldimension;
                vindex1 += vdimension;
            }
            backpropoutputs[oindex1 + j] = sum;
        }
        oindex1 += vdimension;
    }
}

void Attention::calculateIntermediateBackprops()
{
    int vindex1 = 0;
    int sindex1 = 0;
    int oindex1 = 0;
    double sum = 0.0;
    double val = 0.0;
    for (int i = 0; i < modeldimension; i++) {
        sindex1 = 0;
        for (int j = 0; j < modeldimension; j++) {
            sum = 0.0;
            for (int k = 0; k < vdimension; k++) {
                if (outputbuffer[sindex1 + k] != 0.0) {
                    val = backpropinputs[vindex1 + k] / outputbuffer[sindex1 + k];
                    if (val > sum_add_limit) {
                        val = sum_add_limit;
                    }
                    else if (val < -sum_add_limit) {
                        val = -sum_add_limit;
                    }
                    sum += val;
                }
            }
            intermediatebackpropoutputs[oindex1 + j] = sum;
            sindex1 += vdimension;
        }
        oindex1 += modeldimension;
        vindex1 += vdimension;
    }
}

void Attention::calculateIntermediateBackSoftMax()
{
    if (processsoftmax) {
        double gi = 0.0;
        double gj = 0.0;
        double dij = 0.0;
        double error = 0.0;
        double delta = 0.0;
        double derivative = 0.0;
        for (int i = 0; i < intermedate_size; i++) {
            derivative = 0.0;
            gi = intermediatebuffer[i];
            for (int j = 0; j < intermedate_size; j++) {
                gj = intermediatebuffer[j];
                if (i == j) {
                    dij = 1.0;
                }
                else {
                    dij = 0.0;
                }
                derivative = (gi * (dij - gj));
                delta = intermediatebackpropoutputs[j];
                error += (delta * derivative);
            }
            intermediatesoftmaxbackpropoutputs[i] = error;
        }
    }
    else {
        for (int i = 0; i < intermedate_size; i++) {
            intermediatesoftmaxbackpropoutputs[i] = intermediatebackpropoutputs[i];
        }
    }
}

void Attention::calculateKBackprops()
{
    int qindex1 = 0;
    int oindex1 = 0;
    int kindex1 = kstart_index;
    double sum = 0.0;
    double val = 0.0;
    double invdk = 1 / sqrt((double)kdimension);
    for (int i = 0; i < modeldimension; i++) {
        for (int j = 0; j < kdimension; j++) {
            sum = 0.0;
            oindex1 = i;
            qindex1 = qstart_index + j;
            for (int k = 0; k < modeldimension; k++) {
                if (*(double*)inputpointers[qindex1] != 0.0) {
                    val = intermediatesoftmaxbackpropoutputs[oindex1] / *(double*)inputpointers[qindex1];
                    if (val > sum_add_limit) {
                        val = sum_add_limit;
                    }
                    else if (val < -sum_add_limit) {
                        val = -sum_add_limit;
                    }
                    sum += val;
                }
                oindex1 += modeldimension;
                qindex1 += modeldimension;
            }
            backpropoutputs[kindex1 + j] = invdk * sum;
        }
        kindex1 += kdimension;
    }
}

void Attention::calculateQBackprops()
{
    int kindex1 = 0;
    int oindex1 = 0;
    int qindex1 = qstart_index;
    double sum = 0.0;
    double val = 0.0;
    double invdk = 1 / sqrt((double)kdimension);
    for (int i = 0; i < modeldimension; i++) {
        for (int j = 0; j < kdimension; j++) {
            sum = 0.0;
            kindex1 = kstart_index;
            for (int k = 0; k < modeldimension; k++) {
                if (*(double*)inputpointers[kindex1 + j] != 0.0) {
                    val = intermediatesoftmaxbackpropoutputs[oindex1 + k] / *(double*)inputpointers[kindex1 + j];
                    if (val > sum_add_limit) {
                        val = sum_add_limit;
                    }
                    else if (val < -sum_add_limit) {
                        val = -sum_add_limit;
                    }
                    sum += val;
                }
                kindex1 += modeldimension;
            }
            backpropoutputs[qindex1 + j] = invdk * sum;
        }
        oindex1 += modeldimension;
        qindex1 += kdimension;
    }
}

double Attention::softmaxDerivative(double data)
{
    double sigmoid_leakage = 0.001;
    if (data > (1.0 - sigmoid_leakage)) {
        return(sigmoid_leakage);
    }
    else if (data < sigmoid_leakage) {
        return(sigmoid_leakage);
    }
    return(data * (1 - data));
}

void Attention::addPanel1String(string msg)
{
#ifdef GCC
    cout << msg << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addPanel1String((CString)msg.c_str());
#endif
}






//******************************************************************************************************

bool SelfAttention::init(string nid, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool ninput)
{
    return(allocate(nid, naddsum, nsoft, ninputs, nbackpropinputs, ninput));
}

bool SelfAttention::allocate(string nid, double naddsum, bool nsoft, int ninputs, int nbackpropinputs, bool ninput)
{
    addPanel1String("Adding self attention layer " + id);

    input_size = ninputs;

    int third = input_size / 3;
    if ((input_size % third) > 0) {
        third++;
    }

    int kdim = (int)sqrt((double)third);
    int vdim = kdim;
    int mdim = kdim;

    int test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);

    if (test_num_inputs < input_size) {
        vdim++;
        test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);
        if (test_num_inputs < input_size) {
            mdim++;
            test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);
            if (test_num_inputs < input_size) {
                kdim++;
                test_num_inputs = 2 * (2 * kdim * mdim) + (vdim * mdim);
            }
        }
    }
    
    int moffset = 0;
    if (test_num_inputs > input_size) {
        moffset = test_num_inputs - input_size;
        input_size = test_num_inputs;
    }

    //char s[200];  sprintf_s(s, "SelfAttention : allocate : kd = %i : vd = %i : d = %i : ", kdim, vdim, mdim);  addErrorString(s + id);

    Attention::allocate(nid, kdim, vdim, mdim, naddsum, nsoft, input_size, nbackpropinputs, ninput);

    offset = moffset;
    if (offset > 0) {
        char s[200];  sprintf_s(s, "SelfAttention : setting offset to %i for ", offset);
        addErrorString(s + id);
    }

    for (int i = 0; i < input_size; i++) {
        inputpointers[i] = (int64_t)&one;
    }

    addPanel1String("");

    return(true);

}

void SelfAttention::clear()
{
    Attention::clear();
    offset = 0;
    input_index = 0;
    backprop_set_index = 0;
    zero = 0.0;
    one = 1.0;
}

// ****************************************************************************
// ****************************************************************************

bool SelfAttention::join(HiddenNode* hnode)
{
    // set hnode input pointer
    if (!addInputPointer(hnode->id, &hnode->output)) {
        return(false);
    }
    // set self backprop inputs
    if (!hnode->setBackpropInputPointer(getBackpropOutputPointer(hnode->id), id)) {
        return(false);
    }
    return(true);
}

bool SelfAttention::join(HiddenConvolution* conv)
{
    if (input_size == conv->output_data_size) {
        boolInt tval = conv->addNewBackpropConnection(id);
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < conv->output_data_size; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(conv->id + s, conv->getOutputPointer(i))) {
                    addErrorString("SelfAttention::join for " + id + " for convolution " + conv->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(conv->id + s);
                if (!conv->setBackpropInputPointer2(tval.value, pointer)) {  
                    addErrorString("SelfAttention::join for " + id + " for conv " + conv->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("SelfAttention::join for " + id + " for conv " + conv->id + " problem with conv backprop index");
        }
    }
    else {
        char s[200];  sprintf_s(s, " input_data_size (%i) : output_data_size (%i)", input_size, conv->output_data_size);
        addErrorString("SelfAttention::join for " + id + " : input_data_size != output_data_size" + s);
    }
    return(true);
}

bool SelfAttention::join(HiddenNet* hnet)
{
    for (int i = 0; i < hnet->num_nodes_layer; i++) {
        // set hnode input pointer
        if (!addInputPointer(hnet->hnodes[i].id, &hnet->hnodes[i].output)) {
            addErrorString("SelfAttention::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set input pointer");
            return(false);
        }
        // set self backprop inputs
        if (!hnet->hnodes[i].setBackpropInputPointer(getBackpropOutputPointer(hnet->hnodes[i].id), id)) {
            addErrorString("SelfAttention::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set backprop pointer");
            return(false);
        }
    }
    return(true);
}

bool SelfAttention::join(NetCluster* ncluster)
{
    for (int i = 0; i < ncluster->num_outputs; i++) {
        char s[100];  sprintf_s(s, ":%i", i);
        if (!addInputPointer(ncluster->id + s, ncluster->getOutputPointer(i))) {
            addErrorString("SelfAttention::join for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
            return(false);
        }
    }
    return(true);
}

bool SelfAttention::join(Dropout* dropout)
{
    if (input_size == (dropout->num_inputs + offset)) {
        boolInt tval = dropout->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < dropout->num_inputs; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(dropout->id + s, dropout->getOutputPointer(i))) {
                    addErrorString("SelfAttention::join for " + id + " for dropout " + dropout->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(dropout->id + s);
                if (!dropout->setBackpropInputPointer(tval.value, pointer)) {  
                    addErrorString("SelfAttention::join for " + id + " for dropout " + dropout->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("SelfAttention::join for " + id + " for dropout " + dropout->id + " problem with dropout backprop index");
        }
    }
    else {
        char s[200];  sprintf_s(s, " : self attention input_size (%i) != dropout num_inputs + offset (%i)", input_size, dropout->num_inputs + offset);
        addErrorString("SelfAttention::join for " + id + s);
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************




//******************************************************************************************************

void NetCluster::clear() {
    allocated = false;
    id = "";
    type = SIGMOID;
    num_inputs = 0;
    num_outputs = 0;
    num_active_nodes = 1;
    alpha = 0.2;
    eta = 0.02;
    random_scale = 0.2;
    sigmoid_leakage = 0.01;
    relu_leakage = 0.001;
    dummy = 0.0;

    has_softmax = false;
    
    filter_alpha = 1.0 / 100.0;
    filtered_error = 1.0;
    delta_error = 1.0;

    my_filtered_error = 1.0;

    num_history = 5;
    for (int i = 0; i < num_history; i++) {
        error_history[i] = 1.0;
    }

    num_epochs = 0.0;
    num_iterations = 0;
    num_sub_iterations = 0;
    update_epochs = 5.0;
    num_test_epochs = update_epochs;
    num_update_iterations = 0;

    test_error = 0.100;
    split_variable = 1.0;

    separating_data = true;
    learning_nodes = false;
    last_realign = true;

    dataset_allocated = false;
    num_dataset = 0;
    data_allocation_size = 0;

    test_cluster_index = 0;
    num_test_dataset = 0;

    cluster_nodes = NULL;
    cluster_sort_indices = NULL;
    index_cluster_to_sub = NULL;
    index_sub_to_cluster = NULL;
    last_net_outputs = NULL;
    output_test_values = NULL;

}

void NetCluster::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void NetCluster::addErrorString(string err) {
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}

bool NetCluster::init(string nid, string nmode, int ninputs, int noutputs, int ntype, double neta, double nalpha,
    double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutput, double nepochs) {
    return(allocate(nid, nmode, ninputs, noutputs, ntype, neta, nalpha, nrandom_scale, nrandomize,
        ntrainable, nzerooutput, nepochs));
}

bool NetCluster::allocate(string nid, string nmode, int ninputs, int noutputs, int ntype, double neta, double nalpha,
    double nrandom_scale, bool nrandomize, bool ntrainable, bool nzerooutput, double nepochs)
{
    deallocate();

    clear();

    // Set net parameters
    id = nid;
    mode = nmode;
    num_inputs = ninputs;
    num_outputs = noutputs;
    update_epochs = nepochs;
    num_test_epochs = update_epochs;

    if ((num_inputs == 0) || (num_outputs == 0)) {
        char s[200];  sprintf_s(s, " : num_inputs = %i : num_outputs = %i : ", num_inputs, num_outputs);
        addErrorString((string)("Error : NetCluster input parameters equal to zero") + s + id);
        return(false);
    }

    type = ntype;
    if (type == SOFTMAX) {
        has_softmax = true;
    }

    eta = neta;
    alpha = nalpha;
    random_scale = nrandom_scale;

    testoutputs = new double[num_outputs];
    if (testoutputs == 0) {
        addErrorString("Error : NetCluster unable to allocate testoutputs");
        return(false);
    }
    for (int i = 0; i < num_outputs; i++) {
        testoutputs[i] = 0.0;
    }

    addPanel1String("Adding net " + id + " - initializing subcluster");

    subcluster.init("sub", mode, num_inputs, neta, alpha, random_scale);

    addPanel1String("Adding net " + id + " - initializing nodes");

    onodes = new OutputNode[num_outputs];
    if (onodes == 0) {
        addErrorString("Error : NetCluster unable to allocate nodes");
        return(false);
    }

    for (int i = 0; i < num_outputs; i++) {
        onodes[i].allocate(num_inputs, num_outputs, type, eta, alpha, random_scale);
        onodes[i].setTestOutputPointer((double*)(testoutputs + i));
        char tid[200];  sprintf_s(tid, "layer%iindex%i", 0, i);
        onodes[i].setID(id + tid);
        onodes[i].trainable = ntrainable;
        onodes[i].zeroout = nzerooutput;
    }

    zeroBackpropErrors();

    addPanel1String("Adding net " + id + " - randomizing weights");

    randomizeWeights();

    allocated = true;

    addPanel1String("");

    return(true);

}

bool NetCluster::initializeNetCluster(int64_t tnum)
{
    if (tnum > 0) {
        if (!allocateDataset(tnum)) {
            return(false);
        }
        setupSubClusterIndices(0);
        subcluster.initializeNetSubCluster(tnum);
        filter_alpha = 1.0 / (double)tnum;
        return(true);
    }
    return(false);
}

bool NetCluster::allocateDataset(int64_t tnum)
{
    if (dataset_allocated) {
        delete[] cluster_nodes;
        delete[] cluster_sort_indices;
        delete[] index_cluster_to_sub;
        delete[] index_sub_to_cluster;
        delete[] last_net_outputs;
        delete[] output_test_values;
    }
    num_dataset = tnum;
    num_test_dataset = num_dataset;
    num_update_iterations = (int64_t)(update_epochs * (double)num_dataset);
    data_allocation_size = num_dataset * (int64_t)num_outputs;
    cluster_nodes = new int64_t[num_dataset];
    cluster_sort_indices = new int64_t[num_dataset];
    index_cluster_to_sub = new int64_t[num_dataset];
    index_sub_to_cluster = new int64_t[num_dataset];
    last_net_outputs = new double[data_allocation_size];
    output_test_values = new double[data_allocation_size];
    if ((cluster_nodes == 0) || (cluster_sort_indices == 0) || (index_cluster_to_sub == 0) || (index_sub_to_cluster == 0) || (last_net_outputs == 0) || (output_test_values == 0)) {
        addErrorString("NetCluster::allocateDataset : allocation failure");
        dataset_allocated = false;
        return(false);
    }
    for (int64_t i = 0; i < num_dataset; i++) {
        cluster_nodes[i] = 0;
        cluster_sort_indices[i] = i;
        index_cluster_to_sub[i] = -1;
        index_sub_to_cluster[i] = -1;
    }
    for (int64_t i = 0; i < data_allocation_size; i++) {
        last_net_outputs[i] = 0.5;
        output_test_values[i] = 0.0;
    }
    dataset_allocated = true;
    //testSort();
    return(true);
}

int64_t NetCluster::setupSubClusterIndices(int cindex)
{
    int64_t cnt = 0;
    for (int64_t i = 0; i < num_dataset; i++) {
        if (cluster_nodes[i] == cindex) {
            index_cluster_to_sub[i] = cnt;
            index_sub_to_cluster[cnt] = i;
            cnt++;
        }
        else {
            index_cluster_to_sub[i] = -1;
        }
    }
    for (int64_t i = cnt; i < num_dataset; i++) {
        index_sub_to_cluster[i] = -1;
    }
    return(cnt);
}

void NetCluster::setOutputTestValuesLearning()
{
    for (int64_t j = 0; j < num_dataset; j++) {
        int64_t dindex = j * num_outputs;
        for (int i = 0; i < num_outputs; i++) {
            if (cluster_nodes[j] == i) {
                output_test_values[(int64_t)i + dindex] = 1.0 - sigmoid_leakage;
            }
            else {
                output_test_values[(int64_t)i + dindex] = sigmoid_leakage;
            }
        }
    }
}

// ****************************************************************************
// Sort cluster data by indices

void NetCluster::sortNetOutputsByCluster()
{
    quickSortOutputsByCluster(0, (num_dataset - 1));
}

void NetCluster::quickSortOutputsByCluster(int64_t lo, int64_t hi)
{
    if (lo < hi) {
        int64_t p = partitionOutputsByCluster(lo, hi);
        quickSortOutputsByCluster(lo, p - 1);
        quickSortOutputsByCluster(p + 1, hi);
    }
}

int64_t NetCluster::partitionOutputsByCluster(int64_t lo, int64_t hi)
{
    int64_t pivot = cluster_nodes[cluster_sort_indices[hi]];
    int64_t i = lo;
    int64_t cindex = 0;
    for (int64_t j = lo; j < hi; j++) {
        if (cluster_nodes[cluster_sort_indices[j]] < pivot) {
            cindex = cluster_sort_indices[i];
            cluster_sort_indices[i] = cluster_sort_indices[j];
            cluster_sort_indices[j] = cindex;
            i++;
        }
    }
    cindex = cluster_sort_indices[i];
    cluster_sort_indices[i] = cluster_sort_indices[hi];
    cluster_sort_indices[hi] = cindex;
    return(i);
}

void NetCluster::testSort()
{
    for (int64_t i = 0; i < num_dataset; i++) {
        cluster_nodes[i] = num_dataset - i;
        cluster_sort_indices[i] = i;
    }

    sortNetOutputsByCluster();

    for (int64_t i = 0; i < num_dataset; i++) {
        char s[200];  sprintf_s(s, "netcluster sortTest - i = %lli : cluster_sort_indices = %lli, cluster_nodes = %lli", i, cluster_sort_indices[i], cluster_nodes[cluster_sort_indices[i]]);
        addErrorString(s);
    }

    for (int64_t i = 0; i < num_dataset; i++) {
        cluster_nodes[i] = 0;
        cluster_sort_indices[i] = i;
    }
}

// ****************************************************************************

void NetCluster::getClusteredOutputs(vector<vector<double>>* data)
{
    data->clear();
    if (separating_data) {
        vector<double> tdata;
        data->push_back(tdata);
        subcluster.getClusteredOutputs(data);
    }
    else if (learning_nodes) {
        for (int j = 0; j < num_active_nodes; j++) {
            vector<double> tdata;
            data->push_back(tdata);
        }
        for (int64_t i = 0; i < num_dataset; i++) {
            for (int j = 0; j < num_active_nodes; j++) {
                data[0][j].push_back(last_net_outputs[(cluster_sort_indices[i] * (int64_t)num_outputs) + (int64_t)j]);
            }
        }
    }
}

double* NetCluster::getBackpropOutputPointer(int input_index, int backprop_index)
{
    if (allocated) {
        if (input_index < num_outputs) { 
            if (backprop_index < onodes[input_index].num_inputs) {
                return(onodes[input_index].getBackpropOutputPointer(onodes[input_index].getInputPointerName(backprop_index)));
            }
            else {
                char s[200];  sprintf_s(s, " - backprop index = %i, node num inputs = %i (1)", backprop_index, onodes[input_index].num_inputs);
                addErrorString("Get backprop pointer error for  " + id + s);
            }
        }
        else {
            char s[200];  sprintf_s(s, " - input_index = %i, num_nodes_layer = %i", input_index, num_outputs);
            addErrorString("Get backprop pointer error for  " + id + s);
        }
    }
    return(0);
}

double NetCluster::getBackpropError(int nindex, int eindex)
{
    if ((nindex >= 0) && (nindex < num_outputs)) {
        return(onodes[nindex].getBackpropError(eindex));
    }
    return(0.0);
}

bool NetCluster::addInputPointer(string name, double* tinput)
{
    if (allocated) {
        if (!subcluster.setInputPointer(name, tinput)) {
            return(false);
        }
        for (int i = 0; i < num_outputs; i++) {
            //char s[200];  sprintf_s(s, "NetCluster::addInputPointer : i = %i : ", i);  addErrorString(s + id + " : " + onodes[i].id + " : " + name);
            if (!onodes[i].addInputPointer(name, tinput)) {
                addErrorString("NetCluster::addInputPointer for " + id + " : error setting input " + name + " for node " + onodes[i].id);
                return(false);
            }
        }
        return(true);
    }
    else {
        addErrorString("NetCluster::addInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* NetCluster::getOutputPointer(int index)
{
    if (allocated) {
        if ((index >= 0) && (index < num_outputs)) {
            return(onodes[index].getOutputPointer());
        }
        else {
            char s[200];  sprintf_s(s, "getOutputPointer index out of bounds : %i : %i", index, num_outputs);
            addErrorString(s);
        }
    }
    return(&dummy);
}

void NetCluster::setOutputTestData(int64_t dindex)
{
    if (dindex < num_dataset) {
        int64_t offset = dindex * (int64_t)num_outputs;
        for (int i = 0; i < num_active_nodes; i++) {
            testoutputs[i] = output_test_values[offset + (int64_t)i];
        }
    }
    else {
        char s[200];  sprintf_s(s, " : index = %lli : num_dataset = %lli", dindex, num_dataset);
        addErrorString("NetCluster::setOutputTestData : dindex out of bounds : " + (string)s);
        return;
    }
}

// ****************************************************************************
// ****************************************************************************

bool NetCluster::join(HiddenNode* hnode)
{
    subcluster.onode.addInputPointer(hnode->id, &hnode->output);
    for (int i = 0; i < num_outputs; i++) {   // num_outputs here because clusters are only one layer
        //char s[200];  sprintf_s(s, "NetCluster::join : i = %i : ", i);  addErrorString(s + id + " : " + onodes[i].id + " : " + hnode->id);
        if (!onodes[i].join(hnode)) {
            return(false);
        }
    }
    return(true);
}

bool NetCluster::join(HiddenConvolution* conv)
{
    if (conv->allocated) {
        char s[100];
        for (int i = 0; i < conv->output_data_size; i++) {
            // Set inputs
            sprintf_s(s, ":%i", i);
            if (!subcluster.onode.addInputPointer(conv->id + s, conv->getOutputPointer(i))) {
                addErrorString("NetCluster::join for " + id + " for convolution " + conv->id + " unable to set input pointers");
                return(false);
            }
        }
        for (int i = 0; i < num_outputs; i++) {
            if (!onodes[i].join(conv)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("NetCluster::join for " + id + " for hidden net " + conv->id + " - conv not allocated");
        return(false);
    }
    return(true);
}

bool NetCluster::join(HiddenNet* hnet)
{
    if (hnet->num_nodes_layer > 0) {
        char s[100];
        for (int i = 0; i < hnet->num_nodes_layer; i++) {
            sprintf_s(s, ":%i", i);
            if (!subcluster.onode.addInputPointer(hnet->id + s, hnet->getOutputPointer(i))) {
                addErrorString("NetCluster::join for " + id + " for hidden net " + hnet->id + " unable to set input pointers");
                return(false);
            }
        }
        for (int i = 0; i < num_outputs; i++) {
            if (!onodes[i].join(hnet)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("NetCluster::join for " + id + " for hidden net " + hnet->id + " - no hnet outputs");
        return(false);
    }
    return(true);
}

bool NetCluster::join(NetCluster* ncluster)
{
    if (ncluster->num_outputs > 0) {  
        char s[100];
        for (int i = 0; i < ncluster->num_outputs; i++) {
            sprintf_s(s, ":%i", i);
            if (!subcluster.onode.addInputPointer(ncluster->id + s, ncluster->getOutputPointer(i))) {
                addErrorString("NetCluster::join for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
                return(false);
            }
        }
        for (int i = 0; i < num_outputs; i++) {
            if (!onodes[i].join(ncluster)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("NetCluster::join for " + id + " for cluster " + ncluster->id + " - no cluster outputs");
        return(false);
    }
    return(true);
}

bool NetCluster::join(Attention* attention)
{
    if (attention->output_size > 0) {
        char s[100];
        for (int i = 0; i < attention->output_size; i++) {
            // set attention input pointers
            sprintf_s(s, ":%i", i);
            if (!subcluster.onode.addInputPointer(attention->id + s, attention->getOutputPointer(i))) {
                addErrorString("NetCluster::join for " + id + " for attention " + attention->id + " unable to set input pointers");
                return(false);
            }
        }
        for (int i = 0; i < num_outputs; i++) {
            if (!onodes[i].join(attention)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("NetCluster::join for " + id + " for attention " + attention->id + " - no ttention outputs");
        return(false);
    }
    return(true);
}

bool NetCluster::join(SelfAttention* selfattention)
{
    return(join((Attention*)selfattention));
}

bool NetCluster::join(Dropout* dropout)
{
    if (dropout->allocated) {
        char s[100];
        for (int i = 0; i < dropout->num_inputs; i++) {
            // Set inputs
            sprintf_s(s, ":%i", i);
            if (!subcluster.onode.addInputPointer(dropout->id + s, dropout->getOutputPointer(i))) {
                addErrorString("NetCluster::join for " + id + " for dropout " + dropout->id + " unable to set input pointers");
                return(false);
            }
        }
        for (int i = 0; i < num_outputs; i++) {
            if (!onodes[i].join(dropout)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("NetCluster::join for " + id + " for dropout " + dropout->id + " - not allocated");
        return(false);
    }
    return(true);
}

bool NetCluster::join(Addition* addition)
{
    if (addition->allocated && (addition->model_dimension > 0)) {
        char s[100];
        for (int i = 0; i < addition->model_dimension; i++) {
            sprintf_s(s, ":%i", i);
            if (!subcluster.onode.addInputPointer(addition->id + s, addition->getOutputPointer(i))) {
                addErrorString("NetCluster::join for " + id + " for addition " + addition->id + " unable to set input pointers");
                return(false);
            }
        }
        for (int i = 0; i < num_outputs; i++) {
            if (!onodes[i].join(addition)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("NetCluster::join for " + id + " for addition " + addition->id + " - no addition model size");
        return(false);
    }
    return(true);
}

bool NetCluster::join(Multiplication* multiplication)
{
    if (multiplication->allocated && (multiplication->model_dimension > 0)) {
        char s[100];
        for (int i = 0; i < multiplication->model_dimension; i++) {
            sprintf_s(s, ":%i", i);
            if (!subcluster.onode.addInputPointer(multiplication->id + s, multiplication->getOutputPointer(i))) {
                addErrorString("NetCluster::join for " + id + " for multiplication " + multiplication->id + " unable to set input pointers");
                return(false);
            }
        }
        for (int i = 0; i < num_outputs; i++) {
            if (!onodes[i].join(multiplication)) {
                return(false);
            }
        }
    }
    else {
        addErrorString("NetCluster::join for " + id + " for multiplication " + multiplication->id + " - no multiplication model size");
        return(false);
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void NetCluster::deallocate() {
    if (allocated) {
        delete[] testoutputs;
        subcluster.clear();
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].deallocate();
            onodes[i].clear();
        }
        delete[] onodes;
        allocated = false;
    }
    if (dataset_allocated) {
        delete[] cluster_nodes;
        delete[] index_cluster_to_sub;
        delete[] index_sub_to_cluster;
        delete[] last_net_outputs;
        delete[] output_test_values;
        dataset_allocated = false;
    }
}

void NetCluster::clearWeights() {
    subcluster.clearWeights();
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].clearWeights();
    }
}

void NetCluster::randomizeWeights() {
    subcluster.randomizeWeights();
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].randomizeWeights();
    }
}

bool NetCluster::checkValidWeights()
{
    subcluster.checkValidWeights();
    for (int i = 0; i < num_outputs; i++) {
        if (!onodes[i].checkValidWeights()) {
            return(false);
        }
    }
    return(true);
}

void NetCluster::forward()
{
    if (separating_data) {
        subcluster.forward();
    }
    else {
        for (int i = 0; i < num_active_nodes; i++) {
            onodes[i].forward();
        }
        softmaxNormalizeOutputLayers();
    }
}

void NetCluster::backward(int64_t dindex)
{
    if (separating_data) {
        filtered_error = subcluster.filtered_error;
        int64_t sindex = index_cluster_to_sub[dindex];
        if (sindex >= 0) {
            subcluster.backward(sindex);
        }
        copySubClusterBackpropErrors(sindex);
    }
    else {
        setOutputTestData(dindex);
        if (dindex < num_dataset) {
            int64_t offset = dindex * (int64_t)num_outputs;
            for (int i = 0; i < num_active_nodes; i++) {
                last_net_outputs[offset + (int64_t)i] = onodes[i].getOutput();
            }
        }
        getFilteredError();
        softmaxOutputLayerBackpropProcessing();
        for (int i = 0; i < num_active_nodes; i++) {
            if ((i != num_active_nodes) || !separating_data) {
                onodes[i].backward();
            }
        }
    }
    updateNetCluster();
}

void NetCluster::softmaxNormalizeOutputLayers()
{
    if (has_softmax && !separating_data) {
        double softmax_sum = 0.0;
        for (int i = 0; i < num_active_nodes; i++) {
            softmax_sum += onodes[i].getOutput();
        }
        if (softmax_sum < 0.0) {
            softmax_sum = -1 * softmax_sum;
        }
        if (softmax_sum > 0.0) {
            for (int i = 0; i < num_active_nodes; i++) {
                onodes[i].setOutput(onodes[i].getOutput() / softmax_sum);
            }
        }
    }
}

void NetCluster::softmaxOutputLayerBackpropProcessing()
{
    if (has_softmax && !separating_data) {
        double gi = 0.0;
        double gj = 0.0;
        double dij = 0.0;
        double error = 0.0;
        double delta = 0.0;
        double derivative = 0.0;
        for (int i = 0; i < num_active_nodes; i++) {
            error = 0.0;
            gi = onodes[i].getOutput();
            for (int j = 0; j < num_active_nodes; j++) {
                gj = onodes[j].getOutput();
                if (i == j) {
                    dij = 1.0;
                }
                else {
                    dij = 0.0;
                }
                derivative = (gi * (dij - gj));
                delta = onodes[j].getBackpropSum();
                error += (delta * derivative);
            }
            onodes[i].setSoftmaxError(error);
        }
    }
}

void NetCluster::realignClusters()
{
    for (int64_t j = 0; j < num_dataset; j++) {
        int max_index = -1;
        double max_output = 0.0;
        int64_t dindex = j * num_outputs;
        for (int i = 0; i < num_active_nodes; i++) {
            double toutput = last_net_outputs[dindex + i];
            if (toutput > max_output) {
                max_index = i;
                max_output = toutput;
            }
            if (max_index >= 0) {
                cluster_nodes[j] = max_index;
            }
        }
    }
}

double NetCluster::getClusterRMSError(int cindex)
{
    // Accumulate errors for all data points for selected cluster index and calculate rms error
    if (cindex < num_active_nodes) {
        double err = 0.0;
        double delta = 0.0;
        for (int64_t j = 0; j < num_dataset; j++) {
            int64_t dindex = j * num_outputs;
            delta = output_test_values[(int64_t)cindex + dindex] - last_net_outputs[(int64_t)cindex + dindex];
            err += (delta * delta);
        }
        if (err > 0.0) {
            err = err / (double)num_dataset;
            err = sqrt(err);
            return(err);
        }
        else {
            return(0.0);
        }
    }
    return(1.0);
}

double NetCluster::getClusterStdError(int cindex)
{
    // Accumulate errors for all data points for selected cluster index and calculate rms error
    if (cindex < num_active_nodes) {
        int64_t cnt = 0;
        double sum = 0.0;
        double sum2 = 0.0;
        for (int64_t j = 0; j < num_dataset; j++) {
            if (cluster_nodes[j] == cindex) {
                int64_t dindex = j * num_outputs;
                double value = output_test_values[(int64_t)cindex + dindex] - last_net_outputs[(int64_t)cindex + dindex];
                sum += value;
                sum2 += (value * value);
                cnt++;
            }
        }
        if (cnt > 2) {
            double std = (sum2 - (sum * sum) / (double)cnt) / (double)cnt;
            if (std > 0) {
                std = sqrt(std);
            }
            std = std * (double)pow((double)cnt, split_variable);
            return(std);
        }
        return(0.0);
    }
    return(1.0);
}

void NetCluster::copySubClusterBackpropErrors(int64_t sindex)
{
    if (sindex >= 0) {
        for (int i = 0; i < num_inputs; i++) {
            onodes[test_cluster_index].setBackpropError(i, subcluster.getBackpropError(i));
        }
    }
    else {
        onodes[test_cluster_index].zeroBackpropErrors();
    }
}

void NetCluster::zeroBackpropErrors()
{
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].zeroBackpropErrors();
    }
}

void NetCluster::updateNetCluster()
{
    num_iterations++;
    if (num_iterations > num_dataset) {
        num_iterations = 0;
        num_epochs++;
    }

    num_sub_iterations++;
    if (num_sub_iterations >= num_update_iterations) {

        my_filtered_error = 1.0;

        if (separating_data) {

            subcluster.updateNetCluster();

            my_filtered_error = subcluster.filtered_error;

        }
        else if (learning_nodes) {

            my_filtered_error = filtered_error;

        }

        if ((separating_data && (my_filtered_error < test_error)) || (learning_nodes && (my_filtered_error < test_error/*0.10*/))) {

            for (int i = 0; i < num_history; i++) {
                error_history[i] = 1.0;
            }

            if (separating_data) {

                //addTrainString("separating_data");

                int64_t split = subcluster.splitCluster();
                for (int64_t i = 0; i < split; i++) {
                    cluster_nodes[index_sub_to_cluster[subcluster.cluster_sort_indices[i]]] = num_active_nodes;
                }
                num_active_nodes++;

                sortNetOutputsByCluster();

                setOutputTestValuesLearning();

                separating_data = false;
                learning_nodes = true;

            }
            else if (learning_nodes) {

                //addTrainString("learning_nodes");

                if (num_active_nodes < num_outputs) {

                    realignClusters();

                    setupNextSubCluster();

                    separating_data = true;
                    learning_nodes = false;

                }
                else if (last_realign) {
                    realignClusters();
                    last_realign = false;
                }

            }
            else {
            }

            updateGraphLabel();

        }

        //char s[200];  sprintf_s(s, " : num_active_nodes %i : num_test_dataset %lli : error = %.4f", num_active_nodes, num_test_dataset, my_filtered_error);
        //addTrainString(getErrorString()); // "updateNetCluster : " + id + s);

        num_sub_iterations = 0;
    }
}

string NetCluster::getErrorString()
{
    char s[200];  sprintf_s(s, " error = %.4f", my_filtered_error);
    string rval = id + s;
    return(rval);
}

void NetCluster::setupNextSubCluster()
{
    if ((split_variable == 1.0) || (num_active_nodes < 2)) {
        // Choose next cluster to split by largest size
        int64_t max = 0;
        int64_t index = 0;
        vector<int> ncnt;
        // Initialize empty cluster counts
        for (int64_t i = 0; i < num_outputs; i++) {
            int cnt = 0;
            ncnt.push_back(cnt);
        }
        // Count population of each of my clusters
        for (int64_t j = 0; j < num_dataset; j++) {
            index = cluster_nodes[cluster_sort_indices[j]];
            if ((index >= 0) && (index < num_outputs)) {
                ncnt[index]++;
            }
        }
        // Find the cluster with the highest population
        for (int64_t i = (num_outputs - 1); i >= 0; i--) {
            if (ncnt[i] >= max) {
                max = ncnt[i];
                index = i;
            }
        }
        // Set test cluster index and num in max cluster
        test_cluster_index = (int)index;;
        num_test_dataset = max;
    }
    else {
        // Choose next cluster to split by largest standard deviation in test output values
        int mindex = -1;
        double merror = 0.0;
        // Find the cluster with the highest error
        for (int i = 0; i < num_active_nodes; i++) {
            double err = getClusterStdError(i);
            if (err > merror) {
                mindex = i;
                merror = err;
            }
        }
        if (mindex >= 0) {
            // Set test cluster index and num in max cluster
            num_test_dataset = 0;
            for (int64_t j = 0; j < num_dataset; j++) {
                if (cluster_nodes[cluster_sort_indices[j]] == mindex) {
                    num_test_dataset++;
                }
            }
            test_cluster_index = mindex;
        }
        else {
            AfxMessageBox(_T("setupNextSubCluster : error finding mindex"));
            return;
        }
    }
    // Set up and initialize sub cluster
    setupSubClusterIndices(test_cluster_index);
    subcluster.initializeNetSubCluster(num_test_dataset);
    zeroBackpropErrors();
}

double NetCluster::getFilteredError()
{
    if (learning_nodes) {
        double error = 0.0;
        for (int i = 0; i < num_active_nodes; i++) {
            double terr = onodes[i].getOutput() - testoutputs[i];
            if (terr < 0.0) {
                terr = -terr;
            }
            if (terr > error) {
                error = terr;
            }
        }
        filtered_error = (1.0 - filter_alpha) * filtered_error + filter_alpha * error;
    }
    return(filtered_error);
}

double NetCluster::getCumulativeError(int64_t dindex)
{
    double error = 0;
    if (separating_data) {
        int64_t mindex = index_cluster_to_sub[dindex];
        if (mindex >= 0) {
            subcluster.setOutputTestData(mindex);
            subcluster.getFilteredError();
            error = subcluster.getError();
        }
    }
    else if (learning_nodes) {
        if (num_active_nodes > 0) {
            setOutputTestData(dindex);
            getFilteredError();
            for (int i = 0; i < num_active_nodes; i++) {
                error += (onodes[i].getOutput() - testoutputs[i]) * (onodes[i].getOutput() - testoutputs[i]);
            }
            error = error / (double)num_active_nodes;
            if (error > 0) {
                error = sqrt(error);
            }
            error = (double)num_active_nodes * error;
        }
    }
    return(error);
}

void NetCluster::updateGraphLabel()
{
#ifndef GCC
    CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
    CString s = _T("");
    if (separating_data) {
        s.Format(_T(" : Learning %i of %i"), num_active_nodes, num_outputs);
    }
    else if (learning_nodes) {
        s.Format(_T(" : Training %i of %i"), num_active_nodes, num_outputs);
    }
    CString tid = (CString)id.c_str();
    pMainWnd->m_wndClusterGraph.setTitle(tid + s);
#endif
}

bool NetCluster::finished()
{
    if (mode == "Output") {
        return(false);
    }
    return(false);
}

void NetCluster::setParameters(double sleak, double rleak) {
    if (allocated) {
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].sigmoid_leakage = sleak;
            onodes[i].relu_leakage = rleak;
        }
    }
}

void NetCluster::setParameters2(double testerror, double splitvariable, string splitmethod)
{
    test_error = testerror;
    split_variable = splitvariable;
    if (split_variable < 0.0) {
        split_variable = 0.0;
    }
    else if (split_variable > 1.0) {
        split_variable = 1.0;
    }
    if (splitmethod == "Adaptive") {
        subcluster.adaptive_split = true;
    }
    else {
        subcluster.adaptive_split = false;
    }
}

int NetCluster::getNumWeights()
{
    int num_weights = 0;
    for (int i = 0; i < num_outputs; i++) {
        num_weights += onodes[i].num_inputs;
    }
    return(num_weights);
}

double NetCluster::getWeightSquares()
{
    double square_weights = 0;
    for (int i = 0; i < num_outputs; i++) {
        square_weights += onodes[i].squareWeights();
    }
    return(square_weights);
}

double NetCluster::maxWeights()
{
    double max = 0;
    for (int i = 0; i < num_outputs; i++) {
        double tmax = onodes[i].maxWeights();
        if (tmax > max) {
            max = tmax;
        }
    }
    return(max);
}

void NetCluster::setTrainable(bool val)
{
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].setTrainable(val);
    }
}

void NetCluster::setZeroOut(bool val)
{
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].setZeroOut(val);
    }
}

void NetCluster::setInputToZero(string inputid)
{
    if (allocated) {
        subcluster.onode.setInputToZero(inputid);
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].setInputToZero(inputid);
        }
    }
}

void NetCluster::setInputToData(string inputid)
{
    if (allocated) {
        subcluster.onode.setInputToData(inputid);
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].setInputToData(inputid);
        }
    }
}

hiddenWeightData NetCluster::getWeights()
{
    hiddenWeightData rval;
    if (allocated) {
        rval.id = id;
        for (int j = 0; j < num_outputs; j++) {
            rval.nodeweightdata.push_back(onodes[j].getWeights());
        }
    }
    return(rval);
}

void NetCluster::setWeights(hiddenWeightData cwd)
{
    if (allocated) {
        if (cwd.id == id) {
            int i = 0;
            bool done = false;
            bool name_search = false;
            while (!done) {
                if ((i < num_outputs) && (i < (int)cwd.nodeweightdata.size())) {
                    if (cwd.nodeweightdata[i].id == onodes[i].id) {
                        onodes[i].setWeights(cwd.nodeweightdata[i]);
                    }
                    else {
                        i = num_outputs;
                        name_search = true;
                        done = true;
                    }
                    i++;
                }
                else {
                    done = true;
                }
            }
            if (name_search) {
                for (i = 0; i < (int)cwd.nodeweightdata.size(); i++) {
                    for (int j = 0; j < num_outputs; j++) {
                        if (cwd.nodeweightdata[i].id == onodes[j].id) {
                            onodes[j].setWeights(cwd.nodeweightdata[i]);
                            j = num_outputs;
                        }
                    }
                }
            }
        }
    }
}

void NetCluster::archiveWeights()
{
    if (allocated) {
        for (int j = 0; j < num_outputs; j++) {
            onodes[j].archiveWeights();
        }
    }
}

void NetCluster::restoreWeights()
{
    if (allocated) {
        for (int j = 0; j < num_outputs; j++) {
            onodes[j].restoreWeights();
        }
    }
}

void NetCluster::addPanel1String(string msg)
{
#ifdef GCC
    cout << msg << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addPanel1String((CString)msg.c_str());
#endif
}










//******************************************************************************************************

void SubCluster::clear() {
    allocated = false;
    id = "";
    type = SIGMOID;
    num_inputs = 0;
    alpha = 0.2;
    eta = 0.02;
    random_scale = 0.2;
    sigmoid_leakage = 0.01;
    relu_leakage = 0.001;
    dummy = 0.0;

    filter_alpha = 1.0 / 100.0;
    filtered_error = 1.0;
    delta_error = 1.0;

    num_history = 5;
    for (int i = 0; i < num_history; i++) {
        error_history[i] = 1.0;
    }

    dataset_allocated = false;
    num_dataset = 0;

    adaptive_split = false;
    index2percent = (2 * num_dataset) / 100;
    index98percent = (98 * num_dataset) / 100;

    cluster_nodes = NULL;
    cluster_sort_indices = NULL;
    last_net_outputs = NULL;
    output_test_values = NULL;

}

void SubCluster::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void SubCluster::addErrorString(string err) {
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}

bool SubCluster::init(string nid, string nmode, int ninputs, double neta, double nalpha, double nrandom_scale) {
    return(allocate(nid, nmode, ninputs, neta, nalpha, nrandom_scale));
}

bool SubCluster::allocate(string nid, string nmode, int ninputs, double neta, double nalpha, double nrandom_scale)
{
    deallocate();

    clear();

    // Set net parameters
    id = nid;
    mode = nmode;
    num_inputs = ninputs;

    if (num_inputs == 0) {
        char s[200];  sprintf_s(s, " : num_inputs = %i : ", num_inputs);
        addErrorString((string)("Error : NetCluster input parameters equal to zero") + s + id);
        return(false);
    }

    type = SIGMOID;

    eta = neta;
    alpha = nalpha;
    random_scale = nrandom_scale;

    // Allocate node
    addPanel1String("Adding sub cluster " + id + " - initializing node");

    onode.allocate(num_inputs, 1, type, eta, alpha, random_scale);
    onode.setTestOutputPointer(&testoutput);
    onode.setID(id);
 
    addPanel1String("Adding net " + id + " - randomizing weights");

    randomizeWeights();

    allocated = true;

    addPanel1String("");

    return(true);

}

bool SubCluster::initializeNetSubCluster(int64_t tnum)
{
    if (tnum > 0) {
        if (!allocateDataset(tnum)) {
            return(false);
        }
        onode.randomizeWeights();
        randomizeClusterIndices();
        setOutputTestValuesSorting();
        filter_alpha = 1.0 / (double)tnum;
        return(true);
    }
    return(false);
}

bool SubCluster::allocateDataset(int64_t tnum)
{
    if (dataset_allocated) {
        delete[] cluster_nodes;
        delete[] cluster_sort_indices;
        delete[] last_net_outputs;
        delete[] output_test_values;
    }
    num_dataset = tnum;
    cluster_nodes = new int64_t[num_dataset];
    cluster_sort_indices = new int64_t[num_dataset];
    last_net_outputs = new double[num_dataset];
    output_test_values = new double[num_dataset];
    if ((cluster_nodes == 0) || (cluster_sort_indices == 0) || (last_net_outputs == 0) || (output_test_values == 0)) {
        addErrorString("NetCluster::allocateDataset : allocation failure");
        dataset_allocated = false;
        return(false);
    }
    for (int64_t i = 0; i < num_dataset; i++) {
        cluster_nodes[i] = 0;
        cluster_sort_indices[i] = i;
        last_net_outputs[i] = 0.5;
        output_test_values[i] = 0.0;
    }
    index2percent = (2 * num_dataset) / 100;
    index98percent = (98 * num_dataset) / 100;
    dataset_allocated = true;
    //testSort();
    return(true);
}

void SubCluster::randomizeClusterIndices()
{
    if (dataset_allocated && (num_dataset > 0)) {
        vector<int64_t> my_nodes;
        for (int64_t i = 0; i < num_dataset; i++) {
            my_nodes.push_back(0);
        }
        int64_t half = num_dataset / 2;
        for (int64_t i = 0; i < half; i++) {
            int64_t tindex = (num_dataset * (int64_t)rand()) / (int64_t)RAND_MAX;
            if ((tindex < num_dataset) && (tindex >= 0)) {
                if (my_nodes[tindex] == 0) {
                    my_nodes[tindex] = 1;
                }
                else {
                    do {
                        tindex++;
                        if (tindex >= num_dataset) {
                            tindex = 0;
                        }
                    } while (my_nodes[tindex] != 0);
                    my_nodes[tindex] = 1;
                }
            }
            else {
                // sometimes we land here
            }
        }
        for (int64_t i = 0; i < num_dataset; i++) {
            cluster_nodes[i] = my_nodes[i];
        }
    }
    else {
        addErrorString("randomizeClusterIndices : not allocated or no dataset");
    }
}

// ****************************************************************************
// sort cluster indices by most recent output values

void SubCluster::sortNetOutputs(int64_t size)
{
    quickSortOutputs(0, size);
}

void SubCluster::quickSortOutputs(int64_t lo, int64_t hi)
{
    if (lo < hi) {
        int64_t p = partitionOutputs(lo, hi);
        quickSortOutputs(lo, p - 1);
        quickSortOutputs(p + 1, hi);
    }
}

int64_t SubCluster::partitionOutputs(int64_t lo, int64_t hi)
{
    double pivot = last_net_outputs[cluster_sort_indices[hi]];
    int64_t i = lo;
    int64_t cindex = 0;
    for (int64_t j = lo; j < hi; j++) {
        if (last_net_outputs[cluster_sort_indices[j]] < pivot) {
            cindex = cluster_sort_indices[i];
            cluster_sort_indices[i] = cluster_sort_indices[j];
            cluster_sort_indices[j] = cindex;
            i = i + 1;
        }
    }
    cindex = cluster_sort_indices[i];
    cluster_sort_indices[i] = cluster_sort_indices[hi];
    cluster_sort_indices[hi] = cindex;
    return(i);
}

void SubCluster::testSort()
{
    for (int64_t i = 0; i < num_dataset; i++) {
        cluster_sort_indices[i] = i;
        last_net_outputs[i] = (double)(num_dataset - i) * 0.5;
    }

    sortNetOutputs(num_dataset - 1);

    for (int64_t i = 0; i < num_dataset; i++) {
        char s[200];  sprintf_s(s, "subcluster sortTest - i = %lli : cluster_sort_indices = %lli, last_net_outputs = %.3f", i, cluster_sort_indices[i], last_net_outputs[cluster_sort_indices[i]]);
        addErrorString(s);
    }

    for (int64_t i = 0; i < num_dataset; i++) {
        cluster_sort_indices[i] = i;
        last_net_outputs[i] = 0.5;
    }
}

// ****************************************************************************

void SubCluster::getClusteredOutputs(vector<vector<double>>* data)
{
    data[0][0].clear();
    for (int64_t i = 0; i < num_dataset; i++) {
        data[0][0].push_back(last_net_outputs[cluster_sort_indices[i]]);
    }
}

double* SubCluster::getBackpropOutputPointer(int backprop_index)
{
    if (allocated) {
        if (backprop_index < onode.num_inputs) {
            return(onode.getBackpropOutputPointer(onode.getInputPointerName(backprop_index)));
        }
        else {
            char s[200];  sprintf_s(s, " - backprop index = %i, node num inputs = %i", backprop_index, onode.num_inputs);
            addErrorString("Get backprop pointer error for  " + id + s);
        }
    }
    return(0);
}

double SubCluster::getBackpropError(int eindex)
{
    return(onode.getBackpropError(eindex));
}

void SubCluster::deallocate() {
    if (allocated) {
        onode.deallocate();
        allocated = false;
    }
    if (dataset_allocated) {
        delete[] cluster_nodes;
        delete[] last_net_outputs;
        delete[] output_test_values;
        dataset_allocated = false;
    }
}

void SubCluster::clearWeights() 
{
    onode.clearWeights();
}

void SubCluster::randomizeWeights()
{
    onode.randomizeWeights();
}

bool SubCluster::checkValidWeights()
{
    if (!onode.checkValidWeights()) {
        return(false);
    }
    return(true);
}

bool SubCluster::setInputPointer(string name, double* tinput)
{
    if (allocated) {
        if (onode.addInputPointer(name, tinput)) {
            return(true);
        }
        else {
            addErrorString("setInputPointer for " + id + " : unable to find input " + name);
        }
    }
    else {
        addErrorString("setInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

void SubCluster::setOutputTestData(int64_t dindex)
{
    if (dindex < num_dataset) {
        testoutput = output_test_values[dindex];
    }
    else {
        char s[200];  sprintf_s(s, " : index = %lli : num_dataset = %lli", dindex, num_dataset);
        addErrorString("SubCluster::setOutputTestData : dindex out of bounds : " + (string)s);
        return;
    }
}

double SubCluster::getFilteredError()
{
    double error = getError();
    filtered_error = (1.0 - filter_alpha) * filtered_error + filter_alpha * error;
    return(filtered_error);
}

double SubCluster::getError()
{
    double error = onode.getOutput() - testoutput;
    if (error < 0.0) {
        error = -error;
    }
    return(error);
}

void SubCluster::forward()
{
    onode.forward();
}

void SubCluster::backward(int64_t dindex)
{
    if (dindex < num_dataset) {
        last_net_outputs[dindex] = onode.getOutput();
    }
    setOutputTestData(dindex);
    getFilteredError();
    onode.backward();
}

void SubCluster::zeroBackpropErrors()
{
    onode.zeroBackpropErrors();
}

void SubCluster::updateNetCluster()
{
    // Sort input data indices by most recent output values
    sortNetOutputs(num_dataset - 1);

    // Initialize cluster nodes to zero
    for (int64_t i = 0; i < num_dataset; i++) {
        cluster_nodes[i] = 0;
    }

    // Get cluster split index 
    int64_t half = splitCluster();

    // Ser cluster nodes at indices half anf above to 1
    for (int64_t i = half; i < num_dataset; i++) {
        cluster_nodes[cluster_sort_indices[i]] = 1;
    }

    // Set output train values by cluster nodes
    setOutputTestValuesSorting();

}

int64_t SubCluster::splitCluster()
{
    // Set default split to median
    int64_t split = num_dataset / 2;

    // Check if adaptive split
    if (adaptive_split) {

        // Adaptive split tries to set at median output value

        // Set adaptive split to default (median)
        int64_t split2 = split;

        // Min and max output values
        double twopercent = last_net_outputs[cluster_sort_indices[index2percent]];
        double ninetyeightpercent = last_net_outputs[cluster_sort_indices[index98percent]];

        // Get median output value
        double medianoutput = (ninetyeightpercent - twopercent) / 2;

        // Find split where output value just exceeds median
        for (int64_t i = 0; i < num_dataset; i++) {
            if (last_net_outputs[cluster_sort_indices[i]] > medianoutput) {
                split2 = i;
                i = num_dataset;
            }
        }

        // delta is an ad hoc variable used to pust split toward median if 
        //  the difference between max and min outputs is small
        double delta = (ninetyeightpercent - twopercent) / 0.98;
        if (delta < 0.02) {
            delta = 0.02;
        }
        else if (delta > 0.98) {
            delta = 0.98;
        }

        // Calculate adaptive split with control based on delta between min and max outputs
        split = (int64_t)(delta * (double)split2 + (1.0 - delta) * (double)split);

    }

    return(split);
}

void SubCluster::setOutputTestValuesSorting()
{
    // For all dataset indices
    for (int64_t j = 0; j < num_dataset; j++) {
        // Initialize output train values to minimum train value
        output_test_values[j] = sigmoid_leakage;
        // If cluster node is 1
        if (cluster_nodes[j] == 1) {
            // Set output train value to maximum train value
            output_test_values[j] = 1.0 - sigmoid_leakage;
        }
    }
}

void SubCluster::setParameters(double sleak, double rleak) {
    if (allocated) {
        onode.sigmoid_leakage = sleak;
        onode.relu_leakage = rleak;
    }
}

int SubCluster::getNumWeights()
{
    return(onode.num_inputs);
}

double SubCluster::getWeightSquares()
{
    return(onode.squareWeights());
}

double SubCluster::maxWeights()
{
    return(onode.maxWeights());
}

void SubCluster::addPanel1String(string msg)
{
#ifdef GCC
    cout << msg << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addPanel1String((CString)msg.c_str());
#endif
}




//******************************************************************************************************

void Dropout::clear()
{
    allocated = false;
    id = "";
    zero = 0.0;
    dummy = 0.0;
    percent = 0.0;
    num_inputs = 0;
    input_index = 0;
    num_output_connections = 0;
    output_connection_index = -1;
    backproppointerssize = 0;
    backprop_set_index = 0;
    drop_point = 100;
    num_drops = 0;
}

bool Dropout::init(string nid, int ninputs, double npercent, int noutconnections)
{
    return(allocate(nid, ninputs, npercent, noutconnections));
}

bool Dropout::allocate(string nid, int ninputs, double npercent, int noutconnections)
{
    deallocate();

    id = nid;
    percent = npercent;
    num_inputs = ninputs;
    num_output_connections = noutconnections;

    outputdata = new double[num_inputs];
    inputpointers = new int64_t[num_inputs];
    backupinputpointers = new int64_t[num_inputs];
    inputpointernames = new string[num_inputs];
    backpropdatabuffer = new double[num_inputs];
    drop_indices = new int64_t[num_inputs];
    if ((outputdata == 0) || (inputpointers == 0) || (backupinputpointers == 0) || (inputpointernames == 0) || (backpropdatabuffer == 0) || (drop_indices == 0)) {
        addErrorString("Dropout::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < num_inputs; i++) {
        outputdata[i] = 0.0;
        inputpointers[i] = (int64_t)&dummy;
        backupinputpointers[i] = (int64_t)&dummy;
        inputpointernames[i] = "";
        backpropdatabuffer[i] = 0.0;
        drop_indices[i] = 0;
    }

    backproppointerssize = num_output_connections * num_inputs;
    backpropinputpointers = new int64_t[backproppointerssize];
    if (backpropinputpointers == 0) {
        addErrorString("Dropout::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < backproppointerssize; i++) {
        backpropinputpointers[i] = (int64_t)&dummy;
    }

    backpropinputindices = new int[num_output_connections];
    if (backpropinputindices == 0) {
        addErrorString("Dropout::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < num_output_connections; i++) {
        backpropinputindices[i] = 0;
    }

    setNextDropPoint();

    allocated = true;

    return(true);
}

void Dropout::deallocate()
{
    if (allocated) {
        delete[] outputdata;
        delete[] inputpointers;
        delete[] backupinputpointers;
        delete[] inputpointernames;
        delete[] backpropdatabuffer;
        delete[] backpropinputpointers;
        delete[] backpropinputindices;
        delete[] drop_indices;
    }
    allocated = false;
}

bool Dropout::setNextDropPoint()
{
    if (percent > 0.0) {
        double tnum = 1.0 / percent; // ((double)num_inputs / percent);
        drop_point += (int)((tnum * (1.0 + ((double)rand() / (double)RAND_MAX))) / 2.0);
        if (drop_point < 0) {
            drop_point = 0;
        }
        //char s[200];  sprintf_s(s, "setNextDropPoint : drop_point = %i : num_inputs = %i : ", drop_point, num_inputs);  addErrorString(s + (string)id);
        return(true);
    }
    return(false);
}

bool Dropout::addInputPointer(string name, double* tinput)
{
    if (allocated) {
        if (input_index < num_inputs) {
            inputpointers[input_index] = (int64_t)tinput;
            backupinputpointers[input_index] = (int64_t)tinput;
            inputpointernames[input_index] = name;
            input_index++;
            return(true);
        }
        char s[200];  sprintf_s(s, " : input_index = %i : num_inputs = %i", input_index, num_inputs);
        addErrorString("Dropout::addInputPointer for " + id + " from " + name + s);
    }
    else {
        addErrorString("Dropout::addInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* Dropout::getOutputPointer(int index)
{
    if (allocated && (index < num_inputs)) {
        return(&outputdata[index]);
    }
    addErrorString("Dropout::getOutputPointer unable to get output pointer");
    return(&dummy);
}

bool Dropout::setBackpropInputPointer(int connection_index, double* tprop)
{
    if (allocated) {
        if (connection_index < num_output_connections) {
            if (backpropinputindices[connection_index] < num_inputs) {
                int index = (connection_index * num_inputs) + backpropinputindices[connection_index];
                backpropinputpointers[index] = (int64_t)tprop;
                backpropinputindices[connection_index]++;
                return(true);
            }
        }
        char s[200];  sprintf_s(s, " (%i) : ", num_inputs);
        addErrorString("Dropout::setBackpropInputPointer for " + id + " : index > num_allocated_backprop_inputs" + s);
    }
    else {
        addErrorString("Dropout::setBackpropInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* Dropout::getBackpropOutputPointer(string name) {
    if (allocated) {
        if (backprop_set_index < num_inputs) {
            if (name == inputpointernames[backprop_set_index]) {
                double* rval = &backpropdatabuffer[backprop_set_index];
                backprop_set_index++;
                if (backprop_set_index > num_output_connections) {
                    backprop_set_index = 0;
                }
                return(rval);
            }
        }
        for (int i = 0; i < num_inputs; i++) {
            if (name == inputpointernames[i]) {
                double* rval = &backpropdatabuffer[i];
                backprop_set_index = i + 1;
                if (backprop_set_index > num_inputs) {
                    backprop_set_index = 0;
                }
                return(rval);
            }
        }
        addErrorString("Dropout::getBackpropOutputPointer for " + id + " : unable to find " + name + " : example " + inputpointernames[0]);
    }
    else {
        addErrorString("Dropout::getBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

boolInt Dropout::addNewBackpropConnection()
{
    boolInt rval;
    rval.valid = false;
    rval.value = -1;
    if (output_connection_index < (num_output_connections - 1)) {
        output_connection_index++;
        rval.value = output_connection_index;
        rval.valid = true;
    }
    return(rval);
}


// ****************************************************************************
// ****************************************************************************

bool Dropout::join(HiddenNode* hnode)
{
    // set hnode input pointer
    if (!addInputPointer(hnode->id, &hnode->output)) {
        addErrorString("Dropout::join for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
        return(false);
    }
    // set self backprop inputs
    if (!hnode->setBackpropInputPointer(getBackpropOutputPointer(hnode->id), id)) {
        addErrorString("Dropout::join for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
        return(false);
    }
    return(true);
}

bool Dropout::join(HiddenConvolution* conv)
{
    if (num_inputs == conv->output_data_size) {
        boolInt tval = conv->addNewBackpropConnection(id);
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < conv->output_data_size; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(conv->id + s, conv->getOutputPointer(i))) {
                    addErrorString("Dropout::join for " + id + " for convolution " + conv->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(conv->id + s);
                if (!conv->setBackpropInputPointer2(tval.value, pointer)) {  
                    addErrorString("Dropout::join for " + id + " for conv " + conv->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("Dropout::join for " + id + " for conv " + conv->id + " problem with conv backprop index");
        }
    }
    else {
        char s[200];  sprintf_s(s, " input_data_size (%i) : output_data_size (%i)", num_inputs, conv->output_data_size);
        addErrorString("Dropout::join for " + id + " : input_data_size != output_data_size" + s);
    }
    return(true);
}

bool Dropout::join(HiddenNet* hnet)
{
    for (int i = 0; i < hnet->num_nodes_layer; i++) {
        // set hnode input pointer
        if (!addInputPointer(hnet->hnodes[i].id, &hnet->hnodes[i].output)) {
            addErrorString("Dropout::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set input pointer");
            return(false);
        }
        // set self backprop inputs
        if (!hnet->hnodes[i].setBackpropInputPointer(getBackpropOutputPointer(hnet->hnodes[i].id), id)) {
            addErrorString("Dropout::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set backprop pointer");
            return(false);
        }
    }
    return(true);
}

bool Dropout::join(NetCluster* ncluster)
{
    for (int i = 0; i < ncluster->num_outputs; i++) {
        char s[100];  sprintf_s(s, ":%i", i);
        if (!addInputPointer(ncluster->id + s, ncluster->getOutputPointer(i))) {
            addErrorString("Dropout::join for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
            return(false);
        }
    }
    return(true);
}

bool Dropout::join(Attention* attention)
{
    boolInt tval = attention->addNewBackpropConnection(id);
    if (tval.valid) {
        char s[100];
        for (int i = 0; i < attention->output_size; i++) {
            // set attention input pointers
            sprintf_s(s, ":%i", i);
            if (!addInputPointer(attention->id + s, attention->getOutputPointer(i))) {
                addErrorString("Dropout::join for " + id + " for attention " + attention->id + " unable to set input pointers");
                return(false);
            }
            // set self backprop inputs
            double* pointer = getBackpropOutputPointer(attention->id + s);
            if (!attention->setBackpropInputPointer(tval.value, pointer)) {  
                addErrorString("Dropout::join for " + id + " for attention " + attention->id + " unable to set backprop pointer");
                return(false);
            }
        }
    }
    else {
        addErrorString("Dropout::join for " + id + " for dropout " + attention->id + " problem with backprop index");
    }
    return(true);
}

bool Dropout::join(Addition* addition)
{
    if (addition->allocated && (addition->model_dimension > 0)) {
        boolInt tval = addition->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < addition->model_dimension; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(addition->id + s, addition->getOutputPointer(i))) {
                    addErrorString("Dropout::join for " + id + " for addition " + addition->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(addition->id + s);
                if (!addition->setBackpropInputPointer(tval.value, pointer)) {
                    addErrorString("Dropout::join for " + id + " for addition " + addition->id + " unable to set backprop pointer");
                    return(false);
                }
            }
            return(true);
        }
        else {
            addErrorString("Dropout::join for " + id + " for addition " + addition->id + " problem with addition backprop index");
        }
    }
    return(true);
}

bool Dropout::join(Multiplication* multiplication)
{
    if (multiplication->allocated && (multiplication->model_dimension > 0)) {
        boolInt tval = multiplication->addNewBackpropConnection();
        if (tval.valid) {
            char s[100];
            for (int i = 0; i < multiplication->model_dimension; i++) {
                // Set inputs
                sprintf_s(s, ":%i", i);
                if (!addInputPointer(multiplication->id + s, multiplication->getOutputPointer(i))) {
                    addErrorString("Dropout::join for " + id + " for multiplication " + multiplication->id + " unable to set input pointers");
                    return(false);
                }
                // set self backprop inputs
                double* pointer = getBackpropOutputPointer(multiplication->id + s);
                if (!multiplication->setBackpropInputPointer(tval.value, pointer)) {
                    addErrorString("Dropout::join for " + id + " for multiplication " + multiplication->id + " unable to set backprop pointer");
                    return(false);
                }
                //char s2[100]; sprintf_s(s2, " : tval.value = %i", tval.value); addErrorString("OutputNode::join : " + id + " to " + multiplication->id + s + s2 + "");
            }
            return(true);
        }
        else {
            addErrorString("Dropout::join for " + id + " for multiplication " + multiplication->id + " problem with multiplication backprop index");
        }
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void Dropout::forward(bool train)
{
    for (int i = 0; i < num_inputs; i++) {
        outputdata[i] = *(double*)inputpointers[i];
        drop_indices[i] = 0;
    }
    num_drops = 0;
    if (percent > 0.0) {
        if (train) {
            //char s[200];  sprintf_s(s, "forward : drop_point = %i : ", drop_point);  addErrorString(s + (string)id);
            if (drop_point < num_inputs) {
                while ((drop_point < num_inputs) && (drop_point >= 0) && (num_drops < num_inputs)) {
                    drop_indices[num_drops] = drop_point;
                    setNextDropPoint();
                    num_drops++;
                }
                for (int i = 0; i < num_drops; i++) {
                    outputdata[drop_indices[i]] = 0.0;
                    //char s[200];  sprintf_s(s, "forward : i = %i : num_drops = %i : drop_indices = %lli : ", i, num_drops, drop_indices[i]);  addErrorString(s + (string)id);
                }
            }
            else {
                drop_point -= num_inputs;
            }
        }
    }
}

void Dropout::backward()
{
    outputBackpropProcessing();
}

void Dropout::outputBackpropProcessing()
{
    for (int i = 0; i < num_inputs; i++) {
        backpropdatabuffer[i] = 0.0;
        for (int j = 0; j < num_output_connections; j++) {
            int index = (j * num_inputs) + i;
            backpropdatabuffer[i] += *(double*)backpropinputpointers[index];
        }
    }
    for (int i = 0; i < num_drops; i++) {
        backpropdatabuffer[drop_indices[i]] = 0.0;
    }
}

void Dropout::setInputToZero(string inputid)
{
    if (allocated) {
        string tname = getStringBeforeColon(inputid);
        bool found = false;
        for (int i = 0; i < num_inputs; i++) {
            if (tname == getStringBeforeColon(inputpointernames[i])) {
                inputpointers[i] = (int64_t)&zero;
                return;
            }
        }
    }
    else {
        addErrorString("Dropout::setInputToZero for " + id + " : memory allocation error");
    }
}

void Dropout::setInputToData(string inputid)
{
    if (allocated) {
        string tname = getStringBeforeColon(inputid);
        bool found = false;
        for (int i = 0; i < num_inputs; i++) {
            if (tname == getStringBeforeColon(inputpointernames[i])) {
                inputpointers[i] = backupinputpointers[i];
                return;
            }
        }
    }
    else {
        addErrorString("Dropout::setInputToData for " + id + " : memory allocation error");
    }
}

string Dropout::getStringBeforeColon(string cname)
{
    string mname = cname;
    for (int i = 0; i < mname.length(); i++) {
        if (mname[i] == ':') {
            return(mname.substr(0, i));
        }
}
    return(cname);
}

void Dropout::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void Dropout::addErrorString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}

void Dropout::addPanel1String(string msg)
{
#ifdef GCC
    cout << msg << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addPanel1String((CString)msg.c_str());
#endif
}



//******************************************************************************************************

void Addition::clear()
{
    allocated = false;
    id = "";
    zero = 0.0;
    dummy = 0.0;
    num_input_nodes = 0;
    num_output_nodes = 0;
    model_dimension = 0;
    backproppointerssize = 0;
    input_connection_index = -1;
    output_connection_index = -1;
    input_nodes_only = false;
    input_node_connection.valid = false;
    input_node_connection.value = -1;
    nodebackpropoutputpointer = 0;
}

bool Addition::init(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly)
{
    return(allocate(nid, ninputnodes, noutputnodes, nmodeldimension, ninputnodesonly));
}

bool Addition::allocate(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly)
{
    deallocate();

    id = nid;
    num_input_nodes = ninputnodes;
    num_output_nodes = noutputnodes;
    model_dimension = nmodeldimension;

    outputdata = new double[model_dimension];
    inputindices = new int64_t[num_input_nodes];
    backpropdatabuffer = new double[model_dimension];
    inputpointernames = new string[num_input_nodes];
    inputpointers = new int64_t[model_dimension * num_input_nodes];
    backupinputpointers = new int64_t[model_dimension * num_input_nodes];
    if ((outputdata == 0) || (inputpointers == 0) || (inputpointernames == 0) || (backpropdatabuffer == 0) || (inputindices == 0) || (backupinputpointers == 0)) {
        addErrorString("Addition::init for " + id + " unable to allocate memory");
        return(false);
    }
    int cindex = 0;
    for (int i = 0; i < model_dimension; i++) {
        outputdata[i] = 0.0;
        backpropdatabuffer[i] = 0.0;
        for (int j = 0; j < num_input_nodes; j++) {
            inputpointers[cindex] = (int64_t)&dummy;
            backupinputpointers[cindex] = (int64_t)&dummy;
            cindex++;
        }
    }
    for (int j = 0; j < num_input_nodes; j++) {
        inputindices[j] = 0;
        inputpointernames[j] = "";
    }

    backproppointerssize = model_dimension * num_output_nodes;
    backpropinputpointers = new int64_t[backproppointerssize];
    if ((backpropinputpointers == 0)) {
        addErrorString("Addition::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < backproppointerssize; i++) {
        backpropinputpointers[i] = (int64_t)&dummy;
    }

    backpropinputindices = new int[num_output_nodes];
    if (backpropinputindices == 0) {
        addErrorString("Addition::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < num_output_nodes; i++) {
        backpropinputindices[i] = 0;
    }

    allocated = true;

    //char s[200];  sprintf_s(s, " : num_input_nodes = %i : num_output_nodes = %i : model_dimension = %i", num_input_nodes, num_output_nodes, model_dimension);
    //addErrorString("Addition::allocate for " + id + s);

    return(true);
}

void Addition::deallocate()
{
    if (allocated) {
        delete[] outputdata;
        delete[] inputpointers;
        delete[] backupinputpointers;
        delete[] inputpointernames;
        delete[] backpropdatabuffer;
        delete[] backpropinputpointers;
        delete[] backpropinputindices;
        delete[] inputindices;
    }
    allocated = false;
}

boolInt Addition::addNewInputConnection(string name)
{
    boolInt rval;
    rval.valid = false;
    rval.value = -1;
    for (int i = 0; i < input_connection_index; i++) {
        if (inputpointernames[i] == name) {
            addErrorString("Addition::addNewInputConnection for " + id + " input name " + name + " already used");
            return(rval);
        }
    }
    if (input_connection_index < (num_input_nodes - 1)) {
        input_connection_index++;
        inputpointernames[input_connection_index] = name;
        rval.value = input_connection_index;
        rval.valid = true;
    }
    else {
        char s[200];  sprintf_s(s, " : num_input_nodes = %i", num_input_nodes);
        addErrorString("Addition::addNewInputConnection input connection index out of bounds for " + id + s);
    }
    return(rval);
}

bool Addition::addInputPointer(int nindex, double* tinput)
{
    if (allocated) {
        if (nindex < num_input_nodes) {
            if (inputindices[nindex] < model_dimension) {
                int64_t index = (nindex * model_dimension) + inputindices[nindex];
                inputpointers[index] = (int64_t)tinput;
                backupinputpointers[index] = (int64_t)tinput;
                return(true);
            }
            inputindices[nindex]++;
            return(true);
        }
        char s[200];  sprintf_s(s, " : nindex = %i : num_input_nodes = %i", nindex, num_input_nodes);
        addErrorString("Addition::addInputPointer index error for " + id + s);
    }
    else {
        addErrorString("Addition::addInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

bool Addition::addInputPointerForInputNode(string name, double* tinput)
{
    boolInt mval = input_node_connection;
    //char s[200];  sprintf_s(s, " mval.value = %i", mval.value); addErrorString("Addition::addInputPointerForInputNode for " + id + " to " + name + s);
    if (!mval.valid || input_nodes_only) {
        //addErrorString("addInputPointerForInputNode : adding new input connection");
        mval = addNewInputConnection(name);
        input_node_connection = mval;
    }
    if (mval.valid) {
        // set hnode input pointer
        //addErrorString("addInputPointerForInputNode : adding new input pointer");
        if (!addInputPointer(mval.value, tinput)) {
            addErrorString("addInputPointerForInputNode::join for " + id + " for hidden node " + name + " unable to set input pointer");
            return(false);
        }
    }
    else {
        addErrorString("Addition::addInputPointerForInputNode for " + id + " for hnode " + name + " problem with input index");
    }
    return(true);
}

double* Addition::getOutputPointer(int index)
{
    if (allocated && (index < model_dimension)) {
        //char s[200];  sprintf_s(s, " : index = %i : model_dimension = %i", index, model_dimension);  addErrorString("Addition::getOutputPointer for " + id + s);
        return(&outputdata[index]);
    }
    addErrorString("Addition::getOutputPointer unable to get output pointer");
    return(&dummy);
}

boolInt Addition::addNewBackpropConnection()
{
    boolInt rval;
    rval.valid = false;
    rval.value = -1;
    if (output_connection_index < (num_output_nodes - 1)) {
        output_connection_index++;
        rval.value = output_connection_index;
        rval.valid = true;
    }
    else {
        char s[200];  sprintf_s(s, " : output_connection_index = %i : num_output_nodes = %i : model_dimension = %i : ", output_connection_index, num_output_nodes, model_dimension);  addErrorString("Addition::addNewBackpropConnection for " + id + s);
    }
    return(rval);
}

bool Addition::setBackpropInputPointer(int connection_index, double* tprop)
{
    if (allocated) {
        if (connection_index < num_output_nodes) {
            if (backpropinputindices[connection_index] < model_dimension) {
                int index = (connection_index * model_dimension) + backpropinputindices[connection_index];
                backpropinputpointers[index] = (int64_t)tprop;
                backpropinputindices[connection_index]++;
                return(true);
            }
        }
        char s[200];  sprintf_s(s, " (%i) : ", model_dimension);
        addErrorString("Addition::setBackpropInputPointer for " + id + " : index > num_allocated_backprop_inputs" + s);
    }
    else {
        addErrorString("Addition::setBackpropInputPointer for " + id + " : memory allocation error");
    }
    return(false);
}

double* Addition::getBackpropOutputPointer(int index) {
    if (allocated) {
        if (index < model_dimension) {
            double* rval = &backpropdatabuffer[index];
            return(rval);
        }
        addErrorString("Addition::getBackpropOutputPointer for " + id + " : index >=  model_dimension");
    }
    else {
        addErrorString("Addition::getBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}


// ****************************************************************************
// ****************************************************************************

bool Addition::join(HiddenNode* hnode)
{
    boolInt mval = input_node_connection;
    int mbackpropoutputpointer = nodebackpropoutputpointer;
    if ((!mval.valid) || (!input_nodes_only)) {
        mbackpropoutputpointer = 0;
        mval = addNewInputConnection(hnode->id);
    }
    if (mval.valid) {
        // set hnode input pointer
        if (!addInputPointer(mval.value, &hnode->output)) {
            addErrorString("Addition::join for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
            return(false);
        }
        // set hnode backprop inputs
        if (!hnode->setBackpropInputPointer(getBackpropOutputPointer(mbackpropoutputpointer), id)) {
            addErrorString("Addition::join for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
            return(false);
        }
        // If only nodes for this addition keep trak of whech backprop pointer per node
        if (input_nodes_only) {
            if (nodebackpropoutputpointer < (model_dimension - 1)) {
                nodebackpropoutputpointer++;
            }
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for hnode " + hnode->id + " problem with input index");
    }
    return(true);
}

bool Addition::join(HiddenConvolution* conv)
{
    boolInt mval = addNewInputConnection(conv->id);
    if (mval.valid) {
        if (conv->output_data_size <= model_dimension) {
            boolInt tval = conv->addNewBackpropConnection(id);
            if (tval.valid) {
                for (int i = 0; i < conv->output_data_size; i++) {
                    // Set inputs
                    if (!addInputPointer(mval.value, conv->getOutputPointer(i))) {
                        addErrorString("Addition::join for " + id + " for convolution " + conv->id + " unable to set input pointers");
                        return(false);
                    }
                    // set self backprop inputs
                    double* pointer = getBackpropOutputPointer(i);
                    if (!conv->setBackpropInputPointer2(tval.value, pointer)) {
                        addErrorString("Addition::join for " + id + " for conv " + conv->id + " unable to set backprop pointer");
                        return(false);
                    }
                }
                return(true);
            }
            else {
                addErrorString("Addition::join for " + id + " for conv " + conv->id + " problem with conv backprop index");
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : conv output_data_size (%i)", model_dimension, conv->output_data_size);
            addErrorString("Addition::join for " + id + " for hnet " + conv->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for conv " + conv->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Addition::join(HiddenNet* hnet)
{
    boolInt mval = addNewInputConnection(hnet->id);
    if (mval.valid) {
        if (hnet->num_nodes_layer <= model_dimension) {
            for (int i = 0; i < hnet->num_nodes_layer; i++) {
                // set hnode input pointer
                if (!addInputPointer(mval.value, &hnet->hnodes[i].output)) {
                    addErrorString("Addition::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set input pointer");
                    return(false);
                }
                // set self backprop inputs
                if (!hnet->hnodes[i].setBackpropInputPointer(getBackpropOutputPointer(i), id)) {
                    addErrorString("Addition::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set backprop pointer");
                    return(false);
                }
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : hnet num_nodes_layer (%i)", model_dimension, hnet->num_nodes_layer);
            addErrorString("Addition::join for " + id + " for hnet " + hnet->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for hnet " + hnet->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Addition::join(NetCluster* ncluster)
{
    boolInt mval = addNewInputConnection(ncluster->id);
    if (mval.valid) {
        if (ncluster->num_outputs <= model_dimension) {
            for (int i = 0; i < ncluster->num_outputs; i++) {
                if (!addInputPointer(mval.value, ncluster->getOutputPointer(i))) {
                    addErrorString("Addition::join for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
                    return(false);
                }
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : ncluster num_outputs (%i)", model_dimension, ncluster->num_outputs);
            addErrorString("Addition::join for " + id + " for cluster " + ncluster->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for cluster " + ncluster->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Addition::join(Attention* attention)
{
    boolInt mval = addNewInputConnection(attention->id);
    if (mval.valid) {
        if (attention->output_size <= model_dimension) {
            boolInt tval = attention->addNewBackpropConnection(id);
            if (tval.valid) {
                for (int i = 0; i < attention->output_size; i++) {
                    // set attention input pointers
                    if (!addInputPointer(mval.value, attention->getOutputPointer(i))) {
                        addErrorString("Addition::join for " + id + " for attention " + attention->id + " unable to set input pointers");
                        return(false);
                    }
                    // set self backprop inputs
                    double* pointer = getBackpropOutputPointer(i);
                    if (!attention->setBackpropInputPointer(tval.value, pointer)) {
                        addErrorString("Addition::join for " + id + " for attention " + attention->id + " unable to set backprop pointer");
                        return(false);
                    }
                }
            }
            else {
                addErrorString("Addition::join for " + id + " for attention " + attention->id + " problem with backprop index");
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : attention output_size (%i)", model_dimension, attention->output_size);
            addErrorString("Addition::join for " + id + " for attention " + attention->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for attention " + attention->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Addition::join(Dropout* dropout)
{
    boolInt mval = addNewInputConnection(dropout->id);
    if (mval.valid) {
        if (dropout->num_inputs <= model_dimension) {
            boolInt tval = dropout->addNewBackpropConnection();
            if (tval.valid) {
                for (int i = 0; i < dropout->num_inputs; i++) {
                    // Set inputs
                    if (!addInputPointer(mval.value, dropout->getOutputPointer(i))) {
                        addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " unable to set input pointers");
                        return(false);
                    }
                    // set self backprop inputs
                    double* pointer = getBackpropOutputPointer(i);
                    if (!dropout->setBackpropInputPointer(tval.value, pointer)) {
                        addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " unable to set backprop pointer");
                        return(false);
                    }
                }
                return(true);
            }
            else {
                addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " problem with dropout backprop index");
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : dropout num_inputs (%i)", model_dimension, dropout->num_inputs);
            addErrorString("Addition::join for " + id + " for dropout " + dropout->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " problem with input index");
        return(false);
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void Addition::forward()
{
    for (int i = 0; i < model_dimension; i++) {
        outputdata[i] = 0.0;
        for (int j = 0; j < num_input_nodes; j++) {
            int index = i + (j * model_dimension);
            outputdata[i] = outputdata[i] + *(double*)inputpointers[index];
            //char s[200];  sprintf_s(s, " : i = %i : j = %i : index = %i : inputpointer = %.3f : outputdata = %.3f", i, j, index, *(double*)inputpointers[index], outputdata[i]); addErrorString("Addition::forward for " + id + s);
        }
    }
}

void Addition::backward()
{
    for (int i = 0; i < model_dimension; i++) {
        backpropdatabuffer[i] = 0.0;
        for (int j = 0; j < num_output_nodes; j++) {
            int index = (j * model_dimension) + i;
            backpropdatabuffer[i] += *(double*)backpropinputpointers[index];
        }
    }
}

void Addition::setInputToZero(string inputid)
{
    if (allocated) {
        for (int i = 0; i < num_input_nodes; i++) {
            if (inputid == inputpointernames[i]) {
                for (int j = 0; j < model_dimension; j++) {
                    int index = (i * model_dimension) + j;
                    inputpointers[index] = (int64_t)&zero;
                }
                return;
            }
        }
    }
    else {
        addErrorString("Addition::setInputToZero for " + id + " : memory allocation error");
    }
}

void Addition::setInputToData(string inputid)
{
    if (allocated) {
        for (int i = 0; i < num_input_nodes; i++) {
            if (inputid == inputpointernames[i]) {
                for (int j = 0; j < model_dimension; j++) {
                    int index = (i * model_dimension) + j;
                    inputpointers[index] = backupinputpointers[index];
                }
                return;
            }
        }
    }
    else {
        addErrorString("Addition::setInputToData for " + id + " : memory allocation error");
    }
}

void Addition::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void Addition::addErrorString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}

void Addition::addPanel1String(string msg)
{
#ifdef GCC
    cout << msg << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addPanel1String((CString)msg.c_str());
#endif
}



//******************************************************************************************************

void Multiplication::clear()
{
    Addition::clear();
    sum_add_limit = 0.10;
}

bool Multiplication::init(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly)
{
    return(allocate(nid, ninputnodes, noutputnodes, nmodeldimension, ninputnodesonly));
}

bool Multiplication::allocate(string nid, int ninputnodes, int noutputnodes, int nmodeldimension, bool ninputnodesonly)
{
    deallocate();

    id = nid;
    num_input_nodes = ninputnodes;
    num_output_nodes = noutputnodes;
    model_dimension = nmodeldimension;

    outputdata = new double[model_dimension];
    inputindices = new int64_t[num_input_nodes];
    inputpointernames = new string[num_input_nodes];
    inputpointers = new int64_t[model_dimension * num_input_nodes];
    backpropdatabuffer = new double[model_dimension * num_input_nodes];
    backupinputpointers = new int64_t[model_dimension * num_input_nodes];
    if ((outputdata == 0) || (inputpointers == 0) || (inputpointernames == 0) || (backpropdatabuffer == 0) || (inputindices == 0) || (backupinputpointers == 0)) {
        addErrorString("Addition::init for " + id + " unable to allocate memory");
        return(false);
    }
    int cindex = 0;
    for (int i = 0; i < model_dimension; i++) {
        outputdata[i] = 0.0;
        for (int j = 0; j < num_input_nodes; j++) {
            inputpointers[cindex] = (int64_t)&dummy;
            backpropdatabuffer[cindex] = 0.0;
            backupinputpointers[cindex] = (int64_t)&dummy;
            cindex++;
        }
    }
    for (int j = 0; j < num_input_nodes; j++) {
        inputindices[j] = 0;
        inputpointernames[j] = "";
    }

    backproppointerssize = model_dimension * num_output_nodes;
    backpropinputpointers = new int64_t[backproppointerssize];
    if ((backpropinputpointers == 0)) {
        addErrorString("Addition::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < backproppointerssize; i++) {
        backpropinputpointers[i] = (int64_t)&dummy;
    }

    backpropinputindices = new int[num_output_nodes];
    if (backpropinputindices == 0) {
        addErrorString("Addition::init for " + id + " unable to allocate memory");
        return(false);
    }
    for (int i = 0; i < num_output_nodes; i++) {
        backpropinputindices[i] = 0;
    }

    allocated = true;

    //char s[200];  sprintf_s(s, " : num_input_nodes = %i : num_output_nodes = %i : model_dimension = %i", num_input_nodes, num_output_nodes, model_dimension);
    //addErrorString("Addition::allocate for " + id + s);

    return(true);
}

double* Multiplication::getBackpropOutputPointer(int inputindex, int modelindex) {
    if (allocated) {
        if ((inputindex < num_input_nodes) && (modelindex < model_dimension)) {
            int index = (inputindex * model_dimension) + modelindex;
            double* rval = &backpropdatabuffer[index];
            return(rval);
        }
        addErrorString("Multiplication::getBackpropOutputPointer for " + id + " : index problem");
    }
    else {
        addErrorString("Multiplication::getBackpropOutputPointer for " + id + " : memory allocation error");
    }
    return(&dummy);
}

// ****************************************************************************
// ****************************************************************************

bool Multiplication::join(HiddenNode* hnode)
{
    boolInt mval = input_node_connection;
    int mbackpropoutputpointer = nodebackpropoutputpointer;
    if ((!mval.valid) || (!input_nodes_only)) {
        mbackpropoutputpointer = 0;
        mval = addNewInputConnection(hnode->id);
    }
    if (mval.valid) {
        // set hnode input pointer
        if (!addInputPointer(mval.value, &hnode->output)) {
            addErrorString("Addition::join for " + id + " for hidden node " + hnode->id + " unable to set input pointer");
            return(false);
        }
        // set hnode backprop inputs
        if (!hnode->setBackpropInputPointer(getBackpropOutputPointer(mval.value, mbackpropoutputpointer), id)) {
            addErrorString("Addition::join for " + id + " for hidden node " + hnode->id + " unable to set backprop pointer");
            return(false);
        }
        // If only nodes for this addition keep trak of whech backprop pointer per node
        if (input_nodes_only) {
            if (nodebackpropoutputpointer < (model_dimension - 1)) {
                nodebackpropoutputpointer++;
            }
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for hnode " + hnode->id + " problem with input index");
    }
    return(true);
}

bool Multiplication::join(HiddenConvolution* conv)
{
    boolInt mval = addNewInputConnection(conv->id);
    if (mval.valid) {
        if (conv->output_data_size <= model_dimension) {
            boolInt tval = conv->addNewBackpropConnection(id);
            if (tval.valid) {
                for (int i = 0; i < conv->output_data_size; i++) {
                    // Set inputs
                    if (!addInputPointer(mval.value, conv->getOutputPointer(i))) {
                        addErrorString("Addition::join for " + id + " for convolution " + conv->id + " unable to set input pointers");
                        return(false);
                    }
                    // set self backprop inputs
                    double* pointer = getBackpropOutputPointer(mval.value, i);
                    if (!conv->setBackpropInputPointer2(tval.value, pointer)) {
                        addErrorString("Addition::join for " + id + " for conv " + conv->id + " unable to set backprop pointer");
                        return(false);
                    }
                }
                return(true);
            }
            else {
                addErrorString("Addition::join for " + id + " for conv " + conv->id + " problem with conv backprop index");
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : conv output_data_size (%i)", model_dimension, conv->output_data_size);
            addErrorString("Addition::join for " + id + " for hnet " + conv->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for conv " + conv->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Multiplication::join(HiddenNet* hnet)
{
    boolInt mval = addNewInputConnection(hnet->id);
    if (mval.valid) {
        if (hnet->num_nodes_layer <= model_dimension) {
            for (int i = 0; i < hnet->num_nodes_layer; i++) {
                // set hnode input pointer
                if (!addInputPointer(mval.value, &hnet->hnodes[i].output)) {
                    addErrorString("Addition::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set input pointer");
                    return(false);
                }
                // set self backprop inputs
                if (!hnet->hnodes[i].setBackpropInputPointer(getBackpropOutputPointer(mval.value, i), id)) {
                    addErrorString("Addition::join for " + id + " for hidden node " + hnet->hnodes[i].id + " unable to set backprop pointer");
                    return(false);
                }
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : hnet num_nodes_layer (%i)", model_dimension, hnet->num_nodes_layer);
            addErrorString("Addition::join for " + id + " for hnet " + hnet->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for hnet " + hnet->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Multiplication::join(NetCluster* ncluster)
{
    boolInt mval = addNewInputConnection(ncluster->id);
    if (mval.valid) {
        if (ncluster->num_outputs <= model_dimension) {
            for (int i = 0; i < ncluster->num_outputs; i++) {
                if (!addInputPointer(mval.value, ncluster->getOutputPointer(i))) {
                    addErrorString("Addition::join for " + id + " for cluster " + ncluster->id + " unable to set input pointers");
                    return(false);
                }
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : ncluster num_outputs (%i)", model_dimension, ncluster->num_outputs);
            addErrorString("Addition::join for " + id + " for cluster " + ncluster->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for cluster " + ncluster->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Multiplication::join(Attention* attention)
{
    boolInt mval = addNewInputConnection(attention->id);
    if (mval.valid) {
        if (attention->output_size <= model_dimension) {
            boolInt tval = attention->addNewBackpropConnection(id);
            if (tval.valid) {
                for (int i = 0; i < attention->output_size; i++) {
                    // set attention input pointers
                    if (!addInputPointer(mval.value, attention->getOutputPointer(i))) {
                        addErrorString("Addition::join for " + id + " for attention " + attention->id + " unable to set input pointers");
                        return(false);
                    }
                    // set self backprop inputs
                    double* pointer = getBackpropOutputPointer(mval.value, i);
                    if (!attention->setBackpropInputPointer(tval.value, pointer)) {
                        addErrorString("Addition::join for " + id + " for attention " + attention->id + " unable to set backprop pointer");
                        return(false);
                    }
                }
            }
            else {
                addErrorString("Addition::join for " + id + " for attention " + attention->id + " problem with backprop index");
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : attention output_size (%i)", model_dimension, attention->output_size);
            addErrorString("Addition::join for " + id + " for attention " + attention->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for attention " + attention->id + " problem with input index");
        return(false);
    }
    return(true);
}

bool Multiplication::join(Dropout* dropout)
{
    boolInt mval = addNewInputConnection(dropout->id);
    if (mval.valid) {
        if (dropout->num_inputs <= model_dimension) {
            boolInt tval = dropout->addNewBackpropConnection();
            if (tval.valid) {
                for (int i = 0; i < dropout->num_inputs; i++) {
                    // Set inputs
                    if (!addInputPointer(mval.value, dropout->getOutputPointer(i))) {
                        addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " unable to set input pointers");
                        return(false);
                    }
                    // set self backprop inputs
                    double* pointer = getBackpropOutputPointer(mval.value, i);
                    if (!dropout->setBackpropInputPointer(tval.value, pointer)) {
                        addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " unable to set backprop pointer");
                        return(false);
                    }
                }
                return(true);
            }
            else {
                addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " problem with dropout backprop index");
            }
        }
        else {
            char s[200];  sprintf_s(s, " : input size problem : model_dimension (%i) : dropout num_inputs (%i)", model_dimension, dropout->num_inputs);
            addErrorString("Addition::join for " + id + " for dropout " + dropout->id + s);
            return(false);
        }
    }
    else {
        addErrorString("Addition::join for " + id + " for dropout " + dropout->id + " problem with input index");
        return(false);
    }
    return(true);
}

// ****************************************************************************
// ****************************************************************************

void Multiplication::forward()
{
    for (int i = 0; i < model_dimension; i++) {
        outputdata[i] = 1.0;
        //string s2 = "Multiplication::forward : " + id;
        for (int j = 0; j < num_input_nodes; j++) {
            int index = i + (j * model_dimension);
            outputdata[i] = outputdata[i] * *(double*)inputpointers[index];
            //char s[200];  sprintf_s(s, " : %.3f", *(double*)inputpointers[index]); s2 += s;
        }
        //addErrorString(s2);
    }
}

void Multiplication::backward()
{
    for (int i = 0; i < model_dimension; i++) {
        double backpropsum = 0.0;
        for (int j = 0; j < num_output_nodes; j++) {
            int index = (j * model_dimension) + i;
            backpropsum += *(double*)backpropinputpointers[index];
        }
        for (int j = 0; j < num_input_nodes; j++) {
            double derivative = 1.0;
            int index = (j * model_dimension) + i;
            for (int k = 0; k < num_input_nodes; k++) {
                if (k != j) {
                    derivative *= (*(double*)inputpointers[k]);
                }
            }
            if (derivative != 0.0) {
                derivative = 1 / derivative;
                if (derivative > sum_add_limit) {
                    derivative = sum_add_limit;
                }
                else if (derivative < -sum_add_limit) {
                    derivative = -sum_add_limit;
                }
                backpropdatabuffer[i] = derivative * backpropsum;
            }
            else {
                backpropdatabuffer[i] = 0.0;
            }
        }
    }
}




//******************************************************************************************************

NetCore::NetCore()
{
    allocated = false;
    test_inputs_allocated = false;
    num_inputs = 0;
    num_hidden_nodes = 0;
    num_active_convs = 0;
    num_hidden_nets = 0;
    num_attention_layers = 0;
    num_self_attention_layers = 0;
    num_net_clusters = 0;
    num_dropouts = 0;
    num_additions = 0;
    num_multiplications = 0;
    num_outputs = 0;
    errorfilter = 100;
    errorFiltercount = 100;
    validationfiltercount = 100;
    filter = 0.001;
    accumulative_error = 0.0;
    accumulative_error_slope = 0;
    accumulative_validation_error = 0;

    parallel_processing_test = 100;

    conv_output_test_index = -1;

    for (int i = 0; i < 10; i++) {
        avgneterrors[i] = 0;
    }

    randomize_data = true;
    use_validation_index = false;
    data_file_index = 0;
    num_filedata_rows = 0;
   
    data_index.dindex = 0;
    data_index.findex = 0;
    data_index.sindex = 0;

    error_index.dindex = 0;
    error_index.findex = 0;
    error_index.sindex = 0;

    validation_index.dindex = 0;
    validation_index.findex = 0;
    validation_index.sindex = 0;

    num_validationdata_rows = 0;

    use_validation_files = false;
    use_percent_validation = false;
    dovalidation = false;
    validation_percent = 0.0F;

    train_error = 0.0;
    train_error_delta = 0.0;
    validation_error = 0.0;

    train_inc = 10;
    error_inc = 2;
    update_inc = 100;

    train_count = 0;
    err_count = 0;

    updateRate = 10;

    num_train_data = 0;
    num_validation_data = 0;

    inputtraindatasize = 0;
    inputvalidationdatasize = 0;
    outputtraindatasize = 0;
    outputvalidationdatasize = 0;

    inputs = NULL;
    testoutputs = NULL;

    has_series_conv = false;
    series_data_allocated = false;
    series_input_width = 0;
    series_input_height = 0;
    series_mem_size = 0;
    series_backprop_size = 0;
    series_data = NULL;
    series_backprop_data = NULL;

    has_image_conv = false;
    image_data_allocated = false;
    use_images_as_outputs = false;
    image_mem_size = 0;
    image_backprop_size = 0;
    image_data = NULL;
    image_backprop_data = NULL;
    input_image_conv_name = "";

    image_train_data_size = 0;
    image_train_data_allocated = false;
    image_train_data = NULL;

    has_wave_conv = false;
    has_wave_fft_conv = false;
    wave_data_allocated = false;
    wave_mem_size = 0;
    wave_data = NULL;

    wave_train_data_allocated = false;
    wave_allocation_size = 0;
    wave_files_longest = 0;
    wave_train_data = NULL;

    has_text_conv = false;

    conv_variables_initialized = false;

    has_softmax = false;
    softmax_sum = 0.0;

    sound_data_process_type = NONE;
    sound_down_sample = 1;
    sound_data_stride = 1;
    sound_process_stride = 0;
    process_sound_ffts = false;
    soundfftnames.clear();
    soundfftprocess = "";
    soundfftstride = 1;
    soundfftdownsample = 1;
    normaiize_file_set = false;


    num_copy_outputs_to_inputs = 0;

    sigmoid_leakage = 0.01;

    dummy = 0.0;

    set_data_count = 0;
    
    onodes = NULL;
    hnodes = NULL;
    newConvs = NULL;
    hnets = NULL;
    attentions = NULL;
    selfattentions = NULL;
    nclusters = NULL;
    dropouts = NULL;
    additions = NULL;


}

NetCore::~NetCore()
{
    clearAndDeallocate();
}

void NetCore::clearAndDeallocate()
{
    if (allocated) {
        delete[] inputs;
        delete[] testoutputs;
        if (num_outputs > 0) {
            for (int i = 0; i < num_outputs; i++) {
                onodes[i].deallocate();
                onodes[i].clear();
            }
            delete[] onodes;
        }
        if (num_hidden_nodes > 0) {
            for (int i = 0; i < num_hidden_nodes; i++) {
                hnodes[i].deallocate();
                hnodes[i].clear();
            }
            delete[] hnodes;
        }
        if (num_active_convs > 0) {
            for (int i = 0; i < num_active_convs; i++) {
                newConvs[i].deallocate();
                newConvs[i].clear();
            }
            delete[] newConvs;
        }
        if (num_hidden_nets > 0) {
            for (int i = 0; i < num_hidden_nets; i++) {
                hnets[i].deallocate();
                hnets[i].clear();
            }
            delete[] hnets;
        }
        if (num_attention_layers > 0) {
            for (int i = 0; i < num_attention_layers; i++) {
                attentions[i].deallocate();
                attentions[i].clear();
            }
            delete[] attentions;
        }
        if (num_self_attention_layers > 0) {
            for (int i = 0; i < num_self_attention_layers; i++) {
                selfattentions[i].deallocate();
                selfattentions[i].clear();
            }
            delete[] selfattentions;
        }
        if (num_net_clusters > 0) {
            for (int i = 0; i < num_net_clusters; i++) {
                nclusters[i].deallocate();
                nclusters[i].clear();
            }
            delete[] nclusters;
        }
        if (num_dropouts > 0) {
            for (int i = 0; i < num_dropouts; i++) {
                dropouts[i].deallocate();
                dropouts[i].clear();
            }
            delete[] dropouts;
        }
        if (num_additions > 0) {
            for (int i = 0; i < num_additions; i++) {
                additions[i].deallocate();
                additions[i].clear();
            }
            delete[] additions;
        }
        if (num_multiplications > 0) {
            for (int i = 0; i < num_multiplications; i++) {
                multiplications[i].deallocate();
                multiplications[i].clear();
            }
            delete[] multiplications;
        }
        if (test_inputs_allocated) {
            delete[] test_inputs;
            test_inputs_allocated = false;
        }
        allocated = false;
    }

    if (inputtraindatasize > 0) {
        delete[] inputtraindata;
    }
    inputtraindatasize = 0;

    if (inputvalidationdatasize > 0) {
        delete[] inputvalidationdata;
    }
    inputvalidationdatasize = 0;

    if (outputtraindatasize > 0) {
        delete[] outputtraindata;
    }
    outputtraindatasize = 0;

    if (outputvalidationdatasize > 0) {
        delete[] outputvalidationdata;
    }
    outputvalidationdatasize = 0;

    if (series_data_allocated) {
        if (series_mem_size > 0) {
            delete[] series_data;
        }
        if (series_backprop_size > 0) {
            delete[] series_backprop_data;
        }
    }
    series_data_allocated = false;
    has_series_conv = false;
    series_input_width = 0;
    series_input_height = 0;
    series_mem_size = 0;
    series_backprop_size = 0;
    csv_with_filename = false;
    csv_filename_header = "";
    csv_image_filenames.clear();

    if (image_data_allocated) {
        delete[] image_data;
        delete[] image_backprop_data;
    }
    image_data_allocated = false;
    use_images_as_outputs = false;
    has_image_conv = false;
    image_mem_size = 0;
    image_backprop_size = 0;
    input_image_conv_name = "";

    if (image_train_data_allocated) {
        delete[] image_train_data;
    }
    image_train_data_allocated = false;
    image_train_data_size = 0;

    imageTrainIndices.clear();
    imageValidationIndices.clear();

    if (wave_data_allocated) {
        delete[] wave_data;
    }
    wave_data_allocated = false;
    has_wave_conv = false;
    has_wave_fft_conv = false;
    wave_mem_size = 0;

    if (wave_train_data_allocated) {
        delete[] wave_train_data;
    }
    wave_train_data_allocated = false;
    wave_files_longest = 0;
    wave_allocation_size = 0;

    has_text_conv = false;

    if (num_copy_outputs_to_inputs > 0) {
        delete[] copy_outputs_offsets;
        delete[] copy_to_input_indices;
        delete[] copy_from_output_indices;
    }
    num_copy_outputs_to_inputs = 0;

    num_train_data = 0;
    num_validation_data = 0;

    num_inputs = 0;
    num_outputs = 0;
    num_hidden_nodes = 0;
    num_active_convs = 0;
    num_hidden_nets = 0;
    num_attention_layers = 0;
    num_self_attention_layers = 0;
    num_net_clusters = 0;
    num_dropouts = 0;
    num_additions = 0;
    num_multiplications = 0;

    has_softmax = false;
    softmax_sum = 0.0;

    for (int i = 0; i < (int)netInputs.size(); i++) {
        netInputs[i].inputFields.clear();
    }
    netInputs.clear();
    netOutputs.clear();

    clearProcessedData();

    conv_output_test_index = -1;
    image_gray = false;
    image_width = 0;
    image_height = 0;
    image_single = 0;
    image_max_size = 0;
    conv_variables_initialized = false;
    conv_gray = false;
    conv_numx = 0;
    conv_numy = 0;
    conv_single = 0;
    conv_max_size = 0;

    sound_data_process_type = NONE;
    sound_down_sample = 1;
    sound_data_stride = 1;
    sound_process_stride = 0;
    process_sound_ffts = false;
    soundfftnames.clear();
    soundfftprocess = "";
    soundfftstride = 1;
    soundfftdownsample = 1;
    normaiize_file_set = false;

    randomize_data = true;
    use_validation_index = false;
    data_file_index = 0;
    
    data_index.dindex = 0;
    data_index.findex = 0;
    data_index.sindex = 0;

    error_index.dindex = 0;
    error_index.findex = 0;
    error_index.sindex = 0;

    validation_index.dindex = 0;
    validation_index.findex = 0;
    validation_index.sindex = 0;

    train_count = 0;
    err_count = 0;

    updateRate = 10;

    num_filedata_rows = 0;
    fileTrainIndicies.clear();
    fileTrainSizes.clear();

    fileValidationIndicies.clear();
    fileValidationSizes.clear();

    use_validation_files = false;
    use_percent_validation = false;
    dovalidation = false;
    validation_percent = 0.0F;

    dataPathnames.clear();
    validationPathnames.clear();
    combinedPathnames.clear();

    previousDataPathnames.clear();
    previousValidationPathnames.clear();

    hiddenNodesWithInputsIndices.clear();
    activeConvsWithInputsIndices.clear();
    activeNetsWithInputsIndices.clear();
    activeNetClustersWithInputsIndices.clear();
    activeDropoutsWithInputsIndices.clear();
    activeAdditionsWithInputsIndices.clear();
    activeMultiplicationsWithInputsIndices.clear();

    conv_frame_size = 1;
    conv_input_size = 1;
    index_conv = false;
    sound_conv_only = false;
    processed_data_conv = false;

    num_validationdata_rows = 0;

    train_error = 0.0;
    train_error_delta = 0.0;
    validation_error = 0.0;

    sigmoid_leakage = 0.01;

    filter = 0.001;
    accumulative_error = 0.0;
    accumulative_error_slope = 0;
    accumulative_validation_error = 0.0;
    errorfilter = 100;
    errorFiltercount = 1000;
    validationfiltercount = 1000;
    for (int i = 0; i < 10; i++) {
        avgneterrors[i] = 0;
    }

    set_data_count = 0;


}

void NetCore::getIndicesWithInputs()
{
    hiddenNodesWithInputsIndices.clear();
    activeConvsWithInputsIndices.clear();
    activeNetsWithInputsIndices.clear();
    activeNetClustersWithInputsIndices.clear();
    activeDropoutsWithInputsIndices.clear();
    activeAdditionsWithInputsIndices.clear();
    activeMultiplicationsWithInputsIndices.clear();
    if (netInputs.size() > 0) {
        // Get indices for nodes with connections to inputs
        if (num_hidden_nodes > 0) {
            for (int k = 0; k < netInputs.size(); k++) {
                for (int i = 0; i < num_hidden_nodes; i++) {
                    if (hnodes[i].num_inputs > 0) {
                        for (int j = 0; j < hnodes[i].num_inputs; j++) {
                            if (netInputs[k].nodeId == hnodes[i].inputpointernames[j]) {
                                bool found = false;
                                if (hiddenNodesWithInputsIndices.size() > 0) {
                                    for (int m = 0; m < (int)hiddenNodesWithInputsIndices.size(); m++) {
                                        if (hnodes[i].id == hnodes[hiddenNodesWithInputsIndices[m]].id) {
                                            m = (int)hiddenNodesWithInputsIndices.size();
                                            found = true;
                                        }
                                    }
                                }
                                if (!found) {
                                    hiddenNodesWithInputsIndices.push_back(i);
                                    j = hnodes[i].num_inputs;
                                }
                            }
                        }
                    }
                }
            }
        }
        // Get indices for convs with connections to inputs
        if (num_active_convs > 0) {
            for (int k = 0; k < netInputs.size(); k++) {
                for (int i = 0; i < num_active_convs; i++) {
                    if (newConvs[i].input_data_size > 0) {
                        for (int j = 0; j < newConvs[i].input_data_size; j++) {
                            if (netInputs[k].nodeId == newConvs[i].inputpointernames[j]) {
                                bool found = false;
                                if (activeConvsWithInputsIndices.size() > 0) {
                                    for (int m = 0; m < (int)activeConvsWithInputsIndices.size(); m++) {
                                        if (newConvs[i].id == newConvs[activeConvsWithInputsIndices[m]].id) {
                                            m = (int)activeConvsWithInputsIndices.size();
                                            found = true;
                                        }
                                    }
                                }
                                if (!found) {
                                    activeConvsWithInputsIndices.push_back(i);
                                    j = newConvs[i].input_data_size;
                                }
                            }
                        }
                    }
                }
            }
        }
        // Get indices for hnets with connections to inputs
        if (num_hidden_nets > 0) {
            for (int k = 0; k < netInputs.size(); k++) {
                for (int i = 0; i < num_hidden_nets; i++) {
                    if (hnets[i].num_nodes_layer > 0) {
                        for (int j = 0; j < hnets[i].hnodes[0].num_inputs; j++) {
                            if (netInputs[k].nodeId == hnets[i].hnodes[0].inputpointernames[j]) {
                                bool found = false;
                                if (activeNetsWithInputsIndices.size() > 0) {
                                    for (int m = 0; m < (int)activeNetsWithInputsIndices.size(); m++) {
                                        if (hnets[i].id == hnets[activeNetsWithInputsIndices[m]].id) {
                                            m = (int)activeNetsWithInputsIndices.size();
                                            found = true;
                                        }
                                    }
                                }
                                if (!found) {
                                    activeNetsWithInputsIndices.push_back(i);
                                    j = hnets[i].hnodes[0].num_inputs;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (num_net_clusters > 0) {
            for (int k = 0; k < netInputs.size(); k++) {
                for (int i = 0; i < num_net_clusters; i++) {
                    if (nclusters[i].num_outputs > 0) {
                        for (int j = 0; j < nclusters[i].onodes[0].num_inputs; j++) {
                            if (netInputs[k].nodeId == nclusters[i].onodes[0].inputpointernames[j]) {
                                bool found = false;
                                if (activeNetClustersWithInputsIndices.size() > 0) {
                                    for (int m = 0; m < (int)activeNetClustersWithInputsIndices.size(); m++) {
                                        if (nclusters[i].id == nclusters[activeNetClustersWithInputsIndices[m]].id) {
                                            m = (int)activeNetClustersWithInputsIndices.size();
                                            found = true;
                                        }
                                    }
                                }
                                if (!found) {
                                    activeNetClustersWithInputsIndices.push_back(i);
                                    j = nclusters[i].onodes[0].num_inputs;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (num_dropouts > 0) {
            for (int k = 0; k < netInputs.size(); k++) {
                for (int i = 0; i < num_dropouts; i++) {
                    if (dropouts[i].num_inputs > 0) {
                        for (int j = 0; j < dropouts[i].num_inputs; j++) {
                            if (netInputs[k].nodeId == dropouts[i].inputpointernames[j]) {
                                bool found = false;
                                if (activeDropoutsWithInputsIndices.size() > 0) {
                                    for (int m = 0; m < (int)activeDropoutsWithInputsIndices.size(); m++) {
                                        if (dropouts[i].id == dropouts[activeDropoutsWithInputsIndices[m]].id) {
                                            m = (int)activeDropoutsWithInputsIndices.size();
                                            found = true;
                                        }
                                    }
                                }
                                if (!found) {
                                    activeDropoutsWithInputsIndices.push_back(i);
                                    j = dropouts[i].num_inputs;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (num_additions > 0) {
            for (int k = 0; k < netInputs.size(); k++) {
                for (int i = 0; i < num_additions; i++) {
                    if (additions[i].num_input_nodes > 0) {
                        for (int j = 0; j < additions[i].num_input_nodes; j++) {
                            if (netInputs[k].nodeId == additions[i].inputpointernames[j]) {
                                bool found = false;
                                if (activeAdditionsWithInputsIndices.size() > 0) {
                                    for (int m = 0; m < (int)activeAdditionsWithInputsIndices.size(); m++) {
                                        if (additions[i].id == additions[activeAdditionsWithInputsIndices[m]].id) {
                                            m = (int)activeAdditionsWithInputsIndices.size();
                                            found = true;
                                        }
                                    }
                                }
                                if (!found) {
                                    activeAdditionsWithInputsIndices.push_back(i);
                                    j = additions[i].num_input_nodes;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (num_multiplications > 0) {
            for (int k = 0; k < netInputs.size(); k++) {
                for (int i = 0; i < num_multiplications; i++) {
                    if (multiplications[i].num_input_nodes > 0) {
                        for (int j = 0; j < multiplications[i].num_input_nodes; j++) {
                            if (netInputs[k].nodeId == multiplications[i].inputpointernames[j]) {
                                bool found = false;
                                if (activeMultiplicationsWithInputsIndices.size() > 0) {
                                    for (int m = 0; m < (int)activeMultiplicationsWithInputsIndices.size(); m++) {
                                        if (multiplications[i].id == multiplications[activeMultiplicationsWithInputsIndices[m]].id) {
                                            m = (int)activeMultiplicationsWithInputsIndices.size();
                                            found = true;
                                        }
                                    }
                                }
                                if (!found) {
                                    activeMultiplicationsWithInputsIndices.push_back(i);
                                    j = multiplications[i].num_input_nodes;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void NetCore::setInputToZero(string inputid)
{
    if (hiddenNodesWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)hiddenNodesWithInputsIndices.size(); i++) {
            hnodes[hiddenNodesWithInputsIndices[i]].setInputToZero(inputid);
        }
    }
    if (activeConvsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeConvsWithInputsIndices.size(); i++) {
            newConvs[activeConvsWithInputsIndices[i]].setInputToZero(inputid);
        }
    }
    if (activeNetsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeNetsWithInputsIndices.size(); i++) {
            hnets[activeNetsWithInputsIndices[i]].setInputToZero(inputid);
        }
    }
    if (activeNetClustersWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeNetClustersWithInputsIndices.size(); i++) {
            nclusters[activeNetClustersWithInputsIndices[i]].setInputToZero(inputid);
        }
    }
    if (activeDropoutsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeDropoutsWithInputsIndices.size(); i++) {
            dropouts[activeDropoutsWithInputsIndices[i]].setInputToZero(inputid);
        }
    }
    if (activeAdditionsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeAdditionsWithInputsIndices.size(); i++) {
            additions[activeAdditionsWithInputsIndices[i]].setInputToZero(inputid);
        }
    }
    if (activeMultiplicationsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeMultiplicationsWithInputsIndices.size(); i++) {
            multiplications[activeMultiplicationsWithInputsIndices[i]].setInputToZero(inputid);
        }
    }
}

void NetCore::setInputToData(string inputid)
{
    if (hiddenNodesWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)hiddenNodesWithInputsIndices.size(); i++) {
            hnodes[hiddenNodesWithInputsIndices[i]].setInputToData(inputid);
        }
    }
    if (activeConvsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeConvsWithInputsIndices.size(); i++) {
            newConvs[activeConvsWithInputsIndices[i]].setInputToData(inputid);
        }
    }
    if (activeNetsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeNetsWithInputsIndices.size(); i++) {
            hnets[activeNetsWithInputsIndices[i]].setInputToData(inputid);
        }
    }
    if (activeNetClustersWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeNetClustersWithInputsIndices.size(); i++) {
            nclusters[activeNetClustersWithInputsIndices[i]].setInputToData(inputid);
        }
    }
    if (activeDropoutsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeDropoutsWithInputsIndices.size(); i++) {
            dropouts[activeDropoutsWithInputsIndices[i]].setInputToData(inputid);
        }
    }
    if (activeAdditionsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeAdditionsWithInputsIndices.size(); i++) {
            additions[activeAdditionsWithInputsIndices[i]].setInputToData(inputid);
        }
    }
    if (activeMultiplicationsWithInputsIndices.size() > 0) {
        for (int i = 0; i < (int)activeMultiplicationsWithInputsIndices.size(); i++) {
            multiplications[activeMultiplicationsWithInputsIndices[i]].setInputToData(inputid);
        }
    }
}


// train memory management start ----------------------

bool NetCore::allocateInputTrainData(int64_t num_data)
{
    //CString s = _T("");
    //s.Format(_T("allocateInputTrainData : num_data = %i : num_inputs = %i"), num_data, num_inputs);
    //AfxMessageBox(s);
    num_train_data = num_data;
    inputtraindatasize = int64_t(num_inputs) * num_data;
    inputtraindata = new double[inputtraindatasize];
    if (inputtraindata == 0) {
        inputtraindatasize = 0;
        addErrorString("Error : unable to allocate memory for net input train data");
        return(false);
    }
    for (int i = 0; i < inputtraindatasize; i++) {
        inputtraindata[i] = 0.0;
    }
    return(true);
}

bool NetCore::allocateOutputTrainData(int64_t num_data)
{
    //CString s = _T("");
    //s.Format(_T("allocateOutputTrainData : num_data = %i : num_outputs = %i"), num_data, num_outputs);
    //AfxMessageBox(s);
    outputtraindatasize = int64_t(num_outputs) * num_data;
    outputtraindata = new double[outputtraindatasize];
    if (outputtraindata == 0) {
        outputtraindatasize = 0;
        addErrorString("Error : unable to allocate memory for net output train data");
        return(false);
    }
    for (int i = 0; i < outputtraindatasize; i++) {
        outputtraindata[i] = 0.0;
    }
    return(true);
}

bool NetCore::allocateInputValidationData(int64_t num_data)
{
    //CString s = _T("");
    //s.Format(_T("allocateInputValidationData : num_data = %i : num_inputs = %i"), num_data, num_inputs);
    //AfxMessageBox(s);
    num_validation_data = num_data;
    inputvalidationdatasize = int64_t(num_inputs) * num_data;
    inputvalidationdata = new double[inputvalidationdatasize];
    if (inputvalidationdata == 0) {
        inputvalidationdatasize = 0;
        addErrorString("Error : unable to allocate memory for net input valdation data");
        return(false);
    }
    for (int i = 0; i < inputvalidationdatasize; i++) {
        inputvalidationdata[i] = 0.0;
    }
    return(true);
}

bool NetCore::allocateOutputValidationData(int64_t num_data)
{
    //CString s = _T("");
    //s.Format(_T("allocateOutputValidationData : num_data = %i : num_outputs = %i"), num_data, num_outputs);
    //AfxMessageBox(s);
    outputvalidationdatasize = int64_t(num_outputs) * num_data;
    outputvalidationdata = new double[outputvalidationdatasize];
    if (outputvalidationdata == 0) {
        outputvalidationdatasize = 0;
        addErrorString("Error : unable to allocate memory for net output valdation data");
        return(false);
    }
    for (int i = 0; i < outputvalidationdatasize; i++) {
        outputvalidationdata[i] = 0.0;
    }
    return(true);
}

bool NetCore::checkAndAllocateSeriesMemory(int mem_size, int backprop_size)
{
    series_mem_size = mem_size;
    series_backprop_size = backprop_size;
    if (series_mem_size > 0) {
        series_data = new double[series_mem_size];
        if (series_data == 0) {
            addErrorString("NetCore::checkAndAllocateSeriesMemory : unable to allocate memory for series data");
            return(false);
        }
        for (int i = 0; i < series_mem_size; i++) {
            series_data[i] = 0.0;
        }
        series_data_allocated = true;
        has_series_conv = true;
    }
    if (series_backprop_size > 0) {
        series_backprop_data = new double[series_backprop_size];
        if (series_backprop_data == 0) {
            addErrorString("NetCore::checkAndAllocateSeriesMemory : unable to allocate memory for series backprps");
            return(false);
        }
        for (int i = 0; i < series_backprop_size; i++) {
            series_backprop_data[i] = 0;
        }
        series_data_allocated = true;
        has_series_conv = true;
    }
    return(true);
}

bool NetCore::checkAndAllocateImageMemory(int mem_size, int backprop_size)
{
    //char s[200]; sprintf_s(s, "checkAndAllocateImageMemory : %i : %i", mem_size, image_mem_size); addErrorString(s);
    if (image_train_data_allocated) {
        if (mem_size != image_mem_size) {
            char s[200];  sprintf_s(s, "checkAndAllocateImageMemory : bad : %i : %i", mem_size, image_mem_size);
            addErrorString(s);
            return(false);
        }
    }
    else {
        image_mem_size = mem_size;
    }
    image_backprop_size = backprop_size;
    image_data = new double[image_mem_size];
    image_backprop_data = new double[image_backprop_size];
    if ((image_data == 0) || (image_backprop_data == 0)) {
        addErrorString("NetCore::checkAndAllocateImageMemory : unable to allocate image memory");
        return(false);
    }
    for (int i = 0; i < image_mem_size; i++) {
        image_data[i] = 0.5;
    }
    for (int i = 0; i < image_backprop_size; i++) {
        image_backprop_data[i] = 0;
    }
    image_data_allocated = true;
    has_image_conv = true;
    return(true);
}

bool NetCore::checkAndAllocateTrainImageMemory(int mem_size, int num_images)
{
    //char s[200]; sprintf_s(s, "checkAndAllocateTrainImageMemory : %i : %i", mem_size, image_mem_size); addErrorString(s);
    if (image_data_allocated) {
        if (mem_size != image_mem_size) {
            char s[200]; sprintf_s(s, "checkAndAllocateTrainImageMemory : bad : %i : %i", mem_size, image_mem_size); addErrorString(s);
            return(false);
        }
    }
    else {
        image_mem_size = mem_size;
    }
    //addErrorString("checkAndAllocateTrainImageMemory : good");
    num_train_images = num_images;
    image_train_data_size = _int64(image_mem_size) * _int64(num_train_images);
    image_train_data = new double[image_train_data_size];
    if (image_train_data == 0) {
        addErrorString("Error : unable to allocate memory for image data");
        image_train_data_size = 0;
        return(false);
    }
    for (int i = 0; i < image_train_data_size; i++) {
        image_train_data[i] = 0.0;
    }
    image_train_data_allocated = true;
    return(true);
}

bool NetCore::checkAndAllocateSoundMemory(int mem_size)
{
    wave_mem_size = mem_size;
    wave_data = new double[wave_mem_size];
    if (wave_data == 0) {
        addErrorString("NetCore::checkAndAllocateSoundMemory : unable to allocate memory for sound data");
        return(false);
    }
    for (int i = 0; i < wave_mem_size; i++) {
        wave_data[i] = 0.0;
    }
    wave_data_allocated = true;
    return(true);
}

bool NetCore::checkAndAllocateTrainSoundMemory(int mem_size)
{
    wave_allocation_size = mem_size;
    wave_train_data = new double[wave_allocation_size];
    if (wave_train_data == 0) {
        addErrorString("Error : unable to allocate memory for sound data");
        return(false);
    }
    for (int i = 0; i < wave_allocation_size; i++) {
        wave_train_data[i] = 0.0;
    }
    wave_train_data_allocated = true;
    return(true);
}

double NetCore::getInputTrainData(int index)
{
    if ((index >= 0) && (index < inputtraindatasize)) {
        return(inputtraindata[index]);
    }
    addErrorString("Error : getInputTrainData index out of range");
    return(0.0);
}

bool NetCore::setInputTrainData(int index, double data)
{
    if ((index >= 0) && (index < inputtraindatasize)) {
        inputtraindata[index] = data;
        return(true);
    }
    char s[200];  sprintf_s(s, "Error : setInputTrainData index out of range : index = %i : inputtraindatasize = %i", index, (int)inputtraindatasize);
    addErrorString(s);
    return(false);
}

double NetCore::getOutputTrainData(int index)
{
    if ((index >= 0) && (index < outputtraindatasize)) {
        return(outputtraindata[index]);
    }
    addErrorString("Error : getOutputTrainData index out of range");
    return(0.0);
}

bool NetCore::setOutputTrainData(int index, double data)
{
    if ((index >= 0) && (index < outputtraindatasize)) {
        outputtraindata[index] = data;
        return(true);
    }
    char s[200];  sprintf_s(s, "Error : setOutputTrainData index out of range : index = %i : outputtraindatasize = %i", index, (int)outputtraindatasize);
    addErrorString(s);
    return(false);
}

double NetCore::getInputValidationData(int index)
{
    if ((index >= 0) && (index < inputvalidationdatasize)) {
        return(inputvalidationdata[index]);
    }
    addErrorString("Error : getInputValidationData index out of range");
    return(0.0);
}

bool NetCore::setInputValidationData(int index, double data)
{
    if ((index >= 0) && (index < inputvalidationdatasize)) {
        inputvalidationdata[index] = data;
        return(true);
    }
    if (set_data_count < 20) {
        char s[200];  sprintf_s(s, "Error : setInputValidationData index out of range : index = %i : inputvalidationdatasize = %i", index, (int)inputvalidationdatasize);
        addErrorString(s);
        set_data_count++;
    }
    return(false);
}

double NetCore::getOutputValidationData(int index)
{
    if ((index >= 0) && (index < outputvalidationdatasize)) {
        return(outputvalidationdata[index]);
    }
    addErrorString("Error : getOutputValidationData index out of range");
    return(0.0);
}

bool NetCore::setOutputValidationData(int index, double data)
{
    if ((index >= 0) && (index < outputvalidationdatasize)) {
        outputvalidationdata[index] = data;
        return(true);
    }
    if (set_data_count < 20) {
        char s[200];  sprintf_s(s, "Error : setOutputValidationData index out of range : index = %i : outputvalidationdatasize = %i", index, (int)outputvalidationdatasize);
        addErrorString(s);
        set_data_count++;
    }
    return(false);
}

void NetCore::loadExternalDataToNet(train_index tindex)
{
    use_validation_index = false;
    data_file_index = tindex.findex;  // needed to display waveform data correctly for fft waveforms
    if (has_series_conv && !has_wave_conv) {
        loadTrainSeriesDataToNet(tindex.dindex);
    }
    if (image_train_data_allocated) {
        loadTrainImageToNet(tindex.dindex);
    }
    if (has_wave_conv && !has_wave_fft_conv) {
        loadTrainWaveDataToNet(tindex);
    }
    if (has_text_conv) {
        addErrorString("loadExternalDataToNet : no text processing");
    }
    return;
}

void NetCore::loadExternalValidationDataToNet(train_index tindex)
{
    use_validation_index = true;
    data_file_index = tindex.dindex;  // needed to display waveform data correctly for fft waveforms
    if (has_series_conv && !has_wave_conv) {
        loadValidationSeriesToNet(tindex.dindex);
    }
    if (image_train_data_allocated) {
        loadValidationImageToNet(tindex.dindex);
    }
    if (has_wave_conv && !has_wave_fft_conv) {
        loadValidationWaveDataToNet(tindex);
    }
    if (has_text_conv) {
        addErrorString("loadExternalValidationDataToNet : no text processing");
    }
    return;
}

void NetCore::loadTrainSeriesDataToNet(int index)
{
    if (has_series_conv) {
        static bool series_data_input_mismatch = false;
        if (series_input_height != num_inputs) {
            if (!series_data_input_mismatch) {
                char s[200];  sprintf_s(s, "loadTrainSeriesDataToNet : series_input_height (%i) not equal to num_inputs (%i) : series_mem_size (%i)", series_input_height, num_inputs, series_mem_size);
                addErrorString(s);
            }
            series_data_input_mismatch = true;
            //return; 
        }
        int dindex = 0;
        for (int i = 0; i < series_input_width; i++) {
            int mindex = index - i;
            int mindex2 = mindex * num_inputs;
            for (int j = 0; j < num_inputs; j++) {
                if ((mindex >= 0) && (mindex < num_train_data)) {
                    if (dindex < series_mem_size) {
                        series_data[dindex] = getInputTrainData(mindex2 + j);
                    }
                }
                else {
                    if (dindex < series_mem_size) {
                        series_data[dindex] = 0.0;
                    }
                }
                dindex++;
            }
        }
    }
}

void NetCore::loadValidationSeriesToNet(int index)
{
    if (has_series_conv) {
        static bool series_data_input_mismatch = false;
        if (series_input_height != num_inputs) {
            if (!series_data_input_mismatch) {
                char s[200];  sprintf_s(s, "loadValidationSeriesToNet : series_input_height (%i) not equal to num_inputs (%i) : series_mem_size (%i)", series_input_height, num_inputs, series_mem_size);
                addErrorString(s);
            }
            series_data_input_mismatch = true;
            //return;
        }
        int dindex = 0;
        int mindex = 0;
        for (int j = 0; j < num_inputs; j++) {
            for (int i = 0; i < series_input_width; i++) {
                mindex = index - i;
                if ((mindex >= 0) && (mindex < num_validation_data)) {
                    mindex = mindex * num_inputs;
                    if (dindex < series_mem_size) {
                        series_data[dindex] = getInputValidationData(mindex + j);
                    }
                }
                else {
                    if (dindex < series_mem_size) {
                        series_data[dindex] = 0.0;
                    }
                }
                dindex++;
            }
        }
    }
}

void NetCore::setImageParametersData(int width, int height, bool gray)
{
    image_gray = gray;
    image_width = width;
    image_height = height;
    image_single = image_width * image_height;
    image_max_size = image_single;
    image_max_size = 3 * image_max_size;
}

void NetCore::setInputImageData(double* pointer)
{
    if (image_data_allocated && (image_width > 0)) {
        double mclr = 0.5;
        for (int j = 0; j < image_mem_size; j++) {
            image_data[j] = mclr;
        }
        for (int j = 0; j < image_height; j++) {
            int index1 = j * 3 * image_width;
            int index2 = j * image_width;
            for (int i = 0; i < image_width; i++) {
                if (image_gray) {
                    image_data[index2] = (pointer[index1 + 0] + pointer[index1 + 1] + pointer[index1 + 2]) / 3;
                }
                else {
                    image_data[index2] = pointer[index1 + 0];
                    image_data[index2 + image_single] = pointer[index1 + 1];
                    image_data[index2 + (2 * image_single)] = pointer[index1 + 2];
                }
                index1 += 3;
                index2++;
            }
        }
    }
    
}

void NetCore::loadTrainImageToNet(int index)
{
    if (index < (int)imageTrainIndices.size()) {
        int mindex = imageTrainIndices[index];
        if (mindex < num_train_images) {
            setInputImageData(image_train_data + ((int64_t)mindex * 3 * image_single));
            data_file_index = mindex;
        }
        else {
            char s[200];  sprintf_s(s, "loadTrainImageToNet : mindex = %i : num_train_images = %i", mindex, num_train_images);
            addErrorString(s);
        }
    }
    else {
        char s[200];  sprintf_s(s, "loadTrainImageToNet : index = %i : imageTrainIndices size = %i", index, (int)imageTrainIndices.size());
        addErrorString(s);
    }
}

void NetCore::loadValidationImageToNet(int index)
{
    if (index < (int)imageValidationIndices.size()) {
        int mindex = imageValidationIndices[index];
        if (mindex < num_train_images) {
            setInputImageData(image_train_data + ((int64_t)mindex * 3 * image_single));
            data_file_index = mindex;
        }
        else {
            char s[200];  sprintf_s(s, "loadValidationImageToNet : mindex = %i : num_train_images = %i", mindex, num_train_images);
            addErrorString(s);
        }
    }
    else {
        char s[200];  sprintf_s(s, "loadValidationImageToNet : index = %i : imageValidationIndices size = %i", index, (int)imageValidationIndices.size());
        addErrorString(s);
    }
}

bool NetCore::loadTrainWaveDataToNet(train_index tindex)
{
    if (wave_train_data_allocated) {
        if (tindex.findex < imageTrainIndices.size()) {
            int mindex = imageTrainIndices[tindex.findex];
            if ((mindex < (int)wresults.wave_start_indices.size())) {
                _int64 mstart = wresults.wave_start_indices[mindex] + (_int64)tindex.sindex;
                int msize = wresults.wave_data_lengths[mindex];
                /*if (sound_process_stride > 0) {
                    msize = sound_process_stride;
                }*/
                if (has_wave_conv && has_series_conv && !has_wave_fft_conv) {
                    msize = conv_frame_size;
                    int offset = tindex.sindex * conv_frame_size;
                    if ((msize + offset) >= wresults.wave_data_lengths[mindex]) {
                        msize = wresults.wave_data_lengths[mindex] - offset;
                        if (msize < 0) {
                            msize = 0;
                        }
                    }
                    mstart = wresults.wave_start_indices[mindex] + (_int64)offset;
                }
                if (((mstart + msize) <= wave_allocation_size) && (msize <= wave_mem_size)) {
                    /*static int d13 = 40;
                    if (d13 > 0) {
                        char s[200];  sprintf_s(s, "loadTrainWaveDataToNet : mstart %lli : msize %i : wave_allocation_size %i : wave_mem_size %i", mstart, msize, wave_allocation_size, wave_mem_size);  addErrorString(s);
                        d13--;
                    }*/
                    for (int j = 0; j < msize; j++) {
                        wave_data[j] = wave_train_data[mstart + j];
                    }
                    for (int j = msize; j < wave_mem_size; j++) {
                        wave_data[j] = 0.0;
                    }
                    data_file_index = mindex;
                    return(true);
                }
                char s[200];  sprintf_s(s, "loadTrainWaveDataToNet : mstart %lli : msize %i : wave_allocation_size %i : wave_mem_size %i", mstart, msize, wave_allocation_size, wave_mem_size);  addErrorString(s);
            }
            else {
                char s[200];  sprintf_s(s, "loadTrainWaveDataToNet : mindex (%i) greater than wave_start_indices size (%i)", mindex, (int)wresults.wave_start_indices.size());
                addErrorString(s);
            }
        }
        else {
            char s[200];  sprintf_s(s, "loadTrainWaveDataToNet unable to load : findex = %i : size = %i", tindex.findex, (int)imageTrainIndices.size());
            addErrorString(s);
        }
    }
    else {
        addErrorString("loadTrainWaveDataToNet : memory not allocated");
    }
    return(false);
}

bool NetCore::loadValidationWaveDataToNet(train_index tindex)
{
    if (wave_train_data_allocated) {
        if (tindex.findex < imageValidationIndices.size()) {
            int mindex = imageValidationIndices[tindex.findex];
            if ((mindex < wresults.wave_start_indices.size())) {
                _int64 mstart = wresults.wave_start_indices[mindex] + (_int64)tindex.sindex;
                int msize = wresults.wave_data_lengths[mindex];
                if (has_wave_conv && has_series_conv && !has_wave_fft_conv) {
                    msize = conv_frame_size;
                    int offset = tindex.sindex * conv_frame_size;
                    if ((msize + offset) >= wresults.wave_data_lengths[mindex]) {
                        msize = wresults.wave_data_lengths[mindex] - offset;
                        if (msize < 0) {
                            msize = 0;
                        }
                    }
                    mstart = wresults.wave_start_indices[mindex] + (_int64)offset;
                }
                if (((mstart + msize) <= wave_allocation_size) && (msize <= wave_mem_size)) {
                    for (int j = 0; j < msize; j++) {
                        wave_data[j] = wave_train_data[mstart + j];
                    }
                    for (int j = msize; j < wave_mem_size; j++) {
                        wave_data[j] = 0.0;
                    }
                    data_file_index = mindex;
                    return(true);
                }
                char s[200];  sprintf_s(s, "loadValidationWaveDataToNet : findex = %i : sindex = %i : mstart %lli : msize %i : wave_allocation_size %i : wave_mem_size %i", tindex.findex, tindex.sindex, mstart, msize, wave_allocation_size, wave_mem_size);
                addErrorString(s);
            }
            else {
                char s[200];  sprintf_s(s, "loadValidationWaveDataToNet : mindex (%i/%i) greater than wave_start_indices size (%i)", mindex, tindex.dindex, (int)wresults.wave_start_indices.size());
                addErrorString(s);
            }
        }
        else {
            char s[200];  sprintf_s(s, "loadValidationWaveDataToNet unable to load : findex = %i : size = %i", tindex.findex, (int)imageValidationIndices.size());
            addErrorString(s);
        }
    }
    else {
        addErrorString("loadValidationWaveDataToNet : memory not allocated");
    }
    return(false);
}

void NetCore::clearProcessedData()
{
    if (processedData.size() > 0) {
        for (int i = 0; i < (int)processedData.size(); i++) {
            processedData[i].file_path = "";
            if (processedData[i].set_data.size() > 0) {
                for (int j = 0; j < (int)processedData[i].set_data.size(); j++) {
                    processedData[i].set_data[j].header = "";
                    processedData[i].set_data[j].column_data.clear();
                    processedData[i].set_data[j].column_string_data.clear();
                }
                processedData[i].set_data.clear();
            }
        }
        processedData.clear();
    }
    if (processedValidationData.size() > 0) {
        for (int i = 0; i < (int)processedValidationData.size(); i++) {
            processedValidationData[i].file_path = "";
            if (processedValidationData[i].set_data.size() > 0) {
                for (int j = 0; j < (int)processedValidationData[i].set_data.size(); j++) {
                    processedValidationData[i].set_data[j].header = "";
                    processedValidationData[i].set_data[j].column_data.clear();
                    processedValidationData[i].set_data[j].column_string_data.clear();
                }
                processedValidationData[i].set_data.clear();
            }
        }
        processedValidationData.clear();
    }
}

bool NetCore::loadFileData()
{
    // Load data if data filenames have changed
    if ((dataPathnames != previousDataPathnames) || (validationPathnames != previousValidationPathnames)) {

        CDataOperations dops;

        // Clear CSV file data vector
        if (csvFileData.size() > 0) {
            for (int i = 0; i < (int)csvFileData.size(); i++) {
                csvFileData[i].file_path = "";
                if (csvFileData[i].set_data.size() > 0) {
                    for (int j = 0; j < (int)csvFileData[i].set_data.size(); j++) {
                        csvFileData[i].set_data[j].header = "";
                        csvFileData[i].set_data[j].column_data.clear();
                        csvFileData[i].set_data[j].column_string_data.clear();
                    }
                    csvFileData[i].set_data.clear();
                }
            }
            csvFileData.clear();
        }

        // Add train and validation pathnames into a single vector
        vector<string> mycombinedPathnames;
        if (dataPathnames.size() > 0) {
            for (int i = 0; i < (int)dataPathnames.size(); i++) {
                if (dataPathnames[i] != "") {
                    //addErrorString("adding data pathname : " + dataPathnames[i]);
                    mycombinedPathnames.push_back(dataPathnames[i]);
                }
            }
        }
        if (validationPathnames.size() > 0) {
            for (int i = 0; i < (int)validationPathnames.size(); i++) {
                if (validationPathnames[i] != "") {
                    //addErrorString("adding validation pathname : " + validationPathnames[i]);
                    mycombinedPathnames.push_back(validationPathnames[i]);
                }
            }
        }

        // If CSV files load them
        if (dops.areFilesCsv(dataPathnames)) {
            if (mycombinedPathnames.size() > 0) {
                for (int i = 0; i < (int)mycombinedPathnames.size(); i++) {
                    int percent = (int)((100.0F * (float)i) / (float)mycombinedPathnames.size());
                    showPopUpMessage("Getting CSV file data . . .", percent);
                    dops.getDataFromCsv(mycombinedPathnames[i], &csvFileData);
                }
                closePopUp();
            }
            else {
                addErrorString("loadFileData : empty filename list");
                addPanel1String("");
                return(false);
            }
        }

        // If wave files need to know the longest file to set max save length
        if (dops.areFilesWave(dataPathnames)) {
            addPanel1String("Getting wave max length");
            wave_files_longest = dops.getLongestWaveFile(mycombinedPathnames);
        }

        addPanel1String("");

        // Diagnostics - show csv headers
        /*if (csvFileData.size() > 0) {
            if (csvFileData[0].set_data.size() > 0) {
                for (int i = 0; i < (int)csvFileData[0].set_data.size(); i++) {
                    addErrorString(csvFileData[0].set_data[i].header);
                }
            }
        }*/

    }

    previousDataPathnames = dataPathnames;
    previousValidationPathnames = validationPathnames;

    return(true);

}

bool NetCore::getTrainDataFromFileData()
{
    CLineParser func;
    CDataOperations dops;

    clearProcessedData();

    // Can we change inputFields to simple single variable
    for (int i = 0; i < (int)netInputs.size(); i++) {
        if (netInputs[i].inputFields.size() != 1) {
            char s[200];  sprintf_s(s, "getTrainDataFromFileData : netInputs[i].inputFields.size = %i : ", (int)netInputs[i].inputFields.size()); addErrorString(s + netInputs[i].nodeId + " **********");
        }
    }

    // ************************************************************************
    // Get fields from net inputs
    showPopUpMessage("Processing input fields . . .", 10);
    vector<string> fields;
    vector<string> no_image_fields;
    for (int i = 0; i < (int)netInputs.size(); i++) {
        if (netInputs[i].inputFields.size() > 0) {
            for (int j = 0; j < (int)netInputs[i].inputFields.size(); j++) {
                if (fields.size() > 0) {
                    bool found = false;
                    for (int k = 0; k < (int)fields.size(); k++) {
                        if (fields[k] == netInputs[i].inputFields[j]) {
                            k = (int)fields.size();
                            found = true;
                        }
                    }
                    if (!found && (netInputs[i].inputFields[j] != "")) {
                        no_image_fields.push_back(netInputs[i].inputFields[j]);
                        fields.push_back(netInputs[i].inputFields[j]);
                    }
                }
                else if (netInputs[i].inputFields[j] != "") {
                    no_image_fields.push_back(netInputs[i].inputFields[j]);
                    fields.push_back(netInputs[i].inputFields[j]);
                }
            }
        }
    }
    // Next get fields from net outputs
    for (int i = 0; i < (int)netOutputs.size(); i++) {
        bool found = false;
        for (int k = 0; k < (int)fields.size(); k++) {
            if (fields[k] == netOutputs[i].outputField) {
                k = (int)fields.size();
                found = true;
            }
        }
        if (!found && (netOutputs[i].outputField != "")) {
            if (!netOutputs[i].imageOutput) {
                no_image_fields.push_back(netOutputs[i].outputField);
            }
            fields.push_back(netOutputs[i].outputField);
        }
    }

    // Diagnostics - show field names
    /*for (int i = 0; i < (int)fields.size(); i++) {
        addErrorString(fields[i]);
    }*/

    // ************************************************************************
    // Check for and get CSV data
    if (dops.areFilesCsv(dataPathnames)) {
        // Get data from train pathnames
        showPopUpMessage("Getting process data from file data . . .", 20);
        if (!getProcessedDataFromFileData(dataPathnames, no_image_fields, &processedData, &csvFileData)) {
            addErrorString("Error getting train data from file data");
            return(false);
        }
        if (use_validation_files) {
            // Get data from train pathnames
            showPopUpMessage("Getting validation data from file data . . .", 20);
            if (!getProcessedDataFromFileData(validationPathnames, no_image_fields, &processedValidationData, &csvFileData)) {
                addErrorString("Error getting validation data from file data");
                return(false);
            }
        }
    }


    // ************************************************************************
    // Check for missing columns in data and add empty if necessary
    for (int i = 0; i < (int)dataPathnames.size(); i++) {
        int percent = 20 + (int)((70.0F * (float)i) / (float)dataPathnames.size());
        showPopUpMessage("Adding missing train columns . . .", percent);
        dops.addMissingDataColumns(dataPathnames[i], fields, netOutputs, &processedData);
    }
    if (use_validation_files) {
        for (int i = 0; i < (int)validationPathnames.size(); i++) {
            int percent = 20 + (int)((70.0F * (float)i) / (float)validationPathnames.size());
            showPopUpMessage("Adding missing validation columns . . .", percent);
            dops.addMissingDataColumns(validationPathnames[i], fields, netOutputs, &processedValidationData);
        }
    }

    // Do non csv file processing (must be after adding missing column because getMaxWaveMagnitude uses processedData and processedValidationData)
    if (dops.areFilesJpg(dataPathnames)) {
    }
    else if (dops.areFilesWave(dataPathnames)) {
        // Checking and getting wave sizing here to adjust column data sizes if nedded
        if (getMaxWaveMagnitude()) {
            if (checkForSoundFFTs()) {
                int fft_size = stoi(soundfftprocess.substr(3));
                dops.fillSoundFFTProcessData(fft_size, soundfftstride, soundfftdownsample, &wresults, &processedData);
                if (use_validation_files) {
                    dops.fillSoundFFTProcessData(fft_size, soundfftstride, soundfftdownsample, &wresults, &processedValidationData);
                }
            }
        }
        else {
            addErrorString("Error getting wave sizing data");
            return(false);
        }
    }

    // Add column data for sound convolutions
    if (has_wave_conv && has_series_conv && !has_wave_fft_conv) {
        for (int i = 0; i < (int)processedData.size(); i++) {
            int percent = 20 + (int)((70.0F * (float)i) / (float)processedData.size());
            showPopUpMessage("Adding missing train rows . . .", percent);
            int nsteps =  getSoundConvNumStepsThroughTrainWaveFile(i);
            if (nsteps > 0) {
                dops.addSoundConvDataRows(processedData[i].file_path, nsteps, &processedData);
            }
            else {
                addErrorString("Conv input size greater than file size for " + processedData[i].file_path + " - consider decreasing conv input size");
            }
        }
        if (use_validation_files) {
            for (int i = 0; i < (int)processedValidationData.size(); i++) {
                int percent = 20 + (int)((70.0F * (float)i) / (float)processedValidationData.size());
                showPopUpMessage("Adding missing validation rows . . .", percent);
                //int vi = (int)processedData.size();
                int nsteps =  getSoundConvNumStepsThroughValidationWaveFile(i);
                if (nsteps > 0) {
                    dops.addSoundConvDataRows(processedValidationData[i].file_path, nsteps, &processedValidationData);
                }
                else {
                    addErrorString("Conv input size greater than file size for " + processedValidationData[i].file_path + " - consider decreasing conv input size");
                }
            }
        }
    }

    // ************************************************************************
    // Adjust data alignment by offset indices
    showPopUpMessage("Applying index offsets . . .", 10);
    dops.applyOffsetToData(&processedData);

    // Perform normalization and other processing to data by input fields
    showPopUpMessage("Processing input train data . . .", 20);
    vector<string> fftnames;
    vector< processData> fftprocesses;
    for (int i = 0; i < (int)netInputs.size(); i++) {
        if (netInputs[i].inputFields.size() > 0) {
            for (int j = 0; j < (int)netInputs[i].inputFields.size(); j++) {
                if (netInputs[i].inputFields[j].substr(0, 7) == "Output:") {
                    // do nothing
                }
                else if (netInputs[i].inputFields[j].substr(0, 3) == "FFT") {
                    if (netInputs[i].inputFields[j].substr(netInputs[i].inputFields[j].length() - 9) != "SoundFile") {
                        string fftname = func.getStringAfterLastColon(netInputs[i].inputFields[j]);
                        if (fftnames.size() > 0) {
                            bool found = false;
                            for (int k = 0; k < (int)fftnames.size(); k++) {
                                if (fftname == fftnames[k]) {
                                    k = (int)fftnames.size();
                                    found = true;
                                }
                            }
                            if (!found) {
                                fftnames.push_back(fftname);
                                fftprocesses.push_back(netInputs[i].processdata);
                            }
                        }
                        else {
                            fftnames.push_back(fftname);
                            fftprocesses.push_back(netInputs[i].processdata);
                        }
                    }
                }
                else {
                    string fn = netInputs[i].inputFields[j];
                    processData tprocess = netInputs[i].processdata;
                    if (netInputs[i].from_data_conv) {
                        if (!dops.getProcessData(&fn, &tprocess, &processedData)) {
                            addErrorString("getTrainDataFromFileData : " + fn + " : unable to process individual normalization - reverting to group");
                        }
                    }
                    dops.processDataColumn(&fn, tprocess, sigmoid_leakage, &processedData);
                }
            }
        }
    }

    // If determined above FFTs are needed, do them
    if (fftnames.size() > 0) {
        showPopUpMessage("Processing fft train data . . .", 50);
        for (int k = 0; k < (int)fftnames.size(); k++) {
             dops.processFFTColumn(fftnames[k], fftprocesses[k], &processedData);
        }
    }

    // Perform normalization and other processing to data by output fields
    showPopUpMessage("Processing output train data . . .", 80);
    if (netOutputs.size() > 0) {
        for (int i = 0; i < (int)netOutputs.size(); i++) {
            if (!netOutputs[i].imageOutput) {
                string fn = netOutputs[i].outputField;
                dops.processDataColumn(&fn, netOutputs[i].processdata, sigmoid_leakage, &processedData);
                if (fn != netOutputs[i].outputField) {
                    netOutputs[i].outputField = fn;
                    netOutputs[i].outputFieldAlias = fn;
                }
                //char s[200];  sprintf_s(s, "processDataColumn : output index %i : ", i); addErrorString(s + netOutputs[i].outputField + " : " + fn + " : " + netOutputs[i].processdata.key1);
            }
        }
    }

    /*if (processedData.size() > 0) {
        if (processedData[0].set_data.size() > 0) {
            for (int i = 0; i < (int)processedData[0].set_data.size(); i++) {
                addErrorString("getTrainDataFromFileData : " + processedData[0].set_data[i].header);
            }
        }
    }*/

    // ************************************************************************
    if (use_validation_files) {
        // Adjust data alignment by offset indices
        showPopUpMessage("Applying validation index offsets . . .", 10);
        dops.applyOffsetToData(&processedValidationData);

        // Perform normalization and other processing to data by input fields
        showPopUpMessage("Processing input validation data . . .", 20);
        for (int i = 0; i < (int)netInputs.size(); i++) {
            if (netInputs[i].inputFields.size() > 0) {
                for (int j = 0; j < (int)netInputs[i].inputFields.size(); j++) {
                    if (netInputs[i].inputFields[j].substr(0, 7) == "Output:") {
                        // do nothing
                    }
                    else if (netInputs[i].inputFields[j].substr(0, 3) == "FFT") {
                        if (netInputs[i].inputFields[j].substr(netInputs[i].inputFields[j].length() - 9) != "SoundFile") {
                            string fftname = func.getStringAfterLastColon(netInputs[i].inputFields[j]);
                            if (fftnames.size() > 0) {
                                bool found = false;
                                for (int k = 0; k < (int)fftnames.size(); k++) {
                                    if (fftname == fftnames[k]) {
                                        k = (int)fftnames.size();
                                        found = true;
                                    }
                                }
                                if (!found) {
                                    fftnames.push_back(fftname);
                                    fftprocesses.push_back(netInputs[i].processdata);
                                }
                            }
                            else {
                                fftnames.push_back(fftname);
                                fftprocesses.push_back(netInputs[i].processdata);
                            }
                        }
                    }
                    else {
                        string fn = netInputs[i].inputFields[j];
                        dops.processDataColumn(&fn, netInputs[i].processdata, sigmoid_leakage, &processedValidationData);
                    }
                }
            }
        }

        // If determined above FFTs are needed, do them
        if (fftnames.size() > 0) {
            showPopUpMessage("Processing fft validation data . . .", 50);
            for (int k = 0; k < (int)fftnames.size(); k++) {
                dops.processFFTColumn(fftnames[k], fftprocesses[k], &processedValidationData);
            }
        }

        // Perform normalization and other processing to data by output fields
        showPopUpMessage("Processing output validation data . . .", 80);
        for (int i = 0; i < (int)netOutputs.size(); i++) {
            if (!netOutputs[i].imageOutput) {
                string fn = netOutputs[i].outputField;
                dops.processDataColumn(&fn, netOutputs[i].processdata, sigmoid_leakage, &processedValidationData);
                if (fn != netOutputs[i].outputField) {
                    netOutputs[i].outputField = fn;
                    netOutputs[i].outputFieldAlias = fn;
                }
            }
        }
    }


    // ************************************************************************
    if (use_percent_validation) {
        if (validation_percent > 0) {

            // For percent validation, get total number of train rows
            showPopUpMessage("Counting validation rows . . .", 5);
            int tnrows = 0;
            int mvrows = 0;
            if (processedData.size() > 0) {
                for (int i = 0; i < (int)processedData.size(); i++) {
                    if (processedData[i].set_data.size() > 0) {
                        tnrows += (int)processedData[i].set_data[0].column_data.size();
                    }
                }
                mvrows = (int)((validation_percent * (float)tnrows) / 100);
            }

            // Make selectors to determine validation or not
            vector<vector<bool>> selectors;
            for (int i = 0; i < (int)processedData.size(); i++) {
                vector<bool> tsel;
                selectors.push_back(tsel);
                for (int j = 0; j < processedData[i].set_data[0].column_data.size(); j++) {
                    selectors[i].push_back(true);
                }
            }
            for (int j = 0; j < mvrows; j++) {
                int cnt = 0;
                bool found = false;
                while (!found && (cnt < 10)) {
                    int tsum = 0;
                    int findex = 0;
                    int rindex = 0;
                    int tindex = ((tnrows) * (int64_t)rand()) / (int64_t)RAND_MAX;
                    for (int i = 0; i < (int)processedData.size(); i++) {
                        rindex = tindex - tsum;
                        if (processedData[i].set_data.size() > 0) {
                            tsum += (int)processedData[i].set_data[0].column_data.size();
                        }
                        if (tsum >= tindex) {
                            findex = i;
                            i = (int)processedData.size();
                        }
                    }
                    if ((findex < 0) || (findex >= (int)processedData.size())) {
                        findex = 0;
                    }
                    if ((rindex < 0) || (rindex >= processedData[findex].set_data[0].column_data.size())) {
                        rindex = 0;
                    }
                    //char s[200];  sprintf_s(s, "findex = %i : rindex = %i", findex, rindex); addErrorString(s);
                    if (selectors[findex][rindex]) {
                        selectors[findex][rindex] = false;
                        found = true;
                    }
                    cnt++;
                    if (cnt >= 10) {
                        //addErrorString("tp1");
                    }
                }
            }

            // Tempory processed set
            vector<dataset2> tprocessedData;

            // Fill sets by selectors
            int pindex = -1;
            int vindex = -1;
            for (int i = 0; i < (int)processedData.size(); i++) {
                bool pfound = false;
                bool vfound = false;
                for (int k = 0; k < processedData[i].set_data[0].column_data.size(); k++) {
                    if (selectors[i][k]) {
                        if (!pfound) {
                            pindex++;
                            dataset2 tpdataset;
                            tprocessedData.push_back(tpdataset);
                            tprocessedData[pindex].file_path = processedData[i].file_path;
                            for (int j = 0; j < processedData[i].set_data.size(); j++) {
                                data_column2 tpcolumn;
                                tprocessedData[pindex].set_data.push_back(tpcolumn);
                                tprocessedData[pindex].set_data[j].header = processedData[i].set_data[j].header;
                            }
                            pfound = true;
                        }
                        for (int j = 0; j < processedData[i].set_data.size(); j++) {
                            tprocessedData[pindex].set_data[j].column_data.push_back(processedData[i].set_data[j].column_data[k]);
                            tprocessedData[pindex].set_data[j].column_string_data.push_back(processedData[i].set_data[j].column_string_data[k]);
                            //char s[200];  sprintf_s(s, "train size = %i : data = %.4f", (int)tprocessedData[i].set_data[j].column_data.size(), tprocessedData[i].set_data[j].column_data[(int)tprocessedData[i].set_data[j].column_data.size() - 1]); addErrorString(s);
                        }
                    }
                    else {
                        if (!vfound) {
                            vindex++;
                            dataset2 tpdataset;
                            processedValidationData.push_back(tpdataset);
                            processedValidationData[vindex].file_path = processedData[i].file_path;
                            for (int j = 0; j < processedData[i].set_data.size(); j++) {
                                data_column2 tpcolumn;
                                processedValidationData[vindex].set_data.push_back(tpcolumn);
                                processedValidationData[vindex].set_data[j].header = processedData[i].set_data[j].header;
                            }
                            vfound = true;
                        }
                        for (int j = 0; j < processedData[i].set_data.size(); j++) {
                            processedValidationData[vindex].set_data[j].column_data.push_back(processedData[i].set_data[j].column_data[k]);
                            processedValidationData[vindex].set_data[j].column_string_data.push_back(processedData[i].set_data[j].column_string_data[k]);
                            //char s[200];  sprintf_s(s, "validation size = %i : data = %.4f", (int)processedValidationData[i].set_data[j].column_data.size(), processedValidationData[i].set_data[j].column_data[(int)processedValidationData[i].set_data[j].column_data.size()-1]); addErrorString(s);
                        }
                    }
                }
            }

            processedData.clear();
            processedData = tprocessedData;

        }
    }


    // ************************************************************************
    showPopUpMessage("Finishing data processing . . .", 90);
    // Get num train data rows
    num_filedata_rows = 0;
    for (int i = 0; i < (int)processedData.size(); i++) {
        if (processedData[i].set_data.size() > 0) {
            num_filedata_rows += (int)processedData[i].set_data[0].column_data.size();
        }
    }

    // Get num validation data rows
    num_validationdata_rows = 0;
    for (int i = 0; i < (int)processedValidationData.size(); i++) {
        if (processedValidationData[i].set_data.size() > 0) {
            num_validationdata_rows += (int)processedValidationData[i].set_data[0].column_data.size();
        }
    }
    //char s[200];  sprintf_s(s, "num_validationdata_rows = %i", num_validationdata_rows); addErrorString(s);

    // Get train indices array by file group
    int total_count = 0;
    fileTrainIndicies.clear();
    for (int i = 0; i < (int)processedData.size(); i++) {
        if (processedData[i].set_data.size() > 0) {
            fileTrainIndicies.push_back(total_count);
            total_count += (int)processedData[i].set_data[0].column_data.size();
        }
    }

    // Get train sizes by file group
    fileTrainSizes.clear();
    for (int i = 0; i < (int)processedData.size(); i++) {
        if (processedData[i].set_data.size() > 0) {
            fileTrainSizes.push_back((int)processedData[i].set_data[0].column_data.size());
        }
    }

    // Get validation indices array by file group
    total_count = 0;
    fileValidationIndicies.clear();
    for (int i = 0; i < (int)processedValidationData.size(); i++) {
        if (processedValidationData[i].set_data.size() > 0) {
            fileValidationIndicies.push_back(total_count);
            total_count += (int)processedValidationData[i].set_data[0].column_data.size();
        }
    }

    // Get validation sizes by file group
    fileValidationSizes.clear();
    for (int i = 0; i < (int)processedValidationData.size(); i++) {
        if (processedValidationData[i].set_data.size() > 0) {
            fileValidationSizes.push_back((int)processedValidationData[i].set_data[0].column_data.size());
        }
    }

    closePopUp();

    /*if (processedData.size() > 0) {
        if (processedData[0].set_data.size() > 0) {
            for (int i = 0; i < (int)processedData[0].set_data.size(); i++) {
                addErrorString("processedData[0].set_data.header = " + processedData[0].set_data[i].header);
            }
        }
    }*/

    return(true);

}

bool NetCore::getProcessedDataFromFileData(vector<string> fnames, vector<string> fields, vector<dataset2>* processedData, vector<dataset2>* fileData)
{
    if ((fnames.size() > 0) && (fields.size() > 0)) {
        CDataOperations dops;
        for (int i = 0; i < (int)fnames.size(); i++) {
            int percent = 20 + (int)((70.0F * (float)i) / (float)fnames.size());
            showPopUpMessage("Getting process data from file data . . .", percent);
            dops.getProcessedDataFromFileData(fnames[i], fields, processedData, fileData);
            if (dops.csv_with_filename) {
                csv_with_filename = true;
                csv_filename_header = dops.csv_filename_header;
                csv_image_filenames = dops.csv_image_filenames;
            }
            else if ((csv_with_filename != dops.csv_with_filename) || (csv_filename_header != dops.csv_filename_header)) {
                addErrorString("getProcessedDataFromFileData : problem with csv in filename check");
            }
        }
    }
    else if (fnames.size() <= 0) {
        addErrorString("getProcessedDataFromFileData : empty filename list");
        addPanel1String("");
        return(false);
    }
    else {
        addErrorString("getProcessedDataFromFileData : empty data field list");
        addPanel1String("");
        return(false);
    }
    addPanel1String("");
    return(true);
}

int NetCore::getSoundConvNumStepsThroughTrainWaveFile(int pindex)
{
    int nsteps = 0;
    if (has_wave_conv && has_series_conv && !has_wave_fft_conv) {
        if (processedData.size() > 0) {
            if (processedData[pindex].set_data.size() > 0) {
                if (has_wave_conv && !has_wave_fft_conv) {
                    if (conv_frame_size > 0) {
                        if (pindex < (int)wresults.wave_data_lengths.size()) {
                            //nsteps = wresults.wave_data_lengths[i] - conv_frame_size;
                            nsteps = wresults.wave_data_lengths[pindex] / conv_frame_size;
                            if (nsteps < 1) {
                                nsteps = 1;
                            }
                        }
                    }
                }
                else {
                    nsteps = (int)processedData[pindex].set_data[0].column_data.size() - conv_frame_size;
                }
            }
        }
    }
    return(nsteps);
}

int NetCore::getSoundConvNumStepsThroughValidationWaveFile(int pindex)
{
    int nsteps = 0;
    if (has_wave_conv && has_series_conv && !has_wave_fft_conv) {
        if (processedValidationData.size() > 0) {
            if (processedValidationData[pindex].set_data.size() > 0) {
                if (has_wave_conv && !has_wave_fft_conv) {
                    if (conv_frame_size > 0) {
                        if (pindex < (int)wresults.wave_data_lengths.size()) {
                            //nsteps = wresults.wave_data_lengths[i] - conv_frame_size;
                            nsteps = wresults.wave_data_lengths[pindex] / conv_frame_size;
                            if (nsteps < 1) {
                                nsteps = 1;
                            }
                        }
                    }
                }
                else {
                    nsteps = (int)processedValidationData[pindex].set_data[0].column_data.size() - conv_frame_size;
                }
            }
        }
    }
    return(nsteps);
}

bool NetCore::getImageAndWaveData()
{
    if (!getImages()) {
        return(false);
    }
    if (!getSoundData()) {
        return(false);
    }
    return(true);
}

bool NetCore::getImages()
{
    CDataOperations dops;
    if (dops.areFilesJpg(dataPathnames) || dops.areFilesRaw(dataPathnames) || csv_with_filename) {
        if (image_train_data_allocated) {
            if (processedData.size() > 0) {
                if (csv_with_filename) {
                    int cnt = 0;
                    combinedPathnames.clear();
                    imageTrainIndices.clear();
                    imageValidationIndices.clear();
                    for (int i = 0; i < (int)csv_image_filenames.size(); i++) {
                        combinedPathnames.push_back(csv_image_filenames[i]);
                        imageTrainIndices.push_back(cnt);
                        cnt++;
                    }
                    if (validationPathnames.size() > 0) {
                        addErrorString("Currently unable to process validation images for image files in CSV");
                    }
                }
                else {
                    int cnt = 0;
                    combinedPathnames.clear();
                    imageTrainIndices.clear();
                    imageValidationIndices.clear();
                    for (int i = 0; i < (int)processedData.size(); i++) {
                        combinedPathnames.push_back(processedData[i].file_path);
                        imageTrainIndices.push_back(cnt);
                        cnt++;
                    }

                    if (processedValidationData.size() > 0) {
                        for (int i = 0; i < (int)processedValidationData.size(); i++) {
                            combinedPathnames.push_back(processedValidationData[i].file_path);
                            imageValidationIndices.push_back(cnt);
                            cnt++;
                        }
                    }
                }
                if (!dops.getImages(combinedPathnames, image_width, image_height, image_train_data_size, image_train_data)) {
                    return(false);
                }
            }
            else {
                addErrorString("getImages : processedData empty");
                return(false);
            }
        }
        else {
            addErrorString("getImages : image memory not allocated");
            return(false);
        }
    }
    return(true);
}

bool NetCore::checkForSoundFFTs()
{
    process_sound_ffts = false;
    soundfftnames.clear();
    soundfftprocess = "";
    soundfftstride = 1;
    soundfftdownsample = 1;
    for (int i = 0; i < (int)netInputs.size(); i++) {
        if (netInputs[i].inputFields.size() > 0) {
            for (int j = 0; j < (int)netInputs[i].inputFields.size(); j++) {
                if (netInputs[i].inputFields[j].substr(0, 3) == "FFT") {
                    if (netInputs[i].inputFields[j].substr(netInputs[i].inputFields[j].length() - 9) == "SoundFile") {
                        process_sound_ffts = true;
                        string fftname = netInputs[i].inputFields[j];
                        if (soundfftnames.size() > 0) {
                            bool found = false;
                            for (int k = 0; k < (int)soundfftnames.size(); k++) {
                                if (fftname == soundfftnames[k]) {
                                    k = (int)soundfftnames.size();
                                    found = true;
                                }
                            }
                            if (!found) {
                                soundfftnames.push_back(fftname);
                                if ((soundfftprocess != netInputs[i].processdata.process) || (soundfftstride != netInputs[i].processdata.stride) || (soundfftdownsample != netInputs[i].processdata.downsample)) {
                                    static bool spmessage = true;
                                    if (spmessage) {
                                        addErrorString("Problem with setting sound process : " + soundfftprocess + " : " + netInputs[i].processdata.process);
                                        spmessage = false;
                                    }
                                }
                            }
                        }
                        else {
                            soundfftnames.push_back(fftname);
                            soundfftprocess = netInputs[i].processdata.process;
                            soundfftstride = netInputs[i].processdata.stride;
                            soundfftdownsample = netInputs[i].processdata.downsample;
                        }
                    }
                }
            }
        }
    }
    return(process_sound_ffts);
}

bool NetCore::getMaxWaveMagnitude()
{
    CDataOperations dops;
    if (dops.areFilesWave(dataPathnames)) {
        int cnt = 0;
        combinedPathnames.clear();
        imageTrainIndices.clear();
        imageValidationIndices.clear();
        if (processedData.size() > 0) {
            for (int i = 0; i < (int)processedData.size(); i++) {
                combinedPathnames.push_back(processedData[i].file_path);
                imageTrainIndices.push_back(cnt);
                cnt++;
            }
            if (processedValidationData.size() > 0) {
                for (int i = 0; i < (int)processedValidationData.size(); i++) {
                    combinedPathnames.push_back(processedValidationData[i].file_path);
                    imageValidationIndices.push_back(cnt);
                    cnt++;
                }
            }
            //char s[200];  sprintf_s(s, "num train indices = %i : num validation indices = %i", (int)imageTrainIndices.size(), (int)imageValidationIndices.size()); addErrorString(s);
        }
        else if (dataPathnames.size() > 0) {
            for (int i = 0; i < (int)dataPathnames.size(); i++) {
                combinedPathnames.push_back(dataPathnames[i]);
            }
            if (validationPathnames.size() > 0) {
                for (int i = 0; i < (int)validationPathnames.size(); i++) {
                    combinedPathnames.push_back(validationPathnames[i]);
                }
            }
        }
        if (dops.getMaxWaveMagnitude(combinedPathnames, &wresults)) {
            //char s[200];  sprintf_s(s, "tp0 : sound_process_stride = %i", sound_process_stride);  addErrorString(s);
            if ((sound_data_process_type != NONE) && (sound_data_process_type != FULLY_CONNECTED_FOLLOWING) && (sound_process_stride > 0)) {
                wresults.wave_stride = sound_process_stride;
                if (sound_data_process_type == CONV_FOLLOWING) {
                    wresults.num_copy_iterations.clear();
                    for (int i = 0; i < wresults.wave_data_lengths.size(); i++) {
                        int nsteps = wresults.wave_data_lengths[i] / sound_process_stride;
                        //char s[200];  sprintf_s(s, "nsteps = %i", nsteps);  addErrorString(s);
                        if (i < (int)wresults.num_copy_iterations.size()) {
                            wresults.num_copy_iterations[i] = nsteps;
                        }
                        else {
                            wresults.num_copy_iterations.push_back(nsteps);
                        }
                    }
                }
                else if (wresults.wave_data_lengths.size() == wresults.num_copy_iterations.size()) {
                    if ((sound_process_stride > 0) && (sound_down_sample > 0) && (sound_data_stride > 0)) {
                        for (int i = 0; i < wresults.wave_data_lengths.size(); i++) {
                            int nsteps = wresults.wave_data_lengths[i] / ((sound_down_sample * sound_process_stride) + sound_data_stride);
                            if (i < (int)wresults.num_copy_iterations.size()) {
                                wresults.num_copy_iterations[i] = nsteps;
                            }
                            else {
                                wresults.num_copy_iterations.push_back(nsteps);
                            }
                        }
                    }
                    else {
                        addErrorString("getMaxWaveMagnitude : Error setting sound nsteps - stride or downsample = 0");
                        return(false);
                    }
                }
                else {
                    char s[200];  sprintf_s(s, "getMaxWaveMagnitude : Unable to verify sound nsteps : nnet.wave_data_lengths.size = %i : wresults.num_copy_iterations.size = %i", (int)wresults.wave_data_lengths.size(), (int)wresults.num_copy_iterations.size());
                    addErrorString(s);
                    return(false);
                }
            }
            if (wresults.files_longest != wave_files_longest) {
                char s[200];  sprintf_s(s, "getMaxWaveMagnitude : Problem with longest file match : wresults.files_longest = %i : wave_files_longest = %i", wresults.files_longest, wave_files_longest);
                addErrorString(s);
                return(false);
            }
            return(true);
        }
        return(false);
    }
    return(true);
}

bool NetCore::getSoundData()
{
    CDataOperations dops;
    if (dops.areFilesWave(dataPathnames)) {
        if (wave_train_data_allocated) {
            for (int j = 0; j < wave_allocation_size; j++) {
                wave_train_data[j] = 0.0;
            }
            if (dops.getSoundData(combinedPathnames, &wresults, wave_allocation_size, wave_train_data)) {
                return(true);
            }
        }
        else if (process_sound_ffts) {
            int fft_size = stoi(soundfftprocess.substr(3));
            if (dops.processSoundFFTData(fft_size, soundfftstride, soundfftdownsample, normaiize_file_set, &wresults, &processedData)) {
                if (dovalidation) {
                    if (dops.processSoundFFTData(fft_size, soundfftstride, soundfftdownsample, normaiize_file_set, &wresults, &processedValidationData)) {
                        return(true);
                    }
                }
            }
        }
        return(false);
    }
    return(true);
}

int NetCore::getCombinedImageCount()
{
    vector<string> tcombinedPathnames1;
    CStatus myProgressDlg;
    myProgressDlg.CreateDlg();
    myProgressDlg.setLabel("Combining train files . . .");
    myProgressDlg.setProgress(0);
    int numfiles = (int)dataPathnames.size();
    int numvalfiles = (int)validationPathnames.size();
    if (numfiles > 0) {
        int pupdatecnt = 0;
        int pupdatelimit = numfiles / 10;
        for (int j = 0; j < numfiles; j++) {
            if (tcombinedPathnames1.size() > 0) {
                bool found = false;
                for (int k = 0; k < tcombinedPathnames1.size(); k++) {
                    if (dataPathnames[j] == tcombinedPathnames1[k]) {
                        found = true;
                        k = (int)tcombinedPathnames1.size();
                    }
                }
                if (!found && (dataPathnames[j] != "")) {
                    tcombinedPathnames1.push_back(dataPathnames[j]);
                }
            }
            else {
                if (dataPathnames[j] != "") {
                    tcombinedPathnames1.push_back(dataPathnames[j]);
                }
            }
            pupdatecnt++;
            if (pupdatecnt >= pupdatelimit) {
                myProgressDlg.setProgress((100 * j) / numfiles);
                pupdatecnt = 0;
            }
        }
    }
    if (use_validation_files) {
        if (numvalfiles > 0) {
            if ((numvalfiles > 1) || (validationPathnames[0] != "")) {
                int pupdatecnt = 0;
                int pupdatelimit = numvalfiles / 10;
                myProgressDlg.setLabel("Combining validation files . . .");
                for (int j = 0; j < numvalfiles; j++) {
                    if (tcombinedPathnames1.size() > 0) {
                        bool found = false;
                        for (int k = 0; k < tcombinedPathnames1.size(); k++) {
                            if (validationPathnames[j] == tcombinedPathnames1[k]) {
                                found = true;
                                k = (int)tcombinedPathnames1.size();
                            }
                        }
                        if (!found && (validationPathnames[j] != "")) {
                            tcombinedPathnames1.push_back(validationPathnames[j]);
                        }
                    }
                    else {
                        if (validationPathnames[j] != "") {
                            tcombinedPathnames1.push_back(validationPathnames[j]);
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

    vector<string> tcombinedPathnames2;
    CDataOperations dops;
    if (dops.areFilesJpg(dataPathnames) || csv_with_filename) {
        if (processedData.size() > 0) {
            if (csv_with_filename) {
                int cnt = 0;
                for (int i = 0; i < (int)csv_image_filenames.size(); i++) {
                    tcombinedPathnames2.push_back(csv_image_filenames[i]);
                    cnt++;
                }
                if (validationPathnames.size() > 0) {
                    addErrorString("Currently unable to process validation images for image files in CSV");
                }
            }
            else {
                int cnt = 0;
                for (int i = 0; i < (int)processedData.size(); i++) {
                    tcombinedPathnames2.push_back(processedData[i].file_path);
                    cnt++;
                }

                if (processedValidationData.size() > 0) {
                    for (int i = 0; i < (int)processedValidationData.size(); i++) {
                        tcombinedPathnames2.push_back(processedValidationData[i].file_path);
                        cnt++;
                    }
                }
            }
            if (tcombinedPathnames2 != tcombinedPathnames1) {
                //char s[200];  sprintf_s(s, "**** tcombinedPathnames1 size = %i : tcombinedPathnames2 = %i ****", (int)tcombinedPathnames1.size(), (int)tcombinedPathnames2.size());  addErrorString(s);
                //addErrorString("tcombinedPathnames2 != tcombinedPathnames1");
            }
        }
        else {
            addErrorString("getCombinedImages : processedData empty");
        }
    }

    return((int)tcombinedPathnames1.size());
}

bool NetCore::copyProcessedDataToNetTrainArrays()
{
    if (!dataInitializationErrorCheck1()) {
        addErrorString("Unable to copy processed data net");
        return(false);
    }
    if (!dataInitializationErrorCheck2()) {
        addErrorString("Unable to copy processed data net");
        return(false);
    }
    // Get data for all data files
    if (processedData.size() > 0) {
        int cnt = 0;
        int total_cnt = 1;
        for (int i = 0; i < (int)processedData.size(); i++) {
            if (processedData[i].set_data.size() > 0) {
                for (int j = 0; j < (int)processedData[i].set_data.size(); j++) {
                    total_cnt += (int)processedData[i].set_data[j].column_data.size();
                }
            }
        }
        showPopUpMessage("Copying train data to net . . .", 0);
        int offset = 0;
        for (int k = 0; k < (int)processedData.size(); k++) {
            if (processedData[k].set_data.size() > 0) {
                if (netInputs.size() > 0) {
                    for (int j = 0; j < (int)netInputs.size(); j++) {
                        bool found = false;
                        if (netInputs[j].inputFields.size() > 0) {
                            string field = netInputs[j].inputFields[0];
                            if ((netInputs[j].inputFields.size() > 1) && (k == 0)) {
                                addErrorString("copyProcessedDataToNetTrainArrays unable to process multiple inputs for input " + netInputs[j].nodeId);
                            }
                            for (int i = 0; i < (int)processedData[k].set_data.size(); i++) {
                                if (processedData[k].set_data[i].column_data.size() > 0) {
                                    if (field == processedData[k].set_data[i].header) {
                                        found = true;
                                        for (int m = 0; m < (int)processedData[k].set_data[i].column_data.size(); m++) {
                                            int index = j + ((m + offset) * num_inputs);
                                            if (!setInputTrainData(index, processedData[k].set_data[i].column_data[m])) {
                                                addErrorString("copyProcessedDataToNetTrainArrays unable to copy input data output " + netInputs[j].nodeId);
                                                return(false);
                                            }
                                            cnt++;
                                        }
                                        int p = (int)((100.0F * (float)cnt) / (float)total_cnt);
                                        showPopUpMessage("Copying train data to net . . .", p);
                                        i = (int)processedData[k].set_data.size();
                                    }
                                }
                                else if (k == 0) {
                                    addErrorString("copyProcessedDataToNetTrainArrays no column data for " + field + " in file " + processedData[k].file_path);
                                }
                            }
                        }
                        if (!found && (k == 0)) {
                            addErrorString("copyProcessedDataToNetTrainArrays unable to find data for input " + netInputs[j].nodeId);
                        }
                    }
                }
                if (netOutputs.size() > 0) {
                    for (int j = 0; j < (int)netOutputs.size(); j++) {
                        // Copy CSV data
                        if (!netOutputs[j].imageOutput) {
                            bool found = false;
                            for (int i = 0; i < (int)processedData[k].set_data.size(); i++) {
                                if (netOutputs[j].outputField == processedData[k].set_data[i].header) {
                                    found = true;
                                    for (int m = 0; m < (int)processedData[k].set_data[i].column_data.size(); m++) {
                                        int index = j + ((m + offset) * num_outputs);
                                        if (!setOutputTrainData(index, processedData[k].set_data[i].column_data[m])) {
                                            addErrorString("copyProcessedDataToNetTrainArrays unable to copy output data output " + netOutputs[j].nodeId);
                                            return(false);
                                        }
                                        cnt++;
                                    }
                                    int p = (int)((100.0F * (float)cnt) / (float)total_cnt);
                                    showPopUpMessage("Copying train data to net . . .", p);
                                    i = (int)processedData[k].set_data.size();
                                }
                            }
                            if (!found && (k == 0)) {
                                addErrorString("copyProcessedDataToNetTrainArrays unable to find data for output " + netOutputs[j].nodeId);
                            }
                        }
                    }
                }
                offset += (int)processedData[k].set_data[0].column_data.size();
            }
        }
        closePopUp();
        //char s[200];  sprintf_s(s, "copyProcessedDataToNetTrainArrays : cnt = %i : total_cnt = %i", cnt, total_cnt);  addErrorString(s);
    }
    return(true);
}

bool NetCore::copyProcessedDataToNetValidationArrays()
{
    // Get data for all data files
    if (processedValidationData.size() > 0) {
        int cnt = 0;
        int total_cnt = 0;
        for (int i = 0; i < (int)processedValidationData.size(); i++) {
            if (processedValidationData[i].set_data.size() > 0) {
                for (int j = 0; j < (int)processedValidationData[i].set_data.size(); j++) {
                    total_cnt += (int)processedValidationData[i].set_data[j].column_data.size();
                }
            }
        }
        int offset = 0;
        for (int k = 0; k < (int)processedValidationData.size(); k++) {
            if (processedValidationData[k].set_data.size() > 0) {
                if (netInputs.size() > 0) {
                    for (int j = 0; j < (int)netInputs.size(); j++) {
                        bool found = false;
                        for (int i = 0; i < (int)processedValidationData[k].set_data.size(); i++) {
                            if (netInputs[j].inputFields.size() > 0) {
                                string field = netInputs[j].inputFields[0];
                                if ((netInputs[j].inputFields.size() > 1) && (k == 0)) {
                                    addErrorString("copyProcessedDataToNetValidationArrays unable to process multiple inputs for input " + netInputs[j].nodeId);
                                }
                                if (field == processedValidationData[k].set_data[i].header) {
                                    found = true;
                                    for (int m = 0; m < (int)processedValidationData[k].set_data[i].column_data.size(); m++) {
                                        int index = j + ((m + offset) * num_inputs);
                                        if (!setInputValidationData(index, processedValidationData[k].set_data[i].column_data[m])) {
                                            addErrorString("copyProcessedDataToNetValidationArrays unable to copy input data output " + netInputs[j].nodeId);
                                            return(false);
                                        }
                                        cnt++;
                                    }
                                    int p = (int)((100.0F * (float)cnt) / (float)total_cnt);
                                    showPopUpMessage("Copying validation data to net . . .", p);
                                    i = (int)processedValidationData[k].set_data.size();
                                }
                            }
                        }
                        if (!found && (k == 0)) {
                            addErrorString("copyProcessedDataToNetValidationArrays unable to find data for input " + netInputs[j].nodeId);
                            closePopUp();
                            return(false);
                        }
                    }
                }
                if (netOutputs.size() > 0) {
                    for (int j = 0; j < (int)netOutputs.size(); j++) {
                        if (!netOutputs[j].imageOutput) {
                            bool found = false;
                            for (int i = 0; i < (int)processedValidationData[k].set_data.size(); i++) {
                                if (netOutputs[j].outputField == processedValidationData[k].set_data[i].header) {
                                    found = true;
                                    for (int m = 0; m < (int)processedValidationData[k].set_data[i].column_data.size(); m++) {
                                        int index = j + ((m + offset) * num_outputs);
                                        if (!setOutputValidationData(index, processedValidationData[k].set_data[i].column_data[m])) {
                                            //addErrorString("output " + netOutputs[j].nodeId + " : field " + netOutputs[j].outputField);
                                            addErrorString("copyProcessedDataToNetValidationArrays unable to copy output data output " + netOutputs[j].nodeId);
                                            return(false);
                                        }
                                        cnt++;
                                    }
                                    int p = (int)((100.0F * (float)cnt) / (float)total_cnt);
                                    showPopUpMessage("Copying validation data to net . . .", p);
                                    i = (int)processedValidationData[k].set_data.size();
                                }
                            }
                            if (!found && (k == 0)) {
                                addErrorString("copyProcessedDataToNetValidationArrays unable to find data for output " + netOutputs[j].nodeId);
                                closePopUp();
                                return(false);
                            }
                        }
                    }
                }
                offset += (int)processedValidationData[k].set_data[0].column_data.size();
            }
        }
        closePopUp();
        //char s[200];  sprintf_s(s, "copyProcessedDataToNetValidationArrays : cnt = %i : total_cnt = %i", cnt, total_cnt);  addErrorString(s);
    }
    return(true);
}

void NetCore::copyImageOutputDataToNetTrainArrays()
{
    bool outputimagenodes = false;
    for (int j = 0; j < (int)netOutputs.size(); j++) {
        if (netOutputs[j].imageOutput) {
            outputimagenodes = true;
            j = (int)netOutputs.size();
        }
    }
    if (outputimagenodes) {
        for (int k = 0; k < (int)fileTrainIndicies.size(); k++) {
            if (image_train_data_allocated) {
                if (netOutputs.size() == num_outputs) {
                    // Set netoutputs equal to image data
                    for (int m = 0; m < image_height; m++) {
                        for (int n = 0; n < image_width; n++) {
                            if (image_gray) {
                                int mindex1 = n + (m * image_width) + (fileTrainIndicies[k] * image_max_size);
                                int mindex2 = n + (m * image_width) + (k * image_max_size);
                                setOutputTrainData(mindex2, *(image_train_data + mindex1));
                            }
                            else {
                                int mindex1 = (n * 3) + (m * 3 * image_width) + (fileTrainIndicies[k] * image_max_size);
                                int mindex2 = (n * 3) + (m * 3 * image_width) + (k * image_max_size);
                                setOutputTrainData(mindex2 + 0, *(image_train_data + mindex1 + 0));
                                setOutputTrainData(mindex2 + 1, *(image_train_data + mindex1 + 1));
                                setOutputTrainData(mindex2 + 2, *(image_train_data + mindex1 + 2));
                            }
                        }
                    }
                }
                else {
                    char s[200];  sprintf_s(s, "Adding train data error : unable to set outputIndices : netOutputs %i : nnet.num_outputs %i", (int)netOutputs.size(), num_outputs);
                    addErrorString(s);
                    return;
                }
            }
        }
    }
}

void NetCore::copyImageOutputDataToNetValidationArrays()
{
    if (dovalidation) {
        bool outputimagenodes = false;
        for (int j = 0; j < (int)netOutputs.size(); j++) {
            if (netOutputs[j].imageOutput) {
                outputimagenodes = true;
                j = (int)netOutputs.size();
            }
        }
        if (outputimagenodes) {
            for (int k = 0; k < (int)fileValidationIndicies.size(); k++) {
                if (image_train_data_allocated) {
                    // Set netoutputs equal to image data
                    for (int m = 0; m < image_height; m++) {
                        for (int n = 0; n < image_width; n++) {
                            if (image_gray) {
                                int mindex1 = n + (m * image_width) + (fileValidationIndicies[k] * image_max_size);
                                int mindex2 = n + (m * image_width) + (k * image_max_size);
                                setOutputValidationData(mindex2, *(image_train_data + mindex1));
                            }
                            else {
                                int mindex1 = (n * 3) + (m * 3 * image_width) + (fileValidationIndicies[k] * image_max_size);
                                int mindex2 = (n * 3) + (m * 3 * image_width) + (k * image_max_size);
                                setOutputValidationData(mindex2 + 0, *(image_train_data + mindex1 + 0));
                                setOutputValidationData(mindex2 + 1, *(image_train_data + mindex1 + 1));
                                setOutputValidationData(mindex2 + 2, *(image_train_data + mindex1 + 2));
                            }
                        }
                    }
                }
            }
        }
    }
}

bool NetCore::dataInitializationErrorCheck1()
{
    bool rval = true;
    if (num_inputs != (int)netInputs.size()) {
        char s[200];  sprintf_s(s, "Error : netInputs (%i) doesn't match num nnet inputs (%i)", (int)netInputs.size(), num_inputs);
        addErrorString(s);
        for (int i = 0; i < (int)netInputs.size(); i++) {
            addErrorString(netInputs[i].nodeId);
        }
        rval = false;
    }
    if (num_outputs != (int)netOutputs.size()) {
        char s[200];  sprintf_s(s, "Error : netOutputs (%i) doesn't match num nnet outputs (%i)", (int)netOutputs.size(), num_outputs);
        addErrorString(s);
        rval = false;
    }
    return(rval);
}

bool NetCore::dataInitializationErrorCheck2()
{
    if (processedData.size() > 0) {

        if (((int64_t)num_filedata_rows * (int64_t)num_inputs) != inputtraindatasize) {
            char s[200];  sprintf_s(s, "Error : num_filedata_rows * num_inputs (%lli) not equal to inputtraindatasize (%lli)", ((int64_t)num_filedata_rows * (int64_t)num_inputs), inputtraindatasize);
            addErrorString(s);
            return(false);
        }

        if (((int64_t)num_filedata_rows * (int64_t)num_outputs) != outputtraindatasize) {
            char s[200];  sprintf_s(s, "Error : num_filedata_rows * num_outputs (%lli) not equal to outputtraindatasize (%lli)", ((int64_t)num_filedata_rows *(int64_t) num_outputs), outputtraindatasize);
            addErrorString(s);
            return(false);
        }

        if (((int64_t)num_validationdata_rows * (int64_t)num_inputs) != inputvalidationdatasize) {
            char s[200];  sprintf_s(s, "Error : num_validationdata_rows * num_inputs (%lli) not equal to inputvalidationdatasize (%lli)", ((int64_t)num_validationdata_rows * (int64_t)num_inputs), inputvalidationdatasize);
            addErrorString(s);
            return(false);
        }

        if (((int64_t)num_validationdata_rows * (int64_t)num_outputs) != outputvalidationdatasize) {
            char s[200];  sprintf_s(s, "Error : num_validationdata_rows * num_outputs (%lli) not equal to outputvalidationdatasize (%lli)", ((int64_t)num_validationdata_rows * (int64_t)num_outputs), outputvalidationdatasize);
            addErrorString(s);
            return(false);
        }

        for (int i = 0; i < (int)processedData.size(); i++) {
            if (processedData[i].set_data.size() > 0) {
                int tsize = (int)processedData[i].set_data[0].column_data.size();
                if (tsize != processedData[i].set_data[0].column_string_data.size()) {
                    char s[200];  sprintf_s(s, "Error : processed column data size (%i) not equal to string data column (%i) for column ", tsize, (int)processedData[i].set_data[0].column_string_data.size());
                    addErrorString(s + processedData[i].set_data[0].header + " for file " + processedData[i].file_path);
                    return(false);
                }
                if (processedData[i].set_data.size() > 1) {
                    for (int j = 1; j < (int)processedData[i].set_data.size(); j++) {
                        if (tsize != processedData[i].set_data[j].column_string_data.size()) {
                            char s[200];  sprintf_s(s, "Error : processed column string data size (%i) not equal to test size (%i) for column ", (int)processedData[i].set_data[j].column_string_data.size(), tsize);
                            addErrorString(s + processedData[i].set_data[j].header + " for file " + processedData[i].file_path);
                            return(false);
                        }
                        if (tsize != processedData[i].set_data[j].column_data.size()) {
                            char s[200];  sprintf_s(s, "Error : processed column data size (%i) not equal to test size (%i) for columns ", (int)processedData[i].set_data[j].column_data.size(), tsize);
                            addErrorString(s + processedData[i].set_data[(int)(j-1)].header + " and " + processedData[i].set_data[j].header + " for file " + processedData[i].file_path);
                            return(false);
                        }
                    }
                }
            }
        }

    }
    else {
        addErrorString("Error : processed data empty");
        return(false);
    }
    return(true);
}

bool NetCore::dataInitializationErrorCheck2b()
{
    bool rval = true;
    if (fileTrainIndicies.size() != fileTrainSizes.size()) {
        char s[200];  sprintf_s(s, "Error : fileTrainIndicies.size not equal to fileTrainSizes.size : %i : %i", (int)fileTrainIndicies.size(), (int)fileTrainSizes.size());
        addErrorString(s);
        rval = false;
    }
    if (fileTrainIndicies.size() != processedData.size()) {
        char s[200];  sprintf_s(s, "Error : fileTrainIndicies.size not equal to processedData.size : %i : %i", (int)fileTrainIndicies.size(), (int)processedData.size());
        addErrorString(s);
        rval = false;
    }
    int scnt = 0;
    for (int i = 0; i < (int)fileTrainSizes.size(); i++) {
        scnt += fileTrainSizes[i];
    }
    if (scnt != num_filedata_rows) {
        char s[200];  sprintf_s(s, "Error : fileTrainSizes not equal to num_filedata_rows : %i : %i", scnt, num_filedata_rows);
        addErrorString(s);
        rval = false;
    }
    if (dovalidation) {
        if (fileValidationIndicies.size() != processedValidationData.size()) {
            char s[200];  sprintf_s(s, "Error : fileValidationIndicies.size not equal to processedValidationData.size : %i : %i", (int)fileValidationIndicies.size(), (int)processedValidationData.size());
            addErrorString(s);
            rval = false;
        }
        if (fileValidationIndicies.size() != fileValidationSizes.size()) {
            char s[200];  sprintf_s(s, "Error : fileValidationIndicies.size not equal to fileValidationSizes.size : %i : %i", (int)fileValidationIndicies.size(), (int)fileValidationSizes.size());
            addErrorString(s);
            rval = false;
        }
        if (!use_percent_validation) {
            if (fileValidationIndicies.size() != fileValidationSizes.size()) {
                char s[200];  sprintf_s(s, "Error : fileValidationIndicies.size not equal to validationPathnames.size : %i : %i", (int)fileValidationIndicies.size(), (int)fileValidationSizes.size());
                addErrorString(s);
                rval = false;
            }
        }
        scnt = 0;
        for (int i = 0; i < (int)fileValidationSizes.size(); i++) {
            scnt += fileValidationSizes[i];
        }
        if (scnt != num_validationdata_rows) {
            char s[200];  sprintf_s(s, "Error : fileValidationSizes not equal to num_validationdata_rows : %i : %i", scnt, num_validationdata_rows);
            addErrorString(s);
            rval = false;
        }
    }
    if (netInputs.size() != num_inputs) {
        char s[200];  sprintf_s(s, "Error : netInputs.size not equal to nnet.num_inputs : %i : %i", (int)netInputs.size(), num_inputs);
        addErrorString(s);
        rval = false;
    }
    if (netOutputs.size() != num_outputs) {
        char s[200];  sprintf_s(s, "Error : netOutputs.size not equal to nnet.num_outputs : %i : %i", (int)netOutputs.size(), num_outputs);
        addErrorString(s);
        rval = false;
    }
    if (has_wave_conv) {
        if (((int)processedData.size() + (int)processedValidationData.size()) > (int)wresults.wave_data_lengths.size()) {
            char s2[200];  sprintf_s(s2, "Error : processedData size (%i) + processedValidationData size (%i) greater than wresults wave_data_lengths.size (%i)", (int)processedData.size(), (int)processedValidationData.size(), (int)wresults.wave_data_lengths.size()); addErrorString(s2);
            return(false);
        }
    }
    return(rval);
}

bool NetCore::dataInitializationErrorCheck3()
{
    bool rval = true;
    if (use_percent_validation) {
        if ((imageTrainIndices.size() + imageValidationIndices.size()) != combinedPathnames.size()) {
            char s[200];  sprintf_s(s, "Error : Problem with image indices : imageTrainIndices.size = %i : imageValidationIndices.size = %i : combinedPathnames.size = %i", (int)imageTrainIndices.size(), (int)imageValidationIndices.size(), (int)combinedPathnames.size());
            addErrorString(s);
            rval = false;
        }
    }
    else if (dovalidation && !use_percent_validation) {
        if ((imageTrainIndices.size() == 0) || (imageValidationIndices.size() == 0)) {
            char s[200];  sprintf_s(s, "Error : Problem with image indices : imageTrainIndices.size = %i : imageValidationIndices.size = %i : combinedPathnames.size = %i", (int)imageTrainIndices.size(), (int)imageValidationIndices.size(), (int)combinedPathnames.size());
            addErrorString(s);
            rval = false;
        }
        for (int i = 0; i < (int)validationPathnames.size(); i++) {
            if (validationPathnames[i] == "") {
                char s[200];  sprintf_s(s, "Error : Problem with validationPathnames : index %i is blank", i);
                addErrorString(s);
                i = (int)validationPathnames.size();
                rval = false;
            }
        }
    }
    if (rval) {
        if (imageTrainIndices.size() != num_filedata_rows) {
            char s[200];  sprintf_s(s, "Error : Problem with image indices : imageTrainIndices.size = %i : num_filedata_rows = %i", (int)imageTrainIndices.size(), (int)num_filedata_rows);
            addErrorString(s);
            rval = false;
        }
    }
    if (rval) {
        if (imageValidationIndices.size() != num_validationdata_rows) {
            char s[200];  sprintf_s(s, "Error : Problem with image indices : imageValidationIndices.size = %i : num_validationdata_rows = %i", (int)imageValidationIndices.size(), (int)num_validationdata_rows);
            addErrorString(s);
            rval = false;
        }
    }
    return(rval);
}

void NetCore::setupCopyOutputIndices()
{
    if (num_copy_outputs_to_inputs > 0) {

        CLineParser func;

        // allocate memory for indices
        copy_to_input_indices = new int[num_copy_outputs_to_inputs];
        copy_from_output_indices = new int[num_copy_outputs_to_inputs];
        copy_outputs_offsets = new int[num_copy_outputs_to_inputs];
        if ((copy_to_input_indices == 0) || (copy_from_output_indices == 0) || (copy_outputs_offsets == 0)) {
            addErrorString("setupCopyOutputIndices : unable to allocate memory for copy_from_output_indices");
            num_copy_outputs_to_inputs = 0;
            return;
        }
        for (int i = 0; i < num_copy_outputs_to_inputs; i++) {
            copy_to_input_indices[i] = 0;
            copy_from_output_indices[i] = 0;
            copy_outputs_offsets[i] = -1;
        }

        // set up indices (using loadTrainSeriesDataToNet for reference)
        int tcnt = 0;
        vector<string> outnames;
        for (int i = 0; i < num_inputs; i++) {
            if ((int)netInputs[i].inputFields.size() == 1) {
                if (netInputs[i].inputFields[0].substr(0, 7) == "Output:") {
                    //addErrorString("setting output input for " + netInputs[i].inputFields[0]);
                    outnames.push_back(func.getStringAfterColon(netInputs[i].inputFields[0]));
                    copy_outputs_offsets[tcnt] = (int)netInputs[i].indexOffset;;
                    copy_to_input_indices[tcnt] = (int)i;
                    tcnt++;
                }
            }
            else if ((int)netInputs[i].inputFields.size() > 1) {
                for (int j = 0; j < (int)netInputs[i].inputFields.size(); j++) {
                    if (netInputs[i].inputFields[j].substr(0, 7) == "Output:") {
                        addErrorString("setupCopyOutputIndices : cannot process inputs from outputs for multiple inputs : " + netInputs[i].nodeId);
                    }
                }
            }
        }
        if (tcnt != num_copy_outputs_to_inputs) {
            char s[200];  sprintf_s(s, "setupCopyOutputIndices : unable to match net inputs with output ids : tcnt = %i : num_copy_outputs_to_inputs = %i", tcnt, num_copy_outputs_to_inputs);
            addErrorString(s);
            num_copy_outputs_to_inputs = 0;
            return;
        }

        // set up indices (using loadTrainSeriesDataToNet for reference)
        tcnt = 0;
        for (int j = 0; j < outnames.size(); j++) {
            bool found = false;
            for (int i = 0; i < num_outputs; i++) {
                //addErrorString("testing " + onodes[i].id + " against " + outnames[j]);
                if (onodes[i].id == outnames[j]) {
                    if (tcnt < num_copy_outputs_to_inputs) {
                        copy_from_output_indices[tcnt] = i;
                        i = num_outputs;
                        found = true;
                        tcnt++;
                    }
                }
            }
            if (!found) {
                addErrorString("setupCopyOutputIndices : unable to find output " + outnames[j]);
            }
        }
        if (tcnt != num_copy_outputs_to_inputs) {
            char s[200];  sprintf_s(s, "setupCopyOutputIndices : unable to match net outputs with input ids : tcnt = %i : num_copy_outputs_to_inputs = %i", tcnt, num_copy_outputs_to_inputs);
            addErrorString(s);
            num_copy_outputs_to_inputs = 0;
            return;
        }

    }
}

// train memory management end ------------------------


// train functions start ------------------------------

// **********************************************************************************
//
//  For output convolutions, need to copy convolution outputs to output_image_buffer
//    conv_output_pointer still needs to be connected properly for this to be able
//    to happen
//
// **********************************************************************************
bool NetCore::addLoopControlData(string dependency, int hnode_index, bool hidden_net, bool attention, bool selfattention, bool netcluster, bool dropout, bool addition, bool multiplication)
{
    loopControlData ldata;
    ldata.mode = "";
    ldata.isconv = false;
    ldata.ishiddennet = false;
    ldata.isattention = false;
    ldata.isselfattention = false;
    ldata.isnetcluster = false;
    ldata.hasinput = false;
    ldata.isconvoutput = false;
    ldata.isdropout = false;
    ldata.isaddition = false;
    ldata.ismultiplication = false;
    ldata.hidden_conv_index = -1;
    ldata.start_index = 0;
    ldata.end_index = 0;
    if (dependency == "None") {
        ldata.id = "None";
        ldata.start_index = 0;
        if (loopControl.size() > 0) {
            for (int i = ((int)loopControl.size() - 1); i >= 0; i--) {
                if (loopControl[i].id == "None") {
                    ldata.start_index = loopControl[i].end_index;
                    i = -1;
                }
            }
        }
        ldata.end_index = hnode_index;
        if ((ldata.end_index - ldata.start_index) == 0) {
            return(false);
        }
        loopControl.push_back(ldata);
    }
    else if (hidden_net) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = "Hidden";
        ldata.ishiddennet = true;
        loopControl.push_back(ldata);
    }
    else if (attention) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = "Hidden";
        ldata.isattention = true;
        loopControl.push_back(ldata);
    }
    else if (selfattention) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = "Hidden";
        ldata.isselfattention = true;
        loopControl.push_back(ldata);
    }
    else if (netcluster) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = nclusters[ldata.hidden_conv_index].mode;
        ldata.isnetcluster = true;
        loopControl.push_back(ldata);
    }
    else if (dropout) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = "Hidden";
        ldata.isdropout = true;
        loopControl.push_back(ldata);
    }
    else if (addition) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = "Hidden";
        ldata.isaddition = true;
        loopControl.push_back(ldata);
    }
    else if (multiplication) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = "Hidden";
        ldata.ismultiplication = true;
        loopControl.push_back(ldata);
    }
    else if (getConvFromConnectName(dependency)) {
        loopControlData ldata;
        ldata.hidden_conv_index = hnode_index;
        ldata.id = dependency;
        ldata.mode = newConvs[ldata.hidden_conv_index].mode;
        ldata.isconv = true;
        if (ldata.mode == "Output") {
            ldata.isconvoutput = true;
            conv_output_test_index = ldata.hidden_conv_index;
        }
        loopControl.push_back(ldata);
    }
    else {
        return(false);
    }
    return(true);

}

bool NetCore::initializeNetClusters()
{
    for (int i = 0; i < num_net_clusters; i++) {
        if (!nclusters[i].initializeNetCluster(num_filedata_rows)) {
            return(false);
        }
    }
    return(true);
}

void NetCore::setRandomize(bool rand)
{
    randomize_data = rand;
}

void NetCore::setParallelProcessingTest(int test)
{
    if (test > 0) {
        parallel_processing_test = test;
        for (int i = 0; i < num_hidden_nets; i++) {
            hnets[i].setParallelProcessingTest(test);
        }
    }
}

void NetCore::setUseValidationFiles(bool use)
{ 
    use_validation_files = use;
    if (use_validation_files) {
        dovalidation = true;
    }
}

void NetCore::setUsePercentValidation(bool use, float percent)
{ 
    validation_percent = percent;
    use_percent_validation = use; 
    if (use_percent_validation) {
        dovalidation = true;
    }
}

void NetCore::forward(int index, bool training)
{
    forward(inputtraindata + index, training);
}

void NetCore::backward(int index1, int index2, int index3)
{
    backward((inputtraindata + index1), (outputtraindata + index2), index3);
}

void NetCore::checkError(int index1, int index2, int input_index)
{
    checkError2((inputtraindata + index1), (outputtraindata + index2), input_index);
}

void NetCore::checkValidationError(int index1, int index2)
{
    checkValidationError1((inputvalidationdata + index1), (outputvalidationdata + index2));
}

_int64 NetCore::train()
{
    int index1 = 0;
    int index2 = 0;
    for (long i = 0; i < train_inc; i++) {
        index1 = data_index.dindex * num_inputs;
        index2 = data_index.dindex * num_outputs;
        loadExternalDataToNet(data_index);
        forward(index1, true);
        copyOutputsToInputs(data_index.dindex);
        backward(index1, index2, data_index.dindex);
        data_index = updateTrainIndex(data_index);
        train_count++;
        if (train_count > err_count) {
            if (!updateError()) {
                return(-1);
            }
            err_count = train_count + (_int64)(0.11 * (double)updateRate * (double)num_filedata_rows);
        }
    }
    return(train_count);
}

bool NetCore::updateError()
{
    int index1 = 0;
    int index2 = 0;
    if (dovalidation) {
        int vcount = error_inc;
        if (vcount > num_validationdata_rows) {
            vcount = num_validationdata_rows;
        }
        for (int i = 0; i < vcount; i++) {
            index1 = validation_index.dindex * num_inputs;
            index2 = validation_index.dindex * num_outputs;
            loadExternalValidationDataToNet(validation_index);
            checkValidationError(index1, index2);
            copyValidationOutputsToInputs(validation_index.dindex);
            validation_index = updateValidationIndex(validation_index);
        }
        validation_error = accumulative_validation_error;
    }
    for (int i = 0; i < error_inc; i++) {
        index1 = error_index.dindex * num_inputs;
        index2 = error_index.dindex * num_outputs;
        loadExternalDataToNet(error_index);
        checkError(index1, index2, error_index.dindex);
        copyOutputsToInputs(error_index.dindex);
        error_index = updateTrainIndex(error_index);
    }
    train_error = accumulative_error;
    train_error_delta = accumulative_error_slope;
    if (isnan(train_error) || isinf(train_error)) {
        addErrorString("Training stopped due to numerical over/underflow");
        return(false);
    }
    return(true);
}

void NetCore::forward(double* tinputs, bool training)
{
    for (int i = 0; i < num_inputs; i++) {
        inputs[i] = tinputs[i];
    }
    for (int j = 0; j < (int)loopControl.size(); j++) {
        if (loopControl[j].isconv) {
            newConvs[loopControl[j].hidden_conv_index].forward();
        }
        else if (loopControl[j].ishiddennet) {
            hnets[loopControl[j].hidden_conv_index].forward();
        }
        else if (loopControl[j].isattention) {
            attentions[loopControl[j].hidden_conv_index].forward();
        }
        else if (loopControl[j].isselfattention) {
            selfattentions[loopControl[j].hidden_conv_index].forward();
        }
        else if (loopControl[j].isnetcluster) {
            nclusters[loopControl[j].hidden_conv_index].forward();
        }
        else if (loopControl[j].isdropout) {
            dropouts[loopControl[j].hidden_conv_index].forward(training);
        }
        else if (loopControl[j].isaddition) {
            additions[loopControl[j].hidden_conv_index].forward();
        }
        else if (loopControl[j].ismultiplication) {
            multiplications[loopControl[j].hidden_conv_index].forward();
        }
        else {
            int start = loopControl[j].start_index;
            int end = loopControl[j].end_index;
            for (int i = start; i < end; i++) {
                hnodes[i].forward();
            }
        }
    }
    if (num_outputs > parallel_processing_test) {
#ifdef GCC
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].forward();
        }
#else
        parallel_for(long(0), (long)num_outputs, [&](long i) {
            onodes[i].forward();
            });
#endif
    }
    else {
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].forward();
        }
    }
    softmaxNormalizeOutputLayers();
}

void NetCore::backward(double* tinputs, double* toutputs, int input_index)
{
    for (int i = 0; i < num_inputs; i++) {
        inputs[i] = tinputs[i];
    }
    for (int i = 0; i < num_outputs; i++) {
        testoutputs[i] = toutputs[i];
    }

    softmaxOutputLayerBackpropProcessing();
    
    if (num_outputs > parallel_processing_test) {
#ifdef GCC
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].backward();
        }
#else
        parallel_for(long(0), (long)num_outputs, [&](long i) {
            onodes[i].backward();
            });
#endif
    }
    else {
        for (int i = 0; i < num_outputs; i++) {
            onodes[i].backward();
        }
    }

    for (int j = ((int)loopControl.size() - 1); j >= 0; j--) {
        if (loopControl[j].isconv) {
            newConvs[loopControl[j].hidden_conv_index].backward();
        }
        else if (loopControl[j].ishiddennet) {
            hnets[loopControl[j].hidden_conv_index].backward();
        }
        else if (loopControl[j].isattention) {
            attentions[loopControl[j].hidden_conv_index].backward();
        }
        else if (loopControl[j].isselfattention) {
            selfattentions[loopControl[j].hidden_conv_index].backward();
        }
        else if (loopControl[j].isnetcluster) {
            nclusters[loopControl[j].hidden_conv_index].backward((int64_t)input_index);
        }
        else if (loopControl[j].isdropout) {
            dropouts[loopControl[j].hidden_conv_index].backward();
        }
        else if (loopControl[j].isaddition) {
            additions[loopControl[j].hidden_conv_index].backward();
        }
        else if (loopControl[j].ismultiplication) {
            multiplications[loopControl[j].hidden_conv_index].backward();
        }
        else {
            int start = loopControl[j].start_index;
            int end = loopControl[j].end_index;
            for (int i = (end - 1); i >= start; i--) {
                hnodes[i].backward();
            }
        }
    }

}

void NetCore::resetUpdateInc()
{
    train_inc = 10;
    error_inc = 2;
    update_inc = 100;
}

void NetCore::setUpdateRate(float rate)
{
    updateRate = rate;
}

train_index NetCore::updateTrainIndex(train_index tindex)
{
    train_index rval = tindex;
    if (randomize_data) {
        rval.findex = 0;
        if (!index_conv) {
            if (fileTrainIndicies.size() > 1) {
                rval.findex = (((_int64)(int)fileTrainIndicies.size()) * (_int64)rand()) / (_int64)RAND_MAX;
                if (rval.findex >= (int)fileTrainIndicies.size()) {
                    rval.findex = 0;
                }
                rval.sindex = (int)((((_int64)fileTrainSizes[rval.findex]) * (_int64)rand()) / (_int64)RAND_MAX);
                if (rval.sindex >= fileTrainSizes[rval.findex]) {
                    rval.sindex = 0;
                }
                rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
            }
            else {
                rval.dindex = (int)((((_int64)num_filedata_rows - 1) * (_int64)rand()) / (_int64)RAND_MAX);
            }
        }
        else if (sound_conv_only) {
            if (fileTrainIndicies.size() > 1) {
                rval.findex = (((_int64)(int)fileTrainIndicies.size()) * (_int64)rand()) / (_int64)RAND_MAX;
                if (rval.findex >= (int)fileTrainIndicies.size()) {
                    rval.findex = 0;
                }
                rval.sindex = (int)((((_int64)fileTrainSizes[rval.findex]) * (_int64)rand()) / (_int64)RAND_MAX);
                if (rval.sindex >= fileTrainSizes[rval.findex]) {
                    rval.sindex = 0;
                }
                rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
            }
        }
        else if (processed_data_conv) {
            if (fileTrainIndicies.size() > 0) {
                rval.findex = (((_int64)(int)fileTrainIndicies.size()) * (_int64)rand()) / (_int64)RAND_MAX;
                if (rval.findex >= (int)fileTrainIndicies.size()) {
                    rval.findex = 0;
                }
                rval.sindex = (int)((((_int64)fileTrainSizes[rval.findex]) * (_int64)rand()) / (_int64)RAND_MAX);
                if (rval.sindex >= fileTrainSizes[rval.findex]) {
                    rval.sindex = 0;
                }
                rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
            }
        }
    }
    else {
        if (!index_conv) {
            if (fileTrainIndicies.size() > 1) {
                rval.sindex++;
                if (rval.sindex >= fileTrainSizes[rval.findex]) {
                    rval.sindex = 0;
                    rval.findex++;
                    if (rval.findex >= (int)fileTrainIndicies.size()) {
                        rval.findex = 0;
                    }
                }
                rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
            }
            else {
                rval.dindex++;
            }
        }
        else if (sound_conv_only) {
            rval.sindex++;
            if (rval.sindex >= fileTrainSizes[rval.findex]) {
                rval.sindex = 0;
                rval.findex++;
                if (rval.findex >= (int)fileTrainSizes.size()) {
                    rval.findex = 0;
                }
            }
            rval.dindex = fileTrainIndicies[rval.findex] +rval.sindex;
        }
        else if (processed_data_conv) {
            rval.sindex++;
            if (rval.sindex >= fileTrainSizes[rval.findex]) {
                rval.sindex = 0;
                rval.findex++;
                if (rval.findex >= (int)fileTrainSizes.size()) {
                    rval.findex = 0;
                }
            }
            rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
        }
    }
    if (rval.dindex >= num_filedata_rows) {
        rval.dindex = 0;
    }
    //char s[200];  sprintf_s(s, "findex = %i : dindex = %i : sindex = %i", rval.findex, rval.dindex, rval.sindex); addErrorString(s);
    return(rval);
}

train_index NetCore::updateTrainIndexNoRandom(train_index tindex)
{
    train_index rval = tindex;
    if (!index_conv) {
        if (fileTrainIndicies.size() > 1) {
            rval.sindex++;
            if (rval.sindex >= fileTrainSizes[rval.findex]) {
                rval.sindex = 0;
                rval.findex++;
                if (rval.findex >= (int)fileTrainIndicies.size()) {
                    rval.findex = 0;
                }
            }
            rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
        }
        else {
            rval.dindex++;
        }
    }
    else if (sound_conv_only) {
        rval.sindex++;
        if (rval.sindex >= fileTrainSizes[rval.findex]) {
            rval.sindex = 0;
            rval.findex++;
            if (rval.findex >= (int)fileTrainSizes.size()) {
                rval.findex = 0;
            }
        }
        rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
    }
    else if (processed_data_conv) {
        rval.sindex++;
        if (rval.sindex >= fileTrainSizes[rval.findex]) {
            rval.sindex = 0;
            rval.findex++;
            if (rval.findex >= (int)fileTrainSizes.size()) {
                rval.findex = 0;
            }
        }
        rval.dindex = fileTrainIndicies[rval.findex] + rval.sindex;
    }
    if (rval.dindex >= num_filedata_rows) {
        rval.dindex = 0;
    }
    //char s[200];  sprintf_s(s, "findex = %i : dindex = %i : sindex = %i", rval.findex, rval.dindex, rval.sindex); addErrorString(s);
    return(rval);
}

train_index NetCore::updateValidationIndex(train_index tindex)
{
    train_index rval = tindex;
    if (!index_conv) {
        if (fileValidationIndicies.size() > 1) {
            rval.sindex++;
            if (rval.sindex >= fileValidationSizes[rval.findex]) {
                rval.sindex = 0;
                rval.findex++;
                if (rval.findex >= (int)fileValidationIndicies.size()) {
                    rval.findex = 0;
                }
            }
            rval.dindex = fileValidationIndicies[rval.findex] + rval.sindex;
        }
        else {
            rval.dindex++;
        }
    }
    else if (sound_conv_only) {
        rval.sindex++;
        if (rval.sindex >= fileValidationSizes[rval.findex]) {
            rval.sindex = 0;
            rval.findex++;
            if (rval.findex >= (int)fileValidationSizes.size()) {
                rval.findex = 0;
            }
        }
        rval.dindex = fileValidationIndicies[rval.findex] + rval.sindex;
    }
    else if (processed_data_conv) {
        rval.sindex++;
        if (rval.sindex >= fileValidationSizes[rval.findex]) {
            rval.sindex = 0;
            rval.findex++;
            if (rval.findex >= (int)fileValidationSizes.size()) {
                rval.findex = 0;
            }
        }
        rval.dindex = fileValidationIndicies[rval.findex] + rval.sindex;
    }
    if (rval.dindex >= num_validationdata_rows) {
        rval.dindex = 0;
    }
    //char s[200];  sprintf_s(s, "findex = %i : dindex = %i : sindex = %i", rval.findex, rval.dindex, rval.sindex); addErrorString(s);
    return(rval);
}

void NetCore::copyOutputsToInputs(int index)
{
    if (num_copy_outputs_to_inputs > 0) {
        for (int i = 0; i < num_copy_outputs_to_inputs; i++) {
            int rindex = ((index + copy_outputs_offsets[i]) * num_inputs) + copy_to_input_indices[i];
            double tdata = getOutput(copy_from_output_indices[i]);
            setInputTrainData(rindex, tdata);
        }
    }
}

void NetCore::copyValidationOutputsToInputs(int index)
{
    if (num_copy_outputs_to_inputs > 0) {
        for (int i = 0; i < num_copy_outputs_to_inputs; i++) {
            int rindex = ((index + copy_outputs_offsets[i]) * num_inputs) + copy_to_input_indices[i];
            double tdata = getOutput(copy_from_output_indices[i]);
            setInputValidationData(rindex, tdata);
        }
    }
}

void NetCore::softmaxNormalizeOutputLayers()
{
    if (has_softmax) {
        softmax_sum = 0.0;
        for (int i = 0; i < num_outputs; i++) {
            if (onodes[i].type == SOFTMAX) {
                softmax_sum += onodes[i].getOutput();
            }
        }
        if (softmax_sum < 0.0) {
            softmax_sum = -1 * softmax_sum;
        }
        if (softmax_sum > 0.0) {
            for (int i = 0; i < num_outputs; i++) {
                if (onodes[i].type == SOFTMAX) {
                    onodes[i].setOutput(onodes[i].getOutput() / softmax_sum);
                }
            }
        }
    }
}

void NetCore::softmaxOutputLayerBackpropProcessing()
{
    if (has_softmax) {
        double gi = 0.0;
        double gj = 0.0;
        double dij = 0.0;
        double error = 0.0;
        double delta = 0.0;
        double derivative = 0.0;
        for (int i = 0; i < num_outputs; i++) {
            if (onodes[i].type == SOFTMAX) {
                derivative = 0.0;
                gi = onodes[i].getOutput();
                for (int j = 0; j < num_outputs; j++) {
                    gj = onodes[j].getOutput();
                    if (i == j) {
                        dij = 1.0;
                    }
                    else {
                        dij = 0.0;
                    }
                    derivative = (gi * (dij - gj));
                    delta = *(double*)(testoutputs + j) - gj;
                    error += (delta * derivative);
                }
                onodes[i].setSoftmaxError(error);
            }
        }
    }
}

double NetCore::checkError1(double* tinputs, double* toutputs, int input_index)
{
    double error = 0;

    forward(tinputs, false);

    int mnum_outputs = 0;

    if (image_data_allocated && (conv_output_test_index >= 0)) {
        static bool s2 = false;
        int ncindex = conv_output_test_index;
        if (!conv_variables_initialized) {
            addErrorString("checkError1 : conv_variables_initialized not set");
            return(error);
        }
        if ((conv_numx * conv_numy) > conv_max_size) {
            if (!s2) {
                char s[200];  sprintf_s(s, "checkError1 : conv_numx (%i) times conv_numy (%i) greater than conv_max_size (%i)", conv_numx, conv_numy, conv_max_size);  addErrorString(s);
                s2 = true;
            }
            return(error);
        }
        if ((conv_numx * conv_numy) > newConvs[ncindex].output_data_size) {
            if (!s2) {
                char s[200];  sprintf_s(s, "checkError1 : conv_numx (%i) times conv_numy (%i) greater than output_data_size (%i)", conv_numx, conv_numy, newConvs[ncindex].output_data_size);  addErrorString(s);
                s2 = true;
            }
            return(error);
        }
        if (conv_gray) {
            if (((conv_numx * conv_numy) + (2 * conv_single)) > conv_max_size) {
                if (!s2) {
                    char s[200];  sprintf_s(s, "checkError1 : conv_numx (%i) times conv_numy (%i) plus 2*conv_single (%i) greater than conv_max_size (%i)", conv_numx, conv_numy, (2 * conv_single), conv_max_size);  addErrorString(s);
                    s2 = true;
                }
                return(error);
            }
            if (((conv_numx * conv_numy) + (2 * conv_single)) > newConvs[ncindex].output_data_size) {
                if (!s2) {
                    char s[200];  sprintf_s(s, "checkError1 : conv_numx (%i) times conv_numy (%i) plus 2*conv_single (%i) greater than output_data_size (%i)", conv_numx, conv_numy, (2 * conv_single), newConvs[ncindex].output_data_size);  addErrorString(s);
                    s2 = true;
                }
                return(error);
            }
        }

        int index = 0;
        int jindex = 0;
        double img1 = 0;
        double img2 = 0;
        double img3 = 0;
        double out1 = 0;
        double out2 = 0;
        double out3 = 0;
        for (int j = 0; j < conv_numy; j++) {
            for (int i = 0; i < conv_numx; i++) {
                index = jindex + i;
                img1 = image_data[index];
                out1 = newConvs[ncindex].output_data[index];
                error += ((img1 - out1) * (img1 - out1));
                mnum_outputs++;
                if (!conv_gray) {
                    img2 = image_data[index + conv_single];
                    out2 = newConvs[ncindex].output_data[index + conv_single];
                    img3 = image_data[index + (2 * conv_single)];
                    out3 = newConvs[ncindex].output_data[index + (2 * conv_single)];
                    error += ((img2 - out2) * (img2 - out2));
                    mnum_outputs++;
                    error += ((img3 - out3) * (img3 - out3));
                    mnum_outputs++;
                }
            }
            jindex += conv_numx;
        }
    }

    for (int i = 0; i < num_outputs; i++) {
        error += ((toutputs[i] - getOutput(i)) * (toutputs[i] - getOutput(i)));
        mnum_outputs++;
    }

    if (num_net_clusters > 0) {
        for (int i = 0; i < num_net_clusters; i++) {
            if (nclusters[i].mode == "Output") {
                error += nclusters[i].getCumulativeError(input_index);
                mnum_outputs += nclusters[i].num_active_nodes;
            }
        }
    }

    if (mnum_outputs > 0) {

        error = error / (double)mnum_outputs;
        error = sqrt(error);

        if (errorFiltercount > 0) {
            if (accumulative_error <= 0) {
                accumulative_error = error;
            }
            else {
                accumulative_error = ((errorFiltercount - 1) * accumulative_error + error) / errorFiltercount;
            }
        }

        if (errorFiltercount < errorfilter) {
            errorFiltercount = errorFiltercount + 1;
        }
        else {
            errorFiltercount = errorfilter;
        }

    }

    return(error);
}

double NetCore::checkValidationError1(double* tinputs, double* toutputs)
{
    double error = 0;

    forward(tinputs, false);

    for (int i = 0; i < num_outputs; i++) {
        error += ((toutputs[i] - getOutput(i)) * (toutputs[i] - getOutput(i)));
    }
    error = error / (double)num_outputs;
    error = sqrt(error);

    if (validationfiltercount > 0) {
        if (accumulative_validation_error <= 0) {
            accumulative_validation_error = error;
        }
        else {
            accumulative_validation_error = ((validationfiltercount - 1) * accumulative_validation_error + error) / validationfiltercount;
        }
    }

    int verrorfilter = (int)(errorfilter / 2);// 10);
    if (validationfiltercount < verrorfilter) {
        validationfiltercount = validationfiltercount + 1;
    }
    else {
        validationfiltercount = verrorfilter;
    }

    return(error);
}

double NetCore::checkError2(double* tinputs, double* toutputs, int input_index)
{
    checkError1(tinputs, toutputs, input_index);

    for (int i = 9; i > 0; i--) {
        avgneterrors[i] = avgneterrors[i - 1];
    }
    avgneterrors[0] = accumulative_error;

    double derivative = avgneterrors[0] / 60.0 -
        3.0 * avgneterrors[1] / 20.0 +
        3.0 * avgneterrors[2] / 4.0 -
        3.0 * avgneterrors[4] / 4.0 +
        3.0 * avgneterrors[5] / 20.0 -
        avgneterrors[6] / 60.0;

    accumulative_error_slope = (1.0 - filter) * accumulative_error_slope + filter * derivative;

    return(accumulative_error);
}

double NetCore::getOutput(int index)
{
    double rval = 0;
    if (index < num_outputs) {
        rval = onodes[index].getOutput();
    }
    return(rval);
}


// train functionst end -------------------------------






void NetCore::archiveWeights()
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].archiveWeights();
    }
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].archiveWeights();
    }
    for (int i = 0; i < num_active_convs; i++) {
        newConvs[i].archiveWeights();
    }
    for (int i = 0; i < num_hidden_nets; i++) {
        hnets[i].archiveWeights();
    }
    for (int i = 0; i < num_net_clusters; i++) {
        nclusters[i].archiveWeights();
    }
}

void NetCore::restoreWeights()
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].restoreWeights();
    }
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].restoreWeights();
    }
    for (int i = 0; i < num_active_convs; i++) {
        newConvs[i].restoreWeights();
    }
    for (int i = 0; i < num_hidden_nets; i++) {
        hnets[i].restoreWeights();
    }
    for (int i = 0; i < num_net_clusters; i++) {
        nclusters[i].restoreWeights();
    }
}






bool NetCore::getConvFromConnectName(string cname)
{
    if ((cname.substr(0, 9) == "ConvLayer") || (cname.substr(0, 5) == "Layer") || (cname.substr(0, 9) == "ImageData") ||
        (cname.substr(0, 9) == "Attention") || (cname.substr(0, 10) == "PatternNet") || (cname.substr(0, 7) == "Dropout")) {
        return(true);
    }
    return(false);
}




void NetCore::addErrorString(string err) {
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addErrorString((CString)err.c_str());
#endif
}

void NetCore::addTrainString(string err)
{
#ifdef GCC
    cout << err << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addTrainString((CString)err.c_str());
#endif
}

void NetCore::addPanel1String(string msg)
{
#ifdef GCC
    cout << msg << endl;
#else
    CWnd* pWndMain = AfxGetMainWnd();
    CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
    pDoc->addPanel1String((CString)msg.c_str());
#endif
}

void NetCore::showPopUpMessage(string msg, int progress)
{
#ifdef GCC
    char s[200];  sprintf_s(s, " : %i%%", progress);
    cout << msg << s << endl;
#else
    if (!progress_dialog_up) {
        progressDlg.CreateDlg();
        progress_dialog_up = true;
    }
    progressDlg.setLabel(msg);
    progressDlg.setProgress(progress);
#endif
}

void NetCore::closePopUp()
{
#ifndef GCC
    progressDlg.DestroyDlg();
    progress_dialog_up = false;
#endif
}




//******************************************************************************************************

NetBuilder::NetBuilder()
{
    allocated = false;
    test_inputs_allocated = false;
    num_inputs = 0;
    num_hidden_nodes = 0;
    num_active_convs = 0;
    num_hidden_nets = 0;
    num_attention_layers = 0;
    num_self_attention_layers = 0;
    num_net_clusters = 0;
    num_dropouts = 0;
    num_additions = 0;
    num_multiplications = 0;
    nactive_index = 0;
    nhidden_net_index = 0;
    nattention_layer_index = 0;
    nself_attention_layer_index = 0;
    nnet_cluster_index = 0;
    ndropout_index = 0;
    naddition_index = 0;
    nmultiplication_index = 0;
    num_outputs = 0;
    errorfilter = 100;
    errorFiltercount = 100;
    validationfiltercount = 100;
    wrms = 0.0;
    filter = 0.001;
    accumulative_error = 0.0;
    accumulative_error_slope = 0;
    accumulative_validation_error = 0;
    zero = 0.0;

    conv_output_test_index = -1;

    for (int i = 0; i < 10; i++) {
        avgneterrors[i] = 0;
    }

    inputs = NULL;
    testoutputs = NULL;

    has_series_conv = false;
    series_data_allocated = false;
    series_input_width = 0;
    series_input_height = 0;
    series_mem_size = 0;
    series_backprop_size = 0;
    series_data = NULL;
    series_backprop_data = NULL;

    csv_with_filename = false;
    csv_filename_header = "";
    csv_image_filenames.clear();

    has_image_conv = false;
    image_data_allocated = false;
    use_images_as_outputs = false;
    image_mem_size = 0;
    image_backprop_size = 0;
    image_data = NULL;
    image_backprop_data = NULL;
    input_image_conv_name = "";

    has_wave_conv = false;
    has_wave_fft_conv = false;
    wave_data_allocated = false;
    wave_mem_size = 0;
    wave_data = NULL;

    has_text_conv = false;
    //text_data_allocated = false;
    //text_mem_size = 0;
    //text_mem_output_size = 0;
    //text_data = NULL;
    //text_output_data = NULL;

    image_variables_initialized = false;

    has_softmax = false;
    softmax_sum = 0.0;

    onodes = NULL;
    hnodes = NULL;
    newConvs = NULL;
    hnets = NULL;
    attentions = NULL;
    selfattentions = NULL;

}

NetBuilder::~NetBuilder()
{
    clearAndDeallocate();
}

int NetBuilder::getOutputImageDataSize(string id)
{
    int cindex = getConvIndexFromID(id);
    if (cindex >= 0) {
        if (convs[cindex].mode == "Input") {
            int size = 0;
            if (image_data_allocated) {
                size = getConvImageMemorySize(id);
                if (getConvGray(id)) {
                    size = 3 * size;
                }
            }
            return(size);
        }
    }
    for (int i = 0; i < nactive_index; i++) {
        if (newConvs[i].id == id) {
            int size = newConvs[i].output_data_size;
            if (getConvGray(id)) {
                size = 3 * size;
            }
            return(size);
        }
    }
    return(0);
}

imageData NetBuilder::getOutputImageData(string id, double* output_image_pointer)
{
    imageData rval;
    rval.numx = 0;
    rval.numy = 0;
    rval.xstride = 0;
    rval.gray = false;
    rval.pointer = output_image_pointer;
    if (image_data_allocated) {
        int cindex = getConvIndexFromID(id);
        if (cindex >= 0) {
            if (convs[cindex].mode == "Input") {
                rval.gray = convs[cindex].gray;
                rval.numx = convs[cindex].output_width;
                rval.numy = convs[cindex].output_height;
                rval.xstride = 3 * rval.numx;
                int offset = 0;
                int size1 = getConvImageMemorySize(id);
                int size2 = getOutputImageDataSize(id);
                int single = convs[cindex].input_width * convs[cindex].input_height;
                if (rval.gray) {
                    int index = 0;
                    int index2 = 0;
                    double gclr = 0;
                    for (int j = 0; j < rval.numy; j++) {
                        for (int i = 0; i < rval.numx; i++) {
                            index = (j * rval.numx) + i;
                            index2 = (j * rval.xstride) + (i * 3);
                            if (((offset + index) < image_mem_size) && (index < size1) && ((index2 + 2) < size2)) {
                                gclr = image_data[offset + index];
                                rval.pointer[index2 + 0] = gclr;
                                rval.pointer[index2 + 1] = gclr;
                                rval.pointer[index2 + 2] = gclr;
                            }
                            else if ((offset + index) >= image_mem_size) {
                                addErrorString("Error: getOutputImageData exceeding mem_size for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                            else if ((index2 + 2) >= size2) {
                                addErrorString("Error: getOutputImageData exceeding size2 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                            else {
                                addErrorString("Error: getOutputImageData exceeding size1 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                        }
                    }
                }
                else {
                    int index = 0;
                    int index2 = 0;
                    for (int j = 0; j < rval.numy; j++) {
                        for (int i = 0; i < rval.numx; i++) {
                            index = (j * rval.numx) + i;
                            index2 = (j * rval.xstride) + (i * 3);
                            if (((offset + index + (2 * single)) < image_mem_size) && ((index + (2 * single)) < size1) && ((index2 + 2) < size2)) {
                                rval.pointer[index2 + 2] = image_data[offset + index + (2 * single)];
                                rval.pointer[index2 + 1] = image_data[offset + index + single];
                                rval.pointer[index2 + 0] = image_data[offset + index];
                            }
                            else if ((offset + index + (2 * single)) >= image_mem_size) {
                                addErrorString("Error: getOutputImageData exceeding mem_size for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                            else if ((index2 + 2) >= size2) {
                                addErrorString("Error: getOutputImageData exceeding size2 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                            else {
                                addErrorString("Error: getOutputImageData exceeding size1 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                        }
                    }
                }
            }
        }
        for (int k = 0; k < nactive_index; k++) {
            if (newConvs[k].id == id) {
                rval.gray = convs[newConvToConv[k]].gray;
                rval.numx = newConvs[k].outputdims[0];
                rval.numy = newConvs[k].outputdims[1];
                rval.xstride = 3 * rval.numx;
                int size1 = newConvs[k].output_data_size;
                int size2 = getOutputImageDataSize(id);
                int single = newConvs[k].outputdims[0] * newConvs[k].outputdims[1];
                if (rval.gray) {
                    int index = 0;
                    int index2 = 0;
                    double gclr = 0;
                    for (int j = 0; j < rval.numy; j++) {
                        for (int i = 0; i < rval.numx; i++) {
                            index = (j * rval.numx) + i;
                            index2 = (j * rval.xstride) + (i * 3);
                            if ((index < size1) && ((index2 + 2) < size2)) {
                                gclr = newConvs[k].output_data[index];
                                rval.pointer[index2 + 0] = gclr;
                                rval.pointer[index2 + 1] = gclr;
                                rval.pointer[index2 + 2] = gclr;
                            }
                            else if ((index2 + 2) >= size2) {
                                addErrorString("Error: getOutputImageData exceeding size2 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                            else {
                                addErrorString("Error: getOutputImageData exceeding size1 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                        }
                    }
                }
                else {
                    int index = 0;
                    int index2 = 0;
                    for (int j = 0; j < rval.numy; j++) {
                        for (int i = 0; i < rval.numx; i++) {
                            index = (j * rval.numx) + i;
                            index2 = (j * rval.xstride) + (i * 3);
                            if (((index + (2 * single)) < size1) && ((index2 + 2) < size2)) {
                                rval.pointer[index2 + 2] = newConvs[k].output_data[index + (2 * single)];
                                rval.pointer[index2 + 1] = newConvs[k].output_data[index + single];
                                rval.pointer[index2 + 0] = newConvs[k].output_data[index];
                            }
                            else if ((index2 + 2) >= size2) {
                                addErrorString("Error: getOutputImageData exceeding size2 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                            else {
                                addErrorString("Error: getOutputImageData exceeding size1 for " + id);
                                rval.numx = rval.numy = rval.xstride = 0;
                                return(rval);
                            }
                        }
                    }
                }
            }
        }
    }
    return(rval);
}

bool NetBuilder::checkAndGetImageOutputLayers()
{
    bool rval = true;
    use_images_as_outputs = false;
    vector<string> image_output_layers;
    if (image_train_data_allocated) {
        CStatus progressDlg;
        int pathname_size = (int)dataPathnames.size();
        if (use_percent_validation) {
            if ((num_filedata_rows + num_validationdata_rows) != pathname_size) {
                char s[200]; sprintf_s(s, "Error : num_filedata_rows (%i) plus num_validationdata_rows (%i) not equal to dataPathnames size (%i)", num_filedata_rows, num_validationdata_rows, pathname_size);
                addErrorString(s);
                rval = false;
            }
        }
        if (rval) {
            if (getDataPathType() == FILE_TYPE_JPG) {
                if (pathname_size > 0) {
                    progressDlg.CreateDlg();
                    progressDlg.setLabel("Getting data file indices . . .");
                    progressDlg.setProgress(0);
                    for (int i = 0; i < pathname_size; i++) {
                        if (getCombinedIndex(getDataPathName(i)) < 0) {
                            addErrorString("Error : unable to find train file in combinedPathnames : " + getDataPathName(i));
                            i = pathname_size;
                            rval = false;
                        }
                        progressDlg.setProgress((100 * i) / pathname_size);
                    }
                    progressDlg.DestroyDlg();
                    bool check2 = true;
                    for (int i = 0; i < netData.size(); i++) {
                        if (netData.isConvLayer(i)) {
                            if ((netData.getNodeMode(i) == "Output") && (netData.getConvType(i) == IMAGE_CONV)) {
                                vector<string> snodes = netData.getImmediateSupportingNodes(i);
                                if (snodes.size() > 0) {
                                    use_images_as_outputs = true;
                                }
                                else {
                                    vector<string> sconvs = netData.getImmediateSupportingConvNets(i);
                                    if (sconvs.size() > 0) {
                                        for (int j = 0; j < sconvs.size(); j++) {
                                            if (netData.getConvType(sconvs[j]) == HIDDEN_NET) {
                                                use_images_as_outputs = true;
                                                j = (int)sconvs.size();
                                            }
                                        }
                                    }
                                }
                            }
                            if (netData.getNodeMode(i) == "Input") {
                                if (netData.getConvType(i) == IMAGE_CONV) {
                                    check2 = false;
                                }
                            }
                        }
                    }
                    if (use_images_as_outputs && check2) {
                        // do nothing
                    }
                }
            }
            else if ((getDataPathType() == FILE_TYPE_CSV) && csv_with_filename) {
                bool check2 = true;
                if (csv_image_filenames.size() > 0) {
                    for (int i = 0; i < netData.size(); i++) {
                        if (netData.isConvLayer(i)) {
                            if ((netData.getNodeMode(i) == "Output") && (netData.getConvType(i) == IMAGE_CONV)) {
                                vector<string> snodes = netData.getImmediateSupportingNodes(i);
                                if (snodes.size() > 0) {
                                    use_images_as_outputs = true;
                                }
                                else {
                                    vector<string> sconvs = netData.getImmediateSupportingConvNets(i);
                                    if (sconvs.size() > 0) {
                                        for (int j = 0; j < sconvs.size(); j++) {
                                            if (netData.getConvType(sconvs[j]) == HIDDEN_NET) {
                                                use_images_as_outputs = true;
                                                j = (int)sconvs.size();
                                            }
                                        }
                                    }
                                }
                            }
                            if ((netData.getNodeMode(i) == "Input") && (netData.getConvType(i) == IMAGE_CONV)) {
                                //check2 = false;
                            }
                        }
                    }
                }
                if (use_images_as_outputs && check2) {
                    // do nothing
                }
            }
        }
        if (rval) {
            if (getDataPathType() == FILE_TYPE_JPG) {
                int numvalfiles = (int)validationPathnames.size();
                if (!dovalidation) {
                    // Sometimes even though validation files aren't in use files get stuck in 
                    // validationPathnames, so clear any residuals
                    validationPathnames.clear();
                }
                else if (numvalfiles > 0) {
                    progressDlg.CreateDlg();
                    progressDlg.setLabel("Getting validation file indices . . .");
                    progressDlg.setProgress(0);
                    for (int i = 0; i < numvalfiles; i++) {
                        if (getCombinedIndex(getValidationPathName(i)) < 0) {
                            addErrorString("Error : unable to find validation file in combinedPathnames : " + getValidationPathName(i));
                            i = numvalfiles;
                            rval = false;
                        }
                        progressDlg.setProgress((100 * i) / numvalfiles);
                    }
                    progressDlg.DestroyDlg();
                }
            }
        }
    }
    return(rval);
}

bool NetBuilder::checkAndAllocateTrainImageMemory()
{
    bool rval = true;
    int data_path_type = getDataPathType();
    if ((data_path_type == FILE_TYPE_JPG) || (data_path_type == FILE_TYPE_RAW) || ((data_path_type == FILE_TYPE_CSV) && csv_with_filename)) {
        int color = 3;
        int image_count = 0;
        if (data_path_type == FILE_TYPE_JPG) {
            image_count = getCombinedImageCount();
        }
        else if (data_path_type == FILE_TYPE_RAW) {
            image_count = getCombinedImageCount();
        }
        else if ((data_path_type == FILE_TYPE_CSV) && csv_with_filename) {
            for (int i = 0; i < csv_image_filenames.size(); i++) {
                image_count += (int)csv_image_filenames[i].size();
            }
        }
        else {
            char s[200]; sprintf_s(s, "Error : type selection problem allocating image memory : data_path_type = %i", data_path_type);
            addErrorString(s);
            return(false);
        }
        int64_t max_image_allocation_size = 10000000000;
        int image_size = color * image_width * image_height;
        if (image_size > 0) {
            int image_load_count = (int)(max_image_allocation_size / image_size);
            if (image_load_count > image_count) {
                image_load_count = image_count;
            }
            else {
                char s[200]; sprintf_s(s, "image_load_count limited to image_count : image_count = %i : image_load_count = %i", image_count, image_load_count);
                addErrorString(s);
            }
            addPanel1String("Allocating image data array . . .");
            if (NetCore::checkAndAllocateTrainImageMemory(image_size, image_load_count)) {
            }
        }
        else {
            char s[200]; sprintf_s(s, "Error : image size problem allocating image memory : image_size = %i", image_size);
            addErrorString(s);
            rval = false;
        }
    }
    return(rval);
}

bool NetBuilder::checkAndAllocateTrainSoundMemory()
{
    bool rval = true;
    if (getDataPathType() == FILE_TYPE_WAV) {
        CAuxFunctions func;
        int wave_count = getCombinedImageCount();
        addPanel1String("Getting sound data parameters . . .");
        if ((wresults.wave_paths.size() != wresults.wave_data_lengths.size()) || (wresults.wave_data_lengths.size() == 0)) {
            char s[200]; sprintf_s(s, "Error : paths size error for sound files : wave_paths size = %i : wave_data_lengths size = %i", (int)wresults.wave_paths.size(), (int)wresults.wave_data_lengths.size());
            addErrorString(s);
            rval = false;
        }
        wresults.wave_start_indices.clear();
        wave_allocation_size = 0;
        sound_data_process_type = netData.getSoundConvDataProcessType();
        if ((sound_data_process_type == CONV_FOLLOWING) || (sound_data_process_type == FULLY_CONNECTED_FOLLOWING) || (sound_data_process_type == HIDDEN_NET_FOLLOWING)) {
            if (rval) {
                _int64 accumulated_starts = 0;
                _int64 max_sound_allocation_size = 1000000000;
                for (int i = 0; i < wresults.wave_data_lengths.size(); i++) {
                    wave_allocation_size += wresults.wave_data_lengths[i];
                    wresults.wave_start_indices.push_back(accumulated_starts);
                    accumulated_starts += wresults.wave_data_lengths[i];
                }
                if (wave_allocation_size > max_sound_allocation_size) {
                    addErrorString("Error : sound file allocation error");
                    rval = false;
                }
            }
            if (rval) {
                addPanel1String("Allocating sound data array . . .");
                rval = NetCore::checkAndAllocateTrainSoundMemory(wave_allocation_size);
            }
        }
        else {
            // do nothing
        }
    }
    return(rval);
}

bool NetBuilder::checkAndAllocateSeriesMemory()
{
    has_series_conv = false;
    if (convs.size() > 0) {
        int mem_size = 0;
        int backprop_size = 0;
        int data_conv_count = 0;
        for (int i = 0; i < (int)convs.size(); i++) {
            if (convs[i].data_type == DATA_CONV) {
                if ((convs[i].inputNodeIDs.size() > 0) || (convs[i].supportingConvIDs.size() > 0)) {
                    data_conv_count++;
                    has_series_conv = true;
                    series_input_width = convs[i].input_width;
                    series_input_height = convs[i].input_height;
                    if (num_inputs > series_input_height) {
                        series_input_height = num_inputs;
                    }
                    int tseries_mem_size = convs[i].input_width * convs[i].input_height;
                    if (tseries_mem_size > series_mem_size) {
                        mem_size = tseries_mem_size;
                    }
                    int tseries_backprop_size = convs[i].output_width * convs[i].output_height;
                    if (tseries_backprop_size > series_backprop_size) {
                        backprop_size = tseries_backprop_size;
                    }
                    //char s[200];  sprintf_s(s, "checkAndAllocateImageMemory - DATA_CONV : %i : %i", mem_size, backprop_size); addErrorString(convs[i].id + s);
                }
            }
            else if (convs[i].data_type == HIDDEN_NET) {
                if (convs[i].inputNodeIDs.size() > 0) {
                    for (int j = 0; j < (int)convs[i].inputNodeIDs.size(); j++) {
                        int nindex = netData.getIndexFromID(convs[i].inputNodeIDs[j]);
                        if ((netData.getNodeMode(nindex) == "Input") || (netData.getNodeMode(nindex) == "PreProcess")) {
                            int md = netData.getAttentionDm(netData.getIndexFromID(convs[i].id));
                            if (md > 1) {
                                has_series_conv = true;
                                series_input_width = md;
                                series_input_height = num_inputs;
                                int tseries_mem_size = series_input_width * series_input_height;
                                if (tseries_mem_size > series_mem_size) {
                                    mem_size = tseries_mem_size;
                                }
                                j = (int)convs[i].inputNodeIDs.size();
                                //char s[200];  sprintf_s(s, "checkAndAllocateImageMemory - HIDDEN_NET : %i : %i", mem_size, backprop_size); addErrorString(convs[i].id + s);
                            }
                        }
                    }
                }
            }
        }
        if (has_series_conv) {
            return(NetCore::checkAndAllocateSeriesMemory(mem_size, backprop_size));
        }
    }
    return(true);
}

bool NetBuilder::checkAndAllocateImageMemory()
{
    has_image_conv = false;
    if (convs.size() > 0) {
        int mem_size = 0;
        int backprop_size = 0;
        for (int i = 0; i < (int)convs.size(); i++) {
            if (convs[i].data_type == IMAGE_CONV) {
                has_image_conv = true;
                i = (int)convs.size();
            }
        }
        if (has_image_conv) {
            mem_size = image_width * image_height;
            if (!netData.getImageGray()) {
                mem_size = 3 * mem_size;
            }
            backprop_size = getConvBackpropMemorySize();
            return(NetCore::checkAndAllocateImageMemory(mem_size, backprop_size));
        }
    }
    return(true);
}

bool NetBuilder::checkAndAllocateSoundMemory()
{
    has_wave_conv = false;
    if (convs.size() > 0) {
        int mem_size = 0;
        for (int i = 0; i < (int)convs.size(); i++) {
            if (convs[i].data_type == SOUND_CONV) {
                mem_size = convs[i].input_width * convs[i].input_height;
                has_wave_conv = true;
                i = (int)convs.size();
            }
        }
        if (has_wave_conv) {
            if (mem_size > 0) {
                return(NetCore::checkAndAllocateSoundMemory(mem_size));
            }
        }
    }
    return(true);
}

/*bool NetBuilder::checkAndAllocateTextMemory()
{
    has_text_conv = false;
    if (tparams.text_data) {
        int will_need_to_allocate_text_backprop_data_here;
        CWnd* pWndMain = AfxGetMainWnd();
        CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
        text_mem_size = num_inputs;   
        text_mem_output_size = num_outputs * tparams.nword_vector;
        text_data = new double[text_mem_size];
        text_output_data = new double[text_mem_output_size];
        if ((text_data == 0) || (text_output_data == 0)) {
            AfxMessageBox(_T("NetBuilder::initialize : unable to allocate memory for text data"));
            return(false);
        }
        for (int i = 0; i < text_mem_size; i++) {
            text_data[i] = 0.0;
        }
        for (int i = 0; i < text_mem_output_size; i++) {
            text_output_data[i] = 0.0;
        }
        text_data_allocated = true;
        has_text_conv = true;
        //CString s = _T("");
        //s.Format(_T("checkAndAllocateTextMemory : text_mem_size = %i"), text_mem_size);
        //addErrorString(s);
    }
    return(true);
}*/

bool NetBuilder::initialize(NetData netdata, int ctype, bool randomize_weights, bool trandomize_data, float urate)
{

    train_count = 0;
    err_count = 0;

    netData = netdata;

    expandAssemblies();

    getNetData();

    getNetInputs();

    // Set up progress bar
#ifndef GCC
    CStatus myProgressDlg;
    myProgressDlg.CreateDlg();
    myProgressDlg.setLabel("Initializing Net . . .");
    myProgressDlg.setProgress(0);
#endif
    addPanel1String("Initializing Net");

    // Clear diagnostic strings
    verifyStrings.clear();

    // Deallocate allocatable memory
    if (allocated) {
        clearAndDeallocate();
    }

    updateRate = urate;
    randomize_data = trandomize_data;

    // Some initialization
    //tparams = getTextParameters();

    // Count hidden and output nodes
    num_outputs = 0;
    num_hidden_nodes = 0;
    num_inputs = (int)netInputs.size();
    int total_num_connects = 0;
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        if (netData.getIsOutput(nodeIndices[i])) {
            if (netData.getGroupIndex(nodeIndices[i]) == -1) {
                num_outputs++;
            }
        }
        else if ((netData.getNodeFunction(nodeIndices[i]) != "Add") && (netData.getNodeFunction(nodeIndices[i]) != "Multiply")) {
            num_hidden_nodes++;
        }
        total_num_connects += (int)nodeInputs[i].size();
    }

#ifndef GCC
    myProgressDlg.setProgress(5);
    myProgressDlg.setLabel("Initializing Net - sorting convolutions");
#endif
    addPanel1String("Sorting convolutions");

    // Check for and sort conv nets
    getConvGroups();
 
    num_active_convs = getActiveConvs();
    num_hidden_nets = getHiddenNets();
    num_attention_layers = getAttentionLayers();
    num_self_attention_layers = getSelfAttentionLayers();
    num_net_clusters = getNetClusters();
    num_dropouts = getDropouts();
    num_additions = getAdditions();
    num_multiplications = getMultiplications();

    has_image_conv = false;
    has_wave_conv = false;
    has_wave_fft_conv = false;
    has_series_conv = false;
    has_text_conv = false;
    csv_with_filename = false;
    csv_filename_header = "";
    csv_image_filenames.clear();

#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - checking memory");
#endif
    addPanel1String("Checking memory");

    if (!checkAndAllocateImageMemory()) {
        return(false);
    }
    if (!checkAndAllocateSoundMemory()) {
        return(false);
    }
    if (!checkAndAllocateSeriesMemory()) {
        return(false);
    }

#ifndef GCC
    myProgressDlg.setProgress(10);
#endif

    if (num_outputs == 0) {
        if (convs.size() > 0) {
            //verifyStrings.push_back("Observation : NetBuilder::initialize : num outputs = 0");
        }
        else {
            verifyStrings.push_back("Error : NetBuilder::initialize : num outputs = 0");
            return(false);
        }
    }
    else if ((num_inputs == 0) && (convInputs.size() == 0)) {
        verifyStrings.push_back("Error : NetBuilder::initialize : num input nodes = 0");
        return(false);
    }

#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - allocating memory");
#endif
    addPanel1String("Allocating memory");

    // Allocate input/output memory
    inputs = new double[num_inputs];
    testoutputs = new double[num_outputs];
    if ((inputs == 0) || (testoutputs == 0)) {
        verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate inputs and testoutputs");
        return(false);
    }
    for (int i = 0; i < num_inputs; i++) {
        dataInput tinput;
        tinput.label = netInputs[i].nodeId;
        tinput.inputpointer = (inputs + i);
        inputPointers.push_back(tinput);
        //addErrorString("adding pointer for " + netInputs[i].nodeId + " to input pointers");
    }

#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - allocating nodes");
#endif
    addPanel1String("Allocating nodes");

    // Allocate nodes
    //CString s7 = _T(""); s7.Format(_T("num_outputs = %i"), num_outputs);  AfxMessageBox(s7);
    if (num_outputs> 0) {
        onodes = new OutputNode[num_outputs];
        if (onodes == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate output nodes");
            return(false);
        }
    }

    //s7.Format(_T("num_hidden_nodes = %i"), num_hidden_nodes);  AfxMessageBox(s7);
    if (num_hidden_nodes > 0) {
        hnodes = new HiddenNode[num_hidden_nodes];
        if (hnodes == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate hidden nodes");
            return(false);
        }
    }

    //s7.Format(_T("num_active_convs = %i"), num_active_convs);  AfxMessageBox(s7);
    if (num_active_convs> 0) {
        newConvs = new HiddenConvolution[num_active_convs];
        if (newConvs == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate convolutional layers");
            return(false);
        }
    }

    // Allocate hidden nets
    //s7.Format(_T("num_hidden_nets = %i"), num_hidden_nets);  AfxMessageBox(s7);
    if (num_hidden_nets > 0) {
        hnets = new HiddenNet[num_hidden_nets];
        if (hnets == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate hidden nets");
            return(false);
        }
    }

    // Allocate attention layers
    //s7.Format(_T("num_attention_layers = %i"), num_attention_layers);  AfxMessageBox(s7);
    if (num_attention_layers > 0) {
        attentions = new Attention[num_attention_layers];
        if (attentions == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate attention layers");
            return(false);
        }
    }

    // Allocate self attention layers
    //s7.Format(_T("num_self_attention_layers = %i"), num_self_attention_layers);  AfxMessageBox(s7);
    if (num_self_attention_layers > 0) {
        selfattentions = new SelfAttention[num_self_attention_layers];
        if (selfattentions == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate self attention layers");
            return(false);
        }
    }

    // Allocate net clusters
    //s7.Format(_T("num_net_clusters = %i"), num_net_clusters);  AfxMessageBox(s7);
    if (num_net_clusters > 0) {
        nclusters = new NetCluster[num_net_clusters];
        if (nclusters == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate net patterns");
            return(false);
        }
    }

    // Allocate dropouts
    //s7.Format(_T("num_dropouts = %i"), num_dropouts);  AfxMessageBox(s7);
    if (num_dropouts > 0) {
        dropouts = new Dropout[num_dropouts];
        if (dropouts == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate dropouts");
            return(false);
        }
    }

    // Allocate additions
    //s7.Format(_T("num_additions = %i"), num_additions);  AfxMessageBox(s7);
    if (num_additions > 0) {
        additions = new Addition[num_additions];
        if (additions == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate additions");
            return(false);
        }
    }

    // Allocate multiplications
    //s7.Format(_T("num_multiplications = %i"), num_multiplications);  AfxMessageBox(s7);
    if (num_multiplications > 0) {
        multiplications = new Multiplication[num_multiplications];
        if (multiplications == 0) {
            verifyStrings.push_back("Error : NetBuilder::initialize : unable to allocate multiplications");
            return(false);
        }
    }

    allocated = true;

#ifndef GCC
    myProgressDlg.setProgress(20);
#endif

    // Vector nodes_used is used to keep track of nodes used as they are added in feed forward order
    nodes_used.clear();
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        nodes_used.push_back(false);
    }

    // Add hidden nodes to net
    nactive_index = 0;
    nhidden_net_index = 0;
    nattention_layer_index = 0;
    nself_attention_layer_index = 0;
    nnet_cluster_index = 0;
    ndropout_index = 0;
    naddition_index = 0;
    nmultiplication_index = 0;
    bool done = false;
    bool changed = false;
    int hnode_index = 0;
    vector<int> hnode_to_net_indices;
    vector<string> waiting_conv_layers;

#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - initializing net components");
#endif
    addPanel1String("Initializing net components");

    // Continue to look through un-added nodes to see if their input requirements are met
    int lcnt = 0;
    while (!done) {

        changed = false;

        //AfxMessageBox(_T("addNextHiddenNodes"));
        // Look through all non-output nodes (should be only hidden nodes)
        if (addNextHiddenNodes(&hnode_index, &hnode_to_net_indices, randomize_weights) > 0) {
            changed = true;
        }

        // If there are dependencies on conv layers
        if (areConvsRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextConvs2"));
            if (addNextConvs2(ctype, randomize_weights) > 0) {
                changed = true;
            }
        }

        // If there are dependencies on hidden nets
        if (areHiddenNetsRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextHiddenNet"));
            if (addNextHiddenNet(randomize_weights) > 0) {
                changed = true;
            }
        }

        // If there are dependencies on attention layers
        if (areAttentionsRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextAttentionLayer"));
            if (addNextAttentionLayer() > 0) {
                changed = true;
            }
        }

        // If there are dependencies on self attention layers
        if (areSelfAttentionsRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextSelfAttentionLayer"));
            if (addNextSelfAttentionLayer() > 0) {
                changed = true;
            }
        }

        // If there are dependencies on net clusters
        if (areNetClustersRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextNetCluster"));
            if (addNextNetCluster(randomize_weights) > 0) {
                changed = true;
            }
        }

        // If there are dependencies on dropouts
        if (areDropoutsRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextDropout"));
            if (addNextDropout() > 0) {
                changed = true;
            }
        }

        // If there are dependencies on additions
        if (areAdditionsRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextAddition"));
            if (addNextAddition() > 0) {
                changed = true;
            }
        }

        // If there are dependencies on multiplications
        if (areMultiplicationsRemainingToProcess() > 0) {
            //AfxMessageBox(_T("addNextMultiplication"));
            if (addNextMultiplication() > 0) {
                changed = true;
            }
        }

        // If we've reached our previous count or nothing's changed, exit loop
        if (((hnode_index == num_hidden_nodes) && 
            (nactive_index == num_active_convs) && 
            (nhidden_net_index == num_hidden_nets) && 
            (nattention_layer_index == num_attention_layers) && 
            (nself_attention_layer_index == num_self_attention_layers) &&
            (nnet_cluster_index == num_net_clusters) &&
            (ndropout_index == num_dropouts) &&
            (naddition_index == num_additions) &&
            (nmultiplication_index == num_multiplications)) ||
            !changed || (lcnt > 20)) {
            done = true;
        }
        else {
            //AfxMessageBox(_T("continue"));
            lcnt++;
        }

        if (num_hidden_nodes > 0) {
#ifndef GCC
            myProgressDlg.setProgress(20 + (hnode_index * 30) / num_hidden_nodes);
#endif
        }

    }

    addPanel1String("");

#ifndef GCC
    myProgressDlg.setProgress(50);
    myProgressDlg.setLabel("Initializing Net - error checks");
#endif
    addPanel1String("Error checks");

    // Check to see if we missed any hidden nodes
    vector<string> unconnected;
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        if (!netData.getIsOutput(nodeIndices[i])) {
            if (!nodes_used[i]) {
                unconnected.push_back(netData.getID(nodeIndices[i]));
            }
        }
    }
    for (int i = 0; i < (int)convs.size(); i++) {
        if (!convs[i].used && isActiveConvolution(i)) {
            unconnected.push_back(convs[i].id);
        }
    }
    if (unconnected.size() > 0) {
        char ls[200];  sprintf_s(ls, "Unable to connect %i nodes after hidden node input search", (int)unconnected.size());
        string s = ls + netData.sortNodesAll(unconnected, false);
        addErrorString("");
        addErrorString(s);
    }
    if (hnode_index != num_hidden_nodes) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : hnode_index (%i) != num_hidden_nodes (%i)", hnode_index, num_hidden_nodes);
        verifyStrings.push_back(s);
        return(false);
    }
    if (nactive_index != num_active_convs) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : nactive_index (%i) != num_active_convs (%i)", nactive_index, num_active_convs);
        verifyStrings.push_back(s);
        return(false);
    }
    if (nhidden_net_index != num_hidden_nets) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : nhidden_net_index (%i) != num_hidden_nets (%i)", nhidden_net_index, num_hidden_nets);
        verifyStrings.push_back(s);
        return(false);
    }
    if (nattention_layer_index != num_attention_layers) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : nattention_layer_index (%i) != num_attention_layers (%i)", nattention_layer_index, num_attention_layers);
        verifyStrings.push_back(s);
        return(false);
    }
    if (nself_attention_layer_index != num_self_attention_layers) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : nself_attention_layer_index (%i) != num_self_attention_layers (%i)", nself_attention_layer_index, num_self_attention_layers);
        verifyStrings.push_back(s);
        return(false);
    }
    if (nnet_cluster_index != num_net_clusters) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : nnet_cluster_index (%i) != num_net_clusters (%i)", nnet_cluster_index, num_net_clusters);
        verifyStrings.push_back(s);
        return(false);
    }
    if (ndropout_index != num_dropouts) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : ndropout_index (%i) != num_dropouts (%i)", ndropout_index, num_dropouts);
        verifyStrings.push_back(s);
        return(false);
    }
    if (naddition_index != num_additions) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : naddition_index (%i) != num_additions (%i)", naddition_index, num_additions);
        verifyStrings.push_back(s);
        return(false);
    }
    if (nmultiplication_index != num_multiplications) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : nmultiplication_index (%i) != num_multiplications (%i)", nmultiplication_index, num_multiplications);
        verifyStrings.push_back(s);
        return(false);
    }

    int dcnt = 0;
    int dupdate = num_outputs / 200;
    if (dupdate < 1) {
        dupdate = 1;
    }

#ifndef GCC
    myProgressDlg.setProgress(60);
    myProgressDlg.setLabel("Initializing Net - initializing output nodes");
#endif
    char s[200];  sprintf_s(s, "Initializing output nodes (0:%i)", num_outputs);
    addPanel1String(s);

    // Add output nodes to net
    int added = 0;
    done = false;
    int onode_index = 0;
    vector<int> onode_to_net_indices;
    // Continue to look through un-added nodes to see if their input requirements are met
    while (!done) {
        added = 0;
        // Look through all output nodes
        for (int i = 0; i < (int)nodeIndices.size(); i++) {
            if (netData.getIsOutput(nodeIndices[i])) {
                // Toggle conv nodes first and then non-conv nodes
                    // if the nodes haven't already been added
                if (!nodes_used[i]) {
                    // Check to see if input conditions are satisfied before adding node to net
                    if (areInputsCompleted(i)) {
                        // Add output node
                        if (addOutputNode(i, onode_index, randomize_weights)) {
                            // Save node index and finish up
                            onode_to_net_indices.push_back(i);
                            nodes_used[i] = true;
                            onode_index++;
                            added++;
                            //addErrorString("output node " + netData.getID(nodeIndices[i]) + " added");
                        }
                        else {
                            // If our count is off, error out
                            verifyStrings.push_back("Error : NetBuilder::initialize : error in addOutputNode");
                            return(false);
                        }
                        if (dcnt >= dupdate) {
                            char s[200];  sprintf_s(s, "Initializing output nodes (%i:%i)", onode_index, num_outputs);
                            addPanel1String(s);
                            dcnt = 0;
                        }
                        dcnt++;
                    }
                }
            }
        }
        // If we've reached our previous count or nothing's changed, exit loop
        if ((onode_index == num_outputs) || (added == 0)) {
            /*if (onode_index == num_outputs)   verifyStrings.push_back("Exiting output node search : onode_index == num_outputs");  }
            else if (added == 0) {  verifyStrings.push_back("Exiting output search : added == 0");  }*/
            done = true;
        }
    }

    // Check to see if we missed any output nodes
    if (onode_index != num_outputs) {
        char s[200];  sprintf_s(s, "Error : NetBuilder::initialize : onode_index (%i) != num_outputs (%i)", onode_index, num_outputs);
        verifyStrings.push_back(s);
        return(false);
    }

    addPanel1String("Joining components");

    //AfxMessageBox(_T("joining hidden nodes"));
#ifndef GCC
    myProgressDlg.setProgress(70);
    myProgressDlg.setLabel("Initializing Net - joining hidden nodes");
#endif
    if (!joinHiddenNodesToPrevious()) {
        verifyStrings.push_back("Error joining hidden nodes");
        return(false);
    }

    //AfxMessageBox(_T("joining output nodes"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining output nodes");
#endif
    if (!joinOutputNodesToPrevious()) {
        verifyStrings.push_back("Error joining output nodes");
        return(false);
    }

    //AfxMessageBox(_T("joining hidden nets"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining hidden nets");
#endif
    if (!joinHiddenNetsToPrevious()) {
        verifyStrings.push_back("Error joining hidden nets");
        return(false);
    }

    //AfxMessageBox(_T("joining convolutions"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining convolutions");
#endif
    if (!joinConvolutionsToPrevious()) {
        verifyStrings.push_back("Error joining convolutions");
        return(false);
    }

    //AfxMessageBox(_T("joining attentions"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining attentions");
#endif
    if (!joinAttentionsToPrevious()) {
        verifyStrings.push_back("Error joining attentions");
        return(false);
    }

    //AfxMessageBox(_T("joining self attentions"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining self attentions");
#endif
    if (!joinSelfAttentionsToPrevious()) {
        verifyStrings.push_back("Error joining self attentions");
        return(false);
    }

    //AfxMessageBox(_T("joining clusters"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining cluster nets");
#endif
    if (!joinClustersToPrevious()) {
        verifyStrings.push_back("Error joining cluster nets");
        return(false);
    }

    //AfxMessageBox(_T("joining dropouts"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining dropouts");
#endif
    if (!joinDropoutsToPrevious()) {
        verifyStrings.push_back("Error joining dropouts");
        return(false);
    }

    //AfxMessageBox(_T("joining additions"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining additions");
#endif
    if (!joinAdditionsToPrevious()) {
        verifyStrings.push_back("Error joining additions");
        return(false);
    }

    //AfxMessageBox(_T("joining multiplications"));
#ifndef GCC
    myProgressDlg.setLabel("Initializing Net - joining multiplications");
#endif
    if (!joinMultiplicationsToPrevious()) {
        verifyStrings.push_back("Error joining multiplications");
        return(false);
    }


    //AfxMessageBox(_T("done joining"));

    // Set inputs turned off to zero
    getIndicesWithInputs();
    for (int i = 0; i < (int)netInputs.size(); i++) {
        int nindex = netData.getIndexFromID(netInputs[i].nodeId);
        if (netData.getNodeMode(nindex) == "PreProcess") {
            if (netData.getNodeZeroOutput(nindex)) {
                setInputToZero(netInputs[i].nodeId);
            }
            else {
                vector<string> snodes = netData.getImmediateSupportingNodes(nindex);
                if (snodes.size() == 1) {
                    int sindex = netData.getIndexFromID(snodes[0]);
                    if (netData.getNodeZeroOutput(sindex)) {
                        setInputToZero(netInputs[i].nodeId);
                    }
                }
            }
        }
        else if (netData.getNodeMode(nindex) == "Input") {
            if (netData.getNodeZeroOutput(nindex)) {
                setInputToZero(netInputs[i].nodeId);
            }
        }
    }

#ifndef GCC
    myProgressDlg.setProgress(80);
    myProgressDlg.setLabel("Initializing Net - checking for softmax connections");
#endif
    addPanel1String("Checking for softmax connections");

    // Check and process softmax layer functionality
    addPanel1String("Checking softmax");
    int nsoft = 0;
    has_softmax = false;
    for (int i = 0; i < num_outputs; i++) {
        if (onodes[i].type == SOFTMAX) {
            nsoft++;
            if (nsoft > 1) {
                has_softmax = true;
                i = num_outputs;
            }
        }
    }

#ifndef GCC
    myProgressDlg.setProgress(95);
    addPanel1String("");
#endif

    // Report node count eror or values
    displayInitData(total_num_connects);

#ifndef GCC
    myProgressDlg.DestroyDlg();
#endif

    return(true);

}

bool NetBuilder::joinHiddenNodesToPrevious()
{
    // Connect hidden nodes to their supporting components
    for (int i = 0; i < num_hidden_nodes; i++) {
        //addErrorString("joining hidden node " + hnodes[i].id + " to previous");
        // Get netData index of hidden node to connect (need this index to find its following components)
        int hindex = netData.getIndexFromID(hnodes[i].id);
        if (hindex >= 0) {

            // If index is valid, get the node's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(hindex);
            if (snodes.size() > 0) {
                // If there are node supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!hnodes[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in additions
                        for (int k = 0; k < num_additions; k++) {
                            if (additions[k].id == snodes[j]) {
                                if (!hnodes[i].join(&additions[k])) {
                                    return(false);
                                }
                                k = num_additions;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in multiplications
                        for (int k = 0; k < num_multiplications; k++) {
                            if (multiplications[k].id == snodes[j]) {
                                if (!hnodes[i].join(&multiplications[k])) {
                                    return(false);
                                }
                                k = num_multiplications;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!hnodes[i].addInputPointer(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        // In case of fft input
                        boolIntVectorStringDoublePointer ival = isInputProcessNode(snodes[j]);
                        if (ival.valid && (ival.count > 0)) {
                            for (int m = 0; m < ival.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!hnodes[i].addInputPointer(snodes[j] + s, ival.pointer + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting hidden nodes : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get node's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(hindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!hnodes[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!hnodes[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in clusters
                        for (int k = 0; k < num_net_clusters; k++) {
                            if (sconvs[j] == nclusters[k].id) {
                                if (!hnodes[i].join(&nclusters[k])) {
                                    return(false);
                                }
                                k = num_net_clusters;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in dropouts
                        for (int k = 0; k < num_dropouts; k++) {
                            if (sconvs[j] == dropouts[k].id) {
                                if (!hnodes[i].join(&dropouts[k])) {
                                    return(false);
                                }
                                k = num_dropouts;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!hnodes[i].addInputPointer(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!hnodes[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!hnodes[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting hidden nodes : unable to find supporting conv for " + hnodes[i].id + " : " + sconvs[j]);
                        return(false);
                    }
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting hidden nodes : unable to find hindex for " + hnodes[i].id);
            return(false);
        }

    }
    // Check for hidden nodes that still need input pointers
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (hnodes[i].num_inputs == 0) {
            verifyStrings.push_back("hnode " + hnodes[i].id + " has no input connections");
            return(false);
        }
    }

    return(true);
}

bool NetBuilder::joinOutputNodesToPrevious()
{
    // Connect output nodes to their supporting components
    for (int i = 0; i < num_outputs; i++) {
        //addErrorString("joining output node " + onodes[i].id + " to previous");
        // Get netData index of output node to connect (need this index to find its following components)
        int oindex = netData.getIndexFromID(onodes[i].id);
        if (oindex >= 0) {

            // If index is valid, get the node's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(oindex);
            if (snodes.size() > 0) {
                // If there are node supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!onodes[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in additions
                        for (int k = 0; k < num_additions; k++) {
                            if (additions[k].id == snodes[j]) {
                                if (!onodes[i].join(&additions[k])) {
                                    return(false);
                                }
                                k = num_additions;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in multiplications
                        for (int k = 0; k < num_multiplications; k++) {
                            if (multiplications[k].id == snodes[j]) {
                                if (!onodes[i].join(&multiplications[k])) {
                                    return(false);
                                }
                                k = num_multiplications;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!onodes[i].addInputPointer(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        // In case of fft input
                        boolIntVectorStringDoublePointer ival = isInputProcessNode(snodes[j]);
                        if (ival.valid && (ival.count > 0)) {
                            for (int m = 0; m < ival.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!onodes[i].addInputPointer(snodes[j] + s, ival.pointer + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting output nodes : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get node's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(oindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!onodes[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!onodes[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in clusters
                        for (int k = 0; k < num_net_clusters; k++) {
                            if (sconvs[j] == nclusters[k].id) {
                                if (!onodes[i].join(&nclusters[k])) {
                                    return(false);
                                }
                                k = num_net_clusters;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in dropouts
                        for (int k = 0; k < num_dropouts; k++) {
                            if (sconvs[j] == dropouts[k].id) {
                                if (!onodes[i].join(&dropouts[k])) {
                                    return(false);
                                }
                                k = num_dropouts;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting output nodes : unable to find supporting conv for " + onodes[i].id + " : " + sconvs[j] + " (1)");
                        return(false);
                    }
                }
            }

            // This is a hack needed to deal with another hack but we convert convs to nodes 
            //  for some processes and we need to check for that posibility also
            if (getConvFromConnectName(onodes[i].id)) {
                string cname = netData.getConvNameFromConnectName(onodes[i].id);
                int cindex = netData.getIndexFromID(cname);
                if (cindex >= 0) {

                    // If index is valid, get the node's supporting nodes
                    vector<string> snodes = netData.getImmediateSupportingNodes(cindex);
                    if (snodes.size() > 0) {
                        static bool t3 = true;
                        // If there are node supporting, find their match (by name) in the net structure
                        for (int j = 0; j < (int)snodes.size(); j++) {
                            // Search for a hidden node match
                            bool found = false;
                            for (int k = 0; k < num_hidden_nodes; k++) {
                                if (hnodes[k].id == snodes[j]) {
                                    if (!onodes[i].join(&hnodes[k])) {
                                        return(false);
                                    }
                                    k = num_hidden_nodes;
                                    found = true;
                                }
                            }
                            if (!found) {
                                verifyStrings.push_back("Connecting output nodes : unable to find supporting node for " + onodes[i].id + " : " + snodes[j]);
                                return(false);
                            }
                        }
                    }

                    // Get supporting convs
                    sconvs = netData.getImmediateSupportingConvNets(cindex);
                    if (sconvs.size() > 0) {
                        static bool t4 = true;
                        // If there are convs supporting, find their match (by name) in the net structure
                        for (int j = 0; j < (int)sconvs.size(); j++) {
                            bool found = false;
                            // Look for ID in hidden nets
                            for (int k = 0; k < num_hidden_nets; k++) {
                                if (sconvs[j] == hnets[k].id) {
                                    if (!onodes[i].join(&hnets[k])) {
                                        return(false);
                                    }
                                    k = num_hidden_nets;
                                    found = true;
                                }
                            }
                            if (!found) {
                                for (int k = 0; k < num_active_convs; k++) {
                                    if (sconvs[j] == newConvs[k].id) {
                                        if (!onodes[i].join(&newConvs[k])) {
                                            return(false);
                                        }
                                        k = num_active_convs;
                                        found = true;
                                    }
                                }
                            }
                            if (!found) {
                                verifyStrings.push_back("Connecting output nodes : unable to find supporting conv for " + onodes[i].id + " : " + sconvs[j] + " (2)");
                                return(false);
                            }
                        }
                    }

                }
            }

        }
        else {
            verifyStrings.push_back("Connecting output nodes : unable to find oindex for " + onodes[i].id);
            return(false);
        }

    }
    // Check for output nodes that still need input pointers
    for (int i = 0; i < num_outputs; i++) {
        if (onodes[i].num_inputs == 0) {
            verifyStrings.push_back("onode " + onodes[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinConvolutionsToPrevious()
{
    // Connect conv nets to their supporting components
    for (int i = 0; i < num_active_convs; i++) {
        //addErrorString("joining convolution " + newConvs[i].id + " to previous");
        // Get netData index of conv net to connect (need this index to find its following components)
        int cindex = netData.getIndexFromID(newConvs[i].id);
        if (cindex >= 0) {

            // If index is valid, get the conv's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(cindex);
            if (snodes.size() > 0) {
                // If there are node supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    bool found = false;
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!newConvs[i].addInputPointer(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        // In case of fft input
                        boolIntVectorStringDoublePointer ival = isInputProcessNode(snodes[j]);
                        if (ival.valid) {
                            if (ival.count > 0) {
                                for (int m = 0; m < ival.count; m++) {
                                    char s[100];  sprintf_s(s, ":%i", m);
                                    if (!newConvs[i].addInputPointer(snodes[j] + s, ival.pointer + m)) {
                                        return(false);
                                    }
                                }
                                found = true;
                            }
                            else if (ival.inputs.size() > 0) {
                                for (int m = 0; m < (int)ival.inputs.size(); m++) {
                                    boolDoublePointer dval = getInputPointerFromInput(ival.inputs[m]);
                                    if (dval.valid) {
                                        if (!newConvs[i].addInputPointer(ival.inputs[m], dval.input)) {
                                            return(false);
                                        }
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting conv nets : unable to find supporting connection for " + snodes[j] + " to " + newConvs[i].id);
                        //return(false);
                    }
                }
            }

            // Get node's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(cindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!newConvs[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in dropouts
                        for (int k = 0; k < num_dropouts; k++) {
                            if (sconvs[j] == dropouts[k].id) {
                                if (!newConvs[i].join(&dropouts[k])) {
                                    return(false);
                                }
                                k = num_dropouts;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!newConvs[i].addInputPointer(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!newConvs[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            int msize = newConvs[i].inputdims[0] * newConvs[i].inputdims[1];
                            if (dval.count < msize) {
                                msize = dval.count;
                            }
                            for (int m = 0; m < msize; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!newConvs[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting conv nets : unable to find supporting connection for " + sconvs[j] + " to " + newConvs[i].id);
                        //return(false);
                    }
                }
            }

            // Get conv following input image data
            if ((netData.getNodeMode(cindex) == "Output") && (netData.getConvType(cindex) == IMAGE_CONV)) {
                if (!setupConvolutionToImageOutputLayerBackpropPointers(i)) {
                    return(false);
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting conv nets : unable to find cindex for " + newConvs[i].id);
            return(false);
        }

    }
    // Check for conv nets that still need input pointers
    for (int i = 0; i < num_active_convs; i++) {
        if (newConvs[i].input_data_size == 0) {
            verifyStrings.push_back("conv " + newConvs[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinHiddenNetsToPrevious()
{
    // Connect hidden nets to their supporting components
    for (int i = 0; i < num_hidden_nets; i++) {
        // Get netData index of hidden net to connect (need this index to find its following components)
        //addErrorString("joining hidden net " + hnets[i].id + " to previous");
        int hindex = netData.getIndexFromID(hnets[i].id);
        if (hindex >= 0) {

            // If index is valid, get the node's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(hindex);
            if (snodes.size() > 0) {
                // If there are node supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!hnets[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in additions
                        for (int k = 0; k < num_additions; k++) {
                            if (additions[k].id == snodes[j]) {
                                if (!hnets[i].join(&additions[k])) {
                                    return(false);
                                }
                                k = num_additions;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in multiplications
                        for (int k = 0; k < num_multiplications; k++) {
                            if (multiplications[k].id == snodes[j]) {
                                if (!hnets[i].join(&multiplications[k])) {
                                    return(false);
                                }
                                k = num_multiplications;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!hnets[i].addInputPointer(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        // In case of fft input
                        boolIntVectorStringDoublePointer ival = isInputProcessNode(snodes[j]);
                        if (ival.valid) {
                            if (ival.count > 0) {
                                for (int m = 0; m < ival.count; m++) {
                                    char s[100];  sprintf_s(s, ":%i", m);
                                    if (!hnets[i].addInputPointer(snodes[j] + s, ival.pointer + m)) {
                                        return(false);
                                    }
                                }
                                found = true;
                            }
                            else if (ival.inputs.size() > 0) {
                                for (int m = 0; m < (int)ival.inputs.size(); m++) {
                                    boolDoublePointer dval = getInputPointerFromInput(ival.inputs[m]);
                                    if (dval.valid) {
                                        if (!hnets[i].addInputPointer(ival.inputs[m], dval.input)) {
                                            return(false);
                                        }
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting hidden nets : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get node's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(hindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!hnets[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!hnets[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in attentions
                        for (int k = 0; k < num_attention_layers; k++) {
                            if (sconvs[j] == attentions[k].id) {
                                if (!hnets[i].join(&attentions[k])) {
                                    return(false);
                                }
                                //addErrorString("hnet " + hnets[i].id + " completing join to attention " + attentions[k].id);
                                k = num_attention_layers;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in self attentions
                        for (int k = 0; k < num_self_attention_layers; k++) {
                            if (sconvs[j] == selfattentions[k].id) {
                                if (!hnets[i].join(&selfattentions[k])) {
                                    return(false);
                                }
                                k = num_self_attention_layers;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in clusters
                        for (int k = 0; k < num_net_clusters; k++) {
                            if (sconvs[j] == nclusters[k].id) {
                                if (!hnets[i].join(&nclusters[k])) {
                                    return(false);
                                }
                                k = num_net_clusters;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in dropouts
                        for (int k = 0; k < num_dropouts; k++) {
                            if (sconvs[j] == dropouts[k].id) {
                                if (!hnets[i].join(&dropouts[k])) {
                                    return(false);
                                }
                                k = num_dropouts;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!hnets[i].addInputPointer(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!hnets[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!hnets[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting hidden nets : unable to find supporting conv for " + hnets[i].id + " : " + sconvs[j]);
                        return(false);
                    }
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting hidden nets : unable to find hindex for " + hnets[i].id);
            return(false);
        }

    }
    // Check for hidden nets that still need input pointers
    for (int i = 0; i < num_hidden_nets; i++) {
        if (hnets[i].num_inputs == 0) {
            verifyStrings.push_back("hnet " + hnets[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinAttentionsToPrevious()
{
    // Connect attentions to their supporting components
    for (int i = 0; i < num_attention_layers; i++) {
        // Get netData index of attention to connect (need this index to find its following components)
        int aindex = netData.getIndexFromID(attentions[i].id);
        if (aindex >= 0) {
            // If index is valid, get supporting connections
            vector<twoStrings> sconnections = netData.getImmediateSupportingConnections(aindex);
            if (sconnections.size() > 0) {
                // If there are supporting connections, find their match (by name) in the net structure
                vector<attentionConnection> toconnect;
                for (int j = 0; j < (int)sconnections.size(); j++) {
                    string output = netData.getConvNameFromConnectName(sconnections[j].s2);
                    if (output == attentions[i].id) {
                        
                        // Sort input connections
                        string input = netData.getConvNameFromConnectName(sconnections[j].s1);
                        int c = netData.getSecondNumberAfterColon(sconnections[j].s2);
                        bool found = false;
                        if (toconnect.size() > 0) {
                            for (int k = 0; k < (int)toconnect.size(); k++) {
                                if (input == toconnect[k].input) {
                                    if (c == 0) {
                                        toconnect[k].q = true;
                                    }
                                    else if (c == 1) {
                                        toconnect[k].k = true;
                                    }
                                    else if (c == 2) {
                                        toconnect[k].v = true;
                                    }
                                    found = true;
                                    k = (int)toconnect.size();
                                }
                            }
                        }
                        if (!found) {
                            attentionConnection ac;
                            ac.input = input;
                            if (c == 0) {
                                ac.q = true;
                            }
                            else if (c == 1) {
                                ac.k = true;
                            }
                            else if (c == 2) {
                                ac.v = true;
                            }
                            toconnect.push_back(ac);
                        }

                    }
                    else {
                        addErrorString("joinAttentionsToPrevious : problem with output connection for " + attentions[i].id);
                    }
                }

                // Process input connections
                if (toconnect.size() > 0) {
                    for (int j = 0; j < (int)toconnect.size(); j++) {

                        /*string s5 = toconnect[j].input;
                        if (toconnect[j].k) {
                            s5 += " : k";
                        }
                        if (toconnect[j].q) {
                            s5 += " : q";
                        }
                        if (toconnect[j].v) {
                            s5 += " : v";
                        }
                        addErrorString(s5);*/

                        bool found = false;
                        // Look for ID in hidden nets
                        for (int k = 0; k < num_hidden_nodes; k++) {
                            // If node id matches input
                            if (toconnect[j].input == hnodes[k].id) {
                                // If connected to k input
                                if (toconnect[j].k) {
                                    // Try to join to k input
                                    if (!attentions[i].joink(&hnodes[k])) {
                                        // If fails but also connected to q keep trying
                                        if (toconnect[j].q) {
                                            if (!attentions[i].joinq(&hnodes[k])) {
                                                // If fails but also connected to v keep trying
                                                if (toconnect[j].v) {
                                                    if (!attentions[i].joinv(&hnodes[k])) {
                                                        // Otherwise fail
                                                        addErrorString("Attention::joinv for " + attentions[i].id + " for hidden node " + hnodes[k].id + " unable to set input pointer");
                                                        return(false);
                                                    }
                                                }
                                                else {
                                                    // Otherwise fail
                                                    addErrorString("Attention::joinq for " + attentions[i].id + " for hidden node " + hnodes[k].id + " unable to set input pointer");
                                                    return(false);
                                                }
                                            }
                                        }
                                        else {
                                            // Otherwise fail
                                            addErrorString("Attention::joink for " + attentions[i].id + " for hidden node " + hnodes[k].id + " unable to set input pointer");
                                            return(false);
                                        }
                                    }
                                }
                                // Else if connected to q input
                                else if (toconnect[j].q) {
                                    if (!attentions[i].joinq(&hnodes[k])) {
                                        // If fails but also connected to v keep trying
                                        if (toconnect[j].v) {
                                            if (!attentions[i].joinv(&hnodes[k])) {
                                                // Otherwise fail
                                                addErrorString("Attention::joinv for " + attentions[i].id + " for hidden node " + hnodes[k].id + " unable to set input pointer");
                                                return(false);
                                            }
                                        }
                                        else {
                                            // Otherwise fail
                                            addErrorString("Attention::joinq for " + attentions[i].id + " for hidden node " + hnodes[k].id + " unable to set input pointer");
                                            return(false);
                                        }
                                    }
                                }
                                // Else if connected to v input
                                else if (toconnect[j].v) {
                                    if (!attentions[i].joinv(&hnodes[k])) {
                                        // Otherwise fail
                                        addErrorString("Attention::joinv for " + attentions[i].id + " for hidden node " + hnodes[k].id + " unable to set input pointer");
                                        return(false);
                                    }
                                }
                                k = num_hidden_nodes;
                                found = true;
                            }
                        }

                        // Look for ID in hidden nets
                        if (!found) {
                            for (int k = 0; k < num_hidden_nets; k++) {
                                // If net id matches input
                                if (toconnect[j].input == hnets[k].id) {
                                    // If connected to k input
                                    if (toconnect[j].k) {
                                        // Try to join to k input
                                        boolInt tval1 = attentions[i].joink(&hnets[k]);
                                        if (!tval1.valid) {
                                            // If fails but also connected to q keep trying
                                            if (toconnect[j].q) {
                                                boolInt tval2 = attentions[i].joinq(&hnets[k], tval1.value);
                                                if (!tval2.valid) {
                                                    // If fails but also connected to v keep trying
                                                    if (toconnect[j].v) {
                                                        //char s[100];  sprintf_s(s, "tp25 : i = %i : j = %i : k = %i", i, j, k); addErrorString(s);
                                                        boolInt tval3 = attentions[i].joinv(&hnets[k], tval2.value);
                                                        if (!tval3.valid) {
                                                            // Otherwise fail
                                                            addErrorString("Attention::joinv for " + attentions[i].id + " for hidden net " + hnets[k].id + " - join error 1");
                                                            return(false);
                                                        }
                                                        //addErrorString("attention " + attentions[i].id + " completing joinv with hidden net " + hnets[k].id);
                                                    }
                                                    else {
                                                        // Otherwise fail
                                                        addErrorString("Attention::joinq for " + attentions[i].id + " for hidden net " + hnets[k].id + " - join error 1");
                                                        return(false);
                                                    }
                                                }
                                            }
                                            else {
                                                // Otherwise fail
                                                addErrorString("Attention::joink for " + attentions[i].id + " for hidden net " + hnets[k].id + " - join error 1");
                                                return(false);
                                            }
                                        }
                                    }
                                    // Else if connected to q input
                                    else if (toconnect[j].q) {
                                        boolInt tval2 = attentions[i].joinq(&hnets[k], 0);
                                        if (!tval2.valid) {
                                            // If fails but also connected to v keep trying
                                            if (toconnect[j].v) {
                                                boolInt tval3 = attentions[i].joinv(&hnets[k], tval2.value);
                                                if (!tval3.valid) {
                                                    // Otherwise fail
                                                    addErrorString("Attention::joinv for " + attentions[i].id + " for hidden net " + hnets[k].id + " unable to set input pointer 2");
                                                    return(false);
                                                }
                                            }
                                            else {
                                                // Otherwise fail
                                                addErrorString("Attention::joinq for " + attentions[i].id + " for hidden net " + hnets[k].id + " unable to set input pointer 2");
                                                return(false);
                                            }
                                        }
                                    }
                                    // Else if connected to v input
                                    else if (toconnect[j].v) {
                                        boolInt tval3 = attentions[i].joinv(&hnets[k], 0);
                                        if (!tval3.valid) {
                                            // Otherwise fail
                                            addErrorString("Attention::joinv for " + attentions[i].id + " for hidden net " + hnets[k].id + " unable to set input pointer 3");
                                            return(false);
                                        }
                                    }
                                    k = num_hidden_nets;
                                    found = true;
                                }
                            }
                        }
                        
                        if (!found) {
                            // Look for ID in conv nets
                            for (int k = 0; k < num_active_convs; k++) {
                                if (toconnect[j].input == newConvs[k].id) {
                                    if (toconnect[j].k) {
                                        addErrorString("Attention::join for " + attentions[i].id + " for convolution " + newConvs[k].id + " unable to connect to k input");
                                        return(false);
                                    }
                                    else if (toconnect[j].q) {
                                        addErrorString("Attention::join for " + attentions[i].id + " for convolution " + newConvs[k].id + " unable to connect to q input");
                                        return(false);
                                    }
                                    else if (toconnect[j].v) {
                                        if (!attentions[i].joinv(&newConvs[k])) {
                                            return(false);
                                        }
                                    }
                                    k = num_active_convs;
                                    found = true;
                                }
                            }
                        }
                        
                        if (!found) {
                            // Look for ID in attentions
                            for (int k = 0; k < num_net_clusters; k++) {
                                if (toconnect[j].input == nclusters[k].id) {
                                    if (toconnect[j].v) {
                                        addErrorString("Attention::join for " + attentions[i].id + " for convolution " + newConvs[k].id + " unable to connect to v input");
                                        return(false);
                                    }
                                    else if (toconnect[j].k && toconnect[j].q) {
                                        addErrorString("Attention::join for " + attentions[i].id + " for convolution " + newConvs[k].id + " unable to connect to both k anf q inputs for cluster input");
                                        return(false);
                                    }
                                    else if (toconnect[j].k) {
                                        if (!attentions[i].joink(&nclusters[k])) {
                                            return(false);
                                        }
                                    }
                                    else if (toconnect[j].q) {
                                        if (!attentions[i].joinq(&nclusters[k])) {
                                            return(false);
                                        }
                                    }
                                    k = num_net_clusters;
                                    found = true;
                                }
                            }
                        }

                        if (!found) {
                            // Look for ID in dropouts
                            for (int k = 0; k < num_dropouts; k++) {
                                if (toconnect[j].input == dropouts[k].id) {
                                    if (!attentions[i].join(&dropouts[k])) {
                                        return(false);
                                    }
                                    k = num_dropouts;
                                    found = true;
                                }
                            }
                        }

                    }
                }

            }

        }
        else {
            verifyStrings.push_back("Connecting attentions : unable to find aindex for " + attentions[i].id);
            return(false);
        }

    }
    // Check for attentions that still need input pointers
    for (int i = 0; i < num_attention_layers; i++) {
        if (attentions[i].input_size == 0) {
            verifyStrings.push_back("attention " + attentions[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinSelfAttentionsToPrevious()
{
    // Connect self attentions to their supporting components
    for (int i = 0; i < num_self_attention_layers; i++) {
        // Get netData index of self attention to connect (need this index to find its following components)
        int sindex = netData.getIndexFromID(selfattentions[i].id);
        if (sindex >= 0) {

            // If index is valid, get the node's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(sindex);
            if (snodes.size() > 0) {
                // If there are node supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!selfattentions[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!selfattentions[i].addInputPointer(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        // In case of fft input
                        boolIntVectorStringDoublePointer ival = isInputProcessNode(snodes[j]);
                        if (ival.valid) {
                            if (ival.count > 0) {
                                for (int m = 0; m < ival.count; m++) {
                                    char s[100];  sprintf_s(s, ":%i", m);
                                    if (!selfattentions[i].addInputPointer(snodes[j] + s, ival.pointer + m)) {
                                        return(false);
                                    }
                                }
                                found = true;
                            }
                            else if (ival.inputs.size() > 0) {
                                for (int m = 0; m < (int)ival.inputs.size(); m++) {
                                    boolDoublePointer dval = getInputPointerFromInput(ival.inputs[m]);
                                    if (dval.valid) {
                                        if (!selfattentions[i].addInputPointer(ival.inputs[m], dval.input)) {
                                            return(false);
                                        }
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting self attentions : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get node's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(sindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!selfattentions[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!selfattentions[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in clusters
                        for (int k = 0; k < num_net_clusters; k++) {
                            if (sconvs[j] == nclusters[k].id) {
                                if (!selfattentions[i].join(&nclusters[k])) {
                                    return(false);
                                }
                                k = num_net_clusters;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in dropouts
                        for (int k = 0; k < num_dropouts; k++) {
                            if (sconvs[j] == dropouts[k].id) {
                                if (!selfattentions[i].join(&dropouts[k])) {
                                    return(false);
                                }
                                k = num_dropouts;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!selfattentions[i].addInputPointer(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!selfattentions[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!selfattentions[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting self attentions : unable to find supporting conv for " + selfattentions[i].id + " : " + sconvs[j]);
                        return(false);
                    }
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting self attentions : unable to find hindex for " + selfattentions[i].id);
            return(false);
        }

    }
    // Check for self attention that still need input pointers
    for (int i = 0; i < num_self_attention_layers; i++) {
        if (selfattentions[i].input_size == 0) {
            verifyStrings.push_back("self attention " + selfattentions[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinClustersToPrevious()
{
    // Connect cluster nets to their supporting components
    for (int i = 0; i < num_net_clusters; i++) {
        // Get netData index of cluster net to connect (need this index to find its following components)
        int cindex = netData.getIndexFromID(nclusters[i].id);
        if (cindex >= 0) {

            // If index is valid, get the cluster's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(cindex);
            if (snodes.size() > 0) {
                // If there are nodes supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!nclusters[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in additions
                        for (int k = 0; k < num_additions; k++) {
                            if (additions[k].id == snodes[j]) {
                                if (!nclusters[i].join(&additions[k])) {
                                    return(false);
                                }
                                k = num_additions;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in multiplications
                        for (int k = 0; k < num_multiplications; k++) {
                            if (multiplications[k].id == snodes[j]) {
                                if (!nclusters[i].join(&multiplications[k])) {
                                    return(false);
                                }
                                k = num_multiplications;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!nclusters[i].addInputPointer(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        // In case of fft input
                        boolIntVectorStringDoublePointer ival = isInputProcessNode(snodes[j]);
                        if (ival.valid) {
                            if (ival.count > 0) {
                                for (int m = 0; m < ival.count; m++) {
                                    char s[100];  sprintf_s(s, ":%i", m);
                                    if (!nclusters[i].addInputPointer(snodes[j] + s, ival.pointer + m)) {
                                        return(false);
                                    }
                                }
                                found = true;
                            }
                            else if (ival.inputs.size() > 0) {
                                for (int m = 0; m < (int)ival.inputs.size(); m++) {
                                    boolDoublePointer dval = getInputPointerFromInput(ival.inputs[m]);
                                    if (dval.valid) {
                                        if (!nclusters[i].addInputPointer(ival.inputs[m], dval.input)) {
                                            return(false);
                                        }
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting cluster nets : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get node's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(cindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!nclusters[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!nclusters[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in attentions
                        for (int k = 0; k < num_attention_layers; k++) {
                            if (sconvs[j] == attentions[k].id) {
                                if (!nclusters[i].join(&attentions[k])) {
                                    return(false);
                                }
                                k = num_attention_layers;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in dropouts
                        for (int k = 0; k < num_dropouts; k++) {
                            if (sconvs[j] == dropouts[k].id) {
                                if (!nclusters[i].join(&dropouts[k])) {
                                    return(false);
                                }
                                k = num_dropouts;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!nclusters[i].addInputPointer(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!nclusters[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!nclusters[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting cluster nets : unable to find supporting conv for " + nclusters[i].id + " : " + sconvs[j]);
                        return(false);
                    }
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting cluster nets : unable to find hindex for " + nclusters[i].id);
            return(false);
        }

    }
    // Check for cluster nets that still need input pointers
    for (int i = 0; i < num_net_clusters; i++) {
        if (nclusters[i].num_inputs == 0) {
            verifyStrings.push_back("cluster " + nclusters[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinDropoutsToPrevious()
{
    // Connect dropouts to their supporting components
    for (int i = 0; i < num_dropouts; i++) {
        // Get netData index of dropout to connect (need this index to find its following components)
        int dindex = netData.getIndexFromID(dropouts[i].id);
        if (dindex >= 0) {

            // If index is valid, get the drpout's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(dindex);
            if (snodes.size() > 0) {
                // If there are nodes supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!dropouts[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        if (netData.getNodeFunction(netData.getIndexFromID(snodes[j])) == "Add") {
                            if (num_additions > 0) {
                                for (int k = 0; k < num_additions; k++) {
                                    if (additions[k].id == snodes[j]) {
                                        if (!dropouts[i].join(&additions[k])) {
                                            return(false);
                                        }
                                        k = num_additions;
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!found) {
                        if (netData.getNodeFunction(netData.getIndexFromID(snodes[j])) == "Multiply") {
                            if (num_multiplications > 0) {
                                for (int k = 0; k < num_multiplications; k++) {
                                    if (multiplications[k].id == snodes[j]) {
                                        if (!dropouts[i].join(&multiplications[k])) {
                                            return(false);
                                        }
                                        k = num_multiplications;
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!dropouts[i].addInputPointer(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        // In case of fft input
                        boolIntVectorStringDoublePointer ival = isInputProcessNode(snodes[j]);
                        if (ival.valid) {
                            if (ival.count > 0) {
                                for (int m = 0; m < ival.count; m++) {
                                    char s[100];  sprintf_s(s, ":%i", m);
                                    if (!dropouts[i].addInputPointer(snodes[j] + s, ival.pointer + m)) {
                                        return(false);
                                    }
                                }
                                found = true;
                            }
                            else if (ival.inputs.size() > 0) {
                                for (int m = 0; m < (int)ival.inputs.size(); m++) {
                                    boolDoublePointer dval = getInputPointerFromInput(ival.inputs[m]);
                                    if (dval.valid) {
                                        if (!dropouts[i].addInputPointer(ival.inputs[m], dval.input)) {
                                            return(false);
                                        }
                                        found = true;
                                    }
                                }
                            }
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting dropouts : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get dropout's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(dindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!dropouts[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!dropouts[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in attentions
                        for (int k = 0; k < num_attention_layers; k++) {
                            if (sconvs[j] == attentions[k].id) {
                                if (!dropouts[i].join(&attentions[k])) {
                                    return(false);
                                }
                                k = num_attention_layers;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in clusters
                        for (int k = 0; k < num_net_clusters; k++) {
                            if (sconvs[j] == nclusters[k].id) {
                                if (!dropouts[i].join(&nclusters[k])) {
                                    return(false);
                                }
                                k = num_net_clusters;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!dropouts[i].addInputPointer(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!dropouts[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!dropouts[i].addInputPointer(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting dropouts : unable to find supporting conv for " + dropouts[i].id + " : " + sconvs[j]);
                        return(false);
                    }
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting dropouts : unable to find hindex for " + dropouts[i].id);
            return(false);
        }

    }
    // Check for hidden nets that still need input pointers
    for (int i = 0; i < num_dropouts; i++) {
        if (dropouts[i].num_inputs == 0) {
            verifyStrings.push_back("dropout " + dropouts[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinAdditionsToPrevious()
{
    // Connect additions to their supporting components
    for (int i = 0; i < num_additions; i++) {
        // Get netData index of addition to connect (need this index to find its following components)
        int aindex = netData.getIndexFromID(additions[i].id);
        if (aindex >= 0) {

            // If index is valid, get the addition's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(aindex);
            if (snodes.size() > 0) {
                // If there are nodes supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!additions[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!additions[i].addInputPointerForInputNode(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting additions : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get addition's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(aindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!additions[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!additions[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in attentions
                        for (int k = 0; k < num_attention_layers; k++) {
                            if (sconvs[j] == attentions[k].id) {
                                if (!additions[i].join(&attentions[k])) {
                                    return(false);
                                }
                                k = num_attention_layers;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in self attentions
                        for (int k = 0; k < num_self_attention_layers; k++) {
                            if (sconvs[j] == selfattentions[k].id) {
                                if (!additions[i].join(&selfattentions[k])) {
                                    return(false);
                                }
                                k = num_self_attention_layers;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in dropouts
                        for (int k = 0; k < num_dropouts; k++) {
                            if (sconvs[j] == dropouts[k].id) {
                                if (!additions[i].join(&dropouts[k])) {
                                    return(false);
                                }
                                k = num_dropouts;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in clusters
                        for (int k = 0; k < num_net_clusters; k++) {
                            if (sconvs[j] == nclusters[k].id) {
                                if (!additions[i].join(&nclusters[k])) {
                                    return(false);
                                }
                                k = num_net_clusters;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!additions[i].addInputPointerForInputNode(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!additions[i].addInputPointerForInputNode(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!additions[i].addInputPointerForInputNode(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting additions : unable to find supporting conv for " + additions[i].id + " : " + sconvs[j]);
                        return(false);
                    }
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting additions : unable to find hindex for " + additions[i].id);
            return(false);
        }

    }
    // Check for hidden nets that still need input pointers
    for (int i = 0; i < num_additions; i++) {
        if (additions[i].num_input_nodes == 0) {
            verifyStrings.push_back("addition " + additions[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::joinMultiplicationsToPrevious()
{
    // Connect multiplications to their supporting components
    for (int i = 0; i < num_multiplications; i++) {
        // Get netData index of multiplication to connect (need this index to find its following components)
        int aindex = netData.getIndexFromID(multiplications[i].id);
        if (aindex >= 0) {

            // If index is valid, get the multiplication's supporting nodes
            vector<string> snodes = netData.getImmediateSupportingNodes(aindex);
            if (snodes.size() > 0) {
                // If there are nodes supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)snodes.size(); j++) {
                    // First search for a hidden node match
                    bool found = false;
                    for (int k = 0; k < num_hidden_nodes; k++) {
                        if (hnodes[k].id == snodes[j]) {
                            if (!multiplications[i].join(&hnodes[k])) {
                                return(false);
                            }
                            k = num_hidden_nodes;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(snodes[j]);
                        if (dval.valid) {
                            if (!multiplications[i].addInputPointerForInputNode(snodes[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting multiplications : unable to find input connection for " + snodes[j]);
                        return(false);
                    }
                }
            }

            // Get multiplication's supporting other structures
            vector<string> sconvs = netData.getImmediateSupportingConvNets(aindex);
            if (sconvs.size() > 0) {
                // If there are convs supporting, find their match (by name) in the net structure
                for (int j = 0; j < (int)sconvs.size(); j++) {
                    bool found = false;
                    // Look for ID in hidden nets
                    for (int k = 0; k < num_hidden_nets; k++) {
                        if (sconvs[j] == hnets[k].id) {
                            if (!multiplications[i].join(&hnets[k])) {
                                return(false);
                            }
                            k = num_hidden_nets;
                            found = true;
                        }
                    }
                    if (!found) {
                        // Look for ID in conv nets
                        for (int k = 0; k < num_active_convs; k++) {
                            if (sconvs[j] == newConvs[k].id) {
                                if (!multiplications[i].join(&newConvs[k])) {
                                    return(false);
                                }
                                k = num_active_convs;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in attentions
                        for (int k = 0; k < num_attention_layers; k++) {
                            if (sconvs[j] == attentions[k].id) {
                                if (!multiplications[i].join(&attentions[k])) {
                                    return(false);
                                }
                                k = num_attention_layers;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for ID in clusters
                        for (int k = 0; k < num_net_clusters; k++) {
                            if (sconvs[j] == nclusters[k].id) {
                                if (!multiplications[i].join(&nclusters[k])) {
                                    return(false);
                                }
                                k = num_net_clusters;
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        // Look for possible net inputs here
                        boolDoublePointer dval = getInputPointerFromInput(sconvs[j]);
                        if (dval.valid) {
                            if (!multiplications[i].addInputPointerForInputNode(sconvs[j], dval.input)) {
                                return(false);
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getImageNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!multiplications[i].addInputPointerForInputNode(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        boolIntDoublePointer dval = getSoundNodePointerFromInputs(sconvs[j]);
                        if (dval.valid && (dval.count > 0)) {
                            for (int m = 0; m < dval.count; m++) {
                                char s[100];  sprintf_s(s, ":%i", m);
                                if (!multiplications[i].addInputPointerForInputNode(sconvs[j] + s, dval.input + m)) {
                                    return(false);
                                }
                            }
                            found = true;
                        }
                    }
                    if (!found) {
                        verifyStrings.push_back("Connecting multiplications : unable to find supporting conv for " + multiplications[i].id + " : " + sconvs[j]);
                        return(false);
                    }
                }
            }

        }
        else {
            verifyStrings.push_back("Connecting multiplications : unable to find hindex for " + multiplications[i].id);
            return(false);
        }

    }
    // Check for hidden nets that still need input pointers
    for (int i = 0; i < num_multiplications; i++) {
        if (multiplications[i].num_input_nodes == 0) {
            verifyStrings.push_back("multiplication " + multiplications[i].id + " has no input connections");
            return(false);
        }
    }
    return(true);
}

void NetBuilder::expandAssemblies()
{
    // Check if assembly layers exist
    vector<string> assembly_names = getAndSortAssemblies();
    
    // If no assemblies, return
    if (assembly_names.size() <= 0) {
        return;
    }

    char s[200];

    // First copy components from referenced assemblies to referencies
    for (int i = 0; i < (int)assembly_names.size(); i++) {
        int aindex = netData.getIndexFromID(assembly_names[i]);
        string reference = netData.getAssemblyReference(aindex);
        // For groups that reference fron other groups, convert components to new 
        //  unique ids and make sure connection input/outputs reference appropriately
        if (reference != "") {
            populateAssemblyWithReference(reference, assembly_names[i]);
        }
    }

    // Check assembly parameters
    for (int i = 0; i < (int)assembly_names.size(); i++) {

        // To save assembly items to be removed
        vector<string> assembly_items_to_remove;

        // Get indices and such for current assembly
        int aindex = netData.getIndexFromID(assembly_names[i]);
        int sindex = getAssemblyIndex(assembly_names[i]);
        int nsindex = getNoSubAssemblyIndex(assembly_names[i]);
        int gindex = netData.getGroupIndex(aindex);
        string reference = netData.getAssemblyReference(aindex);

        // Display some diagnostics
        //sprintf_s(s, " : nsindex = %i : gindex = %i", nsindex, gindex); addErrorString("expandAssemblies : " + assembly_names[i] + s + reference);

        vector<string> inputs = getNumAssemblyInputs(nsindex);
        vector<string> outputs = getNumAssemblyOutputs(nsindex);

        // Display some more diagnostics
        //sprintf_s(s, " : ninputs = %i : noutputs = %i", (int)inputs.size(), (int)outputs.size()); addErrorString("expandAssemblies : " + assembly_names[i] + s);

        vector<assembly_input_connections> myinputconnections = getAssemblyInputConnections(assembly_names[i], inputs);
        if (myinputconnections.size() != inputs.size()) {
            sprintf_s(s, " : ninputs (%i) != myinputconnections size (%i)", (int)inputs.size(), (int)myinputconnections.size()); addErrorString("expandAssemblies : " + assembly_names[i] + s);
            return;
        }

        vector<assembly_output_connections> myoutputconnections = getAssemblyOutputConnections(assembly_names[i], outputs);
        if (myoutputconnections.size() != outputs.size()) {
            sprintf_s(s, " : noutputs (%i) != myoutputconnections size (%i)", (int)outputs.size(), (int)myoutputconnections.size()); addErrorString("expandAssemblies : " + assembly_names[i] + s);
            return;
        }

        for (int j = 0; j < (int)myinputconnections.size(); j++) {

            // Add connections for later deletion
            for (int k = 0; k < (int)myinputconnections[j].old_connection_names.size(); k++) {
                assembly_items_to_remove.push_back(myinputconnections[j].old_connection_names[k]);
            }

            // Add assembly input for later deletion
            assembly_items_to_remove.push_back(inputs[j]);

            // Add connections to assembly internal components
            for (int k = 0; k < (int)myinputconnections[j].new_input_node_names.size(); k++) {
                for (int m = 0; m < (int)myinputconnections[j].new_output_node_names.size(); m++) {
                    sprintf_s(s, "Connect%i", getLastConnectIndex() + 1);
                    string mid = s;
                    vector<double> sweights;
                    vector<twoValues> mpoints;
                    netData.pushConnectData(mid, mid, myinputconnections[j].new_input_node_names[k], myinputconnections[j].new_output_node_names[m], gindex, 0.0, sweights, mpoints);
                    //addErrorString("expandAssemblies adding input connection from " + myinputconnections[j].new_input_node_names[k] + " to " + myinputconnections[j].new_output_node_names[m]);
                }
            }

        }

        for (int j = 0; j < (int)myoutputconnections.size(); j++) {

            // Add assembly connections for later deletion
            for (int k = 0; k < (int)myoutputconnections[j].old_connection_names.size(); k++) {
                assembly_items_to_remove.push_back(myoutputconnections[j].old_connection_names[k]);
            }

            // Add assembly output for later eletion
            assembly_items_to_remove.push_back(outputs[j]);

            // Add connections to assembly internal components
            for (int k = 0; k < (int)myoutputconnections[j].new_output_node_names.size(); k++) {
                for (int m = 0; m < (int)myoutputconnections[j].new_input_node_names.size(); m++) {
                    sprintf_s(s, "Connect%i", getLastConnectIndex() + 1);
                    string mid = s;
                    vector<double> sweights;
                    vector<twoValues> mpoints;
                    netData.pushConnectData(mid, mid, myoutputconnections[j].new_input_node_names[m], myoutputconnections[j].new_output_node_names[k], gindex, 0.0, sweights, mpoints);
                    //addErrorString("expandAssemblies adding output connection from " + myoutputconnections[j].new_input_node_names[m] + " to " + myoutputconnections[j].new_output_node_names[k]);
                }
            }

        }

        // Add assembly for later deletion
        assembly_items_to_remove.push_back(assembly_names[i]);

        // Remove assembly related items
        for (int i = 0; i < (int)assembly_items_to_remove.size(); i++) {
            // Remove component 
            //addErrorString("expandAssemblies removing item " + assembly_items_to_remove[i]);
            netData.removeItem(assembly_items_to_remove[i]);
        }

        //addErrorString("");

    }

}

vector<string> NetBuilder::getAndSortAssemblies()
{
    vector<bool> completed;
    vector<int> temp_indices;
    for (int i = 0; i < (int)netData.size(); i++) {
        if (netData.isAssemblyLayer(i)) {
            temp_indices.push_back(i);
            completed.push_back(false);
        }
    }

    int cnt = 0;
    vector<string> assembly_names;
    if (temp_indices.size() > 0) {
        while ((assembly_names.size() != temp_indices.size()) && (cnt < 200)) {
            for (int i = 0; i < (int)temp_indices.size(); i++) {
                if (!completed[i]) {
                    bool has_unused_sub_assemblies = false;
                    int sindex = getAssemblyIndex(netData.getID(temp_indices[i]));
                    for (int j = 0; j < (int)temp_indices.size(); j++) {
                        if ((i != j) && !completed[j]) {
                            int sgindex = netData.getGroupIndex(temp_indices[j]);
                            if (sgindex == sindex) {
                                has_unused_sub_assemblies = true;
                                j = (int)temp_indices.size();
                            }
                        }
                    }
                    if (!has_unused_sub_assemblies) {
                        assembly_names.push_back(netData.getID(temp_indices[i]));
                        completed[i] = true;
                    }
                }
            }
        }
    }

    if (cnt >= 200) {
        addErrorString("getAndSortAssemblies : problem with indices not aligning");
    }

    /*for (int i = 0; i < (int)assembly_names.size(); i++) {
        char s[200];  sprintf_s(s, " : i = %i", i);
        addErrorString("ordered assemblies : " + assembly_names[i] + s);
    }*/

    return(assembly_names);
}

void NetBuilder::populateAssemblyWithReference(string fromname, string toname)
{
    vector<netDataStruct> copyFromAssemblyComponents;
    vector<netDataStruct> copyFromAssemblyConnections;

    int tgindex = getNoSubAssemblyIndex(toname);
    int fgindex = getNoSubAssemblyIndex(fromname);

    char s[200];  //sprintf_s(s, "populateAssemblyWithReference : fgindex = %i for ", fgindex);  addErrorString(s + fromname);

    for (int i = 0; i < netData.size(); i++) {
        //sprintf_s(s, " : gindex = %i", netData.getGroupIndex(i)); addErrorString("populateAssemblyWithReference : from " + fromname + " : checking " + netData.getID(i) + s);
        if (netData.getGroupIndex(i) == fgindex) {
            if (netData.isConnect(i)) {
                //addErrorString("populateAssemblyWithReference : from " + fromname + " to " + toname + " : adding copy connection " + netData.getID(i));
                copyFromAssemblyConnections.push_back(netData.getNode(i));
            }
            else {
                //addErrorString("populateAssemblyWithReference : from " + fromname + " to " + toname + " : adding copy component " + netData.getID(i));
                copyFromAssemblyComponents.push_back(netData.getNode(i));
            }
        }
    }

    for (int i = 0; i < (int)copyFromAssemblyComponents.size(); i++) {
        
        string mid = "";
        string tid = copyFromAssemblyComponents[i].getID();
        if (copyFromAssemblyComponents[i].isNode()) {
            sprintf_s(s, "Node%i", getLastNodeIndex() + 1); mid = s;
            copyFromAssemblyComponents[i].setID(mid);
            copyFromAssemblyComponents[i].setGroupIndex(tgindex);
            //addErrorString("populateAssemblyWithReference : adding new node " + mid);
            netData.pushNodeData(copyFromAssemblyComponents[i]);
        }
        else if (copyFromAssemblyComponents[i].isConvLayer()) {
            sprintf_s(s, "Layer%i", getLastConvIndex() + 1); mid = s;
            copyFromAssemblyComponents[i].setID(mid);
            copyFromAssemblyComponents[i].setGroupIndex(tgindex);
            //addErrorString("populateAssemblyWithReference : adding new layer " + mid);
            netData.pushConvLayer(copyFromAssemblyComponents[i]);
        }
        else if (copyFromAssemblyComponents[i].isAttentionLayer()) {
            sprintf_s(s, "Attention%i", getLastAttentionIndex() + 1); mid = s;
            copyFromAssemblyComponents[i].setID(mid);
            copyFromAssemblyComponents[i].setGroupIndex(tgindex);
            //addErrorString("populateAssemblyWithReference : adding new attention " + mid);
            netData.pushAttentionLayer(copyFromAssemblyComponents[i]);
        }
        else if (copyFromAssemblyComponents[i].isSelfAttentionLayer()) {
            sprintf_s(s, "Attention%i", getLastAttentionIndex() + 1); mid = s;
            copyFromAssemblyComponents[i].setID(mid);
            copyFromAssemblyComponents[i].setGroupIndex(tgindex);
            //addErrorString("populateAssemblyWithReference : adding new self attention " + mid);
            netData.pushAttentionLayer(copyFromAssemblyComponents[i]);
        }
        else if (copyFromAssemblyComponents[i].isNetCluster()) {
            sprintf_s(s, "NetCluster%i", getLastNetClusterIndex() + 1); mid = s;
            copyFromAssemblyComponents[i].setID(mid);
            copyFromAssemblyComponents[i].setGroupIndex(tgindex);
            //addErrorString("populateAssemblyWithReference : adding new cluster " + mid);
            netData.pushNetCluster(copyFromAssemblyComponents[i]);
        }
        else if (copyFromAssemblyComponents[i].isDropout()) {
            sprintf_s(s, "Dropout%i", getLastDropoutIndex() + 1); mid = s;
            copyFromAssemblyComponents[i].setID(mid);
            copyFromAssemblyComponents[i].setGroupIndex(tgindex);
            //addErrorString("populateAssemblyWithReference : adding new dropout " + mid);
            netData.pushDropout(copyFromAssemblyComponents[i]);
        }
        else {
            addErrorString("populateAssemblyWithReference : unable to resolve component type for " + tid);
            return;
        }

        for (int j = 0; j < (int)copyFromAssemblyConnections.size(); j++) {
            if (copyFromAssemblyConnections[j].getInputNode() == tid) {
                //addErrorString("populateAssemblyWithReference : updating input to " + copyFromAssemblyConnections[j].getID() + " to " + mid);
                copyFromAssemblyConnections[j].setInputNode(mid);
            }
            else if (copyFromAssemblyConnections[j].getOutputNode() == tid) {
                //addErrorString("populateAssemblyWithReference : updating output to " + copyFromAssemblyConnections[j].getID() + " to " + mid);
                copyFromAssemblyConnections[j].setOutputNode(mid);
            }
            else if (netData.getConvNameFromConnectName(copyFromAssemblyConnections[j].getInputNode()) == tid) {
                string nid = mid + ":" + netData.getStringAfterColon(copyFromAssemblyConnections[j].getInputNode());
                //addErrorString("populateAssemblyWithReference : updating input to " + copyFromAssemblyConnections[j].getID() + " to " + nid);
                copyFromAssemblyConnections[j].setInputNode(nid);
            }
            else if (netData.getConvNameFromConnectName(copyFromAssemblyConnections[j].getOutputNode()) == tid) {
                string nid = mid + ":" + netData.getStringAfterColon(copyFromAssemblyConnections[j].getOutputNode());
                //addErrorString("populateAssemblyWithReference : updating output to " + copyFromAssemblyConnections[j].getID() + " to " + nid);
                copyFromAssemblyConnections[j].setOutputNode(nid);
            }
        }

    }

    for (int j = 0; j < (int)copyFromAssemblyConnections.size(); j++) {
        sprintf_s(s, "Connect%i", getLastConnectIndex() + 1);
        string mid = s;
        vector<double> sweights;
        vector<twoValues> mpoints;
        //addErrorString("populateAssemblyWithReference adding connection from " + copyFromAssemblyConnections[j].getInputNode() + " to " + copyFromAssemblyConnections[j].getOutputNode());
        netData.pushConnectData(mid, mid, copyFromAssemblyConnections[j].getInputNode(), copyFromAssemblyConnections[j].getOutputNode(), tgindex, 0.0, sweights, mpoints);
    }

}

vector<string> NetBuilder::getNumAssemblyInputs(int aindex)
{
    vector<string> rval;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.getGroupIndex(i) == aindex) {
            if (netData.isNode(i) && (netData.getNodeMode(i) == "Input")) {
                rval.push_back(netData.getID(i));
            }
        }
    }
    return(rval);
}

vector<string> NetBuilder::getNumAssemblyOutputs(int aindex)
{
    vector<string> rval;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.getGroupIndex(i) == aindex) {
            if (netData.isNode(i) && (netData.getNodeMode(i) == "Output")) {
                rval.push_back(netData.getID(i));
            }
        }
    }
    return(rval);
}

vector<assembly_input_connections> NetBuilder::getAssemblyInputConnections(string aname, vector<string> inputs)
{
    vector<assembly_input_connections> rval;
    if (inputs.size() > 0) {
        char s[100];
        for (int i = 0; i < (int)inputs.size(); i++) {
            assembly_input_connections myconnects;
            myconnects.input_node_name = inputs[i];
            // need to use 'o' for connect output 
            sprintf_s(s, ":o:%i", i);
            string oname = aname + s;
            for (int j = 0; j < netData.size(); j++) {
                if (netData.isConnect(j)) {
                    //addErrorString("getAssemblyInputConnections : testing " + netData.getOutputNode(j) + " against " + cname);
                    if (netData.getOutputNode(j) == oname) {
                        myconnects.old_connection_names.push_back(netData.getID(j));
                        myconnects.new_input_node_names.push_back(netData.getInputNode(j));
                    }
                    else if (netData.getInputNode(j) == inputs[i]) {
                        myconnects.old_connection_names.push_back(netData.getID(j));
                        myconnects.new_output_node_names.push_back(netData.getOutputNode(j));
                    }
                }
            }
            rval.push_back(myconnects);
        }
    }
    return(rval);
}

vector<assembly_output_connections> NetBuilder::getAssemblyOutputConnections(string aname, vector<string> outputs)
{
    vector<assembly_output_connections> rval;
    if (outputs.size() > 0) {
        char s[100];
        for (int i = 0; i < (int)outputs.size(); i++) {
            assembly_output_connections myconnects;
            myconnects.output_node_name = outputs[i];
            // need to use 'i' for connect input 
            sprintf_s(s, ":i:%i", i);
            string cname = aname + s;
            for (int j = 0; j < netData.size(); j++) {
                if (netData.isConnect(j)) {
                    if (netData.getInputNode(j) == cname) {
                        myconnects.old_connection_names.push_back(netData.getID(j));
                        myconnects.new_output_node_names.push_back(netData.getOutputNode(j));
                    }
                    else if (netData.getOutputNode(j) == outputs[i]) {
                        myconnects.old_connection_names.push_back(netData.getID(j));
                        myconnects.new_input_node_names.push_back(netData.getInputNode(j));
                    }
                }
            }
            rval.push_back(myconnects);
        }
    }
    return(rval);
}

int NetBuilder::getLastNodeIndex()
{
    int rval = 0;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.isNode(i)) {
            string lbl = netData.getID(i);
            string num = lbl.substr(4);
            //addErrorString("getLastNodeIndex : lbl = " + lbl + " : num = " + num);
            int tval = 0;
            if (isWordNumber(num)) {
                tval = myStoi(num);
            }
            if (tval > rval) {
                rval = tval;
            }
        }
    }
    return(rval);
}

int NetBuilder::getLastConvIndex()
{
    int rval = 0;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.isConvLayer(i)) {
            int tval = 0;
            string lbl = netData.getID(i);
            if (lbl.substr(0, 5) == "Layer") {
                string num = lbl.substr(5);
                //addErrorString("getLastConvIndex : lbl = " + lbl + " : num = " + num);
                if (isWordNumber(num)) {
                    tval = myStoi(num);
                }
            }
            else if (lbl.substr(0, 9) == "ConvLayer") {
                string num = lbl.substr(9);
                //addErrorString("getLastConvIndex : lbl = " + lbl + " : num = " + num);
                if (isWordNumber(num)) {
                    tval = myStoi(num);
                }
            }
            else {
                addErrorString("getLastConvIndex unable to resolve layer type");
            }
            if (tval > rval) {
                rval = tval;
            }
        }
    }
    return(rval);
}

int NetBuilder::getLastAttentionIndex()
{
    int rval = 0;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.isAttentionLayer(i)) {
            int tval = 0;
            string lbl = netData.getID(i);
            if (lbl.substr(0, 9) == "Attention") {
                string num = lbl.substr(9);
                //addErrorString("getLastAttentionIndex : lbl = " + lbl + " : num = " + num);
                if (isWordNumber(num)) {
                    tval = myStoi(num);
                }
            }
            else {
                addErrorString("getLastAttentionIndex unable to resolve layer type");
            }
            if (tval > rval) {
                rval = tval;
            }
        }
    }
    return(rval);
}

int NetBuilder::getLastNetClusterIndex()
{
    int rval = 0;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.isNetCluster(i)) {
            int tval = 0;
            string lbl = netData.getID(i);
            if (lbl.substr(0, 10) == "NetCluster") {
                string num = lbl.substr(10);
                //addErrorString("getLastNetClusterIndex : lbl = " + lbl + " : num = " + num);
                if (isWordNumber(num)) {
                    tval = myStoi(num);
                }
            }
            else {
                addErrorString("getLastNetClusterIndex unable to resolve layer type");
            }
            if (tval > rval) {
                rval = tval;
            }
        }
    }
    return(rval);
}

int NetBuilder::getLastDropoutIndex()
{
    int rval = 0;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.isDropout(i)) {
            int tval = 0;
            string lbl = netData.getID(i);
            if (lbl.substr(0, 7) == "Dropout") {
                string num = lbl.substr(7);
                //addErrorString("getLastDropoutIndex : lbl = " + lbl + " : num = " + num);
                if (isWordNumber(num)) {
                    tval = myStoi(num);
                }
            }
            else {
                addErrorString("getLastDropoutIndex unable to resolve layer type");
            }
            if (tval > rval) {
                rval = tval;
            }
        }
    }
    return(rval);
}

int NetBuilder::getLastConnectIndex()
{
    int rval = 0;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.isConnect(i)) {
            string lbl = netData.getID(i);
            string num = lbl.substr(7);
            //addErrorString("getLastConnectIndex : lbl = " + lbl + " : num = " + num);
            int tval = 0;
            if (isWordNumber(num)) {
                tval = myStoi(num);
            }
            if (tval > rval) {
                rval = tval;
            }
        }
    }
    return(rval);
}

int NetBuilder::getAssemblyIndex(string aname)
{
    int rval = -1;
    int nindex = netData.getIndexFromID(aname);
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
        rval = getNoSubAssemblyIndex(netData.getID(mindex));
    }
    return(rval);
}

int NetBuilder::getNoSubAssemblyIndex(string aname)
{
    int rval = -1;
    if (aname.substr(0, 8) == "Assembly") {
        string t3 = aname.substr(8);
        if (isWordNumber(t3)) {
            rval = myStoi(t3);
        }
    }
    return(rval);
}

void NetBuilder::getNetData()
{
    netDataInitializations();

    getNetConnections();

#ifndef GCC
    CStatus myProgressDlg;
    myProgressDlg.CreateDlg();
    myProgressDlg.setProgress(0);
    myProgressDlg.setLabel("Getting layer inputs . . .");
#endif
    addPanel1String("Getting layer inputs");

    convInputs = getConvInputs();

#ifndef GCC
    myProgressDlg.setProgress(10);
    myProgressDlg.setLabel("Getting nodes . . .");
#endif
    addPanel1String("Getting nodes");

    nodeStructure mval = getNodeIndices();
    nodeIndices = mval.indices;
    nodeIDs = mval.ids;
    hiddenNodeIndices = hiddenNodeIndices;
    outputNodeIndices = outputNodeIndices;
    convertHiddenToAll = convertHiddenToAll;
    convertOutputToAll = convertOutputToAll;

    addPanel1String("Getting node inputs");

    if (nodeIndices.size() > 0) {
        int dupdate = (int)nodeIndices.size() / 200;
        if (dupdate < 1) {
            dupdate = 1;
        }
#ifndef GCC
        int dcnt = 0;
        myProgressDlg.setProgress(20);
        myProgressDlg.setLabel("Getting node inputs . . .");
#endif
        // Get inputs for each element
        for (int i = 0; i < (int)nodeIndices.size(); i++) {
            vector<string> inputs1 = getInputsFromNetData(nodeIndices[i]);
            nodeInputs.push_back(inputs1);
#ifndef GCC
            if (dcnt > dupdate) {
                char s[200];  sprintf_s(s, "Getting node inputs (%i:%i)", i, (int)nodeIndices.size());    addPanel1String(s);
                myProgressDlg.setProgress(20 + (60 * i) / (int)nodeIndices.size());
                myProgressDlg.setLabel("Getting node inputs . . .");
                dcnt = 0;
            }
            dcnt++;
#endif
        }
        if (nodeInputs.size() != nodeIndices.size()) {
            addErrorString("Problem getting node inputs from net");
        }
    }

#ifndef GCC
    myProgressDlg.setProgress(80);
    myProgressDlg.setLabel("Getting layers . . .");
#endif
    addPanel1String("Getting layers");

    netConvs = getConvs();

#ifndef GCC
    myProgressDlg.DestroyDlg();
    addPanel1String("");
#endif

}

void NetBuilder::getConvInputSize()
{
    conv_frame_size = 1;
    conv_input_size = 1;
    index_conv = false;
    sound_conv_only = false;
    processed_data_conv = false;
    int cindex = -1;
    for (int i = 0; i < netData.size(); i++) {
        if (netData.isConvLayer(i)) {
            if ((netData.getNodeMode(i) != "Input") && (netData.getConvType(i) == DATA_CONV)) {
                if (cindex >= 0) {
                    addErrorString("getConvInputSize : unable to process multiple convolutions");
                }
                else {
                    cindex = i;
                }
            }
        }
    }
    if (cindex >= 0) {
        CAuxFunctions func;
        if (has_series_conv && !has_wave_conv) {
            if (has_wave_fft_conv) {
                int mxin1 = func.getConvInputX(netData.getID(cindex));
                int mxin2 = func.getInputWidthFromLayerName(netData.getID(cindex));
                int stridey = netData.getConvOutputStrideY(cindex);
                conv_frame_size = stridey;
                conv_input_size = mxin2 * stridey;
                //CString s = _T(""); s.Format(_T("has_wave_fft_conv : mxin1 = %i  : mxin2 = %i : stridey = %i : conv_frame_size = %i : "), mxin1, mxin2, stridey, conv_frame_size); AfxMessageBox(s + (CString)netData.getID(cindex).c_str());
            }
            else {
                int mxin1 = func.getConvInputX(netData.getID(cindex));
                int mxin2 = func.getInputWidthFromLayerName(netData.getID(cindex));
                int stridey = netData.getConvOutputStrideY(cindex);
                conv_frame_size = stridey;
                conv_input_size = mxin2 * stridey;
                //CString s = _T(""); s.Format(_T("not has_wave_fft_conv : mxin1 = %i  : mxin2 = %i : stridey = %i : conv_frame_size = %i : "), mxin1, mxin2, stridey, conv_frame_size); AfxMessageBox(s + (CString)netData.getID(cindex).c_str());
            }
            processed_data_conv = true;
            index_conv = true;
        }
        if (has_wave_conv && !has_wave_fft_conv) {
            int mxin1 = func.getConvInputX(netData.getID(cindex));
            int mxin2 = func.getInputWidthFromLayerName(netData.getID(cindex));
            int stridey = netData.getConvOutputStrideY(cindex);
            conv_frame_size = stridey;
            conv_input_size = mxin2 * stridey;
            sound_conv_only = true;
            index_conv = true;
            //CString s = _T(""); s.Format(_T("has_wave_conv : mxin1 = %i  : mxin1 = %i : stridey = %i : conv_frame_size = %i : "), mxin1, mxin2, stridey, conv_frame_size); AfxMessageBox(s + (CString)netData.getID(cindex).c_str());
        }
    }
}

bool NetBuilder::setupConvolutionToImageOutputLayerBackpropPointers(int index)
{
    // **** Don't delete ****
    // Set backprop pointers to backprop_data for image output layer (images are copied to backprop_data each iteration)
    int num_conv_outputs = newConvs[loopControl[index].hidden_conv_index].output_data_size;
    if (num_conv_outputs <= image_backprop_size) {
        getConvImageMemorySize(newConvs[loopControl[index].hidden_conv_index].id);
        for (int k = 0; k < num_conv_outputs; k++) {
            double* pointer = image_data /*image_backprop_data*/ + k;
            newConvs[loopControl[index].hidden_conv_index].setBackpropInputPointer(k, pointer);
        }
    }
    else {
        char s[200];  sprintf_s(s, " : num_conv_outputs = %i : backprop_size = %i", num_conv_outputs, image_backprop_size);
        verifyStrings.push_back("Unable to copy backprop pointers for " + newConvs[loopControl[index].hidden_conv_index].id + s);
        return(false);
    }
    return(true);
}

int NetBuilder::addNextHiddenNodes(int* hnode_index, vector<int>* hnode_to_net_indices, bool randomize)
{
    int added = 0;
    // Look through all non-output nodes (should be only hidden nodes)
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        if (!netData.getIsOutput(nodeIndices[i]) && (netData.getNodeFunction(nodeIndices[i]) != "Add") && (netData.getNodeFunction(nodeIndices[i]) != "Multiply")) {
            // if the nodes haven't already been added
            if (!nodes_used[i]) {
                // Check to see if input conditions are satisfied before adding node to net
                if (areInputsCompleted(i)) {
                    // Add hidden node
                    addPanel1String("Adding node " + netData.getID(nodeIndices[i]));
                    if (addHiddenNode(i, hnode_index[0], randomize)) {
                        // Save node index and finish up
                        hnode_to_net_indices[0].push_back(i);
                        nodes_used[i] = true;
                        hnode_index[0]++;
                        added++;
                    }
                    else {
                        // If our count is off, error out
                        verifyStrings.push_back("Error : NetBuilder::initialize : error in addHiddenNode");
                        return(false);
                    }
                }
            }
        }
    }
    if (added > 0) {
        addLoopControlData("None", *hnode_index, false, false, false, false, false, false, false);
    }
    return(added);
}

int NetBuilder::addNextConvs2(int ctype, bool randomize)
{
    int added = 0;

    // This is a double check to makesure we aren't trying to set up more convolutions than we think we have
    if (nactive_index < num_active_convs) {

        // Look through all convolutions to see if any are ready to add to net 
        for (int i = 0; i < (int)convs.size(); i++) {
            // Input convs aren't active and so we're not processing them here
            if ((convs[i].mode != "Input") && (convs[i].data_type != HIDDEN_NET) && (convs[i].data_type != ATTENTION) && 
                (convs[i].data_type != SELFATTENTION) && (convs[i].data_type != NETCLUSTER) && (convs[i].data_type != DROPOUT)) {
                // if the conv hasn't already been added
                if (!convs[i].used) {
                    // Check to see if input convolutions are completed
                    bool completed = true;
                    for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                        for (int k = 0; k < (int)convs.size(); k++) {
                            if ((convs[i].supportingConvIDs[j] == convs[k].id) && (convs[i].id != convs[k].id)) {
                                if (!convs[k].used && (convs[k].mode != "Input")) {
                                    completed = false;
                                    j = (int)convs[i].supportingConvIDs.size();
                                    k = (int)convs.size();
                                }
                            }
                        }
                    }

                    if (completed) {
                        // Check to see if input nodes are completed
                        completed = areConvInputsCompleted(i);

                        if (completed) {

                            addPanel1String("Adding layer " + convs[i].id);

                            // Do some error checking
                            if (convs[i].numxsteps < 1) {
                                char s[200];  sprintf_s(s, " : numxsteps = %i", convs[i].numxsteps);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].numysteps < 1) {
                                char s[200];  sprintf_s(s, " : numysteps = %i", convs[i].numysteps);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].nxin < 1) {
                                char s[200];  sprintf_s(s, " : nxin = %i", convs[i].nxin);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].nyin < 1) {
                                char s[200];  sprintf_s(s, " : nyin = %i", convs[i].nyin);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].nxout < 1) {
                                char s[200];  sprintf_s(s, " : nxout = %i", convs[i].nxout);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].nyout < 1) {
                                char s[200];  sprintf_s(s, " : nyout = %i", convs[i].nyout);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].nxstride < 1) {
                                char s[200];  sprintf_s(s, " : nxstride = %i", convs[i].nxstride);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].nystride < 1) {
                                char s[200];  sprintf_s(s, " : nystride = %i", convs[i].nystride);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].input_width < 1) {
                                char s[200];  sprintf_s(s, " : input_width = %i", convs[i].input_width);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if (convs[i].input_height < 1) {
                                char s[200];  sprintf_s(s, " : input_height = %i", convs[i].input_height);
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + s);
                                return(-1);
                            }
                            else if ((convs[i].supportingConvIDs.size() > 0) && (convs[i].inputNodeIDs.size() > 0)) {
                                vector<string> tnodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(convs[i].id));
                                if ((tnodes.size() > 0) && (convs[i].supportingConvIDs.size() > 0)) {
                                    addErrorString("addNextConvs2 : Unable to process both node and conv inputs for " + convs[i].id);
                                    return(-1);
                                }
                            }
                            else if ((convs[i].supportingConvIDs.size() == 1) && (convs[i].data_type != IMAGE_CONV)) {
                                for (int k = 0; k < (int)convs.size(); k++) {
                                    if (convs[i].supportingConvIDs[0] == convs[k].id) {
                                        if (convs[k].data_type == HIDDEN_NET) {
                                            addErrorString("Unable to process net input for convolution " + convs[i].id);
                                            return(-1);
                                        }
                                    }
                                }
                            }
                            else if (convs[i].supportingConvIDs.size() > 1) {
                                addErrorString("addNextConvs2 : Unable to process multiple conv inputs for " + convs[i].id + " (1)");
                                return(-1);
                            }

                            // Set up convolution configurations for initialization
                            string nid = convs[i].id;
                            string nmode = convs[i].mode;
                            int nlayers = convs[i].layers;
                            if (nlayers < 1) {
                                nlayers = 1;
                            }
                            int nnodesperlayer = convs[i].nodes;
                            if (nnodesperlayer < 1) {
                                nnodesperlayer = 12;
                            }
                            int ntype = SIGMOID;
                            if (convs[i].function == "Sigmoid") {
                                ntype = SIGMOID;
                            }
                            else if (convs[i].function == "Relu") {
                                ntype = RELU;
                            }
                            else if (convs[i].function == "Tanh") {
                                ntype = TANH;
                            }
                            else if (convs[i].function == "Gaussian") {
                                ntype = GAUSSIAN;
                            }
                            else if (convs[i].function == "Shockley") {
                                ntype = SHOCKLEY;
                            }
                            else if (convs[i].function == "MaxPool") {
                                ntype = MAXPOOL;
                            }
                            else {
                                addErrorString("addNextConvs2 : unable to process function " + convs[i].function);
                            }
                            double neta = convs[i].eta;
                            double nalpha = convs[i].alpha;
                            double nrandom_scale = convs[i].rand;
                            int ndata_type = convs[i].data_type;
                            int mctype = ctype;
                            bool trainable = convs[i].trainable;
                            bool zerooutput = netData.getNodeZeroOutput(netData.getIndexFromID(nid));

                            vector<string> snodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(nid));
                            vector<string> sconvs = netData.getImmediateSupportingConvNets(nid);

                            vector<string> fnodes = netData.getImmediateFollowingNodes(netData.getIndexFromID(nid));
                            vector<string> fconvs = netData.getImmediateFollowingConvNets(nid);

                            int noutconnections = 0;
                            if (fnodes.size() > 0) {
                                noutconnections += (int)fnodes.size();
                            }
                            if (fconvs.size() > 0) {
                                for (int k = 0; k < (int)fconvs.size(); k++) {
                                    int cindex = netData.getIndexFromID(fconvs[k]);
                                    if (netData.isConvLayerOnly(cindex)) {
                                        noutconnections++;
                                    }
                                    else if (netData.isHiddenNetOnly(cindex)) {
                                        noutconnections += netData.getConvNodesPerLayer(cindex);
                                    }
                                    else if (netData.isAttentionLayerOnly(cindex)) {
                                        noutconnections += netData.getDv(cindex) * netData.getD(cindex);
                                    }
                                    else if (netData.isSelfAttentionLayerOnly(cindex)) {
                                        int tindex = getConvIndexFromName(fconvs[k]);
                                        if (tindex >= 0) {
                                            int dim = getSelfAttentionOutputSize(getNumInputsFromConvs(tindex));
                                            noutconnections += (dim * dim);
                                        }
                                    }
                                    else if (netData.isNetClusterOnly(cindex)) {
                                        noutconnections += netData.getConvNodesPerLayer(cindex);
                                    }
                                    else if (netData.isDropout(cindex)) {
                                        noutconnections++;
                                    }
                                    else {
                                        addErrorString("addNextConvs2 : unable to get num inputs for " + fconvs[k]);
                                    }
                                }
                            }
                            if (noutconnections <= 0) {
                                noutconnections = 1;
                            }

                            // Allocate and assign conv sizes and strides
                            int nnumdims = 3;
                            int* nconvins = NULL;
                            int* nconvouts = NULL;
                            int* nconvstrideins = NULL;
                            int* ninputdims = NULL;
                            if (convs[i].data_type == DATA_CONV) {      // Either data or image onvolutions are currently allowed
                                int mynyin = convs[i].nyin;
                                int mynyout = convs[i].nyout;
                                if (sconvs.size() == 1) {
                                    int cindex = netData.getIndexFromID(sconvs[0]);
                                    int tnum = (int)netData.getInputFieldsFromLayerName(cindex).size();
                                    if (tnum > 0) {
                                        mynyin = tnum;
                                        mynyout = tnum;
                                    }
                                }
                                mctype = CONV_GENERIC;
                                nnumdims = 2;
                                nconvins = new int[nnumdims];
                                nconvins[0] = convs[i].nxin;
                                nconvins[1] = mynyin;
                                nconvouts = new int[nnumdims];
                                nconvouts[0] = convs[i].nxout;
                                nconvouts[1] = mynyout;
                                nconvstrideins = new int[nnumdims];
                                nconvstrideins[0] = convs[i].nxstride;
                                nconvstrideins[1] = convs[i].nystride;
                                ninputdims = new int[nnumdims];
                                ninputdims[0] = convs[i].input_width;
                                ninputdims[1] = convs[i].input_height;
                            }
                            else if (convs[i].data_type == IMAGE_CONV) {    // Image num dimensions are fixed to three
                                mctype = ctype;
                                nnumdims = 3;
                                nconvins = new int[nnumdims];
                                nconvins[0] = convs[i].nxin;
                                nconvins[1] = convs[i].nyin;
                                nconvins[2] = 3;
                                if (convs[i].gray) {
                                    nconvins[2] = 1;
                                }
                                nconvouts = new int[nnumdims];
                                nconvouts[0] = convs[i].nxout;
                                nconvouts[1] = convs[i].nyout;
                                nconvouts[2] = 3;
                                if (convs[i].gray) {
                                    nconvouts[2] = 1;
                                }
                                nconvstrideins = new int[nnumdims];
                                nconvstrideins[0] = convs[i].nxstride;
                                nconvstrideins[1] = convs[i].nystride;
                                nconvstrideins[2] = 3;
                                if (convs[i].gray) {
                                    nconvstrideins[2] = 1;
                                }
                                ninputdims = new int[nnumdims];
                                ninputdims[0] = convs[i].input_width;
                                ninputdims[1] = convs[i].input_height;
                                ninputdims[2] = 3;
                                if (convs[i].gray) {
                                    ninputdims[2] = 1;
                                }
                            }
                            else {
                                addErrorString("addNextConvs2 : Error for " + convs[i].id + " : data type must be series or image");
                                return(-1);
                            }

                            // Some error checking to make sure we haven't screwed up the code somewhere
                            if (newConvs[nactive_index].allocated) {
                                addErrorString("addNextConvs2 : deallocating " + nid + " : something is amiss");
                                newConvs[nactive_index].deallocate();
                                newConvs[nactive_index].clear();
                            }

                            // Does conv need an input layer of nodes (needed when image conv is fed by nodes for instance)
                            bool nlayer_on_input = false;
                            int nnum_layer_inputs = 0;
                            string input_hidden_net_id = "";
                            if ((convs[i].data_type == IMAGE_CONV)) {
                                if (snodes.size() > 0) {
                                    nlayer_on_input = true;
                                }
                                if (!nlayer_on_input) {
                                    if (sconvs.size() > 0) {
                                        for (int m = 0; m < (int)sconvs.size(); m++) {
                                            for (int n = 0; n < (int)convs.size(); n++) {
                                                if (sconvs[m] == convs[n].id) {
                                                    if (convs[n].data_type == HIDDEN_NET) {
                                                        input_hidden_net_id = convs[n].id;
                                                        nlayer_on_input = true;
                                                        m = (int)sconvs.size();
                                                        n = (int)convs.size();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (nlayer_on_input) {
                                    //vector<string> snodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(nid));
                                    nnum_layer_inputs = (int)snodes.size();
                                }
                                /*if (num_inputs == 0) {  - where is num_inputs used?  seems like leftover code from something else
                                    vector<string> sconvs = netData.getImmediateSupportingConvNets(nid);
                                    if (sconvs.size() == 1) {
                                        num_inputs = netData.getNumOutputsFromConv(sconvs[0]);
                                    }
                                }*/
                            }

                            // Initialize convolution
                            if (!newConvs[nactive_index].init(nid, nmode, nlayers, nnodesperlayer, ntype, neta, nalpha, nrandom_scale, ndata_type, trainable, zerooutput,
                                                            randomize, nnumdims, nconvins, nconvouts, nconvstrideins, ninputdims, mctype, nlayer_on_input,
                                                            nnum_layer_inputs, noutconnections)) {
                                return(-1);
                            }

                            // Save convolution reference indices
                            newConvToConv.push_back(i);
                            convToNewConv[i] = nactive_index;

                            // Mark convolution as added
                            convs[i].used = true;

                            // Deallocate size and stride memory
                            delete[] nconvins;
                            delete[] nconvouts;
                            delete[] nconvstrideins;
                            delete[] ninputdims;


                            // If current convolution is following one previous convolution
                            if (convs[i].supportingConvIDs.size() == 1) {

                                // Get supporting convolution index and check if input
                                bool hasinput = false;
                                for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                                    for (int k = 0; k < (int)convs.size(); k++) {
                                        if (convs[k].id == convs[i].supportingConvIDs[0]) {
                                            if (convs[k].mode == "Input") {
                                                hasinput = true;
                                                j = (int)convs[i].supportingConvIDs.size();
                                                k = (int)convs.size();
                                            }
                                        }
                                    }
                                }
                                int input_conv_type = NONE;
                                for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                                    for (int k = 0; k < (int)convs.size(); k++) {
                                        if (convs[k].id == convs[i].supportingConvIDs[0]) {
                                            input_conv_type = convs[k].data_type;
                                            j = (int)convs[i].supportingConvIDs.size();
                                            k = (int)convs.size();
                                        }
                                    }
                                }

                                // Processing for convolution input pointers
                                // If input to convolution is directly connected to input layer
                                if (hasinput) {
                                    if (input_conv_type == IMAGE_CONV) {
                                        // Keep this
                                        //addErrorString("tp1 : " + nid);
                                        initializeConvImageErrorVariables();
                                    }
                                }
                                // Else if input from hidden net
                                else if (input_conv_type == HIDDEN_NET) {
                                }
                                // Else if input to convolution is from preceeding active convolution
                                else {
                                    // Keep this
                                    initializeConvImageErrorVariables();
                                }

                            }

                            if (!addLoopControlData(convs[i].id, nactive_index, false, false, false, false, false, false, false)) {
                                return(added);
                            }
                            nactive_index++;
                            added++;

                       }
                    }
                }
            }
        }

    }

    return(added);
}

int NetBuilder::addNextHiddenNet(bool randomize)
{
    int added = 0;

    // This is a double check to make sure we aren't trying to set up more hidden nets than we think we have
    if (nhidden_net_index < num_hidden_nets) {

        for (int i = 0; i < (int)convs.size(); i++) {
            // Check if conv is hidden and a net
            if ((convs[i].mode == "Hidden") && (convs[i].data_type == HIDDEN_NET)) {
                // if the conv hasn't already been added
                if (!convs[i].used) {
                    // Check to see if input convolutions are completed
                    bool completed = true;
                    for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                        for (int k = 0; k < (int)convs.size(); k++) {
                            if ((convs[i].supportingConvIDs[j] == convs[k].id) && (convs[i].id != convs[k].id)) {
                                if (!convs[k].used && (convs[k].mode != "Input")) {
                                    completed = false;
                                    j = (int)convs[i].supportingConvIDs.size();
                                    k = (int)convs.size();
                                }
                            }
                        }
                    }
                    
                    if (completed) {
                        completed = areConvInputsCompleted(i);
                        if (completed) {

                            //string s = "";

                            addPanel1String("Adding net " + convs[i].id);

                            int ninputs = getNumInputsFromConvs(i);
                            //char s2[200];  sprintf_s(s2, "addNextHiddenNet : ninputs = %i : ", ninputs); addErrorString(s2 + convs[i].id);

                            if (ninputs == 0) {
                                addErrorString("addNextHiddenNet : No inputs for " + convs[i].id);
                                return(-1);
                            }

                            addPanel1String("Adding net " + convs[i].id + " - getting outputs");
                            int nbinputs = getNumOutputs(netData.getIndexFromID(convs[i].id));

                            // Set up hidden net configurations for initialization
                            addPanel1String("Adding net " + convs[i].id + " - setting parameters");
                            string nid = convs[i].id;
                            string nmode = convs[i].mode;
                            int nlayers = convs[i].layers;
                            if (nlayers < 1) {
                                nlayers = 1;
                            }
                            int nnodesperlayer = convs[i].nodes;
                            if (nnodesperlayer < 1) {
                                nnodesperlayer = 1;
                            }
                            int ntype = SIGMOID;
                            if (convs[i].function == "Sigmoid") {
                                ntype = SIGMOID;
                            }
                            else if (convs[i].function == "Relu") {
                                ntype = RELU;
                            }
                            else if (convs[i].function == "Tanh") {
                                ntype = TANH;
                            }
                            else if (convs[i].function == "Gaussian") {
                                ntype = GAUSSIAN;
                            }
                            else if (convs[i].function == "Shockley") {
                                ntype = SHOCKLEY;
                            }
                            else if (convs[i].function == "SoftMax") {
                                ntype = SOFTMAX;
                            }
                            else {
                                addErrorString("addNextHiddenNet : setting activation function for hidden net " + convs[i].id + " to sigmoid");
                            }
                            double neta = convs[i].eta;
                            double nalpha = convs[i].alpha;
                            double nrandom_scale = convs[i].rand;
                            bool trainable = convs[i].trainable;
                            bool zerooutput = netData.getNodeZeroOutput(netData.getIndexFromID(nid));

                            // Some error checking to make sure we haven't screwed up the code somewhere
                            if (hnets[nhidden_net_index].allocated) {
                                addErrorString("addNextHiddenNet : deallocating " + nid + " : something is amiss");
                                hnets[nhidden_net_index].deallocate();
                                hnets[nhidden_net_index].clear();
                            }

                            // Initialize hidden net
                            addPanel1String("Adding net " + convs[i].id + " - initializing net");
                            if (!hnets[nhidden_net_index].init(nid, ninputs, nbinputs, nlayers, nnodesperlayer, ntype,
                                neta, nalpha, nrandom_scale, randomize, trainable, zerooutput)) {
                                return(-1);
                            }

                            // Mark convolution as added
                            convs[i].used = true;

                            addPanel1String("Adding net " + convs[i].id + " - adding to control loop");
                            if (!addLoopControlData(convs[i].id, nhidden_net_index, true, false, false, false, false, false, false)) {
                                return(added);
                            }
                            nhidden_net_index++;
                            added++;

                        }
                    }
                }
            }

        }

    }

    return(added);
}

int NetBuilder::addNextAttentionLayer()
{
    int added = 0;

    // This is a double check to make sure we aren't trying to set up more attention layers than we think we have
    if (nattention_layer_index < num_attention_layers) {

        for (int i = 0; i < (int)convs.size(); i++) {

            // Check if conv is hidden and a net
            if (convs[i].data_type == ATTENTION) {
                // if the conv hasn't already been added
                if (!convs[i].used) {
                    // Check to see if input convolutions are completed
                    bool completed = true;
                    for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                        for (int k = 0; k < (int)convs.size(); k++) {
                            if ((convs[i].supportingConvIDs[j] == convs[k].id) && (convs[i].id != convs[k].id)) {
                                if (!convs[k].used && (convs[k].mode != "Input")) {
                                    completed = false;
                                    j = (int)convs[i].supportingConvIDs.size();
                                    k = (int)convs.size();
                                }
                            }
                        }
                    }
                    if (completed) {
                        completed = areConvInputsCompleted(i);
                        if (completed) {

                            addPanel1String("Adding attention " + convs[i].id);

                            int nnodeinputs = convs[i].getNumInputNodes();
                            int nconvinputs = (int)convs[i].supportingConvIDs.size();

                            addPanel1String("Adding attention " + convs[i].id + " - getting inputs");

                            bool oninput = false;
                            if (convs[i].inputNodeIDs.size() > 0) {
                                oninput = true;
                                for (int j = 0; j < (int)convs[i].inputNodeIDs.size(); j++) {
                                    int nindex = netData.getIndexFromID(convs[i].inputNodeIDs[j]);
                                    if ((netData.getNodeMode(nindex) != "Input") && (netData.getNodeMode(nindex) != "PreProcess")) {
                                        j = (int)convs[i].inputNodeIDs.size();
                                        oninput = false;
                                    }
                                }
                            }

                            int ninputs = nnodeinputs;
                            if (nconvinputs > 0) {
                                for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                                    int cindex = -1;
                                    string conv_input_id = convs[i].supportingConvIDs[j];
                                    for (int k = 0; k < (int)convs.size(); k++) {
                                        if (conv_input_id == convs[k].id) {
                                            cindex = k;
                                            k = (int)convs.size();
                                        }
                                    }
                                    if (cindex >= 0) {
                                        if (convs[cindex].data_type == HIDDEN_NET) {
                                            ninputs += netData.getNodesPerLayerFromHiddenNet(convs[cindex].id);
                                        }
                                        else if (convs[cindex].data_type == ATTENTION) {
                                            ninputs += netData.getAttentionOutputSize(netData.getIndexFromID(convs[cindex].id));
                                        }
                                        else if (convs[cindex].data_type == SELFATTENTION) {
                                            ninputs += netData.getAttentionOutputSize(netData.getIndexFromID(convs[cindex].id));
                                        }
                                        else if (convs[cindex].data_type == NETCLUSTER) {
                                            ninputs += netData.getConvNodesPerLayer(netData.getIndexFromID(convs[cindex].id));
                                        }
                                        else if (convs[cindex].data_type == DROPOUT) {
                                            ninputs += getNumInputsFromConvs(cindex);
                                        }
                                        else {
                                            int tinputs = convs[cindex].output_width * convs[cindex].output_height;
                                            if (!conv_gray && (convs[cindex].data_type == IMAGE_CONV)) {
                                                tinputs = 3 * ninputs;
                                            }
                                            ninputs += tinputs;
                                        }
                                    }
                                }
                            }

                            if (ninputs == 0) {
                                addErrorString("addNextAttentionLayer : No inputs for " + convs[i].id);
                                return(-1);
                            }

                            addPanel1String("Adding attention " + convs[i].id + " - getting outputs");
                            int nbinputs = getNumOutputs(netData.getIndexFromID(convs[i].id));

                            // Set up attention configurations for initialization
                            addPanel1String("Adding attention " + convs[i].id + " - setting parameters");
                            string nid = convs[i].id;

                            int mindex = netData.getIndexFromID(nid);
                            int ndk = netData.getAttentionDk(mindex);
                            int ndv = netData.getAttentionDv(mindex);
                            int ndm = netData.getAttentionDm(mindex);
                            double naddsum = netData.getAttentionSumAddLimit(mindex);
                            bool nsoft = netData.hasSoftmaxProcessing(mindex);

                            // Some error checking to make sure we haven't screwed up the code somewhere
                            if (attentions[nattention_layer_index].allocated) {
                                addErrorString("addNextAttentionLayer : deallocating " + nid + " : something is amiss");
                                attentions[nattention_layer_index].deallocate();
                                attentions[nattention_layer_index].clear();
                            }

                            // Initialize hidden net
                            addPanel1String("Adding attention " + convs[i].id + " - initializing");
                            if (!attentions[nattention_layer_index].init(nid, ndk, ndv, ndm, naddsum, nsoft, ninputs, nbinputs, oninput)) {
                                return(-1);
                            }

                            // Mark convolution as added
                            convs[i].used = true;

                            addPanel1String("Adding attention " + convs[i].id + " - setting inputs");

                            addPanel1String("Adding net " + convs[i].id + " - adding to control loop");
                            if (!addLoopControlData(convs[i].id, nattention_layer_index, false, true, false, false, false, false, false)) {
                                return(added);
                            }
                            nattention_layer_index++;
                            added++;

                        }
                    }
                }
            }

        }

    }
    
    return(added);
}

int NetBuilder::addNextSelfAttentionLayer()
{
    int added = 0;

    // This is a double check to make sure we aren't trying to set up more attention layers than we think we have
    if (nself_attention_layer_index < num_self_attention_layers) {

        for (int i = 0; i < (int)convs.size(); i++) {

            // Check if conv is hidden and a net
            if (convs[i].data_type == SELFATTENTION) {
                // if the conv hasn't already been added
                if (!convs[i].used) {
                    // Check to see if input convolutions are completed
                    bool completed = true;
                    for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                        for (int k = 0; k < (int)convs.size(); k++) {
                            if ((convs[i].supportingConvIDs[j] == convs[k].id) && (convs[i].id != convs[k].id)) {
                                if (!convs[k].used && (convs[k].mode != "Input")) {
                                    completed = false;
                                    j = (int)convs[i].supportingConvIDs.size();
                                    k = (int)convs.size();
                                }
                            }
                        }
                    }
                    if (completed) {
                        completed = areConvInputsCompleted(i);
                        if (completed) {

                            addPanel1String("Adding self attention " + convs[i].id);

                            int nnodeinputs = convs[i].getNumInputNodes();
                            int nconvinputs = (int)convs[i].supportingConvIDs.size();

                            addPanel1String("Adding self attention " + convs[i].id + " - getting inputs");

                            int ninputs = getNumInputsFromConvs(i);

                            //char s[200];  sprintf_s(s, "addNextSelfAttentionLayer : ninputs = %i for ", ninputs); addErrorString(s + convs[i].id);

                            if (ninputs == 0) {
                                addErrorString("addNextSelfAttentionLayer : No inputs for " + convs[i].id);
                                return(-1);
                            }

                            bool oninput = false;
                            if (convs[i].inputNodeIDs.size() > 0) {
                                oninput = true;
                                for (int j = 0; j < (int)convs[i].inputNodeIDs.size(); j++) {
                                    int nindex = netData.getIndexFromID(convs[i].inputNodeIDs[j]);
                                    if ((netData.getNodeMode(nindex) != "Input") && (netData.getNodeMode(nindex) != "PreProcess")) {
                                        j = (int)convs[i].inputNodeIDs.size();
                                        oninput = false;
                                    }
                                }
                            }

                            int nnodeoutputs = (int)netData.getFollowingNodes(convs[i].id, true).size();
                            int nconvoutputs = (int)convs[i].followingConvIDs.size();

                            // Do some error checking
                            if ((nconvoutputs > 0) && (nnodeoutputs > 0)) {
                                char s[200];  sprintf_s(s, "addNextSelfAttentionLayer : Unable to process both node (%i) and conv (%i) outputs for ", nnodeoutputs, nconvoutputs);
                                addErrorString(s + convs[i].id);
                                return(-1);
                            }
                            else if (nconvoutputs > 1) {
                                addErrorString("addNextSelfAttentionLayer : Unable to process multiple conv puts for " + convs[i].id + " (2)");
                                return(-1);
                            }

                            addPanel1String("Adding self attention " + convs[i].id + " - getting outputs");
                            int nbinputs = nnodeoutputs;
                            if (nbinputs == 0) {
                                int test = getNumOutputs(netData.getIndexFromID(convs[i].id));
                                if (test > 0) {
                                    nbinputs = test;
                                }
                            }

                            // Set up attention configurations for initialization
                            addPanel1String("Adding self attention " + convs[i].id + " - setting parameters");
                            string nid = convs[i].id;

                            int mindex = netData.getIndexFromID(nid);
                            double naddsum = netData.getAttentionSumAddLimit(mindex);
                            bool nsoft = netData.hasSoftmaxProcessing(mindex);

                            // Some error checking to make sure we haven't screwed up the code somewhere
                            if (selfattentions[nself_attention_layer_index].allocated) {
                                addErrorString("addNextSelfAttentionLayer : deallocating " + nid + " : something is amiss");
                                selfattentions[nself_attention_layer_index].deallocate();
                                selfattentions[nself_attention_layer_index].clear();
                            }

                            // Initialize hidden net
                            addPanel1String("Adding self attention " + convs[i].id + " - initializing");
                            if (!selfattentions[nself_attention_layer_index].init(nid, naddsum, nsoft, ninputs, nbinputs, oninput)) {
                                return(-1);
                            }

                            // Mark convolution as added
                            convs[i].used = true;

                            addPanel1String("Adding self attention " + convs[i].id + " - setting inputs");

                            addPanel1String("Adding net " + convs[i].id + " - adding to control loop");
                            if (!addLoopControlData(convs[i].id, nself_attention_layer_index, false, false, true, false, false, false, false)) {
                                return(added);
                            }
                            nself_attention_layer_index++;
                            added++;

                        }
                    }
                }
            }

        }

    }

    return(added);
}

int NetBuilder::addNextNetCluster(bool randomize)
{
    int added = 0;

    // This is a double check to make sure we aren't trying to set up more hidden nets than we think we have
    if (nnet_cluster_index < num_net_clusters) {

        for (int i = 0; i < (int)convs.size(); i++) {
            // Check if conv is hidden and a net
            if (convs[i].data_type == NETCLUSTER) {
                // if the conv hasn't already been added
                if (!convs[i].used) {
                    // Check to see if input convolutions are completed
                    bool completed = true;
                    for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                        for (int k = 0; k < (int)convs.size(); k++) {
                            if ((convs[i].supportingConvIDs[j] == convs[k].id) && (convs[i].id != convs[k].id)) {
                                if (!convs[k].used && (convs[k].mode != "Input")) {
                                    completed = false;
                                    j = (int)convs[i].supportingConvIDs.size();
                                    k = (int)convs.size();
                                }
                            }
                        }
                    }
                    if (completed) {
                        completed = areConvInputsCompleted(i);
                        if (completed) {

                            string s = "";

                            addPanel1String("Adding cluster " + convs[i].id);

                            int nnodeinputs = convs[i].getNumInputNodes();
                            int nconvinputs = (int)convs[i].supportingConvIDs.size();

                            addPanel1String("Adding cluster " + convs[i].id + " - getting inputs");
                            int ninputs = nnodeinputs;
                            if (nconvinputs > 0) {
                                for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                                    int cindex = -1;
                                    string conv_input_id = convs[i].supportingConvIDs[j];
                                    for (int k = 0; k < (int)convs.size(); k++) {
                                        if (conv_input_id == convs[k].id) {
                                            cindex = k;
                                            k = (int)convs.size();
                                        }
                                    }
                                    if (cindex >= 0) {
                                        if (convs[cindex].data_type == HIDDEN_NET) {
                                            ninputs += netData.getNodesPerLayerFromHiddenNet(convs[cindex].id);
                                        }
                                        else if ((convs[cindex].data_type == ATTENTION) || (convs[cindex].data_type == SELFATTENTION)) {
                                            ninputs += netData.getAttentionOutputSize(netData.getIndexFromID(convs[cindex].id));
                                        }
                                        else if (convs[cindex].data_type == DROPOUT) {
                                            ninputs += getNumInputsFromConvs(cindex);
                                        }
                                        else {
                                            int tinputs = convs[cindex].output_width * convs[cindex].output_height;
                                            if (!conv_gray && (convs[cindex].data_type == IMAGE_CONV)) {
                                                tinputs = 3 * tinputs;
                                            }
                                            ninputs += tinputs;
                                        }
                                    }
                                }
                            }

                            int noutputs = convs[i].nodes;

                            // Set up hidden net configurations for initialization
                            addPanel1String("Adding cluster " + convs[i].id + " - setting parameters");
                            string nid = convs[i].id;
                            string nmode = convs[i].mode;
                            int ntype = SIGMOID;
                            if (convs[i].function == "Sigmoid") {
                                ntype = SIGMOID;
                            }
                            else if (convs[i].function == "Relu") {
                                ntype = RELU;
                            }
                            else if (convs[i].function == "Tanh") {
                                ntype = TANH;
                            }
                            else if (convs[i].function == "Gaussian") {
                                ntype = GAUSSIAN;
                            }
                            else if (convs[i].function == "Shockley") {
                                ntype = SHOCKLEY;
                            }
                            else {
                                addErrorString("addNextNetCluster : setting activation function for hidden net " + convs[i].id + " to sigmoid");
                            }
                            double neta = convs[i].eta;
                            double nalpha = convs[i].alpha;
                            double nrandom_scale = convs[i].rand;
                            bool trainable = convs[i].trainable;
                            bool zerooutput = netData.getNodeZeroOutput(netData.getIndexFromID(nid));
                            double nepochs = netData.getClusterUpdateEpochs(netData.getIndexFromID(nid));

                            // Some error checking to make sure we haven't screwed up the code somewhere
                            if (nclusters[nnet_cluster_index].allocated) {
                                addErrorString("addNextNetCluster : deallocating " + nid + " : something is amiss");
                                nclusters[nnet_cluster_index].deallocate();
                                nclusters[nnet_cluster_index].clear();
                            }

                            // Initialize hidden net
                            addPanel1String("Adding cluster " + convs[i].id + " - initializing net");
                            if (!nclusters[nnet_cluster_index].init(nid, nmode, ninputs, noutputs, ntype,
                                neta, nalpha, nrandom_scale, randomize, trainable, zerooutput, nepochs)) {
                                return(-1);
                            }

                            // Mark convolution as added
                            convs[i].used = true;

                            addPanel1String("Adding net " + convs[i].id + " - adding to control loop");
                            if (!addLoopControlData(convs[i].id, nnet_cluster_index, false, false, false, true, false, false, false)) {
                                return(added);
                            }
                            nnet_cluster_index++;
                            added++;

                        }
                    }
                }
            }

        }

    }

    return(added);
}

int NetBuilder::addNextDropout()
{
    int added = 0;

    // This is a double check to makesure we aren't trying to set up more convolutions than we think we have
    if (ndropout_index < num_dropouts) {

        // Look through all convolutions to see if any are ready to add to net 
        for (int i = 0; i < (int)convs.size(); i++) {
            // Input convs aren't active and so we're not processing them here
            if (convs[i].data_type == DROPOUT) {
                // if the conv hasn't already been added
                if (!convs[i].used) {
                    // Check to see if input convolutions are completed
                    bool completed = true;
                    for (int j = 0; j < (int)convs[i].supportingConvIDs.size(); j++) {
                        for (int k = 0; k < (int)convs.size(); k++) {
                            if ((convs[i].supportingConvIDs[j] == convs[k].id) && (convs[i].id != convs[k].id)) {
                                if (!convs[k].used && (convs[k].mode != "Input")) {
                                    completed = false;
                                    j = (int)convs[i].supportingConvIDs.size();
                                    k = (int)convs.size();
                                }
                            }
                        }
                    }

                    if (completed) {
                        // Check to see if input nodes are completed
                        completed = areConvInputsCompleted(i);

                        if (completed) {

                            addPanel1String("Adding dropout " + convs[i].id);

                            // Set up convolution configurations for initialization
                            string nid = convs[i].id;

                            addPanel1String("Adding dropout " + convs[i].id + " - getting inputs");
                            int ninputs = getNumInputsFromConvs(i);

                            vector<string> fnodes = netData.getImmediateFollowingNodes(netData.getIndexFromID(nid));
                            vector<string> fconvs = netData.getImmediateFollowingConvNets(nid);

                            int noutconnections = 0;
                            if (fnodes.size() > 0) {
                                noutconnections = (int)fnodes.size();
                            }
                            if (fconvs.size() > 0) {
                                for (int k = 0; k < (int)fconvs.size(); k++) {
                                    int cindex = netData.getIndexFromID(fconvs[k]);
                                    if (netData.isConvLayerOnly(cindex)) {
                                        noutconnections++;
                                    }
                                    else if (netData.isHiddenNetOnly(cindex)) {
                                        noutconnections += netData.getConvNodesPerLayer(cindex);
                                    }
                                    else if (netData.isAttentionLayerOnly(cindex)) {
                                        int attentioninputsizeforconv = netData.getDv(cindex) * netData.getD(cindex);
                                        noutconnections += attentioninputsizeforconv;
                                    }
                                    else if (netData.isSelfAttentionLayerOnly(cindex)) {
                                        int attentioninputsizeforconv = netData.getDv(cindex) * netData.getD(cindex);
                                        noutconnections += attentioninputsizeforconv;
                                    }
                                    else if (netData.isNetClusterOnly(cindex)) {
                                        noutconnections += netData.getConvNodesPerLayer(cindex);
                                    }
                                    else {
                                        addErrorString("addNextDropout : unable to get num inputs for " + fconvs[k]);
                                    }
                                }
                            }
                            if (noutconnections <= 0) {
                                noutconnections = 1;
                            }

                            double npercent = netData.getDropoutPercent(netData.getIndexFromID(nid));

                            // Some error checking to make sure we haven't screwed up the code somewhere
                            if (dropouts[ndropout_index].allocated) {
                                dropouts[ndropout_index].deallocate();
                                dropouts[ndropout_index].clear();
                            }

                            // Initialize dropout
                            if (!dropouts[ndropout_index].init(nid, ninputs, npercent, noutconnections)) {
                                return(-1);
                            }

                            // Save convolution reference indices
                            newConvToConv.push_back(i);
                            convToNewConv[i] = nactive_index;

                            // Mark convolution as added
                            convs[i].used = true;

                            if (!addLoopControlData(convs[i].id, ndropout_index, false, false, false, false, true, false, false)) {
                                return(added);
                            }
                            ndropout_index++;
                            added++;

                        }
                    }
                }
            }
        }

    }

    return(added);
}

int NetBuilder::addNextAddition()
{
    int added = 0;

    // This is a double check to make sure we aren't trying to set up more additions than we think we have
    if (naddition_index < num_additions) {

        // Look through all additions to see if any are ready to add to net 
        for (int i = 0; i < (int)nodeIndices.size(); i++) {
            // Look for hidden nodes with addition function
            if (!netData.getIsOutput(nodeIndices[i]) && (netData.getNodeFunction(nodeIndices[i]) == "Add")) {
                // if the node hasn't already been added
                if (!nodes_used[i]) {

                    // Check to see if input nodes are completed
                    if (areInputsCompleted(i)) {

                        string nid = netData.getID(nodeIndices[i]);

                        addPanel1String("Adding addition " + nid + " - getting inputs");
                        vector<string> snodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(nid));
                        vector<string> sconvs = netData.getImmediateSupportingConvNets(nid, true);

                        int ninputnodes = 0;
                        bool ninputnodesonly = false;
                        if (snodes.size() > 0) {
                            if (sconvs.size() == 0) {
                                ninputnodes = (int)snodes.size();
                                ninputnodesonly = true;
                            }
                            else {
                                ninputnodes = (int)sconvs.size() + 1;
                            }
                        }
                        else if (sconvs.size() > 0) {
                            ninputnodes = (int)sconvs.size();
                        }
                        //char s[200]; sprintf_s(s, "addNextAddition : ninputnodes = %i", ninputnodes);  addErrorString(s);

                        int nmodeldimension = 0;
                        if (sconvs.size() == 0) {
                            nmodeldimension = 1;
                        }
                        else {
                            if (snodes.size() > 0) {
                                nmodeldimension = (int)snodes.size();
                            }
                            for (int k = 0; k < (int)sconvs.size(); k++) {
                                int cindex = netData.getIndexFromID(sconvs[k]);
                                if (netData.isConvLayerOnly(cindex)) {
                                    int tinputs = netData.getImageConvOutputWidth(cindex) * netData.getImageConvOutputHeight(cindex);
                                    if (!conv_gray && (netData.getConvType(cindex) == IMAGE_CONV)) {
                                        tinputs = 3 * tinputs;
                                    }
                                    if (tinputs > nmodeldimension) {
                                        nmodeldimension = tinputs;
                                    }
                                }
                                else if (netData.isHiddenNetOnly(cindex)) {
                                    if (netData.getConvNodesPerLayer(cindex) > nmodeldimension) {
                                        nmodeldimension = netData.getConvNodesPerLayer(cindex);
                                    }
                                }
                                else if (netData.isAttentionLayerOnly(cindex)) {
                                    int attentioninputsizeforconv = netData.getDv(cindex) * netData.getD(cindex);
                                    if (attentioninputsizeforconv > nmodeldimension) {
                                        nmodeldimension = attentioninputsizeforconv;
                                    }
                                }
                                else if (netData.isSelfAttentionLayerOnly(cindex)) {
                                    int attentioninputsizeforconv = netData.getDv(cindex) * netData.getD(cindex);
                                    if (attentioninputsizeforconv > nmodeldimension) {
                                        nmodeldimension = attentioninputsizeforconv;
                                    }
                                }
                                else if (netData.isNetClusterOnly(cindex)) {
                                    if (netData.getConvNodesPerLayer(cindex) > nmodeldimension) {
                                        nmodeldimension = netData.getConvNodesPerLayer(cindex);
                                    }
                                }
                                else if (netData.isDropout(cindex)) {
                                    int tindex = -1;
                                    string conv_input_id = sconvs[k];
                                    for (int m = 0; m < (int)convs.size(); m++) {
                                        if (conv_input_id == convs[m].id) {
                                            tindex = m;
                                            m = (int)convs.size();
                                        }
                                    }
                                    if (tindex >= 0) {
                                        int ninputs = getNumInputsFromConvs(tindex);
                                        //char s[200]; sprintf_s(s, "addNextAddition : ninputs = %i", ninputs);  addErrorString(s);
                                        if (ninputs > nmodeldimension) {
                                            nmodeldimension = ninputs;
                                        }
                                    }
                                }
                                else {
                                    addErrorString("addNextAddition : unable to get num inputs for " + sconvs[k] + " for addition " + nid);
                                }
                            }
                        }
                        //char s[200]; sprintf_s(s, "addNextAddition : nmodeldimension = %i", nmodeldimension);  addErrorString(s);

                        addPanel1String("Adding addition " + nid + " - getting outputs");
                        vector<string> fnodes = netData.getImmediateFollowingNodes(netData.getIndexFromID(nid));
                        vector<string> fconvs = netData.getImmediateFollowingConvNets(nid);

                        int noutputnodes = 0;
                        if (fnodes.size() > 0) {
                            if (fconvs.size() == 0) {
                                noutputnodes = (int)fnodes.size();
                            }
                            else {
                                noutputnodes = (int)fconvs.size() + 1;
                            }
                        }
                        else if (sconvs.size() > 0) {
                            noutputnodes = (int)fconvs.size();
                        }
                        if (noutputnodes <= 0) {
                            noutputnodes = 1;
                        }

                        // Some error checking to make sure we haven't screwed up the code somewhere
                        if (additions[naddition_index].allocated) {
                            additions[naddition_index].deallocate();
                            additions[naddition_index].clear();
                        }

                        // Initialize dropout
                        if (!additions[naddition_index].init(nid, ninputnodes, noutputnodes, nmodeldimension, ninputnodesonly)) {
                            return(-1);
                        }

                        // Mark addition as added
                        nodes_used[i] = true;

                        if (!addLoopControlData(nid, naddition_index, false, false, false, false, false, true, false)) {
                            return(added);
                        }
                        naddition_index++;
                        added++;

                    }
                }
            }
        }

    }

    return(added);
}

int NetBuilder::addNextMultiplication()
{
    int added = 0;

    // This is a double check to make sure we aren't trying to set up more multiplications than we think we have
    if (nmultiplication_index < num_multiplications) {

        // Look through all multiplications to see if any are ready to add to net 
        for (int i = 0; i < (int)nodeIndices.size(); i++) {
            // Look for hidden nodes with multiplication function
            if (!netData.getIsOutput(nodeIndices[i]) && (netData.getNodeFunction(nodeIndices[i]) == "Multiply")) {
                // if the node hasn't already been added
                if (!nodes_used[i]) {

                    // Check to see if input nodes are completed
                    if (areInputsCompleted(i)) {

                        string nid = netData.getID(nodeIndices[i]);

                        addPanel1String("Adding multiplication " + nid + " - getting inputs");
                        vector<string> snodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(nid));
                        vector<string> sconvs = netData.getImmediateSupportingConvNets(nid, true);

                        int ninputnodes = 0;
                        bool ninputnodesonly = false;
                        if (snodes.size() > 0) {
                            if (sconvs.size() == 0) {
                                ninputnodes = (int)snodes.size();
                                ninputnodesonly = true;
                            }
                            else {
                                ninputnodes = (int)sconvs.size() + 1;
                            }
                        }
                        else if (sconvs.size() > 0) {
                            ninputnodes = (int)sconvs.size();
                        }

                        int nmodeldimension = 0;
                        if (sconvs.size() == 0) {
                            nmodeldimension = 1;
                        }
                        else {
                            if (snodes.size() > 0) {
                                nmodeldimension = (int)snodes.size();
                            }
                            for (int k = 0; k < (int)sconvs.size(); k++) {
                                int cindex = netData.getIndexFromID(sconvs[k]);
                                if (netData.isConvLayerOnly(cindex)) {
                                    int tinputs = netData.getImageConvOutputWidth(cindex) * netData.getImageConvOutputHeight(cindex);
                                    if (!conv_gray && (netData.getConvType(cindex) == IMAGE_CONV)) {
                                        tinputs = 3 * tinputs;
                                    }
                                    if (tinputs > nmodeldimension) {
                                        nmodeldimension = tinputs;
                                    }
                                }
                                else if (netData.isHiddenNetOnly(cindex)) {
                                    if (netData.getConvNodesPerLayer(cindex) > nmodeldimension) {
                                        nmodeldimension = netData.getConvNodesPerLayer(cindex);
                                    }
                                }
                                else if (netData.isAttentionLayerOnly(cindex)) {
                                    int attentioninputsizeforconv = netData.getDv(cindex) * netData.getD(cindex);
                                    if (attentioninputsizeforconv > nmodeldimension) {
                                        nmodeldimension = attentioninputsizeforconv;
                                    }
                                }
                                else if (netData.isSelfAttentionLayerOnly(cindex)) {
                                    int attentioninputsizeforconv = netData.getDv(cindex) * netData.getD(cindex);
                                    if (attentioninputsizeforconv > nmodeldimension) {
                                        nmodeldimension = attentioninputsizeforconv;
                                    }
                                }
                                else if (netData.isNetClusterOnly(cindex)) {
                                    if (netData.getConvNodesPerLayer(cindex) > nmodeldimension) {
                                        nmodeldimension = netData.getConvNodesPerLayer(cindex);
                                    }
                                }
                                else {
                                    addErrorString("addNextMultiplication : unable to get num inputs for " + sconvs[k] + " for multiplication " + nid);
                                }
                            }
                        }

                        addPanel1String("Adding multiplication " + nid + " - getting outputs");
                        vector<string> fnodes = netData.getImmediateFollowingNodes(netData.getIndexFromID(nid));
                        vector<string> fconvs = netData.getImmediateFollowingConvNets(nid);

                        int noutputnodes = 0;
                        if (fnodes.size() > 0) {
                            if (fconvs.size() == 0) {
                                noutputnodes = (int)fnodes.size();
                            }
                            else {
                                noutputnodes = (int)fconvs.size() + 1;
                            }
                        }
                        else if (sconvs.size() > 0) {
                            noutputnodes = (int)fconvs.size();
                        }
                        if (noutputnodes <= 0) {
                            noutputnodes = 1;
                        }

                        //char s[200];  sprintf_s(s, " : snodes size = %i : sconvs size = %i : fnodes size = %i : fconvs size = %i", (int)snodes.size(), (int)sconvs.size(), (int)fnodes.size(), (int)fconvs.size());
                        //addErrorString("addNextMultiplication : " + nid + s);

                        //sprintf_s(s, " : ninputnodes = %i : noutputnodes = %i : nmodeldimension = %i", ninputnodes, noutputnodes, nmodeldimension);
                        //addErrorString("addNextMultiplication : " + nid + s);

                        // Some error checking to make sure we haven't screwed up the code somewhere
                        if (multiplications[nmultiplication_index].allocated) {
                            multiplications[nmultiplication_index].deallocate();
                            multiplications[nmultiplication_index].clear();
                        }

                        // Initialize dropout
                        if (!multiplications[nmultiplication_index].init(nid, ninputnodes, noutputnodes, nmodeldimension, ninputnodesonly)) {
                            return(-1);
                        }

                        // Mark multiplication as added
                        nodes_used[i] = true;

                        if (!addLoopControlData(nid, nmultiplication_index, false, false, false, false, false, false, true)) {
                            return(added);
                        }
                        nmultiplication_index++;
                        added++;

                    }
                }
            }
        }

    }

    return(added);
}

int NetBuilder::getNumInputsFromConvs(int tindex)
{
    if (tindex >= (int)convs.size()) {
        char s[200]; sprintf_s(s, "getNumInputsFromConvs : tindex (%i) > convs size (%i)", tindex, (int)convs.size());  addErrorString(s);
        return(0);
    }

    addPanel1String("Getting num inputs for " + convs[tindex].id);

    //int nnodeinputs = convs[tindex].getNumInputNodes();
    int ninputs = 0;// nnodeinputs;
    vector<string> snodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(convs[tindex].id));
    if (snodes.size() > 0) {
        for (int j = 0; j < (int)snodes.size(); j++) {
            int nindex = netData.getIndexFromID(snodes[j]);
            if (netData.getNodeFunction(nindex) == "Add") {
                if (num_additions > 0) {
                    for (int k = 0; k < num_additions; k++) {
                        if (additions[k].id == snodes[j]) {
                            int tmodel = additions[k].model_dimension;
                            ninputs += tmodel;
                            //char s2[200];  sprintf_s(s2, "addNextDropout : ninputs = %i : tmodel = %i : ", ninputs, tmodel);
                            //addErrorString(s2 + convs[i].id + " for " + snodes[j]);
                        }
                    }
                }
            }
            else if (netData.getNodeFunction(nindex) == "Multiplication") {
                if (num_multiplications > 0) {
                    for (int k = 0; k < num_multiplications; k++) {
                        if (multiplications[k].id == snodes[j]) {
                            int tmodel = multiplications[k].model_dimension;
                            ninputs += tmodel;
                            //char s2[200];  sprintf_s(s2, "addNextDropout : tinputs = %i : toutputs = %i : tmodel = %i : ", tinputs, toutputs, tmodel);
                            //addErrorString(s2 + convs[i].id + " for " + snodes[j]);
                        }
                    }
                }
            }
            else if (netData.getNodeMode(nindex) == "PreProcess") {
                if (netData.getNodePreProcessor(nindex).substr(0, 3) == "FFT") {
                    string process = netData.getNodePreProcessor(nindex);
                    ninputs += myStoi(process.substr(3));
                }
                else {
                    vector<string> sconvs = netData.getImmediateSupportingConvNets(nindex);
                    if (sconvs.size() == 0) {
                        ninputs++;
                    }
                    else if (sconvs.size() == 1) {
                        int scid = netData.getIndexFromID(sconvs[0]);
                        if ((netData.getNodeMode(scid) == "Input") && (netData.getConvType(sconvs[0]) == DATA_CONV)) {
                            int nfields = (int)netData.getDataFields(scid).size();
                            int num_model = netData.getD(scid);
                            if (num_model > 1) {
                                nfields = num_model * nfields;
                            }
                            ninputs += nfields;
                        }
                        else {
                            addErrorString("getNumInputsFromConvs : need processing for input conv " + sconvs[0]);
                        }
                    }
                    else {
                        addErrorString("getNumInputsFromConvs : need processing for multiple input convs for " + netData.getID(tindex));
                    }
                }
            }
            else {
                ninputs++;
            }
        }
    }

    int nconvinputs = (int)convs[tindex].supportingConvIDs.size();
    if (nconvinputs > 0) {
        bool once = true;
        for (int j = 0; j < (int)convs[tindex].supportingConvIDs.size(); j++) {
            int mindex = -1;
            string conv_input_id = convs[tindex].supportingConvIDs[j];
            for (int k = 0; k < (int)convs.size(); k++) {
                if (conv_input_id == convs[k].id) {
                    mindex = k;
                    k = (int)convs.size();
                }
            }
            if (mindex >= 0) {
                if (convs[mindex].data_type == HIDDEN_NET) {
                    ninputs += netData.getNodesPerLayerFromHiddenNet(convs[mindex].id);
                }
                else if (convs[mindex].data_type == ATTENTION) {
                    ninputs += netData.getAttentionOutputSize(netData.getIndexFromID(convs[mindex].id));
                }
                else if (convs[mindex].data_type == SELFATTENTION) {
                    int dim = getSelfAttentionOutputSize(getNumInputsFromConvs(mindex));
                    ninputs += (dim * dim);
                }
                else if (convs[mindex].data_type == DROPOUT) {
                    ninputs += getNumInputsFromConvs(mindex);
                }
                else if (convs[mindex].data_type == IMAGE_CONV) {
                    int tinputs = convs[mindex].output_width * convs[mindex].output_height;
                    if (!conv_gray) {
                        tinputs = 3 * tinputs;
                    }
                    ninputs += tinputs;
                }
                else if (convs[mindex].data_type == DATA_CONV) {

                    int myin = 0;
                    int myout = 0;
                    int stridex = 0;
                    int stridey = 0;

                    int sindex = netData.getIndexFromID(convs[mindex].id);

                    vector<int> dimensions = netData.getConvDimensions(sindex);
                    if (dimensions.size() > 1) {
                        myin = dimensions[1];
                        if (dimensions.size() > 3) {
                            myout = dimensions[3];
                        }
                    }

                    vector<int> strides = netData.getConvStrides(sindex);
                    if (strides.size() > 0) {
                        stridex = strides[0];
                        if (strides.size() > 1) {
                            stridey = strides[1];
                        }
                    }

                    int outwidth = 0;

                    if (stridex > 0) {
                        outwidth = myout * (1 + (stridey - myin) / stridex);
                    }

                    int mynyout = convs[mindex].nyout;
                    if (convs[tindex].supportingConvIDs.size() == 1) {
                        int cindex = netData.getIndexFromID(convs[tindex].supportingConvIDs[0]);
                        int tnum = (int)netData.getInputFieldsFromLayerName(cindex).size();
                        if (tnum > 0) {
                            mynyout = tnum;
                        }
                    }
                    
                    outwidth = outwidth * mynyout;

                    //char s2[200];
                    //sprintf_s(s2, "getNumInputsFromConvs : adding %i (%i : %i : %i : %i : %i) inputs to ", convs[mindex].nxout * mynyout, convs[mindex].nxout, mynyout, convs[tindex].nxstride, convs[tindex].nystride, outwidth);
                    //addErrorString(s2 + convs[tindex].id + " from " + convs[mindex].id);
                    ninputs += outwidth;// convs[mindex].nxout* mynyout;

                }
                else if ((convs[mindex].data_type == SOUND_CONV) && (convs[mindex].mode == "Input")) {
                    int cindex = netData.getIndexFromID(convs[mindex].id);
                    int mxin = 5000;
                    vector<int> dims = netData.getConvDimensions(cindex);
                    vector<int> strides = netData.getConvStrides(cindex);
                    if ((dims.size() > 0) && (strides.size() > 1)) {
                        mxin = dims[0] * strides[1];
                        if (FULLY_CONNECTED_FOLLOWING) {
                            mxin = wave_files_longest;
                        }
                    }
                    ninputs += mxin;
                }
                else {
                    char s2[200];  
                    sprintf_s(s2, "getNumInputsFromConvs : unable to find match for data_type %i for ", convs[mindex].data_type);
                    addErrorString(s2 + convs[tindex].id);
                }
            }
            else {
                addErrorString("getNumInputsFromConvs : need extra processing for " + conv_input_id);
            }
        }
    }

    //CString s2 = _T(""); s2.Format(_T("getNumInputsFromConvs : ninputs = %i : "), ninputs);  AfxMessageBox(s2 + (CString)convs[tindex].id.c_str());

    return(ninputs);
}

int NetBuilder::getNumInputsFromNetData(int nindex)
{
    if (nindex >= netData.size()) {
        char s[200]; sprintf_s(s, "getNumInputsFromNetData : nindex (%i) > netData size (%i)", nindex, netData.size());  addErrorString(s);
        return(0);
    }

    addPanel1String("Getting num inputs for " + netData.getID(nindex));

    int ninputs = 0;
    vector<string> snodes = netData.getImmediateSupportingNodes(nindex);
    if (snodes.size() > 0) {
        for (int j = 0; j < (int)snodes.size(); j++) {
            int nindex = netData.getIndexFromID(snodes[j]);
            if (netData.getNodeFunction(nindex) == "Add") {
                if (num_additions > 0) {
                    for (int k = 0; k < num_additions; k++) {
                        if (additions[k].id == snodes[j]) {
                            int tmodel = additions[k].model_dimension;
                            ninputs += tmodel;
                            //char s2[200];  sprintf_s(s2, "addNextDropout : ninputs = %i : tmodel = %i : ", ninputs, tmodel);
                            //addErrorString(s2 + convs[i].id + " for " + snodes[j]);
                        }
                    }
                }
            }
            else if (netData.getNodeFunction(nindex) == "Multiplication") {
                if (num_multiplications > 0) {
                    for (int k = 0; k < num_multiplications; k++) {
                        if (multiplications[k].id == snodes[j]) {
                            int tmodel = multiplications[k].model_dimension;
                            ninputs += tmodel;
                            //char s2[200];  sprintf_s(s2, "addNextDropout : tinputs = %i : toutputs = %i : tmodel = %i : ", tinputs, toutputs, tmodel);
                            //addErrorString(s2 + convs[i].id + " for " + snodes[j]);
                        }
                    }
                }
            }
            else if (netData.getNodeMode(nindex) == "PreProcess") {
                if (netData.getNodePreProcessor(nindex).substr(0, 3) == "FFT") {
                    string process = netData.getNodePreProcessor(nindex);
                    ninputs += myStoi(process.substr(3));
                }
                else {
                    ninputs++;
                }
            }
            else {
                ninputs++;
            }
        }
    }

    vector<string> sconvs = netData.getImmediateSupportingConvNets(nindex);
    if (sconvs.size() > 0) {
        for (int j = 0; j < (int)sconvs.size(); j++) {
            int nindex = netData.getIndexFromID(sconvs[j]);
            if (netData.getConvType(nindex) == HIDDEN_NET) {
                ninputs += netData.getNodesPerLayerFromHiddenNet(netData.getID(nindex));
            }
            else if (netData.getConvType(nindex) == ATTENTION) {
                ninputs += netData.getAttentionOutputSize(nindex);
            }
            else if (netData.getConvType(nindex) == SELFATTENTION) {
                int dim = getSelfAttentionOutputSize(getNumInputsFromNetData(nindex));
                ninputs += (dim * dim);
            }
            else if (netData.getConvType(nindex) == DROPOUT) {
                ninputs += getNumInputsFromNetData(nindex);
            }
            else {
                for (int k = 0; k < (int)convs.size(); k++) {
                    if (convs[k].id == netData.getID(nindex)) {
                        int tinputs = convs[k].output_width * convs[k].output_height;
                        if (!conv_gray && (netData.getConvType(nindex) == IMAGE_CONV)) {
                            tinputs = 3 * tinputs;
                        }
                        ninputs += tinputs;
                        k = (int)convs.size();
                    }
                }
            }
        }
    }

    //CString s2 = _T(""); s2.Format(_T("getNumInputsFromNetData : ninputs = %i : "), ninputs);  AfxMessageBox(s2 + (CString)netData.getID(nindex).c_str());

    return(ninputs);
}

int NetBuilder::getSelfAttentionOutputSize(int ninputs)
{
    int input_size = ninputs;

    int third = input_size / 3;
    if ((input_size % third) > 0) {
        third++;
    }

    int dim = (int)sqrt((double)third);

    int test_num_inputs = 3 * (dim * dim);

    if (test_num_inputs < input_size) {
        dim++;
        test_num_inputs = 3 * (dim * dim);
    }

    return(dim);
}

int NetBuilder::getConvIndexFromName(string name)
{
    if (convs.size() > 0) {
        for (int k = 0; k < (int)convs.size(); k++) {
            if (convs[k].id == name) {
                return(k);
            }
        }
    }
    return(-1);
}

void NetBuilder::initializeConvImageErrorVariables()
{
    // Getting parameters to copy image data for error calculations for output layer
    // Only need to do this once per net set up (trying to save time)
    if (!conv_variables_initialized) {
        if (convs.size() > 0) {

            int numx1 = 0;
            int numy1 = 0;
            for (int k = 0; k < (int)convs.size(); k++) {
                if ((convs[k].mode == "Input") && (convs[k].data_type == IMAGE_CONV)) {
                    numx1 = convs[k].input_width;
                    numy1 = convs[k].input_height;
                    conv_single = numx1 * numy1;
                    conv_max_size = getConvImageMemorySize(convs[k].id);
                    k = (int)convs.size();
                }
            }
            if (numx1 == 0) {
                for (int k = 0; k < (int)convs.size(); k++) {
                    if ((convs[k].mode == "Output") && (convs[k].data_type == IMAGE_CONV)) {
                        numx1 = convs[k].output_width;
                        numy1 = convs[k].output_height;
                        conv_single = convs[k].input_width * convs[k].input_height;
                        conv_max_size = getConvImageMemorySize(convs[k].id);
                        k = (int)convs.size();
                    }
                }
            }

            conv_gray = convs[0].gray;
            conv_numx = newConvs[nactive_index].inputdims[0];
            conv_numy = newConvs[nactive_index].inputdims[1];
            if (conv_single != (conv_numx * conv_numy)) {
                char s[200]; sprintf_s(s, "expectation is that conv_single (%i) equals conv_numx (%i) times conv_numy (%i)", conv_single, conv_numx, conv_numy);  addErrorString(s);
            }

            conv_variables_initialized = true;

        }
    }
}

string NetBuilder::displayInitData(int total_num_connects)
{
    string s = "";
    for (int i = ((int)loopControl.size() - 1); i >= 0; i--) {
        char s3[200];  sprintf_s(s3, "Loop Control : index %i ", i);
        if (loopControl[i].isconv) {
            char s4[200];  sprintf_s(s4, " : hidden conv index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else if (loopControl[i].ishiddennet) {
            char s4[200];  sprintf_s(s4, " : hidden net index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else if (loopControl[i].isattention) {
            char s4[200];  sprintf_s(s4, " : attention index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else if (loopControl[i].isselfattention) {
            char s4[200];  sprintf_s(s4, " : self attention index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else if (loopControl[i].isnetcluster) {
            char s4[200];  sprintf_s(s4, " : net pattern index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else if (loopControl[i].isdropout) {
            char s4[200];  sprintf_s(s4, " : net dropout index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else if (loopControl[i].isaddition) {
            char s4[200];  sprintf_s(s4, " : net addition index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else if (loopControl[i].ismultiplication) {
            char s4[200];  sprintf_s(s4, " : net multiplication index = %i", loopControl[i].hidden_conv_index);
            s = s3 + (string)(": ") + loopControl[i].id + " : " + loopControl[i].mode + s4;
        }
        else {
            char s4[200];  sprintf_s(s4, ": num nodes = %i", (loopControl[i].end_index - loopControl[i].start_index));
            s = (string)(s3) + s4;
        }
        verifyStrings.push_back(s);
    }
    int non_convs = 0;
    if (convs.size() > 0) {
        int trainable_convs = 0;
        for (int i = 0; i < (int)convs.size(); i++) {
            if ((convs[i].mode == "Output") && (convs[i].data_type == IMAGE_CONV) && (convs[i].supportingConvIDs.size() == 0)) {
                non_convs++;
            }
            else if (convs[i].getTrainable() && ((convs[i].mode == "Hidden") || (convs[i].mode == "Output"))) {
                trainable_convs++;
            }
        }
        if (((int)convs.size() - non_convs) > 0) {
            char s[200];  sprintf_s(s, "Trainable Layers: %i (%i total)", trainable_convs, (int)convs.size());
            verifyStrings.push_back(s);
        }
    }
    if (num_outputs > 0) {
        int trainable_outputs = 0;
        for (int i = 0; i < num_outputs; i++) {
            if (onodes[i].trainable) {
                trainable_outputs++;
            }
        }
        char s[200];  sprintf_s(s, "Trainable Output Nodes: %i (%i total)", trainable_outputs, num_outputs);
        verifyStrings.push_back(s);
    }
    if (num_hidden_nodes > 0) {
        int trainable_hiddens = 0;
        for (int i = 0; i < num_hidden_nodes; i++) {
            if (hnodes[i].trainable) {
                trainable_hiddens++;
            }
        }
        char s[200];  sprintf_s(s, "Trainable Hidden Nodes: %i (%i total)", trainable_hiddens, num_hidden_nodes);
        verifyStrings.push_back(s);
    }
    if (((int)convs.size() - non_convs) > 0) {
        if (convInputs.size() > 0) {
            char s[200];  sprintf_s(s, "Num Convolution Inputs = %i", (int)convInputs.size());
            verifyStrings.push_back(s);
        }
        if (num_active_convs > 0) {
            char s[200];  sprintf_s(s, "Num Active Convs = %i", num_active_convs);
            verifyStrings.push_back(s);
        }
        if (convs.size() > 0) {
            char s[200];  sprintf_s(s, "Num Convolutions = %i", (int)convs.size());
            verifyStrings.push_back(s);
        }
    }
    if (total_num_connects > 0) {
        char s[200];  sprintf_s(s, "Num Connections = %i", total_num_connects);
        verifyStrings.push_back(s);
    }
    if (num_outputs > 0) {
        char s[200];  sprintf_s(s, "Num Output Nodes = %i", num_outputs);
        verifyStrings.push_back(s);
    }
    if (num_hidden_nodes > 0) {
        char s[200];  sprintf_s(s, "Num Hidden Nodes = %i", num_hidden_nodes);
        verifyStrings.push_back(s);
    }
    if (nodeIndices.size() > 0) {
        char s[200];  sprintf_s(s, "Total Num Nodes = %i", (int)nodeIndices.size());
        verifyStrings.push_back(s);
    }
    if (num_inputs > 0) {
        char s[200];  sprintf_s(s, "Num Inputs = %i", num_inputs);
        verifyStrings.push_back(s);
    }
    verifyStrings.push_back("--- NetBuilder Initialization ---");
    verifyStrings.push_back("");
    return(s);
}

int NetBuilder::areNodesRemainingToProcess()
{
    int rval = 0;
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        if (!netData.getIsOutput(nodeIndices[i])) {
            if (!nodes_used[i]) {
                rval++;
            }
        }
    }
    return(rval);
}

int NetBuilder::areConvsRemainingToProcess()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (!convs[j].used && isActiveConvolution(j)) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::areHiddenNetsRemainingToProcess()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (!convs[j].used && (convs[j].data_type == HIDDEN_NET)) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::areAttentionsRemainingToProcess()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (!convs[j].used && (convs[j].data_type == ATTENTION)) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::areSelfAttentionsRemainingToProcess()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (!convs[j].used && (convs[j].data_type == SELFATTENTION)) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::areNetClustersRemainingToProcess()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (!convs[j].used && (convs[j].data_type == NETCLUSTER)) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::areDropoutsRemainingToProcess()
{
    int rval = 0;
    if (convs.size() > 0) {
        for (int j = 0; j < (int)convs.size(); j++) {
            if (!convs[j].used && (convs[j].data_type == DROPOUT)) {
                rval++;
            }
        }
    }
    return(rval);
}

int NetBuilder::areAdditionsRemainingToProcess()
{
    int rval = 0;
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        if (!netData.getIsOutput(nodeIndices[i]) && (netData.getNodeFunction(nodeIndices[i]) == "Add")) {
            // if the nodes haven't already been added
            if (!nodes_used[i]) {
                rval++;
            }
        }
    }
    return(rval);
}

int NetBuilder::areMultiplicationsRemainingToProcess()
{
    int rval = 0;
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        if (!netData.getIsOutput(nodeIndices[i]) && (netData.getNodeFunction(nodeIndices[i]) == "Multiply")) {
            // if the nodes haven't already been added
            if (!nodes_used[i]) {
                rval++;
            }
        }
    }
    return(rval);
}

bool NetBuilder::addHiddenNode(int index, int hnode_index, bool randomize)
{
    // Check to make sure we haven't overrun number of nodes we counted earlier
    if (hnode_index < num_hidden_nodes) {
        int net_data_index = nodeIndices[index];
        string mid = netData.getID(net_data_index);
        if (net_data_index >= 0) {
            // Set function type 
            string function = netData.getNodeFunction(net_data_index);
            int ntype = SIGMOID;
            if (function == "Relu") {
                ntype = RELU;
            }
            else if (function == "Tanh") {
                ntype = TANH;
            }
            else if (function == "Shockley") {
                ntype = SHOCKLEY;
            }
            else if (function == "Gaussian") {
                ntype = GAUSSIAN;
            }
            else if (function == "SoftMax") {
                ntype = SIGMOID;        // no hidden softmax nodes
            }
            else if (function == "MaxPool") {
                ntype = MAXPOOL;
            }
            else if (function == "Multiply") {
                ntype = MULTIPLY;
            }
            else if (function == "Add") {
                ntype = ADD;
            }
            // Get node inputs and outputs
            int nout = getNumOutputs(net_data_index);
            //char s[200];  sprintf_s(s, " : nout = %i", nout);  addErrorString(mid + s);
            vector<string> myinputs = nodeInputs[index];
            // Allocate node to net
            //char s[200];  sprintf_s(s, " : getNumInputNodes = %i", (int)myinputs.size());  addErrorString(mid + s);
            addPanel1String("Allocating node " + mid);
            hnodes[hnode_index].allocate((int)myinputs.size(), nout, ntype, netData.getNodeEta(net_data_index), netData.getNodeAlpha(net_data_index), netData.getNodeRand(net_data_index));
            // Set pointers to input data
            hnodes[hnode_index].bias = netData.getNodeBias(net_data_index);
            // Set some extra net parameters
            hnodes[hnode_index].setID(mid);  //addErrorString("addHiddenNode : " + hnodes[hnode_index].id);
            hnodes[hnode_index].trainable = netData.getNodeTrainable(net_data_index);
            hnodes[hnode_index].zeroout = netData.getNodeZeroOutput(net_data_index);
            addPanel1String("Setting weights for node " + mid);
            if (randomize) {
                hnodes[hnode_index].randomizeWeights();
            }
            else if (hnodes[hnode_index].bias == 0.0) {
                hnodes[hnode_index].randomizeWeights();
            }
            else {
                for (int j = 0; j < hnodes[hnode_index].num_inputs; j++) {
                    if (hnodes[hnode_index].weights[j] == 0.0) {
                        hnodes[hnode_index].randomizeWeights();
                        j = hnodes[hnode_index].num_inputs;
                    }
                }
            }
            addPanel1String("");
            return(true);
        }
        else {
            addErrorString("Unable to add node " + mid + " - unable to find hidden node in net data");
        }
    }
    else {
        char s[200];  sprintf_s(s, "addHiddenNode : hnode_index >= num_hidden_nodes : %i (%i)", hnode_index, num_hidden_nodes);
        addErrorString(s);
    }
    return(false);
}

bool NetBuilder::addOutputNode(int index, int onode_index, bool randomize)
{
    if (onode_index < num_outputs) {
        int net_data_index = nodeIndices[index];
        string mid = netData.getID(net_data_index);
        if (netData.isConvLayer(net_data_index)) {
            char s[200];  sprintf_s(s, ":%i", onode_index);  //if (onode_index < 10) { addErrorString("addOutputNode " + mid + s); }
            mid += s;
        }
        if (net_data_index < 0) {
            addErrorString(mid + " : " + netData.getStringAfterColon(mid));
            net_data_index = netData.getIndexFromID(netData.getStringBeforeColon(mid));
        }
        if (net_data_index >= 0) {
            // Set function type 
            string function = netData.getNodeFunction(net_data_index);
            int ntype = SIGMOID;
            if (function == "Relu") {
                ntype = RELU;
            }
            else if (function == "Tanh") {
                ntype = TANH;
            }
            else if (function == "Shockley") {
                ntype = SHOCKLEY;
            }
            else if (function == "Gaussian") {
                ntype = GAUSSIAN;
            }
            else if (function == "SoftMax") {
                ntype = SOFTMAX;
            }
            else if (function == "MaxPool") {
                ntype = MAXPOOL;
            }
            else if (function == "Multiply") {
                ntype = MULTIPLY;
            }
            else if (function == "Add") {
                ntype = ADD;
            }
            // Allocate node to net
            int ninputs = getNumInputsFromNetData(net_data_index);
            //vector<string> myinputs = nodeInputs[index];
            //if (onode_index < 10) { char s[200];  sprintf_s(s, " : ninputs = %i", ninputs);  addErrorString("addOutputNode " + mid + s); }
            onodes[onode_index].allocate(ninputs, 0, ntype, netData.getNodeEta(net_data_index), netData.getNodeAlpha(net_data_index), netData.getNodeRand(net_data_index));
            // Set pointers to input data
            onodes[onode_index].bias = netData.getBiasFromNetData(mid);
            // Set output data pointer
            onodes[onode_index].setTestOutputPointer((double*)(testoutputs + onode_index));
            // Set some extra net parameters
            onodes[onode_index].setID(nodeIDs[index]);  //addErrorString("addOutputNode : " + onodes[onode_index].id);
            onodes[onode_index].trainable = netData.getNodeTrainable(net_data_index);
            onodes[onode_index].zeroout = netData.getNodeZeroOutput(net_data_index);
            if (randomize) {
                onodes[onode_index].randomizeWeights();
            }
            else if (onodes[onode_index].bias == 0.0) {
                onodes[onode_index].randomizeWeights();
            }
            else {
                for (int j = 0; j < onodes[onode_index].num_inputs; j++) {
                    if (onodes[onode_index].weights[j] == 0.0) {
                        onodes[onode_index].randomizeWeights();
                        j = onodes[onode_index].num_inputs;
                    }
                }
            }
            return(true);
        }
        else {
            addErrorString("Unable to add node " + mid + " - unable to find output node in net data");
        }
    }
    return(false);
}

vector<string> NetBuilder::getSortedConvolutions()
{
    vector<string> rval;

    if (netConvs.size() > 0) {

        vector<string> cdata = getConvOrder();

        if (cdata.size() == netConvs.size()) {
            /*for (int i = 0; i < (int)cdata.size(); i++) {
                addErrorString("getSortedConvolutions : testing " + cdata[i] + " against " + netConvs[i].id);
            }*/
        }
        else {
            char s[200];  sprintf_s(s, "getSortedConvolutions : count error : cdata.size = %i : netConvs.size = %i", (int)cdata.size(), (int)netConvs.size());
            addErrorString(s);
        }

        for (int i = 0; i < (int)cdata.size(); i++) {
            rval.push_back(cdata[i]);
        }

    }

    return(rval);
}

bool NetBuilder::getConvGroups()
{
    if (netConvs.size() > 0) {
        vector<string> sorted_convs = getSortedConvolutions();
        convs.clear();
        for (int i = 0; i < (int)sorted_convs.size(); i++) {
            for (int j = 0; j < (int)netConvs.size(); j++) {
                if (sorted_convs[i] == netConvs[j].id) {
                    convToNewConv.push_back((int)convs.size());
                    convs.push_back(netConvs[j]);
                }
            }
        }
    }
    return(false);
}

int NetBuilder::getHiddenNets()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if ((convs[j].mode == "Hidden") && (convs[j].data_type == HIDDEN_NET)) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::getNetClusters()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (convs[j].data_type == NETCLUSTER) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::getDropouts()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (convs[j].data_type == DROPOUT) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::getAdditions()
{
    int rval = 0;
    for (int j = 0; j < netData.size(); j++) {
        if (netData.isNode(j) && (netData.getNodeMode(j) == "Hidden") && (netData.getNodeFunction(j) == "Add")) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::getMultiplications()
{
    int rval = 0;
    for (int j = 0; j < netData.size(); j++) {
        if (netData.isNode(j) && (netData.getNodeMode(j) == "Hidden") && (netData.getNodeFunction(j) == "Multiply")) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::getAttentionLayers()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (convs[j].data_type == ATTENTION) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::getSelfAttentionLayers()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (convs[j].data_type == SELFATTENTION) {
            rval++;
        }
    }
    return(rval);
}

int NetBuilder::getActiveConvs()
{
    int rval = 0;
    for (int j = 0; j < (int)convs.size(); j++) {
        if (isActiveConvolution(j)) {
            rval++;
        }
    }
    return(rval);
}

bool NetBuilder::isActiveConvolution(int cindex) 
{
    if (convs[cindex].mode == "Input") {
        return(false);
    }
    else if ((convs[cindex].mode == "Output") && (convs[cindex].data_type == DATA_CONV)) {
        return(false);
    }
    else if ((convs[cindex].mode == "Output") && (convs[cindex].data_type == IMAGE_CONV)) {
        if (convs[cindex].supportingConvIDs.size() == 0) {
            return(false);
        }
        else {
            vector<string> sconvs = netData.getImmediateSupportingConvNets(convs[cindex].id, true);
            for (int i = 0; i < (int)sconvs.size(); i++) {
                if (netData.getConvType(sconvs[i]) == HIDDEN_NET) {
                    return(false);
                }
            }
        }
    }
    else if ((convs[cindex].mode == "Hidden") && (convs[cindex].data_type == HIDDEN_NET)) {
        return(false);
    }
    else if (convs[cindex].data_type == ATTENTION) {
        return(false);
    }
    else if (convs[cindex].data_type == SELFATTENTION) {
        return(false);
    }
    else if (convs[cindex].data_type == NETCLUSTER) {
        return(false);
    }
    else if (convs[cindex].data_type == DROPOUT) {
        return(false);
    }
    return(true);
}

int NetBuilder::convertConvConnectionToIndex(string name, int hindex)
{
    int rval = -1;
    if (getConvFromConnectName(name)) {
        string cinput = netData.getConvNameFromConnectName(name);
        char color = netData.getConvColorFromConnectName(name);
        int cx = netData.getConvXFromConnectName(name);
        int cy = netData.getConvYFromConnectName(name);
        int size = newConvs[hindex].output_data_size;
        int offset = (cy * newConvs[hindex].outputdims[0]) + cx;
        if (color == 'g') {
            offset += (newConvs[hindex].outputdims[0] * newConvs[hindex].outputdims[1]);
        }
        else if (color == 'b') {
            offset += (newConvs[hindex].outputdims[0] * newConvs[hindex].outputdims[1]);
            offset += (newConvs[hindex].outputdims[0] * newConvs[hindex].outputdims[1]);
        }
        if (offset >= size) {
            char s2[200];  sprintf_s(s2, " : %i : %i", offset, size);
            string s = "Error : convertConvConnectionToIndex offset >= allocated size for " + name + s;
            addErrorString(s);
        }
        else {
            rval = offset;
        }
    }
    return(rval);
}

bool NetBuilder::softRestart()
{
    // Clear diagnostic strings
    verifyStrings.clear();

    if ((nodeIndices.size() == 0) && (netConvs.size() == 0)) {
        addErrorString("Unable to soft restart - problem with nodes / layers setup");
        return(false);
    }

    return(true);
}

void NetBuilder::randomizeNode(string id)
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (hnodes[i].id == id) {
            hnodes[i].randomizeWeights();
            return;
        }
    }
    for (int i = 0; i < num_outputs; i++) {
        if (onodes[i].id == id) {
            onodes[i].randomizeWeights();
            return;
        }
    }
}

void NetBuilder::randomizeConvLayer(string id)
{
    for (int i = 0; i < num_active_convs; i++) {
        if (newConvs[i].id == id) {
            newConvs[i].randomizeWeights();
            return;
        }
    }
    for (int i = 0; i < num_hidden_nets; i++) {
        if (hnets[i].id == id) {
            hnets[i].randomizeWeights();
            return;
        }
    }
}

int NetBuilder::getIndexFromHiddenID(string tid) {
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (tid == hnodes[i].id) {
            return(i);
        }
    }
    return(-1);
}

int NetBuilder::getIndexFromOutputID(string tid) {
    for (int i = 0; i < num_outputs; i++) {
        if (tid == onodes[i].id) {
            return(i);
        }
    }
    return(-1);
}

bool NetBuilder::areInputsCompleted(int index)
{
    //addErrorString("areInputsCompleted : " + nodeIDs[index]);
    for (int i = 0; i < (int)nodeInputs[index].size(); i++) {
        bool found = false;
        string input = nodeInputs[index][i];
        //addErrorString("areInputsCompleted : " + nodeIDs[index] + " : " + input);
        for (int j = 0; j < (int)netInputs.size(); j++) {
            //addErrorString("areInputsCompleted : " + nodeIDs[index] + " : " + input + " : " + netInputs[j].nodeId);
            if (input == netInputs[j].nodeId) {
                found = true;
                j = (int)netInputs.size();
            }
        }       
        if (!found) {
            string input2 = netData.getConvNameFromConnectName(input);
            //addErrorString("areInputsCompleted : checking input conv " + input2);
            int cindex = getConvIndexFromID(input2);
            if (cindex >= 0) {
                //addErrorString("areInputsCompleted : index found for input conv " + input2);
                if ((convs[cindex].getMode() == "Input") || convs[cindex].used) {
                    //addErrorString("areInputsCompleted : found for input conv " + input2);
                    found = true;
                }
                else if (!convs[cindex].used) {
                    //addErrorString("areInputsCompleted : not used for input conv " + input2);
                }
                else {
                    //addErrorString("areInputsCompleted : not input for input conv " + input2);
                }
            }
            else {
                //addErrorString("areInputsCompleted : index not found for input conv " + input2);
            }
        }
        if (!found) {
            for (int j = 0; j < (int)hiddenNodeIndices.size(); j++) {
                if (input == netData.getID(hiddenNodeIndices[j])) {
                    if (convertHiddenToAll[j] < 0) {
                        addErrorString("error0");
                        return(false);
                    }
                    if (convertHiddenToAll[j] >= (int)nodes_used.size()) {
                        addErrorString("error1");
                        return(false);
                    }
                    if (!nodes_used[convertHiddenToAll[j]]) {
                        return(false);
                    }
                    else {
                        found = true;
                        j = (int)hiddenNodeIndices.size();
                    }
                }
            }
        }
        if (!found) {
            return(false);
        }
    }
    return(true);
}

bool NetBuilder::areConvInputsCompleted(int index)
{
    if ((index >= 0) && (index < (int)convs.size())) {
        for (int i = 0; i < (int)convs[index].inputNodeIDs.size(); i++) {
            bool found = false;
            string input = convs[index].inputNodeIDs[i];
            // First check if input to convolution is an input
            for (int j = 0; j < (int)netInputs.size(); j++) {
                if (input == netInputs[j].nodeId) {
                    found = true;
                    j = (int)netInputs.size();
                }
            }
            if (!found) {
                // Then check if input to convolution is another convolution
                string input2 = netData.getConvNameFromConnectName(input);
                int cindex = getConvIndexFromID(input2);
                if (cindex >= 0) {
                    if ((convs[cindex].getMode() == "Input") || convs[cindex].used) {
                        found = true;
                    }
                }
            }
            if (!found) {
                // And then check if input to convolution is node
                int nindex = -1;
                for (int j = 0; j < (int)nodeIndices.size(); j++) {
                    //addErrorString("areConvInputsCompleted : comparing " + input + " against " + netData.getID(nodeIndices[j]));
                    if (input == netData.getID(nodeIndices[j])) {
                        nindex = j;
                        j = (int)nodeIndices.size();
                    }
                }
                if ((nindex < (int)nodes_used.size()) && (nindex >= 0)) {
                    if (nodes_used[nindex]) {
                        found = true;
                    }
                }
            }
            if (!found) {
                if (netData.isProcessNode(input)) {
                    vector<string> snodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(input));
                    if (snodes.size() > 0) {
                        //addErrorString("areConvInputsCompleted found process node with supporting nodes");
                        for (int n = 0; n < (int)snodes.size(); n++) {
                            addErrorString(snodes[n]);
                        }
                        //found = true;
                    }
                    else {
                        vector<string> sconvs = netData.getImmediateSupportingConvNets(input, true);
                        if (sconvs.size() > 0) {
                            vector<string> input_fields = netData.getInputFieldsFromLayerName(sconvs[0]);
                            if (input_fields.size() > 0) {
                                bool all_fields_found = true;
                                for (int n = 0; n < (int)input_fields.size(); n++) {
                                    bool found2 = false;
                                    for (int j = 0; j < (int)netInputs.size(); j++) {
                                        string minput = netData.getStringAfterColon(netInputs[j].nodeId);
                                        if (input_fields[n] == minput) {
                                            found2 = true;
                                            j = (int)netInputs.size();
                                        }
                                    }
                                    if (!found2) {
                                        all_fields_found = false;
                                        n = (int)input_fields.size();
                                    }
                                }
                                if (all_fields_found) {
                                    found = true;
                                }
                            }
                        }
                    }
                }
            }
            if (!found) {
                return(false);
            }
        }
    }
    return(true);
}

bool NetBuilder::areOutputsCompleted(int index)
{
    string tnode = netData.getID(nodeIndices[index]);
    for (int i = 0; i < (int)nodeIndices.size(); i++) {
        if (i != index) {
            for (int j = 0; j < (int)nodeInputs[i].size(); j++) {
                string input = nodeInputs[i][j];
                if (tnode == input) {
                    if (!netData.getIsOutput(nodeIndices[i]) && !nodes_used[i]) {
                        return(false);
                    }
                }
            }
        }
    }
    return(true);
}

double* NetBuilder::getInputPointer(string input)
{
    static bool once = true;
    if (getConvFromConnectName(input)) {
        string cinput = netData.getConvNameFromConnectName(input);
        int cindex = getConvIndexFromID(cinput);
        if (cindex >= 0) {
            if ((convs[cindex].mode == "Input") && (convs[cindex].data_type == IMAGE_CONV)) {
                if (image_data_allocated) {
                    char color = netData.getConvColorFromConnectName(input);
                    int cx = netData.getConvXFromConnectName(input);
                    int cy = netData.getConvYFromConnectName(input);
                    int stride = convs[cindex].input_width;
                    int single = convs[cindex].input_width * convs[cindex].input_height;
                    int size = getConvImageMemorySize(cinput);// +getConvImageMemoryOffset(cinput);
                    int offset = (cy * stride) + cx;
                    if (color == 'g') {
                        offset += single;
                    }
                    else if (color == 'b') {
                        offset += single;
                        offset += single;
                    }
                    if (offset >= size) {
                        if (once) {
                            char s[200];  sprintf_s(s, "getInputPointer offset >= mem_size : %i : %i", offset, size);  addErrorString(s);
                            once = false;
                        }
                        return(inputs);
                    }
                    return(image_data + offset);
                }
                else {
                    addErrorString("getInputPointer : Error - " + input + " can't get input pointer - image memory not allocated");
                    return(inputs);
                }
            }
            else if ((convs[cindex].mode == "Input") && (convs[cindex].data_type == SOUND_CONV)) {
                if (wave_data_allocated) {
                    int stride = convs[cindex].input_width;
                    int cx = netData.getConvPreXFromConnectName(input);
                    int cy = netData.getConvXFromConnectName(input);
                    int offset = (cy * stride) + cx;
                    if (offset >= wave_mem_size) {
                        if (once) {
                            char s[200];  sprintf_s(s, "getInputPointer offset >= mem_size : %i : %i", offset, wave_mem_size);  addErrorString(s);
                            once = false;
                        }
                        return(inputs);
                    }
                    return(wave_data + offset);
                }
                else {
                    addErrorString("getInputPointer : Error - " + input + " can't get input pointer - sound memory not allocated");
                    return(inputs);
                }
            }
            else if ((convs[cindex].data_type == DATA_CONV) && ((convs[cindex].mode == "Input") || (convs[cindex].mode == "Output"))) {
                string input_field = netData.getStringAfterColon(input);
                for (int j = 0; j < (int)netInputs.size(); j++) {
                    if (input_field == netInputs[j].nodeId) {
                        return(inputs + j);
                    }
                }
            }
            else {
                int cdata_type = convs[cindex].data_type;
                if (cdata_type == HIDDEN_NET) {
                    int hindex = -1;
                    for (int i = 0; i < nhidden_net_index; i++) {
                        if (hnets[i].id == cinput) {
                            hindex = i;
                            i = nhidden_net_index;
                        }
                    }
                    if (hindex < 0) {
                        addErrorString("getInputPointer : unable to find hidden net for " + input);
                        return(inputs);
                    }
                    int cx = netData.getConvPreXFromConnectName(input);
                    //char s[200]; sprintf_s(s, "%i", cx);  addErrorString("setting input pointer : " + input + " : " + s);
                    return(hnets[hindex].getOutputPointer(cx));
                }
                else if (cdata_type == ATTENTION) {
                    int hindex = -1;
                    for (int i = 0; i < nattention_layer_index; i++) {
                        if (attentions[i].id == cinput) {
                            hindex = i;
                            i = nattention_layer_index;
                        }
                    }
                    if (hindex < 0) {
                        addErrorString("getInputPointer : unable to find attention for " + input);
                        return(inputs);
                    }
                    int cx = netData.getConvPreXFromConnectName(input);
                    return(attentions[hindex].getOutputPointer(cx));
                }
                else if (cdata_type == SELFATTENTION) {
                    int hindex = -1;
                    for (int i = 0; i < nself_attention_layer_index; i++) {
                        if (selfattentions[i].id == cinput) {
                            hindex = i;
                            i = nself_attention_layer_index;
                        }
                    }
                    if (hindex < 0) {
                        addErrorString("getInputPointer : unable to find self attention for " + input);
                        return(inputs);
                    }
                    int cx = netData.getConvPreXFromConnectName(input);
                    return(selfattentions[hindex].getOutputPointer(cx));
                }
                else {
                    if ((cdata_type != DATA_CONV) && (cdata_type != IMAGE_CONV)) {
                        addErrorString("getInputPointer : unable to process for convolution data type for " + input);
                        return(inputs);
                    }
                    int hindex = -1;
                    for (int i = 0; i < nactive_index; i++) {
                        if (newConvs[i].id == cinput) {
                            hindex = i;
                            i = nactive_index;
                        }
                    }
                    if (hindex < 0) {
                        addErrorString("getInputPointer : unable to find convolution for " + input);
                        return(inputs);
                    }
                    if (cdata_type == IMAGE_CONV) {
                        int offset = convertConvConnectionToIndex(input, hindex);
                        if ((offset >= 0) && (offset < newConvs[hindex].output_data_size)) {
                            double* pointer = newConvs[hindex].output_data + offset;
                            return(pointer);
                        }
                        else {
                            char s[200];  sprintf_s(s, " : offset = %i : output_data_size = %i", offset, newConvs[hindex].output_data_size);
                            addErrorString("getInputPointer : image conv offset problem for input " + input + s);
                            return(inputs);
                        }
                    }
                    else if (cdata_type == DATA_CONV) {
                        int cx = netData.getConvPreXFromConnectName(input);
                        int offset = cx;
                        if ((offset >= 0) && (offset < newConvs[hindex].output_data_size)) {
                            double* pointer = newConvs[hindex].output_data + offset;
                            return(pointer);
                        }
                        else {
                            char s[200];  sprintf_s(s, " : offset = %i : output_data_size = %i", offset, newConvs[hindex].output_data_size);
                            addErrorString("getInputPointer : data conv offset problem for input " + input + s);
                            return(inputs);
                        }
                    }
                    else {
                        addErrorString("getInputPointer : unable to get input pointer for " + input);
                        return(inputs);
                    }
                }
            }
        }
        else {
            addErrorString("getInputPointer : unable to get input pointer for " + cinput);
            return(inputs);
        }
    }
    else {
        for (int k = 0; k < num_hidden_nodes; k++) {
            if (input == hnodes[k].id) {
                return(hnodes[k].getOutputPointer());
            }
        }
        for (int j = 0; j < (int)netInputs.size(); j++) {
            if (input == netInputs[j].nodeId) {
                return(inputs + j);
            }
        }
    }
    if (once) {
        addErrorString("NetBuilder::getInputPointer : " + input + " unable to find input pointer");
        once = false;
    }
    return(inputs);
}

boolIntDoublePointer NetBuilder::getImageNodePointerFromInputs(string input)
{
    boolIntDoublePointer rval;
    rval.valid = false;
    rval.count = 0;
    rval.input = image_data;
    if (getConvFromConnectName(input)) {
        string cinput = netData.getConvNameFromConnectName(input);
        int cindex = getConvIndexFromID(cinput);
        if (cindex >= 0) {
            if (convs[cindex].mode == "Input") {
                if (convs[cindex].data_type == IMAGE_CONV) {
                    if (image_data_allocated) {
                        rval.valid = true;
                        rval.count = image_mem_size;
                        rval.input = image_data;
                        return(rval);
                    }
                    else {
                        addErrorString("getImageNodePointerFromInputs : Error - " + input + " can't get input pointer - image memory not allocated");
                    }
                }
            }
        }
    }
    return(rval);
}

boolIntDoublePointer NetBuilder::getSoundNodePointerFromInputs(string input)
{
    boolIntDoublePointer rval;
    rval.valid = false;
    rval.count = 0;
    rval.input = wave_data;
    if (getConvFromConnectName(input)) {
        string cinput = netData.getConvNameFromConnectName(input);
        int cindex = getConvIndexFromID(cinput);
        if (cindex >= 0) {
            if (convs[cindex].mode == "Input") {
                if (convs[cindex].data_type == SOUND_CONV) {
                    if (wave_data_allocated) {
                        rval.valid = true;
                        rval.count = wave_mem_size;
                        rval.input = wave_data;
                        return(rval);
                    }
                    else {
                        addErrorString("getSoundNodePointerFromInputs : Error - " + input + " can't get input pointer - wave memory not allocated");
                    }
                }
            }
        }
    }
    return(rval);
}

boolIntVectorStringDoublePointer NetBuilder::isInputProcessNode(string input)
{
    boolIntVectorStringDoublePointer rval;
    rval.valid = false;
    rval.count = 0;
    rval.inputs.clear();
    rval.pointer = inputs;
    int nindex = netData.getIndexFromID(input);
    if (nindex >= 0) {
        if (netData.isProcessNode(nindex)) {
            if (netData.getNodePreProcessor(nindex).substr(0, 3) == "FFT") {
                string process = netData.getNodePreProcessor(nindex);
                rval.count = myStoi(process.substr(3));
                rval.valid = true;
                return(rval);
            }
            else if (netData.getNodePreProcessor(nindex) == "NormLinear") {
                vector<string> sconvs = netData.getImmediateSupportingConvNets(nindex);
                if (sconvs.size() > 0) {
                    for (int i = 0; i < (int)sconvs.size(); i++) {
                        int cindex = netData.getIndexFromID(sconvs[i]);
                        if (netData.getConvType(cindex) == DATA_CONV) {
                            if (netData.getNodeMode(cindex) == "Input") {
                                for (int j = 0; j < (int)netInputs.size(); j++) {
                                    if (netData.getStringBeforeColon(netInputs[j].nodeId) == input) {
                                        rval.inputs.push_back(netInputs[j].nodeId);
                                        rval.valid = true;
                                    }
                                }
                            }
                        }                    
                    }
                }
            }
            else if (netData.getNodePreProcessor(nindex) == "NormLog") {
                vector<string> sconvs = netData.getImmediateSupportingConvNets(nindex);
                if (sconvs.size() > 0) {
                    for (int i = 0; i < (int)sconvs.size(); i++) {
                        int cindex = netData.getIndexFromID(sconvs[i]);
                        if (netData.getConvType(cindex) == DATA_CONV) {
                            if (netData.getNodeMode(cindex) == "Input") {
                                for (int j = 0; j < (int)netInputs.size(); j++) {
                                    if (netData.getStringBeforeColon(netInputs[j].nodeId) == input) {
                                        rval.inputs.push_back(netInputs[j].nodeId);
                                        rval.valid = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                addErrorString("isInputProcessNode : need processing for " + input + " with process " + netData.getNodePreProcessor(nindex));
            }
        }
    }
    return(rval);
}

boolDoublePointer NetBuilder::getInputPointerFromInput(string input)
{
    boolDoublePointer rval;
    rval.valid = false;
    rval.input = inputs;
    static bool once = true;
    if (getConvFromConnectName(input)) {
        string cinput = netData.getConvNameFromConnectName(input);
        int cindex = getConvIndexFromID(cinput);
        if (cindex >= 0) {
            if (convs[cindex].mode == "Input") {
                if (convs[cindex].data_type == IMAGE_CONV) {
                    if (image_data_allocated) {
                        if (netData.getConvYFromConnectName(cinput) >= 0) {
                            // In case image convolution name format
                            char color = netData.getConvColorFromConnectName(cinput);
                            int cx = netData.getConvXFromConnectName(cinput);
                            int cy = netData.getConvYFromConnectName(cinput);
                            int stride = convs[cindex].input_width;
                            int single = convs[cindex].input_width * convs[cindex].input_height;
                            int offset = (cy * stride) + cx;
                            if (color == 'g') {
                                offset += single;
                            }
                            else if (color == 'b') {
                                offset += single;
                                offset += single;
                            }
                            if ((offset >= 0) && (offset < image_mem_size)) {
                                rval.input = image_data + offset;
                                rval.valid = true;
                                return(rval);
                            }
                            if (once) {
                                char s[200];  sprintf_s(s, "getInputPointerFromInput offset >= mem_size : %i : %i", offset, image_mem_size);  addErrorString(s);
                                CString s2 = _T(""); s2.Format(_T("getInputPointerFromInput offset >= mem_size : %i : %i"), offset, image_mem_size);  AfxMessageBox(s2);
                                once = false;
                            }
                            rval.input = image_data;
                            return(rval);
                        }
                        else {
                            // In case other image name format
                            //AfxMessageBox(_T("getInputPointerFromInput : unhandled image format for input ") + (CString)input.c_str());
                            rval.input = image_data;
                            return(rval);
                        }
                    }
                    else {
                        addErrorString("getInputPointerFromInput : Error - " + input + " can't get input pointer - image memory not allocated");
                        return(rval);
                    }
                }
                else if (convs[cindex].data_type == SOUND_CONV) {
                    if (wave_data_allocated) {
                        if (netData.getConvXFromConnectName(input) >= 0) {
                            // In case sound convolution name format
                            int stride = convs[cindex].input_width;
                            int cx = netData.getConvPreXFromConnectName(input);
                            int cy = netData.getConvXFromConnectName(input);
                            int offset = (cy * stride) + cx;
                            if (offset >= wave_mem_size) {
                                if (once) {
                                    char s[200];  sprintf_s(s, "getInputPointerFromInput offset >= mem_size : %i : %i", offset, wave_mem_size);  addErrorString(s);
                                    once = false;
                                }
                                return(rval);
                            }
                            rval.input = wave_data + offset;
                            rval.valid = true;
                            return(rval);
                        }
                        else {
                            // In case other sound name format
                            //AfxMessageBox(_T("getInputPointerFromInput : unhandled sound format for input ") + (CString)input.c_str());
                            rval.input = wave_data;
                            return(rval);
                        }
                    }
                    else {
                        addErrorString("getInputPointerFromInput : Error - " + input + " can't get input pointer - sound memory not allocated");
                        return(rval);
                    }
                }
                else if (convs[cindex].data_type == DATA_CONV) {
                    string input_field = netData.getStringAfterColon(input);
                    for (int j = 0; j < (int)netInputs.size(); j++) {
                        //addErrorString(input_field + " : " + netInputs[j].nodeId);
                        if (input_field == netInputs[j].nodeId) {
                            rval.input = inputs + j;
                            rval.valid = true;
                            return(rval);
                        }
                    }
                }
            }
        }
        else {
            addErrorString("getInputPointerFromInput : unable to get input pointer for " + cinput);
            return(rval);
        }
    }
    else {
        for (int j = 0; j < (int)netInputs.size(); j++) {
            //addErrorString("getInputPointerFromInput : input = " + input + " : nodeId = " + netInputs[j].nodeId);
            if (input == netInputs[j].nodeId) {
                rval.input = inputs + j;
                rval.valid = true;
                return(rval);
            }
        }
    }
    /*if (once) {
        addErrorString("getInputPointerFromInput : " + input + " unable to find input pointer");
        once = false;
    }*/
    return(rval);
}

int NetBuilder::getConvIndexFromID(string id)
{
    //char s[200];  sprintf_s(s, "getConvIndexFromID : convs size = %i : ", (int)convs.size());  addErrorString(s + id);
    for (int i = 0; i < (int)convs.size(); i++) {
        //addErrorString("getConvIndexFromID : testing " + id + " : " + convs[i].getID());
        if (id == convs[i].getID()) {
            return(i);
        }
    }
    return(-1);
}

int NetBuilder::getConvImageMemorySize(string id)
{
    int rval = 0;
    int cindex = getConvIndexFromID(id);
    if (cindex >= 0) {
        if (convs[cindex].getMode() == "Output") {
            twoValues pt = netData.getInputImageSize();
            rval = pt.x * pt.y;
            if (!netData.getImageGray()) {
                rval = 3 * rval;
            }
        }
        else {
            rval = convs[cindex].input_width * convs[cindex].input_height;
            if (!convs[cindex].gray) {
                rval = 3 * rval;
            }
        }
    }
    return(rval);
}

int NetBuilder::getConvBackpropMemorySize()
{
    int rval = 0;
    for (int i = 0; i < nactive_index; i++) {
        if (newConvs[i].output_data_size > rval) {
            rval = newConvs[i].output_data_size;
        }
    }
    for (int i = 0; i < (int)convs.size(); i++) {
        int tval = convs[i].output_width * convs[i].output_height;
        if (!convs[i].gray) {
            tval = 3 * tval;
        }
        if (tval > rval) {
            rval = tval;
        }
    }
    return(rval);
}

bool NetBuilder::getConvGray(string id)
{
    int cindex = getConvIndexFromID(id);
    if (cindex >= 0) {
        return(convs[cindex].gray);
    }
    return(false);
}

void NetBuilder::clearAndDeallocate()
{
    NetCore::clearAndDeallocate();

    image_variables_initialized = false;

    has_image_conv = false;
    has_wave_conv = false;
    has_wave_fft_conv = false;
    has_series_conv = false;
    has_text_conv = false;
    csv_with_filename = false;
    csv_filename_header = "";
    csv_image_filenames.clear();

    if (convs.size() > 0) {
        for (int i = 0; i < (int)convs.size(); i++) {
            convs.clear();
        }
    }
    convs.clear();

    has_softmax = false;

    verifyStrings.clear();
    loopControl.clear();
    nodes_used.clear();
    newConvToConv.clear();
    convToNewConv.clear();

    //NetBuilderData.clear();
    nodeIDs.clear();
    nodeIndices.clear();
    if (nodeInputs.size() > 0) {
        for (int i = 0; i < (int)nodeInputs.size(); i++) {
            nodeInputs[i].clear();
        }
    }
    hiddenNodeIndices.clear();
    outputNodeIndices.clear();
    convertHiddenToAll.clear();
    convertOutputToAll.clear();
    nodeInputs.clear();
    convInputs.clear();
    netConvs.clear();

    outputconnectioncnt = 0;
    input_test_indices.clear();
    input_test_to_ndata_indices.clear();
    input_from_output_test_indices.clear();
    output_to_input_test_indices.clear();

    inputPointers.clear();

    image_mem_size = 0;
    image_backprop_size = 0;
    series_mem_size = 0;
    series_backprop_size = 0;
    series_input_width = 0;
    series_input_height = 0;
    wave_mem_size = 0;
    num_inputs = 0;
    num_hidden_nodes = 0;
    num_active_convs = 0;
    nactive_index = 0;
    num_outputs = 0;
    conv_output_test_index = -1;
    image_variables_initialized = false;
    image_gray = false;
    image_width = 0;
    image_height = 0;
    image_single = 0;
    image_max_size = 0;
    conv_variables_initialized = false;
    conv_gray = false;
    conv_numx = 0;
    conv_numy = 0;
    conv_single = 0;
    conv_max_size = 0;

    sound_data_process_type = NONE;
    sound_process_stride = 0;
    sound_down_sample = 1;
    sound_data_stride = 1;
    process_sound_ffts = false;
    soundfftnames.clear();
    soundfftprocess = "";
    soundfftstride = 1;
    soundfftdownsample = 1;
    normaiize_file_set = false;

    accumulative_error = 0.0;
    accumulative_error_slope = 0;
    accumulative_validation_error = 0.0;
    errorFiltercount = 1000;
    validationfiltercount = 1000;
    for (int i = 0; i < 10; i++) {
        avgneterrors[i] = 0;
    }

}

double NetBuilder::getWeight(string lbl, int input)
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (lbl == hnodes[i].id) {
            if (input < hnodes[i].num_inputs) {
                return((double)(hnodes[i].weights[input]));
            }
            else {
                return(0);
            }
        }
    }
    for (int i = 0; i < num_outputs; i++) {
        if (lbl == onodes[i].id) {
            if (input < onodes[i].num_inputs) {
                return((double)(onodes[i].weights[input]));
            }
            else {
                return(0);
            }
        }
    }
    return(0);
}

double NetBuilder::getBias(string lbl)
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (lbl == hnodes[i].id) {
            return((double)(hnodes[i].bias));
        }
    }
    for (int i = 0; i < num_outputs; i++) {
        if (lbl == onodes[i].id) {
            return((double)(onodes[i].bias));
        }
    }
    return(0);
}

netWeightData NetBuilder::getWeights()
{
    netWeightData rval;
    if (allocated) {
#ifndef GCC
        CStatus sdlg;
        sdlg.CreateDlg();
        sdlg.setLabel("Getting weights from net : hidden nodes");
        sdlg.setProgress(0);
#endif
        addPanel1String("Getting weights from net : hidden nodes");
        for (int i = 0; i < num_hidden_nodes; i++) {
            rval.hiddennodeweightdata.push_back(hnodes[i].getWeights());
        }
#ifndef GCC
        sdlg.setLabel("Getting weights from net : output nodes");
        sdlg.setProgress(25);
#endif
        addPanel1String("Getting weights from net : output nodes");
        for (int i = 0; i < num_outputs; i++) {
            rval.outputnodeweightdata.push_back(onodes[i].getWeights());
        }
#ifndef GCC
        sdlg.setLabel("Getting weights from net : conv layers");
        sdlg.setProgress(50);
#endif
        addPanel1String("Getting weights from net : conv layers");
        for (int i = 0; i < num_active_convs; i++) {
            rval.convweightdata.push_back(newConvs[i].getWeights());
        }
#ifndef GCC
        sdlg.setLabel("Getting weights from net : hidden nets");
        sdlg.setProgress(75);
#endif
        addPanel1String("Getting weights from net : hidden nets");
        for (int i = 0; i < num_hidden_nets; i++) {
            rval.hiddenweightdata.push_back(hnets[i].getWeights());
        }
#ifndef GCC
        sdlg.DestroyDlg();
        addPanel1String("");
#endif
    }
    return(rval);
}

void NetBuilder::setWeights(netWeightData nwd)
{
    if (allocated) {
#ifndef GCC
        CStatus sdlg;
        sdlg.CreateDlg();
        sdlg.setLabel("Setting weights from net : hidden nodes");
        sdlg.setProgress(0);
#endif
        addPanel1String("Setting weights from net : hidden nodes");
        // Set hidden node weight data
        int i = 0;
        bool done = false;
        bool name_search = false;
        while (!done) {
            if ((i < num_hidden_nodes) && (i < (int)nwd.hiddennodeweightdata.size())) {
                if (nwd.hiddennodeweightdata[i].id == hnodes[i].id) {
                    addPanel1String(hnodes[i].id);
                    hnodes[i].setWeights(nwd.hiddennodeweightdata[i]);
                }
                else {
                    i = num_hidden_nodes;
                    name_search = true;
                    done = true;
                }
                i++;
            }
            else {
                done = true;
            }
        }
        if (name_search) {
            for (i = 0; i < (int)nwd.hiddennodeweightdata.size(); i++) {
                for (int j = 0; j < num_hidden_nodes; j++) {
                    if (nwd.hiddennodeweightdata[i].id == hnodes[j].id) {
                        addPanel1String(hnodes[j].id);
                        hnodes[j].setWeights(nwd.hiddennodeweightdata[i]);
                        j = num_hidden_nodes;
                    }
                }
            }
        }
        // Set output node weight data
#ifndef GCC
        sdlg.setLabel("Setting weights from net : output nodes");
        sdlg.setProgress(25);
#endif
        addPanel1String("Setting weights from net : output nodes");
        i = 0;
        done = false;
        name_search = false;
        while (!done) {
            if ((i < num_outputs) && (i < (int)nwd.outputnodeweightdata.size())) {
                if (nwd.outputnodeweightdata[i].id == onodes[i].id) {
                    addPanel1String(onodes[i].id);
                    onodes[i].setWeights(nwd.outputnodeweightdata[i]);
                }
                else {
                    i = num_outputs;
                    name_search = true;
                    done = true;
                }
                i++;
            }
            else {
                done = true;
            }
        }
        if (name_search) {
            for (i = 0; i < (int)nwd.outputnodeweightdata.size(); i++) {
                for (int j = 0; j < num_outputs; j++) {
                    if (nwd.outputnodeweightdata[i].id == onodes[j].id) {
                        addPanel1String(onodes[j].id);
                        onodes[j].setWeights(nwd.outputnodeweightdata[i]);
                        j = num_outputs;
                    }
                }
            }
        }
        // Set conv weight data
#ifndef GCC
        sdlg.setLabel("Setting weights from net : conv layers");
        sdlg.setProgress(50);
#endif
        addPanel1String("Setting weights from net : conv layers");
        i = 0;
        done = false;
        name_search = false;
        while (!done) {
            if ((i < num_active_convs) && (i < (int)nwd.convweightdata.size())) {
                if (nwd.convweightdata[i].id == newConvs[i].id) {
                    addPanel1String(newConvs[i].id);
                    newConvs[i].setWeights(nwd.convweightdata[i]);
                }
                else {
                    i = num_active_convs;
                    name_search = true;
                    done = true;
                }
                i++;
            }
            else {
                done = true;
            }
        }
        if (name_search) {
            for (i = 0; i < (int)nwd.convweightdata.size(); i++) {
                for (int j = 0; j < num_active_convs; j++) {
                    if (nwd.convweightdata[i].id == newConvs[j].id) {
                        addPanel1String(newConvs[j].id);
                        newConvs[j].setWeights(nwd.convweightdata[i]);
                        j = num_active_convs;
                    }
                }
            }
        }
        // Set hidden net weight data
#ifndef GCC
        sdlg.setLabel("Setting weights from net : hidden nets");
        sdlg.setProgress(75);
#endif
        addPanel1String("Setting weights from net : hidden nets");
        i = 0;
        done = false;
        name_search = false;
        while (!done) {
            if ((i < num_hidden_nets) && (i < (int)nwd.hiddenweightdata.size())) {
                if (nwd.hiddenweightdata[i].id == hnets[i].id) {
                    addPanel1String(hnets[i].id);
                    hnets[i].setWeights(nwd.hiddenweightdata[i]);
                }
                else {
                    i = num_hidden_nets;
                    name_search = true;
                    done = true;
                }
                i++;
            }
            else {
                done = true;
            }
        }
        if (name_search) {
            for (i = 0; i < (int)nwd.hiddenweightdata.size(); i++) {
                for (int j = 0; j < num_hidden_nets; j++) {
                    if (nwd.hiddenweightdata[i].id == hnets[j].id) {
                        addPanel1String(hnets[j].id);
                        hnets[j].setWeights(nwd.hiddenweightdata[i]);
                        j = num_hidden_nets;
                    }
                }
            }
        }
#ifndef GCC
        addPanel1String("");
        sdlg.DestroyDlg();
#endif
    }
}

double NetBuilder::rmsWeights(string lbl)
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (lbl == hnodes[i].id) {
            double cnt = (double)hnodes[i].num_inputs;
            if (cnt > 0) {
                double square = hnodes[i].squareWeights() - (hnodes[i].bias * hnodes[i].bias);
                square = square / cnt;
                square = sqrt(square);
                return(square);
            }
            i = num_hidden_nodes;
        }
    }
    for (int i = 0; i < num_outputs; i++) {
        if (lbl == onodes[i].id) {
            double cnt = (double)onodes[i].num_inputs;
            if (cnt > 0) {
                double square = onodes[i].squareWeights() - (onodes[i].bias * onodes[i].bias);
                square = square / cnt;
                square = sqrt(square);
                return(square);
            }
            i = num_outputs;
        }
    }
    return(0); 
}

double NetBuilder::rmsWeights()
{
    int cnt = 0;
    double squares = 0;
    for (int i = 0; i < num_hidden_nodes; i++) {
        cnt += (hnodes[i].num_inputs + 1);
        squares += hnodes[i].squareWeights();
    }
    for (int i = 0; i < num_outputs; i++) {
        cnt += (onodes[i].num_inputs + 1);
        squares += onodes[i].squareWeights();
    }
    for (int i = 0; i < num_active_convs; i++) {
        cnt += newConvs[i].getNumWeights();
        squares += newConvs[i].getWeightSquares();
    }
    for (int i = 0; i < num_hidden_nets; i++) {
        cnt += hnets[i].getNumWeights();
        squares += hnets[i].getWeightSquares();
    }
    for (int i = 0; i < num_net_clusters; i++) {
        cnt += nclusters[i].getNumWeights();
        squares += nclusters[i].getWeightSquares();
    }
    if (cnt > 0) {
        squares = squares / (double)cnt;
        squares = sqrt(squares);
    }
    return(squares);
}

double NetBuilder::maxWeights()
{
    double max = 0;
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (hnodes[i].maxWeights() > max) {
            max = hnodes[i].maxWeights();
        }
    }
    for (int i = 0; i < num_outputs; i++) {
        if (onodes[i].maxWeights() > max) {
            max = onodes[i].maxWeights();
        }
    }
    for (int i = 0; i < num_active_convs; i++) {
        double tmax = newConvs[i].maxWeights();
        if (tmax > max) {
            max = tmax;
        }
    }
    for (int i = 0; i < num_hidden_nets; i++) {
        double tmax = hnets[i].maxWeights();
        if (tmax > max) {
            max = tmax;
        }
    }
    for (int i = 0; i < num_net_clusters; i++) {
        double tmax = nclusters[i].maxWeights();
        if (tmax > max) {
            max = tmax;
        }
    }
    return(max);
}

string NetBuilder::maxWeightString()
{
    string rval = "";
    double max = 0.0;
    int max_index = -1;
    bool first = true;
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (hnodes[i].maxWeights() > max) {
            max = hnodes[i].maxWeights();
            max_index = i;
        }
    }
    if (max_index >= 0) {
        if (first) {
            rval += " :: Max Weights";
            first = false;
        }
        char s[200];  sprintf_s(s, " = %.2f", max);
        rval += (" : " + hnodes[max_index].id + s);
    }
    max = 0.0;
    max_index = -1;
    for (int i = 0; i < num_outputs; i++) {
        if (onodes[i].maxWeights() > max) {
            max = onodes[i].maxWeights();
            max_index = i;
        }
    }
    if (max_index >= 0) {
        if (first) {
            rval += " :: Max Weights";
            first = false;
        }
        char s[200];  sprintf_s(s, " = %.2f", max);
        rval += (" : " + onodes[max_index].id + s);
    }
    max = 0.0;
    max_index = -1;
    for (int i = 0; i < num_active_convs; i++) {
        double tmax = newConvs[i].maxWeights();
        if (tmax > max) {
            max = tmax;
            max_index = i;
        }
    }
    if (max_index >= 0) {
        if (first) {
            rval += " :: Max Weights";
            first = false;
        }
        char s[200];  sprintf_s(s, " = %.2f", max);
        rval += (" : " + newConvs[max_index].id + s);
    }
    max = 0.0;
    max_index = -1;
    for (int i = 0; i < num_hidden_nets; i++) {
        double tmax = hnets[i].maxWeights();
        if (tmax > max) {
            max = tmax;
            max_index = i;
        }
    }
    if (max_index >= 0) {
        if (first) {
            rval += " :: Max Weights";
            first = false;
        }
        char s[200];  sprintf_s(s, " = %.2f", max);
        rval += (" : " + hnets[max_index].id + s);
    }
    max = 0.0;
    max_index = -1;
    for (int i = 0; i < num_net_clusters; i++) {
        double tmax = nclusters[i].maxWeights();
        if (tmax > max) {
            max = tmax;
            max_index = i;
        }
    }
    if (max_index >= 0) {
        if (first) {
            rval += " :: Max Weights";
            first = false;
        }
        char s[200];  sprintf_s(s, " = %.2f", max);
        rval += (" : " + nclusters[max_index].id + s);
    }
    return(rval);
}

bool NetBuilder::checkValidWeights()
{
    for (int i = 0; i < num_hidden_nodes; i++) {
        if (!hnodes[i].checkValidWeights()) {
            return(false);
        }
    }
    for (int i = 0; i < num_outputs; i++) {
        if (!onodes[i].checkValidWeights()) {
            return(false);
        }
    }
    for (int i = 0; i < num_active_convs; i++) {
        if (!newConvs[i].checkValidWeights()) {
            return(false);
        }
    }
    for (int i = 0; i < num_hidden_nets; i++) {
        if (!hnets[i].checkValidWeights()) {
            return(false);
        }
    }
    for (int i = 0; i < num_net_clusters; i++) {
        if (!nclusters[i].checkValidWeights()) {
            return(false);
        }
    }
    return(true);
}

void NetBuilder::setParameters(double sleak, double rleak)
{
    sigmoid_leakage = sleak;
    for (int i = 0; i < num_hidden_nodes; i++) {
        hnodes[i].setParameters(sleak, rleak);
    }
    for (int i = 0; i < num_outputs; i++) {
        onodes[i].setParameters(sleak, rleak);
    }
    for (int i = 0; i < num_active_convs; i++) {
        newConvs[i].setParameters(sleak, rleak);
    }
    for (int i = 0; i < num_hidden_nets; i++) {
        hnets[i].setParameters(sleak, rleak);
    }
    for (int i = 0; i < num_net_clusters; i++) {
        nclusters[i].setParameters(sleak, rleak);
    }
}

void NetBuilder::setClusterParameter()
{
    for (int i = 0; i < num_net_clusters; i++) {
        int cindex = netData.getIndexFromID(nclusters[i].id);
        nclusters[i].setParameters2(netData.getClusterTestError(cindex),
            netData.getClusterSplitVariable(cindex), netData.getPatternSplitMethod(cindex));
    }
}

vector<int> NetBuilder::getClustersFromID(string cid)
{
    vector<int> rval;
#ifndef GCC
    for (int i = 0; i < num_net_clusters; i++) {
        if (nclusters[i].id == cid) {
            for (int j = 0; j < nclusters[i].num_dataset; j++) {
                rval.push_back((int)nclusters[i].cluster_nodes[j]);
            }
            return(rval);
        }
    }
#endif
    return(rval);
}

int NetBuilder::getNumClustersFromID(string cid)
{
#ifndef GCC
    for (int i = 0; i < num_net_clusters; i++) {
        if (nclusters[i].id == cid) {
            return(nclusters[i].num_outputs);
        }
    }
#endif
    return(0);
}

string NetBuilder::getClustersErrorString()
{
    string rval = "";
    for (int i = 0; i < num_net_clusters; i++) {
        rval += " : ";
        rval += nclusters[i].getErrorString();
    }
    return(rval);
}


//****************************************************************************
// netData helper functions

bool NetBuilder::isNumber(char data)
{
    if ((data >= 48) && (data <= 57)) {
        return(true);
    }
    else if ((data == 46) || (data == 43) || (data == 45)) {
        return(true);
    }
    return(false);
}

bool NetBuilder::isWordNumber(string word)
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



//****************************************************************************
// netData initialization functions (from CNetInitialization)

void NetBuilder::netDataInitializations()
{
    for (int i = 0; i < (int)netInputs.size(); i++) {
        netInputs[i].inputFields.clear();
    }
    netInputs.clear();
    netOutputs.clear();
    tweights.clear();
    tinputNodeIDs.clear();
    tinputConvIDs.clear();
    tprecedingConvIDs.clear();
    tfollowingConvIDs.clear();
    inputNodeIndices.clear();
    processNodeIndices.clear();
    hiddenNodeIndices.clear();
    outputNodeIndices.clear();
    connectIndices.clear();
    convLayerIndices.clear();
    convertHiddenToAll.clear();
    convertOutputToAll.clear();
    immediateSupportingNodes.clear();
    immediateFollowingNodes.clear();
    immediateSupportingConvs.clear();
    immediateFollowingConvs.clear();
}

void NetBuilder::getNetConnections()
{
#ifndef GCC
    int update_count = 0;
    CStatus progressDlg;
    progressDlg.setCancellable(false);
    progressDlg.CreateDlg();
    progressDlg.setLabel(_T("Getting net connections"));
    progressDlg.setProgress(0);
#endif
    addPanel1String("Getting net connections");

    for (int i = 0; i < (int)netData.size(); i++) {
        vector<int> initial1;
        immediateSupportingNodes.push_back(initial1);
        vector<int> initial2;
        immediateFollowingNodes.push_back(initial2);
        vector<int> initial3;
        immediateSupportingConvs.push_back(initial3);
        vector<int> initial4;
        immediateFollowingConvs.push_back(initial4);
    }

#ifndef GCC
    progressDlg.setProgress(10);
#endif

    for (int i = 0; i < (int)netData.size(); i++) {
        if (netData.isConnect(i)) {
            bool add = true;
            if (netData.hasSingleColon(netData.getInputNode(i))) {
                int mnum = netData.getSecondNumberAfterColon(netData.getInputNode(i));
                if (mnum > 0) {
                    add = false;
                }
            }
            if (add) {
                if (netData.hasSingleColon(netData.getOutputNode(i))) {
                    int mnum = netData.getSecondNumberAfterColon(netData.getOutputNode(i));
                    if (mnum > 0) {
                        add = false;
                    }
                }
                if (add) {
                    int input_index = netData.getIndexFromID(netData.getConvNameFromConnectName(netData.getInputNode(i)));
                    int output_index = netData.getIndexFromID(netData.getConvNameFromConnectName(netData.getOutputNode(i)));
                    if ((input_index >= 0) && (input_index < (int)netData.size()) && (output_index >= 0) && (output_index < (int)netData.size())) {
                        if (netData.isNode(input_index)) {
                            immediateSupportingNodes[output_index].push_back(input_index);
                        }
                        else if (netData.isConvLayer(input_index) || netData.isAttentionLayer(input_index) || netData.isDropout(input_index)) {
                            immediateSupportingConvs[output_index].push_back(input_index);
                        }
                        if (netData.isNode(output_index)) {
                            immediateFollowingNodes[input_index].push_back(output_index);
                        }
                        else if (netData.isConvLayer(output_index)) {
                            immediateFollowingConvs[input_index].push_back(output_index);
                        }
                    }
                    else if (input_index < 0) {
                        addErrorString("getNetConnections : unable to get input connection " + netData.getInputNode(i) + " for " + netData.getID(i));
                        return;
                    }
                    else if (input_index >= (int)netData.size()) {
                        addErrorString("getNetConnections : unable to get input connection " + netData.getInputNode(i) + " for " + netData.getID(i));
                        return;
                    }
                    else if (output_index < 0) {
                        addErrorString("getNetConnections : unable to get output connection " + netData.getOutputNode(i) + " for " + netData.getID(i));
                        return;
                    }
                    else if (output_index >= (int)netData.size()) {
                        addErrorString("getNetConnections : unable to get output connection " + netData.getOutputNode(i) + " for " + netData.getID(i));
                        return;
                    }
                }
            }
        }
#ifndef GCC
        update_count++;
        if (update_count > 20) {
            if (netData.size() > 0) {
                progressDlg.setProgress(10 + (90 * (i / (int)netData.size())));
            }
            update_count = 0;
        }
#endif
    }

#ifndef GCC
    progressDlg.DestroyDlg();
    addPanel1String("");
#endif

}

nodeStructure NetBuilder::getNodeIndices()
{
    int dcnt = 0;
    int dupdate = (int)netData.size() / 1000;
    if (dupdate < 1) {
        dupdate = 1;
    }
    addPanel1String("Getting nodes");
    nodeStructure rval;
    for (int i = 0; i < (int)netData.size(); i++) {
        if (netData.isNode(i)) {
            if (netData.getNodeMode(i) == "Input") {
                inputNodeIndices.push_back(i);
            }
            else if (netData.getNodeMode(i) == "PreProcess") {
                processNodeIndices.push_back(i);
            }
            else if ((netData.getNodeMode(i) == "Hidden") || (netData.getNodeMode(i) == "Output")) {
                if (netData.getNodeMode(i) == "Hidden") {
                    rval.indices.push_back(i);
                    rval.ids.push_back(netData.getID(i));
                    hiddenNodeIndices.push_back(i);
                    convertHiddenToAll.push_back((int)rval.indices.size() - 1);
                }
                else if (netData.getNodeMode(i) == "Output") {
                    if (netData.getGroupIndex(i) == -1) {
                        //addErrorString("getNodeIndices : adding " + netData.getID(i));
                        rval.indices.push_back(i);
                        rval.ids.push_back(netData.getID(i));
                        outputNodeIndices.push_back(i);
                        convertOutputToAll.push_back((int)rval.indices.size() - 1);
                    }
                }
            }
        }
        else if (netData.isConnect(i)) {
            connectIndices.push_back(i);
        }
        else if (netData.isConvLayer(i)) {
            convLayerIndices.push_back(i);
            if (netData.getNodeMode(i) == "Output") {
                if (netData.getConvType(i) == IMAGE_CONV) {
                    bool add = false;
                    vector<int> supporting = immediateSupportingNodes[i];
                    if (supporting.size() > 0) {
                        add = true;
                    }
                    else {
                        vector<int> supporting2 = immediateSupportingConvs[i];
                        if (supporting2.size() > 0) {
                            for (int j = 0; j < (int)supporting2.size(); j++) {
                                if (netData.getConvType(supporting2[j]) == HIDDEN_NET) {
                                    j = (int)supporting2.size();
                                    add = true;
                                }
                            }
                        }
                    }
                    if (add) {
                        int image_size = netData.getOutputImageSize();  //getDataImageWidth
                        for (int j = 0; j < image_size; j++) {
                            char mid[200];  sprintf_s(mid, ":%i", j);
                            rval.indices.push_back(i);
                            rval.ids.push_back(netData.getID(i) + mid);
                        }
                    }
                }
                else if (netData.getConvType(i) == DATA_CONV) {
                    vector<string> fields = netData.getDataFields(i);
                    if (fields.size() > 0) {
                        for (int j = 0; j < (int)fields.size(); j++) {
                            rval.indices.push_back(i);
                            rval.ids.push_back(netData.getID(i) + ":" + fields[j]);
                        }
                    }
                }
            }
        }
        if (dcnt >= dupdate) {
            //s.Format(_T("Getting nodes (%i:%i)"), i, (int)netData.size());
            char s[200];  sprintf_s(s, "Getting nodes (%i:%i)", i, (int)netData.size());
            addPanel1String(s);
            dcnt = 0;
        }
        dcnt++;
    }
    if (rval.ids.size() != rval.indices.size()) {
        addErrorString("getNodeIndices : size error");
    }
    addPanel1String("");
    return(rval);
}

vector<string> NetBuilder::getInputsFromNetData(int index)
{
    vector<string> rval;
    vector<int> snodes = immediateSupportingNodes[index];
    if (snodes.size() > 0) {
        for (int i = 0; i < (int)snodes.size(); i++) {
            int nindex = snodes[i];
            string nid = netData.getID(nindex);
            if (netData.getNodeMode(nindex) == "Input") {
                rval.push_back(nid);
            }
            else if (netData.getNodeMode(nindex) == "PreProcess") {
                string process = netData.getNodePreProcessor(nindex);
                if (process.substr(0, 3) == "FFT") {
                    int size = myStoi(process.substr(3));
                    for (int m = 0; m < size; m++) {
                        // Check and add input node ids
                        char s[200];  sprintf_s(s, ":%i", m);
                        string nname = nid + s;
                        rval.push_back(nname);
                    }
                }
                else if (((process == "WordIndex") || (process == "Dictionary"))) {
                    addErrorString("WordIndex and Dictionary process not working");
                }
                else {
                    if (netData.isProcessNode(snodes[i])) {
                        vector<int> sconvs = immediateSupportingConvs[i];
                        if (sconvs.size() > 0) {
                            for (int m = 0; m < (int)sconvs.size(); m++) {
                                vector<string> input_fields = netData.getInputFieldsFromLayerName(sconvs[m]);
                                for (int n = 0; n < (int)input_fields.size(); n++) {
                                    string myinput = nid + ":" + input_fields[n];
                                    rval.push_back(myinput);
                                }
                            }
                        }
                        else {
                            rval.push_back(nid);
                        }
                    }
                    else {
                        rval.push_back(nid);
                    }
                }
            }
            else {
                rval.push_back(nid);
            }
        }
    }

    vector<int> sconvs = immediateSupportingConvs[index];
    if (sconvs.size() > 0) {
        for (int i = 0; i < (int)sconvs.size(); i++) {
            int cindex = sconvs[i];
            string cid = netData.getID(cindex);
            if (netData.getConvType(cindex) == DATA_CONV) {
                if (netData.getNodeMode(cindex) == "Input") {
                    vector<string> mfields = netData.getDataFields(cindex);
                    if (mfields.size() > 0) {
                        for (int q = 0; q < (int)mfields.size(); q++) {
                            string nname = cid + ":" + mfields[q];
                            rval.push_back(nname);
                        }
                    }
                }
                else {
                    int outsize = netData.getDataConvOutputSize(cindex);
                    //char s[200];  sprintf_s(s, "getInputsFromNetData data_conv not input  : outsize = %i : ", outsize); addErrorString(s + netData.getID(cindex));
                    for (int q = 0; q < outsize; q++) {
                        char s[200];  sprintf_s(s, ":%i", q);
                        string nname = cid + s;
                        rval.push_back(nname);
                    }
                }
            }
            else if (netData.getConvType(cindex) == IMAGE_CONV) {
                imageSizeData imageSize = netData.getImageSizeData(cindex);
                int mxin = imageSize.input_width;
                int myin = imageSize.input_height;
                // Determine if we should use conv inputs or full image imputs
                if (cindex >= 0) {
                    if (netData.getNodeMode(cindex) != "Input") {
                        mxin = netData.getImageConvOutputWidth(cindex);
                        myin = netData.getImageConvOutputHeight(cindex);
                    }
                }
                bool gray = netData.getImageGray();
                // Use this variable to control number of nodes per conv step
                int color_cnt = 3;		// 3 for color
                if (gray) {
                    color_cnt = 1;		// 1 for gray
                }
                // Add nodes for all colors
                for (int c2 = 0; c2 < color_cnt; c2++) {
                    // Set color adding m for memory layer
                    string color2 = ":y";
                    if (!gray) {
                        if (c2 == 0) {
                            color2 = ":r";
                        }
                        if (c2 == 1) {
                            color2 = ":g";
                        }
                        if (c2 == 2) {
                            color2 = ":b";
                        }
                    }
                    // Add node for each supporting conv step
                    for (int q = 0; q < myin; q++) {
                        for (int p = 0; p < mxin; p++) {
                            // Make supporting node name and save it
                            char s[200];  sprintf_s(s, ":%i:%i", p, q);
                            string nname = cid + color2 + s;
                            rval.push_back(nname);
                        }
                    }
                }
            }
            else if (netData.getConvType(cindex) == SOUND_CONV) {
                if (netData.getNodeMode(cindex) == "Input") {
                    int mxin = 5000;
                    vector<int> dims = netData.getConvDimensions(cindex);
                    vector<int> strides = netData.getConvStrides(cindex);
                    if ((dims.size() > 0) && (strides.size() > 1)) {
                        mxin = dims[0] * strides[1];
                        if (FULLY_CONNECTED_FOLLOWING) {
                            //char s[200];  sprintf_s(s, "getInputsFromNetData : mxin = %i : files_max = %i", mxin, wave_files_longest); addErrorString(s);
                            mxin = wave_files_longest;
                        }
                    }
                    else {
                        addErrorString("getInputsFromNetData : problem setting mxin");
                    }
                    int myin = 1;
                    for (int q = 0; q < myin; q++) {
                        for (int p = 0; p < mxin; p++) {
                            char s[200];  sprintf_s(s, ":%i:%i", p, q);
                            string nname = cid + s;
                            rval.push_back(nname);
                        }
                    }
                }
                else {
                    addErrorString("getInputsFromNetData : unable to process non-input sound layers : " + sconvs[i]);
                }
            }
            else if (netData.getConvType(cindex) == HIDDEN_NET) {
                int num_per_layer = netData.getConvNodesPerLayer(cindex);
                for (int q = 0; q < num_per_layer; q++) {
                    char s[200];  sprintf_s(s, ":%i", q);
                    string nname = cid + s;
                    rval.push_back(nname);
                }
            }
            else if (netData.getConvType(cindex) == ATTENTION) {
                int num_per_layer = netData.getAttentionOutputSize(cindex);
                for (int q = 0; q < num_per_layer; q++) {
                    char s[200];  sprintf_s(s, ":%i", q);
                    string nname = cid + s;
                    rval.push_back(nname);
                }
            }
            else if (netData.getConvType(cindex) == NETCLUSTER) {

                AfxMessageBox(_T("getInputsFromNetData for ") + (CString)netData.getID(index).c_str() + _T(" - need processing for ") + (CString)cid.c_str());

            }
            else if (netData.getConvType(cindex) == DROPOUT) { 
                int cnt = 0;
                vector<string> mnodes = netData.getImmediateSupportingNodes(cindex);
                for (int q = 0; q < (int)mnodes.size(); q++) {
                    char s[200];  sprintf_s(s, ":%i", cnt);
                    string nname = cid + s;
                    rval.push_back(nname);
                    cnt++;
                }
                vector<string> mconvs = netData.getImmediateSupportingLayers(cindex);
                if (mconvs.size() > 0) {
                    for (int j = 0; j < (int)mconvs.size(); j++) {
                        int mindex = netData.getIndexFromID(mconvs[j]);
                        if (mindex >= 0) {
                            if (netData.isHiddenNetOnly(mindex)) {
                                int num_per_layer = netData.getConvNodesPerLayer(mindex);
                                for (int q = 0; q < num_per_layer; q++) {
                                    char s[200];  sprintf_s(s, ":%i", cnt);
                                    string nname = cid + s;
                                    rval.push_back(nname);
                                    cnt++;
                                }
                            }
                            else if (netData.isAttentionLayer(mindex)) {
                                int num_per_layer = netData.getAttentionOutputSize(mindex);
                                for (int q = 0; q < num_per_layer; q++) {
                                    char s[200];  sprintf_s(s, ":%i", cnt);
                                    string nname = cid + s;
                                    rval.push_back(nname);
                                    cnt++;
                                }
                            }
                            else if (netData.getConvType(mindex) == DATA_CONV) {
                                if (netData.getNodeMode(mindex) == "Input") {
                                    vector<string> mfields = netData.getDataFields(mindex);
                                    if (mfields.size() > 0) {
                                        for (int q = 0; q < (int)mfields.size(); q++) {
                                            char s[200];  sprintf_s(s, ":%i", cnt);
                                            string nname = cid + s;
                                            rval.push_back(nname);
                                            cnt++;
                                        }
                                    }
                                }
                                else {
                                    int outsize = netData.getDataConvOutputSize(mindex);
                                    for (int q = 0; q < outsize; q++) {
                                        char s[200];  sprintf_s(s, ":%i", cnt);
                                        string nname = cid + s;
                                        rval.push_back(nname);
                                        cnt++;
                                    }
                                }
                            }
                            else if (netData.getConvType(mindex) == IMAGE_CONV) {
                                imageSizeData imageSize = netData.getImageSizeData(mindex);
                                int mxin = imageSize.input_width;
                                int myin = imageSize.input_height;
                                // Determine if we should use conv inputs or full image imputs
                                if (mindex >= 0) {
                                    if (netData.getNodeMode(mindex) != "Input") {
                                        mxin = netData.getImageConvOutputWidth(mindex);
                                        myin = netData.getImageConvOutputHeight(mindex);
                                    }
                                }
                                bool gray = netData.getImageGray();
                                // Use this variable to control number of nodes per conv step
                                int color_cnt = 3;		// 3 for color
                                if (gray) {
                                    color_cnt = 1;		// 1 for gray
                                }
                                // Add nodes for all colors
                                for (int c2 = 0; c2 < color_cnt; c2++) {
                                    // Add node for each supporting conv step
                                    for (int q = 0; q < myin; q++) {
                                        for (int p = 0; p < mxin; p++) {
                                            // Make supporting node name and save it
                                            char s[200];  sprintf_s(s, ":%i", cnt);
                                            string nname = cid + s;
                                            rval.push_back(nname);
                                            cnt++;
                                        }
                                    }
                                }
                            }
                            else if (netData.getConvType(mindex) == SOUND_CONV) {
                                if (netData.getNodeMode(mindex) == "Input") {
                                    int mxin = 5000;
                                    vector<int> dims = netData.getConvDimensions(mindex);
                                    vector<int> strides = netData.getConvStrides(mindex);
                                    if ((dims.size() > 0) && (strides.size() > 1)) {
                                        mxin = dims[0] * strides[1];
                                        if (FULLY_CONNECTED_FOLLOWING) {
                                            mxin = wave_files_longest;
                                        }
                                    }
                                    else {
                                        addErrorString("getInputsFromNetData : problem setting mxin");
                                    }
                                    int myin = 1;
                                    for (int q = 0; q < myin; q++) {
                                        for (int p = 0; p < mxin; p++) {
                                            char s[200];  sprintf_s(s, ":%i", cnt);
                                            string nname = cid + s;
                                            rval.push_back(nname);
                                            cnt++;
                                        }
                                    }
                                }
                                else {
                                    addErrorString("getInputsFromNetData : unable to process non-input sound layers : " + mconvs[j]);
                                }
                            }
                        }
                    }
                }
                //CString s2 = _T(""); s2.Format(_T("getInputsFromNetData : rval size = %i for "), (int)rval.size()); AfxMessageBox(s2 + (CString)netData.getID(index).c_str() + _T(" and ") + (CString)cid.c_str());
            }
            else {
                if (!isInArray(cid, rval)) {
                    rval.push_back(cid);
                }
            }
        }
    }

    /*addErrorString("getInputsFromNetData for " + netData.getID(index));
    for (int i = 0; i < (int)rval.size(); i++) {
        addErrorString("getInputsFromNetData for " + netData.getID(index) + " : " + rval[i]);
    }*/

    return(rval);
}

int NetBuilder::getNumOutputs(int index)
{
    int rval = 0;
    string node = netData.getConvNameFromConnectName(netData.getID(index));
    for (int i = 0; i < (int)netData.size(); i++) {
        if (netData.isConnect(i)) {
            string input = netData.getConvNameFromConnectName(netData.getInputNode(i));
            if (input == node) {
                int outdex = netData.getIndexFromID(netData.getConvNameFromConnectName(netData.getOutputNode(i)));
                if (netData.isNode(outdex)) {
                    rval++;
                }
                else if (netData.isConvLayer(outdex)) {
                    if (netData.getConvType(outdex) == DATA_CONV) {
                        if (netData.getNodeMode(outdex) == "Output") {
                            vector<string> fields = netData.getDataFields(outdex);
                            if (fields.size() > 0) {
                                rval += (int)fields.size();
                            }
                            else {
                                addErrorString("Error - getting number of component outputs unable to process for " + node + " to " + netData.getOutputNode(i) + " (1)");
                            }
                        }
                        else {
                            addErrorString("Error 0912764");
                        }
                    }
                    else if (netData.getConvType(outdex) == HIDDEN_NET) {
                        rval += netData.getConvNodesPerLayer(outdex);
                    }
                    else if (netData.getConvType(outdex) == IMAGE_CONV) {
                        if (netData.getNodeMode(outdex) == "Output") {
                            if (netData.isNode(index)) {
                                imageSizeData imageSize = netData.getImageSizeData(netData.getIndexFromID(netData.getConvNameFromConnectName(netData.getOutputNode(i))));
                                int msize = imageSize.output_width * imageSize.output_height;
                                rval += msize;
                            }
                            else if (netData.getConvType(index) == HIDDEN_NET) {
                                imageSizeData imageSize = netData.getImageSizeData(netData.getIndexFromID(netData.getConvNameFromConnectName(netData.getOutputNode(i))));
                                int msize = imageSize.output_width * imageSize.output_height;
                                rval += msize;
                            }
                            else {
                                addErrorString("getNumOutputs : error in getting outputs (1)");
                            }
                        }
                        else {
                            if (netData.isNode(index)) {
                                addErrorString("getNumOutputs : error in getting outputs (2)");
                            }
                            else if (netData.getConvType(index) == HIDDEN_NET) {
                                imageSizeData imageSize = netData.getImageSizeData(netData.getIndexFromID(netData.getConvNameFromConnectName(netData.getOutputNode(i))));
                                int msize = imageSize.input_width * imageSize.input_height;
                                //if (!netData.getImageGray()) {
                                    //msize = 3 * msize;
                                //}
                                rval += msize;
                            }
                            else {
                                addErrorString("getNumOutputs : error in getting outputs (4)");
                            }
                        }
                    }
                    else {
                        addErrorString("Error - getting number of component outputs unable to process for " + node + " to " + netData.getOutputNode(i) + " (2)");
                    }
                }
                else if (netData.isAttentionLayer(outdex)) {
                    if (netData.isSelfAttentionLayer(outdex)) {
                        int input_size = getNumberOfSelftAttentionLayerInputs(outdex);
                        rval += input_size;
                    }
                    else {
                        int input_size = (2 * netData.getDk(outdex) * netData.getD(outdex)) + (netData.getDv(outdex) * netData.getD(outdex));
                        rval += input_size;
                    }
                }
                else if (netData.isNetCluster(outdex)) {
                    int input_size = netData.getConvNodesPerLayer(outdex);
                    rval += input_size;
                }
                else if (netData.isDropout(outdex)) {
                    if (netData.isNode(index)) {
                        rval++;
                    }
                    else if (netData.isConvLayerOnly(index)) {
                        rval++;
                    }
                    else if (netData.isHiddenNetOnly(index)) {
                        rval += netData.getConvNodesPerLayer(index);
                    }
                    else if (netData.isAttentionLayerOnly(index)) {
                        rval += (netData.getDv(index) * netData.getD(index));
                    }
                    else if (netData.isSelfAttentionLayerOnly(index)) {
                        rval += (netData.getDv(index) * netData.getD(index));
                    }
                    else if (netData.isNetClusterOnly(index)) {
                        rval += netData.getConvNodesPerLayer(index);
                    }
                }
                else {
                    addErrorString("Error - getting number of component outputs unable to process for " + node + " to " + netData.getOutputNode(i) + " (3)");
                }
            }
        }
    }
    return(rval);
}

int NetBuilder::getNumberOfSelftAttentionLayerInputs(int index)
{
    vector<string> sconvs = netData.getImmediateSupportingConvNets(index);
    if (sconvs.size() == 1) {
        if (netData.isHiddenNetOnly(netData.getIndexFromID(sconvs[0]))) {
            int input_size = 0;
            for (int i = 0; i < (int)convs.size(); i++) {
                if (convs[i].id == sconvs[0]) {
                    input_size = convs[i].nodes;
                    i = (int)convs.size();
                }
            }
            if (input_size > 0) {
                int third = input_size / 3;
                if ((input_size % third) > 0) {
                    third++;
                }
                int kdim = (int)sqrt((double)third);
                int vdim = kdim;
                int mdim = kdim;
                int test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);
                if (test_num_inputs < input_size) {
                    vdim++;
                    test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);
                    if (test_num_inputs < input_size) {
                        mdim++;
                        test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);
                        if (test_num_inputs < input_size) {
                            kdim++;
                            test_num_inputs = 2 * (2 * kdim * mdim) + (vdim * mdim);
                        }
                    }
                }
                input_size = (2 * kdim * mdim) + (vdim * mdim);
                return(input_size);
            }
        }
        else {
            addErrorString("getNumberOfSelftAttentionLayerInputs : can only support hidden net input for " + netData.getID(index));
        }
    }
    else if (sconvs.size() > 1) {
        addErrorString("getNumberOfSelftAttentionLayerInputs : can't support more than one conv input for " + netData.getID(index));
    }
    else if (sconvs.size() < 1) {
        addErrorString("getNumberOfSelftAttentionLayerInputs : must have one conv input for " + netData.getID(index));
    }

    // following is the input calculation from self attention

    /*int third = input_size / 3;
    if ((input_size % third) > 0) {
        third++;
    }

    int kdim = (int)sqrt((double)third);
    int vdim = kdim;
    int mdim = kdim;

    int test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);

    if (test_num_inputs < input_size) {
        vdim++;
        test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);
        if (test_num_inputs < input_size) {
            mdim++;
            test_num_inputs = (2 * kdim * mdim) + (vdim * mdim);
            if (test_num_inputs < input_size) {
                kdim++;
                test_num_inputs = 2 * (2 * kdim * mdim) + (vdim * mdim);
            }
        }
    }*/

    //char s[200];  sprintf_s(s, "getNumOutputs : dk = %i : dv = %i : d = %i : ", netData.getDk(outdex), netData.getDv(outdex), netData.getD(outdex));
    //addErrorString(s + netData.getID(outdex));

    return(0);
}

vector<inputData> NetBuilder::getNetInputs()
{
    num_copy_outputs_to_inputs = 0;
    for (int i = 0; i < (int)netInputs.size(); i++) {
        netInputs[i].inputFields.clear();
    }
    netInputs.clear();
    netOutputs.clear();

    // Look for all nodes and convnets in net data
    for (int i = 0; i < (int)netData.size(); i++) {
        if (netData.getNodeMode(i) == "Input") {
            if (netData.isNode(i)) {
                bool add = true;
                vector<string> fnodes = netData.getImmediateFollowingNodes(i);
                if (fnodes.size() == 1) {
                    if (netData.getNodeMode(netData.getIndexFromID(fnodes[0])) == "PreProcess") {
                        add = false;
                    }
                }
                if (add) {
                    if (addToInputs(netData.getID(i))) {
                        addProcessToInput(i, (int)netInputs.size() - 1);
                        addFieldToInput(i, (int)netInputs.size()-1);
                    }
                }
            }
            else if (netData.isConvLayer(i)) {
                if (netData.getConvType(i) == DATA_CONV) {
                    bool add = true;
                    vector<string> fnodes = netData.getImmediateFollowingNodes(i);
                    if (fnodes.size() == 1) {
                        if (netData.getNodeMode(netData.getIndexFromID(fnodes[0])) == "PreProcess") {
                            add = false;
                        }
                    }
                    if (add) {
                        int offset = 0;
                        if (netData.getNodeIndexMode(i) == "Index") {
                            offset = netData.getNodeXIndex(i);
                        }
                        vector<string> mfields = netData.getDataFields(i);
                        if (mfields.size() > 0) {
                            int num_model = 1;
                            if (netData.getD(i) > 1) {
                                AfxMessageBox(_T("getNetInputs - need model processing for ") + (CString)netData.getID(i).c_str());
                            }
                            for (int q = 0; q < (int)mfields.size(); q++) {
                                if (addToInputs(mfields[q])) {
                                    addProcessToInput(i, (int)netInputs.size() - 1);
                                    netInputs[(int)netInputs.size() - 1].inputFields.clear();
                                    netInputs[(int)netInputs.size() - 1].inputFields.push_back(mfields[q]);
                                    netInputs[(int)netInputs.size() - 1].from_data_conv = true;     
                                    if (mfields[q].substr(0, 7) == "Output:") {
                                        num_copy_outputs_to_inputs++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (netData.getNodeMode(i) == "PreProcess") {
            if (netData.isNode(i)) {
                // If process node, check and save node ID
                if (netData.getNodePreProcessor(i).substr(0, 3) == "FFT") {
                    string process = netData.getNodePreProcessor(i);
                    int size = myStoi(process.substr(3));
#ifndef GCC
                    if (size != (int)_tstof((LPCTSTR)(CString)process.substr(3).c_str())) {
                        addErrorString("integer conversion error 7");
                    }
#endif
                    vector<string> snodes = netData.getImmediateSupportingNodes(i);
                    if ((int)snodes.size() == 1) {
                        int snindex = netData.getIndexFromID(snodes[0]);
                        for (int m = 0; m < size; m++) {
                            if (addIndexToInput(netData.getID(i), m)) {
                                addProcessToInput(i, ((int)netInputs.size() - 1));
                                addFftFieldToInput(snindex, ((int)netInputs.size() - 1), m);
                            }
                        }
                    }
                    else {
                        vector<string> sconvs = netData.getImmediateSupportingConvNets(i);
                        if ((int)sconvs.size() == 1) {
                            int snindex = netData.getIndexFromID(sconvs[0]);
                            for (int m = 0; m < size; m++) {
                                if (addIndexToInput(netData.getID(i), m)) {
                                    addProcessToInput(i, ((int)netInputs.size() - 1));
                                    if (netData.getConvType(snindex) == SOUND_CONV) {
                                        netInputs[((int)netInputs.size() - 1)].inputFields.clear();
                                        char p[200]; sprintf_s(p, "FFT%i:SoundFile", m);
                                        netInputs[((int)netInputs.size() - 1)].inputFields.push_back(p);
                                    }
                                    else {
                                        addFftFieldToInput(snindex, ((int)netInputs.size() - 1), m);
                                    }
                                }
                            }
                        }
                        else {
                            addErrorString("getNetInputs for " + netData.getID(i) + " unable to process multiple input fields for FFT");
                        }
                    }
                }
                else {
                    string nname = netData.getID(i);
                    vector<int> supporting = immediateSupportingConvs[i];
                    // Check/add name for process node with one input node
                    if (supporting.size() == 0) {
                        vector<string> snodes = netData.getImmediateSupportingNodes(i);
                        if ((int)snodes.size() == 1) {
                            int snindex = netData.getIndexFromID(snodes[0]);
                            if (addToInputs(nname)) {
                                addProcessToInput(i, (int)netInputs.size() - 1);
                                addFieldToInput(snindex, (int)netInputs.size() - 1);
                            }
                        }
                        else {
                            addErrorString("getNetInputs for " + netData.getID(i) + " unable process input fields");
                        }
                    }
                    // Check/add name for process node with one input layer
                    else if (supporting.size() == 1) {
                        int sindex = supporting[0];
                        if (sindex >= 0) {
                            int offset = 0;
                            if (netData.getNodeIndexMode(sindex) == "Index") {
                                offset = netData.getNodeXIndex(sindex);
                            }
                            vector<string> mfields = netData.getDataFields(sindex);
                            if (mfields.size() > 0) {
                                int num_model = 1;
                                if (netData.getD(sindex) > 1) {
                                    num_model = netData.getD(sindex);
                                }
                                for (int q = 0; q < (int)mfields.size(); q++) {
                                    for (int p = 0; p < num_model; p++) {
                                        string mfield = mfields[q];
                                        if (p > 0) {
                                            char s[200];  sprintf_s(s, ":m%i", p);
                                            mfield += s;
                                        }
                                        string mname = nname + ":" + mfield;
                                        //addErrorString("getNetInputs : tp1 : " + netData.getID(i) + " : " + mname); int here2;
                                        if (addToInputs(mname)) {
                                            addProcessToInput(i, (int)netInputs.size() - 1);
                                            netInputs[(int)netInputs.size() - 1].inputFields.clear();
                                            netInputs[(int)netInputs.size() - 1].inputFields.push_back(mfield);
                                            netInputs[(int)netInputs.size() - 1].from_data_conv = true;     
                                            //addErrorString("getNetInputs : " + mname+ " : " + mfields[q]);
                                            if (mfields[q].substr(0, 7) == "Output:") {
                                                num_copy_outputs_to_inputs++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    // Show error
                    else {
                        addErrorString("Process node " + nname + " can't support mulitiple input layers");
                    }
                }
            }
            else if (netData.isConvLayer(i)) {
                // nothing here
            }
        }
        else if (netData.getNodeMode(i) == "Output") {
            if (netData.isNode(i)) {
                if (addToOutputs(netData.getID(i))) {
                    addFieldToOutput(i, (int)netOutputs.size() - 1);
                }
            }
            else if (netData.isConvLayer(i)) {
                if (netData.getConvType(i) == DATA_CONV) {
                    vector<string> fields = netData.getDataFields(i);
                    if (fields.size() > 0) {
                        int offset = 0;
                        if (netData.getNodeIndexMode(i) == "Index") {
                            offset = netData.getNodeXIndex(i);
                        }
                        for (int j = 0; j < (int)fields.size(); j++) {
                            string mid = netData.getID(i) + ":" + fields[j];
                            if (addToOutputs(mid)) {
                                netOutputs[(int)netOutputs.size() - 1].outputField = fields[j];
                                netOutputs[(int)netOutputs.size() - 1].indexOffset = offset;
                            }
                        }
                    }
                }
                else if (netData.getConvType(i) == IMAGE_CONV) {
                    // Check to see if output image is supported ny nodes or hidden net
                    //  If so, image is made up of nodes unstead of an output convolution
                    bool add = false;
                    vector<int> supporting = immediateSupportingNodes[i];
                    if (supporting.size() > 0) {
                        add = true;
                    }
                    else {
                        vector<int> supporting2 = immediateSupportingConvs[i];
                        if (supporting2.size() > 0) {
                            for (int j = 0; j < (int)supporting2.size(); j++) {
                                if (netData.getConvType(supporting2[j]) == HIDDEN_NET) {
                                    j = (int)supporting2.size();
                                    add = true;
                                }
                            }
                        }
                    }
                    if (add) {
#ifndef GCC
                        int update_count = 0;
                        CStatus progressDlg;
                        progressDlg.setCancellable(false);
                        progressDlg.CreateDlg();
                        progressDlg.setLabel(_T("Building nodes for image output"));
                        progressDlg.setProgress(0);
#endif
                        addPanel1String("Building nodes for image output");
                        int image_size = netData.getOutputImageSize();
                        // Add nodes for all colors
                        for (int j = 0; j < image_size; j++) {
                            char s[200];  sprintf_s(s, ":%i", j);
                            string mid = netData.getID(i) + s;
                            if (addToOutputs(mid)) {
                                netOutputs[(int)netOutputs.size() - 1].imageOutput = true;
                                netOutputs[(int)netOutputs.size() - 1].outputField = mid;
                                //if (j < 10) {
                                    //AfxMessageBox(_T("Building nodes for image output for ") + (CString)mid.c_str());
                                //}
                                // Do we need to add offset index and field here?
                            }
#ifndef GCC
                            if ((update_count > 500) && (image_size > 0)) {
                                progressDlg.setProgress((100 * j) / image_size);
                                update_count = 0;
                            }
                            update_count++;
#endif
                        }
#ifndef GCC
                        progressDlg.DestroyDlg();
                        addPanel1String("");
#endif
                    }
                }
            }
        }
    }

    return(netInputs);
}

void NetBuilder::preProcessWaveParameters() 
{
    sound_down_sample = 1;
    sound_data_stride = 1;
    sound_process_stride = 0;
    for (int i = 0; i < (int)convs.size(); i++) {
        if (convs[i].data_type == SOUND_CONV) {
            has_wave_conv = true;
            i = (int)convs.size();
        }
    }
    if (has_wave_conv) {
        sound_data_process_type = netData.getSoundConvDataProcessType();
        string tid = "";
        vector<int> fnodes;
        vector<int> fconvs;
        for (int i = 0; i < netData.size(); i++) {
            if (netData.isConvLayer(i)) {
                if ((netData.getConvType(i) == SOUND_CONV) && (netData.getNodeMode(i) == "Input")) {
                    fnodes = immediateFollowingNodes[i];
                    fconvs = immediateFollowingConvs[i];
                    tid = netData.getID(i);
                    i = (int)netData.size();
                }
            }
        }
        sound_data_stride = 1;
        sound_down_sample = 1;
        sound_process_stride = 0;
        if (sound_data_process_type == FFT_FOLLOWING) {
            if (fnodes.size() > 0) {
                for (int i = 0; i < fnodes.size(); i++) {
                    if (fnodes[i] >= 0) {
                        if (netData.isNode(fnodes[i])) {
                            if (netData.getNodeMode(fnodes[i]) == "PreProcess") {
                                string process = netData.getNodePreProcessor(fnodes[i]);
                                if (process.substr(0, 3) == "FFT") {
                                    sound_process_stride = myStoi(process.substr(3));
#ifndef GCC
                                    if (sound_process_stride != (int)_tstof((LPCTSTR)(CString)process.substr(3).c_str())) {
                                        addErrorString("**** Problem converting FFT size for sound_process_stride");
                                    }
#endif
                                    sound_data_stride = netData.getNodePreProcessorStride(fnodes[i]);
                                    sound_down_sample = netData.getNodePreProcessorDownSample(fnodes[i]);
                                    has_wave_conv = false;    // This needs to be false because net input data comes from fft data and not wave data
                                    has_wave_fft_conv = true;
                                    i = (int)fnodes.size();
                                }
                            }
                        }
                    }
                }
            }
        }
        else if ((sound_data_process_type == CONV_FOLLOWING) && (fconvs.size() > 0)) {
            if (fconvs.size() == 1) {
                if (netData.isConvLayer(fconvs[0])) {
                    vector<int> dims = netData.getConvDimensions(fconvs[0]);
                    vector<int> strides = netData.getConvStrides(fconvs[0]);
                    if ((dims.size() > 0) && (strides.size() > 1)) {
                        sound_process_stride = dims[0] * strides[1];
                        //char s[200];  sprintf_s(s, "tpd : sound_process_stride = %i : dims[0] = %i : strides[1] = %i", sound_process_stride, dims[0], strides[1]);  addErrorString(s);
                    }
                }
            }
            else {
                addErrorString("loadNet : Unable to process copy iterations for " + tid);
            }
        }
        else if (sound_data_process_type == CONV_FOLLOWING_FFT_FOLLOWING) {
            addErrorString("********* loadNet : need to process copy iterations for conv following fft for" + tid);
            has_wave_conv = false;
            has_wave_fft_conv = true;
        }
        //char s[200];  sprintf_s(s, "sound_process_stride = %i", sound_process_stride);  addErrorString(s);
    }
}

vector<string> NetBuilder::getConvInputs()
{
    vector<string> rval;
    for (int i = 0; i < (int)netData.size(); i++) {
        if (netData.isConvLayer(i)) {
            if (netData.getNodeMode(i) == "Input") {
                rval.push_back(netData.getID(i));
            }
        }
    }
    return(rval);
}

vector<conv> NetBuilder::getConvs()
{
    vector<conv> convs;

    vector<string> conv_names = getConvOrder();

    // Vectors for supporting weights and nodes
    tweights.clear();
    tinputNodeIDs.clear();
    tinputConvIDs.clear();

    // Look for and set up convolutions
    for (int l = 0; l < (int)conv_names.size(); l++) {

        int cindex = netData.getIndexFromID(conv_names[l]);

        //char s[200];  sprintf_s(s, "getConvs : l = %i : cindex = %i : ", l, cindex);  addErrorString(s + conv_names[l] + " : " + netData.getID(cindex));

        // Or is this a hidden or output convlayer
        if (netData.isConvLayer(cindex) || netData.isAttentionLayer(cindex) || netData.isNetCluster(cindex) || netData.isDropout(cindex)) {

            int mtype = netData.getConvType(cindex);

            //char s[200];  sprintf_s(s, "getConvs : l = %i : cindex = %i : mtype = %i : nodeType = %i : ", l, cindex, mtype, netData.getNode(cindex).nodeType);  addErrorString(s + netData.getNodeMode(cindex) + " : " + conv_names[l]);

            if (!((mtype == DATA_CONV) && (netData.getNodeMode(cindex) == "Output"))) {

                // Going to get supporting weights and nodes for each node, so clear
                //double bias = 0;
                tweights.clear();
                tinputNodeIDs.clear();
                tinputConvIDs.clear();

                // Get archived parameters
                string mmode = netData.getNodeMode(cindex);
                bool mtrainable = netData.getNodeTrainable(cindex);
                double mbias = netData.getNodeBias(cindex);
                int mlayers = netData.getConvNumLayers(cindex);
                int mnodes = netData.getConvNodesPerLayer(cindex);

                bool mgray = false;
                int mxin = 0;
                int myin = 0;
                int mxout = 0;
                int myout = 0;
                int stridex = 0;
                int stridey = 0;
                int mwidth = 0;
                int mheight = 0;

                int md = 0;
                int mdk = 0;
                int mdv = 0;

                // Get input dimensions for this convlayer
                if (mtype == IMAGE_CONV) {
                    mgray = netData.getImageGray();
                    mxin = netData.getConvInputX(cindex);
                    myin = netData.getConvInputY(cindex);
                    mxout = netData.getConvOutputX(cindex);
                    myout = netData.getConvOutputY(cindex);
                    stridex = netData.getConvOutputStrideX(cindex);
                    stridey = netData.getConvOutputStrideY(cindex);
                    imageSizeData imageSize = netData.getImageSizeData(cindex);
                    mwidth = imageSize.input_width;
                    mheight = imageSize.input_height;
                    //char s[200];  sprintf_s(s, "getConvs : mwidth = %i : mheight = %i : ", mwidth, mheight);  addErrorString(s + conv_names[l] + " : " + netData.getID(cindex));
                }
                else if (mtype == SOUND_CONV) {
                    mgray = true;
                    mxin = netData.getConvInputY(cindex);
                    myin = 1;
                    mxout = netData.getConvOutputY(cindex);
                    stridex = netData.getConvOutputStrideX(cindex);
                    stridey = 1;
                    //char p[200]; sprintf_s(p, "wave_files_longest = %i:", wave_files_longest);  addErrorString(p);
                    mwidth = wave_files_longest;
                    mheight = 1;
                } 
                else if (mtype == TEXT_CONV) {
                    mgray = true;
                    mxin = netData.getConvInputY(cindex);
                    myin = 1;
                    mxout = netData.getConvOutputY(cindex);
                    stridex = netData.getConvOutputStrideX(cindex);
                    stridey = 1;
                    mwidth = stridex;
                    mheight = 1;
                }
                /*else if ((mtype == ATTENTION) || (mtype == SELFATTENTION)) {
                    addErrorString("mtype == ATTENTION");
                    md = netData.getAttentionDm(cindex);
                    mdk = netData.getAttentionDk(cindex);
                    mdv = netData.getAttentionDv(cindex);
                }*/
                else {
                    
                    mgray = false;
                    mxin = netData.getConvInputY(cindex);
                    myin = netData.getConvInputX(cindex);
                    mxout = netData.getConvOutputY(cindex);
                    myout = netData.getConvOutputX(cindex);
                    myin = netData.getInputWidthFromLayerName(netData.getID(cindex));
                    md = netData.getAttentionDm(cindex);
                    mdk = netData.getAttentionDk(cindex);
                    mdv = netData.getAttentionDv(cindex);
                    bool is_sound_input = false;
                    vector<int> sconvs = netData.getSupportingConvNets(cindex);
                    if (sconvs.size() > 0) {
                        for (int j = 0; j < (int)sconvs.size(); j++) {
                            if ((netData.getNodeMode(sconvs[j]) == "Input") && (netData.getConvType(sconvs[j]) == SOUND_CONV)) {
                                is_sound_input = true;
                            }
                        }
                    }
                    if (!is_sound_input) {
                        myout = myin;
                    }
                    stridex = netData.getConvOutputStrideX(cindex);
                    stridey = netData.getConvOutputStrideY(cindex);
                    mwidth = stridey;
                    mheight = myin;
                    stridey = mheight;
                    if (stridey < 1) {
                        stridey = 1;
                    }
                    //if (nword_vector > 0) {
                        //mheight = nword_vector * mheight;
                    //}

                }

                tprecedingConvIDs.clear();
                tfollowingConvIDs.clear();

                // Look through all net components for input connections to this convlayer
                for (int j = 0; j < (int)netData.size(); j++) {
                    // Is this component a connect
                    if (netData.isConnect(j)) {
                        // Get potential output convlayer name
                        string connect_out_name = netData.getConvNameFromConnectName(netData.getOutputNode(j));
                        // Is the connect connected to our current convlayer
                        if (connect_out_name == netData.getID(cindex)) {
                            // Get potential input convlayer name
                            string connect_in_name = netData.getConvNameFromConnectName(netData.getInputNode(j));
                            // Get index of input component
                            int connect_in_index = netData.getIndexFromID(connect_in_name);
                            // If convlayer
                            if (netData.isConvLayer(connect_in_index)) {
                                if ((netData.getNodeMode(connect_in_index) == "Input") && (netData.getConvType(connect_in_index) == DATA_CONV)) {
                                    int num_model = 1;
                                    if (netData.getD(connect_in_index) > 1) {
                                        num_model = netData.getD(connect_in_index);
                                    }
                                    vector<string> mfields = netData.getDataFields(connect_in_index);
                                    if (mfields.size() > 0) {
                                        for (int q = 0; q < (int)mfields.size(); q++) {
                                            string nname = /*connect_in_name + _T(":") + */mfields[q];
                                            if (!inInputNodes(nname)) {
                                                //addErrorString(_T("adding input ") + nname + _T(" from ") + connect_in_name + _T(" for conv ") + connect_out_name);
                                                tinputNodeIDs.push_back(nname);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (!inPreceedingConvs(connect_in_name)) {
                                        tprecedingConvIDs.push_back(connect_in_name);
                                    }
                                }
                            }
                            // Else if attention
                            else if (netData.isAttentionLayer(connect_in_index)) {
                                if (!inPreceedingConvs(connect_in_name)) {
                                    tprecedingConvIDs.push_back(connect_in_name);
                                }
                            }
                            // Else if cluster
                            else if (netData.isNetCluster(connect_in_index)) {
                                if (!inPreceedingConvs(connect_in_name)) {
                                    tprecedingConvIDs.push_back(connect_in_name);
                                }
                            }
                            // Else if dropout
                            else if (netData.isDropout(connect_in_index)) {
                                if (!inPreceedingConvs(connect_in_name)) {
                                    tprecedingConvIDs.push_back(connect_in_name);
                                }
                            }
                            // Else if node
                            else if (netData.isNode(connect_in_index)) {
                                if (netData.getNodeMode(connect_in_index) == "Input") {
                                    // Check and add input node ids
                                    if (!inInputNodes(connect_in_name)) {
                                        tinputNodeIDs.push_back(connect_in_name);
                                    }
                                }
                                else if (netData.getNodeMode(connect_in_index) == "PreProcess") {
                                    // If process node, check and save node ID(s)
                                    if (netData.getNodePreProcessor(connect_in_index).substr(0, 3) == "FFT") {
                                        string process = netData.getNodePreProcessor(connect_in_index);
                                        //addErrorString("getConvs : " + process + " : substr = " + process.substr(3));
                                        int size = myStoi(process.substr(3));
#ifndef GCC
                                        if (size != (int)_tstof((LPCTSTR)(CString)process.substr(3).c_str())) {
                                            addErrorString("integer conversion error 8");
                                        }
#endif
                                        //char p[200]; sprintf_s(p, "%i", size);  addErrorString("getConvs : " + process + " : " + process.substr(3) + " : " + p);
                                        for (int m = 0; m < size; m++) {
                                            // Check and add input node ids
                                            string nname = "";
                                            //nname.Format(_T(":%i"), m);
                                            char s[200];  sprintf_s(s, ":%i", m);
                                            nname = connect_in_name + s;
                                            if (!inInputNodes(nname)) {
                                                tinputNodeIDs.push_back(nname);
                                            }
                                        }
                                    }
                                    /*else if (tparams.text_data && ((netData.getNodePreProcessor(connect_in_index) == _T("WordIndex")) || (netData.getNodePreProcessor(connect_in_index) == _T("Dictionary")))) {
                                        CString nname = _T("");
                                        for (int m = 0; m < tparams.nword_vector; m++) {
                                            nname.Format(_T(":%i"), m);
                                            nname = connect_in_name + nname;
                                            if (!inInputNodes(nname)) {
                                                tinputNodeIDs.push_back(nname);
                                            }
                                        }
                                    }*/
                                    else {
                                        if (netData.isProcessNode(connect_in_name)) {
                                            vector<string> sconvs = netData.getImmediateSupportingConvNets(connect_in_name, true);
                                            if (sconvs.size() > 0) {
                                                vector<string> input_fields = netData.getInputFieldsFromLayerName(sconvs[0]);
                                                for (int n = 0; n < (int)input_fields.size(); n++) {
                                                    string myinput = connect_in_name + ":" + input_fields[n];
                                                    if (!inInputNodes(myinput)) {
                                                        tinputNodeIDs.push_back(myinput);
                                                    }
                                                }
                                            }
                                            else {
                                                if (!inInputNodes(connect_in_name)) {
                                                    tinputNodeIDs.push_back(connect_in_name);
                                                }
                                            }
                                        }
                                        else {
                                            if (!inInputNodes(connect_in_name)) {
                                                tinputNodeIDs.push_back(connect_in_name);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (!inInputNodes(connect_in_name)) {
                                        tinputNodeIDs.push_back(connect_in_name);
                                    }
                                }
                            }
                            // Else error
                            else {
                                addErrorString("Error : " + netData.getInputNode(j) + " connected to " + netData.getID(cindex) + " unable to determine input node type");
                            }
                        }
                    }
                }

                // Look through all net components for output connections from this convlayer
                for (int j = 0; j < (int)netData.size(); j++) {
                    // Is this component a connect
                    if (netData.isConnect(j)) {
                        // Get potential input convlayer name
                        string connect_in_name = netData.getConvNameFromConnectName(netData.getInputNode(j));
                        // Is the connect connected to our current convlayer
                        if (connect_in_name == netData.getID(cindex)) {
                            // Get potential output convlayer name
                            string connect_out_name = netData.getConvNameFromConnectName(netData.getOutputNode(j));
                            // Get index of output component
                            int connect_out_index = netData.getIndexFromID(connect_out_name);
                            // If convlayer
                            if (netData.isConvLayer(connect_out_index)) {
                                if (!inFollowingConvs(connect_out_name)) {
                                    tfollowingConvIDs.push_back(connect_out_name);
                                }
                            }
                            // Else if attention layer
                            else if (netData.isAttentionLayer(connect_out_index)) {
                                if (!inFollowingConvs(connect_out_name)) {
                                    tfollowingConvIDs.push_back(connect_out_name);
                                }
                            }
                            // Else if cluster layer
                            else if (netData.isNetCluster(connect_out_index)) {
                                if (!inFollowingConvs(connect_out_name)) {
                                    tfollowingConvIDs.push_back(connect_out_name);
                                }
                            }
                            // Else if dropout layer
                            else if (netData.isDropout(connect_out_index)) {
                                if (!inFollowingConvs(connect_out_name)) {
                                    tfollowingConvIDs.push_back(connect_out_name);
                                }
                            }
                            // Else if not a node then error
                            else if (!netData.isNode(connect_out_index)) {
                                addErrorString("Error : " + netData.getInputNode(j) + " connected to " + netData.getID(cindex) + " unable to determine output node type");
                            }
                        }
                    }
                }

                vector<string> sinputNodeIDs;
                vector<string> sprecedingConvIDs;
                vector<string> sfollowingConvIDs;
                for (int j = 0; j < (int)tinputNodeIDs.size(); j++) {
                    sinputNodeIDs.push_back(tinputNodeIDs[j]);
                }
                for (int j = 0; j < (int)tprecedingConvIDs.size(); j++) {
                    sprecedingConvIDs.push_back(tprecedingConvIDs[j]);
                }
                for (int j = 0; j < (int)tfollowingConvIDs.size(); j++) {
                    sfollowingConvIDs.push_back(tfollowingConvIDs[j]);
                }

                conv mconv;
                mconv.setData(netData.getID(cindex), mmode, netData.getNodeFunction(cindex),
                    netData.getNodeEta(cindex), netData.getNodeAlpha(cindex), netData.getNodeRand(cindex),
                    mlayers, mnodes, mbias, tweights, mtrainable,
                    mxin, myin, mxout, myout, stridex, stridey, mwidth, mheight, mgray, mtype,
                    md, mdk, mdv,
                    sinputNodeIDs, sprecedingConvIDs, sfollowingConvIDs);
                convs.push_back(mconv);

            }

        }

    }

    input_image_conv_name = "";
    if (convs.size() > 0) {
        for (int i = 0; i < (int)convs.size(); i++) {
            //char s[200];  sprintf_s(s, "i = %i : ", i);  addErrorString(s + convs[i].id);
            if ((convs[i].getMode() == "Input") && (convs[i].data_type == IMAGE_CONV)) {
                input_image_conv_name = convs[i].id;
            }
        }
    }

    return(convs);

}

vector<string> NetBuilder::getConvOrder()
{
    // Sorting convs in order of dependency

    vector<string> rval;

    // Get all convs
    vector <int> conv_indices;
    vector <string> conv_names;
    for (int i = 0; i < (int)netData.size(); i++) {
        if (netData.isConvLayer(i) || netData.isAttentionLayer(i) || netData.isNetCluster(i) || netData.isDropout(i)) {
            bool found = false;
            for (int j = 0; j < (int)conv_names.size(); j++) {
                if (netData.getID(i) == conv_names[j]) {
                    found = true;
                    j = (int)conv_names.size();
                }
            }
            if (!found) {
                conv_indices.push_back(i);
                conv_names.push_back(netData.getID(i));
                //addErrorString("getConvOrder : adding " + netData.getID(i));
            }
        }
    }

    // Sort if we have convs
    if ((int)conv_names.size() == 1) {
        rval.push_back(conv_names[0]);
    }
    else if ((int)conv_names.size() > 1) {
        // Made used array to track convs that have already been sorted
        vector<bool> convs_used;
        for (int i = 0; i < (int)conv_names.size(); i++) {
            convs_used.push_back(false);
        }
        int cnt = 0;
        bool done = false;
        while (!done && (cnt < (int)netData.size())) {
            bool changed = false;
            for (int i = 0; i < (int)conv_names.size(); i++) {
                if (!convs_used[i]) {
                    //addErrorString("getConvOrder : checking " + conv_names[i]);
                    vector<int> recursivesupporting = netData.getSupportingConvNetsRecursive(conv_names[i]);
                    if ((int)recursivesupporting.size() == 0) {
                        //addErrorString("getConvOrder : adding " + conv_names[i] + " - no supporting");
                        rval.push_back(conv_names[i]);
                        convs_used[i] = true;
                        changed = true;
                    }
                    else {
                        bool complete = true;
                        for (int j = 0; j < (int)recursivesupporting.size(); j++) {
                            bool good = false;
                            if (netData.getNodeMode(recursivesupporting[j]) == "Input") {
                                good = true;
                            }
                            else {
                                for (int m = 0; m < (int)conv_names.size(); m++) {
                                    if (recursivesupporting[j] == conv_indices[m]) {
                                        if (convs_used[m]) {
                                            good = true;
                                        }
                                        m = (int)conv_names.size();
                                    }
                                }
                            }
                            if (!good) {
                                j = (int)recursivesupporting.size();
                                complete = false;
                            }
                        }
                        if (complete) {
                            //addErrorString("getConvOrder : adding " + conv_names[i] + " - complete");
                            rval.push_back(conv_names[i]);
                            convs_used[i] = true;
                            changed = true;
                        }
                    }
                }
            }
            if (changed) {
                done = false;
            }
            else {
                done = true;
                for (int i = 0; i < (int)conv_names.size(); i++) {
                    if (!convs_used[i]) {
                        done = false;
                    }
                }
            }
            cnt++;
        }

    }

    return(rval);
}


bool NetBuilder::inInputNodes(string minput)
{
    for (int k = 0; k < (int)tinputNodeIDs.size(); k++) {
        if (minput == tinputNodeIDs[k]) {
            return(true);
        }
    }
    return(false);
}

bool NetBuilder::inPreceedingConvs(string mname)
{
    for (int k = 0; k < (int)tprecedingConvIDs.size(); k++) {
        if (mname == tprecedingConvIDs[k]) {
            return(true);
        }
    }
    return(false);
}

bool NetBuilder::inFollowingConvs(string mname)
{
    for (int k = 0; k < (int)tfollowingConvIDs.size(); k++) {
        if (mname == tfollowingConvIDs[k]) {
            return(true);
        }
    }
    return(false);
}

bool NetBuilder::addToInputs(string minput)
{
    for (int k = 0; k < (int)netInputs.size(); k++) {
        if (minput == netInputs[k].nodeId) {
            return(false);
        }
    }    
    inputData tinput;
    tinput.nodeId = minput;
    netInputs.push_back(tinput);
    return(true);
}

void NetBuilder::addFieldToInput(int dataindex, int inputindex)
{
    string mfield = "";
    string prefix = "";
    vector<string> fields = netData.getDataFields(dataindex);
    if (fields.size() > 0) {
        mfield = fields[0];
        addErrorString("addFieldToInput for " + netData.getID(dataindex) + " unhandled multiple input fieldss");
    }
    else {
        mfield = netData.getDataField(dataindex);
    }
    if (mfield.substr(0, 7) == "Output:") {
        num_copy_outputs_to_inputs++;
    }
    else {
        if (netInputs[inputindex].indexOffset > 0) {
            char p[200]; sprintf_s(p, "IndexP%i:", netInputs[inputindex].indexOffset);
            prefix = p;
        }
        else if (netInputs[inputindex].indexOffset < 0) {
            char p[200]; sprintf_s(p, "IndexM%i:", -netInputs[inputindex].indexOffset);
            prefix = p;
        }
    }
    netInputs[inputindex].inputFields.clear();
    netInputs[inputindex].inputFields.push_back(prefix + mfield);
}

void NetBuilder::addFftFieldToInput(int dataindex, int inputindex, int fftindex)
{
    vector<string> fields = netData.getDataFields(dataindex);
    netInputs[inputindex].inputFields.clear();
    char p[200]; sprintf_s(p, "FFT%i:", fftindex);
    if (fields.size() > 0) {
        netInputs[inputindex].inputFields.push_back(p + fields[0]);
        addErrorString("addFftFieldToInput for " + netData.getID(dataindex) + " unhandled multiple input fieldss");
    }
    else {
        netInputs[inputindex].inputFields.push_back(p + netData.getDataField(dataindex));
    }
}

void NetBuilder::addProcessToInput(int dataindex, int inputindex)
{
    if (netData.getNodeIndexMode(dataindex) == "Index") {
        netInputs[inputindex].indexOffset = netData.getNodeXIndex(dataindex);
    }
    else {
        netInputs[inputindex].indexOffset = 0;
    }
    if (netData.getNodeMode(dataindex) == "Input") {
        netInputs[inputindex].processdata.process = "";
        netInputs[inputindex].processdata.stride = 1;
        netInputs[inputindex].processdata.lfactor = 0.0;
        netInputs[inputindex].processdata.ufactor = 0.0;
        netInputs[inputindex].processdata.key1 = "";
        netInputs[inputindex].processdata.key2 = "";
        netInputs[inputindex].processdata.downsample = 1;
    }
    else if (netData.getNodeMode(dataindex) == "PreProcess") {
        netInputs[inputindex].processdata.process = netData.getNodePreProcessor(dataindex);
        netInputs[inputindex].processdata.stride = netData.getNodePreProcessorStride(dataindex);
        netInputs[inputindex].processdata.lfactor = netData.getNodeLowerNormLimit(dataindex);
        netInputs[inputindex].processdata.ufactor = netData.getNodeUpperNormLimit(dataindex);
        netInputs[inputindex].processdata.key1 = netData.getNodePreProcessorTime(dataindex);
        netInputs[inputindex].processdata.key2 = netData.getDataField(dataindex);
        netInputs[inputindex].processdata.key3 = netData.getNodePreProcessorTime(dataindex);
        netInputs[inputindex].processdata.downsample = netData.getNodePreProcessorDownSample(dataindex);
    }
    //addErrorString("addProcessToInput : " + netInputs[inputindex].nodeId + " : " + netInputs[inputindex].processdata.process);
}

bool NetBuilder::addToOutputs(string moutput)
{
    for (int k = 0; k < (int)netOutputs.size(); k++) {
        if (moutput == netOutputs[k].nodeId) {
            return(false);
        }
    }
    outputData toutput;
    toutput.nodeId = moutput;
    netOutputs.push_back(toutput);
    return(true);
}

void NetBuilder::addFieldToOutput(int dataindex, int inputindex)
{
    if (netData.getNodeIndexMode(dataindex) == "Index") {
        netOutputs[inputindex].indexOffset = netData.getNodeXIndex(dataindex);
    }
    else {
        netOutputs[inputindex].indexOffset = 0;
    }
    string prefix = "";
    if (netOutputs[inputindex].indexOffset > 0) {
        char p[200]; sprintf_s(p, "IndexP%i:", netOutputs[inputindex].indexOffset);
        prefix = p;
    }
    else if (netOutputs[inputindex].indexOffset < 0) {
        char p[200]; sprintf_s(p, "IndexM%i:", -netOutputs[inputindex].indexOffset);
        prefix = p;
    }
    vector<string> fields = netData.getDataFields(dataindex);
    if (fields.size() > 0) {
        addErrorString("addFieldToOutput for " + netData.getID(dataindex) + " unhandled multiple output fields");
    }
    netOutputs[inputindex].outputField = prefix + netData.getDataField(dataindex);
    netOutputs[inputindex].outputFieldAlias = netOutputs[inputindex].outputField;
    netOutputs[inputindex].processdata.process = netData.getNodePreProcessor(dataindex);
    netOutputs[inputindex].processdata.stride = netData.getNodePreProcessorStride(dataindex);
    netOutputs[inputindex].processdata.lfactor = netData.getNodeLowerNormLimit(dataindex);
    netOutputs[inputindex].processdata.ufactor = netData.getNodeUpperNormLimit(dataindex);
    netOutputs[inputindex].processdata.key1 = netData.getNodePreProcessorTime(dataindex);
    netOutputs[inputindex].processdata.key2 = netData.getDataField(dataindex);
    netOutputs[inputindex].processdata.downsample = netData.getNodePreProcessorDownSample(dataindex);
}

bool NetBuilder::addIndexToInput(string mname, int index)
{
    //nname.Format(_T(":%i"), index);
    char s[200];  sprintf_s(s, ":%i", index);
    string nname = mname + s;
    return(addToInputs(nname));
}

bool NetBuilder::isInArray(string test, vector<string> marray)
{
    for (int i = 0; i < (int)marray.size(); i++) {
        if (test == marray[i]) {
            return(true);
        }
    }
    return(false);
}

int NetBuilder::myStoi(string s)
{
    if (isWordNumber(s)) {
        return(stoi(s));
    }
    addErrorString(s);
    return(0);
}

double NetBuilder::processData(int dindex, double pdata)
{
    double rval = pdata;
    static bool process_error_shown = false;
    if (dindex >= 0) {
        string process = netData.getNodePreProcessor(dindex);
        string process2 = process.substr(0, 10);
        string process3 = process.substr(0, 3);
        int stride = netData.getNodePreProcessorStride(dindex);
        double lfactor = netData.getNodeLowerNormLimit(dindex);
        double ufactor = netData.getNodeUpperNormLimit(dindex);
        if (process == "NormLinear") {
            rval = lfactor + (ufactor * pdata);
        }
        else if (process == "NormLog") {
            if ((pdata > 0)) {
                rval = lfactor + (ufactor * log(pdata));
            }
        }
        else if (process2 == "Derivative") {
            // **** Note that this function is only used by CEvaluateDlg and, since it would be user unfriendly 
            // **** to have to enter multiple input values to support a derivative, the derivative function doesn't 
            // **** need to be supported here 
            /*bool relative = false;
            if (process[process.GetLength() - 1] == 'R') {
                relative = true;
            }
            process = process.Left(11);
            if (process == _T("Derivative1")) {
                double der = trainData.firstDerivativeBackward(i, stride, relative, mdataset.set_data[data_column_index].column_data);
                rval = lfactor + (der * ufactor);
            }
            else if (process == _T("Derivative2")) {
                double der = trainData.secondDerivativeBackward(i, stride, relative, mdataset.set_data[data_column_index].column_data);
                rval = lfactor + (der * ufactor);
            }
            else if (process == _T("Derivative3")) {
                double der = trainData.thirdDerivativeBackward(i, stride, relative, mdataset.set_data[data_column_index].column_data);
                rval = lfactor + (der * ufactor);
            }
            else if (process == _T("Derivative4")) {
                double der = trainData.fourthDerivativeBackward(i, stride, relative, mdataset.set_data[data_column_index].column_data);
                rval = lfactor + (der * ufactor);
            }*/
        }
        else if (process == "FNParser") {
            if (netData.getNodeFunction(dindex) != "SoftMax") {
                double delta = 1.0 - (2 * sigmoid_leakage);
                if (delta > 0.0) {
                    double nfactormin = netData.getNodeLowerNormalizationFactor(dindex);
                    double nfactormax = netData.getNodeUpperNormalizationFactor(dindex);
                    double lfactor = (nfactormin * (1.0 - sigmoid_leakage) - nfactormax * sigmoid_leakage) / delta;
                    double ufactor = (nfactormax - nfactormin) / delta;
                    rval = lfactor + (ufactor * pdata);
                }
            }
        }
        else if (process == "FieldKey") {
            if (netData.getNodeFunction(dindex) != "SoftMax") {
                double delta = 1.0 - (2 * sigmoid_leakage);
                if (delta > 0.0) {
                    double nfactormin = netData.getNodeLowerNormalizationFactor(dindex);
                    double nfactormax = netData.getNodeUpperNormalizationFactor(dindex);
                    double lfactor = (nfactormin * (1.0 - sigmoid_leakage) - nfactormax * sigmoid_leakage) / delta;
                    double ufactor = (nfactormax - nfactormin) / delta;
                    rval = lfactor + (ufactor * pdata);
                }
            }
        }
        else {
            //addErrorString(_T("processData : no processing for ") + dname);
        }
    }
    else if (!process_error_shown) {
        addErrorString("processData : unable to process data for " + netData.getID(dindex));
        process_error_shown = true;
    }
    //CString s = _T("");
    //s.Format(_T("processData : processing %.4f to %.4f for "), pdata, rval);
    //addErrorString(s + dname);
    return(rval);
}



vector<string> NetBuilder::getInputDataLabels()
{
    vector<string> dlabels;
    if (netInputs.size() > 0) {
        for (int i = 0; i < (int)netInputs.size(); i++) {
            vector<string> snodes = netData.getImmediateSupportingNodes(netData.getIndexFromID(netInputs[i].nodeId));
            if (snodes.size() == 0) {
                //addErrorString("getInputDataLabels : no supporting nodes for " + netInputs[i].nodeId);
                dlabels.push_back(netInputs[i].nodeId);
            }
            else {
                if (snodes.size() > 1) {
                    addErrorString("getInputDataLabels : multiple supporting nodes for " + netInputs[i].nodeId + " - using only " + snodes[0]);
                }
                int dindex = netData.getIndexFromID(snodes[0]);
                string dfield = netData.getDataField(dindex);
                dlabels.push_back(dfield);
            }
            //addErrorString("getInputDataLabels : " + netInputs[i].nodeId + " : " + dlabels[i]);
        }
    }
    return(dlabels);
}

vector<doubleKvPair> NetBuilder::testForward(vector<doubleKvPair> inputs, bool use_previous_align)
{
    vector<doubleKvPair> rvals;

    if (!use_previous_align) {

        outputconnectioncnt = 0;
        for (int j = 0; j < (int)netInputs.size(); j++) {
            if (netInputs[j].nodeId.substr(0, 7) == "Output:") {
                outputconnectioncnt++;
            }
        }

        if ((int)inputs.size() == ((int)netInputs.size() - outputconnectioncnt)) {

            input_test_indices.clear();
            input_test_to_ndata_indices.clear();
            input_from_output_test_indices.clear();
            output_to_input_test_indices.clear();
            for (int i = 0; i < (int)inputs.size(); i++) {
                input_test_indices.push_back(0);
                input_test_to_ndata_indices.push_back(0);
            }
            if (outputconnectioncnt > 0) {
                for (int i = 0; i < outputconnectioncnt; i++) {
                    input_from_output_test_indices.push_back(0);
                    output_to_input_test_indices.push_back(0);
                }
            }

            for (int i = 0; i < (int)inputs.size(); i++) {
                bool found = false;
                for (int j = 0; j < (int)netInputs.size(); j++) {
                    if (inputs[i].key == netInputs[j].nodeId) {
                        input_test_indices[i] = j;
                        j = (int)netInputs.size();
                        found = true;
                    }
                }
                if (!found) {
                    if (inputs[i].key.substr(0, 7) == "Output:") {
                        addErrorString("testForward : found input connected from output in input data set : " + inputs[i].key);
                        found = true;
                    }
                }
                if (!found) {
                    addErrorString("testForward : unble to find input match for " + inputs[i].key);
                }
                int ndindex = netData.getIndexFromID(inputs[i].key);
                if (ndindex >= 0) {
                    input_test_to_ndata_indices[i] = ndindex;
                }
                else {
                    addErrorString("testForward : unble to find netData match for " + inputs[i].key);
                }
            }

            if (outputconnectioncnt > 0) {
                if (num_outputs > 0) {
                    int cindex = 0;
                    for (int j = 0; j < (int)netInputs.size(); j++) {
                        if (cindex < outputconnectioncnt) {
                            if (netInputs[j].nodeId.substr(0, 7) == "Output:") {
                                string oid = netData.getStringAfterColon(netInputs[j].nodeId);
                                for (int k = 0; k < num_outputs; k++) {
                                    if (oid == onodes[k].id) {
                                        input_from_output_test_indices[cindex] = j;
                                        output_to_input_test_indices[cindex] = k;
                                        k = num_outputs;
                                        cindex++;
                                    }
                                }
                            }
                        }
                        else {
                            addErrorString("testForward : problem finding indices connecting output nodes to input nodes");
                        }
                    }
                }
                else {
                    addErrorString("testForward : found inputs connected from outputs without output nodes");
                }
            }

            if (test_inputs_allocated) {
                delete[] test_inputs;
            }
            test_inputs = new double[(int)netInputs.size()];
            if (test_inputs == 0) {
                addErrorString("testForward : unable to allocate memory for test inputs");
                return(rvals);
            }
            test_inputs_allocated = true;

        }
        else {
            char s[200]; sprintf_s(s, "testForward : inputs size (%i) not equal to netInputs size (%i)", (int)inputs.size(), (int)netInputs.size());
            addErrorString(s);
        }

    }

    if ((inputs.size() == input_test_indices.size()) && 
        (outputconnectioncnt == (int)input_from_output_test_indices.size()) && 
        (input_from_output_test_indices.size() == output_to_input_test_indices.size()) && 
        (input_test_indices.size() == input_test_to_ndata_indices.size()) &&
        test_inputs_allocated) {

        for (int i = 0; i < (int)inputs.size(); i++) {
            test_inputs[input_test_indices[i]] = processData(input_test_to_ndata_indices[i], inputs[i].value);
        }
        if (outputconnectioncnt > 0) {
            for (int i = 0; i < outputconnectioncnt; i++) {
                test_inputs[input_from_output_test_indices[i]] = onodes[output_to_input_test_indices[i]].getOutput();
            }
        }
        forward(test_inputs, false);

        /*string result = "Forward Test Output(s)";
        for (int i = 0; i < num_outputs; i++) {
            doubleKvPair pair;
            pair.key = onodes[i].id;
            pair.value = onodes[i].output;
            rvals.push_back(pair);
            result += " : ";
            char s[200];  sprintf_s(s, " : %.4f", pair.value);
            result += "(";
            result += pair.key;
            result += s;
            result += ")";
        }
        addErrorString(result);*/

    }
    else {
        addErrorString("testForward : indices size mismatch");
    }

    return(rvals);
}


int NetBuilder::getDataPathType()
{
    return(getDataPathType(dataPathnames));
}

int NetBuilder::getDataPathType(vector<string> mpaths)
{
    if (mpaths.size() > 0) {
        CLineParser func;
        string ext = func.getFileExtension(mpaths[0]);
        ext = func.makeLower(ext);
        if (ext == "csv") {
            return(FILE_TYPE_CSV);
        }
        else if (ext == "jpg") {
            return(FILE_TYPE_JPG);
        }
        else if (ext == "raw") {
            return(FILE_TYPE_RAW);
        }
        else if (ext == "wav") {
            return(FILE_TYPE_WAV);
        }
        else if (ext == "txt") {
            return(FILE_TYPE_TXT);
        }
    }
    return(FILE_TYPE_NONE);
}

int NetBuilder::getCombinedIndex(string fname)
{
    if (combinedPathnames.size() > 0) {
        for (int i = 0; i < combinedPathnames.size(); i++) {
            if (fname == combinedPathnames[i]) {
                return(i);
            }
        }
    }
    addErrorString("getCombinedIndex unable to find " + fname);
    return(-1);
}

string NetBuilder::getDataPathName(int index)
{
    string rval = "";
    if (index < dataPathnames.size()) {
        rval = dataPathnames[index];
    }
    return(rval);
}

string NetBuilder::getValidationPathName(int index)
{
    string rval = "";
    if (index < validationPathnames.size()) {
        rval = validationPathnames[index];
    }
    return(rval);
}



