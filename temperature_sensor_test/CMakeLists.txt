# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set up ESP-IDF environment
set(EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/components)

project(temperature_sensor_test)

