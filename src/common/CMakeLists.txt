MESSAGE(STATUS "working on common")

ADD_SUBDIRECTORY(easylogging)
ADD_SUBDIRECTORY(utils)

set(easylogging_SRCS ${easylogging_SRCS} PARENT_SCOPE)
set(common_utils_SRCS ${common_utils_SRCS} PARENT_SCOPE)