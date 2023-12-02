cmake_minimum_required(VERSION 3.8)

project(MetaJson VERSION 1.0.0 LANGUAGES CXX)

option(UseInstalled "Uses the installed veriosn of the library or the in-source one" OFF)

add_executable(${PROJECT_NAME} Json.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

find_package(fmt REQUIRED)
find_package(ctre REQUIRED)

if(UseInstalled)
    find_package(CoreMetaJson REQUIRED)
    target_link_libraries(${PROJECT_NAME} fmt::fmt Core::Meta::Json)
    message("Using the installed version of the library")
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ../Library)
    target_link_libraries(${PROJECT_NAME} fmt::fmt ctre::ctre)
    message("Using the in-source version of the library")
endif()
