# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\CommStudioCoreTest_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CommStudioCoreTest_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\CommStudio_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CommStudio_autogen.dir\\ParseCache.txt"
  "CommStudioCoreTest_autogen"
  "CommStudio_autogen"
  )
endif()
