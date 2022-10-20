
// TempoView.cpp : implementation of the CTempoView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Tempo.h"
#endif



#include "TempoDoc.h"
#include "TempoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTempoView

IMPLEMENT_DYNCREATE(CTempoView, CView)

BEGIN_MESSAGE_MAP(CTempoView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTempoView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_CREATE(&CTempoView::OnCreate)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WMU_SETFPS, OnSetfps)
	//ON_MESSAGE(WMU_SETFPS_START, OnSetfps)
	ON_MESSAGE(WMU_SETFPS_STOP, OnSetfps_stop)
	
	ON_BN_CLICKED(IDC_BUTTON1, OnBtn1Clicked_Vpered)		
	ON_BN_CLICKED(IDC_BUTTON2, OnBtn1Clicked_Nazad)

	ON_BN_CLICKED(IDC_BUTTON4, OnBtn1Clicked_Vpered_Jump)
	ON_BN_CLICKED(IDC_BUTTON5, OnBtn1Clicked_Nazad_Jump)	

	ON_BN_CLICKED(IDC_BUTTON7_timer, OnBtn1Clicked_Play)

	ON_WM_SIZE()
END_MESSAGE_MAP()

// CTempoView construction/destruction

CTempoView::CTempoView()
	:m_dZoomFactor(1.0)
{

	Home = CPoint(1230, 100); End = CPoint(1800, 700);
	//Create(NULL, "Computing the Multi-cliques of a Graph",
		//WS_OVERLAPPEDWINDOW, CRect(0, 0, 1000, 800), NULL);
	bNGraph.Create("New Graph", WS_CHILD | WS_VISIBLE |
		BS_DEFPUSHBUTTON,
		CRect(CPoint(Home.x, 30), CSize(180, 40)), this, IDC_NGRAPH);
	bCompute.Create("Compute", WS_CHILD | WS_VISIBLE
		| BS_DEFPUSHBUTTON, CRect(CPoint(Home.x + 200, 30),
			CSize(180, 40)),
		this, IDC_COMPUTE);
	fArial.CreatePointFont(80, "Arial");
	fVerdana.CreatePointFont(100, "Verdana");
	int Color[] = { RGB(150,150,150),RGB(0,0,0),RGB(0,0,200),
	 RGB(0,200,0),RGB(200,0,0),RGB(0,200,200),RGB(255,0,50) };
	for (int i = 0; i <= 6; i++)
	{
		if (i == 0)
			pColor[i].CreatePen(PS_SOLID, 1, Color[i]);
		else
			pColor[i].CreatePen(PS_SOLID, 3, Color[i]);
	}

	// init

	int i, j;
	double distance;
	srand(time(0));
	for (i = 1; i <= N; i++)
	{
		v[i].Home.x = Home.x + 20 + rand() % (End.x - Home.x - 50);
		v[i].Home.y = Home.y + 20 + rand() % (End.y - Home.y - 50);
		v[i].rct = CRect(CPoint(v[i].Home.x - 10, v[i].Home.y - 10), CSize
		(25, 25));
	}
	//Computing the Maximum Clique 191
		for (i = 1; i <= N; i++)
		{
			e[i][i].adj = 0;
			for (j = i + 1; j <= N; j++)
			{
				distance = sqrt(pow(double(v[i].Home.x - v[j].Home.x), 2)
					+ pow(double(v[i].Home.y - v[j].Home.y), 2));
				e[i][j].adj = ((distance <= R) ? 1 : 0);
				e[j][i].adj = e[i][j].adj;
			}
		}

	// TODO: add construction code here
}

CTempoView::~CTempoView()
{
	if(NULL != m_brush.GetSafeHandle())
		m_brush.DeleteObject();
}

int CTempoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	

	//m_Btn_Nazad.Create(_T(" -- < ---"), BS_PUSHBUTTON,
	//	CRect(50, 50, 280, 100), this, IDC_BUTTON2);
	////m_Btn_Nazad.ShowWindow(SW_SHOW);

	//m_Btn_vpered.Create(_T(" -- > ---"), BS_PUSHBUTTON,
	//	CRect(50, 100, 280, 150), this, IDC_BUTTON1);
	////m_Btn_vpered.ShowWindow(SW_SHOW);

	m_Edit_info_num_cadr.Create(BS_PUSHBUTTON,
		CRect(50, 170, 280, 220), this, IDC_BUTTON3);	
	//m_Edit_info_num_cadr.ShowWindow(SW_SHOW);
	m_Edit_info_num_cadr.SetWindowTextA(_T("cadr"));


	m_Btn_Nazad_jump.Create(_T(" -- <<< ---"), BS_PUSHBUTTON,
		CRect(50, 240, 280, 290), this, IDC_BUTTON5);
	m_Btn_Nazad_jump.ShowWindow(SW_SHOW);

	m_Btn_vpered_jump.Create(_T(" -- >>> ---"), BS_PUSHBUTTON,
		CRect(50, 290, 280, 340), this, IDC_BUTTON4);
	m_Btn_vpered_jump.ShowWindow(SW_SHOW);

	/*m_Btn_StartPlay.Create(_T(" <<< Play >>> "), BS_PUSHBUTTON,
		CRect(50, 350, 280, 390), this, IDC_BUTTON7_timer);
	m_Btn_StartPlay.ShowWindow(SW_SHOW);*/

	return 0;

	return 0;

}

