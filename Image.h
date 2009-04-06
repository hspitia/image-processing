#ifndef IMAGE_H
#define IMAGE_H

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QVector>
#include <cassert>
#include <iostream>
#include "utils/Matrix.h"
#include "utils/require.h"

using namespace std;

class Image
{
	public:
    enum format_t {NONE, P2, P3, P5, P6, SAME};
		
		Image();
		Image(const Image & imageObject);
		Image(const QString & fileName);
		Image(	const int & width,
						const int & height,
						format_t format,
						const int & maxLevel,
						const int & backgroundLevel);
		
		virtual ~Image();
		
		virtual bool load(const QString & fileName);
		virtual bool save(const QString & fileName, format_t formatValue=Image::SAME);
		
		virtual int getFormat();
		virtual QString formatToString(format_t);
		
		virtual void setPixel(const int & x, const int & y, const int & nivelGris);
		virtual void setPixel(const int & x, const int & y, const int & red, const int & green, const int & blue);
		
		virtual int getPixel(const int & x, const int & y);
		virtual void getPixel(const int & x, const int & y, int & green, int & red, int & blue);
		
		virtual int getRows();
		virtual int getCols();
		virtual int getMaxLevel();
		virtual int getWidth();
		virtual int getHeight();
		virtual QVector<int> & getLut();
		virtual void setLut(const QVector<int> & lut);
		virtual Matrix<int> * getMatrix();
		
		
	private:
		Matrix<int> * matrix;
    QVector<int> lut;
		
		format_t format;
		int rows;
		int cols;
		int maxLevel;
		int width;
		int height;
		
		//Metodos
		virtual void initLut();
		virtual bool parseHeader(QTextStream & in);
		virtual bool parseData(QTextStream & in);
		virtual void writeHeader(QTextStream & out, format_t formatValue);
		virtual void writeData(QTextStream & out, format_t formatValue);
		
};

#endif // IMAGE_H

