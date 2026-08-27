# Q4WIN10 Style Plugin - Standalone Makefile
# (C) 2026 Q4OS / TDE Developers

# Paths
TDE_PREFIX ?= /opt/trinity
TDE_INCLUDE ?= $(TDE_PREFIX)/include
TDE_LIB ?= $(TDE_PREFIX)/lib
TQT_INCLUDE ?= /usr/include/tqt3
PLUGIN_DIR ?= $(TDE_PREFIX)/lib/trinity/plugins/styles
CONFIG_DIR ?= $(TDE_PREFIX)/lib/trinity
THEME_DIR ?= $(TDE_PREFIX)/share/apps/tdestyle/themes

# Tools
CXX ?= g++
MOC := $(shell which tmoc moc-tqt 2>/dev/null | head -n 1)

# Aggressive Optimization Flags
CXXFLAGS := -fPIC \
    -I. \
    -I$(TDE_INCLUDE) \
    -I$(TQT_INCLUDE) \
    -I$(TQT_INCLUDE)/../tqt \
    -include tqt.h \
    -DTQT_NO_ASCII_CAST -DTQT_NO_STL -DTQT_NO_COMPAT \
    -DTQT_NO_TRANSLATION -DTQT_THREAD_SUPPORT -D_REENTRANT \
    -DQT_PLUGIN -D_DEFAULT_SOURCE -DNDEBUG \
    -O2 -flto=auto -fstrict-aliasing \
    -ffunction-sections -fdata-sections \
    -fno-ident -fno-plt -fomit-frame-pointer \
    -fno-stack-protector -fno-math-errno \
    -fvisibility=hidden -fvisibility-inlines-hidden \
    -fmerge-all-constants -fno-asynchronous-unwind-tables \
    -std=c++03 -fno-threadsafe-statics -fno-exceptions \
    -Wno-deprecated-declarations -fpermissive

LDFLAGS := -shared -Wl,-O1 -Wl,-Bsymbolic -Wl,--gc-sections -Wl,--as-needed \
    -Wl,--build-id=none -Wl,-z,norelro -flto=auto -s -Wl,--no-undefined \
    -L$(TDE_LIB) \
    -ltdecore -ltdefx -ltqt-mt -lX11

# Sources
MAIN_SRCS := q4win10style.cpp
CONFIG_SRCS := config/q4win10styleconf.cpp

# Generated files
MAIN_MOCS := q4win10style.moc
CONFIG_MOCS := config/q4win10styleconf.moc

# Targets
MAIN_TARGET := q4win10.so
CONFIG_TARGET := config/tdestyle_q4win10_config.so

.PHONY: all clean install

all: $(MAIN_TARGET) $(CONFIG_TARGET)
	@echo "Build complete!"
	@ls -lh $(MAIN_TARGET) $(CONFIG_TARGET)

# MOC generation
%.moc: %.h
	$(MOC) $< -o $@

config/q4win10styleconf.moc: config/q4win10styleconf.h
	$(MOC) $< -o $@

# Logo header generation
config/q4win10_logo.h: tdewin_128_16.png convert_images.py
	python3 ./convert_images.py

# Main style plugin
$(MAIN_TARGET): $(MAIN_MOCS) $(MAIN_SRCS)
	@$(CXX) $(CXXFLAGS) $(MAIN_SRCS) -o $@ $(LDFLAGS)
	@strip --strip-unneeded $@

# Config plugin
$(CONFIG_TARGET): config/q4win10_logo.h $(CONFIG_MOCS) $(CONFIG_SRCS)
	@$(CXX) $(CXXFLAGS) -Iconfig $(CONFIG_SRCS) -o $@ $(LDFLAGS) -ltdeui
	@# Generate .la file
	@echo "# $(notdir $(CONFIG_TARGET:.so=.la)) - a libtool library file" > $(CONFIG_TARGET:.so=.la)
	@echo "dlname='$(notdir $(CONFIG_TARGET))'" >> $(CONFIG_TARGET:.so=.la)
	@echo "library_names='$(notdir $(CONFIG_TARGET)) $(notdir $(CONFIG_TARGET)) $(notdir $(CONFIG_TARGET))'" >> $(CONFIG_TARGET:.so=.la)
	@echo "old_library=''" >> $(CONFIG_TARGET:.so=.la)
	@echo "dependency_libs=' -L$(TDE_LIB) -ltdeui -ltdecore -ltdefx -ltqt-mt '" >> $(CONFIG_TARGET:.so=.la)
	@echo "current=0" >> $(CONFIG_TARGET:.so=.la)
	@echo "age=0" >> $(CONFIG_TARGET:.so=.la)
	@echo "revision=0" >> $(CONFIG_TARGET:.so=.la)
	@echo "installed=yes" >> $(CONFIG_TARGET:.so=.la)
	@echo "shouldnotlink=yes" >> $(CONFIG_TARGET:.so=.la)
	@echo "dlopen=''" >> $(CONFIG_TARGET:.so=.la)
	@echo "dlpreopen=''" >> $(CONFIG_TARGET:.so=.la)
	@echo "libdir='$(TDE_PREFIX)/lib/trinity'" >> $(CONFIG_TARGET:.so=.la)
	@strip --strip-unneeded $@

install: all
	install -d $(DESTDIR)$(PLUGIN_DIR)
	install -d $(DESTDIR)$(CONFIG_DIR)
	install -d $(DESTDIR)$(THEME_DIR)
	install -d $(DESTDIR)$(TDE_PREFIX)/share/apps/kstyle/themes
	install -m 755 $(MAIN_TARGET) $(DESTDIR)$(PLUGIN_DIR)/
	install -m 755 $(CONFIG_TARGET) $(DESTDIR)$(CONFIG_DIR)/
	install -m 644 $(CONFIG_TARGET:.so=.la) $(DESTDIR)$(CONFIG_DIR)/
	install -m 644 q4win10.themerc $(DESTDIR)$(THEME_DIR)/
	install -m 644 q4win10.themerc $(DESTDIR)$(TDE_PREFIX)/share/apps/kstyle/themes/
	@echo "Installed plugins to $(DESTDIR)$(PLUGIN_DIR)/"
	@echo "Installed config to $(DESTDIR)$(CONFIG_DIR)/"
	@echo "Installed .themerc to $(DESTDIR)$(THEME_DIR)/"
	@echo "Run: tdebuildsycoca"

clean:
	rm -f $(MAIN_TARGET) $(CONFIG_TARGET) $(CONFIG_TARGET:.so=.la)
	rm -f $(MAIN_MOCS) $(CONFIG_MOCS)
	rm -f *.o config/*.o config/q4win10_logo.h
