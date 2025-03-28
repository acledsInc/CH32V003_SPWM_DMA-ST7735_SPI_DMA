/// \brief ST7735 Driver for CH32V003
/// \author Li Mingjie
///  - Email:  limingjie@outlook.com
///  - GitHub: https://github.com/limingjie/
/// \date Aug 2023
/// \section References
///  - https://github.com/moononournation/Arduino_GFX
///  - https://gitee.com/morita/ch32-v003/tree/master/Driver
///  - https://github.com/cnlohr/ch32v003fun/tree/master/examples/spi_oled
/// \copyright Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0)

#ifndef __ST7735_H__
#define __ST7735_H__

// Note: To not use CS, uncomment the following line and pull CS to ground.
//  #define ST7735_NO_CS

#define RGB565(r, g, b) ((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | ((b) >> 3))
#define BGR565(r, g, b) ((((b)&0xF8) << 8) | (((g)&0xFC) << 3) | ((r) >> 3))
#define RGB         RGB565

#define BLACK       RGB(0, 0, 0)
#define NAVY        RGB(0, 0, 123)
#define DARKGREEN   RGB(0, 125, 0)
#define DARKCYAN    RGB(0, 125, 123)
#define MAROON      RGB(123, 0, 0)
#define PURPLE      RGB(123, 0, 123)
#define OLIVE       RGB(123, 125, 0)
#define LIGHTGREY   RGB(198, 195, 198)
#define DARKGREY    RGB(123, 125, 123)
#define BLUE        RGB(0, 0, 255)
#define GREEN       RGB(0, 255, 0)
#define CYAN        RGB(0, 255, 255)
#define RED         RGB(255, 0, 0)
#define MAGENTA     RGB(255, 0, 255)
#define YELLOW      RGB(255, 255, 0)
#define WHITE       RGB(255, 255, 255)
#define ORANGE      RGB(255, 165, 0)
#define GREENYELLOW RGB(173, 255, 41)
#define PINK        RGB(255, 130, 198)

/// \brief Initialize ST7735
void tft_init(void);

/// \brief Set Cursor Position for Print Functions
/// \param x X coordinate, from left to right.
/// \param y Y coordinate, from top to bottom.
void tft_set_cursor(uint16_t x, uint16_t y);

/// \brief Set Text Color
/// \param color Text color
void tft_set_color(uint16_t color);

/// \brief Set Text Background Color
/// \param color Text background color
void tft_set_background_color(uint16_t color);

/// \brief Print a Character
/// \param c Character to print
void tft_print_char(char c);

/// \brief Print a String
/// \param str String to print
void tft_print(const char* str);

/// \brief Print an Integer
/// \param num Number to print
/// \param width Expected width of the number.
/// Align left if it is less than the width of the number.
/// Align right if it is greater than the width of the number.
void tft_print_number(int32_t num, uint16_t width);

/// \brief Draw a Pixel
/// \param x X
/// \param y Y
/// \param color Pixel color
void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/// \brief Draw a Line
/// \param x0 Start X coordinate
/// \param y0 Start Y coordinate
/// \param x1 End X coordinate
/// \param y1 End Y coordinate
/// \param color Line color
void tft_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

/// \brief Draw a Rectangle
/// \param x Start X coordinate
/// \param y Start Y coordinate
/// \param width Width
/// \param height Height
/// \param color Rectangle Color
void tft_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

/// \brief Fill a Rectangle Area
/// \param x Start X coordinate
/// \param y Start Y coordinate
/// \param width Width
/// \param height Height
/// \param color Fill Color
void tft_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

/// \brief Draw a Bitmap
/// \param x Start X coordinate
/// \param y Start Y coordinate
/// \param width Width
/// \param height Height
/// \param bitmap Bitmap
void tft_draw_bitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* bitmap);

#endif  // __ST7735_H__
