// CEvaluateDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CEvaluateDlg.h"
#include "afxdialogex.h"


// CEvaluateDlg dialog

vector<double> saved_data;

IMPLEMENT_DYNAMIC(CEvaluateDlg, CDialog)

CEvaluateDlg::CEvaluateDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_EVALUATE_DLG, pParent)
{
	allocated = false;
	use_images_as_outputs = false;

	input_label_id = 100;
	input_value_id = 200;
	output_label_id = 300;
	output_value_id = 400;

	x_offset = 25;
	y_offset = 30;

	x_separation = 10;
	y_separation = 28;

	static_height = 16;
	static_width = 140;

	image_border = 10;

	edit_height = 20;
	edit_width = 70;

	ninputs = 0;
	noutputs = 0;

	image_width = 0;
	image_height = 0;

	m_label2 = _T("");

}

CEvaluateDlg::~CEvaluateDlg()
{
	if (allocated) {
		delete[] input_labels;
		delete[] input_values;
	}
}

void CEvaluateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTLINE, m_outline_ctrl);
	DDX_Control(pDX, IDC_VERT_LINE, m_vert_ctrl);
	DDX_Control(pDX, IDC_HORZ1, m_horz1_ctrl);
	DDX_Control(pDX, IDC_HORZ2, m_horz2_ctrl);
	DDX_Control(pDX, IDC_EVALUATE, m_evaluate_ctrl);
	DDX_Control(pDX, IDC_LABEL1, m_label1_ctrl);
	DDX_Control(pDX, IDC_LABEL2, m_label2_ctrl);
	DDX_Text(pDX, IDC_LABEL2, m_label2);
}


BEGIN_MESSAGE_MAP(CEvaluateDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_EVALUATE, &CEvaluateDlg::OnBnClickedEvaluate)
END_MESSAGE_MAP()


// CEvaluateDlg message handlers

