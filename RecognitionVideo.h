#pragma once

#include "stdafx.h"
#include "jsonlib.h"


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
	if (radius > 30 && radius < 40 && 40 < sqrt(r) && sqrt(r) < 100) return true;

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
			InsertJsonRect(rect, numCadr);
			cv::rectangle(src_mat, rect, cv::Scalar(0, 0, 255), 2);
		}
		//cv::polylines(src_mat, polylines, true, cv::Scalar(0, 255, 0), 2);		
		if (r_center(radius, center.x, center.y, squares))
		{
			InsertJsonCircle(radius, center, numCadr);
			cv::circle(src_mat, center, radius, cv::Scalar(255, 0, 0), 2);
		}

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
	if (label != "CIR")return;

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

