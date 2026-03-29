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
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	GLfloat getPDD() { return PDD; }
	GLfloat getPDT() { return PDT; }
	GLfloat getPID() { return PID; }
	GLfloat getPIT() { return PIT; }
	GLfloat getDesplazamiento() { return desplazamiento; }
	GLfloat getRuedas() { return ruedas; }
	GLfloat getCofre() { return cofre; }
	
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
	GLfloat PDD = 0, PDT = 0, PID = 0, PIT = 0;
	GLfloat flag_PDD = 0, flag_PDT = 0, flag_PID = 0, flag_PIT = 0;
	GLfloat desplazamiento = 0, ruedas = 0, cofre = 0;
	GLfloat flag_cofre = 0;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

