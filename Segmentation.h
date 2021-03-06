#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <iostream>
#include <cmath>
#include "Histogram.h"
#include "Image.h"
#include "utils/Array.h"

using namespace std;

class Segmentation {
	private:
		
		
	public:
		Segmentation();
		~Segmentation();
		
		static int thresholdingIsodata(Image * image);
		static int thresholdingOtsu(Image * image);
		static Array<int> multiThresholdingOtsu(Image * image, int const & numThresholds);
		static Image segmentate(const Image & image, const int & threshold);
		
	
};
#endif        //  #ifndef SEGMENTATION_H

