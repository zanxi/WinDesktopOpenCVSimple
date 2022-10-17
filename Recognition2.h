#pragma once

#include "stdafx.h"
#include "jsonlib.h"


using namespace std;
using namespace cv;
//using namespace	cv::videostab;

Mat horizon_cont_original(Mat img)
{
	resize(img, img, Size(640, 360));

	Mat lowhalfremove = Mat::zeros(img.size(), CV_8UC1);
	lowhalfremove(Rect(0, 0, img.cols, img.rows / 2)) = 1;

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
	Mat diamond = Mat(5, 5, CV_8U, Scalar(1));
	diamond.at<uchar>(0, 0) = 0;
	diamond.at<uchar>(0, 1) = 0;
	diamond.at<uchar>(1, 0) = 0;
	diamond.at<uchar>(4, 4) = 0;
	diamond.at<uchar>(3, 4) = 0;
	diamond.at<uchar>(4, 3) = 0;
	diamond.at<uchar>(4, 0) = 0;
	diamond.at<uchar>(4, 1) = 0;
	diamond.at<uchar>(3, 0) = 0;
	diamond.at<uchar>(0, 4) = 0;
	diamond.at<uchar>(0, 3) = 0;
	diamond.at<uchar>(1, 4) = 0;

	vector<Mat> imgl(3);

	Mat imgopen, imgclos;
	morphologyEx(img, imgopen, MORPH_OPEN, element);
	morphologyEx(imgopen, imgclos, MORPH_CLOSE, element);

	Mat temefr1, temefr2, tframe;
	dilate(imgclos, temefr1, diamond);
	morphologyEx(imgclos, temefr2, MORPH_CLOSE, diamond);

	tframe = temefr1 - temefr2;

	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);

	vector<Mat> imm(3);
	split(tframe, imm);

	multiply(imm[0], lowhalfremove, imm[0]);				// Assumption:: Horizon wont exist below the vertical midpoint of the image
	multiply(imm[1], lowhalfremove, imm[1]);				// Hence lower half removed
	multiply(imm[2], lowhalfremove, imm[2]);

	Mat t1, t2, t3;
	threshold(imm[0], t1, 11, 255, THRESH_BINARY | THRESH_OTSU);
	threshold(imm[1], t2, 12, 255, THRESH_BINARY | THRESH_OTSU);
	threshold(imm[2], t3, 13, 255, THRESH_BINARY | THRESH_OTSU);

	bitwise_and(t1, t2, t2);
	bitwise_and(t2, t3, t3);

	morphologyEx(t3, t3, MORPH_OPEN, Mat());
	morphologyEx(t3, t3, MORPH_CLOSE, Mat());

	dilate(t3, t3, Mat());
	dilate(t3, t3, Mat());
	dilate(t3, t3, Mat());

	erode(t3, t3, Mat());
	erode(t3, t3, Mat());
	erode(t3, t3, Mat());

	Mat Mask = Mat::zeros(t3.size(), CV_8UC1);
	vector<vector<Point> > newcontours;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(t3, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	int kdivision = 6;
	int* maxx = NULL, * maxy = NULL;
	maxx = new int[kdivision];
	maxy = new int[kdivision];

	for (int i = 0; i < kdivision; i++) { maxx[i] = 0; maxy[i] = 0; }

	for (int k = 0; k < kdivision; k++)
	{

		int A = 0, B = 0;
		A = (t3.cols / kdivision) * (k);
		B = (t3.cols / kdivision) * (k + 1);


		for (int i = 0; i < contours.size(); i++)
		{
			for (int j = 0; j < contours[i].size(); j++)
			{
				/*
				if( ( contours[i][j].x < t3.cols / 2)&&(contours[i][j].y < t3.rows / 2) )		// For accessing only the first quadrant
				{
					if(contours[i][j].y > maxy1)
					{
					maxy1 = contours[i][j].y;
					maxx1 = contours[i][j].x;
					}

				}

				if( (contours[i][j].x > t3.cols / 2)&&( contours[i][j].y < t3.rows / 2) )		// For accessing only the second quadrant
				{

					if(contours[i][j].y > maxy2)
					{
					maxy2 = contours[i][j].y;
					maxx2 = contours[i][j].x;
					}

				}
			*/

				if ((contours[i][j].x > A) && (contours[i][j].x < B) && (contours[i][j].y < t3.rows / 2))
				{
					if (contours[i][j].y > maxy[k])
					{
						maxx[k] = contours[i][j].x;
						maxy[k] = contours[i][j].y;
					}
				}


			}
		}


	}
	/*	for(int i = 0; i< contours.size();i++)
	{
	vector<Point> tmppoint;

		for(int j = 0; j< contours[i].size();j++)
		{

			if( ( contours[i][j].x > maxx1 )&&(contours[i][j].x < maxx2) )		// For accessing only the first quadrant
			{
				tmppoint.push_back(contours[i][j]);

			}


		}



		newcontours.push_back(tmppoint);

	} */

	// For finding contours
	vector<Point> tmp;
	tmp.push_back(Point(t3.cols - 1, 0));
	tmp.push_back(Point(0, 0));
	tmp.push_back(Point(0, maxy[0]));
	tmp.push_back(Point(maxx[0], maxy[0]));
	for (int u = 1; u < kdivision; u++)
	{
		//	line(Mask,Point(maxx[u-1],maxy[u-1]),Point(maxx[u],maxy[u]),Scalar(255),1,8,0);
		tmp.push_back(Point(maxx[u], maxy[u]));
	}
	tmp.push_back(Point(t3.cols - 1, maxy[kdivision - 1]));

	//	line(Mask,Point(0,maxy[0]),Point(maxx[0],maxy[0]),Scalar(255),1,8,0);
	//	line(Mask,Point(t3.cols - 1,maxy[kdivision - 1]),Point(maxx[kdivision - 1],maxy[kdivision - 1]),Scalar(255),1,8,0);

	newcontours.push_back(tmp);
	drawContours(Mask, newcontours, -1, Scalar::all(255), CV_FILLED, 8, vector<Vec4i>(), 0, Point());
	bitwise_not(Mask, Mask);

	vector<Mat> imggg;
	split(img, imggg);
	bitwise_and(imggg[0], Mask, imggg[0]);
	bitwise_and(imggg[1], Mask, imggg[1]);
	bitwise_and(imggg[2], Mask, imggg[2]);
	merge(imggg, img);

	delete[] maxx;
	delete[] maxy;

	return img;

}

