#ifndef LOWPASSFILTERS_H
#define LOWPASSFILTERS_H

#include <iostream>
#include <cmath>
#include <vector>
#include "Image.h"
#include "utils/Matrix.h"

using namespace std;

class LowPassFilters {
	private:
		
		static double gaussFunction(const int & x, const int & y, const double & sigma = 1);
		
		static Matrix<double> createGaussianMask(const double & sigma);
		static double convolutionCenter(const int & row, const int & col, Image * image, Matrix<double> & mask);
		static double convolutionBorders(const int & row, const int & col, Image * image, Matrix<double> & mask);
		static Image convolution(Image * image, Matrix<double> & mask);
		
		
		
	public:
		LowPassFilters();
		virtual ~LowPassFilters();
		
		static Image mean(Image * image, const int & maskSize = 3);
		static Image gaussian(Image * image, const int & maskSize = 3);
		static Matrix<double> createMeanMask(const int & size);
		
		
};
#endif        //  #ifndef LOWPASSFILTERS_H

