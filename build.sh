#!/bin/bash

# Define colors
GREEN="\e[32m"
RED="\e[31m"
PINK="\e[35m"
RESET="\e[0m"

# Function to detect the package manager and distribution
detect_distro() {
    if command -v dpkg-query &> /dev/null; then
        echo "debian"
    elif command -v rpm &> /dev/null; then
        echo "rpm"
    elif command -v pacman &> /dev/null; then
        echo "arch"
    else
        echo "unsupported"
    fi
}

# Function to check if a package is installed
check_package_installed() {
    local distro="$1"
    local package="$2"
    
    if [ "$distro" == "debian" ]; then
        dpkg-query -W -f='${Status}' "$package" 2>/dev/null | grep -c "ok installed"
    elif [ "$distro" == "rpm" ]; then
        rpm -q "$package" &> /dev/null
    elif [ "$distro" == "arch" ]; then
        pacman -Q "$package" &> /dev/null
    else
        echo "Unsupported package manager."
        return 1
    fi
}

# Function to install packages based on the distribution
install_packages() {
    local distro="$1"
    shift
    local packages=("$@")
    
    echo -e "${PINK}Installing missing packages: ${packages[*]}${RESET}"
    if [ "$distro" == "debian" ]; then
        sudo apt-get update
        sudo apt-get install -y "${packages[@]}"
    elif [ "$distro" == "rpm" ]; then
        sudo yum install -y "${packages[@]}"
    elif [ "$distro" == "arch" ]; then
        sudo pacman -Syu --needed "${packages[@]}"
    else
        echo -e "${RED}Unsupported distribution.${RESET}"
        exit 1
    fi
}

# Title
echo -e "${GREEN}===================="
echo -e "      LiteFM        "
echo -e "====================${RESET}"

# Detect the distribution
distro=$(detect_distro)
if [ "$distro" == "unsupported" ]; then
    echo -e "${RED}Unsupported Linux distribution.${RESET}"
    exit 1
fi

echo -e "${PINK}Building for $distro...${RESET}"

# Define the required packages based on the distribution
if [ "$distro" == "debian" ]; then
    required_packages=("libncurses-dev" "cmake" "make" "libarchive-dev")
elif [ "$distro" == "rpm" ]; then
    required_packages=("ncurses" "cmake" "make" "libarchive")
elif [ "$distro" == "arch" ]; then
    required_packages=("ncurses" "cmake" "make" "libarchive")
fi

# Check for required packages
missing_packages=()

for package in "${required_packages[@]}"; do
    if check_package_installed "$distro" "$package"; then
        echo -e "${GREEN}Package $package is already installed.${RESET}"
    else
        echo -e "${RED}Package $package is missing.${RESET}"
        missing_packages+=("$package")
    fi
done

if [ ${#missing_packages[@]} -ne 0 ]; then
    install_packages "$distro" "${missing_packages[@]}"
else
    echo -e "${GREEN}All required packages are already installed.${RESET}"
fi

# Prompt for build type
read -p "Enter the type of build (cmake or make): " build_type

# Build commands
if [ "$build_type" == "cmake" ]; then
    cmake -S . -B build/
    cmake --build build/
elif [ "$build_type" == "make" ]; then
    make
else
    echo -e "${RED}Invalid build type. Please enter 'cmake' or 'make'.${RESET}"
    exit 1
fi

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed.${RESET}"
    exit 1
fi

# Copy the man file and gzip it
echo -e "${PINK}============ MAN PAGE =============${RESET}"
read -p "Add manual page? (y/n) " confirm_man
if [ "$confirm_man" == "y" ]; then
    sudo cp components/litefm.1 /usr/share/man/man1/
    sudo gzip /usr/share/man/man1/litefm.1
fi 

# Create alias in the appropriate shell configuration file
echo -e "${PINK}============= SETTING ALIAS =========${RESET}"
SHELL_NAME=$(basename "$SHELL")
case "$SHELL_NAME" in
    bash)
        echo "alias lfm='$(pwd)/build/litefm'" >> ~/.bashrc
        ;;
    zsh)
        echo "alias lfm='$(pwd)/build/litefm'" >> ~/.zshrc
        ;;
    *)
        echo -e "${RED}Shell $SHELL_NAME not supported for alias creation. Please create the alias manually.${RESET}"
        ;;
esac

echo -e "${GREEN}Build completed and man page installed. Please restart your terminal or source your rc file to use the litefm command.${RESET}"
