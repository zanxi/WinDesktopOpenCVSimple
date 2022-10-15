#pragma once

#include "stdafx.h"
#include "jsonlib.h"

// Функция детектирования знаков 
// подбор по размеру и наименьшему расстоянию до точек прямоугольника
// деление на - круглый и прямоугольный знак

//void Bound1(int numCadr, std::vector<std::vector<cv::Point> > squares, cv::Mat& src_mat)


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
	//if (radius > 30 && radius < 40 && 40 < sqrt(r) && sqrt(r) < 100) return true;
	if ((radius > 30 && radius < 70) && (50 < sqrt(r) && sqrt(r) < 140)) return true;

	return false;
}

bool r_rectangle(cv::Rect rect, std::vector<std::vector<cv::Point> > squares)
{
	double r = 0;
	double S = (rect.br().x - rect.tl().x) * (rect.br().y - rect.tl().y);
	//for (auto& m : squares)
	// считаем среднее расстояние от вершин найденного контура до оболочки
	{
		r = (rect.br().x - squares[0][1].x) * (rect.br().x - squares[0][1].x) +
			(rect.br().y - squares[0][1].y) * (rect.br().y - squares[0][1].y) +
			(rect.tl().x - squares[0][0].x) * (rect.tl().x - squares[0][0].x) +
			(rect.tl().y - squares[0][0].y) * (rect.tl().y - squares[0][0].y);
	}
	//if (S>100&&140<sqrt(r)&& sqrt(r) < 200) return true;
	//if (((S >30 &&S < 40 && sqrt(r) < 70))||(S > 100 && sqrt(r) < 115)) return true;
	//if (((S > 20 && S < 60 && sqrt(r) < 70)) || (S > 100 && sqrt(r) < 115)) return true;
	if ( (S > 150 && sqrt(r)> 50 && sqrt(r) < 115)) return true;

	return false;
}

int Bound4(cv::Mat& img);
int Bound5(cv::Mat& img);
int Bound7(cv::Mat& img);
int Bound8(cv::Mat& img); //!!!!!!!!!!!!!!!!!
int Bound9(cv::Mat& img);
void Bound11(cv::Mat& img);
void Bound12(cv::Mat& img);
void Bound13(cv::Mat& img);
int Bound14(cv::Mat& img);
void Bound17(cv::Mat& img);
void loop(cv::Mat& img);
void Bound18(cv::Mat& img);
void Bound19(cv::Mat& img);


void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour);

// Функция детектирования знаков 
// подбор по размеру и наименьшему расстоянию до точек прямоугольника
// деление на - круглый и прямоугольный знак

void Bound1(int numCadr, std::vector<std::vector<cv::Point> > squares, cv::Mat& src_mat)
{
	Bound8(src_mat);
	Bound19(src_mat);
	//loop(src_mat);
	
	
	return;

	//std::vector<std::vector<cv::Point>> contours2;
	//std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(src_mat, contours2, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	//cv::Scalar red(0, 0, 255);
	//cv::drawContours(src_mat, contours2, -1, red, 2);
	////showImg(input, "Detected Contour");

	//return;

	//const char* file_name = "shape.png";

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
		if (!r_rectangle(rect, squares)) continue;
		{
			//InsertJsonRect(rect, numCadr);

			std::vector<cv::Point> coordTectCapture = { cv::Point(rect.br().x, rect.br().y),
				cv::Point(rect.br().x + 20, rect.br().y + 40) };
			
			//setLabel(src_mat, "Дорожный знак", contours[i]);
			cv::rectangle(src_mat, rect, cv::Scalar(0, 0, 255), 2);
			setLabel(src_mat, "Road sign", coordTectCapture);
		}
		
		//cv::polylines(src_mat, polylines, true, cv::Scalar(0, 255, 0), 2);		
		//if (r_center(radius, center.x, center.y, squares))
		//{
		//	InsertJsonCircle(radius, center, numCadr);
		//	//setLabel(src_mat, "Дорожный знак", contours[i]);			

		//	std::vector<cv::Point> coordTectCapture = { cv::Point(center.x, center.y),
		//		cv::Point(center.x + 20, center.y + 40) };			
		//	cv::circle(src_mat, center, radius, cv::Scalar(255, 0, 0), 2);
		//	setLabel(src_mat, "Road sign", coordTectCapture);
		//}

	}
}

// ***************************************************

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




// ***************************************************

void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	//return;
	//if (label != "CIR")return;

	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	//cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), cv::FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(255, 0, 0), thickness, 8);
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
int Bound4(cv::Mat& img)
{
	if (img.empty())
	{
		cout << "Could not open image..." << endl;
		return -1;
	}

	cvtColor(img, img, cv::COLOR_BGR2GRAY);

	int dilation_type = 0;
	int dilation_elem = 0;

	if (dilation_elem == 0) { dilation_type = cv::MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = cv::MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = cv::MORPH_ELLIPSE; }

	int size = 1;

	cv::Mat element = cv::getStructuringElement(dilation_type, cv::Size(2 * size + 1, 2 * size + 1), cv::Point(size, size));
	morphologyEx(img, img, cv::MORPH_OPEN, element);

	vector<cv::Point2f> points;
	for (int x = 0; x < img.cols; x++)
	{
		for (int y = 0; y < img.rows; y++)
		{
			if (img.at<uchar>(y, x) > 0)
			{
				points.push_back(cv::Point2f(x, y));
			}
		}
	}

	//// Least Square Algorithm 

	float xn = 0, xsum = 0;
	float yn = 0, ysum = 0;
	float n = points.size();

	for (int i = 0; i < n; i++)
	{
		xsum = xsum + points[i].x;
		ysum = ysum + points[i].y;
	}

	xn = xsum / n;
	yn = ysum / n;

	float ui = 0;
	float vi = 0;
	float suu = 0, suuu = 0;
	float svv = 0, svvv = 0;
	float suv = 0;
	float suvv = 0, svuu = 0;

	for (int i = 0; i < n; i++)
	{
		ui = points[i].x - xn;
		vi = points[i].y - yn;

		suu = suu + (ui * ui);
		suuu = suuu + (ui * ui * ui);

		svv = svv + (vi * vi);
		svvv = svvv + (vi * vi * vi);

		suv = suv + (ui * vi);

		suvv = suvv + (ui * vi * vi);
		svuu = svuu + (vi * ui * ui);
	}

	cv::Mat A = (cv::Mat_<float>(2, 2) <<
		suu, suv,
		suv, svv);

	cv::Mat B = (cv::Mat_<float>(2, 1) <<
		0.5 * (suuu + suvv),
		0.5 * (svvv + svuu));

	cv::Mat abc;
	cv::solve(A, B, abc);

	float u = abc.at<float>(0);
	float v = abc.at<float>(1);

	float x = u + xn;
	float y = v + yn;

	float alpha = u * u + v * v + ((suu + svv) / n);
	float r = sqrt(alpha);

	////

	cvtColor(img, img, cv::COLOR_GRAY2BGR);

	// Draw circle
	circle(img, cv::Point(x, y), r, cv::Scalar(255, 0, 0), 1, 8, 0);
	//imshow("window", img);
	//waitKey(0);
	return 0;
}

// **********************************************


std::vector<cv::Rect> getRedObjects(cv::Mat image);
vector<cv::Mat> detectAndDisplaySpeedLimit(cv::Mat frame);
vector<cv::Mat> detectAndDisplayNoParking(cv::Mat frame);
vector<cv::Mat> detectAndDisplayWarning(cv::Mat frame);
void trainDigitClassifier(cv::Mat& src);
string getDigits(cv::Mat image);
vector<cv::Mat> loadAllImage();
int getSpeedLimit(string speed);

//path of the haar cascade files
string no_parking_signs_cascade = "/Users/giuliopettenuzzo/Desktop/cascade_classifiers/no_parking_cascade.xml";
string speed_signs_cascade = "/Users/giuliopettenuzzo/Desktop/cascade_classifiers/speed_limit_cascade.xml";
string warning_signs_cascade = "/Users/giuliopettenuzzo/Desktop/cascade_classifiers/warning_cascade.xml";

cv::CascadeClassifier speed_limit_cascade;
cv::CascadeClassifier no_parking_cascade;
cv::CascadeClassifier warning_cascade;

//int CV_HAAR_SCALE_IMAGE_ = 15;
int SCALE_IMAGE = 5;

int Bound5(cv::Mat& src)
{
	//train the classifier for digit recognition, this require a manually train, read the report for more details
	trainDigitClassifier(src);

	cv::Mat sceneImage;
	vector<cv::Mat> allImages = loadAllImage();

	for (int i = 0; i <= allImages.size(); i++) {
		sceneImage = allImages[i];

		//load the haar cascade files
		if (!speed_limit_cascade.load(speed_signs_cascade)) { printf("--(!)Error loading\n"); return -1; };
		if (!no_parking_cascade.load(no_parking_signs_cascade)) { printf("--(!)Error loading\n"); return -1; };
		if (!warning_cascade.load(warning_signs_cascade)) { printf("--(!)Error loading\n"); return -1; };

		cv::Mat scene = sceneImage.clone();

		//detect the red objects
		std::vector<cv::Rect> allObj = getRedObjects(scene);

		//use the three cascade classifier for each object detected by the getRedObjects() method
		for (int j = 0; j < allObj.size(); j++) {
			cv::Mat img = sceneImage(cv::Rect(allObj[j]));
			vector<cv::Mat> warningVec = detectAndDisplayWarning(img);
			if (warningVec.size() > 0) {
				cv::Rect box = allObj[j];
			}
			vector<cv::Mat> noParkVec = detectAndDisplayNoParking(img);
			if (noParkVec.size() > 0) {
				cv::Rect box = allObj[j];
			}
			vector<cv::Mat> speedLitmitVec = detectAndDisplaySpeedLimit(img);
			if (speedLitmitVec.size() > 0) {
				cv::Rect box = allObj[j];
				for (int i = 0; i < speedLitmitVec.size(); i++) {
					//get speed limit and skatch it in the image
					int digit = getSpeedLimit(getDigits(speedLitmitVec[i]));
					if (digit > 0) {
						cv::Point point = box.tl();
						point.y = point.y + 30;
						cv::putText(sceneImage,
							"SPEED LIMIT " + to_string(digit),
							point,
							cv::FONT_HERSHEY_COMPLEX_SMALL,
							0.7,
							cv::Scalar(0, 255, 0),
							1,
							cv::CV__CAP_PROP_LATEST);
					}
				}
			}
		}
		//imshow("currentobj", sceneImage);
		//waitKey(0);
	}
}