void CTempoView::OnBtn1Clicked_Vpered_Jump()
{
	CTempoDoc* pDoc = GetDocument();
	pDoc->ShowNextFrameVpered_Jump();
	pDoc->UpdateAllViews(NULL);

	m_Edit_info_num_cadr.SetWindowTextA(std::to_string(pDoc->numCadrs[pDoc->numcalccadrCount]).c_str());
}

void CTempoView::OnBtn1Clicked_Nazad_Jump()
{
	CTempoDoc* pDoc = GetDocument();
	pDoc->ShowNextFrameNazad_Jump();
	pDoc->UpdateAllViews(NULL);

	m_Edit_info_num_cadr.SetWindowTextA(std::to_string(pDoc->numCadrs[pDoc->numcalccadrCount]).c_str());
}

void CTempoView::OnBtn1Clicked_Play()
{
	SetTimer(ID_TIMER_RENDER, static_cast<UINT>(timestump), NULL);
}

void CTempoView::OnBtn1Clicked_Vpered()
{
	CTempoDoc* pDoc = GetDocument();
	pDoc->ShowNextFrameVpered();
	pDoc->UpdateAllViews(NULL);

	m_Edit_info_num_cadr.SetWindowTextA(std::to_string(pDoc->cadrCount).c_str());

	//m_redBlack = !m_redBlack;
	//AfxMessageBox("Hi");
}


void CTempoView::OnBtn1Clicked_Nazad()
{
	CTempoDoc* pDoc = GetDocument();
	pDoc->ShowNextFrameNazad();
	pDoc->UpdateAllViews(NULL);

	m_Edit_info_num_cadr.SetWindowTextA(std::to_string(pDoc->cadrCount).c_str());

}



BOOL CTempoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// Create a push button.
	//c_Play.Create(_T("My button"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
//		CRect(10, 10, 100, 30), this, 1);

	return CView::PreCreateWindow(cs);
}

// CTempoView drawing

void CTempoView::OnDraw(CDC* pDC)
{
	
	//return;
	CTempoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(NULL == pDoc || NULL == pDoc->m_pBmi)
		return;



	// TODO: add draw code for native data here

	CRect rectClient;
	GetClientRect(&rectClient);

	//if (m_redBlack)pDC->FillSolidRect(rectClient, RGB(255,0,0));
	//pDC->FillSolidRect(rectClient, RGB(255, 124, 0));

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(pDC, rectClient.Width(), rectClient.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&bmpMem);

	if(NULL == m_brush.GetSafeHandle())
		m_brush.CreateHatchBrush(HS_DIAGCROSS, RGB(255, 220, 255));
	CBrush* pOldBrush = memDC.SelectObject(&m_brush);
	memDC.PatBlt(0, 0, rectClient.right, rectClient.bottom, PATCOPY);

	// *******Init graph **********************

	int i, j;
	double distance;
	srand(time(0));
	for (i = 1; i <= N; i++)
	{
		v[i].Home.x = Home.x + 20 + rand() % (End.x - Home.x - 50);
		v[i].Home.y = Home.y + 20 + rand() % (End.y - Home.y - 50);
		v[i].rct = CRect(CPoint(v[i].Home.x - 10, v[i].Home.y - 10), CSize
		(25, 25));
	}
	//Computing the Maximum Clique 191
	for (i = 1; i <= N; i++)
	{
		e[i][i].adj = 0;
		for (j = i + 1; j <= N; j++)
		{
			distance = sqrt(pow(double(v[i].Home.x - v[j].Home.x), 2)
				+ pow(double(v[i].Home.y - v[j].Home.y), 2));
			e[i][j].adj = ((distance <= R) ? 1 : 0);
			e[j][i].adj = e[i][j].adj;
		}
	}




	//************************************
	CString s;
	//int i, j;
	memDC.SelectObject(pColor[0]);
	for (i = 1; i <= N; i++)
		for (j = 1; j <= N; j++)
			if (e[i][j].adj)
			{
				memDC.MoveTo(CPoint(v[i].Home));
				memDC.LineTo(CPoint(v[j].Home));
			}
	memDC.SelectObject(fArial); memDC.SelectObject(pColor[1]);
	for (i = 1; i <= N; i++)
	{
		memDC.Rectangle(v[i].rct);
		s.Format("%d", i);
		memDC.TextOutA(v[i].Home.x - 5, v[i].Home.y - 5, s);
	}

	//**************************

	const int cx = (int)(rectClient.right/3);				// view client area width;
	const int cy = rectClient.bottom;				// view client area height;
	const int bx = (int)(pDoc->m_Mat.cols);				// source bitmap width;
	const int by = pDoc->m_Mat.rows;				// source bitmap height;
	const int vx = (int)((double)bx * m_dZoomFactor);// virtual document width;
	const int vy = (int)((double)by * m_dZoomFactor);// virtual document height;

	// source and destination coordinates and sizes
	int xSrc, ySrc, nSrcWidth, nSrcHeight, 
		xDst, yDst, nDstWidth, nDstHeight;

	if(vx > cx)
	{
		xSrc = (int)m_dZoomFactor;
		nSrcWidth = bx - xSrc;
		xDst = 0;
		nDstWidth = vx;
	}
	else
	{
		xSrc = 0;
		nSrcWidth = bx;
		xDst = cx / 2 - vx / 2;
		nDstWidth = vx;
	}

	if(vy > cy)
	{
		ySrc = (int)m_dZoomFactor;
		nSrcHeight = by - ySrc;
		yDst = 0;
		nDstHeight = vy;
	}
	else
	{
		ySrc = 0;
		nSrcHeight = by;
		yDst = cy / 2 - vy / 2;
		nDstHeight = vy;
	}

	memDC.SetStretchBltMode(COLORONCOLOR);
	
	StretchDIBits(memDC.m_hDC, xDst, yDst, nDstWidth, nDstHeight, 
		xSrc, ySrc, nSrcWidth, nSrcHeight, pDoc->m_Mat.data, pDoc->m_pBmi, DIB_RGB_COLORS, SRCCOPY);

	pDC->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &memDC, 0, 0, SRCCOPY); 

	memDC.SelectObject(pOldBitmap);
	bmpMem.DeleteObject();

	m_Edit_info_num_cadr.SetWindowTextA(std::to_string(pDoc->numCadrs[pDoc->numcalccadrCount]).c_str());
}

