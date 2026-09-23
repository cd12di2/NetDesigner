#include "pch.h"
#include "MainFrm.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CNetData.h"
#include "NetVerify.h"
#include "AuxFunctions.h"


/*
bool NetVerify::doesNetHaveDataSource(NetData netData)
{
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isDataSource(i)) {
			return(true);
		}
	}
	return(false);
}
*/

void NetVerify::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

bool NetVerify::verifyInputNodes(NetData netData, CString fname, vector<CString> dataSourceHeaders)
{
	bool rval = true;
	bool found = false;
	int ncnt = 0;
	CString s = _T("");
	verifyStrings.clear();
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			if (netData.getNodeMode(i) == "Input") {
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getOutputNode(j) == netData.getID(i)) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (found) {
					s = _T("Error : Input Node ") + (CString)netData.getLabel(i).c_str() + _T(" should not have Input Connection");
					verifyStrings.push_back(s);
					rval = false;
				}
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getInputNode(j) == netData.getID(i)) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (!found) {
					s = _T("Error : Input Node ") + (CString)netData.getLabel(i).c_str() + _T(" needs Output Connection");
					verifyStrings.push_back(s);
					rval = false;
				}
				if (netData.getDataField(i) == "") {
					s = _T("Error : Input Node ") + (CString)netData.getLabel(i).c_str() + _T(" needs Data Reference");
					verifyStrings.push_back(s);
					rval = false;
				}
				else {
					found = false;
					for (int j = 0; j < dataSourceHeaders.size(); j++) {
						if (dataSourceHeaders[j] == (CString)netData.getDataField(i).c_str()) {
							found = true;
							j = (int)dataSourceHeaders.size();
						}
					}
					if (!found) {
						CString mfield = (CString)netData.getDataField(i).c_str();
						if (mfield.Left(7) == _T("Output:")) {
							mfield = mfield.Right(mfield.GetLength() - 7);
						}
						for (int k = 0; k < netData.size(); k++) {
							if (netData.isNode(k)) {
								if (netData.getNodeMode(k) == "Output") {
									if (mfield == (CString)netData.getID(k).c_str()) {
										found = true;
									}
								}
							}
						}
					}
					if (!found) {
						//s = _T("Error : Input Node ") + (CString)netData.getLabel(i).c_str() + _T(" Data Reference ") + (CString)netData.getDataField(i).c_str() + _T(" does not match Data Source");
						//verifyStrings.push_back(s);
						//rval = false;
					}
				}
				ncnt++;
			}
		}
	}
	if (rval) {
		if (ncnt > 0) {
			s.Format(_T("Pass : Input Node Check : Count = %i"), ncnt);
			verifyStrings.push_back(s);
		}
	}
	else {
		s.Format(_T("Fail : Input Node Check : Count = %i"), ncnt);
		verifyStrings.push_back(s);
	}

	return(rval);
}

bool NetVerify::verifyHiddenNodes(NetData netData)
{
	bool rval = true;
	bool found = false;
	int ncnt = 0;
	CString s = _T("");
	verifyStrings.clear();
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			if (netData.getNodeMode(i) == "Hidden") {
				if ((netData.getNodeFunction(i) != "Sigmoid") && (netData.getNodeFunction(i) != "Relu") && 
					(netData.getNodeFunction(i) != "Tanh") && (netData.getNodeFunction(i) != "MaxPool") && 
					(netData.getNodeFunction(i) != "Shockley") && (netData.getNodeFunction(i) != "Multiply") && 
					(netData.getNodeFunction(i) != "SoftMax") && (netData.getNodeFunction(i) != "Gaussian") && 
					(netData.getNodeFunction(i) != "Add") && (netData.getNodeFunction(i) != "Connector")) {
					s = _T("Error : Hidden Node ") + (CString)netData.getLabel(i).c_str() + _T(" Invalid Function - ") + (CString)netData.getNodeFunction(i).c_str();
					verifyStrings.push_back(s);
					rval = false;
				}
				if (netData.getNodeEta(i) == 0) {
					s = _T("Warning : Hidden Node ") + (CString)netData.getLabel(i).c_str() + _T(" - Eta = 0.0");
					verifyStrings.push_back(s);
				}
				if (netData.getNodeAlpha(i) == 0) {
					s = _T("Observation : Hidden Node ") + (CString)netData.getLabel(i).c_str() + _T(" - Alpha = 0.0");
					verifyStrings.push_back(s);
				}
				if (netData.getNodeRand(i) == 0) {
					s = _T("Observation : Hidden Node ") + (CString)netData.getLabel(i).c_str() + _T(" - Rand = 0.0");
					verifyStrings.push_back(s);
				}
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getOutputNode(j) == netData.getID(i)) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (!found) {
					s = _T("Error : Hidden Node ") + (CString)netData.getLabel(i).c_str() + _T(" needs Input Connection");
					verifyStrings.push_back(s);
					rval = false;
				}
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getInputNode(j) == netData.getID(i)) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (!found) {
					s = _T("Error : Hidden Node ") + (CString)netData.getLabel(i).c_str() + _T(" needs Output Connection");
					verifyStrings.push_back(s);
					rval = false;
				}
				ncnt++;
			}
		}
	}
	if (ncnt > 0) {
		if (rval) {
			s.Format(_T("Pass : Hidden Node Check : Count = %i"), ncnt);
			verifyStrings.push_back(s);
		}
		else {
			s.Format(_T("Fail : Hidden Node Check : Count = %i"), ncnt);
			verifyStrings.push_back(s);
		}
	}

	return(rval);
}

