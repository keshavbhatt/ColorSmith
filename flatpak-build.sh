#!/bin/bash

# Flatpak Build and Test Script for ColorSmith
# This script helps build and test the Flatpak package locally

set -e

APP_ID="com.ktechpit.colorsmith"
MANIFEST="com.ktechpit.colorsmith.yml"
BUILD_DIR="flatpak-build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if flatpak is installed
if ! command -v flatpak &> /dev/null; then
    print_error "flatpak is not installed"
    echo "Install it with: sudo apt install flatpak"
    exit 1
fi

# Check if flatpak-builder is installed
if ! command -v org.flatpak.Builder &> /dev/null; then
    print_error "flatpak-builder is not installed"
    print_info "Installing flatpak-builder..."
    flatpak install flathub org.flatpak.Builder -y
fi

# Check if runtime is installed
print_info "Checking if KDE runtime is installed..."
if ! flatpak list | grep -q "org.kde.Platform.*6.10"; then
    print_warning "KDE Platform 6.10 not found. Installing..."
    flatpak install flathub org.kde.Platform//6.10 org.kde.Sdk//6.10 -y
else
    print_info "KDE Platform 6.10 is installed"
fi

# Parse command line arguments
COMMAND=${1:-build}

case $COMMAND in
    build)
        print_info "Building Flatpak for $APP_ID..."
        flatpak run org.flatpak.Builder --user --install --force-clean $BUILD_DIR $MANIFEST
        print_info "Build complete!"
        print_info "Run with: flatpak run $APP_ID"
        ;;

    run)
        print_info "Running $APP_ID..."
        flatpak run $APP_ID
        ;;

    test)
        print_info "Building and testing $APP_ID..."
        flatpak run org.flatpak.Builder --user --install --force-clean $BUILD_DIR $MANIFEST
        print_info "Build complete! Launching application..."
        flatpak run $APP_ID
        ;;

    clean)
        print_info "Cleaning build directory..."
        rm -rf $BUILD_DIR .flatpak-builder
        print_info "Clean complete!"
        ;;

    uninstall)
        print_info "Uninstalling $APP_ID..."
        flatpak uninstall --user $APP_ID -y || true
        print_info "Uninstall complete!"
        ;;

    validate)
        print_info "Validating manifest..."
        if flatpak list | grep -q "org.flatpak.Builder"; then
            flatpak run --command=flatpak-builder-lint org.flatpak.Builder manifest $MANIFEST
        else
            print_warning "org.flatpak.Builder not found"
            print_info "Install with: flatpak install flathub org.flatpak.Builder"
        fi
        ;;

    rebuild)
        print_info "Cleaning and rebuilding..."
        rm -rf $BUILD_DIR .flatpak-builder
        flatpak run org.flatpak.Builder --user --install --force-clean $BUILD_DIR $MANIFEST
        print_info "Rebuild complete!"
        ;;

    help|--help|-h)
        echo "ColorSmith Flatpak Build Script"
        echo ""
        echo "Usage: $0 [command]"
        echo ""
        echo "Commands:"
        echo "  build       Build and install the Flatpak (default)"
        echo "  run         Run the installed Flatpak"
        echo "  test        Build and immediately run the Flatpak"
        echo "  clean       Remove build directories"
        echo "  uninstall   Uninstall the Flatpak"
        echo "  validate    Validate the manifest file"
        echo "  rebuild     Clean and rebuild from scratch"
        echo "  help        Show this help message"
        echo ""
        echo "Examples:"
        echo "  $0 build    # Build the Flatpak"
        echo "  $0 test     # Build and test"
        echo "  $0 run      # Run the installed app"
        ;;

    *)
        print_error "Unknown command: $COMMAND"
        echo "Run '$0 help' for usage information"
        exit 1
        ;;
esac
