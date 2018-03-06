PROJECT(feather_lux)

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(OpenMP REQUIRED)
FIND_PACKAGE(Boost COMPONENTS thread program_options filesystem serialization iostreams regex system REQUIRED)

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)

SET(feather_lux_SRCS
    main.cpp
)

ADD_LIBRARY(feather_lux SHARED ${feather_lux_SRCS})

#SET(CMAKE_LINKER_FLAGS "-WL, --whole-archive luxcore.a -WL, --no-whole-archive" )
#SET(CMAKE_LINKER_FLAGS "-WL, --whole-archive /usr/lib/libluxrays.a -WL, --no-whole-archive" )
#SET(CMAKE_CXX_FLAGS "-fPIC" )

ADD_LIBRARY(luxcore_library STATIC IMPORTED)
SET_TARGET_PROPERTIES(luxcore_library PROPERTIES IMPORTED_LOCATION /lib/libluxcore.a)
ADD_LIBRARY(luxrays_library STATIC IMPORTED)
SET_TARGET_PROPERTIES(luxrays_library PROPERTIES IMPORTED_LOCATION /lib/libluxrays.a)
ADD_LIBRARY(smallluxgpu_library STATIC IMPORTED)
SET_TARGET_PROPERTIES(smallluxgpu_library PROPERTIES IMPORTED_LOCATION /lib/libsmallluxgpu.a)


TARGET_LINK_LIBRARIES(feather_lux
    #${Boost_SYSTEM_LIBRARY} 
    /usr/lib/feather/libfeather_plugin.so
    ${Boost_LIBRARIES}
    ${OpenCL_LIBRARIES} 
    ${OpenMP_CXX_LIBRARIES} 
    /lib/libOpenImageIO.so
    /lib/libembree.so
    #/lib/libpthread.so
    #/lib/libboost_thread.so
    luxcore_library
    smallluxgpu_library
    luxrays_library
    #/usr/lib/liblux.so
    #/usr/lib/libluxcore.a
    #/usr/lib/libluxrays.a
)


# Static Linking
#FIND_LIBRARY(feather_lux "/usr/lib64/libluxcore.a" )

#SET_TARGET_PROPERTIES(feather_lux PROPERTIES LINK_SEARCH_START_STATIC 1)
#SET_TARGET_PROPERTIES(feather_lux PROPERTIES LINK_SEARCH_END_STATIC 1)
#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(CMAKE_EXE_LINKER_FLAGS "-static-libluxcore -static-libluxrays")

#QT5_USE_MODULES(feather_lux OpenGL)

SET_TARGET_PROPERTIES(feather_lux
    PROPERTIES
    VERSION 0.01
    SOVERSION 1)

INSTALL(TARGETS feather_lux
    LIBRARY DESTINATION /usr/share/feather/plugins)