bool NetVerify::verifyOutputNodes(NetData netData, CString fname, vector<CString> dataSourceHeaders, vector<CString> dataPaths)
{
	bool rval = true;
	bool found = false;
	int ncnt = 0;
	CString s = _T("");
	verifyStrings.clear();
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			if (netData.getNodeMode(i) == "Output") {
				if ((netData.getNodeFunction(i) != "Sigmoid") && (netData.getNodeFunction(i) != "Relu") &&
					(netData.getNodeFunction(i) != "Tanh") && (netData.getNodeFunction(i) != "MaxPool") && 
					(netData.getNodeFunction(i) != "Shockley") && (netData.getNodeFunction(i) != "SoftMax") && 
					(netData.getNodeFunction(i) != "Gaussian") && (netData.getNodeFunction(i) != "Add") && 
					(netData.getNodeFunction(i) != "Connector")) {
					s = _T("Error : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" Invalid Function - ") + (CString)netData.getNodeFunction(i).c_str();
					verifyStrings.push_back(s);
					rval = false;
				}
				if (netData.getNodeEta(i) == 0) {
					s = _T("Warning : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" - Eta = 0.0");
					verifyStrings.push_back(s);
				}
				if (netData.getNodeAlpha(i) == 0) {
					s = _T("Observation : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" - Alpha = 0.0");
					verifyStrings.push_back(s);
				}
				if (netData.getNodeRand(i) == 0) {
					s = _T("Observation : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" - Rand = 0.0");
					verifyStrings.push_back(s);
				}
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getOutputNode(j) == netData.getID(i)) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (!found) {
					s = _T("Error : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" needs Input Connection");
					verifyStrings.push_back(s);
					rval = false;
				}
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getInputNode(j) == netData.getID(i)) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (found) {
					s = _T("Observation : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" has Output Connection");
					verifyStrings.push_back(s);
				}
					if (netData.getNodePreProcessor(i) == "FNParser") {
						found = false;
						CAuxFunctions func;
						vector<CString> datanames = func.getDataNamesFromFilenames(dataPaths);
						for (int j = 0; j < datanames.size(); j++) {
							if (datanames[j] == (CString)netData.getDataField(i).c_str()) {
								found = true;
								j = (int)datanames.size();
							}
						}
						if (!found) {
							//s = _T("Error : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" Data Reference ") + (CString)netData.getDataField(i).c_str() + _T(" does not match Data Name");
							//verifyStrings.push_back(s);
							//rval = false;
						}
					}
					else {
						found = false;
						for (int j = 0; j < dataSourceHeaders.size(); j++) {
							if (dataSourceHeaders[j] == (CString)netData.getDataField(i).c_str()) {
								found = true;
								j = (int)dataSourceHeaders.size();
							}
						}
						if (!found) {
							//s = _T("Error : Output Node ") + (CString)netData.getLabel(i).c_str() + _T(" Data Reference ") + (CString)netData.getDataField(i).c_str() + _T(" does not match Data Source");
							//verifyStrings.push_back(s);
							//rval = false;
						}
					}
				if ((netData.getNodePreProcessor(i) == "FNParser") || (netData.getNodePreProcessor(i) == "Dictionary") || (netData.getNodePreProcessor(i) == "WordIndex")) {
					//addErrorString(_T("Pass : File name parsers verified"));
				}
				if (netData.getNodePreProcessor(i) != "") {
					//if ((netData.getNodePreProcessor(i) != _T("NormLinear")) && (netData.getNodePreProcessor(i) != _T("NormLog"))) {
						//verifyStrings.push_back(_T("Error : Output Node has Unknown Process Function"));
						//rval = false;
					//}
				}
				ncnt++;
			}
		}
	}
	if (rval) {
		s.Format(_T("Pass : Output Node Check : Count = %i"), ncnt);
		verifyStrings.push_back(s);
	}
	else {
		s.Format(_T("Fail : Output Node Check : Count = %i"), ncnt);
		verifyStrings.push_back(s);
	}

	return(rval);
}

bool NetVerify::verifyConvNets(NetData netData, CString fname, vector<CString> dataSourceHeaders)
{
	bool rval = true;
	bool found = false;
	bool conv_input = false;
	bool conv_output = false;
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			found = true;
			if (netData.getNodeMode(i) == "Input") {
				conv_input = true;
			}
			else if (netData.getNodeMode(i) == "Output") {
				conv_output = true;
			}
		}
	}
	if (!found) {
		return(rval);
	}
	CString s = _T("");
	CString s2 = _T("");
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			if ((netData.getNodeMode(i) == "Input") && conv_input) {
				s = _T("Error : Unable to Support Input Nodes along with Input Convolutional Layers : ") + (CString)netData.getLabel(i).c_str();
				verifyStrings.push_back(s);
				rval = false;
			}
			else if ((netData.getNodeMode(i) == "Output") && conv_output) {
				s = _T("Error : Unable to Support Output Nodes along with Output Convolutional Layers : ") + (CString)netData.getLabel(i).c_str();
				verifyStrings.push_back(s);
				rval = false;
			}
		}
	}
	int ncnt = 0;
	verifyStrings.clear();
	conv_sizes.clear();
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConvLayer(i)) {
			if ((netData.getNodeMode(i) == "Hidden") || (netData.getNodeMode(i) == "Output")) {
				if ((netData.getNodeFunction(i) != "Sigmoid") && (netData.getNodeFunction(i) != "Relu") &&
					(netData.getNodeFunction(i) != "Tanh") && (netData.getNodeFunction(i) != "MaxPool") &&
					(netData.getNodeFunction(i) != "Shockley") && (netData.getNodeFunction(i) != "SoftMax") && 
					(netData.getNodeFunction(i) != "Gaussian") && (netData.getNodeFunction(i) != "Connector")) {
					s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + _T(" Invalid Function - ") + (CString)netData.getNodeFunction(i).c_str();
					verifyStrings.push_back(s);
					rval = false;
				}
				if (netData.getNodeEta(i) == 0) {
					s = _T("Warning : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + _T(" : Eta = 0.0");
					verifyStrings.push_back(s);
				}
				if (netData.getNodeAlpha(i) == 0) {
					s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + _T(" : Alpha = 0.0");
					verifyStrings.push_back(s);
				}
				if (netData.getNodeRand(i) == 0) {
					s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + _T(" : Rand = 0.0");
					verifyStrings.push_back(s);
				}
			}
			vector<int> dimensions = netData.getConvDimensions(i);
			if (((netData.getNodeMode(i) == "Input") || (netData.getNodeMode(i) == "Hidden")) && (dimensions.size() < 4)) {
				s2.Format(_T(" : num dimensions = %i, should be 4"), (int)dimensions.size());
				s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
				verifyStrings.push_back(s);
				rval = false;
			}
			else if ((netData.getNodeMode(i) == "Output") && (dimensions.size() < 2)) {
				s2.Format(_T(" : num dimensions = %i, should be 2"), (int)dimensions.size());
				s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
				verifyStrings.push_back(s);
				rval = false;
			}
			if (dimensions.size() > 0) {
				if (dimensions[0] < 1) {
					s2.Format(_T(" : width1 = %i"), dimensions[0]);
					s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					rval = false;
				}
				else if (dimensions[0] > 48) {
					s2.Format(_T(" : width1 = %i"), dimensions[0]);
					s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					//rval = false;
				}
			}
			if (dimensions.size() > 1) {
				if (dimensions[1] < 1) {
					s2.Format(_T(" : height1 = %i"), dimensions[1]);
					s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					rval = false;
				}
				else if (dimensions[1] > 48) {
					s2.Format(_T(" : height1 = %i"), dimensions[1]);
					s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					//rval = false;
				}
			}
			if (dimensions.size() > 2) {
				if (dimensions[2] < 1) {
					s2.Format(_T(" : width2 = %i"), dimensions[2]);
					s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					rval = false;
				}
				else if (dimensions[2] > 48) {
					s2.Format(_T(" : width2 = %i"), dimensions[2]);
					s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					//rval = false;
				}
			}
			if (dimensions.size() > 3) {
				if (dimensions[3] < 1) {
					s2.Format(_T(" : height2 = %i"), dimensions[3]);
					s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					rval = false;
				}
				else if (dimensions[3] > 48) {
					s2.Format(_T(" : height2 = %i"), dimensions[3]);
					s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					//rval = false;
				}
			}
			vector<int> strides = netData.getConvStrides(i);
			if (strides.size() > 0) {
				if (strides[0] < 1) {
					s2.Format(_T(" : stride1 = %i"), strides[0]);
					s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					rval = false;
				}
				else if (strides[0] > 48) {
					s2.Format(_T(" : stride1 = %i"), strides[0]);
					s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					//rval = false;
				}
			}
			if (strides.size() > 1) {
				if (strides[1] < 1) {
					s2.Format(_T(" : stride2 = %i"), strides[1]);
					s = _T("Error : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					verifyStrings.push_back(s);
					rval = false;
				}
				else if (strides[1] > 48) {
					//s2.Format(_T(" : stride2 = %i"), strides[1]);
					//s = _T("Observation : Convolutional Layer ") + (CString)netData.getLabel(i).c_str() + s2;
					//verifyStrings.push_back(s);
				}
			}
			ncnt++;
		}
	}

	if (rval) {
		conv_forward_return_values mval = getConvolutionFeedForwardSetup(netData);
		rval = mval.good;
		if (!mval.good) {
			s = _T("Error : Convolutional Layer Feedforward Setup Error");
			verifyStrings.push_back(s);
		}
	}

	if (rval) {
		s.Format(_T("Pass : Convolutional Layer Check : Count = %i"), ncnt);
		verifyStrings.push_back(s);
	}
	else {
		s.Format(_T("Fail : Convolutional Layer Check : Count = %i"), ncnt);
		verifyStrings.push_back(s);
	}
	return(rval);
}

