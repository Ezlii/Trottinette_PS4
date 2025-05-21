/*
 * display.c
 *
 *  Created on: Apr 6, 2025
 *      Author: elias
 */
/*
 * File:   display.c
 * Author: elias
 *
 * Created on 5. April 2025, 20:29
 */
#include "stm32wbxx_hal.h"
#include "display.h"
#include <string.h>
#include "Font5x7.h"
#include "font8x16.h"




#define DISPLAY_MAX_CHARS         20
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_PAGES (DISPLAY_HEIGHT / 8)



extern SPI_HandleTypeDef hspi1;

uint8_t data[DISPLAY_MAX_CHARS + 1];
uint8_t display_clear_data[2] = {0x00, 0x01};
uint8_t display_return_home[2] = {0x00, 0x02};
uint8_t display_ON[2] = {0x00, 0x0C};
uint8_t display_Cursor_left[2] = {0x00, 0x10};
uint8_t display_Cursor_right[2] = {0x00, 0x14};
uint8_t display_double_height[2] = {0x00, 0x15};

// Globaler Display-Puffer (bereits definiert)
uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_PAGES];



void Display_Initialize(void)                                                   //Inizialisierung des Displays
{
	HAL_GPIO_WritePin(Activate_Display_GPIO_Port, Activate_Display_Pin, GPIO_PIN_SET);

	HAL_Delay(100);
    send_command_display(0xAE); // Display off
    send_command_display(0xD5); send_command_display(0x80); // Set Display Clock
    send_command_display(0xA8); send_command_display(0x3F); // Set Multiplex Ratio (0x3F = 63)
    send_command_display(0xD3); send_command_display(0x00); // Display offset
    send_command_display(0x40); // Start line at 0
    send_command_display(0xAD); send_command_display(0x8B); // DC-DC Control ON
    send_command_display(0xA1); // Segment remap
    send_command_display(0xC8); // COM scan direction
    send_command_display(0xDA); send_command_display(0x12); // COM pins hardware config
    send_command_display(0x81); send_command_display(0x7F); // Contrast
    send_command_display(0xD9); send_command_display(0xF1); // Pre-charge
    send_command_display(0xDB); send_command_display(0x40); // VCOMH
    send_command_display(0xA4); // Entire display ON (resume to RAM)
    send_command_display(0xA6); // Normal display (not inverse)
    send_command_display(0xAF); // Display ON

    HAL_Delay(120);

}

void send_command_display (uint8_t command)                                     //Command an das Display schreiben
{
	HAL_GPIO_WritePin(Data_Command_GPIO_Port, Data_Command_Pin, GPIO_PIN_RESET);
    //HAL_Delay(10);
	for (volatile uint32_t i = 0; i < 100; i++);
    HAL_SPI_Transmit(&hspi1,& command, 1, 10);
    //HAL_Delay(10);
    for (volatile uint32_t i = 0; i < 100; i++);
    HAL_GPIO_WritePin(Data_Command_GPIO_Port, Data_Command_Pin, GPIO_PIN_SET);
}


void send_data_display(uint8_t* data, uint16_t size)
{
	HAL_GPIO_WritePin(Data_Command_GPIO_Port, Data_Command_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi1, data, size, 10);
}


void Clear_Display (void)                                                       //Display löschen
{
    send_command_display(0b00000001);
}


void SH1106_Clear(void) {
    uint8_t buffer[128];
    memset(buffer, 0x00, sizeof(buffer));

    for (uint8_t page = 0; page < 8; page++) {
    	send_command_display(0xB0 + page); // Set page
    	send_command_display(0x02);        // Set lower column
    	send_command_display(0x10);        // Set higher column
    	send_data_display(buffer, sizeof(buffer));
    }
}

// Löscht das gesamte Display (Puffer + Anzeige)
void SH1106_ClearDisplay(void) {
    // Setze den gesamten Puffer auf 0
    for (uint16_t i = 0; i < DISPLAY_WIDTH * DISPLAY_PAGES; i++) {
        display_buffer[i] = 0x00;
    }

    // Aktualisiere das Display mit dem leeren Puffer
    SH1106_UpdateDisplay();
}


void Return_Home (void)                                                         //Bei der 1. Zeile ganz am Anfang beginnen
{
send_command_display(0b00000010);
}

void Display_line_2 (void)                                                      //Zweite Zeile des Displays wechseln
{
    send_command_display(0xA0);
}

void Display_line_3 (void)                                                      //Dritte Zeile des Displays wechseln
{
    send_command_display(0xC0);
}

void Display_line_4 (void)                                                      //Vierte Zeile des Displays wechseln
{
    send_command_display(0xE0);
}



void sh1106SetCursor(uint8_t page, uint8_t column) {
	send_command_display(0xB0 | page); // Set page address
	send_command_display(0x10 | ((column >> 4) & 0x0F)); // Set high nibble column
	send_command_display(column & 0x0F); // Set low nibble column
}


void SH1106_GotoXY(uint8_t x, uint8_t y) {
    // Füge hier einen Offset hinzu (z.B. 1 Pixel)
    uint8_t fixed_x = x + 2;

    send_command_display(0xB0 + y);                      // Setze Page
    send_command_display(0x00 + (fixed_x & 0x0F));         // Lower nibble ohne zusätzlichen Offset
    send_command_display(0x10 + ((fixed_x >> 4) & 0x0F));  // Upper nibble
}



