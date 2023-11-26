#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
void InitBuffer();
void reset();
void menu();
void timer(int value);
void animation_timer(int value);
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUp(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid MouseWheel(int wheel, int direction, int x, int y);
char* filetobuf(const char* file);
void ReadObj(FILE* path, int index);
GLvoid Display();
void animation1_reset();
void animation2_reset();

float floor_xz[36] = {
	 -100.0, 0.0, -100.0, 0.0, 1.0, 0.0,  -100.0, 0.0, 100.0, 0.0, 1.0, 0.0,  100.0, 0.0, 100.0, 0.0, 1.0, 0.0,
	 -100.0, 0.0, -100.0, 0.0, 1.0, 0.0,  100.0, 0.0, 100.0, 0.0, 1.0, 0.0,  100.0, 0.0, -100.0, 0.0, 1.0, 0.0
};

GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vao[401], vbo[401];
std::vector<GLfloat> data[400];
glm::vec3 cameraPos;      //--- 카메라 위치
glm::vec3 cameraDirection; //--- 카메라 바라보는 방향
glm::vec3 cameraUp;        //--- 카메라 위쪽 방향
int window_w, window_h;
int height_size, width_size;
float cameraPos_x, cameraPos_y, cameraPos_z;
float cube_x[20][20], cube_y[20][20], cube_z[20][20];
float length_width, length_height;               // 육면체 길이의 절반
BOOL key_1, key_2, key_3, key_t, key_y, key_Y;
BOOL key_w, key_a, key_s, key_d;
int key_c;                                       // 현재 선택된 빛의 색깔
float light_r[3], light_g[3], light_b[3];        // 빛의 색깔
int case_display;                                // 시점
float radian_y;                                  // 카메라 공전
int move_check[20][20];                         // 블록 위, 아래 이동 확인   0 위로 이동 / 1 아래 이동
float animation1_speed[20][20];                  // 애니메이션1 블록 이동속도 0.05 ~ 0.15
float speed;                                     // 블록 움직이는 속도
float camera_move_x, camera_move_z;              // 카메라 움직임 위치
float cameraDirection_x, cameraDirection_y, cameraDirection_z;      // 카메라 바라보는 방향
BOOL left_button;                                
int animation_speed;                             // 애니메이션 스피드
float cameraSpeed;
float pitch, yaw;                                // 마우스 화면 이동
float fov;                                       // 줌

void menu() {
	std::cout << "-----------명령어------------" << std::endl;
	std::cout << "1 : 애니메이션 1" << std::endl;
	std::cout << "2 : 애니메이션 2" << std::endl;
	std::cout << "3 : 애니메이션 3" << std::endl;
	std::cout << "t : 조명 on / off" << std::endl;
	std::cout << "c : 조명 색 변경" << std::endl;
	std::cout << "y/Y : 카메라 회전" << std::endl;
	std::cout << "+/- : 육면체 이동 속도 증가 / 감소" << std::endl;
	std::cout << "w/a/s/d : 카메라 이동" << std::endl;
	std::cout << "r : 리셋" << std::endl;
	std::cout << "q : 프로그램 종료" << std::endl;
}

void animation1_reset() {
	for (int i = 0; i < height_size; ++i) {
		for (int j = 0; j < width_size; ++j) {
			cube_y[i][j] = rand() % 401 * 0.01;
			animation1_speed[i][j] = (rand() % 11 + 5) * 0.01;
			move_check[i][j] = 0;
		}
	}
}

void animation2_reset() {
	float cnt = 0;
	for (int i = 0; i < height_size; ++i) {
		for (int j = 0; j < width_size; ++j) {
			cube_y[i][j] = cnt;
			animation1_speed[i][j] = 0.1;
			move_check[i][j] = 0;
		}
		cnt += 0.5;
	}
}

void animation3_reset() {
	for (int i = 0; i < height_size; ++i) {
		for (int j = 0; j < width_size; ++j) {
			cube_y[i][j] = 0;
			move_check[i][j] = 2;
			animation1_speed[i][j] = 0.1;
		}
	}
	move_check[0][0] = 0;
}

void reset() {
	key_1 = key_t = true;
	key_2 = key_3 = key_y = key_Y = false;
	key_w = key_a = key_s = key_d = false;
	key_c = 0;
	light_r[0] = 1.0, light_g[0] = 1.0, light_b[0] = 1.0;
	light_r[1] = 1.0, light_g[1] = 0.2, light_b[1] = 0.2;
	light_r[2] = 0.0, light_g[2] = 0.5, light_b[2] = 0.5;
	radian_y = 0;
	speed = 1;
	camera_move_x = camera_move_z = 0;
	cameraDirection_x = cameraDirection_y = cameraDirection_z = 0;
	left_button = false;
	animation_speed = 100;
	cameraSpeed = 0.5f;
	cameraPos_x = -10.0f;
	cameraPos_y = 20.0f;
	cameraPos_z = 10.0f;
	pitch = yaw = -40;
	fov = 50;
	while (1) {
		std::cout << "가로 세로 크기를 입력해 주세요(최소 5, 최대 20) : ";
		std::cin >> width_size;
		std::cin >> height_size;
		if (!(5 <= height_size && 20 >= height_size && 5 <= width_size && 20 >= width_size))
			std::cout << "범위에 벗어났습니다." << std::endl;
		else
			break;
	}
	menu();
	length_width = (static_cast <float>(10) / (width_size * 2));
	length_height = (static_cast <float>(10) / (height_size * 2));
	for (int i = 0; i < height_size; ++i) {
		for (int j = 0; j < width_size; ++j) {
			cube_x[i][j] = -5 + length_width + (length_width * 2 * j);
			cube_z[i][j] = -5 + length_height + (length_height * 2 * i);
		}
	}
	animation1_reset();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	const char* cube_obj = "cube.obj";
	FILE* file_cube[400];
	for (int i = 0; i < height_size * width_size; ++i) {
		file_cube[i] = fopen(cube_obj, "r");
		ReadObj(file_cube[i], i);
		fclose(file_cube[i]);
	}
}

void main(int argc, char** argv)
{
	srand(time(NULL));
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(200, 30);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example");
	glewExperimental = GL_TRUE;
	reset();
	glewInit();
	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMouseWheelFunc(MouseWheel);
	glutTimerFunc(100, timer, 1);
	glutTimerFunc(animation_speed, animation_timer, 1);
	glutMainLoop();
}

GLvoid Display() {
	glClearColor(0.2, 0.6, 0.9, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	case_display = 0;
	glViewport(0, 0, window_w, window_h);
	drawScene();
	case_display = 1;
	glViewport(650, 450, 100, 100);
	drawScene();

	glutSwapBuffers();
}

GLvoid drawScene() {
	glUseProgram(shaderProgramID);
	int modelLoc = glGetUniformLocation(shaderProgramID, "model"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderProgramID, "projection");
	if (case_display == 0) {
		cameraDirection_x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraDirection_y = sin(glm::radians(pitch));
		cameraDirection_z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		cameraPos = glm::vec3(cameraPos_x, cameraPos_y, cameraPos_z);      //--- 카메라 위치
		cameraDirection = glm::vec3(cameraDirection_x, cameraDirection_y, cameraDirection_z); //--- 카메라 바라보는 방향
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);        //--- 카메라 위쪽 방향

		glm::mat4 vTransform = glm::mat4(1.0f);
		vTransform = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::perspective(glm::radians(fov), (float)window_w / (float)window_h, 0.1f, 200.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
	}
	else if (case_display == 1) {
		float cameraPos1_x = 0.0f;
		float cameraPos1_y = 3.0f;
		float cameraPos1_z = 0.0f;

		glm::vec3 cameraPos1 = glm::vec3(cameraPos1_x, cameraPos1_y, cameraPos1_z);      //--- 카메라 위치
		glm::vec3 cameraDirection1 = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp1 = glm::vec3(0.0f, 0.0f, 1.0f);        //--- 카메라 위쪽 방향

		glm::mat4 vTransform = glm::mat4(1.0f);
		vTransform = glm::lookAt(cameraPos1, cameraDirection1, cameraUp1);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
	}

	unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	if(key_t)
		glUniform3f(lightColorLocation, light_r[key_c], light_g[key_c], light_b[key_c]);
	else
		glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos_x, cameraPos_y, cameraPos_z);

	glUniform3f(objColorLocation, 1.0, 0.5, 0.0);
	int cube_cnt = 0;
	for (int i = 0; i < height_size; ++i) {
		for (int j = 0; j < width_size; ++j) {
			glm::mat4 cube = glm::mat4(1.0f);
			glm::mat4 cube_T = glm::mat4(1.0f);
			glm::mat4 cube_S = glm::mat4(1.0f);
			cube_T = glm::translate(cube_T, glm::vec3(cube_x[i][j], cube_y[i][j], cube_z[i][j]));
			cube_S = glm::scale(cube_S, glm::vec3(length_width, cube_y[i][j], length_height));

			cube = cube_T * cube_S;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cube));
			glBindVertexArray(vao[cube_cnt]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			++cube_cnt;
		}
	}

	// 바닥 생성
	glUniform3f(objColorLocation, 0.1, 0.1, 0.3);
	glm::mat4 floor = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(floor));
	glBindVertexArray(vao[height_size * width_size]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	if(key_t)
		glUniform3f(lightPosLocation, 0.0, 30.0, 0.0);
	else
		glUniform3f(lightPosLocation, 0.0, 0.0, 0.0);
}