/*
 *  detect the red object in the image given in the param,
 *  return a vector containing all the Rect of the red objects
 */
std::vector<cv::Rect> getRedObjects(cv::Mat image)
{
	cv::Mat3b res = image.clone();
	std::vector<cv::Rect> result;

	cvtColor(image, image, cv::COLOR_BGR2HSV);

	cv::Mat1b mask1, mask2;
	//ranges of red color
	inRange(image, cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255), mask1);
	inRange(image, cv::Scalar(170, 70, 50), cv::Scalar(180, 255, 255), mask2);

	cv::Mat1b mask = mask1 | mask2;
	cv::Mat nonZeroCoordinates;
	vector<cv::Point> pts;

	cv::findNonZero(mask, pts);
	for (int i = 0; i < nonZeroCoordinates.total(); i++) {
		std::cout << "Zero#" << i << ": " << nonZeroCoordinates.at<cv::Point>(i).x << ", " << nonZeroCoordinates.at<cv::Point>(i).y << endl;
	}

	int th_distance = 2; // radius tolerance

	 // Apply partition
	 // All pixels within the radius tolerance distance will belong to the same class (same label)
	vector<int> labels;

	// With lambda function (require C++11)
	int th2 = th_distance * th_distance;
	int n_labels = partition(pts, labels, [th2](const cv::Point& lhs, const cv::Point& rhs) {
		return ((lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y)) < th2;
		});

	// You can save all points in the same class in a vector (one for each class), just like findContours
	vector<vector<cv::Point>> contours(n_labels);
	for (int i = 0; i < pts.size(); ++i) {
		contours[labels[i]].push_back(pts[i]);
	}

	// Get bounding boxes
	vector<cv::Rect> boxes;
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::Rect box = boundingRect(contours[i]);
		if (contours[i].size() > 500) {//prima era 1000
			boxes.push_back(box);

			cv::Rect enlarged_box = box + cv::Size(100, 100);
			enlarged_box -= cv::Point(30, 30);

			if (enlarged_box.x < 0) {
				enlarged_box.x = 0;
			}
			if (enlarged_box.y < 0) {
				enlarged_box.y = 0;
			}
			if (enlarged_box.height + enlarged_box.y > res.rows) {
				enlarged_box.height = res.rows - enlarged_box.y;
			}
			if (enlarged_box.width + enlarged_box.x > res.cols) {
				enlarged_box.width = res.cols - enlarged_box.x;
			}

			cv::Mat img = res(cv::Rect(enlarged_box));
			result.push_back(enlarged_box);
		}
	}
	cv::Rect largest_box = *max_element(boxes.begin(), boxes.end(), [](const cv::Rect& lhs, const cv::Rect& rhs) {
		return lhs.area() < rhs.area();
		});

	//draw the rects in case you want to see them
	for (int j = 0; j <= boxes.size(); j++) {
		if (boxes[j].area() > largest_box.area() / 3) {
			rectangle(res, boxes[j], cv::Scalar(0, 0, 255));

			cv::Rect enlarged_box = boxes[j] + cv::Size(20, 20);
			enlarged_box -= cv::Point(10, 10);

			rectangle(res, enlarged_box, cv::Scalar(0, 255, 0));
		}
	}

	rectangle(res, largest_box, cv::Scalar(0, 0, 255));

	cv::Rect enlarged_box = largest_box + cv::Size(20, 20);
	enlarged_box -= cv::Point(10, 10);

	rectangle(res, enlarged_box, cv::Scalar(0, 255, 0));

	return result;
}

/*
 *  code for detect the speed limit sign , it draws a circle around the speed limit signs
 */
