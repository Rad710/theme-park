#include "HelperFunctions.h"

#include <GL/GLU.h>
#include "Utilities/Pnt3f.H"

#include <utility>

#include <map>

#include <opencv2/opencv.hpp>

using namespace std;

//draw front of train
void drawTrainFront(bool doingShadows, int r, int g, int b, float angle)
{
	if (!doingShadows)
	{
		glColor3ub(r, g, b);
	}

	// train body
	glPushMatrix();
	glScalef(3.0f, 2.0f, 1.2f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// train roof
	if (!doingShadows)
	{
		glColor3ub(r - 20, g - 20, b - 20);
	}

	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	glScalef(3.0f, 0.1f, 1.2f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// train front
	if (!doingShadows)
	{
		glColor3ub(r - 20, g - 20, b - 20);
	}

	glPushMatrix();
	glTranslatef(1.6f, -0.2f, 0.0f);
	glScalef(0.9f, 0.8f, 0.6f);
	glutSolidSphere(1.0f, 20, 20);
	glPopMatrix();

	//train chimney
	if (!doingShadows)
	{
		glColor3ub(r, g, b);
	}

	glPushMatrix();
	glTranslatef(0.5f, 1.5f, 0.0f);
	glScalef(0.3f, 0.8f, 0.3f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// train wheels
	if (!doingShadows)
	{
		glColor3ub(50, 50, 50);
	}
	//four wheels
	for (int i = -1; i < 2; i += 2)
	{
		for (int j = -1; j < 2; j += 2)
		{
			glPushMatrix();
			glTranslatef(1.1f * i, -1.0f, 0.7f * j);
			glRotatef(angle * 10000.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.3f, 0.3f, 0.3f);
			glutSolidTorus(0.6f, 0.9f, 10, 10);
			glPopMatrix();
		}
	}
}

//draw wagons of train
void drawWagon(bool doingShadows, int r, int g, int b, float angle)
{
	if (!doingShadows)
	{
		glColor3ub(r, g, b);
	}

	// wagon body
	glPushMatrix();
	glScalef(3.0f, 2.0f, 1.2f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// roof
	if (!doingShadows)
	{
		glColor3ub(100, 100, 100);
	}

	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	glScalef(3.0f, 0.1f, 1.2f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// wheels
	if (!doingShadows)
	{
		glColor3ub(50, 50, 50);
	}
	// four wheels
	for (int i = -1; i < 2; i += 2)
	{
		for (int j = -1; j < 2; j += 2)
		{
			glPushMatrix();
			glTranslatef(1.1f * i, -1.0f, 0.7f * j);
			glRotatef(angle * 100000.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.3f, 0.3f, 0.3f);
			glutSolidTorus(0.6f, 0.9f, 10, 10);
			glPopMatrix();
		}
	}
}

//use to draw sleepers or line
void drawTrackPart(bool doingShadows, Pnt3f qt, Pnt3f qtLast, Pnt3f orient_t,
	float width, float thickness, float depth, int r, int g, int b)
{
	//rotation matrix
	Pnt3f u = (qt - qtLast);
	u.normalize();
	Pnt3f w = u * orient_t;
	w.normalize();
	Pnt3f v = w * u;
	v.normalize();

	float rotation[16] = {
		u.x, u.y, u.z, 0.0,
		v.x, v.y, v.z, 0.0,
		w.x, w.y, w.z, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	Pnt3f midPoint = (qt + qtLast) / 2;

	glPushMatrix();
	glTranslatef(midPoint.x, midPoint.y, midPoint.z);
	glMultMatrixf(rotation);

	// front face

	if (!doingShadows)
	{
		glColor3ub(r, g, b);
	}

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); // normal vector
	glVertex3f(-depth / 2, -thickness / 2, width/ 2);
	glVertex3f(depth / 2, -thickness / 2, width/ 2);
	glVertex3f(depth / 2, thickness / 2, width/ 2);
	glVertex3f(-depth / 2, thickness / 2, width/ 2);
	glEnd();

	// draw the back face
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f); // normal vector
	glVertex3f(-depth / 2, -thickness / 2, -width/ 2);
	glVertex3f(depth / 2, -thickness / 2, -width/ 2);
	glVertex3f(depth / 2, thickness / 2, -width/ 2);
	glVertex3f(-depth / 2, thickness / 2, -width/ 2);
	glEnd();

	// top face
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); // normal vector
	glVertex3f(-depth / 2, thickness / 2, width/ 2);
	glVertex3f(depth / 2, thickness / 2, width/ 2);
	glVertex3f(depth / 2, thickness / 2, -width/ 2);
	glVertex3f(-depth / 2, thickness / 2, -width/ 2);
	glEnd();

	// bottom face
	glBegin(GL_QUADS);
	glNormal3f(0.0f, -1.0f, 0.0f); // normal vector
	glVertex3f(-depth / 2, -thickness / 2, width/ 2);
	glVertex3f(depth / 2, -thickness / 2, width/ 2);
	glVertex3f(depth / 2, -thickness / 2, -width/ 2);
	glVertex3f(-depth / 2, -thickness / 2, -width/ 2);
	glEnd();

	// left face
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f); // normal vector
	glVertex3f(-depth / 2, -thickness / 2, width/ 2);
	glVertex3f(-depth / 2, -thickness / 2, -width/ 2);
	glVertex3f(-depth / 2, thickness / 2, -width/ 2);
	glVertex3f(-depth / 2, thickness / 2, width/ 2);
	glEnd();

	// right face
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f); // normal vector
	glVertex3f(depth / 2, -thickness / 2, width/ 2);
	glVertex3f(depth / 2, -thickness / 2, -width/ 2);
	glVertex3f(depth / 2, thickness / 2, -width/ 2);
	glVertex3f(depth / 2, thickness / 2, width/ 2);
	glEnd();

	glPopMatrix();
}

void drawBeam(bool doingShadows, Pnt3f pos, Pnt3f orient)
{
	// dot product with groung
	Pnt3f groundDirection(0, -1, 0);
	float dotProduct = orient.dot(groundDirection);

	float lengthA = orient.magnitude();
	float lengthB = groundDirection.magnitude();

	// angle between vectors
	float cosTheta = dotProduct / (lengthA * lengthB);
	float theta = acosf(cosTheta) * 180.0f / 3.1415f;

	if (pos.y > 0 && theta >= 100.0f) //if above floot and not pointing to ground
	{
		float thickness = pos.y;
		float depth = 5.0f;
		float width = 5.0f;

		pos = pos - orient * 3.0f;
		pos.y = pos.y / 2;

		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);

		// front face

		if (!doingShadows)
		{
			glColor3ub(100, 100, 100);
		}

		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f); // normal vector
		glVertex3f(-depth / 2, -thickness / 2, width / 2);
		glVertex3f(depth / 2, -thickness / 2, width / 2);
		glVertex3f(depth / 2, thickness / 2, width / 2);
		glVertex3f(-depth / 2, thickness / 2, width / 2);
		glEnd();

		// back face
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); // normal vector
		glVertex3f(-depth / 2, -thickness / 2, -width / 2);
		glVertex3f(depth / 2, -thickness / 2, -width / 2);
		glVertex3f(depth / 2, thickness / 2, -width / 2);
		glVertex3f(-depth / 2, thickness / 2, -width / 2);
		glEnd();

		// top face
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f); // normal vector
		glVertex3f(-depth / 2, thickness / 2, width / 2);
		glVertex3f(depth / 2, thickness / 2, width / 2);
		glVertex3f(depth / 2, thickness / 2, -width / 2);
		glVertex3f(-depth / 2, thickness / 2, -width / 2);
		glEnd();

		// bottom face
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f); // normal vector
		glVertex3f(-depth / 2, -thickness / 2, width / 2);
		glVertex3f(depth / 2, -thickness / 2, width / 2);
		glVertex3f(depth / 2, -thickness / 2, -width / 2);
		glVertex3f(-depth / 2, -thickness / 2, -width / 2);
		glEnd();

		// left face
		glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f); // normal vector
		glVertex3f(-depth / 2, -thickness / 2, width / 2);
		glVertex3f(-depth / 2, -thickness / 2, -width / 2);
		glVertex3f(-depth / 2, thickness / 2, -width / 2);
		glVertex3f(-depth / 2, thickness / 2, width / 2);
		glEnd();

		// right face
		glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f); // normal vector
		glVertex3f(depth / 2, -thickness / 2, width / 2);
		glVertex3f(depth / 2, -thickness / 2, -width / 2);
		glVertex3f(depth / 2, thickness / 2, -width / 2);
		glVertex3f(depth / 2, thickness / 2, width / 2);
		glEnd();

		glPopMatrix();
	}
}

