# ColorSmith

A modern, elegant color picker application built with Qt for Linux.

## Features

- 🎨 **Screen Color Picker**: Pick colors from anywhere on your screen using native portal integration
- 🎲 **Random Colors**: Generate random colors for inspiration
- 🔄 **Format Conversion**: Convert between HEX and RGB color formats
- 📋 **Clipboard Support**: Copy color values with one click
- 🎚️ **Precision Controls**: Adjust RGB values using sliders and spin boxes
- 💾 **Settings Persistence**: Remembers your last used color

## Screenshots

![ColorSmith Main Window](https://github.com/keshavbhatt/colorsmith/blob/main/images/1.png?raw=true)

![Gradient generator](https://github.com/keshavbhatt/colorsmith/blob/main/images/2.png?raw=true)

![Gradient generator radial](https://github.com/keshavbhatt/colorsmith/blob/main/images/3.png?raw=true)

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
 
