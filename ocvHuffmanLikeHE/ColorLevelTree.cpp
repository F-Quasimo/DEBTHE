#include "stdafx.h"
#include "ColorLevelTree.h"

ColorLevelTree::ColorLevelTree(Mat& srcImage, int prefer) {
	if (srcImage.channels() != 1) {
		cvtColor(srcImage, image, CV_BGR2GRAY);
	} else {
		image = srcImage.clone();
	}

	this->prefer = prefer;
	getCountPixel();
	createOriginalLink();
}
ColorLevelTree::ColorLevelTree(Mat& srcImage) {
	/*
	if (srcImage.channels() != 1) {
		cvtColor(srcImage, image, CV_BGR2GRAY);
	} else {
		image = srcImage.clone();
	}*/
	image = srcImage.clone();
	getCountPixel();
	createOriginalLink();
}
int ColorLevelTree::getCountPixel() {
	//统计每个灰度有多少个像素，并将结果存放在countPixel[];
	int tmpCount0 = 0;
	theGrayLevelUsed = 0;
	for (int i = 0; i < GRAY_LEVEL; i++) {
		countPixel[i] = 0;
	}
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
		//std::cout << "The 0 Gray NUM:(in tree)" << tmpCount0 << std::endl;
		return 0;
	} else {
		return -1;
	}
}
void ColorLevelTree::printParam() {
	//printf("TheGrayLevelUsed: %d\n", theGrayLevelUsed);
	using namespace std;
	for (int i = 0; i < GRAY_LEVEL; ++i) {
		cout << "Gray:\t" << i << "\thas" << countPixel[i] << "\t";
	}
	cout << endl;
	cout << "TheGrayLevelUsed:" << theGrayLevelUsed << endl;
	cout << "The Picture Row:\t" << image.rows << "\tThe Picture Cols:\t" << image.cols << endl;
	//imshow("CLass_Mat_image", image);
}
int ColorLevelTree::reduceGrayLevel(int n) {
	//减少原直方图占用的灰度级
	if (n > theGrayLevelUsed)
		n = theGrayLevelUsed - 1;
	if (n > 0) {
		while (n > 0) {
			/*****************    DEBUG   **********************/
			//	std::cout << "\nREDUCE Num:\t" << n << std::endl;
			//printLink(-1);
			/***************************************************/
			n--;
			mergeA_Node();
		}
		return 0;
	} else if (n < 0) {
		/*************************************    DEBUG    ***********************************************/
		std::cout << "Reduce NUM is less than 0!!!\n";
	}

}
int ColorLevelTree::mergeA_Node() {
	//合并连续两个像素占有最少的相邻直方图，其实还可以更优化，根据prefer决定
	//从前往后还是从后往前，算了，假装不会出现同样大小的sum
	//m_TreeNode * &p = theStartOfReducedGrayLink, * &pMin = theStartOfReducedGrayLink;
	//int tmpSum = 0, minWeight = p->getWeight() + p->getPReducedNext()->getWeight();
	//p = p->getPReducedNext();
	//while (p->getPReducedNext() != NULL) {
	//	tmpSum = p->getWeight() + p->getPReducedNext()->getWeight();
	//	if (tmpSum <= minWeight) {
	//		//如果用 <= 就是在多个相同的最小值情况下滑到最高的灰度，如果只是 < 就是合并低的灰度级
	//		//因为直方图均衡对过曝的图优化本身就不好，多以这里直接针对较低灰度的的，实际该根据prefer优化的
	//		minWeight = tmpSum;
	//		pMin = p;
	//	}
	//	p = p->getPReducedNext();
	//}
	m_TreeNode * pMin = theStartOfReducedGrayLink;
	m_TreeNode * p = theStartOfReducedGrayLink, *pre = theStartOfReducedGrayLink;
	int tmpSum = 0, minWeight = 0;
	if (p->getPReducedNext() != NULL) {
		minWeight = p->getWeight() + p->getPReducedNext()->getWeight();
	} else {
		minWeight = p->getWeight();
		return 0;
	}
	p = p->getPReducedNext();
	while (p->getPReducedNext() != NULL) {
		tmpSum = p->getWeight() + p->getPReducedNext()->getWeight();
		if (tmpSum <= minWeight) {
			//如果用 <= 就是在多个相同的最小值情况下滑到最高的灰度，如果只是 < 就是合并低的灰度级
			//因为直方图均衡对过曝的图优化本身就不好，多以这里直接针对较低灰度的的，实际该根据prefer优化的
			minWeight = tmpSum;
			pMin = p;
			//	std::cout << "The Min Of Sun:\t" << minWeight << "\tPoint:\t" << (*ppMin) << "\t"
			//		<< "theStartOfReducedGrayLink:" << theStartOfReducedGrayLink << std::endl;
		}
		p = p->getPReducedNext();
	}
	//std::cout << "\n:::::::::::::::NODE FIRST::::::\n" << std::endl;
	//pMin->coutM_TreeNode();
	//std::cout << "\nFUCK MIN:\t" << pMin->getWeight() + pMin->getPReducedNext()->getWeight() << std::endl;
	//合并两个结点并构造一个二叉树//合并一个结点后可以考虑把两个结点对应的像素直接根据线性插值合并？
	//好吧，暂时先不试验了，本来我也不聪明
	//pMin指向的是和最小的结点首节点
	m_TreeNode * pTmp = new m_TreeNode(pMin->getWeight() + pMin->getPReducedNext()->getWeight(),
		pMin, pMin->getPReducedNext(), pMin->getPReducedNext()->getPReducedNext());
	//把合并后的两个结点的pReducedNext指针清空
	//std::cout << "\n:::::::::::::::NODE SECONF::::::\n" << std::endl;
	//pTmp->coutM_TreeNode();
	pMin->getPReducedNext()->setPReducedNext(NULL);
	pMin->setPReducedNext(NULL);

	//*ppMin = pTmp;
	if (pMin == theStartOfReducedGrayLink) {
		theStartOfReducedGrayLink = pTmp;
	} else {
		while (pre->getPReducedNext() != pMin) {
			pre = pre->getPReducedNext();
		}
		pre->setPReducedNext(pTmp);
	}
	theGrayLevelUsed--;
	//std::cout << "TheGrayLevel:\t\t" << theGrayLevelUsed << std::endl;
	return 0;
}
ColorLevelTree::~ColorLevelTree() {
	m_TreeNode * p = theStartOfReducedGrayLink;
	freeTree(p);
}
void ColorLevelTree::freeReducedLink(m_TreeNode * p) {
	//
	if (p->getPReducedNext() != NULL) {
		//
		freeReducedLink(p->getPReducedNext());

	}
	freeTree(p);
	return;
}
void ColorLevelTree::freeTree(m_TreeNode *p) {
	//
	if (p != NULL) {
		freeTree(p->getPLeft());
		freeTree(p->getPRight());
		delete(p);
	}
}
int ColorLevelTree::createOriginalLink() {
	//在精简之前要被调用，创建基础灰度级链表
	int i = GRAY_LEVEL - 1, countOfZero = 0;
	m_TreeNode * p = NULL;
	for (; i >= 0; --i) {
		if (countPixel[i] != 0) {
			p = new m_TreeNode(i, countPixel[i], theStartOfReducedGrayLink);
			theStartOfReducedGrayLink = p;
		} else {
			countOfZero++;
		}
	}
	theStartOfOriginalLink = theStartOfReducedGrayLink;
	//****************************** DEBUG  ***************************************
	//std::cout << "\nCount of Zero:\t" << countOfZero << std::endl;
	return 0;
}
void ColorLevelTree::printLink(int printOpt) {
	m_TreeNode * pOriginal = theStartOfOriginalLink;
	m_TreeNode * pReduced = theStartOfReducedGrayLink;
	switch (printOpt) {
	case PRINT_ORIGINAL_LINK:
		while (pOriginal != NULL) {
			pOriginal->coutM_TreeNode();
			pOriginal = pOriginal->getPOriginalNext();
		}
		break;
	case PRINT_REDUCED_LINK:
		while (pReduced != NULL) {
			pReduced->coutM_TreeNode();
			pReduced = pReduced->getPReducedNext();
		}
		break;
	default:
		while (pReduced != NULL) {
			std::cout << "\tweight:" << pReduced->getWeight() << "\t";
			pReduced = pReduced->getPReducedNext();
		}
	}

}
void ColorLevelTree::m_TreeNode::coutM_TreeNode() {
	std::cout << "OriginalColor:\t" << originalColor
		<< "\tequalizedColor:\t" << equalizedColor
		<< "\tpixelNum:\t" << pixelNum
		<< "\tweight:\t" << weight
		<< "\tpLeft:\t" << pLeft
		<< "\tpRight:\t" << pRight
		<< "\tpOriginalNext:\t" << pOriginalNext
		<< "\tpReducedNext:t" << pReducedNext << std::endl;
}
int ColorLevelTree::preOrderTraval(m_TreeNode * root, int colorLow, int colorHight) {
	//
	int leftWeight = -1;
	int rightWeight = -1;
	int cover = colorHight - colorLow;
	int midOfLeft = -1, midOfRight = -1;
	root->colorLow = colorLow;
	root->colorHigh = colorHight;
	if (root->getPLeft() != NULL && root->getPRight() != NULL) {
		//这其实一定是个不存在孤立结点的串二叉树
		leftWeight = root->getPLeft()->getWeight();
		rightWeight = root->getPRight()->getWeight();
		if (cover != 0) {			//********************这里可以优化，根据prefer，不过我就不管了
			midOfLeft = cover * (leftWeight*1.0 / root->getWeight()) + colorLow;
			midOfRight = midOfLeft + 1;
			if (midOfLeft < colorLow || midOfRight > colorHight) {
				std::cout << "\n\t\t这尼玛就尴尬了。。。不要出现这种情况\n";
				return -1;
			}
			preOrderTraval(root->getPLeft(), colorLow, midOfLeft);
			preOrderTraval(root->getPRight(), midOfRight, colorHight);

		} else {
			preOrderTraval(root->getPLeft(), colorLow, colorHight);
			preOrderTraval(root->getPRight(), colorLow, colorHight);
		}

	} else if (root->getPLeft() == NULL && root->getPRight() == NULL) {
		//到了根结点
		root->setEqualizedColor((colorLow + colorHight) / 2);
		colorMap[root->getOriginalColor()] = root->getEqualizedColor();
		if (root->getEqualizedColor() > 255 || root->getEqualizedColor() < 0) {
			std::cout << "***********************************出错了***********************\n"
				<< "ROOT->Equalized:\t" << root->getEqualizedColor()
				<< "\n***********************************出错了***********************\n";
		}
		if (root->getEqualizedColor() == 0) {
			;
		}
		//std::cout << "  color: " << root->getEqualizedColor();
		return 0;
	} else {
		/*if (root->getPLeft() != NULL && root->getPRight() == NULL){
		leftWeight = root->getPLeft()->getWeight();
		rightWeight = root->getPRight()->getWeight();
		}*/
		std::cout << "\n\t\t应该不会不运行到这。。。不要出现这种情况\n";
		return -1;
	}
	return 0;
}
int ColorLevelTree::preOrderTraval(m_TreeNode * root, int colorLeft, int colorRight, double lighterOrDarker) {
	if (lighterOrDarker > 1 || lighterOrDarker < 0)
		return -1;
	int cover = colorRight - colorLeft;
	int midLeft = cover * lighterOrDarker + colorLeft;
	int midRight = midLeft + 1;
	preOrderTraval(root->getPLeft(), colorLeft, midLeft);
	preOrderTraval(root->getPRight(), midRight, colorRight);
}
int ColorLevelTree::getColorMap(int theGrayLevelReduced) {
	//这里要开始进行对直方图均衡了

	reduceGrayLevel(theGrayLevelReduced);
	m_TreeNode * p = theStartOfReducedGrayLink;
	double theGrayLevelEchNodeHave = (GRAY_LEVEL - 1) * 1.0 / theGrayLevelUsed;
	int i = 0;

	while (p) {
		/*
		{
		std::cout << "\ni * theGrayLevelEchNodeHave: " << i << "\t:" << i * theGrayLevelEchNodeHave
		<< "\t(i + 1) * theGrayLevelEchNodeHave: " << (i + 1) * theGrayLevelEchNodeHave << std::endl;
		}*/
		preOrderTraval(p, (int)i * theGrayLevelEchNodeHave, (int)(i + 1) * theGrayLevelEchNodeHave);
		p = p->getPReducedNext();
		i++;
	}
	//std::cout << std::endl;
	/*
	for (int i = 0; i < GRAY_LEVEL; i++){
	std::cout << "colorMap: " << colorMap[i] << "\t";
	}*/
	//计算树进行分配灰度时候
	//std::cout << "\nTHE GRAY LEVEL ECH NODE HAVE:\t" << theGrayLevelEchNodeHave << "\ntheGrayLevelUsed:\t" << theGrayLevelUsed << std::endl;
	p = theStartOfReducedGrayLink;
	/*
	while (p->getPReducedNext()) {

	std::cout << "colorLow:\t" << p->colorLow << "colorHigh:\t" << p->colorHigh;
	p = p->getPReducedNext();
	}*/
	/*
	reduceGrayLevel(getTheGrayLevelUsed() - 1);
	preOrderTraval(theStartOfReducedGrayLink, 0, GRAY_LEVEL-1);
	map = colorMap;
	*/
	return -1;
}
int ColorLevelTree::getColorMap() {
	//这里要开始进行对直方图均衡了
	reduceGrayLevel(getTheGrayLevelUsed() - 1);
	//这里的函数有重载
	//preOrderTraval(theStartOfReducedGrayLink, 0, GRAY_LEVEL-1, 0.1);
	preOrderTraval(theStartOfReducedGrayLink, 0, GRAY_LEVEL - 1);

	return 0;
}
Mat ColorLevelTree::getEqualizedMat(int theGrayLevelReduced) {
	getColorMap(theGrayLevelReduced);
	if (image.channels() == 1) {
		//如果是灰度图
		Mat_<uchar>::iterator it = image.begin<uchar>();
		Mat_<uchar>::iterator itEnd = image.end<uchar>();
		for (; it != itEnd; ++it) {
			(*it) = colorMap[*it];
		}
		getCountPixel();
		return image;
	} else {
	
	}
}
Mat ColorLevelTree::getEqualizedMat() {
	getColorMap();
	if (image.channels() == 1) {
		//如果是灰度图
		Mat_<uchar>::iterator it = image.begin<uchar>();
		Mat_<uchar>::iterator itEnd = image.end<uchar>();
		for (; it != itEnd; ++it) {
			(*it) = colorMap[*it];
		}
		//重新统计统计参数
		getCountPixel();
		return image;
	} else {
		return image;
	}
}
