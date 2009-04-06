#include "Histogram.h"

Histogram::Histogram(){
	histogram = Array<int>(0);
	numLevels = 0;
	maxLevel = 0;
	minLevel = 0;
	maxFreq = 0;
	contrast = 0.0;
}

Histogram::Histogram(const int & numLevelsValue){
	numLevels = numLevelsValue;
	histogram = Array<int>(numLevels);
	maxLevel = 0;
	minLevel = 0;
	maxFreq = 0;
	contrast = 0.0;
}

Histogram::Histogram(const Histogram & histogramObject){
	histogram = Array<int>(histogramObject.histogram);
	numLevels = histogramObject.numLevels;
	maxLevel = histogramObject.maxLevel;
	minLevel = 0;
	maxFreq = histogramObject.maxFreq;
	contrast = histogramObject.contrast;
}

Histogram::Histogram(Image * image)
{
	numLevels = image->getMaxLevel()+1;
	histogram = Array<int>(numLevels);
	maxLevel = 0;
	minLevel = 0;
	maxFreq = 0;
	contrast = 0.0;
	calculate(image);
}

Histogram::~Histogram(){
	
}

void Histogram::calculate(Image * imagen)
{
	if(isEmpty()){
		numLevels = imagen->getMaxLevel()+1;
		histogram = Array<int>(numLevels);
	}
	
	// 2 71 60 28 asahi
	
	int pixel = 0;
	
	for(int i = 0; i < imagen->getRows(); i++ ){
		for(int j = 0; j < imagen->getCols(); j++ ){
			pixel = imagen->getPixel(i,j);
			histogram[pixel]++;
			if (maxLevel < pixel) maxLevel = pixel;
			if (minLevel > pixel) minLevel = pixel;
			if (maxFreq <  histogram[pixel]) maxFreq = histogram[pixel];
		}
	}
}

Histogram * Histogram::calculatesCum()
{
	Histogram * cummulative = new Histogram(numLevels);
	int sum = 0;
	
	cout<<cummulative;
	for(int i = 0; i < numLevels; i++ ){
		sum += histogram[i]; 
		cummulative->setAt(i,sum);
	}
	cummulative->setMaxLevel((*cummulative)[numLevels-1]);
	return cummulative;
}

bool Histogram::isEmpty()
{
	if(histogram.lenght() == 0) return true;
	return false;
}

double Histogram::average(const int & startIndex, const int & endIndex)
{
	// cout << "start:" <<startIndex<<"  end: "<<endIndex << endl;
	require(startIndex >= 0 && startIndex < numLevels, "Histogram::average. startIndex Out Of Bounds.\n");
	require(endIndex >= 0 && endIndex < numLevels, "Histogram::average. endIndex Out Of Bounds.\n");
	require(startIndex < endIndex, "Histogram::average. startIndex must be lesser than endIndex");
	
	double product_sum = 0;
	double freq_sum = 0;
	for(int i = startIndex; i <= endIndex; ++i){
		freq_sum += histogram[i];
		product_sum += histogram[i] * i;
	}
	
	return product_sum / freq_sum;
	
}

Image Histogram::equalize(Image * image)
{
	int levels = image->getMaxLevel()+1;
	int max = levels-1;
	Histogram hist = Histogram(image);
	
	int accum = 0;
	int value = 0; 
	Image equalizedImage = Image((*image));
	
	for(int i = 0; i < levels; i++ ){
		accum += hist[i];
		// value = (int) ceil((accum * hist.getMaxLevel()) / (image->getWidth() * image->getHeight()));
		value = (int) (accum * max) / (image->getWidth() * image->getHeight());
		equalizedImage.getLut()[i] = value;
	}
	
	return equalizedImage;
}

