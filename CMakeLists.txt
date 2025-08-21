cmake_minimum_required(VERSION 3.20)
project(fiblib CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


add_executable(${PROJECT_NAME}
    tester.cpp
)