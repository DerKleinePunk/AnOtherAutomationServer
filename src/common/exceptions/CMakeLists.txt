message(STATUS "working on exceptions dir")

FILE (GLOB_RECURSE SRCS "*.cpp" "*.cxx" "*.cc" "*.c")

set(common_exceptions_SRCS ${SRCS} PARENT_SCOPE)