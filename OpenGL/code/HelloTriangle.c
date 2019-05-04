#include "glew.h"
#include <GLFW/glfw3.h>
void cs(GLuint p, GLenum type, const char *src)
{
	GLuint sh=glCreateShader(type);
	glShaderSource(sh, 1, (const GLchar**)&src, NULL);
	glCompileShader(sh);
	glAttachShader(p, sh);
}
int main (int argc, char **argv)
{
	GLFWwindow *win;
	GLuint p, vao;
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if ( !(win=glfwCreateWindow( 800, 600, "Hello, Triangle", NULL, NULL)) ) {
		glfwTerminate();
		return 2;
	}
	glfwMakeContextCurrent(win);
	glewExperimental=GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return 2;
	}
	p=glCreateProgram();
	cs(p,GL_VERTEX_SHADER,"#version 150 core\nvoid main(){gl_Position=vec4(gl_VertexID&1,(gl_VertexID&2)>>1,0,1);}");
	cs(p,GL_FRAGMENT_SHADER,"#version 150 core\nout vec4 color; void main() {color=vec4(1);}");
	glLinkProgram(p);
	glUseProgram(p);
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	while (!glfwWindowShouldClose(win)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