vector<cv::Mat> detectAndDisplaySpeedLimit(cv::Mat frame)
{
	std::vector<cv::Rect> signs;
	vector<cv::Mat> result;
	cv::Mat frame_gray;

	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	//normalizes the brightness and increases the contrast of the image
	equalizeHist(frame_gray, frame_gray);

	//-- Detect signs
	speed_limit_cascade.detectMultiScale(frame_gray, signs, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	//std::cout << speed_limit_cascade.getFeatureType();

	for (size_t i = 0; i < signs.size(); i++)
	{
		cv::Point center(signs[i].x + signs[i].width * 0.5, signs[i].y + signs[i].height * 0.5);
		ellipse(frame, center, cv::Size(signs[i].width * 0.5, signs[i].height * 0.5), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);


		cv::Mat resultImage = frame(cv::Rect(center.x - signs[i].width * 0.5, center.y - signs[i].height * 0.5, signs[i].width, signs[i].height));
		result.push_back(resultImage);
	}
	return result;
}

/*
 *  code for detect the warning sign , it draws a circle around the warning signs
 */
vector<cv::Mat> detectAndDisplayWarning(cv::Mat frame)
{
	std::vector<cv::Rect> signs;
	vector<cv::Mat> result;
	cv::Mat frame_gray;

	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect signs
	warning_cascade.detectMultiScale(frame_gray, signs, 1.1, 3, 0 | SCALE_IMAGE, cv::Size(30, 30));
	//cout << warning_cascade.getFeatureType();
	cv::Rect previus;


	for (size_t i = 0; i < signs.size(); i++)
	{
		cv::Point center(signs[i].x + signs[i].width * 0.5, signs[i].y + signs[i].height * 0.5);
		cv::Rect newRect = cv::Rect(center.x - signs[i].width * 0.5, center.y - signs[i].height * 0.5, signs[i].width, signs[i].height);
		if ((previus & newRect).area() > 0) {
			previus = newRect;
		}
		else {
			ellipse(frame, center, cv::Size(signs[i].width * 0.5, signs[i].height * 0.5), 0, 0, 360, cv::Scalar(0, 0, 255), 4, 8, 0);
			cv::Mat resultImage = frame(newRect);
			result.push_back(resultImage);
			previus = newRect;
		}
	}
	return result;
}

/*
 *  code for detect the no parking sign , it draws a circle around the no parking signs
 */
vector<cv::Mat> detectAndDisplayNoParking(cv::Mat frame)
{
	std::vector<cv::Rect> signs;
	vector<cv::Mat> result;
	cv::Mat frame_gray;

	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect signs
	no_parking_cascade.detectMultiScale(frame_gray, signs, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	//cout << no_parking_cascade.getFeatureType();
	cv::Rect previus;

	for (size_t i = 0; i < signs.size(); i++)
	{
		cv::Point center(signs[i].x + signs[i].width * 0.5, signs[i].y + signs[i].height * 0.5);
		cv::Rect newRect = cv::Rect(center.x - signs[i].width * 0.5, center.y - signs[i].height * 0.5, signs[i].width, signs[i].height);
		if ((previus & newRect).area() > 0) {
			previus = newRect;
		}
		else {
			ellipse(frame, center, cv::Size(signs[i].width * 0.5, signs[i].height * 0.5), 0, 0, 360, cv::Scalar(255, 0, 0), 4, 8, 0);
			cv::Mat resultImage = frame(newRect);
			result.push_back(resultImage);
			previus = newRect;
		}
	}
	return result;
}

/*
 *  train the classifier for digit recognition, this could be done only one time, this method save the result in a file and
 *  it can be used in the next executions
 *  in order to train user must enter manually the corrisponding digit that the program shows, press space if the red box is just a point (false positive)
 */
void trainDigitClassifier(cv::Mat& src) {
	cv::Mat thr, gray, con;
	//cv::Mat src = imread("/Users/giuliopettenuzzo/Desktop/all_numbers.png", 1);
	cvtColor(src, gray, cv::COLOR_BGR2GRAY);
	threshold(gray, thr, 125, 255, cv::THRESH_BINARY_INV); //Threshold to find contour
	//imshow("ci", thr);
	//waitKey(0);
	thr.copyTo(con);

	// Create sample and label data
	vector< vector <cv::Point> > contours; // Vector for storing contour
	vector< cv::Vec4i > hierarchy;
	cv::Mat sample;
	cv::Mat response_array;
	findContours(con, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE); //Find contour

	for (int i = 0; i < contours.size(); i = hierarchy[i][0]) // iterate through first hierarchy level contours
	{
		cv::Rect r = boundingRect(contours[i]); //Find bounding rect for each contour
		rectangle(src, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(0, 0, 255), 2, 8, 0);
		cv::Mat ROI = thr(r); //Crop the image
		cv::Mat tmp1, tmp2;
		cv::resize(ROI, tmp1, cv::Size(10, 10), 0, 0, cv::INTER_LINEAR); //resize to 10X10
		tmp1.convertTo(tmp2, CV_32FC1); //convert to float

		//imshow("src", src);

		int c = cv::waitKey(0); // Read corresponding label for contour from keyoard
		c -= 0x30;     // Convert ascii to intiger value
		response_array.push_back(c); // Store label to a mat
		rectangle(src, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(0, 255, 0), 2, 8, 0);
		sample.push_back(tmp2.reshape(1, 1)); // Store  sample data
	}

	// Store the data to file
	cv::Mat response, tmp;
	tmp = response_array.reshape(1, 1); //make continuous
	tmp.convertTo(response, CV_32FC1); // Convert  to float

	cv::FileStorage Data("TrainingData.yml", cv::FileStorage::WRITE); // Store the sample data in a file
	//Data << "data" << sample;
	Data.release();

	cv::FileStorage Label("LabelData.yml", cv::FileStorage::WRITE); // Store the label data in a file
	//cv::Label << "label" << response;
	Label.release();
	//cout << "Training and Label data created successfully....!! " << endl;

	//imshow("src", src);
	//waitKey(0);


}

/*
 *  get digit from the image given in param, using the classifier trained before
 */
string getDigits(cv::Mat image)
{
	cv::Mat thr1, gray1, con1;
	cv::Mat src1 = image.clone();
	cv::cvtColor(src1, gray1, cv::COLOR_BGR2GRAY);
	threshold(gray1, thr1, 125, 255, cv::THRESH_BINARY_INV); // Threshold to create input
	thr1.copyTo(con1);


	// Read stored sample and label for training
	cv::Mat sample1;
	cv::Mat response1, tmp1;
	cv::FileStorage Data1("TrainingData.yml", cv::FileStorage::READ); // Read traing data to a Mat
	Data1["data"] >> sample1;
	Data1.release();

	cv::FileStorage Label1("LabelData.yml", cv::FileStorage::READ); // Read label data to a Mat
	Label1["label"] >> response1;
	Label1.release();


	cv::Ptr<cv::ml::KNearest>  knn(cv::ml::KNearest::create());

	knn->train(sample1, cv::ml::ROW_SAMPLE, response1); // Train with sample and responses
	cout << "Training compleated.....!!" << endl;

	vector< vector <cv::Point> > contours1; // Vector for storing contour
	vector< cv::Vec4i > hierarchy1;

	//Create input sample by contour finding and cropping
	findContours(con1, contours1, hierarchy1, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
	cv::Mat dst1(src1.rows, src1.cols, CV_8UC3, cv::Scalar::all(0));
	string result;

	for (int i = 0; i < contours1.size(); i = hierarchy1[i][0]) // iterate through each contour for first hierarchy level .
	{
		cv::Rect r = boundingRect(contours1[i]);
		cv::Mat ROI = thr1(r);
		cv::Mat tmp1, tmp2;
		resize(ROI, tmp1, cv::Size(10, 10), 0, 0, cv::INTER_LINEAR);
		tmp1.convertTo(tmp2, CV_32FC1);
		cv::Mat bestLabels;
		float p = knn->findNearest(tmp2.reshape(1, 1), 4, bestLabels);
		char name[4];
		sprintf(name, "%d", (int)p);
		cout << "num = " << (int)p;
		result = result + to_string((int)p);

		putText(dst1, name, cv::Point(r.x, r.y + r.height), 0, 1, cv::Scalar(0, 255, 0), 2, 8);
	}

	imwrite("dest.jpg", dst1);
	return  result;
}
/*
 *  from the digits detected, it returns a speed limit if it is detected correctly, -1 otherwise
 */
int getSpeedLimit(string numbers) {
	if ((numbers.find("30") != std::string::npos) || (numbers.find("03") != std::string::npos)) {
		return 30;
	}
	if ((numbers.find("50") != std::string::npos) || (numbers.find("05") != std::string::npos)) {
		return 50;
	}
	if ((numbers.find("80") != std::string::npos) || (numbers.find("08") != std::string::npos)) {
		return 80;
	}
	if ((numbers.find("70") != std::string::npos) || (numbers.find("07") != std::string::npos)) {
		return 70;
	}
	if ((numbers.find("90") != std::string::npos) || (numbers.find("09") != std::string::npos)) {
		return 90;
	}
	if ((numbers.find("100") != std::string::npos) || (numbers.find("001") != std::string::npos)) {
		return 100;
	}
	if ((numbers.find("130") != std::string::npos) || (numbers.find("031") != std::string::npos)) {
		return 130;
	}
	return -1;
}

/*
 *  load all the image in the file with the path hard coded below
 */
vector<cv::Mat> loadAllImage() {
	vector<cv::String> fn;
	cv::glob("/Users/giuliopettenuzzo/Desktop/T1/dataset/*.jpg", fn, false);

	vector<cv::Mat> images;
	return images;
	size_t count = fn.size(); //number of png files in images folder
	for (size_t i = 0; i < count; i++)
		images.push_back(cv::imread(fn[i]));
	return images;
}

// ************************************************************

float verifyCircle(cv::Mat dt, cv::Point2f center, float radius, std::vector<cv::Point2f>& inlierSet)
{
	unsigned int counter = 0;
	unsigned int inlier = 0;
	float minInlierDist = 2.0f;
	float maxInlierDistMax = 100.0f;
	float maxInlierDist = radius / 25.0f;
	if (maxInlierDist < minInlierDist) maxInlierDist = minInlierDist;
	if (maxInlierDist > maxInlierDistMax) maxInlierDist = maxInlierDistMax;

	// choose samples along the circle and count inlier percentage
	for (float t = 0; t < 2 * 3.14159265359f; t += 0.05f)
	{
		counter++;
		float cX = radius * cos(t) + center.x;
		float cY = radius * sin(t) + center.y;

		if (cX < dt.cols)
			if (cX >= 0)
				if (cY < dt.rows)
					if (cY >= 0)
						if (dt.at<float>(cY, cX) < maxInlierDist)
						{
							inlier++;
							inlierSet.push_back(cv::Point2f(cX, cY));
						}
	}

	return (float)inlier / float(counter);
}


void getCircle(cv::Point2f& p1, cv::Point2f& p2, cv::Point2f& p3, cv::Point2f& center, float& radius)
{
	float x1 = p1.x;
	float x2 = p2.x;
	float x3 = p3.x;

	float y1 = p1.y;
	float y2 = p2.y;
	float y3 = p3.y;

	// PLEASE CHECK FOR TYPOS IN THE FORMULA :)
	center.x = (x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) + (x3 * x3 + y3 * y3) * (y1 - y2);
	center.x /= (2 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));

	center.y = (x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) + (x3 * x3 + y3 * y3) * (x2 - x1);
	center.y /= (2 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));

	radius = sqrt((center.x - x1) * (center.x - x1) + (center.y - y1) * (center.y - y1));
}



std::vector<cv::Point2f> getPointPositions(cv::Mat binaryImage)
{
	std::vector<cv::Point2f> pointPositions;

	for (unsigned int y = 0; y < binaryImage.rows; ++y)
	{
		//unsigned char* rowPtr = binaryImage.ptr<unsigned char>(y);
		for (unsigned int x = 0; x < binaryImage.cols; ++x)
		{
			//if(rowPtr[x] > 0) pointPositions.push_back(cv::Point2i(x,y));
			if (binaryImage.at<unsigned char>(y, x) > 0) pointPositions.push_back(cv::Point2f(x, y));
		}
	}

	return pointPositions;
}

int Bound7(cv::Mat& color)
{
	//cv::Mat color = cv::imread("../inputData/semi_circle_contrast.png");
	//cv::Mat color = cv::imread("../inputData/semi_circle_median.png");
	cv::Mat gray;

	// convert to grayscale
	cv::cvtColor(color, gray, cv::COLOR_BGR2GRAY);

	// now map brightest pixel to 255 and smalles pixel val to 0. this is for easier finding of threshold
	double min, max;
	cv::minMaxLoc(gray, &min, &max);
	float sub = min;
	float mult = 255.0f / (float)(max - sub);
	cv::Mat normalized = gray - sub;
	normalized = mult * normalized;
	//cv::imshow("normalized", normalized);
	//--------------------------------


	// now compute threshold
	// TODO: this might ne a tricky task if noise differs...
	cv::Mat mask;
	//cv::threshold(input, mask, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::threshold(normalized, mask, 100, 255, cv::THRESH_BINARY);



	std::vector<cv::Point2f> edgePositions;
	edgePositions = getPointPositions(mask);

	// create distance transform to efficiently evaluate distance to nearest edge
	cv::Mat dt;
	cv::distanceTransform(255 - mask, dt, cv::DIST_L1, 3);

	//TODO: maybe seed random variable for real random numbers.

	unsigned int nIterations = 0;

	cv::Point2f bestCircleCenter;
	float bestCircleRadius;
	float bestCirclePercentage = 0;
	float minRadius = 50;   // TODO: ADJUST THIS PARAMETER TO YOUR NEEDS, otherwise smaller circles wont be detected or "small noise circles" will have a high percentage of completion

	//float minCirclePercentage = 0.2f;
	float minCirclePercentage = 0.05f;  // at least 5% of a circle must be present? maybe more...

	int maxNrOfIterations = 2;// edgePositions.size();   // TODO: adjust this parameter or include some real ransac criteria with inlier/outlier percentages to decide when to stop

	for (unsigned int its = 0; its < maxNrOfIterations; ++its)
	{
		//RANSAC: randomly choose 3 point and create a circle:
		//TODO: choose randomly but more intelligent, 
		//so that it is more likely to choose three points of a circle. 
		//For example if there are many small circles, it is unlikely to randomly choose 3 points of the same circle.
		unsigned int idx1 = rand() % edgePositions.size();
		unsigned int idx2 = rand() % edgePositions.size();
		unsigned int idx3 = rand() % edgePositions.size();

		// we need 3 different samples:
		if (idx1 == idx2) continue;
		if (idx1 == idx3) continue;
		if (idx3 == idx2) continue;

		// create circle from 3 points:
		cv::Point2f center; float radius;
		getCircle(edgePositions[idx1], edgePositions[idx2], edgePositions[idx3], center, radius);

		// inlier set unused at the moment but could be used to approximate a (more robust) circle from alle inlier
		std::vector<cv::Point2f> inlierSet;

		//verify or falsify the circle by inlier counting:
		float cPerc = verifyCircle(dt, center, radius, inlierSet);

		// update best circle information if necessary
		if (cPerc >= bestCirclePercentage)
			if (radius >= minRadius)
			{
				bestCirclePercentage = cPerc;
				bestCircleRadius = radius;
				bestCircleCenter = center;
			}

	}

	// draw if good circle was found
	if (bestCirclePercentage >= minCirclePercentage)
		if (bestCircleRadius >= minRadius);
	cv::circle(color, bestCircleCenter, bestCircleRadius, cv::Scalar(155, 255, 0), 4);


	/*cv::imshow("output", color);
	cv::imshow("mask", mask);
	cv::waitKey(0);*/

	return 0;
}

// ************* Half Circle **********************

//float verifyCircle(cv::Mat dt, cv::Point2f center, float radius, std::vector<cv::Point2f>& inlierSet)
//{
//	unsigned int counter = 0;
//	unsigned int inlier = 0;
//	float minInlierDist = 2.0f;
//	float maxInlierDistMax = 100.0f;
//	float maxInlierDist = radius / 25.0f;
//	if (maxInlierDist < minInlierDist) maxInlierDist = minInlierDist;
//	if (maxInlierDist > maxInlierDistMax) maxInlierDist = maxInlierDistMax;
//
//	// choose samples along the circle and count inlier percentage
//	for (float t = 0; t < 2 * 3.14159265359f; t += 0.05f)
//	{
//		counter++;
//		float cX = radius * cos(t) + center.x;
//		float cY = radius * sin(t) + center.y;
//
//		if (cX < dt.cols)
//			if (cX >= 0)
//				if (cY < dt.rows)
//					if (cY >= 0)
//						if (dt.at<float>(cY, cX) < maxInlierDist)
//						{
//							inlier++;
//							inlierSet.push_back(cv::Point2f(cX, cY));
//						}
//	}
//
//	return (float)inlier / float(counter);
//}
//
//
//inline void getCircle2(cv::Point2f& p1, cv::Point2f& p2, cv::Point2f& p3, cv::Point2f& center, float& radius)
//{
//	float x1 = p1.x;
//	float x2 = p2.x;
//	float x3 = p3.x;
//
//	float y1 = p1.y;
//	float y2 = p2.y;
//	float y3 = p3.y;
//
//	// PLEASE CHECK FOR TYPOS IN THE FORMULA :)
//	center.x = (x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) + (x3 * x3 + y3 * y3) * (y1 - y2);
//	center.x /= (2 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));
//
//	center.y = (x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) + (x3 * x3 + y3 * y3) * (x2 - x1);
//	center.y /= (2 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));
//
//	radius = sqrt((center.x - x1) * (center.x - x1) + (center.y - y1) * (center.y - y1));
//}
//
//
//
//std::vector<cv::Point2f> getPointPositions(cv::Mat binaryImage)
//{
//	std::vector<cv::Point2f> pointPositions;
//
//	for (unsigned int y = 0; y < binaryImage.rows; ++y)
//	{
//		//unsigned char* rowPtr = binaryImage.ptr<unsigned char>(y);
//		for (unsigned int x = 0; x < binaryImage.cols; ++x)
//		{
//			//if(rowPtr[x] > 0) pointPositions.push_back(cv::Point2i(x,y));
//			if (binaryImage.at<unsigned char>(y, x) > 0) pointPositions.push_back(cv::Point2f(x, y));
//		}
//	}
//
//	return pointPositions;
//}
//
//void Bound8(cv::Mat& color)
//{
//	//RANSAC
//
//	//load edge image
//	//cv::Mat color = cv::imread("../circleDetectionEdges.png");
//
//	// convert to grayscale
//	cv::Mat gray;
//	cv::cvtColor(color, gray, cv::COLOR_RGB2GRAY);
//
//	// get binary image
//	cv::Mat mask = gray > 0;
//	//erode the edges to obtain sharp/thin edges (undo the blur?)
//	cv::erode(mask, mask, cv::Mat());
//
//	std::vector<cv::Point2f> edgePositions;
//	edgePositions = getPointPositions(mask);
//
//	// create distance transform to efficiently evaluate distance to nearest edge
//	cv::Mat dt;
//	cv::distanceTransform(255 - mask, dt, cv::DIST_L1, 3);
//
//	//TODO: maybe seed random variable for real random numbers.
//
//	unsigned int nIterations = 0;
//
//	char quitKey = 'q';
//	std::cout << "press " << quitKey << " to stop" << std::endl;
//	while (cv::waitKey(-1) != quitKey)
//	{
//		//RANSAC: randomly choose 3 point and create a circle:
//		//TODO: choose randomly but more intelligent, 
//		//so that it is more likely to choose three points of a circle. 
//		//For example if there are many small circles, it is unlikely to randomly choose 3 points of the same circle.
//		unsigned int idx1 = rand() % edgePositions.size();
//		unsigned int idx2 = rand() % edgePositions.size();
//		unsigned int idx3 = rand() % edgePositions.size();
//
//		// we need 3 different samples:
//		if (idx1 == idx2) continue;
//		if (idx1 == idx3) continue;
//		if (idx3 == idx2) continue;
//
//		// create circle from 3 points:
//		cv::Point2f center; float radius;
//		getCircle2(edgePositions[idx1], edgePositions[idx2], edgePositions[idx3], center, radius);
//
//		float minCirclePercentage = 0.4f;
//
//		// inlier set unused at the moment but could be used to approximate a (more robust) circle from alle inlier
//		std::vector<cv::Point2f> inlierSet;
//
//		//verify or falsify the circle by inlier counting:
//		float cPerc = verifyCircle(dt, center, radius, inlierSet);
//
//		if (cPerc >= minCirclePercentage)
//		{
//			std::cout << "accepted circle with " << cPerc * 100.0f << " % inlier" << std::endl;
//			// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
//			// but that's a TODO
//
//			//std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
//			cv::circle(color, center, radius, cv::Scalar(255, 255, 0), 1);
//
//			// accept circle => remove it from the edge list
//			cv::circle(mask, center, radius, cv::Scalar(0), 10);
//
//			//update edge positions and distance transform
//			edgePositions = getPointPositions(mask);
//			cv::distanceTransform(255 - mask, dt, cv::DIST_L1, 3);
//		}
//
//		cv::Mat tmp;
//		mask.copyTo(tmp);
//
//		// prevent cases where no fircle could be extracted (because three points collinear or sth.)
//		// filter NaN values
//		if ((center.x == center.x) && (center.y == center.y) && (radius == radius))
//		{
//			cv::circle(tmp, center, radius, cv::Scalar(255));
//		}
//		else
//		{
//			//std::cout << "circle illegal" << std::endl;
//		}
//
//		++nIterations;
//		//cv::namedWindow("RANSAC"); cv::imshow("RANSAC", tmp);
//	}
//
//	/*std::cout << nIterations << " iterations performed" << std::endl;
//
//
//	cv::namedWindow("edges"); cv::imshow("edges", mask);
//	cv::namedWindow("color"); cv::imshow("color", color);
//
//	cv::imwrite("detectedCircles.png", color);
//	cv::waitKey(-1);*/
//	
//}

// *********************************************************

void check_program_arguments(int argc) {
	if (argc != 2) {
		std::cout << "Error! Program usage:" << std::endl;
		std::cout << "./circle_detect image_circles_path" << std::endl;
		std::exit(-1);
	}
}

void check_if_image_exist(const cv::Mat& img, const std::string& path) {
	if (img.empty()) {
		std::cout << "Error! Unable to load image: " << path << std::endl;
		std::exit(-1);
	}
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int Bound8(cv::Mat& bgr_image) {
	// Usage: ./circle_detect image_circles_path
	//check_program_arguments(argc);

	// Load input image
	//std::string path_image{ argv[1] };
	//cv::Mat bgr_image = cv::imread(path_image);

	// Check if the image can be loaded
	//check_if_image_exist(bgr_image, path_image);

	cv::Mat orig_image = bgr_image.clone();

	cv::medianBlur(bgr_image, bgr_image, 3);

	// Convert input image to HSV
	cv::Mat hsv_image;
	cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

	// Threshold the HSV image, keep only the red pixels
	cv::Mat lower_red_hue_range;
	cv::Mat upper_red_hue_range;
	cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
	cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

	// Combine the above two images
	cv::Mat red_hue_image;
	cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

	cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);

	// Use the Hough transform to detect circles in the combined threshold image
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(red_hue_image, circles, cv::HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 0);

	// Loop over all detected circles and outline them on the original image
	//if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		//cv::circle(orig_image, center, radius, cv::Scalar(0, 255, 0), 5);
		cv::Scalar red(0, 0, 255);
		cv::circle(bgr_image, center, radius, red, 5);
		std::vector<cv::Point> coordTectCapture = 
		{ cv::Point(center.x, center.y), 
			cv::Point(center.x - 70, center.y-170) };
		setLabel(bgr_image, "Circle Road sign", coordTectCapture);
	}

	// Show images
	/*cv::namedWindow("Threshold lower image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold lower image", lower_red_hue_range);
	cv::namedWindow("Threshold upper image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold upper image", upper_red_hue_range);
	cv::namedWindow("Combined threshold images", cv::WINDOW_AUTOSIZE);
	cv::imshow("Combined threshold images", red_hue_image);
	cv::namedWindow("Detected red circles on the input image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Detected red circles on the input image", orig_image);

	cv::waitKey(0);*/


	return 0;
}

