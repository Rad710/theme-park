/************************************************************************
     File:        TrainView.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						The TrainView is the window that actually shows the 
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within 
						a TrainWindow
						that is the outer window with all the widgets. 
						The TrainView needs 
						to be aware of the window - since it might need to 
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know 
						about it (beware circular references)

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glu.h>

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"

#include <opencv2\opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <GL/gl.h>

#include <Fl/Fl_Box.H>

#include <Windows.h>

#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif

//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l) 
	: Fl_Gl_Window(x,y,w,h,l), t_time(0.0f), divideLine(300), front(0, 0, 0, 0), person(Pnt3f(100, 5,-10), Pnt3f(1, 0, 0))
//========================================================================
{
	wagons.push_back(TrainWagon(1, 255, 255, 255));

	TrainWagon::t = 0.01f;
	TrainWagon::view = this;

	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event)) 
			return 1;

	// remember what button was used
	static int last_push;

	switch(event) {
		// Mouse button being pushed event
		case FL_PUSH:
			last_push = Fl::event_button();
			// if the left button be pushed is left mouse button
			if (last_push == FL_LEFT_MOUSE  ) {
				doPick();
				damage(1);
				return 1;
			};
			break;

	   // Mouse button release event
		case FL_RELEASE: // button release
			damage(1);
			last_push = 0;
			return 1;

		// Mouse button drag event
		case FL_DRAG:

			// Compute the new control point position
			if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
				ControlPoint* cp = &m_pTrack->points[selectedCube];

				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
								static_cast<double>(cp->pos.x), 
								static_cast<double>(cp->pos.y),
								static_cast<double>(cp->pos.z),
								rx, ry, rz,
								(Fl::event_state() & FL_CTRL) != 0);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				damage(1);
			}
			break;

		// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;

		// every time the mouse enters this window, aggressively take focus
		case FL_ENTER:	
			focus(this);
			break;

		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k == 'p') {
					// Print out the selected control point information
					if (selectedCube >= 0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
								 selectedCube,
								 m_pTrack->points[selectedCube].pos.x,
								 m_pTrack->points[selectedCube].pos.y,
								 m_pTrack->points[selectedCube].pos.z,
								 m_pTrack->points[selectedCube].orient.x,
								 m_pTrack->points[selectedCube].orient.y,
								 m_pTrack->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");

					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...
		
		if (tw->textureBrowser->value() == 1)
		{
			this->shader = new
				Shader(
					PROJECT_DIR "/src/shaders/ref.vert",
					nullptr, nullptr, nullptr,
					PROJECT_DIR "/src/shaders/ref.frag");
		}

		else if (tw->textureBrowser->value() == 2)
		{
			this->shader = new
				Shader(
					PROJECT_DIR "/src/shaders/wave.vert",
					nullptr, nullptr, nullptr,
					PROJECT_DIR "/src/shaders/wave.frag");
		}
		else if (tw->textureBrowser->value() == 3)
		{
			this->shader = new
				Shader(
					PROJECT_DIR "/src/shaders/heightmap.vert",
					nullptr, nullptr, nullptr,
					PROJECT_DIR "/src/shaders/heightmap.frag");
		}



		if (!this->cubemapShader)
			this->cubemapShader = new
			Shader(
				PROJECT_DIR "/src/shaders/cubemap.vert",
				nullptr, nullptr, nullptr,
				PROJECT_DIR "/src/shaders/cubemap.frag");

		if (!this->commom_matrices)
			this->commom_matrices = new UBO();
			this->commom_matrices->size = 2 * sizeof(glm::mat4);
			glGenBuffers(1, &this->commom_matrices->ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
			glBufferData(GL_UNIFORM_BUFFER, this->commom_matrices->size, NULL, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

		if (!this->plane) 
		{
			vector<GLfloat> vertices;
			vector<GLfloat> normalVectors;
			vector<GLfloat> textureCoordinates;
			int length = 50;
			int width = 50;
			for (int j = 0; j <= length; j++)
			{
				for (int i = 0; i <= width; i++)
				{
					//position
					vertices.push_back(float(i) - width / 2.0f);
					vertices.push_back(0.0f);
					vertices.push_back(float(j) - length / 2.0f);

					//normal vector
					normalVectors.push_back(0.0f);
					normalVectors.push_back(1.0f);
					normalVectors.push_back(0.0f);

					// texture Coordinate
					textureCoordinates.push_back((GLfloat)i / width); 
					textureCoordinates.push_back((GLfloat)j / length);
				}
			}

			vector<GLuint> vertexIndices;
			for (int j = 0; j < length; j++)
			{
				for (int i = 0; i < width; i++)
				{
					int initial = (j * (width + 1)) + i;
					// triangle 1
					vertexIndices.push_back(initial);  // bottom left
					vertexIndices.push_back(initial + 1);  // bottom right
					vertexIndices.push_back(initial + width + 1);  // top left

					// triangle 2
					vertexIndices.push_back(initial + 1);  // bottom right 
					vertexIndices.push_back(initial + width + 1); // top left
					vertexIndices.push_back(initial + width + 2); // top right
				}
			}

			this->plane = new VAO;
			this->plane->element_amount = vertexIndices.size();
			
			glGenVertexArrays(1, &this->plane->vao);
			glGenBuffers(3, this->plane->vbo);
			glGenBuffers(1, &this->plane->ebo);

			glBindVertexArray(this->plane->vao);

			// Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Normal attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, normalVectors.size() * sizeof(GLfloat), &normalVectors[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);

			// Texture attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[2]);
			glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(GLfloat), &textureCoordinates[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(2);

			//Element attribute
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->plane->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(GLuint), &vertexIndices[0], GL_STATIC_DRAW);

			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->cubemap) {
			float vertices[] = {
				// Front face
				-1.0f,  1.0f, -1.0f,   // Vertex 1
				-1.0f, -1.0f, -1.0f,   // Vertex 2
				 1.0f, -1.0f, -1.0f,   // Vertex 3
				 1.0f, -1.0f, -1.0f,   // Vertex 4
				 1.0f,  1.0f, -1.0f,   // Vertex 5
				-1.0f,  1.0f, -1.0f,   // Vertex 6

				// Left face
				-1.0f, -1.0f,  1.0f,   // Vertex 7
				-1.0f, -1.0f, -1.0f,   // Vertex 8
				-1.0f,  1.0f, -1.0f,   // Vertex 9
				-1.0f,  1.0f, -1.0f,   // Vertex 10
				-1.0f,  1.0f,  1.0f,   // Vertex 11
				-1.0f, -1.0f,  1.0f,   // Vertex 12

				// Right face
				 1.0f, -1.0f, -1.0f,   // Vertex 13
				 1.0f, -1.0f,  1.0f,   // Vertex 14
				 1.0f,  1.0f,  1.0f,   // Vertex 15
				 1.0f,  1.0f,  1.0f,   // Vertex 16
				 1.0f,  1.0f, -1.0f,   // Vertex 17
				 1.0f, -1.0f, -1.0f,   // Vertex 18

				// Back face
				-1.0f, -1.0f,  1.0f,   // Vertex 19
				-1.0f,  1.0f,  1.0f,   // Vertex 20
				 1.0f,  1.0f,  1.0f,   // Vertex 21
				 1.0f,  1.0f,  1.0f,   // Vertex 22
				 1.0f, -1.0f,  1.0f,   // Vertex 23
				-1.0f, -1.0f,  1.0f,   // Vertex 24

				// Top face
				-1.0f,  1.0f, -1.0f,   // Vertex 25
				 1.0f,  1.0f, -1.0f,   // Vertex 26
				 1.0f,  1.0f,  1.0f,   // Vertex 27
				 1.0f,  1.0f,  1.0f,   // Vertex 28
				-1.0f,  1.0f,  1.0f,   // Vertex 29
				-1.0f,  1.0f, -1.0f,   // Vertex 30

				// Bottom face
				-1.0f, -1.0f, -1.0f,   // Vertex 31
				-1.0f, -1.0f,  1.0f,   // Vertex 32
				 1.0f, -1.0f, -1.0f,   // Vertex 33
				 1.0f, -1.0f, -1.0f,   // Vertex 34
				-1.0f, -1.0f,  1.0f,   // Vertex 35
				 1.0f, -1.0f,  1.0f    // Vertex 36
			};


			for (int i = 0; i < sizeof(vertices) / sizeof(float); i++)
			{
				vertices[i] *= 600.0f;
			}


			std::vector<unsigned int> indices = {
				// Front face
				0, 1, 2,
				2, 3, 0,
				// Back face
				4, 5, 6,
				6, 7, 4,
				// Left face
				8, 9, 10,
				10, 11, 8,
				// Right face
				12, 13, 14,
				14, 15, 12,
				// Top face
				16, 17, 18,
				18, 19, 16,
				// Bottom face
				20, 21, 22,
				22, 23, 20
			};

			this->cubemap = new VAO;
			this->cubemap->element_amount = indices.size();
			glGenVertexArrays(1, &this->cubemap->vao);
			glGenBuffers(4, this->cubemap->vbo);
			glGenBuffers(1, &this->cubemap->ebo);

			glBindVertexArray(this->cubemap->vao);

			//We only need texture Coordinate attribute for Skybox
			glBindBuffer(GL_ARRAY_BUFFER, this->cubemap->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);


			//Element attribute
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->cubemap->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);


			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->poolCubemap) {
			float vertices[] = {
				// Front face
				-1.0f,  1.0f, -1.0f,   // Vertex 1
				-1.0f, -1.0f, -1.0f,   // Vertex 2
				 1.0f, -1.0f, -1.0f,   // Vertex 3
				 1.0f, -1.0f, -1.0f,   // Vertex 4
				 1.0f,  1.0f, -1.0f,   // Vertex 5
				-1.0f,  1.0f, -1.0f,   // Vertex 6

				// Left face
				-1.0f, -1.0f,  1.0f,   // Vertex 7
				-1.0f, -1.0f, -1.0f,   // Vertex 8
				-1.0f,  1.0f, -1.0f,   // Vertex 9
				-1.0f,  1.0f, -1.0f,   // Vertex 10
				-1.0f,  1.0f,  1.0f,   // Vertex 11
				-1.0f, -1.0f,  1.0f,   // Vertex 12

				// Right face
				 1.0f, -1.0f, -1.0f,   // Vertex 13
				 1.0f, -1.0f,  1.0f,   // Vertex 14
				 1.0f,  1.0f,  1.0f,   // Vertex 15
				 1.0f,  1.0f,  1.0f,   // Vertex 16
				 1.0f,  1.0f, -1.0f,   // Vertex 17
				 1.0f, -1.0f, -1.0f,   // Vertex 18

				// Back face
				-1.0f, -1.0f,  1.0f,   // Vertex 19
				-1.0f,  1.0f,  1.0f,   // Vertex 20
				 1.0f,  1.0f,  1.0f,   // Vertex 21
				 1.0f,  1.0f,  1.0f,   // Vertex 22
				 1.0f, -1.0f,  1.0f,   // Vertex 23
				-1.0f, -1.0f,  1.0f,   // Vertex 24

				// Top face
				-1.0f,  1.0f, -1.0f,   // Vertex 25
				 1.0f,  1.0f, -1.0f,   // Vertex 26
				 1.0f,  1.0f,  1.0f,   // Vertex 27
				 1.0f,  1.0f,  1.0f,   // Vertex 28
				-1.0f,  1.0f,  1.0f,   // Vertex 29
				-1.0f,  1.0f, -1.0f,   // Vertex 30

				// Bottom face
				-1.0f, -1.0f, -1.0f,   // Vertex 31
				-1.0f, -1.0f,  1.0f,   // Vertex 32
				 1.0f, -1.0f, -1.0f,   // Vertex 33
				 1.0f, -1.0f, -1.0f,   // Vertex 34
				-1.0f, -1.0f,  1.0f,   // Vertex 35
				 1.0f, -1.0f,  1.0f    // Vertex 36
			};

			for (int i = 0; i < sizeof(vertices) / sizeof(float); i++)
			{
				vertices[i] *= 10.0f;
			}


			std::vector<unsigned int> indices = {
				// Front face
				0, 1, 2,
				2, 3, 0,
				// Back face
				4, 5, 6,
				6, 7, 4,
				// Left face
				8, 9, 10,
				10, 11, 8,
				// Right face
				12, 13, 14,
				14, 15, 12,
				// Top face
				16, 17, 18,
				18, 19, 16,
				// Bottom face
				20, 21, 22,
				22, 23, 20
			};

			this->poolCubemap = new VAO;
			this->poolCubemap->element_amount = indices.size();
			glGenVertexArrays(1, &this->poolCubemap->vao);
			glGenBuffers(4, this->poolCubemap->vbo);
			glGenBuffers(1, &this->poolCubemap->ebo);

			glBindVertexArray(this->poolCubemap->vao);

			//We only need texture Coordinate attribute for Skybox
			glBindBuffer(GL_ARRAY_BUFFER, this->poolCubemap->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);


			//Element attribute
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->poolCubemap->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);


			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->texture)
			this->texture = new Texture2D(PROJECT_DIR "/Images/water.jpg");

		if (!this->floorTexture)
			this->floorTexture = new Texture2D(PROJECT_DIR "/Images/tiles.jpg");

		if (!this->skyboxTexture)
		{
			vector<std::string> faces({ PROJECT_DIR "/Images/Skybox/right.jpg", PROJECT_DIR "/Images/Skybox/left.jpg",
				PROJECT_DIR "/Images/Skybox/top.jpg", PROJECT_DIR "/Images/Skybox/bottom.jpg",
				PROJECT_DIR "/Images/Skybox/front.jpg",  PROJECT_DIR "/Images/Skybox/back.jpg" });

			skyboxTexture = new CubemapTexture(faces);
		}

		if (!this->poolTexture)
		{
			vector<std::string> faces({ PROJECT_DIR "/Images/building.jpg", PROJECT_DIR "/Images/building.jpg",
				PROJECT_DIR "/Images/roof.jpg", PROJECT_DIR "/Images/building.jpg",
				PROJECT_DIR "/Images/front.jpg",  PROJECT_DIR "/Images/building.jpg" });

			poolTexture = new CubemapTexture(faces);
		}

		if (!this->device){
			//Tutorial: https://ffainelli.github.io/openal-example/
			this->device = alcOpenDevice(NULL);
			if (!this->device)
				puts("ERROR::NO_AUDIO_DEVICE");

			ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
			if (enumeration == AL_FALSE)
				puts("Enumeration not supported");
			else
				puts("Enumeration supported");

			this->context = alcCreateContext(this->device, NULL);
			if (!alcMakeContextCurrent(context))
				puts("Failed to make context current");

			this->source_pos = glm::vec3(0.0f, 5.0f, 0.0f);

			ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
			alListener3f(AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alListener3f(AL_VELOCITY, 0, 0, 0);
			alListenerfv(AL_ORIENTATION, listenerOri);

			alGenSources((ALuint)1, &this->source);
			alSourcef(this->source, AL_PITCH, 1);
			alSourcef(this->source, AL_GAIN, 1.0f);
			alSource3f(this->source, AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alSource3f(this->source, AL_VELOCITY, 0, 0, 0);
			alSourcei(this->source, AL_LOOPING, AL_TRUE);

			alGenBuffers((ALuint)1, &this->buffer);

			ALsizei size, freq;
			ALenum format;
			ALvoid* data;
			ALboolean loop = AL_TRUE;

			//Material from: ThinMatrix
			alutLoadWAVFile((ALbyte*)PROJECT_DIR "/Audios/water.wav", &format, &data, &size, &freq, &loop);
			alBufferData(this->buffer, format, data, size, freq);
			alSourcei(this->source, AL_BUFFER, this->buffer);

			if (format == AL_FORMAT_STEREO16 || format == AL_FORMAT_STEREO8)
				puts("TYPE::STEREO");
			else if (format == AL_FORMAT_MONO16 || format == AL_FORMAT_MONO8)
				puts("TYPE::MONO");

			alSourcePlay(this->source);

			// cleanup context
			//alDeleteSources(1, &source);
			//alDeleteBuffers(1, &buffer);
			//device = alcGetContextsDevice(context);
			//alcMakeContextCurrent(NULL);
			//alcDestroyContext(context);
			//alcCloseDevice(device);
		}
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	if (tw->lightBrowser->value() == lightMode::defaultLight) //default lighting from template
	{
		GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
		GLfloat lightPosition2[] = { 1, 0, 0, 0 };
		GLfloat lightPosition3[] = { 0, -1, 0, 0 };
		GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
		GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
		GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
		GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
		glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

		glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

		glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);
	}
	else if (tw->lightBrowser->value() == lightMode::directional) //one directional light
	{
		//Directional light
		float noAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float whiteDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float position[] = { 1.0f, 1.0f, 0.0f, 0.0f };

		glLightfv(GL_LIGHT0, GL_AMBIENT, noAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, position);

		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else if (tw->lightBrowser->value() == lightMode::point) //a point is light source
	{
		////Point light 
		float whiteDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float position[] = { -2.0f, 2.0f, -5.0f, 1.0f };

		glLightfv(GL_LIGHT0, GL_AMBIENT, whiteDiffuse);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, position);

		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else if (tw->lightBrowser->value() == lightMode::spot) //spot light, above ground
	{
		//Spot light
		float noAmbient[] = { 0.0f, 0.0f, 0.2f, 1.0f }; //low ambient light
		float diffuse[] = { 1.0f, 0.9f, 0.5f, 1.0f };
		float position[] = { 0.0f, 50.0f, 0.0f, 1.0f };

		//properties of the light
		glLightfv(GL_LIGHT1, GL_AMBIENT, noAmbient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT1, GL_POSITION, position);

		/*Spot properties*/
		//spot direction
		float xSpotDir = 0.0f, ySpotDir = -1.0f, zOffset = 0.0f;
		float direction[] = { xSpotDir, ySpotDir, zOffset };
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);

		//angle of the cone light emitted by the spot : value between 0 to 180
		float spotCutOff = 100;
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutOff);
		//exponent properties defines the concentration of the light
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 15.0f);
		//light attenuation (default values used here : no attenuation with the distance)
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT2);
	}
	else if (tw->lightBrowser->value() == lightMode::headlight) //headlight of train
	{ //it is just a spotlight that moves with the train and points in the direction of movement
		Pnt3f pos1 = findClosestKey(TrainWagon::t, TrainWagon::parameterCurveT).second;
		Pnt3f pos2 = findClosestKey(TrainWagon::t + 1.0f / divideLine, TrainWagon::parameterCurveT).second;
		Pnt3f directionVector = pos2 - pos1;
		directionVector.normalize();
		//Spot light
		float diffuse[] = { 1.0f, 0.95f, 0.4f, 1.0f };
		float position[] = { pos1.x, pos1.y, pos1.z, 1.0f };

		//properties of the light
		glLightfv(GL_LIGHT1, GL_AMBIENT, diffuse);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT1, GL_POSITION, position);

		/*Spot properties*/
		//spot direction
		float direction[] = { directionVector.x, directionVector.y, directionVector.z };
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);

		//angle of the cone light emitted by the spot : value between 0 to 180
		float spotCutOff = 45;
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutOff);
		//exponent properties defines the concentration of the light
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 15.0f);
		//light attenuation (default values used here : no attenuation with the distance)
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT2);
	}

	// set linstener position 
	if(selectedCube >= 0)
		alListener3f(AL_POSITION, 
			m_pTrack->points[selectedCube].pos.x,
			m_pTrack->points[selectedCube].pos.y,
			m_pTrack->points[selectedCube].pos.z);
	else
		alListener3f(AL_POSITION, 
			this->source_pos.x, 
			this->source_pos.y,
			this->source_pos.z);

	setupFloor();
	//glDisable(GL_LIGHTING);
	//if (tw->scenery->value())
	//{
	this->floorTexture->bind(0);
	drawFloor(500, 30, this->floorTexture->id);

	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}

	// draw skybox first
	this->cubemapShader->Use();

	glm::mat4 view;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view[0][0]);

	glm::mat4 projection;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(this->cubemapShader->Program, "view"), 1, GL_FALSE, &glm::mat4(glm::mat3(view))[0][0]); //without the translation
	glUniformMatrix4fv(glGetUniformLocation(this->cubemapShader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);

	glm::vec3 light_dir1 = glm::vec3(-9999.0, -9999.0, -9999.0);
	glm::vec3 light_pos1 = glm::vec3(-9999.0, -9999.0, -9999.0);

	if (tw->lightBrowser->value() == lightMode::headlight)
	{
		Pnt3f pos1 = findClosestKey(TrainWagon::t, TrainWagon::parameterCurveT).second;
		Pnt3f pos2 = findClosestKey(TrainWagon::t + 1.0f / divideLine, TrainWagon::parameterCurveT).second;
		Pnt3f directionVector = pos2 - pos1;
		directionVector.normalize();

		light_dir1 = glm::vec3(directionVector.x, directionVector.y, directionVector.z);
		light_pos1 = glm::vec3(pos1.x, pos1.y, pos1.z);
	}

	glUniform3f(glGetUniformLocation(this->cubemapShader->Program, "light_dir"), light_dir1[0], light_dir1[1], light_dir1[2]);
	glUniform3f(glGetUniformLocation(this->cubemapShader->Program, "light_pos"), light_pos1[0], light_pos1[1], light_pos1[2]);

	// Bind the vertex array object and texture
	glBindVertexArray(this->cubemap->vao);
	this->skyboxTexture->bind(0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glUseProgram(0);


	this->cubemapShader->Use();

	// Create a translation matrix to move the cubemap to the desired position
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 10.0f, -50.0f));
	glm::mat4 viewTranslated = view * translation; // Apply the translation to the original view matrix

	glUniformMatrix4fv(glGetUniformLocation(this->cubemapShader->Program, "view"), 1, GL_FALSE, &viewTranslated[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(this->cubemapShader->Program, "projection"), 1, GL_FALSE, &projection[0][0]);

	glUniform3f(glGetUniformLocation(this->cubemapShader->Program, "light_dir"), light_dir1[0], light_dir1[1], light_dir1[2]);
	glUniform3f(glGetUniformLocation(this->cubemapShader->Program, "light_pos"), light_pos1[0], light_pos1[1], light_pos1[2]);

	// Bind the vertex array object and texture
	glBindVertexArray(this->poolCubemap->vao);
	this->poolTexture->bind(0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glUseProgram(0);



	setUBO();
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);


	//bind shader
	this->shader->Use();
	
	//set variables
	float dirX = cos(tw->direction->value() * 3.14159f / 180.0f);
	float dirY = sin(tw->direction->value() * 3.14159f / 180.0f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "time"), t_time);
	glUniform1f(glGetUniformLocation(this->shader->Program, "amplitude"), tw->amplitude->value());
	glUniform1f(glGetUniformLocation(this->shader->Program, "wavelength"), tw->waveLength->value());
	glUniform1f(glGetUniformLocation(this->shader->Program, "speed"), 5.0f);
	glUniform2f(glGetUniformLocation(this->shader->Program, "direction"), dirX, dirY);
	glm::vec3 light_dir2 = glm::vec3(-9999.0, -9999.0, -9999.0);
	glm::vec3 light_pos2 = glm::vec3(-9999.0, -9999.0, -9999.0);

	if (tw->lightBrowser->value() == lightMode::headlight)
	{
		Pnt3f pos1 = findClosestKey(TrainWagon::t, TrainWagon::parameterCurveT).second;
		Pnt3f pos2 = findClosestKey(TrainWagon::t + 1.0f / divideLine, TrainWagon::parameterCurveT).second;
		Pnt3f directionVector = pos2 - pos1;
		directionVector.normalize();

		light_dir2 = glm::vec3(-directionVector.x, -directionVector.y, -directionVector.z);
		light_pos2 = glm::vec3(pos1.x, pos1.y, pos1.z);
	}

	glUniform3f(glGetUniformLocation(this->shader->Program, "light_dir"), light_dir2[0], light_dir2[1], light_dir2[2]);
	glUniform3f(glGetUniformLocation(this->shader->Program, "light_pos"), light_pos2[0], light_pos2[1], light_pos2[2]);


	glm::mat4 model = glm::mat4();
	model = glm::translate(model, this->source_pos);

	glUniformMatrix4fv(
		glGetUniformLocation(this->shader->Program, "u_model"), 1, GL_FALSE, &model[0][0]);

	this->texture->bind(0);
	glUniform1i(glGetUniformLocation(this->shader->Program, "u_texture"), 0);

	glUniform1i(glGetUniformLocation(this->shader->Program, "skybox"), 1);

	this->skyboxTexture->bind(1);

	glUniform1i(glGetUniformLocation(this->shader->Program, "tiles"), 2);

	this->poolTexture->bind(2);

	//bind VAO
	glBindVertexArray(this->plane->vao);

	////to see through water
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, this->plane->element_amount, GL_UNSIGNED_INT, 0);

	//glDisable(GL_BLEND);

	//unbind VAO
	glBindVertexArray(0);

	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);

	// Create an array to store the pixel data

	if (tw->effectBrowser->value() != 1)
	{
		int windowWidth = tw->trainView->w() * 2;
		int windowHeight = tw->trainView->h() * 2;

		// Create an array to store the pixel data
		GLubyte* pixelData = new GLubyte[3 * windowWidth * windowHeight];



		// Make the GL context current
		tw->trainView->make_current();

		// Read the pixel data from the frame buffer
		glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, pixelData);

		GLubyte* outputPixels = new GLubyte[3 * windowWidth * windowHeight];
		if (tw->effectBrowser->value() == 2)
		{
			for (int i = 0; i < windowWidth * windowHeight; i++)
			{
				//I = 0.299r + 0.587g + 0.114b
				outputPixels[i * 3] = outputPixels[i * 3 + 1] = outputPixels[i * 3 + 2] =
					0.299 * pixelData[i * 3] + 0.587 * pixelData[i * 3 + 1] + 0.114 * pixelData[i * 3 + 2];
			}
		}
		else if (tw->effectBrowser->value() == 3)
		{
			for (int i = 0; i < 3 * windowWidth * windowHeight; i++)
			{
				//I = 0.299r + 0.587g + 0.114b
				outputPixels[i] = 255 - pixelData[i];
			}
		}
		else if (tw->effectBrowser->value() == 4)
		{
			cartoonizeImage(pixelData, windowWidth, windowHeight, outputPixels);
		}

		else if (tw->effectBrowser->value() == 5)
		{
			watercolorizeImage(pixelData, windowWidth, windowHeight, outputPixels);
		}
		else if (tw->effectBrowser->value() == 6)
		{
			for (int i = 0; i < windowWidth * windowHeight; i++)
			{
				//I = 0.299r + 0.587g + 0.114b
				unsigned int pixel = 0.299 * pixelData[i * 3] + 0.587 * pixelData[i * 3 + 1] + 0.114 * pixelData[i * 3 + 2];

				if (pixel < 128)
					pixel = 0;
				else
					pixel = 255;
				outputPixels[i * 3] = outputPixels[i * 3 + 1] = outputPixels[i * 3 + 2] = pixel;
			}
		}
		else if (tw->effectBrowser->value() == 7)
		{
			pixelateImage(pixelData, windowWidth, windowHeight, 15, outputPixels);
		}

		// Draw the modified image using glDrawPixels
		glDrawPixels(windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, outputPixels);

		// Clean up the memory
		delete[] pixelData;
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		}
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1, 0, 0);
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else if (tw->trainCam->value())
	{
		// Define the camera height, as to see above the tracks
		float cameraHeight = 4.0f;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90.0f, aspect, 0.1f, 1000.0f);

		// Set up the modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Pnt3f pos1 = findClosestKey(TrainWagon::t, TrainWagon::parameterCurveT).second;
		Pnt3f pos2 = findClosestKey(TrainWagon::t + 1.0f / divideLine, TrainWagon::parameterCurveT).second;

		//up vector of train
		Pnt3f upVector = findClosestKey(TrainWagon::t, TrainWagon::parameterOrientT).second;

		//direction the train is following
		Pnt3f cameraDir = pos2 - pos1;
		cameraDir.normalize();

		//move camera position a bit up
		Pnt3f cameraPos = pos1 + cameraHeight * upVector - cameraHeight * cameraDir;

		gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
			pos1.x + cameraDir.x,
			pos1.y + cameraDir.y,
			pos1.z + cameraDir.z,
			upVector.x, upVector.y, upVector.z);
	}
	else
	{
		// Define the camera height, as to see above the tracks
		float cameraHeight = 4.0f;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90.0f, aspect, 0.1f, 1000.0f);

		// Set up the modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//up vector of train
		Pnt3f upVector(0.0f, 1.0f, 0.0f);

		//direction the train is following
		Pnt3f cameraDir = person.direction;
		cameraDir.normalize();

		//move camera position a bit up
		Pnt3f cameraPos = person.position + cameraHeight * upVector - cameraHeight * cameraDir;

		gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
			person.position.x + cameraDir.x,
			person.position.y + cameraDir.y,
			person.position.z + cameraDir.z,
			upVector.x, upVector.y, upVector.z);
	}
}


