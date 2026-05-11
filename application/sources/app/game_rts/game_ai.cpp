/**
 * @file game_ai.cpp
 * @brief Triển khai thuật toán AI đơn giản dựa trên trọng số quân lính và khoảng cách.
 */
#include "game_ai.h"

void ai_think(GameState_t* gs, bool* want_attack, uint8_t* src, uint8_t* dst) {
    *want_attack = false;
    // Tìm một node của AI có nhiều quân để làm điểm xuất phát
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        if (gs->nodes[i].owner == OWNER_AI && gs->nodes[i].troops > 15) {
            // Duyệt các node kề cạnh
            for (uint8_t j = 0; j < MAX_NODES; j++) {
                if (gs->adj[i] & (1 << j)) {
                    // Ưu tiên chiếm node trung lập hoặc node người chơi nếu quân AI nhiều hơn gấp rưỡi
                    if (gs->nodes[j].owner != OWNER_AI && gs->nodes[j].troops < gs->nodes[i].troops * 2 / 3) {
                        *src = i;
                        *dst = j;
                        *want_attack = true;
                        return; // Quyết định đánh ngay
                    }
                }
            }
        }
    }
}
