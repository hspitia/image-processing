#include "SobelFilter.h"

// const int SobelFilter::MAX_DEPTH = 1;

SobelFilter::SobelFilter(const int & lowThreshold, 
                         const int & highThreshold,
                         const bool & preprocess,
                         const int & maxDepth,
                         const int & edgeColor, 
                         const int & backgroundColor){
	// createMasks();
  
  
  this->lowThreshold = lowThreshold;
  this->highThreshold = highThreshold;
  this->preprocess = preprocess;
  this->maxDepth = maxDepth;
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

Image SobelFilter::sobel(Image * originImage, const int & threshold, const int & preprocessFilterBoxSize){
	
  Image * image = NULL;
  if(originImage->getFormat() == Image::P3) image = originImage->toGray();
  else if(originImage->getFormat() == Image::P2) image = new Image(*originImage);
  
  if(preprocess){
    LowPassFiltering * medianFilter = new LowPassFiltering(preprocessFilterBoxSize);
    image = medianFilter->medianFiltering(image);
  }
	
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
  //CAMBIO//
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
	
  //CAMBIO//
	// magnitude = (int) sqrt((magnitudeX * magnitudeX) + (magnitudeY * magnitudeY));
	magnitude = (int) (abs(magnitudeX) + abs(magnitudeY));
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
		// // // /*1*/cout << "DEBUG - dentro setValues" << endl<<"magnitude "<<magnitude<<", threshold "<<threshold<<endl;
		sobelImage->setPixel(row, col, edgeColor);
		
	}
	else sobelImage->setPixel(row, col, backgroundColor);
	
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
	
  //CAMBIO//
	if( angle != 0 ) {
		if( angle == 135 || angle == 315 ) {
			rowIdx = row - sideNeighbors;
			colIdx = col - sideNeighbors;
			rowIncr = 1;
			colIncr = 1;
		}
		else if( angle == 180 || angle == 360 ){
			rowIdx = row;             
			colIdx = col - sideNeighbors;
			colIncr = 1;
		}
		else if( angle == 45 || angle == 225 ){
			rowIdx = row - sideNeighbors;
			colIdx = col + sideNeighbors;
			rowIncr = 1;
			colIncr = -1;
		}
		else if( angle == 90 || angle == 270 ){
			rowIdx = row - sideNeighbors;
			colIdx = col;
			rowIncr = 1;
		}
		for(int i = 0; i < end; ++i){
       //CAMBIO//
			// if( angle == gradientAngles->getAt(rowIdx, colIdx) ) {
				if( magnitude < gradientMagnitudes->getAt(rowIdx, colIdx) ) {
					nonMaximalMagnitudes->setAt(row, col, 0);
					nonMaximalAngles->setAt(row, col, 0);
					suppressed = true;
					break;
				}
			// }
			rowIdx += rowIncr;
			colIdx += colIncr;
		}
	}
	return suppressed;
}

void SobelFilter::hysteresis(){
	hysteresisImage = new Image(nonMaximalImage->getWidth(), nonMaximalImage->getHeight(), Image::P2, 255, backgroundColor);
	hysteresisMagnitudes = new Matrix<int> (*nonMaximalMagnitudes);
	edgePaths = new QVector< QLinkedList<QPoint> >();
  
  // /*1*/cout << "Bordes en dirección: 270 y 90"<< endl;
  
	checkForEdgePoints(270);
	checkForEdgePoints(90);
  checkForEdgePoints(360);
  checkForEdgePoints(180);
  checkForEdgePoints(45);
  checkForEdgePoints(225);
  checkForEdgePoints(135);
  checkForEdgePoints(315);
  
	
}