//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}

	if (tw->splineBrowser->value() == trackMode::linear) //use linear tracks
	{
		TrainWagon::setParameter(drawLinearTrack(doingShadows));
	}
	else //draw curve tracks using matrix
	{
		TrainWagon::setParameter(drawCurveTrack(doingShadows));
	}

	if (!tw->trainCam->value()) //when train view dont draw train. Otherwise cant see 
	{
		front.draw(doingShadows, 0); //draw train

		for (int i = 0; i < wagons.size(); i++)
		{
			wagons[i].draw(doingShadows, i + 1);
		}

		//smoke shouldt have shadows
		if (!doingShadows && tw->smokeButton->value())
		{
			Pnt3f position = findClosestKey(TrainWagon::t, TrainWagon::parameterCurveT).second;
			Pnt3f positionNext = findClosestKey(TrainWagon::t + 1.0f / divideLine, TrainWagon::parameterCurveT).second;

			Pnt3f orientation = findClosestKey(TrainWagon::t, TrainWagon::parameterOrientT).second;
			Pnt3f direction = positionNext - position;
			direction.normalize();
			direction = direction * 3.0f;

			position = position + orientation * 5.0f + direction;

			//update smoke every 2 units of time
			float deltaTime = 2.0f;
			particleSystemManager.update(deltaTime, position, orientation);
			float dirX = cos(tw->direction->value() * 3.14159f / 180.0f);
			float dirZ = sin(tw->direction->value() * 3.14159f / 180.0f);
			Pnt3f dir = Pnt3f(dirX, 0.0f, dirZ);
			dir.normalize();
			particleSystemManager.setDirection(dir);

			particleSystemManager.render();
		}
	}

	if (!tw->smokeButton->value()) //if no smoke, clear particl
	{
		particleSystemManager.clear();
	}

	//draw beams only whe no cubic bspline
	if (tw->splineBrowser->value() != trackMode::bSpline)
	{
		for (size_t i = 0; i < m_pTrack->points.size(); ++i)
		{
			drawBeam(doingShadows, m_pTrack->points[i].pos, m_pTrack->points[i].orient);
		}
	}

	//draw sphere at top of building
	glPushMatrix();
	if (!doingShadows)
	{
		glColor3ub(184, 134, 11);
	}
	glm::vec3 sphereCenter = { -50.0f, 30.0f, -50.0f };
	glTranslatef(sphereCenter[0], sphereCenter[1], sphereCenter[2]);

	//for level of detail
	// Extract the camera position (eye position)
	GLfloat modelViewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
	GLfloat cameraPosition[3] = { -modelViewMatrix[12], -modelViewMatrix[13], -modelViewMatrix[14] };

	float distance = (sphereCenter[0] - cameraPosition[0]) * (sphereCenter[0] - cameraPosition[0]) +
		(sphereCenter[1] - cameraPosition[1]) * (sphereCenter[1] - cameraPosition[1]) +
		(sphereCenter[2] - cameraPosition[2]) * (sphereCenter[2] - cameraPosition[2]);

	distance = sqrt(distance);

	if (distance < 500.0f)
		drawSphere(7.5f, tw->subdivisions->value());
	else
		drawSphere(7.5f, 4);

	if (!doingShadows)
	{
		glColor3ub(50, 0, 0);
	}
	glTranslatef(0.0f, -10.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // align the cone along the y-axis
	glutSolidCone(5.0f, 7.5f, 10.0f, 10.0f);
	glPopMatrix();

	if (!doingShadows)
	{
		srand(100);
		//draw cactus
		for (int i = 0; i < 25; i++)
		{
			float height = (rand() % 30) + 20;
			drawTrees(rand() % 401 - 200, 0.0f, rand() % 401 - 200, height, height / 2, tw->subdivisions->value());
		}
	}

	//draw ferris and carousel
	drawCarousel(doingShadows, t_time);
	drawFerrisWheel(doingShadows, t_time);

	//move person if keys pressed
	if (tw->runButton->value())
	{
		if (GetKeyState('W') & 0x8000)
		{
			person.advance();
		}

		if (GetKeyState('S') & 0x8000)
		{
			person.reverse();
		}

		if (GetKeyState('A') & 0x8000)
		{
			person.rotateDirection(-2);
		}

		if (GetKeyState('D') & 0x8000)
		{
			person.rotateDirection(2);
		}
	}

	//draw person
	if (!doingShadows)
	{
		person.draw();
	}
}

