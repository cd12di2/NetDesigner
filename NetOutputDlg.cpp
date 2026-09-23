// NetOutputDlg.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "NetOutputDlg.h"
#include "CDataOutputOptionsDlg.h"
#include "afxdialogex.h"


// NetOutputDlg dialog

bool already3 = false;
CRect lastRect3(0, 0, 0, 0);

IMPLEMENT_DYNAMIC(NetOutputDlg, CDialog)

NetOutputDlg::NetOutputDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_OUTPUT_VIEW, pParent)
{
	ncols = 0;
	trows = 0;
	vrows = 0;
	dcols = 0;
	drows = 0;
	dialog_up = false;
	extend_last_column_width = false;
	classification_matrix = false;
	extend_last_column_characters = 40; //200;
	max_train_data = 5000;
	max_validation_data = max_train_data / 2;
	max_display_data = 240000;
	trainClassificationAccuracy = 0.0;
	validationClassificationAccuracy = 0.0;
	trainClassificationCounts.clear();
	validationClassificationCounts.clear();
}

NetOutputDlg::~NetOutputDlg()
{
}

void NetOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_control);
	DDX_Control(pDX, IDOK, m_ok_button_control);
	DDX_Control(pDX, IDC_SAVE_CSV, m_save_control);
	DDX_Control(pDX, IDC_SHOW_ALL, m_show_ctrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
}


BEGIN_MESSAGE_MAP(NetOutputDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_SAVE_CSV, &NetOutputDlg::OnBnClickedSaveCsv)
	ON_BN_CLICKED(IDC_SHOW_ALL, &NetOutputDlg::OnBnClickedShowAll)
END_MESSAGE_MAP()


// NetOutputDlg message handlers


BOOL NetOutputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_brush.CreateSolidBrush(GetGlobalData()->clrBarFace);

	adjustSize();

	//getData();  // Using progress dialog in a modal dialog box seems to cause a flash to main windows

	fillHeaders();
	fillData();

	if (already3) {
		MoveWindow(lastRect3);
	}

	m_list_control.SetExtendedStyle(m_list_control.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_progress_ctrl.ShowWindow(false);

	dialog_up = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void NetOutputDlg::OnOK()
{
	GetWindowRect(lastRect3);
	already3 = true;
	MoveWindow(lastRect3);
	CDialog::OnOK();
}

void NetOutputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	adjustSize();
}

void NetOutputDlg::adjustSize()
{
	CRect rect1;
	CRect rect2;
	CRect rect3;
	CRect rect4;
	CRect rect5;
	CRect rect6;
	GetWindowRect(&rect1);
	m_ok_button_control.GetWindowRect(&rect2);
	m_save_control.GetWindowRect(&rect3);
	m_list_control.GetWindowRect(&rect4);
	m_show_ctrl.GetWindowRect(&rect5);
	m_progress_ctrl.GetWindowRect(&rect6);
	m_ok_button_control.MoveWindow(rect1.Width() - 80, rect1.Height() - 83, rect2.Width(), rect2.Height(), true);
	m_show_ctrl.MoveWindow(9, rect1.Height() - 83, rect5.Width(), rect3.Height(), true);
	m_save_control.MoveWindow(rect5.Width() + 9 + 9, rect1.Height() - 83, rect3.Width(), rect3.Height(), true);
	m_list_control.MoveWindow(9, 12, rect1.Width() - 37, rect1.Height() - 103, true);
	int pwidth = rect1.Width() - (rect5.Width() + 9 + 9 + rect3.Width() + 80 + 9 + 9);
	m_progress_ctrl.MoveWindow(rect5.Width() + 9 + 9 + rect3.Width() + 9, rect1.Height() - 83 + 2, pwidth, rect3.Height() - 4);
}

void NetOutputDlg::fillHeaders()
{
	m_list_control.DeleteAllItems();
	for (int i = 0; i < ncols; i++) {
		m_list_control.DeleteColumn(0);
	}
	ncols = 0;
	m_list_control.InsertColumn(1, _T(""), LVCFMT_CENTER, 40);
	ncols++;
	for (int i = 0; i < doutputheaders.size(); i++) {
		int width = 20 + (7 * doutputheaders[i].GetLength());
		if (width < 100) {
			width = 100;
		}
		m_list_control.InsertColumn(ncols + 1, doutputheaders[i], LVCFMT_CENTER, width);
		ncols++;
	}
	for (int i = 0; i < dinputheaders.size(); i++) {
		int width = 20 + (7 * dinputheaders[i].GetLength());
		if (width < 80) {
			width = 80;
		}
		if (extend_last_column_width && (i == (dinputheaders.size() - 1))) {
			width = 20 + (7 * extend_last_column_characters);
			if (width < 500) {
				//width = 500;
			}
		}
		m_list_control.InsertColumn(ncols + 1, dinputheaders[i], LVCFMT_CENTER, width);
		ncols++;
	}
}

