#pragma once

#include "gui_common.h"
#include "../src/BigfileManager.h"
#include "../src/ModManager.h"
#include "../src/GameLauncher.h"

void run_launcher(GuiResources& res,
                  BigfileManager& bigfile_manager,
                  ModManager&     mod_manager,
                  GameLauncher&   launcher,
                  const std::string& exe_path);
