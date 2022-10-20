
// TempoView.h : interface of the CTempoView class
//

#pragma once

constexpr UINT IDC_BUTTON1{ 1001 };
constexpr UINT IDC_BUTTON2{ 1002 };

constexpr UINT IDC_BUTTON4{ 1003 };
constexpr UINT IDC_BUTTON5{ 1004 };

constexpr UINT IDC_BUTTON3{ 1003 };

constexpr UINT IDC_BUTTON7_timer{ 1007 };

#define IDC_NGRAPH 500
#define IDC_COMPUTE 501
#define N 20
#define R 250


class CTempoView : public CView
{
protected: // create from serialization only
	CTempoView();
	DECLARE_DYNCREATE(CTempoView)

// Attributes
public:
	CTempoDoc* GetDocument() const;

// Operations
public:
	void ZoomToWindow();


private:
	int vCliq[N + 1], p[N + 1];
	int max[N + 1];
	int maxi, maximum, nCliq;
	CPoint Home, End;
	CFont fArial, fVerdana;
	CPen pColor[N + 1];
	CButton bNGraph, bCompute;
	typedef struct
	{
		int adj;
	} EDGE;
	EDGE e[N + 1][N + 1];
	typedef struct
	{
		CPoint Home;
		CRect rct;
	} NODE;
	NODE v[N + 1];

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

// Implementation
public:
	virtual ~CTempoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CBrush m_brush;
	double m_dZoomFactor;

	CStatic	c_Status;
	CSliderCtrl	c_Volume;
	CButton	c_Stop;
	CButton	c_Pause;
	CButton	c_Play;
	CSliderCtrl	c_Slider;
	CStatic	c_Static;
	int		m_PlayPauseStop;
	int		m_Pos;

private:
	CButton m_Btn_vpered;
	CButton m_Btn_Nazad;

	CButton m_Btn_vpered_jump;
	CButton m_Btn_Nazad_jump;

	CButton m_Btn_StartPlay;

	CEdit m_Edit_info_num_cadr;
	bool m_redBlack{false};

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	void OnBtn1Clicked_Nazad();
	void OnBtn1Clicked_Vpered();

	void OnBtn1Clicked_Nazad_Jump();
	void OnBtn1Clicked_Vpered_Jump();

	void OnBtn1Clicked_Play();


// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnSetfps(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetfps_start(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetfps_stop(WPARAM wParam, LPARAM lParam);
	int timestump = 100;
	
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TempoView.cpp
inline CTempoDoc* CTempoView::GetDocument() const
   { return reinterpret_cast<CTempoDoc*>(m_pDocument); }
#endif

