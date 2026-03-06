# SOR4 Mod Launcher — Development Roadmap

---

## Architecture Philosophy

The project is intentionally split into two independent layers:

```
┌─────────────────────────────────────────┐
│                  GUI                    │
│  (pyray, tkinter, web, Qt, or any UI)   │
│                                         │
│  Only knows how to draw and handle      │
│  user input. Has no file system logic.  │
└────────────────┬────────────────────────┘
                 │  calls
                 ▼
┌─────────────────────────────────────────┐
│                 CORE                    │
│           (src/core.py)                 │
│                                         │
│  Only knows about files and symlinks.   │
│  Has no drawing or UI code.             │
└─────────────────────────────────────────┘
```

The Core never imports from GUI.
The GUI always imports from Core.
This is the only rule that must never be broken.

---

## Current Stack

| Layer | Technology | File |
|---|---|---|
| Core logic | Python + pathlib | `src/core.py` |
| Main screen | Python + pyray | `gui/launcher.py` |
| Settings screen | Python + pyray + tkinter | `gui/config.py` |
| Entry point | Python | `main.py` |
| Config storage | JSON | `cfg/paths.json`, `cfg/mods.json` |

---

## Core API — Public Contract

These are the functions and variables the GUI depends on.
**This interface must remain stable regardless of refactoring.**

### Variables (exported via `src/__init__.py`)

```
game_path         Path   — game installation folder
mods_path         Path   — mods folder
bigfile           Path   — symlink file
bigfile_bkp       Path   — backup of original bigfile
bigfile_original  Path   — renamed original bigfile
paths_json        Path   — paths config file
mods_json         Path   — mods config file
config_path       Path   — cfg/ folder
```

### Functions

```
preparar_ambiente(bigf, bigf_bkp)     → None
  Prepares symlink structure on first run.

restauracao_completa(bigf, bigf_bkp)  → None
  Removes symlink, restores original from backup.

listar_mods(path)                     → list[{nome, caminho}]
  Scans a folder and returns available mod files.

iniciar_mod(bigf, caminho_mod)        → None
  Redirects bigfile symlink to chosen mod.

sanitizar_nomes_mods(path)            → None
  Renames files: removes spaces/parentheses.

recarregar_paths()                    → dict
  Re-reads paths.json from disk (bypasses cache).

validar_arquivo(arquivo)              → bool
  Validates a file by name, existence and size.
```

---

## Refactoring to Another Language

Follow these steps in order to safely port the project.

---

### Step 1 — Port the Core first, ignore GUI completely

The Core is pure logic — no UI, no rendering, just file system operations.
Port these functions one by one and test each before moving on:

```
1. recarregar_paths()        — read JSON config
2. preparar_ambiente()       — backup + symlink creation
3. listar_mods()             — scan folder
4. sanitizar_nomes_mods()    — rename files
5. validar_arquivo()         — file validation
6. iniciar_mod()             — redirect symlink
7. restauracao_completa()    — restore from backup
```

**Key behaviors to preserve:**
- `cfg/` must be writable and located next to the executable
- `paths.json` must be created with defaults if it doesn't exist
- Symlinks require admin/elevated privileges on Windows
- `mods.json` schema: `[{nome, caminho, label, ativo}]`

---

### Step 2 — Define the interface between Core and GUI

Before writing any GUI code, write the interface contract in your new language.
This is the list of calls the GUI will make to the Core:

```
On app start:
  preparar_ambiente()
  recarregar_paths()         → get mods_path
  listar_mods(mods_path)     → populate list
  read mods_json             → get labels and active flags

On mod click:
  iniciar_mod(bigfile, path)
  launch Steam               → steam://rungameid/985890

On "Restore original" click:
  restauracao_completa()
  delete mods_json
  clear UI list

On Settings open:
  recarregar_paths()
  listar_mods()
  read mods_json

On Settings save:
  write paths_json
  write mods_json

On Settings "Back":
  recarregar_paths()
  listar_mods(updated_path)
  read mods_json             → reload active mods
```

---

### Step 3 — Port the GUI

Only after the Core is fully working, port the GUI.
The GUI should only:

- Read from Core (call functions, read returned data)
- Draw the interface
- Handle user input and call Core functions in response

The GUI should never:
- Read or write JSON directly (delegate to Core)
- Manipulate symlinks directly
- Know what `bigfile` or `bigfile_bkp` are internally

---

### Step 4 — Plug GUI into Core

Connect them through the interface defined in Step 2.
If the Core API is stable, this step should require no changes to the Core.

---

## Possible GUI Replacements

| GUI | Language | Notes |
|---|---|---|
| pyray (current) | Python | Lightweight, game-like feel |
| PyQt6 / PySide6 | Python | Native Windows look, more widgets |
| Dear PyGui | Python | Immediate mode, similar to pyray |
| Tauri + React | Rust + JS | Web UI, modern look, good for distribution |
| Electron + React | JS | Web UI, heavier but very flexible |
| Qt (C++) | C++ | Full port, maximum performance |
| WinUI 3 | C# | Native Windows 11 look |

**Recommendation for a future port:**
Tauri + React would be the best balance — Rust handles the Core logic
(file system, symlinks, JSON), React handles the GUI, and the result is
a lightweight native executable with a modern interface.

---

## Possible Core Replacements

| Language | Notes |
|---|---|
| Python (current) | Simple, works well |
| Rust | Best for symlinks and file ops on Windows, great for Tauri |
| C# | Natural fit for Windows-native apps |
| Go | Simple, fast, easy cross-compilation |

---

## Data Schema Reference

### cfg/paths.json
```json
{
    "game": "C:\\...\\Streets of Rage 4\\data",
    "mods": "C:\\...\\SOR4ModLauncher\\mods"
}
```

### cfg/mods.json
```json
[
    {
        "nome":    "LVLS-1-2-3_bigfile",
        "caminho": "C:\\...\\mods\\LVLS-1-2-3_bigfile",
        "label":   "Levels 1-2-3",
        "ativo":   true
    }
]
```

---

## Feature Backlog (possible next versions)

- [ ] Drag to reorder mods in the settings list
- [ ] Mod preview image support
- [ ] First-run setup wizard (guide user through paths configuration)
- [ ] Support for multiple games (not just SOR4)
- [ ] Auto-detect Steam game path from Windows registry
- [ ] Mod download integration (GameBanana API)
- [ ] Dark/light theme toggle
- [ ] Portable mode (all config inside app folder, no default paths)

---

## Project File Map

```
sor4-mod-launcher/
│
├── main.py                 Entry point. Calls Core then GUI.
│
├── src/
│   ├── __init__.py         Exports Core public API.
│   └── core.py             All file system logic. No GUI imports.
│
├── gui/
│   ├── __init__.py         Empty package marker.
│   ├── launcher.py         Main screen. Imports from src only.
│   └── config.py           Settings screen. Imports from src only.
│
├── cfg/                    Runtime config (writable, next to .exe)
│   ├── paths.json
│   └── mods.json
│
├── mods/                   User mod files (next to .exe)
│
├── assets/                 Bundled inside .exe
│   ├── background.png
│   ├── BebasNeue-Regular.ttf
│   └── icon.ico
│
├── build.py                Build script → dist/SOR4ModLauncher/
├── requirements.txt
├── README.md
├── RELEASE_NOTES.md
└── LICENSE
```
