PROJECT(feather_deformer)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

SET(feather_deformer_SRCS
    main.cpp
)

ADD_DEFINITIONS("-D GL_GLEXT_PROTOTYPES")

ADD_LIBRARY(feather_deformer SHARED ${feather_deformer_SRCS})

TARGET_LINK_LIBRARIES(feather_deformer
    ${Boost_SYSTEM_LIBRARY} 
    /usr/lib/feather/libfeather_plugin.so
)

QT5_USE_MODULES(feather_deformer OpenGL)

SET_TARGET_PROPERTIES(feather_deformer
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_deformer
    LIBRARY DESTINATION /usr/share/feather/plugins)
