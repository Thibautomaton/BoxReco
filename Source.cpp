#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


Mat getColorRangeImage(Mat);


/**
Mat getColorRangeImage(Mat color_img)
{
	Mat range_img = Mat(color_img.size(), CV_8UC3);

	inRange(color_img, Scalar(80, 130, 150), Scalar(210, 215, 230), range_img);

	return range_img;
}**/
  
Mat getColorRangeImage(Mat color_img)
{
	Mat range_img = Mat(color_img.size(), CV_8UC3);

	inRange(color_img, Scalar(150, 130, 102), Scalar(215, 165, 120), range_img);

	return range_img;
}

vector<Point> findPointsNoTransfo(string image)
{
	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string directory = "goodPictures";

	vector<vector<Point>> contours;
	vector<Point> cnts_poly;

	if (!source.empty())
	{

		proc_img = Mat(source.size(), CV_8UC3);

		inRange(source, Scalar(80, 130, 150), Scalar(210, 215, 230), proc_img);

		struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));

		struct_element1 = getStructuringElement(MORPH_RECT, Size(15, 15), Point(3, 3));

		struct_element2 = getStructuringElement(MORPH_RECT, Size(12, 12), Point(3, 3));

		//morphological operations to eliminate most noise on the box 
		morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

		dilate(proc_img, proc_img, struct_element2);

		erode(proc_img, proc_img, struct_element1);
		Canny(proc_img, canny, 100, 200);

		findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
		dest1 = Mat::zeros(source.size(), CV_8UC3);
		dest2 = Mat::zeros(source.size(), CV_8UC3);

		cv::Rect rect = boundingRect(contours[0]);
		double area = rect.area();
		double maxArea = area;
		int indexMax = 0;

		for (int i = 0; i < contours.size(); i++)
		{
			//the area of the boundingRect is a better estimation than contourArea
			rect = boundingRect(contours[i]);
			area = rect.area();
			if (area > maxArea)
			{
				indexMax = i;
				maxArea = area;

			}
		}

		//this function approximate points of a polygon for the contour
		approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
	}

	return cnts_poly;
}

vector<Point> findPointsWithTransfo(string image)
{
	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string directory = "goodPictures";

	vector<vector<Point>> contours;
	vector<Point> cnts_poly;

	if (!source.empty())
	{
		cerr << "couldn't open the image";

		cvtColor(source, source, COLOR_BGR2YCrCb); //2Luv, 2XYZ, 2YUV 

		proc_img = Mat(source.size(), CV_8UC3);

		inRange(source, Scalar(150, 130, 102), Scalar(215, 165, 120), proc_img);


		struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
		struct_element1 = getStructuringElement(MORPH_RECT, Size(15, 15), Point(3, 3));

		struct_element2 = getStructuringElement(MORPH_RECT, Size(12, 12), Point(3, 3));

		//morphological operations to eliminate most noise on the box 
		morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

		dilate(proc_img, proc_img, struct_element2);

		erode(proc_img, proc_img, struct_element1);
		Canny(proc_img, canny, 100, 200);

		findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
		dest1 = Mat::zeros(source.size(), CV_8UC3);
		dest2 = Mat::zeros(source.size(), CV_8UC3);

		cv::Rect rect = boundingRect(contours[0]);
		double area = rect.area();
		double maxArea = area;
		int indexMax = 0;

		for (int i = 0; i < contours.size(); i++)
		{
			//the area of the boundingRect is a better estimation than contourArea
			rect = boundingRect(contours[i]);
			area = rect.area();
			if (area > maxArea)
			{
				indexMax = i;
				maxArea = area;
			}
		}

		//this function approximate points of a polygon for the contour
		approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
	}
	return cnts_poly;
}

