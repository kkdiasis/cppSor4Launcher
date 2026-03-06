CXX      = g++
CC       = gcc
WINDRES  = windres
CXXFLAGS = -std=c++17 -Wall -Wextra -I C:/raylib/raylib/src -MMD -MP
CFLAGS   = -std=c99   -I C:/raylib/raylib/src -MMD -MP
LDFLAGS  = -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32 -lole32 -lstdc++fs -lkernel32 -lshell32 -mwindows

TARGET   = SOR4ModLauncher
BUILDDIR = build

# =============================================================================
# SOURCES
# =============================================================================

SRC_CPP = src/main.cpp \
          src/BigfileManager.cpp \
          src/ModManager.cpp \
          src/GameLauncher.cpp \
          src/config.cpp \
          src/utils.cpp \
          gui/gui_common.cpp \
          gui/gui_launcher.cpp \
          gui/gui_config.cpp

SRC_C   = src/tinyfiledialogs.c
SRC_RC  = src/app.rc

# =============================================================================
# OBJECTS & DEPS
# =============================================================================

OBJ_CPP = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(SRC_CPP))
OBJ_C   = $(patsubst %.c,   $(BUILDDIR)/%.o, $(SRC_C))
OBJ_RC  = $(BUILDDIR)/src/app.res
OBJ     = $(OBJ_CPP) $(OBJ_C) $(OBJ_RC)
DEPS    = $(OBJ_CPP:.o=.d) $(OBJ_C:.o=.d)

# =============================================================================
# BUILD
# =============================================================================

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo.
	@echo Build complete: $(TARGET).exe

$(BUILDDIR)/%.o: %.cpp
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.c
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile resource file
$(BUILDDIR)/src/app.res: $(SRC_RC)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(WINDRES) $< -O coff -o $@

-include $(DEPS)

# =============================================================================
# CLEAN
# =============================================================================

clean:
	@if exist $(BUILDDIR) rmdir /Q /S $(BUILDDIR)
	@del /Q $(TARGET).exe 2>nul
	@echo Cleaned.

.PHONY: all clean
