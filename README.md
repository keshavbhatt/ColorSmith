# ColorSmith

A modern, elegant color picker application built with Qt for Linux.

## Features

### Color Picking & Selection
- 🎨 **Screen Color Picker**: Pick colors from anywhere on your screen using native portal integration
- 🎨 **HSV Color Plane**: Interactive 2D color plane for precise color selection
- 🔆 **Brightness Slider**: Adjust color brightness with dedicated slider control
- 🎲 **Random Color Generator**: Generate random colors for inspiration
- 📐 **RGBA Support**: Full support for alpha channel transparency

### Color Formats & Conversion
- 🔄 **Format Conversion**: Convert between HEX, RGB, and RGBA color formats
- 📋 **Clipboard Support**: Copy color values with one click

### Gradient Maker
- 🌈 **Linear & Radial Gradients**: Create beautiful linear and radial gradients
- 🎨 **Multi-Stop Gradients**: Add, remove, and adjust unlimited gradient stops
- 🔄 **Gradient Rotation**: Control gradient angle (0-360°)
- 🎲 **Random Gradients**: Generate random gradient combinations
- 📤 **CSS Export**: Copy gradient as CSS code for web development
- 📄 **SVG Export**: Copy or export gradients as SVG files
- 🖼️ **Image Export**: Export gradients as PNG images with custom dimensions
- 👁️ **Fullscreen Preview**: View gradients in fullscreen mode

### Palette Management
- 🎨 **Color Palettes**: Create and manage multiple color palettes
- ➕ **Quick Add**: Add current color to palette with one click
- 🖼️ **Image Color Extraction**: Generate palettes from images using K-means clustering
- 💾 **Import/Export**: Import and export palettes in JSON format
- ✏️ **Palette Operations**: Create, rename, delete, and clear palettes
- 🔖 **Named Colors**: Add optional names to palette colors
- 📚 **Multiple Palettes**: Switch between different palettes easily

### User Interface
- 🎚️ **Precision Controls**: Adjust RGB values using sliders and spin boxes
- ⌨️ **Keyboard Shortcuts**: Comprehensive keyboard navigation and shortcuts
- 💾 **Settings Persistence**: Remembers your last used color and window geometry
- 🎨 **Intuitive Design**: Clean, modern Qt-based interface

## Screenshots

| Main Window | Gradient Generator | Radial Gradient |
|-------------|-------------------|-----------------|
| ![ColorSmith Main Window](https://github.com/keshavbhatt/colorsmith/blob/main/images/1.png?raw=true) | ![Gradient generator](https://github.com/keshavbhatt/colorsmith/blob/main/images/2.png?raw=true) | ![Gradient generator radial](https://github.com/keshavbhatt/colorsmith/blob/main/images/3.png?raw=true) |

## Building from Source

### Prerequisites

- CMake 3.16 or later
- Qt 6.x (Core, Gui, Widgets, DBus)
- C++17 compatible compiler (GCC, Clang)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/keshavbhatt/colorsmith.git
cd colorsmith

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make

# Install (optional)
sudo make install
```

### Build Options

```bash
# Build with custom install prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr ..

# Build in Debug mode
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build with tests
cmake -DBUILD_TESTING=ON ..
```

## Installation

### Snap Store

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/colorsmith)

```bash
sudo snap install colorsmith
```

### From Source

After building, install with:

```bash
cd build
sudo make install
```
## Usage

Launch the application from your application menu or run:

```bash
colorsmith
```

### Keyboard Shortcuts

#### General Actions
- `Ctrl+C`: Copy current color to clipboard
- `Ctrl+P`: Pick color from screen

#### RGB(A) Spinbox Inputs
When the cursor is inside any R / G / B / A field:

| Shortcut | Behavior |
|----------|----------|
| `↑` / `↓` | ±1 |
| `Shift` + `↑` / `↓` | ±10 |
| `Alt` + `↑` / `↓` | ±5 |
| `PageUp` / `PageDown` | ±25 |
| `Home` / `End` | Min (0) / Max (255) |

## Development

### Code Style

This project follows standard Qt/C++ coding conventions:
- Use camelCase for methods and variables
- Use PascalCase for class names
- Use `m_` prefix for member variables
- Keep headers in `include/` directory
- Keep implementations in `src/` directory

### Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the GNU General Public License v3.0 or later. See the LICENSE file for details.

## Support

- Report bugs: https://github.com/keshavbhatt/colorsmith/issues

## Donation

If you find ColorSmith useful, consider supporting its development!

<div align="center">

### Donate via PayPal
[![Donate with PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.com/ncp/payment/KS3TNDR4XLUE2)

### Or scan the QR code
<img src="https://github.com/keshavbhatt/colorsmith/blob/main/images/ColorSmith%20Don-qrcode.png?raw=true" alt="Donation QR Code" width="250"/>

</div>

Your support helps maintain and improve ColorSmith. Thank you! 💙

