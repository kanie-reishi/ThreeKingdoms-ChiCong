/**
 * @file game_logic.h
 * @brief File trung tâm đóng vai trò bao bọc (Wrapper) toàn bộ các module logic lại với nhau.
 *        Chỉ cần include file này trong task_game là có thể truy cập toàn bộ logic của game.
 */
#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game_types.h"
#include "game_node.h"
#include "game_hero.h"
#include "game_combat.h"
#include "game_map.h"
#include "game_ai.h"

/**
 * @brief Kiểm tra điều kiện thắng thua của trò chơi
 * @param gs Con trỏ tới GameState
 * @return Phe chiến thắng (OWNER_PLAYER, OWNER_AI) hoặc OWNER_NEUTRAL nếu chưa ai thắng
 */
uint8_t check_win(GameState_t* gs);

#endif // GAME_LOGIC_H