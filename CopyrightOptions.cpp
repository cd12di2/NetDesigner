// CopyrightOptions.cpp : implementation file
//

#include "pch.h"
#include "NetDesigner.h"
#include "NetDesignerDoc.h"
#include "CopyrightOptions.h"
#include "afxdialogex.h"


// CopyrightOptions dialog

IMPLEMENT_DYNAMIC(CopyrightOptions, CDialog)

CopyrightOptions::CopyrightOptions(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_COPYRIGHT_OPTIONS, pParent)
{

}

CopyrightOptions::~CopyrightOptions()
{
}

void CopyrightOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list2_ctrl);
}


BEGIN_MESSAGE_MAP(CopyrightOptions, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CopyrightOptions message handlers


BOOL CopyrightOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	CNetDesignerDoc* pMyDoc = (CNetDesignerDoc*)((CFrameWnd*)AfxGetApp()->GetMainWnd())->GetActiveDocument();

	mbrush.CreateSolidBrush(GetGlobalData()->clrBarFace);
	mbrush2.CreateSolidBrush(GetGlobalData()->clrActiveCaption);

	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(" "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(" "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("==========================================="));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("NNetDesigner "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("Version ") + pMyDoc->getVersion() + _T(" "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("Copyright \xa9 2022 GradientN "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("All rights reserved. "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(" "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(" "));
	/*m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("License for the IconExperience Toolbox v9.1"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("==========================================="));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("Copyright (c) 2008-2014 INCORS GmbH. All rights reserved."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("This software may only be used by licensees of one or more IconExperience icon products. "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("It may be used for any commercial and non-commercial purpose, exept as follows:"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("This software may not be used to create commercial icon collections or other products, "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("that compete with any of INCORS GmbH's IconExperience products."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("You may not reverse-engineer, decompile, or disassemble this software, except as expressly "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("permitted by the law in effect in the jurisdiction in which you are located."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, "));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("If any part of this license is found to be unenforceable in any jurisdiction, the remaining portions"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("of the license remain in force."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));*/
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("==========================================="));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("Solarized Color Scheme"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("Copyright (c) 2011 Ethan Schoonover"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("MIT License"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("Permission is hereby granted, free of charge, to any person obtaining a copy"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("of this software and associated documentation files (the \"Software\"), to deal"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("in the Software without restriction, including without limitation the rights"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("copies of the Software, and to permit persons to whom the Software is"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("furnished to do so, subject to the following conditions:"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("The above copyright notice and this permission notice shall be included in all"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("copies or substantial portions of the Software."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("SOFTWARE."));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("==========================================="));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("Icons used in NNetDesigner are copyrighted by INCORS"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T("and their license purchased from IconExperience"));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));
	m_list2_ctrl.InsertString(m_list2_ctrl.GetCount() - 1, _T(""));


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CopyrightOptions::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	HBRUSH hbr = mbrush;

	if (pWnd->GetDlgCtrlID() == IDC_TAB1) {
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	else {

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

	}

	return hbr;
}

void CopyrightOptions::setSize(CRect rect)
{
	MoveWindow(rect, true);
}

void CopyrightOptions::OnOK()
{
	//CDialog::OnOK();
}

void CopyrightOptions::OnCancel()
{
	//CDialog::OnCancel();
}
