/**
 * @file game_logic.cpp
 * @brief Chứa các hàm logic tiện ích chung không phụ thuộc cụ thể vào Node hay Combat.
 *        Đặc biệt bao gồm logic kiểm tra điều kiện kết thúc game (thắng/thua).
 */
#include "game_logic.h"

/**
 * @brief Kiểm tra xem ván game đã kết thúc hay chưa bằng cách đếm số thành trì của mỗi phe.
 *        Hàm này sẽ được gọi ở mỗi Tick chung của hệ thống.
 * 
 * @param gs Con trỏ tới trạng thái game hiện tại (GameState_t)
 * @return Owner_t Phe chiến thắng (OWNER_PLAYER, OWNER_AI) hoặc OWNER_NEUTRAL nếu game chưa ngã ngũ.
 */
uint8_t check_win(GameState_t* gs) {
    uint8_t ai_nodes = 0, player_nodes = 0;
    
    // Duyệt qua toàn bộ các thành trì trên bản đồ
    for (uint8_t i = 0; i < MAX_NODES; i++) {
        if (gs->nodes[i].owner == OWNER_AI) {
            ai_nodes++; // Tăng biến đếm nếu đây là thành của AI
        }
        if (gs->nodes[i].owner == OWNER_PLAYER) {
            player_nodes++; // Tăng biến đếm nếu đây là thành của người chơi
        }
    }
    
    // Nếu AI không còn bất kỳ thành trì nào -> Người chơi chiếm toàn bộ -> Người chơi thắng
    if (ai_nodes == 0)     return OWNER_PLAYER; 
    
    // Nếu Người chơi không còn thành trì nào -> AI chiếm toàn bộ -> AI thắng
    if (player_nodes == 0) return OWNER_AI;     
    
    // Nếu cả hai phe đều còn ít nhất 1 thành trì -> Trò chơi tiếp diễn
    return OWNER_NEUTRAL;                       
}