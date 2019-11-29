# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS components)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-cam)

target_compile_definitions("__idf_esp32-camera" PUBLIC
        CONFIG_CAMERA_MODEL_AI_THINKER=1 )