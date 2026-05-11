/**
 * @file game_types.h
 * @brief Định nghĩa các cấu trúc dữ liệu và kiểu dữ liệu (Struct, Enum) cốt lõi của game RTS.
 *        Tất cả các module khác sẽ include file này để dùng chung dữ liệu mà không bị lỗi Circular Dependency.
 */
#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ── Enums ── */
/**
 * @brief Phe phái sở hữu các Node trong game
 */
typedef enum { 
    OWNER_NEUTRAL = 0, // Phe trung lập
    OWNER_PLAYER = 1,  // Người chơi (Lưu Bị)
    OWNER_AI = 2       // Máy (Tào Tháo)
} Owner_t;

/**
 * @brief Các trạng thái chính của Game State Machine
 */
typedef enum {
    STATE_MAIN_MENU = 0,     // Màn hình menu chính
    STATE_INIT,              // Khởi tạo game
    STATE_IDLE,              // Trạng thái chờ, người chơi di chuyển con trỏ
    STATE_SRC_SELECTED,      // Đã chọn thành trì nguồn, chờ chọn mục tiêu
    STATE_INTERNAL_AFFAIRS,  // Nội Chính: chọn thẻ bài
    STATE_CARD_TARGET,       // Nội Chính: chọn node mục tiêu để áp dụng thẻ
    STATE_GAME_OVER          // Trò chơi kết thúc
} GameStateEnum_t;

/**
 * @brief Cấu trúc lưu trữ các buff TOÀN CỤC của người chơi (từ thẻ GLOBAL)
 */
typedef struct {
    uint8_t hero_bonus;    // Hệ số nhân sát thương khi có Hero (x1 mặc định)
    uint8_t march_speed;   // Bonus tốc độ hành quân (%) toàn bộ đạo quân
} PlayerBuffs_t;

/* ── Structs ── */
#define MAX_NODES 6      // Số lượng tối đa các thành trì trên bản đồ
#define NO_NODE   0xFF   // Giá trị biểu diễn không có thành trì nào được chọn

#define LEVEL_2_THRESHOLD 30
#define LEVEL_3_THRESHOLD 60

/**
 * @brief Cấu trúc dữ liệu của một Node (Thành trì)
 */
typedef struct {
    uint8_t x, y;              // Tọa độ trên màn hình để render
    uint8_t troops;            // Số lượng quân lính hiện tại
    uint8_t level;             // Cấp độ của thành trì (1, 2, 3...)
    bool    has_hero;          // Thành trì có Hero hay không
    uint8_t owner;             // Phe sở hữu (Owner_t)
    uint8_t node_spawn_bonus;  // Buff Đồn Điền: +X quân/tick riêng cho node này
    uint8_t node_defense_bonus;// Buff Tu Thành: +X điểm giáp riêng cho node này
} Node_t;

/**
 * @brief Cấu trúc dữ liệu cho một đạo quân đang hành quân
 */
typedef struct {
    uint8_t src, dst;    // Node nguồn và Node đích
    uint8_t troops;      // Số lượng quân lính tham gia hành quân
    uint8_t progress;    // Tiến trình di chuyển (0 - 100%)
    bool    is_hero_march; // Đạo quân này có chứa Hero hay không
    uint8_t owner;       // Phe sở hữu đội quân (Owner_t)
} March_t;

#define MAX_MARCHES 8

/**
 * @brief Trạng thái tổng thể của ván game
 */
typedef struct {
    Node_t  nodes[MAX_NODES];   // Danh sách các thành trì
    uint8_t adj[MAX_NODES];     // Ma trận kề (bitmask) lưu kết nối giữa các Node
    uint8_t cost[MAX_NODES][MAX_NODES]; // Ma trận trọng số của các cạnh (cost di chuyển)
    March_t marches[MAX_MARCHES]; // Mảng thông tin các đạo quân đang hành quân
    uint8_t cursor;             // Vị trí con trỏ hiện tại đang trỏ vào Node nào
    uint8_t selected_src;       // Node đang được chọn để xuất quân
    bool    hero_selected;      // Đã chọn Hero để xuất quân hay chưa
    uint16_t map_seed;          // Seed để sinh map
    uint8_t tick;               // Bộ đếm thời gian (tick) trong game
    uint16_t year;              // Năm hiện tại trong game (bắt đầu từ 184)
    PlayerBuffs_t buffs;        // Các buff toàn cục của người chơi
    uint8_t cards_offered[3];   // 3 thẻ bài đang được hiển thị để chọn
    uint8_t card_cursor;        // Vị trí con trỏ thẻ bài (0-2)
    uint8_t pending_card;       // Thẻ bài đang chờ chọn node mục tiêu
    bool    hero_homeless;      // Hero đang "lưu vong" (mất thành, chờ tái sinh)
    uint8_t dirty;              // Cờ đánh dấu cần render lại màn hình
} GameState_t;

#endif // GAME_TYPES_H
