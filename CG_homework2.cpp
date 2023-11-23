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
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUp(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
char* filetobuf(const char* file);
void ReadObj(FILE* path, int index);
GLvoid Display();
void animation1_reset();

float floor_xz[36] = {
	 -100.0, 0.0, -100.0, 0.0, 1.0, 0.0,  -100.0, 0.0, 100.0, 0.0, 1.0, 0.0,  100.0, 0.0, 100.0, 0.0, 1.0, 0.0,
	 -100.0, 0.0, -100.0, 0.0, 1.0, 0.0,  100.0, 0.0, 100.0, 0.0, 1.0, 0.0,  100.0, 0.0, -100.0, 0.0, 1.0, 0.0
};

GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vao[401], vbo[401];
std::vector<GLfloat> data[400];
int window_w, window_h;
int height_size, width_size;
float cameraPos_x, cameraPos_y, cameraPos_z;
float cube_x[20][20], cube_y[20][20], cube_z[20][20];
float length_width, length_height;               // ����ü ������ ����
BOOL key_1, key_2, key_3, key_t, key_y, key_Y;
BOOL key_w, key_a, key_s, key_d;
int key_c;                                       // ���� ���õ� ���� ����
float light_r[3], light_g[3], light_b[3];        // ���� ����
int case_display;                                // ����
float radian_y;                                  // ī�޶� ����
BOOL move_check[20][20];                         // ��� ��, �Ʒ� �̵� Ȯ��   true ���� �̵� / false �Ʒ� �̵�
float animation1_speed[20][20];                  // �ִϸ��̼�1 ��� �̵��ӵ� 0.05 ~ 0.15
float speed;                                     // ��� �����̴� �ӵ�
float camera_move_x, camera_move_z;              // ī�޶� ������ ��ġ
float cameraDirection_x, cameraDirection_y;      // ī�޶� �ٶ󺸴� ����
BOOL left_button;                                

void menu() {
	std::cout << "-----------��ɾ�------------" << std::endl;
	std::cout << "1 : �ִϸ��̼� 1" << std::endl;
	std::cout << "2 : �ִϸ��̼� 2" << std::endl;
	std::cout << "3 : �ִϸ��̼� 3" << std::endl;
	std::cout << "t : ���� on / off" << std::endl;
	std::cout << "c : ���� �� ����" << std::endl;
	std::cout << "y/Y : ī�޶� ȸ��" << std::endl;
	std::cout << "+/- : ����ü �̵� �ӵ� ���� / ����" << std::endl;
	std::cout << "w/a/s/d : ī�޶� �̵�" << std::endl;
	std::cout << "r : ����" << std::endl;
	std::cout << "q : ���α׷� ����" << std::endl;
}

void animation1_reset() {
	for (int i = 0; i < height_size; ++i) {
		for (int j = 0; j < width_size; ++j) {
			cube_y[i][j] = rand() % 401 * 0.01;
			animation1_speed[i][j] = (rand() % 11 + 5) * 0.01;
			move_check[i][j] = true;
		}
	}
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
	cameraDirection_x = cameraDirection_y = 0;
	left_button = false;
	while (1) {
		std::cout << "���� ���� ũ�⸦ �Է��� �ּ���(�ּ� 5, �ִ� 20) : ";
		std::cin >> width_size;
		std::cin >> height_size;
		if (!(5 <= height_size && 20 >= height_size && 5 <= width_size && 20 >= width_size))
			std::cout << "������ ������ϴ�." << std::endl;
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
	//--- ������ �����ϱ�
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
	glutTimerFunc(100, timer, 1);
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
	int modelLoc = glGetUniformLocation(shaderProgramID, "model"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��� �������� �޾ƿ´�.
	int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	int projLoc = glGetUniformLocation(shaderProgramID, "projection");
	if (case_display == 0) {
		cameraPos_x = -10.0f;
		cameraPos_y = 15.0f;
		cameraPos_z = 10.0f;

		glm::vec3 cameraPos = glm::vec3(cameraPos_x, cameraPos_y, cameraPos_z);      //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(cameraDirection_x, cameraDirection_y, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);        //--- ī�޶� ���� ����

		glm::mat4 vTransform = glm::mat4(1.0f);
		glm::mat4 R_camera = glm::mat4(1.0f);
		glm::mat4 T_camera = glm::mat4(1.0f);
		R_camera = glm::rotate(R_camera, glm::radians(radian_y), glm::vec3(0.0, 1.0, 0.0));
		T_camera = glm::translate(T_camera, glm::vec3(camera_move_x, 0.0, camera_move_z));
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		vTransform = vTransform * T_camera * R_camera;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::perspective(glm::radians(50.0f), (float)window_w / (float)window_h, 0.1f, 200.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
	}
	else if (case_display == 1) {
		cameraPos_x = 0.0f;
		cameraPos_y = 3.0f;
		cameraPos_z = 0.0f;

		glm::vec3 cameraPos = glm::vec3(cameraPos_x, cameraPos_y, cameraPos_z);      //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);        //--- ī�޶� ���� ����

		glm::mat4 vTransform = glm::mat4(1.0f);
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::perspective(glm::radians(50.0f), (float)window_w / (float)window_h, 0.1f, 200.0f);
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

	// �ٴ� ����
	glUniform3f(objColorLocation, 0.1, 0.1, 0.3);
	glm::mat4 floor = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(floor));
	glBindVertexArray(vao[height_size * width_size]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	if(key_t)
		glUniform3f(lightPosLocation, 0.0, 10.0, 0.0);
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
	if(key_1) {
		for (int i = 0; i < height_size; ++i) {
			for (int j = 0; j < width_size; ++j) {
				if(move_check[i][j] == true) {
					cube_y[i][j] += animation1_speed[i][j] * speed;
					if (cube_y[i][j] >= 5.0)
						move_check[i][j] = false;
				}
				else {
					cube_y[i][j] -= animation1_speed[i][j] * speed;
					if (cube_y[i][j] <= 0.0)
						move_check[i][j] = true;
				}
			}
		}
	}
	else if(key_2) {}
	else if(key_3) {}
	if(key_w) {
		camera_move_z += 1;
	}
	if(key_a) {
		camera_move_x += 1;
	}
	if(key_s) {
		camera_move_z -= 1;
	}
	if(key_d) {
		camera_move_x -= 1;
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
		break;
	case '3':
		key_3 = true;
		key_2 = key_1 = false;
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
		if (speed < 2)
			speed += 0.2;
		break;
	case '-':
		if (speed > 0.5)
			speed -= 0.2;
		break;
	case 'r':   // ����
		reset();
		InitBuffer();
		break;
	case 'q':   // ���α׷� ����
		std::cout << "���α׷��� �����մϴ�." << std::endl;
		glutLeaveMainLoop();
		break;
	}
	Display();
}

GLvoid KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		key_w = false;
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
		prev_mouse_x = normalized_x;  // ����� ���� ���콺 ��ġ
		prev_mouse_y = normalized_y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_button = false;
	}
}

void Motion(int x, int y) {
	float normalized_x, normalized_y;

	normalized_x = (2.0 * x / 800) - 1.0;
	normalized_y = 1.0 - (2.0 * y / 600);
	if (left_button == true) {
		float delta_x = normalized_x - prev_mouse_x;
		float delta_y = normalized_y - prev_mouse_y;

		cameraDirection_x += delta_x * 5;
		cameraDirection_y += delta_y * 5;

		prev_mouse_x = normalized_x;
		prev_mouse_y = normalized_y;
	}
	Display();
}

GLvoid Reshape(int w, int h) {
	window_w = w;
	window_h = h;
	glViewport(0, 0, w, h);
}

void make_shaderProgram()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
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