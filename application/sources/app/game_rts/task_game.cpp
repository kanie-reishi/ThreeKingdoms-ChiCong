#include "task_game.h"
#include "game_logic.h"
#include "game_card.h"
#include "game_hero.h"
#include "screens/scr_map.h"

// Biến toàn cục lưu trữ trạng thái game và state hiện tại
static GameState_t gs;
static uint8_t game_state = STATE_MAIN_MENU;

// Khai báo ngoài để tìm Node kề (cần được implement thêm logic chọn mục tiêu)
extern uint8_t prev_adj(uint8_t current_cursor);
extern uint8_t next_adj(uint8_t current_cursor);

/**
 * @brief Tìm một khe trống để xuất quân, đồng thời đếm số quân đang đi của Owner
 * @return Index của march_slot hoặc -1 nếu đã đạt tối đa 4 đạo quân
 */
static int get_free_march_slot(GameState_t* gs_ptr, uint8_t owner) {
    int active_count = 0;
    int free_slot = -1;
    for (int i = 0; i < MAX_MARCHES; i++) {
        if (gs_ptr->marches[i].troops > 0) {
            if (gs_ptr->marches[i].owner == owner) active_count++;
        } else {
            if (free_slot == -1) free_slot = i;
        }
    }
    if (active_count >= 4) return -1;
    return free_slot;
}

/**
 * @brief Xử lý sự kiện Tick chung cho các state (cập nhật quân, hành quân, AI)
 */
static void handle_global_tick() {
    gs.tick++;
    
    // Mỗi 50 tick (1 năm) -> Kích hoạt Thẻ Bài Nội Chính
    if (gs.tick >= 50) {
        gs.tick = 0;
        gs.year++;
        // Shuffle 3 thẻ ngẫu nhiên từ 5 loại bằng module game_card
        card_shuffle_offered(&gs);
        game_state = STATE_INTERNAL_AFFAIRS;
        gs.dirty = 1;
        return;
    }

    node_spawn_troops(&gs);   // +1 quân mỗi N tick
    if (check_win(&gs) != OWNER_NEUTRAL) {
        game_state = STATE_GAME_OVER;
        return;
    }

    // Cập nhật tất cả các đạo quân đang đi
    for (int i = 0; i < MAX_MARCHES; i++) {
        if (gs.marches[i].troops > 0) {
            uint8_t cost = gs.cost[gs.marches[i].src][gs.marches[i].dst];
            uint8_t base_speed = 100 / (cost > 0 ? cost : 4);
            // Áp dụng buff Tốc Hành (march_speed bonus %)
            uint8_t speed = base_speed + (base_speed * gs.buffs.march_speed / 100);
            gs.marches[i].progress += speed;
            if (gs.marches[i].progress >= 100) {
                combat_resolve(&gs, i);
                gs.marches[i].troops   = 0;
                gs.marches[i].progress = 0;
            }
        }
    }

    // Kiểm tra Hero lưu vong
    hero_tick(&gs);
    // AI đưa ra quyết định nếu chưa đạt giới hạn 4 đạo quân
    int ai_slot = get_free_march_slot(&gs, OWNER_AI);
    if (ai_slot != -1) {
        bool want_attack = false;
        uint8_t ai_src, ai_dst;
        ai_think(&gs, &want_attack, &ai_src, &ai_dst);
        
        if (want_attack) {
            bool send_hero = gs.nodes[ai_src].has_hero;
            // Đưa quân đi đánh
            gs.marches[ai_slot] = (March_t){ ai_src, ai_dst, (uint8_t)(gs.nodes[ai_src].troops / 2), 0, send_hero, OWNER_AI };
            gs.nodes[ai_src].troops /= 2;
            if (send_hero) gs.nodes[ai_src].has_hero = false;
        }
    }
    gs.dirty = 1;
}

/**
 * @brief Hàm xử lý các message gửi đến task game (State Machine)
 * @param msg Con trỏ chứa thông tin tín hiệu sự kiện (nút bấm, tick...)
 */
