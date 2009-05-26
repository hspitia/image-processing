#include "SobelFilter.h"

const int SobelFilter::MAX_DEPTH = 1; 

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
  //CAMBIO//
	nonMaximalSuppression(3);
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
	//magnitude = (int) sqrt((magnitudeX * magnitudeX) + (magnitudeY * magnitudeY));
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
		// // cout << "DEBUG - dentro setValues" << endl<<"magnitude "<<magnitude<<", threshold "<<threshold<<endl;
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
		/*-*/if( angle == 135 || angle == 315 ) {
		// /*+*/if( angle == 45 || angle == 225 ) {
			rowIdx = row - sideNeighbors;
			colIdx = col - sideNeighbors;
			rowIncr = 1;
			colIncr = 1;
		}
		/*-*/else if( angle == 180 || angle == 360 ){
		// /*+*/else if( angle == 90 || angle == 270 ){
			rowIdx = row;             
			colIdx = col - sideNeighbors;
			colIncr = 1;
		}
		/*-*/else if( angle == 45 || angle == 225 ){
		// /*+*/else if( angle == 135 || angle == 315 ){
			rowIdx = row - sideNeighbors;
			colIdx = col + sideNeighbors;
			rowIncr = 1;
			colIncr = -1;
		}
		/*-*/else if( angle == 90 || angle == 270 ){
		// /*+*/else if( angle == 180 || angle == 360 ){
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
	// Matrix<int> * pointMarks = new Matrix<int>(hysteresisMagnitudes->getRows(), 
																						// hysteresisMagnitudes->getCols(), 0);
	edgePaths = new QVector< QVector<QPoint> > ();
  // int angle = 270;
  // int MAX_DEPTHs = 3;
  // /*1*/cout << "Imagen (maxRow, maxCol): "<< "(" << hysteresisImage->getRows() - 1 << "," << hysteresisImage->getCols() - 1 << ")" << endl;
  // /*1*/cout << "*** checkForEdgePoints(" << angle <<"): ***" << endl;
	// checkForEdgePoints(270);
	// checkForEdgePoints(90);
	// checkForEdgePoints(360);
	// checkForEdgePoints(angle, MAX_DEPTH);
  
	checkForEdgePoints(270);
  checkForEdgePoints(90);
  checkForEdgePoints(360);
  checkForEdgePoints(180);
  
	
}

// void SobelFilter::checkForEdgePoints(const int & gradientNormal, const int & MAX_DEPTH){
void SobelFilter::checkForEdgePoints(const int & gradientNormal){
  QVector<QPoint> currentPath = QVector<QPoint> ();
	int rowStart = 0;
	int colStart = MAX_DEPTH;
	int rowEnd = 0;
	int colEnd = 0;
	
	if(gradientNormal == 270 || gradientNormal == 315){
		rowEnd = nonMaximalMagnitudes->getRows() - MAX_DEPTH;
		colEnd = nonMaximalMagnitudes->getCols() - MAX_DEPTH;
    
    for(int rowIdx = rowStart; rowIdx < rowEnd; ++rowIdx){
      for(int colIdx = colStart; colIdx < colEnd; ++colIdx){
        // verifyThisPoint(rowIdx, colIdx, gradientNormal, maxDepth);
        verifyThisPoint(rowIdx, colIdx, gradientNormal, currentPath);
        // /*1*/cout << "(" << rowIdx << "," << colIdx << ")";
      }
      // /*1*/cout << endl;
    }
  }

	else if(gradientNormal == 90 || gradientNormal == 135){
	  rowStart = nonMaximalMagnitudes->getRows() - 1 - MAX_DEPTH;
    colStart = nonMaximalMagnitudes->getCols() - 1;
    rowEnd = MAX_DEPTH;
    colEnd = MAX_DEPTH;
    
    for(int rowIdx = rowStart; rowIdx >= rowEnd; --rowIdx){
      for(int colIdx = colStart; colIdx >= colEnd; --colIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal, currentPath);
        // /*1*/cout <<  "(" << rowIdx << "," << colIdx << ")";
      }
      // /*1*/cout << endl;
    }
	}
  else if(gradientNormal == 360 || gradientNormal == 45){
	  rowStart = nonMaximalMagnitudes->getRows() - 1 - MAX_DEPTH;
    colStart = 0;
    rowEnd = MAX_DEPTH;
    colEnd = nonMaximalMagnitudes->getCols() - MAX_DEPTH;
    for(int colIdx = colStart; colIdx < colEnd; ++colIdx){
      for(int rowIdx = rowStart; rowIdx >= rowEnd; --rowIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal, currentPath);
        // /*1*/cout <<  "(" << rowIdx << "," << colIdx << ")";
      }
      // /*1*/cout << endl;
    }
	}
  else if(gradientNormal == 180 || gradientNormal == 225){
	  rowStart = 0;
    colStart = nonMaximalMagnitudes->getCols() - 1 - MAX_DEPTH;
    rowEnd = nonMaximalMagnitudes->getRows() - MAX_DEPTH;
    colEnd = MAX_DEPTH;
    
    for(int colIdx = colStart; colIdx >= colEnd; --colIdx){
      for(int rowIdx = rowStart; rowIdx < rowEnd; ++rowIdx){
        verifyThisPoint(rowIdx, colIdx, gradientNormal, currentPath);
        // /*1*/cout <<  "(" << rowIdx << "," << colIdx << ")";
      }
      // /*1*/cout << endl;
    }
	}
	
	edgePaths->append(currentPath);
}

