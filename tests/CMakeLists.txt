

add_executable(tests main.cpp )

target_link_libraries(tests PRIVATE cat::cat fmt::fmt)