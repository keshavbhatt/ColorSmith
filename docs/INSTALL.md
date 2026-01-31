# Installation Instructions

This document provides detailed installation instructions for ColorSmith.

## Table of Contents

- [System Requirements](#system-requirements)
- [Building from Source](#building-from-source)
- [Installing Pre-built Packages](#installing-pre-built-packages)
- [Post-Installation](#post-installation)
- [Troubleshooting](#troubleshooting)

## System Requirements

### Minimum Requirements

- **OS**: Linux (any modern distribution)
- **Qt**: Version 6.0 or later
- **CMake**: Version 3.16 or later
- **Compiler**: GCC 7+ or Clang 5+ with C++17 support
- **RAM**: 512 MB
- **Disk Space**: 50 MB

### Runtime Dependencies

- Qt6 Core, Gui, Widgets, DBus modules
- XDG Desktop Portal (for screen color picking)
- D-Bus session bus

## Building from Source

### 1. Install Dependencies

#### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential cmake git
sudo apt install qt6-base-dev qt6-base-dev-tools libqt6dbus6
```

#### Fedora

```bash
sudo dnf install @development-tools cmake git
sudo dnf install qt6-qtbase-devel qt6-qtbase
```

#### Arch Linux

```bash
sudo pacman -S base-devel cmake git
sudo pacman -S qt6-base
```

#### openSUSE

```bash
sudo zypper install -t pattern devel_basis
sudo zypper install cmake git
sudo zypper install qt6-base-devel
```

### 2. Clone the Repository

```bash
git clone https://github.com/keshavbhatt/colorpicker.git
cd colorpicker
```

### 3. Build the Application

#### Quick Build

```bash
./build.sh
```

#### Manual Build

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

#### Build with Custom Options

```bash
# Debug build
./build.sh --debug

# Custom install prefix
./build.sh --prefix=/usr

# Clean build
./build.sh --clean
```

### 4. Install

```bash
cd build
sudo make install
```

Or use the build script:

```bash
./build.sh --install
```

### 5. Verify Installation

```bash
which colorpicker
colorpicker --version
```

## Installing Pre-built Packages

### Debian/Ubuntu (.deb)

```bash
# Download the package
wget https://github.com/keshavbhatt/colorpicker/releases/download/v1.0.0/colorpicker_1.0.0_amd64.deb

# Install
sudo dpkg -i colorpicker_1.0.0_amd64.deb

# Fix dependencies if needed
sudo apt-get install -f
```

### Fedora/RHEL (.rpm)

```bash
# Download the package
wget https://github.com/keshavbhatt/colorpicker/releases/download/v1.0.0/colorpicker-1.0.0.x86_64.rpm

# Install
sudo rpm -i colorpicker-1.0.0.x86_64.rpm

# Or use dnf
sudo dnf install colorpicker-1.0.0.x86_64.rpm
```

### Arch Linux (AUR)

```bash
# Using yay
yay -S colorpicker

# Using paru
paru -S colorpicker

# Manual installation
git clone https://aur.archlinux.org/colorpicker.git
cd colorpicker
makepkg -si
```

### Flatpak

```bash
flatpak install flathub com.ktechpit.colorpicker
```

### Snap

```bash
sudo snap install colorpicker
```

## Post-Installation

### Desktop Integration

The application should appear in your application menu under "Graphics" or "Utilities".

If not visible, update the desktop database:

```bash
sudo update-desktop-database
```

### File Associations

No file associations are required for this application.

### First Run

1. Launch ColorSmith from your application menu or run `colorsmith` in terminal
2. Grant screen capture permissions if prompted (required for color picking)
3. The application will start with a random color

## Uninstallation

### If Installed from Source

```bash
cd build
sudo make uninstall
```

Or use Make:

```bash
make uninstall
```

### If Installed from Package

#### Debian/Ubuntu

```bash
sudo apt remove colorsmith
```

#### Fedora/RHEL

```bash
sudo dnf remove colorsmith
```

#### Arch Linux

```bash
sudo pacman -R colorsmith
```

## Troubleshooting

### Color Picker Not Working

**Problem**: Color picker button doesn't work or shows an error.

**Solutions**:
1. Ensure XDG Desktop Portal is installed:
   ```bash
   # Ubuntu/Debian
   sudo apt install xdg-desktop-portal xdg-desktop-portal-gtk
   
   # Fedora
   sudo dnf install xdg-desktop-portal xdg-desktop-portal-gtk
   ```

2. Check if the portal is running:
   ```bash
   ps aux | grep portal
   ```

3. Restart your session or run:
   ```bash
   systemctl --user restart xdg-desktop-portal
   ```

### Qt Platform Plugin Error

**Problem**: `qt.qpa.plugin: Could not load the Qt platform plugin`

**Solution**: Install Qt platform plugins:
```bash
# Ubuntu/Debian
sudo apt install qt6-qpa-plugins

# Fedora
sudo dnf install qt6-qtbase-gui
```

### Build Errors

**Problem**: CMake cannot find Qt6

**Solution**: Set Qt6 path manually:
```bash
export Qt6_DIR=/usr/lib/cmake/Qt6
cmake ..
```

**Problem**: Missing Qt modules

**Solution**: Install all required Qt modules:
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-base-dev-tools

# Fedora
sudo dnf install qt6-qtbase-devel
```

### Application Doesn't Start

**Problem**: Application crashes on startup

**Solutions**:
1. Run from terminal to see error messages:
   ```bash
   colorsmith
   ```

2. Check dependencies:
   ```bash
   ldd $(which colorsmith)
   ```

3. Remove config file and try again:
   ```bash
   rm ~/.config/ktechpit/ColorSmith.conf
   ```

### Permission Issues

**Problem**: Cannot install or permission denied errors

**Solution**: Ensure you're using `sudo` for system-wide installation:
```bash
sudo make install
```

For user-only installation:
```bash
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
make install
```

## Getting Help

If you encounter issues not covered here:

1. Check the [GitHub Issues](https://github.com/keshavbhatt/colorpicker/issues)
2. Read the [FAQ](docs/FAQ.md)
3. Join our community chat
4. Create a new issue with detailed information

## Additional Resources

- [User Guide](docs/USER_GUIDE.md)
- [Contributing Guide](CONTRIBUTING.md)
- [Changelog](CHANGELOG.md)
- [License](LICENSE)
