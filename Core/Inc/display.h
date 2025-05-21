/*
 * display.h
 *
 *  Created on: Apr 6, 2025
 *      Author: elias
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#define Activate_Display_Pin GPIO_PIN_8
#define Activate_Display_GPIO_Port GPIOB
#define Data_Command_Pin GPIO_PIN_9
#define Data_Command_GPIO_Port GPIOB

typedef enum {
    FONT_6x8,
    FONT_8x16
} FontSize;

void Display_Initialize(void);

void Clear_Display (void);

void SH1106_ClearDisplay(void);

void Return_Home (void);

void send_command_display(uint8_t command);

void send_data_display(uint8_t* data, uint16_t size);

void Display_line_2 (void);

void Display_line_3 (void);

void sh1106SetCursor(uint8_t page, uint8_t column);

void SH1106_GotoXY(uint8_t x, uint8_t y);

void SH1106_WriteChar_5x7(char c);

void SH1106_WriteString(uint8_t x, uint8_t y, const char* str, FontSize font_size);

void SH1106_Clear(void);

void SH1106_WriteChar_8x16(uint8_t x, uint8_t page, char c);

void SH1106_UpdateDisplay(void);

void SH1106_WriteString_AllAtOnce(uint8_t x, uint8_t y, const char* str, FontSize font_size);

#endif /* INC_DISPLAY_H_ */
