#include <cmath>
#include "utils/Array.h"
#include "utils/Matrix.h"
#include "Histogram.h"
#include "Image.h"
#include "Segmentation.h"
#include "LowPassFilters.h"
#include "SobelFilter.h"
#include <QVector>

double gauss(int x, int y);

int main( int argc, char * argv[] )
{
	
	// Ejemplo de la utilizacion de la clase SobelFilter:
	
  // creacion del ojeto
	SobelFilter sobel = SobelFilter(20,185);
	// Imagen original
  Image * img = new Image(argv[1]);
  // Aplicación filtro a la imagen
	Image filtered = sobel.sobel(img,80);
	// Se guarda la imagen filtrada en disco
	filtered.save("data/07_SobelFiltered.pgm");
	// Puede accederse a la misma imagen de esta manera:
  // Image * imgSobel = sobel.getSobelImage();	
  
  // Creación de imágenes de angulos a color and partir de las imagenes
  // generadas durante el proceso:
  // imagen de angulos sobel (primer paso)
	Image * colorSobel = sobel.createAnglesColorImage(SobelFilter::SOBEL);
	colorSobel->save("data/08_ColorSobelFiltered.pgm");
  
  // Se guarda la imagen producto de NonMaximal Suppression en disco
	Image * imgNonMaximal = sobel.getNonMaximalImage();
	imgNonMaximal->save("data/09_nonMaximalFiltered.pgm");
	
  // Se crea y guarda la imagen deangulos a color 
  // producto de NonMaximal Suppression en disco
	Image * colorNonMaximal = sobel.createAnglesColorImage(SobelFilter::NON_MAXIMAL);
	colorNonMaximal->save("data/10_colorNonMaximalFiltered.pgm");
  
  // Se guardan los caminos en disco
  // sobel.saveEdgePaths("data/paths.txt");
  
  // Se otiene y guarda la imagen producto de Hysteresis en disco
  // IMAGEN PARCIAL --- NO DEFINITIVA
  Image * imgHysteresis = sobel.getHysteresisImage();
  imgHysteresis->save("data/11_hysteresisFiltered.pgm");
  
  // Se guarda en disco un archivo texto con la matriz de Angulos
  // Cualquier matriz se puede gauradar en disco
  sobel.getGradientAngles()->save("data/matrix.txt");
  
	return 0;//status
}

double gauss(int x, int y)
{
		double sigma = 1.0;
		double sigma2 = 2 * pow(sigma,2); 
		return (1/(sigma2*3.1416)) * exp(((-1)*((pow((double)x,2)+pow((double)y,2))))/sigma2);
}