void NetOutputDlg::fillData()
{
	if (dcols == (int)(doutputheaders.size() + dinputheaders.size())) {
		CString s = _T("");
		int stop = drows;
		if (stop > max_display_data) {
			//CWnd* pWndMain = AfxGetMainWnd();
			//CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
			//CString s = _T("");
			//s.Format(_T("Output display data size = %i - limited to %i"), stop, max_display_data);
			//pDoc->addErrorString(s);
			stop = max_display_data;
		}
		for (int i = 0; i < stop; i++) {
			s.Format(_T("%i"), i + 1);
			int nIndex = m_list_control.InsertItem(i + 1, s);
			for (int j = 0; j < doutputheaders.size(); j++) {
				int dindex = (i * dcols) + j;
				if (dindex < ddata.size()) {
					m_list_control.SetItemText(nIndex, j + 1, ddata[dindex]);
				}
			}
			for (int j = 0; j < dinputheaders.size(); j++) {
				int dindex = (i * dcols) + (int)doutputheaders.size() + j;
				if (dindex < ddata.size()) {
					m_list_control.SetItemText(nIndex, j + (int)doutputheaders.size() + 1, ddata[dindex]);
				}
			}
		}
	}
	else {
		CString s = _T("");
		s.Format(_T("dcols = %i : num outputheaders = %i : num inputheaders = %i"), dcols, (int)doutputheaders.size(), (int)dinputheaders.size());
		AfxMessageBox(s);
	}
}

HBRUSH NetOutputDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return m_brush;
}

BOOL NetOutputDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnEraseBkgnd(pDC);
}

void NetOutputDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(400, 300);
}

void NetOutputDlg::OnBnClickedShowAll()
{
	ddata.clear();
	dinputheaders.clear();
	doutputheaders.clear();
	int tncols = ncols;
	ncols = dcols = drows = trows = vrows = 0;
	getData(true);
	m_list_control.DeleteAllItems();
	for (int i = 0; i < tncols; i++) {
		m_list_control.DeleteColumn(0);
	}
	fillHeaders();
	fillData();
}

