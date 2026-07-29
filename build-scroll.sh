#!/bin/bash

echo "[*] Cloning the repository..."
echo ""

rm -rf scroll
git clone https://github.com/dawsers/scroll 2>%1 | tail -4

cd scroll
echo -n "You are currently installing scroll version : "
git rev-parse --short HEAD
echo ""
cd ..

echo "[*] Installing dependecies..."
echo ""

sudo pacman -S --noconfirm \
  meson ninja gcc pkgconf glslang \
  wayland wayland-protocols libdrm libinput \
  libxkbcommon pixman vulkan-headers vulkan-icd-loader \
  libxcb xcb-util xcb-util-wm lua sed xcb-util-image \
  xcb-util-errors xcb-util-renderutil xcb-util-cursor \
  libdisplay-info hwdata seatd quickshell \
  ffmpeg egl-wayland mesa libliftoff \
  lcms2 xorg-xwayland vulkan-devel wget \
  xorg-xwayland git wayvnc vulkan-swrast \
  vulkan-tools swaybg kitty fastfetch fakeroot \
  2>&1 | tail -3 

echo ""
read -p "[*] Would you like to apply wlroots patches for kgsl support?: " ltr
echo ""

if [ "$ltr" = "y" ] || [ "$ltr" = "Y" ]; then
  cd scroll/subprojects/wlroots

  for f in render/vulkan/vulkan.c render/vulkan/renderer.c render/vulkan/pass.c \
          include/render/vulkan.h render/wlr_renderer.c render/vulkan/pixel_format.c \
          render/vulkan/texture.c types/wlr_layer_shell_v1.c; do
    [ -f "$f.orig" ] || cp "$f" "$f.orig"
    cp "$f.orig" "$f"
  done

  cd ../../..

  echo "[*] Applying patches..."
  echo ""

  python3 apply_wlr_patches.py 2>%1 | tail -4
else
  echo "[*] Patches disabled, continuing..."
fi

cd scroll

meson setup build > /dev/null
sudo ninja -C build install 2>%1 | tail -3

echo ""
echo "[*] If this gives an error or doesn't install, make local signature check optional on /etc/pacman.conf."

cd ..

rm -rf mesa-for-android-container_26.2.0-devel-20260709_archlinux_arm64.tar
wget https://github.com/lfdevs/mesa-for-android-container/releases/download/mesa-26.2.0-devel-20260709/mesa-for-android-container_26.2.0-devel-20260709_archlinux_arm64.tar 2>%1 | tail -3

if [ -d "mesa" ]; then
  rm -rf mesa
  mkdir mesa
else
  mkdir mesa
fi

tar -xf mesa-for-android-*_archlinux_arm64.tar -C mesa
cd mesa

sudo pacman -U --noconfirm ./*pkg.tar.xz 2>%1 | tail -3

cd ..

cat <<EOF > start-scroll-vnc.sh 
#!/bin/bash
echo "[*] Creating XDG_RUNTIME_DIR and setting it's permissions..."
echo "" 
export MESA_LOADER_DRIVER_OVERRIDE=kgsl 
export TU_DEBUG=noconform 
export XDG_RUNTIME_DIR=/tmp/runtime-$UID 
mkdir -p "$XDG_RUNTIME_DIR" 
chmod 700 "$XDG_RUNTIME_DIR" 
echo "[*] Setting final env variables and starting scroll..."
echo ""
export WLR_BACKENDS=headless
export WLR_RENDERER=vulkan
export WLR_NO_HARDWARE_CURSORS=1
export WAYLAND_DISPLAY=wayland-1
dbus-run-session scroll > /dev/null 2>&1 &
echo "[*] Starting the vnc session..."
echo ""
sleep 2
wayvnc 127.0.0.1 5900 > /dev/null 2>&1 &
echo "Done!"
EOF

cat <<EOF > start-scroll-x11.sh
#!/bin/bash
echo "[*] Creating XDG_RUNTIME_DIR and setting it's permissions..."
echo ""
export MESA_LOADER_DRIVER_OVERRIDE=kgsl
export TU_DEBUG=noconform
export XDG_RUNTIME_DIR=/tmp/runtime-$UID
mkdir -p "$XDG_RUNTIME_DIR"
chmod 700 "$XDG_RUNTIME_DIR" 
echo "[*] Starting a termux-x11 session..."
echo ""
export DISPLAY=:0
termux-x11 :0 > /dev/null 2>&1 &
echo "[*] Setting final env variables and starting scroll..."
echo ""
export WLR_BACKENDS=x11
export WLR_RENDERER=vulkan
export WLR_NO_HARDWARE_CURSORS=1
unset WAYLAND_DISPLAY
sleep 2
dbus-run-session scroll > /dev/null 2>&1 &
echo "Done!"
EOF

chmod +x start-scroll-vnc.sh start-scroll-x11.sh

rm %1

echo ""
echo "Setup has finished!"
echo ""
echo "Run scroll on chroot with start-scroll-vnc.sh, on proot with start-scroll-x11.sh."

