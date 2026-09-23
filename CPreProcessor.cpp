// CPreProcessor.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CPreProcessor.h"
#include "afxdialogex.h"


// CPreProcessor dialog

bool already2 = false;
CRect lastRect2(0, 0, 0, 0);



IMPLEMENT_DYNAMIC(CPreProcessor, CDialog)

CPreProcessor::CPreProcessor(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MATH, pParent)
{
	ready = false;
	displayed = false;
	outputnode = false;
	imagefiles = false;
	soundfiles = false;
	//textfiles = false;
	normalize_only = false;
	tanh = false;
	changed = false;
	normalizeUp = false;
	datetimeUp = false;
	fftUp = false;
	derivativeUp = false;
	fnparserUp = false;
	//dictionaryUp = false;
	//windexUp = false;
	fieldkeyUp = false;
	node_id = _T("");
	input_node_id = _T("");
	input_data = _T("");
	doubledata.clear();
	currentTab = 0;
}

CPreProcessor::~CPreProcessor()
{
}

void CPreProcessor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab1_control);
	DDX_Control(pDX, IDOK, m_ok_control);
}


BEGIN_MESSAGE_MAP(CPreProcessor, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CPreProcessor::OnTcnSelchangeTab1)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPreProcessor message handlers


BOOL CPreProcessor::OnInitDialog()
{
	CDialog::OnInitDialog();

	normalizeDlg.data_updated = false;
	derivativeDlg.data_updated = false;
	fftDlg.data_updated = false;
	datetimeDlg.data_updated = false;
	fnparserDlg.data_updated = false;
	//dictionaryDlg.data_updated = false;
	//windexDlg.data_updated = false;
	fieldkeyDlg.data_updated = false;

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);

	adjustSize();

	m_tab1_control.SetCurSel(currentTab);
	
	setCurrentTab();

	changed = true;
	setDataToDialogs();

	if (already2) {
		MoveWindow(lastRect2);
	}

	m_tab1_control.EnableWindow(true);
	m_tab1_control.ShowWindow(SW_SHOW);

	ready = true;

	setSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreProcessor::CreateDlg()
{
	if (!displayed) {
		Create(IDD_MATH);
		m_tab1_control.EnableWindow(true);
		m_tab1_control.ShowWindow(SW_SHOW);
		displayed = true;
	}
}

void CPreProcessor::DestroyDlg()
{
	DestroyWindow();
	displayed = false;
}

void CPreProcessor::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	setTab(m_tab1_control.GetCurSel());
	setDataToDialogs();
	setSize();	
	*pResult = 0;
}

HBRUSH CPreProcessor::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return mbrush;
}

void CPreProcessor::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(400,420);
}

void CPreProcessor::OnOK()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->updateDataFromEquation();
	GetWindowRect(lastRect2);
	already2 = true;
	displayed = false;
	DestroyDlg();
}

void CPreProcessor::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	adjustSize();
	setSize();
}

void CPreProcessor::adjustSize()
{
	CRect rect1;
	CRect rect2;
	GetWindowRect(&rect1);
	m_ok_control.GetWindowRect(&rect2);
	m_ok_control.MoveWindow(rect1.Width() - 80, rect1.Height() - 83, rect2.Width(), rect2.Height(), true);
	m_tab1_control.MoveWindow(9, 12, rect1.Width() - 37, rect1.Height() - 103, true);
}

void CPreProcessor::setSize()
{
	if (ready) {
		CRect rect1;
		m_tab1_control.GetWindowRect(&rect1);
		CRect rect2;
		rect2.left = 2;
		rect2.top = 22 + 4;
		rect2.right = (rect1.right - rect1.left) - 2;
		rect2.bottom = (rect1.bottom - rect1.top) - 2 + 4;
		normalizeDlg.setSize(rect2);
		datetimeDlg.setSize(rect2);
		derivativeDlg.setSize(rect2);
		fftDlg.setSize(rect2);
		fnparserDlg.setSize(rect2);
		//dictionaryDlg.setSize(rect2);
		//windexDlg.setSize(rect2);
		fieldkeyDlg.setSize(rect2);
	}
}

void CPreProcessor::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialog::OnPaint() for painting messages

}

