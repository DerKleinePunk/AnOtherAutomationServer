# - Find MOSQUITTO

if(MOSQUITTO_INCLUDE_DIRS AND MOSQUITTO_LIBRARIES)
    set(MOSQUITTO_FOUND TRUE)

else(MOSQUITTO_INCLUDE_DIRS AND MOSQUITTO_LIBRARIES)
    SET(MOSQUITTO_SEARCH_PATHS
      ${MOSQUITTO_SRC}
      ~/Library/Frameworks
      /Library/Frameworks
      /usr/local
      /usr
      /sw # Fink
      /opt/local # DarwinPorts
      /opt/csw # Blastwave
      /opt
      /usr/lib/arm-linux-gnueabihf/
      $ENV{VCPKG_ROOT}\\installed\\x86-windows
    )

    find_path(MOSQUITTO_INCLUDE_DIRS NAMES mosquitto.h
      HINTS
        PATH_SUFFIXES include/mosquitto include
        PATHS ${MOSQUITTO_SEARCH_PATHS}
      )

  find_library(MOSQUITTO_LIBRARIES NAMES mosquitto
      PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      )

  if(MOSQUITTO_INCLUDE_DIRS AND MOSQUITTO_LIBRARIES)
    set(MOSQUITTO_FOUND TRUE)
    message(STATUS "Found MOSQUITTO: ${MOSQUITTO_INCLUDE_DIRS}, ${MOSQUITTO_LIBRARIES}")
  else(MOSQUITTO_INCLUDE_DIRS AND MOSQUITTO_LIBRARIES)
    set(MOSQUITTO_FOUND FALSE)
    message(STATUS "MOSQUITTO not found.")
  endif(MOSQUITTO_INCLUDE_DIRS AND MOSQUITTO_LIBRARIES)

  mark_as_advanced(MOSQUITTO_INCLUDE_DIRS MOSQUITTO_LIBRARIES)

endif(MOSQUITTO_INCLUDE_DIRS AND MOSQUITTO_LIBRARIES)
    