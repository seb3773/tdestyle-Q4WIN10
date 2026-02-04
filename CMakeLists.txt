

add_definitions( -DQT_PLUGIN -D_DEFAULT_SOURCE -DNDEBUG -O2 -g -W -Wall -Wchar-subscripts -Wshadow -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fvisibility=hidden -fvisibility-inlines-hidden -fdata-sections -ffunction-sections -fomit-frame-pointer -ffast-math -fmerge-all-constants -flto )
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--as-needed -flto -O2")


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
