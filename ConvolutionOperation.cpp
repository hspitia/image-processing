#include "ConvolutionOperation.h"


ConvolutionOperation::ConvolutionOperation(){
	
}

ConvolutionOperation::~ConvolutionOperation(){
	
}

double ConvolutionOperation::convolutionCenter(const int & row, const int & col, Image * image, Matrix<double> & mask){
	
	int delta = (mask.getRows() - 1) / 2;
	int end = mask.getRows() - delta ;
	int start = (-1) * ((mask.getRows() - 1) / 2);
	
	double sum = 0.0;
	for(int i = start; i < end; ++i){
		for(int j = start; j < end; ++j){
			sum += mask.getAt(i+delta, j+delta) * image->getPixel(row+i, col+j);
		}
	}
	return sum;
}

// double ConvolutionOperation::convolutionBorder(const int & row, const int & col, Image * image, Matrix<double> & mask){
	
// 	int delta = (mask.getRows() - 1) / 2;
// 	int end = mask.getRows() - delta ;
// 	int start = (-1) * ((mask.getRows() - 1) / 2);
// 	int rowIdx = 0;
// 	int colIdx = 0;
// 	int imgRows = image->getRows();
// 	int imgCols = image->getCols();
	
// 	double sum = 0.0;
// 	for(int i = start; i < end; ++i){
// 		for(int j = start; j < end; ++j){
// 			rowIdx = row+i;
// 			colIdx = col+j;
// 			if (rowIdx < 0) rowIdx += imgRows;  
// 			if (colIdx < 0) colIdx += imgCols;
// 			if (rowIdx >= imgRows) rowIdx -= imgRows;  
// 			if (colIdx >= imgCols) colIdx -= imgCols;
// 			sum += mask.getAt(i+delta, j+delta) * image->getPixel(rowIdx, colIdx);
// 		}
// 	}
// 	return sum;
// }

double ConvolutionOperation::convolutionBorder(const int & row, const int & col, Image * image, Matrix<double> & mask){
	
	int delta = (mask.getRows() - 1) / 2;
	int end = mask.getRows() - delta ;
	int start = (-1) * ((mask.getRows() - 1) / 2);
	int rowIdx = 0;
	int colIdx = 0;
	int imgMaxRow = image->getRows() - 1;
	int imgMaxCol = image->getCols() - 1;
	
	double sum = 0.0;
	for(int i = start; i < end; ++i){
		for(int j = start; j < end; ++j){
			rowIdx = row+i;
			colIdx = col+j;
			if (rowIdx < 0) rowIdx *= (-1);  
			if (colIdx < 0) colIdx *= (-1);
			if (rowIdx > imgMaxRow) rowIdx = (2 * imgMaxRow) - rowIdx;  
			if (colIdx > imgMaxCol) colIdx = (2 * imgMaxCol) - colIdx;
			sum += mask.getAt(i+delta, j+delta) * image->getPixel(rowIdx, colIdx);
		}
	}
	return sum;
}


Image ConvolutionOperation::convolution(Image * image, Matrix<double> & mask)
{
	Image * imgFiltered = new Image((*image));
	int maskSize = mask.getRows();
	int newPixelValue = 0;
	int startRow = (maskSize - 1) / 2;
	int startCol = startRow;
	int endRow = image->getRows() - (startRow + 1); 
	int endCol = image->getCols() - (startCol + 1);
	
	// Convolution for image central area (without borders)
	for(int i = startRow; i < endRow; ++i){
		for(int j = startCol; j < endCol; ++j){
			newPixelValue = (int) (convolutionCenter(i, j, image, mask));
			imgFiltered->setPixel(i,j, newPixelValue);
		}
	}
	
	// Convolution for rows out borders (up and bottom)
	int idxLowerRow = endRow;
	endRow = startRow; 
	endCol = image->getCols();
	startRow = 0;
	startCol = 0;
	
	for(int i = startRow; i < endRow; ++i){
		
		idxLowerRow += i;
		for(int j = startCol; j < endCol; ++j){
			// Upper row
			newPixelValue = (int) (convolutionBorder(i, j, image, mask));
			imgFiltered->setPixel(i,j, newPixelValue);
			// Lower row
			newPixelValue = (int) (convolutionBorder(idxLowerRow, j, image, mask));
			imgFiltered->setPixel(idxLowerRow,j, newPixelValue);
		}
	}
	
	// Convolution for columns out borders (left and right)
	startRow = endRow;
	startCol = 0;
	endRow = image->getRows() - startRow; 
	endCol = startRow;
	int idxRightCol = image->getCols() - startRow;

	for(int j = startCol; j < endCol; ++j){
		idxRightCol += j;
		for(int i = startRow; i < endRow; ++i){
			// Left col
			newPixelValue = (int) (convolutionBorder(i, j, image, mask));
			imgFiltered->setPixel(i,j, newPixelValue);
			// Right col
			newPixelValue = (int) (convolutionBorder(i, idxRightCol, image, mask));
			imgFiltered->setPixel(i,idxRightCol, newPixelValue);
		}
	}
	
	return (*imgFiltered);
}