BOOL CEvaluateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	CAuxFunctions func;

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	ninputs = pDoc->nnet.num_inputs;
	noutputs = pDoc->nnet.num_outputs;
	use_images_as_outputs = pDoc->nnet.use_images_as_outputs;
	image_width = pDoc->getDataImageWidth();
	image_height = pDoc->getDataImageHeight();
	if (use_images_as_outputs) {
		m_label2 = _T("Output");
		UpdateData(false);
		noutputs = 0;
	}
	else {
		m_label2 = _T("Outputs");
		UpdateData(false);
	}

	RECT crect;
	GetClientRect(&crect);

	int fontHeight = static_height - 1;
	CFont* font = new CFont();
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = fontHeight;
	//_tcscpy_s(lf.lfFaceName, _T("Arial"));
	font->CreateFontIndirect(&lf);

	netInputs.clear();
	vector<inputData> tinputs = pDoc->nnet.getNetInputs();
	for (int j = 0; j < tinputs.size(); j++) {
		netInputs.push_back((CString)tinputs[j].nodeId.c_str());
	}

	netOutputs.clear();
	vector<outputData> toutputs = pDoc->nnet.netOutputs;
	for (int j = 0; j < toutputs.size(); j++) {
		netOutputs.push_back((CString)toutputs[j].nodeId.c_str());
	}

	if ((netInputs.size() != ninputs) || (netOutputs.size() != noutputs)) {
		pDoc->addErrorString(_T("Error - problem with input / output sizes"));
		ninputs = 0;
		noutputs = 0;
	}

	int tlength = 0;
	for (int i = 0; i < ninputs; i++) {
		CString mname = getInputField(i);
		mname += _T(":");
		if (mname.GetLength() > tlength) {
			tlength = mname.GetLength();
		}
	}
	for (int i = 0; i < noutputs; i++) {
		CString mname = netOutputs[i];
		int nindex = pDoc->netData.getIndexFromID(string(CT2CA(mname)));
		string fname = pDoc->netData.getDataField(nindex);
		if (fname != "") {
			mname = (CString)fname.c_str();
		}
		mname += _T(":");
		if (mname.GetLength() > tlength) {
			tlength = mname.GetLength();
		}
	}
	static_width = (tlength + 2) * 7;

	updateSize();

	input_labels = new CStatic[ninputs];
	input_values = new CEdit[ninputs];
	output_labels = new CStatic[noutputs];
	output_values = new CEdit[noutputs];
	if ((input_labels != 0) && (input_values != 0) && (((output_labels != 0) && (output_values != 0)) || use_images_as_outputs)) {
		allocated = true;
		for (int i = 0; i < ninputs; i++) {
			RECT srect;
			RECT srect2;
			srect.top = crect.top + y_offset + ((i + 1) * y_separation);
			srect.left = crect.left + x_offset;
			srect.bottom = srect.top + static_height;
			srect.right = srect.left + static_width;
			srect2.top = crect.top + y_offset + ((i + 1) * y_separation) - ((edit_height - static_height) / 2);
			srect2.left = srect.right + x_separation;
			srect2.bottom = srect2.top + edit_height;
			srect2.right = srect2.left + edit_width;
			CString mname = getInputField(i); 
			mname += _T(":");
			input_labels[i].Create(mname, WS_VISIBLE | WS_CHILD, srect, this, (input_label_id+i));
			input_labels[i].SetFont(font);
			input_values[i].Create(WS_VISIBLE | WS_BORDER | ES_CENTER | WS_CHILD | WS_TABSTOP, srect2, this, (input_value_id + i));
			input_values[i].SetFont(font);
			if (i < saved_data.size()) {
				CString s = func.convertNumber(saved_data[i], 4);
				input_values[i].SetWindowText(s);
			}
			else {
				input_values[i].SetWindowText(_T("0.0"));
			}
		}
		for (int i = 0; i < noutputs; i++) {
			RECT srect;
			RECT srect2;
			srect.top = crect.top + y_offset + ((i + 1) * y_separation);
			srect.left = crect.left + x_offset + static_width + x_separation + edit_width + (4 * x_separation);
			srect.bottom = srect.top + static_height;
			srect.right = srect.left + static_width;
			srect2.top = crect.top + y_offset + ((i + 1) * y_separation) - ((edit_height - static_height) / 2);
			srect2.left = srect.right + x_separation;
			srect2.bottom = srect2.top + edit_height;
			srect2.right = srect2.left + edit_width;
			CString mname = netOutputs[i];
			int nindex = pDoc->netData.getIndexFromID(string(CT2CA(mname)));
			string fname = pDoc->netData.getDataField(nindex);
			if (fname != "") {
				mname = (CString)fname.c_str();
			}
			mname += _T(":");
			output_labels[i].Create(mname, WS_VISIBLE | WS_CHILD, srect, this, (output_label_id + i));
			output_labels[i].SetFont(font);
			output_values[i].Create(WS_VISIBLE | WS_BORDER | ES_CENTER | WS_CHILD, srect2, this, (output_value_id + i));
			output_values[i].SetFont(font);
			//output_values[i].EnableWindow(false);
			output_values[i].SetWindowText(_T("0.0"));
		}
		if (saved_data.size() > 0) {
			OnBnClickedEvaluate();
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEvaluateDlg::OnOK()
{
	saveDataBeforeExit();
	CDialog::OnOK();
}

BOOL CEvaluateDlg::DestroyWindow()
{
	//saveDataBeforeExit();
	return CDialog::DestroyWindow();
}

void CEvaluateDlg::OnCancel()
{
	saveDataBeforeExit();
	CDialog::OnCancel();
}

void CEvaluateDlg::saveDataBeforeExit() 
{
	if (saved_data.size() != ninputs) {
		saved_data.clear();
		for (int i = 0; i < ninputs; i++) {
			CString mdata = _T("");
			input_values[i].GetWindowText(mdata);
			double data = (double)_tstof((LPCTSTR)mdata);
			saved_data.push_back(data);
		}
	}
	else {
		for (int i = 0; i < ninputs; i++) {
			CString mdata = _T("");
			input_values[i].GetWindowText(mdata);
			double data = (double)_tstof((LPCTSTR)mdata);
			saved_data[i] = data;
		}
	}
}

HBRUSH CEvaluateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	hbr = mbrush;

	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
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
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_LISTBOX:
		hbr = mbrush2;
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);
		break;
	case CTLCOLOR_MSGBOX:
		pDC->SetTextColor(RGB(255, 0, 255));
		break;
	case CTLCOLOR_SCROLLBAR:
		pDC->SetTextColor(RGB(0, 0, 255));
		break;
	default:
		pDC->SetTextColor(GetGlobalData()->clrGrayedText);

	}

	return hbr;
}

