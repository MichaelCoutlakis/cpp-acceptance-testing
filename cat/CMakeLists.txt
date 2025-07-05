add_library(cat cat.h cat.cpp  "assert_macros.h"  "console_reporter.h" "console_reporter.cpp")
add_library(cat::cat ALIAS cat)
target_include_directories(cat PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
target_link_libraries(cat PUBLIC fmt::fmt)