int ncount(int A, int B, vector< vector<Point> > vect, int R)
{
	int count = 0;

	for (int i = 0; i < vect.size(); i++)
	{
		for (int j = 0; j < vect[i].size(); j++)
		{

			if ((vect[i][j].x > A) && (vect[i][j].x < B) && (vect[i][j].y < (R / 2)))
			{
				count++;
			}

		}
	}

	return count;
}

Mat horizon_cont(Mat img, int kdivision = 6)			// New and improved horizon cont
{
	resize(img, img, Size(640, 360));

	Mat lowhalfremove = Mat::zeros(img.size(), CV_8UC1);
	lowhalfremove(Rect(0, 0, img.cols, img.rows / 2)) = 1;

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
	Mat diamond = Mat(5, 5, CV_8U, Scalar(1));
	diamond.at<uchar>(0, 0) = 0;
	diamond.at<uchar>(0, 1) = 0;
	diamond.at<uchar>(1, 0) = 0;
	diamond.at<uchar>(4, 4) = 0;
	diamond.at<uchar>(3, 4) = 0;
	diamond.at<uchar>(4, 3) = 0;
	diamond.at<uchar>(4, 0) = 0;
	diamond.at<uchar>(4, 1) = 0;
	diamond.at<uchar>(3, 0) = 0;
	diamond.at<uchar>(0, 4) = 0;
	diamond.at<uchar>(0, 3) = 0;
	diamond.at<uchar>(1, 4) = 0;

	vector<Mat> imgl(3);

	Mat imgopen, imgclos;
	morphologyEx(img, imgopen, MORPH_OPEN, element);
	morphologyEx(imgopen, imgclos, MORPH_CLOSE, element);

	Mat temefr1, temefr2, tframe;
	dilate(imgclos, temefr1, diamond);
	morphologyEx(imgclos, temefr2, MORPH_CLOSE, diamond);

	tframe = temefr1 - temefr2;

	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);
	dilate(tframe, tframe, diamond);

	vector<Mat> imm(3);
	split(tframe, imm);

	multiply(imm[0], lowhalfremove, imm[0]);				// Assumption:: Horizon wont exist below the vertical midpoint of the image
	multiply(imm[1], lowhalfremove, imm[1]);				// Hence lower half removed
	multiply(imm[2], lowhalfremove, imm[2]);

	Mat t1, t2, t3;
	threshold(imm[0], t1, 11, 255, THRESH_BINARY | THRESH_OTSU);
	threshold(imm[1], t2, 12, 255, THRESH_BINARY | THRESH_OTSU);
	threshold(imm[2], t3, 13, 255, THRESH_BINARY | THRESH_OTSU);

	bitwise_and(t1, t2, t2);
	bitwise_and(t2, t3, t3);

	morphologyEx(t3, t3, MORPH_OPEN, Mat());
	morphologyEx(t3, t3, MORPH_CLOSE, Mat());

	dilate(t3, t3, Mat());
	dilate(t3, t3, Mat());
	dilate(t3, t3, Mat());

	erode(t3, t3, Mat());
	erode(t3, t3, Mat());
	erode(t3, t3, Mat());

	Mat Mask = Mat::zeros(t3.size(), CV_8UC1);
	vector<vector<Point> > newcontours;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(t3, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	int* maxx = NULL, * maxy = NULL;
	maxx = new int[kdivision];
	maxy = new int[kdivision];

	for (int i = 0; i < kdivision; i++) { maxx[i] = 0; maxy[i] = 0; }

	for (int k = 0; k < kdivision; k++)
	{

		int A = 0, B = 0;
		A = (t3.cols / kdivision) * (k);
		B = (t3.cols / kdivision) * (k + 1);


		if (ncount(A, B, contours, t3.rows))						// If there are points present between A and B process normally
		{
			for (int i = 0; i < contours.size(); i++)
			{
				for (int j = 0; j < contours[i].size(); j++)
				{
					/*
					if( ( contours[i][j].x < t3.cols / 2)&&(contours[i][j].y < t3.rows / 2) )		// For accessing only the first quadrant
					{
						if(contours[i][j].y > maxy1)
						{
						maxy1 = contours[i][j].y;
						maxx1 = contours[i][j].x;
						}

					}

					if( (contours[i][j].x > t3.cols / 2)&&( contours[i][j].y < t3.rows / 2) )		// For accessing only the second quadrant
					{

						if(contours[i][j].y > maxy2)
						{
						maxy2 = contours[i][j].y;
						maxx2 = contours[i][j].x;
						}

					}
				*/


					if ((contours[i][j].x > A) && (contours[i][j].x < B) && (contours[i][j].y < t3.rows / 2))
					{
						if (contours[i][j].y > maxy[k])
						{
							maxx[k] = contours[i][j].x;
							maxy[k] = contours[i][j].y;
						}
					}


				}
			}

		}

		else
		{
			Point nearestpt = contours[0][0];
			int minval = std::pow(std::pow(contours[0][0].x - A, 2), 0.5);		// can replace by std::pow(std::pow(X,2),0.5);
			int miny = nearestpt.y;

			for (int pp = 0; pp < contours.size(); pp++)				// contours may have only one value :: WARNING
			{
				for (int l = 0; l < contours[pp].size(); l++)
				{
					int diff = std::pow(std::pow(contours[pp][l].x - A, 2), 0.5);
					int ytmp = contours[pp][l].y;

					if ((minval > diff) && (ytmp > miny) && (contours[pp][l].y < (t3.rows / 2)))
					{
						minval = diff;
						nearestpt = contours[pp][l];
						miny = ytmp;
					}

				}
			}			// have nearest point closest to A via the X coordinate 
			maxx[k] = A;
			maxy[k] = nearestpt.y;

		}
	}
	/*	for(int i = 0; i< contours.size();i++)
	{
	vector<Point> tmppoint;

		for(int j = 0; j< contours[i].size();j++)
		{

			if( ( contours[i][j].x > maxx1 )&&(contours[i][j].x < maxx2) )		// For accessing only the first quadrant
			{
				tmppoint.push_back(contours[i][j]);

			}


		}



		newcontours.push_back(tmppoint);

	} */

	// For finding contours
	vector<Point> tmp;
	tmp.push_back(Point(t3.cols - 1, 0));
	tmp.push_back(Point(0, 0));
	tmp.push_back(Point(0, maxy[0]));
	tmp.push_back(Point(maxx[0], maxy[0]));

	for (int u = 1; u < kdivision; u++)
	{
		//	line(Mask,Point(maxx[u-1],maxy[u-1]),Point(maxx[u],maxy[u]),Scalar(255),1,8,0);
		tmp.push_back(Point(maxx[u], maxy[u]));
	}
	tmp.push_back(Point(t3.cols - 1, maxy[kdivision - 1]));

	//	line(Mask,Point(0,maxy[0]),Point(maxx[0],maxy[0]),Scalar(255),1,8,0);
	//	line(Mask,Point(t3.cols - 1,maxy[kdivision - 1]),Point(maxx[kdivision - 1],maxy[kdivision - 1]),Scalar(255),1,8,0);

	newcontours.push_back(tmp);
	drawContours(Mask, newcontours, -1, Scalar::all(255), CV_FILLED, 8, vector<Vec4i>(), 0, Point());
	bitwise_not(Mask, Mask);

	vector<Mat> imggg;
	split(img, imggg);
	bitwise_and(imggg[0], Mask, imggg[0]);
	bitwise_and(imggg[1], Mask, imggg[1]);
	bitwise_and(imggg[2], Mask, imggg[2]);
	merge(imggg, img);

	delete[] maxx;
	delete[] maxy;

	return img;

}

