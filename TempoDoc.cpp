
// TempoDoc.cpp : implementation of the CTempoDoc class
//

#include "stdafx.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Tempo.h"
#endif

#include "TempoDoc.h"
#include "RecognitionVideo.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTempoDoc

IMPLEMENT_DYNCREATE(CTempoDoc, CDocument)

BEGIN_MESSAGE_MAP(CTempoDoc, CDocument)
END_MESSAGE_MAP()

// CTempoDoc construction/destruction

CTempoDoc::CTempoDoc()
	:m_bErase(FALSE)
	,m_pBmi(NULL)
{
	// TODO: add one-time construction code here

}

CTempoDoc::~CTempoDoc()
{
	m_VideoDav->~DAV();
	if(NULL != m_pBmi)
		delete m_pBmi;
}

BOOL CTempoDoc::OnNewDocument()
{
	if (! CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CTempoDoc serialization

void CTempoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CTempoDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CTempoDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CTempoDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CTempoDoc diagnostics

void CTempoDoc::ResizeMat(cv::Mat& mat)
{
	int nPadding = 0;
	if (CV_8UC4 != mat.type()) // padding is not needed for 32bit images
	{
		nPadding = 4 - (mat.cols % 4);
		if(4 == nPadding)
			nPadding = 0;
	}

	cv::Mat matTemp;
	if(nPadding > 0 || ! mat.isContinuous())
	{
		// Adding needed columns on the right (max 3 px)
		cv::copyMakeBorder(mat, matTemp, 0, 0, 0, nPadding, cv::BORDER_REPLICATE, 0);
		mat = matTemp;
	}
}

/**
 * Helper function to display text in the center of a contour
 */



void CTempoDoc::SetupBitmapInfo(cv::Mat& mat)
{
	if(NULL != m_pBmi)
	{
		delete m_pBmi;
		m_pBmi = NULL;
	}
	m_pBmi = new BITMAPINFO;
	BITMAPINFOHEADER* pHeader = &m_pBmi->bmiHeader;
	pHeader->biSize				= sizeof(BITMAPINFOHEADER);
	pHeader->biPlanes			= 1;
	pHeader->biCompression		= BI_RGB;
	pHeader->biXPelsPerMeter	= 100;
	pHeader->biYPelsPerMeter	= 100;
	pHeader->biClrUsed			= 0;
	pHeader->biClrImportant		= 0;
	pHeader->biWidth			= m_Mat.cols;
	pHeader->biHeight			= -m_Mat.rows;
	pHeader->biBitCount			= 24;
	m_pBmi->bmiHeader.biSizeImage = 0;

	//Bound(m_Mat);

	//debugSquares(std::vector<std::vector<cv::Point>>
	//{
	//	{cv::Point(584, 338), cv::Point(634, 378)}, // test_1.json
	//	{ cv::Point(622,438), cv::Point(644,394) }, // Test_2.json
	//	{ cv::Point(339,4), cv::Point(651,149) }, // Test_21.json
	//	{ cv::Point(636,357), cv::Point(688,302) }, // Test_3.json
	//	{ cv::Point(684,339), cv::Point(753,266) }, // Test_4.json
	//	{ cv::Point(664,339), cv::Point(731,269) }, // Test_46.json
	//	{ cv::Point(736,210), cv::Point(826,305) }, // Test_47.json
	//	{ cv::Point(777,195), cv::Point(876,302) }, // Test_5.json
	//	{ cv::Point(1004,148), cv::Point(1024,31) }, // Test_6.json
	//	//{ cv::Point(), cv::Point() }, // Test_.json
	//	//{ cv::Point(), cv::Point() }, // Test_.json

	//	{ cv::Point(777, 195), cv::Point(876, 302) }, // Test_48.json

	//}, mat);

	
}

#ifdef _DEBUG
void CTempoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTempoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CTempoDoc commands

template <typename SubstrType>
void test_substring(const std::string& str, SubstrType subs)
{
	constexpr char delim = std::is_scalar_v<SubstrType> ? '\'' : '\"';
	/*std::cout << std::quoted(str)
		<< (str.contains(subs) ? " contains "
			: " does not contain ")
		<< std::quoted(std::string{ subs }, delim) << '\n';*/
}

BOOL CTempoDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	this->lpszPathName = lpszPathName;

	if (! CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO: Add your specialized creation code here

	CString sTemp;
	sTemp.LoadString(AFX_IDS_IDLEMESSAGE);
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SETMESSAGESTRING, 0, (LPARAM)(LPCTSTR)sTemp);

	m_bErase = TRUE;
	CView* pView = NULL;
	POSITION pos = GetFirstViewPosition();
	while(NULL != pos)
	{
		pView = GetNextView(pos);
		break;
	}

	if ((std::string(lpszPathName).find(".dav") != std::string::npos))
	{
		m_VideoDav = new DAV(std::string(lpszPathName));
		
		numCadrs = {1,2, 3,4, 5,6, 21,46,47,48};

		naborCadrs[1] = { 
			{ cv::Point(584, 338), cv::Point(634, 378) },			
			//{ cv::Point(), cv::Point() },
		};
		naborCadrs[2] = { 
			{ cv::Point(622,438), cv::Point(644,394) },			
			//{ cv::Point(), cv::Point() },
		}; // Test_2.json		
		naborCadrs[3] = { 
			{ cv::Point(636,357), cv::Point(688,302) },
		    { cv::Point(636, 357), cv::Point(688, 302) },
			{ cv::Point(638, 388), cv::Point(692, 358) },
		}; // Test_3.json
		naborCadrs[4] = { 
			{ cv::Point(684,339), cv::Point(753,266) },
			{ cv::Point(687, 379), cv::Point(758, 340) },
			{ cv::Point(708, 458), cv::Point(746, 381) },
		}; // Test_4.json
		naborCadrs[5] = {
			{ cv::Point(777,195), cv::Point(876,302)},
		    { cv::Point(823, 369), cv::Point(879, 486) },		    
		}; // Test_5.json
		naborCadrs[6] = {
			{ cv::Point(1004,148), cv::Point(1024,31) },			
		}; // Test_6.jsonnaborCadrs[46] = { { cv::Point(664,339), cv::Point(731,269) } }; // Test_46.json
		naborCadrs[21] = { 
			{ cv::Point(339,4), cv::Point(651,149) },			
		}; // Test_21.json
		naborCadrs[46] = {
			{ cv::Point(664,339), cv::Point(731, 269) },
			{ cv::Point(666,376), cv::Point(733,340) },
			{ cv::Point(686, 447), cv::Point(723, 377) },
		}; // Test_47.json		
		naborCadrs[47] = { 
			{ cv::Point(736,210), cv::Point(826,305) }, 
			{ cv::Point(774,361), cv::Point(827,464)},
			{cv::Point(743,	305), cv::Point(836, 357)} 
		}; // Test_47.json		
		naborCadrs[48] = { 
			{ cv::Point(886,78), cv::Point(1022, 226) },
			{ cv::Point(985, 321), cv::Point(1023, 511) },
			{ cv::Point(922, 222), cv::Point(1022, 310) }
		}; // Test_48.json

		//Calccadr;

		m_Mat = m_VideoDav->cadr(cadrCount);
		SetupBitmapInfo(m_Mat);
		numcalccadrCount = 0;
		debugSquares({ naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat);
		Bound1(numCadrs[numcalccadrCount], { naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat); // рисуем оболочку знака
	    //::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 1000 / (int)10, 0);		
	}
	
	{
		if (!m_Video.open(lpszPathName) || NULL == pView)
		{
			if ((std::string(lpszPathName).find(".dav") != std::string::npos)) return TRUE;
			return FALSE;
		}

		if (m_Video.get(CV_CAP_PROP_FRAME_COUNT) < 2)    // is image, not video
		{
			::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 0, 0);
			m_Mat.release();
			m_Mat = cv::imread(lpszPathName);
			ResizeMat(m_Mat);
			SetupBitmapInfo(m_Mat);
			return TRUE;
		}

		double dFPS = 100 + 0 * m_Video.get(CV_CAP_PROP_FPS);
		m_Video.read(m_Mat);
		SetupBitmapInfo(m_Mat);

		if (!(std::string(lpszPathName).find(".jpg") != std::string::npos ||
			std::string(lpszPathName).find(".png") != std::string::npos ||
			std::string(lpszPathName).find(".jpag") != std::string::npos ||
			std::string(lpszPathName).find(".bmp") != std::string::npos))
		{
			::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 1000 / (int)dFPS, 0);
		}
	}

	return TRUE;

}



// --- < ---
void CTempoDoc::ShowNextFrameNazad()
{

	//if ((std::string(lpszPathName).find(".dav") != std::string::npos))
	{
		//return;
		//if (cadrCount > 1)
		{
			m_Mat = m_VideoDav->cadr(cadrCount);		
			int nn = (cadrCount % (numCadrs.size() - 1));
			debugSquares({ naborCadrs[numCadrs[nn]] }, m_Mat);
			
			Bound1(numcalccadrCount, { naborCadrs[numCadrs[nn]] }, m_Mat); // рисуем оболочку знака
			/**/

			

			if (cadrCount > 1) cadrCount--;

			/*debugSquares(std::vector<std::vector<cv::Point>>
			{
				{cv::Point(584, 338), cv::Point(634, 378)},
				{cv::Point(777, 195), cv::Point(876, 302)}
			}, m_Mat);*/

			/*find_squares(m_Mat, std::vector<std::vector<cv::Point>>
				{
					{cv::Point(584, 338), cv::Point(634, 378)},
				});*/

				//debugSquares([[cv::Point(10, 10), cv::Point(10, 10)]], m_Mat);
				// cadrCount = 1;
				//SetupBitmapInfo(m_Mat);

				//CView* pView = NULL;
				//POSITION pos = GetFirstViewPosition();
				//::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS_STOP, 0, 0);
		}
		
		return;		
	}	
}

// --- >>> ---
void CTempoDoc::ShowNextFrameVpered_Jump()
{
	if (numcalccadrCount < numCadrs.size() - 1)numcalccadrCount++;
	else numcalccadrCount = numCadrs.size()-1;

	m_Mat = m_VideoDav->cadr(numCadrs[numcalccadrCount]);	
	debugSquares({ naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat);
	Bound1(numCadrs[numcalccadrCount],{ naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat); // рисуем оболочку знака

}
// --- <<< ---
void CTempoDoc::ShowNextFrameNazad_Jump()
{
	if (numcalccadrCount - 1 >= 0)numcalccadrCount--;
	else numcalccadrCount = 1;

	m_Mat = m_VideoDav->cadr(numCadrs[numcalccadrCount]);
	debugSquares({ naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat);

	Bound1(numCadrs[numcalccadrCount], { naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat); // рисуем оболочку знака

	
}

// --- > ---

void CTempoDoc::ShowNextFrameVpered()
{

	//if ((std::string(lpszPathName).find(".dav") != std::string::npos))
	{
		//return;
		//if (cadrCount < m_VideoDav->CountKadr)
		{			
			m_Mat = m_VideoDav->cadr(cadrCount);						
			int nn = (cadrCount % (numCadrs.size() - 1));
			debugSquares( naborCadrs[numCadrs[nn]] , m_Mat);

			/**/

			Bound1(cadrCount, naborCadrs[numCadrs[nn]] , m_Mat); // рисуем оболочку знака



			/*debugSquares(std::vector<std::vector<cv::Point>>
			{
				{cv::Point(584, 338), cv::Point(634, 378)},
				{cv::Point(777, 195), cv::Point(876, 302)}
			}, m_Mat);*/

			/*find_squares(m_Mat, std::vector<std::vector<cv::Point>>
				{
					{cv::Point(584, 338), cv::Point(634, 378)},
				});*/

				//debugSquares([[cv::Point(10, 10), cv::Point(10, 10)]], m_Mat);
				// cadrCount = 1;
				//SetupBitmapInfo(m_Mat);

				//CView* pView = NULL;
				//POSITION pos = GetFirstViewPosition();
				//::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS_STOP, 0, 0);
		}
		
		if (cadrCount < m_VideoDav->CountKadr-1) cadrCount++;
		return;
	}
}

// Play
void CTempoDoc::ShowNextFrame()
{

		//return;
		m_Mat = m_VideoDav->cadr(cadrCount);

		int nn = (cadrCount % (numCadrs.size() - 1));
		debugSquares( naborCadrs[numCadrs[nn]], m_Mat);

		Bound1(numcalccadrCount,  naborCadrs[numCadrs[nn]] , m_Mat); // рисуем оболочку знака
				
		if (cadrCount < m_VideoDav->CountKadr-1) cadrCount++;
		else 
		{			
			CView* pView = NULL;
			POSITION pos = GetFirstViewPosition();
			while (NULL != pos)
			{
				pView = GetNextView(pos);
				::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS_STOP, 0, 0);
			}
		}
		return;
}

void CTempoDoc::ShowNextFrameDav()
{

}
