/**
 * @file game_card.h
 * @brief Định nghĩa và khai báo hệ thống Thẻ Bài Nội Chính.
 *        Mỗi thẻ bài có thể tác dụng lên toàn cục (GLOBAL) hoặc lên một
 *        thành trì cụ thể (NODE) do người chơi chọn sau khi bốc thẻ.
 */
#ifndef GAME_CARD_H
#define GAME_CARD_H

#include "game_types.h"

/* ── Enums ── */
/**
 * @brief Các loại thẻ bài Nội Chính (6 thẻ tổng cộng)
 */
typedef enum {
    CARD_SPAWN_BOOST   = 0, // Đồn Điền:  +1 quân/tick (NODE)
    CARD_DEFENSE_BOOST = 1, // Tu Thành:  +5 giáp (NODE)
    CARD_RECRUIT       = 2, // Mộ Binh:   +10 quân ngay lập tức (NODE)
    CARD_HERO_BOOST    = 3, // Luyện Binh: +1 hệ số sát thương Hero (GLOBAL)
    CARD_SWIFT_MARCH   = 4, // Tốc Hành:  +20% tốc độ hành quân (GLOBAL)
    CARD_TOTAL_COUNT   = 5  // Tổng số loại thẻ (sentinel)
} CardType_t;

/**
 * @brief Phạm vi tác dụng của thẻ bài
 */
typedef enum {
    SCOPE_NODE   = 0, // Thẻ cần người chơi chọn 1 thành trì mục tiêu
    SCOPE_GLOBAL = 1  // Thẻ tác dụng toàn bộ, không cần chọn node
} CardScope_t;

/* ── Functions ── */

/**
 * @brief Truy vấn phạm vi tác dụng của một loại thẻ bài.
 * @param card_type Loại thẻ (CardType_t)
 * @return SCOPE_NODE hoặc SCOPE_GLOBAL
 */
CardScope_t card_get_scope(uint8_t card_type);

/**
 * @brief Truy vấn tên hiển thị ngắn (tối đa 12 ký tự) của một loại thẻ bài.
 * @param card_type Loại thẻ
 * @return Con trỏ chuỗi hằng (const char*)
 */
const char* card_get_name(uint8_t card_type);

/**
 * @brief Áp dụng hiệu ứng của một thẻ bài vào trạng thái game.
 *        Với thẻ SCOPE_NODE, target_node phải là index node hợp lệ.
 *        Với thẻ SCOPE_GLOBAL, target_node có thể truyền NO_NODE.
 * @param gs         Con trỏ tới GameState_t
 * @param card_type  Loại thẻ cần áp dụng
 * @param target_node Index của node được chọn (với SCOPE_NODE)
 */
void card_apply(GameState_t* gs, uint8_t card_type, uint8_t target_node);

/**
 * @brief Shuffle ngẫu nhiên 3 thẻ bài từ tổng số 5 loại để người chơi chọn.
 *        Kết quả được ghi thẳng vào gs->cards_offered[0..2].
 * @param gs Con trỏ tới GameState_t
 */
void card_shuffle_offered(GameState_t* gs);

#endif // GAME_CARD_H
