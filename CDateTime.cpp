// CDateTime.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CDateTime.h"
#include "afxdialogex.h"


// CDateTime dialog

IMPLEMENT_DYNAMIC(CDateTime, CDialog)

CDateTime::CDateTime(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DATE_TIME, pParent)
{
	//m_date = COleDateTime::GetCurrentTime();
	m_selection = 3;
	m_normalize = true;
	data_updated = false;
	nmin = 0;
	nmax = 1;
	m_date = COleDateTime(1970, 1, 1, 12, 0, 0);
}

CDateTime::~CDateTime()
{
}

void CDateTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_date);
	DDX_Radio(pDX, IDC_SECONDS, m_selection);
	DDX_Check(pDX, IDC_NORMALIZE, m_normalize);
}


BEGIN_MESSAGE_MAP(CDateTime, CDialog)
	ON_BN_CLICKED(IDC_SECONDS, &CDateTime::OnBnClickedSeconds)
	ON_BN_CLICKED(IDC_MINUTES, &CDateTime::OnBnClickedMinutes)
	ON_BN_CLICKED(IDC_HOURS, &CDateTime::OnBnClickedHours)
	ON_BN_CLICKED(IDC_DAYS, &CDateTime::OnBnClickedDays)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CDateTime::OnDtnDatetimechangeDatetimepicker1)
	ON_BN_CLICKED(IDC_NORMALIZE, &CDateTime::OnBnClickedNormalize)
END_MESSAGE_MAP()


// CDateTime message handlers

void CDateTime::setSize(CRect rect)
{
	MoveWindow(rect, true);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	//if (height > ((2 * width) / 3)) {
		//height = ((2 * width) / 3);
		//rect.bottom = rect.top + height;
	//}
	CRect rect3;
	rect3.left = 170;
	rect3.top = 5;
	rect3.right = (rect.right - rect.left) - 5;
	rect3.bottom = (rect.bottom - rect.top) - 20;
	m_list_ctrl.MoveWindow(rect3.left, rect3.top, rect3.Width(), rect3.Height(), true);
}

void CDateTime::setData(vector<vector<CString>> sdata)
{
	if (!data_updated) {
		stringdata.clear();
		for (int i = 0; i < sdata.size(); i++) {
			stringdata.insert(stringdata.end(), sdata[i].begin(), sdata[i].end());
		}
	}
}

void CDateTime::OnBnClickedSeconds()
{
	UpdateData();
	setListBox();
}

void CDateTime::OnBnClickedMinutes()
{
	UpdateData();
	setListBox();
}

void CDateTime::OnBnClickedHours()
{
	UpdateData();
	setListBox();
}

void CDateTime::OnBnClickedDays()
{
	UpdateData();
	setListBox();
}

void CDateTime::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