void task_game_handler(ak_msg_t* msg) {
    switch (game_state) {

    case STATE_MAIN_MENU:
        if (msg->sig == SIG_BTN_UP || msg->sig == SIG_BTN_DOWN) {
            gs.cursor = (gs.cursor == 0) ? 1 : 0; // 0: Map cơ bản, 1: Map động
        }
        if (msg->sig == SIG_BTN_OK) {
            if (gs.cursor == 0) {
                game_map_init_fixed(&gs);
            } else {
                game_map_init_procedural(&gs, gs.tick);
            }
            game_state = STATE_IDLE;
        }
        if (msg->sig == SIG_TICK) {
            gs.tick++; // Dùng làm seed ngẫu nhiên
        }
        gs.dirty = 1;
        break;

    case STATE_INIT:
        game_map_init_fixed(&gs);
        game_state = STATE_IDLE; 
        break;

    case STATE_IDLE:
        if (msg->sig == SIG_BTN_UP)   gs.cursor = prev_adj(gs.cursor);
        if (msg->sig == SIG_BTN_DOWN) gs.cursor = next_adj(gs.cursor);

        if (msg->sig == SIG_BTN_OK
            && gs.nodes[gs.cursor].owner == OWNER_PLAYER) {
            gs.selected_src = gs.cursor;
            game_state = STATE_SRC_SELECTED;
        }
        
        if (msg->sig == SIG_TICK) {
            handle_global_tick();
        }
        gs.dirty = 1; 
        break;

    case STATE_SRC_SELECTED:
        if (msg->sig == SIG_BTN_UP)   gs.cursor = prev_adj(gs.cursor);
        if (msg->sig == SIG_BTN_DOWN) gs.cursor = next_adj(gs.cursor);

        if (msg->sig == SIG_BTN_CANCEL) {
            gs.selected_src = NO_NODE;
            gs.hero_selected = false;
            game_state = STATE_IDLE;
        }
        
        // Double-click (nhấn OK ngay tại thành đang chọn) để bật cờ gửi Hero
        if (msg->sig == SIG_BTN_OK && gs.cursor == gs.selected_src) {
            if (gs.nodes[gs.selected_src].has_hero) {
                gs.hero_selected = !gs.hero_selected;
            }
        }
        
        // Xuất quân nếu ấn OK ở thành kề
        else if (msg->sig == SIG_BTN_OK
            && (gs.adj[gs.selected_src] & (1 << gs.cursor))) {
            
            // Chỉ xuất quân nếu chưa đạt giới hạn 4 đạo quân
            int player_slot = get_free_march_slot(&gs, OWNER_PLAYER);
            if (player_slot != -1) {
                gs.marches[player_slot] = (March_t){ gs.selected_src, gs.cursor,
                                      (uint8_t)(gs.nodes[gs.selected_src].troops / 2), 0, gs.hero_selected, OWNER_PLAYER };
                gs.nodes[gs.selected_src].troops /= 2; 
                if (gs.hero_selected) gs.nodes[gs.selected_src].has_hero = false;
            }
            // Trở về trạng thái IDLE
            gs.selected_src = NO_NODE;
            gs.hero_selected = false;
            game_state = STATE_IDLE; 
        }

        if (msg->sig == SIG_TICK) {
            handle_global_tick();
        }
        gs.dirty = 1;
        break;

    case STATE_INTERNAL_AFFAIRS:
        if (msg->sig == SIG_BTN_UP) {
            if (gs.card_cursor > 0) gs.card_cursor--;
        }
        if (msg->sig == SIG_BTN_DOWN) {
            if (gs.card_cursor < 2) gs.card_cursor++;
        }
        if (msg->sig == SIG_BTN_OK) {
            uint8_t chosen_card = gs.cards_offered[gs.card_cursor];
            gs.pending_card = chosen_card;

            if (card_get_scope(chosen_card) == SCOPE_NODE) {
                // Thẻ NODE: Chuyển sang chế độ chọn thành mục tiêu
                // Đặt cursor về node đầu tiên thuộc Player
                for (uint8_t i = 0; i < MAX_NODES; i++) {
                    if (gs.nodes[i].owner == OWNER_PLAYER) {
                        gs.cursor = i;
                        break;
                    }
                }
                game_state = STATE_CARD_TARGET;
            } else {
                // Thẻ GLOBAL: Áp dụng ngay, không cần chọn node
                card_apply(&gs, chosen_card, NO_NODE);
                game_state = STATE_IDLE;
            }
        }
        gs.dirty = 1;
        break;

    case STATE_CARD_TARGET:
        // Di chuyển cursor chỉ qua các node thuộc Player
        if (msg->sig == SIG_BTN_UP || msg->sig == SIG_BTN_DOWN) {
            int dir = (msg->sig == SIG_BTN_DOWN) ? 1 : -1;
            int c = gs.cursor;
            for (int step = 0; step < MAX_NODES; step++) {
                c = (c + dir + MAX_NODES) % MAX_NODES;
                if (gs.nodes[c].owner == OWNER_PLAYER) {
                    gs.cursor = (uint8_t)c;
                    break;
                }
            }
        }
        if (msg->sig == SIG_BTN_OK) {
            // Áp dụng thẻ bài lên node đang chọn
            card_apply(&gs, gs.pending_card, gs.cursor);
            gs.pending_card = 0xFF;
            game_state = STATE_IDLE;
        }
        gs.dirty = 1;
        break;

    case STATE_GAME_OVER:
        // TODO: Hiển thị kết quả
        break;
    }

    if (gs.dirty) {
        scr_map_render(&gs, game_state);
        gs.dirty = 0; 
    }
}