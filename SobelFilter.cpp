#include "SobelFilter.h"

SobelFilter::SobelFilter(){
	// createMasks();
	verticalMask = NULL;
	horizontalMask = NULL;
	
	verticalMatrix = NULL;
	horizontalMatrix = NULL;
	gradientMagnitudes = NULL;
	gradientAngles = NULL;
	nonMaximalMagnitudes = NULL;
	nonMaximalAngles = NULL;
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
	Image * colorImgFiltered = new Image(image->getWidth(), image->getHeight(), Image::P3, 255, 0);
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
			setValues(rowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, colorImgFiltered, threshold);
		}
	}
	
	
	// Convolution for columns out borders (left and right)
	int rightColIdx = endCol;
	int colIdx = 0;
	for(int rowIdx = 1; rowIdx < endRow; ++rowIdx){
		// Upper row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *verticalMask);
		setValues(rowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, colorImgFiltered, threshold);
		
		// Lower row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, rightColIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, rightColIdx, image, *verticalMask);
		setValues(rowIdx, rightColIdx, magnitudeX, magnitudeY, imageFiltered, colorImgFiltered, threshold);
	}
	
	
	// Convolution for rows out borders (up and bottom)
	endCol++;
	int lowerRowIdx = endRow;
	int rowIdx = 0;
	
	for(int colIdx = 0; colIdx < endCol; ++colIdx){
		// Upper row
		magnitudeX = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(rowIdx, colIdx, image, *verticalMask);
		setValues(rowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, colorImgFiltered, threshold);
		
		// Lower row
		magnitudeX = ConvolutionOperation::convolutionBorder(lowerRowIdx, colIdx, image, *horizontalMask);
		magnitudeY = ConvolutionOperation::convolutionBorder(lowerRowIdx, colIdx, image, *verticalMask);
		setValues(lowerRowIdx, colIdx, magnitudeX, magnitudeY, imageFiltered, colorImgFiltered, threshold);
	}
	nonMaximalSuppression(2);
	
	// Test
	
	int nonMaximalAngle = 0;
	// Image * imgSuppressed = new Image(*image);
	Image * imgSuppressed = new Image(image->getWidth(), image->getHeight(), Image::P3, 255, 0);
	for(int row = 0; row < nonMaximalMagnitudes->getRows(); ++row){
		for(int col = 0; col < nonMaximalMagnitudes->getCols(); ++col){
			if( nonMaximalMagnitudes->getAt(row,col) >= threshold) {
				// imgSuppressed->setPixel(row, col, 255);
				
				nonMaximalAngle = gradientAngles->getAt(row,col);
				
				if( nonMaximalAngle == 135 || nonMaximalAngle == 315 ) {
					imgSuppressed->setPixel(row,col,0,255,0);		//Green
				}
				else if( nonMaximalAngle == 180 || nonMaximalAngle == 360 ){
					imgSuppressed->setPixel(row,col,0,0,255);		//Blue
				}
				else if( nonMaximalAngle == 45 || nonMaximalAngle == 225 ){
					imgSuppressed->setPixel(row,col,255,0,0);		//Red
				}
				else if( nonMaximalAngle == 90 || nonMaximalAngle == 270 ){
					imgSuppressed->setPixel(row,col,255,255,0);	//Yellow
				}
				
			}
			// else imgSuppressed->setPixel(row, col, 0);
			
			
		}
	}
	imgSuppressed->save("data/nonMaximal.ppm");
	// imgSuppressed->save("data/nonMaximal.pgm");
	colorImgFiltered->save("data/colorSobelFiltered.ppm");
	
	/*
	cout<<"Horizontal Mask"<<endl<<horizontalMask<<endl<<endl;
	cout<<"Vertical Mask"<<endl<<verticalMask<<endl<<endl;
	cout<<"IMAGEN"<<endl<<(image->getMatrix())<<endl<<endl;
	cout<<"ANGULOS"<<endl<<gradientAngles<<endl<<endl;
	cout<<"MAGNITUDES"<<endl<<gradientMagnitudes<<endl<<endl;
	cout<<"VERTICAL"<<endl<<verticalMatrix<<endl<<endl;
	cout<<"HORIZONTAL"<<endl<<horizontalMatrix<<endl<<endl;
	*/
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