void NetOutputDlg::getData(bool all_rows, bool include_inputs)
{
	vector<CString> inputheaders;
	vector<CString> inputheadernodes;
	vector<CString> inputheaders3;
	vector<CString> outputheaders;
	vector<CString> outputheadernodes;
	vector<CString> data;
	vector<int> windices;
	CAuxFunctions func;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	bool needs_word_column = false;
	bool has_convolution = pDoc->hasConvolution();

	if (all_rows) {
		max_train_data = 2000000;
		max_validation_data = max_train_data / 2;
	}

	for (int i = 0; i < pDoc->nnet.num_outputs; i++) {
		//pDoc->addErrorString("nnet output : " + pDoc->nnet.netOutputs[i].nodeId + " : " + pDoc->nnet.netOutputs[i].outputField + " : " + pDoc->nnet.netOutputs[i].processdata.process);
		outputheaders.push_back((CString)pDoc->nnet.netOutputs[i].outputField.c_str());
		outputheadernodes.push_back((CString)pDoc->nnet.netOutputs[i].nodeId.c_str());
	}
	for (int i = 0; i < pDoc->nnet.num_inputs; i++) {
		if (pDoc->nnet.netInputs[i].inputFields.size() > 0) {
			for (int j = 0; j < (int)pDoc->nnet.netInputs[i].inputFields.size(); j++) {
				//pDoc->addErrorString("nnet input : " + pDoc->nnet.netInputs[i].nodeId + " : " + pDoc->nnet.netInputs[i].inputFields[j] + " : " + pDoc->nnet.netInputs[i].processdata.process);
				inputheaders.push_back((CString)pDoc->nnet.netInputs[i].inputFields[j].c_str());
				inputheadernodes.push_back((CString)pDoc->nnet.netInputs[i].nodeId.c_str());
			}
		}
	}
	if (has_convolution) {
		if (pDoc->nnet.imageTrainIndices.size() > 0) {
			inputheaders3.push_back(_T("Filename"));
		}
	}

	int index1 = 0;
	int index2 = 0;
	CString s = _T("");
	double tdata = 0.0;

	trows = pDoc->nnet.num_filedata_rows;
	if (trows > max_train_data) {
		trows = max_train_data;
	}
	drows = trows;

	vrows = 0;
	if ((pDoc->nnet.num_validationdata_rows > 0) && (pDoc->nnet.num_filedata_rows > 0)) {
		vrows = pDoc->nnet.num_validationdata_rows;
		if (vrows > max_validation_data) {
			vrows = (2 * trows * pDoc->nnet.num_validationdata_rows) / pDoc->nnet.num_filedata_rows;
		}
	}

	int mcnt = 0;
	int tcnt = 0;
	int ttotal = 1 + trows + vrows;

	//s.Format(_T("outputheaders.size = %i : nword_vector = %i : nnet num_outputs = %i"), (int)outputheaders.size(), tparams.nword_vector, pDoc->nnet.num_outputs);
	//addErrorString(s);

	CStatus progress;
	if (!dialog_up) {
		progress.CreateDlg();
		progress.setLabel(_T("Calculating data . . ."));
		progress.setProgress(0);
	}
	else if (all_rows) {
		m_progress_ctrl.ShowWindow(true);
		m_progress_ctrl.SetPos(0);
	}

	vector<double> mdata;
	for (int j = 0; j < outputheaders.size(); j++) {
		mdata.push_back(0.0);
	}

	vector<int> inputHeaderIndices;
	vector<int> outputHeaderIndices;
	if (((int)pDoc->nnet.netInputs.size() == pDoc->nnet.num_inputs) && (inputheaders.size() > 0)) {
		for (int j = 0; j < (int)inputheaders.size(); j++) {
			bool found = false;
			for (int k = 0; k < pDoc->nnet.num_inputs; k++) {
				if (pDoc->nnet.netInputs[k].inputFields.size() > 0) {
					for (int m = 0; m < (int)pDoc->nnet.netInputs[k].inputFields.size(); m++) {
						//pDoc->addErrorString(inputheaders[j] + _T(" : ") + (CString)pDoc->nnet.netInputs[k].inputFields[m].c_str());
						if (inputheaders[j] == (CString)pDoc->nnet.netInputs[k].inputFields[m].c_str()) {
							inputHeaderIndices.push_back(k);
							m = (int)pDoc->nnet.netInputs[k].inputFields.size();
							found = true;
						}
					}
					if (found) {
						k = pDoc->nnet.num_inputs;
					}
				}
			}
			if (!found) {
				pDoc->addErrorString(_T("NetOutputDlg::getData : unable to find a match for input field ") + inputheaders[j] + _T(" in net"));
				return;
			}
		}
	}
	else if ((int)pDoc->nnet.netInputs.size() != pDoc->nnet.num_inputs) {
		CString s = _T("");
		s.Format(_T(" : netInputs size = %i : num_inputs = %i"), (int)pDoc->nnet.netInputs.size(), pDoc->nnet.num_inputs);
		pDoc->addErrorString(_T("NetOutputDlg::getData : netInputs size and num_inputs don't match") + s);
		return;
	}
	if (((int)pDoc->nnet.netOutputs.size() == pDoc->nnet.num_outputs) && (outputheaders.size() > 0)) {
		for (int j = 0; j < (int)outputheaders.size(); j++) {
			bool found = false;
			for (int k = 0; k < pDoc->nnet.num_outputs; k++) {
				//pDoc->addErrorString(_T("outputheader ") + outputheaders[j] + _T(" : ") + (CString)pDoc->nnet.netOutputs[k].outputField.c_str());
				if ((outputheaders[j] == (CString)pDoc->nnet.netOutputs[k].outputField.c_str()) || (outputheaders[j] == (CString)pDoc->nnet.netOutputs[k].outputFieldAlias.c_str())) {
					outputHeaderIndices.push_back(k);
					k = pDoc->nnet.num_outputs;
					found = true;
				}
			}
			if (!found) {
				pDoc->addErrorString(_T("NetOutputDlg::getData : unable to find a match for output field ") + outputheaders[j] + _T(" in net"));
				return;
			}
		}
	}
	else {
		pDoc->addErrorString(_T("NetOutputDlg::getData : netOutputs size and num_outputs don't match "));
		return;
	}

	int i = 0;
	int efindex = 0;
	train_index data_index;
	data_index.dindex = 0;
	data_index.findex = 0;
	data_index.sindex = 0;
	while (i < trows) {
		index1 = data_index.dindex * pDoc->nnet.num_inputs;
		index2 = data_index.dindex * pDoc->nnet.num_outputs;
		efindex = data_index.findex;
		pDoc->nnet.loadExternalDataToNet(data_index);
		pDoc->nnet.copyOutputsToInputs(data_index.dindex);
		pDoc->nnet.forward(index1, false);
		data_index = pDoc->nnet.updateTrainIndexNoRandom(data_index);
		for (int j = 0; j < outputheaders.size(); j++) {
			double tdata = pDoc->nnet.getOutput(j);
			mdata[j] = mdata[j] + tdata;
		}
		for (int j = 0; j < outputheaders.size(); j++) {
			double tdata = pDoc->nnet.getOutputTrainData(index2 + outputHeaderIndices[j]);
			double odata = mdata[j];
			s.Format(_T("%.3lf (%.3f)"), pDoc->reProcessData(outputheadernodes[j], tdata), pDoc->reProcessData(outputheadernodes[j], odata));
			data.push_back(s);
		}
		if (inputheaders.size() > 0) {
			s = _T("");
			for (int j = 0; j < (int)inputheaders.size(); j++) {
				if (include_inputs) {
					if (inputHeaderIndices[j] >= 0) {
						double tdata = pDoc->nnet.getInputTrainData(index1 + inputHeaderIndices[j]);
						s.Format(_T("%.4lf"), pDoc->reProcessData(inputheadernodes[j], tdata));
					}
				}
				data.push_back(s);
			}
		}
		if (has_convolution) {
			if (pDoc->getDataPathType() == FILE_TYPE_CSV) {
				if (pDoc->nnet.csv_with_filename) {
					if (data_index.dindex < 0) {
						data.push_back(_T("pindex < 0"));
					}
					else if (data_index.dindex < (int)pDoc->nnet.csv_image_filenames.size()) {
						CString pname = (CString)pDoc->nnet.csv_image_filenames[data_index.dindex].c_str();
						data.push_back(pname);
						extend_last_column_width = true;
						if (pname.GetLength() > extend_last_column_characters) {
							extend_last_column_characters = pname.GetLength();
						}
					}
					else {
						data.push_back(_T("pindex >= dataPathnames.size"));
					}
				}
			}
			else {
				if (efindex < (int)pDoc->nnet.processedData.size()) {
					CString pname = (CString)pDoc->nnet.processedData[efindex].file_path.c_str();
					data.push_back(pname);
					extend_last_column_width = true;
					if (pname.GetLength() > extend_last_column_characters) {
						extend_last_column_characters = pname.GetLength();
					}
				}
				else {
					data.push_back(_T("pindex >= dataPathnames.size"));
				}
			}
		}

		for (int j = 0; j < outputheaders.size(); j++) {
			mdata[j] = 0;
		}
		if (mcnt > 50) {
			if (!dialog_up) {
				progress.setProgress((100 * tcnt) / ttotal);
			}
			else if (all_rows) {
				m_progress_ctrl.SetPos((100 * tcnt) / ttotal);
				MSG msg;
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			mcnt = 0;
		}
		tcnt++;
		mcnt++;
		i++;
	}

	if (vrows > 0) {

		efindex = 0;
		data_index.dindex = 0;
		data_index.findex = 0;
		data_index.sindex = 0;

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < outputheaders.size(); j++) {
				data.push_back(_T("0.000"));
			}
			for (int j = 0; j < (int)inputheaders.size(); j++) {
				data.push_back(_T("0.000"));
			}
			if (has_convolution) {
				if (pDoc->nnet.imageTrainIndices.size() > 0) {
					data.push_back(_T(""));
				}
			}
		}

		drows += vrows;

		mdata.clear();
		for (int j = 0; j < outputheaders.size(); j++) {
			mdata.push_back(0.0);
		}

		i = 0;
		while (i < vrows) {
			index1 = data_index.dindex * pDoc->nnet.num_inputs;
			index2 = data_index.dindex * pDoc->nnet.num_outputs;
			efindex = data_index.findex;
			pDoc->nnet.loadExternalValidationDataToNet(data_index);
			pDoc->nnet.copyOutputsToInputs(data_index.dindex);
			pDoc->nnet.forward(pDoc->nnet.inputvalidationdata + index1, false);
			data_index = pDoc->nnet.updateValidationIndex(data_index);
			for (int j = 0; j < outputheaders.size(); j++) {
				double tdata = pDoc->nnet.getOutput(j);
				mdata[j] = mdata[j] + tdata;
			}
			for (int j = 0; j < outputheaders.size(); j++) {
				double tdata = pDoc->nnet.getOutputValidationData(index2 + outputHeaderIndices[j]);
				double odata = mdata[j];
				s.Format(_T("%.3lf (%.3f)"), pDoc->reProcessData(outputheadernodes[j], tdata), pDoc->reProcessData(outputheadernodes[j], odata));
				data.push_back(s);
			}
			if (inputheaders.size() > 0) {
				s = _T("");
				for (int j = 0; j < (int)inputheaders.size(); j++) {
					if (include_inputs) {
						if (inputHeaderIndices[j] >= 0) {
							double tdata = pDoc->nnet.getInputValidationData(index1 + inputHeaderIndices[j]);
							s.Format(_T("%.4lf"), pDoc->reProcessData(inputheadernodes[j], tdata));
						}
					}
					data.push_back(s);
				}
			}
			if (has_convolution) {
				if (pDoc->getDataPathType() == FILE_TYPE_CSV) {
					if (pDoc->nnet.csv_with_filename) {
						int mvindex = data_index.dindex + (int)pDoc->nnet.processedData.size();
						if (mvindex < 0) {
							data.push_back(_T("pindex < 0"));
						}
						else if (mvindex < (int)pDoc->nnet.csv_image_filenames.size()) {
							CString pname = (CString)pDoc->nnet.csv_image_filenames[mvindex].c_str();
							data.push_back(pname);
						}
						else {
							data.push_back(_T("pindex >= dataPathnames.size"));
						}
					}
				}
				else {
					if (efindex < (int)pDoc->nnet.processedValidationData.size()) {
						CString pname = (CString)pDoc->nnet.processedValidationData[efindex].file_path.c_str();
						data.push_back(pname);
					}
					else {
						data.push_back(_T("pindex >= processedValidationData.size"));
					}
				}
			}
			for (int j = 0; j < outputheaders.size(); j++) {
				mdata[j] = 0;
			}
			if (mcnt > 50) {
				if (!dialog_up) {
					progress.setProgress((100 * tcnt) / ttotal);
				}
				mcnt = 0;
			}
			tcnt++;
			mcnt++;
			i++;
		}
	}

	vector<CString> inputheaders2;
	if ((inputheadernodes.size() > 0) && (inputheaders.size() > 0)) {
		for (int i = 0; i < inputheaders.size(); i++) {
			inputheaders2.push_back(inputheaders[i] + _T("_") + inputheadernodes[i]);
		}
	}
	if (inputheaders3.size() > 0) {
		for (int i = 0; i < inputheaders3.size(); i++) {
			inputheaders2.push_back(inputheaders3[i]);
		}
	}
	dinputheaders = inputheaders2;

	vector<CString> outputheaders2;
	if ((outputheadernodes.size() > 0) && (outputheaders.size() > 0)) {
		for (int i = 0; i < outputheaders.size(); i++) {
			outputheaders2.push_back(outputheaders[i] + _T(" (") + outputheadernodes[i] + _T(")"));
		}
	}
	doutputheaders = outputheaders2;

	dcols = (int)outputheaders2.size() + (int)inputheaders2.size();
	ddata = data;

	int test_limit = max_train_data;
	if (pDoc->nnet.num_validationdata_rows > 0) {
		test_limit += max_validation_data;
	}
	if ((drows < test_limit) || all_rows) {
		m_show_ctrl.EnableWindow(false);
	}

	if (!dialog_up) {
		progress.DestroyDlg();
	}
	m_progress_ctrl.ShowWindow(false);

}

