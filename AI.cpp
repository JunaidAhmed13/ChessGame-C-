#include "AI.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"
#include <algorithm>
#include <limits>

// Piece values for evaluation
static int pieceValue(const Piece* p) {
    if (!p) return 0;
    if (dynamic_cast<const Queen*>(p))  return 900;
    if (dynamic_cast<const Rook*>(p))   return 500;
    if (dynamic_cast<const Bishop*>(p)) return 330;
    if (dynamic_cast<const Knight*>(p)) return 320;
    if (dynamic_cast<const Pawn*>(p))   return 100;
    if (dynamic_cast<const King*>(p))   return 20000;
    return 0;
}

// Evaluate the board from aiColor's perspective
int AI::evaluateBoard(const Board& board, Color aiColor) {
    int score = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Piece* p = board.getPiece(x, y);
            if (p) {
                int val = pieceValue(p);
                score += (p->getColor() == aiColor) ? val : -val;
            }
        }
    }
    return score;
}

bool AI::isKingAlive(const Board& board, Color color) const {
    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 8; ++y) {
            Piece* p = board.getPiece(x, y);
            if (p && dynamic_cast<King*>(p) && p->getColor() == color)
                return true;
        }
    return false;
}

// Minimax with alpha-beta pruning
int AI::minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, Color aiColor, int& bestSx, int& bestSy, int& bestDx, int& bestDy) {
    Color currentColor = maximizingPlayer ? aiColor : (aiColor == WHITE ? BLACK : WHITE);

    // Check for king capture
    if (!isKingAlive(board, aiColor)) return -1000000 - depth;
    if (!isKingAlive(board, aiColor == WHITE ? BLACK : WHITE)) return 1000000 + depth;

    if (depth == 0) {
        return evaluateBoard(board, aiColor);
    }

    int bestScore = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    for (int sx = 0; sx < 8; ++sx) {
        for (int sy = 0; sy < 8; ++sy) {
            Piece* piece = board.getPiece(sx, sy);
            if (piece && piece->getColor() == currentColor) {
                for (int dx = 0; dx < 8; ++dx) {
                    for (int dy = 0; dy < 8; ++dy) {
                        if ((sx != dx || sy != dy) && board.isValidMove(sx, sy, dx, dy, currentColor)) {
                            // Save state
                            Piece* captured = board.getPiece(dx, dy);
                            Piece* moving = board.getPiece(sx, sy);
                            board.setPiece(dx, dy, moving);
                            board.setPiece(sx, sy, nullptr);

                            // Handle promotion for pawns
                            Piece* promoted = nullptr;
                            Pawn* pawn = dynamic_cast<Pawn*>(moving);
                            if (pawn && ((pawn->getColor() == WHITE && dy == 7) || (pawn->getColor() == BLACK && dy == 0))) {
                                promoted = new Queen(pawn->getColor());
                                delete board.getPiece(dx, dy);
                                board.setPiece(dx, dy, promoted);
                            }

                            int dummySx, dummySy, dummyDx, dummyDy;
                            int score = minimax(board, depth - 1, alpha, beta, !maximizingPlayer, aiColor, dummySx, dummySy, dummyDx, dummyDy);

                            // Undo move
                            if (promoted) {
                                delete board.getPiece(dx, dy);
                                board.setPiece(dx, dy, pawn);
                            }
                            board.setPiece(sx, sy, moving);
                            board.setPiece(dx, dy, captured);

                            if (maximizingPlayer) {
                                if (score > bestScore) {
                                    bestScore = score;
                                    if (depth == 4) { // root call, store best move
                                        bestSx = sx; bestSy = sy; bestDx = dx; bestDy = dy;
                                    }
                                }
                                alpha = std::max(alpha, score);
                                if (beta <= alpha) return bestScore;
                            }
                            else {
                                if (score < bestScore) {
                                    bestScore = score;
                                }
                                beta = std::min(beta, score);
                                if (beta <= alpha) return bestScore;
                            }
                        }
                    }
                }
            }
        }
    }
    return bestScore;
}

// Boss-level AI: Minimax with alpha-beta pruning, depth 4 (increase for even harder AI)
Board::MoveResult AI::makeMove(Board& board, Color color) {
    int bestSx = -1, bestSy = -1, bestDx = -1, bestDy = -1;
    int depth = 4; // Increase to 5 or 6 for even harder AI (but will be slower)
    minimax(board, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, color, bestSx, bestSy, bestDx, bestDy);

    if (bestSx != -1 && bestSy != -1 && bestDx != -1 && bestDy != -1) {
        return board.movePiece(bestSx, bestSy, bestDx, bestDy);
    }
    return Board::MOVE_NONE;
}
