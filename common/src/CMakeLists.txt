PROJECT(feather_common)

FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

SET(feather_common_SRCS
    main.cpp
)

ADD_LIBRARY(feather_common SHARED ${feather_common_SRCS})

TARGET_LINK_LIBRARIES(feather_common
    ${Boost_SYSTEM_LIBRARY} 
    /usr/lib/feather/libfeather_core.so
    /usr/lib/feather/libfeather_plugin.so
)

QT5_USE_MODULES(feather_common OpenGL)

SET_TARGET_PROPERTIES(feather_common
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_common
    LIBRARY DESTINATION /usr/local/feather/plugins)
