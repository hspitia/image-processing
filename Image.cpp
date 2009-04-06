#include "Image.h"

Image::Image()
{
  matrix = NULL;
	format = NONE;
	rows = 0;
	cols = 0;
	maxLevel = 0;
	width = 0;
	height = 0;
}

Image::Image(const Image & imageObject)
{
	if (imageObject.matrix != NULL) matrix = new Matrix<int>(*(imageObject.matrix));
	else matrix = NULL;
	
	lut = QVector<int>(imageObject.lut);
	format = imageObject.format;
	rows = imageObject.rows;
	cols = imageObject.cols;
	maxLevel = imageObject.maxLevel;
	width = imageObject.width;
	height = imageObject.height;
}

Image::Image(const QString & fileName)
{
  matrix = NULL;
	format = NONE;
	rows = 0;
	cols = 0;
	maxLevel = 0;
	width = 0;
	height = 0;
	load(fileName);
}

Image::Image(const int & width,  const int & height, format_t format, const int & maxLevel, const int & backgroundLevel)
{
	require(backgroundLevel < maxLevel, "Image::Image. The background level must be lesser or equal than maxLevel.");
	rows = height; 
	cols = width;
  if(format == P3 || format == P6) cols = width * 3;
	
	
	matrix = new Matrix<int>(rows, cols, backgroundLevel);
	this->format = format;
	this->maxLevel = maxLevel;
	this->width = width;
	this->height = height;
	initLut();
	// cout<<"-- DEBUG -- "<<endl;
} 

Image::~Image()
{
	if (matrix != NULL){
		delete  matrix;
		matrix = NULL;
	}
}

bool Image::load(const QString & fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		cout<<"Image::Error al load el file \""<<qPrintable(fileName)<<"\". Archivo inexistente."<<endl;
		return false;
	}
	
	QTextStream in(&file);
	if (parseHeader(in)){
		
		initLut();
		
		if(!parseData(in)) {
			cout<<"Image::Error al load el file \""<<qPrintable(fileName)<<"\". Final de file inesperado."<<endl;
			return false;
		}
	}
	else {
		cout<<"Image::Error al load el file \""<<qPrintable(fileName)<<"\". Formato de file no valido."<<endl;
		return false;
	}
	
	return true;
}

bool Image::parseData(QTextStream & in)
{
	matrix = new Matrix<int>(rows, cols);
	int value = 0;
	
	for(int i = 0; i < rows; i++ ){
		for(int j = 0; j < cols; j++ ){
			if(in.atEnd()) return false;
			in >> value;
			matrix->setAt(i, j, lut.at(value));
		}
	}

	return true;
}

bool Image::parseHeader(QTextStream & in)
{
	int headerLines = 3;
	int readLines = 0;
	bool ok;
	
	while (readLines < headerLines && !in.atEnd()) {
		QString line = in.readLine();
		
		if(!line.startsWith("#")) {
			if(readLines == 0){
				if(QString::compare(line, "P2") == 0){
					format = Image::P2;
				}
				else if(QString::compare(line, "P3") == 0){
					format = Image::P3;
				}
				else if(QString::compare(line, "P5") == 0){
					format = Image::P5;
				}
				else if(QString::compare(line, "P6") == 0){
					format = Image::P6;
				}
				else 
					return false;
				
				readLines++;
			}
			
			else if(readLines == 1){
				QStringList list = line.split(" ");
				
				if(list.size() != 2) return false;
				
				width = list[0].toInt(&ok);
				cols = width;
				if(format == P3 || format == P6) cols = width * 3;
				
				height = list[1].toInt(&ok);
				rows = height;
				
				if(height <= 0 || width <= 0) return false;
				
				readLines++;
			}
			
			else if(readLines == 2){
				maxLevel = line.toInt(&ok);
				
				if(maxLevel <= 0) return false;
				
				readLines++;
			}
		}
	}
	return true;
}

bool Image::save(const QString & fileName, format_t formatValue)
{
	if(matrix == NULL){
		cout<<"Image::save. Error al save la imagen.  No se ha cargado ningun file.";
		return false;
	}
	
	if(format == P2 && formatValue == P3){
		cout<<"No se puede convertir a format PPM desde PGM.";
		return false;
	}
	else{
		QFile data(fileName);
		if (data.open(QFile::WriteOnly | QFile::Truncate)) {
			QTextStream out(&data);
			
			writeHeader(out, formatValue);
			writeData(out, formatValue);
		}
	}
	
	return true;
}


