PROJECT(feather_ply)

SET(feather_ply_SRCS
    #obj.cpp
    main.cpp
)

ADD_LIBRARY(feather_ply SHARED ${feather_ply_SRCS})

#QT5_USE_MODULES(feather_ply OpenGL)

SET_TARGET_PROPERTIES(feather_ply
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_ply
    LIBRARY DESTINATION /usr/local/feather/plugins)
