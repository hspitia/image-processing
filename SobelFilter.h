#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include <iostream>
#include <cmath>
#include <QPoint>
#include <QVector>
#include "Image.h"
#include "ConvolutionOperation.h"
#include "utils/Matrix.h"
#include "utils/utils.h"    

using namespace std;

class SobelFilter {
	public:
		enum process_stage_t {SOBEL, NON_MAXIMAL, HYSTERESIS};

	private:
    int lowThreshold;
    int highThreshold;
		int edgeColor;
		int backgroundColor;
		Matrix<double> * verticalMask;
		Matrix<double> * horizontalMask;
		
		Matrix<double> * verticalMatrix;
		Matrix<double> * horizontalMatrix;
		Matrix<int> * gradientMagnitudes;
		Matrix<int> * gradientAngles;
		Matrix<int> * nonMaximalMagnitudes;
		Matrix<int> * nonMaximalAngles;
		Matrix<int> * hysteresisMagnitudes;
    QVector< QVector<QPoint> > * edgePaths;
		
		Image * sobelImage;
		Image * nonMaximalImage;
		Image * hysteresisImage;
		
		void createMasks();
		void initMatrices(Image * image);
		int quantizeAngle(double angle);
		void setValues(	const int & row, 
										const int & col,
										const double & magnitudeX,
										const double & magnitudeY,
										const int & threshold);
		
		void nonMaximalSuppression(const int & sideNeighbors);
		bool nonMaximalSuppressionOperation(const int & row, const int & col, const int & sideNeighbors);
		Image * constructColorImage(Image * originImage);
		void hysteresis();
		// void checkForEdgePoints(const int & gradientNormal, const int & maxDepth);
		void checkForEdgePoints(const int & gradientNormal);
    int calculateNormal(const int & angle);
    // void verifyThisPoint(const int & row, 
                         // const int & col, 
                         // const int & gradientNormal,
                         // const int & maxDepth);
    void verifyThisPoint(const int & row, 
                         const int & col, 
                         const int & gradientNormal);
		// void followThisEdge(const int & row, 
                        // const int & col,
                        // const int & gradientNormal,
                        // const int & maxDepth);
    void followThisEdge(const int & row, 
                        const int & col,
                        const int & gradientNormal);
    // void SobelFilter::findNeighboringEdgePoint(const int & row, 
                                               // const int & col, 
                                               // const int & gradientNormal,
                                               // const int & depth,
                                               // const int & maxDepth,
                                               // QVector<QPoint> & currentPath,
                                               // QVector<bool> & edgePointsFound);
    void SobelFilter::findNeighboringEdgePoint(const int & row, 
                                               const int & col, 
                                               const int & gradientNormal,
                                               const int & depth,
                                               QVector<QPoint> & currentPath,
                                               QVector<bool> & edgePointsFound);
		
	public:
		SobelFilter(const int & lowThreshold, const int & highThreshold, const int & edgeColor = 255, const int & backgroundColor = 0);
		virtual ~SobelFilter();
    
    static const int MAX_DEPTH;
		
		Image sobel(Image * image, const int & threshold);
		
		Matrix<double> * getVerticalMask();
		void setVerticalMask(Matrix<double> * verticalMaskValue);
		Matrix<double> * getHorizontalMask();
		void setHorizontalMask(Matrix<double> * horizontalMaskValue);
		Matrix<double> * getVerticalMatrix();
		void setVerticalMatrix(Matrix<double> * verticalMatrixValue);
		Matrix<double> * getHorizontalMatrix();
		void setHorizontalMatrix(Matrix<double> * horizontalMatrixValue);
		Matrix<int> * getGradientMagnitudes();
		void setGradientMagnitudes(Matrix<int> * gradientMagnitudesValue);
		Matrix<int> * getGradientAngles();
		void setGradientAngles(Matrix<int> * gradientAnglesValue);
		Matrix<int> * getNonMaximalMagnitudes();
		void setNonMaximalMagnitudes(Matrix<int> * nonMaximalMagnitudesValue);
		Matrix<int> * getNonMaximalAngles();
		void setNonMaximalAngles(Matrix<int> * nonMaximalAnglesValue);
		
		Image * getSobelImage();
		Image * getNonMaximalImage();
		Image * getHysteresisImage();
		
		Image * createAnglesColorImage(process_stage_t processStageForImage);
    
};
#endif        //  #ifndef SOBELFILTER_H