void adaptiveSubdivide(int& divideLine, Pnt3f lastDirection, Pnt3f direction)
{
	// dot product between the previous and current orientation vectors
	float dotProduct = lastDirection.dot(direction);

	// change in direction between the previous and current orientations
	float angle = acos(dotProduct) * 180.0f / 3.1415f;

	const float minAngleThreshold = 1.0f;
	const float maxAngleThreshold = 5.0f;
	const int minSubdivisions = 300;
	const int maxSubdivisions = 1000;
	int step;
	if (angle < minAngleThreshold) //reduce subdivisions value
	{
		step = -1;
	}
	else if (angle > maxAngleThreshold) //increase subdivisions value
	{
		step = 10;
	}
	else //increase subdivisions value
	{
		step = 5;
	}

	if (divideLine + step < minSubdivisions)
	{
		divideLine = minSubdivisions;
	}
	else if (divideLine + step > maxSubdivisions) 
	{
		divideLine = maxSubdivisions;
	}
	else
	{
		divideLine += step;
	}
}

Pnt3f multiplyControlPoint(float G[3][4], float M[4][4], float t)
{
	float T[4][1] = {
			{t * t * t},
			{t * t},
			{t},
			{1.0f}
	};

	float MT[4][1];

	float QT[3][1];

	//Compute MT by multiplying M and T
	for (size_t i = 0; i < 4; i++)
	{
		MT[i][0] = 0.0f;
		for (size_t j = 0; j < 4; j++)
		{
			MT[i][0] += M[i][j] * T[j][0];
		}
	}

	// Compute GMT by multiplying G and MT
	for (size_t i = 0; i < 3; i++)
	{
		QT[i][0] = 0.0f;
		for (size_t k = 0; k < 4; k++)
		{
			QT[i][0] += G[i][k] * MT[k][0];
		}
	}

	return Pnt3f(QT[0][0], QT[1][0], QT[2][0]);
}


