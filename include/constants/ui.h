#pragma once

#define FONT u8g2_font_squeezed_b7_tr
#define START_FONT u8g2_font_squeezed_b7_tr
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define CENTER_X OLED_WIDTH / 2
#define CENTER_Y OLED_HEIGHT / 2

/// ACP table, same geometry as constants/gyverui.h
// (U8G2_FIRST_ROW doubles as the menu single-value position)
// 3 columns, 5 rows (header + 4 sensors)

#define U8G2_MARGIN_X 5
#define U8G2_MARGIN_Y 10
#define U8G2_COLUMN_STEP 39
#define U8G2_ROW_STEP 12

#define U8G2_FIRST_COLUMN (U8G2_MARGIN_X + 0 * U8G2_COLUMN_STEP)  // 5
#define U8G2_SECOND_COLUMN (U8G2_MARGIN_X + 1 * U8G2_COLUMN_STEP) // 44
#define U8G2_THIRD_COLUMN (U8G2_MARGIN_X + 2 * U8G2_COLUMN_STEP)  // 83

#define U8G2_FIRST_ROW (U8G2_MARGIN_Y + 0 * U8G2_ROW_STEP)  // 5
#define U8G2_SECOND_ROW (U8G2_MARGIN_Y + 1 * U8G2_ROW_STEP) // 17
#define U8G2_THIRD_ROW (U8G2_MARGIN_Y + 2 * U8G2_ROW_STEP)  // 29
#define U8G2_FOURTH_ROW (U8G2_MARGIN_Y + 3 * U8G2_ROW_STEP) // 41
#define U8G2_FIFTH_ROW (U8G2_MARGIN_Y + 4 * U8G2_ROW_STEP)  // 53

#define ROWS 5

/// menu selection marker column, left of the first table column
#define U8G2_MENU_MARKER_X 0

#define VERSION_X getWidth () - 28
#define VERSION_Y 10
