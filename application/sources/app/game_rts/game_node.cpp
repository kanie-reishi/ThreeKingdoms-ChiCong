/**
 * @file game_node.cpp
 * @brief Triển khai các hàm xử lý logic sinh quân, cập nhật cấp độ cho các thành trì.
 */
#include "game_node.h"

void node_spawn_troops(GameState_t* gs) {
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        // Cập nhật cấp độ dựa trên số quân hiện tại
        if (gs->nodes[i].troops >= LEVEL_3_THRESHOLD) {
            gs->nodes[i].level = 3;
        } else if (gs->nodes[i].troops >= LEVEL_2_THRESHOLD) {
            gs->nodes[i].level = 2;
        } else {
            gs->nodes[i].level = 1;
        }

        // Chỉ tăng quân cho người chơi và AI, tối đa 99 quân
        if (gs->nodes[i].owner != OWNER_NEUTRAL && gs->nodes[i].troops < 99) {
            uint8_t spawn_amount = gs->nodes[i].level;
            
            // Áp dụng buff Đồn Điền riêng cho từng node của Player
            if (gs->nodes[i].owner == OWNER_PLAYER) {
                spawn_amount += gs->nodes[i].node_spawn_bonus;
            }
            
            gs->nodes[i].troops += spawn_amount;
            if (gs->nodes[i].troops > 99) gs->nodes[i].troops = 99;
        }
    }
}