void Bound26(Mat& img)
{
	/*
	Mat img = imread("frame71.jpeg");
	Mat horri = horizon_cont(img);
	imshow("Horizon Made",horri);
	waitKey();
	*/

	//Mat img;
	VideoCapture cap;
	//cap.open("dolv.mp4");

	VideoWriter wir;
	//int codec = cv::FOURCC('M', 'J', 'P', 'G');
	//wir.open("dolv(1)_horizon_removed.avi", codec, cap.get(CV_CAP_PROP_FPS), Size(640, 360));

	//while (cap.get(CV_CAP_PROP_POS_FRAMES) < cap.get(CV_CAP_PROP_FRAME_COUNT))
	{

		//cap >> img;
		Mat horri = horizon_cont(img);
		wir << horri;
	}

	//cout << "\n Done!!!\n";
	return;
}

//**************************************************

Mat histogram_equalization(Mat src)
{
	Mat dst;
	equalizeHist(src, dst);

	return dst;
}

/**
Remove noise by using gaussian blur
*/
Mat gaussian_blur(Mat src, int kernel_size)
{
	Mat dst;
	GaussianBlur(src, dst, Size(kernel_size, kernel_size), 0, 0, BORDER_DEFAULT);

	return dst;
}

/**
Discover the median value in the list
*/
int discover_median(vector<int> list)
{
	sort(list.begin(), list.end());

	return list[list.size() / 2];
}


