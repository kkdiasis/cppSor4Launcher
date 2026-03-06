#pragma once

#include <windows.h>
#include <string>
#include <stdexcept>

// Drop-in replacement para std::filesystem::create_symlink no Windows/MinGW
void create_wsymlink(const std::string& alvo, const std::string& link);