#include "GameLauncher.h"
#include <iostream>
#include <windows.h>

using namespace std;

GameLauncher::GameLauncher(const string& app_id)
    : steam_app_id(app_id)
{}

void GameLauncher::lancar() const {
    string url = "steam://rungameid/" + steam_app_id;
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    cout << "Launching game via Steam (app id: " << steam_app_id << ")..." << endl;
}
