/**
 * @file game_combat.h
 * @brief Xử lý logic giao tranh khi các đạo quân (March) đi đến đích.
 *        Tính toán thiệt hại, chiếm thành và tương tác của Hero trong combat.
 */
#ifndef GAME_COMBAT_H
#define GAME_COMBAT_H

#include "game_types.h"

/**
 * @brief Xử lý kết quả khi đạo quân đến đích
 */
void combat_resolve(GameState_t* gs, uint8_t march_idx);

#endif // GAME_COMBAT_H
