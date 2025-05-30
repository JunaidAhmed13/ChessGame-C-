#pragma once
#include "Piece.h"
#include <SFML/Graphics.hpp>

class Board {
public:
    enum MoveResult {
        MOVE_NONE,
        MOVE_OK,
        MOVE_WHITE_KING_CAPTURED,
        MOVE_BLACK_KING_CAPTURED
    };

    Board();
    ~Board();

    Piece* getPiece(int x, int y) const;
    void setPiece(int x, int y, Piece* piece);
    bool isPathClear(int sx, int sy, int dx, int dy) const;
    bool isInsideBoard(int x, int y) const;

    bool isValidMove(int sx, int sy, int dx, int dy, Color color) const;
    MoveResult movePiece(int sx, int sy, int dx, int dy);

    void setup();
    void draw(sf::RenderWindow& window) const;

    // Pawn promotion support
    bool promotionPending = false;
    Color promotionColor;
    void promotePawn(int x, int y, Piece* newPiece);

private:
    Piece* squares[8][8];
};
