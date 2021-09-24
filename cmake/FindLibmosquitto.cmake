# - Find libmosquitto

if(LIBMOSQUITTO_INCLUDE_DIRS AND LIBMOSQUITTO_LIBRARIES)
    set(LIBHTTPSERVER_FOUND TRUE)

else(LIBMOSQUITTO_INCLUDE_DIRS AND LIBMOSQUITTO_LIBRARIES)
    find_path(libmosquitto_INCLUDE_DIRS mosquitto
      /usr/include
      /usr/include/mosquitto
      /usr/local/include/
      /usr/local/include/mosquitto
      )

  find_library(LIBMOSQUITTO_LIBRARIES NAMES mosquitto
      PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      )

  if(LIBMOSQUITTO_INCLUDE_DIRS AND LIBMOSQUITTO_LIBRARIES)
    set(LIBMOSQUITTO_FOUND TRUE)
    message(STATUS "Found libmosquitto: ${LIBMOSQUITTO_LIBRARIES}, ${LIBMOSQUITTO_LIBRARIES}")
  else(LIBMOSQUITTO_INCLUDE_DIRS AND LIBMOSQUITTO_LIBRARIES)
    set(LIBMOSQUITTO_FOUND FALSE)
    message(STATUS "libmosquitto not found.")
  endif(LIBMOSQUITTO_INCLUDE_DIRS AND LIBMOSQUITTO_LIBRARIES)

  mark_as_advanced(LIBMOSQUITTO_INCLUDE_DIRS LIBMOSQUITTO_LIBRARIES)

endif(LIBMOSQUITTO_INCLUDE_DIRS AND LIBMOSQUITTO_LIBRARIES)
    