template<class T, class Compare>
map<T, T, Compare> reverseMap(const map<T, T, Compare>& myMap)
{
	map<T, T, Compare> reversedMyMap;

	for (const auto& pair : myMap)
	{
		reversedMyMap[pair.second] = pair.first;
	}
	return reversedMyMap;
}


template <class T, class Compare>
pair<float, T> findClosestKey(float target, const map<float, T, Compare>& myMap)
{
	auto it = myMap.lower_bound(target);
	if (it == myMap.begin())
	{
		return *it;
	}
	else if (it == myMap.end())
	{
		return *prev(it);
	}
	else
	{
		float d1 = it->first - target;
		float d2 = target - prev(it)->first;
		if (d1 < d2)
		{
			return *it;
		}
		else
		{
			return *prev(it);
		}
	}
}

//declare template functions types to actually be used
template map<float, float, FloatCompare> reverseMap(const map<float, float, FloatCompare>& myMap);

template pair<float, float> findClosestKey<float, FloatCompare>(float target,
	const map<float, float, FloatCompare>& myMap);

template pair<float, Pnt3f> findClosestKey<Pnt3f, FloatCompare>(float target,
	const map<float, Pnt3f, FloatCompare>& myMap);

float heightFunction(float x, float y) //model hills using wave functions
{
	return 3.0f * (sin(0.1f * x) * cos(0.1f * y));
}

