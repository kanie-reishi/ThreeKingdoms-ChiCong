/**
 * @file game_assets.h
 * @brief Định nghĩa các tài nguyên đồ họa (Graphics Assets) như Font và Sprite.
 *        Toàn bộ đồ họa được lưu dưới dạng bitmask 1 chiều, hoàn toàn độc lập với phần cứng.
 */
#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include <stdint.h>

// ---------------------------------------------------------
// FONT DATA (Monospace)
// ---------------------------------------------------------
extern const uint8_t font3x5[10][5];       // Số 0-9 (Kích thước 3x5)
extern const uint8_t font3x5_alpha[26][5]; // Chữ A-Z (Kích thước 3x5)

// ---------------------------------------------------------
// SPRITES (1-bit Pixel Art)
// ---------------------------------------------------------
extern const uint8_t sprite_castle_lv1[8]; // Thành Cấp 1 (8x8 pixels)
extern const uint8_t sprite_castle_lv2[8]; // Thành Cấp 2 (8x8 pixels)
extern const uint8_t sprite_castle_lv3[8]; // Thành Cấp 3 (8x8 pixels)
extern const uint8_t sprite_hero[5];       // Biểu tượng Hero (5x5 pixels)
extern const uint16_t sprite_cursor[12];   // Khung nhắm mục tiêu Cursor (12x12 pixels)

#endif // GAME_ASSETS_H
