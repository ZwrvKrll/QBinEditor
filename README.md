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
- **Customizable Appearance:** The application uses a monospaced font (Courier New) for clean alignment and displays highlighted matches in a light gray color (#999999).
- **Cross-Platform:** Built with Qt, QBinEditor can run on various Linux distributions and other platforms supported by Qt.

## Requirements

- Qt 5 (Widgets, GUI, Core)
- g++ (or a compatible C++ compiler supporting C++11)
- Libraries:
  - libQt5Widgets
  - libQt5Gui
  - libQt5Core

## Building

1. **Clone or Download the Source Code**

   Clone the repository or download the source as a ZIP file and extract it.

2. **Build with qmake and make**

   Open a terminal and navigate to the source directory. Then run:

   ```bash
   qmake
   make
