#pragma once

#include <GL/GLU.h>
#include "Utilities/Pnt3f.H"
#include <map>
#include <utility>
#include <vector>
#include <ctime>
#include <random>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <FL/glut.h>
#include <opencv2/opencv.hpp>

using namespace std;

namespace 
{
	const float EPSILON = 0.001f;
}


struct FloatCompare 
{
	bool operator()(float a, float b) const 
	{
		return abs(a - b) > EPSILON ? a < b : false;
	}
};

//draw front of the train
void drawTrainFront(bool doingShadows, int r, int g, int b, float angle);

//draw wagons
void drawWagon(bool doingShadows, int r, int g, int b, float angle);

//for drawing sleepers or lines
void drawTrackPart(bool doingShadows, Pnt3f qt, Pnt3f qtLast, Pnt3f orient_t,
	float width, float thickness, float depth, int r, int g, int b);

//drw controlpoint support beams
void drawBeam(bool doingShadows, Pnt3f pos, Pnt3f orient);

//calculate the needed subdivision number
void adaptiveSubdivide(int& divideLine, Pnt3f lastDirection, Pnt3f direction);

//matrix multiplication for curves
Pnt3f multiplyControlPoint(float G[3][4], float M[4][4], float t);

//returns reverse of a hashmap with unique values
template<class T, class Compare>
map<T, T, Compare> reverseMap(const map<T, T, Compare>& myMap);

//finds the closest pair corresponding to the passed key value
template <class T, class Compare>
pair<float, T> findClosestKey(float target, const map<float, T, Compare>& myMap);

//height for hills in desert map
float heightFunction(float x, float y);

//draw desert floot and incldue some hills
void drawFloor(float size, int nSquares, GLuint textureID);

void drawTrees(float x, float y, float z, float height, float width, int subdivision);

void drawCarousel(bool doingShadows, float angle);

void drawFerrisWheel(bool doingShadows, float angle);

void drawSphere(float radius, int subdivisions);

void cartoonizeImage(GLubyte* imageData, int width, int height, GLubyte* cartoonImage);

void watercolorizeImage(GLubyte* imageData, int width, int height, GLubyte* watercolorImage);

void pixelateImage(GLubyte* imageData, int width, int height, int pixelSize, GLubyte* pixelatedImage);