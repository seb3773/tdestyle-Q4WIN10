

add_definitions( -DQT_PLUGIN -DNDEBUG )
set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CXX_FLAGS_RELEASE "-O2 -s -fvisibility=hidden -fvisibility-inlines-hidden" )


include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
)

tde_add_kpart( tde_style_q4win10 AUTOMOC
  SOURCES q4win10style.cpp
  LINK tdecore-shared tdefx-shared
  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles
)

install( FILES q4win10.themerc DESTINATION ${DATA_INSTALL_DIR}/tdestyle/themes )
