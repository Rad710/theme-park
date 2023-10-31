#pragma once

#include <tuple>
#include <map>

#include "Utilities/Pnt3f.H"

#include "HelperFunctions.h"

class TrainView;
class TrainWindow;
class CTrack;

using namespace std;

class TrainWagon
{
	friend class SmokeSystem;
public:
	TrainWagon(int i, int red, int green, int blue) : index(i), r(red), g(green), b(blue) { }

	static void setParameter(tuple<map<float, Pnt3f, FloatCompare>, 
		map<float, float, FloatCompare>, map<float, Pnt3f, FloatCompare>>& parameters);

	void draw(bool doingShadows, int i);

	int getIndex() { return index; }

public:
	//curent parameter t of train
	static float t;

	//train view object
	static TrainView* view;

	//point corresponding to each parameter t
	static map<float, Pnt3f, FloatCompare> parameterCurveT;

	//accumulated arc length corresponding to each parameter t
	static map<float, float, FloatCompare> arcLengthIntegral;

	//t corresponding to current accumulated arc length
	static map<float, float, FloatCompare> arcLengthIntegralInverse;

	//normal vector corresponding to current t
	static map<float, Pnt3f, FloatCompare> parameterOrientT;

private:
	//index (train is 0, wagons [1,..]
	int index;

	int r, g, b;
};