int SobelFilter::quantizeAngle(double angle)
{
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
														Image * imageFiltered,
														Image * colorImgFiltered,
														const int & threshold)
{
	double magnitude = 0.0;
	int angle = 0;
	
	magnitude = (double) sqrt((magnitudeX * magnitudeX) + (magnitudeY * magnitudeY));
	double angleTmp = 0.0;
	
	if (magnitudeX == 0 && magnitudeY == 0) angle = 0; //No change, then No direction
	else{
		angleTmp = (atan2(magnitudeY,magnitudeX) * 180) / 3.14159265;
		angle = quantizeAngle(angleTmp);
	}
	
	horizontalMatrix->setAt(row, col, magnitudeX);
	verticalMatrix->setAt(row, col, magnitudeY);
	gradientMagnitudes->setAt(row, col, (int) magnitude);
	gradientAngles->setAt(row, col, angle);
	
	if(magnitude >= threshold) {
		imageFiltered->setPixel(row, col, 255);
		
		if( angle == 135 || angle == 315 ) {
			colorImgFiltered->setPixel(row,col,0,255,0);		//Green
		}
		else if( angle == 180 || angle == 360 ){
			colorImgFiltered->setPixel(row,col,0,0,255);		//Blue
		}
		else if( angle == 45 || angle == 225 ){
			colorImgFiltered->setPixel(row,col,255,0,0);		//Red
		}
		else if( angle == 90 || angle == 270 ){
			colorImgFiltered->setPixel(row,col,255,255,0);	//Yellow
		}
		
	}
	else imageFiltered->setPixel(row, col, 0);
	
}

void SobelFilter::nonMaximalSuppression(const int & sideNeighbors)
{
	
	nonMaximalMagnitudes = new Matrix<int> (*gradientMagnitudes);
	nonMaximalAngles = new Matrix<int> (gradientMagnitudes->getRows(), gradientMagnitudes->getCols());
	
	int start = sideNeighbors;
	int endRow = gradientMagnitudes->getRows() - sideNeighbors;
	int endCol = gradientMagnitudes->getCols() - sideNeighbors;
	
	for(int row = start; row < endRow; ++row){
		for(int col = start; col < endCol; ++col){
			nonMaximalSuppressionOperation(row, col, sideNeighbors);
		}
	}
	
}

void SobelFilter::nonMaximalSuppressionOperation(const int & row, const int & col, const int & sideNeighbors)
{
	int end	= (sideNeighbors * 2) + 1;	
	int rowIdx = 0;
	int colIdx = 0;
	int rowIncr = 0;
	int colIncr = 0;
	
	int angle = gradientAngles->getAt(row, col);
	int magnitude = gradientMagnitudes->getAt(row, col);
	
	if( angle != 0 ) {
		// if( angle == 135 || angle == 315 ) {
		if( angle == 45 || angle == 225 ) {
			rowIdx = row - sideNeighbors;
			colIdx = col - sideNeighbors;
			rowIncr = 1;
			colIncr = 1;
		}
		// else if( angle == 180 || angle == 360 ){
		else if( angle == 90 || angle == 270 ){
			rowIdx = row;
			colIdx = col - sideNeighbors;
			colIncr = 1;
		}
		// else if( angle == 45 || angle == 225 ){
		else if( angle == 135 || angle == 315 ){
			rowIdx = row - sideNeighbors;
			colIdx = col + sideNeighbors;
			rowIncr = 1;
			colIncr = -1;
		}
		// else if( angle == 90 || angle == 270 ){
		else if( angle == 180 || angle == 360 ){
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
					break;
				}
			}
			rowIdx += rowIncr;
			colIdx += colIncr;
		}
	}
}

