#pragma once

#include <iostream>

#include "ChessModel.h"

class GameState
{
public:
    ChessPiece board[8][8];

    GameState();

    void Setup();

    friend std::ostream& operator<<(std::ostream& out, const GameState& C) {
        for (int r = 0; r < 8; r++)
        {
            for (int c = 0; c < 8; c++)
            {
                switch (C.board[r][c])
                {
                case empty:
                    out << " ";
                    break;
                case pawn:
                    out << "P";
                    break;
                case rook:
                    out << "R";
                    break;
                case knight:
                    out << "N";
                    break;
                case bishop:
                    out << "B";
                    break;
                case queen:
                    out << "Q";
                    break;
                case king:
                    out << "K";
                    break;
                }
                out << " ";
            }
            out << "\n";
        }
        return out;
    }
};