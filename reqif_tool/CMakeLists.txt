
add_executable(reqif_tool main.cpp)

target_link_libraries(reqif_tool PRIVATE nlohmann_json::nlohmann_json cxxopts::cxxopts pugixml::pugixml)
