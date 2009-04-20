#include "Segmentation.h"

Segmentation::Segmentation()
{
	
}

Segmentation::~Segmentation()
{
	
}

int Segmentation::thresholdingIsodata(Image * image)
{
	Histogram histogram = Histogram(image);
	int levels = histogram.getLevels() - 1;
	int threshold = (int) histogram.average(0, levels);
	int threshold_temp = threshold;
	
	do {
		threshold  = threshold_temp; 
		threshold_temp = (int) (histogram.average(0, threshold) + histogram.average(threshold+1, levels)) / 2;
	} while(threshold != threshold_temp);
	
	return threshold;
}

int Segmentation::thresholdingOtsu(Image * image)
{
	Histogram histogram = Histogram(image);
	int imageSize = image->getWidth() * image->getHeight();
	int levels = histogram.getLevels();
	int threshold = 0;
	double mu_1 = 0.0;
	double mu_2 = 0.0;
	double omega_1 = 0.0;
	double omega_2 = 0.0;
	
	Array<double> prob = Array<double>(levels);
	
	// Probabilities calculation
	for(int i = 0; i < prob.lenght(); ++i){
		prob[i] = (double) histogram[i] / imageSize;
	}
	
	Array<double> omegaArray = Array<double>(levels);
	Array<double> muArray = Array<double>(levels, 0.0);
	
	// Accumulated momentum of order zero and order one calculations
	double sum_prod =  0.0;
	double sum = 0.0;
	
	for(int i = 0; i < levels; ++i){
		sum += prob[i];
		omegaArray[i] = sum;
		
		sum_prod += (i * prob[i]);
		muArray[i] = sum_prod; 
	}
	
	//Otsu threshold calculation
	double sigmaB2max = 0.0;
	double sigmaB2 = 0.0;
	double mu_t =  muArray[levels-1];
	
	for(int i = 0; i < levels; ++i){
		omega_1 = omegaArray[i];
		omega_2 = 1 - omega_1;
		if(omega_1 != 0 && omega_2 != 0){
			mu_1 = (double) muArray[i] / omega_1;
			mu_2 = (double) (mu_t - muArray[i]) / omega_2;
			
			sigmaB2  = (double) 	(omega_1 * pow((mu_1 - mu_t),2)) + 
													(omega_2 * pow((mu_2 - mu_t),2));
			if(sigmaB2 > sigmaB2max){
				sigmaB2max = sigmaB2;
				threshold = i-1;
			}
		}
	}
	
	return threshold;
}

Array<int> Segmentation::multiThresholdingOtsu(Image * image, int const & numThresholds)
{
	Histogram histogram = Histogram(image);
	int imageSize = image->getWidth() * image->getHeight();
	int levels = histogram.getLevels();
	Array<int> thresholds = Array<int>(numThresholds,0);
	// double mu_1 = 0.0;
	// double mu_2 = 0.0;
	Array<double> mu_k = Array<double>(numThresholds,0.0);
	
	// double omega_1 = 0.0;
	// double omega_2 = 0.0;
	Array<double> omega_k = Array<double>(numThresholds,0.0);
	
	Array<double> prob = Array<double>(levels);
	
	// Probabilities calculation
	for(int i = 0; i < prob.lenght(); ++i){
		prob[i] = (double) histogram[i] / imageSize;
	}
	
	Array<double> omegaArray = Array<double>(levels);
	Array<double> muArray = Array<double>(levels, 0.0);
	
	// Accumulated momentum of order zero and order one calculations
	double sum_prod =  0.0;
	double sum = 0.0;
	
	for(int i = 0; i < levels; ++i){
		sum += prob[i];
		omegaArray[i] = sum;
		
		sum_prod += (i * prob[i]);
		muArray[i] = sum_prod; 
	}
	
	//Otsu thresholds calculation
	double sigmaB2max = 0.0;
	double sigmaB2 = 0.0;
	double mu_t =  muArray[levels-1];
	
	for(int i = 0; i < levels; ++i){
		
		//Initialization of omega_k array
		omega[0] = omegaArray[i];
		
		for(int j = 1; j < to; ++j ){
			omega_k[i] = ;
		} 
		omega_2 = 1 - omega_1;
		
		
		
		if(omega_1 != 0 && omega_2 != 0){
			mu_1 = (double) muArray[i] / omega_1;
			mu_2 = (double) (mu_t - muArray[i]) / omega_2;
			
			sigmaB2  = (double) 	(omega_1 * pow((mu_1 - mu_t),2)) + 
													(omega_2 * pow((mu_2 - mu_t),2));
			if(sigmaB2 > sigmaB2max){
				sigmaB2max = sigmaB2;
				thresholds = i-1;
			}
		}
	}
	
	return thresholds;
	
}

Image Segmentation::segmentate(const Image & image, const int & threshold){
	Image segmentated = Image(image);
	int maxLevel = segmentated.getMaxLevel();
	
	for(int i = 0; i < segmentated.getHeight(); ++i){
		for(int j = 0; j < segmentated.getWidth(); ++j){
			if(segmentated.getPixel(i,j) <= threshold) segmentated.setPixel(i,j,0);
			else segmentated.setPixel(i,j,maxLevel);
		}
	}
	
	return segmentated;
}
