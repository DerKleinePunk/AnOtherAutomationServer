project("AutoIoBackend" CXX)

MESSAGE(STATUS "Try Build ${PROJECT_NAME}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

## Use all the *.cpp files we found under this folder for the project Find better way for coomen cpp files
FILE(GLOB SRCS "*.cpp" "*/*.cpp")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    SET (project_BIN ${PROJECT_NAME})
else()
    SET (project_BIN ${PROJECT_NAME}.bin)
endif()

INCLUDE_DIRECTORIES(${LIBHTTPSERVER_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LIBMOSQUITTO_INCLUDE_DIRS})

add_executable(${project_BIN} ${SRCS} ${easylogging_SRCS} ${common_utils_SRCS})

TARGET_LINK_LIBRARIES(${project_BIN} ${LIBHTTPSERVER_LIBRARIES} ${LIBMOSQUITTO_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} "stdc++fs" "uuid")

set(MAINFONTSOURCE "${MAINFOLDER}/src/IoBackend/loggerIoBackend.conf")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                       "${MAINFONTSOURCE}"      # <--this is in-file
                       $<TARGET_FILE_DIR:${project_BIN}>)