void SobelFilter::verifyThisPoint(const int & row, 
                                  const int & col, 
                                  const int & gradientNormal, 
                                  QVector<QPoint> & currentPath){
  int magnitude = 0;
  int tmpNormal = calculateNormal(gradientAngles->getAt(row, col));
  // cout<<"** verifyThisPoint(" << row <<", " << col <<", " << gradientNormal<<")  "<<endl;
  // cout << "   tmpNormal == gradientNormal ? "<< tmpNormal <<" == "<< gradientNormal<< endl;
  if (tmpNormal == gradientNormal){
    // /*2*/cout<<"** verifyThisPoint(" << row <<", " << col <<", " << gradientNormal<<")  "<<endl;
    magnitude = nonMaximalMagnitudes->getAt(row, col);
    if(magnitude < lowThreshold) {
      // /*2*/cout << "   magnitude < lowThreshold ? "<<magnitude<<" < "<<lowThreshold <<"   -> NO BORDE"<<endl;
      hysteresisMagnitudes->setAt(row, col, 0);
      hysteresisImage->setPixel(row, col, backgroundColor);
    }
    else if(magnitude > highThreshold){
      hysteresisImage->setPixel(row, col, edgeColor);
      // /*2*/cout << "   magnitude > lowThreshold ? "<<magnitude<<" > "<<highThreshold <<"  -> BORDE"<<endl;
      // /*2*/cout << "     followThisEdge("<<row<<" , " << col<<" , " << gradientNormal<<")" << endl;
      // followThisEdge(row, col, gradientNormal, maxDepth);
      // currentPath << QPoint(row, col);
      followThisEdge(row, col, gradientNormal, currentPath);
    }
  }
}