void CEvaluateDlg::updateSize()
{
	CRect rect;
	GetWindowRect(&rect);
	int nheight = ninputs;
	if (use_images_as_outputs) {
		//int need_work_here;
	} 
	else {
		if (noutputs > nheight) {
			nheight = noutputs;
		}
	}
	nheight++;
	int mheight = ((nheight - 1) * y_separation) + (2 * y_offset);
	if (mheight < (image_height + (2 * image_border) + (2 * y_offset))) {
		mheight = image_height + (2 * image_border) + (2 * y_offset);
	}
	int mwidth = static_width + edit_width;
	if (mwidth < (image_width + (2 * image_border))) {
		mwidth = image_width + (2 * image_border);
	}
	rect.bottom = rect.top + mheight + 95;
	rect.right = rect.left + x_offset + static_width + x_separation + edit_width + (8 * x_separation) + mwidth + x_separation;
	MoveWindow(rect, true);

	CRect rect2;
	m_evaluate_ctrl.GetWindowRect(&rect2);
	int width = rect2.Width();
	int height = rect2.Height();
	rect2.left = rect.Width() - width - 30;
	rect2.top = rect.Height() - height - 60;
	rect2.right = rect2.left + width;
	rect2.bottom = rect2.top + height;
	m_evaluate_ctrl.MoveWindow(rect2, true);

	CRect rect1;
	rect1.left = 7;
	rect1.top = 1;
	rect1.right = rect1.left + rect.Width() - 31;
	rect1.bottom = rect2.top - 5;
	m_outline_ctrl.MoveWindow(rect1, true);

	CRect rect3;
	rect3.left = /*rect.left + */x_offset + static_width + x_separation + edit_width + (2 * x_separation);
	rect3.top = rect1.top + 14;
	rect3.right = rect3.left + 1;
	rect3.bottom = rect1.bottom - 8;
	m_vert_ctrl.MoveWindow(rect3, true);

	CRect rect4;
	m_label1_ctrl.GetWindowRect(&rect4);
	int width4 = rect4.Width();
	rect4.top = y_offset - 6;
	rect4.bottom = rect4.top + static_height;
	rect4.left = x_offset;
	rect4.right = rect4.left + width4;
	m_label1_ctrl.MoveWindow(rect4, true);

	CRect rect5;
	m_label2_ctrl.GetWindowRect(&rect5);
	int width5 = rect5.Width();
	rect5.top = rect4.top;
	rect5.bottom = rect4.bottom;
	rect5.left = x_offset + static_width + x_separation + edit_width + (4 * x_separation);
	rect5.right = rect5.left + width5;
	m_label2_ctrl.MoveWindow(rect5, true);

	CRect rect6;
	rect6.top = y_offset + static_height;
	rect6.bottom = rect6.top + 1;
	rect6.left = x_offset - 4;
	rect6.right = rect6.left + static_width + x_separation + edit_width + 8;
	m_horz1_ctrl.MoveWindow(rect6);

	CRect rect7;
	rect7.top = rect6.top;
	rect7.bottom = rect6.bottom;
	rect7.left = x_offset + static_width + x_separation + edit_width + (4 * x_separation) - 4;
	rect7.right = rect7.left + static_width + x_separation + edit_width + 8;
	m_horz2_ctrl.MoveWindow(rect7);

}

