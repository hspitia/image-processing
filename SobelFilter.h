#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include <iostream>
#include <cmath>
#include "Image.h"
#include "ConvolutionOperation.h"
#include "utils/Matrix.h"

using namespace std;

class SobelFilter {
	private:
		Matrix<double> * verticalMask;
		Matrix<double> * horizontalMask;
		
		Matrix<double> * verticalMatrix;
		Matrix<double> * horizontalMatrix;
		Matrix<double> * gradientMagnitudes;
		Matrix<double> * gradientAngles;
		
		void createMasks();
		void initMatrices(Image * image);
		double classifyAngle(double angle);
		void setValues(	const int & row, 
										const int & col,
										const double & magnitudeX,
										const double & magnitudeY,
										Image * imageFiltered,
										const int & threshold);
		
	public:
		SobelFilter();
		virtual ~SobelFilter();
		
		Image sobel(Image * image, const int & threshold);
		
		
		
		
		
};
#endif        //  #ifndef SOBELFILTER_H
