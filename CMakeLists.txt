cmake_minimum_required(VERSION 3.20)
project(cat VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_REQIF_CPP_TOOL "Build the ReqIF to C++ converter / tool" ON)

# Output directories
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Example external deps
find_package(pugixml CONFIG REQUIRED)
find_package(libzippp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

if(BUILD_REQIF_CPP_TOOL)
	#find_package(boost REQUIRED)
	find_package(nlohmann_json CONFIG REQUIRED)
	  find_package(cxxopts CONFIG REQUIRED)
  
	add_subdirectory(reqif_tool)

	#add_executable(reqif_tool "reqif_tool/main.cpp")
	
endif()


add_subdirectory(cat)
add_subdirectory(tests)