void InitBuffer()
{
	glGenVertexArrays(height_size * width_size + 1, vao);
	glGenBuffers(height_size * width_size + 1, vbo);

	for (int i = 0; i < height_size * width_size; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, data[i].size() * sizeof(GLfloat), data[i].data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	glBindVertexArray(vao[height_size * width_size]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[height_size * width_size]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_xz), floor_xz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void animation_timer(int value) {
	if (key_1 || key_2) {
		for (int i = 0; i < height_size; ++i) {
			for (int j = 0; j < width_size; ++j) {
				if (move_check[i][j] == 0) {
					cube_y[i][j] += animation1_speed[i][j];
					if (cube_y[i][j] >= 10.0) {
						cube_y[i][j] -= animation1_speed[i][j];
						move_check[i][j] = 1;
					}
				}
				else if (move_check[i][j] == 1) {
					cube_y[i][j] -= animation1_speed[i][j];
					if (cube_y[i][j] <= 0.0)
						move_check[i][j] = 0;
				}
			}
		}
	}
	else if (key_3) {
		for (int i = 0; i < height_size; ++i) {
			for (int j = 0; j < width_size; ++j) {
				if (move_check[i][j] == 0) {
					cube_y[i][j] += animation1_speed[i][j];
					if (cube_y[i][j] >= 10.0) {
						cube_y[i][j] -= animation1_speed[i][j];
						move_check[i][j] = 1;
					}
					if (cube_y[i][j] > 0.2 && j < width_size - 1 && move_check[i][j + 1] == 2) {
						move_check[i][j + 1] = 0;
					}
					else if (cube_y[i][j] > 0.2 && j == width_size - 1 && move_check[i + 1][0] == 2) {
						move_check[i + 1][0] = 0;
					}
				}
				else if (move_check[i][j] == 1) {
					cube_y[i][j] -= animation1_speed[i][j];
					if (cube_y[i][j] <= 0.0)
						move_check[i][j] = 0;
				}
			}
		}
	}
	Display();
	glutTimerFunc(animation_speed, animation_timer, 1);
}

void timer(int value) {
	if (key_y) {
		radian_y += 10.0f;
		if (radian_y >= 360.0f)
			radian_y = 0;
	}
	if (key_Y) {
		radian_y -= 10.0f;
		if (radian_y <= -360.0f)
			radian_y = 0;
	}
	if (key_w) {
		cameraPos += cameraSpeed * cameraDirection;
	}
	if (key_a) {
		cameraPos -= cameraSpeed * cameraDirection;
	}
	if (key_s) {
		cameraPos -= glm::normalize(glm::cross(cameraDirection, cameraUp)) * cameraSpeed;
	}
	if (key_d) {
		cameraPos += glm::normalize(glm::cross(cameraDirection, cameraUp)) * cameraSpeed;
	}
	Display();
	glutTimerFunc(100, timer, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		key_1 = true;
		key_2 = key_3 = false;
		animation1_reset();
		break;
	case '2':
		key_2 = true;
		key_1 = key_3 = false;
		animation2_reset();
		break;
	case '3':
		key_3 = true;
		key_2 = key_1 = false;
		animation3_reset();
		break;
	case 't':
		key_t = !key_t;
		break;
	case 'c':
		key_c = (key_c + 1) % 3;
		break;
	case 'y':
		key_y = !key_y;
		key_Y = false;
		break;
	case 'Y':
		key_Y = !key_Y;
		key_y = false;
		break;
	case 'w':
		key_w = true;
		cameraPos += cameraSpeed * cameraDirection;
		break;
	case 'a':
		key_a = true;
		break;
	case 's':
		key_s = true;
		break;
	case 'd':
		key_d = true;
		break;
	case '+':
		if (animation_speed > 20)
			animation_speed -= 10;
		break;
	case '-':
		if (animation_speed < 300)
			animation_speed += 10;
		break;
	case 'r':   // 리셋
		reset();
		InitBuffer();
		break;
	case 'q':   // 프로그램 종료
		std::cout << "프로그램을 종료합니다." << std::endl;
		glutLeaveMainLoop();
		break;
	}
	Display();
}

GLvoid KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		key_w = false;
		break;
	case 'a':
		key_a = false;
		break;
	case 's':
		key_s = false;
		break;
	case 'd':
		key_d = false;
		break;
	}
	drawScene();
}

