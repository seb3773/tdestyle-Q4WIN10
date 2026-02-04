add_definitions( -DQT_PLUGIN )

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

tde_add_library( tdestyle_q4win10_config SHARED AUTOMOC
  SOURCES q4win10styleconf.cpp
  LINK tdeui-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)