/**
Using morphology to remove characters
*/
Mat dilation(Mat src, pair<int, int> shape)
{
	Mat dst;

	Mat element = cv::getStructuringElement(MORPH_RECT, Size(2 * shape.first + 1, 2 * shape.second + 1),
		Point(shape.first, shape.second));
	dilate(src, dst, element);

	return dst;
}


/**
Finding contours in the image and get all bounding boxes
Choose the shape of element structure by calculate median value of shape in all bounding boxes
*/
pair<int, int> get_element_structure_shape(Mat src, int min_thresh, int max_thresh)
{
	// Convert the grayscale image to the binary image by using canny edge detection algorithm
	Mat canny_output;
	Canny(src, canny_output, min_thresh, max_thresh);

	// Find contours and store array of contours
	vector<vector<Point> > contours;
	findContours(canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

	// Approximate a polygonal curve and draw a retangle bounding box around this curve
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], contours_poly[i], 3, true);
		boundRect[i] = boundingRect(contours_poly[i]);
	}

	// Analyze statistically size of each bounding box
	vector<int> height;
	vector<int> width;

	for (size_t i = 0; i < boundRect.size(); ++i)
	{
		height.push_back(boundRect[i].height);
		width.push_back(boundRect[i].width);
	}

	pair<int, int> median;
	median.first = discover_median(height) / 2 + 1;
	median.second = discover_median(width) / 2 + 1;

	return median;
}

