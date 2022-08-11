add_executable(testUsdStl
  testUsdStl.cpp
)
add_dependencies(testUsdStl
  usdStl
)
target_link_libraries(testUsdStl
  ${PXR_LIBRARIES}
)
configure_file(testUsdStl.usda ${CMAKE_CURRENT_BINARY_DIR}
  COPYONLY
)
configure_file(cube.stl ${CMAKE_CURRENT_BINARY_DIR}
  COPYONLY
)
configure_file(testUsdStlExpected.usda ${CMAKE_CURRENT_BINARY_DIR}/
  COPYONLY
)

add_test(NAME testUsdStl COMMAND testUsdStl)