void drawFloor(float size, int nSquares, GLuint textureID)
{
	float maxX = size / 2, maxY = size / 2;
	float minX = -size / 2, minY = -size / 2;

	int x, y, i;
	float xp, yp, xd, yd;
	xd = (maxX - minX) / ((float)nSquares);
	yd = (maxY - minY) / ((float)nSquares);

	// Enable texture mapping
	glEnable(GL_TEXTURE_2D);

	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Specify texture coordinates and draw the floor
	glBegin(GL_QUADS);
	for (x = 0, xp = minX; x < nSquares; x++, xp += xd)
	{
		for (y = 0, yp = minY, i = x; y < nSquares; y++, i++, yp += yd)
		{
			GLfloat color[] = { 1.0, 1.0, 1.0, 1.0};
			glColor4fv(color);

			float height1 = heightFunction(xp, yp);
			float height2 = heightFunction(xp, yp + yd);
			float height3 = heightFunction(xp + xd, yp + yd);
			float height4 = heightFunction(xp + xd, yp);

			// normal for the current quad
			float normal_x = -2 * (height4 - height1);
			float normal_y = 4 * xd * yd;
			float normal_z = -2 * (height2 - height1);
			float normal_length = sqrt(normal_x * normal_x + normal_y * normal_y + normal_z * normal_z);
			glNormal3f(normal_x / normal_length, normal_y / normal_length, normal_z / normal_length);

			// Specify texture coordinates
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d(xp, height1, yp);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3d(xp, height2, yp + yd);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3d(xp + xd, height3, yp + yd);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3d(xp + xd, height4, yp);
		}
	}
	glEnd();

	// Disable texture mapping
	glDisable(GL_TEXTURE_2D);
}


// Function to draw a cone
void drawCone(float height, float radius, int subdivision)
{
	const int slices = 100;  // number of subdivisions around the z-axis
	const int stacks = 100;  // number of subdivisions along the z-axis

	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // align the cone along the y-axis
	glutSolidCone(radius, height, 2 * subdivision, subdivision);
	glPopMatrix();
}

// Function to draw trees
void drawTrees(float x, float y, float z, float height, float width, int subdivision)
{
	glPushMatrix();
	glTranslatef(x, y, z);  // move the tree to the specified position

	float trunkHeight = height * 0.3f;
	float trunkRadius = width * 0.1f;
	float coneHeight = height * 0.7f;
	float coneRadius = width * 0.5f;

	// Draw the trunk
	glColor3f(0.298f, 0.149f, 0.043f);
	drawCone(trunkHeight * 3, trunkRadius, subdivision);

	// Move up to draw the first cone
	glColor3f(0.0f, 0.4f, 0.0f);
	glTranslatef(0.0f, trunkHeight, 0.0f);

	// Draw the first cone
	drawCone(coneHeight, coneRadius, subdivision);

	glPopMatrix();
}

void drawCarousel(bool doingShadows, float angle)
{
	// Draw your object here
	glPushMatrix();

	// Draw the central pole
	glTranslatef(0.0f, 0.0f, 100.0f);

	if (!doingShadows)
		glColor3ub(100, 100, 100);

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gluNewQuadric(), 2.5f, 2.5f, 5.0f, 50.0f, 10.0f);

	// Draw the base platform
	glRotatef(270.0f, -1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 5.0f, 0.0f);
	glRotatef(angle * 100.0f, 0.0f, 1.0f, 0.0f);


	if (!doingShadows)
		glColor3ub(128, 0, 0);

	glBegin(GL_POLYGON);
	int numVertices = 10; // Number of vertices in the base polygon
	float radius = 20.0f;
	for (int i = 0; i < numVertices; ++i)
	{
		float angle = 2.0f * 3.14159f * i / numVertices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	// Draw the spinning seats
	for (int i = 0; i < 8; ++i)
	{
		glPushMatrix();
		glRotatef(45.0f * i, 0.0f, 1.0f, 0.0f);

		// Draw the seat support stick
		glTranslatef(0.0f, 0.0f, -5.0f);

		if (!doingShadows)
			glColor3ub(100, 100, 100);

		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 6.5f, 0.0f);
		gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 2.5f, 20.0f, 10.0f);

		// Draw the seat
		glTranslatef(0.0f, 0.0f, 2.5f);

		if (!doingShadows)
			glColor3ub(0, 0, 255);
		
		glutSolidCube(2.0f);

		glPopMatrix();
	}

	glPopMatrix();
}


