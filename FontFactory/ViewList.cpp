#include "stdafx.h"
#include "ViewList.h"
#include "Global.h"

CViewList::CViewList()
{
    m_csFolder = _T("");
}


CViewList::~CViewList()
{
}


BOOL CViewList::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    BOOL bRes = CListCtrl::OnNotify(wParam, lParam, pResult);

    NMHDR* pNMHDR = (NMHDR*)lParam;
    ASSERT(pNMHDR != NULL);

    if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
    {
        GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
    }

    return bRes;
}


BEGIN_MESSAGE_MAP(CViewList, CMFCListCtrl)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CViewList::OnLvnItemchanged)
END_MESSAGE_MAP()


void CViewList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    //if (pNMLV->uChanged & LVIF_STATE)
    //{
    //    if ((pNMLV->uNewState ^ pNMLV->uOldState) & LVIS_SELECTED)
    //    {
    //        //List selection changed
    //    }
    //}

    //if (pNMLV->uNewState & LVIS_SELECTED &&  pNMLV->uNewState & LVIS_FOCUSED)
    //{
    //    CString *cs = new CString();
    //    *cs = m_csFolder + _T("\\") + GetItemText(pNMLV->iItem, 0);
    //    AfxGetMainWnd()->PostMessageW(WM_FILEVIEW_FILE_SELECTED, 0, (LPARAM)cs);
    //}

    *pResult = 0;
}