CString CPreProcessor::getMode()
{
	if (fnparserUp) {
		return(fnparserDlg.getMode());
	}
	else if (normalizeUp) {
		return(normalizeDlg.getMode());
	}
	else if (derivativeUp) {
		return(derivativeDlg.getMode());
	}
	else if (datetimeUp) {
		return(datetimeDlg.getMode());
	}
	else if (fftUp) {
		return(fftDlg.getMode());
	}
	//else if (dictionaryUp) {
		//return(dictionaryDlg.getMode());
	//}
	//else if (windexUp) {
		//return(windexDlg.getMode());
	//}
	else if (fieldkeyUp) {
		return(fieldkeyDlg.getMode());
	}
	return(_T(""));
}

void CPreProcessor::setMode(CString mode, int ftype)
{
	if (ftype == FILE_TYPE_JPG) {
		if (!imagefiles) {
			changed = true;
		}
	}
	else if (imagefiles) {
		changed = true;
	}

	if (ftype == FILE_TYPE_WAV) {
		if (!soundfiles) {
			changed = true;
		}
	}
	else if (soundfiles) {
		changed = true;
	}

	//if (ftype == FILE_TYPE_TXT) {
		//if (!textfiles) {
			//changed = true;
		//}
	//}
	//else if (textfiles) {
		//changed = true;
	//}

	if (changed) {
		m_tab1_control.DeleteAllItems();
	}

	CString tmode = mode.Left(10);
	CString tmode2 = mode.Left(3);
	CString tmode3 = mode.Left(8);

	if ((ftype == FILE_TYPE_JPG)/* && outputnode*/) {

		if ((tmode3 == _T("FNParser")) || (mode == _T(""))) {
			SetWindowText(_T("Pre Processor - ") + node_id + _T(" : File Name Parser"));
			if (changed) {
				m_tab1_control.InsertItem(0, _T(" File Name Parser "));
			}
			fnparserDlg.setMode(mode);
		}
		imagefiles = true;
		soundfiles = false;
		//textfiles = false;

	}
	else if (ftype == FILE_TYPE_WAV) {

		if (outputnode) {
			SetWindowText(_T("Pre Processor - ") + node_id + _T(" : File Name Processor"));
			if (changed) {
				m_tab1_control.InsertItem(0, _T(" File Name Parser "));
			}
			fnparserDlg.setMode(mode);
		}
		else {
			SetWindowText(_T("Pre Processor - ") + node_id + _T(" : FFT Processor"));
			if (changed) {
				m_tab1_control.InsertItem(0, _T(" FFT "));
			}
			fftDlg.setMode(mode);
		}
		imagefiles = false;
		soundfiles = true;
		//textfiles = false;

	}
	/*else if (ftype == FILE_TYPE_TXT) {

		SetWindowText(_T("Pre Processor - ") + node_id + _T(" : Text Processing"));
		if (outputnode) {
			if (changed) {
				m_tab1_control.InsertItem(0, _T(" File Name Parser "));
				m_tab1_control.InsertItem(1, _T(" Word Relative Index "));
				m_tab1_control.InsertItem(2, _T(" Dictionary "));
			}
			fnparserDlg.setMode(mode);
			dictionaryDlg.setMode(mode);
			windexDlg.setMode(mode);
		}
		else {
			if (changed) {
				m_tab1_control.InsertItem(0, _T(" Word Relative Index "));
				m_tab1_control.InsertItem(1, _T(" Dictionary "));
			}
			dictionaryDlg.setMode(mode);
			windexDlg.setMode(mode);
		}
		imagefiles = false;
		soundfiles = false;
		textfiles = true;

	}*/
	else {

		if (imagefiles || soundfiles/* || textfiles*/) {
			changed = true;
		}

		SetWindowText(_T("Pre Processor - ") + input_node_id + _T(" : ") + input_data + _T(""));
		if (changed) {
			m_tab1_control.InsertItem(0, _T(" Normalization "));
			if (!normalize_only) {
				m_tab1_control.InsertItem(1, _T(" Field Key "));
				m_tab1_control.InsertItem(2, _T(" File Name Parser "));
				if (!outputnode) {
					m_tab1_control.InsertItem(3, _T(" Derivative "));
					m_tab1_control.InsertItem(4, _T(" Date/Time "));
					m_tab1_control.InsertItem(5, _T(" FFT "));
				}
			}
		}
		else if (!normalizeUp && !datetimeUp && !fftUp && !derivativeUp && !fieldkeyUp && !fnparserUp/* && !dictionaryUp && !windexUp*/) {
			SetWindowText(_T("Pre Processor"));
			m_tab1_control.InsertItem(0, _T(" Normalization "));
		}

		if ((mode == _T("NormLinear")) || (mode == _T("NormLog"))) {
			normalizeDlg.setMode(mode);
		}
		else if (tmode == _T("Derivative")) {
			derivativeDlg.setMode(mode);
		}
		else if ((mode == _T("NormSeconds")) || (mode == _T("NormMinutes")) || (mode == _T("NormHours")) || (mode == _T("NormDays")) ||
			(mode == _T("Seconds")) || (mode == _T("Minutes")) || (mode == _T("Hours")) || (mode == _T("Days"))) {
			datetimeDlg.setMode(mode);
		}
		else if (tmode2 == _T("FFT")) {
			fftDlg.setMode(mode);
		}
		else if (tmode3 == _T("FieldKey")) {
			fieldkeyDlg.setMode(mode);
		}
		else if (tmode3 == _T("FNParser")) {
			fnparserDlg.setMode(mode);
		}

		imagefiles = false;
		soundfiles = false;
		//textfiles = false;

	}
}

