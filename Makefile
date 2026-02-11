# Q4WIN10 Style Plugin - Standalone Makefile

# Paths
TDEBASE := $(shell cd ../../ && pwd)
TDE_PREFIX := /opt/trinity
TDE_INCLUDE := $(TDE_PREFIX)/include
TDE_LIB := $(TDE_PREFIX)/lib
TQT_INCLUDE := /usr/include/tqt3
PLUGIN_DIR := $(TDE_PREFIX)/lib/trinity/plugins/styles
DATA_DIR := $(TDE_PREFIX)/share/apps/tdestyle/themes

# Tools
CXX := g++
MOC := $(shell which tmoc moc-tqt 2>/dev/null | head -n 1)

CXXFLAGS := -fPIC \
    -isystem $(TDE_INCLUDE) \
    -isystem $(TQT_INCLUDE) \
    -isystem $(TQT_INCLUDE)/../tqt \
    -include tqt.h \
    -DTQT_THREAD_SUPPORT -D_REENTRANT \
    -DQT_PLUGIN -D_DEFAULT_SOURCE -DNDEBUG \
    -O2 \
    -fdata-sections -ffunction-sections -fomit-frame-pointer \
    -ffast-math -fmerge-all-constants -flto

# TDE Version Detection for Cross-Version Compatibility
TDE_VERSION := $(shell tde-config --version 2>/dev/null | sed -n 's/^TDE: R//p')
# If TDE >= 14.1.5, add deprecated TQT macros
ifeq ($(shell expr "$(TDE_VERSION)" \>= "14.1.5"), 1)
    CXXFLAGS += -DTQT_SIGNAL=TQ_SIGNAL -DTQT_SLOT=TQ_SLOT
endif

LDFLAGS := -shared -Wl,--gc-sections -Wl,--as-needed -flto -O2 \
    -L$(TDE_LIB) \
    -ltdecore -ltdefx -ltqt-mt

# Sources
MAIN_SRCS := q4win10style.cpp
CONFIG_SRCS := config/q4win10styleconf.cpp

# Generated files
MAIN_MOCS := q4win10style.moc
CONFIG_MOCS := config/q4win10styleconf.moc

# Targets
MAIN_TARGET := tde_style_q4win10.so
CONFIG_TARGET := config/tdestyle_q4win10_config.so

.PHONY: all clean install

all: $(MAIN_TARGET) $(CONFIG_TARGET)
	@echo "Build complete!"
	@ls -lh $(MAIN_TARGET) $(CONFIG_TARGET)

# MOC generation
%.moc: %.h
	$(MOC) $< -o $@

config/%.moc: config/%.h
	@cd config && $(MOC) $(notdir $<) -o $(notdir $@)

# Main style plugin
$(MAIN_TARGET): $(MAIN_MOCS) $(MAIN_SRCS)
	@$(CXX) $(CXXFLAGS) $(MAIN_SRCS) -o $@ $(LDFLAGS)
	@if command -v sstrip >/dev/null 2>&1; then sstrip $@ 2>/dev/null || true; else strip --strip-all $@; fi

# Config plugin
$(CONFIG_TARGET): $(CONFIG_MOCS) $(CONFIG_SRCS)
	@$(CXX) $(CXXFLAGS) -Iconfig $(CONFIG_SRCS) -o $@ $(LDFLAGS) -ltdeui
	@if command -v sstrip >/dev/null 2>&1; then sstrip $@ 2>/dev/null || true; else strip --strip-all $@; fi

install: all
	install -d $(DESTDIR)$(PLUGIN_DIR)
	install -d $(DESTDIR)$(DATA_DIR)
	install -m 755 $(MAIN_TARGET) $(DESTDIR)$(PLUGIN_DIR)/
	install -m 755 $(CONFIG_TARGET) $(DESTDIR)$(TDE_PREFIX)/lib/trinity/
	install -m 644 q4win10.themerc $(DESTDIR)$(DATA_DIR)/
	@echo "Installed plugins to $(DESTDIR)$(PLUGIN_DIR)/"
	@echo "Installed .themerc to $(DESTDIR)$(DATA_DIR)/"
	@echo "Run: tdebuildsyscoca"

clean:
	rm -f $(MAIN_TARGET) $(CONFIG_TARGET)
	rm -f $(MAIN_MOCS) $(CONFIG_MOCS)
	rm -f *.o config/*.o
