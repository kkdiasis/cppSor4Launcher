#include "gui_common.h"
#include <algorithm>

Color clarear(Color cor, int v) {
    return {
        (unsigned char)std::min(cor.r + v, 255),
        (unsigned char)std::min(cor.g + v, 255),
        (unsigned char)std::min(cor.b + v, 255),
        255
    };
}

void draw_background(const GuiResources& res) {
    ClearBackground(FUNDO);
    DrawTexture(res.bg_texture, 0, 70, WHITE);
    DrawRectangle(0, 0, WIN_WIDTH, WIN_HEIGHT, {0, 0, 0, 100});
    DrawRectangleGradientH(25, 0, FAIXA_LARG, WIN_HEIGHT,
        {0, 0, 0, 255}, {0, 0, 0, 200});
}

void draw_text_glow(const Font& fonte, const std::string& texto, int x, int y, float tamanho) {
    const char* t = texto.c_str();
    DrawTextEx(fonte, t, {(float)(x-4), (float)(y-4)}, tamanho+8, 1, {255, 100,  0,  20});
    DrawTextEx(fonte, t, {(float)(x-3), (float)(y-3)}, tamanho+6, 1, {255, 140,  0,  20});
    DrawTextEx(fonte, t, {(float)(x-2), (float)(y-2)}, tamanho+4, 1, {255, 180,  0,  20});
    DrawTextEx(fonte, t, {(float)(x-1), (float)(y-1)}, tamanho+2, 1, {255, 220,  0,  40});
    DrawTextEx(fonte, t, {(float)x,     (float)y    }, tamanho,   1, {255, 255, 100, 255});
}

bool desenhar_botao(const Font& fonte, const std::string& label,
                    int x, int y, int larg, int alt, Color cor) {
    Rectangle rect  = {(float)x, (float)y, (float)larg, (float)alt};
    Vector2   mouse = GetMousePosition();
    bool      hover = CheckCollisionPointRec(mouse, rect);

    int   tamanho  = hover ? 50 : 30;
    int   offset_x = hover ? 20 : 0;

    if (hover) {
        draw_text_glow(fonte, label,
            x + 18 + offset_x,
            y + alt / 2 - tamanho / 2,
            (float)tamanho);
    } else {
        DrawTextEx(fonte, label.c_str(),
            {(float)(x + 18), (float)(y + alt / 2 - tamanho / 2)},
            (float)tamanho, 1, WHITE);
    }

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool desenhar_botao_hover(const Font& fonte, const std::string& label,
                           int x, int y, int larg, int alt, Color cor,
                           bool& out_hover) {
    Rectangle rect = {(float)x, (float)y, (float)larg, (float)alt};
    out_hover      = CheckCollisionPointRec(GetMousePosition(), rect);

    int tamanho  = out_hover ? 70 : 50;
    int offset_x = out_hover ? 30 : 0;

    if (out_hover) {
        draw_text_glow(fonte, label,
            x + 18 + offset_x,
            y + alt / 2 - tamanho / 2,
            (float)tamanho);
    } else {
        DrawTextEx(fonte, label.c_str(),
            {(float)(x + 18), (float)(y + alt / 2 - tamanho / 2)},
            (float)tamanho, 1, WHITE);
    }

    return out_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}


bool desenhar_botao_filled(const Font& fonte, const std::string& label,
                           int x, int y, int larg, int alt, Color cor) {
    Rectangle rect  = {(float)x, (float)y, (float)larg, (float)alt};
    Vector2   mouse = GetMousePosition();
    bool      hover = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRec(rect, hover ? clarear(cor) : cor);
    DrawTextEx(fonte, label.c_str(),
        {(float)(x + 14), (float)(y + alt / 2 - 10)}, 20, 1, TEXTO);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}



void desenhar_separador(int y) {
    DrawRectangle(MARGEM, y, WIN_WIDTH - MARGEM * 2, 1, SEPARADOR);
}

std::string truncar_path(const std::string& caminho, int largura_px, int tamanho_fonte) {
    int chars_max = largura_px / (tamanho_fonte / 2 + 3);

    if ((int)caminho.size() <= chars_max) return caminho;

    fs::path p(caminho);
    auto partes = p;

    std::string inicio = p.root_path().string();
    std::string fim    = (p.parent_path().filename() / p.filename()).string();
    std::string result = inicio + "\\...\\" + fim;

    if ((int)result.size() > chars_max)
        result = "...\\" + fim;

    return result;
}
