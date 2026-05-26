# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\SpatialAnalysisEngine_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SpatialAnalysisEngine_autogen.dir\\ParseCache.txt"
  "SpatialAnalysisEngine_autogen"
  )
endif()
