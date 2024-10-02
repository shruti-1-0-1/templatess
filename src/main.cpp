#include <raylib.h>

const int mazeWidth = 20;
const int mazeHeight = 20;
const int cellSize = 35;
const float playerSpeed = 150.0f;  // Player movement speed (adjusted for smooth movement)

// Maze layout (1 = wall, 0 = path, 2 = win position)
int maze[mazeHeight][mazeWidth] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    { 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// Player starting position (in terms of actual coordinates, not cells)
Vector2 playerPosition = {cellSize * 1.5f, cellSize * 1.5f};  // Starting in the center of the first cell (1,1)

bool IsValidPosition(Vector2 position) {
    int cellX = (int)(position.x / cellSize);
    int cellY = (int)(position.y / cellSize);
    return (cellX >= 0 && cellX < mazeWidth && cellY >= 0 && cellY < mazeHeight && maze[cellY][cellX] != 1);
}

void MovePlayer(Vector2* position, Vector2 direction, float deltaTime) {
    Vector2 newPosition = {position->x + direction.x * playerSpeed * deltaTime,
                           position->y + direction.y * playerSpeed * deltaTime};

    // Only move if the new position is valid
    if (IsValidPosition(newPosition)) {
        // Check if the new position is on the edge of a cell
        int newCellX = (int)(newPosition.x / cellSize);
        int newCellY = (int)(newPosition.y / cellSize);
        int oldCellX = (int)(position->x / cellSize);
        int oldCellY = (int)(position->y / cellSize);

        // If the new position is on the edge of a cell, move to the center of the cell
        if (newCellX != oldCellX || newCellY != oldCellY) {
            newPosition.x = newCellX * cellSize + cellSize / 2;
            newPosition.y = newCellY * cellSize + cellSize / 2;
        }

        *position = newPosition;
    }
}

int main() {
    InitWindow(mazeWidth * cellSize, mazeHeight * cellSize, "Smooth Maze Game");
    SetTargetFPS(60);

    bool gameWon = false;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        Vector2 direction = {0.0f, 0.0f};

        // Smooth movement based on arrow key inputs
        if (IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;
        if (IsKeyDown(KEY_LEFT)) direction.x -= 1.0f;
        if (IsKeyDown(KEY_DOWN)) direction.y += 1.0f;
        if (IsKeyDown(KEY_UP)) direction.y -= 1.0f;

        // Move the player smoothly
        MovePlayer(&playerPosition, direction, deltaTime);

        // Check if player reached the win position
        int playerCellX = (int)(playerPosition.x / cellSize);
        int playerCellY = (int)(playerPosition.y / cellSize);
        if (maze[playerCellY][playerCellX] == 2) {
            gameWon = true;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw maze
        for (int y = 0; y < mazeHeight; y++) {
            for (int x = 0; x < mazeWidth; x++) {
                if (maze[y][x] == 1) {
                    DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, DARKGRAY);
                } else if (maze[y][x] == 2) {
                    DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, GOLD);  // Win position
                } else {
                    DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, LIGHTGRAY);
                }
            }
        }

        // Draw player (ball) centered in the current cell
        DrawCircle((int)playerPosition.x, (int)playerPosition.y, cellSize / 3, BLUE);

        // Display win message if player wins
        if (gameWon) {
            DrawText("You Win!", mazeWidth * cellSize / 2 - 50, mazeHeight * cellSize / 2 - 10, 20, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}