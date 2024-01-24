#include <stdio.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include "chipmunk/chipmunk.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "src/shaders/common_vert_src.h"
#include "src/shaders/common_frag_src.h"

#define WIDTH  800
#define HEIGHT 800
#define TITLE "devfloat"

float mouse_pos[2] = { 0.5, 0.5 };

typedef struct Rect {
    vec2 pos; float rot;
} Rect;

Rect koharu = {
    .pos = { 0.0, 0.5 },
    .rot = 0.0,
};

float vertices[] = {
   //           position,
    -1.0, -1.0, 0.0, 0.0, // left  bottom
     1.0, -1.0, 1.0, 0.0, // right bottom
    -1.0,  1.0, 0.0, 1.0, // left  top
     1.0,  1.0, 1.0, 1.0, // right top
};
GLuint vbo;

unsigned int indices[] = {
    0, 1, 2,
    1, 2, 3
};

static void check_error_impl(unsigned int line_number) {
    GLenum err = glGetError();
    switch (err) {
    case GL_NO_ERROR:
        break;
    default:
        printf("Error<%d>: %d", line_number, err);
    }
}

#define check_error() check_error_impl(__LINE__)

static GLuint compile_shader(const char *src, const int shader_type) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success; GLchar info[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info);
        printf("Compile error: %s\n", info);
        return -1;
    }

    return shader;
}

static GLuint create_shader_program(const char *vert_src, const char *frag_src) {
    GLuint vert_shader = compile_shader(vert_src, GL_VERTEX_SHADER);
    GLuint frag_shader = compile_shader(frag_src, GL_FRAGMENT_SHADER);

    GLuint shad_prog = glCreateProgram();
    glAttachShader(shad_prog, vert_shader);
    glAttachShader(shad_prog, frag_shader);
    glLinkProgram(shad_prog);

    return shad_prog;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ENTER:
        glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_DOWN:
        for (int i = 0; i < 4; i++) {
            vertices[i*4 + 1] -= 0.01;
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void cursor_pos_callback(GLFWwindow *window, double x, double y) {
    mouse_pos[0] = x / WIDTH;
    mouse_pos[1] = 1 - y / HEIGHT;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, WIDTH, HEIGHT);

    glClearColor(0.8f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    GLuint shad_prog = create_shader_program(common_vert_src, common_frag_src);

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nchannel;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("assets/img.jpg", &width, &height, &nchannel, 0);
    if (!data) puts("Failed loading picture.");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glUseProgram(shad_prog);
    glUniform1i(glGetUniformLocation(shad_prog, "texture0"), 0);

    check_error();
    while (!glfwWindowShouldClose(window)) {
        glUseProgram(shad_prog);

        glUniform2fv(glGetUniformLocation(shad_prog, "mouse_pos"), 1, mouse_pos);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}