// void SobelFilter::followThisEdge(const int & row, const int & col, const int & gradientNormal, const int & MAX_DEPTH){
void SobelFilter::followThisEdge(const int & row, 
                                 const int & col, 
                                 const int & gradientNormal, 
                                 QVector<QPoint> & currentPath){

	// QVector<QPoint> currentPath = QVector<QPoint> ();
	// QPoint currentPoint = QPoint(row, col); // Start of this Path
  // currentPath << currentPoint;
  // currentPath << QPoint(row, col); // Start of this Path
  
	int iterations = 0;                  
  int rowStart = 0;                    
	int colStart = 0;                    //  Search Range
	int rowEnd = 0;                      //
	int colEnd = 0;                      //        m          m -> edge point
  int rowIncr = 0;                     //  o x - - - x o    - -> depth one
  int colIncr = 0;                     //  o x x x x x o    x -> depth two
  int rowIdx = 0;                      //  o o o o o o o    o -> depth tree
  int colIdx = 0;
  // int magnitude = 0;
  // int normal = 0;
  
  QVector<bool> edgePointsFound = QVector<bool>(MAX_DEPTH+1, false);
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
      for(int depth = 1; depth <= MAX_DEPTH; ++depth){
        // /*2*/cout << "      Depth: " << depth <<endl;
        // tour of sides points              //  Search Range
        if (depth > 1) {                     //        m          |            m      
          rowStart = row + 1;                // -> x       x <-   |   -> o           o <-
          rowEnd = row + depth;              //    x x x x x      |   -> o           o <-
          colStart = col - depth;            //                   |      o o o o o o o
          colIncr = depth * 2;               //
          colEnd = colStart + colIncr;
          // /*2*/cout << "        * tour of sides points:" << endl;
          // /*2*/cout << "          inicio (" << rowStart << "," << colStart << ")" <<  "final (" << rowEnd << "," << colEnd << ")";
          // /*2*/cout << " colIcr: " << colIncr <<" rowIncr: " << 1 << endl;
          for(int rowIdx = rowStart; rowIdx < rowEnd; ++rowIdx){
            for(int colIdx = colStart; colIdx <= colEnd; colIdx += colIncr){
              // /*2*/cout << "          rowIdx: " << rowIdx <<"  colIdx: " << colIdx; 
              // /*2*/cout << "  findNeighboringEdgePoint("<< rowIdx <<", "<< colIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
              findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
            }
          }
        }
        
        // tour of row points                //  Search Range
        iterations = (depth * 2) + 1;        //        m      |         m        |            m      
        rowStart = row + depth;              //   -> - - -    |     x       x    |      o           o
        colStart = col - depth;              //               |  -> x x x x x    |      o           o
        colEnd = colStart + iterations;      //               |                  |   -> o o o o o o o
        colIncr = 1;
        rowIncr = 0;
        rowIdx = rowStart;
        
        // /*2*/cout << "        * tour of row points:" << endl;
        // /*2*/cout << "           inicio (" << rowStart << "," << colStart << ")" <<  "final (" << rowEnd << "," << colEnd << ")";
        // /*2*/cout << " colIcr: " << colIncr <<" rowIncr: " << rowIncr << endl;
        for(int colIdx = colStart; colIdx < colEnd; colIdx += colIncr){
          // /*2*/cout << "           rowIdx: " << rowIdx <<"  colIdx: " << colIdx; 
          // /*2*/cout << "  findNeighboringEdgePoint("<< rowIdx <<", "<< colIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
          findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
        }
        
        // LINK POINTS ??
        
      }
      break;
    }
    case 90: {
      for(int depth = 1; depth <= MAX_DEPTH; ++depth){
        // /*2*/cout << "      Depth: " << depth <<endl;
        // tour of sides points              
        if (depth > 1) {                     
          rowStart = row - 1;                
          rowEnd = row - depth;              
          colStart = col - depth;            
          colIncr = depth * 2;               
          colEnd = colStart + colIncr;       
          // /*2*/cout << "        * tour of sides points:" << endl;
          // /*2*/cout << "          inicio (" << rowStart << "," << colStart << ")" <<  "final (" << rowEnd << "," << colEnd << ")";
          // /*2*/cout << " colIcr: " << colIncr <<" rowIncr: " << -1 << endl;
          for(int rowIdx = rowStart; rowIdx > rowEnd; --rowIdx){
            for(int colIdx = colStart; colIdx <= colEnd; colIdx += colIncr){
              // /*2*/cout << "          rowIdx: " << rowIdx <<"  colIdx: " << colIdx; 
              // /*2*/cout << "  findNeighboringEdgePoint("<< rowIdx <<", "<< colIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
              findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
            }
          }
        }
        
        // tour of row points                
        iterations = (depth * 2) + 1;        
        rowStart = row - depth;              
        colStart = col - depth;              
        colEnd = colStart + iterations;      
        colIncr = 1;
        rowIncr = 0;
        rowIdx = rowStart;
        
        // /*2*/cout << "        * tour of row points:" << endl;
        // /*2*/cout << "           inicio (" << rowStart << "," << colStart << ")" <<  "final (" << rowEnd << "," << colEnd << ")";
        // /*2*/cout << " colIcr: " << colIncr <<" rowIncr: " << rowIncr << endl;
        for(int colIdx = colStart; colIdx < colEnd; colIdx += colIncr){
          // /*2*/cout << "           rowIdx: " << rowIdx <<"  colIdx: " << colIdx; 
          // /*2*/cout << "  findNeighboringEdgePoint("<< rowIdx <<", "<< colIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
          findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
        }
        
        // LINK POINTS ??
        
      }
      break;
    }
    case 360: {
      for(int depth = 1; depth <= MAX_DEPTH; ++depth){
        // /*2*/cout << "      Depth: " << depth <<endl;
        // tour of sides points              //  Search Range
        if (depth > 1) {                     //        m          |            m      
          colStart = col + 1;                // -> x       x <-   |   -> o           o <-
          colEnd = col + depth;              //    x x x x x      |   -> o           o <-
          rowStart = row - depth;            //                   |      o o o o o o o
          rowIncr = depth * 2;               //
          rowEnd = rowStart + rowIncr;
          // /*2*/cout << "        * tour of sides points:" << endl;
          // /*2*/cout << "          inicio (" << colStart << "," << rowStart << ")" <<  "final (" << colEnd << "," << rowEnd << ")";
          // /*2*/cout << " rowIcr: " << rowIncr <<" colIncr: " << 1 << endl;
          for(int colIdx = colStart; colIdx < colEnd; ++colIdx){
            for(int rowIdx = rowStart; rowIdx <= rowEnd; rowIdx += rowIncr){
              // /*2*/cout << "          colIdx: " << colIdx <<"  rowIdx: " << rowIdx; 
              // /*2*/cout << "  findNeighboringEdgePoint("<< colIdx <<", "<< rowIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
              findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
            }
          }
        }
        
        // tour of col points                
        iterations = (depth * 2) + 1;        
        colStart = col + depth;              
        rowStart = row - depth;              
        rowEnd = rowStart + iterations;      
        rowIncr = 1;
        colIncr = 0;
        colIdx = colStart;
        
        // /*2*/cout << "        * tour of col points:" << endl;
        // /*2*/cout << "           inicio (" << colStart << "," << rowStart << ")" <<  "final (" << colEnd << "," << rowEnd << ")";
        // /*2*/cout << " rowIcr: " << rowIncr <<" colIncr: " << colIncr << endl;
        for(int rowIdx = rowStart; rowIdx < rowEnd; rowIdx += rowIncr){
          // /*2*/cout << "           colIdx: " << colIdx <<"  rowIdx: " << rowIdx; 
          // /*2*/cout << "  findNeighboringEdgePoint("<< colIdx <<", "<< rowIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
          findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
        }
        
        // LINK POINTS ??
        
      }
      break;
    }
    case 180: {
      for(int depth = 1; depth <= MAX_DEPTH; ++depth){
        // /*2*/cout << "      Depth: " << depth <<endl;
        // tour of sides points              //  Search Range
        if (depth > 1) {                     //        m          |            m      
          colStart = col - 1;                // -> x       x <-   |   -> o           o <-
          colEnd = col - depth;              //    x x x x x      |   -> o           o <-
          rowStart = row - depth;            //                   |      o o o o o o o
          rowIncr = depth * 2;               //
          rowEnd = rowStart + rowIncr;
          // /*2*/cout << "        * tour of sides points:" << endl;
          // /*2*/cout << "          inicio (" << colStart << "," << rowStart << ")" <<  "final (" << colEnd << "," << rowEnd << ")";
          // /*2*/cout << " rowIcr: " << rowIncr <<" colIncr: " << 1 << endl;
          for(int colIdx = colStart; colIdx > colEnd; --colIdx){
            for(int rowIdx = rowStart; rowIdx <= rowEnd; rowIdx += rowIncr){
              // /*2*/cout << "          colIdx: " << colIdx <<"  rowIdx: " << rowIdx; 
              // /*2*/cout << "  findNeighboringEdgePoint("<< colIdx <<", "<< rowIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
              findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
            }
          }
        }
        
        // tour of col points                
        iterations = (depth * 2) + 1;        
        colStart = col - depth;              
        rowStart = row - depth;              
        rowEnd = rowStart + iterations;      
        rowIncr = 1;
        colIncr = 0;
        colIdx = colStart;
        
        // /*2*/cout << "        * tour of col points:" << endl;
        // /*2*/cout << "           inicio (" << colStart << "," << rowStart << ")" <<  "final (" << colEnd << "," << rowEnd << ")";
        // /*2*/cout << " rowIcr: " << rowIncr <<" colIncr: " << colIncr << endl;
        for(int rowIdx = rowStart; rowIdx < rowEnd; rowIdx += rowIncr){
          // /*2*/cout << "           colIdx: " << colIdx <<"  rowIdx: " << rowIdx; 
          // /*2*/cout << "  findNeighboringEdgePoint("<< colIdx <<", "<< rowIdx <<", " << gradientNormal <<", " << depth <<", currentPath, edgePointsFound)" << endl;
          findNeighboringEdgePoint(rowIdx, colIdx, gradientNormal, depth, currentPath, edgePointsFound);
        }
        
        // LINK POINTS ??
        
      }
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
  
	// edgePaths->append(currentPath);
}

