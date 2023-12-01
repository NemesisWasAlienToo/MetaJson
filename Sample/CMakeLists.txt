cmake_minimum_required(VERSION 3.8)

project(MetaJson VERSION 1.0.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME} Json.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

find_package(fmt REQUIRED)
find_package(ctre REQUIRED)
find_package(CoreMetaJson REQUIRED)

target_link_libraries(${PROJECT_NAME} fmt::fmt ctre::ctre Core::Meta::Json)
