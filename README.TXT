# SOR4 Mod Launcher — C++

An unofficial, fan-made mod launcher for **Streets of Rage 4** written in C++ with raylib.  
Switch between mods instantly — no manual file replacement needed.

> **Beta v0.1** — Core features complete. Visual polish in progress.

---

## How It Works

Streets of Rage 4 loads its game data from a single file called `bigfile`.  
This launcher creates a symbolic link named `bigfile` that points to whichever mod you select — the original or a custom one. Switching is instant and non-destructive: your original game file is never modified or deleted.

---

## Features

- Switch between up to 5 mods from a clean, game-styled menu
- Launches the game directly via Steam after selecting a mod
- Custom display names for each mod (inline rename)
- One-click restore to the original game files
- Settings screen to configure game path and mods folder
- Automatic mod filename sanitization (removes spaces and parentheses)

---

## Requirements

- Windows 10 / 11
- Streets of Rage 4 (Steam)
- Administrator privileges (required for symlink creation)

---

## Building from Source

**Dependencies:**
- MinGW-w64 with g++ (C++17)
- [raylib](https://www.raylib.com/) — expected at `C:/raylib/raylib/src`
- [nlohmann/json](https://github.com/nlohmann/json) — `json.hpp` in `src/`
- [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) — `tinyfiledialogs.h` + `.c` in `src/`

**Build:**
```bash
make
```

**Clean:**
```bash
make clean
```

Output: `SOR4ModLauncher.exe` in the project root.

---

## Project Structure

```
sor4-mod-launcher-cpp/
├── Makefile
├── src/
│   ├── main.cpp
│   ├── BigfileManager.h / .cpp   — symlink, backup, restore
│   ├── ModManager.h / .cpp       — list, sanitize, activate mods
│   ├── GameLauncher.h / .cpp     — launch game via Steam
│   ├── config.h / .cpp           — read/write config.json
│   ├── utils.h / .cpp            — Windows symlink helper
│   ├── app.rc                    — application icon resource
│   ├── json.hpp                  — nlohmann/json (third-party)
│   └── tinyfiledialogs.h / .c    — native folder dialog (third-party)
├── gui/
│   ├── gui_common.h / .cpp       — shared colors, font, helpers
│   ├── gui_launcher.h / .cpp     — main screen
│   └── gui_config.h / .cpp       — settings screen
├── cfg/                          — runtime config (auto-created)
│   ├── config.json
│   └── mods.json
├── mods/                         — place mod files here
└── assets/
    ├── background.png
    ├── BebasNeue-Regular.ttf
    └── icon.ico
```

---

## First Run

On the first launch the launcher will automatically:

1. Locate `bigfile` in your Streets of Rage 4 installation folder
2. Create a backup named `bigfile_bkp`
3. Rename the original to `bigfile_original`
4. Create a symbolic link named `bigfile` pointing to the original

> Run as Administrator — Windows requires elevated privileges to create symlinks.

The default game path is:
```
C:\Program Files (x86)\Steam\steamapps\common\Streets of Rage 4\data
```
If your Steam library is elsewhere, open **Settings** and browse to the correct folder.

---

## Architecture

The project is split into two fully independent layers:

```
GUI (raylib)      — draws, handles input, calls Core
Core (C++ / STL)  — file system, symlinks, JSON config
```

The Core never imports from GUI. Swapping the GUI for Qt, Dear ImGui, or any other framework requires no changes to the Core.

---

## Disclaimer

This is an unofficial fan-made tool. It is not affiliated with, endorsed by, or connected to **Dotemu**, **Lizardcube**, or **Guard Crush Games**. Streets of Rage 4 and all related trademarks are property of their respective owners. This launcher does not distribute, modify, or reproduce any game files.

---

## License

MIT License — see [LICENSE](LICENSE) for details.
