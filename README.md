# QBinEditor

**QBinEditor** is a binary file editor written in Qt. It provides a graphical interface for searching, viewing, and editing binary files in three different representations: HEX, ASCII, and BIN.

## Features

- **File Search:** Search for files by directory and pattern.
- **Multiple Views:** View and edit file contents in:
  - **HEX view:** Displays binary data as hexadecimal digits (with delimiters).
  - **ASCII view:** Displays binary data as ASCII characters.
  - **BIN view:** Displays binary data in binary (bit) format.
- **In-File Search:** 
  - The default search highlights only the first matching occurrence.
  - Navigate between matches using the up (↑) and down (↓) arrow buttons or keys.
  - An extra "Highlight All" option allows you to highlight all matches.
  - "Clear Search" removes all highlighting and resets the search field.
- **Editable Views:** All views (HEX, ASCII, and BIN) are editable.

## Requirements

- Qt 5 (Widgets, GUI, Core)
- g++ (or a compatible C++ compiler supporting C++11)
- Libraries:
  - libQt5Widgets
  - libQt5Gui
  - libQt5Core

## For Debian-based distros

- Download **.deb** package from **Releases**
- Open a terminal and navigate to the package directory. Then run:

   ```bash
  sudo dpkg -i qbineditor.deb
  ```
## For Arch-based distros

Open a terminal and run:
```bash
yay -S qbineditor
```

## For other distros

- Download **qbineditor** file from **Releases**
- (optional) Transfer this file to home directory (/home/<user>/qbineditor)
- Open a terminal and run:

  ```bash
  ./qbineditor
  ```

## Building

1. **Clone or Download the Source Code**

   Clone the repository or download the source as a ZIP file and extract it.

2. **Build with qmake and make**

   Open a terminal and navigate to the source directory. Then run:

   ```bash
   qmake
   make
