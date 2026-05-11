#include "screens/scr_map.h"
#include <SDL2/SDL.h>
#include <stdio.h>

#include "game_assets.h"
#include "game_card.h"

// Extern renderer từ main.cpp
extern SDL_Renderer* g_renderer;

// Hàm vẽ Sprite 8-bit
void draw_bitmap(SDL_Renderer* r, int x, int y, const uint8_t* bitmap, int w, int h, int size) {
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            if (bitmap[row] & (1 << (w - 1 - col))) {
                SDL_Rect px = { x + col * size, y + row * size, size, size };
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

// Hàm vẽ Sprite 16-bit
void draw_bitmap16(SDL_Renderer* r, int x, int y, const uint16_t* bitmap, int w, int h, int size) {
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            if (bitmap[row] & (1 << (w - 1 - col))) {
                SDL_Rect px = { x + col * size, y + row * size, size, size };
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

// Hàm vẽ 1 chữ số
void draw_digit(SDL_Renderer* r, int x, int y, int digit, int size) {
    if (digit < 0 || digit > 9) return;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            if (font3x5[digit][row] & (1 << (2 - col))) {
                SDL_Rect px = { x + col * size, y + row * size, size, size };
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

// Hàm vẽ số có nhiều chữ số
void draw_number(SDL_Renderer* r, int x, int y, int number) {
    int size = 2; // pixel size multiplier
    int spacing = 3 * size + 2; // khoảng cách giữa các chữ số
    
    char buf[10];
    sprintf(buf, "%d", number);
    
    for (int i = 0; buf[i] != '\0'; i++) {
        draw_digit(r, x + i * spacing, y, buf[i] - '0', size);
    }
}

// Hàm vẽ 1 ký tự A-Z
void draw_char(SDL_Renderer* r, int x, int y, char c, int size) {
    if (c >= 'a' && c <= 'z') c -= 32; // To uppercase
    if (c < 'A' || c > 'Z') return;
    int idx = c - 'A';
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            if (font3x5_alpha[idx][row] & (1 << (2 - col))) {
                SDL_Rect px = { x + col * size, y + row * size, size, size };
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

// Hàm vẽ chuỗi ký tự (hỗ trợ A-Z và khoảng trắng)
void draw_string(SDL_Renderer* r, int x, int y, const char* str) {
    int size = 2;
    int spacing = 3 * size + 2;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ') {
            draw_char(r, x + i * spacing, y, str[i], size);
        }
    }
}

void scr_map_render(GameState_t* gs, uint8_t game_state) {
    if (!g_renderer) return;

    // Xóa nền đen
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);

    if (game_state == 0) { // STATE_MAIN_MENU
        SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
        draw_number(g_renderer, 150, 100, 1);
        draw_string(g_renderer, 170, 100, "FIXED MAP");
        
        draw_number(g_renderer, 150, 150, 2);
        draw_string(g_renderer, 170, 150, "RANDOM MAP");
        
        SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 255);
        if (gs->cursor == 0) {
            SDL_Rect cursor_rect = { 130, 98, 10, 10 };
            SDL_RenderFillRect(g_renderer, &cursor_rect);
        } else {
            SDL_Rect cursor_rect = { 130, 148, 10, 10 };
            SDL_RenderFillRect(g_renderer, &cursor_rect);
        }
        
        SDL_RenderPresent(g_renderer);
        return;
    }

    // Tỷ lệ scale: Vẽ đồ họa trên hệ tọa độ 128x64, nhưng phóng to x4 trên PC
    int scale = 4;

    // Vẽ các đường kết nối (Adj)
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        for (uint8_t j = i + 1; j < MAX_NODES; j++) {
            if (gs->adj[i] & (1 << j)) {
                if (gs->cost[i][j] > 4) {
                    SDL_SetRenderDrawColor(g_renderer, 139, 69, 19, 255); // Màu nâu (SaddleBrown) cho đường núi
                } else {
                    SDL_SetRenderDrawColor(g_renderer, 100, 100, 100, 255); // Màu xám cho đường bằng
                }
                SDL_RenderDrawLine(g_renderer, 
                    gs->nodes[i].x * scale, gs->nodes[i].y * scale,
                    gs->nodes[j].x * scale, gs->nodes[j].y * scale);
            }
        }
    }

    // Vẽ các Node (Thành trì)
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        Node_t* n = &gs->nodes[i];
        
        // Tọa độ trung tâm để vẽ
        int cx = n->x * scale - 12; // Do sprite 8x8 * scale 3 = 24. Để căn giữa 24 ta dùng 12
        int cy = n->y * scale - 12;
        int sprite_scale = 3; // Vẽ sprite to gấp 3 lần pixel gốc
        
        // Màu sắc dựa trên phe sở hữu
        if (n->owner == OWNER_PLAYER) {
            SDL_SetRenderDrawColor(g_renderer, 0, 200, 0, 255); // Xanh lá - Player
        } else if (n->owner == OWNER_AI) {
            SDL_SetRenderDrawColor(g_renderer, 200, 0, 0, 255); // Đỏ - AI
        } else {
            SDL_SetRenderDrawColor(g_renderer, 150, 150, 150, 255); // Xám - Trung lập
        }

        // Vẽ Sprite thay vì hình vuông cục mịch
        if (n->level == 3) {
            draw_bitmap(g_renderer, cx, cy, sprite_castle_lv3, 8, 8, sprite_scale);
        } else if (n->level == 2) {
            draw_bitmap(g_renderer, cx, cy, sprite_castle_lv2, 8, 8, sprite_scale);
        } else {
            draw_bitmap(g_renderer, cx, cy, sprite_castle_lv1, 8, 8, sprite_scale);
        }

        // Highlight con trỏ (Yellow)
        if (i == gs->cursor) {
            SDL_SetRenderDrawColor(g_renderer, 255, 255, 0, 255);
            // Vẽ Cursor 12x12
            draw_bitmap16(g_renderer, cx - 6, cy - 6, sprite_cursor, 12, 12, sprite_scale);
        }
        
        // Highlight thành đang được chọn làm nguồn xuất quân (Cyan/Magenta)
        if (i == gs->selected_src) {
            if (gs->hero_selected) {
                SDL_SetRenderDrawColor(g_renderer, 255, 0, 255, 255); // Magenta
            } else {
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255); // Cyan
            }
            draw_bitmap16(g_renderer, cx - 6, cy - 6, sprite_cursor, 12, 12, sprite_scale);
        }

        // Vẽ số lượng quân của Node (trắng)
        SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
        int offset_x = (n->troops >= 10) ? 7 : 3; // Căn giữa nhẹ
        draw_number(g_renderer, n->x * scale - offset_x, n->y * scale - 7, n->troops);

        // Vẽ biểu thị level (các chấm vàng nhỏ) bên dưới số quân
        SDL_SetRenderDrawColor(g_renderer, 255, 215, 0, 255); // Màu vàng gold
        int total_w = n->level * 6 - 2; // Khoảng cách 6px mỗi chấm
        int start_x = n->x * scale - total_w / 2;
        for (int l = 0; l < n->level; l++) {
            SDL_Rect star = { start_x + l * 6, n->y * scale + 5, 4, 4 };
            SDL_RenderFillRect(g_renderer, &star);
        }
        
        // Vẽ Hero nếu có (Thanh kiếm ở góc phải)
        if (n->has_hero) {
            SDL_SetRenderDrawColor(g_renderer, 0, 150, 255, 255); // Màu xanh dương
            draw_bitmap(g_renderer, cx + 18, cy - 6, sprite_hero, 5, 5, 2);
        }
    }

    // Vẽ các đội quân đang hành quân và đếm số đạo quân
    int player_marches = 0;
    int ai_marches = 0;

    for (int i = 0; i < MAX_MARCHES; i++) {
        if (gs->marches[i].troops > 0) {
            if (gs->marches[i].owner == OWNER_PLAYER) player_marches++;
            if (gs->marches[i].owner == OWNER_AI) ai_marches++;
            
            if (gs->marches[i].progress > 0) {
                Node_t* src = &gs->nodes[gs->marches[i].src];
                Node_t* dst = &gs->nodes[gs->marches[i].dst];
                
                // Nội suy tọa độ dựa trên progress
                int mx = src->x + (dst->x - src->x) * gs->marches[i].progress / 100;
                int my = src->y + (dst->y - src->y) * gs->marches[i].progress / 100;
                
                // Màu đạo quân theo phe
                if (gs->marches[i].owner == OWNER_PLAYER) {
                    SDL_SetRenderDrawColor(g_renderer, 0, 200, 0, 255); // Xanh lá
                } else {
                    SDL_SetRenderDrawColor(g_renderer, 200, 0, 0, 255); // Đỏ
                }
                SDL_Rect mrect = { mx * scale - 4, my * scale - 4, 8, 8 };
                SDL_RenderFillRect(g_renderer, &mrect);
                
                // Vẽ thêm màu chấm hero bên trong nếu đạo quân có hero
                if (gs->marches[i].is_hero_march) {
                    SDL_SetRenderDrawColor(g_renderer, 0, 150, 255, 255);
                    draw_bitmap(g_renderer, mx * scale - 4, my * scale - 4, sprite_hero, 5, 5, 2);
                }
            }
        }
    }

    // Vẽ UI hiển thị số lượng hành quân ở góc màn hình (X - 4)
    // Của người chơi (Góc trái)
    SDL_SetRenderDrawColor(g_renderer, 0, 200, 0, 255); // Hộp xanh lá
    SDL_Rect p_rect = { 5, 5, 10, 10 };
    SDL_RenderFillRect(g_renderer, &p_rect);
    SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
    draw_number(g_renderer, 20, 5, player_marches);
    SDL_Rect slash1 = { 30, 10, 4, 2 }; // Dấu gạch ngang biểu tượng phân số
    SDL_RenderFillRect(g_renderer, &slash1);
    draw_number(g_renderer, 38, 5, 4);

    // Của AI (Góc phải)
    SDL_SetRenderDrawColor(g_renderer, 200, 0, 0, 255); // Hộp đỏ
    SDL_Rect ai_rect = { 512 - 50, 5, 10, 10 }; // SCREEN_WIDTH = 512
    SDL_RenderFillRect(g_renderer, &ai_rect);
    SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
    draw_number(g_renderer, 512 - 35, 5, ai_marches);
    SDL_Rect slash2 = { 512 - 25, 10, 4, 2 };
    SDL_RenderFillRect(g_renderer, &slash2);
    draw_number(g_renderer, 512 - 17, 5, 4);

    // Vẽ Năm (Year) ở giữa trên cùng
    SDL_SetRenderDrawColor(g_renderer, 255, 215, 0, 255); // Gold
    draw_string(g_renderer, 200, 5, "YEAR");
    draw_number(g_renderer, 240, 5, gs->year);

    // Vẽ Overlay Nội Chính (Không che hoàn toàn map)
    if (game_state == STATE_INTERNAL_AFFAIRS || game_state == STATE_CARD_TARGET) {
        SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 200);
        SDL_Rect bg = { 50, 155, 412, 115 };
        SDL_RenderFillRect(g_renderer, &bg);
        SDL_SetRenderDrawColor(g_renderer, 255, 215, 0, 255); // Viền vàng
        SDL_RenderDrawRect(g_renderer, &bg);
        SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_NONE);

        if (game_state == STATE_INTERNAL_AFFAIRS) {
            // -- Giao diện chọn thẻ bài --
            SDL_SetRenderDrawColor(g_renderer, 255, 215, 0, 255);
            draw_string(g_renderer, 170, 162, "INTERNAL AFFAIRS");

            for (int i = 0; i < 3; i++) {
                int cy = 186 + i * 24;
                uint8_t ct = gs->cards_offered[i];
                const char* scope_str = (card_get_scope(ct) == SCOPE_NODE) ? "NODE" : "GLOBAL";

                if (i == gs->card_cursor) {
                    // Hàng đang chọn: nền vàng nhạt
                    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(g_renderer, 255, 215, 0, 50);
                    SDL_Rect hl = { 60, cy - 2, 392, 18 };
                    SDL_RenderFillRect(g_renderer, &hl);
                    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_NONE);
                    SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 255);
                    draw_string(g_renderer, 68, cy, ">");
                } else {
                    SDL_SetRenderDrawColor(g_renderer, 180, 180, 180, 255);
                }
                draw_string(g_renderer, 88, cy, card_get_name(ct));

                // Hiển thị phạm vi (NODE/GLOBAL)
                SDL_SetRenderDrawColor(g_renderer, 100, 180, 255, 255);
                draw_string(g_renderer, 310, cy, scope_str);
            }
        } else {
            // -- Giao diện chọn node mục tiêu (STATE_CARD_TARGET) --
            SDL_SetRenderDrawColor(g_renderer, 255, 215, 0, 255);
            draw_string(g_renderer, 160, 162, "CHON THANH MUC TIEU");

            SDL_SetRenderDrawColor(g_renderer, 200, 200, 200, 255);
            draw_string(g_renderer, 100, 190, card_get_name(gs->pending_card));
            draw_string(g_renderer, 280, 190, "-> OK DE AP DUNG");

            // Highlight các node hợp lệ (của Player) trên map
            for (uint8_t i = 0; i < MAX_NODES; i++) {
                if (gs->nodes[i].owner == OWNER_PLAYER) {
                    int cx = gs->nodes[i].x * scale - 12;
                    int cy = gs->nodes[i].y * scale - 12;
                    if (i == gs->cursor) {
                        // Node đang chọn: Cursor vàng nhấp nháy
                        SDL_SetRenderDrawColor(g_renderer, 255, 255, 0, 255);
                    } else {
                        // Node hợp lệ khác: viền xanh lá nhạt
                        SDL_SetRenderDrawColor(g_renderer, 0, 180, 0, 255);
                    }
                    draw_bitmap16(g_renderer, cx - 6, cy - 6, sprite_cursor, 12, 12, 3);
                }
            }
        }
    }

    // Đẩy hình ảnh ra màn hình SDL
    SDL_RenderPresent(g_renderer);
}
