PROJECT(feather_lux_ui)

FILE(GLOB LUX_UI_QML_FILES "*.qml")

#ADD_SUBDIRECTORY(common)

INSTALL(FILES ${LUX_UI_QML_FILES} DESTINATION /usr/share/feather/plugins/lux)

