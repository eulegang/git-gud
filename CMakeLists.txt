cmake_minimum_required(VERSION 3.16)

project(git-gud
    VERSION 0.4.0
		DESCRIPTION "A git hooks manager"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIT2 libgit2)
pkg_check_modules(LUA lua)

configure_file(
    src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
    @ONLY
)


set(GIT_GUD_SOURCES
    src/main.c
    src/hooks/hooks.c
    src/util.c
    src/gitgud.c
    src/gversion.c
    src/ginstall.c
		src/mbytes.c

    src/hooks/post-checkout.c
    src/hooks/commit-msg.c

    src/commit/commit.c

    src/config/config.c
    src/config/internal.c
    src/config/on_post_checkout.c
    src/config/on_commit.c
)

add_executable(git-gud ${GIT_GUD_SOURCES})

target_include_directories(git-gud PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(git-gud PRIVATE ${GIT2_LDFLAGS})
target_compile_options(git-gud PRIVATE ${GIT2_CFLAGS})
target_link_libraries(git-gud PRIVATE ${LUA_LDFLAGS})
target_compile_options(git-gud PRIVATE ${LUA_CFLAGS})

find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if(CLANG_TIDY_EXECUTABLE)
    add_custom_target(lint
        COMMAND ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR} ${GIT_GUD_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-tidy"
        VERBATIM
    )
else()
    message(STATUS "clang-tidy not found; lint target unavailable")
endif()

include(GNUInstallDirs)
install(TARGETS git-gud RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES man/git-gud.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES lua/git-gud.lua DESTINATION ${CMAKE_INSTALL_DATADIR}/git-gud)

include(CTest)

if(BUILD_TESTING)
    add_library(munit STATIC third_party/munit/munit.c)
    target_include_directories(munit SYSTEM PUBLIC third_party/munit)

    add_executable(git-gud-unit-tests tests/detect.c src/hooks/hooks.c)
    target_include_directories(git-gud-unit-tests PRIVATE src)
    target_link_libraries(git-gud-unit-tests PRIVATE munit)
    add_test(NAME unit.detect COMMAND git-gud-unit-tests)

    add_executable(git-gud-commit-tests tests/commit.c src/commit/commit.c)
    target_include_directories(git-gud-commit-tests PRIVATE src/commit)
    target_link_libraries(git-gud-commit-tests PRIVATE munit)
    add_test(NAME unit.commit COMMAND git-gud-commit-tests)

    add_executable(git-gud-syslog-tests tests/syslog.c src/util.c)
    target_include_directories(git-gud-syslog-tests PRIVATE src)
    target_link_libraries(git-gud-syslog-tests PRIVATE munit)
    add_test(NAME unit.syslog COMMAND git-gud-syslog-tests)

    find_program(BATS_EXECUTABLE NAMES bats bats-core)

    if(BATS_EXECUTABLE)
        file(GLOB BATS_TEST_FILES CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.bats
        )

        foreach(BATS_TEST_FILE IN LISTS BATS_TEST_FILES)
            get_filename_component(BATS_TEST_NAME ${BATS_TEST_FILE} NAME_WE)
            add_test(
                NAME bats.${BATS_TEST_NAME}
                COMMAND ${BATS_EXECUTABLE} ${BATS_TEST_FILE}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            set_tests_properties(bats.${BATS_TEST_NAME} PROPERTIES
                ENVIRONMENT "GIT_GUD_BINARY=$<TARGET_FILE:git-gud>"
            )
        endforeach()
    else()
        message(STATUS "bats not found; skipping bats tests")
    endif()
endif()