float prev_mouse_x, prev_mouse_y;
void Mouse(int button, int state, int x, int y) {
	float normalized_x, normalized_y;

	normalized_x = (2.0 * x / 800) - 1.0;
	normalized_y = 1.0 - (2.0 * y / 600);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		left_button = true;
		prev_mouse_x = normalized_x;  // 저장된 이전 마우스 위치
		prev_mouse_y = normalized_y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_button = false;
	}
}

// 카메라 회전으로 바라보는 방향으로 이동할 수 있게
void Motion(int x, int y) {
	float normalized_x, normalized_y;

	normalized_x = (2.0 * x / 800) - 1.0;
	normalized_y = 1.0 - (2.0 * y / 600);
	if (left_button == true) {
		float delta_x = normalized_x - prev_mouse_x;
		float delta_y = normalized_y - prev_mouse_y;

		prev_mouse_x = normalized_x;
		prev_mouse_y = normalized_y;

		yaw += delta_x * 50;
		pitch += delta_y * 50;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;


	}
	Display();
}

GLvoid MouseWheel(int wheel, int direction, int x, int y) {
	if (direction == 1) {
		if (fov >= 1.0f && fov <= 70.0f)
			fov -= 1;
		if (fov <= 1.0f)
			fov = 1.0f;
	}
	else if (direction == -1) {
		if (fov >= 1.0f && fov <= 70.0f)
			fov += 1;
		if (fov >= 70.0f)
			fov = 70.0f;
	}
}

