#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include "hermione.h"
Hermione hermione; // Create Hermione object


const int nrows = 20; // Number of rows
const int ncols = 20; // Number of columns
const int cellSize = 30; // Size of each cell
const int numGhosts = 5; // Number of ghosts

// Cell structure definition
struct Cell {
    bool visited = false;
    bool walls[4] = { true, true, true, true }; // Top, Right, Bottom, Left
    int r, c; // Row and Column
    std::vector<Cell*> neighbors; // Neighboring cells
};

// Player structure definition
struct Player {
    int r, c; // Player's row and column position
    Texture2D texture; // Player texture
};

// Ghost structure definition
struct Ghost {
    int r, c; // Ghost's row and column position
    Color color; // Ghost color
};

// Global variables
std::vector<Cell> grid(nrows * ncols);
Cell* current = nullptr;
std::vector<Cell*> stack;
bool play = false;
Player player = { 0, 0 }; // Initialize player at the starting position
std::vector<Ghost> ghosts; // List of ghosts

// Function to initialize the grid
void InitGrid() {
    for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
            grid[r * ncols + c].r = r;
            grid[r * ncols + c].c = c;
            if (r > 0) grid[r * ncols + c].neighbors.push_back(&grid[(r - 1) * ncols + c]); // Top neighbor
            if (c < ncols - 1) grid[r * ncols + c].neighbors.push_back(&grid[r * ncols + (c + 1)]); // Right neighbor
            if (r < nrows - 1) grid[r * ncols + c].neighbors.push_back(&grid[(r + 1) * ncols + c]); // Bottom neighbor
            if (c > 0) grid[r * ncols + c].neighbors.push_back(&grid[r * ncols + (c - 1)]); // Left neighbor
        }
    }
}

void DrawMaze() {
    for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
            Cell& cell = grid[r * ncols + c];
            // Draw cell background
            if (cell.visited) {
                DrawRectangle(c * cellSize, r * cellSize, cellSize, cellSize, DARKGRAY);
            }
            // Draw walls
            if (cell.walls[0]) DrawLine(c * cellSize, r * cellSize, (c + 1) * cellSize, r * cellSize, BLACK); // Top
            if (cell.walls[1]) DrawLine((c + 1) * cellSize, r * cellSize, (c + 1) * cellSize, (r + 1) * cellSize, BLACK); // Right
            if (cell.walls[2]) DrawLine(c * cellSize, (r + 1) * cellSize, (c + 1) * cellSize, (r + 1) * cellSize, BLACK); // Bottom
            if (cell.walls[3]) DrawLine(c * cellSize, r * cellSize, c * cellSize, (r + 1) * cellSize, BLACK); // Left
        }
    }

    // Draw the player's frame
    int playerFrameX = player.c * cellSize; // X position of the player's frame
    int playerFrameY = player.r * cellSize; // Y position of the player's frame
    DrawRectangle(playerFrameX, playerFrameY, cellSize-1, cellSize-1, DARKGRAY); // Frame background

    // Draw the player texture with scaling
    float scale = 0.08f; // Scale the player texture to 5% of original size
    DrawTextureEx(player.texture,
                   Vector2{playerFrameX + (cellSize - player.texture.width * scale) / 2,
                            playerFrameY + (cellSize - player.texture.height * scale) / 2},
                   0.0f, // No rotation
                   scale, // Scale factor
                   WHITE); // Color tint

    // Draw ghosts
    for (const Ghost& ghost : ghosts) {
        DrawRectangle(ghost.c * cellSize, ghost.r * cellSize, cellSize, cellSize, ghost.color);
    }
}

