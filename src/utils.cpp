#include "utils.h"

void create_wsymlink(const std::string& alvo, const std::string& link) {
    DWORD flags = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

    if (!CreateSymbolicLinkA(link.c_str(), alvo.c_str(), flags)) {
        throw std::runtime_error("Falha ao criar symlink. Erro: " + 
                                  std::to_string(GetLastError()));
    }
}