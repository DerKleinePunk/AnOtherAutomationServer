project("SimpelIoBackend" CXX)

MESSAGE(STATUS "Try Build ${PROJECT_NAME}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

## Use all the *.cpp files we found under this folder for the project Find better way for comon cpp files
FILE(GLOB SRCS "*.cpp" "*/*.cpp" "*/*/*.cpp")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    SET (project_BIN ${PROJECT_NAME})
else()
    SET (project_BIN ${PROJECT_NAME}.bin)
endif()

INCLUDE_DIRECTORIES(${MOSQUITTO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GLIB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${NM_INCLUDE_DIRS})
# INCLUDE_DIRECTORIES(libwebsockets)

add_executable(${project_BIN} ${SRCS} ${easylogging_SRCS} ${common_utils_SRCS} ${GPIOHelper_SRCS} ${exception_SRCS} ${common_database_SRCS} ${common_exceptions_SRCS})

TARGET_LINK_LIBRARIES(${project_BIN} PRIVATE ${Python3_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} PRIVATE ${MOSQUITTO_LIBRARIES} ${GLIB_LDFLAGS} ${NM_LIBRARIES} ${SQLITE3_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} PRIVATE websockets GSL)
TARGET_LINK_LIBRARIES(${project_BIN} PRIVATE "stdc++fs" "uuid")
TARGET_LINK_LIBRARIES(${project_BIN} PRIVATE "curl")

set(MAINFONTSOURCE "${MAINFOLDER}/src/IoBackend/loggerIoBackend.conf")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                       "${MAINFONTSOURCE}"      # <--this is in-file
                       $<TARGET_FILE_DIR:${project_BIN}>)

set(MAINFONTSOURCE "${MAINFOLDER}/src/IoBackend/webpage/index.html")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                    "${MAINFONTSOURCE}"      # <--this is in-file
                    $<TARGET_FILE_DIR:${project_BIN}>/webpage/index.html)

set(MAINFONTSOURCE "${MAINFOLDER}/src/IoBackend/webpage/404.html")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                                        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                                        "${MAINFONTSOURCE}"      # <--this is in-file
                                        $<TARGET_FILE_DIR:${project_BIN}>/webpage/404.html)

set(MAINFONTSOURCE "${MAINFOLDER}/src/IoBackend/webpage/blockypage.html")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                                        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                                        "${MAINFONTSOURCE}"      # <--this is in-file
                                        $<TARGET_FILE_DIR:${project_BIN}>/webpage/blockypage.html) 
                                              
set(MAINFONTSOURCE "${MAINFOLDER}/src/IoBackend/webpage/scripts/websocket.js")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                    "${MAINFONTSOURCE}"      # <--this is in-file
                    $<TARGET_FILE_DIR:${project_BIN}>/webpage/scripts/websocket.js)
                    
set(MAINFONTSOURCE "${MAINFOLDER}/src/IoBackend/PythonRunner/sample.py")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                    "${MAINFONTSOURCE}"      # <--this is in-file
                    $<TARGET_FILE_DIR:${project_BIN}>/sample.py)

set(MAINFONTSOURCE "${MAINFOLDER}/resources/icon1.png")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                    "${MAINFONTSOURCE}"      # <--this is in-file
                    $<TARGET_FILE_DIR:${project_BIN}>/resources/icon1.png)

set(MAINFONTSOURCE "${MAINFOLDER}/resources/icon2.png")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                    "${MAINFONTSOURCE}"      # <--this is in-file
                    $<TARGET_FILE_DIR:${project_BIN}>/resources/icon2.png)