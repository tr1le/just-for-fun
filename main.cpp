//internal includes
#include "common.h"
#include "ShaderProgram.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const GLsizei WIDTH = 640, HEIGHT = 480; //размеры окна

int initGL()
{
	int res = 0;
	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: "   << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: "  << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: "     << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	return 0;
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

  	GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if(initGL() != 0) 
		return -1;
	
  	//Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	//используется класс-обертка ShaderProgram
	std::unordered_map<GLenum, std::string> shaders;
	shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
	shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
	ShaderProgram program(shaders); GL_CHECK_ERRORS;

	glfwSwapInterval(1); // force 60 frames per second
	
	//Создаем и загружаем геометрию поверхности
	//
	GLuint g_vertexArrayObject;
	{
		float vertices[] =
		{
		-1, 0, 0,
		0, -1, 0,
		0, 0, -1,

		-1, 0, 0,
		0, -1, 0,
		0, 0, 1,

		-1, 0, 0,
		0, 1, 0,
		0, 0, -1,

		-1, 0, 0,
		0, 1, 0,
		0, 0, 1,

		1, 0, 0,
		0, -1, 0,
		0, 0, -1,

		1, 0, 0,
		0, -1, 0,
		0, 0, 1,

		1, 0, 0,
		0, 1, 0,
		0, 0, -1,

		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		};
		
		unsigned int indices[] =
		{
			0, 1, 2,
			3, 4, 5,
			6, 7, 8,
			9, 10, 11,
			12, 13, 14,
			15, 16, 17,
			18, 19, 20,
			21, 22, 23,
		};

		GLuint g_vertexBufferObject;
		GLuint g_elementBufferObject;

		g_vertexBufferObject = 0;
		GLuint vertexLocation = 0; // simple layout, assume have only positions at location = 0

		glGenVertexArrays(1, &g_vertexArrayObject);
		glGenBuffers(1, &g_vertexBufferObject);
		glGenBuffers(1, &g_elementBufferObject);

		glBindVertexArray(g_vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 8 * 3 * 3 * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8 * 3 * sizeof(indices[0]), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	
    glm::mat4 view;
	view = glm::mat4(1.0f);
	// view = glm::rotate(view, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));


    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 8.0f / 6.0f, 0.1f, 100.0f);


    program.StartUseShader();
    int viewLoc = glGetUniformLocation(program.GetProgram(), "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projectionLoc = glGetUniformLocation(program.GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	//цикл обработки сообщений и отрисовки сцены каждый кадр
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//очищаем экран каждый кадр
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);               GL_CHECK_ERRORS;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

		program.StartUseShader();                           GL_CHECK_ERRORS;


		// очистка и заполнение экрана цветом
		//
		glViewport  (0, 0, WIDTH, HEIGHT);
		glClearColor(0.0f, 0.5f, 0.5f, 0.0f);
		glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// draw call
		//
		float time = glfwGetTime();

		{
			float off = 0.0f;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(off + sin(time * 2 + off) / 2, 1.5f - time / 4 + off, 2.0f + cos(time * 2 + off) / 2));
			float scale = 0.3f;
			model = glm::scale(model, glm::vec3(scale, scale, scale));
			model = glm::rotate(model, glm::radians(-time * 100 + off), glm::vec3(0.0f, 1.0f, 0.0f));

			int modelLoc = glGetUniformLocation(program.GetProgram(), "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		}

		glBindVertexArray(g_vertexArrayObject); GL_CHECK_ERRORS;
		glDrawArrays(GL_TRIANGLES, 0, 3);       GL_CHECK_ERRORS;  // The last parameter of glDrawArrays is equal to VS invocations
		glDrawElements(GL_TRIANGLES, 8 * 3 * sizeof(unsigned int), GL_UNSIGNED_INT, 0); GL_CHECK_ERRORS;

		{
			float off = -0.9f;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(off + sin(time * 2 + off) / 2, 1.5f - time / 4 + off, 0.0f + cos(time * 2 + off) / 2));
			float scale = 0.3f;
			model = glm::scale(model, glm::vec3(scale, scale, scale));
			model = glm::rotate(model, glm::radians(time * 100 + off), glm::vec3(0.0f, 1.0f, 0.0f));

			int modelLoc = glGetUniformLocation(program.GetProgram(), "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		}

		glBindVertexArray(g_vertexArrayObject); GL_CHECK_ERRORS;
		glDrawArrays(GL_TRIANGLES, 0, 3);       GL_CHECK_ERRORS;  // The last parameter of glDrawArrays is equal to VS invocations
		glDrawElements(GL_TRIANGLES, 8 * 3 * sizeof(unsigned int), GL_UNSIGNED_INT, 0); GL_CHECK_ERRORS;

		{
			float off = 0.5f;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(off * 2 + sin(time * 2 + off + 2) / 2, 1.5f - time / 4 + off, 0.0f + cos(time * 2 + off + 2) / 2));
			float scale = 0.3f;
			model = glm::scale(model, glm::vec3(scale, scale, scale));
			model = glm::rotate(model, glm::radians(time * 100 + off), glm::vec3(0.0f, 1.0f, 0.0f));

			int modelLoc = glGetUniformLocation(program.GetProgram(), "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		}

		glBindVertexArray(g_vertexArrayObject); GL_CHECK_ERRORS;
		glDrawArrays(GL_TRIANGLES, 0, 3);       GL_CHECK_ERRORS;  // The last parameter of glDrawArrays is equal to VS invocations
		glDrawElements(GL_TRIANGLES, 8 * 3 * sizeof(unsigned int), GL_UNSIGNED_INT, 0); GL_CHECK_ERRORS;

		program.StopUseShader();

		glfwSwapBuffers(window); 
	}

	//очищаем vboи vao перед закрытием программы
	glDeleteVertexArrays(1, &g_vertexArrayObject);

	glfwTerminate();
	return 0;
}
