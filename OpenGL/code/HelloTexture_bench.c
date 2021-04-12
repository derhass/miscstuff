#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
void cs(GLuint p, GLenum type, const char *src)
{
	GLuint sh=glCreateShader(type);
	glShaderSource(sh, 1, (const GLchar**)&src, NULL);
	glCompileShader(sh);
	glAttachShader(p, sh);
}
int main (int argc, char **argv)
{
	//const GLubyte img[2][2][3]={{{255, 0, 0}, {0, 255, 0}},{{0, 0, 255}, {255, 255, 255}}};
	const size_t w = 1024;
	const int query_count = 4;
	int cur=0;
	size_t i;
	GLubyte *img = (GLubyte*)malloc(w*w*4);
	GLuint query[query_count][2];
	srand(1);
	for (i=0; i<w*w*4; i++) {
		unsigned r = rand();
		img[i] = (GLubyte)r;

	}
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
	cs(p,GL_VERTEX_SHADER,"#version 150 core\nout vec2 tc;void main(){tc=vec2( (gl_VertexID & 1)<<1, (gl_VertexID & 2)); gl_Position=vec4(tc*2.0-1.0,0,1);}");
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, w, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

	glGenQueries(2*query_count, &query[0][0]);

	int avail = 0;
	GLuint64 draw_time = 0.0;

	double timeStart = glfwGetTime();
	unsigned int frame = 0;

	while (!glfwWindowShouldClose(win)) {
		GLuint64 gpuA, gpuB;
		glClear(GL_COLOR_BUFFER_BIT);
		glQueryCounter(query[cur][0], GL_TIMESTAMP);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glQueryCounter(query[cur][1], GL_TIMESTAMP);
		glfwSwapBuffers(win);
		glfwPollEvents();
		if (++cur >= query_count) {
			cur = 0;
			avail = 1;
		}
		if (avail) {
			glGetQueryObjecti64v(query[cur][0], GL_QUERY_RESULT, &gpuA);
			glGetQueryObjecti64v(query[cur][1], GL_QUERY_RESULT, &gpuB);
			draw_time += (gpuB - gpuA);
		}
		frame++;
		double now = glfwGetTime();
		double elapsed = now - timeStart;
		if (elapsed >= 1.0) {
			timeStart = now;
			printf("FPS: %.1f, GPUdraw: %.3fms\n", (double)frame/elapsed,
					(double)draw_time / ((double)frame * 1000000.0));
			frame = 0;
			draw_time = 0;
		}
	}
	glfwTerminate();
	return 0;
}