bool NetVerify::verifyProcessNodes(NetData netData, CString fname, vector<CString> dataSourceHeaders)
{
	bool rval = true;
	bool found = false;
	int ncnt = 0;
	CString s = _T("");
	verifyStrings.clear();
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isNode(i)) {
			if (netData.getNodeMode(i) == "PreProcess") {
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getInputNode(j) == netData.getID(i)) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (!found) {
					s = _T("Error : Process Node ") + (CString)netData.getLabel(i).c_str() + _T(" needs Output Connection");
					verifyStrings.push_back(s);
					rval = false;
				}
				if (!verifyProcessNode(i, netData, fname, dataSourceHeaders)) {
					rval = false;
				}
				ncnt++;
			}
		}
	}
	if (ncnt > 0) {
		if (rval) {
			s.Format(_T("Pass : Process Node Check : Count = %i"), ncnt);
			verifyStrings.push_back(s);
		}
		else {
			s.Format(_T("Fail : Process Node Check : Count = %i"), ncnt);
			verifyStrings.push_back(s);
		}
	}

	return(rval);
}

bool NetVerify::verifyConnects(NetData netData)
{
	bool rval = true;
	bool found = false;
	int ncnt = 0;
	CString s = _T("");
	verifyStrings.clear();
	for (int i = 0; i < netData.size(); i++) {
		if (netData.isConnect(i)) {
			CAuxFunctions func;
			CString nname = (CString)netData.getInputNode(i).c_str();
			CString nname2 = func.getConvNameFromConnectName(nname);
			int cnode = func.getConvNodeFromConnectName(nname);
			bool input = func.getConvInputFromConnectName(nname);
			if (netData.getInputNode(i) == "") {
				s = _T("Error : Connection ") + (CString)netData.getLabel(i).c_str() + _T(" Blank Input Node");
				verifyStrings.push_back(s);
				rval = false;
			}
			else {
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isNode(j)) {
						if (netData.getInputNode(i) == netData.getID(j)) {
							found = true;
							j = netData.size();
						}
					}
					else if (netData.isConvLayer(j)) {
						if (nname2 == (CString)netData.getID(j).c_str()) {
							if (input) {
								if ((cnode >= 0) && (cnode < 3)) {
									found = true;
								}
							}
							j = netData.size();
						}
					}
					else if (netData.isAttentionLayer(j)) {
						if (nname2 == (CString)netData.getID(j).c_str()) {
							if (input) {
								if ((cnode >= 0) && (cnode < 3)) {
									found = true;
								}
							}
							j = netData.size();
						}
					}
					else if (netData.isNetCluster(j)) {
						if (nname2 == (CString)netData.getID(j).c_str()) {
							if (input) {
								if ((cnode >= 0) && (cnode < 3)) {
									found = true;
								}
							}
							j = netData.size();
						}
					}
					else if (netData.isDropout(j)) {
						if (nname2 == (CString)netData.getID(j).c_str()) {
							if (input) {
								if ((cnode >= 0) && (cnode < 3)) {
									found = true;
								}
							}
							j = netData.size();
						}
					}
					else if (netData.isAssemblyLayer (j)) {
						if (nname2 == (CString)netData.getID(j).c_str()) {
							if (input) {
								//if ((cnode >= 0) && (cnode < 3)) {
									found = true;
								//}
							}
							j = netData.size();
						}
					}
				}
				if (!found) {
					s = (CString)("Error : Connection " + netData.getLabel(i) + " needs Input Connection : " + netData.getInputNode(i) + "/" + netData.getOutputNode(i)).c_str();
					verifyStrings.push_back(s);
					rval = false;
				}
			}

			CString nname3 = (CString)netData.getOutputNode(i).c_str();
			CString nname4 = func.getConvNameFromConnectName(nname3);
			cnode = func.getConvNodeFromConnectName(nname3);
			input = func.getConvInputFromConnectName(nname3);
			if (netData.getOutputNode(i) == "") {
				s = _T("Error : Connection ") + (CString)netData.getLabel(i).c_str() + _T(" Blank Output Node");
				verifyStrings.push_back(s);
				rval = false;
			}
			else {
				found = false;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isNode(j)) {
						if (netData.getOutputNode(i) == netData.getID(j)) {
							found = true;
							j = netData.size();
						}
					}
					else if (netData.isConvLayer(j)) {
						if (nname4 == (CString)netData.getID(j).c_str()) {
							if (!input) {
								if ((cnode >= 0) && (cnode < 3)) {
									found = true;
								}
							}
							j = netData.size();
						}
					}
					else if (netData.isAttentionLayer(j)) {
						if (nname4 == (CString)netData.getID(j).c_str()) {
							if (!input) {
								if ((cnode >= 0) && (cnode < 3)) {
									found = true;
								}
							}
							j = netData.size();
						}
					}
					else if (netData.isNetCluster(j)) {
						if (nname4 == (CString)netData.getID(j).c_str()) {
							found = true;
							j = netData.size();
						}
					}
					else if (netData.isDropout(j)) {
						if (nname4 == (CString)netData.getID(j).c_str()) {
							found = true;
							j = netData.size();
						}
					}
					else if (netData.isAssemblyLayer(j)) {
						if (nname4 == (CString)netData.getID(j).c_str()) {
							found = true;
							j = netData.size();
						}
					}
				}
				if (!found) {
					s = (CString)("Error : Connection " + netData.getLabel(i) + " needs Output Connection : " + netData.getInputNode(i) + "/" + netData.getOutputNode(i)).c_str();
					verifyStrings.push_back(s);
					rval = false;
				}
			}
			if (netData.getInputNode(i) == netData.getOutputNode(i)) {
				s = (CString)("Error : Connection " + netData.getLabel(i) + " has the same Input and Output Nodes (" + netData.getInputNode(i) + ")").c_str();
				verifyStrings.push_back(s); 
				rval = false;
			}
			for (int j = (i+1); j < netData.size(); j++) {
				if (netData.isConnect(j)) {
					if ((netData.getOutputNode(i) == netData.getOutputNode(j)) && (netData.getInputNode(i) == netData.getInputNode(j))) {
						s = (CString)("Error : Connection " + netData.getLabel(i) + " and " + netData.getLabel(j) + " have the same Input and Output (" + netData.getInputNode(i) + "/" + netData.getOutputNode(i) + ")").c_str();
						verifyStrings.push_back(s);
						rval = false;
					}
				}
			}
			ncnt++;
		}
	}
	if (rval) {
		s.Format(_T("Pass : Connection Check : Count = %i"), ncnt);
		verifyStrings.push_back(s);
	}
	else {
		s.Format(_T("Fail : Connection Check : Count = %i"), ncnt);
		verifyStrings.push_back(s);
	}

	return(rval);
}

