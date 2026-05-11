/**
 * @file game_map.cpp
 * @brief Chứa thuật toán Prim sinh MST và các thiết lập tọa độ ban đầu cho map.
 *        Cung cấp 2 hàm khởi tạo: Map cố định (Fixed Map) và Map động ngẫu nhiên (Procedural Map).
 */
#include "game_map.h"

/**
 * @brief Khởi tạo một bản đồ tĩnh (Fixed Map) với các thông số mặc định cứng.
 *        Sử dụng cho chế độ chơi cơ bản.
 * @param gs Con trỏ tới GameState_t
 */
void game_map_init_fixed(GameState_t* gs) {
    // 1. Cấu hình tọa độ, quân số ban đầu, cấp độ và Hero cho 6 Node mặc định.
    gs->nodes[0] = {10, 10, 20, 1, true, OWNER_PLAYER};  // Base của người chơi (có Hero)
    gs->nodes[1] = {50, 10,  5, 1, false, OWNER_NEUTRAL}; // Các thành trung lập
    gs->nodes[2] = {90, 10,  5, 1, false, OWNER_NEUTRAL};
    gs->nodes[3] = {10, 45,  5, 1, false, OWNER_NEUTRAL};
    gs->nodes[4] = {50, 45,  5, 1, false, OWNER_NEUTRAL};
    gs->nodes[5] = {90, 45, 20, 1, true, OWNER_AI};      // Base của AI (có Hero)

    // 2. Định nghĩa các cạnh nối giữa các Node (dùng bitmask)
    gs->adj[0] = (1<<1)|(1<<3);
    gs->adj[1] = (1<<0)|(1<<2)|(1<<4);
    gs->adj[2] = (1<<1)|(1<<5);
    gs->adj[3] = (1<<0)|(1<<4);
    gs->adj[4] = (1<<1)|(1<<3)|(1<<5);
    gs->adj[5] = (1<<2)|(1<<4);

    // 3. Khởi tạo ma trận trọng số (cost) mặc định = 4 (Đường bằng phẳng)
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        for (uint8_t j = 0; j < MAX_NODES; j++) {
            gs->cost[i][j] = 4;
        }
    }
    
    // 4. Thiết lập một số đường cụ thể thành đường núi (cost = 10, di chuyển chậm hơn)
    gs->cost[1][4] = 10; gs->cost[4][1] = 10;
    gs->cost[2][5] = 10; gs->cost[5][2] = 10;

    // 5. Xóa sạch các đạo quân đang hành quân (reset lại mảng marches)
    for (uint8_t i = 0; i < MAX_MARCHES; i++) {
        gs->marches[i].troops = 0;
        gs->marches[i].progress = 0;
        gs->marches[i].is_hero_march = false;
    }

    // 6. Đặt lại các biến trạng thái giao diện và thời gian
    gs->cursor         = 0;
    gs->selected_src   = NO_NODE;
    gs->hero_selected  = false;
    gs->tick           = 0;
    gs->year           = 184;
    gs->buffs.hero_bonus   = 1;  // Hệ số nhân mặc định = 1 (chưa tăng)
    gs->buffs.march_speed  = 0;  // Không có bonus tốc độ bổ sung
    gs->hero_homeless      = false;
    gs->pending_card       = 0xFF;
    // Reset bộ đếm buff riêng của từng node về 0
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        gs->nodes[i].node_spawn_bonus   = 0;
        gs->nodes[i].node_defense_bonus = 0;
    }
    gs->dirty = 1;
}

/* Biến lưu trữ trạng thái của bộ sinh số ngẫu nhiên */
static uint32_t current_seed;

/**
 * @brief Hàm sinh số ngẫu nhiên Pseudo-Random Number Generator (PRNG).
 *        Sử dụng thuật toán LCG (Linear Congruential Generator) đơn giản.
 * @return uint32_t Số ngẫu nhiên
 */
static uint32_t my_rand() {
    current_seed = current_seed * 1103515245 + 12345;
    return (current_seed >> 16);
}

/**
 * @brief Tính bình phương khoảng cách giữa 2 điểm (A,B).
 *        Việc dùng bình phương giúp tránh phải dùng hàm sqrt() gây tốn CPU trên hệ thống nhúng.
 */
static uint32_t get_dist_sq(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx*dx + dy*dy;
}

/**
 * @brief Khởi tạo một bản đồ động (Procedural Map) với cấu trúc và trọng số sinh ngẫu nhiên.
 * @param gs Con trỏ tới GameState_t
 * @param seed Hạt giống ngẫu nhiên (dùng để sinh map)
 */
