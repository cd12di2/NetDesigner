

//#define GCC


#include "pch.h"
#include "MainFrm.h"
#include "NetDesignerDoc.h"
#include "CNetData.h"
#include "CFileOperations.h"
#include "CJSON.h"


void CJSON::addErrorString(string err)
{
#ifdef GCC
	cout << err << endl;
#else
	CWnd* pWndMain = AfxGetMainWnd();
	CNetDesignerDoc* pDoc = (CNetDesignerDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
	pDoc->addErrorString((CString)err.c_str());
#endif
}


