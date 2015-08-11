set(ucca_srcs
    passage.cc
)

set(ucca_headers
    passage.h
    ucca-corpus.h
    cmp_id.h
    exceptions.h
)

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cc)

foreach(test_src ${TEST_SRCS})
  get_filename_component(testName ${test_src} NAME_WE)
  add_executable(${testName} ${test_src})
  target_link_libraries(${testName} ucca ${LIBS})
  set_target_properties(${testName} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests.bin)
  add_test(NAME ${testName}
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests.bin
     COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests.bin/${testName} )
endforeach(test_src)

add_library(ucca ${ucca_srcs} ${ucca_headers} ${LIBS})
target_include_directories(ucca PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES ${ucca_headers} DESTINATION include/ucca)
install(TARGETS ucca DESTINATION lib)
