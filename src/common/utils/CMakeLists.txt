message(STATUS "working on utils dir")

FILE (GLOB SRCS "commonutils.cpp" "Tokenizer.cpp" )

set(common_utils_SRCS ${SRCS} PARENT_SCOPE)
