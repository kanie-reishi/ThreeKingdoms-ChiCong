/**
 * @file game_card.cpp
 * @brief Triển khai hệ thống Thẻ Bài Nội Chính.
 *        Bao gồm: áp dụng buff, shuffle thẻ ngẫu nhiên, truy vấn metadata thẻ.
 */
#include "game_card.h"

/* ── Lookup Tables ── */

/**
 * @brief Bảng tra phạm vi tác dụng của từng loại thẻ.
 *        Index tương ứng với CardType_t enum.
 */
static const CardScope_t card_scope_table[CARD_TOTAL_COUNT] = {
    SCOPE_NODE,    // CARD_SPAWN_BOOST   (Đồn Điền)
    SCOPE_NODE,    // CARD_DEFENSE_BOOST (Tu Thành)
    SCOPE_NODE,    // CARD_RECRUIT       (Mộ Binh)
    SCOPE_GLOBAL,  // CARD_HERO_BOOST    (Luyện Binh)
    SCOPE_GLOBAL,  // CARD_SWIFT_MARCH   (Tốc Hành)
};

/**
 * @brief Bảng tra tên hiển thị ngắn gọn của từng loại thẻ (A-Z, max 12 ký tự).
 */
static const char* card_name_table[CARD_TOTAL_COUNT] = {
    "DON DIEN",    // CARD_SPAWN_BOOST
    "TU THANH",    // CARD_DEFENSE_BOOST
    "MO BINH",     // CARD_RECRUIT
    "LUYEN BINH",  // CARD_HERO_BOOST
    "TOC HANH",    // CARD_SWIFT_MARCH
};

/* ── Pseudo-random counter để shuffle ── */
static uint16_t shuffle_seed = 0x5A3C;

static uint16_t prng_next() {
    shuffle_seed ^= (shuffle_seed << 7);
    shuffle_seed ^= (shuffle_seed >> 9);
    shuffle_seed ^= (shuffle_seed << 8);
    return shuffle_seed;
}

/* ── Public Functions ── */

CardScope_t card_get_scope(uint8_t card_type) {
    if (card_type >= CARD_TOTAL_COUNT) return SCOPE_GLOBAL;
    return card_scope_table[card_type];
}

const char* card_get_name(uint8_t card_type) {
    if (card_type >= CARD_TOTAL_COUNT) return "UNKNOWN";
    return card_name_table[card_type];
}

void card_apply(GameState_t* gs, uint8_t card_type, uint8_t target_node) {
    switch (card_type) {
        case CARD_SPAWN_BOOST:
            // Đồn Điền: Tăng tốc sinh quân tại node cụ thể (+1 quân/tick)
            if (target_node < MAX_NODES && gs->nodes[target_node].owner == OWNER_PLAYER) {
                if (gs->nodes[target_node].node_spawn_bonus < 5) { // Giới hạn cộng dồn tối đa
                    gs->nodes[target_node].node_spawn_bonus++;
                }
            }
            break;

        case CARD_DEFENSE_BOOST:
            // Tu Thành: Tăng điểm giáp phòng thủ tại node cụ thể (+5 điểm)
            if (target_node < MAX_NODES && gs->nodes[target_node].owner == OWNER_PLAYER) {
                if (gs->nodes[target_node].node_defense_bonus < 20) { // Giới hạn tối đa
                    gs->nodes[target_node].node_defense_bonus += 5;
                }
            }
            break;

        case CARD_RECRUIT:
            // Mộ Binh: Cộng ngay 10 quân vào node được chọn
            if (target_node < MAX_NODES && gs->nodes[target_node].owner == OWNER_PLAYER) {
                gs->nodes[target_node].troops += 10;
                if (gs->nodes[target_node].troops > 99) gs->nodes[target_node].troops = 99;
            }
            break;

        case CARD_HERO_BOOST:
            // Luyện Binh: Tăng toàn cục hệ số nhân sát thương Hero (+1)
            if (gs->buffs.hero_bonus < 5) { // Giới hạn tối đa nhân 5
                gs->buffs.hero_bonus++;
            }
            break;

        case CARD_SWIFT_MARCH:
            // Tốc Hành: Tăng toàn cục tốc độ hành quân (+20%)
            if (gs->buffs.march_speed < 80) { // Giới hạn tối đa +80%
                gs->buffs.march_speed += 20;
            }
            break;

        default:
            break;
    }
}

void card_shuffle_offered(GameState_t* gs) {
    // Tạo pool 5 thẻ, shuffle, rút 3 thẻ đầu tiên
    uint8_t pool[CARD_TOTAL_COUNT];
    for (uint8_t i = 0; i < CARD_TOTAL_COUNT; i++) {
        pool[i] = i;
    }

    // Trộn bài (Fisher-Yates shuffle)
    // Kết hợp seed với year để kết quả khác mỗi năm
    shuffle_seed ^= (uint16_t)gs->year;
    for (uint8_t i = CARD_TOTAL_COUNT - 1; i > 0; i--) {
        uint8_t j = prng_next() % (i + 1);
        uint8_t tmp = pool[i];
        pool[i] = pool[j];
        pool[j] = tmp;
    }

    // Lấy 3 thẻ đầu tiên sau khi trộn
    gs->cards_offered[0] = pool[0];
    gs->cards_offered[1] = pool[1];
    gs->cards_offered[2] = pool[2];
    gs->card_cursor = 0;
}
