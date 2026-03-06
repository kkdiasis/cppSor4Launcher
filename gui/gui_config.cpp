#include "gui_config.h"
#include "../src/config.h"
#include "../src/tinyfiledialogs.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

constexpr int MODS_VISIVEIS = 4;
constexpr int MODS_AREA_ALT = MODS_VISIVEIS * (ITEM_ALT + 6);

// =============================================================================
// STATE
// =============================================================================

struct ConfigState {
    std::string game_path;
    std::string mods_path;
    std::vector<ModInfo> mods;
    int  editando_idx  = -1;
    int  scroll_offset = 0;
    std::string input_buffer;
    std::string mensagem;
};

// =============================================================================
// FOLDER DIALOG
// =============================================================================

static std::string selecionar_pasta(const std::string& titulo, const std::string& path_atual) {
    const char* resultado = tinyfd_selectFolderDialog(titulo.c_str(), path_atual.c_str());
    return resultado ? std::string(resultado) : "";
}

// =============================================================================
// COMPONENTS
// =============================================================================

static void desenhar_secao(const Font& fonte, const std::string& titulo, int y) {
    DrawTextEx(fonte, titulo.c_str(), {(float)MARGEM, (float)y}, 16, 1, TEXTO_FRACO);
}

static void desenhar_campo_path(const Font& fonte, const std::string& valor,
                                 int x, int y, int larg) {
    Rectangle rect = {(float)x, (float)y, (float)larg, (float)INPUT_ALT};
    DrawRectangleRec(rect, INPUT_FUNDO);
    DrawRectangleLinesEx(rect, 1, INPUT_BORDA);
    std::string texto = truncar_path(valor, larg - 16);
    DrawTextEx(fonte, texto.c_str(), {(float)(x + 8), (float)(y + 8)}, 16, 1, TEXTO);
}

static bool desenhar_checkbox(bool marcado, int x, int y) {
    Rectangle rect  = {(float)x, (float)y, (float)CHECK_TAM, (float)CHECK_TAM};
    Vector2   mouse = GetMousePosition();
    bool      hover = CheckCollisionPointRec(mouse, rect);

    Color cor_fundo = marcado ? CHECK_ATIVO : (hover ? clarear(ITEM_NORMAL) : ITEM_NORMAL);
    DrawRectangleRec(rect, cor_fundo);
    DrawRectangleLinesEx(rect, 1, CHECK_BORDA);

    if (marcado) {
        DrawLine(x+3,  y+10, x+8,  y+15, WHITE);
        DrawLine(x+8,  y+15, x+17, y+5,  WHITE);
    }

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void desenhar_label_editavel(const Font& fonte, ConfigState& estado,
                                     int idx, int x, int y, int larg, int alt) {
    ModInfo&  mod     = estado.mods[idx];
    Rectangle rect    = {(float)x, (float)y, (float)larg, (float)alt};
    bool      editando = (estado.editando_idx == idx);
    bool      hover    = CheckCollisionPointRec(GetMousePosition(), rect);

    Color cor_fundo = editando ? INPUT_ATIVO : (hover ? ITEM_HOVER : ITEM_NORMAL);
    DrawRectangleRec(rect, cor_fundo);

    if (editando) {
        DrawRectangleLinesEx(rect, 1, INPUT_BORDA);

        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125)
                estado.input_buffer += (char)key;
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !estado.input_buffer.empty())
            estado.input_buffer.pop_back();

        if (IsKeyPressed(KEY_ENTER)) {
            mod.label           = estado.input_buffer.empty() ? mod.nome : estado.input_buffer;
            estado.editando_idx = -1;
            estado.input_buffer = "";
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            estado.editando_idx = -1;
            estado.input_buffer = "";
        }

        std::string cursor = (fmod(GetTime(), 1.0) < 0.5) ? "|" : "";
        DrawTextEx(fonte, (estado.input_buffer + cursor).c_str(),
            {(float)(x + 10), (float)(y + alt / 2 - 12)}, 24, 1, TEXTO);

    } else {
        DrawTextEx(fonte, mod.label.c_str(),
            {(float)(x + 10), (float)(y + alt / 2 - 12)}, 24, 1, TEXTO);
        DrawTextEx(fonte, "click to rename",
            {(float)(x + 10), (float)(y + alt - 16)}, 14, 1, TEXTO_FRACO);

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            estado.editando_idx = idx;
            estado.input_buffer = mod.label;
        }
    }
}

// =============================================================================
// CONFIG SCREEN LOOP
// =============================================================================

