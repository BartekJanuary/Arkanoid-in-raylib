#include "raylib.h"

const int screenWidth = 800;
const int screenHeight = 700;
const int paddleWidth = 100;
const int paddleHeight = 20;
const int ballRadius = 10;
const int brickWidth = 78;
const int brickHeight = 30;
const int brickRows = 6; // Increased number of rows
const int brickCols = 10;
const int brickSpacingY = 60; // Przerwa pomiędzy rzędami klocków

const int maxLives = 3;
int currentLives = maxLives;

struct Ball {
    Vector2 position;
    Vector2 speed;
};

struct Paddle {
    Rectangle rect;
};

struct Brick {
    Rectangle rect;
    bool active;
    Color color; // Added color property
};

Texture2D heartTexture;

Ball ball;
Paddle paddle;
Brick bricks[brickRows][brickCols];

bool gameWon = false;
Rectangle resetButton = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50 };

void InitGame() {
    gameWon = false;
    currentLives = maxLives;
    ball.position = { screenWidth / 2, screenHeight / 2 };
    ball.speed = { 4, 4 };

    paddle.rect = { screenWidth / 2 - paddleWidth / 2, screenHeight - 50, paddleWidth, paddleHeight };

    // Initialize bricks with random colors
    for (int i = 0; i < brickRows; i++) {
        for (int j = 0; j < brickCols; j++) {
            bricks[i][j].rect = { j * (brickWidth + 2) + 2, i * (brickHeight + 2) + brickSpacingY, brickWidth, brickHeight };
            bricks[i][j].active = true;
            bricks[i][j].color = { GetRandomValue(50, 255), GetRandomValue(50, 255), GetRandomValue(50, 255), 255 };
        }
    }

    // Load heart texture
    heartTexture = LoadTexture("heart.png");
}

void UpdateGame() {
    if (!gameWon) {
        // Move the paddle
        if (IsKeyDown(KEY_LEFT) && paddle.rect.x > 0)
            paddle.rect.x -= 10;
        if (IsKeyDown(KEY_RIGHT) && paddle.rect.x + paddle.rect.width < screenWidth)
            paddle.rect.x += 10;

        // Move the ball
        ball.position.x += ball.speed.x;
        ball.position.y += ball.speed.y;

        // Check for collisions with walls
        if (ball.position.x + ballRadius >= screenWidth || ball.position.x - ballRadius <= 0)
            ball.speed.x *= -1;
        if (ball.position.y - ballRadius <= 0)
            ball.speed.y *= -1;

        // Check for collision with paddle
        if (CheckCollisionCircleRec(ball.position, ballRadius, paddle.rect)) {
            ball.speed.y *= -1;
        }

        // Check for collisions with bricks
        bool allBricksDestroyed = true;
        for (int i = 0; i < brickRows; i++) {
            for (int j = 0; j < brickCols; j++) {
                if (bricks[i][j].active && CheckCollisionCircleRec(ball.position, ballRadius, bricks[i][j].rect)) {
                    ball.speed.y *= -1;
                    bricks[i][j].active = false;
                }
                if (bricks[i][j].active) {
                    allBricksDestroyed = false;
                }
            }
        }

        // Check for ball falling out of screen
        if (ball.position.y >= screenHeight + ballRadius) {
            currentLives--;
            if (currentLives <= 0) {
                InitGame(); // Restart the game
            } else {
                ball.position = { screenWidth / 2, screenHeight / 2 };
                ball.speed = { 4, 4 };
            }
        }

        // Check for game won
        if (allBricksDestroyed) {
            gameWon = true;
        }
    } else {
        // Check for click on reset button
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePoint = GetMousePosition();
            if (CheckCollisionPointRec(mousePoint, resetButton)) {
                InitGame(); // Restart the game
            }
        }
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (!gameWon) {
        // Draw the paddle
        DrawRectangleRec(paddle.rect, BLUE);

        // Draw the ball
        DrawCircleV(ball.position, ballRadius, RED);

        // Draw bricks
        for (int i = 0; i < brickRows; i++) {
            for (int j = 0; j < brickCols; j++) {
                if (bricks[i][j].active) {
                    DrawRectangleRec(bricks[i][j].rect, bricks[i][j].color);
                }
            }
        }

        // Draw hearts
        for (int i = 0; i < maxLives; i++) {
            if (i < currentLives) {
                DrawTexture(heartTexture, 10 + i * (heartTexture.width + 5), 10, WHITE);
            } else {
                DrawTexture(heartTexture, 10 + i * (heartTexture.width + 5), 10, Fade(WHITE, 0.3f));
            }
        }
    } else {
        // Draw "You Win" message
        DrawText("YOU WIN!", screenWidth / 2 - MeasureText("YOU WIN!", 40) / 2, screenHeight / 2 - 50, 40, RED);

        // Draw reset button
        DrawRectangleLinesEx(resetButton, 2, RED);
        DrawText("Reset", resetButton.x + 50, resetButton.y + 10, 20, BLACK);
    }

    EndDrawing();
}

int main() {
    InitWindow(screenWidth, screenHeight, "Arkanoid");
    SetTargetFPS(60);

    InitGame();

    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    CloseWindow();

    UnloadTexture(heartTexture); // Unload heart texture when not needed

    return 0;
}
