PROJECT(feather_io)

SET(feather_io_SRCS
    io.cpp
    main.cpp
)

set(feather_io_LIBS
    "-lassimp"
)

ADD_LIBRARY(feather_io SHARED ${feather_io_SRCS})

TARGET_LINK_LIBRARIES(feather_io ${feather_io_LIBS})

QT5_USE_MODULES(feather_io OpenGL)

SET_TARGET_PROPERTIES(feather_io
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_io
    LIBRARY DESTINATION /usr/local/feather/plugins)
