#pragma once

#include <string>

class GameLauncher {
private:
    std::string steam_app_id;

public:
    GameLauncher(const std::string& app_id);

    void lancar() const;
};
