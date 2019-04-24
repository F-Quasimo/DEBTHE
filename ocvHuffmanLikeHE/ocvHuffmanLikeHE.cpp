// huffmanLikeEqualizetion.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "ColorLevelTree.h"
#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
int getCountPixel(Mat image);
static void on_Contrast(int n, void *);
int g_TheGrayLevelReduced;
Mat myEqualizedImage, img, opencvHistImg;

int evaluate();
int evaluateOutCurve(string fileName);
int evaluateEdge(string WindowName, Mat mat, int EDGE_TYPE);
#define _EDGE_CANNY_METHOD_ 1
#define _EDGE_SOBEL_METHOD_ 2
#define _EDGE_PREWITT_METHOD_ 3
#define _EDGE_ROBERTS_METHOD_ 4
#define _EDGE_LOG_METHOD_ 5
Mat matHSVs[3];

vector<string> paramCurveBuffer;
int main2(int argc, char** argv) {
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	//	Mat img = imread("demo_100_50.bmp");
	if (argc == 1) {
		img = imread("seed.tif");
	} else {
		img = imread(argv[1]);
		cout << argv[0] << "\n" << argv[1] << endl;
	}

	Mat mat = img;
	Mat matHSV;
	cvtColor(mat, matHSV, COLOR_BGR2HSV);
	split(matHSV, matHSVs);
	Mat matHSV_H = matHSVs[0];
	Mat matHSV_S = matHSVs[1];
	Mat matHSV_V = matHSVs[2];

	Mat matYCrCb, matYCrCbs[3];
	cvtColor(mat, matYCrCb, COLOR_BGR2YCrCb);
	split(matYCrCb, matYCrCbs);
	Mat matYCrCbsY = matYCrCbs[0];
	Mat matYCrCbsCr = matYCrCbs[1];
	Mat matYCrCbsCb = matYCrCbs[2];


	namedWindow("ORIGINAL IMG");
	imshow("ORIGINAL IMG", img);
	moveWindow("ORIGINAL IMG", 100, 100);
	cout << "\nLoadedpicture";

	opencvHistImg = matHSVs[2].clone();
	equalizeHist(opencvHistImg, opencvHistImg);
	matHSVs[2] = opencvHistImg;
	merge(matHSVs, 3, opencvHistImg);
	cvtColor(opencvHistImg, opencvHistImg, COLOR_HSV2BGR);
	namedWindow("OpenCV_Equalized");
	imshow("OpenCV_Equalized", opencvHistImg);
	moveWindow("OpenCV_Equalized", 150, 150);
	cout << "\nOpenCV_Equalized";
	getCountPixel(opencvHistImg);

	myEqualizedImage = matHSV_V.clone();
	namedWindow("MyAlgorithmEqualized");
	namedWindow("myAlgorithmEDGE");

	createTrackbar("contrast", "MyAlgorithmEqualized", &g_TheGrayLevelReduced, GRAY_LEVEL - 1, on_Contrast);
	if (argc == 3) {
		string str = argv[2];
		g_TheGrayLevelReduced = atoi(str.c_str());
		on_Contrast(0, 0);
	} else {
		on_Contrast(0, 0);
	}
	moveWindow("MyAlgorithmEqualized", 200, 200);
	moveWindow("myAlgorithmEDGE", 350, 350);

	waitKey(0);
	return 0;
}
int main(int argc, char* argv[]) {
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	//	Mat img = imread("demo_100_50.bmp");
	if (argc == 1) {
		img = imread("beautiful.bmp");
	} else {
		img = imread(argv[1]);
		cout << argv[0] << "\n" << argv[1] << endl;
	}

	//cvtColor(img, img, CV_BGR2GRAY);
	//Mat mat = img.clone();
	//mat;
	//namedWindow("Loadedpicture");
	//imshow("Loadedpicture", img);
	namedWindow("ORIGINAL IMG");
	imshow("ORIGINAL IMG", img);
	moveWindow("ORIGINAL IMG", 100, 100);
	cout << "\nLoadedpicture";
	getCountPixel(img);

	myEqualizedImage = img.clone();

	//tree.printParam();
	//tree.printLink(PRINT_REDUCED_LINK);
	//myEqualizedImage = tree.getEqualizedMat();

	namedWindow("MyAlgorithmEqualized");
	//namedWindow("myAlgorithmEDGE");

	createTrackbar("contrast", "MyAlgorithmEqualized", &g_TheGrayLevelReduced, GRAY_LEVEL - 1, on_Contrast);
	if (argc == 3) {
		string str = argv[2];
		g_TheGrayLevelReduced = atoi(str.c_str());
		on_Contrast(0, 0);
	} else {
		on_Contrast(0, 0);
	}
	moveWindow("MyAlgorithmEqualized", 200, 200);
	//moveWindow("myAlgorithmEDGE", 350, 350);

	/*
	opencvHistImg = img.clone();
	equalizeHist(opencvHistImg, opencvHistImg);
	namedWindow("OpenCV_Equalized");
	imshow("OpenCV_Equalized", opencvHistImg);
	moveWindow("OpenCV_Equalized", 150, 150);
	cout << "\nOpenCV_Equalized";
	getCountPixel(opencvHistImg);

	imwrite("grayPic.png", img, compression_params);
	imwrite("opencvHistImg.png", opencvHistImg, compression_params);
	*/
	if (argc == 3) {
		string str = argv[2];
		imwrite("myEqualizedImage" + str + ".png", myEqualizedImage, compression_params);
	} else {
		imwrite("myEqualizedImage.png", myEqualizedImage, compression_params);
	}
	//评估算法
	//evaluate();
	/*
	namedWindow("OriginalCanny");
	moveWindow("OriginalCanny", 250, 250);
	evaluateEdge("OriginalCanny", img, _EDGE_CANNY_METHOD_);
	namedWindow("OpenCV_EDGE_Canny");
	moveWindow("OpenCV_EDGE_Canny", 300, 300);
	evaluateEdge("OpenCV_EDGE_Canny", opencvHistImg, _EDGE_CANNY_METHOD_);
	*/
	waitKey(0);
	return 0;
}
static void on_Contrast(int n, void *) {
	//
	Mat mat;
	/*
	Mat mat, matHSV;
	cvtColor(img, matHSV, COLOR_BGR2HSV);
	split(matHSV, matHSVs);

	myEqualizedImage = matHSVs[2].clone();
	*/

	/*
	
	Mat matHSV, matHSVs[3], matTmp = image;
	cvtColor(image, matHSV, COLOR_BGR2HSV);
	split(matHSV, matHSVs);
	image = matHSVs[2];
	matTmp = image;
	matTmp = getEqualizedMat(theGrayLevelReduced);
	matHSVs[2] = image;
	merge(matHSVs, 3, matHSV);
	cvtColor(matHSV, image, COLOR_HSV2BGR);
	return image;
	
	*/
	if (img.type() == CV_8UC1) {
		myEqualizedImage = img.clone();
		mat = myEqualizedImage;
		ColorLevelTree tree = ColorLevelTree(myEqualizedImage);
		myEqualizedImage = tree.getEqualizedMat(g_TheGrayLevelReduced);
		//matHSVs[2] = myEqualizedImage;
		//merge(matHSVs, 3, mat);
		//cvtColor(mat, mat, COLOR_HSV2BGR);
		imshow("MyAlgorithmEqualized", myEqualizedImage);

		cout << "\nMyAlgorithmEqualized";
		getCountPixel(myEqualizedImage);
	} else {
		Mat matHSV, matHSVs[3], matTmp = img;
		cvtColor(img, matHSV, COLOR_BGR2HSV);
		split(matHSV, matHSVs);

		ColorLevelTree tree = ColorLevelTree(matHSVs[2]);
		matHSVs[2] = tree.getEqualizedMat(g_TheGrayLevelReduced);
		merge(matHSVs, 3, matHSV);
		cvtColor(matHSV, myEqualizedImage, COLOR_HSV2BGR);
		//matHSVs[2] = myEqualizedImage;
		//merge(matHSVs, 3, mat);
		//cvtColor(mat, mat, COLOR_HSV2BGR);
		imshow("MyAlgorithmEqualized", myEqualizedImage);

		cout << "\nMyAlgorithmEqualized";
		getCountPixel(myEqualizedImage);
		
	}
	

	//evaluateEdge("myAlgorithmEDGE", myEqualizedImage, _EDGE_CANNY_METHOD_);
	return;
}
int evaluate() {
	//评估合并次数为0~255时灰度级使用数量和图像平均灰度、梯度、边缘
	/**/
	for (int i = 0; i < GRAY_LEVEL; i++) {
		myEqualizedImage = img.clone();
		string str = "";
		ColorLevelTree tree = ColorLevelTree(myEqualizedImage);
		cout << "\nbefore Equalized GrayLevel Used:\t" << tree.getTheGrayLevelUsed()
			<< "\tthe NUM of 0 Gray : \t" << GRAY_LEVEL - tree.getTheGrayLevelUsed() << "\t i = " << i << endl;
		str = to_string(i) + " " + to_string(tree.getTheGrayLevelUsed()) + " ";	//构建二叉树之前的灰度数据
		myEqualizedImage = tree.getEqualizedMat(i);
		cout << "after  Equalized GrayLevel Used:\t" << tree.getTheGrayLevelUsed()
			<< "\tthe NUM of 0 Gray : \t" << GRAY_LEVEL - tree.getTheGrayLevelUsed() << endl;
		str = str + to_string(tree.getTheGrayLevelUsed()) + "\n";
		paramCurveBuffer.push_back(str);
	}
	cout << "**************************统计完成************************" << endl;
	evaluateOutCurve(string("myAlgorithmOutCurve.txt"));

	return 0;
}
int evaluateOutCurve(string fileName) {
	ofstream paramCurveOut(fileName);
	vector<string>::iterator it = paramCurveBuffer.begin();
	if (paramCurveOut.is_open()) {
		while (it != paramCurveBuffer.end()) {
			paramCurveOut << *it;
			it++;
		}
		paramCurveOut.close();
	}
	return 0;
}
int evaluateEdge(string WindowName, Mat mat, int EDGE_TYPE) {
	//边缘检测评估伪边缘
	Mat edge;
	switch (EDGE_TYPE) {
	case _EDGE_CANNY_METHOD_:
		Canny(mat, edge, 70, 100, 3);
		break;
	case _EDGE_SOBEL_METHOD_:
		;
		break;
	case _EDGE_LOG_METHOD_:
		;
		break;
	case _EDGE_PREWITT_METHOD_:
		;
		break;
	case _EDGE_ROBERTS_METHOD_:
		;
		break;
	default:
		break;
	}
	imshow(WindowName, edge);

	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	imwrite(WindowName + ".png", edge, compression_params);
	return 0;
}
int getCountPixel(Mat image) {
	//统计每个灰度有多少个像素，并将结果存放在countPixel[];
	int tmpCount0 = 0;
	int countPixel[256] = { 0 };
	int theGrayLevelUsed = 0;
	if (image.channels() == 1) {
		//如果是灰度图
		Mat_<uchar>::iterator it = image.begin<uchar>();
		Mat_<uchar>::iterator itEnd = image.end<uchar>();
		for (; it != itEnd; ++it) {
			countPixel[*it]++;
		}
		for (int i = 0; i < GRAY_LEVEL; ++i) {
			if (countPixel[i] > 0) {
				theGrayLevelUsed++;
			} else if (countPixel[i] == 0) {
				tmpCount0++;
			}
		}
		//*********************  DEBUG     ******************************
		std::cout << "\t\ttheGrayLevelUsed:\t" << theGrayLevelUsed << "\tThe 0 Gray NUM(in main):" << tmpCount0 << endl;
		return 0;
	} else {
		return -1;
	}
}