// CTempoView printing


void CTempoView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CTempoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTempoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTempoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CTempoView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTempoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CTempoView diagnostics

#ifdef _DEBUG
void CTempoView::AssertValid() const
{
	CView::AssertValid();
}

void CTempoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTempoDoc* CTempoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTempoDoc)));
	return (CTempoDoc*)m_pDocument;
}
#endif //_DEBUG

// CTempoView message handlers

void CTempoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	ZoomToWindow();
}

void CTempoView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnTimer(nIDEvent);

	if(ID_TIMER_RENDER == nIDEvent)
	{
		CTempoDoc* pDoc = GetDocument();		
		pDoc->ShowNextFrame();
		pDoc->UpdateAllViews(NULL);
		m_Edit_info_num_cadr.SetWindowTextA(std::to_string(pDoc->cadrCount).c_str());
	}
}


BOOL CTempoView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	CTempoDoc* pDoc = GetDocument();
	if(pDoc->m_bErase)
	{
		pDoc->m_bErase = FALSE;
		return CView::OnEraseBkgnd(pDC);
	}

	return TRUE;
}

LRESULT CTempoView::OnSetfps(WPARAM wParam, LPARAM lParam)
{

	//return 1;

	if (static_cast<int>(wParam) <= 0)
	{
		KillTimer(ID_TIMER_RENDER);
	}
	else
	{
		SetTimer(ID_TIMER_RENDER, static_cast<UINT>(timestump), NULL);
	}

	return 1;
}

LRESULT CTempoView::OnSetfps_start(WPARAM wParam, LPARAM lParam)
{
		SetTimer(ID_TIMER_RENDER, static_cast<UINT>(timestump), NULL);
     	return 1;
}

LRESULT CTempoView::OnSetfps_stop(WPARAM wParam, LPARAM lParam)
{
    KillTimer(ID_TIMER_RENDER);
	return 1;
}




void CTempoView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	pDC->SetMapMode(MM_TEXT);

	CView::OnPrepareDC(pDC, pInfo);
}

void CTempoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	CTempoDoc* pDoc = GetDocument();
	pDoc->m_bErase = TRUE;
	ZoomToWindow();
}

void CTempoView::ZoomToWindow()
{
	CRect rectClient;
	GetClientRect(rectClient);

	CTempoDoc* pDoc = GetDocument();
	CRect rectSource(CPoint(0, 0), CSize(pDoc->m_Mat.cols, pDoc->m_Mat.rows));

	int nX = rectSource.Width() * rectClient.bottom;
	int nY = rectSource.Height() * rectClient.right;

	if(nX < nY)
		m_dZoomFactor = (double)rectClient.bottom / (double)rectSource.Height();
	else
		m_dZoomFactor = (double)rectClient.right / (double)rectSource.Width();
}
