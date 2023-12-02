cmake_minimum_required(VERSION 3.12)

project(CoreMetaJson VERSION 0.1.1 LANGUAGES CXX)

set(LIBRARY_TARGET Json)
set(LIBRARY_NAMESPACE Core::Meta::)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(${LIBRARY_TARGET} INTERFACE)
target_compile_features(${LIBRARY_TARGET} INTERFACE cxx_std_20)

# This library is (at least for now) dependent on ctre
find_package(ctre)
link_libraries(ctre::ctre)

target_include_directories(${LIBRARY_TARGET} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Library>
    $<INSTALL_INTERFACE:include>)

install(TARGETS ${LIBRARY_TARGET} EXPORT ${PROJECT_NAME}-targets)

if(NOT EXISTS "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake.in")
    file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake.in [[
    @PACKAGE_INIT@
    include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")
  ]])
endif()

configure_package_config_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}-targets
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
    NAMESPACE ${LIBRARY_NAMESPACE})

install(
    FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME})

install(DIRECTORY Library/Core DESTINATION include)
