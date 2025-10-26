#ifndef DISPLAY_H_
#define DISPLAY_H_

typedef enum { RED = 0xFF00000, GREEN = 0x00FF00, BLUE = 0x0000FF, YELLOW = 0xFFFF00, ORANGE = 0xFFA500, CYAN = 0x00FFFF, MAGENTA = 0xFF00FF, WHITE = 0xFFFFFF, OFF= 0x000000} Color_t;

void DisplayEnable(void);
void DisplayPrint(const int line, const char * msg, ...);
void DisplayColor(Color_t color);

void UpdateDisplay(void);

#endif /* DISPLAY_H_ */
