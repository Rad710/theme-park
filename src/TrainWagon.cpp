#include "TrainWagon.h"

#include "TrainWindow.H"
#include "TrainView.H"
#include "GL/GLU.h"
#include "Utilities/3DUtils.H"
#include "HelperFunctions.h"

#include <tuple>
#include <map>
#include <limits>

using namespace std;

//initialize static member variables
float TrainWagon::t = 0.1f;
TrainView* TrainWagon::view = nullptr;

map<float, Pnt3f, FloatCompare> TrainWagon::parameterCurveT;
map<float, float, FloatCompare> TrainWagon::arcLengthIntegral;
map<float, float, FloatCompare> TrainWagon::arcLengthIntegralInverse;
map<float, Pnt3f, FloatCompare> TrainWagon::parameterOrientT;

//store calculated values
void TrainWagon::setParameter(tuple<map<float, Pnt3f, FloatCompare>, 
	map<float, float, FloatCompare>, map<float, Pnt3f, FloatCompare>>& parameters)
{
	parameterCurveT = (get<0>(parameters));
	arcLengthIntegral = (get<1>(parameters));
	parameterOrientT = (get<2>(parameters));
	arcLengthIntegralInverse = reverseMap(arcLengthIntegral);
}

//draw train/wagon
void TrainWagon::draw(bool doingShadows, int i)
{
	float separationDistance = 18.0f;

	//get arc length of current parameter t
	float arcLength = findClosestKey(TrainWagon::t, arcLengthIntegral).second;

	//get relative arc length
	float maxArcLength = arcLengthIntegral.rbegin()->second;
	arcLength = fmodf(arcLength - i * separationDistance + maxArcLength, maxArcLength);

	//find t of the relative arc length
	float relativeT = findClosestKey(arcLength, arcLengthIntegralInverse).second;

	if (relativeT <= 0.01f)
		relativeT = 0.01f;

	Pnt3f qt = findClosestKey(relativeT, parameterCurveT).second; //get current point

	Pnt3f orient_t = findClosestKey(relativeT, parameterOrientT).second;

	Pnt3f pos_p1 = qt;

	relativeT += 1.0f / view->divideLine;

	Pnt3f pos_p2 = findClosestKey(relativeT, parameterCurveT).second;

	Pnt3f u = (pos_p2 - pos_p1);
	u.normalize();
	Pnt3f w = u * orient_t;
	w.normalize();
	Pnt3f v = w * u;
	v.normalize();

	//draw the train
	float rotation[16] = {
		u.x, u.y, u.z, 0.0,
		v.x, v.y, v.z, 0.0,
		w.x, w.y, w.z, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	qt = qt + orient_t * 2.2f;
	glPushMatrix();
	glTranslatef(qt.x, qt.y, qt.z);
	glMultMatrixf(rotation);
	glScalef(5.0f, 5.0f, 5.0f);
	glTranslatef(0.0f, 0.5f, 0.0f);

	if (i == 0) //0 means train front
	{
		drawTrainFront(doingShadows, r, g, b, view->t_time);
	}
	else
	{
		drawWagon(doingShadows, r, g, b, view->t_time);
	}
	glPopMatrix();
}