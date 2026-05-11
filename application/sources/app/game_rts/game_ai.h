/**
 * @file game_ai.h
 * @brief Xử lý trí tuệ nhân tạo (AI) cho kẻ địch trong game.
 *        Quyết định khi nào nên điều quân, và đánh vào đâu.
 */
#ifndef GAME_AI_H
#define GAME_AI_H

#include "game_types.h"

/**
 * @brief AI đánh giá và đưa ra quyết định hành quân
 * @param gs Con trỏ tới GameState
 * @param want_attack AI có muốn tấn công hay không (true/false)
 * @param src Node nguồn AI chọn
 * @param dst Node đích AI muốn đánh
 */
void ai_think(GameState_t* gs, bool* want_attack, uint8_t* src, uint8_t* dst);

#endif // GAME_AI_H
