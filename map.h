/*
	Modified from: https://github.com/23scurtu/3D_Terrain_Generation
*/

#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using  namespace std;

class Map
{
        int totalsize, x, y; 
		float **heightmap; 
    public:
        vector<vector<float>> elements;
        Map(int x, int y);
		Map(float **input, int x, int y);

        void display(); //Output data to console
        void clearElements();
        bool isedge(int x, int y);
        int getX();
        int getY();
		int getTotalsize();
		float getElement(int x, int y);
        vector<vector<float> > getElements();
        void setElement(int x, int y, int val);
        void loadElements(vector<vector<float> > input);

		//variouse overloaded functions to generate the heightmap
		//based on various parameters.
		void generate(int size, int roughness, int min, int max);
		void generate(int size, int roughness);
		void generate(int size, float roughness);
		void generate(int size, float mag, float roughness);

		void square(int x, int y, int size, int offset);
		void diamond(int x, int y, int size, int offset);
		GLvoid* diamondSquare(int start, int mapsize);
		bool isPastEdge(int x, int y);

		//Caps the heightmap data by setting values greater than
		//upper equal to upper and vice versa
		void capData(int lower, int upper);
		void multiplyData(float multiple);

		//Smooths terrain using a mean/box filter
		void smooth(int filterSize, int passes);
		void dynamicHeightSmooth(int filterSize, int passMin, int passMax);



};