void drawFerrisWheel(bool doingShadows, float angle)
{
	// Draw your object here
	glPushMatrix();

	// Draw the central pole
	glTranslatef(0.0f, 5.0f, -100.0f);

	if (!doingShadows)
		glColor3ub(100, 100, 100);

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gluNewQuadric(), 2.5f, 2.5f, 50.0f, 50.0f, 10.0f);

	// Draw the rotating wheel
	glPushMatrix();

	if (!doingShadows)
		glColor3ub(128, 0, 0);

	glBegin(GL_QUADS);
	// Front face
	glVertex3f(-25.0f, 25.0f, 0.0f);
	glVertex3f(25.0f, 25.0f, 0.0f);
	glVertex3f(25.0f, -25.0f, 0.0f);
	glVertex3f(-25.0f, -25.0f, 0.0f);

	// Back face
	glVertex3f(-25.0f, 25.0f, -5.0f);
	glVertex3f(25.0f, 25.0f, -5.0f);
	glVertex3f(25.0f, -25.0f, -5.0f);
	glVertex3f(-25.0f, -25.0f, -5.0f);

	// Top face
	glVertex3f(-25.0f, 25.0f, 0.0f);
	glVertex3f(25.0f, 25.0f, 0.0f);
	glVertex3f(25.0f, 25.0f, -5.0f);
	glVertex3f(-25.0f, 25.0f, -5.0f);

	// Bottom face
	glVertex3f(-25.0f, -25.0f, 0.0f);
	glVertex3f(25.0f, -25.0f, 0.0f);
	glVertex3f(25.0f, -25.0f, -5.0f);
	glVertex3f(-25.0f, -25.0f, -5.0f);

	// Left face
	glVertex3f(-25.0f, 25.0f, 0.0f);
	glVertex3f(-25.0f, 25.0f, -5.0f);
	glVertex3f(-25.0f, -25.0f, -5.0f);
	glVertex3f(-25.0f, -25.0f, 0.0f);

	// Right face
	glVertex3f(25.0f, 25.0f, 0.0f);
	glVertex3f(25.0f, 25.0f, -5.0f);
	glVertex3f(25.0f, -25.0f, -5.0f);
	glVertex3f(25.0f, -25.0f, 0.0f);
	glEnd();

	glPopMatrix();

	// Draw the cabins
	glTranslatef(0.0f, -5.0f, 40.0f);
	glRotatef(angle * 50.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	if (!doingShadows)
		glColor3ub(100, 100, 100);

	glutSolidTorus(2.0f, 25.0f, 100, 100);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	for (int i = 0; i < 8; ++i)
	{
		glPushMatrix();
		glRotatef(45.0f * i, 0.0f, 1.0f, 0.0f);

		// Draw the cabin support stick
		glTranslatef(0.0f, -25.0f, 0.0f);

		if (!doingShadows)
			glColor3ub(100, 100, 100);

		glTranslatef(0.0f, 25.5f, 0.0f);
		gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 25.0f, 20.0f, 10.0f);

		// Draw the cabin
		glTranslatef(0.0f, 5.0f, 2.5f);

		if (!doingShadows)
			glColor3ub(150, 150, 0);

		glTranslatef(0.0f, -5.0f, 25.0f);
		glutSolidCube(10.0f);

		glPopMatrix();
	}

	glPopMatrix();
}



void drawSphere(float radius, int subdivisions) 
{
	const float M_PI = 3.1415f;
	int stacks = max(1, subdivisions);
	int slices = max(2, subdivisions * 2);

	float stackStep = 1.0f / stacks;
	float sliceStep = 1.0f / slices;

	glRotatef(90.0f, 1.0f, 0.0f, 1.0f);
	for (int i = 0; i < stacks; ++i)
	{
		float phi1 = i * stackStep * M_PI;
		float phi2 = (i + 1) * stackStep * M_PI;

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= slices; ++j) {
			float theta = j * sliceStep * 2 * M_PI;

			// Compute the vertex position
			float x = std::cos(theta) * std::sin(phi2);
			float y = std::cos(phi2);
			float z = std::sin(theta) * std::sin(phi2);

			// Compute the normal vector
			float nx = std::cos(theta) * std::sin(phi2);
			float ny = std::cos(phi2);
			float nz = std::sin(theta) * std::sin(phi2);

			// Normalize the normal vector
			float length = std::sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			// Set the normal vector
			glNormal3f(nx, ny, nz);

			// Set the vertex position
			glVertex3f(x * radius, y * radius, z * radius);

			// Repeat the same steps for the second vertex of the quad strip
			x = std::cos(theta) * std::sin(phi1);
			y = std::cos(phi1);
			z = std::sin(theta) * std::sin(phi1);

			nx = std::cos(theta) * std::sin(phi1);
			ny = std::cos(phi1);
			nz = std::sin(theta) * std::sin(phi1);

			length = std::sqrt(nx * nx + ny * ny + nz * nz);
			nx /= length;
			ny /= length;
			nz /= length;

			glNormal3f(nx, ny, nz);
			glVertex3f(x * radius, y * radius, z * radius);
		}
		glEnd();
	}
	glRotatef(-90.0f, 1.0f, 0.0f, 1.0f);
}

