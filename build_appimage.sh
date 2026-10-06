#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
APPDIR="${BUILD_DIR}/AppDir"
CLEAN_PLUGINS="/tmp/qshut_qt6_clean_plugins"
WRAPPER_QMAKE="/tmp/qshut_qmake_wrapper.sh"

echo "==> [1/5] Compilazione con CMake..."
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr "${SCRIPT_DIR}"
make -j"$(nproc)"

echo "==> [2/5] Installazione in AppDir..."
rm -rf "${APPDIR}"
make install DESTDIR="${APPDIR}"

echo "==> [3/5] Preparazione plugin Qt puliti ed esclusione KDE..."
rm -rf "${CLEAN_PLUGINS}"
mkdir -p "${CLEAN_PLUGINS}"

# Symlink dei plugin Qt6 escludendo kimg_* di terze parti
for d in /usr/lib/qt6/plugins/*; do
    b=$(basename "$d")
    if [ "$b" == "imageformats" ]; then
        mkdir -p "${CLEAN_PLUGINS}/imageformats"
        for p in /usr/lib/qt6/plugins/imageformats/libq*.so; do
            ln -sf "$p" "${CLEAN_PLUGINS}/imageformats/"
        done
    else
        ln -sf "$d" "${CLEAN_PLUGINS}/$b"
    fi
done

cat <<'WRAPPER_EOF' > "${WRAPPER_QMAKE}"
#!/usr/bin/env bash
/usr/bin/qmake6 "$@" | sed "s|QT_INSTALL_PLUGINS:/usr/lib/qt6/plugins|QT_INSTALL_PLUGINS:/tmp/qshut_qt6_clean_plugins|"
WRAPPER_EOF
chmod +x "${WRAPPER_QMAKE}"

echo "==> [4/5] Esecuzione linuxdeploy..."
export NO_STRIP=true
export QMAKE="${WRAPPER_QMAKE}"

linuxdeploy \
  --appdir "${APPDIR}" \
  -e "${APPDIR}/usr/bin/qshutdown" \
  -d "${SCRIPT_DIR}/resources/qshutdown.desktop" \
  -i "${SCRIPT_DIR}/resources/qshutdown.png" \
  -i "${SCRIPT_DIR}/resources/qshutdown.svg"

# Esecuzione del plugin Qt
linuxdeploy-plugin-qt --appdir "${APPDIR}" -m wayland

# Includi supporto Wayland
cp -f /usr/lib/qt6/plugins/platforms/libqwayland*.so "${APPDIR}/usr/plugins/platforms/" 2>/dev/null || true
cp -rf /usr/lib/qt6/plugins/wayland* "${APPDIR}/usr/plugins/" 2>/dev/null || true
linuxdeploy --appdir "${APPDIR}" --deploy-deps-only="${APPDIR}/usr/plugins/platforms/libqwayland.so" >/dev/null 2>&1 || true

# AppRun script
cat <<'APPRUN_EOF' > "${APPDIR}/AppRun"
#!/usr/bin/env bash
SELF=$(readlink -f "$0")
HERE="${SELF%/*}"

export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${HERE}/usr/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="${HERE}/usr/plugins/platforms"
export XDG_DATA_DIRS="${HERE}/usr/share:${XDG_DATA_DIRS}"

if [ -d "${HERE}/apprun-hooks" ]; then
    for hook in "${HERE}/apprun-hooks"/*; do
        if [ -f "${hook}" ]; then
            source "${hook}"
        fi
    done
fi

exec "${HERE}/usr/bin/qshutdown" "$@"
APPRUN_EOF
chmod +x "${APPDIR}/AppRun"

# File root per AppImage
cp "${SCRIPT_DIR}/resources/qshutdown.png" "${APPDIR}/qshutdown.png"
cp "${SCRIPT_DIR}/resources/qshutdown.svg" "${APPDIR}/qshutdown.svg"
cp "${SCRIPT_DIR}/resources/qshutdown.desktop" "${APPDIR}/qshutdown.desktop"

echo "==> [5/5] Creazione AppImage con appimagetool..."
OUTPUT_APPIMAGE="${SCRIPT_DIR}/QShut-x86_64.AppImage"
appimagetool "${APPDIR}" "${OUTPUT_APPIMAGE}"

echo "==> Build AppImage completata con successo!"
ls -lh "${OUTPUT_APPIMAGE}"
