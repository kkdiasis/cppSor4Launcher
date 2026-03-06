#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class BigfileManager {
private:
    std::string original;
    std::string mod_original;
    std::string bkp;
    fs::path    caminho;
    fs::path    pasta_bkp;

public:
    BigfileManager(const std::string& caminho_jogo, const std::string& caminho_bkp);

    void preparar_ambiente();
    void restauracao_completa();

    fs::path get_bigfile()          const;
    fs::path get_bigfile_original() const;
};