/**
Detect lines in image by using canny edge algorithm and hough line transform
*/
vector<Point> hough_line_transform(Mat src, Mat draw)
{
	Mat dst;

	Canny(src, dst, 150, 180);

	vector<Vec2f> lines;
	vector<pair<Vec2f, Vec2f>> parallels;
	int thresh = 40;
	double epsilon = 0.0001;

	HoughLines(dst, lines, 1, CV_PI * 4 / 180, thresh, 0, 0);

	// Get relevant lines from lines
	for (size_t i = 0; i < lines.size(); ++i)
	{
		for (size_t j = 0; j < lines.size(); ++j)
		{
			if (abs(lines[i][1] - lines[j][1]) > 1.5 && abs(lines[i][1] - lines[j][1]) < 1.65)
				break;
			if (j == lines.size() - 1)
			{
				lines.erase(lines.begin() + i);
				i--;
			}
		}
	}

	for (size_t i = 0; i < lines.size(); ++i)
	{
		for (size_t j = 0; j < lines.size(); ++j)
		{
			if ((abs(lines[i][1] - lines[j][1]) > 3.1 || abs(lines[i][1] - lines[j][1]) < 0.1) &&
				abs(lines[i][0] - lines[j][0]) > 100)
			{
				parallels.push_back(pair<Vec2f, Vec2f>(lines[i], lines[j]));
				break;
			}
			if (j == lines.size() - 1)
			{
				lines.erase(lines.begin() + i);
				i--;
			}
		}
	}

	// Get 4 edges
	vector<Vec2f> edges;
	bool break_flag = false;
	for (size_t i = 0; i < parallels.size(); ++i)
	{
		for (size_t j = i + 1; j < parallels.size(); ++j)
		{
			if (abs(parallels[i].first[1] - parallels[j].first[1]) < 1.65 &&
				abs(parallels[i].first[1] - parallels[j].first[1]) > 1.5)
			{
				edges.push_back(parallels[i].first);
				edges.push_back(parallels[j].first);
				edges.push_back(parallels[i].second);
				edges.push_back(parallels[j].second);
				break_flag = true;
				break;
			}
		}
		if (break_flag == true) break;
	}
	cout << "Edge size: " << edges.size() << endl;


	// Get all intersections in the list of lines
	vector<Point> intersections;
	for (size_t i = 0; i < edges.size(); ++i)
	{
		for (size_t j = i + 1; j < edges.size(); ++j)
		{
			/*
			x * cos(theta1) + y * sin(theta1) = d1
			x * cos(theta2) + y * sin(theta2) = d2
			*/
			float theta1 = edges[i][1];
			float d1 = edges[i][0];
			float theta2 = edges[j][1];
			float d2 = edges[j][0];

			float det3 = cos(theta1) * sin(theta2) - cos(theta2) * sin(theta1);
			float det2 = cos(theta1) * d2 - cos(theta2) * d1;
			float det1 = sin(theta1) * d2 - sin(theta2) * d1;

			Point intersection;
			intersection.x = int(-det1 / det3);
			intersection.y = int(det2 / det3);
			if (intersection.x >= 0 && intersection.x < src.size().width && intersection.y >= 0 && intersection.y < src.size().height)
				intersections.push_back(intersection);
		}
	}
	cout << "Size: " << intersections.size() << endl;

	// 4 vertexs of bounding polygen
	Point left_up = intersections[0];
	Point left_down = intersections[0];
	Point right_up = intersections[0];
	Point right_down = intersections[0];

	for (size_t i = 0; i < intersections.size(); ++i)
	{
		if (left_up.x >= src.size().width / 2 ||
			left_up.y >= intersections[i].y && intersections[i].x <= src.size().width / 2)
		{
			left_up = intersections[i];
		}
		if (left_down.x >= src.size().width / 2 ||
			left_down.y <= intersections[i].y && intersections[i].x <= src.size().width / 2)
		{
			left_down = intersections[i];
		}
		if (right_up.x <= src.size().width / 2 ||
			right_up.y >= intersections[i].y && intersections[i].x >= src.size().width / 2)
		{
			right_up = intersections[i];
		}
		if (right_down.x <= src.size().width / 2 ||
			right_down.y <= intersections[i].y && intersections[i].x >= src.size().width / 2)
		{
			right_down = intersections[i];
		}
	}

	vector<Point> vertexes({ left_up, left_down, right_down, right_up });

	// for (size_t i = 0; i < vertexes.size(); ++i)
	//    cout << vertexes[i];

	// polylines(draw, vertexes, true, Scalar(0, 0, 225), 2);

	return vertexes;
}