BOOL CDateTime::OnInitDialog()
{
	CDialog::OnInitDialog();

	setListBox();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDateTime::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CDateTime::setListBox()
{
	UpdateData();

	m_list_ctrl.DeleteAllItems();
	m_list_ctrl.DeleteColumn(0);
	m_list_ctrl.DeleteColumn(0);
	m_list_ctrl.DeleteColumn(0);

	CString label = _T("Seconds");
	if (m_selection == 1) {
		label = _T("Minutes");
	}
	else if (m_selection == 2) {
		label = _T("Hours");
	}
	else if (m_selection == 3) {
		label = _T("Days");
	}
	m_list_ctrl.InsertColumn(1, _T(""), LVCFMT_CENTER, 10);
	m_list_ctrl.InsertColumn(2, _T("Date/Time"), LVCFMT_CENTER, 140);
	m_list_ctrl.InsertColumn(3, label, LVCFMT_CENTER, 140);

	bool isdate = false;
	if ((int)stringdata.size() > 2) {
		for (int i = 0; i < stringdata[1].GetLength(); i++) {
			//addErrorString(_T("* = ") + stringdata[1]);
			if ((stringdata[1][i] == '/') || (stringdata[1][i] == '-')) {
				isdate = true;
				i = stringdata[1].GetLength();
			}
		}
	}

	if (isdate) {

		CAuxFunctions aux;

		doubledata.clear();

		double max = 0;

		for (int i = 0; i < (int)stringdata.size(); i++) {

			COleDateTime mtime = aux.getTimeFromString(stringdata[i]);
			COleDateTimeSpan span = mtime - m_date;
			long delta = 0;
			if (m_selection == 0) {
				delta = 8640 * span.GetDays() + 360 * span.GetHours() + 60 * span.GetMinutes() + span.GetSeconds();
			}
			else if (m_selection == 1) {
				delta = 1440 * span.GetDays() + 60 * span.GetHours() + span.GetMinutes();
			}
			else if (m_selection == 2) {
				delta = 24 * span.GetDays() + span.GetHours();
			}
			else if (m_selection == 3) {
				delta = span.GetDays();
			}

			doubledata.push_back(delta);

			if (delta > max) {
				max = delta;
			}

		}

		if (m_normalize && (max > 0)) {
			nmax = 1 / max;
			for (int i = 0; i < (int)stringdata.size(); i++) {
				doubledata[i] = doubledata[i] * nmax;
			}
		}

		int numrows = (int)stringdata.size();
		if (numrows > 1000) {
			numrows = 1000;
		}

		if (numrows > 0) {
			//CString sbuf = _T("");
			for (int i = 0; i < numrows; i++) {
				//sbuf.Format(_T("%i"), i + 1);
				int nIndex = m_list_ctrl.InsertItem(i + 1, _T(""));
				m_list_ctrl.SetItemText(nIndex, 1, stringdata[i]);
				CString s = _T("");
				if (m_normalize) {
					s.Format(_T("%.6f"), doubledata[i]);
				}
				else {
					s.Format(_T("%.0f"), doubledata[i]);
				}
				m_list_ctrl.SetItemText(nIndex, 2, s);
			}
		}

	}
	else {

		int nIndex = m_list_ctrl.InsertItem(1, _T(""));
		m_list_ctrl.SetItemText(nIndex, 1, _T("No Date"));

		/*int size = (int)stringdata.size();
		if (size > 10) {
			size = 10;
		}
		for (int i = 0; i < size; i++) {
			addErrorString(stringdata[i]);
		}*/

		
	}
}

void CDateTime::OnDtnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	UpdateData();
	setListBox();
	*pResult = 0;
}

void CDateTime::OnBnClickedNormalize()
{
	UpdateData();
	setListBox();
}

CString CDateTime::getMode()
{
	if (m_normalize) {
		if (m_selection == 0) {
			return(_T("NormSeconds"));
		}
		else if (m_selection == 1) {
			return(_T("NormMinutes"));
		}
		else if (m_selection == 2) {
			return(_T("NormHours"));
		}
		else if (m_selection == 3) {
			return(_T("NormDays"));
		}
	}
	else {
		if (m_selection == 0) {
			return(_T("Seconds"));
		}
		else if (m_selection == 1) {
			return(_T("Minutes"));
		}
		else if (m_selection == 2) {
			return(_T("Hours"));
		}
		else if (m_selection == 3) {
			return(_T("Days"));
		}
	}
	return(_T(""));
}

void CDateTime::setMode(CString mode)
{
	if (mode == _T("NormSeconds")) {
		m_normalize = true;
		m_selection = 0;
	}
	else if (mode == _T("NormMinutes")) {
		m_normalize = true;
		m_selection = 1;
	}
	else if (mode == _T("NormHours")) {
		m_normalize = true;
		m_selection = 2;
	}
	else if (mode == _T("NormDays")) {
		m_normalize = true;
		m_selection = 3;
	}
	else if (mode == _T("Seconds")) {
		m_normalize = false;
		m_selection = 0;
	}
	else if (mode == _T("Minutes")) {
		m_normalize = false;
		m_selection = 1;
	}
	else if (mode == _T("Hours")) {
		m_normalize = false;
		m_selection = 2;
	}
	else if (mode == _T("Days")) {
		m_normalize = false;
		m_selection = 3;
	}
	else {
		m_normalize = true;
		m_selection = 0;
	}
}

double CDateTime::getLowerNormalizationFactor()
{
	return(0);
}

void CDateTime::setLowerNormalizationFactor(double factor)
{
}

double CDateTime::getUpperNormalizationFactor()
{
	return(nmax);
}

void CDateTime::setUpperNormalizationFactor(double factor)
{
	nmax = factor;
}

CString CDateTime::getTime()
{
	return(m_date.Format(_T("%D")));
}

void CDateTime::setTime(CString time)
{
	CAuxFunctions aux;
	m_date = aux.getTimeFromString(time);
}

void CDateTime::addErrorString(CString err)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	CNetDesignerView* pView = (CNetDesignerView*)pChild->GetActiveView();
	pView->addErrorString(err);
}