void SobelFilter::checkForEdgePoints(const int & gradientNormal){
  
	int rowStart = 0;
	int colStart = maxDepth;
	int rowEnd = 0;
	int colEnd = 0;
	
	if(gradientNormal == 270 || gradientNormal == 315){
		rowEnd = nonMaximalMagnitudes->getRows() - maxDepth;
		colEnd = nonMaximalMagnitudes->getCols() - maxDepth;
    
    for(int rowIdx = rowStart; rowIdx < rowEnd; ++rowIdx){
      for(int colIdx = colStart; colIdx < colEnd; ++colIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal);
      }
    }
  }

	else if(gradientNormal == 90 || gradientNormal == 135){
	  rowStart = nonMaximalMagnitudes->getRows() - 1 - maxDepth;
    colStart = nonMaximalMagnitudes->getCols() - 1;
    rowEnd = maxDepth;
    colEnd = maxDepth;
    
    for(int rowIdx = rowStart; rowIdx >= rowEnd; --rowIdx){
      for(int colIdx = colStart; colIdx >= colEnd; --colIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal);
      }
    }
	}
  else if(gradientNormal == 360 || gradientNormal == 45){
	  rowStart = nonMaximalMagnitudes->getRows() - 1 - maxDepth;
    colStart = 0;
    rowEnd = maxDepth;
    colEnd = nonMaximalMagnitudes->getCols() - maxDepth;
    for(int colIdx = colStart; colIdx < colEnd; ++colIdx){
      for(int rowIdx = rowStart; rowIdx >= rowEnd; --rowIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal);
      }
    }
	}
  else if(gradientNormal == 180 || gradientNormal == 225){
	  rowStart = 0;
    colStart = nonMaximalMagnitudes->getCols() - 1 - maxDepth;
    rowEnd = nonMaximalMagnitudes->getRows() - maxDepth;
    colEnd = maxDepth;
    
    for(int colIdx = colStart; colIdx >= colEnd; --colIdx){
      for(int rowIdx = rowStart; rowIdx < rowEnd; ++rowIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal);
      }
    }
	}
	
}

void SobelFilter::verifyThisPoint(const int & row, 
                                  const int & col, 
                                  const int & gradientNormal)
{
  QLinkedList<QPoint> newPath = QLinkedList<QPoint>();
  int normal = calculateNormal(gradientAngles->getAt(row, col));
  if (normal == gradientNormal){
    int magnitude = nonMaximalMagnitudes->getAt(row, col);
    
    if(magnitude > 0 && magnitude < lowThreshold) { //No edge
      // // /*1*/cout << "\t\tno " << endl;
      hysteresisMagnitudes->setAt(row, col, 0);
      hysteresisImage->setPixel(row, col, backgroundColor);
    }
    else if(magnitude > highThreshold && !isPointMarked(QPoint(row,col))){ // Edge point not included in a path previously
      // /*1*/cout << "(" << row << "," << col << ") - " << "normal: " << normal<< "  ok"<<endl;
      // /*1*/cout << "\tmagnitude: " << magnitude  <<" edge? (" << magnitude << " > " << highThreshold << ")   SI" <<endl;
      hysteresisImage->setPixel(row, col, edgeColor);
      newPath = followThisEdge(row, col, gradientNormal);
      if(!newPath.isEmpty()) edgePaths->append(newPath);
    }
  }
}

/***************************************************************************************/
void SobelFilter::followEdgeVertical(const int & normalGradient, const QPoint & startPoint, QLinkedList<QPoint> & newPath){
	
  int colStart = 0;
	int colEnd = 0;
  int rowIdx = 0;
  int maxMagnitude = 0;
  bool stop = false;
  int lowLimitRow = maxDepth;
  int lowLimitCol = maxDepth;
  int highLimitRow = nonMaximalMagnitudes->getRows() - 1 - maxDepth;
  int highLimitCol = nonMaximalMagnitudes->getCols() - 1 - maxDepth;
  
  QPoint currentPoint = QPoint(startPoint.x(), startPoint.y());
  // /*0*/cout << "\t\t\tfollowEdgeVertical: "<< normalGradient <<" (" << startPoint.x() << "," << startPoint.y() << ")" << endl;
  while( !stop ){
    QPoint * maxPoint = NULL;
    
    for(int depth = 1; depth <= maxDepth; ++depth){
      // /*1*/cout << "\t\t\t\tDepth: " << depth << endl;
      
      /**/
      rowIdx = currentPoint.x() + depth; // For normal 270
      if(normalGradient == 90) rowIdx = currentPoint.x() - depth;
      
      colStart = currentPoint.y() - depth;
      colEnd = colStart + (depth * 2);
      maxMagnitude = 0;
      maxPoint = NULL;
      
      for(int colIdx = colStart; colIdx <= colEnd; ++colIdx){
        // /*1*/cout << "\t\t\t\t\t" "(" << rowIdx << "," << colIdx << ") = ";
        neighborCheckPoint(QPoint(rowIdx, colIdx), normalGradient, maxMagnitude, maxPoint);
        // /*1*/cout << endl;
      }
      if(maxPoint != NULL){
        // /*1*/cout << "\t\t\t\t\t\tmaxPoint: " << "(" << maxPoint->x() << "," << maxPoint->y() << "),  maxMagnitude: " << maxMagnitude  <<endl;
        if(depth == 1 && !isPointMarked((*maxPoint))) {
          newPath.append( *maxPoint );
          hysteresisImage->setPixel(maxPoint->x(), maxPoint->y(), edgeColor);  // the neighbor point is edge
        }
        else {
          linkPoints(startPoint, maxPoint, newPath, 270); // link points between the maxPoint and start 
        }
        break;  // Point found, then the search of neighbors end
      }
      else {
        // /*1*/cout << "\t\t\t\t\t\tmaxPoint: " << "NULL,   maxMagnitude: " << maxMagnitude  <<endl;
      }
    } // end for
    
    if(maxPoint != NULL && 
       maxPoint->x() >= lowLimitRow && maxPoint->x() <= highLimitRow &&
       maxPoint->y() >= lowLimitCol && maxPoint->y() <= highLimitCol)
    { 
      currentPoint = (*maxPoint); // Point found is now the start of the next search
      // /*1*/cout << "\t\t\tNuevo punto de partida: " << "(" << currentPoint.x() << "," << currentPoint.y() << ")" << endl;
    }
    else{
      stop = true; // No point was found; this edge end
    }
    
  } // end while
}// end followEdgeVertical()