int Bound8_rect(cv::Mat& bgr_image) {
	// Usage: ./circle_detect image_circles_path
	//check_program_arguments(argc);

	// Load input image
	//std::string path_image{ argv[1] };
	//cv::Mat bgr_image = cv::imread(path_image);

	// Check if the image can be loaded
	//check_if_image_exist(bgr_image, path_image);

	cv::Mat orig_image = bgr_image.clone();

	cv::medianBlur(bgr_image, bgr_image, 3);

	// Convert input image to HSV
	cv::Mat hsv_image;
	cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

	// Threshold the HSV image, keep only the red pixels
	cv::Mat lower_red_hue_range;
	cv::Mat upper_red_hue_range;
	cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
	cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

	// Combine the above two images
	cv::Mat red_hue_image;
	cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

	cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);

	// Use the Hough transform to detect circles in the combined threshold image
	std::vector<cv::Vec3f> circles;
	cv::HoughLines(red_hue_image, circles, cv::HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 0);

	// Loop over all detected circles and outline them on the original image
	//if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		//cv::circle(orig_image, center, radius, cv::Scalar(0, 255, 0), 5);
		cv::Scalar red(0, 0, 255);
		cv::circle(bgr_image, center, radius, red, 5);
	}

	// Show images
	/*cv::namedWindow("Threshold lower image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold lower image", lower_red_hue_range);
	cv::namedWindow("Threshold upper image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold upper image", upper_red_hue_range);
	cv::namedWindow("Combined threshold images", cv::WINDOW_AUTOSIZE);
	cv::imshow("Combined threshold images", red_hue_image);
	cv::namedWindow("Detected red circles on the input image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Detected red circles on the input image", orig_image);

	cv::waitKey(0);*/


	return 0;
}