// Direction when take the image from camera
void direction(Mat src, vector<Point> vertexes, bool& flag)
{
	Point left_up = vertexes[0];
	Point left_down = vertexes[1];
	Point right_up = vertexes[2];
	Point right_down = vertexes[3];

	double left_edge = sqrt(pow(left_up.x - left_down.x, 2) + pow(left_up.y - left_down.y, 2));
	double right_edge = sqrt(pow(right_up.x - right_down.x, 2) + pow(right_up.y - right_down.y, 2));
	double up_edge = sqrt(pow(left_up.x - right_up.x, 2) + pow(left_up.y - right_up.y, 2));
	double down_edge = sqrt(pow(left_down.x - right_down.x, 2) + pow(left_down.y - right_down.y, 2));
	double diagonal = sqrt(pow(left_up.x - right_down.x, 2) + pow(left_up.y - right_down.y, 2));
	double half_circumference1 = 1.0 / 2 * (left_edge + down_edge + diagonal);
	double half_circumference2 = 1.0 / 2 * (right_edge + up_edge + diagonal);
	double area = sqrt(half_circumference1 * (half_circumference1 - left_edge) * (half_circumference1 - down_edge) * (half_circumference1 - diagonal)) +
		sqrt(half_circumference2 * (half_circumference2 - right_edge) * (half_circumference2 - up_edge) * (half_circumference2 - diagonal));

	if (left_up.y < 50 || right_up.y < 50)
		cout << "Log: Move the camera up!" << endl;
	else if (src.size().height - left_down.y < 50 || src.size().height - right_down.y < 50)
		cout << "Log: Move the camera down!" << endl;
	else if (left_up.x < 50 || left_down.x < 50)
		cout << "Log: Move the camera to the left!" << endl;
	else if (src.size().width - right_up.x < 50 || src.size().width - right_down.x < 50)
		cout << "Log: Move the camera to the right!" << endl;

	else if (up_edge > 7.0 / 6 * down_edge)
		cout << "Log: Rotate the camera up!" << endl;
	else if (down_edge > 7.0 / 6 * down_edge)
		cout << "Log: Rotate the camera down!" << endl;
	else if (right_edge > 7.0 / 6 * left_edge)
		cout << "Log: Rotate the camera to the right!" << endl;
	else if (left_edge > 7.0 / 6 * right_edge)
		cout << "Log: Rotate the camera to the left!" << endl;

	else if (area < 0.6 * src.size().width * src.size().height)
		cout << "Log: Move the camera down near a book!" << endl;
	else
	{
		flag = true;
		cout << "Tack" << endl;
	}
}

