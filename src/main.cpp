#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include "Renderer/include/Renderer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(900, 600, "Learn OpenGL", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create a window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to oinitialize GLAD\n";
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	std::array Hello_Triangle_Vertices = { // x,y,z,Tx,Ty,r,g,b
		-0.5f, -0.5f, 0.0f,        0.0f, 0.0f,            1.0f, 0.0f, 0.0f, // 0
		 0.5f, -0.5f, 0.0f,        1.0f, 0.0f,            0.0f, 1.0f, 0.0f, // 1
		 0.5f,  0.5f, 0.0f,        1.0f, 1.0f,            0.0f, 0.0f, 1.0f, // 2
		-0.5f,  0.5f, 0.0f,        0.0f, 1.0f,            0.0f, 1.0f, 0.0f // 3
	};

	std::array index_buffer = {
		0, 1, 2, 2, 3, 0
	};


	VertexArray v_arr;
	VertexBuffer v_buff;
	ElementBuffer e_buff;

	v_arr.bind();

	v_buff.bind(GL_ARRAY_BUFFER);
	v_buff.set_data(GL_ARRAY_BUFFER, Hello_Triangle_Vertices, GL_STATIC_DRAW);

	e_buff.bind();
	e_buff.set_data(GL_ELEMENT_ARRAY_BUFFER, index_buffer, GL_STATIC_DRAW);

	v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	v_arr.push_arrtib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 3 * sizeof(float));
	v_arr.push_arrtib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 5 * sizeof(float));

	v_arr.enable(0);
	v_arr.enable(1);
	v_arr.enable(2);


	std::string VertexShaderSource;
	std::string FragmentShaderSource;

	ShaderInput::ReadFromFile("dependencies/Shaders/vertexshader.shader", VertexShaderSource);
	ShaderInput::ReadFromFile("dependencies/Shaders/fragmentshader.shader", FragmentShaderSource);

	const char* v_src = VertexShaderSource.c_str();
	const char* f_src = FragmentShaderSource.c_str();

	ShaderVertex vertex_shader(&v_src);
	ShaderFragment fragment_shader(&f_src);

	ShaderProgram shader_program(vertex_shader, fragment_shader);

	VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
	VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

	shader_program.Bind();
	Texture texture("Assets/hellotexture.png");
	texture.Bind();
	shader_program.SetUniform1i("u_Texture", 0);

	while (!glfwWindowShouldClose(window)) {



		processInput(window);

		VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
		VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT));


		//VALIDATE_GL_(glUseProgram(shader_program.id()));
		shader_program.Bind();
		
		//VALIDATE_GL_(glBindVertexArray(v_arr.id()));
		v_arr.bind();

		VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}