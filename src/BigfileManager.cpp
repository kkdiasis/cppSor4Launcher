#include "BigfileManager.h"
#include "utils.h"
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

BigfileManager::BigfileManager(const string& caminho_jogo, const string& caminho_bkp)
    : original("bigfile"),
      mod_original("bigfile_original"),
      bkp("bigfile_bkp"),
      caminho(caminho_jogo),
      pasta_bkp(caminho_bkp)
{}

void BigfileManager::preparar_ambiente() {
    fs::path bf          = caminho / original;
    fs::path bf_original = caminho / mod_original;
    fs::path bf_bkp      = caminho / bkp;

    // Subsequent runs — structure already in place
    if (fs::is_symlink(bf) && fs::exists(bf_bkp) && fs::exists(bf_original)) {
        cout << "Environment already configured." << endl;
        return;
    }

    // First run — bigfile exists as a regular file
    if (fs::exists(bf) && fs::is_regular_file(bf)) {
        cout << "File found. First run — creating backup..." << endl;

        try {
            fs::copy(bf, bf_bkp);

            if (!fs::exists(pasta_bkp)) {
                fs::create_directories(pasta_bkp);
            }
            fs::copy(bf_bkp, pasta_bkp / bkp, fs::copy_options::skip_existing);

            fs::rename(bf, bf_original);

            string bf_str = caminho.string();
            create_wsymlink(
                bf_str + "\\" + mod_original,
                bf_str + "\\" + original
            );

            cout << "Environment ready." << endl;

        } catch (const exception& e) {
            cerr << "Error preparing environment: " << e.what() << endl;
        }

        return;
    }

    cerr << "Error: bigfile not found or corrupted. Check the game path." << endl;
}

void BigfileManager::restauracao_completa() {
    fs::path bf          = caminho / original;
    fs::path bf_original = caminho / mod_original;
    fs::path bf_bkp      = caminho / bkp;

    if (fs::exists(bf_bkp)) {
        if (fs::exists(bf) || fs::is_symlink(bf))
            fs::remove(bf);
        if (fs::exists(bf_original))
            fs::remove(bf_original);

        fs::rename(bf_bkp, bf);
        cout << "Restore complete." << endl;
    } else {
        cerr << "Error: backup file not found." << endl;
    }
}

fs::path BigfileManager::get_bigfile() const {
    return caminho / original;
}

fs::path BigfileManager::get_bigfile_original() const {
    return caminho / mod_original;
}
