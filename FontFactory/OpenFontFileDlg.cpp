// OpenFontFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseFont.h"
#include "OpenFontFileDlg.h"
#include "afxdialogex.h"
//#include "global.h"
//#include "Model.h"
#include "ApiHeader.h"

// COpenFontFileDlg dialog

IMPLEMENT_DYNAMIC(COpenFontFileDlg, CDialogEx)

COpenFontFileDlg::COpenFontFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COpenFontFileDlg::IDD, pParent)
   , m_nFontType(0)
{
   m_csPathName = "";
   m_nFontType = 0;
   m_nFontSize = 24;
   m_nDPI = 96; // Windows default DPI is 96
   m_nCharNum = -1;
   m_bDebug = false;
}

COpenFontFileDlg::COpenFontFileDlg(int fontSize, int nDPI, bool bDrawOutline, int nCharNum, CWnd* pParent)
	: CDialogEx(COpenFontFileDlg::IDD, pParent)
{
   m_csPathName = "";
   m_nFontType = 0;
   m_nFontSize = fontSize;
   m_nDPI = nDPI;
   m_bDrawOutlineOnly = bDrawOutline;
   m_nCharNum = nCharNum;
   m_bDebug = true;
}

COpenFontFileDlg::~COpenFontFileDlg()
{
}

void COpenFontFileDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCEDITBROWSE_FONTFILE, m_editBrowseFontFile);
    DDX_Control(pDX, IDC_COMBOBOXEX1, m_cbFontType);
    DDX_Control(pDX, IDC_EDIT_OPENFILE_FONT_SIZE, m_editFontSize);
    DDX_Control(pDX, IDC_CHECK_DRAW_OUTLINE_ONLY, m_cbDrawOutlineOnly);
    DDX_Control(pDX, IDC_EDIT_OPENFILE_DPI, m_editDPI);
    DDX_Control(pDX, IDC_EDIT_OPENFILE_CHAR_NUM, m_editCharNum);
    DDX_Control(pDX, IDC_CHECK_OPEN_FONT_DEBUG, m_chkDebug);
}


BEGIN_MESSAGE_MAP(COpenFontFileDlg, CDialogEx)
   ON_BN_CLICKED(IDOK, &COpenFontFileDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COpenFontFileDlg message handlers


BOOL COpenFontFileDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   // TODO:  Add extra initialization here
   m_editBrowseFontFile.EnableFileBrowseButton();
   CString firstFile = AfxGetApp()->GetProfileStringW(L"Recent File List", L"File1");
   m_editBrowseFontFile.SetWindowTextW(firstFile);

   m_cbFontType.InsertString(FT_TRUETYPE, L"TrueType");

   m_cbFontType.InsertString(FT_TYPE_1_COMPACT, L"Type1 Compact");

   m_cbFontType.InsertString(FT_TYPE_1, L"Type 1");

   m_cbFontType.InsertString(FT_TYPE_2, L"Type");

   m_cbFontType.SetCurSel(0);

   CString cs;
   cs.Format(_T("%d"), m_nFontSize);
   m_editFontSize.SetWindowTextW(cs);

   cs.Format(_T("%d"), m_nDPI);
   m_editDPI.SetWindowTextW(cs);

   cs.Format(_T("%d"), m_nCharNum);
   m_editCharNum.SetWindowTextW(cs);

   m_cbDrawOutlineOnly.SetCheck(m_bDrawOutlineOnly ? BST_CHECKED : BST_UNCHECKED);
   m_chkDebug.SetCheck(m_bDebug ? BST_CHECKED : BST_UNCHECKED);

   m_nDPI = GetDC()->GetDeviceCaps(LOGPIXELSX);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void COpenFontFileDlg::OnBnClickedOk()
{
   // TODO: Add your control notification handler code here
   m_editBrowseFontFile.GetWindowTextW(m_csPathName);
   m_nFontType = m_cbFontType.GetCurSel();
   m_bDrawOutlineOnly = (m_cbDrawOutlineOnly.GetCheck() == BST_CHECKED);
   m_bDebug = (m_chkDebug.GetCheck() == BST_CHECKED);

   CString cs;
   m_editFontSize.GetWindowTextW(cs);
   m_nFontSize = _wtoi(cs.GetBuffer());
   m_editDPI.GetWindowTextW(cs);
   m_nDPI = _wtoi(cs.GetBuffer());
   m_editCharNum.GetWindowTextW(cs);
   m_nCharNum = _wtoi(cs.GetBuffer());

   CDialogEx::OnOK();
}

