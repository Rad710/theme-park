#pragma once

#include <GL/GLU.h>
#include "Utilities/Pnt3f.H"

class Person
{
public:
	Person(Pnt3f pos, Pnt3f dir) : position(pos), direction(dir), t(-0.034f) {}

	void advance()
	{
		position = position + direction;
        t += 0.005f;
	}

	void reverse()
	{
		position = position - direction;
        t -= 0.005f;
	}

	void rotateDirection(float angleDegrees)
	{
		float angleRad = angleDegrees * 3.14159f / 180.0f;  // Convert angle to radians
		float cosAngle = cos(angleRad);
		float sinAngle = sin(angleRad);

		float rotatedX = direction.x * cosAngle - direction.z * sinAngle;
		float rotatedZ = direction.x * sinAngle + direction.z * cosAngle;

		direction.x = rotatedX;
		direction.z = rotatedZ;
		direction.normalize();
	}

	void draw()
	{
        float x = position.x;
        float y = position.y;
        float z = position.z;

        float a = direction.x;
        float b = direction.y;
        float c = direction.z;

        float angle = atan2(c, a) * 180.0 / 3.14159f + 90.0f;

        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(-angle, 0.0f, 1.0f, 0.0f);

        // Draw the body
        glPushMatrix();
        glColor3f(0.0f, 0.0f, 1.0f);
        glScalef(1.0f, 2.0f, 0.5f);
        glutSolidCube(1.00);
        glPopMatrix();

        // Draw the head
        glPushMatrix();
        glColor3f(1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 1.5f, 0.0f);
        glutSolidSphere(0.5, 20, 20);
        glPopMatrix();

        // Draw the limbs
        glPushMatrix();
        glColor3f(0.0f, 1.0f, 0.0f);

        // Right arm
        glPushMatrix();
        glTranslatef(0.75f, 1.0f, -0.3f);
        glRotatef(a, 1.0f, 0.0f, 0.0f);
        glRotatef(c, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 1.5f, 0.5f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Left arm
        glPushMatrix();
        glTranslatef(-0.75f, 1.0f, -0.3f);
        glRotatef(a, 1.0f, 0.0f, 0.0f);
        glRotatef(c, 0.0f, 0.0f, 1.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 1.5f, 0.5f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Right leg
        glPushMatrix();
        glTranslatef(0.75f, -1.5f, 0.0f);
        glRotatef(b, 1.0f, 0.0f, 0.0f);
        
        if (t > 0.034)
        {
            t = -0.034;
            multiplier = -multiplier;
        }

        if (t < -0.035)
        {
            t = 0.034;
            multiplier = -multiplier;
        }

        glTranslatef(0.0f, 1.0f, 0.0f);

        glRotatef(multiplier * t * 1000, 1.0f, 0.0f, 0.0f);

        glScalef(1.0f, 8.0f, 1.0f);
       
        float size = 0.5f; // size of the cube

        // Calculate half the size of the cube
        float halfSize = size / 2.0f;

        // draw the cube
        glBegin(GL_QUADS);

        // draw the top face
        glNormal3f(0.0f, 1.0f, 0.0f); // normal vector
        glVertex3f(-halfSize, 0.0f, -halfSize); // bottom-left vertex
        glVertex3f(-halfSize, 0.0f, halfSize);  // top-left vertex
        glVertex3f(halfSize, 0.0f, halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, -halfSize);  // bottom-right vertex

        glNormal3f(0.0f, 0.0f, 1.0f);  // normal vector
        glVertex3f(-halfSize, 0.0f, halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, halfSize);  // bottom-right vertex

        glNormal3f(0.0f, 0.0f, -1.0f);  // normal vector
        glVertex3f(-halfSize, 0.0f, -halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, -halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, -halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, -halfSize);  // bottom-right vertex

        glNormal3f(-1.0f, 0.0f, 0.0f);  // normal vector
        glVertex3f(-halfSize, 0.0f, -halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, -halfSize);  // top-left vertex
        glVertex3f(-halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(-halfSize, 0.0f, halfSize);  // bottom-right vertex

        glNormal3f(1.0f, 0.0f, 0.0f);  // normal vector
        glVertex3f(halfSize, 0.0f, -halfSize);  // bottom-left vertex
        glVertex3f(halfSize, -halfSize, -halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, halfSize);  // bottom-right vertex

        glNormal3f(0.0f, -1.0f, 0.0f);  // normal vector
        glVertex3f(-halfSize, -halfSize, -halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(halfSize, -halfSize, -halfSize);  // bottom-right vertex
        glEnd();


        glPopMatrix();

        // Left leg
        glPushMatrix();
        glTranslatef(-0.75f, -1.5f, 0.0f);
        glRotatef(b, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 1.0f, 0.0f);
        glRotatef( (-multiplier) * t * 1000, 1.0f, 0.0f, 0.0f);

        glScalef(1.0f, 8.0f, 1.0f);

        // draw the cube
        glBegin(GL_QUADS);

        // draw the top face
        glNormal3f(0.0f, 1.0f, 0.0f); // normal vector
        glVertex3f(-halfSize, 0.0f, -halfSize); // bottom-left vertex
        glVertex3f(-halfSize, 0.0f, halfSize);  // top-left vertex
        glVertex3f(halfSize, 0.0f, halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, -halfSize);  // bottom-right vertex

        glNormal3f(0.0f, 0.0f, 1.0f);  // normal vector
        glVertex3f(-halfSize, 0.0f, halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, halfSize);  // bottom-right vertex

        glNormal3f(0.0f, 0.0f, -1.0f);  // normal vector
        glVertex3f(-halfSize, 0.0f, -halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, -halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, -halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, -halfSize);  // bottom-right vertex

        glNormal3f(-1.0f, 0.0f, 0.0f);  // normal vector
        glVertex3f(-halfSize, 0.0f, -halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, -halfSize);  // top-left vertex
        glVertex3f(-halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(-halfSize, 0.0f, halfSize);  // bottom-right vertex

        glNormal3f(1.0f, 0.0f, 0.0f);  // normal vector
        glVertex3f(halfSize, 0.0f, -halfSize);  // bottom-left vertex
        glVertex3f(halfSize, -halfSize, -halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(halfSize, 0.0f, halfSize);  // bottom-right vertex

        glNormal3f(0.0f, -1.0f, 0.0f);  // normal vector
        glVertex3f(-halfSize, -halfSize, -halfSize);  // bottom-left vertex
        glVertex3f(-halfSize, -halfSize, halfSize);  // top-left vertex
        glVertex3f(halfSize, -halfSize, halfSize);   // top-right vertex
        glVertex3f(halfSize, -halfSize, -halfSize);  // bottom-right vertex
        glEnd();

        glPopMatrix();

        glPopMatrix();

        glPopMatrix();
	}

public:
	Pnt3f position;
	Pnt3f direction;

    float t;
    float multiplier = 1;
};