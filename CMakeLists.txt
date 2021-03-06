#### NLUNE CMAKE FILE
cmake_minimum_required(VERSION 3.0)
project(nlune)

## C++11 COMPILER SUPPORT
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

## PROJECT FILES
include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

## EXECUTABLE
add_executable(${PROJECT_NAME} ${PROJECT_SRC})

## PACKAGES
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  PKG_SEARCH_MODULE(NCURSES REQUIRED ncurses)

  target_include_directories(${PROJECT_NAME} PUBLIC ${NCURSES_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${NCURSES_LIBRARIES})
else()
  message(STATUS "ERROR: pkg-config is not installed on this system.")
endif()

## EXAMPLES
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/examples/)

## FLAGS
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -DDEBUG_BUILD")