void SobelFilter::findNeighboringEdgePoint(const int & row, 
                                           const int & col, 
                                           const int & gradientNormal,
                                           const int & depth,
                                           // const int & MAX_DEPTH,
                                           QVector<QPoint> & currentPath,
                                           QVector<bool> & edgePointsFound){
  int lowRowLimit = MAX_DEPTH;
  int highRowLimit = hysteresisImage->getRows() - MAX_DEPTH - 1;
  int lowColLimit = MAX_DEPTH;
  int highColLimit = hysteresisImage->getCols() - MAX_DEPTH - 1;
  int magnitude = nonMaximalMagnitudes->getAt(row, col);
  int normal = calculateNormal(gradientAngles->getAt(row, col));
  
  // /*3*/cout << "             " << normal <<" == "<< gradientNormal<< " && ";
  // /*3*/cout << lowThreshold << " <= " << magnitude << " <= " << highThreshold <<"?  ";
  if(normal == gradientNormal && magnitude <= highThreshold && magnitude >= lowThreshold){
    // /*3*/cout<<"   SI"<<endl;
    // /*3*/cout<<"                 hysteresisImage(), currentPath << Qpoint()  en el punto ("<<row<<","<<col << ") "<<endl;
    hysteresisImage->setPixel(row, col, edgeColor);  // is edge
    // currentPath << QPoint(row, col);                 // save the point in the current path
    edgePointsFound[0] = true;                       // found a point of edge
    edgePointsFound[depth] = true;                   // depth at wich found the point
    if(row >= lowRowLimit && row <= highRowLimit &&
       col >= lowColLimit && col <= highColLimit)
    {
      // /*3*/cout<<"                 followThisEdge("<<row<<","<<col << ", currentPath)  -- Llamado recursivo"<<endl;
      followThisEdge(row, col, normal, currentPath);  // continue with the search 
    }
    else{
      // /*3*/cout<<"                  -- PARADA : coordenada fuera de los lImites: "<<"(" << lowRowLimit << "," << lowColLimit << ")" <<"(" << highRowLimit << "," << highColLimit << ")"<<endl;
      // currentPath << QPoint(row, col);
    }
    
  }
  else{
    // /*3*/cout<<"   NO"<<endl;
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


void SobelFilter::saveEdgePaths(const char * fileName){
  ofstream outputFile(fileName);
  if(edgePaths != NULL){
    if(!edgePaths->isEmpty()){
      for(int i = 0; i < edgePaths->size(); i++ ){
        outputFile<<"Path "<<i<<endl;
        for(int j = 0; j < edgePaths->at(i).size(); j++ ){
          outputFile<<" ("<<edgePaths->at(i).at(j).x()<<", "<<edgePaths->at(i).at(j).y()<<")";
        }
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


