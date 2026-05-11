/**
 * @file game_node.h
 * @brief Chứa logic quản lý các thành trì (Node) trong game, 
 *        bao gồm sinh quân (spawn) và tính toán cấp độ (level).
 */
#ifndef GAME_NODE_H
#define GAME_NODE_H

#include "game_types.h"

/**
 * @brief Cập nhật cấp độ và sinh thêm quân lính cho các thành trì
 * @param gs Con trỏ tới GameState
 */
void node_spawn_troops(GameState_t* gs);

#endif // GAME_NODE_H
