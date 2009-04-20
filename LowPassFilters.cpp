#include "LowPassFilters.h"


LowPassFilters::LowPassFilters(){
	
}

LowPassFilters::~LowPassFilters(){
	
}

Image LowPassFilters::mean(Image * image, const int & maskSize){
	require(maskSize > 2, "LowPassFilter::mean. The maskSize parameter must be an odd integer greater than 2.");
	require((maskSize % 2) == 1, "LowPassFilter::mean. The maskSize parameter must be an odd integer greater than 2.");
	
	// Image * imgFiltered = new Image((*image));
	Matrix<double> mask = createMeanMask(maskSize);
	
	return ConvolutionOperation::convolution(image, mask);
}

Image LowPassFilters::gaussian(Image * image, const int & maskSize){
	Image imgFiltered = Image((*image)); 
	return imgFiltered;
}

double LowPassFilters::gaussFunction(const int & x, const int & y, const double & sigma){
	double sigma2 = 2 * pow(sigma,2); 
	return (1/(sigma2*3.1416)) * exp(((-1)*((pow((double)x,2)+pow((double)y,2))))/sigma2);
}

Matrix<double> LowPassFilters::createMeanMask(const int & size){
	require(size > 2, "LowPassFilter::createMeanMask. The size parameter must be an odd integer greater than 2.");
	require((size % 2) == 1, "LowPassFilter::createMeanMask. The size parameter must be an odd integer greater than 2.");
	
	double value = (double) 1/(size*size);
	Matrix<double> mask (size, size, value);
	// Matrix<double> mask (size,size,1.0);
	
	return mask;
}


Matrix<double> LowPassFilters::createGaussianMask(const double & sigma){
	Matrix<double> mask = Matrix<double> ();
	vector<double> values(25,0.0);
	double sum = 0.0;
	double gaussValue = 0.0;
	double error = 0.05;
	int delta = 1;
	int start = 0;
	int end = 0;
	
	// Central value
	gaussValue = gaussFunction(0, 0, sigma);
	values.push_back(gaussValue);
	
	do
	{
		start = (-1) * delta;
		end = delta;
		
		// Calculates rows (up and bottom)
		for(int i = start; i < end; ++i){
			
			for(int j = start; j < end; ++j){
				
			}
			
		}
		
		++delta;
		
	}
	while((1.0 - sum) > error);
	
	
	
	return mask;
}
