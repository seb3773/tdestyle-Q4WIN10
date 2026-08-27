include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIBRARY_DIRS}
)

##### tdestyle_q4win10_config (module) ##########

tde_add_kpart( tdestyle_q4win10_config AUTOMOC
  SOURCES
    q4win10styleconf.cpp
  LINK
    tdecore-shared
    tdeui-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}
)

if( SSTRIP_EXECUTABLE AND Q4WIN10_AGGRESSIVE_FLAGS )
  add_custom_command( TARGET tdestyle_q4win10_config-module POST_BUILD
    COMMAND ${SSTRIP_EXECUTABLE} $<TARGET_FILE:tdestyle_q4win10_config-module>
    COMMENT "Aggressively stripping tdestyle_q4win10_config.so with sstrip"
  )
endif()
