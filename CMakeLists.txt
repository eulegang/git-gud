cmake_minimum_required(VERSION 3.16)

project(git-gud
    VERSION 0.1.0
    DESCRIPTION "A C project"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIT2 libgit2)

configure_file(
    src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
    @ONLY
)


add_executable(git-gud
    src/main.c
    src/detect.c
		src/gitgud.c
		src/gversion.c
		src/ginstall.c
)

target_include_directories(git-gud PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(git-gud PRIVATE ${GIT2_LDFLAGS})
target_compile_options(git-gud PRIVATE ${GIT2_CFLAGS})

enable_testing()
