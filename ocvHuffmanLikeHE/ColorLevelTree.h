#pragma once
/**
* 建立了一个灰度树，初始为一个统计灰度级各个灰度像素数量的链表，依据这个链表建立
* 灰度像素的二叉树。
*/
#ifndef _COLOR_LEVEL_TREE_
#define _COLOR_LEVEL_TREE_

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>

using namespace cv;
#ifndef UC_LIGHTER
#define UC_LIGHTER 1
#endif
#ifndef UC_DARKER
#define UC_DARKER 0
#endif

/************************   DEBUG   ************************/
#define PRINT_ORIGINAL_LINK 0x00000001
#define PRINT_REDUCED_LINK	0x00000002
#define PRINT_BUILDTREE_LINK 0x00000004
/***********************************************************/

#define GRAY_LEVEL 256

class ColorLevelTree {
	class m_TreeNode {
		//把这个设计成一个结构体就轻松多了，不过为了练习OOP。
		int originalColor = -1;
		int equalizedColor = -1;
		//需不需要标记一下结点的深度呢？
		int pixelNum = 0;
		//权值，在不考虑灰度差对图像贡献情况下
		//权值等于像素数
		int weight = 0;
		m_TreeNode * pLeft = NULL;
		m_TreeNode * pRight = NULL;
		m_TreeNode * pOriginalNext = NULL;
		m_TreeNode * pReducedNext = NULL;
	public:
		int colorLow = -1, colorHigh = -1;
		m_TreeNode(int originalColor, int pixelNum,
			m_TreeNode * pLeft, m_TreeNode * pRight,
			m_TreeNode * pOriginalNext, m_TreeNode * pReducedNext) {
			this->originalColor = originalColor;
			this->equalizedColor = -1;
			this->pixelNum = pixelNum;
			this->weight = pixelNum;
			this->pLeft = pLeft;
			this->pRight = pRight;
			this->pReducedNext = pReducedNext;
			this->pOriginalNext = pOriginalNext;
		};
		m_TreeNode(int originalColor, int pixelNum,
			m_TreeNode * pOriginalNext) {
			//createOriginalLink调用
			this->originalColor = originalColor;
			this->equalizedColor = -1;
			this->pixelNum = pixelNum;
			this->weight = pixelNum;
			this->pReducedNext = pOriginalNext;
			this->pOriginalNext = pOriginalNext;
		}
		m_TreeNode(int weight, m_TreeNode *pLeft, m_TreeNode * pRight, m_TreeNode * pReducedNext) {
			//这里用于trackBar精简我感觉需要在结点里加一个isEndNode的标记
			this->weight = weight;
			this->pLeft = pLeft;
			this->pRight = pRight;
			this->pReducedNext = pReducedNext;
		};

		void setPLeft(m_TreeNode * p) { pLeft = p; }
		void setPRight(m_TreeNode * p) { pRight = p; }
		void setPOriginalNext(m_TreeNode * p) { pOriginalNext = p; }
		void setPReducedNext(m_TreeNode * p) { pReducedNext = p; }
		void setEqualizedColor(int color) { equalizedColor = color; }
		m_TreeNode * getPLeft() { return pLeft; }
		m_TreeNode * getPRight() { return pRight; }
		m_TreeNode * getPOriginalNext() { return pOriginalNext; }
		m_TreeNode * getPReducedNext() { return pReducedNext; }
		int getWeight() { return weight; }
		int getPixelNum() { return pixelNum; }
		int getOriginalColor() { return originalColor; }
		int getEqualizedColor() { return equalizedColor; }

		void coutM_TreeNode();
	};
	int countPixel[GRAY_LEVEL];
	int colorMap[GRAY_LEVEL];
	int theGrayLevelUsed = 0;
	//int reduceGrayLevel(int n);
	int prefer = UC_LIGHTER;
	Mat image;
	m_TreeNode * theStartOfReducedGrayLink = NULL;
	m_TreeNode * theStartOfOriginalLink = NULL;
public:
	int preOrderTraval(m_TreeNode * root, int colorLeft, int colorRight);
	int preOrderTraval(m_TreeNode * root, int colorLeft, int colorRight, double lighterOrDarker);
	int getColorMap(int theGrayLevelReduced);
	int getColorMap();
	Mat getEqualizedMat(int theGrayLevelReduced);
	Mat getEqualizedMat();
	ColorLevelTree(Mat& srcImage, int prefer);
	ColorLevelTree(Mat& srcImage);
	~ColorLevelTree();
	int getTheGrayLevelUsed() { return theGrayLevelUsed; }
	int reduceGrayLevel(int n);
	//*****************DEBUG用*******************
	void printParam();
	void printLink(int printOpt);
	//*****************DEBUG用*******************
private:
	int getCountPixel();
	int mergeA_Node();
	int createOriginalLink();
	void freeTree(m_TreeNode *p);
	void freeReducedLink(m_TreeNode *p);

};

#endif
