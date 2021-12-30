#include "GL/glew.h"
#include <GLFW/glfw3.h>
const int FBO_WIDTH = 256, FBO_HEIGHT = 256;
static void cs(GLuint p, GLenum type, const char *src)
{
	GLuint sh=glCreateShader(type);
	glShaderSource(sh, 1, (const GLchar**)&src, NULL);
	glCompileShader(sh);
	glAttachShader(p, sh);
}
int main (int argc, char **argv)
{
	GLFWwindow *win;
	GLuint vs,p, vao, tex, fbo, lUseTex;
	if (!glfwInit()) return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if ( !(win=glfwCreateWindow( 800, 600, "Hello, Texture", NULL, NULL)) ) return 2;
	glfwMakeContextCurrent(win);
	glewExperimental=GL_TRUE;
	if (glewInit() != GLEW_OK) return 3;
	p=glCreateProgram();
	cs(p,GL_VERTEX_SHADER,"#version 150 core\nout vec2 tc;void main(){tc=vec2( (gl_VertexID & 1)<<1, (gl_VertexID & 2)); gl_Position=vec4(tc*2.0-1.0,0,1);}");
	cs(p,GL_FRAGMENT_SHADER,"#version 150 core\nuniform sampler2D tex;uniform int useTex; in vec2 tc;out vec4 color; void main() {if (useTex>0)color=texture(tex,vec2(tc.x,tc.y+0.01*sin(gl_FragCoord.x*0.1)));else color=vec4(tc,0.0f,1.0f);}");
	glLinkProgram(p);
	glUseProgram(p);
	lUseTex=glGetUniformLocation(p,"useTex");
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
	while (!glfwWindowShouldClose(win)) {
		int w,h;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glViewport(0,0,FBO_WIDTH,FBO_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1i(lUseTex,0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glfwGetFramebufferSize(win,&w,&h);
		glViewport(0,0,w,h);
		glUniform1i(lUseTex,1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	return 0;
}