/*CString NetVerify::verifyDataField(CString field, CString fname, vector<CString> dataSourceHeaders)
{
	CAuxFunctions func;
	if (!allocated) {
		unsigned long maxfilelength = 10000000;
		unsigned char* filedata = new unsigned char[maxfilelength + static_cast<unsigned __int64>(100)];
		unsigned long filelength = func.getCSVFile(fname, filedata, maxfilelength, false);
		int maxrows = 200000;
		numcols = (long)dataSourceHeaders.size();
		data = new CString[(static_cast<unsigned __int64>(numcols) * maxrows) + static_cast<unsigned __int64>(100)];
		numrows = func.parseCSVData(filedata, filelength, data, numcols, maxrows);
		delete[] filedata;
		allocated = true;
	}
	int index = -1;
	for (int i = 0; i < dataSourceHeaders.size(); i++) {
		if (field == dataSourceHeaders[i]) {
			index = i;
			i = (int)dataSourceHeaders.size();
		}
	}
	if (index >= 0) {
		for (int i = 0; i < numrows; i++) {
			CString s = data[(i * (int)numcols) + index];
			if (!func.isWordNumber(string(CT2CA(s)))) {
				return(s);
			}
		}
	}
	return(_T(""));
}

bool NetVerify::verifyDataFieldNotZero(CString field, CString fname, vector<CString> dataSourceHeaders)
{
	CAuxFunctions func;
	if (!allocated) {
		unsigned long maxfilelength = 10000000;
		unsigned char* filedata = new unsigned char[maxfilelength + static_cast<unsigned __int64>(100)];
		unsigned long filelength = func.getCSVFile(fname, filedata, maxfilelength, false);
		int maxrows = 200000;
		numcols = (long)dataSourceHeaders.size();
		data = new CString[(static_cast<unsigned __int64>(numcols) * maxrows) + static_cast<unsigned __int64>(100)];
		//addErrorString(_T("verifyDataFieldNotZero"));
		numrows = func.parseCSVData(filedata, filelength, data, numcols, maxrows);
		delete[] filedata;
		allocated = true;
	}
	int index = -1;
	for (int i = 0; i < dataSourceHeaders.size(); i++) {
		if (field == dataSourceHeaders[i]) {
			index = i;
			i = (int)dataSourceHeaders.size();
		}
	}
	if (index >= 0) {
		for (int i = 0; i < numrows; i++) {
			CString s = data[(i * (int)numcols) + index];
			double d = _tstof((LPCTSTR)s);
			if (!func.isWordNumber(string(CT2CA(s))) || (d <= 0)) {
				return(false);
			}
		}
	}
	return(true);
}*/

