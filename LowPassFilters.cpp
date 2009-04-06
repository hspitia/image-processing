#include "LowPassFilters.h"


LowPassFilters::LowPassFilters(){
	
}

LowPassFilters::~LowPassFilters(){
	
}


double LowPassFilters::convolutionCenter(const int & row, const int & col, Image * image, Matrix<double> & mask){
	
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

// double LowPassFilters::convolutionBorders(const int & row, const int & col, Image * image, Matrix<double> & mask){
	
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

double LowPassFilters::convolutionBorders(const int & row, const int & col, Image * image, Matrix<double> & mask){
	
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


Image LowPassFilters::convolution(Image * image, Matrix<double> & mask)
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
			newPixelValue = (int) (convolutionBorders(i, j, image, mask));
			imgFiltered->setPixel(i,j, newPixelValue);
			// Lower row
			newPixelValue = (int) (convolutionBorders(idxLowerRow, j, image, mask));
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
			newPixelValue = (int) (convolutionBorders(i, j, image, mask));
			imgFiltered->setPixel(i,j, newPixelValue);
			// Right col
			newPixelValue = (int) (convolutionBorders(i, idxRightCol, image, mask));
			imgFiltered->setPixel(i,idxRightCol, newPixelValue);
		}
	}
	
	return (*imgFiltered);
}

Image LowPassFilters::mean(Image * image, const int & maskSize){
	require(maskSize > 2, "LowPassFilter::mean. The maskSize parameter must be an odd integer greater than 2.");
	require((maskSize % 2) == 1, "LowPassFilter::mean. The maskSize parameter must be an odd integer greater than 2.");
	
	// Image * imgFiltered = new Image((*image));
	Matrix<double> mask = createMeanMask(maskSize);
	
	// double weightMask = maskSize * maskSize;
	// int newPixelValue = 0;
	// int startRow = (maskSize - 1) / 2;
	// int startCol = startRow;
	// int endRow = image->getRows() - (startRow + 1); 
	// int endCol = image->getCols() - (startCol + 1);
	
	// // Convolution for image central area (without borders)
	// for(int i = startRow; i < endRow; ++i){
	// 	for(int j = startCol; j < endCol; ++j){
	// 		newPixelValue = (int) (convolutionCenter(i, j, image, mask) / weightMask);
	// 		imgFiltered->setPixel(i,j, newPixelValue);
	// 	}
	// }
	
	// // Convolution for rows out borders (up and bottom)
	// int idxLowerRow = endRow;
	// endRow = startRow; 
	// endCol = image->getCols();
	// startRow = 0;
	// startCol = 0;
	
	// for(int i = startRow; i < endRow; ++i){
		
	// 	idxLowerRow += i;
	// 	for(int j = startCol; j < endCol; ++j){
	// 		// Upper row
	// 		newPixelValue = (int) (convolutionBorders(i, j, image, mask) / weightMask);
	// 		imgFiltered->setPixel(i,j, newPixelValue);
	// 		// Lower row
	// 		newPixelValue = (int) (convolutionBorders(idxLowerRow, j, image, mask) / weightMask);
	// 		imgFiltered->setPixel(idxLowerRow,j, newPixelValue);
	// 	}
	// }
	
	// // Convolution for columns out borders (left and right)
	// startRow = endRow;
	// startCol = 0;
	// endRow = image->getRows() - startRow; 
	// endCol = startRow;
	// int idxRightCol = image->getCols() - startRow;

	// for(int j = startCol; j < endCol; ++j){
	// 	idxRightCol += j;
	// 	for(int i = startRow; i < endRow; ++i){
	// 		// Left col
	// 		newPixelValue = (int) (convolutionBorders(i, j, image, mask) / weightMask);
	// 		imgFiltered->setPixel(i,j, newPixelValue);
	// 		// Right col
	// 		newPixelValue = (int) (convolutionBorders(i, idxRightCol, image, mask) / weightMask);
	// 		imgFiltered->setPixel(i,idxRightCol, newPixelValue);
	// 	}
	// }
	
	// return (*imgFiltered);
	return convolution(image, mask);
}

Image LowPassFilters::gaussian(Image * image, const int & maskSize){
	Image imgFiltered = Image((*image)); 
	return imgFiltered;
}

double LowPassFilters::gaussFunction(const int & x, const int & y, const double & sigma){
	double sigma2 = 2 * pow(sigma,2); 
	return (1/(sigma2*3.1416)) * exp(((-1)*((pow((double)x,2)+pow((double)y,2))))/sigma2);
}

Matrix<double> LowPassFilters::createMeanMask(const int & size){
	require(size > 2, "LowPassFilter::createMeanMask. The size parameter must be an odd integer greater than 2.");
	require((size % 2) == 1, "LowPassFilter::createMeanMask. The size parameter must be an odd integer greater than 2.");
	
	double value = (double) 1/(size*size);
	Matrix<double> mask (size, size, value);
	// Matrix<double> mask (size,size,1.0);
	
	return mask;
}


Matrix<double> LowPassFilters::createGaussianMask(const double & sigma){
	Matrix<double> mask = Matrix<double> ();
	vector<double> values(25,0.0);
	double sum = 0.0;
	double gaussValue = 0.0;
	double error = 0.05;
	int delta = 1;
	int start = 0;
	int end = 0;
	
	// Central value
	gaussValue = gaussFunction(0, 0, sigma);
	values.push_back(gaussValue);
	
	do
	{
		start = (-1) * delta;
		end = delta;
		
		// Calculates rows (up and bottom)
		for(int i = start; i < end; ++i){
			
			for(int j = start; j < end; ++j){
				
			}
			
		}
		
		++delta;
		
	}
	while((1.0 - sum) > error);
	
	
	
	return mask;
}