vector<Point> selectGoodPointsCam1(vector<Point> cnts_poly)
{
	double MaxY = cnts_poly[0].y;
	double MinY = cnts_poly[0].y;
	double MinX = cnts_poly[0].x;
	double MaxX = cnts_poly[0].x;

	vector<Point> good_points;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		if (cnts_poly[i].y >= MaxY)
		{
			MaxY = cnts_poly[i].y;
		}
		if (cnts_poly[i].y <= MinY)
		{
			MinY = cnts_poly[i].y;
		}
		if (cnts_poly[i].x >= MaxX)
		{
			MaxX = cnts_poly[i].x;
		}
		if (cnts_poly[i].x <= MinX)
		{
			MinX = cnts_poly[i].x;
		}
	}

	double minx_local = cnts_poly[0].x;
	double maxx_local = cnts_poly[0].x;
	double index_p11 = 0;
	double index_p12 = 0;
	double index_p21 = 0;
	double index_p22 = 0;
	double index_p31 = 0;
	double index_p32 = 0;
	double max_width = 0;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		cout << MaxY << endl;
		if (abs(cnts_poly[i].y - MaxY) < 10)
		{
			for (int j = 0; j < cnts_poly.size(); j++)
			{
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x < minx_local))
				{
					minx_local = cnts_poly[j].x;
					index_p11 = j;
				}
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x > maxx_local))
				{
					maxx_local = cnts_poly[j].x;
					index_p12 = j;
				}
			}
		}

		if (abs(cnts_poly[i].y - MinY) < 15)
		{
			for (int j = 0; j < cnts_poly.size(); j++)
			{
				if ((abs(cnts_poly[j].y - MinY) < 15) && (cnts_poly[j].x < minx_local))
				{
					minx_local = cnts_poly[j].x;
					index_p31 = j;
				}
				if ((abs(cnts_poly[j].y - MinY) < 15) && (cnts_poly[j].x > maxx_local))
				{
					maxx_local = cnts_poly[j].x;
					index_p32 = j;
				}
			}
		}

		for (int j = 0; j < cnts_poly.size(); j++)
		{
			if ((abs(cnts_poly[i].x - cnts_poly[j].x) > max_width) && (abs(cnts_poly[i].y -cnts_poly[j].y)<15)) 
			{
				max_width = abs(cnts_poly[i].x - cnts_poly[j].x);
				index_p21 = i;
				index_p22 = j;
			}
		}
		
	}
	good_points.push_back(cnts_poly[index_p31]);
	good_points.push_back(cnts_poly[index_p32]);

	good_points.push_back(cnts_poly[index_p21]);
	good_points.push_back(cnts_poly[index_p22]);
	good_points.push_back(cnts_poly[index_p11]);
	good_points.push_back(cnts_poly[index_p12]);

	return good_points;
}

vector<Point> selectGoodPointsCam2(vector<Point> cnts_poly)
{
	double MaxY = cnts_poly[0].y;
	double MinY = cnts_poly[0].y;
	double MinX = cnts_poly[0].x;
	double MaxX = cnts_poly[0].x;

	vector<Point> good_points;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		if (cnts_poly[i].y >= MaxY)
		{
			MaxY = cnts_poly[i].y;
		}
		if (cnts_poly[i].y <= MinY)
		{
			MinY = cnts_poly[i].y;
		}
		if (cnts_poly[i].x >= MaxX)
		{
			MaxX = cnts_poly[i].x;
		}
		if (cnts_poly[i].x <= MinX)
		{
			MinX = cnts_poly[i].x;
		}
	}

	double minx_local = cnts_poly[0].x;
	double maxx_local = cnts_poly[0].x;
	double index_p11 = 0;
	double index_p12 = 0;
	double index_p21 = 0;
	double index_p22 = 0;
	double max_width = 0;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		cout << MaxY << endl;
		if (abs(cnts_poly[i].y - MaxY) < 20)
		{
			for (int j = 0; j < cnts_poly.size(); j++)
			{
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x < minx_local))
				{
					minx_local = cnts_poly[j].x;
					index_p11 = j;
				}
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x > maxx_local))
				{
					maxx_local = cnts_poly[j].x;
					index_p12 = j;
				}
			}
		}


		if ((abs(cnts_poly[i].x - cnts_poly[index_p11].x) < 20) && (abs(cnts_poly[i].y - cnts_poly[index_p11].y) > 50))
		{
			index_p21 = i;
		}
		if ((abs(cnts_poly[i].x - cnts_poly[index_p12].x) < 20) && (abs(cnts_poly[i].y - cnts_poly[index_p12].y) > 50))
		{
			index_p22 = i;
		}
	}

	good_points.push_back(cnts_poly[index_p21]);
	good_points.push_back(cnts_poly[index_p22]);
	good_points.push_back(cnts_poly[index_p11]);
	good_points.push_back(cnts_poly[index_p12]);

	return good_points;
}

void work_on_points()
{
	for (int i = 0; i < 8; i++)
	{
		if (i != 5)
		{
			string image1 = "carton_" + to_string(i + 1) + "_pos1.jpg";
			vector<Point> foundPoints1, foundPoints2;
			foundPoints1 = findPointsNoTransfo(image1);

			string image2 = "carton_" + to_string(i + 1) + "_pos2.jpg";
			foundPoints2 = findPointsWithTransfo(image2);

		}
	}
}

