#include "SobelFilter.h"

SobelFilter::SobelFilter(const int & borderColor, const int & backgroundColor){
	// createMasks();
	this->borderColor = borderColor;
	this->backgroundColor = backgroundColor;
	
	verticalMask = NULL;
	horizontalMask = NULL;
	
	verticalMatrix = NULL;
	horizontalMatrix = NULL;
	
	gradientMagnitudes = NULL;
	gradientAngles = NULL;
	nonMaximalMagnitudes = NULL;
	nonMaximalAngles = NULL;
	hysteresisMagnitudes = NULL;
	
	sobelImage = NULL;
	nonMaximalImage = NULL;
	hysteresisImage = NULL;
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
	
	if(nonMaximalMagnitudes != NULL) delete nonMaximalMagnitudes;
	nonMaximalMagnitudes = NULL;
	
	if(nonMaximalAngles != NULL) delete nonMaximalAngles;
	nonMaximalAngles = NULL;
	
	if(hysteresisMagnitudes != NULL) delete hysteresisMagnitudes;
	hysteresisMagnitudes = NULL;
	
	
	if(sobelImage != NULL) delete sobelImage;
	sobelImage = NULL;
	
	if(nonMaximalImage != NULL) delete nonMaximalImage;
	nonMaximalImage = NULL;
	
	if(hysteresisImage != NULL) delete hysteresisImage;
	hysteresisImage = NULL;
}

Image SobelFilter::sobel(Image * image, const int & threshold){
	
	sobelImage = new Image(*image);
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
			// cout << "DEBUG " <<rowIdx<<", "<<colIdx<< endl;
			setValues(rowIdx, colIdx, magnitudeX, magnitudeY, threshold);
		}
	}
	
	// Convolution for columns out borders (left and right)
	int rightColIdx = endCol;
	int colIdx = 0;
	for(int rowIdx = 1; rowIdx < endRow; ++rowIdx){
		// Upper row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *verticalMask);
		setValues(rowIdx, colIdx, magnitudeX, magnitudeY, threshold);
		
		// Lower row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, rightColIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, rightColIdx, image, *verticalMask);
		setValues(rowIdx, rightColIdx, magnitudeX, magnitudeY, threshold);
	}
	
	// Convolution for rows out borders (up and bottom)
	endCol++;
	int lowerRowIdx = endRow;
	int rowIdx = 0;
	
	for(int colIdx = 0; colIdx < endCol; ++colIdx){
		// Upper row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *verticalMask);
		setValues(rowIdx, colIdx, magnitudeX, magnitudeY, threshold);
		
		// Lower row
		magnitudeX = ConvolutionOperation::convolutionBorder(lowerRowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(lowerRowIdx, colIdx, image, *verticalMask);
		setValues(lowerRowIdx, colIdx, magnitudeX, magnitudeY, threshold);
	}
	nonMaximalSuppression(2);
	
	return (*sobelImage);
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
	
	horizontalMask->setAt(0,0,-1);
	horizontalMask->setAt(0,1,0);
	horizontalMask->setAt(0,2,1);
	horizontalMask->setAt(1,0,-2);
	horizontalMask->setAt(1,1,0);
	horizontalMask->setAt(1,2,2);
	horizontalMask->setAt(2,0,-1);
	horizontalMask->setAt(2,1,0);
	horizontalMask->setAt(2,2,1);
}

int SobelFilter::quantizeAngle(double angle){
	int quantizedAngle = 0;
	
	if(angle <= 0) angle += 360;
	
	if(angle >= 22.5 && angle < 67.5) {
		quantizedAngle = 45;
	}
	else if(angle >= 67.5 && angle < 112.5) {
		quantizedAngle = 90;
	}
	else if(angle >= 112.5 && angle < 157.5) {
		quantizedAngle = 135;
	}
	else if(angle >= 157.5 && angle < 202.5) {
		quantizedAngle = 180;
	}
	else if(angle >= 202.5 && angle < 247.5) {
		quantizedAngle = 225;
	}
	else if(angle >= 247.5 && angle < 292.5) {
		quantizedAngle = 270;
	}
	else if(angle >= 292.5 && angle < 337.5) {
		quantizedAngle = 315;
	}
	else quantizedAngle = 360;
	
	return quantizedAngle;
}

