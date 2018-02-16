PROJECT(feather_lux)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

SET(feather_lux_SRCS
    main.cpp
)

ADD_LIBRARY(feather_lux SHARED ${feather_lux_SRCS})

TARGET_LINK_LIBRARIES(feather_lux
    ${Boost_SYSTEM_LIBRARY} 
    /usr/lib/feather/libfeather_plugin.so
)

#QT5_USE_MODULES(feather_lux OpenGL)

SET_TARGET_PROPERTIES(feather_lux
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_lux
    LIBRARY DESTINATION /usr/share/feather/plugins)
