#include "Histogram.h"
#include "Image.h"
#include "LowPassFilters.h"
#include "Segmentation.h"
#include "SobelFilter.h"
#include "utils/Array.h"
#include "utils/Matrix.h"
#include <QLinkedList>
#include <QString>
#include <QVector>
#include <cmath>
#include <iostream>

double gauss(int x, int y);

int main( int argc, char * argv[] )
{
	
	// Ejemplo de la utilizacion de la clase SobelFilter:
	
  // creacion del ojeto
	SobelFilter sobel = SobelFilter(3,50);
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
	colorSobel->save("data/08_ColorSobelFiltered.ppm");
  
  // Se guarda la imagen producto de NonMaximal Suppression en disco
	Image * imgNonMaximal = sobel.getNonMaximalImage();
	imgNonMaximal->save("data/09_nonMaximalFiltered.pgm");
	
  // Se crea y guarda la imagen deangulos a color 
  // producto de NonMaximal Suppression en disco
	Image * colorNonMaximal = sobel.createAnglesColorImage(SobelFilter::NON_MAXIMAL);
	colorNonMaximal->save("data/10_colorNonMaximalFiltered.ppm");
  
  // Se guardan los caminos en disco
  // sobel.saveEdgePaths("data/paths.txt");
  
  // Se otiene y guarda la imagen producto de Hysteresis en disco
  // IMAGEN PARCIAL --- NO DEFINITIVA
  Image * imgHysteresis = sobel.getHysteresisImage();
  imgHysteresis->save("data/11_hysteresisFiltered.pgm");
  Image * colorHysteresis = sobel.createAnglesColorImage(SobelFilter::HYSTERESIS);
  colorHysteresis->save("data/12_colorHysteresisFiltered.ppm");
  
  // Se guarda en disco un archivo texto con la matriz de Angulos
  // Cualquier matriz se puede gauradar en disco
  sobel.getGradientAngles()->save("data/gadientAnglesMatrix.txt");
  sobel.getNonMaximalMagnitudes()->save("data/nonMaximalMagnitudes.txt");
  
  sobel.saveEdgePaths("data/paths.txt");
  
  /*
  QLinkedList<int> list = QLinkedList<int>();
  list << 1 << 2 << 3 << 4;
  
  QLinkedList<int>::iterator i;
  for (i = list.begin(); i != list.end(); ++i){
     cout << *i <<" ";
  }
  cout << endl;
  
  QLinkedList<int>::iterator iter;
  iter = list.end();
  
  for(int i = 11; i > 4; --i){
    iter = list.insert(iter, i);
  }
  
  for (i = list.begin(); i != list.end(); ++i){
     cout << *i <<" ";
  }
  cout<< endl;
  */
	return 0;//status
}

