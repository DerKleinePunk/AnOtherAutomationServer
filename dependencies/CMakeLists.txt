include(FetchContent)

FetchContent_Declare(libwebsockets 
    GIT_REPOSITORY    https://github.com/warmcat/libwebsockets.git
    GIT_TAG           8d605f0649ed1ab6d27a443c7688598ea21fdb75 # v4.2.2 
)

message("Fetching libwebsockets...")

set(LWS_WITH_SHARED OFF)
set(LWS_WITHOUT_TESTAPPS ON)
set(LWS_WITHOUT_TEST_SERVER ON)
set(LWS_WITHOUT_TEST_SERVER_EXTPOLL ON)
set(LWS_WITHOUT_TEST_PING ON)
set(LWS_WITHOUT_TEST_CLIENT ON)

FetchContent_MakeAvailable(libwebsockets)