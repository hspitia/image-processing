#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <cmath>
#include <iostream>
#include "utils/Array.h"
#include "Image.h"

using namespace std;

class Histogram {
	
	private:
		Array<int> histogram;
		int numLevels;
		int maxLevel;
		int minLevel;
		int maxFreq;
		double contrast;
		
		void print(std::ostream & outStream) const;
		void calculateContrast();
		
	public:
		Histogram();
		Histogram(const int & numLevelsValue);
		Histogram(const Histogram & histogramObject);
		Histogram(Image * image);
		~Histogram();
		
		void calculate(Image * imagen);
		Histogram* calculatesCum();
		bool isEmpty();
		double average(const int & startIndex, const int & endIndex);
		Image paintHistogram();
		
		static Image equalize(Image * image);
		
		int getLevels();
		void setMaxLevel(const int & maxLevelValue);
		int getMaxLevel();
		void setMinLevel(const int & minLevelValue);
		int getMinLevel();
		int getMaxFreq();
		double getContrast();
		void setAt(const int & index, const int & value);
		int getAt(const int & index);
		
		
		int & operator[] (const int & index);
		int operator[] (const int & index) const;
		friend std::ostream & operator<<(std::ostream & outStream, const Histogram & histogramObject);
		friend std::ostream & operator<<(std::ostream & outStream, const Histogram * histogramObject);
		
		
};
#endif        //  #ifndef HISTOGRAM_H

