cmake_minimum_required(VERSION 3.16)

project(git-gud
    VERSION 0.1.0
    DESCRIPTION "A C project"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

configure_file(
    src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
    @ONLY
)

add_executable(git-gud
    src/main.c
    src/detect.c
		src/gitgud.c
)

target_include_directories(git-gud PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

enable_testing()
