#include <oled.h>
#include <util/delay.h>

// Function to draw a smiley face
void drawSmiley(int x, int y) {
    // Clear the display area where the smiley will be drawn
    oled_drawPixel(x - 10, y - 10, BLACK);
    oled_drawPixel(x + 10, y - 10, BLACK);
    oled_drawPixel(x - 3, y - 3, BLACK);
    oled_drawPixel(x + 3, y - 3, BLACK);
    for (int i = -5; i <= 5; i++) {
        oled_drawPixel(x + i, y + 5, BLACK);
    }

    // Draw face outline (circle)
    for (int i = -10; i <= 10; i++) {
        for (int j = -10; j <= 10; j++) {
            if (i * i + j * j <= 100) { // Radius squared (10^2)
                oled_drawPixel(x + i, y + j, WHITE);
            }
        }
    }

    // Draw eyes
    oled_drawPixel(x - 3, y - 3, BLACK); // Left eye
    oled_drawPixel(x + 3, y - 3, BLACK); // Right eye

    // Draw mouth (smile)
    for (int i = -5; i <= 5; i++) {
        if (i < 0) {
            oled_drawPixel(x + i, y + 5, BLACK); // Left side of smile
        } else {
            oled_drawPixel(x + i, y + 4 - (i * i) / 10, BLACK); // Right side of smile
        }
    }
}

int main(void)
{
    oled_init(OLED_DISP_ON);
    oled_clrscr();

    int x = 0; // Starting x position
    int y = 32; // Fixed y position

    // Main loop to move the smiley face
    while (1) {
        oled_clrscr(); // Clear the screen

        drawSmiley(x, y); // Draw the smiley at the current position

        oled_display(); // Update the display

        // Move the smiley face to the right
        x++;
        if (x > 128) { // Reset to left side after reaching the right edge
            x = -20; // Start off-screen on the left
        }

        // Delay for a smoother animation (adjust as needed)
        _delay_ms(50);
    }

    return 0;
}
