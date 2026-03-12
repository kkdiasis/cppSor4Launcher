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

    int tamanho  = hover ? 70 : 50;
    int offset_x = hover ? 30 : 0;

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

    int ITEM_LARG = WIN_WIDTH - MARGEM * 2;
    std::string ativo        = "";
    std::string ultimo_hover = "";

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

        Rectangle rect_original = {(float)MARGEM, (float)y, (float)ITEM_LARG, (float)ITEM_ALT};
        bool hover_original = CheckCollisionPointRec(GetMousePosition(), rect_original);

        if (hover_original && ultimo_hover != caminho_original) {
            PlaySound(res.sfx_hover);
            ultimo_hover = caminho_original;
        } else if (!hover_original && ultimo_hover == caminho_original) {
            ultimo_hover = "";
        }

        if (desenhar_item(res.fonte, "Original", caminho_original, ativo, MARGEM, y, ITEM_LARG, ITEM_ALT)) {
            PlaySound(res.sfx_click);
            mod_manager.ativar_original(
                bigfile_manager.get_bigfile(),
                bigfile_manager.get_bigfile_original()
            );
            ativo = caminho_original;
            launcher.lancar();
            return; //Quit after load a start mod
        }

        y += ITEM_ALT + 6;

        // -----------------------------------------------------------------
        // Mod list
        // -----------------------------------------------------------------
        if (!mods_ativos.empty()) {
            desenhar_separador(y);
            y += 12;

            for (auto& mod : mods_ativos) {
                Rectangle rect_mod = {(float)MARGEM, (float)y, (float)ITEM_LARG, (float)ITEM_ALT};
                bool hover_mod = CheckCollisionPointRec(GetMousePosition(), rect_mod);

                if (hover_mod && ultimo_hover != mod.caminho) {
                    PlaySound(res.sfx_hover);
                    ultimo_hover = mod.caminho;
                } else if (!hover_mod && ultimo_hover == mod.caminho) {
                    ultimo_hover = "";
                }

                if (desenhar_item(res.fonte, mod.label, mod.caminho, ativo, MARGEM, y, ITEM_LARG, ITEM_ALT)) {
                    PlaySound(res.sfx_click);
                    mod_manager.iniciar_mod(
                        bigfile_manager.get_bigfile(),
                        fs::path(mod.caminho)
                    );
                    ativo = mod.caminho;
                    launcher.lancar();
                    return; //quit after load  and start mod
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

        bool hov = false;

        if (desenhar_botao_fixo_hover(res.fonte, "Restore original", MARGEM, y, ITEM_LARG, BTN_ALT, BTN_RESTAURAR, hov)) {
            PlaySound(res.sfx_click);
            bigfile_manager.restauracao_completa();
            mods_ativos.clear();
            ativo = "";
        }
        if (hov && ultimo_hover != "__restore__") { PlaySound(res.sfx_hover); ultimo_hover = "__restore__"; }
        else if (!hov && ultimo_hover == "__restore__") ultimo_hover = "";

        y += BTN_ALT + 8;

        if (desenhar_botao_fixo_hover(res.fonte, "Settings", MARGEM, y, ITEM_LARG, BTN_ALT, BTN_CONFIG_C, hov)) {
            PlaySound(res.sfx_click);
            run_config(res, mod_manager, exe_path);
            recarregar_mods();
            ativo = "";
        }
        if (hov && ultimo_hover != "__settings__") { PlaySound(res.sfx_hover); ultimo_hover = "__settings__"; }
        else if (!hov && ultimo_hover == "__settings__") ultimo_hover = "";

        y += BTN_ALT + 8;
        
        if (hov && ultimo_hover != "__quit__") { PlaySound(res.sfx_hover); ultimo_hover = "__quit__"; }
        else if (!hov && ultimo_hover == "__quit__") ultimo_hover = "";

        if (desenhar_botao_fixo_hover(res.fonte, "Quit", MARGEM, y, ITEM_LARG, BTN_ALT, {18, 24, 42, 255}, hov)) {
            PlaySound(res.sfx_click);
            return;
        }

        EndDrawing();
    }
}