tuple<map<float, Pnt3f, FloatCompare>, map<float, float, FloatCompare>, map<float, Pnt3f, FloatCompare>>
TrainView::drawLinearTrack(bool doingShadows)
{
	//store already computed values and parameters
	map<float, Pnt3f, FloatCompare> parameterCurveT;
	map<float, float, FloatCompare> arcLengthIntegral{ {0.0f, 0.0f} };
	map<float, Pnt3f, FloatCompare> parameterOrientT;
	// draw the track
	float continuousT = 0; //for arc length parametrization
	float arcLength = 0.0f;
	for (size_t i = 0; i < m_pTrack->points.size(); i++)
	{
		//INITIALIZE
		//pos
		Pnt3f cp_pos_p1 = m_pTrack->points[i].pos;
		Pnt3f cp_pos_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;

		//orient
		Pnt3f cp_orient_p1 = m_pTrack->points[i].orient;
		Pnt3f cp_orient_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;

		float percent = 1.0f / divideLine;
		float t = 0;
		Pnt3f qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;

		parameterCurveT[t] = qt;

		bool waitGap = true; //gaps between sleepers
		Pnt3f qtLast = qt;
		float sleeperLength = 2.0f; // length of each rail sleeper
		float sleeperGap = 1.0f; // gap between sleepers
		for (size_t j = 0; j < divideLine; j++)
		{
			//DRAW LINES
			Pnt3f qt0 = qt;

			t += percent;
			continuousT += percent;

			qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;
			parameterCurveT[continuousT] = qt; //store parameter t and its Point

			Pnt3f qt1 = qt;

			Pnt3f orient_t = (1 - t) * cp_orient_p1 + t * cp_orient_p2;
			orient_t.normalize();
			parameterOrientT[continuousT] = orient_t;

			Pnt3f cross_t = (qt1 - qt0) * orient_t;
			cross_t.normalize();
			cross_t = cross_t * 2.5f;

			//draw lines of track
			drawTrackPart(doingShadows, qt1 + cross_t - orient_t * 1.0f,
				qt0 + cross_t - orient_t * 1.0f, orient_t,
				0.2f, 1.0f, (qt1 - qt0).magnitude() + 0.05f, 80, 80, 80);

			drawTrackPart(doingShadows, qt1 - cross_t - orient_t * 1.0f,
				qt0 - cross_t - orient_t * 1.0f, orient_t,
				0.2f, 1.0f, (qt1 - qt0).magnitude() + 0.05f, 80, 80, 80);

			arcLength += (qt1 - qt0).magnitude(); //current arc length
			arcLengthIntegral[continuousT] = arcLength; //current point for current arc length

			////DRAW RAIL SLEEPERS
			if (waitGap)
			{
				sleeperGap -= (qt1 - qt0).magnitude();

				if (sleeperGap <= 0)
				{
					sleeperGap = 3.0f; //reset sleeperGap distance
					waitGap = false;
					qtLast = qt;
				}
			}
			else
			{
				sleeperLength -= (qt1 - qt0).magnitude();

				if (sleeperLength <= 0)
				{
					sleeperLength = 2.0f;

					waitGap = true;
					cross_t = cross_t * 1.6f;
					//draw sleeper with wood color
					drawTrackPart(doingShadows, qt - orient_t * 1.5f, qtLast - orient_t * 1.5f,
						orient_t, (cross_t * 2.0f).magnitude(), 0.4f, (qt - qtLast).magnitude(),
						102, 51, 0);

					qtLast = qt;
				}
			}
		}
	}

	return make_tuple(parameterCurveT, arcLengthIntegral, parameterOrientT);
}


