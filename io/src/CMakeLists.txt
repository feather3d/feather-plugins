PROJECT(feather_io)

SET(feather_io_SRCS
    io.cpp
    main.cpp
)

ADD_LIBRARY(feather_io SHARED ${feather_io_SRCS})

QT5_USE_MODULES(feather_io OpenGL)

SET_TARGET_PROPERTIES(feather_io
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_io
    LIBRARY DESTINATION /usr/local/feather/plugins)
