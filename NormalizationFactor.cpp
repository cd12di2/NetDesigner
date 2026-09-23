// NormalizationFactor.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NormalizationFactor.h"
#include "afxdialogex.h"


// NormalizationFactor dialog

IMPLEMENT_DYNAMIC(NormalizationFactor, CDialog)

NormalizationFactor::NormalizationFactor(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_NORM_SCALE, pParent)
{
	m_lower = _T("0.20");
	m_upper = _T("0.80");
}

NormalizationFactor::~NormalizationFactor()
{
}

void NormalizationFactor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LOWER, m_lower);
	DDX_Text(pDX, IDC_UPPER, m_upper);
}


BEGIN_MESSAGE_MAP(NormalizationFactor, CDialog)
END_MESSAGE_MAP()


// NormalizationFactor message handlers