bool NetVerify::verifyProcessNode(int index, NetData netData, CString fname, vector<CString> dataSourceHeaders)
{
	CAuxFunctions func;
	bool rval = true;
	int found = 0;
	CString s = _T("");
	vector<CString> inputNodes;
	if (index < netData.size()) {
		if (netData.isNode(index) && (netData.getNodeMode(index) == "PreProcess")) {
			string process = netData.getNodePreProcessor(index);
			string process2 = process.substr(0, 10);
			string process3 = process.substr(0, 3);
			if (process == "") {
				verifyStrings.push_back(_T("Error : Process Node needs Process Function"));
				rval = false;
			}
			else if ((process == "NormLinear") || (process == "NormLog") ||
				(process == "NormSeconds") || (process == "Seconds") || (process == "NormMinutes") || (process == "Minutes") ||
				(process == "NormHours") || (process == "Hours") || (process == "NormDays") || (process == "Days") ||
				(process2 == "Derivative") || (process3 == "FFT") || (process == "WordIndex") || (process == "Dictionary") || 
				(process == "FNParser")) {
				found = 0;
				for (int j = 0; j < netData.size(); j++) {
					if (netData.isConnect(j)) {
						if (netData.getOutputNode(j) == netData.getID(index)) {
							inputNodes.push_back((CString)netData.getInputNode(j).c_str());
							found++;
						}
					}
				}
				if (found == 1) {
					int index2 = netData.getIndexFromID(string(CT2CA(func.getConvNameFromConnectName(inputNodes[0]))));
					if (netData.isNode(index2) && (netData.getNodeMode(index2) == "Input")) {
						if (netData.getNodePreProcessor(index) == "NormLog") {
							/*if (!verifyDataFieldNotZero((CString)netData.getDataField(index2).c_str(), fname, dataSourceHeaders)) {
								CString s2 = _T("Error : Process Node ") + (CString)netData.getID(index).c_str() + _T(" with Process ") + (CString)netData.getNodePreProcessor(index).c_str() + _T(" not valid with Inputs <= 0");
								verifyStrings.push_back(s2);
								rval = false;
							}*/
						}
					}
					else if (netData.isNode(index2) && (netData.getNodeMode(index2) == "PreProcess")) {
						verifyProcessNode(index2, netData, fname, dataSourceHeaders);
					}
					else if (netData.isConvLayer(index2)/* && (netData.getNodeMode(index2) == "Input")*/) {
						verifyProcessNode(index2, netData, fname, dataSourceHeaders);
					}
					else {
						s = _T("Error : Process Node ") + (CString)netData.getID(index).c_str() + _T(" Not Connected to Input or Process Node (2) : ") + (CString)netData.getID(index2).c_str();
						verifyStrings.push_back(s);
						rval = false;
					}
				}
				else if (found > 1) {
					s = _T("Error : Process Node ") + (CString)netData.getID(index).c_str() + _T(" with ") + (CString)netData.getNodePreProcessor(index).c_str() + _T(" should not have Multiple Inputs");
					verifyStrings.push_back(s);
					rval = false;
				}
				else {
					s = _T("Error : Process Node ") + (CString)netData.getID(index).c_str() + _T(" not connected to Input or Process Node");
					verifyStrings.push_back(s);
					rval = false;
				}
			}
			else {
				verifyStrings.push_back(_T("Error : Process Node has Unknown Function : ") + (CString)process.c_str());
				rval = false;
			}
		}
	}
	return(rval);
}



