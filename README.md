# SD1 v1.0 to GUI (Task Nr. 4)

This solution converts the original SD1 v1.0 console program into a Windows GUI application written in C++.

## What you get (matches the assignment)
- **Windows GUI** with a main window and a large **TextBox-like control** (Win32 multiline Edit) for logs and results.
- **Menu items**
  - File: Open data file, Save log to file, Exit
  - Tools: Generate data file, Process file
  - Help: About
- Uses **OpenFileDialog / SaveFileDialog equivalents** via Win32 Common Dialogs:
  - `GetOpenFileNameW`
  - `GetSaveFileNameW`
- Processing uses the same SD1 logic (vector/list/deque + 2 split strategies).
- A basic **installer script** is included (Inno Setup) to produce `setup.exe`.

## How to run (Visual Studio)
1. Open `StudentGradesGui.sln`
2. Build `Release | x64`
3. Run

## App workflow
1. Tools -> Generate data
   - Choose where to save the generated file (Save dialog)
   - Enter number of records
2. File -> Open
   - Select an existing data file (Open dialog)
3. Tools -> Process file
   - Choose container + split strategy
   - Choose where to save the PASSED file (Save dialog)
   - The FAILED file is automatically saved next to it with `_failed` suffix

## Installer (Inno Setup)
File: `installer/StudentGradesGui.iss`

1. Install **Inno Setup**
2. Open the `.iss` file and edit:
   - `AppPublisher` to **VVK** (already set)
   - `DefaultDirName` to `"{pf}\VVK\Your_name_Your_surname"`
3. Compile the script to produce `setup.exe`

## Notes
- The core SD1 logic is kept in `src/` (FileGenerator, Processor, Student, Timer).
- `src/gui_main.cpp` contains all GUI code (window, menu, dialogs, Open/Save, and calling the core logic).
