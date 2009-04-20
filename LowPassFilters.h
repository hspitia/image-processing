#ifndef LOWPASSFILTERS_H
#define LOWPASSFILTERS_H

#include <iostream>
#include <cmath>
#include <vector>
#include "Image.h"
#include "utils/Matrix.h"
#include "ConvolutionOperation.h"

using namespace std;

class LowPassFilters {
	private:
		
		static double gaussFunction(const int & x, const int & y, const double & sigma = 1);
		static Matrix<double> createMeanMask(const int & size);
		static Matrix<double> createGaussianMask(const double & sigma);
		
		
	public:
		LowPassFilters();
		virtual ~LowPassFilters();
		
		static Image mean(Image * image, const int & maskSize = 3);
		static Image gaussian(Image * image, const int & maskSize = 3);
		
		
};
#endif        //  #ifndef LOWPASSFILTERS_H

