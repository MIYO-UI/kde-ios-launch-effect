# Installation Instructions

This document provides detailed instructions for building and installing the iOS-Style Launch Animation Effect for KWin.

## Prerequisites

### Fedora

```bash
sudo dnf install cmake extra-cmake-modules gcc-c++ \
  kf5-kconfig-devel kf5-kconfigwidgets-devel kf5-ki18n-devel \
  kf5-kglobalaccel-devel kf5-kxmlgui-devel kwin-devel
```

### Ubuntu/Debian

```bash
sudo apt install build-essential cmake extra-cmake-modules gettext \
  libkf5config-dev libkf5configwidgets-dev libkf5i18n-dev \
  libkf5globalaccel-dev libkf5xmlgui-dev kwin-dev
```

### Arch Linux

```bash
sudo pacman -S cmake extra-cmake-modules kwidgetsaddons kconfig kconfigwidgets \
  ki18n kglobalaccel kxmlgui kwin
```

### openSUSE

```bash
sudo zypper install cmake extra-cmake-modules libKF5Config-devel \
  libKF5ConfigWidgets-devel libKF5I18n-devel libKF5GlobalAccel-devel \
  libKF5XmlGui-devel kwin-devel
```

## Building from Source

1. Clone or extract the source code:
   ```bash
   git clone https://github.com/yourusername/kwin-ioslauncheffect.git
   cd kwin-ioslauncheffect
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure the build:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

4. Build the effect:
   ```bash
   make
   ```

5. Install (requires root privileges):
   ```bash
   sudo make install
   ```

6. Update the running KWin instance:
   ```bash
   # If using X11
   kwin_x11 --replace &
   
   # If using Wayland, you need to log out and log back in
   ```

## Testing the Effect

To test if the effect is working:

1. Open System Settings
2. Navigate to Workspace Behavior → Desktop Effects
3. Find "iOS-Style Launch Animation" in the list and ensure it's enabled
4. Launch a new application and observe if it grows from the click point

## Manual Uninstallation

If you need to remove the effect:

1. From the build directory:
   ```bash
   sudo make uninstall
   ```

2. Or manually remove the installed files:
   ```bash
   sudo rm /usr/lib64/qt5/plugins/kwin/effects/plugins/kwin_ioslauncheffect.so
   sudo rm /usr/lib64/qt5/plugins/kwin/effects/configs/kwin_ioslauncheffect_config.so
   sudo rm /usr/share/kwin/effects/ioslauncheffect.desktop
   ```

   Note: Paths may vary depending on your distribution and installation prefix.

## Troubleshooting

### Effect not showing up in System Settings

- Make sure all dependencies were correctly installed
- Check if the .desktop file was properly installed
- Look for any error messages during installation

### Effect not working

- Run `qdbus org.kde.KWin /KWin supportInformation` to check if the effect is loaded
- Check `~/.xsession-errors` for any relevant error messages
- Make sure the effect is enabled in System Settings

### Build errors

- Ensure you have all the required development packages installed
- Try using a newer version of KDE Frameworks if available
- Make sure your KWin version is compatible with the effect