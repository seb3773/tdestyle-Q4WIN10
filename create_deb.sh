#!/bin/bash

# Configuration
PACKAGE_NAME="tdestyle-q4win10"
VERSION="2.0"
MAINTAINER="seb3773 <https://github.com/seb3773>"
DESCRIPTION="Q4WIN10 : a Windows10 like style for Trinity desktop"
ARCH=$(dpkg --print-architecture)

# Directories
SOURCE_DIR=$(pwd)
BUILD_DIR="../../build/kstyles/q4win10"
PKG_DIR="package_build"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}Creating .deb package for $PACKAGE_NAME...${NC}"

# Check if build files exist
if [ ! -f "$BUILD_DIR/tde_style_q4win10.so" ]; then
    echo -e "${RED}Error: Compiled file tde_style_q4win10.so not found in $BUILD_DIR${NC}"
    echo "Please compile the project first."
    exit 1
fi

# Clean previous build
rm -rf $PKG_DIR
rm -f ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb

# Create directory structure
mkdir -p $PKG_DIR/DEBIAN
mkdir -p $PKG_DIR/opt/trinity/lib/trinity/plugins/styles
mkdir -p $PKG_DIR/opt/trinity/share/apps/tdestyle/themes

# Copy files
echo "Copying files..."
cp "$BUILD_DIR/tde_style_q4win10.so" "$PKG_DIR/opt/trinity/lib/trinity/plugins/styles/"
cp "$BUILD_DIR/tde_style_q4win10.la" "$PKG_DIR/opt/trinity/lib/trinity/plugins/styles/"
cp "q4win10.themerc" "$PKG_DIR/opt/trinity/share/apps/tdestyle/themes/"

# Create control file
echo "Creating control file..."
cat > $PKG_DIR/DEBIAN/control << EOF
Package: $PACKAGE_NAME
Version: $VERSION
Section: x11
Priority: optional
Architecture: $ARCH
Depends: tdebase-trinity, libtqt3-mt
Maintainer: $MAINTAINER
Description: $DESCRIPTION
 A flat, modern Windows 10 inspired widget style for the Trinity Desktop Environment (TDE).
 optimized for performance and aesthetics.
EOF

# Build package
echo "Building package..."
dpkg-deb --build $PKG_DIR "${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"

# Cleanup
rm -rf $PKG_DIR

if [ -f "${PACKAGE_NAME}_${VERSION}_${ARCH}.deb" ]; then
    echo -e "${GREEN}Success! Package created: ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb${NC}"
    ls -lh "${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"
else
    echo -e "${RED}Error: Package creation failed.${NC}"
    exit 1
fi
