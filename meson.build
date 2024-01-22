project('learn-opengl', ['c'])

cc = meson.get_compiler('c')

math = cc.find_library('m')
glfw = cc.find_library('glfw')
cglm = cc.find_library('cglm')
chipmunk = cc.find_library('chipmunk')

files = files()
subdir('src')

executable(
    'main',
    [
        'src/glad.c',
        'src/main.c',
        'src/shader.c',
    ],
    include_directories: 'include',
    dependencies: [
        math,
        glfw,
        cglm,
        chipmunk,
    ],
)
