#ifndef HERMIONE_H
#define HERMIONE_H

#include "raylib.h"

struct Hermione {
    int r, c; // Hermione's row and column position
    Texture2D texture; // Hermione texture
};

// Function declarations
void InitHermione(Hermione& hermione, const char* texturePath, int row, int col);
void DrawHermione(const Hermione& hermione, int cellSize);

#endif // HERMIONE_H
