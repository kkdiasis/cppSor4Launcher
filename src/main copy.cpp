#include <iostream>
#include <string>
#include "BigfileManager.h"
#include "ModManager.h"
#include "GameLauncher.h"
#include "config.h"

using namespace std;
namespace fs = std::filesystem;

#define STEAM_APP_ID "985890" // Streets of Rage 4

// =============================================================================
// MENU
// =============================================================================

void print_menu() {
    cout << endl;
    cout << "==============================" << endl;
    cout << "   SOR4 Mod Launcher — CLI    " << endl;
    cout << "==============================" << endl;
    cout << " 1. Prepare environment       " << endl;
    cout << " 2. List mods                 " << endl;
    cout << " 3. Activate mod + launch     " << endl;
    cout << " 4. Activate original + launch" << endl;
    cout << " 5. Sanitize mod names        " << endl;
    cout << " 6. Restore original          " << endl;
    cout << " 0. Exit                      " << endl;
    cout << "==============================" << endl;
    cout << "Choice: ";
}

// =============================================================================
// ENTRY POINT
// =============================================================================

int main(int argc, char* argv[]) {
    try {
        criar_config_padrao(argv[0]);
        json config = carregar_config("cfg\\config.json", argv[0]);

        string caminho_jogo = config["game"]["caminho"];
        string caminho_bkp  = config["game"]["backup"];
        string caminho_mods = config["mods"]["caminho"];

        // Create mods folder if it doesn't exist
        if (!fs::exists(caminho_mods)) {
            fs::create_directories(caminho_mods);
            cout << "Mods folder created at: " << caminho_mods << endl;
        }

        BigfileManager bigfile_manager(caminho_jogo, caminho_bkp);
        ModManager     mod_manager(caminho_mods);
        GameLauncher   launcher(STEAM_APP_ID);

        int opcao = -1;

        while (opcao != 0) {
            print_menu();
            cin >> opcao;

            switch (opcao) {

                case 1:
                    cout << endl << "--- Prepare environment ---" << endl;
                    bigfile_manager.preparar_ambiente();
                    break;

                case 2:
                    cout << endl << "--- Mod list ---" << endl;
                    mod_manager.listar_mods();
                    mod_manager.print_mods();
                    if (mod_manager.get_mods().empty())
                        cout << "No mods found in: " << caminho_mods << endl;
                    break;

                case 3: {
                    cout << endl << "--- Activate mod + launch ---" << endl;
                    mod_manager.listar_mods();
                    mod_manager.print_mods();

                    if (mod_manager.get_mods().empty()) {
                        cout << "No mods available." << endl;
                        break;
                    }

                    cout << "Enter mod number: ";
                    int idx;
                    cin >> idx;

                    const auto& mods = mod_manager.get_mods();
                    if (idx < 1 || idx > (int)mods.size()) {
                        cout << "Invalid selection." << endl;
                        break;
                    }

                    mod_manager.iniciar_mod(
                        bigfile_manager.get_bigfile(),
                        fs::path(mods[idx - 1].caminho)
                    );
                    launcher.lancar();
                    break;
                }

                case 4:
                    cout << endl << "--- Activate original + launch ---" << endl;
                    mod_manager.ativar_original(
                        bigfile_manager.get_bigfile(),
                        bigfile_manager.get_bigfile_original()
                    );
                    launcher.lancar();
                    break;

                case 5:
                    cout << endl << "--- Sanitize mod names ---" << endl;
                    mod_manager.sanitizar_nomes();
                    cout << "Done." << endl;
                    break;

                case 6:
                    cout << endl << "--- Restore original ---" << endl;
                    bigfile_manager.restauracao_completa();
                    break;

                case 0:
                    cout << "Goodbye!" << endl;
                    break;

                default:
                    cout << "Invalid option." << endl;
                    break;
            }
        }

    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
