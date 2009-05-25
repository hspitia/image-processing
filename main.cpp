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
	
	/*
	// FUNCIONES DE LA CLASE IMAGEN
	// ---------------------------------------------------------------------------------
	// Lectura archivos ppm
	// Image * imagenPpm = new Image("data/Lena.ppm");
	Image * imagenPpm = new Image(argv[1]);
	
	// Escritura de imagen y conversiOn de color a grises
	imagenPpm->save("data/01_ColorAGrises.pgm", Image::P2);
	
	
	Image * imagen = new Image(argv[2]);
	// FUNCIONES CLASE HISTOGRAM
	// ---------------------------------------------------------------------------------
	// Calculo de histograma de una imagen
	Histogram * hist = new Histogram(imagen);
	cout << "Calculo de histograma de una imagen" << endl;
	
	// GeneraciOn de imagen de histograma
	Image imgHistograma = hist->paintHistogram();
	imgHistograma.save("data/02_histograma.pgm");
	cout << "Generaci�n de imagen de histograma" << endl;
	
	// EcualizaciOn de una imagen a travEs de su histograma
	Image imgEcualizada = hist->equalize(imagen);
	imgEcualizada.save("data/03_Ecualizada.pgm");
	cout << "Ecualizaci�n de una imagen a travEs de su histograma" << endl;
	
	// FUNCIONES CLASE SEGMENTATION
	// ---------------------------------------------------------------------------------
	// C�lculo de umbral por m�todo Isodata 
	int thresholdIsodata = Segmentation::thresholdingIsodata(imagen);
	cout << "// C�lculo de umbral por mEtodo Isodata" << endl;
	
	// C�lculo de umbral por mEtodo Otsu
	int thresholdOtsu = Segmentation::thresholdingOtsu(imagen);
	cout << "// C�lculo de umbral por m�todo Otsu" << endl;
	
	// Segmentaci�n con umbral Isodata
	Image imgSegmentadaIsodata = Segmentation::segmentate((*imagen), thresholdIsodata);
	imgSegmentadaIsodata.save("data/04_SegmentedIsodata.pgm");
	cout << "// Segmentaci�n con umbral Isodata" << endl;
	
	// Segmentaci�n con umbral Otsu
	Image imgSegmentadaOtsu = Segmentation::segmentate((*imagen), thresholdOtsu);
	imgSegmentadaOtsu.save("data/05_SegmentedOtsu.pgm");
	cout << "// SegmentaciOn con umbral Otsu" << endl;
	*/
	
	// Filtros paso bajo
	// Filtro media
	// Image * imagen = new Image(argv[1]);
	// Image imgFilterd = LowPassFilters::mean(imagen);
	// imgFilterd.save("data/06_MeanFiltered.pgm");

	// Matrix<double> mask = LowPassFilters::createMeanMask(3);
	// cout<<mask<<endl; 
	
	// int x = 0;
	// int y = 0;
	
	// while (x != 99 && y != 99){
	// 	cout << "x = ";
	// 	cin >> x;
	// 	cout << "y = ";
	// 	cin >> y;
		
	// 	if (x != 99 || y != 99) cout<<"gauss: "<<gauss(x,y)<<endl;
	// }
	
	SobelFilter sobel = SobelFilter(40,128);
	Image * img = new Image(argv[1]); 
	Image filtered = sobel.sobel(img,80);
	
	filtered.save("data/07_SobelFiltered.pgm");
	Image * imgSobel = sobel.getSobelImage();	
	Image * colorSobel = sobel.createAnglesColorImage(SobelFilter::SOBEL);
	imgSobel->save("data/07_SobelFiltered_2.pgm");
	colorSobel->save("data/08_ColorSobelFiltered.pgm");
	Image * imgNonMaximal = sobel.getNonMaximalImage();
	imgNonMaximal->save("data/09_nonMaximalFiltered.pgm");
	
	Image * colorNonMaximal = sobel.createAnglesColorImage(SobelFilter::NON_MAXIMAL);
	colorNonMaximal->save("data/10_colorNonMaximalFiltered.pgm");
  
  
  
	return 0;//status
}

double gauss(int x, int y)
{
		double sigma = 1.0;
		double sigma2 = 2 * pow(sigma,2); 
		return (1/(sigma2*3.1416)) * exp(((-1)*((pow((double)x,2)+pow((double)y,2))))/sigma2);
}
