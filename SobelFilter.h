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
	private:
		Matrix<double> * verticalMask;
		Matrix<double> * horizontalMask;
		
		Matrix<double> * verticalMatrix;
		Matrix<double> * horizontalMatrix;
		Matrix<int> * gradientMagnitudes;
		Matrix<int> * gradientAngles;
		Matrix<int> * nonMaximalMagnitudes;
		Matrix<int> * nonMaximalAngles;
		
		void createMasks();
		void initMatrices(Image * image);
		int quantizeAngle(double angle);
		void setValues(	const int & row, 
										const int & col,
										const double & magnitudeX,
										const double & magnitudeY,
										Image * imageFiltered,
										Image * colorImgFiltered,
										const int & threshold);
		
		void nonMaximalSuppression(const int & sideNeighbors);
		// bool equalDirection(const double & dir1, const double & dir2, const double & dir3);
		void nonMaximalSuppressionOperation(const int & row, const int & col, const int & sideNeighbors);
		
	public:
		SobelFilter();
		virtual ~SobelFilter();
		
		Image sobel(Image * image, const int & threshold);
		
		
		
		
		
};
#endif        //  #ifndef SOBELFILTER_H

