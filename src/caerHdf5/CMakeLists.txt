cmake_minimum_required (VERSION 3.1)

add_executable(ddd17ConvertTool "ddd17ConvertTool.c")


# this is the filename including the complete path of the file where this variable is used. 
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE} )
message(STATUS "LINK_LIBS:"  ${LINK_LIBS})

include_directories(/usr/include/hdf5/serial/)

target_link_libraries(ddd17ConvertTool ${LINK_LIBS})