void CPreProcessor::setfileType(int ftype)
{
	if (ftype == FILE_TYPE_JPG) {
		imagefiles = true;
		soundfiles = false;
		//textfiles = false;
	}
	else if (ftype == FILE_TYPE_WAV) {
		imagefiles = false;
		soundfiles = true;
		//textfiles = false;
	}
	else if (ftype == FILE_TYPE_TXT) {
		imagefiles = false;
		soundfiles = false;
		//textfiles = true;
	}
	else {
		imagefiles = false;
		soundfiles = false;
		//textfiles = false;
	}
}

int CPreProcessor::getTabFromMode(CString mode) 
{
	CString tmode = mode.Left(10);
	CString tmode2 = mode.Left(3);
	CString tmode3 = mode.Left(8);
	CString tmode4 = mode.Left(7);
	if ((mode == _T("NormLinear")) || (mode == _T("NormLog"))) {
		return(0);
	}
	else if (tmode3 == _T("FieldKey")) {
		return(1);
	}
	else if (tmode3 == _T("FNParser")) {
		if (imagefiles || soundfiles/* || textfiles*/) {
			return(0);
		}
		else {
			return(2);
		}
	}
	else if (tmode == _T("Derivative")) {
		return(3);
	}
	else if ((mode == _T("NormSecond")) || (mode == _T("NormMinute")) || (mode == _T("NormHours")) || (mode == _T("NormDays")) ||
		(mode == _T("Seconds")) || (mode == _T("Minutes")) || (mode == _T("Hours")) || (mode == _T("Days"))) {
		return(4);
	}
	else if (tmode2 == _T("FFT")) {
		return(5);
	}
	else if (tmode == _T("WordIndex")) {
		if (outputnode) {
			return(1);
		}
		else {
			return(0);
		}
	}
	/*else if (tmode == _T("Dictionary")) {
		if (outputnode) {
			return(2);
		}
		else {
			return(1);
		}
	}*/
	return(0);
}

void CPreProcessor::setOutput(bool toutputnode)
{ 
	if (outputnode != toutputnode) {
		outputnode = toutputnode;
		changed = true;
	}
}

