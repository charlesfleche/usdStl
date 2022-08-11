add_library(usdStl
  MODULE
  fileFormat.cpp
  fileFormat.h
  plugInfo.json
  stl_reader.h
)
target_link_libraries(usdStl
  usdGeom
)
target_include_directories(usdStl
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(usdStl
  PROPERTIES
    PREFIX ""
)

# Making plugInfo.json available to tests

configure_file(plugInfo.json ${CMAKE_CURRENT_BINARY_DIR}/usdStl/resources/plugInfo.json
  COPYONLY
)

install(
  TARGETS usdStl
  LIBRARY DESTINATION .
)

install(
  FILES plugInfo.json
  DESTINATION usdStl/resources
)

add_subdirectory(testenv)
