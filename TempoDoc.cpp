
// TempoDoc.cpp : implementation of the CTempoDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Tempo.h"
#endif

#include "TempoDoc.h"

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

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0) 
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

//void (std::vector<std::vector<cv::Point> >)findSquaresInImage:(cv::Mat)_image
//{
//	std::vector<std::vector<cv::Point> > squares;
//	cv::Mat pyr, timg, gray0(_image.size(), CV_8U), gray;
//	int thresh = 50, N = 11;
//	cv::pyrDown(_image, pyr, cv::Size(_image.cols / 2, _image.rows / 2));
//	cv::pyrUp(pyr, timg, _image.size());
//	std::vector<std::vector<cv::Point> > contours;
//	for (int c = 0; c < 3; c++) {
//		int ch[] = { c, 0 };
//		mixChannels(&timg, 1, &gray0, 1, ch, 1);
//		for (int l = 0; l < N; l++) {
//			if (l == 0) {
//				cv::Canny(gray0, gray, 0, thresh, 5);
//				cv::dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
//			}
//			else {
//				gray = gray0 >= (l + 1) * 255 / N;
//			}
//			cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
//			std::vector<cv::Point> approx;
//			for (size_t i = 0; i < contours.size(); i++)
//			{
//				cv::approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true) * 0.02, true);
//				if (approx.size() == 4 && fabs(contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx))) {
//					double maxCosine = 0;
//
//					for (int j = 2; j < 5; j++)
//					{
//						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
//						maxCosine = MAX(maxCosine, cosine);
//					}
//
//					if (maxCosine < 0.3) {
//						squares.push_back(approx);
//					}
//				}
//			}
//		}
//	}
//	return squares;
//}