CString CEvaluateDlg::getInputField(int index)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	string mname = string(CT2CA(netInputs[index]));

	int nindex = pDoc->netData.getIndexFromID(mname);
	int nindex2 = -1;

	if (pDoc->netData.getNodeMode(nindex) == "PreProcess") {
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConnect(i)) {
				if (pDoc->netData.getOutputNode(i) == mname) {
					string input_node_id = pDoc->netData.getInputNode(i);
					nindex2 = pDoc->netData.getIndexFromID(input_node_id);
					input_node_id = pDoc->netData.getLabel(nindex2);
					string input_data = pDoc->netData.getDataField(nindex2);
					if (input_data != "") {
						mname = input_data;
					}
					i = pDoc->netData.size();
				}
			}
		}
	}
	else {
		string fname = pDoc->netData.getDataField(nindex);
		if (fname != "") {
			mname = fname;
		}
	}
	if (pDoc->netData.getNodeIndexMode(nindex) == "Index") {
		string s = "";
		int offset = pDoc->netData.getNodeXIndex(nindex);
		if (offset > 0) {
			//s.Format(_T(" (i+%i)"), offset);
			char s2[100];  sprintf_s(s2, "(i+%i)", offset);
			s = s2;
		}
		else if (offset < 0) {
			//s.Format(_T(" (i%i)"), offset);
			char s2[100];  sprintf_s(s2, "(i%i)", offset);
			s = s2;
		}
		else {
			s = " (i=0)";
		}
		mname += s;
	} 
	else if (pDoc->netData.getNodeIndexMode(nindex2) == "Index") {
		string s = "";
		int offset = pDoc->netData.getNodeXIndex(nindex2);
		if (offset > 0) {
			//s.Format(_T(" (i+%i)"), offset);
			char s2[100];  sprintf_s(s2, "(i+%i)", offset);
			s = s2;
		}
		else if (offset < 0) {
			//s.Format(_T(" (i%i)"), offset);
			char s2[100];  sprintf_s(s2, "(i%i)", offset);
			s = s2;
		}
		else {
			s = " (i=0)";
		}
		mname += s;
	}

	return((CString)mname.c_str());

}

bool CEvaluateDlg::doesInputMatch(int index, CString tname)
{
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	string mname = string(CT2CA(netInputs[index]));
	if (mname == string(CT2CA(tname))) {
		return(true);
	}
	int nindex = pDoc->netData.getIndexFromID(mname);
	if (pDoc->netData.getNodeMode(nindex) == "PreProcess") {
		for (int i = 0; i < pDoc->netData.size(); i++) {
			if (pDoc->netData.isConnect(i)) {
				if (pDoc->netData.getOutputNode(i) == mname) {
					CString input_node_id = (CString)pDoc->netData.getInputNode(i).c_str();
					if (input_node_id == tname) {
						return(true);
					}
				}
			}
		}
	}
	else {
		string fname = pDoc->netData.getDataField(nindex);
		if ((CString)fname.c_str() == tname) {
			return(true);;
		}
	}
	return(false);
}

