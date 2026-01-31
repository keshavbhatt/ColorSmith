# Flatpak Manifest Files

This directory contains Flatpak manifest files for publishing ColorSmith on Flathub.

## Files

- **com.ktechpit.colorsmith.json** - JSON format manifest (recommended for Flathub)
- **com.ktechpit.colorsmith.yml** - YAML format manifest (alternative)
- **flatpak-build.sh** - Helper script to build and test the Flatpak locally

## Quick Start

### 1. Install Prerequisites

```bash
sudo apt install flatpak flatpak-builder
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.kde.Platform//6.10 org.kde.Sdk//6.10
```

### 2. Update the Manifest

Before building, update the commit hash in the manifest:

1. Create a git tag:
   ```bash
   git tag -a v1.0.0 -m "Release 1.0.0"
   git push origin v1.0.0
   ```

2. Get the commit hash:
   ```bash
   git rev-parse v1.0.0
   ```

3. Replace `PUT_YOUR_COMMIT_HASH_HERE` in the manifest with the actual hash.

### 3. Build and Test

Using the helper script:
```bash
./flatpak-build.sh test
```

Or manually:
```bash
flatpak-builder --user --install --force-clean build-dir com.ktechpit.colorsmith.json
flatpak run com.ktechpit.colorsmith
```

## Configuration

The manifest uses:
- **Runtime**: org.kde.Platform 6.10
- **SDK**: org.kde.Sdk 6.10
- **Build System**: cmake-ninja

### Permissions (finish-args)

The app requires these permissions:
- **X11/Wayland**: For GUI display
- **Session Bus**: For DBus communication
- **Portal Access**: For screen color picking via xdg-desktop-portal
- **Documents Access**: For saving/loading color palettes

## Publishing to Flathub

See the detailed guide in `docs/FLATPAK.md`.

Quick overview:
1. Test the build locally
2. Fork/create repository at `github.com/flathub/com.ktechpit.colorsmith`
3. Push the manifest file
4. Submit to Flathub following their guidelines

## Troubleshooting

### Build fails
- Ensure KDE runtime 6.10 is installed
- Check that the commit hash is correct
- Verify all source files are accessible

### App doesn't run
- Check permissions in `finish-args`
- Test with `flatpak run --verbose com.ktechpit.colorsmith`

### Color picker doesn't work
- Ensure portal permissions are granted
- Check if xdg-desktop-portal is running on your system

## Resources

- [Flathub Documentation](https://docs.flathub.org/)
- [Flatpak Documentation](https://docs.flatpak.org/)
