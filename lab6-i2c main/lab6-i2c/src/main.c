// https://github.com/Sylaina/oled-display

#include <oled.h>

int main(void)
{
    oled_init(OLED_DISP_ON);
    oled_clrscr();

    oled_charMode(DOUBLESIZE);
    oled_puts("TO NEDAM");

    oled_charMode(NORMALSIZE);

    // oled_gotoxy(x, y)
    oled_gotoxy(0, 2);
    oled_puts("Tohle je peklo");

    // oled_drawLine(x1, y1, x2, y2, color)
    oled_drawLine(0, 25, 120, 25, WHITE);

    oled_gotoxy(0, 4);
    oled_puts("JA CHCI DOMU");

    // Copy buffer to display RAM
    oled_display();

    while (1) {
        ;
    }

    return 0;
}