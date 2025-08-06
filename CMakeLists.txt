# Minimum version is CMake 3.26
cmake_minimum_required(VERSION 3.26)

# Export compile commands for the language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project instantiation
project(Cobble VERSION 0.1.0.1)

# Configure the project header
configure_file(include/configuration.txt
    ${PROJECT_SOURCE_DIR}/include/configuration.hpp)

# Import Doxygen to generate documentation
find_package(Doxygen)

# Find what we need
find_package(PkgConfig REQUIRED)
pkg_check_modules(TomlPlusPlus REQUIRED tomlplusplus)

# Build our main executable
add_executable(${PROJECT_NAME}
    src/logger.cpp
    src/exception_handler.cpp
    src/environment.cpp
    src/main.cpp)

# Use C++23 on target too
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

# Include headers here
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${TomlPlusPlus_INCLUDE_DIRS}
    include)

# You can make documentation this way
add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Finally link
target_link_libraries(${PROJECT_NAME} ${TomlPlusPlus_LIBRARIES})
