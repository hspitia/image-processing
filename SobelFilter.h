#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include <iostream>
#include <cmath>
#include "Image.h"
#include "ConvolutionOperation.h"
#include "utils/Matrix.h"
#include "utils/utils.h"    

using namespace std;

class SobelFilter {
	public:
		enum process_stage_t {SOBEL, NON_MAXIMAL, HYSTERESIS};
		
	private:
		int borderColor;
		int backgroundColor;
		Matrix<double> * verticalMask;
		Matrix<double> * horizontalMask;
		
		Matrix<double> * verticalMatrix;
		Matrix<double> * horizontalMatrix;
		Matrix<int> * gradientMagnitudes;
		Matrix<int> * gradientAngles;
		Matrix<int> * nonMaximalMagnitudes;
		Matrix<int> * nonMaximalAngles;
		Matrix<int> * hysteresisMagnitudes;
		
		Image * sobelImage;
		Image * nonMaximalImage;
		Image * hysteresisImage;
		
		void createMasks();
		void initMatrices(Image * image);
		int quantizeAngle(double angle);
		void setValues(	const int & row, 
										const int & col,
										const double & magnitudeX,
										const double & magnitudeY,
										const int & threshold);
		
		void nonMaximalSuppression(const int & sideNeighbors);
		bool nonMaximalSuppressionOperation(const int & row, const int & col, const int & sideNeighbors);
		Image * constructColorImage(Image * originImage);
		void hysteresis(const int & lowThreshold, const int & highThreshold);
		
		
	public:
		SobelFilter(const int & borderColor = 255, const int & backgroundColor = 0);
		virtual ~SobelFilter();
		
		Image sobel(Image * image, const int & threshold);
		
		Matrix<double> * getVerticalMask();
		void setVerticalMask(Matrix<double> * verticalMaskValue);
		Matrix<double> * getHorizontalMask();
		void setHorizontalMask(Matrix<double> * horizontalMaskValue);
		Matrix<double> * getVerticalMatrix();
		void setVerticalMatrix(Matrix<double> * verticalMatrixValue);
		Matrix<double> * getHorizontalMatrix();
		void setHorizontalMatrix(Matrix<double> * horizontalMatrixValue);
		Matrix<int> * getGradientMagnitudes();
		void setGradientMagnitudes(Matrix<int> * gradientMagnitudesValue);
		Matrix<int> * getGradientAngles();
		void setGradientAngles(Matrix<int> * gradientAnglesValue);
		Matrix<int> * getNonMaximalMagnitudes();
		void setNonMaximalMagnitudes(Matrix<int> * nonMaximalMagnitudesValue);
		Matrix<int> * getNonMaximalAngles();
		void setNonMaximalAngles(Matrix<int> * nonMaximalAnglesValue);
		
		Image * getSobelImage();
		Image * getNonMaximalImage();
		Image * getHysteresisImage();
		
		Image * createAnglesColorImage(process_stage_t processStageForImage);
};
#endif        //  #ifndef SOBELFILTER_H




