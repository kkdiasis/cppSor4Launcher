#include "config.h"
#include <fstream>
#include <stdexcept>
#include <windows.h>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

static fs::path exe_dir(const string& exe_path) {
    return fs::absolute(fs::path(exe_path)).parent_path();
}


void criar_config_padrao(const string& exe_path) {
    fs::path base     = exe_dir(exe_path);
    fs::path cfg_path = base / "cfg";
    fs::path cfg_file = cfg_path / "config.json";

    if (fs::exists(cfg_file)) return;

    fs::create_directories(cfg_path);

    json config = {
        {"game", {
            {"caminho", "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Streets of Rage 4\\data"},
            {"backup",  "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Streets of Rage 4\\data\\bkp"}
        }},
        {"mods", {
            {"caminho", (base / "mods").string()}
        }}
    };

    ofstream arquivo(cfg_file);
    arquivo << config.dump(4);
    cout << "config.json created with defaults at: " << cfg_file << endl;
}



json carregar_config(const string& path, const string& exe_path) {
    fs::path full_path = exe_dir(exe_path) / path;

    ifstream arquivo(full_path);
    if (!arquivo.is_open()) {
        throw runtime_error("config.json not found at: " + full_path.string());
    }
    json config;
    arquivo >> config;
    return config;
}

void salvar_config(const string& path, const json& config) {
    ofstream f(exe_dir(path) / path);
    f << config.dump(4);
}