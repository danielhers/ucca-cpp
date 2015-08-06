project(ucca)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

enable_testing()

find_package(RapidXML)

if(NOT RAPIDXML_FOUND)
    message(STATUS "RapidXML not found - using internal version.")
    set(RapidXML_INCLUDE_DIR "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/rapidxml/>" CACHE PATH "Include directory for RapidXML" FORCE)
    file(GLOB RapidXML_HEADERS ${PROJECT_SOURCE_DIR}/rapidxml/*.hpp)
    install(FILES ${RapidXML_HEADERS} DESTINATION include)
endif()

find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(ucca)
