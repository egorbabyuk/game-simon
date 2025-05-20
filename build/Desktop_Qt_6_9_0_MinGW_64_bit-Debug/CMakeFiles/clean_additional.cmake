# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\combGame_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\combGame_autogen.dir\\ParseCache.txt"
  "combGame_autogen"
  )
endif()