void CPreProcessor::setDataToDialogs()
{
	if (imagefiles/* && outputnode*/) {

		if (fnparserUp) {
			fnparserDlg.setTanh(tanh);
			fnparserDlg.setListBox();
			fnparserDlg.UpdateData(false);
		}
		else {
			addErrorString(_T("setDataToDialogs : fnparserDlg not up"));
		}

	}
	else if (soundfiles) {

		if (outputnode) {
			if (fnparserUp) {
				fnparserDlg.setTanh(tanh);
				fnparserDlg.setListBox();
				fnparserDlg.UpdateData(false);
			}
			else {
				addErrorString(_T("setDataToDialogs : fnparserDlg not up"));
			}
		}
		else {
			if (fftUp) {
				fftDlg.setNodeLabel(node_id);
				fftDlg.setDataLabel(input_data);
				if (doubledata.size() > 0) {
					fftDlg.setData(doubledata[0]);
				}
				fftDlg.UpdateData(false);
				fftDlg.OnCbnSelchangeCombo1();
			}
			else {
				//addErrorString(_T("setDataToDialogs : fftUp not up"));
			}
		}

	}
	/*else if (textfiles) {

		if (outputnode) {
			if (fnparserUp) {
				fnparserDlg.setTanh(tanh);
				fnparserDlg.setListBox();
				fnparserDlg.UpdateData(false);
			}
			else if (dictionaryUp) {
				dictionaryDlg.setData();
				dictionaryDlg.UpdateData(false);
			}
			else if (windexUp) {
				windexDlg.setData();
				windexDlg.UpdateData(false);
			}
			else {
				addErrorString(_T("setDataToDialogs : neither fnparserDlg nor dictionaryDlg are up"));
			}
		}
		else {
			if (dictionaryUp) {
				dictionaryDlg.setData();
				dictionaryDlg.UpdateData(false);
			}
			else if (windexUp) {
				windexDlg.setData();
				windexDlg.UpdateData(false);
			}
		}

	}*/
	else {

		if (normalizeUp) {
			normalizeDlg.setNodeLabel(node_id);
			normalizeDlg.setDataLabel(input_data);
			normalizeDlg.setData(doubledata);
			normalizeDlg.setOutput(outputnode);
			normalizeDlg.setTanh(tanh);
			normalizeDlg.UpdateData(false);
			normalizeDlg.setNormalizedData();
		}
		else if (derivativeUp) {
			derivativeDlg.setNodeLabel(node_id);
			derivativeDlg.setDataLabel(input_data);
			derivativeDlg.setData(doubledata);
			derivativeDlg.setOutput(outputnode);
			derivativeDlg.setTanh(tanh);
			derivativeDlg.UpdateData(false);
			derivativeDlg.getDerivative();
		}
		else if (fieldkeyUp) {
			fieldkeyDlg.setTanh(tanh);
			fieldkeyDlg.setData(stringdata);
			fieldkeyDlg.UpdateData(false);
		}
		else if (datetimeUp) {
			datetimeDlg.setData(stringdata);
			datetimeDlg.UpdateData(false);
			datetimeDlg.setListBox();
		}
		else if (fftUp) {
			fftDlg.setNodeLabel(node_id);
			fftDlg.setDataLabel(input_data);
			if (doubledata.size() > 0) {
				fftDlg.setData(doubledata[0]);
				fftDlg.UpdateData(false);
			}
			fftDlg.OnCbnSelchangeCombo1();
		}
		else if (fnparserUp) {
			fnparserDlg.setTanh(tanh);
			fnparserDlg.setListBox();
			fnparserDlg.UpdateData(false);
		}

	}

}

