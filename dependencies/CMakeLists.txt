include(FetchContent)

FetchContent_Declare(libwebsockets 
    GIT_REPOSITORY    https://github.com/warmcat/libwebsockets.git
    GIT_TAG           8d605f0649ed1ab6d27a443c7688598ea21fdb75 # v4.2.2 
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

if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
  set(LWS_WITH_MINIMAL_EXAMPLES ON) # Later Off Build Time
  set(LWS_WITH_SHARED ON)
endif()

FetchContent_MakeAvailable(libwebsockets)