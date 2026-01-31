# Makefile wrapper for CMake-based ColorPicker project
# This provides convenient targets for common operations

.PHONY: all build clean install uninstall run test help

# Default target
all: build

# Build the project
build:
	@./build.sh

# Build in debug mode
debug:
	@./build.sh --debug

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf build/

# Build and install
install:
	@./build.sh --install

# Uninstall
uninstall:
	@if [ -d build ]; then \
		cd build && sudo make uninstall; \
	else \
		echo "Build directory not found. Nothing to uninstall."; \
	fi

# Run the application
run:
	@./build.sh --run

# Build and run tests
test:
	@./build.sh --test

# Create distribution packages
package:
	@echo "Creating packages..."
	@mkdir -p build
	@cd build && cmake .. && make package

# Create Debian package
deb: package
	@echo "Debian package created in build/"

# Create RPM package
rpm: package
	@echo "RPM package created in build/"

# Format code (requires clang-format)
format:
	@echo "Formatting code..."
	@find src include -name '*.cpp' -o -name '*.h' | xargs clang-format -i

# Show help
help:
	@echo "ColorSmith Makefile Targets:"
	@echo ""
	@echo "  make              - Build the project (default)"
	@echo "  make build        - Build the project"
	@echo "  make debug        - Build in debug mode"
	@echo "  make clean        - Clean build artifacts"
	@echo "  make install      - Build and install"
	@echo "  make uninstall    - Uninstall the application"
	@echo "  make run          - Build and run"
	@echo "  make test         - Build and run tests"
	@echo "  make package      - Create distribution packages"
	@echo "  make deb          - Create Debian package"
	@echo "  make rpm          - Create RPM package"
	@echo "  make format       - Format code with clang-format"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "For more options, run: ./build.sh --help"