void scan(Mat& src)
{
	bool flag = false;

	//Mat src;
	Mat src_copy;
	Mat output;
	vector<Point> vertexes;
	int repair = 0;

	//while (flag == false)
	{
		/*src = imread(path_to_image, IMREAD_COLOR);
		if (src.empty())
		{
			cout << "Could not open or find the picture!" << endl;
			return;
		}*/

		/*VideoCapture camera(0);
		if (!camera.isOpened())
		{
			cerr << "ERROR: Could not open camera" << endl;
			return;
		}
		namedWindow("Webcam", WINDOW_AUTOSIZE);
		camera >> src;
		imshow("Webcam", src);*/

		src.copyTo(src_copy);
		cvtColor(src, src_copy, COLOR_BGR2GRAY);
		src_copy = histogram_equalization(src_copy);
		src_copy = gaussian_blur(src_copy,1); // ?????????????
		pair<int, int> shape = get_element_structure_shape(src_copy, 50, 120);
		src_copy = dilation(src_copy, shape);
		vertexes = hough_line_transform(src_copy, src);
		//direction(src, vertexes, flag);
		//waitKey(0);

		/*repair += 1;
		if (repair == 5)*/
		flag = true;
	}

	src.copyTo(output);
	polylines(src, vertexes, true, Scalar(0, 0, 225), 3);

	//imshow("Source image", src);
	//imshow("Outpute image", output);
	//imwrite("D:/Documents/Programming/edgeDetection/data/detect.png", output);
	//waitKey(0);
}

// *************************************************************

struct Cerc {
	Point2f center;
	float radius;
};

struct SimpleShapes {
	Mat img_src;
	Mat img_dst;
	Mat img_bw;
	vector<RotatedRect> rectangles;
	vector<vector<Point>> triangles;
	vector<Cerc> cercles;
};

double angleCos3(Point pt1, Point pt2, Point pt0);
void setLabel2(Mat& img, const string label, vector<Point>& contour);

