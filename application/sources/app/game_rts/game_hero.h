/**
 * @file game_hero.h
 * @brief Xử lý các logic đặc biệt liên quan đến Hero Unit.
 *        Bao gồm việc đóng quân tại thành (transfer) và hồi sinh khi tử trận (defeat).
 */
#ifndef GAME_HERO_H
#define GAME_HERO_H

#include "game_types.h"

/**
 * @brief Xử lý khi Hero đến một thành (thắng hoặc tiếp viện)
 */
void hero_handle_transfer(GameState_t* gs, uint8_t target_node);

/**
 * @brief Xử lý khi Hero thua trận (hồi sinh ở thành khác)
 */
void hero_handle_defeat(GameState_t* gs, uint8_t owner);

/**
 * @brief Kiểm tra mỗi tick xem Hero đang lưu vong đã có thành để về chưa.
 *        Gọi trong handle_global_tick() mỗi SIG_TICK.
 */
void hero_tick_homeless(GameState_t* gs);

#endif // GAME_HERO_H
