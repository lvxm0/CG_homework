#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
//#include "imgui.h"
//#include "imgui_impl_glfw_gl3.h"
#include "shader.h"
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

unsigned int VAO, VBO;
float PosX = 0;
float PosY = 0;
// mouse click point
vector<glm::vec2> point;
float vertices[100];
// surpose 1000 point
int curvePointCount = 1000;
//animation time
float time = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursor_pos_callback(GLFWwindow* window, double x, double y);
void click_callback(GLFWwindow* window, int button, int action, int mods);
long int jiecheng(int x);
float* Bezier(vector<glm::vec2> point);
void animation(vector<glm::vec2> vertex);

int main() {
	glfwInit();
	// OpenGL version & mode setting
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window & context/viewpoint setting
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "16340164-Bezier", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// shader init
	Shader shader("vShaderSrc.txt", "fShaderSrc.txt");
	point = vector<glm::vec2>();
	/*
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsLight();
	*/
	
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, click_callback);

	shader.use();
	
	// ---- render loop ----
	while (!glfwWindowShouldClose(window)) {

		processInput(window);
		glfwPollEvents();
		//ImGui_ImplGlfwGL3_NewFrame();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//point put in vertices
		for (int i = 0; i < point.size(); i++) {
			vertices[i * 2] = point[i].x;
			vertices[i * 2 + 1] = point[i].y;
		}
		//vertics
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * point.size(), vertices, GL_STATIC_DRAW);	
		//postion
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//draw arr
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, point.size());
		glDrawArrays(GL_LINE_STRIP, 0, point.size());
		
		//beizer curve
		float* curve = Bezier(point);
		//curve point
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * curvePointCount, curve, GL_STATIC_DRAW);
		//postion
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// draw arr  1000  point 
		glPointSize(1.0f);
		glDrawArrays(GL_POINTS, 0, curvePointCount);
		// 动态呈现效果
		if (point.size() >= 3) {
			animation(point);
			time += 0.001;
			if (time > 1) {
				Sleep(1000);
				time = 0;
			}

		}
		
		//ImGui::Render();
		//ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}


	//ImGui_ImplGlfwGL3_Shutdown();
	//ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	// press ESC to close
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	PosX = (x - WINDOW_WIDTH / 2) / (WINDOW_WIDTH / 2);
	PosY = ((WINDOW_HEIGHT - y) - WINDOW_HEIGHT / 2) / (WINDOW_HEIGHT / 2);
}
// 阶乘
long int jiecheng(int x) {
	if (x == 0) return 1;
	int result = 1;
	for (int i = 1; i <= x; i++) {
		result *= i;
	}
	return result;
}

//bezier curve 公式
float* Bezier(vector<glm::vec2> point) {
	//beizer curve 点个数
	int num = 0;
	//beizer curve 点坐标
	float* bezierCurve = new float[10000];
	for (float t = 0; t < 1; t += 0.001, num++) {
		// x,y 坐标 = 0
		bezierCurve[num * 2] = 0;
		bezierCurve[num * 2 + 1] = 0;
		for (int i = 0, n = point.size() - 1; i <= n; i++) {
			// 常数部分 C=n!/i!(n-i)!
			float C = jiecheng(n) / (jiecheng(i) * jiecheng(n - i));
			//X
			bezierCurve[num * 2] += C * point[i].x  * pow(1 - t, n - i) * pow(t, i);
			//Y
			bezierCurve[num * 2 + 1] += C * point[i].y  * pow(1 - t, n - i) * pow(t, i);
		}

	}

	return bezierCurve;
}

// 动态呈现
void animation(vector<glm::vec2> vertex) {
	float animationVertex[10000];
	int n = vertex.size();
	if (n == 1) return;

	vector<glm::vec2> next = vector<glm::vec2>();
	for (int i = 0; i < n - 1; i++) {
		float tempx = (1 - time) * vertex[i].x + time * vertex[i + 1].x;
		float tempy = (1 - time) * vertex[i].y + time * vertex[i + 1].y;
		glm::vec2 temp = glm::vec2(tempx, tempy);
		 animationVertex[i * 2] = tempx;
		 animationVertex[i * 2 + 1] = tempy;
		next.push_back(temp);
	}
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * next.size(),  animationVertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, next.size());

	glPointSize(1.0f);
	glDrawArrays(GL_LINE_STRIP, 0, next.size());

	animation(next);
}
void click_callback(GLFWwindow* window, int button, int action, int mods) {
	// 左键添加顶点
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		bool flag = true;
		for (int i = 0; i < point.size(); i++) {
			if (point[i].x == PosX && point[i].y == PosY) {
				flag = false;
			}
		}
		if (flag) {
			point.push_back(glm::vec2(PosX, PosY));
		}
	}
	// 右键删除顶点
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		vector<glm::vec2>::iterator iter;
		for (iter = point.begin(); iter != point.end(); iter++) {
			if (pow(iter->x - PosX, 2) + pow(iter->y - PosY, 2) <= 0.1*0.1) {
				point.erase(iter);
				break;
			}
		}
	}
}
