/**
 * @file game_hero.cpp
 * @brief Triển khai các kỹ năng và tương tác cơ bản của Hero trên bản đồ.
 */
#include "game_hero.h"

/**
 * @brief Xử lý khi đạo quân có Hero đến được thành mục tiêu (Thắng hoặc Tiếp viện).
 *        Hero được đặt vào thành trì đích.
 */
void hero_handle_transfer(GameState_t* gs, uint8_t target_node) {
    gs->nodes[target_node].has_hero = true;
}

/**
 * @brief Xử lý khi Hero bị thất bại (đạo quân bị đánh lui hoặc thành thủ bị mất).
 *        Hero sẽ tái sinh ở thành mạnh nhất còn thuộc về phe đó.
 *        Nếu không còn thành nào, Hero vào trạng thái "lưu vong".
 * @param owner Phe sở hữu Hero bị thất bại (OWNER_PLAYER hoặc OWNER_AI)
 */
void hero_handle_defeat(GameState_t* gs, uint8_t owner) {
    uint8_t best_node = NO_NODE;
    uint8_t max_t = 0;
    for (int i = 0; i < MAX_NODES; i++) {
        // Tìm thành có đông quân nhất thuộc về phe đó
        if (gs->nodes[i].owner == owner && gs->nodes[i].troops >= max_t) {
            max_t = gs->nodes[i].troops; 
            best_node = i;
        }
    }

    if (best_node != NO_NODE) {
        // Tìm được thành -> Tái sinh Hero tại đây
        gs->nodes[best_node].has_hero = true;
        if (owner == OWNER_PLAYER) {
            gs->hero_homeless = false;
        }
    } else {
        // Không còn thành nào -> Hero vào trạng thái "lưu vong"
        if (owner == OWNER_PLAYER) {
            gs->hero_homeless = true;
        }
    }
}

/**
 * @brief Kiểm tra mỗi tick xem Hero "lưu vong" đã có thành để trở về chưa.
 *        Nếu Player vừa chiếm lại được 1 thành, Hero sẽ tự động xuất hiện ở đó.
 *        Gọi hàm này mỗi SIG_TICK trong handle_global_tick().
 */
void hero_tick_homeless(GameState_t* gs) {
    if (!gs->hero_homeless) return;

    // Tìm thành đầu tiên của Player
    for (int i = 0; i < MAX_NODES; i++) {
        if (gs->nodes[i].owner == OWNER_PLAYER) {
            gs->nodes[i].has_hero = true;
            gs->hero_homeless = false;
            return;
        }
    }
}