void cartoonizeImage(GLubyte* imageData, int width, int height, GLubyte* cartoonImage) 
{
	// Convert the GLubyte* image data to cv::Mat
	cv::Mat inputImage(height, width, CV_8UC3, imageData);

	// Convert image to grayscale
	cv::Mat grayImage;
	cv::cvtColor(inputImage, grayImage, cv::COLOR_RGB2GRAY);

	// Apply median blur to reduce noise and enhance edges
	cv::Mat blurredImage;
	cv::medianBlur(grayImage, blurredImage, 7);

	// Detect edges using adaptive thresholding
	cv::Mat edges;
	cv::adaptiveThreshold(blurredImage, edges, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 9, 2);

	// Create output image with cartoon-like effect
	cv::Mat cartoonizedImage;
	cv::bitwise_and(inputImage, inputImage, cartoonizedImage, edges);

	// Convert the result back to GLubyte* format
	cv::Mat resultImage;
	cartoonizedImage.convertTo(resultImage, CV_8U);

	// Store the result in the cartoonImage array
	memcpy(cartoonImage, resultImage.data, 3 * width * height * sizeof(GLubyte));
}

void watercolorizeImage(GLubyte* imageData, int width, int height, GLubyte* watercolorImage)
{
	// Convert the GLubyte* image data to cv::Mat
	cv::Mat inputImage(height, width, CV_8UC3, imageData);

	// Convert the image to a watercolor-like effect
	cv::Mat watercolorImageMat;
	cv::cvtColor(inputImage, watercolorImageMat, cv::COLOR_RGB2Lab);

	// Perform white balance adjustment
	cv::Mat channels[3];
	cv::split(watercolorImageMat, channels);
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->apply(channels[0], channels[0]);
	cv::merge(channels, 3, watercolorImageMat);
	cv::cvtColor(watercolorImageMat, watercolorImageMat, cv::COLOR_Lab2RGB);

	// Perform detail enhancement using bilateral filtering
	cv::Mat detailEnhancedImage;
	cv::bilateralFilter(watercolorImageMat, detailEnhancedImage, 9, 75, 75);

	// Generate a randomized texture synthesis effect
	cv::Mat textureSynthesizedImage(height, width, CV_8UC3);
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, 255);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			cv::Vec3b randomColor(distribution(generator), distribution(generator), distribution(generator));
			textureSynthesizedImage.at<cv::Vec3b>(i, j) = randomColor;
		}
	}

	// Blend the detail-enhanced image with the synthesized texture
	cv::Mat watercolorImageBlend;
	cv::addWeighted(detailEnhancedImage, 0.7, textureSynthesizedImage, 0.3, 0.0, watercolorImageBlend);

	// Convert the result back to GLubyte* format
	cv::Mat resultImage;
	watercolorImageBlend.convertTo(resultImage, CV_8U);

	// Store the result in the watercolorImage array
	memcpy(watercolorImage, resultImage.data, 3 * width * height * sizeof(GLubyte));
}


void pixelateImage(GLubyte* imageData, int width, int height, int pixelSize, GLubyte* pixelatedImage)
{
	// Convert the GLubyte* image data to cv::Mat
	cv::Mat inputImage(height, width, CV_8UC3, imageData);

	// Resize the image to achieve pixelation effect
	cv::Size newSize(width / pixelSize, height / pixelSize);
	cv::Mat resizedImage;
	cv::resize(inputImage, resizedImage, newSize, cv::INTER_NEAREST);

	// Resize the image back to original dimensions to restore pixelation effect
	cv::Mat pixelatedImageMat;
	cv::resize(resizedImage, pixelatedImageMat, inputImage.size(), cv::INTER_NEAREST);

	// Convert the result back to GLubyte* format
	cv::Mat resultImage;
	pixelatedImageMat.convertTo(resultImage, CV_8U);

	// Store the result in the pixelatedImage array
	memcpy(pixelatedImage, resultImage.data, 3 * width * height * sizeof(GLubyte));
}


