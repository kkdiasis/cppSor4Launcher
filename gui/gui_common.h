#pragma once

#include "raylib.h"
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// =============================================================================
// WINDOW
// =============================================================================

constexpr int   WIN_WIDTH   = 900;
constexpr int   WIN_HEIGHT  = 650;
constexpr int   FAIXA_LARG  = 200;
constexpr int   MARGEM      = 20;
constexpr int   ITEM_ALT    = 54;
constexpr int   BTN_ALT     = 46;
constexpr int   INPUT_ALT   = 32;
constexpr int   CHECK_TAM   = 20;

// =============================================================================
// PALETTE
// =============================================================================

constexpr Color FUNDO        = {10,  14,  26,  255};
constexpr Color ITEM_NORMAL  = {24,  34,  60,  255};
constexpr Color ITEM_HOVER   = {34,  48,  84,  255};
constexpr Color ITEM_ATIVO_C = {30,  80,  180, 255};
constexpr Color BARRA_ATIVA  = {80,  160, 255, 255};
constexpr Color SEPARADOR    = {30,  46,  80,  0};
constexpr Color TEXTO        = {210, 220, 240, 255};
constexpr Color TEXTO_FRACO  = {90,  110, 160, 255};
constexpr Color TITULO_COR   = {100, 160, 255, 255};
constexpr Color INPUT_FUNDO  = {20,  30,  55,  255};
constexpr Color INPUT_ATIVO  = {25,  40,  75,  255};
constexpr Color INPUT_BORDA  = {50,  80,  160, 255};
constexpr Color CHECK_ATIVO  = {30,  80,  180, 255};
constexpr Color CHECK_BORDA  = {80,  110, 180, 255};
constexpr Color BTN_SALVAR   = {30,  100, 60,  255};
constexpr Color BTN_VOLTAR   = {18,  24,  42,  255};
constexpr Color BTN_PASTA    = {28,  42,  80,  255};
constexpr Color BTN_RESTAURAR= {140, 30,  50,  255};
constexpr Color BTN_CONFIG_C = {28,  42,  80,  255};

// =============================================================================
// SHARED RESOURCES (set once in launcher, used everywhere)
// =============================================================================

// gui_common.h
struct GuiResources {
    Texture2D bg_texture;
    Font      fonte;
    Sound     sfx_hover;
    Sound     sfx_click;
};

// =============================================================================
// HELPERS
// =============================================================================

Color clarear(Color cor, int v = 25);

void draw_background(const GuiResources& res);

void draw_text_glow(const Font& fonte, const std::string& texto, int x, int y, float tamanho);


// Texto puro com glow — launcher
bool desenhar_botao_fixo_hover(const Font& fonte, const std::string& label,
                           int x, int y, int larg, int alt, Color cor,
                           bool& out_hover);


bool desenhar_botao_hover(const Font& fonte, const std::string& label,
                           int x, int y, int larg, int alt, Color cor,
                           bool& out_hover);

// Com fundo sólido — config
bool desenhar_botao_filled(const Font& fonte, const std::string& label,
                           int x, int y, int larg, int alt, Color cor);

void desenhar_separador(int y);

std::string truncar_path(const std::string& caminho, int largura_px, int tamanho_fonte = 13);
