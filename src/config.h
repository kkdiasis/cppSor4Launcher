#pragma once

#include <string>
#include "json.hpp"

using json = nlohmann::json;

void criar_config_padrao(const std::string& exe_path);
json carregar_config(const std::string& path, const std::string& exe_path);
void salvar_config(const std::string& path, const json& config);