/*CString NetOutputDlg::getWordError(textParameters tparams, vector<int> windices, int windex, int wfindex)
{
	CString words = _T("");

	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	for (int j = 0; j < windices.size(); j++) {
		if (j > 0) {
			words += _T(", ");
		}
		int mindex = windex + windices[j];
		if (mindex < 0) {
			mindex = 0;
		}
		else if (mindex >= pDoc->textWords[wfindex].size()) {
			mindex = (int)pDoc->textWords[wfindex].size() - 1;
		}
		words += pDoc->textWords[wfindex][mindex];
		if (pDoc->nnet.num_outputs == tparams.nword_vector) {
			float merr = 1;
			float delta = 0;
			float target = 0;
			float my_err = 0;
			float sum_err = 0;
			float highest = 0;
			for (int k = 0; k < pDoc->textDictionary.size(); k++) {
				merr = 1;
				for (int m = 0; m < tparams.nword_vector; m++) {
					target = 0.05F;
					if (pDoc->textDictionary[k].hindices[m] == 1) {
						target = 1.0F - target;
					}
					delta = (target - (float)pDoc->nnet.getOutput(m));
					if (delta < 0) {
						delta = -1 * delta;
					}
					merr = merr * (1 - delta);
				}
				sum_err += merr;
				if (merr > highest) {
					highest = merr;
				}
				if (pDoc->textWords[wfindex][mindex] == pDoc->textDictionary[k].word) {
					my_err = merr;
				}
			}
			if (sum_err) {
				my_err = 100 * my_err / sum_err;
				highest = 100 * highest / sum_err;
			}
			words += _T(" (");
			if (my_err == highest) {
				words += _T("*");
			}
			CString s = _T("");
			if (my_err < 0.01) {
				s.Format(_T("%.4f%%)"), my_err);
			}
			else if (my_err < 0.1) {
				s.Format(_T("%.3f%%)"), my_err);
			}
			else {
				s.Format(_T("%.2f%%)"), my_err);
			}
			words += s;
		}
	}

	return(words);

}*/

