#!/bin/bash

# Define colors and styles
GREEN="\e[32m"
RED="\e[31m"
PINK="\e[35m"
BLUE="\e[34m"
CYAN="\e[36m"
RESET="\e[0m"
BOLD="\e[1m"
UNDERLINE="\e[4m"

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

detect_nerd_fonts() {
    FONT_CHECK=$(fc-list | grep -i "nerd")
    if [ -n "$FONT_CHECK" ]; then
        echo -e "${CYAN}Nerd Fonts are installed and available.${RESET}"
    else
        echo -e "${RED}Nerd Fonts are not installed or not available.${RESET}"
        echo -e "\n${RED}${BOLD}Kindly ensure that you have a nerd font installed and enabled in your terminal for LiteFM!${RESET}"
    fi
}

# Function to check if a package is installed
check_package_installed() {
    local distro="$1"
    local package="$2"
    
    case "$distro" in
        debian)
            dpkg-query -W -f='${Status}' "$package" 2>/dev/null | grep -c "ok installed"
            ;;
        rpm)
            rpm -q "$package" &> /dev/null
            ;;
        arch)
            pacman -Q "$package" &> /dev/null
            ;;
        *)
            echo -e "${RED}Unsupported package manager.${RESET}"
            return 1
            ;;
    esac
}

# Function to install packages based on the distribution
install_packages() {
    local distro="$1"
    shift
    local packages=("$@")
    
    echo -e "${PINK}${BOLD}Installing missing packages:${RESET} ${packages[*]}"
    case "$distro" in
        debian)
            sudo apt-get update
            sudo apt-get install -y "${packages[@]}"
            ;;
        rpm)
            sudo yum install -y "${packages[@]}"
            ;;
        arch)
            sudo pacman -S --needed "${packages[@]}"
            ;;
        *)
            echo -e "${RED}Unsupported distribution.${RESET}"
            exit 1
            ;;
    esac
}

# Function to detect the display server
detect_display_server() {
    if [ -n "$WAYLAND_DISPLAY" ]; then
        echo "wayland"
    elif [ -n "$DISPLAY" ]; then
        echo "x11"
    else
        echo "unknown"
    fi
}

# Title
echo -e "${GREEN}${BOLD}===================="
echo -e "      LiteFM        "
echo -e "====================${RESET}"

# Detect the distribution
distro=$(detect_distro)
if [ "$distro" == "unsupported" ]; then
    echo -e "${RED}${BOLD}Unsupported Linux distribution.${RESET}"
    exit 1
fi

echo -e "${PINK}${BOLD}Building for $distro...${RESET}"

# Detect the display server
display_server=$(detect_display_server)
echo -e "${PINK}${BOLD}Detected display server: $display_server${RESET}"

# Define the required packages based on the distribution and display server
required_packages=("libncursesw5-dev" "cmake" "make" "libarchive-dev" "libyaml-dev" "rsync" "pkg-config")

case "$display_server" in
    wayland)
        required_packages+=("wl-clipboard")
        ;;
    x11)
        required_packages+=("xclip")
        ;;
esac

if [ "$distro" == "rpm" ]; then
    required_packages=("ncurses" "cmake" "make" "libarchive" "libyaml" "rsync" "pkg-config")
elif [ "$distro" == "arch" ]; then
    required_packages=("ncurses" "cmake" "make" "libarchive" "libyaml" "rsync" "pkg-config")
fi

# Check for required packages
missing_packages=()
for package in "${required_packages[@]}"; do
    if check_package_installed "$distro" "$package"; then
        echo -e "${GREEN}✔ Package $package is already installed.${RESET}"
    else
        echo -e "${RED}✘ Package $package is missing.${RESET}"
        missing_packages+=("$package")
    fi
done

if [ ${#missing_packages[@]} -ne 0 ]; then
    install_packages "$distro" "${missing_packages[@]}"
else
    echo -e "${GREEN}${BOLD}All required packages are already installed.${RESET}"
fi

# Prompt for build type
detect_nerd_fonts
read -p "Enter the type of build (cmake or make): " build_type

# Build commands
case "$build_type" in
    cmake)
        echo -e "${CYAN}Running cmake build...${RESET}"
        cmake -S . -B build/
        cmake --build build/
        ;;
    make)
        echo -e "${CYAN}Running make build...${RESET}"
        make
        ;;
    *)
        echo -e "${RED}${BOLD}Invalid build type. Please enter 'cmake' or 'make'.${RESET}"
        exit 1
        ;;
esac

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo -e "${RED}${BOLD}Build failed.${RESET}"
    exit 1
fi

# Copy the man file and gzip it
echo -e "${PINK}${BOLD}============ MAN PAGE =============${RESET}"
read -p "Add manual page? (y/n) " confirm_man
if [ "$confirm_man" == "y" ]; then
    sudo cp components/litefm.1 /usr/share/man/man1/
    sudo gzip /usr/share/man/man1/litefm.1
fi 

# Setup logging
echo -e "${BLUE}${BOLD}=========== LOGGING SETUP =============== ${RESET}"
mkdir -p "$HOME/.cache/litefm/log/"
if [ $? -eq 0 ]; then
  echo -e "${BLUE}${BOLD}================ LOGGING DONE ============= ${RESET}"
else
  echo -e "${RED}Something went wrong while setting up logging. Exiting with code 1${RESET}"
  exit 1 
fi

# Create alias in the appropriate shell configuration file
echo -e "${PINK}${BOLD}============= SETTING ALIAS =========${RESET}"
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

echo -e "${GREEN}${BOLD}Build completed and man page installed. Please restart your terminal or source your rc file to use the litefm command.${RESET}"
