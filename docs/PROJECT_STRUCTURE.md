# ColorSmith Project Structure

This document describes the professional project structure and organization.

## Directory Layout

```
colorpicker/
├── CMakeLists.txt              # Main CMake build configuration
├── Makefile                    # Convenience wrapper for build commands
├── build.sh                    # Build automation script
├── .gitignore                  # Git ignore rules
├── README.md                   # Project overview and quick start
├── LICENSE                     # GPL-3.0 license
├── CHANGELOG.md                # Version history and changes
├── CONTRIBUTING.md             # Contribution guidelines
│
├── src/                        # Source files (.cpp)
│   ├── main.cpp               # Application entry point
│   ├── MainWindow.cpp         # Main window implementation
│   ├── ColorLogic.cpp         # Color conversion utilities
│   └── ScreenPicker.cpp       # Screen color picker (D-Bus)
│
├── include/colorpicker/        # Header files (.h)
│   ├── MainWindow.h           # Main window header
│   ├── ColorLogic.h           # Color logic header
│   ├── ScreenPicker.h         # Screen picker header
│   └── version.h              # Version definitions
│
├── ui/                         # Qt Designer UI files
│   └── MainWindow.ui          # Main window layout
│
├── resources/                  # Application resources
│   ├── colorpicker.desktop    # Desktop entry file
│   ├── colorpicker.appdata.xml # AppStream metadata
│   ├── icons/                 # Application icons
│   │   └── colorpicker.svg    # Main application icon
│   └── translations/          # Translation files (future)
│
├── cmake/                      # CMake modules and scripts
│   └── cmake_uninstall.cmake.in # Uninstall script template
│
├── docs/                       # Documentation
│   └── INSTALL.md             # Installation guide
│
└── debian/                     # Debian packaging files
    ├── control                # Package metadata
    ├── changelog              # Debian changelog
    ├── copyright              # Copyright information
    ├── rules                  # Build rules
    └── compat                 # Debhelper compatibility level
```

## Build System

### CMake (Primary)

Modern, cross-platform build system with:
- Automatic Qt MOC/UIC/RCC handling
- Dependency detection
- Installation targets
- Package generation (CPack)
- Uninstall support

### Makefile Wrapper

Convenience wrapper providing common targets:
- `make` / `make build` - Build the project
- `make debug` - Build in debug mode
- `make install` - Install the application
- `make clean` - Clean build artifacts
- `make run` - Build and run
- `make test` - Run tests
- `make package` - Create distribution packages

### Build Script

Flexible bash script (`build.sh`) with options:
- `-d, --debug` - Debug build
- `-c, --clean` - Clean build
- `-r, --run` - Run after build
- `-p, --prefix` - Custom install prefix
- `-i, --install` - Install after build
- `-t, --test` - Build and run tests

## Source Organization

### Header Files

Located in `include/colorpicker/` for:
- Clear public API separation
- Professional include structure
- Namespace organization
- Easy installation to system paths

Include pattern: `#include "colorpicker/ClassName.h"`

### Source Files

Located in `src/` for:
- Implementation details
- Clean separation from headers
- Standard Qt project layout

### UI Files

Located in `ui/` for:
- Separation from code
- Easy designer workflow
- Clear resource organization

## Linux Integration

### Desktop Entry

File: `resources/colorpicker.desktop`
- Application menu integration
- Icon association
- Categories and keywords
- MIME type handling

Install location: `/usr/share/applications/`

### AppStream Metadata

File: `resources/colorpicker.appdata.xml`
- Software center integration
- Rich application description
- Screenshots and releases
- Searchable metadata

Install location: `/usr/share/metainfo/`

### Icons

File: `resources/icons/colorpicker.svg`
- Scalable SVG format
- FreeDesktop icon theme integration

Install location: `/usr/share/icons/hicolor/scalable/apps/`

## Packaging

### Debian/Ubuntu (.deb)

Files in `debian/` directory:
- `control` - Package metadata and dependencies
- `changelog` - Version history in Debian format
- `copyright` - License information
- `rules` - Build and install rules
- `compat` - Debhelper compatibility level

Build: `cd build && cpack -G DEB`

### RPM (Fedora/RHEL)

Automatic generation via CPack
Build: `cd build && cpack -G RPM`

### Source Archives

Automatic generation via CPack
Build: `cd build && cpack -G TGZ`

## Version Management

Centralized in:
1. `CMakeLists.txt` - Project version
2. `include/colorpicker/version.h` - Version constants
3. `debian/changelog` - Debian version
4. `CHANGELOG.md` - User-facing changelog

## Documentation

### User Documentation
- `README.md` - Quick start and overview
- `docs/INSTALL.md` - Detailed installation guide

### Developer Documentation
- `CONTRIBUTING.md` - Development guidelines
- `CHANGELOG.md` - Version history
- Inline code comments

### Legal
- `LICENSE` - GPL-3.0 license text
- `debian/copyright` - Debian copyright file

## Build Artifacts

Created in `build/` directory (gitignored):
- Compiled binaries
- Generated files (MOC, UIC)
- CMake cache and files
- Installation manifest
- Package files

## Installation Paths

Default prefix: `/usr/local`

Standard paths (FHS compliant):
- Binary: `${prefix}/bin/colorpicker`
- Desktop file: `${prefix}/share/applications/`
- AppStream: `${prefix}/share/metainfo/`
- Icons: `${prefix}/share/icons/hicolor/scalable/apps/`
- Documentation: `${prefix}/share/doc/colorpicker/`

## Development Workflow

### Quick Development Build

```bash
./build.sh --debug --run
```

### Standard Build and Install

```bash
./build.sh --install
```

### Package Creation

```bash
mkdir build && cd build
cmake ..
make
cpack -G DEB  # or RPM
```

### Clean Rebuild

```bash
./build.sh --clean
# or
make clean && make
```

## Best Practices Implemented

1. **Separation of Concerns**
   - Headers, sources, UI, resources in separate directories
   
2. **Modern Build System**
   - CMake for flexibility
   - Makefile for convenience
   - Build script for automation

3. **Linux Integration**
   - Desktop entry for application menu
   - AppStream for software centers
   - Icon theming support
   - FHS-compliant installation

4. **Documentation**
   - README for users
   - CONTRIBUTING for developers
   - INSTALL for detailed setup
   - CHANGELOG for history

5. **Packaging**
   - Debian/Ubuntu support
   - RPM support
   - Source archives
   - Proper dependencies

6. **Version Control**
   - .gitignore for build artifacts
   - Professional commit structure
   - Semantic versioning

7. **Code Quality**
   - Consistent naming conventions
   - Clear project structure
   - Standard Qt patterns
   - Professional organization

## Future Enhancements

- [ ] Continuous Integration (CI/CD)
- [ ] Unit tests framework
- [ ] Internationalization (i18n)
- [ ] Flatpak/Snap packaging
- [ ] API documentation (Doxygen)
- [ ] Code coverage reports
- [ ] Static analysis integration