void SobelFilter::initMatrices(Image * image)
{
	int rows = image->getRows();
	int cols = image->getCols();
	verticalMatrix = new Matrix<double>(rows,cols);
	horizontalMatrix = new Matrix<double>(rows,cols);
	// gradientMagnitudes = new Matrix<double>(rows,cols);
	gradientMagnitudes = new Matrix<int>(rows,cols);
	// gradientMagnitudes = new Image(*image);
	gradientAngles = new Matrix<int>(rows,cols);
}

void SobelFilter::setValues(	const int & row,
														const int & col,
														const double & magnitudeX,
														const double & magnitudeY,
														const int & threshold)
{
	// double magnitude = 0.0;
	int magnitude = 0;
	int angle = 0;
	
	magnitude = (int) sqrt((magnitudeX * magnitudeX) + (magnitudeY * magnitudeY));
	double angleTmp = 0.0;
	
	if (magnitudeX == 0 && magnitudeY == 0) angle = 0; //No change, then No direction
	else{
		angleTmp = (atan2(magnitudeY,magnitudeX) * 180) / 3.14159265;
		angle = quantizeAngle(angleTmp);
	}
	
	horizontalMatrix->setAt(row, col, magnitudeX);
	verticalMatrix->setAt(row, col, magnitudeY);
	// gradientMagnitudes->setAt(row, col, (int) magnitude);
	gradientMagnitudes->setAt(row, col, magnitude);
	gradientAngles->setAt(row, col, angle);
	
	if(magnitude >= threshold) {
		// cout << "DEBUG - dentro setValues" << endl<<"magnitude "<<magnitude<<", threshold "<<threshold<<endl;
		sobelImage->setPixel(row, col, 255);
		
	}
	else sobelImage->setPixel(row, col, 0);
	
}

void SobelFilter::nonMaximalSuppression(const int & sideNeighbors)
{
	nonMaximalImage = new Image(*sobelImage);
	nonMaximalMagnitudes = new Matrix<int> (*gradientMagnitudes);
	nonMaximalAngles = new Matrix<int> (gradientMagnitudes->getRows(), gradientMagnitudes->getCols());
	
	int start = sideNeighbors;
	int endRow = gradientMagnitudes->getRows() - sideNeighbors;
	int endCol = gradientMagnitudes->getCols() - sideNeighbors;
	
	for(int row = start; row < endRow; ++row){
		for(int col = start; col < endCol; ++col){
			if(nonMaximalSuppressionOperation(row, col, sideNeighbors)) {
				nonMaximalImage->setPixel(row, col, 0);
			}
		}
	}
	
}

bool SobelFilter::nonMaximalSuppressionOperation(const int & row, const int & col, const int & sideNeighbors)
{
	int end	= (sideNeighbors * 2) + 1;	
	int rowIdx = 0;
	int colIdx = 0;
	int rowIncr = 0;
	int colIncr = 0;
	
	int angle = gradientAngles->getAt(row, col);
	int magnitude = gradientMagnitudes->getAt(row, col);
	bool suppressed = false;
	
	if( angle != 0 ) {
		if( angle == 135 || angle == 315 ) {
		// if( angle == 45 || angle == 225 ) {
			rowIdx = row - sideNeighbors;
			colIdx = col - sideNeighbors;
			rowIncr = 1;
			colIncr = 1;
		}
		else if( angle == 180 || angle == 360 ){
		// else if( angle == 90 || angle == 270 ){
			rowIdx = row;             
			colIdx = col - sideNeighbors;
			colIncr = 1;
		}
		else if( angle == 45 || angle == 225 ){
		// else if( angle == 135 || angle == 315 ){
			rowIdx = row - sideNeighbors;
			colIdx = col + sideNeighbors;
			rowIncr = 1;
			colIncr = -1;
		}
		else if( angle == 90 || angle == 270 ){
		// else if( angle == 180 || angle == 360 ){
			rowIdx = row - sideNeighbors;
			colIdx = col;
			rowIncr = 1;
		}
		// cout <<"\n- "<<row<<"," <<col<<" Magnitude: "<<magnitude<<" Angle: "<< angle<< endl;
		for(int i = 0; i < end; ++i){
			// cout <<"  " <<rowIdx <<"," <<colIdx<<" Magnitude: "<<gradientMagnitudes->getAt(rowIdx, colIdx)<<" Angle: "<<gradientAngles->getAt(rowIdx, colIdx) << endl;
			if( angle == gradientAngles->getAt(rowIdx, colIdx) ) {
				if( magnitude < gradientMagnitudes->getAt(rowIdx, colIdx) ) {
					nonMaximalMagnitudes->setAt(row, col, 0);
					nonMaximalAngles->setAt(row, col, 0);
					suppressed = true;
					break;
				}
			}
			rowIdx += rowIncr;
			colIdx += colIncr;
		}
	}
	return suppressed;
}

