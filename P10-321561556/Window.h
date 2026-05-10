#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getX() { return x; }
	GLfloat getY() { return y; }
	GLfloat getZ() { return z; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevex_heli() { return muevex_heli; }
	GLfloat getheli_dir() { return heli_dir; }
	GLfloat getz_Pez() { return z_Pez;  }
	GLfloat gety_Pez() { return y_Pez; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	int getapagarLampara() { return apagarLampara;  }
	int getapagarLampara2() { return apagarLampara2; }
	int getcolorFaro() { return colorFaro; }

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevex_heli = 0;
	GLint heli_dir = 1;
	GLfloat z_Pez = 0, y_Pez = 0;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
	int apagarLampara = 0;
	int apagarLampara2 = 0;
	int colorFaro = 0;
	int x = 0, y = 0, z = 0;

};