void MazeGenerator() {
    if (!current->visited) {
        current->visited = true;
        stack.push_back(current);
    }

    std::vector<Cell*> unvisited;
    for (Cell* neigh : current->neighbors) {
        if (!neigh->visited) {
            unvisited.push_back(neigh);
        }
    }

    if (!unvisited.empty()) {
        int randomIndex = std::rand() % unvisited.size();
        Cell* next = unvisited[randomIndex];

        // Remove walls
        if (current->r - next->r == 1) { // Top
            current->walls[0] = false;
            next->walls[2] = false;
        } else if (current->c - next->c == 1) { // Left
            current->walls[3] = false;
            next->walls[1] = false;
        } else if (current->r - next->r == -1) { // Bottom
            current->walls[2] = false;
            next->walls[0] = false;
        } else if (current->c - next->c == -1) { // Right
            current->walls[1] = false;
            next->walls[3] = false;
        }

        current = next;
    } else if (!stack.empty()) {
        current = stack.back();
        stack.pop_back();
    } else {
        play = true; // Maze generation completed
    }
}

void UpdatePlayer() {
    // Check for player movement
    if (IsKeyDown(KEY_UP) && !grid[player.r * ncols + player.c].walls[0]) { // Up
        player.r--;
    }
    if (IsKeyDown(KEY_RIGHT) && !grid[player.r * ncols + player.c].walls[1]) { // Right
        player.c++;
    }
    if (IsKeyDown(KEY_DOWN) && !grid[player.r * ncols + player.c].walls[2]) { // Down
        player.r++;
    }
    if (IsKeyDown(KEY_LEFT) && !grid[player.r * ncols + player.c].walls[3]) { // Left
        player.c--;
        // Check for collision with Hermione
    if (player.r == hermione.r && player.c == hermione.c) {
        // Display a winning message
        DrawText("You Win!", ncols * cellSize / 2 - MeasureText("You Win!", 20) / 2, nrows * cellSize / 2 - 10, 20, RED);
        // Optionally, you can set `play` to false or exit the game here
    }
    }
}

// Function to initialize ghosts
void InitGhosts() {
    for (int i = 0; i < numGhosts; i++) {
        Ghost ghost;
        ghost.r = std::rand() % nrows;
        ghost.c = std::rand() % ncols;
        ghost.color = (i % 2 == 0) ? RED : BLUE; // Alternate colors for ghosts
        ghosts.push_back(ghost);
    }
}

// Function to update ghosts
void UpdateGhosts() {
    for (Ghost& ghost : ghosts) {
        int direction = std::rand() % 4; // Random direction: 0 = up, 1 = right, 2 = down, 3 = left

        int newR = ghost.r;
        int newC = ghost.c;

        switch (direction) {
            case 0: newR--; break; // Up
            case 1: newC++; break; // Right
            case 2: newR++; break; // Down
            case 3: newC--; break; // Left
        }

        // Check for valid move
        if (newR >= 0 && newR < nrows && newC >= 0 && newC < ncols && 
            !grid[ghost.r * ncols + ghost.c].walls[direction]) {
            ghost.r = newR;
            ghost.c = newC;
        }

        // Check for collision with player
        if (ghost.r == player.r && ghost.c == player.c) {
            // Reset player to initial position
            player.r = 0;
            player.c = 0;
        }
    }
}

int main() {
    InitWindow(ncols * cellSize, nrows * cellSize, "Maze Generator");
    SetTargetFPS(60);
    std::srand(static_cast<unsigned>(std::time(0))); // Seed random number generator

    InitGrid(); // Initialize the grid
    current = &grid[0]; // Start from the first cell

    // Load player texture
    player.texture = LoadTexture("src/harry.png"); // Ensure this path is correct
     hermione.texture = LoadTexture("src/hermione.png");
    hermione.r = nrows - 1; // Set Hermione's position to the last row
    hermione.c = ncols - 1; // Set Hermione's position to the last column
    InitGhosts(); // Initialize ghosts
InitHermione(hermione, "src/hermione.png", 19, 19); 
    while (!WindowShouldClose()) {
        if (!play) {
            MazeGenerator(); // Generate the maze
        } else {
            UpdatePlayer(); // Update player position
            UpdateGhosts(); // Update ghosts
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawMaze(); // Draw the maze
         DrawHermione(hermione, cellSize); // Draw Hermione
        EndDrawing();
    }

    // Unload player

    // Unload player texture
    UnloadTexture(player.texture);
    CloseWindow();
    return 0;
}
