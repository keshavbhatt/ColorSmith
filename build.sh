#!/bin/bash

# ColorSmith Build Script
# This script provides an easy way to build the application

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
INSTALL_PREFIX="/usr/local"
CLEAN_BUILD=false
RUN_AFTER_BUILD=false

# Print usage
print_usage() {
    cat << EOF
ColorSmith Build Script

Usage: $0 [options]

Options:
    -h, --help              Show this help message
    -d, --debug             Build in Debug mode (default: Release)
    -c, --clean             Clean build directory before building
    -r, --run               Run the application after building
    -p, --prefix PATH       Installation prefix (default: /usr/local)
    -b, --build-dir DIR     Build directory (default: build)
    -i, --install           Install after building
    -t, --test              Build and run tests

Examples:
    $0                      # Build in Release mode
    $0 -d -r                # Build in Debug mode and run
    $0 -c -i                # Clean build and install
    $0 --prefix=/usr        # Build with /usr prefix

EOF
}

# Parse arguments
INSTALL=false
RUN_TESTS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_usage
            exit 0
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -r|--run)
            RUN_AFTER_BUILD=true
            shift
            ;;
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -t|--test)
            RUN_TESTS=true
            shift
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Print configuration
echo -e "${GREEN}=== ColorSmith Build Configuration ===${NC}"
echo "Build Type:       $BUILD_TYPE"
echo "Build Directory:  $BUILD_DIR"
echo "Install Prefix:   $INSTALL_PREFIX"
echo "Clean Build:      $CLEAN_BUILD"
echo "Run Tests:        $RUN_TESTS"
echo ""

# Clean if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo -e "${GREEN}Configuring with CMake...${NC}"
CMAKE_ARGS=(
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"4
)

if [ "$RUN_TESTS" = true ]; then
    CMAKE_ARGS+=(-DBUILD_TESTING=ON)
fi

cmake "${CMAKE_ARGS[@]}" ..

# Build
echo -e "${GREEN}Building...${NC}"
make -j$(nproc)

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    echo -e "${GREEN}Running tests...${NC}"
    ctest --output-on-failure
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    echo -e "${GREEN}Installing...${NC}"
    sudo make install
fi

echo -e "${GREEN}Build completed successfully!${NC}"

# Run if requested
if [ "$RUN_AFTER_BUILD" = true ]; then
    echo -e "${GREEN}Running ColorSmith...${NC}"
    ./colorsmith
fi
