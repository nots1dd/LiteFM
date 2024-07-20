# !/bin/bash

# Title
echo "===================="
echo "      LiteFM        "
echo "===================="

# Prompt for build type
read -p "Enter the type of build (cmake or make): " build_type

# Build commands
if [ "$build_type" == "cmake" ]; then
    cmake -S . -B build/
    cmake --build build/
elif [ "$build_type" == "make" ]; then
    make
else
    echo "Invalid build type. Please enter 'cmake' or 'make'."
    exit 1
fi

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

# Copy the man file and gzip it
sudo cp components/litefm.1 /usr/share/man/man1/
sudo gzip /usr/share/man/man1/litefm.1

# Create alias in the appropriate shell configuration file
SHELL_NAME=$(basename "$SHELL")
case "$SHELL_NAME" in
    bash)
        echo "alias lfm='$(pwd)/build/litefm'" >> ~/.bashrc
        ;;
    zsh)
        echo "alias lfm='$(pwd)/build/litefm'" >> ~/.zshrc
        ;;
    *)
        echo "Shell $SHELL_NAME not supported for alias creation. Please create the alias manually."
        ;;
esac

echo "Build completed and man page installed. Please restart your terminal or source your rc file to use the litefm command."
