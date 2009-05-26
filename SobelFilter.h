#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include <iostream>
#include <cmath>
#include <fstream>
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
		void setValues(const int & row, 
                   const int & col,
                   const double & magnitudeX,
                   const double & magnitudeY,
                   const int & threshold);
		
		void nonMaximalSuppression(const int & sideNeighbors);
		bool nonMaximalSuppressionOperation(const int & row, const int & col, const int & sideNeighbors);
		Image * constructColorImage(Image * originImage);
		void hysteresis();
		void checkForEdgePoints(const int & gradientNormal);
    int calculateNormal(const int & angle);
    void verifyThisPoint(const int & row, 
                         const int & col, 
                         const int & gradientNormal,
                         QVector<QPoint> & currentPath);
    void followThisEdge(const int & row, 
                        const int & col,
                        const int & gradientNormal,
                        QVector<QPoint> & currentPath);
    void SobelFilter::findNeighboringEdgePoint(const int & row, 
                                               const int & col, 
                                               const int & gradientNormal,
                                               const int & depth,
                                               QVector<QPoint> & currentPath,
                                               QVector<bool> & edgePointsFound);
		
    
    
    
    public:
    /**
    *  Constructor de clase
    */
 		SobelFilter(const int & lowThreshold, 
                const int & highThreshold,
                const int & edgeColor = 255,
                const int & backgroundColor = 0);
		virtual ~SobelFilter();
    
    static const int MAX_DEPTH;
		
    
    /**
     * Ejecuta el filtro sobre la imagen image.  Retorna un objeto Image, el cual es una imagen
     * binarizada de acuerdo al umbral threshold
     */
		Image sobel(Image * image, const int & threshold);
    
    /**
     *  MEtodos de acceso a miebros privados de la clase
     *  Acceso a las matrices producto del proceso
     */
    
		Matrix<double> * getVerticalMask();
		Matrix<double> * getHorizontalMask();
		Matrix<double> * getVerticalMatrix();
		Matrix<double> * getHorizontalMatrix();
		Matrix<int> * getGradientMagnitudes();
		Matrix<int> * getGradientAngles();  // Matriz de Angulos
		Matrix<int> * getNonMaximalMagnitudes();
		Matrix<int> * getNonMaximalAngles();
    
		void setVerticalMask(Matrix<double> * verticalMaskValue);
		void setHorizontalMask(Matrix<double> * horizontalMaskValue);
		void setVerticalMatrix(Matrix<double> * verticalMatrixValue);
		void setHorizontalMatrix(Matrix<double> * horizontalMatrixValue);
		void setGradientMagnitudes(Matrix<int> * gradientMagnitudesValue);
		void setGradientAngles(Matrix<int> * gradientAnglesValue);
		void setNonMaximalMagnitudes(Matrix<int> * nonMaximalMagnitudesValue);
		void setNonMaximalAngles(Matrix<int> * nonMaximalAnglesValue);
		
    /**
    *  Acceso a las imagenes proucto del proceso
    */
		Image * getSobelImage();
		Image * getNonMaximalImage();
		Image * getHysteresisImage();
		
    /**
    *   Crea una imagen de Angulos a color, a partir de la imagen de un estado 
    *   del proceso:
    *   SobelFilter::SOBEL, SobelFilter::NON_MAXIMAL o SobelFilter::HYSTERESIS
    */
		Image * createAnglesColorImage(process_stage_t processStageForImage);
    
    /**
     *  Guarda en else el archivo fielName los puntos de cada camino trazado
     *  al seguir los bordes
     */
    void saveEdgePaths(const char * fileName);
};
#endif        //  #ifndef SOBELFILTER_H




