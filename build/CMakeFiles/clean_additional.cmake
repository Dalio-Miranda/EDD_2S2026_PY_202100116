# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CinemaUSAC_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CinemaUSAC_autogen.dir\\ParseCache.txt"
  "CinemaUSAC_autogen"
  )
endif()
