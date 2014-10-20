add_executable(grcv_test grcv_test.cpp)

target_link_libraries(grcv_test grcv)
include_directories(${GRCV_DIRECTORY}/src)
link_directories(${GRCV_DIRECTORY}/src)

include_directories($ENV{BOOST_HOME}/include
                    $ENV{BUDDY_HOME}/include)

