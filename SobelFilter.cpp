#include "SobelFilter.h"

SobelFilter::SobelFilter(){
	// createMasks();
	verticalMask = NULL;
	horizontalMask = NULL;
	
	verticalMatrix = NULL;
	horizontalMatrix = NULL;
	gradientMagnitudes = NULL;
	gradientAngles = NULL;
}

SobelFilter::~SobelFilter(){
	
	if(verticalMask != NULL) delete verticalMask;
	verticalMask = NULL;
	
	if(horizontalMask != NULL) delete horizontalMask;
	horizontalMask = NULL;
	
	if(verticalMatrix != NULL) delete verticalMatrix;
	verticalMatrix = NULL;
	
	if(horizontalMatrix != NULL) delete horizontalMatrix;
	horizontalMatrix = NULL;
	
	if(gradientMagnitudes != NULL) delete gradientMagnitudes;
	gradientMagnitudes = NULL;
	
	if(gradientAngles != NULL) delete gradientAngles;
	gradientAngles = NULL;
}

Image SobelFilter::sobel(Image * image, const int & threshold){
	
	Image * imageFiltered = new Image(*image);
	createMasks();
	initMatrices(image);
	
	int endRow = image->getRows() - 1;
	int endCol = image->getCols() - 1;
	
	double magnitudeX = 0.0;
	double magnitudeY = 0.0;
	
	// Convolution for image central area
	for(int rowIdx = 1; rowIdx < endRow; ++rowIdx){
		for(int colIdx = 1; colIdx < endCol; ++colIdx){
			magnitudeX = ConvolutionOperation::convolutionCenter(rowIdx, colIdx, image, *horizontalMask);
			magnitudeY = ConvolutionOperation::convolutionCenter(rowIdx, colIdx, image, *verticalMask);
			setValues(rowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, threshold);
		}
	}
	
	
	// Convolution for columns out borders (left and right)
	int rightColIdx = endCol;
	int colIdx = 0;
	for(int rowIdx = 1; rowIdx < endRow; ++rowIdx){
		// Upper row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *verticalMask);
		setValues(rowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, threshold);
		
		// Lower row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, rightColIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, rightColIdx, image, *verticalMask);
		setValues(rowIdx, rightColIdx, magnitudeX, magnitudeY, imageFiltered, threshold);
	}
	
	
	// Convolution for rows out borders (up and bottom)
	endCol++;
	int lowerRowIdx = endRow;
	int rowIdx = 0;
	
	for(int colIdx = 0; colIdx < endCol; ++colIdx){
		// Upper row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *verticalMask);
		setValues(rowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, threshold);
		
		// Lower row
		magnitudeX = ConvolutionOperation::convolutionBorder(lowerRowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(lowerRowIdx, colIdx, image, *verticalMask);
		setValues(lowerRowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, threshold);
	}
	Image * nonMaximal = nonMaximalSuppression(image);
	nonMaximal->save("data/nonMaximal.pgm");
	return (*imageFiltered);
}

void SobelFilter::createMasks(){
	verticalMask = new Matrix<double>(3,3);
	horizontalMask = new Matrix<double>(3,3);
	
	verticalMask->setAt(0,0,1);
	verticalMask->setAt(0,1,2);
	verticalMask->setAt(0,2,1);
	verticalMask->setAt(1,0,0);
	verticalMask->setAt(1,1,0);
	verticalMask->setAt(1,2,0);
	verticalMask->setAt(2,0,-1);
	verticalMask->setAt(2,1,-2);
	verticalMask->setAt(2,2,-1);
	
	horizontalMask->setAt(0,0,1);
	horizontalMask->setAt(0,1,0);
	horizontalMask->setAt(0,2,-1);
	horizontalMask->setAt(1,0,2);
	horizontalMask->setAt(1,1,0);
	horizontalMask->setAt(1,2,-2);
	horizontalMask->setAt(2,0,1);
	horizontalMask->setAt(2,1,0);
	horizontalMask->setAt(2,2,-1);
}

double SobelFilter::classifyAngle(double angle)
{
	if(angle < 0) angle += 360;
	
	if(angle >= 22.5 && angle < 67.5) {
		angle = 45.0;
	}
	else if(angle >= 67.5 && angle < 112.5) {
		angle = 90.0;
	}
	else if(angle >= 112.5 && angle < 157.5) {
		angle = 135.0;
	}
	else if(angle >= 157.5 && angle < 202.5) {
		angle = 180.0;
	}
	else if(angle >= 202.5 && angle < 247.5) {
		angle = 225.0;
	}
	else if(angle >= 247.5 && angle < 292.5) {
		angle = 270.0;
	}
	else if(angle >= 292.5 && angle < 337.5) {
		angle = 315.0;
	}
	else angle = 0.0;
	
	return angle;
}

void SobelFilter::initMatrices(Image * image)
{
	int rows = image->getRows();
	int cols = image->getCols();
	verticalMatrix = new Matrix<double>(rows,cols);
	horizontalMatrix = new Matrix<double>(rows,cols);
	// gradientMagnitudes = new Matrix<double>(rows,cols);
	// gradientMagnitudes = new Matrix<int>(rows,cols);
	gradientMagnitudes = new Image(*image);
	gradientAngles = new Matrix<double>(rows,cols);
}

void SobelFilter::setValues(	const int & row,
														const int & col,
														const double & magnitudeX,
														const double & magnitudeY,
														Image * imageFiltered,
														const int & threshold)
{
	double magnitude = 0.0;
	double angle = 0.0;
	
	magnitude = (double) sqrt((magnitudeX * magnitudeX) + (magnitudeY * magnitudeY));
	
	if (magnitudeX == 0 && magnitudeY == 0) angle = 999;
	else
		angle = classifyAngle((double) (atan2(magnitudeY,magnitudeX) * 180) / 3.14159265);
	
	horizontalMatrix->setAt(row, col, magnitudeX);
	verticalMatrix->setAt(row, col, magnitudeY);
	if( magnitude > 255 ) magnitude = 255;
	if( magnitude < 0 ) magnitude = 0;
	// gradientMagnitudes->setAt(row, col, magnitude);
	// gradientMagnitudes->setAt(row, col, (int) aproxNumber(magnitude));
	gradientMagnitudes->setPixel(row, col, (int) aproxNumber(magnitude));
	gradientAngles->setAt(row, col, angle);
	
	if(magnitude >= threshold) imageFiltered->setPixel(row, col, 255);
	else imageFiltered->setPixel(row, col, 0);
	
}

Image * SobelFilter::nonMaximalSuppression(Image * image)
{
	
	// Image * nonMaximal = new Image(image->getWidth(), image->getHeight(), Image::P2, 255, 0);
	// Image * nonMaximal = new Image(*image);
	// nonMaximal->setMatrix(gradientMagnitudes);
	Image * nonMaximal = new Image(*gradientMagnitudes);
	
	
	int start = 1;
	int endRow = gradientMagnitudes->getRows() - 1;
	int endCol = gradientMagnitudes->getCols() - 1;
	double angle = 0;
	
	for(int rowIdx = start; rowIdx < endRow; ++rowIdx){
		for(int colIdx = start; colIdx < endCol; ++colIdx){
			angle  = gradientAngles->getAt(rowIdx, colIdx);
			if(angle == 45 || angle == 225) {
				if(equalDirection(angle, gradientAngles->getAt(rowIdx-1,colIdx-1), gradientAngles->getAt(rowIdx+1, colIdx+1))){
					if(	gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx+1, colIdx+1) ||
							gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx-1, colIdx-1))
					{
						nonMaximal->setPixel(rowIdx, colIdx, 0);
					}
				}
			}
			else if(angle == 90 || angle == 270) {
				if(equalDirection(angle, gradientAngles->getAt(rowIdx,colIdx-1), gradientAngles->getAt(rowIdx, colIdx+1))){
					if(	gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx, colIdx-1) &&
							gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx, colIdx+1))
					{
						nonMaximal->setPixel(rowIdx, colIdx, 0);
					}
				}
			}
			else if(angle == 125 || angle == 315) {
				if(equalDirection(angle, gradientAngles->getAt(rowIdx,colIdx), gradientAngles->getAt(rowIdx, colIdx))){
					if(	gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx-1, colIdx+1) ||
							gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx+1, colIdx-1))
					{
						nonMaximal->setPixel(rowIdx, colIdx, 0);
					}
				}
			}
			else if(angle == 180 || angle == 0) {
				if(equalDirection(angle, gradientAngles->getAt(rowIdx,colIdx), gradientAngles->getAt(rowIdx, colIdx))){
					if(	gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx-1, colIdx) ||
							gradientMagnitudes->getPixel(rowIdx, colIdx) < gradientMagnitudes->getPixel(rowIdx+1, colIdx))
					{
						nonMaximal->setPixel(rowIdx, colIdx, 0);
					}
				}
			}
			else {
				nonMaximal->setPixel(rowIdx, colIdx, 0);
			}
		}
	}
	return nonMaximal;
}

bool SobelFilter::equalDirection(const double & dir1, const double & dir2, const double & dir3){
	if(dir1 == dir2 && dir1 == dir3) return true;
	return false;
}
