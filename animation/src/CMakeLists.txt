PROJECT(feather_animation)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

SET(feather_animation_SRCS
    main.cpp
)

ADD_DEFINITIONS("-D GL_GLEXT_PROTOTYPES")

ADD_LIBRARY(feather_animation SHARED ${feather_animation_SRCS})

TARGET_LINK_LIBRARIES(feather_animation
    ${Boost_SYSTEM_LIBRARY} 
    /usr/lib/feather/libfeather_plugin.so
)

QT5_USE_MODULES(feather_animation OpenGL)

SET_TARGET_PROPERTIES(feather_animation
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_animation
    LIBRARY DESTINATION /usr/local/feather/plugins)