/***************************************************************************************/
void SobelFilter::followEdgeHorizontal(const int & normalGradient, const QPoint & startPoint, QLinkedList<QPoint> & newPath){
	
  int rowStart = 0;
	int rowEnd = 0;
  int colIdx = 0;
  int maxMagnitude = 0;
  bool stop = false;
  int lowLimitRow = maxDepth;
  int lowLimitCol = maxDepth;
  int highLimitRow = nonMaximalMagnitudes->getRows() - 1 - maxDepth;
  int highLimitCol = nonMaximalMagnitudes->getCols() - 1 - maxDepth;
  
  QPoint currentPoint = QPoint(startPoint.x(), startPoint.y());
  // /*0*/cout << "\t\t\tfollowEdgeHorizontal: "<< normalGradient <<" (" << startPoint.x() << "," << startPoint.y() << ")" << endl;
  while( !stop ){
    QPoint * maxPoint = NULL;
    
    for(int depth = 1; depth <= maxDepth; ++depth){
      // /*2/cout << "\t\t\t\tDepth: " << depth << endl;
      
      /**/
      colIdx = currentPoint.y() + depth; // For normal 360
      if(normalGradient == 180) colIdx = currentPoint.y() - depth;
      
      rowStart = currentPoint.x() - depth;
      rowEnd = rowStart + (depth * 2);
      maxMagnitude = 0;
      maxPoint = NULL;
      
      for(int rowIdx = rowStart; rowIdx <= rowEnd; ++rowIdx){
        // /*2/cout << "\t\t\t\t\t" "(" << rowIdx << "," << colIdx << ") = ";
        neighborCheckPoint(QPoint(rowIdx, colIdx), normalGradient, maxMagnitude, maxPoint);
        // /*2/cout << endl;
      }
      if(maxPoint != NULL){
        // /*2/cout << "\t\t\t\t\t\tmaxPoint: " << "(" << maxPoint->x() << "," << maxPoint->y() << "),  maxMagnitude: " << maxMagnitude  <<endl;
        if(depth == 1 && !isPointMarked((*maxPoint))) {
          newPath.append( *maxPoint );
          hysteresisImage->setPixel(maxPoint->x(), maxPoint->y(), edgeColor);  // the neighbor point is edge
        }
        else {
          linkPoints(startPoint, maxPoint, newPath, 270); // link points between the maxPoint and start 
        }
        break;  // Point found, then the search of neighbors end
      }
      else {
        // /*2/cout << "\t\t\t\t\t\tmaxPoint: " << "NULL,   maxMagnitude: " << maxMagnitude  <<endl;
      }
    } // end for
    
    if(maxPoint != NULL && 
       maxPoint->x() >= lowLimitRow && maxPoint->x() <= highLimitRow &&
       maxPoint->y() >= lowLimitCol && maxPoint->y() <= highLimitCol)
    { 
      currentPoint = (*maxPoint); // Point found is now the start of the next search
      // /*2/cout << "\t\t\tNuevo punto de partida: " << "(" << currentPoint.x() << "," << currentPoint.y() << ")" << endl;
    }
    else{
      stop = true; // No point was found; this edge end
    }
    
  } // end while
}// end followEdgeHorizontal()

/***************************************************************************************/

