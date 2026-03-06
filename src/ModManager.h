#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// =============================================================================
// MOD INFO — full metadata (used by both Core and GUI)
// =============================================================================

struct ModInfo {
    std::string nome;     // filename — never changes
    std::string caminho;  // full path
    std::string label;    // display name — editable by user
    bool        ativo;    // shown in launcher menu
};

// =============================================================================
// MOD MANAGER
// =============================================================================

class ModManager {
private:
    std::string        pasta_mods;
    std::vector<ModInfo> mods;

public:
    ModManager(const std::string& pasta);

    // Core operations
    void sanitizar_nomes();
    void listar_mods();
    void iniciar_mod(const fs::path& bf_caminho, const fs::path& mod_caminho);
    void ativar_original(const fs::path& bf_caminho, const fs::path& bf_original);
    void print_mods() const;

    // Persistence
    void carregar_mods(const std::string& mods_json_path);
    void salvar_mods(const std::string& mods_json_path) const;

    // Getters
    const std::vector<ModInfo>& get_mods()        const;  // all mods
    std::vector<ModInfo>        get_mods_ativos()  const;  // only ativo=true (max 5)

    // Setters — used by GUI config screen
    void set_mods(const std::vector<ModInfo>& lista);
    void set_pasta(const std::string& nova_pasta);
};
