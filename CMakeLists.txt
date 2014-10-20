cmake_minimum_required(VERSION 2.6)

project(grcv)

get_filename_component(GRCV_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)

link_directories($ENV{BUDDY_HOME}/lib)

add_subdirectory(src)

add_subdirectory(test)
add_subdirectory(example)

enable_testing()

add_test(grcv_test_run test/grcv_test)
