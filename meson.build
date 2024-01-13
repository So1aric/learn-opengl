project('learn-opengl', ['c'])

cc = meson.get_compiler('c')

math = cc.find_library('m')
glfw = cc.find_library('glfw')

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
    ]
)