void SobelFilter::followEdgeDiagonal(const int & normalGradient, const QPoint & startPoint, QLinkedList<QPoint> & newPath){
	
  int iterations = 0;
  int rowIdx = 0;
  int colIdx = 0;
  int rowIncr = 0;
  int colIncr = 0;
  int maxMagnitude = 0;
  // int colIdx = 0;
  bool stop = false;
  int lowLimitRow = maxDepth;
  int lowLimitCol = maxDepth;
  int highLimitRow = nonMaximalMagnitudes->getRows() - 1 - maxDepth;
  int highLimitCol = nonMaximalMagnitudes->getCols() - 1 - maxDepth;
  
  QPoint currentPoint = QPoint(startPoint.x(), startPoint.y());
  /*0*/cout << "\t\t\tfollowEdgeDiagonal: "<< normalGradient <<" (" << startPoint.x() << "," << startPoint.y() << ")" << endl;
  while( !stop ){
    QPoint * maxPoint = NULL;
    
    for(int depth = 1; depth <= maxDepth; ++depth){
      // /*2/cout << "\t\t\t\tDepth: " << depth << endl;
      
      /**/
      iterations = (depth * 2) + 1;
      rowIdx = currentPoint.x();
      
      if(normalGradient == 315 || normalGradient == 225) rowIncr = 1;
      else rowIncr = -1;
      
      if(normalGradient == 225 || normalGradient == 135){
        colIdx = currentPoint.y() - depth;
        colIncr = 1;
      }
      else{
        colIdx = currentPoint.y() + depth;
        colIncr = -1;
      }
      maxMagnitude = 0;
      for(int i = 0; i < iterations; ++i){
        // /*2/cout << "\t\t\t\t\t" "(" << rowIdx << "," << colIdx << ") = ";
        neighborCheckPoint(QPoint(rowIdx, colIdx), normalGradient, maxMagnitude, maxPoint);
        if(i < depth) rowIdx += rowIncr;
        else colIdx += colIncr;
        // /*2/cout << endl;
      }
      if(maxPoint != NULL){
        // /*2/cout << "\t\t\t\t\t\tmaxPoint: " << "(" << maxPoint->x() << "," << maxPoint->y() << "),  maxMagnitude: " << maxMagnitude  <<endl;
        if(depth == 1 && !isPointMarked((*maxPoint))) {
          newPath.append( *maxPoint );
          hysteresisImage->setPixel(maxPoint->x(), maxPoint->y(), edgeColor);  // the neighbor point is edge
        }
        else {
          linkPoints(startPoint, maxPoint, newPath, 270); // link points between the maxPoint and start 
        }
        break;  // Point found, then the search of neighbors end
      }
      else {
        // /*2/cout << "\t\t\t\t\t\tmaxPoint: " << "NULL,   maxMagnitude: " << maxMagnitude  <<endl;
      }
    } // end for
    
    if(maxPoint != NULL && 
       maxPoint->x() >= lowLimitRow && maxPoint->x() <= highLimitRow &&
       maxPoint->y() >= lowLimitCol && maxPoint->y() <= highLimitCol)
    { 
      currentPoint = (*maxPoint); // Point found is now the start of the next search
      // /*2/cout << "\t\t\tNuevo punto de partida: " << "(" << currentPoint.x() << "," << currentPoint.y() << ")" << endl;
    }
    else{
      stop = true; // No point was found; this edge end
    }
    
  } // end while
}// end followEdgeDiagonal()

/***************************************************************************************/


void SobelFilter::linkPoints(const QPoint & startPoint,
                             QPoint * maxPoint,
                             QLinkedList<QPoint> & newPath,
                             const int & gradientNormal)
{
  
  int iterations = 0;
  int rowIdx = 0;
  int colIdx = 0;
  int colIncr = 0;
  int rowIncr = 0;
  QLinkedList<QPoint>::iterator iter;
  
  
  switch (gradientNormal) {
    case 270: case 90: {
      if(!isPointMarked((*maxPoint))){
        newPath.append(QPoint(maxPoint->x(), maxPoint->y()));
        hysteresisImage->setPixel(maxPoint->x(), maxPoint->y(), edgeColor);
      }
      
      iter = newPath.end();
      iterations = maxPoint->x() - startPoint.x() - 1;
      
      if(colIdx < startPoint.y()) colIncr = 1;
      else if(colIdx > startPoint.y()) colIncr = -1;
      
      rowIdx = maxPoint->x() - 1; // For normal 270
      rowIncr = -1;               // For normal 270
      if(gradientNormal == 90){
        rowIdx = maxPoint->x() + 1;
        rowIncr = 1;
      }
      
      colIdx += colIncr;
      
      for(int i = 0; i < iterations; ++i){
        hysteresisImage->setPixel(rowIdx, colIdx, edgeColor);
        iter = newPath.insert(iter, QPoint(rowIdx, colIdx));
        rowIdx += rowIncr;
        if(colIdx != startPoint.y()) colIdx += colIncr;
      }
      break;
    }
    
    case 360: case 180: {
      if(!isPointMarked((*maxPoint))){
        newPath.append(QPoint(maxPoint->x(), maxPoint->y()));
        hysteresisImage->setPixel(maxPoint->x(), maxPoint->y(), edgeColor);
      }
      
      iter = newPath.end();
      iterations = maxPoint->y() - startPoint.y() - 1;
      
      if(rowIdx < startPoint.x()) rowIncr = 1;
      else if(rowIdx > startPoint.x()) rowIncr = -1;
      
      colIdx = maxPoint->y() - 1; // For normal 360
      colIncr = -1;               // For normal 360
      if(gradientNormal == 180){
        colIdx = maxPoint->y() + 1;
        colIncr = 1;
      }
      
      rowIdx += rowIncr;
      
      for(int i = 0; i < iterations; ++i){
        hysteresisImage->setPixel(rowIdx, colIdx, edgeColor);
        iter = newPath.insert(iter, QPoint(rowIdx, colIdx));
        colIdx += colIncr;
        if(rowIdx != startPoint.y()) rowIdx += rowIncr;
      }
      break;
    }
    
    default:{
      break;
    }
  }
  
}


