cmake_minimum_required(VERSION 3.8)

project(MetaJson VERSION 1.0.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME} Sample/Json.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

find_package(fmt)
find_package(ctre)

include_directories(Library)

target_link_libraries(${PROJECT_NAME} fmt::fmt ctre::ctre)
