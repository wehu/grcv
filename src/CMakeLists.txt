add_library(grcv SHARED
  grcv_base.cpp
  grcv_context.cpp
  grcv_logger.cpp
  grcv_randgen.cpp
  grcv_randv.cpp
  grcv_range.cpp)

include_directories($ENV{BOOST_HOME}/include
                    $ENV{BUDDY_HOME}/include)

link_directories($ENV{BUDDY_HOME}/lib)

target_link_libraries(grcv
  bdd)

install(TARGETS grcv
  EXPORT grcv
  LIBRARY DESTINATION lib
)

install(FILES
  grcv.h
  grcv_const.h
  grcv_logger.h
  grcv_randv.h
  grcv_base.h
  grcv_context.h
  grcv_randgen.h
  grcv_trait.h
  grcv_range.h
  DESTINATION include)