void find_squares(cv::Mat& image, std::vector<std::vector<cv::Point> >& squares)
{
	// blur will enhance edge detection
	cv::Mat blurred(image);
	medianBlur(image, blurred, 9);

	cv::Mat gray0(blurred.size(), CV_8U), gray;
	std::vector<std::vector<cv::Point> > contours;

	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&blurred, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		const int threshold_level = 2;
		for (int l = 0; l < threshold_level; l++)
		{
			// Use Canny instead of zero threshold level!
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				Canny(gray0, gray, 10, 20, 3); // 

				// Dilate helps to remove potential holes between edge segments
				dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
			}
			else
			{
				gray = gray0 >= (l + 1) * 255 / threshold_level;
			}

			// Find contours and store them in a list

			int CV_RETR_LIST = 2; 
			int CV_CHAIN_APPROX_SIMPLE = 2;
			findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

			// Test contours
			std::vector<cv::Point> approx;
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
					cv::isContourConvex(cv::Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}

cv::Mat CTempoDoc::debugSquares(std::vector<std::vector<cv::Point> > squares, cv::Mat image)
{
	for (int i = 0; i < squares.size(); i++) {
		// draw contour
		cv::drawContours(image, squares, i, cv::Scalar(255, 0, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

		// draw bounding rect
		cv::Rect rect = boundingRect(cv::Mat(squares[i]));
		cv::rectangle(image, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 2, 8, 0);

		// draw rotated rect
		cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
		cv::Point2f rect_points[4];
		minRect.points(rect_points);
		for (int j = 0; j < 4; j++) {
			cv::line(image, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(0, 0, 255), 1, 8); // blue
		}
	}

	return image;
}

void Bound(cv::Mat& src_mat)
{
	const char* file_name = "shape.png";

	//cv::Mat src_mat, gray_mat, canny_mat;
	cv::Mat gray_mat, canny_mat;
	cv::Mat contour_mat;
	//cv::Mat bounding_mat;

	const char* src_name = "Source";
	const char* contours_name = "Contours";
	const char* bounding_name = "Bounding";

	//1.Read image file & clone.
	//src_mat = cv::imread(file_name);
	contour_mat = src_mat.clone();
	//bounding_mat = src_mat.clone();

	//2. Convert to gray image and apply canny edge detection
	cvtColor(src_mat, gray_mat, cv::COLOR_RGB2GRAY);
	Canny(gray_mat, canny_mat, 30, 128, 3, false);

	//3. Find & process the contours
	//3.1 find contours on the edge image.
	std::vector<std::vector<cv::Point> > contours;
	findContours(canny_mat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//3.2 draw contours & property value on the source image.
	double area, length;
	drawContours(contour_mat, contours, -1, cv::Scalar(0), 2);	//draw contours on the image
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::Point2f center;
		float radius;

		area = contourArea(contours[i]);
		length = arcLength(contours[i], true);
		minEnclosingCircle(contours[i], center, radius);

		//draw contour property value at the contour center.
		char buffer[64] = { 0 };
		//sprintf(buffer, "Area: %.2lf", area);
		//putText(contour_mat, buffer, center, cv::FONT_HERSHEY_SIMPLEX, .6, cv::Scalar(0), 1);

		//sprintf(buffer, "Length: %.2lf", length);
		//putText(contour_mat, buffer, cv::Point(center.x, center.y + 20), cv::FONT_HERSHEY_SIMPLEX, .6, cv::Scalar(0), 1);

	}

	//3.3 find bounding of each contour, and draw it on the source image.
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::Point2f points[4];
		cv::Point2f center;
		float radius;
		cv::Rect rect;
		cv::RotatedRect rotate_rect;

		//compute the bounding rect, rotated bounding rect, minum enclosing circle.
		rect = boundingRect(contours[i]);
		rotate_rect = minAreaRect(contours[i]);
		minEnclosingCircle(contours[i], center, radius);

		rotate_rect.points(points);

		std::vector<std::vector<cv::Point> > polylines;
		polylines.resize(1);
		for (int j = 0; j < 4; ++j)
			polylines[0].push_back(points[j]);

		//draw them on the bounding image.
		//cv::rectangle(src_mat, rect, cv::Scalar(0, 0, 255), 2);
		//cv::polylines(src_mat, polylines, true, cv::Scalar(0, 255, 0), 2);
		if(radius > 10 &&radius<22)cv::circle(src_mat, center, radius, cv::Scalar(255, 0, 0), 2);
	}
}

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

	Bound(m_Mat);

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

		m_Mat = m_VideoDav->cadr(cadrCount);
		SetupBitmapInfo(m_Mat);
	    ::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 1000 / (int)10, 0);

		

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

void CTempoDoc::ShowNextFrame()
{

	//if ((std::string(lpszPathName).find(".dav") != std::string::npos))
	{
		//return;
		if (cadrCount < m_VideoDav->CountKadr)
		{
			m_Mat = m_VideoDav->cadr(cadrCount);			

			/*
			debugSquares(std::vector<std::vector<cv::Point>>
			{
				{cv::Point(584, 338), cv::Point(634, 378)}, // test_1.json
				{ cv::Point(622,438), cv::Point(644,394) }, // Test_2.json
				{ cv::Point(339,4), cv::Point(651,149) }, // Test_21.json
				{ cv::Point(636,357), cv::Point(688,302) }, // Test_3.json
				{ cv::Point(684,339), cv::Point(753,266) }, // Test_4.json
				{ cv::Point(664,339), cv::Point(731,269) }, // Test_46.json
				{ cv::Point(736,210), cv::Point(826,305) }, // Test_47.json
				{ cv::Point(777,195), cv::Point(876,302) }, // Test_5.json
				{ cv::Point(1004,148), cv::Point(1024,31) }, // Test_6.json
				//{ cv::Point(), cv::Point() }, // Test_.json
				//{ cv::Point(), cv::Point() }, // Test_.json

				{ cv::Point(777, 195), cv::Point(876, 302) }, // Test_48.json

			}, m_Mat);

			/**/

			Bound(m_Mat);



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
		else
		{
			cadrCount = 1;
			CView* pView = NULL;
			POSITION pos = GetFirstViewPosition();
			while (NULL != pos)
			{
				pView = GetNextView(pos);
			    ::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS_STOP, 0, 0);
			}
			
		}		
		cadrCount++;
		return;

		CView* pView = NULL;
		POSITION pos = GetFirstViewPosition();
		return;
		while (NULL != pos)
		{
			pView = GetNextView(pos);
			if (cadrCount > m_VideoDav->CountKadr - 1)
			{
				::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 0, 0);
			}
		}
	}
	//else
	//return;
	//{
	//	if (m_Video.isOpened())
	//	{
	//		m_Video.read(m_Mat);
	//	}
	//	else
	//	{
	//		return;
	//	}

	//	int nIndex = static_cast<int>(m_Video.get(CV_CAP_PROP_POS_FRAMES));
	//	int nTotal = static_cast<int>(m_Video.get(CV_CAP_PROP_FRAME_COUNT));

	//	if (nIndex >= nTotal)
	//	{
	//		CView* pView = NULL;
	//		POSITION pos = GetFirstViewPosition();
	//		while (NULL != pos)
	//		{
	//			pView = GetNextView(pos);
	//			//::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 0, 0);
	//		}
	//	}
	//	else {
	//		//::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 0, 0);
	//	}
	//}	
}

void CTempoDoc::ShowNextFrameDav()
{

	if (cadrCount < m_VideoDav->CountKadr)m_Mat = m_VideoDav->cadr(cadrCount);
	SetupBitmapInfo(m_Mat);
	cadrCount++;

	CView* pView = NULL;
	POSITION pos = GetFirstViewPosition();
	while (NULL != pos)
	{
		pView = GetNextView(pos);
		::PostMessage(pView->GetSafeHwnd(), WMU_SETFPS, 0, 0);
	}

	
}