void run_config(GuiResources& res, ModManager& mod_manager, const std::string& exe_path) {
    ConfigState estado;

    json cfg = carregar_config("cfg\\config.json", exe_path);  // <- usa exe_path
    estado.game_path  = cfg["game"]["caminho"];
    estado.mods_path  = cfg["mods"]["caminho"];
    estado.mods       = mod_manager.get_mods();

    auto ativos_count = [&]() {
        return (int)std::count_if(estado.mods.begin(), estado.mods.end(),
            [](const ModInfo& m){ return m.ativo; });
    };

    int larg_campo = WIN_WIDTH - MARGEM * 2 - 110;

    while (!WindowShouldClose()) {
        BeginDrawing();
        draw_background(res);

        int y = 20;

        // Title
        DrawTextEx(res.fonte, "SETTINGS", {(float)MARGEM, (float)y}, 36, 1, TITULO_COR);
        y += 50;

        desenhar_separador(y);
        y += 16;

        // -----------------------------------------------------------------
        // Game installation path
        // -----------------------------------------------------------------
        desenhar_secao(res.fonte, "GAME INSTALLATION", y);
        y += 22;

        desenhar_campo_path(res.fonte, estado.game_path, MARGEM, y, larg_campo);

        if (desenhar_botao_filled(res.fonte, "Browse", MARGEM + larg_campo + 10, y, 100, INPUT_ALT, BTN_PASTA)) {
            std::string pasta = selecionar_pasta("Select game folder", estado.game_path);
            if (!pasta.empty())
                estado.game_path = pasta;
        }

        y += INPUT_ALT + 16;
        desenhar_separador(y);
        y += 16;

        // -----------------------------------------------------------------
        // Mods folder path
        // -----------------------------------------------------------------
        desenhar_secao(res.fonte, "MODS FOLDER", y);
        y += 22;

        desenhar_campo_path(res.fonte, estado.mods_path, MARGEM, y, larg_campo);

        if (desenhar_botao_filled(res.fonte, "Browse", MARGEM + larg_campo + 10, y, 100, INPUT_ALT, BTN_PASTA)) {
            std::string pasta = selecionar_pasta("Select mods folder", estado.mods_path);
            if (!pasta.empty()) {
                estado.mods_path   = pasta;
                estado.scroll_offset = 0;
                mod_manager.set_pasta(pasta);
                mod_manager.sanitizar_nomes();
                mod_manager.listar_mods();
                estado.mods = mod_manager.get_mods();
            }
        }

        y += INPUT_ALT + 16;
        desenhar_separador(y);
        y += 16;

        // -----------------------------------------------------------------
        // Mod list with scroll
        // -----------------------------------------------------------------
        std::string secao_titulo = "MODS  (" + std::to_string(ativos_count()) + "/5 enabled)";
        desenhar_secao(res.fonte, secao_titulo, y);
        y += 24;

        if (estado.mods.empty()) {
            DrawTextEx(res.fonte, "No mods found in folder.",
                {(float)MARGEM, (float)y}, 20, 1, TEXTO_FRACO);
            y += 30;
        } else {
            int total      = (int)estado.mods.size();
            int max_scroll = std::max(0, total - MODS_VISIVEIS);

            float delta = GetMouseWheelMove();
            if (delta != 0)
                estado.scroll_offset = std::max(0, std::min(
                    estado.scroll_offset - (int)delta, max_scroll));

            Rectangle area     = {(float)MARGEM, (float)y,
                                   (float)(WIN_WIDTH - MARGEM * 2), (float)MODS_AREA_ALT};
            bool mouse_na_area = CheckCollisionPointRec(GetMousePosition(), area);

            BeginScissorMode(MARGEM, y, WIN_WIDTH - MARGEM * 2, MODS_AREA_ALT);

            for (int i = 0; i < total; i++) {
                int item_y = y + i * (ITEM_ALT + 6) - estado.scroll_offset * (ITEM_ALT + 6);

                if (item_y + ITEM_ALT < y || item_y > y + MODS_AREA_ALT) continue;

                bool clicou = mouse_na_area &&
                    desenhar_checkbox(estado.mods[i].ativo,
                        MARGEM, item_y + ITEM_ALT / 2 - CHECK_TAM / 2);

                if (clicou) {
                    if (estado.mods[i].ativo) {
                        estado.mods[i].ativo = false;
                    } else if (ativos_count() < 5) {
                        estado.mods[i].ativo = true;
                    } else {
                        estado.mensagem = "Maximum of 5 mods reached.";
                    }
                }

                desenhar_label_editavel(res.fonte, estado, i,
                    MARGEM + CHECK_TAM + 10, item_y,
                    WIN_WIDTH - MARGEM * 2 - CHECK_TAM - 10, ITEM_ALT);
            }

            EndScissorMode();

            // Scrollbar
            if (total > MODS_VISIVEIS) {
                int sb_x       = WIN_WIDTH - MARGEM - 6;
                int sb_h_total = MODS_AREA_ALT;
                int sb_h_thumb = std::max(30, sb_h_total / total);
                int sb_y       = max_scroll > 0
                    ? y + (int)((float)estado.scroll_offset / max_scroll * (sb_h_total - sb_h_thumb))
                    : y;

                DrawRectangle(sb_x, y,    4, sb_h_total, ITEM_NORMAL);
                DrawRectangle(sb_x, sb_y, 4, sb_h_thumb, CHECK_BORDA);
            }

            y += MODS_AREA_ALT + 10;
        }

        desenhar_separador(y);
        y += 14;

        // Feedback message
        if (!estado.mensagem.empty())
            DrawTextEx(res.fonte, estado.mensagem.c_str(),
                {(float)MARGEM, (float)y}, 18, 1, TITULO_COR);

        y += 28;

        // -----------------------------------------------------------------
        // Buttons
        // -----------------------------------------------------------------
        int metade = (WIN_WIDTH - MARGEM * 2 - 10) / 2;

        if (desenhar_botao_filled(res.fonte, "Save", MARGEM, y, metade, BTN_ALT, BTN_SALVAR)) {
            // Save paths.json
            json novo_cfg;
            novo_cfg["game"]["caminho"] = estado.game_path;
            novo_cfg["game"]["backup"]  = cfg["game"]["backup"];
            novo_cfg["mods"]["caminho"] = estado.mods_path;
            salvar_config("cfg\\config.json", novo_cfg);

            // Save mods state
            mod_manager.set_mods(estado.mods);
            mod_manager.salvar_mods("cfg\\mods.json");

            estado.mensagem = "Settings saved!";
        }

        if (desenhar_botao_filled(res.fonte, "Back", MARGEM + metade + 10, y, metade, BTN_ALT, BTN_VOLTAR)) {
            EndDrawing();
            return;
        }

        EndDrawing();
    }
}
