
// TempoDoc.cpp : implementation of the CTempoDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Tempo.h"
#endif

#include "TempoDoc.h"
#include "jsonlib.h"

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

void CTempoDoc::debugSquares(std::vector<std::vector<cv::Point> > squares, cv::Mat image)
{
	//return;
	//vector<vector<cv::Point> > contours;
	//vector<cv::Vec4i> hierarchy;

	//cv::Mat gray;// = image.clone();
	//cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	//cv::threshold(gray, gray, 1, 255, cv::THRESH_BINARY);
	////cv::xors(gray, cv::Scalar(255, 0, 0, 0), gray, NULL);
	//cv::dilate(gray, gray, NULL, cv::Point(0,0));

	//findContours(gray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//cv::drawContours(image, contours, -1, CV_RGB(0, 0, 0), -1, cv::FILLED, 8);

	//for (size_t i = 0; i < contours.size(); i++)
	//{
	//	cv::Mat pointsf;
	//	cv::Mat(contours[i]).convertTo(pointsf, CV_32F);

	//	//cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
	//	//cv::Point2f rect_points[4];
	//	//minRect.points(rect_points);
	//	//ellipse(image, minRect, cv::Scalar(0, 0, 255), 1, 8);

	//	cv::RotatedRect box = fitEllipse(pointsf);
	//	ellipse(image, box, cv::Scalar(0, 0, 255), 1, 8);
	//}

	//cv::Mat gray, storage;
	//cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	////cv::cvsm(gray, gray, cv::GaussianBlur, 7, 7);

	////IplImage* cc_img = cv::CreateImage(cv::Size(gray), gray->depth, 3);
	////cv::SetZero(cc_img);
	//cv::Scalar(ext_color);

	//cv::Canny(gray, gray, 10, 30, 3);

	////cv::memStorage* storage = cv::storag createMemStorage(0);
	//cv::HoughCircles(gray, storage, cv::HOUGH_GRADIENT, 1, 40 / 6, 100, 50);
	//cv::cvtColor(gray, src, cv::COLOR_GRAY2BGR);
	//for (size_t i = 0; i < storage->total; i++)
	//{
	//	// round the floats to an int
	//	float* p = (float*)cv::getElemSize(circles, i);
	//	cv::Point center(cvRound(p[0]), cvRound(p[1]));
	//	int radius = cvRound(p[2]);

	//	// draw the circle center
	//	//cvCircle(cc_img, center, 3, CV_RGB(0,255,0), -1, 8, 0 );

	//	// draw the circle outline
	//	cv::circle(cc_img, center, radius + 1, CV_RGB(0, 0, 255), 2, 8, 0);

	//	//printf("x: %d y: %d r: %d\n", center.x, center.y, radius);
	//}

	//CvMemStorage* mem;
	//mem = cvCreateMemStorage(0);
	//CvSeq* contours = 0;
	//cvCvtColor(cc_img, gray, CV_BGR2GRAY);
	//// Use either this:
	//int n = cv::findContours(gray, mem, &contours, sizeof(cv::cvContour), cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
	//// Or this:
	////int n = cvFindContours(gray, mem, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	//for (; contours != 0; contours = contours->h_next)
	//{
	//	ext_color = CV_RGB(rand() & 255, rand() & 255, rand() & 255); //randomly coloring different contours
	//	cv::drawContours(cc_img, contours, ext_color, CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0, 0));
	//}

	//***************************************
	//return;

	for (int i = 0; i < squares.size(); i++) {
		// draw contour
		//cv::drawContours(image, squares, i, cv::Scalar(255, 0, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

		// draw bounding rect
		cv::Rect rect = boundingRect(cv::Mat(squares[i]));
		cv::rectangle(image, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 2, 8, 0);

		// draw rotated rect
		cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
		cv::Point2f rect_points[4];
		minRect.points(rect_points);
		for (int j = 0; j < 4; j++) {
			//cv::line(image, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(0, 0, 255), 1, 8); // blue
		}
	}

	//vector<vector<cv::Point> > contours;

	//findContours(image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	//cv::drawContours(image, contours, 0, cv::Scalar(255, 0, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
	////cv::RotatedRect rotRecs[contours.size()];
	//for (int i = 0; i < contours.size(); i++) {
	//	//rotRecs[i] = fitEllipse(contours[i]);
	//}

	//return image;
}


