#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <GL/gl.h>

using namespace std;

class CubemapTexture
{
public:
	CubemapTexture(vector<std::string> faces)
	{
		//load 6 faces for cubemap
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		for (unsigned int i = 0; i < faces.size(); i++)
		{
			cv::Mat img = cv::imread(faces[i], cv::IMREAD_COLOR);
			cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);

			img.release();
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void bind(GLuint textureUnit)
	{
		//bind textures
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}

private:
	GLuint textureID = -1;
};