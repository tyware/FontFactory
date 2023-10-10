#pragma once
#include "resource.h"
#include "afxwin.h"

// COpenFontFileDlg dialog

class COpenFontFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COpenFontFileDlg)

public:
	COpenFontFileDlg(CWnd* pParent = NULL);   // standard constructor
	COpenFontFileDlg(int fontSize, int nDPI = 72, bool bDrawOutline = false, int nCharNum = -1, CWnd* pParent = NULL);   // standard constructor
	virtual ~COpenFontFileDlg();

   CString m_csPathName;

// Dialog Data
	enum { IDD = IDD_DLG_OPEN_FONT_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   CMFCEditBrowseCtrl m_editBrowseFontFile;
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedOk();
   int m_nFontType;
   CComboBox m_cbFontType;
   CEdit m_editFontSize;
   int m_nFontSize;
   CButton m_cbDrawOutlineOnly;
   BOOL m_bDrawOutlineOnly;
   CEdit m_editDPI;
   int m_nDPI;
   int m_nCharNum;
   CEdit m_editCharNum;
   bool m_bDebug;
   CButton m_chkDebug;
};
