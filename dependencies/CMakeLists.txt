

FetchContent_Declare(libwebsockets 
    GIT_REPOSITORY    https://github.com/warmcat/libwebsockets.git
    GIT_TAG           0bc5b58ee3bf103ba78b6ccacbc58395e3825f8a # v4.3.0
)

message("Fetching libwebsockets...")

# option() honor variables
if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif (POLICY CMP0077)

set(LWS_WITH_SHARED OFF)
set(LWS_WITHOUT_TESTAPPS OFF)
set(LWS_WITHOUT_TEST_SERVER OFF)
set(LWS_WITHOUT_TEST_SERVER_EXTPOLL ON)
set(LWS_WITHOUT_TEST_PING ON)
set(LWS_WITHOUT_TEST_CLIENT ON)
set(LWS_WITH_HTTP_PROXY ON)
set(LWS_WITH_ACCESS_LOG ON)
set(LWS_WITH_EXPORT_LWSTARGETS OFF)

set(LWS_WITH_THREADPOOL ON) # good ?

message("Build for Type ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
  set(LWS_WITH_MINIMAL_EXAMPLES ON) # Later Off Build Time
  set(LWS_WITH_SHARED ON)
else()
  set(LWS_WITHOUT_TESTAPPS ON)
  set(LWS_WITHOUT_TEST_SERVER ON)
endif()

FetchContent_MakeAvailable(libwebsockets)

FetchContent_Declare(msgsl
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG "v3.1.0"
)

FetchContent_MakeAvailable(msgsl)

if(NOT msgsl_POPULATED)
    FetchContent_Populate(msgsl)

    set(GSL_CXX_STANDARD "14" CACHE STRING "" FORCE)
    set(GSL_TEST OFF CACHE BOOL "" FORCE)
    mark_as_advanced(GSL_CXX_STANDARD )
    mark_as_advanced(GSL_TEST)
    mark_as_advanced(FETCHCONTENT_SOURCE_DIR_MSGSL)
    mark_as_advanced(FETCHCONTENT_UPDATES_DISCONNECTED_MSGSL)

    add_subdirectory(
        ${msgsl_SOURCE_DIR}
        ${msgsl_SOURCE_DIR}/../msgsl-build
        EXCLUDE_FROM_ALL)
endif()