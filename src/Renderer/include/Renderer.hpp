#pragma once
#include <iostream>
#include <cstddef>
#include <array>
#include <cassert>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void clear_gl_errors() {
	while (glGetError() != GL_NO_ERROR) {}
}

void log_gl_errors(const char* expression, std::size_t line, const char* file_name) {

	int error_code = glGetError();
	bool loop_ran = false;

	while (error_code != GL_NO_ERROR) {
		std::cout << "OpenGL Error on: " << expression
			<< "\nBreakpoint trigger:"
			<< "\nAt line: " << line
			<< "\nFile: " << file_name
			<< "\nError Code: " << error_code;
		error_code = glGetError();
		loop_ran = true;

	}

	if (loop_ran) __debugbreak(); // some opengl error occured // check call stack and terminal
}

#define VALIDATE_GL_(expr_gl) do {                             \
clear_gl_errors();								                             \
expr_gl;                                                       \
log_gl_errors(#expr_gl, __LINE__, __FILE__);                   \
} while(0)

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

unsigned int CreateShader(unsigned int shader_type_gl, char const* const* const shader_src_string_array, unsigned int src_count) {
	unsigned int shader;
	shader = glCreateShader(shader_type_gl);

	VALIDATE_GL_(glShaderSource(shader, src_count, shader_src_string_array, nullptr));
	VALIDATE_GL_(glCompileShader(shader));

	int success;
	char infolog[512];
	VALIDATE_GL_(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));

	if (!success) {
		VALIDATE_GL_(glGetShaderInfoLog(shader, 512, nullptr, infolog));
		std::cout << "Error compiling shader: \n" << infolog << std::endl;
	}

	return shader;
}

unsigned int CreateShaderProgram(unsigned int VertexShader, unsigned int FragmentShader) {
	unsigned int ShaderProgram;
	ShaderProgram = glCreateProgram();

	VALIDATE_GL_(glAttachShader(ShaderProgram, VertexShader));
	VALIDATE_GL_(glAttachShader(ShaderProgram, FragmentShader));
	VALIDATE_GL_(glLinkProgram(ShaderProgram));

	int success;
	char infolog[512];

	VALIDATE_GL_(glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success));
	if (!success) {
		VALIDATE_GL_(glGetProgramInfoLog(ShaderProgram, 512, nullptr, infolog));
		std::cout << "Error linking : \n" << infolog << std::endl;
	}
	return ShaderProgram;
}




class VertexArray {
private:
	unsigned int VAO;
public:
	
	VertexArray() {
		VALIDATE_GL_(glGenVertexArrays(1, &VAO));
	}
	
	void bind() const {
		VALIDATE_GL_(glBindVertexArray(VAO));
	}

	void push_arrtib_pointer(std::size_t index, std::size_t element_count_per_vertex_arrtib, unsigned int type_type_GLenum,
		int normalise, std::size_t stride_count, std::size_t vertex_element_offset_size_bytes) {
		VALIDATE_GL_(glVertexAttribPointer(

			index, element_count_per_vertex_arrtib, type_type_GLenum, normalise, stride_count, reinterpret_cast<void*>(vertex_element_offset_size_bytes)

		));

	}
	
	void enable(std::size_t index) {
		VALIDATE_GL_(glEnableVertexAttribArray(index));
	}

	unsigned int id() const{
		return VAO;
	}

};

class VertexBuffer {
private:
	unsigned int VBO;
public:
	
	VertexBuffer() {
		VALIDATE_GL_(glGenBuffers(1, &VBO));
	}

	void bind(unsigned int target_type_GLenum) {
		VALIDATE_GL_(glBindBuffer(target_type_GLenum, VBO));
	}
	
	template <typename T, std::size_t buffer_size>
	void set_data(unsigned int target_type_GLenum, const std::array<T, buffer_size>& buffer, unsigned int usage_type_GLenum) {
		VALIDATE_GL_(glBufferData(target_type_GLenum, sizeof(T) * buffer_size, buffer.data(), usage_type_GLenum));
	}

	unsigned int id() const{
		return VBO;
	}
};

class ShaderVertex {

private:
	unsigned int _id;

public:
	ShaderVertex() : _id(0) {
	}
	
	ShaderVertex(const char* const* const shader_src_string_array, std::size_t str_array_size = 1) {
		_id = CreateShader(GL_VERTEX_SHADER, shader_src_string_array, str_array_size);
	}

	void CompileShader_seperately(const char* const* const shader_src_string_array, std::size_t str_array_size) {
		assert(_id == 0 && "id already has a compiled shader");
		_id = CreateShader(GL_VERTEX_SHADER, shader_src_string_array, str_array_size);
	}

	unsigned int id() const{
		return _id;
	}
};

class ShaderFragment {

private:
	unsigned int _id;

public:
	ShaderFragment() : _id(0) {
	}

	ShaderFragment(const char* const* const shader_src_string_array, std::size_t str_array_size = 1) {
		_id = CreateShader(GL_FRAGMENT_SHADER, shader_src_string_array, str_array_size);
	}

	void CompileShader_seperately(const char* const* const shader_src_string_array, std::size_t str_array_size) {
		assert(_id == 0 && "id already has a compiled shader");
		_id = CreateShader(GL_FRAGMENT_SHADER, shader_src_string_array, str_array_size);
	}

	unsigned int id() const{
		return _id;
	}
};

class ElementBuffer {
private:
	unsigned int EBO;

public:
	ElementBuffer() {
		VALIDATE_GL_(glGenBuffers(1, &EBO));
	}

	template <typename T, std::size_t buffer_size>
	void set_data(unsigned int target_type_GLenum, const std::array<T, buffer_size>& buffer, unsigned int usage_type_GLenum) {
		VALIDATE_GL_(glBufferData(target_type_GLenum, sizeof(T) * buffer_size, buffer.data(), usage_type_GLenum));
	}
	
	void bind() {
		VALIDATE_GL_(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	}

	unsigned int id() const {
		return EBO;
	}
}; 

class ShaderProgram {
private:
	unsigned int _id;
public:

	ShaderProgram() : _id(0) {

	}

	ShaderProgram(ShaderVertex VertexShader, ShaderFragment FragmentShader) {
		_id = CreateShaderProgram(VertexShader.id(), FragmentShader.id());
	}

	void CreateProgram_seperately(ShaderVertex VertexShader, ShaderFragment FragmentShader) {
		_id = CreateShaderProgram(VertexShader.id(), FragmentShader.id());
	}

	unsigned int id() {
		return _id;
	}
};

namespace ShaderInput {

	bool ReadFromFile(const std::string& path, std::string& destination_string) {
		std::ifstream shader_file(path);

		if (!shader_file) {
			std::cout << "Error Opening file: " << path << std::endl;
			return false;
		}

		std::string line;

		while (std::getline(shader_file, line)) {
			destination_string += line + '\n';
		}

		return true;
	}

}