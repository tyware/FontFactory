#pragma once
#include "afxcmn.h"

class CViewList : public CMFCListCtrl
{
public:
    CViewList();
    virtual ~CViewList();

public:
    CString m_csFolder;

    // Overrides
protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};

