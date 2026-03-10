#include <iostream>
#include <string>
#include <filesystem>
#include "BigfileManager.h"
#include "ModManager.h"
#include "GameLauncher.h"
#include "config.h"
#include "../gui/gui_common.h"
#include "../gui/gui_launcher.h"
#include "raylib.h"

using namespace std;
namespace fs = std::filesystem;

#define STEAM_APP_ID "985890"

// =============================================================================
// ASSETS PATH
// =============================================================================

static fs::path assets_path(const char* argv0) {
    return fs::absolute(fs::path(argv0)).parent_path() / "assets";
}

// =============================================================================
// ENTRY POINT
// =============================================================================

int main(int argc, char* argv[]) {
    try {
        // ---------------------------------------------------------------------
        // Core setup
        // ---------------------------------------------------------------------
        criar_config_padrao(argv[0]);
        json config = carregar_config("cfg\\config.json", argv[0]);

        string caminho_jogo = config["game"]["caminho"];
        string caminho_bkp  = config["game"]["backup"];
        string caminho_mods = config["mods"]["caminho"];

        if (!fs::exists(caminho_mods))
            fs::create_directories(caminho_mods);

        BigfileManager bigfile_manager(caminho_jogo, caminho_bkp);
        ModManager     mod_manager(caminho_mods);
        GameLauncher   launcher(STEAM_APP_ID);

        bigfile_manager.preparar_ambiente();

        mod_manager.sanitizar_nomes();
        mod_manager.listar_mods();
        mod_manager.carregar_mods("cfg\\mods.json");

        // ---------------------------------------------------------------------
        // Window + resources
        // ---------------------------------------------------------------------
        InitWindow(WIN_WIDTH, WIN_HEIGHT, "SOR4 Mod Launcher");
        SetTargetFPS(60);
        SetWindowState(FLAG_WINDOW_UNDECORATED);
        InitAudioDevice(); 

        fs::path ap = assets_path(argv[0]);

        GuiResources res;
        res.bg_texture = LoadTexture((ap / "background.png").string().c_str());
        res.fonte      = LoadFontEx((ap / "BebasNeue-Regular.ttf").string().c_str(), 128, nullptr, 0);
        res.sfx_hover  = LoadSound((ap / "hover.wav").string().c_str());  // <- adicionar
        res.sfx_click  = LoadSound((ap / "click.wav").string().c_str());  // <- adicionar

        // ---------------------------------------------------------------------
        // Run GUI
        // ---------------------------------------------------------------------
        run_launcher(res, bigfile_manager, mod_manager, launcher, argv[0]);

        // ---------------------------------------------------------------------
        // Cleanup
        // ---------------------------------------------------------------------
        UnloadFont(res.fonte);
        UnloadTexture(res.bg_texture);
        UnloadSound(res.sfx_hover);   // <- adicionar
        UnloadSound(res.sfx_click);   // <- adicionar
        CloseAudioDevice();            // <- adicionar
        CloseWindow();

    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}