int treat_and_display_with_transfo()
{

	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string file = "goodPictures/carton_1_pos2.jpg";

	string directory = "goodPictures";

	string w1 = "source";
	string w2 = "color range image";
	string w3 = "canny";
	string w4 = "segmented";
	string w5 = "contours poly";

	vector<vector<Point>> contours;
	RNG rng(1);

	namedWindow(w1, WINDOW_AUTOSIZE);

	namedWindow(w2, WINDOW_AUTOSIZE);

	namedWindow(w3, WINDOW_AUTOSIZE);

	namedWindow(w4, WINDOW_AUTOSIZE);

	namedWindow(w5, WINDOW_AUTOSIZE);


	source = imread(file);

	if (source.empty())
	{
		cerr << "couldn't open the image";
		return 0;
	}

	cvtColor(source, source, COLOR_BGR2YCrCb); //2Luv, 2XYZ, 2YUV


	proc_img = getColorRangeImage(source);

	struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
	struct_element1 = getStructuringElement(MORPH_RECT, Size(15, 15), Point(3, 3));

	struct_element2 = getStructuringElement(MORPH_RECT, Size(12, 12), Point(3, 3));

	//morphological operations to eliminate most noise on the box 
	morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

	dilate(proc_img, proc_img, struct_element2);

	erode(proc_img, proc_img, struct_element1);
	Canny(proc_img, canny, 100, 200);

	findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
	dest1 = Mat::zeros(source.size(), CV_8UC3);
	dest2 = Mat::zeros(source.size(), CV_8UC3);

	cv::Rect rect = boundingRect(contours[0]);
	double area = rect.area();
	double maxArea = area;
	int indexMax = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(dest1, contours, (int)i, color, 2);

		//the area of the boundingRect is a better estimation than contourArea
		rect = boundingRect(contours[i]);
		area = rect.area();
		if (area > maxArea)
		{
			indexMax = i;
			maxArea = area;
		}
	}

	vector<vector<Point>> cnts;
	vector<Point> cnts_poly;

	//this function approximate points of a polygon for the contour
	approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
	cnts.push_back(cnts_poly);

	fillPoly(dest1, contours[indexMax], Scalar(255, 255, 255));

	drawContours(dest2, cnts, 0, Scalar(0, 0, 255), 5);

	//cornerHarris(dest2, corners, 2, 3, 0.04);
	//goodFeaturesToTrack(dest2, corners, 1, 0.5, 50, noArray(), 3, true);


	//display the points of the polygons. This is what we use to estimate the dimensions

	vector<Point> gPoints;
	gPoints = selectGoodPointsCam1(cnts_poly);
	for (int j = 0; j < gPoints.size(); j++)
	{
		circle(dest2, gPoints[j], 5, Scalar(0, 255, 0), 3);
	}
	//circle(dest2, Point(230, 394), 5, Scalar(0, 255, 0), 3);

	imshow(w1, source);
	imshow(w2, proc_img);
	imshow(w3, canny);
	imshow(w4, dest1);
	imshow(w5, dest2);

	waitKey();
	return 0;

}

int treat_and_display_with_no_transfo()
{
	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string directory = "goodPictures";
	string file = "goodPictures/carton_1_pos1.jpg";

	vector<vector<Point>> contours;
	vector<Point> cnts_poly;

	string w1 = "source";
	string w2 = "color range image";
	string w3 = "canny";
	string w4 = "segmented";
	string w5 = "contours poly";

	namedWindow(w1, WINDOW_AUTOSIZE);

	namedWindow(w2, WINDOW_AUTOSIZE);

	namedWindow(w3, WINDOW_AUTOSIZE);

	namedWindow(w4, WINDOW_AUTOSIZE);

	namedWindow(w5, WINDOW_AUTOSIZE);

	source = imread(file);

	if (!source.empty())
	{

		proc_img = Mat(source.size(), CV_8UC3);

		inRange(source, Scalar(80, 130, 150), Scalar(210, 215, 230), proc_img);

		struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));

		struct_element1 = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));

		struct_element2 = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));

		//morphological operations to eliminate most noise on the box 
		morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

		erode(proc_img, proc_img, struct_element1);
		dilate(proc_img, proc_img, struct_element2);

		
		Canny(proc_img, canny, 100, 200);

		findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
		dest1 = Mat::zeros(source.size(), CV_8UC3);
		dest2 = Mat::zeros(source.size(), CV_8UC3);

		cv::Rect rect = boundingRect(contours[0]);
		double area = rect.area();
		double maxArea = area;
		int indexMax = 0;

		for (int i = 0; i < contours.size(); i++)
		{
			//the area of the boundingRect is a better estimation than contourArea
			rect = boundingRect(contours[i]);
			area = rect.area();
			if (area > maxArea)
			{
				indexMax = i;
				maxArea = area;

			}
		}
		vector<vector<Point>> cnts;
		vector<Point> cnts_poly;

		//this function approximate points of a polygon for the contour
		approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
		cnts.push_back(cnts_poly);

		fillPoly(dest1, contours[indexMax], Scalar(255, 255, 255));

		drawContours(dest2, cnts, 0, Scalar(0, 0, 255), 5);
		//cornerHarris(dest2, corners, 2, 3, 0.04);
//goodFeaturesToTrack(dest2, corners, 1, 0.5, 50, noArray(), 3, true);


//display the points of the polygons. This is what we use to estimate the dimensions

		vector<Point> gPoints;
		gPoints = selectGoodPointsCam2(cnts_poly);
		for (int j = 0; j < gPoints.size(); j++)
		{
			circle(dest2, gPoints[j], 5, Scalar(0, 255, 0), 3);
		}
		//circle(dest2, Point(230, 394), 5, Scalar(0, 255, 0), 3);

		imshow(w1, source);
		imshow(w2, proc_img);
		imshow(w3, canny);
		imshow(w4, dest1);
		imshow(w5, dest2);

		waitKey();
		return 0;
	}
	return 0;

}


int main()
{
	treat_and_display_with_no_transfo();

	return 0;
}