void fill_regions(const cv::Mat& bgr, const cv::Mat& prospective) {
	static cv::Scalar WHITE = cv::Scalar::all(255);
	int rows = bgr.rows;
	int cols = bgr.cols;

	// For the given prospective markers, finds
	// object boundaries on the given BGR image.
	cv::Mat markers = prospective.clone();
	cv::watershed(bgr, markers);

	// Copies the boundaries of the objetcs segmented by cv::watershed().
	// Ensures there is a minimum distance of 1 pixel between boundary
	// pixels and the image border.
	cv::Mat borders(rows + 2, cols + 2, CV_8U);
	for (int i = 0; i < rows; i++) {
		uchar* u = borders.ptr<uchar>(i + 1) + 1;
		int* v = markers.ptr<int>(i);
		for (int j = 0; j < cols; j++, u++, v++) {
			*u = (*v == -1);
		}
	}

	// Calculates contour vectors for the boundaries extracted above.
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(
		borders, contours, hierarchy,
		cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE
	);

	int area = bgr.size().area();
	cv::Mat regions(borders.size(), CV_32S);
	for (int i = 0, n = contours.size(); i < n; i++) {
		// Ignores contours for which the bounding rectangle's
		// area equals the area of the original image.
		std::vector<cv::Point>& contour = contours[i];
		if (cv::boundingRect(contour).area() == area) {
			continue;
		}

		// Draws the selected contour.
		cv::drawContours(
			bgr, contours, i, WHITE,
			cv::FILLED, 8, hierarchy, INT_MAX
		);
	}

	//regions(cv::Rect(1, 1, cols, rows));

	// Removes the 1 pixel-thick border added when the boundaries
	// were first copied from the output of cv::watershed().
	//return regions(cv::Rect(1, 1, cols, rows));
}

//Share
//Follow

// *******************************************************

cv::Mat src_gray;
int _thresh = 100;
cv::RNG rng(12345);
void thresh_callback(int, void*);
int Bound9(cv::Mat& src)
{
	//cv::CommandLineParser parser(argc, argv, "{@input | stuff.jpg | input image}");
	//cv::Mat src = cv::imread(cv::samples::findFile(parser.get<cv::String>("@input")));
	/*if (src.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		cout << "Usage: " << argv[0] << " <Input image>" << endl;
		return -1;
	}*/
	cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	blur(src_gray, src_gray, cv::Size(3, 3));
	const char* source_window = "Source";
	//cv::namedWindow(source_window);
	//imshow(source_window, src);
	const int max_thresh = 255;
	//cv::createTrackbar("Canny thresh:", source_window, &_thresh, max_thresh, thresh_callback);
	thresh_callback(0, 0);
	//cv::waitKey();
	return 0;
}


void thresh_callback(int, void*)
{
	cv::Mat canny_output;
	Canny(src_gray, canny_output, _thresh, _thresh * 2);
	vector<vector<cv::Point> > contours;
	findContours(canny_output, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	vector<cv::RotatedRect> minRect(contours.size());
	vector<cv::RotatedRect> minEllipse(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		minRect[i] = minAreaRect(contours[i]);
		if (contours[i].size() > 5)
		{
			minEllipse[i] = fitEllipse(contours[i]);
		}
	}
	cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		// contour
		drawContours(drawing, contours, (int)i, color);
		// ellipse
		ellipse(drawing, minEllipse[i], color, 2);
		// rotated rectangle
		cv::Point2f rect_points[4];
		minRect[i].points(rect_points);
		for (int j = 0; j < 4; j++)
		{
			line(drawing, rect_points[j], rect_points[(j + 1) % 4], color);
		}
	}
	//imshow("Contours", drawing);
}

// **********************************************
float min_size = 10;
void Bound11(cv::Mat& im)
{
	cv::Mat gray;
	cvtColor(im, gray, cv::COLOR_BGR2GRAY);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	Canny(gray, bw, 0, 100, 5, true);

	// Find contours
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;

	findContours(bw, contours, hierarchy,
		cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);


	// watershed
	cv::Mat markers(bw.size(), CV_32S);
	markers = cv::Scalar::all(0);
	int idx = 0;
	int compCount = 0;
	for (; idx >= 0; idx = hierarchy[idx][0], compCount++) {
		if (fabs(contourArea(contours[compCount])) < min_size)
			continue;
		drawContours(markers, contours, idx, cv::Scalar::all(compCount + 1), 1, 8, hierarchy, INT_MAX);
	}
	watershed(im, markers);
}

// ************************************

void Bound12(cv::Mat& src)
{	
	// the first command-line parameter must be a filename of the binary
	// (black-n-white) image
	/*if (argc != 2 || !(src = imread(argv[1], 0)).data)
		return -1;*/

	/*Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);

	src = src > 1;
	namedWindow("Source", 1);
	imshow("Source", src);*/
	cv::Mat dst = src.clone();

	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;

	cv::findContours(dst, contours, hierarchy,
		cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

	// iterate through all the top-level contours,
	// draw each connected component with its own random color
	int idx = 0;
	for (; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
		//cv::drawContours(dst, contours, idx, color, cv::FILLED, 8, hierarchy);
		cv::drawContours(src, contours, idx, color, cv::FILLED, 8, hierarchy);

	}

	
}

// ***********************************

void Bound13(cv::Mat& input)
{
	//cv::Mat input = cv::imread("../inputData/RotatedRect.png");

	// convert to grayscale (you could load as grayscale instead)
	cv::Mat gray;
	cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);

	// compute mask (you could use a simple threshold if the image is always as good as the one you provided)
	cv::Mat mask;
	cv::threshold(gray, mask, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	// find contours (if always so easy to segment as your image, you could just add the black/rect pixels to a vector)
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	/// Draw contours and find biggest contour (if there are other contours in the image, we assume the biggest one is the desired rect)
	// drawing here is only for demonstration!
	int biggestContourIdx = -1;
	float biggestContourArea = 0;
	cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(0, 100, 0);
		drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());

		float ctArea = cv::contourArea(contours[i]);
		if (ctArea > biggestContourArea)
		{
			biggestContourArea = ctArea;
			biggestContourIdx = i;
		}
	}

	// if no contour found
	if (biggestContourIdx < 0)
	{
		//std::cout << "no contour found" << std::endl;
		return;
	}

	// compute the rotated bounding rect of the biggest contour! (this is the part that does what you want/need)
	cv::RotatedRect boundingBox = cv::minAreaRect(contours[biggestContourIdx]);
	// one thing to remark: this will compute the OUTER boundary box, so maybe you have to erode/dilate if you want something between the ragged lines



	// draw the rotated rect
	cv::Point2f corners[4];
	boundingBox.points(corners);
	/*cv::line(drawing, corners[0], corners[1], cv::Scalar(255, 255, 255));
	cv::line(drawing, corners[1], corners[2], cv::Scalar(255, 255, 255));
	cv::line(drawing, corners[2], corners[3], cv::Scalar(255, 255, 255));
	cv::line(drawing, corners[3], corners[0], cv::Scalar(255, 255, 255));*/

	cv::line(input, corners[0], corners[1], cv::Scalar(255, 255, 255));
	cv::line(input, corners[1], corners[2], cv::Scalar(255, 255, 255));
	cv::line(input, corners[2], corners[3], cv::Scalar(255, 255, 255));
	cv::line(input, corners[3], corners[0], cv::Scalar(255, 255, 255));

	

	// display
	//cv::imshow("input", input);
	//cv::imshow("drawing", drawing);
	//cv::waitKey(0);

	//cv::imwrite("rotatedRect.png", drawing);

	
}

