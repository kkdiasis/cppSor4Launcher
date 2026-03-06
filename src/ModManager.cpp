#include "ModManager.h"
#include "utils.h"
#include "json.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

ModManager::ModManager(const string& pasta) : pasta_mods(pasta) {}

// =============================================================================
// CORE OPERATIONS
// =============================================================================

void ModManager::sanitizar_nomes() {
    for (const auto& item : fs::directory_iterator(pasta_mods)) {
        if (!item.is_regular_file()) continue;

        string nome     = item.path().filename().string();
        string nome_new = nome;

        replace(nome_new.begin(), nome_new.end(), ' ', '-');
        nome_new.erase(remove(nome_new.begin(), nome_new.end(), '('), nome_new.end());
        nome_new.erase(remove(nome_new.begin(), nome_new.end(), ')'), nome_new.end());

        while (nome_new.find("--") != string::npos)
            nome_new.replace(nome_new.find("--"), 2, "-");

        if (nome_new != nome) {
            fs::rename(item.path(), item.path().parent_path() / nome_new);
            cout << "Renamed: " << nome << " -> " << nome_new << endl;
        }
    }
}

void ModManager::listar_mods() {
    // Preserve existing labels and ativo flags when reloading
    map<string, ModInfo> salvos;
    for (auto& m : mods)
        salvos[m.caminho] = m;

    mods.clear();

    if (!fs::exists(pasta_mods) || !fs::is_directory(pasta_mods)) {
        cerr << "Mods folder not found: " << pasta_mods << endl;
        return;
    }

    for (const auto& item : fs::directory_iterator(pasta_mods)) {
        if (!item.is_regular_file()) continue;

        string caminho = item.path().string();
        string nome    = item.path().filename().string();

        if (salvos.count(caminho)) {
            mods.push_back(salvos[caminho]);
        } else {
            mods.push_back({nome, caminho, nome, false});
        }
    }
}

void ModManager::iniciar_mod(const fs::path& bf_caminho, const fs::path& mod_caminho) {
    if (!fs::exists(mod_caminho) || !fs::is_regular_file(mod_caminho)) {
        cerr << "Mod not found: " << mod_caminho.string() << endl;
        return;
    }

    if (fs::exists(bf_caminho) || fs::is_symlink(bf_caminho))
        fs::remove(bf_caminho);

    create_wsymlink(mod_caminho.string(), bf_caminho.string());
    cout << "Mod activated: " << mod_caminho.filename().string() << endl;
}

void ModManager::ativar_original(const fs::path& bf_caminho, const fs::path& bf_original) {
    if (!fs::exists(bf_original)) {
        cerr << "Error: bigfile_original not found." << endl;
        return;
    }

    if (fs::exists(bf_caminho) || fs::is_symlink(bf_caminho))
        fs::remove(bf_caminho);

    create_wsymlink(bf_original.string(), bf_caminho.string());
    cout << "Original activated." << endl;
}

void ModManager::print_mods() const {
    for (int i = 0; i < (int)mods.size(); i++)
        cout << i + 1 << ". [" << (mods[i].ativo ? "x" : " ") << "] "
             << mods[i].label << endl;
}

// =============================================================================
// PERSISTENCE
// =============================================================================

void ModManager::carregar_mods(const string& mods_json_path) {
    ifstream f(mods_json_path);
    if (!f.is_open()) return;

    json dados;
    f >> dados;

    map<string, json> salvos;
    for (auto& item : dados)
        salvos[item["caminho"]] = item;

    for (auto& mod : mods) {
        if (salvos.count(mod.caminho)) {
            auto& s   = salvos[mod.caminho];
            mod.label = s.value("label", mod.nome);
            mod.ativo = s.value("ativo", false);
        }
    }
}

void ModManager::salvar_mods(const string& mods_json_path) const {
    json dados = json::array();
    for (auto& mod : mods) {
        dados.push_back({
            {"nome",    mod.nome},
            {"caminho", mod.caminho},
            {"label",   mod.label},
            {"ativo",   mod.ativo},
        });
    }
    ofstream f(mods_json_path);
    f << dados.dump(4);
    cout << "Mods saved to: " << mods_json_path << endl;
}

// =============================================================================
// GETTERS / SETTERS
// =============================================================================

const vector<ModInfo>& ModManager::get_mods() const {
    return mods;
}

vector<ModInfo> ModManager::get_mods_ativos() const {
    vector<ModInfo> ativos;
    for (auto& m : mods) {
        if (m.ativo) ativos.push_back(m);
        if ((int)ativos.size() == 5) break;
    }
    return ativos;
}

void ModManager::set_mods(const vector<ModInfo>& lista) {
    mods = lista;
}

void ModManager::set_pasta(const std::string& nova_pasta) {
    pasta_mods = nova_pasta;
    mods.clear();
}
