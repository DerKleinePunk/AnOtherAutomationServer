include(FetchContent)

FetchContent_Declare(libwebsockets 
    GIT_REPOSITORY    https://github.com/warmcat/libwebsockets.git
    GIT_TAG           8d605f0649ed1ab6d27a443c7688598ea21fdb75 # v4.2.2 
)

message("Fetching libwebsockets...")

set(LWS_WITH_SHARED OFF)
set(LWS_WITHOUT_TESTAPPS OFF)
set(LWS_WITHOUT_TEST_SERVER OFF)
set(LWS_WITHOUT_TEST_SERVER_EXTPOLL ON)
set(LWS_WITHOUT_TEST_PING ON)
set(LWS_WITHOUT_TEST_CLIENT ON)
set(LWS_WITH_HTTP_PROXY ON)
set(LWS_WITH_ACCESS_LOG ON)

set(LWS_WITH_THREADPOOL ON) # good ?
set(LWS_WITH_MINIMAL_EXAMPLES ON) # Later Off Build Time


FetchContent_MakeAvailable(libwebsockets)