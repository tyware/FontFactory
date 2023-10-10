#pragma once
#include "afxcmn.h"
#include "afxvslistbox.h"

#include "FeApi.h"
#include "afxwin.h"
#include "resource.h"

// CTTDebug dialog

class CTTDebug : public CDialogEx
{
	DECLARE_DYNAMIC(CTTDebug)

public:
	CTTDebug(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTTDebug();

    void SetupDebug(int set);
    void UpdatePoints();
    void UpdateGraphicStates();
    void UpdateStack();
    void UpdateStorage();
    void UpdateCvt();
    void UpdateInstructions();
    void UpdateSubglyphs();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRUETYPE_DEBUG };

public:
    FE_Font m_hFont;
    FE_FontFace m_hFontFace;
    FE_Glyph m_hGlyph;
    int m_nGlyphId;
    FE_DebugInfo m_hDebugInfo;
    int m_nStatus;

private:
    bool m_bInitialized;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void AdjustLayout();

	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_listStorage;
    CListCtrl m_listGraphicState;
    CListCtrl m_listGlyphPoints;
    CListCtrl m_listStack;
    CListCtrl m_listInstruction;
    CListCtrl m_listCvt;
    CListCtrl m_listSubGlyphs;
    CStatic m_staticGid;
    CButton m_chkTwilight;
    CButton m_radioCurrent;
    CButton m_radioOriginal;
    CButton m_radioGrid;
    CButton m_radioEmUnit;
    CButton m_radioFont;
    CButton m_radioFace;
    CButton m_radioGlyph;
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedButtonStepOver();
    afx_msg void OnBnClickedButtonStepIn();
    afx_msg void OnBnClickedButtonStepOut();
    afx_msg void OnClose();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedRadioTtdebugCurrent();
    afx_msg void OnBnClickedRadioTtdebugOriginal();
    afx_msg void OnBnClickedRadioTtdebugGrid();
    afx_msg void OnBnClickedRadioTtdebugEmUnit();
    afx_msg void OnBnClickedCheckTtdebugTwilight();
    afx_msg void OnBnClickedRadioDebugFont();
    afx_msg void OnBnClickedRadioDebugFontFace();
    afx_msg void OnBnClickedRadioDebugGlyph();
    afx_msg void OnBnClickedButtonContinue();
    afx_msg void OnDblclkListTtDebugGlyphInstruction(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonDebugRestart();
};
