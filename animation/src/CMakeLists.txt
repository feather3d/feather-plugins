PROJECT(feather_animation)

FIND_PACKAGE(OpenGL REQUIRED)

SET(feather_animation_SRCS
    main.cpp
)

ADD_DEFINITIONS("-D GL_GLEXT_PROTOTYPES")

ADD_LIBRARY(feather_animation SHARED ${feather_animation_SRCS})

QT5_USE_MODULES(feather_animation OpenGL)

SET_TARGET_PROPERTIES(feather_animation
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_animation
    LIBRARY DESTINATION /usr/local/feather/plugins)