void SobelFilter::neighborCheckPoint(const QPoint & currentNeighborPoint, 
                                     const int & gradientNormal,
                                     int & maxMagnitude,
                                     QPoint * & maxPoint)
{
  int normal = calculateNormal(gradientAngles->getAt(currentNeighborPoint.x(), currentNeighborPoint.y()));
  int magnitude = nonMaximalMagnitudes->getAt(currentNeighborPoint.x(), currentNeighborPoint.y());
  // /*1*/cout << "normal: " << normal << ", magnitude: " << magnitude<< "  cumple? ";
  if(normal == gradientNormal && magnitude >= lowThreshold){
    // /*1*/cout << " SI "<<endl;
    if(magnitude > maxMagnitude && !isPointMarked(currentNeighborPoint)){
      // /*1*/cout<< "\t\t\t\t\t\tmaxMagnitude: " << maxMagnitude;
      maxMagnitude = magnitude;
      // /*1*/cout<< "  entonces ahora maxMagnitude: "<< maxMagnitude;
      maxPoint = new QPoint(currentNeighborPoint.x(), currentNeighborPoint.y());
      // /*1*/cout << ",  maxpoint: (" << maxPoint->x() << "," << maxPoint->y() << ")" << endl;
    }
  }
}

QLinkedList<QPoint> SobelFilter::followThisEdge(const int & row, 
                                            const int & col,
                                            const int & gradientNormal)
{
  QLinkedList<QPoint> newPath = QLinkedList<QPoint>();
  
	switch (gradientNormal) {
    case 270: case 90: {
        // /*1*/cout << "\t\tfollowthisEdge"<<"(" << row << "," << col << ")" << endl;
        // followEdge270(QPoint(row,col), newPath);
        followEdgeVertical(gradientNormal, QPoint(row,col), newPath);
        break;
    }
    
    case 360: case 180: {
      // /*2*/cout << "\t\tfollowthisEdge"<<"(" << row << "," << col << ")" << endl;
      // followEdge270(QPoint(row,col), newPath);
      followEdgeHorizontal(gradientNormal, QPoint(row,col), newPath);
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
  } //end switch
  
  return newPath;
}

bool SobelFilter::isPointMarked(const QPoint & currentPoint){
  if(hysteresisImage->getPixel(currentPoint.x(), currentPoint.y()) == edgeColor) return true;
  return false;
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


void SobelFilter::saveEdgePaths(const char * fileName){
  ofstream outputFile(fileName);
  QLinkedList<QPoint>::iterator it;
  QLinkedList<QPoint> currentPath = QLinkedList<QPoint>(); 
  if(edgePaths != NULL){
    if(!edgePaths->isEmpty()){
      for(int i = 0; i < edgePaths->size(); i++ ){
        outputFile<<"Path "<< (i+1) <<endl;
        currentPath = edgePaths->at(i);
        for(it = currentPath.begin(); it != currentPath.end(); ++it)
          outputFile<< " (" << (*it).x() << ", " << (*it).y() <<")";
        
        
        outputFile<<endl;
      }
    }
    else {
      outputFile<<"No paths"<<endl;
    }
  }
  else {
    outputFile<<"NULL"<<endl;
  }
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

QVector< QLinkedList<QPoint> > * SobelFilter::getEdgePaths(){
  return edgePaths;
}
