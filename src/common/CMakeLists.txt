MESSAGE(STATUS "working on common")

ADD_SUBDIRECTORY(easylogging)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(database)
ADD_SUBDIRECTORY(exceptions)


set(easylogging_SRCS ${easylogging_SRCS} PARENT_SCOPE)
set(common_utils_SRCS ${common_utils_SRCS} PARENT_SCOPE)
set(common_database_SRCS ${common_database_SRCS} PARENT_SCOPE)
set(common_exceptions_SRCS ${common_exceptions_SRCS} PARENT_SCOPE)