void SobelFilter::hysteresis(const int & lowThreshold, const int & highThreshold){
	hysteresisImage = new Image(*nonMaximalImage);
	hysteresisMagnitudes = new Matrix<int> (*nonMaximalMagnitudes);
	
	int endRow = nonMaximalMagnitudes->getrows();
	int endCol = nonMaximalMagnitudes->getCols();
	
	
	
	for(int rowIdx = 0; rowIdx < endRow; ++rowIdx){
		for(int colIdx = 0; colIdx < endcol; ++colIdx){
			if( nonMaximalMagnitudes->getAt(rowIdx, colIdx) > highThreshold ) {
				hysteresisMagnitudes->setAt(rowIdx, colIdx, 1); // Edge
			}
			else if(nonMaximalMagnitudes->getAt(rowIdx, colIdx) < lowThreshold){
			  hysteresisMagnitudes->setAt(rowIdx, colIdx, 0); // No Edge
			}
			else { //if(	nonMaximalMagnitudes->getAt(rowIdx, colIdx) >= lowThreshold &&
						//	nonMaximalMagnitudes->getAt(rowIdx, colIdx) <= highThreshold){
			  hysteresisMagnitudes->setAt(rowIdx, colIdx, 2); // Edge candidate
			}
		}
	}
	
	endRow = hysteresisMagnitudes->getrows();
	endCol = hysteresisMagnitudes->getCols();
	// int magnitude = 0;
	
	for(int rowIdx = 0; rowIdx < endRow; ++rowIdx){
		for(int colIdx = 0; colIdx < endcol; ++colIdx){
			if( hysteresisMagnitudes->getAt(rowIdx, colIdx) == 2) {
				
				if( hysteresisMagnitudes->getAt(rowIdx-1, 	colIdx-1) 	== 1 ||
						hysteresisMagnitudes->getAt(rowIdx-1, 	colIdx) 		== 1 ||
						hysteresisMagnitudes->getAt(rowIdx-1, 	colIdx+1) 	== 1 ||
						hysteresisMagnitudes->getAt(rowIdx, 		colIdx-1) 	== 1 ||
						hysteresisMagnitudes->getAt(rowIdx, 		colIdx+1) 	== 1 ||
						hysteresisMagnitudes->getAt(rowIdx+1, 	colIdx-1) 	== 1 ||
						hysteresisMagnitudes->getAt(rowIdx+1, 	colIdx) 		== 1 ||
						hysteresisMagnitudes->getAt(rowIdx+1, 	colIdx+1) 	== 1 ||
					)
				{
					hysteresisMagnitudes->setAt(rowIdx, colIdx, 1);
				}
				else
					hysteresisMagnitudes->setAt(rowIdx, colIdx, 0);
			}
		}
	}
	
	
}

