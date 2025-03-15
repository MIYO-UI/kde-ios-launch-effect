# iOS-Style Launch Animation for KWin

This KWin effect implements an iOS-style animation for window launches in KDE Plasma. When a new window opens, it will grow smoothly from the point where the user clicked, creating a more intuitive and visually appealing experience.

## Features

- Animates new windows to scale up from the mouse click position
- Configurable animation duration
- Optional closing animation (windows shrink back to a point)
- Smooth scaling and fade-in effects
- Full KDE integration with configuration UI

## Installation

### Dependencies

To build this effect, you need:

- KDE Frameworks 5 development packages
- KWin development headers
- CMake and a C++ compiler

On Fedora, install the dependencies with:

```bash
sudo dnf install cmake extra-cmake-modules kf5-kconfig-devel kf5-kconfigwidgets-devel \
  kf5-ki18n-devel kf5-kglobalaccel-devel kf5-kxmlgui-devel kwin-devel
```

On Ubuntu/Debian, install the dependencies with:

```bash
sudo apt install cmake extra-cmake-modules libkf5config-dev libkf5configwidgets-dev \
  libkf5i18n-dev libkf5globalaccel-dev libkf5xmlgui-dev kwin-dev
```

### Building and Installing

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/kwin-ioslauncheffect.git
   cd kwin-ioslauncheffect
   ```

2. Create a build directory and build the effect:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. Install the effect (requires root privileges):
   ```bash
   sudo make install
   ```

4. Restart KWin to load the new effect:
   ```bash
   # If using X11
   kwin_x11 --replace &
   
   # If using Wayland, log out and back in
   ```

## Configuration

After installation, the effect can be configured through the KDE System Settings:

1. Open **System Settings**
2. Go to **Workspace Behavior** → **Desktop Effects**
3. Find **iOS-Style Launch Animation** in the list
4. Use the configure button to adjust animation duration and other settings

## How It Works

The effect tracks mouse clicks and uses them as the origin point for new window animations. When a window is opened, it starts small at the click point and grows to its full size with a smooth animation. The effect also applies a fade-in animation to make the transition more pleasant.

## Troubleshooting

If the effect doesn't work after installation:

- Make sure it's enabled in System Settings
- Try restarting your KDE Plasma session
- Check the KWin error log: `~/.xsession-errors`

## License

This project is licensed under the GNU General Public License v3 (GPL-3.0).