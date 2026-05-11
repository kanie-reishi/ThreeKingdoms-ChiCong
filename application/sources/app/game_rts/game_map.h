/**
 * @file game_map.h
 * @brief Quản lý logic khởi tạo bản đồ (Map Generation).
 *        Bao gồm cả bản đồ cố định (Fixed Map) và bản đồ sinh tự động (Procedural Map).
 */
#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "game_types.h"

void game_map_init_fixed(GameState_t* gs);
void game_map_init_procedural(GameState_t* gs, uint16_t seed);

#endif // GAME_MAP_H
