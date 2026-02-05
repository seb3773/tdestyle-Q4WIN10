# Q4WIN10 Style Config - Standalone Makefile

# Paths
TDE_PREFIX := /opt/trinity
TDE_INCLUDE := $(TDE_PREFIX)/include
TDE_LIB := $(TDE_PREFIX)/lib
TQT_INCLUDE := /usr/include/tqt3
PLUGIN_DIR := $(TDE_PREFIX)/lib/trinity

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

LDFLAGS := -shared -Wl,--gc-sections -Wl,--as-needed -flto -O2 \
    -L$(TDE_LIB) \
    -ltdeui -ltdecore -ltqt-mt

# Sources and generated files
SOURCES := q4win10styleconf.cpp
MOCS := q4win10styleconf.moc

TARGET := tdestyle_q4win10_config.so

.PHONY: all clean install

all: $(TARGET)

# MOC generation
%.moc: %.h
	$(MOC) $< -o $@

$(TARGET): $(MOCS) $(SOURCES)
	@$(CXX) $(CXXFLAGS) $(SOURCES) -o $@ $(LDFLAGS)
	@if command -v sstrip >/dev/null 2>&1; then sstrip $@ 2>/dev/null || true; else strip --strip-all $@; fi

install: all
	install -d $(DESTDIR)$(PLUGIN_DIR)
	install -m 755 $(TARGET) $(DESTDIR)$(PLUGIN_DIR)/

clean:
	rm -f $(TARGET) $(MOCS)
	rm -f *.o
