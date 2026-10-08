#pragma once

#ifndef GyverOLED_h
#error "сначала подключите <GyverOLED.h>"
#endif

#define GYVER_PANEL SSH1106_128x64
#define GYVER_BUFFER OLED_BUFFER
#define GYVER_CONN OLED_I2C
#define GYVER_CS 10
#define GYVER_DC 9
#define GYVER_RST -1
#define GYVER_ADDRESS 0x3C

#define PERMITTED_OFFSET 4

/// positions

// ################ ACP page #######################
// 3 columns
// 128/3 = 42 px - 5 (padding) = 37 px

#define MARGIN_X 5
#define MARGIN_Y 5
#define COLUMN_STEP 39
// 5 rows: (64 - 8 (font height) - 2 * 5 (margin)) / 4 = 11
#define ROW_STEP 12

#define G_FIRST_COLUMN (MARGIN_X + 0 * COLUMN_STEP)  // 5
#define G_SECOND_COLUMN (MARGIN_X + 1 * COLUMN_STEP) // 44
#define G_THIRD_COLUMN (MARGIN_X + 2 * COLUMN_STEP)  // 83

#define G_FIRST_ROW (MARGIN_Y + 0 * ROW_STEP)  // 5
#define G_SECOND_ROW (MARGIN_Y + 1 * ROW_STEP) // 17
#define G_THIRD_ROW (MARGIN_Y + 2 * ROW_STEP)  // 29
#define G_FOURTH_ROW (MARGIN_Y + 3 * ROW_STEP) // 41
#define G_FIFTH_ROW (MARGIN_Y + 4 * ROW_STEP)  // 53

#define G_CENTERED_X(column, text)                                            \
  ((column) + (COLUMN_STEP - getStrWidth (text)) / 2)

/// columns
// acp
#define G_ACP_X G_SECOND_COLUMN
#define G_ACP_Y G_FIRST_ROW

// temperature
#define G_TEMP_X G_THIRD_COLUMN
#define G_TEMP_Y G_FIRST_ROW
