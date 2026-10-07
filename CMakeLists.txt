cmake_minimum_required(VERSION 3.16)

project(git-gud
    VERSION 0.1.0
    DESCRIPTION "A C project"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_executable(git-gud
    src/main.c
		src/detect.c
)

enable_testing()
