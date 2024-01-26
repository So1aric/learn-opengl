#include <stdio.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define PHYSAC_IMPLEMENTATION
#define PHYSAC_STANDALONE
#include "physac.h"
#undef PHYSAC_IMPLEMENTATION

#include "src/shaders/common_vert_src.h"
#include "src/shaders/common_frag_src.h"

#define TITLE "devfloat"
#define WIDTH  800
#define HEIGHT 800
float window_width = WIDTH;
float window_height = HEIGHT;

float mouse_pos[2] = { 0.5, 0.5 };

float vertices[] = {
   //           position,
     1.0, -1.0, 1.0, 0.0, // right bottom
     1.0,  1.0, 1.0, 1.0, // right top
    -1.0,  1.0, 0.0, 1.0, // left  top
    -1.0, -1.0, 0.0, 0.0, // left  bottom
};

unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3,
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
    window_width = width;
    window_height = height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ENTER:
        glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_DOWN:
        break;
    }
}

void cursor_pos_callback(GLFWwindow *window, double x, double y) {
    mouse_pos[0] = x / window_width;
    mouse_pos[1] = 1 - y / window_height;
}

GLFWwindow *window;
GLuint vao, vbo, ebo, texture, shad_prog;
void init_opengl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(window_width, window_height, TITLE, NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, window_width, window_height);

    glClearColor(0.8f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    shad_prog = create_shader_program(common_vert_src, common_frag_src);

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
}

int main() {
    init_opengl();
    check_error();

    InitPhysics();
    SetPhysicsGravity(0, -5);
    PhysicsBody floor = CreatePhysicsBodyRectangle((Vector2){ 0, -window_height/2 }, window_width*2, 10, 10);
    floor->enabled = false;
    SetPhysicsBodyRotation(floor, 0.1);

    CreatePhysicsBodyRectangle((Vector2){ 0, 100 }, 300, 200, 10);
    CreatePhysicsBodyRectangle((Vector2){ 0, 500 }, 300, 200, 10);
    CreatePhysicsBodyRectangle((Vector2){ 200, 900 }, 300, 200, 10);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shad_prog);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        int body_count = GetPhysicsBodiesCount();
        for (int bi = 0; bi < body_count; bi++) {
            PhysicsBody body = GetPhysicsBody(bi);
            int vertex_count = GetPhysicsShapeVerticesCount(bi);
            // counterlock-wise, start from right bottom
            for (int vi = 0; vi < vertex_count; vi++) {
                Vector2 vertex = GetPhysicsShapeVertex(body, vi);
                vertices[vi*4]     = vertex.x / window_width;
                vertices[vi*4 + 1] = vertex.y / window_height;
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}