//using namespace cv;									// class type, means you dont need to call every function including its full library info like "cv::inRange(" becomes "inRange("
//using namespace std;								//

int Bound14(cv::Mat& Original)    //for conversion between Video and PNG (int argc, char** argv)
{
	//cv::VideoCapture cap(0);		//VideoCapture is OpenCV function that uses webcam stream - cap(0) <-- zero being the first camera, 1 being 2nd camera if multiple
	//if (!cap.isOpened())		//if not cap opened
	//{
	//	cout << "Cannot Open Video Camera" << endl;		//disp error message
	//	cin.ignore();		//wait until key hit
	//	return -1;			//close program, return -1
	//}

	//for (;;)				//for loop, keeps the program running until the esc key break (at the end)
	{
		//cv::Mat Original;		//Declare Matrix, Original (ie. camera feed)
		//namedWindow("Original", 1);		//declare window
		//cap >> Original;		//put "cap" (captured images from webcam) into the Matrix for processing

		cv::Mat BW;		//Declare Matrix BW (to be used for the red filter)
		//namedWindow("Red Colour Detect, GreyScale", 1);

		cv::Mat Processing;		// (to be used with canny)
		//namedWindow("Processing", 1);




		// Convert input image to HSV
		//cv::Mat hsv_image;
		cv::cvtColor(Original, BW, cv::COLOR_BGR2HSV);

		// Threshold the HSV image, keep only the red pixels
		cv::Mat lower_red_hue_range;
		//cv::Mat upper_red_hue_range;
		//cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
		//cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);


		//Colour Filtration
		inRange(BW, cv::Scalar(0, 0, 125), cv::Scalar(80, 80, 256), lower_red_hue_range);		//Colour filtration		(source, low threshold, high threshold, Output)
		//Image Processing
		GaussianBlur(lower_red_hue_range, lower_red_hue_range, cv::Size(15, 15), 1.5, 1.5);		//Gaussian blur
		erode(lower_red_hue_range, lower_red_hue_range, cv::Mat(), cv::Point(-1, -1));				//Erosion
		dilate(lower_red_hue_range, lower_red_hue_range, cv::Mat(), cv::Point(-1, -1), 10, 1, 10);	//Dilation

		//Canny Edge Detection
		Canny(lower_red_hue_range, Processing, 0, 30, 3);

		//Vector of lines, followed by Houghlines
		vector<cv::Vec2f> lines;											//storage for the HoughLines
		HoughLines(Processing, lines, 1, CV_PI / 180, 100, 0, 0);		//Houghlines Function(Input, Storage(output), double rho (keep 1), double theta (keep pi/180), threshold value (pix), keep 0, keep 0)

		//printf("%d Lines Detected.\n", lines.size());			//No. lines detected

		if (lines.size() > 3)								//if the number of lines is greater than 3, continue processing, otherwise skip. This was put in because the program errored later due to loops having values i<array-1 if the array contained no elements
		{
			//Declaring variables
			cv::Point C;					//Centre point
			cv::Point P[500];				//P's of intersection, array arbitrary size 500
			double numer3, denom3, numer4;		//doubles, used later, for calcs of gradients
			cv::Point pt1[500], pt2[500];		//output hough lines end points
			double m1, m2, m3, m4, m5, m6, denom1, denom2, denom4, denom5;	//more doubles
			cv::Point f1[500], f2[500];			//two arrays, for confirmed perpendicular lines f1-->pt1  f2-->pt2
			int numlines = 0;        // Number of intersecting lines detected.


			// Show how many lines were detected by opencv

			//		printf("Lines Detected = %d\n", lines.size());			//uncomment if you want to check this variable


			for (size_t i = 0; i < lines.size(); i++)			//loops through each line
			{
				// Convert from Polar to Cartesian Co-ordinates.
				float rho = lines[i][0], theta = lines[i][1];					//
				double a = cos(theta), b = sin(theta);							//
				double x0 = a * rho, y0 = b * rho;									// This section does the conversion from lines array polar into cartesian end points of the lines, arbitrary length 1000, can be changed but not necessary
				pt1[i].x = cvRound(x0 + 1000 * (-b));							//
				pt1[i].y = cvRound(y0 + 1000 * (a));							//
				pt2[i].x = cvRound(x0 - 1000 * (-b));							//
				pt2[i].y = cvRound(y0 - 1000 * (a));							//
			}
			for (size_t i = 0; i < lines.size(); i++)
			{
				line(Original, pt1[i], pt2[i], cv::Scalar(255, 255, 0), 2, CV_AA);		//draws all detected lines in CYAN
			}


			for (size_t start = 0; start < lines.size() - 1; start++)			//
			{																	//
				for (size_t i = start + 1; i < lines.size(); i++)				// Double looping system for comparisons
				{
					denom1 = (pt2[start].x - pt1[start].x);						//Gradient comparison between the lines start and i
					denom2 = (pt2[i].x - pt1[i].x);								//
					if (denom1 == 0.0) { denom1 = 0.00001; }					//allows for complete comparison between all elements
					if (denom2 == 0.0) { denom2 = 0.00001; }					//
					m1 = (pt2[start].y - pt1[start].y) / denom1;				//
					m2 = (pt2[i].y - pt1[i].y) / denom2;						//
					m3 = -(m1 * m2);												//

					// Check if these two lines intersect at ~90 deg. If correct save into arrays f1 (pt1) and f2 (pt2). I.E. end points of the lines

					if (m3 > 0.1 && m3 < 4.0)			// Constraints for angles. Change for a more stringent 90 degrees (tend to 1) for less stringent, widen constraint
					{
						// If Lines intersect - increment the counter.
						f1[numlines] = pt1[start];    // And save the lines to our final output arrays.
						f2[numlines] = pt2[start];
						numlines++;                // Lines intersect - increment the counter.
						f1[numlines] = pt1[i];    // And save the lines to our final output arrays.
						f2[numlines] = pt2[i];
						numlines++;
					}
				}
			}



		//	printf("Numlines = %d\n", numlines);		//Checking value of numlines

			if (numlines > 3)														//
			{																		//
				for (size_t i = 0; i < numlines - 1; i++)							//
				{																	//
					for (size_t r = i + 1; r < numlines; r++)						// Removes duplicates from the array, makes things simpler later on LEAVE THIS IN!
					{																//
						if (f1[i].x == f1[r].x && f1[i].y == f1[r].y)				//
						{															//
							for (size_t m = r; m < numlines - 1; m++)				//
							{														//
								f1[m] = f1[m + 1];									//
								f2[m] = f2[m + 1];									//
							}
							numlines--;
							//				printf("Numlines is currently %d\n", numlines);				//uncomment if you want to check this variable
						}
					}
				}

				//	printf("Numlines = %d\n", numlines);
				//	for (size_t r = 0; r < numlines; r++)
				//	{
				//		printf("x[%d] = %5d  y[%d] = %5d\n", r, f1[r].x, r, f1[r].y);					//uncomment if you want to check this variable
				//	}


				// Section for finding intersection points

				for (size_t i = 0; i < numlines; i++)									//
				{																		//
					line(Original, f1[i], f2[i], cv::Scalar(0, 255, 0), 3, CV_AA);			// Display lines found from gradient comparison. Here it should only be the 4 (or maybe double detections) lines around the 
				}																		// rectangle.
				int counter = 0;														//
				for (size_t start = 0; start < numlines - 1; start++)					//
				{																		// Double loop system for comparisons, same as before
					for (size_t i = start + 1; i < numlines; i++)						//
					{																	//

						denom4 = (f2[start].x - f1[start].x);							//
						denom5 = (f2[i].x - f1[i].x);									//
						if (denom4 == 0.0) { denom4 = 0.00001; }						//
						if (denom5 == 0.0) { denom5 = 0.00001; }						// Finding the gradient between the f's, used as the order (horizontal, vertical) of the array may not be right for what we need to do 
						m4 = (f2[start].y - f1[start].y) / denom1;						// below. Hence check
						m5 = (f2[i].y - f1[i].y) / denom2;								//
						m6 = -(m4 * m5);													//
						if (m6 > 0.1 && m6 < 4.0)										// if between, use "start" and "i" 
						{
							numer3 = (((f1[start].x * f2[start].y) - (f1[start].y * f2[start].x)) * (f1[i].x - f2[i].x)) - (((f1[start].x - f2[start].x) * ((f1[i].x * f2[i].y) - (f1[i].y * f2[i].x))));
							numer4 = (((f1[start].x * f2[start].y) - (f1[start].y * f2[start].x)) * (f1[i].y - f2[i].y)) - (((f1[start].y - f2[start].y) * ((f1[i].x * f2[i].y) - (f1[i].y * f2[i].x))));
							denom3 = ((f1[start].x - f2[start].x) * (f1[i].y - f2[i].y)) - ((f1[start].y - f2[start].y) * (f1[i].x - f2[i].x));
							P[counter].x = (numer3 / denom3);
							P[counter].y = (numer4 / denom3);
							if ((P[counter].x > 0 && P[counter].x < 640) || (P[counter].y > 0 && P[counter].y < 480))			// if within the size of the camera feed (640x480)
							{
								counter++;												// increment counter
							}
						}
						else
						{																//else, use "start" and "i+1"
							numer3 = (((f1[start].x * f2[start].y) - (f1[start].y * f2[start].x)) * (f1[i + 1].x - f2[i + 1].x)) - (((f1[start].x - f2[start].x) * ((f1[i + 1].x * f2[i + 1].y) - (f1[i + 1].y * f2[i + 1].x))));
							numer4 = (((f1[start].x * f2[start].y) - (f1[start].y * f2[start].x)) * (f1[i + 1].y - f2[i + 1].y)) - (((f1[start].y - f2[start].y) * ((f1[i + 1].x * f2[i + 1].y) - (f1[i + 1].y * f2[i + 1].x))));
							denom3 = ((f1[start].x - f2[start].x) * (f1[i + 1].y - f2[i + 1].y)) - ((f1[start].y - f2[start].y) * (f1[i + 1].x - f2[i + 1].x));
							P[counter].x = (numer3 / denom3);
							P[counter].y = (numer4 / denom3);
							if ((P[counter].x > 0 && P[counter].x < 640) || (P[counter].y > 0 && P[counter].y < 480))			// if within the size of the camera feed (640x480)
							{
								counter++;												// increment counter
							}
						}

					}
				}

				//	printf("Final value for counter = %d\n", counter);							//uncomment if you want to check this variable

				for (size_t i = 0; i < counter - 1; i++)								//
				{																		//
					for (size_t r = i + 1; r < counter; r++)							//
					{																	// Remove duplicates from the P array as was done before with f1 and f2
						if (P[i].x == P[r].x && P[i].y == P[r].y)						// if P = next P
						{																//
							for (size_t m = r; m < counter - 1; m++)					//
							{															//
								P[m] = P[m + 1];										// Make P = next P
							}															//
							counter--;													// Reduce size of array by 1
							//printf("Counter (points) is now %d\n", counter);									//uncomment if you want to check this variable
						}
					}
				}





				for (size_t i = 0; i < counter; i++) {
					cv::circle(Original, P[i], 3, cvScalar(255, 0, 0), 3, 8, 0);			// Draws points of intersection
				}

				int xmax = -999999, xmin = 999999, ymax = -999999, ymin = 999999;		// Starting values
				for (size_t i = 0; i < counter; i++)
				{
					if (P[i].x < xmin) { xmin = P[i].x; }								// Finds smallest x
					if (P[i].x > xmax) { xmax = P[i].x; }
					if (P[i].y < ymin) { ymin = P[i].y; }
					if (P[i].y > ymax) { ymax = P[i].y; }								// Finds biggest y
				}
				//		printf("MinX=%d  MaxX=%d  MinY=%d  MaxY=%d\n", xmin, xmax, ymin, ymax);					//uncomment if you want to check this variable


				C.x = round(((xmax - xmin) / 2) + xmin);								// x value of C = average of xmax and xmin (plus xmin to place it in centre)
				C.y = round(((ymax - ymin) / 2) + ymin);								// y value of C = average of ymax and ymin (plus ymin to place it in centre)

				circle(Original, C, 10, cvScalar(0, 0, 0), 3, 8, 0);					// Disp C on Original. Circle, size 10, thickness 3

				printf("Centre X =%d    Centre y =%d\n", C.x, C.y);						// Centrepoints out, x and y



			}
		}
		//_sleep(250);																	// provides a 0.25s wait betweeen loops. Comment out for full speed *I used this to help see what was going on in real time*
		//imshow("Original", Original);
		//imshow("Processing", Processing);												// shows the windows
		//imshow("Red Colour Detect, GreyScale", BW);
		//		for (size_t r = 0; r < counter; r++)
		//		{																		// uncomment for P point data
		//			printf("P.x[%d] = %d  P.y[%d] = %d\n", r, P[r].x, r, P[r].y);
		//		}

		//if (waitKey(30) >= 1) break;													// If ESC key hit, break from program
	}
	return 0;																			//return output 0

}

