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
	const GLubyte img[2][2][3]={{{255, 0, 0}, {0, 255, 0}},{{0, 0, 255}, {255, 255, 255}}};
	GLFWwindow *win;
	GLuint p, vao, tex;
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if ( !(win=glfwCreateWindow( 800, 600, "Hello, Texture", NULL, NULL)) ) {
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
	cs(p,GL_VERTEX_SHADER,"#version 150 core\nout vec2 tc;void main(){tc=vec2( (gl_VertexID & 2)>>1, 1 - (gl_VertexID & 1)); gl_Position=vec4(tc*2.0-1.0,0,1);}");
	cs(p,GL_FRAGMENT_SHADER,"#version 150 core\nuniform sampler2D tex;in vec2 tc;out vec4 color; void main() {color=texture(tex,tc);}");
	glLinkProgram(p);
	glUseProgram(p);
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	while (!glfwWindowShouldClose(win)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

