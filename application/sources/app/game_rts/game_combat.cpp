/**
 * @file game_combat.cpp
 * @brief Triển khai logic tính toán giao tranh và chiếm thành.
 *
 * Sức chiến đấu được tính dựa trên:
 *  - Số quân lính (atk_power / def_power)
 *  - Buff toàn cục (hero_bonus từ thẻ Luyện Binh)
 *  - Buff riêng node (node_defense_bonus từ thẻ Tu Thành)
 *
 * Khi một thành bị chiếm, TẤT CẢ buff riêng của node đó sẽ bị reset về 0.
 */
#include "game_combat.h"
#include "game_hero.h"

void combat_resolve(GameState_t* gs, uint8_t march_idx) {
    March_t* m   = &gs->marches[march_idx];
    Node_t*  dst = &gs->nodes[m->dst];

    // ── Tiếp viện: cùng phe ──────────────────────────────────────────────
    if (dst->owner == m->owner) {
        dst->troops += m->troops;
        if (dst->troops > 99) dst->troops = 99;
        if (m->is_hero_march) hero_handle_transfer(gs, m->dst);
        return; // Kết thúc sớm
    }

    // ── Giao tranh: khác phe ─────────────────────────────────────────────
    // Tính sức tấn công (kể cả buff Hero toàn cục)
    uint16_t atk_power = m->troops;
    if (m->owner == OWNER_PLAYER && m->is_hero_march) {
        atk_power *= gs->buffs.hero_bonus;
    }

    // Tính sức phòng thủ (Hero + Giáp node)
    uint16_t def_power = dst->troops;
    if (dst->owner == OWNER_PLAYER) {
        if (dst->has_hero) def_power *= gs->buffs.hero_bonus;
        def_power += dst->node_defense_bonus; // Buff Tu Thành riêng cho node này
    }

    // ── Kẻ công thắng -> Chiếm thành ──────────────────────────────────
    if (atk_power > def_power) {
        uint8_t old_owner = dst->owner;

        // [FIX] Nếu thành đang thủ có Hero, xử lý respawn cho phe phòng thủ
        if (dst->has_hero) {
            dst->has_hero = false; // Xóa flag TRƯỚC khi đổi chủ
            hero_handle_defeat(gs, old_owner);
        }

        // Thực hiện đổi chủ
        dst->owner = m->owner;

        // [FIX] Reset buff node khi thành đổi chủ (buff mất theo chủ quyền)
        dst->node_spawn_bonus   = 0;
        dst->node_defense_bonus = 0;

        // Tính quân sót lại của phe công
        uint16_t remaining = m->troops - (m->troops * def_power / atk_power);
        dst->troops = (remaining > 0) ? (uint8_t)remaining : 1;

        if (m->is_hero_march) hero_handle_transfer(gs, m->dst);

    // ── Kẻ công thua -> Thành thủ giữ nguyên ─────────────────────────
    } else {
        // [FIX] Nếu đạo quân tấn công có Hero, xử lý respawn ngay khi thất bại
        if (m->is_hero_march) {
            hero_handle_defeat(gs, m->owner);
        }

        // Tính quân sót lại của phe thủ (tỷ lệ tổn thất theo sức tấn công)
        uint16_t remaining = 0;
        if (def_power > 0) {
            remaining = dst->troops - (dst->troops * atk_power / def_power);
        }
        dst->troops = (uint8_t)remaining;
    }
}