void Bound17(cv::Mat& input)
{
	//cv::Mat input = cv::imread("../inputData/RotatedRect.png");

	// convert to grayscale (you could load as grayscale instead)
	cv::Mat gray;
	cv::cvtColor(input, gray, CV_BGR2GRAY);

	// compute mask (you could use a simple threshold if the image is always as good as the one you provided)
	cv::Mat mask;
	cv::threshold(gray, mask, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	// find contours (if always so easy to segment as your image, you could just add the black/rect pixels to a vector)
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	/// Draw contours and find biggest contour (if there are other contours in the image, we assume the biggest one is the desired rect)
	// drawing here is only for demonstration!
	int biggestContourIdx = -1;
	float biggestContourArea = 0;
	cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(0, 100, 0);
		drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());

		float ctArea = cv::contourArea(contours[i]);
		if (ctArea > biggestContourArea)
		{
			biggestContourArea = ctArea;
			biggestContourIdx = i;
		}
	}

	// if no contour found
	if (biggestContourIdx < 0)
	{
		//std::cout << "no contour found" << std::endl;
		return;
	}

	// compute the rotated bounding rect of the biggest contour! (this is the part that does what you want/need)
	cv::RotatedRect boundingBox = cv::minAreaRect(contours[biggestContourIdx]);
	// one thing to remark: this will compute the OUTER boundary box, so maybe you have to erode/dilate if you want something between the ragged lines



	// draw the rotated rect
	cv::Point2f corners[4];
	boundingBox.points(corners);
	cv::line(input, corners[0], corners[1], cv::Scalar(255, 0, 255));
	cv::line(input, corners[1], corners[2], cv::Scalar(255, 0, 255));
	cv::line(input, corners[2], corners[3], cv::Scalar(255, 0, 255));
	cv::line(input, corners[3], corners[0], cv::Scalar(255, 0, 255));

	

}

//******************************************

#define IMAGE_HW 320
#define IMAGE_VW 240
// 1MB of NC_BSS is not used for malloc function.
// It's better to secure buffer as a static memory in this NC_BSS
uint8_t bgr_buf[3 * IMAGE_HW * IMAGE_VW];// __attribute((section("NC_BSS"), aligned(32)));
uint8_t hsv_buf[3 * IMAGE_HW * IMAGE_VW];// __attribute((section("NC_BSS"), aligned(32)));
uint8_t syn_buf[3 * IMAGE_HW * IMAGE_VW];// __attribute((section("NC_BSS"), aligned(32)));

//cv::Camera camera(IMAGE_HW, IMAGE_VW);
//cv::DisplayApp display_app;
bool g_mode = false;

void setup() {
	//camera.begin();
	//delay(100);
}

