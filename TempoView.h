
// TempoView.h : interface of the CTempoView class
//

#pragma once


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