GLvoid Reshape(int w, int h) {
	window_w = w;
	window_h = h;
	glViewport(0, 0, w, h);
}

void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}

void ReadObj(FILE* path, int index) {
	data[index].clear();
	char bind[128];
	memset(bind, 0, sizeof(bind));
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> faces;
	std::vector<glm::vec3> faces_normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> uvData;
	std::vector<glm::vec3> normals;

	while (!feof(path)) {
		fscanf(path, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			glm::vec3 vertex;
			fscanf(path, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			unsigned int temp_face[3], temp_uv[3], temp_normal[3];
			fscanf(path, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&temp_face[0], &temp_uv[0], &temp_normal[0],
				&temp_face[1], &temp_uv[1], &temp_normal[1],
				&temp_face[2], &temp_uv[2], &temp_normal[2]);

			faces.emplace_back(temp_face[0] - 1, temp_face[1] - 1, temp_face[2] - 1);
			uvData.emplace_back(temp_uv[0] - 1, temp_uv[1] - 1, temp_uv[2] - 1);
			faces_normals.emplace_back(temp_normal[0] - 1, temp_normal[1] - 1, temp_normal[2] - 1);
		}
		else if (bind[0] == 'v' && bind[1] == 't' && bind[2] == '\0') {
			glm::vec2 uv;
			fscanf(path, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (bind[0] == 'v' && bind[1] == 'n' && bind[2] == '\0') {
			glm::vec3 normal;
			fscanf(path, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
	}
	for (int i = 0; i < faces.size(); ++i) {
		data[index].push_back(vertices[faces[i].x].x);
		data[index].push_back(vertices[faces[i].x].y);
		data[index].push_back(vertices[faces[i].x].z);
		data[index].push_back(normals[faces_normals[i].x].x);
		data[index].push_back(normals[faces_normals[i].x].y);
		data[index].push_back(normals[faces_normals[i].x].z);

		data[index].push_back(vertices[faces[i].y].x);
		data[index].push_back(vertices[faces[i].y].y);
		data[index].push_back(vertices[faces[i].y].z);
		data[index].push_back(normals[faces_normals[i].y].x);
		data[index].push_back(normals[faces_normals[i].y].y);
		data[index].push_back(normals[faces_normals[i].y].z);

		data[index].push_back(vertices[faces[i].z].x);
		data[index].push_back(vertices[faces[i].z].y);
		data[index].push_back(vertices[faces[i].z].z);
		data[index].push_back(normals[faces_normals[i].z].x);
		data[index].push_back(normals[faces_normals[i].z].y);
		data[index].push_back(normals[faces_normals[i].z].z);
	}
}