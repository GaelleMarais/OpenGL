#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

#include "stl.h"

#define TINYPLY_IMPLEMENTATION
#include <tinyply.h>
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void error_callback(int /*error*/, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/* PARTICULES */
struct Particule {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 speed;
};

std::vector<Particule> MakeParticules(const int n)
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution01(0, 1);
	std::uniform_real_distribution<float> distributionWorld(-1, 1);

	std::vector<Particule> p;
	p.reserve(n);

	for (int i = 0; i < n; i++)
	{
		p.push_back(Particule{
				{
				distributionWorld(generator),
				distributionWorld(generator),
				distributionWorld(generator)
				},
				{
				distribution01(generator),
				distribution01(generator),
				distribution01(generator)
				},
				{0.f, 0.f, 0.f}
			});
	}

	return p;
}

GLuint MakeShader(GLuint t, std::string path)
{
	std::cout << path << std::endl;
	std::ifstream file(path.c_str(), std::ios::in);
	std::ostringstream contents;
	contents << file.rdbuf();
	file.close();

	const auto content = contents.str();
	std::cout << content << std::endl;

	const auto s = glCreateShader(t);

	GLint sizes[] = { (GLint)content.size() };
	const auto data = content.data();

	glShaderSource(s, 1, &data, sizes);
	glCompileShader(s);

	GLint success;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		GLsizei l;
		glGetShaderInfoLog(s, 512, &l, infoLog);

		std::cout << infoLog << std::endl;
	}

	return s;
}

GLuint AttachAndLink(std::vector<GLuint> shaders)
{
	const auto prg = glCreateProgram();
	for (const auto s : shaders)
	{
		glAttachShader(prg, s);
	}

	glLinkProgram(prg);

	GLint success;
	glGetProgramiv(prg, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		GLsizei l;
		glGetProgramInfoLog(prg, 512, &l, infoLog);

		std::cout << infoLog << std::endl;
	}

	return prg;
}

void APIENTRY opengl_error_callback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cout << message << std::endl;
}

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	int sizeWidth = 640;
	int sizeHeight = 480;
	window = glfwCreateWindow(sizeWidth, sizeHeight, "Simple example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// NOTE: OpenGL error checks have been omitted for brevity

	if (!gladLoadGL()) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(-1);
	}

	// Callbacks
	glDebugMessageCallback(opengl_error_callback, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);


	//charge triangle
	const auto triangle = ReadObj("frog.obj");
	// Shader
	const auto vertex = MakeShader(GL_VERTEX_SHADER, "shader.vert");
	const auto fragment = MakeShader(GL_FRAGMENT_SHADER, "shader.frag");

	const auto program = AttachAndLink({ vertex, fragment });

	glUseProgram(program);

	// Buffers
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(Triangle), triangle.data(), GL_STATIC_DRAW);

	// Bindings

	//position
	auto index = glGetAttribLocation(program, "position");
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2 + sizeof(glm::vec2), nullptr);
	glEnableVertexAttribArray(index);

	//normal
	index = glGetAttribLocation(program, "normal");
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2 + sizeof(glm::vec2), (void*) sizeof(glm::vec3));
	glEnableVertexAttribArray(index);

	//uv
	index = glGetAttribLocation(program, "uv");
	glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2 + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) * 2));
	glEnableVertexAttribArray(index);

	//texture
	int width, height, nbrchanel;
	unsigned char* img = stbi_load("frog.jpg", &width, &height, &nbrchanel, 0);

	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGB8, width, height);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, img);

	int uniformTransform = glGetUniformLocation(program, "transform");
	int uniformView = glGetUniformLocation(program, "view");
	int uniformProjection = glGetUniformLocation(program, "projection");
	int uniformTexture = glGetUniformLocation(program, "texture_");

	glBindTextureUnit(0, texture);
	glUniform1i(uniformTexture, 0);

	int count = 0;
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Allow rescale the window
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		count++;

		glm::mat4 transform = glm::mat4(1);
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)sizeWidth / (float)sizeHeight, 0.1f, 1000.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f));


		transform = glm::translate(transform, glm::vec3(-50, -50, -50));
		transform = glm::scale(transform, glm::vec3(10, 10, 10));
		transform = glm::rotate(transform, glm::radians(1.0f * count), glm::vec3(0, 0, 1));
		transform = glm::translate(transform, glm::vec3(0, 0, sin(count * 0.1f)));

		glUniformMatrix4fv(uniformTransform, 1, false, glm::value_ptr(transform));
		glUniformMatrix4fv(uniformView, 1, false, glm::value_ptr(view));
		glUniformMatrix4fv(uniformProjection, 1, false, glm::value_ptr(projection));

		glDrawArrays(GL_TRIANGLES, 0, triangle.size() * 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
