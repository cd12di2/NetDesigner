

//#define GCC


#include "pch.h"
#include "framework.h"
#include "NetDesignerDoc.h"
#include "NetDesignerView.h"
#include "CNetData.h"


void netDataStruct::addErrorString(string err) {
#ifdef GCC
	cout << err << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString((CString)err.c_str());
#endif
}

void NetData::addErrorString(string err) {
#ifdef GCC
	cout << err << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString(err);
#endif
}

void node::addErrorString(string err) {
#ifdef GCC
	cout << err << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString((CString)err.c_str());
#endif
}

