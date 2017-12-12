PROJECT(feather_polygon)

#FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

SET(feather_polygon_SRCS
    subdiv.cpp
    main.cpp
)

ADD_DEFINITIONS("-D GL_GLEXT_PROTOTYPES")

ADD_LIBRARY(feather_polygon SHARED ${feather_polygon_SRCS})

#QT5_USE_MODULES(feather_polygon OpenGL)

TARGET_LINK_LIBRARIES(feather_polygon
    "-losdCPU"
    "-losdGPU"
    ${Boost_SYSTEM_LIBRARY} 
    /usr/lib/feather/libfeather_core.so
    /usr/lib/feather/libfeather_plugin.so
)

SET_TARGET_PROPERTIES(feather_polygon
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_polygon
    LIBRARY DESTINATION /usr/share/feather/plugins)