SimpleShapes detectShapes(Mat& img_src, int num_cam, int lowThreshold) {
	//VideoCapture cap(num_cam); //open the webcam

	Mat img_dst; //scr: source & dst: destination
	img_src.release(); img_dst.release();
	Mat img_gray, img_canny;
	img_gray.release(); img_canny.release();

	SimpleShapes shapes;
	vector<vector<Point>> contours; contours.clear();
	vector<Point> vertices_contour; vertices_contour.clear();
	vector<vector<Point>> triangles; triangles.clear();
	vector<RotatedRect> rectangles; rectangles.clear();
	vector<Cerc> cercles; cercles.clear();

	int RATIO = 3;
	int kernel_size = 3;

	//checking the webcam
	
	//new frame from the webcam
	//cap >> img_src;
	img_dst = img_src.clone();

	//convert to Grayscale
	cvtColor(img_dst, img_dst, CV_BGR2GRAY);

	//binarisation (Canny filter)
	Canny(img_gray, img_canny, lowThreshold, RATIO * lowThreshold, kernel_size);

	//find coutours
	findContours(img_canny.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//img_dst = img_src.clone();

	//find shapes
	for (int i = 0; i < contours.size(); i++) {
		//approximate contour with accuracy proportional
		//to the contour perimeter
		approxPolyDP(Mat(contours[i]), vertices_contour, arcLength(Mat(contours[i]), true) * 0.02, true);

		//skip small or non-convex objects
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(vertices_contour)) {
			continue;
		}

		if (vertices_contour.size() == 3) {//TRIANGLES
			triangles.push_back(vertices_contour); //add to the set of detected triangles

			//drawing
			for (int i = 0; i < 3; i++)
				line(img_src, vertices_contour[i], vertices_contour[(i + 1) % 3], Scalar(0, 255, 0), 3);

			//label
			setLabel2(img_src, "TRI", contours[i]);
		}
		else if (vertices_contour.size() == 4) {
			int nb_vertices = 4; //vertices_contour.size();

			//calculation of "cos" from all corners
			vector<double> cos;
			for (int j = 2; j < nb_vertices + 1; j++) {
				cos.push_back(angleCos3(vertices_contour[j % nb_vertices], vertices_contour[j - 2], vertices_contour[j - 1]));
			}

			//storage of "cos" in ascending order
			sort(cos.begin(), cos.end());

			//save the min and max values of "cos"
			double mincos = cos.front();
			double maxcos = cos.back();

			//check that the corners are right angles
			if (mincos >= -0.3 && maxcos <= 0.3) { //RECTANGLES
				RotatedRect rotRect = minAreaRect(contours[i]);
				rectangles.push_back(rotRect); //add to the set of detected rectangles

				//drawing
				Point2f coins_rect[4];
				rotRect.points(coins_rect);
				for (int i = 0; i < 4; i++)
					line(img_src, coins_rect[i], coins_rect[(i + 1) % 4], Scalar(0, 255, 0), 3);

				//label
				stringstream sentence;
				sentence << "RECT rot = " << rotRect.angle;
				string rotLabel = sentence.str();
				setLabel2(img_dst, rotLabel, contours[i]);
			}
		}
		else { //CIRCLES
			double area = contourArea(contours[i]);
			Rect r = boundingRect(contours[i]);
			int radius = r.width / 2;

			if (abs(1 - ((double)r.width / r.height)) <= 0.2 && abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2) {
				Cerc cercle;
				minEnclosingCircle(contours[i], cercle.center, cercle.radius);
				cercles.push_back(cercle); //add to the set of detected circles

				//drawing
				circle(img_dst, cercle.center, cercle.radius, CV_RGB(255, 255, 0), 3);

				//label
				setLabel2(img_dst, "CIR", contours[i]);
			}
		}
	}

	//save data
	shapes.img_src = img_src;
	shapes.img_dst = img_dst;
	shapes.img_bw = img_canny;
	shapes.rectangles = rectangles;
	shapes.triangles = triangles;
	shapes.cercles = cercles;

	//printf some informations
	if (triangles.size() == 0 && rectangles.size() == 0 && cercles.size() == 0) printf("NOTHING\n");
	else if (triangles.size() == 1 && rectangles.size() == 0 && cercles.size() == 0) printf("TRIANGLE\n");
	else if (triangles.size() == 0 && rectangles.size() == 1 && cercles.size() == 0) printf("RECTANGLE\n");
	else if (triangles.size() == 0 && rectangles.size() == 0 && cercles.size() == 1) printf("CERCLE\n");
	else printf("ERROR\n");

	//send data
	return shapes;
}

/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
double angleCos3(Point pt1, Point pt2, Point pt0) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2));

}

/**
 * Viewing "label" on the contour detected in the picture "img"
 */
void setLabel2(Mat& img, const string label, vector<Point>& contour) {
	int fontface = FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	Size text = getTextSize(label, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour); //returns the smallest rectangle containing the "contour"

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(img, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	putText(img, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

SimpleShapes shapes;
int edgeThresh = 1;
int low_Threshold = 50;
const int max_lowThreshold = 100;

void Bound30(Mat& img_src)
{
	shapes = detectShapes(img_src,0, low_Threshold);
}
