Step-by-step instructions for building and installing the Q4WIN10 Style from source:

- TDE Versions: 14.1.1 or later (tested on 14.1.1 and 14.1.5)


- Getting the Source Code

First, check which version of TDE is installed on your system:

tde-config --version

Note the TDE version and then download TDE Base Source: You need the `tdebase` source code matching your installed
TDE version (Download from TDE Git)

--> Clone tdebase for your TDE version (replace 14.1.1 with your version)
git clone https://mirror.git.trinitydesktop.org/gitea/TDE/tdebase.git -b r14.1.1 tdebase-trinity-14.1.1

Then, clone the Q4WIN10 Style repository to a temporary location:
cd /tmp
git clone https://github.com/seb3773/tde-style-Q4WIN10

Next, copy the style files into TDE Base: (let's assume you cloned tdebase in ~/src/)

cp -r /tmp/tde-style-Q4WIN10 ~/src/tdebase-trinity-14.1.1/kstyles/Q4WIN10

Then verify the files are in place:
ls ~/src/tdebase-trinity-14.1.1/kstyles/Q4WIN10

You should see files like `Makefile`, `q4win10.cpp`, `q4win10.h`, `style/`, `config/`, `create_deb.sh`, and `README.md`.


- Prerequisites

Make sure you have:
- GCC/G++ with C++11 support
- TDE development headers (libtqt-dev, tdelibs-dev, tdebase-trinity-dev)
- tmoc (TDE Meta Object Compiler) or moc-tqt
- uic-tqt (TDE User Interface Compiler)
- Gold linker (optional, for smaller binaries)
- sstrip (optional, for maximum binary compression)

Verify tools:

g++ --version
tmoc -v
uic-tqt -version
sstrip --version

The gold linker and sstrip are optional but recommended for production builds. If they are not available,
the build system will fall back to the standard linker and strip.


- Build Architecture

The Q4WIN10 Style uses a **standalone Makefile** and does NOT require
running cmake from the tdebase root. This makes it much simpler to build.

The Makefile automatically:
- Detects the tdebase root directory (from relative path ../../)
- Finds tmoc and uic-tqt tools
- Generates MOC and UIC files
- Compiles two shared libraries: the style plugin and the config module
- Applies sstrip (if available) after compilation

The only requirement is that the source files are placed inside the tdebase source tree
(at `kstyles/Q4WIN10/`) so the Makefile can find the `kstyles/` headers if needed.


Now, all commands assume you are in the style source directory:

cd ~/src/tdebase-trinity-14.1.1/kstyles/Q4WIN10


- Building

> The build produces two shared libraries:
> - `q4win10.so` — The style plugin
> - `config/kstyle_q4win10_config.so` — The configuration panel

First, clean any previous build artifacts:

make clean

Then build:

make

That's it. No cmake, no configure step.


During compilation, you should see something like:

/usr/bin/tmoc q4win10.h -o q4win10.moc
/usr/bin/tmoc style/q4win10style.h -o style/q4win10style.moc
...
Build complete!
-rwxr-xr-x 1 user user 142K Feb 11 23:15 q4win10.so
-rwxr-xr-x 1 user user  37K Feb 11 23:15 config/kstyle_q4win10_config.so

Compilation takes only a few seconds.


After a successful build, the key generated files are:

```
Q4WIN10/
├── q4win10.so                          # Style plugin  <--- installed to /opt/trinity/lib/trinity/plugins/styles/
├── q4win10.themerc                     # Theme definition <--- installed to /opt/trinity/share/apps/kstyle/themes/
├── config/
│   └── kstyle_q4win10_config.so        # Config panel plugin <--- installed to /opt/trinity/lib/trinity/
└── ... (source files)
```


* Installing

To install the style system-wide (requires root privileges):

Make sure you're in the style source directory:
cd ~/src/tdebase-trinity-14.1.1/kstyles/Q4WIN10

sudo make install

This will:
- Copy `q4win10.so` to `/opt/trinity/lib/trinity/plugins/styles/`
- Copy `kstyle_q4win10_config.so` to `/opt/trinity/lib/trinity/`
- Copy `q4win10.themerc` to `/opt/trinity/share/apps/kstyle/themes/`

Expected install output:

Installed plugins to /opt/trinity/lib/trinity/
Installed theme file to /opt/trinity/share/apps/kstyle/themes/
Run: tdebuildsycoca


After installing, activate the style:

1. Rebuild TDE system configuration cache:
   tdebuildsycoca

2. Open Trinity Control Center → Appearance & Themes → Style
   You should see "Q4WIN10" in the Widget Style list using the "High Performance" label if described in themerc. Select it and click Apply.


Verify installation:

ls -l /opt/trinity/lib/trinity/plugins/styles/q4win10.so
ls -l /opt/trinity/lib/trinity/kstyle_q4win10_config.so

For a production build (with sstrip):
-rwxr-xr-x 1 root root 142K Feb 11 23:15 /opt/trinity/lib/trinity/plugins/styles/q4win10.so
-rwxr-xr-x 1 root root  37K Feb 11 23:15 /opt/trinity/lib/trinity/kstyle_q4win10_config.so


** Packaging

Build the binary first of course, then run the Packaging Script:

cd ~/src/tdebase-trinity-14.1.1/kstyles/Q4WIN10
chmod +x ./create_deb.sh
./create_deb.sh

Expected output:

```
Creating .deb package for tde-style-q4win10...
  Trinity version: 14.1.1
  Architecture: amd64
Copying binaries and assets...
Stripping binaries...
Creating control file...
Building package...
dpkg-deb: building package 'tde-style-q4win10' in 'tde-style-q4win10_2.0_tde14.1.1_amd64.deb'.
Success! Package created: tde-style-q4win10_2.0_tde14.1.1_amd64.deb
-rw-r--r-- 1 user user 55K Feb 11 23:20 tde-style-q4win10_2.0_tde14.1.1_amd64.deb
```

(warning about "unusual owner or group" is normal when building packages as a non-root user, this can be safely
ignored.)

The created package includes:
- **Style plugin**: `/opt/trinity/lib/trinity/plugins/styles/q4win10.so`
- **Config plugin**: `/opt/trinity/lib/trinity/kstyle_q4win10_config.so`
- **Theme file**: `/opt/trinity/share/apps/kstyle/themes/q4win10.themerc`
- **Package name**: `tde-style-q4win10`
- **Version**: 2.0 (or matching your version)
- **Architecture**: Automatically detected
- **Total size**: ~55KB (compressed)


To install the .deb package:

sudo dpkg -i tde-style-q4win10_2.0_tde14.1.1_amd64.deb


---

## Troubleshooting (some issues I experienced myself)


**** "tmoc: command not found" or "uic-tqt: command not found"

--> The TDE Meta Object Compiler and UI compiler are not installed or not in PATH. Install the TDE
development packages:

sudo apt install libtqt3-mt-dev tqt3-dev-tools

If tmoc is installed but at a non-standard location, edit the Makefile lines:

MOC := /path/to/tmoc
UIC := /path/to/uic-tqt


**** Linker error: "cannot find -ltdecore" or similar

--> The TDE core libraries are not installed. You need the tdebase development files:

sudo apt install tdebase-trinity-dev

Or verify the library exists:

ls /opt/trinity/lib/libtdecore*


**** "make: *** No rule to make target" errors

--> You're in the wrong directory. Make sure you're in the style source directory:

cd ~/src/tdebase-trinity-14.1.1/kstyles/Q4WIN10

The Makefile expects to find the tdebase structure, although for standalone builds it's less dependent on relative paths if TDE_PREFIX is set correctly, but sticking to the structure is safer.


**** Style does not appear in Control Center after installation

--> TDE uses `.themerc` files to discover styles. If the style doesn't appear:

1. Run `tdebuildsycoca` to refresh the system cache
2. Verify the `.themerc` file is installed: `ls /opt/trinity/share/apps/kstyle/themes/q4win10.themerc`
3. Verify permissions (readable by all users).


**** Build produces larger binaries than expected

--> sstrip is probably not installed. Install it:

git clone https://github.com/nicktehrany/sstrip
cd sstrip
make
sudo cp sstrip /usr/local/bin/

Then rebuild:

cd ~/src/tdebase-trinity-14.1.1/kstyles/Q4WIN10
make clean && make

Expected sizes with sstrip:
- Style: ~142KB
- Config: ~37KB

Without sstrip (standard strip only), binaries will be larger.


---

- Makefile configuration:

| Variable | Default | Description |
|----------|---------|-------------|
| `TDE_PREFIX` | `/opt/trinity` | TDE installation prefix |
| `TDE_INCLUDE` | `$(TDE_PREFIX)/include` | TDE headers location |
| `TDE_LIB` | `$(TDE_PREFIX)/lib` | TDE libraries location |
| `TQT_INCLUDE` | `/usr/include/tqt3` | TQt3 headers location |
| `PLUGIN_DIR` | `$(TDE_PREFIX)/lib/trinity/plugins/styles` | Style plugin installation directory |
| `THEME_DIR` | `$(TDE_PREFIX)/share/apps/kstyle/themes` | Theme file installation directory |

To customize paths, edit the top section of the `Makefile` before building.
