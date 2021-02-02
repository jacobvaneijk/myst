project(
    'myst',
    ['c', 'cpp'],
    license: 'MIT',
    version: '0.0.1',
    default_options: [
        'warning_level=2',
        'cpp_std=c++17',
    ],
)

add_project_arguments(
    '-Wno-unused-variable',
    '-Wno-unused-parameter',
    '-Wno-maybo-uninitialized',
    language: ['c', 'cpp']
)

headers = include_directories(
    'src',
    'vendor/glad/include',
    'vendor/glm',
    'vendor/stb'
)

sources = files([
    'src/main.cpp',
    'src/GLShader.cpp',
    'src/GLTexture.cpp',
    'vendor/glad/src/glad.c'
])

glfw_dep = dependency(
    'glfw3',
    method: 'pkg-config'
)

executable(
    meson.project_name(),
    sources,
    link_args: ['-ldl'],
    include_directories: headers,
    dependencies: [glfw_dep]
)
