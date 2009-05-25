#include "SobelFilter.h"

SobelFilter::SobelFilter(const int & lowThreshold, const int & highThreshold, const int & edgeColor, const int & backgroundColor){
	// createMasks();
  this->lowThreshold = lowThreshold;
  this->highThreshold = highThreshold;
	this->edgeColor = edgeColor;
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
	edgePaths = NULL;
	
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
	
  if(edgePaths != NULL) delete edgePaths;
	edgePaths = NULL;
	
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
	
	int rowEnd = image->getRows() - 1;
	int colEnd = image->getCols() - 1;
	
	double magnitudeX = 0.0;
	double magnitudeY = 0.0;
	
	// Convolution for image central area
	for(int rowIdx = 1; rowIdx < rowEnd; ++rowIdx){
		for(int colIdx = 1; colIdx < colEnd; ++colIdx){
			magnitudeX = ConvolutionOperation::convolutionCenter(rowIdx, colIdx, image, *horizontalMask);
			magnitudeY = ConvolutionOperation::convolutionCenter(rowIdx, colIdx, image, *verticalMask);
			setValues(rowIdx, colIdx, magnitudeX, magnitudeY, threshold);
		}
	}
	
	// Convolution for columns out borders (left and right)
	int rightColIdx = colEnd;
	int colIdx = 0;
	for(int rowIdx = 1; rowIdx < rowEnd; ++rowIdx){
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
	colEnd++;
	int lowerRowIdx = rowEnd;
	int rowIdx = 0;
	
	for(int colIdx = 0; colIdx < colEnd; ++colIdx){
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
  hysteresis();
	
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
		// // cout << "DEBUG - dentro setValues" << endl<<"magnitude "<<magnitude<<", threshold "<<threshold<<endl;
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
	int rowEnd = gradientMagnitudes->getRows() - sideNeighbors;
	int colEnd = gradientMagnitudes->getCols() - sideNeighbors;
	
	for(int row = start; row < rowEnd; ++row){
		for(int col = start; col < colEnd; ++col){
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
		// // cout <<"\n- "<<row<<"," <<col<<" Magnitude: "<<magnitude<<" Angle: "<< angle<< endl;
		for(int i = 0; i < end; ++i){
			// // cout <<"  " <<rowIdx <<"," <<colIdx<<" Magnitude: "<<gradientMagnitudes->getAt(rowIdx, colIdx)<<" Angle: "<<gradientAngles->getAt(rowIdx, colIdx) << endl;
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

void SobelFilter::hysteresis(){
	hysteresisImage = new Image(*nonMaximalImage);
	hysteresisMagnitudes = new Matrix<int> (*nonMaximalMagnitudes);
	// Matrix<int> * pointMarks = new Matrix<int>(hysteresisMagnitudes->getRows(), 
																						// hysteresisMagnitudes->getCols(), 0);
	edgePaths = new QVector< QVector<QPoint> > ();
  int angle = 270;
  cout << "*** checkForEdgePoints(" << angle <<"): ***" << endl;
	// checkForEdgePoints(270);
	// checkForEdgePoints(90);
	// checkForEdgePoints(360);
	checkForEdgePoints(angle);
  
	
}

void SobelFilter::checkForEdgePoints(const int & gradientNormal){
	int rowStart = 0;
	int colStart = 0;
	int rowEnd = 0;
	int colEnd = 0;
	
	if(gradientNormal == 270 || gradientNormal == 315){
		rowEnd = nonMaximalMagnitudes->getRows();
		colEnd = nonMaximalMagnitudes->getCols();
    
    for(int rowIdx = rowStart; rowIdx < rowEnd; ++rowIdx){
      for(int colIdx = rowStart; colIdx < colEnd; ++colIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal);
      }
      // cout<< endl;
    }
  }
    
	else if(gradientNormal == 90 || gradientNormal == 135){
	  rowStart = nonMaximalMagnitudes->getRows() - 1;
    colStart = nonMaximalMagnitudes->getCols() - 1;
    rowEnd = 0;
    colEnd = 0;
    
    for(int rowIdx = rowStart; rowIdx >= rowEnd; --rowIdx){
      for(int colIdx = rowStart; colIdx >= colEnd; --colIdx){
        // verifyThisPoint(rowIdx, colIdx, gradientNormal);
        // cout << "("<<rowIdx <<","<<colIdx<<")";
      }
      // cout<< endl;
    }
	}
  else if(gradientNormal == 360 || gradientNormal == 45){
	  rowStart = nonMaximalMagnitudes->getRows() - 1;
    colStart = 0;
    rowEnd = 0;
    colEnd = nonMaximalMagnitudes->getCols();
    // cout << "colStart: "<<colStart<<"colEnd: " << colEnd  <<"rowStart: "<<rowStart<<"rowEnd: " << rowEnd << endl;
    for(int colIdx = colStart; colIdx < colEnd; ++colIdx){
      for(int rowIdx = rowStart; rowIdx >= rowEnd; --rowIdx){
        // verifyThisPoint(rowIdx, colIdx, gradientNormal);
        // cout << "(" << rowIdx << "," << colIdx << ")";
      }
      // cout<< endl;
    }
	}
  else if(gradientNormal == 180 || gradientNormal == 225){
	  rowStart = 0;
    colStart = nonMaximalMagnitudes->getCols() - 1;
    rowEnd = nonMaximalMagnitudes->getRows();
    colEnd = 0;
    
    // cout << " rowStart: " << rowStart <<" rowEnd: " << rowEnd <<" colStart: " << colStart <<" colEnd: " << colEnd  << endl;
    for(int colIdx = colStart; colIdx >= colEnd; --colIdx){
      for(int rowIdx = rowStart; rowIdx < rowEnd; ++rowIdx){
        // verifyThisPoint(rowIdx, colIdx, gradientNormal);
        // cout << "("<<rowIdx <<","<<colIdx<<")";
      }
      // cout<< endl;
    }
	}
	
	
}

void SobelFilter::verifyThisPoint(const int & row, const int & col, const int & gradientNormal){
  int magnitude = 0;
  int tmpNormal = calculateNormal(gradientAngles->getAt(row, col));
  cout<<"-- DEBUG -- "<<"verifyThisPoint(" << row <<", " << col <<", " << gradientNormal<<")" <<endl;
  cout << "tmpNormal == gradientNormal ? "<< tmpNormal <<" == "<< gradientNormal<< endl;
  if (tmpNormal == gradientNormal){
    magnitude = hysteresisMagnitudes->getAt(row, col);
    if(magnitude < lowThreshold) {
      cout << "magnitude < lowThreshold ? "<<magnitude<<" < "<<lowThreshold << endl<<"  no borde"<<endl;
      hysteresisMagnitudes->setAt(row, col, 0);
      hysteresisImage->setPixel(row, col, backgroundColor);
    }
    else if(magnitude > highThreshold){
      hysteresisImage->setPixel(row, col, edgeColor);
      cout << "magnitude > lowThreshold ? "<<magnitude<<" > "<<highThreshold << endl<<"  borde"<<endl;
      cout << "followThisEdge("<<row<<" , " << col<<" , " << gradientNormal<<")" << endl;
      // followThisEdge(row, col, gradientNormal);
    }
  }
}

void SobelFilter::followThisEdge(const int & row, const int & col, const int & gradientNormal){

	QVector<QPoint> currentPath = QVector<QPoint> ();
	// QPoint currentPoint = QPoint(row, col); // Start of this Path
  // currentPath << currentPoint;
  currentPath << QPoint(row, col); // Start of this Path
  
	int iterations = 0;                  
  int rowStart = 0;                    
	int colStart = 0;                    //  Search Range
	int rowEnd = 0;                      //
	int colEnd = 0;                      //        m          m -> edge point
  int rowIncr = 0;                     //  o x - - - x o    - -> depth one
  int colIncr = 0;                     //  o x x x x x o    x -> depth two
  int maxDepth = 2;                    //  o o o o o o o    o -> depth tree
  int rowIdx = 0;
  int colIdx = 0;
  int magnitude = 0;
  int normal = 0;
  
  QVector<bool> edgePointsFound = QVector<bool>(maxDepth+1, false);
  /**
   * CONTENT DESCRIPTION OF edgPoinsFound VECTOR:
   * 
   *                 false iif    |       found in 
   *                 others are   |      this depth?
   *                 all  false   |  
   *                  -------    --------------------------
   * description  ->   flag        -         x         o
   * value        -> [ false,    false,    false,    false ]
   *                     0         1         2         3
   */
	
	switch (gradientNormal) {
    case 270:{
      for(int depth = 1; depth <= maxDepth; ++depth){
        
        // tour of sides points              //  Search Range
        if (depth > 1) {                     //        m          |            m      
          rowStart = row + 1;                // -> x       x <-   |   -> o           o <-
          rowEnd = row + depth;              //    x x x x x      |   -> o           o <-
          colStart = col - depth;            //                   |      o o o o o o o
          colIncr = depth * 2;               //
          
          for(int rowIdx = rowStart; rowIdx <= rowEnd; ++rowIdx){
            for(int colIdx = colStart; colIdx < colEnd; colIdx += colIncr){
              findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
            }
          }
        }
        
        // tour of row points                //  Search Range
        iterations = (depth + 1) * 2;        //        m      |         m        |            m      
        rowIdx = row + depth;                //   -> - - -    |     x       x    |      o           o
        colStart = col - depth;              //               |  -> x x x x x    |      o           o
                                             //               |                  |   -> o o o o o o o
        
        for(int colIdx = colStart; colIdx <= iterations; ++colIdx){
          findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
        }
        
        // Link points?
        
      }
      break;
    }
    case 90: {
      
      break;
    }
    case 360: {
      
      break;
    }
    case 180: {
      
      break;
    }
    case 45: {
      
      break;
    }
    case 135: {
      
      break;
    }
    case 225: {
      
      break;
    }
    case 315: {
      
      break;
    }
    default:{
      break;
    }
  }
  
	edgePaths->append(currentPath);
}

void SobelFilter::findNeighboringEdgePoint(const int & row, 
                                           const int & col, 
                                           const int & gradientNormal,
                                           const int & depth,
                                           QVector<QPoint> & currentPath,
                                           QVector<bool> & edgePointsFound){
    
  int magnitude = hysteresisMagnitudes->getAt(row, col);
  int normal = calculateNormal(gradientAngles->getAt(row, col));
  if(normal == gradientNormal && magnitude <= highThreshold && magnitude >= lowThreshold){
    hysteresisImage->setPixel(row, col, edgeColor);  // is edge
    currentPath << QPoint(row, col); // save the point in the current path
    edgePointsFound[0] = true; // found a point of edge
    edgePointsFound[depth] = true; // depth at wich found the point
    followThisEdge(row, col, normal);  // continue with the search 
  }
}


int SobelFilter::calculateNormal(const int & angle){
  int normal = angle + 90;
  if (normal > 360) normal -= 360;
  return normal;
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