void SH1106_WriteChar_5x7(char c) {
    if (c < 32 || c > 126) c = '?';

    uint8_t buffer[6];  // 5 Spalten + 1 Leerzeichen
    for (uint8_t i = 0; i < 5; i++) {
        buffer[i] = Font5x7[c - 32][i];
    }
    buffer[5] = 0x00; // Abstand

    send_data_display(buffer, sizeof(buffer));  // alles auf einmal senden
}

void SH1106_WriteChar_8x16(uint8_t x, uint8_t page, char c) {
    if (c < 32 || c > 127) c = '?';

    // Obere Hälfte (erste Page)
    send_command_display(0xB0 + page);
    send_command_display(0x00 + (x & 0x0F));              // Low nibble ohne Offset
    send_command_display(0x10 + ((x >> 4) & 0x0F));         // High nibble
    send_data_display((uint8_t*)&Font8x16[c - 32][0], 8);

    // Untere Hälfte (nächste Page)
    send_command_display(0xB0 + page + 1);
    send_command_display(0x00 + (x & 0x0F));              // Low nibble ohne Offset
    send_command_display(0x10 + ((x >> 4) & 0x0F));         // High nibble
    send_data_display((uint8_t*)&Font8x16[c - 32][8], 8);
}



void SH1106_WriteString(uint8_t x, uint8_t y, const char* str, FontSize font_size) {
    while (*str) {
        if (font_size == FONT_6x8) {
            if (x + 6 > DISPLAY_WIDTH) break;
            SH1106_GotoXY(x, y);  // Cursor setzen
            SH1106_WriteChar_5x7(*str++);  // Zeichnen
            x += 6; // Cursor fortschieben
        } else if (font_size == FONT_8x16) {
            if (x + 8 > DISPLAY_WIDTH) break;
            SH1106_WriteChar_8x16(x, y, *str++);
            x += 8;
        }
    }
}

// Aktualisiert das gesamte Display aus dem Puffer
void SH1106_UpdateDisplay(void) {
    for (uint8_t page = 0; page < DISPLAY_PAGES; page++) {
        send_command_display(0xB0 + page); // Setze Page-Adresse
        send_command_display(0x00);        // Setze Spalte (Low nibble)
        send_command_display(0x10);        // Setze Spalte (High nibble)

        // Sende alle 128 Spalten der aktuellen Seite
        for (uint8_t col = 0; col < DISPLAY_WIDTH; col++) {
            // Wichtig: Jeder Byte-Wert muss als Pointer gesendet werden
            uint8_t d = display_buffer[page * DISPLAY_WIDTH + col];
            send_data_display(&d, 1);
        }
    }
}

// Schreibt den Text in den Display-Puffer und aktualisiert dann das Display
void SH1106_WriteString_AllAtOnce(uint8_t x, uint8_t y, const char* str, FontSize font_size) {
    if (font_size == FONT_6x8) {
        // Lösche den relevanten Bereich in der Page y
        for (uint8_t col = x; col < DISPLAY_WIDTH; col++) {
            display_buffer[y * DISPLAY_WIDTH + col] = 0x00;
        }
        // Starte 2 Spalten weiter rechts
        uint8_t pos = x + 2;
        while (*str && (pos + 6 <= DISPLAY_WIDTH)) {
            char c = *str++;
            if (c < 32 || c > 126) c = '?';
            // Kopiere 5 Spalten des Fonts plus 1 Spalte Abstand
            for (uint8_t i = 0; i < 5; i++) {
                display_buffer[y * DISPLAY_WIDTH + pos + i] = Font5x7[c - 32][i];
            }
            // Abstandsspalte
            display_buffer[y * DISPLAY_WIDTH + pos + 5] = 0x00;
            pos += 6;
        }
    } else if (font_size == FONT_8x16) {
        // Für 8x16 gehen wir davon aus, dass der Text zwei benachbarte Pages belegt:
        // obere Hälfte in Page y und untere Hälfte in Page y+1.
        if (y + 1 >= DISPLAY_PAGES) return; // Safety check

        // Lösche den Bereich in beiden Pages
        for (uint8_t col = x; col < DISPLAY_WIDTH; col++) {
            display_buffer[y * DISPLAY_WIDTH + col] = 0x00;
            display_buffer[(y + 1) * DISPLAY_WIDTH + col] = 0x00;
        }
        // Starte 2 Spalten weiter rechts
        uint8_t pos = x + 2;
        while (*str && (pos + 8 <= DISPLAY_WIDTH)) {
            char c = *str++;
            if (c < 32 || c > 127) c = '?';
            // Kopiere 8 Spalten der oberen Hälfte
            for (uint8_t i = 0; i < 8; i++) {
                display_buffer[y * DISPLAY_WIDTH + pos + i] = Font8x16[c - 32][i];
            }
            // Kopiere 8 Spalten der unteren Hälfte in die nächste Page
            for (uint8_t i = 0; i < 8; i++) {
                display_buffer[(y + 1) * DISPLAY_WIDTH + pos + i] = Font8x16[c - 32][8 + i];
            }
            pos += 8;
        }
    }

    // Aktualisiere das Display (alle Seiten werden neu geladen)
    SH1106_UpdateDisplay();
}