static double angle2(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

/**
 * Helper function to display text in the center of a contour
 */
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	if(label!="CIR")return;

	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), cv::FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}


void Bound2(cv::Mat& src)
{
	return;
	//cv::Mat src = cv::imread("polygon.png");
	//cv::Mat src = cv::imread("assets/basic-shapes-2.png");
	
	// Convert to grayscale

	cv::Mat contour_mat = src.clone();

	cv::Mat gray;
	cv::cvtColor(contour_mat, gray, cv::COLOR_BGR2GRAY);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	cv::Canny(gray, bw, 0, 50, 5);

	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	cv::Mat dst = src.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;

		if (approx.size() == 3)
		{
			setLabel(src, "TRI", contours[i]);    // Triangles
		}
		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc + 1; j++)
				cos.push_back(angle(approx[j % vtc], approx[j - 2], approx[j - 1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
				setLabel(src, "RECT", contours[i]);
			else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
				setLabel(src, "PENTA", contours[i]);
			else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
				setLabel(src, "HEXA", contours[i]);
		}
		else
		{
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
				std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
			{
				cv::Scalar red(0, 0, 255);
				cv::drawContours(src, contours[i], radius, red, 2);
				setLabel(src, "CIR", contours[i]);
			}
		}
	}
}

bool r_center(float radius, int cx, int cy, std::vector<std::vector<cv::Point> > squares)
{

	double r = 0;
	double S = 0;
	// считаем среднее расстояние от центра окружности до оболочки
	for (auto& m : squares)
	{
		for (int i = 0; i < 2; i++)
			r += (cx - m[i].x) * (cx - m[i].x) + (cy - m[i].y) * (cy - m[i].y);
	}
	if (radius>30&&radius<40&&40<sqrt(r) &&sqrt(r) < 100) return true;

	return false;
}

bool r_rectangle(cv::Rect rect, std::vector<std::vector<cv::Point> > squares)
{
	double r = 0;
	double S = (rect.br().x - rect.tl().x)* (rect.br().y - rect.tl().y);
	//for (auto& m : squares)
	// считаем среднее расстояние от вершин найденного контура до оболочки
	{		
	  	r = (rect.br().x - squares[0][1].x) * (rect.br().x - squares[0][1].x) +
			(rect.br().y - squares[0][1].y) * (rect.br().y - squares[0][1].y)+
			(rect.tl().x - squares[0][0].x) * (rect.tl().x - squares[0][0].x) +
			(rect.tl().y - squares[0][0].y) * (rect.tl().y - squares[0][0].y);
	}
	//if (S>100&&140<sqrt(r)&& sqrt(r) < 200) return true;
	if (S > 100 && sqrt(r) < 50) return true;

	return false;
}


void Bound1(int numCadr, std::vector<std::vector<cv::Point> > squares, cv::Mat& src_mat)
{

	//std::vector<std::vector<cv::Point>> contours2;
	//std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(src_mat, contours2, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	//cv::Scalar red(0, 0, 255);
	//cv::drawContours(src_mat, contours2, -1, red, 2);
	////showImg(input, "Detected Contour");

	//return;

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
		if (r_rectangle(rect, squares))
		{
			InsertJsonRect(rect,numCadr);
			cv::rectangle(src_mat, rect, cv::Scalar(0, 0, 255), 2);
		}
		//cv::polylines(src_mat, polylines, true, cv::Scalar(0, 255, 0), 2);		
		if (r_center(radius, center.x, center.y, squares))
		{
			InsertJsonCircle(radius,center,numCadr);
			cv::circle(src_mat, center, radius, cv::Scalar(255, 0, 0), 2);
		}
		
	}
}


// **********************************************
cv::Mat img_gray, input_img;
int thresh = 100;
const int MAX_THRESH = 255;