void Image::writeData(QTextStream & out, format_t formatValue)
{
	// cout<<"formatValue: "<<qPrintable(formatToString(formatValue))<<endl;
	if(formatValue == SAME){
		int value = 0;
		for(int i = 0; i < rows; i++ ){
			for(int j = 0; j < cols; j++ ){
				value = lut.at(matrix->getAt(i,j));
				out << value<<"\n";
			}
		}
	}
	else //if(format == P3 && formatValue == P2)
	{
		int grayPixel = 0;
		for(int i = 0; i < rows; i++ ){
			for(int j = 0; j < cols; j+=3 ){
				
				grayPixel = (int)(	(0.299*lut.at(matrix->getAt(i,j))) + 
													(0.587*lut.at(matrix->getAt(i,j+1))) + 
													(0.114*lut.at(matrix->getAt(i,j+2))));
				
				out << grayPixel<<"\n";
			}
		}
	}
}

void Image::writeHeader(QTextStream & out, format_t formatValue)
{
	// cout<<qPrintable(formatToString(formatValue))<<endl;
	out << formatToString(formatValue) <<"\n";
	out << "# CREATOR: ImageProcessing v0.1 - hfabioen UNIVALLE"<<"\n";
	out << width <<" "<<height<<"\n";
	out << maxLevel<<"\n";
}

int Image::getFormat(){
	return format;
}

QString Image::formatToString(format_t formatValue){
	QString str = "";
	switch(formatValue){
		case P2: {	
			str = "P2";
			break;
		}
		case P3: {	
			str = "P3";
			break;
		}
		case P5: {	
			str = "P5";
			break;
		}
		case P6: {	
			str = "P6";
			break;
		}
		case SAME: {	
			str = formatToString(this->format);
			break;
		}
		default:
			str = "NONE";
	}
	return str;
}

void Image::setPixel(const int & x, const int & y, const int & greyLevel)
{
	matrix->setAt(x,y,lut.at(greyLevel));
}

void Image::setPixel(	const int & x, 
											const int & y, 
											const int & red, 
											const int & green, 
											const int & blue)
{
	int trueY = y * 3;
	matrix->setAt(x, trueY, lut.at(red));
	matrix->setAt(x, trueY+1, lut.at(green));
	matrix->setAt(x, trueY+2, lut.at(blue));
}

int Image::getPixel(const int & x, const int & y)
{
	// int row = height - y;
	// int col = x;
	if ( ((x < 0) || (x > rows)) || (((y < 0) || (y > cols))) )	{
		cerr<<" Image::getPixel:: IndexOutOfBounds ["<< x<<"]["<< y<<"] in ["<<rows<<"]["<<cols<<"]"<<endl;
		assert(false);
		return false;
	}
	
	// return matrix->getAt(x,y);
	return lut.at(matrix->getAt(x,y));

}

void Image::getPixel(const int & x, const int & y, int & green, int & red, int & blue)
{
	if ( ((x < 0) || (x > rows)) || (((y < 0) || (y > cols))) )	{
		cerr<<" Image::getPixel:: IndexOutOfBounds ["<< x<<"]["<< y<<"] in ["<<rows<<"]["<<cols<<"]"<<endl;
		assert(false);
	}
	
	int trueY = y * 3;
	red = lut.at(matrix->getAt(x,trueY));
	green = lut.at(matrix->getAt(x,trueY+1));
	blue = lut.at(matrix->getAt(x,trueY+2));
}

void Image::initLut()
{
	lut = QVector<int> (maxLevel+1);
	for(int i = 0; i < lut.size(); i++ ){
		lut[i] = i;
	}
}


int Image::getRows()
{
  return rows;
}


int Image::getCols()
{
  return cols;
}

int Image::getMaxLevel()
{
  return maxLevel;
}

int Image::getWidth()
{
  return width;
}

int Image::getHeight()
{
  return height;
}

void Image::setLut(const QVector<int> & lut){
	this->lut = QVector<int>(lut);
}

QVector<int> & Image::getLut(){
	return lut;
} 

Matrix<int> * Image::getMatrix()
{
	return matrix;
}