void NetOutputDlg::OnBnClickedSaveCsv()
{
	CAuxFunctions func;
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();

	if (pDoc->getDataPathNamesSize(false)) {

		CDataOutputOptionsDlg dodlg;

		dodlg.m_classification = classification_matrix && dodlg.m_classification;

		if (drows > max_display_data) {
			dodlg.m_notes.Format(_T("Num data rows = %i\nSave data limited to %i rows"), drows, max_display_data);
		}
		else {
			dodlg.m_notes.Format(_T("Save data rows = %i"), drows);
		}

		if (dodlg.DoModal() == IDOK) {

			CString myfilename = func.removeExtensionFromFilename(pDoc->GetPathName()) + _T("_NetOutputData.csv");

			CFileDialog dlg2(FALSE, _T("csv"), myfilename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Comma Separated (*.csv)|*.csv||"));

			if (dlg2.DoModal() == IDOK) {

				getData(true, dodlg.m_train_input);

				CFile file;

				myfilename = dlg2.GetPathName();

				// Open csv file
				if (!file.Open(myfilename, CFile::modeCreate | CFile::modeReadWrite)) {
					CString msg = _T("OnBnClickedSaveCsv : Unable to open ") + myfilename;
					AfxMessageBox(msg);
					return;
				}

				// Allocate memory used to parse lines of data
				unsigned long max_data = 50000;
				char* cdata = new char[max_data];
				if (cdata == 0) {
					AfxMessageBox(_T("OnBnClickedSaveCsv : Unable to allocate memory to save csv file"));
					return;
				}

				CString sline = _T("");

				// If we need to add a classification matrix
				if (classification_matrix && dodlg.m_classification) {

					getClassificationMitrices();

					if (trainClassificationCounts.size() > 0) {

						sline = _T("\n");
						if (sline.GetLength() < int(max_data)) {
							for (int j = 0; j < sline.GetLength(); j++) {
								cdata[j] = (char)sline.GetAt(j);
							}
						}
						file.Write(cdata, sline.GetLength());

						sline = _T("Train Classification Matrix\n");
						if (sline.GetLength() < int(max_data)) {
							for (int j = 0; j < sline.GetLength(); j++) {
								cdata[j] = (char)sline.GetAt(j);
							}
						}
						file.Write(cdata, sline.GetLength());

						sline = _T("");
						for (int i = 0; i < doutputheaders.size(); i++) {
							sline += _T(",");
							sline += findFirstWord(doutputheaders[i]);
						}
						sline += _T(",");
						sline += _T("No Actual");
						sline += _T("\n");
						if (sline.GetLength() < int(max_data)) {
							for (int j = 0; j < sline.GetLength(); j++) {
								cdata[j] = (char)sline.GetAt(j);
							}
						}
						file.Write(cdata, sline.GetLength());

						for (int m = 0; m < trainClassificationCounts.size(); m++) {

							if (m < ((int)trainClassificationCounts.size() - 1)) {
								if (m < doutputheaders.size()) {
									sline = findFirstWord(doutputheaders[m]);
								}
								else {
									sline = _T("");
								}
							}
							else {
								sline = _T("No Predict");
							}
							for (int n = 0; n < trainClassificationCounts[m].size(); n++) {
								CString s = _T("");
								s.Format(_T(",%i"), trainClassificationCounts[m][n]);
								sline += s;
							}
							sline += _T("\n");

							if (sline.GetLength() < int(max_data)) {
								for (int j = 0; j < sline.GetLength(); j++) {
									cdata[j] = (char)sline.GetAt(j);
								}
							}
							file.Write(cdata, sline.GetLength());

						}

						sline.Format(_T("Train Classification Accuracy = %.1f%%\n"), trainClassificationAccuracy);
						if (sline.GetLength() < int(max_data)) {
							for (int j = 0; j < sline.GetLength(); j++) {
								cdata[j] = (char)sline.GetAt(j);
							}
						}
						file.Write(cdata, sline.GetLength());

						if (validationClassificationCounts.size() > 0) {

							sline = _T("\n");
							if (sline.GetLength() < int(max_data)) {
								for (int j = 0; j < sline.GetLength(); j++) {
									cdata[j] = (char)sline.GetAt(j);
								}
							}
							file.Write(cdata, sline.GetLength());

							sline = _T("Validation Classification Matrix\n");
							if (sline.GetLength() < int(max_data)) {
								for (int j = 0; j < sline.GetLength(); j++) {
									cdata[j] = (char)sline.GetAt(j);
								}
							}
							file.Write(cdata, sline.GetLength());

							sline = _T("");
							for (int i = 0; i < doutputheaders.size(); i++) {
								sline += _T(",");
								sline += findFirstWord(doutputheaders[i]);
							}
							sline += _T(",");
							sline += _T("No Actual");
							sline += _T("\n");
							if (sline.GetLength() < int(max_data)) {
								for (int j = 0; j < sline.GetLength(); j++) {
									cdata[j] = (char)sline.GetAt(j);
								}
							}
							file.Write(cdata, sline.GetLength());

							for (int m = 0; m < validationClassificationCounts.size(); m++) {

								if (m < ((int)validationClassificationCounts.size() - 1)) {
									if (m < doutputheaders.size()) {
										sline = findFirstWord(doutputheaders[m]);
									}
									else {
										sline = _T("");
									}
								}
								else {
									sline = _T("No Predict");
								}
								for (int n = 0; n < validationClassificationCounts[m].size(); n++) {
									CString s = _T("");
									s.Format(_T(",%i"), validationClassificationCounts[m][n]);
									sline += s;
								}
								sline += _T("\n");

								if (sline.GetLength() < int(max_data)) {
									for (int j = 0; j < sline.GetLength(); j++) {
										cdata[j] = (char)sline.GetAt(j);
									}
								}
								file.Write(cdata, sline.GetLength());

							}

							sline.Format(_T("Validation Classification Accuracy = %.1f%%\n"), validationClassificationAccuracy);
							if (sline.GetLength() < int(max_data)) {
								for (int j = 0; j < sline.GetLength(); j++) {
									cdata[j] = (char)sline.GetAt(j);
								}
							}
							file.Write(cdata, sline.GetLength());

						}

					}

				}

				sline = _T("\n");
				if (sline.GetLength() < int(max_data)) {
					for (int j = 0; j < sline.GetLength(); j++) {
						cdata[j] = (char)sline.GetAt(j);
					}
				}
				file.Write(cdata, sline.GetLength());

				if ((dodlg.m_net_output) || (dodlg.m_train_output) || (dodlg.m_train_input)) {

					bool addFilename = false;
					sline.Format(_T("Train Data - %i rows\n"), trows);
					if (sline.GetLength() < int(max_data)) {
						for (int j = 0; j < sline.GetLength(); j++) {
							cdata[j] = (char)sline.GetAt(j);
						}
					}
					file.Write(cdata, sline.GetLength());
					if (vrows > 0) {
						sline.Format(_T("Validation Data - %i rows\n"), (vrows - 2));
						if (sline.GetLength() < int(max_data)) {
							for (int j = 0; j < sline.GetLength(); j++) {
								cdata[j] = (char)sline.GetAt(j);
							}
						}
						file.Write(cdata, sline.GetLength());
					}

					sline = _T("#");
					for (int i = 0; i < doutputheaders.size(); i++) {
						if (dodlg.m_train_output) {
							sline += _T(",");
							sline += findFirstWord(doutputheaders[i]);
						}
						if ((dodlg.m_train_output) && (dodlg.m_net_output)) {
							sline += _T(",");
							sline += findSecondWord(doutputheaders[i]);
						}
						else if (dodlg.m_net_output) {
							sline += _T(",");
							sline += findFirstWord(doutputheaders[i]);
						}
					}
					if (dodlg.m_train_input) {
						for (int i = 0; i < dinputheaders.size(); i++) {
							sline += _T(",");
							sline += dinputheaders[i];
						}
					}
					else if (dinputheaders[dinputheaders.size() - 1] == _T("Filename")) {
						sline += _T(",");
						sline += dinputheaders[dinputheaders.size() - 1];
						addFilename = true;
					}

					sline += _T("\n");
					if (sline.GetLength() < int(max_data)) {
						for (int j = 0; j < sline.GetLength(); j++) {
							cdata[j] = (char)sline.GetAt(j);
						}
					}
					file.Write(cdata, sline.GetLength());

					int stop = drows;
					/*if (stop >(2 * max_display_data)) {
						CString s = _T("");
						s.Format(_T("CSV data size (%i) exceeds limit (%i) - save all anyway?"), stop, (2 * max_display_data));
						if (AfxMessageBox(s, MB_YESNO) == IDNO) {
							stop = (2 * max_display_data);
						}
					}*/

					for (int i = 0; i < stop; i++) {
						sline.Format(_T("%i"), i + 1);
						for (int j = 0; j < doutputheaders.size(); j++) {
							int dindex = (i * dcols) + j;
							if (dindex < ddata.size()) {
								if (dodlg.m_train_output) {
									sline += _T(",");
									sline += findFirstWord(ddata[dindex]);
								}
								if (dodlg.m_net_output) {
									sline += _T(",");
									sline += findSecondWord(ddata[dindex]);
								}
							}
						}
						if (dodlg.m_train_input) {
							for (int j = 0; j < dinputheaders.size(); j++) {
								int dindex = (i * dcols) + (int)doutputheaders.size() + j;
								if (dindex < ddata.size()) {
									sline += _T(",");
									sline += ddata[dindex];
								}
							}
						}
						else if (addFilename) {
							int j = (int)dinputheaders.size() - 1;
							int dindex = (i * dcols) + (int)doutputheaders.size() + j;
							if (dindex < ddata.size()) {
								sline += _T(",");
								sline += ddata[dindex];
							}
						}
						sline += _T("\n");
						if (sline.GetLength() < int(max_data)) {
							for (int j = 0; j < sline.GetLength(); j++) {
								cdata[j] = (char)sline.GetAt(j);
							}
						}
						file.Write(cdata, sline.GetLength());
					}

				}

				file.Flush();
				file.Close();

				delete[] cdata;

			}

		}

	}

}

CString NetOutputDlg::findFirstWord(CString word)
{
	int index1 = 0;
	while ((index1 < word.GetLength()) && (word[index1] != '(')) {
		index1++;
	}
	index1--;
	while ((index1 > 0) && (word[index1] == ' ')) {
		index1--;
	}
	index1++;
	return(word.Left(index1));
}

CString NetOutputDlg::findSecondWord(CString word)
{
	int index1 = 0;
	int index2 = word.GetLength();
	while ((index1 < word.GetLength()) && (word[index1] != '(')) {
		index1++;
	}
	index1++;
	index2 = index1;
	while ((index2 < word.GetLength()) && (word[index2] != ')')) {
		index2++;
	}
	//index2--;
	return(word.Mid(index1, (index2 - index1)));
}

double NetOutputDlg::getClassificationMitrices()
{
	trainClassificationAccuracy = 0.0;
	validationClassificationAccuracy = 0.0;

	trainClassificationCounts.clear();
	validationClassificationCounts.clear();

	for (int i = 0; i <= doutputheaders.size(); i++) {
		vector<int> tcounts;
		for (int j = 0; j <= doutputheaders.size(); j++) {
			tcounts.push_back(0);
		}
		trainClassificationCounts.push_back(tcounts);
	}

	for (int i = 0; i <= doutputheaders.size(); i++) {
		vector<int> tcounts;
		for (int j = 0; j <= doutputheaders.size(); j++) {
			tcounts.push_back(0);
		}
		validationClassificationCounts.push_back(tcounts);
	}

	int hits = 0;
	int total = 0;

	int i = 0;
	bool stop = false;
	while ((i < drows) && !stop) {

		int dindex = i * dcols;
		if (findSecondWord(ddata[dindex]) == _T("")) {
			stop = true;
		}

		if (!stop) {

			vector<int> match_indices;
			for (int j = 0; j < doutputheaders.size(); j++) {
				int dindex = (i * dcols) + j;
				if (dindex < ddata.size()) {
					float target = (float)_tstof((LPCTSTR)findFirstWord(ddata[dindex]));
					if (target > 0.5) {
						match_indices.push_back(j);
					}
				}
			}

			if (match_indices.size() > 0) {
				for (int k = 0; k < match_indices.size(); k++) {
					bool found = false;
					for (int j = 0; j < doutputheaders.size(); j++) {
						bool add = true;
						for (int m = 0; m < match_indices.size(); m++) {
							if ((match_indices[m] == j) && (match_indices[m] != match_indices[k])) {
								add = false;
							}
						}
						if (add) {
							int dindex = (i * dcols) + j;
							if (dindex < ddata.size()) {
								float noutput = (float)_tstof((LPCTSTR)findSecondWord(ddata[dindex]));
								if (noutput > 0.5) {
									trainClassificationCounts[j][match_indices[k]] = trainClassificationCounts[j][match_indices[k]] + 1;
									if (j == match_indices[k]) {
										hits++;
									}
									total++;
									found = true;
								}
							}
						}
					}
					if (!found) {
						trainClassificationCounts[(int)doutputheaders.size()][match_indices[k]] = trainClassificationCounts[(int)doutputheaders.size()][match_indices[k]] + 1;
						total++;
					}
				}
			}
			else {
				bool found = false;
				for (int j = 0; j < doutputheaders.size(); j++) {
					int dindex = (i * dcols) + j;
					if (dindex < ddata.size()) {
						float noutput = (float)_tstof((LPCTSTR)findSecondWord(ddata[dindex]));
						if (noutput > 0.5) {
							trainClassificationCounts[j][(int)doutputheaders.size()] = trainClassificationCounts[j][(int)doutputheaders.size()] + 1;
							total++;
							found = true;
						}
					}
				}
				if (!found) {
					trainClassificationCounts[(int)doutputheaders.size()][(int)doutputheaders.size()] = trainClassificationCounts[(int)doutputheaders.size()][(int)doutputheaders.size()] + 1;
					hits++;
					total++;
				}
			}

		}

		i++;

	}

	if (total > 0) {
		trainClassificationAccuracy = 100 * (double)hits / (double)total;
	}

	while ((i < drows) && stop) {
		i++;
		stop = false;
		int dindex = i * dcols;
		if (findSecondWord(ddata[dindex]) == _T("")) {
			stop = true;
		}
	}

	hits = 0;
	total = 0;

	while ((i < drows) && !stop) {

		int dindex = i * dcols;

		if (dindex < ddata.size()) {

			if (findSecondWord(ddata[dindex]) == _T("")) {
				stop = true;
			}

			if (!stop) {

				vector<int> match_indices;
				for (int j = 0; j < doutputheaders.size(); j++) {
					int dindex = (i * dcols) + j;
					if (dindex < ddata.size()) {
						float target = (float)_tstof((LPCTSTR)findFirstWord(ddata[dindex]));
						if (target > 0.5) {
							match_indices.push_back(j);
						}
					}
				}

				if (match_indices.size() > 0) {
					for (int k = 0; k < match_indices.size(); k++) {
						bool found = false;
						for (int j = 0; j < doutputheaders.size(); j++) {
							bool add = true;
							for (int m = 0; m < match_indices.size(); m++) {
								if ((match_indices[m] == j) && (match_indices[m] != match_indices[k])) {
									add = false;
								}
							}
							if (add) {
								int dindex = (i * dcols) + j;
								if (dindex < ddata.size()) {
									float noutput = (float)_tstof((LPCTSTR)findSecondWord(ddata[dindex]));
									if (noutput > 0.5) {
										validationClassificationCounts[j][match_indices[k]] = validationClassificationCounts[j][match_indices[k]] + 1;
										if (j == match_indices[k]) {
											hits++;
										}
										total++;
										found = true;
									}
								}
							}
						}
						if (!found) {
							validationClassificationCounts[(int)doutputheaders.size()][match_indices[k]] = validationClassificationCounts[(int)doutputheaders.size()][match_indices[k]] + 1;
							total++;
						}
					}
				}
				else {
					bool found = false;
					for (int j = 0; j < doutputheaders.size(); j++) {
						int dindex = (i * dcols) + j;
						if (dindex < ddata.size()) {
							float noutput = (float)_tstof((LPCTSTR)findSecondWord(ddata[dindex]));
							if (noutput > 0.5) {
								validationClassificationCounts[j][(int)doutputheaders.size()] = validationClassificationCounts[j][(int)doutputheaders.size()] + 1;
								total++;
								found = true;
							}
						}
					}
					if (!found) {
						validationClassificationCounts[(int)doutputheaders.size()][(int)doutputheaders.size()] = validationClassificationCounts[(int)doutputheaders.size()][(int)doutputheaders.size()] + 1;
						hits++;
						total++;
					}
				}

			}

		}

		i++;

	}

	if (total > 0) {
		validationClassificationAccuracy = 100 * (double)hits / (double)total;
	}
	else {
		validationClassificationCounts.clear();
	}

	return(trainClassificationAccuracy);

}