void draw_and_fill_contours(std::vector<std::vector<cv::Point>>& contours,
	std::vector<std::vector<cv::Point>>& hull,
	std::vector<cv::Vec4i>& hierarchy)
{

	cv::Mat contours_result = input_img.clone();
	cv::Mat fill_contours_result = cv::Mat::zeros(img_gray.size(), CV_8UC3);

	for (unsigned int i = 0, n = contours.size(); i < n; ++i)
	{
		cv::Scalar color = cv::Scalar(0, 0, 255);
		cv::drawContours(contours_result, contours, i, color, 4, 8, hierarchy, 0, cv::Point());
	}

	cv::fillPoly(fill_contours_result, hull, cv::Scalar(255, 255, 255));

	//cv::imshow("Contours Result", contours_result);
	//cv::imshow("Fill Contours Result", fill_contours_result);

}




void find_contours(int, void*) {
	cv::Mat canny_output;
	cv::Canny(img_gray, canny_output, thresh, thresh * 2);
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<cv::Point>> hull(contours.size());
	for (unsigned int i = 0, n = contours.size(); i < n; ++i) {
		cv::convexHull(cv::Mat(contours[i]), hull[i], false);
	}

	draw_and_fill_contours(contours, hull, hierarchy);
}

void Bound3(cv::Mat& input_img)
{
	cv::cvtColor(input_img, img_gray, cv::COLOR_BGR2GRAY);
	cv::blur(img_gray, img_gray, cv::Size(3, 3));
	const std::string source_window("Source");
	//cv::namedWindow(source_window.c_str());
	//cv::imshow(source_window.c_str(), input_img);

	//cv::createTrackbar("Thresh: ", source_window, &thresh, MAX_THRESH, find_contours);
	find_contours(0, 0);
}


// **********************************************



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

		naborCadrs[1] = { cv::Point(584, 338), cv::Point(634, 378) };
		naborCadrs[2] = { cv::Point(622,438), cv::Point(644,394) }; // Test_2.json
		naborCadrs[21] = { cv::Point(339,4), cv::Point(651,149) }, // Test_21.json
		naborCadrs[3] = { cv::Point(636,357), cv::Point(688,302) }; // Test_3.json
		naborCadrs[4] = { cv::Point(684,339), cv::Point(753,266) }; // Test_4.json
		naborCadrs[46] = { cv::Point(664,339), cv::Point(731,269) }, // Test_46.json
		naborCadrs[47] = { cv::Point(736,210), cv::Point(826,305) }; // Test_47.json
		naborCadrs[5] = { cv::Point(777,195), cv::Point(876,302) }; // Test_5.json
		naborCadrs[6] = { cv::Point(1004,148), cv::Point(1024,31) }; // Test_6.json
		naborCadrs[48] = { cv::Point(777, 195), cv::Point(876, 302) }; // Test_48.json

		//Calccadr;

		m_Mat = m_VideoDav->cadr(cadrCount);
		SetupBitmapInfo(m_Mat);
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

void CTempoDoc::ShowNextFrameVpered_Jump()
{
	m_Mat = m_VideoDav->cadr(numCadrs[numcalccadrCount]);	
	debugSquares({ naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat);

	Bound1(numcalccadrCount,{ naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat); // рисуем оболочку знака

	if (numcalccadrCount < numCadrs.size()-1)numcalccadrCount++;	
}

void CTempoDoc::ShowNextFrameNazad_Jump()
{
	m_Mat = m_VideoDav->cadr(numCadrs[numcalccadrCount]);
	debugSquares({ naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat);
	Bound1(numcalccadrCount, { naborCadrs[numCadrs[numcalccadrCount]] }, m_Mat); // рисуем оболочку знака

	if (numcalccadrCount >1)numcalccadrCount--;	
}


void CTempoDoc::ShowNextFrameVpered()
{

	//if ((std::string(lpszPathName).find(".dav") != std::string::npos))
	{
		//return;
		//if (cadrCount < m_VideoDav->CountKadr)
		{			
			m_Mat = m_VideoDav->cadr(cadrCount);						
			int nn = (cadrCount % (numCadrs.size() - 1));
			debugSquares({ naborCadrs[numCadrs[nn]] }, m_Mat);

			/**/

			Bound1(cadrCount,{ naborCadrs[numCadrs[nn]] }, m_Mat); // рисуем оболочку знака



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

void CTempoDoc::ShowNextFrame()
{

		//return;
		m_Mat = m_VideoDav->cadr(cadrCount);

		int nn = (cadrCount % (numCadrs.size() - 1));
		debugSquares({ naborCadrs[numCadrs[nn]]}, m_Mat);
				
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
