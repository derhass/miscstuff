// just a quick hack, based on  https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9
// compile g++ gradient_es2.cpp -lSDL2 -lGL

#include <exception>
#include <functional>

#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

// Shader sources
const GLchar* vertexSource =
    "#version 100\n"
    "precision mediump float;\n"
    "attribute vec4 position;    \n"
    "varying vec4 pos;\n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = vec4(position.xyz, 1.0);  \n"
    "   pos = 0.5*position+vec4(0.5);  \n"
    "}                            \n";
const GLchar* fragmentSource =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec4 pos;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4 (pos.xxx, 1.0 );\n"
    "}                                            \n";


std::function<void()> loop;
void main_loop() { loop(); }

int main(int argc, char** argv)
{
    // SDL_Init(SDL_INIT_VIDEO);

    auto wnd(
        SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    auto glc = SDL_GL_CreateContext(wnd);

    SDL_GL_MakeCurrent(wnd, glc);

    printf("%s\n%s\n%s\n", glGetString(GL_RENDERER),glGetString(GL_VENDOR),glGetString(GL_VERSION));

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {-1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f};

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    loop = [&]
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) std::terminate();
        }


        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle from the 3 vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(wnd);
    };

    while(true) main_loop();

    return 0;
}