void CEvaluateDlg::OnBnClickedEvaluate()
{
	static bool mfirst1 = true;
	CAuxFunctions func;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	double* inputs = new double[ninputs];
	if (inputs != 0) {
		vector<int> inputIndices;
		for (int j = 0; j < ninputs; j++) {
			bool found = false;
			for (int i = 0; i < pDoc->nnet.num_inputs; i++) {
				if (doesInputMatch(j, (CString)pDoc->nnet.netInputs[i].nodeId.c_str())) {
					inputIndices.push_back(i);
					found = true;
					i = pDoc->nnet.num_inputs;
				}
			}
			if (!found) {
				pDoc->addErrorString(_T("Evaluate : unable to find matching input for ") + netInputs[j]);
				return;
			}
		}
		if (inputIndices.size() == ninputs) {
			for (int i = 0; i < ninputs; i++) {
				CString mdata = _T("");
				input_values[i].GetWindowText(mdata);
				double data = (double)_tstof((LPCTSTR)mdata);
				inputs[inputIndices[i]] = pDoc->processData(netInputs[i], data);
				int dindex = pDoc->netData.getIndexFromID(string(CT2CA(netInputs[i])));
				string process = pDoc->netData.getNodePreProcessor(dindex);
				if (mfirst1 && (process != "NormLinear") && (process != "NormLog") && (process != "")) {
					//AfxMessageBox(_T("Note that function ") + process + _T(" for ") + netInputs[i] + _T(" isn't processed for Evaluate and that processed value must be entered directly"));
					mfirst1 = false;
				}
			}


			pDoc->nnet.getInputDataLabels();
			vector<doubleKvPair> input_data;
			for (int i = 0; i < pDoc->nnet.netInputs.size(); i++) {
				if (pDoc->nnet.netInputs[i].nodeId.substr(0, 7) != "Output:") {
					doubleKvPair tval;
					tval.key = pDoc->nnet.netInputs[i].nodeId;
					tval.value = 0.0;
					input_data.push_back(tval);
					//pDoc->addErrorString("input key: " + tval.key);
				}
			}
			pDoc->nnet.testForward(input_data, false);




			pDoc->nnet.forward(inputs, false);
			if (use_images_as_outputs) {
				int imsize2 = pDoc->getOutputImageSize();
				int bmHeight = pDoc->getDataImageHeight();
				int bmWidth = pDoc->getDataImageWidth();
				int bmWidthBytes = bmWidth;
				bool gray = pDoc->getDataImageGray();
				if (!gray) {
					bmWidthBytes = 3 * bmWidthBytes;
				}
				unsigned char* mbits = new unsigned char[imsize2];
				if (mbits == 0) {
					return;
				}
				for (int i = 0; i < imsize2; i++) {
					mbits[i] = (int)(255 * pDoc->nnet.getOutput(i));
				}
				unsigned char* image_data = new unsigned char[imsize2];
				if (image_data != 0) {
					for (int i = 0; i < imsize2; i++) {
						image_data[i] = 80;
					}
					int index2 = 0;
					for (int j = 0; j < bmHeight; j++) {
						for (int i = 0; i < bmWidth; i++) {
							index2 = (j * bmWidthBytes) + (i * 3);
							image_data[index2 + 2] = mbits[index2 + 2];
							image_data[index2 + 1] = mbits[index2 + 1];
							image_data[index2 + 0] = mbits[index2 + 0];
						}
					}
					CImage mimage;
					mimage.Create(bmWidth, bmHeight, 24);
					if (gray) {
						int gclr = 0;
						int index = 0;
						for (int j = 0; j < bmHeight; j++) {
							for (int i = 0; i < bmWidth; i++) {
								index = (j * bmWidthBytes) + (i * 3);
								gclr = (image_data[index] + image_data[index + 1] + image_data[index + 2]) / 3;
								if ((index + 2) < imsize2) {
									mimage.SetPixel(i, bmHeight - j - 1, RGB(gclr, gclr, gclr));
								}
							}
						}
					}
					else {
						int index = 0;
						for (int j = 0; j < bmHeight; j++) {
							for (int i = 0; i < bmWidth; i++) {
								index = (j * bmWidthBytes) + (i * 3);
								if ((index + 2) < imsize2) {
									mimage.SetPixel(i, bmHeight - j - 1, RGB(image_data[index + 2], image_data[index + 1], image_data[index]));
								}
							}
						}
					}
					delete[] image_data;
					int x = x_offset + static_width + x_separation + edit_width + (4 * x_separation);
					int y = (1 * image_border) + (1 * y_offset) + 15;// +95;
					CClientDC dc(this);
					mimage.Draw(dc, x, y);
				}
				delete[] mbits;
			}
			else {
				for (int i = 0; i < noutputs; i++) {
					CString mname = netOutputs[i];
					double data = pDoc->reProcessData(mname, pDoc->nnet.getOutput(i));
					output_values[i].SetWindowText(func.convertNumber(data, 4));
				}
			}
			delete[] inputs;
		}
		else {
			CString s2 = _T("");
			s2.Format(_T(" : inputIndices.size (%i) not equal to ninputs (%i)"), (int)inputIndices.size(), ninputs);
			pDoc->addErrorString(_T("OnBnClickedEvaluate : unable to match inputs") + s2);
		}
	}
}