tuple<map<float, Pnt3f, FloatCompare>, map<float, float, FloatCompare>, map<float, Pnt3f, FloatCompare>>
TrainView::drawCurveTrack(bool doingShadows)
{
	//store calculated parameters and values
	map<float, Pnt3f, FloatCompare> parameterCurveT;
	map<float, float, FloatCompare> arcLengthIntegral{ {0.0f, 0.0f} };
	map<float, Pnt3f, FloatCompare> parameterOrientT;

	//define matrix
	float M[4][4];
	if (tw->splineBrowser->value() == trackMode::cardinal)
	{
		//tensions spline. Since tension is actually at max when 0. Do 1 - tension, more fancy
		float tension = 1.0f - tw->tensionSlider->value();
		float matrix[4][4] = {
			{-tension / 2.0f, (2.0f * tension) / 2.0f, -tension / 2.0f, 0.0f / 2.0f},
			{(4.0f - tension) / 2.0f, (tension - 6.0f) / 2.0f, 0.0f / 2.0f, 2.0f / 2.0f},
			{(tension - 4.0f) / 2.0f, (-2.0f * (tension - 3.0f)) / 2.0f, tension / 2.0f, 0.0f / 2.0f},
			{tension / 2.0f, -tension / 2.0f, 0.0f / 2.0f, 0.0f / 2.0f}
		};
		memcpy(M, matrix, sizeof(matrix));
	}
	else if (tw->splineBrowser->value() == trackMode::bSpline)
	{
		float matrix[4][4] = {
			{-1.0f / 6.0f, 3.0f / 6.0f, -3.0f / 6.0f, 1.0f / 6.0f},
			{3.0f / 6.0f, -6.0f / 6.0f, 0.0f / 6.0f, 4.0f / 6.0f},
			{-3.0f / 6.0f, 3.0f / 6.0f, 3.0f / 6.0f, 1.0f / 6.0f},
			{1.0f / 6.0f, 0.0f / 6.0f, 0.0f / 6.0f, 0.0f / 6.0f}
		};

		memcpy(M, matrix, sizeof(matrix));
	}

	float continuousT = 0.0f;
	float arcLength = 0.0f;
	// draw the track
	for (size_t i = 0; i < m_pTrack->points.size(); i++)
	{
		//index for control points, use to draw curve
		size_t indexes[4] = { (i - 1 + m_pTrack->points.size()) % m_pTrack->points.size(),
			i, (i + 1) % m_pTrack->points.size(),
			(i + 2) % m_pTrack->points.size()
		};

		//control points position
		float G_pos[3][4] = {
			{m_pTrack->points[indexes[0]].pos.x, m_pTrack->points[indexes[1]].pos.x,
			m_pTrack->points[indexes[2]].pos.x, m_pTrack->points[indexes[3]].pos.x},

			{m_pTrack->points[indexes[0]].pos.y, m_pTrack->points[indexes[1]].pos.y,
			m_pTrack->points[indexes[2]].pos.y, m_pTrack->points[indexes[3]].pos.y},

			{m_pTrack->points[indexes[0]].pos.z, m_pTrack->points[indexes[1]].pos.z,
			m_pTrack->points[indexes[2]].pos.z, m_pTrack->points[indexes[3]].pos.z}
		};

		//control points orientation
		float G_orient[3][4] = {
			{m_pTrack->points[indexes[0]].orient.x, m_pTrack->points[indexes[1]].orient.x,
			m_pTrack->points[indexes[2]].orient.x, m_pTrack->points[indexes[3]].orient.x},

			{m_pTrack->points[indexes[0]].orient.y, m_pTrack->points[indexes[1]].orient.y,
			m_pTrack->points[indexes[2]].orient.y, m_pTrack->points[indexes[3]].orient.y},

			{m_pTrack->points[indexes[0]].orient.z, m_pTrack->points[indexes[1]].orient.z,
			m_pTrack->points[indexes[2]].orient.z, m_pTrack->points[indexes[3]].orient.z}
		};


		float t = 0;
		Pnt3f qt = multiplyControlPoint(G_pos, M, t);

		parameterCurveT[t] = qt;

		bool waitGap = true; //gaps between sleepers
		Pnt3f qtLast = qt;
		float sleeperLength = 2.0f; // length of each rail sleeper
		float sleeperGap = 1.0f; // gap between sleepers

		Pnt3f lastDirection; //for adaptive subdivisions

		for (size_t j = 0; j < divideLine; j++)
		{
			float percent = 1.0f / divideLine;
			//DRAW LINES
			t += percent;
			continuousT += percent;

			if (t > 1.0f)
			{
				float diff = t - 1.0f;
				t = 1.0f;
				continuousT -= diff;
			}

			Pnt3f qt0 = qt;

			qt = multiplyControlPoint(G_pos, M, t);
			parameterCurveT[continuousT] = qt; //store parameter t and its Point

			Pnt3f qt1 = qt;

			Pnt3f orient_t = multiplyControlPoint(G_orient, M, t);
			orient_t.normalize();
			parameterOrientT[continuousT] = orient_t;

			Pnt3f direction = (qt1 - qt0);
			Pnt3f cross_t = (qt1 - qt0) * orient_t;
			direction.normalize();

			cross_t.normalize();
			cross_t = cross_t * 2.5f;

			//draw lines of track
			drawTrackPart(doingShadows, qt1 + cross_t - orient_t * 1.0f,
				qt0 + cross_t - orient_t * 1.0f, orient_t,
				0.2f, 1.0f, (qt1 - qt0).magnitude() + 0.05f, 80, 80, 80);

			drawTrackPart(doingShadows, qt1 - cross_t - orient_t * 1.0f,
				qt0 - cross_t - orient_t * 1.0f, orient_t,
				0.2f, 1.0f, (qt1 - qt0).magnitude() + 0.05f, 80, 80, 80);

			arcLength += (qt1 - qt0).magnitude(); //current arc length
			arcLengthIntegral[continuousT] = arcLength; //current t for current arc length

			////DRAW RAIL SLEEPERS
			if (waitGap)
			{
				sleeperGap -= (qt1 - qt0).magnitude();

				if (sleeperGap <= 0)
				{
					sleeperGap = 3.0f; //restart sleeperGap distance
					waitGap = false;
					qtLast = qt;
				}
			}
			else
			{
				sleeperLength -= (qt1 - qt0).magnitude();

				if (sleeperLength <= 0)
				{
					sleeperLength = 2.0f;

					waitGap = true;
					cross_t = cross_t * 1.6f;

					//draw sleeper with wood color
					drawTrackPart(doingShadows, qt - orient_t * 1.5f, qtLast - orient_t * 1.5f,
						orient_t, (cross_t * 2.0f).magnitude(), 0.4f, (qt - qtLast).magnitude(),
						102, 51, 0);

					qtLast = qt;
				}
			}
		}
	}
	return make_tuple(parameterCurveT, arcLengthIntegral, parameterOrientT);
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();		

	// where is the mouse?
	int mx = Fl::event_x(); 
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
						5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}

void TrainView::setUBO()
{
	float wdt = this->pixel_w();
	float hgt = this->pixel_h();

	glm::mat4 view_matrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
	//HMatrix view_matrix; 
	//this->arcball.getMatrix(view_matrix);

	glm::mat4 projection_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);
	//projection_matrix = glm::perspective(glm::radians(this->arcball.getFoV()), (GLfloat)wdt / (GLfloat)hgt, 0.01f, 1000.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}