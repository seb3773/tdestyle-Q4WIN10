#!/usr/bin/env bash
set -euo pipefail

# ==============================================================================
# Script de création du paquet Debian universel pour le style Q4WIN10
# Compatible avec toutes les versions de Trinity Desktop R14.1.x (R14.1.0 - R14.1.6+)
# ==============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PACKAGE_NAME="tde-win-style-q4win10"
PACKAGE_VERSION="2.0.3"
ARCH="$(dpkg --print-architecture 2>/dev/null || echo 'amd64')"
BUILD_DIR="${SCRIPT_DIR}/package_build"
BUILD_TIMESTAMP="$(date +'%Y%m%d.%H%M%S')"

DEB_NAME="${PACKAGE_NAME}_${PACKAGE_VERSION}_${ARCH}.deb"

echo "=================================================="
echo " Création du paquet Debian : ${DEB_NAME}"
echo " Version : ${PACKAGE_VERSION} | Build : ${BUILD_TIMESTAMP}"
echo " Architecture : ${ARCH}"
echo "=================================================="

# 1. Mise à jour du header de version avec le timestamp actuel
echo "[1/4] Mise à jour du header de version et compilation..."
cat << VERSION_EOF > "${SCRIPT_DIR}/q4win10style_version.h"
#ifndef Q4WIN10STYLE_VERSION_H
#define Q4WIN10STYLE_VERSION_H

#define Q4WIN10STYLE_VERSION "${PACKAGE_VERSION}"
#define Q4WIN10STYLE_BUILD_TIMESTAMP "${BUILD_TIMESTAMP}"
#define Q4WIN10STYLE_VERSION_STRING "Version ${PACKAGE_VERSION} (Build ${BUILD_TIMESTAMP})"

#endif // Q4WIN10STYLE_VERSION_H
VERSION_EOF

# 2. Compilation des binaires
python3 "${SCRIPT_DIR}/convert_images.py"
make -C "${SCRIPT_DIR}" clean
make -C "${SCRIPT_DIR}" -j"$(nproc)"

# 3. Préparation de l'arborescence
echo "[2/4] Préparation de l'arborescence du paquet..."
rm -rf "$BUILD_DIR"
mkdir -p "${BUILD_DIR}/DEBIAN"
mkdir -p "${BUILD_DIR}/opt/trinity/lib/trinity/plugins/styles"
mkdir -p "${BUILD_DIR}/opt/trinity/lib/trinity"
mkdir -p "${BUILD_DIR}/opt/trinity/share/apps/tdestyle/themes"
mkdir -p "${BUILD_DIR}/opt/trinity/share/apps/kstyle/themes"

# Copie des fichiers compilés (noms directs sans liens symboliques inutiles)
cp -a "${SCRIPT_DIR}/q4win10.so" "${BUILD_DIR}/opt/trinity/lib/trinity/plugins/styles/"
cp -a "${SCRIPT_DIR}/config/tdestyle_q4win10_config.so" "${BUILD_DIR}/opt/trinity/lib/trinity/"
if [ -f "${SCRIPT_DIR}/config/tdestyle_q4win10_config.la" ]; then
    cp -a "${SCRIPT_DIR}/config/tdestyle_q4win10_config.la" "${BUILD_DIR}/opt/trinity/lib/trinity/"
fi

cp -a "${SCRIPT_DIR}/q4win10.themerc" "${BUILD_DIR}/opt/trinity/share/apps/tdestyle/themes/"
cp -a "${SCRIPT_DIR}/q4win10.themerc" "${BUILD_DIR}/opt/trinity/share/apps/kstyle/themes/"

# 4. Fichier control & scripts de maintenance
echo "[3/4] Génération des métadonnées DEBIAN..."
INSTALLED_SIZE=$(du -sk "${BUILD_DIR}/opt" | awk '{print $1}')

cat << CONTROL_EOF > "${BUILD_DIR}/DEBIAN/control"
Package: ${PACKAGE_NAME}
Version: ${PACKAGE_VERSION}
Section: x11
Priority: optional
Architecture: ${ARCH}
Installed-Size: ${INSTALLED_SIZE}
Maintainer: seb3773 <https://github.com/seb3773>
Depends: libtqt3-mt, tdelibs14-trinity
Replaces: tdestyle-q4win10, tde-style-q4win10
Conflicts: tdestyle-q4win10, tde-style-q4win10
Provides: tde-win-style-q4win10
Description: Q4WIN10 Widget Style for Trinity Desktop
 Modern flat Windows 10 & 11 inspired widget style plugin for TDE.
 Features native Windows 11 mode, dark mode support, and seamless X11
 menubar integration.
 Fully decoupled standalone build for all Trinity Desktop R14.1.x versions.
 Author: seb3773 (https://github.com/seb3773)
CONTROL_EOF

# Script postinst
cat << 'POSTINST_EOF' > "${BUILD_DIR}/DEBIAN/postinst"
#!/bin/sh
set -e
# Configuration automatique du dépôt APT pour les futures mises à jour
if [ -d /etc/apt/sources.list.d ]; then
    cat << 'REPEOF' > /etc/apt/sources.list.d/tde-win-style-q4win10.list
# tde-win-style-q4win10 APT Repository
deb [trusted=yes] https://seb3773.github.io/tdestyle-Q4WIN10/ stable main
REPEOF
fi
if [ -x /opt/trinity/bin/tdebuildsycoca ]; then
    /opt/trinity/bin/tdebuildsycoca >/dev/null 2>&1 || true
elif command -v tdebuildsycoca >/dev/null 2>&1; then
    tdebuildsycoca >/dev/null 2>&1 || true
fi
exit 0
POSTINST_EOF
chmod 755 "${BUILD_DIR}/DEBIAN/postinst"

# Script postrm
cat << 'POSTRM_EOF' > "${BUILD_DIR}/DEBIAN/postrm"
#!/bin/sh
set -e
if [ "$1" = "purge" ] || [ "$1" = "remove" ]; then
    rm -f /etc/apt/sources.list.d/tde-win-style-q4win10.list
fi
if [ -x /opt/trinity/bin/tdebuildsycoca ]; then
    /opt/trinity/bin/tdebuildsycoca >/dev/null 2>&1 || true
elif command -v tdebuildsycoca >/dev/null 2>&1; then
    tdebuildsycoca >/dev/null 2>&1 || true
fi
exit 0
POSTRM_EOF
chmod 755 "${BUILD_DIR}/DEBIAN/postrm"

# 5. Construction du paquet .deb
echo "[4/4] Construction de l'archive .deb..."
dpkg-deb --build -Zxz "${BUILD_DIR}" "${SCRIPT_DIR}/${DEB_NAME}"

echo "=================================================="
echo " Paquet créé avec succès : ${SCRIPT_DIR}/${DEB_NAME}"
ls -lh "${SCRIPT_DIR}/${DEB_NAME}"
echo "=================================================="

# Option --install
if [ "${1:-}" = "--install" ] || [ "${1:-}" = "-i" ]; then
    echo "Installation du paquet..."
    sudo dpkg -i "${SCRIPT_DIR}/${DEB_NAME}"
    echo "Mise à jour du cache Sycoca..."
    /opt/trinity/bin/tdebuildsycoca || true
fi
