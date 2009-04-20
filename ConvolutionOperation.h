#ifndef CONVOLUTIONOPERATION_H
#define CONVOLUTIONOPERATION_H

#include <iostream>
#include "Image.h"
#include "utils/Matrix.h"

using namespace std;

class ConvolutionOperation {
	private:
		
	public:
		ConvolutionOperation();
		virtual ~ConvolutionOperation();
		
		static double convolutionCenter(const int & row, const int & col, Image * image, Matrix<double> & mask);
		static double convolutionBorder(const int & row, const int & col, Image * image, Matrix<double> & mask);
		
		static Image convolution(Image * image, Matrix<double> & mask);
};


#endif        //  #ifndef CONVOLUTIONOPERATION_H