conv_forward_return_values NetVerify::getConvolutionFeedForwardSetup(NetData ndata)
{
	conv_forward_return_values rval;

	rval.good = true;

	CAuxFunctions func;

	CString s = _T("");
	CString outstring = _T("");

	vector<int> cindices;
	vector<bool> cused;

	// Get convolution indices
	for (int i = 0; i < ndata.size(); i++) {
		if (ndata.isConvLayer(i)) {
			cindices.push_back(i);
			cused.push_back(false);
		}
	}

	//s.Format(_T("getConvolutionFeedForwardSetup : cindices.size = %i"), (int)cindices.size());
	//addErrorString(s);

	bool input = false;
	bool output = false;

	typedef struct hidden_param {
		int index;
		int nxin;
		int nyin;
		int nxout;
		int nyout;
	} HIDDEN_PARAMS;
	vector<hidden_param> hidden_params;

	hidden_params.clear();

	// Look through convolution indices
	for (int i = 0; i < cindices.size(); i++) {

		// Is this an input convolution?
		if (ndata.getNodeMode(cindices[i]) == "Input") {
			
			conv_forward_parameters fparams;
			
			fparams.index = cindices[i];
			fparams.mode = _T("Input");
			
			fparams.nxin = 0;
			fparams.nyin = 0;
			fparams.nxout = 0;
			fparams.nyout = 0;

			int inwidth = 0;
			int inheight = 0;
			for (int j = 0; j < ndata.size(); j++) {
				if (ndata.isDataSource(j)) {
					inwidth = ndata.getDataImageWidth(j);
					inheight = ndata.getDataImageHeight(j);
				}
			}
			dims_and_strides dims = getConvDimensions(fparams.index, ndata);
			if (dims.istridexin > 0) {
				fparams.nxout = inwidth / dims.istridexin;
			}
			if (dims.istrideyin > 0) {
				fparams.nyout = inheight / dims.istrideyin;
			}

			fparams.forwardconvnets = func.getFollowingConvNets(fparams.index);

			for (int j = 0; j < fparams.forwardconvnets.size(); j++) {
				bool found = false;
				for (int k = 0; k < cindices.size(); k++) {
					if (fparams.forwardconvnets[j] == cindices[k]) {
						found = true;
						k = (int)cindices.size();
					}
				}
				if (!found) {
					rval.good = false;
					s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(fparams.forwardconvnets[j]).c_str() + _T(" : error in getFollowingConvNets (1)");
					verifyStrings.push_back(s);
				}
				if (rval.good) {
					hidden_param hp;
					hp.index = fparams.forwardconvnets[j];
					hp.nxin = fparams.nxout;
					hp.nyin = fparams.nyout;
					dims_and_strides dims = getConvDimensions(hp.index, ndata);
					int outwidth = hp.nxin * dims.idimxout;
					int outheight = hp.nyin * dims.idimyout;
					hp.nxout = 0;
					if (dims.istridexin > 0) {
						hp.nxout = outwidth / dims.istridexin;
					}
					hp.nyout = 0;
					if (dims.istrideyin > 0) {
						hp.nyout = outheight / dims.istrideyin;
					}
					found = false;
					for (int k = 0; k < hidden_params.size(); k++) {
						if (fparams.forwardconvnets[j] == hidden_params[k].index) {
							if ((hidden_params[k].nxin != hp.nxin) || (hidden_params[k].nyin != hp.nyin) || (hidden_params[k].nxout != hp.nxout) || (hidden_params[k].nyout != hp.nyout)) {
								rval.good = false;
								s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(hp.index).c_str() + _T(" dimensional mismatch with ") + (CString)ndata.getLabel(hidden_params[k].index).c_str();
								verifyStrings.push_back(s);
							}
							found = true;
							k = (int)hidden_params.size();
						}
					}
					if (!found) {
						hidden_params.push_back(hp);
					}
				}
			}
			
			//s.Format(_T(" : steps out = %i/%i"), fparams.nxout, fparams.nyout);
			//s = _T("Observation : ") + ndata.getLabel(fparams.index) + s;
			//verifyStrings.push_back(s);

			rval.rvals.push_back(fparams);
			for (int m = 0; m < cindices.size(); m++) {
				if (fparams.index == cindices[m]) {
					cused[m] = true;
					m = (int)cindices.size();
				}
			}

			input = true;

		}

	}

	if (input && rval.good) {

		bool done = false;
		vector<hidden_param> temp_params;

		while (!done) {

			temp_params.clear();

			for (int i = 0; i < hidden_params.size(); i++) {

				conv_forward_parameters fparams;

				fparams.index = hidden_params[i].index;

				fparams.mode = (CString)ndata.getNodeMode(fparams.index).c_str();

				fparams.nxin = hidden_params[i].nxin;
				fparams.nyin = hidden_params[i].nyin;
				fparams.nxout = hidden_params[i].nxout;
				fparams.nyout = hidden_params[i].nyout;

				fparams.forwardconvnets = func.getFollowingConvNets(fparams.index);

				for (int j = 0; j < fparams.forwardconvnets.size(); j++) {

					bool found = false;
					for (int k = 0; k < cindices.size(); k++) {
						if (fparams.forwardconvnets[j] == cindices[k]) {
							found = true;
							k = (int)cindices.size();
						}
					}
					if (!found) {
						//rval.good = false;
						//s = _T("Error : Convolutional Layer ") + ndata.getLabel(fparams.forwardconvnets[j]) + _T(" : error in getFollowingConvNets (2)");
						//verifyStrings.push_back(s);
					}

					hidden_param hp;
					hp.index = fparams.forwardconvnets[j];
					hp.nxin = fparams.nxout;
					hp.nyin = fparams.nyout;
					dims_and_strides dims = getConvDimensions(hp.index, ndata);
					int outwidth = hp.nxin * dims.idimxout;
					int outheight = hp.nyin * dims.idimyout;
					hp.nxout = 0;
					if (dims.istridexin > 0) {
						hp.nxout = outwidth / dims.istridexin;
					}
					hp.nyout = 0;
					if (dims.istrideyin > 0) {
						hp.nyout = outheight / dims.istrideyin;
					}
					
					found = false;
					for (int k = 0; k < temp_params.size(); k++) {
						if (hp.index == temp_params[k].index) {
							if ((temp_params[k].nxin != hp.nxin) || (temp_params[k].nyin != hp.nyin) || (temp_params[k].nxout != hp.nxout) || (temp_params[k].nyout != hp.nyout)) {
								rval.good = false;
								s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(hp.index).c_str() + _T(" dimensional mismatch with ") + (CString)ndata.getLabel(temp_params[k].index).c_str();
								verifyStrings.push_back(s);
							}
							found = true;
							k = (int)temp_params.size();
						}
					}
					
					if (!found) {
						int tindex = 0;
						for (int m = 0; m < rval.rvals.size(); m++) {
							if (hp.index == rval.rvals[m].index) {
								if ((rval.rvals[m].nxin != hp.nxin) || (rval.rvals[m].nyin != hp.nyin) || (rval.rvals[m].nxout != hp.nxout) || (rval.rvals[m].nyout != hp.nyout)) {
									rval.good = false;
									s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(hp.index).c_str() + _T(" dimensional mismatch with ") + (CString)ndata.getLabel(rval.rvals[m].index).c_str();
									verifyStrings.push_back(s);
								}
								found = true;
								m = (int)rval.rvals.size();
							}
						}
					}

					if (!found) {
						temp_params.push_back(hp);
					}

				}

				/*s = _T("");
				if (fparams.mode == _T("Hidden")) {
					s.Format(_T(" : steps in = %i/%i : steps out = %i/%i"), fparams.nxin, fparams.nyin, fparams.nxout, fparams.nyout);
				}
				else if (fparams.mode == _T("Output")) {
					s.Format(_T(" : steps in = %i/%i"), fparams.nxin, fparams.nyin);
				}
				s = _T("Observation : ") + ndata.getLabel(hidden_params[i].index) + s;
				verifyStrings.push_back(s);*/

				if ((temp_params.size() == 0) && (fparams.mode != _T("Output"))) {
					int noutputs = func.countOutputNodes(fparams.index, ndata);
					s.Format(_T(" has %i trainable output nodes"), noutputs);
					if (noutputs == 0) {
						s = _T("Error : ") + (CString)ndata.getLabel(hidden_params[i].index).c_str() + s;
						verifyStrings.push_back(s);
						rval.good = false;
					}
					else {
						//s = _T("Observation : ") + ndata.getLabel(hidden_params[i].index) + s;
						//verifyStrings.push_back(s);
					}
				}

				rval.rvals.push_back(fparams);
				for (int m = 0; m < cindices.size(); m++) {
					if (fparams.index == cindices[m]) {
						cused[m] = true;
						m = (int)cindices.size();
					}
				}

			}

			if (temp_params.size() == 0) {
				done = true;
			} 
			else {
				hidden_params.clear();
				for (int j = 0; j < temp_params.size(); j++) {
					hidden_params.push_back(temp_params[j]);
				}
			}

		}

	}
	
	if (rval.good) {

		bool done = true;
		for (int i = 0; i < cused.size(); i++) {
			if (!cused[i]) {
				done = false;
			}
		}

		if (!done) {

			hidden_params.clear();

			// Look through convolution indices
			for (int i = 0; i < cindices.size(); i++) {

				// Is this an output convolution?
				if (!cused[i] && (ndata.getNodeMode(cindices[i]) == "Output")) {

					conv_forward_parameters fparams;

					fparams.index = cindices[i];
					fparams.mode = _T("Output");

					fparams.nxin = 0;
					fparams.nyin = 0;
					fparams.nxout = 0;
					fparams.nyout = 0;

					int inwidth = 0;
					int inheight = 0;
					for (int j = 0; j < ndata.size(); j++) {
						if (ndata.isDataSource(j)) {
							inwidth = ndata.getDataImageWidth(j);
							inheight = ndata.getDataImageHeight(j);
						}
					}
					dims_and_strides dims = getConvDimensions(fparams.index, ndata);
					if (dims.idimxout > 0) {
						fparams.nxin = inwidth / dims.idimxout;
					}
					if (dims.idimyout > 0) {
						fparams.nyin = inheight / dims.idimyout;
					}

					fparams.forwardconvnets = func.getSupportingConvNets(fparams.index, ndata);

					for (int j = 0; j < fparams.forwardconvnets.size(); j++) {
						bool found = false;
						for (int k = 0; k < cindices.size(); k++) {
							if (fparams.forwardconvnets[j] == cindices[k]) {
								found = true;
								k = (int)cindices.size();
							}
						}
						if (!found) {
							rval.good = false;
							s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(fparams.forwardconvnets[j]).c_str() + _T(" : error in getSupportingConvNets");
							verifyStrings.push_back(s);
						}
						if (rval.good) {
							hidden_param hp;
							hp.index = fparams.forwardconvnets[j];
							hp.nxout = fparams.nxin;
							hp.nyout = fparams.nyin;
							dims_and_strides dims = getConvDimensions(hp.index, ndata);
							int inwidth = hp.nxout * dims.istridexin;
							int inheight = hp.nyout * dims.istrideyin;
							hp.nxin = 0;
							if (dims.idimxout > 0) {
								hp.nxin = inwidth / dims.idimxout;
							}
							hp.nyin = 0;
							if (dims.idimyout > 0) {
								hp.nyin = inheight / dims.idimyout;
							}
							found = false;
							for (int k = 0; k < hidden_params.size(); k++) {
								if (fparams.forwardconvnets[j] == hidden_params[k].index) {
									if ((hidden_params[k].nxin != hp.nxin) || (hidden_params[k].nyin != hp.nyin) || (hidden_params[k].nxout != hp.nxout) || (hidden_params[k].nyout != hp.nyout)) {
										rval.good = false;
										s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(hp.index).c_str() + _T(" dimensional mismatch with ") + (CString)ndata.getLabel(hidden_params[k].index).c_str();
										verifyStrings.push_back(s);
									}
									found = true;
									k = (int)hidden_params.size();
								}
							}
							if (!found) {
								hidden_params.push_back(hp);
							}
						}
					}

					s.Format(_T(" : steps in = %i/%i"), fparams.nxin, fparams.nyin);
					s = _T("Observation : ") + (CString)ndata.getLabel(fparams.index).c_str() + s;
					verifyStrings.push_back(s);

					rval.rvals.push_back(fparams);
					for (int m = 0; m < cindices.size(); m++) {
						if (fparams.index == cindices[m]) {
							cused[m] = true;
							m = (int)cindices.size();
						}
					}

					output = true;

				}

			}

			if (output && rval.good) {

				bool done = false;
				vector<hidden_param> temp_params;

				while (!done) {

					temp_params.clear();

					for (int i = 0; i < hidden_params.size(); i++) {

						bool found = false;
						for (int m = 0; m < cindices.size(); m++) {
							if (cindices[m] == hidden_params[i].index) {
								if (cused[m]) {
									found = true;
								}
							}
						}

						if (!found) {

							conv_forward_parameters fparams;

							fparams.index = hidden_params[i].index;

							fparams.mode = (CString)ndata.getNodeMode(fparams.index).c_str();

							fparams.nxin = hidden_params[i].nxin;
							fparams.nyin = hidden_params[i].nyin;
							fparams.nxout = hidden_params[i].nxout;
							fparams.nyout = hidden_params[i].nyout;

							fparams.forwardconvnets = func.getSupportingConvNets(fparams.index, ndata);

							for (int j = 0; j < fparams.forwardconvnets.size(); j++) {

								bool found = false;
								for (int k = 0; k < cindices.size(); k++) {
									if (fparams.forwardconvnets[j] == cindices[k]) {
										found = true;
										k = (int)cindices.size();
									}
								}
								if (!found) {
									rval.good = false;
									s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(fparams.forwardconvnets[j]).c_str() + _T(" : error in getSupportingConvNets");
									verifyStrings.push_back(s);
								}

								hidden_param hp;
								hp.index = fparams.forwardconvnets[j];
								hp.nxout = fparams.nxin;
								hp.nyout = fparams.nyin;
								dims_and_strides dims = getConvDimensions(hp.index, ndata);
								int inwidth = hp.nxout * dims.istridexin;
								int inheight = hp.nyout * dims.istrideyin;
								hp.nxin = 0;
								if (dims.idimxout > 0) {
									hp.nxin = inwidth / dims.idimxout;
								}
								hp.nyin = 0;
								if (dims.idimyout > 0) {
									hp.nyin = inheight / dims.idimyout;
								}

								found = false;
								for (int k = 0; k < temp_params.size(); k++) {
									if (hp.index == temp_params[k].index) {
										if ((temp_params[k].nxin != hp.nxin) || (temp_params[k].nyin != hp.nyin) || (temp_params[k].nxout != hp.nxout) || (temp_params[k].nyout != hp.nyout)) {
											rval.good = false;
											s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(hp.index).c_str() + _T(" dimensional mismatch with ") + (CString)ndata.getLabel(temp_params[k].index).c_str();
											verifyStrings.push_back(s);
										}
										found = true;
										k = (int)temp_params.size();
									}
								}

								if (!found) {
									int tindex = 0;
									for (int m = 0; m < rval.rvals.size(); m++) {
										if (hp.index == rval.rvals[m].index) {
											if ((rval.rvals[m].nxin != hp.nxin) || (rval.rvals[m].nyin != hp.nyin) || (rval.rvals[m].nxout != hp.nxout) || (rval.rvals[m].nyout != hp.nyout)) {
												rval.good = false;
												s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(hp.index).c_str() + _T(" dimensional mismatch with ") + (CString)ndata.getLabel(rval.rvals[m].index).c_str();
												verifyStrings.push_back(s);
											}
											found = true;
											m = (int)rval.rvals.size();
										}
									}
								}

								if (!found) {
									temp_params.push_back(hp);
								}

							}

							s = _T("");
							if (fparams.mode == _T("Hidden")) {
								s.Format(_T(" : steps in = %i/%i : steps out = %i/%i"), fparams.nxin, fparams.nyin, fparams.nxout, fparams.nyout);
								s = _T("Observation : ") + (CString)ndata.getLabel(fparams.index).c_str() + s;
								verifyStrings.push_back(s);
							}
							else {
								s = _T("Error : Convolutional Layer ") + (CString)ndata.getLabel(fparams.index).c_str() + _T(" unable to process ") + fparams.mode + _T(" in conv nets supporting output conv");
								verifyStrings.push_back(s);
								rval.good = false;
							}

							if ((temp_params.size() == 0) && (fparams.mode != _T("Input"))) {
								int ninputs = func.countInputNodes(fparams.index, ndata);
								s.Format(_T(" has %i trainable input nodes"), ninputs);
								if (ninputs == 0) {
									vector<string> snodes = func.getImmediateSupportingNodes(ndata.getLabel(fparams.index));
									if (snodes.size() > 0) {
										s.Format(_T(" has %i trainable input hidden nodes"), (int)snodes.size());
										s = _T("Observation : ") + (CString)ndata.getLabel(hidden_params[i].index).c_str() + s;
										verifyStrings.push_back(s);
									}
									else {
										s = _T("Error : ") + (CString)ndata.getLabel(hidden_params[i].index).c_str() + s;
										verifyStrings.push_back(s);
										rval.good = false;
									}
								}
								else {
									s = _T("Observation : ") + (CString)ndata.getLabel(hidden_params[i].index).c_str() + s;
									verifyStrings.push_back(s);
								}
							}

							rval.rvals.push_back(fparams);
							for (int m = 0; m < cindices.size(); m++) {
								if (fparams.index == cindices[m]) {
									cused[m] = true;
									m = (int)cindices.size();
								}
							}

						}

					}

					if (temp_params.size() == 0) {
						done = true;
					}
					else {
						hidden_params.clear();
						for (int j = 0; j < temp_params.size(); j++) {
							hidden_params.push_back(temp_params[j]);
						}
					}

				}

			}

		}

	}
	
	if (outstring != _T("")) {
		verifyStrings.push_back(outstring);
	}

	// If all convs don't pass above tests, check to see if they connect to an FFT or text process input (this is an add on test)
	for (int i = 0; i < cused.size(); i++) {
		if (!cused[i]) {
			vector<int> supportong_nodes = func.getImmediateSupportingNodes(cindices[i], ndata);
			if (supportong_nodes.size() > 0) {
				for (int j = 0; j < supportong_nodes.size(); j++) {
					//addErrorString(_T("*** not used : ") + ndata.getID(cindices[i]) + _T(" with supporting nodes testing ") + ndata.getID(supportong_nodes[j]));
					if (ndata.isNode(supportong_nodes[j])) {
						//addErrorString(_T("*** not used : ") + ndata.getID(cindices[i]) + _T(" with supporting nodes testing ") + ndata.getID(supportong_nodes[j]) + _T(" with ") + ndata.getNodeMode(supportong_nodes[j]));
						if (ndata.getNodeMode(supportong_nodes[j]) == "PreProcess") {
							//addErrorString(_T("*** not used : ") + ndata.getID(cindices[i]) + _T(" with supporting nodes testing ") + ndata.getID(supportong_nodes[j]) + _T(" with ") + ndata.getNodeMode(supportong_nodes[j]) + _T(" and ") + ndata.getNodePreProcessor(supportong_nodes[j]));
							string processor = ndata.getNodePreProcessor(supportong_nodes[j]);
							string processor3 = processor.substr(0, 3);
							if (processor3 == "FFT") {
								cused[i] = true;
							}
							else if ((processor == "WordIndex") || (processor == "Dictionary")) {
								cused[i] = true;
							}
						}
					}
				}
			}
			if ((ndata.getNodeMode(cindices[i]) == "Hidden") && (ndata.getConvType(cindices[i]) == DATA_CONV)) {
				cused[i] = true;
			}
			if ((ndata.getNodeMode(cindices[i]) == "Hidden") && (ndata.getConvType(cindices[i]) == HIDDEN_NET)) {
				cused[i] = true;
			}
		}
	}

	bool clear = true;
	for (int i = 0; i < cused.size(); i++) {
		if (!cused[i]) {
			verifyStrings.push_back(_T("Observation : Convolution not used : ") + (CString)ndata.getID(cindices[i]).c_str());
			clear = false;
		}
	}
	if (!clear) {
		if (input) {
			rval.good = false;
			verifyStrings.push_back(_T("Error : Not all convolutional layers are trainable (input)"));
		}
		else if (output) {
			rval.good = false;
			verifyStrings.push_back(_T("Error : Not all convolutional layers are trainable (ouput)"));
		}
		else {
			verifyStrings.push_back(_T("Note : No input or output convolutions found"));
		}
	}

	return(rval);
}

dims_and_strides NetVerify::getConvDimensions(int index, NetData ndata)
{
	dims_and_strides rval;
	rval.idimxin = 0;
	rval.idimyin = 0;
	rval.idimxout = 0;
	rval.idimyout = 0;
	rval.istridexin = 0;
	rval.istrideyin = 0;
	vector<int> dimensions = ndata.getConvDimensions(index);
	if (dimensions.size() > 0) {
		rval.idimxin = dimensions[0];
	}
	if (dimensions.size() > 1) {
		rval.idimyin = dimensions[1];
	}
	if (dimensions.size() > 2) {
		rval.idimxout = dimensions[2];
	}
	if (dimensions.size() > 3) {
		rval.idimyout = dimensions[3];
	}
	vector<int> strides = ndata.getConvStrides(index);
	if (strides.size() > 0) {
		rval.istridexin = strides[0];
	}
	if (strides.size() > 1) {
		rval.istrideyin = strides[1];
	}
	return(rval);
}