Image Histogram::paintHistogram()
{
	int widthHist = numLevels;
	int heightHist = 256;
	int widthImage = widthHist + 60;
	int heightImage = heightHist + 100;
	int xInit = 30;
	int yInit = 15;
	int pixelsToPaint = 0;
	int col = 0;
	int row = 0;
	int rowMax = heightImage-1;
	int fillLevel = 120;
	int barLevelHeight = 27;
	int start = 0;
	int yScaleSize = (int) ceil(heightHist / 8);
	int xScaleSize = (int) ceil(numLevels / 8);
	
	Image image = Image(widthImage, heightImage, Image::P2, 255, 250);
	// Paint the levels bar
	for(int x = 0; x < numLevels+2; ++x){
		col = xInit + x;
		for(int y = 0; y < barLevelHeight; ++y){
			row = ((yInit)*(-1)) + (rowMax-y);
			
			if(x == 0 || x == numLevels+1 || y == 0 || y == barLevelHeight-1)
				image.setPixel(row,col,0);
			else 
				image.setPixel(row,col,x-1);
		}
	}
	
	// Paint the levels frequencies of histogram
	for(int x = 0; x < numLevels; x++ ){
		pixelsToPaint = (int) ((double) histogram[x]/maxFreq * 210);
		start = yInit + barLevelHeight + 20;
		col = xInit + x;
		for(int y = 0; y < pixelsToPaint; ++y){
			row = ((start)*(-1)) + (rowMax-y);
			image.setPixel(row,col,fillLevel);
		}
	}
	
	// Paint the x axis line
	start = yInit + barLevelHeight + 19;
	row = ((start)*(-1)) + (rowMax);
	for(int x = 0; x < numLevels+2; ++x){
		col = xInit + x - 1;
		image.setPixel(row,col,0);
	}
	
	// Paint the y axis line
	col = xInit-1;
	for(int y = 0; y < heightHist+2; ++y){
		row = ((start)*(-1)) + (rowMax-y);
		image.setPixel(row,col,0);
	}
	
	// Paint the scale marks on x axis.
	int scaleMarkSize = 5;
	start -= scaleMarkSize-3;
	for(int x = 0; x < numLevels+2; x += xScaleSize){
		col = xInit + x - 1;
		for(int y = 0; y < scaleMarkSize; ++y){
			row = ((start)*(-1)) + (rowMax-y);
			image.setPixel(row,col,0);
		}
	}
	
	// Paint the scale marks on y axis.
	start = yInit + barLevelHeight + 19;
	for(int y = 0; y < heightHist+2; y += yScaleSize){
		row = ((start)*(-1)) + (rowMax-y);
		for(int x = 0; x < scaleMarkSize; ++x){
			col = xInit - scaleMarkSize + 1 + x;
			image.setPixel(row,col,0);
		}
	}
	
	return image;
}

void Histogram::print(std::ostream & outStream) const
{
	outStream << "Levels: " << numLevels <<"  Contrast: " << contrast << endl;
	outStream << "MaxLevel: " << maxLevel <<"  MinLevel: " << minLevel << endl;
	outStream << "Level    Freq."<<endl<<"-------------"<<endl;
	for(int i = 0; i < numLevels; ++i){
		outStream << setw(4) << i << "    " << setw(5) << histogram[i]<<endl;
	}
}

int Histogram::getLevels(){
	return numLevels;
}

void Histogram::setMaxLevel(const int & maxLevelValue)
{
	maxLevel = maxLevelValue;
	
}

int Histogram::getMaxLevel()
{
	return maxLevel;
}

double Histogram::getContrast(){
	if(contrast == 0.0) calculateContrast();
	return contrast;
}

void Histogram::calculateContrast()
{
	int sum = maxLevel + minLevel;
	if(sum != 0) contrast = (maxLevel - minLevel) / sum;
}

void Histogram::setMinLevel(const int & minLevelValue)
{
	minLevel = minLevelValue;
}

int Histogram::getMinLevel()
{
	return minLevel;
}

int Histogram::getMaxFreq()
{
	return maxFreq;
}

void Histogram::setAt(const int & index, const int & value)
{
	require(index >= 0 && index < numLevels, "Histogram::operator[]. IndexOutOfBounds\n");
	histogram[index] = value;
}

int Histogram::getAt(const int & index)
{
	return histogram[index];
}

int & Histogram::operator[] (const int & index)
{
	require(index >= 0 && index < numLevels, "Histogram::operator[]. IndexOutOfBounds\n");
	return histogram[index];
}

int Histogram::operator[] (const int & index) const
{
	require(index >= 0 && index < numLevels, "Histogram::operator[]. IndexOutOfBounds\n");
	return histogram[index];
}

std::ostream & operator<<(std::ostream & outStream, const Histogram & histogramObject){
	histogramObject.print(outStream);
	return outStream;
}

std::ostream & operator<<(std::ostream & outStream, const Histogram * histogramObject){
	(*histogramObject).print(outStream);
	return outStream;
}
