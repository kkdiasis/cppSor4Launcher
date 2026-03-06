#include "gui_launcher.h"
#include "gui_config.h"
#include <string>

// =============================================================================
// STATE
// =============================================================================

struct ModEntry {
    std::string label;
    std::string caminho;
};

// =============================================================================
// DRAW ITEM
// =============================================================================

static bool desenhar_item(const Font& fonte, const std::string& label,
                           const std::string& caminho,
                           const std::string& ativo,
                           int x, int y, int larg, int alt) {
    Rectangle rect  = {(float)x, (float)y, (float)larg, (float)alt};
    Vector2   mouse = GetMousePosition();
    bool      hover = CheckCollisionPointRec(mouse, rect);
    bool      is_ativo = (ativo == caminho);

    int   tamanho  = hover ? 70 : 50;
    int   offset_x = hover ? 30 : 0;

    

    if (hover) {
        draw_text_glow(fonte, label,
            x + 18 + offset_x,
            y + alt / 2 - tamanho / 2,
            (float)tamanho);
    } else {
        Color cor = WHITE;
        DrawTextEx(fonte, label.c_str(),
            {(float)(x + 18), (float)(y + alt / 2 - tamanho / 2)},
            (float)tamanho, 1, cor);
    }

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// =============================================================================
// DRAW TITLE
// =============================================================================

static void desenhar_titulo(const Font& fonte) {
    DrawTextEx(fonte, "SOR4 MOD LAUNCHER", {20, 22}, 36, 1, TITULO_COR);
    DrawTextEx(fonte, "select a mod and play", {22, 50}, 14, 1, TEXTO_FRACO);
}

// =============================================================================
// MAIN LAUNCHER LOOP
// =============================================================================

void run_launcher(GuiResources& res,
                  BigfileManager& bigfile_manager,
                  ModManager&     mod_manager,
                  GameLauncher&   launcher,
                  const std::string& exe_path) {

    int   ITEM_LARG = WIN_WIDTH - MARGEM * 2;
    std::string ativo = "";

    // Load active mods on start
    mod_manager.listar_mods();
    std::vector<ModEntry> mods_ativos;

    auto recarregar_mods = [&]() {
        mod_manager.listar_mods();
        mods_ativos.clear();
        for (auto& m : mod_manager.get_mods_ativos()) {
            mods_ativos.push_back({m.label, m.caminho});
        }
    };

    recarregar_mods();

    while (!WindowShouldClose()) {
        BeginDrawing();
        draw_background(res);
        desenhar_titulo(res.fonte);

        int y = 80;

        // -----------------------------------------------------------------
        // Original option
        // -----------------------------------------------------------------
        std::string caminho_original = bigfile_manager.get_bigfile_original().string();

        if (desenhar_item(res.fonte, "Original", caminho_original, ativo, MARGEM, y, ITEM_LARG, ITEM_ALT)) {
            mod_manager.ativar_original(
                bigfile_manager.get_bigfile(),
                bigfile_manager.get_bigfile_original()
            );
            ativo = caminho_original;
            launcher.lancar();
        }

        y += ITEM_ALT + 6;

        // -----------------------------------------------------------------
        // Mod list
        // -----------------------------------------------------------------
        if (!mods_ativos.empty()) {
            desenhar_separador(y);
            y += 12;

            for (auto& mod : mods_ativos) {
                if (desenhar_item(res.fonte, mod.label, mod.caminho, ativo, MARGEM, y, ITEM_LARG, ITEM_ALT)) {
                    mod_manager.iniciar_mod(
                        bigfile_manager.get_bigfile(),
                        fs::path(mod.caminho)
                    );
                    ativo = mod.caminho;
                    launcher.lancar();
                }
                y += ITEM_ALT + 6;
            }
        }

        // -----------------------------------------------------------------
        // Fixed buttons
        // -----------------------------------------------------------------
        y += 6;
        desenhar_separador(y);
        y += 14;

        if (desenhar_botao(res.fonte, "Restore original", MARGEM, y, ITEM_LARG, BTN_ALT, BTN_RESTAURAR)) {
            bigfile_manager.restauracao_completa();
            mods_ativos.clear();
            ativo = "";
        }

        y += BTN_ALT + 8;

        if (desenhar_botao(res.fonte, "Settings", MARGEM, y, ITEM_LARG, BTN_ALT, BTN_CONFIG_C)) {
            run_config(res, mod_manager, exe_path);
            recarregar_mods();
            ativo = "";
        }

        y += BTN_ALT + 8;

        if (desenhar_botao(res.fonte, "Quit", MARGEM, y, ITEM_LARG, BTN_ALT, {18, 24, 42, 255})) {
            break;
        }

        EndDrawing();
    }
}
