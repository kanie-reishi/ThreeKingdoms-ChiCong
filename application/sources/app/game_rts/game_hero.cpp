/**
 * @file game_hero.cpp
 * @brief Triển khai các kỹ năng và tương tác cơ bản của Hero trên bản đồ.
 */
#include "game_hero.h"
#include <stdlib.h> // Cho hàm rand()

/**
 * @brief Xử lý khi đạo quân có Hero đến được thành mục tiêu (Thắng hoặc Tiếp viện).
 *        Hero được đặt vào thành trì đích.
 */
void hero_handle_transfer(GameState_t* gs, uint8_t target_node) {
    gs->nodes[target_node].has_hero = true;
}

/**
 * @brief Xử lý khi Hero bị thất bại (đạo quân bị đánh lui hoặc thành thủ bị mất).
 *        Hero sẽ chuyển sang trạng thái lưu vong và bắt đầu đếm ngược hồi sinh.
 * @param owner Phe sở hữu Hero bị thất bại (OWNER_PLAYER hoặc OWNER_AI)
 */
void hero_handle_defeat(GameState_t* gs, uint8_t owner) {
    if (owner == OWNER_PLAYER || owner == OWNER_AI) {
        gs->heroes[owner].state = HERO_EXILED;
        gs->heroes[owner].respawn_timer = HERO_RESPAWN_DELAY_TICKS;
    }
}

/**
 * @brief Kiểm tra mỗi tick xem Hero "lưu vong" đã đếm ngược xong chưa và tìm thành hồi sinh.
 *        Gọi hàm này mỗi SIG_TICK trong handle_global_tick().
 */
void hero_tick(GameState_t* gs) {
    for (uint8_t owner = OWNER_PLAYER; owner <= OWNER_AI; owner++) {
        if (gs->heroes[owner].state == HERO_EXILED) {
            if (gs->heroes[owner].respawn_timer > 0) {
                gs->heroes[owner].respawn_timer--;
            }
            if (gs->heroes[owner].respawn_timer == 0) {
                gs->heroes[owner].state = HERO_RESPAWNING;
            }
        }
        
        if (gs->heroes[owner].state == HERO_RESPAWNING) {
            uint8_t valid_nodes[MAX_NODES];
            uint8_t count = 0;
            bool has_any_node = false;
            
            for (uint8_t i = 0; i < MAX_NODES; i++) {
                if (gs->nodes[i].owner == owner) {
                    has_any_node = true;
                    if (!gs->nodes[i].has_hero) {
                        valid_nodes[count++] = i;
                    }
                }
            }
            
            if (count > 0) {
                // Chọn ngẫu nhiên một node hợp lệ
                uint8_t r = rand() % count;
                uint8_t target = valid_nodes[r];
                gs->nodes[target].has_hero = true;
                gs->heroes[owner].state = HERO_ALIVE;
            } else if (!has_any_node) {
                // Thua game, không còn thành nào phe mình để hồi sinh
                gs->heroes[owner].state = HERO_DEAD_END;
            }
            // Nếu có thành nhưng thành nào cũng có hero (dành cho logic nhiều hero sau này), 
            // hero vẫn ở trạng thái HERO_RESPAWNING chờ tới khi có chỗ trống.
        }
    }
}