Image * SobelFilter::createAnglesColorImage(process_stage_t processStageForImage){
	Image * colorAnglesImage = NULL; 
	switch (processStageForImage){
		case SOBEL:{
			colorAnglesImage = constructColorImage(sobelImage);
			break;
		}
		case NON_MAXIMAL:{
			colorAnglesImage = constructColorImage(nonMaximalImage);
			break;
		}
		case HYSTERESIS:{
			colorAnglesImage = constructColorImage(hysteresisImage);
			break;
		}
		default:
			break;
	}
	return colorAnglesImage;
}

Image * SobelFilter::constructColorImage(Image * originImage){
	int rowsEnd = originImage->getRows();
	int colsEnd = originImage->getCols();
	int angle = 0;
	
	Image * colorImage = new Image(originImage->getWidth(), originImage->getHeight(), Image::P3, 255, 0);
	
	for(int row = 0; row < rowsEnd; ++row){
		for(int col = 0; col < colsEnd; ++col){
			if(originImage->getPixel(row, col) != 0){
				angle = gradientAngles->getAt(row,col);
				
				if( angle == 135 || angle == 315 ) {
					colorImage->setPixel(row,col,0,255,0);		//Green
				}
				else if( angle == 180 || angle == 360 ){
					colorImage->setPixel(row,col,0,0,255);		//Blue
				}
				else if( angle == 45 || angle == 225 ){
					colorImage->setPixel(row,col,255,0,0);		//Red
				}
				else if( angle == 90 || angle == 270 ){
					colorImage->setPixel(row,col,255,255,0);	//Yellow
				}
			}
		}
	}
	return colorImage;
}


//Accesors
Matrix<double> * SobelFilter::getVerticalMask()
{
  return verticalMask;
}

void SobelFilter::setVerticalMask(Matrix<double> * verticalMaskValue)
{
  verticalMask = verticalMaskValue;
}

Matrix<double> * SobelFilter::getHorizontalMask()
{
  return horizontalMask;
}

void SobelFilter::setHorizontalMask(Matrix<double> * horizontalMaskValue)
{
  horizontalMask = horizontalMaskValue;
}

Matrix<double> * SobelFilter::getVerticalMatrix()
{
  return verticalMatrix;
}

void SobelFilter::setVerticalMatrix(Matrix<double> * verticalMatrixValue)
{
  verticalMatrix = verticalMatrixValue;
}

Matrix<double> * SobelFilter::getHorizontalMatrix()
{
  return horizontalMatrix;
}

void setHorizontalMatrix(Matrix<double> * horizontalMatrixValue);

void SobelFilter::setHorizontalMatrix(Matrix<double> * horizontalMatrixValue)
{
  horizontalMatrix = horizontalMatrixValue;
}

Matrix<int> * SobelFilter::getGradientMagnitudes()
{
  return gradientMagnitudes;
}

void SobelFilter::setGradientMagnitudes(Matrix<int> * gradientMagnitudesValue)
{
  gradientMagnitudes = gradientMagnitudesValue;
}

Matrix<int> * SobelFilter::getGradientAngles()
{
  return gradientAngles;
}

void SobelFilter::setGradientAngles(Matrix<int> * gradientAnglesValue)
{
  gradientAngles = gradientAnglesValue;
}

Matrix<int> * SobelFilter::getNonMaximalMagnitudes()
{
  return nonMaximalMagnitudes;
}

void SobelFilter::setNonMaximalMagnitudes(Matrix<int> * nonMaximalMagnitudesValue)
{
  nonMaximalMagnitudes = nonMaximalMagnitudesValue;
}

Matrix<int> * SobelFilter::getNonMaximalAngles()
{
  return nonMaximalAngles;
}

void SobelFilter::setNonMaximalAngles(Matrix<int> * nonMaximalAnglesValue)
{
  nonMaximalAngles = nonMaximalAnglesValue;
}

Image * SobelFilter::getSobelImage()
{
  return sobelImage;
}

Image * SobelFilter::getNonMaximalImage()
{
  return nonMaximalImage;
}

Image * SobelFilter::getHysteresisImage()
{
  return hysteresisImage;
}