void CPreProcessor::setCurrentTab()
{
	normalizeUp = false;
	datetimeUp = false;
	fftUp = false;
	derivativeUp = false;
	fnparserUp = false;

	if (currentTab == 0) {
		if (imagefiles || (outputnode && (soundfiles/* || textfiles*/))) {
			fnparserDlg.Create(IDD_IMAGE_NAME_PARSER, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fnparserUp = true;
		}
		else if (soundfiles) {
			fftDlg.Create(IDD_FFT, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fftUp = true;
		}
		/*else if (textfiles) {
			windexDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			windexDlg.RedrawWindow();
			windexUp = true;
		}*/
		else {
			normalizeDlg.Create(IDD_NORMALIZE, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			normalizeUp = true;
		}
	}
	else if (currentTab == 1) {
		/*if (textfiles) {
			if (outputnode) {
				windexDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
				windexDlg.RedrawWindow();
				windexUp = true;
			} 
			else {
				dictionaryDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
				dictionaryDlg.RedrawWindow();
				dictionaryUp = true;
			}
		}
		else {*/
			fieldkeyDlg.Create(IDD_FIELD_KEY, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fieldkeyUp = true;
		//}
	}
	else if (currentTab == 2) {
		/*if (textfiles) {
			if (outputnode) {
				dictionaryDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
				dictionaryDlg.RedrawWindow();
				dictionaryUp = true;
			}
		}
		else {*/
			fnparserDlg.Create(IDD_IMAGE_NAME_PARSER, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fnparserUp = true;
		//}
	}
	else if (currentTab == 3) {
		derivativeDlg.Create(IDD_DERIVATIVE, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		derivativeUp = true;
	}
	else if (currentTab == 4) {
		datetimeDlg.Create(IDD_DATE_TIME, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		datetimeUp = true;
	}
	else if (currentTab == 5) {
		fftDlg.Create(IDD_FFT, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		fftUp = true;
	}
	else {
		AfxMessageBox(_T("Unsupported Tab"));
	}

}

void CPreProcessor::setTab(CString preprocessor)
{
	setTab(getTabFromMode(preprocessor));
}

void CPreProcessor::setTab(int ntab)
{
	if ((currentTab == ntab) && !changed) {
		return;
	}

	if (fnparserUp) {
		fnparserDlg.DestroyWindow();
		fnparserUp = false;
	}
	if (normalizeUp) {
		normalizeDlg.DestroyWindow();
		normalizeUp = false;
	}
	if (derivativeUp) {
		derivativeDlg.DestroyWindow();
		derivativeUp = false;
	}
	if (datetimeUp) {
		datetimeDlg.DestroyWindow();
		datetimeUp = false;
	}
	if (fftUp) {
		fftDlg.DestroyWindow();
		fftUp = false;
	}
	/*if (dictionaryUp) {
		dictionaryDlg.DestroyWindow();
		dictionaryUp = false;
	}
	if (windexUp) {
		windexDlg.DestroyWindow();
		windexUp = false;
	}*/
	if (fieldkeyUp) {
		fieldkeyDlg.DestroyWindow();
		fieldkeyUp = false;
	}

	CRect rect1;
	m_tab1_control.GetWindowRect(&rect1);
	CRect rect2;
	rect2.left = 2;
	rect2.top = 22 + 4;
	rect2.right = (rect1.right - rect1.left) - 2;
	rect2.bottom = (rect1.bottom - rect1.top) - 2 + 4;

	//CString s = _T("");
	//s.Format(_T("CPreProcessor::setTab : %i : %i : %i : %i"), rect1.left, rect1.top, rect1.right, rect1.bottom);
	//addErrorString(s);

	if (ntab == 0) {
		if (imagefiles || (outputnode && (soundfiles/* || textfiles*/))) {
			fnparserDlg.Create(IDD_IMAGE_NAME_PARSER, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fnparserDlg.setSize(rect2);
			fnparserUp = true;
		}
		else if (soundfiles) {
			fftDlg.Create(IDD_FFT, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fnparserDlg.setSize(rect2);
			fftUp = true;
		}
		/*else if (textfiles) {
			windexDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			windexDlg.setSize(rect2);
			windexDlg.RedrawWindow();
			windexUp = true;
		}*/
		else {
			normalizeDlg.Create(IDD_NORMALIZE, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			normalizeDlg.setSize(rect2);
			normalizeUp = true;
		}
		currentTab = 0;
	}
	else if (ntab == 1) {
		/*if (textfiles) {
			if (outputnode) {
				windexDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
				windexDlg.setSize(rect2);
				windexDlg.RedrawWindow();
				windexUp = true;
			}
			else {
				dictionaryDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
				dictionaryDlg.setSize(rect2);
				dictionaryDlg.RedrawWindow();
				dictionaryUp = true;
			}
		}
		else {*/
			fieldkeyDlg.Create(IDD_FIELD_KEY, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fieldkeyDlg.setSize(rect2);
			fieldkeyUp = true;
		//}
		currentTab = 1;
	}
	else if (ntab == 2) {
		/*if (textfiles) {
			if (outputnode) {
				dictionaryDlg.Create(IDD_DICTIONARY_PROCESS, (CTabCtrl*)GetDlgItem(IDC_TAB1));
				dictionaryDlg.setSize(rect2);
				dictionaryDlg.RedrawWindow();
				dictionaryUp = true;
			}
		}
		else {*/
			fnparserDlg.Create(IDD_IMAGE_NAME_PARSER, (CTabCtrl*)GetDlgItem(IDC_TAB1));
			fnparserDlg.setSize(rect2);
			fnparserUp = true;
		//}
		currentTab = 2;
	}
	else if (ntab == 3) {
		derivativeDlg.Create(IDD_DERIVATIVE, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		derivativeDlg.setSize(rect2);
		derivativeUp = true;
		currentTab = 3;
	}
	else if (ntab == 4) {
		datetimeDlg.Create(IDD_DATE_TIME, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		datetimeDlg.setSize(rect2);
		datetimeUp = true;
		currentTab = 4;
	}
	else if (ntab == 5) {
		fftDlg.Create(IDD_FFT, (CTabCtrl*)GetDlgItem(IDC_TAB1));
		fftDlg.setSize(rect2);
		fftUp = true;
		currentTab = 5;
	}
	else {
		AfxMessageBox(_T("Unsupported Tab"));
	}

	m_tab1_control.SetCurSel(currentTab);

	changed = false;

}

double CPreProcessor::getLowerNormalizationFactor()
{
	if (normalizeUp) {
		return(normalizeDlg.getLowerNormalizationFactor());
	}
	else if (derivativeUp) {
		return(derivativeDlg.getLowerNormalizationFactor());
	}
	else if (datetimeUp) {
		return(datetimeDlg.getLowerNormalizationFactor());
	}
	else if (fftUp) {
		return(fftDlg.getLowerNormalizationFactor());
	}
	return(0);
}

void CPreProcessor::setLowerNormalizationFactor(double factor)
{
	normalizeDlg.setLowerNormalizationFactor(factor);
	datetimeDlg.setLowerNormalizationFactor(factor);
	derivativeDlg.setLowerNormalizationFactor(factor);
	fftDlg.setLowerNormalizationFactor(factor);
}

double CPreProcessor::getUpperNormalizationFactor()
{
	if (normalizeUp) {
		return(normalizeDlg.getUpperNormalizationFactor());
	}
	else if (derivativeUp) {
		return(derivativeDlg.getUpperNormalizationFactor());
	}
	else if (datetimeUp) {
		return(datetimeDlg.getUpperNormalizationFactor());
	}
	else if (fftUp) {
		return(fftDlg.getUpperNormalizationFactor());
	}
	return(0);
}

void CPreProcessor::setUpperNormalizationFactor(double factor)
{
	normalizeDlg.setUpperNormalizationFactor(factor);
	datetimeDlg.setUpperNormalizationFactor(factor);
	derivativeDlg.setUpperNormalizationFactor(factor);
	fftDlg.setUpperNormalizationFactor(factor);
}

double CPreProcessor::getRealLowerNormalizationFactor()
{
	if (fnparserUp) {
		return(fnparserDlg.getRealLowerNormalizationFactor());
	}
	else if (fieldkeyUp) {
		return(fieldkeyDlg.getRealLowerNormalizationFactor());
	}
	return(0);
}

void CPreProcessor::setRealLowerNormalizationFactor(double factor)
{
	fnparserDlg.setRealLowerNormalizationFactor(factor);
	fieldkeyDlg.setRealLowerNormalizationFactor(factor);
}

double CPreProcessor::getRealUpperNormalizationFactor()
{
	if (fnparserUp) {
		return(fnparserDlg.getRealUpperNormalizationFactor());
	}
	else if (fieldkeyUp) {
		return(fieldkeyDlg.getRealUpperNormalizationFactor());
	}
	return(0);
}

void CPreProcessor::setRealUpperNormalizationFactor(double factor)
{
	fnparserDlg.setRealUpperNormalizationFactor(factor);
	fieldkeyDlg.setRealUpperNormalizationFactor(factor);
}

CString CPreProcessor::getTime()
{
	if (datetimeUp) {
		return(datetimeDlg.getTime());
	}
	return(_T(""));
}

void CPreProcessor::setTime(CString time)
{
	datetimeDlg.setTime(time);
}

int CPreProcessor::getStride()
{
	if (derivativeUp) {
		return(derivativeDlg.getStride());
	}
	else if (fftUp) {
		return(fftDlg.getStride());
	}
	return(1);
}

void CPreProcessor::setStride(int stride, bool lock_one)
{
	derivativeDlg.setStride(stride);
	fftDlg.setStride(stride, lock_one);
}

int CPreProcessor::getDownSample()
{
	if (fftUp) {
		return(fftDlg.getDownSample());
	}
	return(1);
}

void CPreProcessor::setDownSample(int downsample)
{
	fftDlg.setDownSample(downsample);
}

void CPreProcessor::setNameKey(CString key)
{
	//if (fnparserUp) {
		fnparserDlg.setNameKey(key);
	//}
	//else if (fieldkeyUp) {
		fieldkeyDlg.setNameKey(key);
	//}
}

CString CPreProcessor::getNameKey()
{
	if (fnparserUp) {
		return(fnparserDlg.getNameKey());
	}
	else if (fieldkeyUp) {
		return(fieldkeyDlg.getNameKey());
	}
	return(_T(""));
}

void CPreProcessor::setOffsetIndex(int mindex)
{
	//windexDlg.setOffsetIndex(mindex);
}

int CPreProcessor::getOffsetIndex()
{
	//if (windexUp) {
		//return(windexDlg.getOffsetIndex());
	//}
	return(0);
}

void CPreProcessor::OnCancel()
{
	DestroyDlg();
}

BOOL CPreProcessor::DestroyWindow()
{
	displayed = false;
	return CDialog::DestroyWindow();
}

void CPreProcessor::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}