void loop(cv::Mat& img_raw) {
	/*static unsigned long last_time = 0;
	if ((millis() - last_time) > 2000) {
		g_mode = !g_mode;
		last_time = millis();
	}*/

	//cv::Mat img_raw(IMAGE_VW, IMAGE_HW, CV_8UC2, camera.getImageAdr());
	cv::Mat img_bgr = img_raw.clone();// (IMAGE_VW, IMAGE_HW, CV_8UC3, bgr_buf);
	cv::cvtColor(img_bgr, img_bgr, cv::COLOR_YUV2BGR_YUYV); // copy camera image to img_bgr

	cv::Mat img_hsv(IMAGE_VW, IMAGE_HW, CV_8UC3, hsv_buf);
	cvtColor(img_bgr, img_hsv, cv::COLOR_BGR2HSV);

	cv::Mat img_temp, img_mask;
	inRange(img_hsv, cv::Scalar(0, 50, 50), cv::Scalar(20, 255, 255), img_temp);
	inRange(img_hsv, cv::Scalar(160, 50, 50), cv::Scalar(180, 255, 255), img_mask);
	img_mask = img_mask + img_temp;

	//    medianBlur(img_mask, img_mask, 3); // cut noise
	GaussianBlur(img_mask, img_mask, cv::Size(27, 27), 3.5, 3.5);
	threshold(img_mask, img_mask, 140, 255, 0);

	// mask processing for drawing a picture
	cv::Mat img_synthesis(IMAGE_VW, IMAGE_HW, CV_8UC3, syn_buf);
	img_synthesis = cv::Scalar(0, 0, 0);

	// find contours
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	findContours(img_mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);


	int fingerCount = 0;
	if (contours.size() > 0) {
		size_t indexOfBiggestContour = -1;
		size_t sizeOfBiggestContour = 0;
		for (size_t i = 0; i < contours.size(); i++) {
			if (contours[i].size() > sizeOfBiggestContour) {
				sizeOfBiggestContour = contours[i].size();
				indexOfBiggestContour = i;
			}
		}
		vector<vector<int> >hull(contours.size());
		vector<vector<cv::Point> >hullPoint(contours.size()); //elin hareketine göre eli çevreleyen çokgen
		vector<vector<cv::Vec4i> > defects(contours.size()); //parmak uclarindaki yesil noktalar..multi dimensional matrix
		vector<vector<cv::Point> >defectPoint(contours.size()); //point olarak parmak ucu noktalarýný x,y olarak tutuyor
		vector<vector<cv::Point> >contours_poly(contours.size()); //eli çevreleyen hareketli dikdörtgen
		cv::Point2f rect_point[4];
		vector<cv::RotatedRect>minRect(contours.size());
		vector<cv::Rect> boundRect(contours.size());
		for (size_t i = 0; i < contours.size(); i++) {
			if (contourArea(contours[i]) > 5000) {
				convexHull(contours[i], hull[i], true);
				convexityDefects(contours[i], hull[i], defects[i]);
				if (indexOfBiggestContour == i) {
					minRect[i] = minAreaRect(contours[i]);
					for (size_t k = 0; k < hull[i].size(); k++) {
						int ind = hull[i][k];
						hullPoint[i].push_back(contours[i][ind]);
					}
					for (size_t k = 0; k < defects[i].size(); k++) {
						if (defects[i][k][3] > 13 * 256) {
							int p_start = defects[i][k][0];
							int p_end = defects[i][k][1];
							int p_far = defects[i][k][2];
							defectPoint[i].push_back(contours[i][p_far]);
							circle(img_synthesis, contours[i][p_end], 3, cv::Scalar(0, 255, 0), 2); //i ydi
							fingerCount++;
						}

					}

					drawContours(img_synthesis, contours, i, cv::Scalar(255, 255, 0), 2, 8, vector<cv::Vec4i>(), 0, cv::Point());
					drawContours(img_synthesis, hullPoint, i, cv::Scalar(255, 255, 0), 1, 8, vector<cv::Vec4i>(), 0, cv::Point());
					drawContours(img_mask, hullPoint, i, cv::Scalar(0, 0, 255), 2, 8, vector<cv::Vec4i>(), 0, cv::Point());
					approxPolyDP(contours[i], contours_poly[i], 3, false);
					boundRect[i] = boundingRect(contours_poly[i]);
					rectangle(img_raw, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(255, 0, 0), 2, 8, 0);
					minRect[i].points(rect_point);
					for (size_t k = 0; k < 4; k++) {
						line(img_raw, rect_point[k], rect_point[(k + 1) % 4], cv::Scalar(0, 255, 0), 2, 8);
					}
				}
			}
		}
	}

	if (g_mode) { img_mask = cv::Scalar(255); }// for showing original image
	img_bgr.copyTo(img_synthesis, img_mask);

	// draw text
	stringstream ss;
	ss << "Finger Count: " << fingerCount;
	putText(img_synthesis, ss.str(), cv::Point(5, img_synthesis.rows - 8),
		cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

	// display image
	//size_t jpegSize = camera.createJpeg(IMAGE_HW, IMAGE_VW, img_synthesis.data, Camera::FORMAT_RGB888);
	//display_app.SendJpeg(camera.getJpegAdr(), jpegSize);

}

//**********************************************************


enum COLOR { RED, BLUE };
//using namespace cv;
//using namespace std;

void showImgContours(cv::Mat&, cv::Mat&, COLOR);

void Bound18(cv::Mat& imgOriginal)
{
	//VideoCapture cap("D:\\비주얼2015\\video1.MOV"); //capture the video from web cam
	////VideoCapture cap(0);

	//if (!cap.isOpened())
	//{
	//	cout << "Can not open web cam" << endl;
	//	return 0;
	//}

	//get video's size

	cv::Size size = cv::Size((int)480, (int)360);

	//cout << "Size = " << size << endl;

	//int  fourcc = VideoWriter::fourcc('x', 'v', 'i', 'd');

	double fps = 35;
	int lowSaturation = 20;
	int lowValue = 60;

	//prepare to save

	//VideoWriter outputVideo("output.avi", fourcc, fps, size, true);



	//if (!outputVideo.isOpened())

	//{

	//	cout << " Can not open outputVideo !!!" << endl;

	//	return 0;

	//}



	//while (true)

	{
		//cv::Mat imgOriginal;
		//cap >> imgOriginal;

		flip(imgOriginal, imgOriginal, 1);

		if (imgOriginal.empty()) return; //break;

		cv::Mat imgHSV;
		cv::Mat imgResult;
		cv::Mat red_part1;
		cv::Mat red_part2;
		cv::Mat redThresholded;
		cv::Mat blueThresholded;

		imgOriginal.copyTo(imgHSV);
		imgOriginal.copyTo(imgResult);


		//vector<Mat> channels(imgHSV.channels());

		//split(imgHSV, channels);

		//equalizeHist(channels[0], channels[0]);

		//equalizeHist(channels[1], channels[1]);

		//equalizeHist(channels[2], channels[2]);

		//merge(channels, imgHSV);



		//GaussianBlur(imgHSV, imgHSV, Size(33, 33), 0);



		cvtColor(imgHSV, imgHSV, cv::COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		//------thresholding-----

		//red

		cv::inRange(imgHSV, cv::Scalar(0, 25, 78, 0), cv::Scalar(12, 248, 255, 0), red_part1);

		cv::inRange(imgHSV, cv::Scalar(154, 25, 78, 0), cv::Scalar(180, 248, 255, 0), red_part2);

		redThresholded = red_part1 | red_part2;

		//redThresholded =  red_part2;

		//blue

		//cv::inRange(imgHSV, cv::Scalar(100, lowSaturation, lowValue, 0), cv::Scalar(140, 255, 255, 0), blueThresholded);





		//------contouring------

		cv::Mat contoursResult = cv::Mat::zeros(imgResult.size(), imgResult.type());

		showImgContours(redThresholded, contoursResult, RED);

		//showImgContours(blueThresholded, contoursResult, BLUE);


		//------morphology------

		//morphological opening (remove small objects from the foreground)

		//erode(contoursResult, contoursResult, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));

		//dilate(contoursResult, contoursResult, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));



		////morphological closing (fill small holes in the foreground)

		//dilate(contoursResult, contoursResult, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));

		//erode(contoursResult, contoursResult, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));


		//------merge------

		for (int row = 0; row < contoursResult.rows; row++) {

			for (int col = 0; col < contoursResult.cols; col++) {


				for (int channel = 0; channel < 3; channel++) {

					if (contoursResult.at<cv::Vec3b>(row, col)[channel] != 0)

						imgResult.at<cv::Vec3b>(row, col)[channel] = contoursResult.at<cv::Vec3b>(row, col)[channel];

				}

			}

		}




		//outputVideo << imgResult;      // save the result image

		//outputVideo << imgOriginal;



		resize(imgResult, imgResult, cv::Size(720, 480));

		//cv::imshow("Result", imgResult);   // show the result image

		//imshow("Original", imgOriginal);// show the original image
        
		//int ckey = waitKey(1000 / fps);      // wait for ESC key

		//30이면 0.03초당 한번찍음

		//200이면 0.2초당 한번찍음

		//if (ckey == 27) break;

	}

	return;



}



void showImgContours(cv::Mat& threshedimg, cv::Mat& result, COLOR color)

{
	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	int largest_area = 0;
	int largest_contour_index = 0;
	findContours(threshedimg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	vector<vector<cv::Point> > contours_poly(contours.size());
	vector<cv::Rect> boundRect(contours.size());

	//contours 0: smallest, contours max: biggest
	sort(contours.begin(), contours.end(), [](
		const vector<cv::Point>& c1, 
		const vector<cv::Point>& c2)
		{
			return contourArea(c1, false) < contourArea(c2, false);
		});

	vector<vector<cv::Point>> top5_contours;

	for (int i = 1; i < 6; i++) {
		if (contours.size() > i)
		{
			top5_contours.push_back(contours[contours.size() - i]);
		}
	}

	for (int i = 0; i < top5_contours.size(); i++)
	{
		approxPolyDP(cv::Mat(top5_contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(cv::Mat(contours_poly[i]));
	}

	/// Draw polygonal contour + bonding rects + circles

	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = CV_RGB(255, 0, 0);
		rectangle(result, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}



	/*

	for (int i = 0; i< contours.size(); i++) // iterate through each contour.

	{

	double a = contourArea(contours[i], false);  //  Find the area of contour

	if (a>largest_area)

	{

	largest_area = a;

	largest_contour_index = i;                //Store the index of largest contour

	}

	}

	*/

	if (contours.size() > 0)
	{
		switch (color) {
		case RED:
			drawContours(result, contours, -1, CV_RGB(255, 0, 0), 2, 8, hierarchy);
			break;
		case BLUE:
			drawContours(result, contours, -1, CV_RGB(0, 0, 255), 2, 8, hierarchy);
			break;
		}
	}
}

// ************************************************************

int thresh_ = 50, N = 5;
const char* wndname = "Square Detection Demo";

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle_(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares(const cv::Mat& image, vector<vector<cv::Point> >& squares)
{
	squares.clear();

	//s    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

		// down-scale and upscale the image to filter out the noise
		//pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
		//pyrUp(pyr, timg, image.size());


		// blur will enhance edge detection
	cv::Mat timg(image);
	cv::medianBlur(image, timg, 9);
	cv::Mat gray0(timg.size(), CV_8U), gray;

	vector<vector<cv::Point> > contours;

	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		for (int l = 0; l < N; l++)
		{
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				Canny(gray0, gray, 5, thresh_, 5);
				// dilate canny output to remove potential
				// holes between edge segments
				dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				gray = gray0 >= (l + 1) * 255 / N;
			}

			// find contours and store them all as a list
			findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

			vector<cv::Point> approx;

			// test each contour
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
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
						// find the maximum cosine of the angle between joint edges
						double cosine = fabs(angle_(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					// if cosines of all angles are small
					// (all angles are ~90 degree) then write quandrange
					// vertices to resultant sequence
					if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}


// the function draws all the squares in the image
void drawSquares(cv::Mat& image, const vector<vector<cv::Point> >& squares)
{
	for (size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];

		int n = (int)squares[i].size();
		//dont detect the border
		if (p->x > 3 && p->y > 3)
			polylines(image, &p, &n, 1, true, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
	}

	//imshow(wndname, image);
}

void Bound19(cv::Mat& image)
{
	static const char* names[] = { "imgs/2Stickies.jpg", "imgs/manyStickies.jpg",0 };
	//help();
	//namedWindow(wndname, 1);
	vector<vector<cv::Point> > squares;

	//for (int i = 0; names[i] != 0; i++)
	{
		//Mat image = imread(names[i], 1);
		if (image.empty())
		{
			//cout << "Couldn't load " << names[i] << endl;
			//continue;
			return;
		}

		findSquares(image, squares);
		drawSquares(image, squares);
		//imwrite( "out", image );
		//int c = waitKey();
		//if ((char)c == 27) break;
	}

	return;
}