void game_map_init_procedural(GameState_t* gs, uint16_t seed) {
    // Lưu lại seed để có thể tái tạo (reproduce) map này nếu cần
    current_seed = seed;
    gs->map_seed = seed;
    
    // Bước 1: Rải ngẫu nhiên các Node lên bản đồ
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        bool valid = false;
        while (!valid) {
            uint8_t rx = 10 + (my_rand() % 100); // Tung tọa độ X ngẫu nhiên (10 đến 109)
            uint8_t ry = 10 + (my_rand() % 40);  // Tung tọa độ Y ngẫu nhiên (10 đến 49)
            
            valid = true;
            // Kiểm tra xem tọa độ vừa tung có bị trùng hoặc quá gần với các Node đã tạo không
            for (uint8_t j = 0; j < i; j++) {
                if (get_dist_sq(rx, ry, gs->nodes[j].x, gs->nodes[j].y) < 400) { 
                    valid = false; // Khoảng cách < 20 (20^2 = 400), cần tung lại
                    break;
                }
            }
            
            // Nếu hợp lệ, gán vào Node
            if (valid) {
                uint8_t init_troops = 5 + (my_rand() % 11); // Sinh 5 đến 15 quân ngẫu nhiên cho thành trung lập
                gs->nodes[i] = {rx, ry, init_troops, 1, false, OWNER_NEUTRAL};
                gs->adj[i] = 0; // Xóa kết nối cũ
            }
        }
    }
    
    // Bước 2: Thiết lập Node 0 cho Người chơi và Node cuối cho AI
    gs->nodes[0].owner = OWNER_PLAYER;
    gs->nodes[0].troops = 20;
    gs->nodes[0].has_hero = true; // Người chơi nhận 1 Hero
    
    gs->nodes[MAX_NODES-1].owner = OWNER_AI;
    gs->nodes[MAX_NODES-1].troops = 20;
    gs->nodes[MAX_NODES-1].has_hero = true; // AI nhận 1 Hero

    // Bước 3: Áp dụng thuật toán Minimum Spanning Tree (Prim) để kết nối các Node
    //         Điều này đảm bảo bản đồ LUÔN liên thông (mọi Node đều có thể đi tới nhau).
    bool in_mst[MAX_NODES] = {false};
    in_mst[0] = true; // Bắt đầu từ Node 0
    
    for (uint8_t edges = 0; edges < MAX_NODES - 1; edges++) {
        uint8_t best_u = 0, best_v = 0;
        uint32_t min_dist = 0xFFFFFFFF;
        
        // Tìm cạnh ngắn nhất nối từ một đỉnh ĐÃ có trong MST đến một đỉnh CHƯA có trong MST
        for (uint8_t u = 0; u < MAX_NODES; u++) {
            if (in_mst[u]) {
                for (uint8_t v = 0; v < MAX_NODES; v++) {
                    if (!in_mst[v]) {
                        uint32_t dist = get_dist_sq(gs->nodes[u].x, gs->nodes[u].y, gs->nodes[v].x, gs->nodes[v].y);
                        if (dist < min_dist) {
                            min_dist = dist;
                            best_u = u;
                            best_v = v;
                        }
                    }
                }
            }
        }
        // Thêm cạnh ngắn nhất vừa tìm được vào đồ thị (kết nối u và v 2 chiều)
        gs->adj[best_u] |= (1 << best_v);
        gs->adj[best_v] |= (1 << best_u);
        in_mst[best_v] = true; // Đưa v vào MST
    }
    
    // Bước 4: Thêm 2 cạnh ngẫu nhiên để tạo các vòng lặp (Cycles)
    //         Giúp map chiến thuật hơn thay vì chỉ có 1 con đường duy nhất (dạng cây).
    for (int i = 0; i < 2; i++) {
        uint8_t u = my_rand() % MAX_NODES;
        uint8_t v = my_rand() % MAX_NODES;
        if (u != v) {
            gs->adj[u] |= (1 << v);
            gs->adj[v] |= (1 << u);
        }
    }

    // Bước 5: Tính Cost (Độ khó địa hình/độ dài) tự động cho các cạnh
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        for (uint8_t j = 0; j < MAX_NODES; j++) {
            if (gs->adj[i] & (1 << j)) {
                // Cost tỷ lệ thuận với khoảng cách giữa 2 điểm (Khoảng cách dài = Hành quân chậm)
                uint32_t dist_sq = get_dist_sq(gs->nodes[i].x, gs->nodes[i].y, gs->nodes[j].x, gs->nodes[j].y);
                uint8_t c = (dist_sq / 400) + 2; 
                if (c > 12) c = 12; // Giới hạn cost tối đa (Tránh đường quá chậm không thể kết thúc)
                gs->cost[i][j] = c;
            } else {
                gs->cost[i][j] = 255; // Không có đường đi
            }
        }
    }

    // Bước 6: Reset các đạo quân và trạng thái game
    for (uint8_t i = 0; i < MAX_MARCHES; i++) {
        gs->marches[i].troops = 0;
        gs->marches[i].progress = 0;
        gs->marches[i].is_hero_march = false;
    }

    gs->cursor         = 0;
    gs->selected_src   = NO_NODE;
    gs->hero_selected  = false;
    gs->tick           = 0;
    gs->year           = 184;
    gs->buffs.hero_bonus   = 1;
    gs->buffs.march_speed  = 0;
    gs->hero_homeless      = false;
    gs->pending_card       = 0xFF;
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        gs->nodes[i].node_spawn_bonus   = 0;
        gs->nodes[i].node_defense_bonus = 0;
    }
    gs->dirty = 1;
}
