#include <stdio.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "src/shaders/common_vert_src.h"
#include "src/shaders/common_frag_src.h"

#define WIDTH 1000
#define HEIGHT 1000
#define TITLE "devfloat"

float vertices[] = {
   // position, tex_coord,          color,
    -0.9, -0.9,  0.0, 0.0,  0.7, 0.6, 0.5, // left  bottom
     0.9, -0.9,  1.0, 0.0,  0.9, 0.1, 0.2, // right bottom
    -0.9,  0.9,  0.0, 1.0,  0.7, 0.3, 0.7, // left  top
     0.9,  0.9,  1.0, 1.0,  1.0, 0.8, 0.2, // right top
};

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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, WIDTH, HEIGHT);

    glClearColor(0.8f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    GLint success; GLchar info[512];
    
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &common_vert_src, NULL);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert_shader, 512, NULL, info);
        puts(info);
    }
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &common_frag_src, NULL);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 512, NULL, info);
        puts(info);
    }

    GLuint shad_prog = glCreateProgram();
    glAttachShader(shad_prog, vert_shader);
    glAttachShader(shad_prog, frag_shader);
    glLinkProgram(shad_prog);

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
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    check_error();

    glUseProgram(shad_prog);
    glUniform1i(glGetUniformLocation(shad_prog, "texture0"), 0);

    while (!glfwWindowShouldClose(window)) {
        glUseProgram(shad_prog);
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
