PROJECT(ucca)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# look for Boost
if(DEFINED ENV{BOOST_ROOT})
  set(Boost_NO_SYSTEM_PATHS ON)
endif()
set(Boost_REALPATH ON)
find_package(Boost COMPONENTS program_options serialization iostreams system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE(passage passage.cc)
target_link_libraries(passage ${Boost_LIBRARIES})

