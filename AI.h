#pragma once
#include "Board.h"

class AI {
public:
    // Returns the result of the move (king capture, ok, or none)
    Board::MoveResult makeMove(Board& board, Color color);

private:
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, Color aiColor, int& bestSx, int& bestSy, int& bestDx, int& bestDy);
    int evaluateBoard(const Board& board, Color aiColor);
    bool isKingAlive(const Board& board, Color color) const;
};
