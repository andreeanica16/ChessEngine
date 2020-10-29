#include "engine.h"
#include "eval.h"

void translatePosition(int line, int col, int piece) {
    positions[piece] = line * 10 + col;
}

void initializeBoard() {
    positions.clear();
    for (int i = 3; i <= 6; i++) {
        for (int j = 1; j <= 8; j++) {
            board[i][j] = EMPTY;
        }
    }

    int pawn_w = PAWN1_W;
    int pawn_b = PAWN1_B;
    for (int i = 1; i <= 8; i++) {
        translatePosition(7, i, pawn_b);
        translatePosition(2, i, pawn_w);
        board[7][i] = pawn_b--;
        board[2][i] = pawn_w++;
    }

    int piece_w = ROOK1_W;
    int piece_b = ROOK1_B;
    for (int i = 1; i <= 3; i++) {
        translatePosition(1, i, piece_w);
        translatePosition(1, 8 - i + 1, piece_w + 1);
        board[1][i] = piece_w++;
        board[1][8 - i + 1] = piece_w;
        piece_w -= 11;

        translatePosition(8, i, piece_b);
        translatePosition(8, 8 - i + 1, piece_b - 1);
        board[8][i] = piece_b--;
        board[8][8 - i + 1] = piece_b;
        piece_b += 11;
    }

    board[1][4] = QUEEN_W;
    board[1][5] = KING_W;
    board[8][4] = QUEEN_B;
    board[8][5] = KING_B;
    translatePosition(1, 4, QUEEN_W);
    translatePosition(1, 5, KING_W);
    translatePosition(8, 4, QUEEN_B);
    translatePosition(8, 5, KING_B);
}

void initializeAttacked() {
    for (int i = 0; i < 2; i++) {
        for (int j = 1; j <= 8; j++) {
            for (int k = 1; k <= 8; k++) {
                attacked[i][j][k].clear();
            }
        }
    }
}

void printMap() {
    for (auto &pair : positions) {
        cout << pair.first << " " << pair.second << endl;
    }
}

void printBoard() {
    for (int i = 8; i >= 1; i--) {
        for (int j = 1; j <= 8; j++) {
            if (abs(board[i][j]) / 10 == 5)
                printf("%3c ", 'P');
            else if (abs(board[i][j]) / 10 == 4)
                printf("%3c ", 'T');
            else if (abs(board[i][j]) / 10 == 3)
                printf("%3c ", 'C');
            else if (abs(board[i][j]) / 10 == 2)
                printf("%3c ", 'N');
            else if (abs(board[i][j]) == 10)
                printf("%3c ", 'K');
            else if (abs(board[i][j]) == 11 || abs(board[i][j]) / 10 == 6)
                printf("%3c ", 'Q');
            else printf("%3c ", '.');
        }
        cout << endl;
    }
}

void readFromInput(string &input) {
    getline(cin, input);
    if (input.size() > 77) {
        exit(0);
    }
}

char *makeMove(int colFrom, int colTo, int lineFrom, int lineTo) {
    char *move = (char *) malloc(7);
    move[0] = colFrom + 'a' - 1;
    move[1] = lineFrom + '0';
    move[2] = colTo + 'a' - 1;
    move[3] = lineTo + '0';
    move[4] = '\0';

    return move;
}

//void makeMoveForMinimax()
void translateStringToPositions(char *move, int *colFrom, int *colTo,
                                int *lineFrom, int *lineTo) {
    *colFrom = move[0] - 'a' + 1;
    *colTo = move[2] - 'a' + 1;
    *lineFrom = move[1] - '0';
    *lineTo = move[3] - '0';
}

void markMoveOnBoard(char *move) {
    int colFrom, colTo, lineFrom, lineTo;
    translateStringToPositions(move, &colFrom, &colTo, &lineFrom, &lineTo);
    bool promotionIsSet = false;
    applyMoveMinimax(board[lineFrom][colFrom], make_pair(lineTo, colTo), promotionIsSet);
}

bool checkCell(int line, int col) {
    return 1 <= line && line <= 8 && 1 <= col && col <= 8;
}

bool checkPiecePosition(int piece) {
    return positions.find(piece) != positions.end();
}

void insertRookMoves(int x, int y, int piece, vector<int> &movesX,
                     vector<int> &movesY, vector<int> &myPieceCovered) {
	// insert positions on the same col
    for (int i = x + 1; i <= 8; i++) {
    	// my piece blocking the way
        if (board[i][y] * piece > 0) {
            myPieceCovered.push_back(board[i][y]);
            break;
        }

        movesX.push_back(i - x);
        movesY.push_back(0);

        if (board[i][y] * piece < 0) {
            break;
        }
    }

    for (int i = 1; i < x; i++) {
        if (board[x - i][y] * piece > 0) {
            myPieceCovered.push_back(board[x - i][y]);
            break;
        }

        movesX.push_back(-1 * i);
        movesY.push_back(0);

        if (board[x - i][y] * piece < 0) {
            break;
        }
    }
    // insert positions on the same row
    for (int i = y + 1; i <= 8; i++) {
        if (board[x][i] * piece > 0) {
            myPieceCovered.push_back(board[x][i]);
            break;
        }

        movesY.push_back(i - y);
        movesX.push_back(0);

        if (board[x][i] * piece < 0) {
            break;
        }
    }

    for (int i = 1; i < y; i++) {
        if (board[x][y - i] * piece > 0) {
            myPieceCovered.push_back(board[x][y - i]);
            break;
        }

        movesY.push_back(-1 * i);
        movesX.push_back(0);

        if (board[x][y - i] * piece < 0) {
            break;
        }
    }
}


void insertBishopMoves(int x, int y, int piece, vector<int> &movesX,
                       vector<int> &movesY, vector<int> &myPieceCovered) {
    // bishop going North-East
    for (int i = 1; i <= 8 && x + i <= 8 && y + i <= 8; i++) {
        if (board[x + i][y + i] * piece > 0) {
            myPieceCovered.push_back(board[x + i][y + i]);
            break;
        }

        movesX.push_back(i); 
        movesY.push_back(i); 

        if (board[x + i][y + i] * piece < 0) {
            break;
        }
    }

    // bishop going North-West
    for (int i = 1; i <= 8 && x + i <= 8 && y - i >= 1; i++) {
        if (board[x + i][y - i] * piece > 0) {
            myPieceCovered.push_back(board[x + i][y - i]);
            break;
        }

        movesX.push_back(i);
        movesY.push_back(-1 * i);

        if (board[x + i][y - i] * piece < 0) {
            break;
        }
    }

    // bishop going South-West
    for (int i = 1; i <= 8 && x - i >= 1 && y - i >= 1; i++) {
        if (board[x - i][y - i] * piece > 0) {
            myPieceCovered.push_back(board[x - i][y - i]);
            break;
        }

        movesX.push_back(-1 * i);
        movesY.push_back(-1 * i);

        if (board[x - i][y - i] * piece < 0) {
            break;
        }
    }

    // bishop going South-East
    for (int i = 1; i <= 8 && x - i >= 1 && y + i <= 8; i++) {
        if (board[x - i][y + i] * piece > 0) {
            myPieceCovered.push_back(board[x - i][y + i]);
            break;
        }

        movesX.push_back(-1 * i); // +
        movesY.push_back(i); // +

        if (board[x - i][y + i] * piece < 0) {
            break;
        }
    }
}

void generateMoveVectors(int piece, vector<int> &movesX, vector<int> &movesY,
                         vector<int> &myPieceCovered) {
    if (checkPiecePosition(piece) == false) {
        return;
    }

    int x = positions[piece] / 10;
    int y = positions[piece] % 10;

    // rook
    if (piece == ROOK1_B || piece == ROOK1_W ||
        piece == ROOK2_B || piece == ROOK2_W) {
        insertRookMoves(x, y, piece, movesX, movesY, myPieceCovered);
        return;
    }

    // bishop
    if (piece == BISHOP1_B || piece == BISHOP1_W ||
        piece == BISHOP2_B || piece == BISHOP2_W) {
        insertBishopMoves(x, y, piece, movesX, movesY, myPieceCovered);
        return;
    }

    // queens or promoted pawns
    if (piece == QUEEN_W || piece == QUEEN_B || abs(piece) / 10 == 6) {
        insertRookMoves(x, y, piece, movesX, movesY, myPieceCovered);
        insertBishopMoves(x, y, piece, movesX, movesY, myPieceCovered);
    }
}

void getPieceMoves(int piece, int movesNr,
                   vector<int> &movesX, vector<int> &movesY,
                   vector<int> &newMovesX, vector<int> &newMovesY,
                   vector<int> &myPieceCovered) {
    if (checkPiecePosition(piece) == false) {
        return;
    }

    int type = abs(piece) / 10;

    for (int i = 0; i < movesNr; i++) {
        int toRow = positions[piece] / 10 + movesX[i];
        int toCol = positions[piece] % 10 + movesY[i];
        if (checkCell(toRow, toCol) == true && // position on board
            (board[toRow][toCol] * piece <= 0)) { // opponent piece or empty
            newMovesX.push_back(toRow);
            newMovesY.push_back(toCol);
        } else if (checkCell(toRow, toCol) == true && // position on board
                   (board[toRow][toCol] * piece > 0)) { // piece of same col
            myPieceCovered.push_back(board[toRow][toCol]);
        }
    }
}

void getPieceMovesPawn(int piece, int movesNr,
                       vector<int> &newMovesX, vector<int> &newMovesY,
                       vector<int> &myPieceCovered,
                       vector<int> &attackButNoMoveX, vector<int> &attackButNoMoveY) {
    // position not on board
    if (checkPiecePosition(piece) == false) {
        return;
    }

    int sign = piece < 0 ? -1 : 1;
    int firstLine = piece < 0 ? 7 : 2;
    int position = positions[piece];
    int row = position / 10;
    int col = position % 10;

    // can move 2 lines ahead
    if (row == firstLine && board[row + sign][col] == EMPTY &&
        board[row + sign * 2][col] == EMPTY) {
        newMovesX.push_back(row + sign * 2);
        newMovesY.push_back(col);
    }

    // can move only 1 line ahead
    if (checkCell(row + sign, col) == true && board[row + sign][col] == EMPTY) {
        newMovesX.push_back(row + sign);
        newMovesY.push_back(col);
    }

    // can capture piece by moving diagonally
    vector<int> diag{1, -1};
    for (auto i : diag) {
        if (checkCell(row + sign, col + i) == false) {
            continue;
        }
        if (board[row + sign][col + i] * piece < 0) {
        	// opponent piece -> can do move
            newMovesX.push_back(row + sign);
            newMovesY.push_back(col + i);
        } else if (board[row + sign][col + i] * piece > 0) {
        	// my piece -> can't do move, but I am protecting the position
            myPieceCovered.push_back(board[row + sign][col + i]);
        } else {
        	// can't moove, no protection for another of my pieces
            // but still attacking the position
            attackButNoMoveX.push_back(row + sign);
            attackButNoMoveY.push_back(col + i);
        }
    }

    if (pawn2moves != EMPTY) {
        // can do en passant
        if ((positions[pawn2moves] / 10 == row) && // same row
            abs(col - positions[pawn2moves] % 10) == 1) {// col next to mine
            newMovesX.push_back(row + sign);
            newMovesY.push_back(positions[pawn2moves] % 10);
        }
    }
}

// returns captured piece
int applyMoveMinimax(int piece, pair<int, int> move, bool &promotionIsSet) {
    int lineFrom = positions[piece] / 10;
    int colFrom = positions[piece] % 10;
    int lineTo = move.first;
    int colTo = move.second;
    int captured = EMPTY;
    int sign = piece < 0 ? -1 : 1;
    int color = sign < 0 ? BLACK : WHITE;
    enPassant = false;

    // if last move was a pawn advancing 2 steps forward
    if (pawn2moves != EMPTY) {
        // and is on the same row as mine, col next to me
        if (abs(piece / 10) == 5 && abs(colFrom - positions[pawn2moves] % 10) == 1
            && colTo == positions[pawn2moves] % 10 &&
            (lineFrom == positions[pawn2moves] / 10)) {
            captured = pawn2moves;
            positions.erase(pawn2moves);
            board[lineFrom][colTo] = EMPTY;
            pawn2moves = EMPTY;
            enPassant = true;
        }
    }

    // if current piece is pawn and advances 2 steps forward
    if (abs(piece) / 10 == 5 && abs(lineFrom - lineTo) == 2) {
        pawn2moves = piece;
    } else {
        pawn2moves = EMPTY;
    }

    // i can capture a piece by performing the move
    if (board[lineTo][colTo] != EMPTY) {
        captured = board[lineTo][colTo];
        positions.erase(board[lineTo][colTo]);
    }

    board[lineFrom][colFrom] = EMPTY;

    // pawn promotion
    if (abs(piece) / 10 == 5 && (lineTo == 8 || lineTo == 1)) {
        
        positions.erase(piece);
        promotionIsSet = true;
        int oldPiece = piece;

        // <new piece codification> = 60 + promoted<color>
        if (sign == 1) {
            promotedWhite++;
            piece = PROMOTED_QUEEN_W + promotedWhite;
        } else {
            promotedBlack++;
            piece = PROMOTED_QUEEN_B - promotedBlack;
        }
        promotedPawn = make_pair(oldPiece, piece);
        board[lineTo][colTo] = piece;
        translatePosition(lineTo, colTo, piece);
        
    } else {
        promotionIsSet = false;
    }

    // castling if king moves across 2 columns
    if ((piece == KING_B || piece == KING_W) && abs(colFrom - colTo) == 2) {
        // castling on right side -> colFrom - colTo < 0
        // castling on left side -> colFrom - colTo > 0

        int castlingType = (colFrom - colTo) / 2;

        
        int rook = sign * 42;

        // right castling
        if (castlingType > 0) {
            rook -= sign;
        }

        if (piece == KING_W) {
            if (rook == ROOK2_W) {
                whiteRookRightMoved++;
            } else {
                whiteRookLeftMoved++;
            }
            whiteKingMoved++;
        } else {
            if (rook == ROOK2_B) {
                blackRookRightMoved++;
            } else {
                blackRookLeftMoved++;
            }
            blackKingMoved++;
        }

        int rookCol = positions[rook] % 10;
        int rookRow = lineTo;

        board[rookRow][rookCol] = EMPTY;
        //rocada mare -> colrege + 1
        //rocada mica -> colrege - 1
        board[rookRow][colTo + castlingType] = rook;
        translatePosition(rookRow, colTo + castlingType, rook);
    } else if (piece == KING_B) {
        blackKingMoved++;
    } else if (piece == KING_W) {
        whiteKingMoved++;
    } else if (piece == ROOK2_B) {
        blackRookRightMoved++;
    } else if (piece == ROOK1_B) {
        blackRookLeftMoved++;
    } else if (piece == ROOK1_W) {
        whiteRookLeftMoved++;
    } else if (piece == ROOK2_W) {
        whiteRookRightMoved++;
    }

    board[lineTo][colTo] = piece;
    translatePosition(lineTo, colTo, piece);

    unordered_map<int, vector<pair<int, int> > > moves; //dummy
    markAttacked(moves, color);

    moves.clear();
    int opponentColor = color == WHITE ? BLACK : WHITE;
    markAttacked(moves, opponentColor);

    return captured;
}


void undoMoveMinimax(int piece, int captured, int initialRow, int initialCol,
        bool &promotionIsSet) {
    // my current positon
    int lineFrom = positions[piece] / 10;
    int colFrom = positions[piece] % 10;

    if (promotionIsSet == true) {
        int color = promotedPawn.second < 0 ? BLACK : WHITE;
        if (color == BLACK) {
            promotedBlack--;
        } else {
            promotedWhite--;
        }
        lineFrom = positions[promotedPawn.second] / 10;
        colFrom = positions[promotedPawn.second] % 10;
        positions.erase(promotedPawn.second); 
        piece = promotedPawn.first;
        promotionIsSet = false;
    }

    if (captured != EMPTY) {
        if (enPassant == true) {
            board[initialRow][colFrom] = captured;
            translatePosition(initialRow, colFrom, captured);
            enPassant = false;
            pawn2moves = captured;
            captured = EMPTY;
        } else {
            translatePosition(lineFrom, colFrom, captured);
        }
    }

    board[lineFrom][colFrom] = captured;

    int sign = piece < 0 ? -1 : 1;
    // castling
    if ((piece == KING_B || piece == KING_W) && abs(colFrom - initialCol) == 2) {
        

        int castlingType = (initialCol - colFrom) / 2;

        
        int rook = sign * 42;

       
        if (castlingType > 0) {
            rook -= sign;
        }

        int rookCol = positions[rook] % 10;
        int rookRow = initialRow;

        if (piece == KING_W) {
            if (rook == ROOK2_W) {
                whiteRookRightMoved--;
                board[1][8] = rook;
                translatePosition(1, 8, rook);
            } else {
                whiteRookLeftMoved--;
                board[1][1] = rook;
                translatePosition(1, 1, rook);
            }
            whiteKingMoved--;
        } else {
            if (rook == ROOK2_B) {
                blackRookRightMoved--;
                board[8][8] = rook;
                translatePosition(8, 8, rook);
            } else {
                blackRookLeftMoved--;
                board[8][1] = rook;
                translatePosition(8, 1, rook);
            }
            blackKingMoved--;
        }

        board[rookRow][rookCol] = EMPTY;
    } else if (piece == KING_B) {
        blackKingMoved--;
    } else if (piece == KING_W) {
        whiteKingMoved--;
    } else if (piece == ROOK2_B) {
        blackRookRightMoved--;
    } else if (piece == ROOK1_B) {
        blackRookLeftMoved--;
    } else if (piece == ROOK1_W) {
        whiteRookLeftMoved--;
    } else if (piece == ROOK2_W) {
        whiteRookRightMoved--;
    }

    board[initialRow][initialCol] = piece;
    translatePosition(initialRow, initialCol, piece);
}

bool checkLeftRightCastling(int color, int rowKing, int rookMoved, int endCol,
                            int offset, int rook) {
    int attackerColor = color == WHITE ? BLACK : WHITE;
    if (rookMoved == 0) {
        // if king is attacked by a rook of the same colour
        // then there are no pieces between them
        bool isAttackedBetween = false;

        if (find(attacked[color - 1][rowKing][5].begin(),
                 attacked[color - 1][rowKing][5].end(), rook) !=
            attacked[color - 1][rowKing][5].end()) {
        	// check if every position between the rook and the king
            // is not attacked by an opponent's piece
            for (int i = 5; i != endCol; i += offset) {
                if (attacked[attackerColor - 1][rowKing][i].size() > 0) {
                    isAttackedBetween = true;
                    return false;
                }
            }

            if (isAttackedBetween == false) {
                return true;
            }
        }
    }

    return false;
}

void canDoCastling(int color,
                   unordered_map<int, vector<pair<int, int> > > &moves) {
    if (isCheck(color) == true) {
        return;
    }

    int kingMoved = color == WHITE ?
                    whiteKingMoved : blackKingMoved;
    int rookLeftMoved = color == WHITE ?
                        whiteRookLeftMoved : blackRookLeftMoved;
    int rookRightMoved = color == WHITE ?
                         whiteRookRightMoved : blackRookRightMoved;
    int rowKing = color == WHITE ? 1 : 8;
    int rookRight = color == WHITE ? ROOK2_W : ROOK2_B;
    int rookLeft = color == WHITE ? ROOK1_W : ROOK1_B;
    int king = color == WHITE ? KING_W : KING_B;

    if (kingMoved > 0) {
        return;
    }

    // right castling
    if (checkLeftRightCastling(color, rowKing, rookRightMoved,
                               7, 1, rookRight) == true) {
    	if (moves.find(king) == moves.end()) {
          vector<pair<int, int>> dummy;
          moves[king] = dummy;
    	}
        moves[king].push_back(make_pair(rowKing, 7));
    }

    // left castling
    if (checkLeftRightCastling(color, rowKing, rookLeftMoved,
                               3, -1, rookLeft) == true) {
    	if (moves.find(king) == moves.end()) {
          vector<pair<int, int>> dummy;
          moves[king] = dummy;
    	}
        moves[king].push_back(make_pair(rowKing, 3));
    }
}


void markAttacked(unordered_map<int, vector<pair<int, int> > > &moves,
                  int myColorToMove) {
    initializeAttacked();
   
    vector<int> kingNewMovesX, kingNewMovesY;
    int otherColor = myColorToMove == WHITE ? BLACK : WHITE;

    for (auto &entry : positions) {
        vector<int> newMovesX, newMovesY, myPieceCovered;
        vector<int> attackButNoMoveX, attackButNoMoveY;
        int piece = entry.first;
        int sign = piece < 0 ? -1 : 1;
        int color = piece < 0 ? BLACK : WHITE;
        int opponentColor = piece < 0 ? WHITE : BLACK;

        if (abs(piece) / 10 == 5) { // pawn
            getPieceMovesPawn(piece, PAWN_MOVES, newMovesX, newMovesY,
                              myPieceCovered, attackButNoMoveX, attackButNoMoveY);
        } else if (abs(piece) / 10 == 3) { // knight
            getPieceMoves(piece, KNIGHT_MOVES, knightMovesX, knightMovesY,
                          newMovesX, newMovesY, myPieceCovered);
        } else if (piece == KING_W || piece == KING_B) { // king
            getPieceMoves(piece, KING_MOVES, kingMovesX, kingMovesY,
                          newMovesX, newMovesY, myPieceCovered);
            if (color == myColorToMove) {
                kingNewMovesX = newMovesX;
                kingNewMovesY = newMovesY;
            }
        } else { // queen, rook, bishop
            vector<int> movesX, movesY;
            generateMoveVectors(piece, movesX, movesY, myPieceCovered);
            getPieceMoves(piece, movesX.size(), movesX, movesY, newMovesX,
                          newMovesY, myPieceCovered);
        }

        // king -> not updating moves map YET
        if (color == myColorToMove && abs(piece) != 10) {
           
            
            for (int i = 0; i < newMovesX.size(); i++) {
            	if (moves.find(piece) == moves.end()) {
            	vector<pair<int, int>> dummy;
                moves[piece] = dummy;
            	}
                moves[piece].push_back(make_pair(newMovesX[i], newMovesY[i]));
            }
        }

        for (int i = 0; i < newMovesX.size(); i++) {
            // pawn that advances on the same col is not attacking that position
            if (abs(piece) / 10 == 5 && newMovesY[i] == positions[piece] % 10) {
                continue;
            }
            attacked[color - 1][newMovesX[i]][newMovesY[i]].push_back(piece);
        }

        // pawn -> attacks positions on next row diagonally, but can't move
        //          if positions are empty
        for (int i = 0; i < attackButNoMoveX.size(); i++) {
            attacked[color - 1][attackButNoMoveX[i]][attackButNoMoveY[i]].push_back(piece);
        }

        for (auto &piece : myPieceCovered) {
            int position = positions[piece];
            attacked[color - 1][position / 10][position % 10].push_back(piece);
        }
    }

    // king can only move if it does not end up in check
    int myKing = myColorToMove == WHITE ? KING_W : KING_B;
    
    
    for (int i = 0; i < kingNewMovesX.size(); i++) {
    	
        if (attacked[otherColor - 1][kingNewMovesX[i]][kingNewMovesY[i]].size() == 0) {
        	if (moves.find(myKing) == moves.end()) {
    			vector<pair<int, int>> dummy;
                moves[myKing] = dummy;
    		}
            moves[myKing].push_back(make_pair(kingNewMovesX[i], kingNewMovesY[i]));
        }
    }
    // checks (and inserts move) for castling
    canDoCastling(colorToMove, moves);
}

bool kingRemainsAttacked(int row, int col, int opponentColor,
                         int rowTo, int colTo) {
    int attackerRow, attackerCol;

    int king = opponentColor == WHITE ? KING_B : KING_W;

    for (auto &attacker : attacked[opponentColor - 1][row][col]) {
        attackerRow = positions[attacker] / 10;
        attackerCol = positions[attacker] % 10;

        // queen or rook
        if (abs(attacker) == 11 || abs(attacker) / 10 == 4) {
            // king attacked horizontally
            if (attackerRow == rowTo && abs(attackerCol - colTo) > 1
                && positions[king] / 10 == attackerRow) {
                return true;
            }

            // king attacked vertically
            if (attackerCol == colTo && abs(attackerRow - rowTo) > 1
                && positions[king] % 10 == attackerCol) {
                return true;
            }
        }

        // queen or bishop
        if (abs(attacker) == 11 || abs(attacker) / 10 == 2) {
            // king maintains the diagonal on which it is attacked
            if (abs(attackerRow - rowTo) == abs(attackerCol - colTo) &&
                abs(attackerRow - rowTo) > 1 &&
                (abs(attackerRow - positions[king] / 10)
                 == abs(attackerCol - positions[king] % 10))) {
                return true;
            }
        }
    }

    return false;
}

void kingMobility(int king, int color,
                  unordered_map<int, vector<pair<int, int> > > &moves) {
    int row = positions[king] / 10;
    int col = positions[king] % 10;
    int toRow, toCol;
    int opponentColor = color == WHITE ? BLACK : WHITE;

    for (int i = 0; i < KING_MOVES; i++) {
        toRow = row + kingMovesX[i];
        toCol = col + kingMovesY[i];
        if (checkCell(toRow, toCol) == true && // is on board
            board[toRow][toCol] * king <= 0 && // empty/enemy piece
            attacked[opponentColor - 1][toRow][toCol].size() == 0 &&
            kingRemainsAttacked(row, col, opponentColor, toRow, toCol) == false) {
            
            
            if (moves.find(king) == moves.end()) {
            	vector<pair<int, int>> dummy;
                moves[king] = dummy;
            }

            moves[king].push_back(make_pair(toRow, toCol));
        }
    }


}

void insertInMovesMap(unordered_map<int, vector<pair<int, int> > > &moves,
                      int row, int col, int color) {
    vector<pair<int, int> > dummy;

    for (auto conterAttack : attacked[color - 1][row][col]) {
    	 // already inserted for king
        if (abs(conterAttack) == 10) {
            continue;
        }
        if (moves.find(conterAttack) == moves.end()) {
            moves[conterAttack] = dummy;
        }

        moves[conterAttack].push_back(make_pair(row, col));
    }
}

void insertAllButPawnAndKing(unordered_map<int, vector<pair<int, int> > > &moves,
                             int row, int col, int color) {
    vector<pair<int, int> > dummy;

    for (auto conterAttack : attacked[color - 1][row][col]) {
        if (abs(conterAttack) / 10 == 5 || abs(conterAttack) == 10) {
            continue;
        }
        if (moves.find(conterAttack) == moves.end()) {
            moves[conterAttack] = dummy;
        }

        moves[conterAttack].push_back(make_pair(row, col));
    }
}

bool isMat(int color, unordered_map<int, vector<pair<int, int> > > &moves) {
    int king = color == WHITE ? KING_W : KING_B;
    int position = positions[king];
    vector<pair<int, int> > dummy;
    // king can move
    kingMobility(king, color, moves);
    int opponentColor = color == WHITE ? BLACK : WHITE;
    int sign = color == WHITE ? 1 : -1;

    // king can't move and is attacked from more than two sides -> check mat
    if (moves.size() == 0 &&
        attacked[opponentColor - 1][position / 10][position % 10].size() >= 2) {
        return true;
    }

    // king can move and is attacked from more than two sides -> only check
    if (moves.size() != 0 && attacked[opponentColor - 1][position / 10][position % 10].size() >= 2 ){
    	return false; 
    }


    // check from one side, I can capture the attacker
    int attacker = attacked[opponentColor - 1][position / 10][position % 10][0];
    int attackerPosition = positions[attacker];

    if (attacked[color - 1]
        [attackerPosition / 10][attackerPosition % 10].size() > 0) {
        insertInMovesMap(moves, attackerPosition / 10,
                         attackerPosition % 10, color);
    }

    // I can block the attacker (queen, rock, bishop)
    int minX = min(position / 10, attackerPosition / 10);
    int maxX = max(position / 10, attackerPosition / 10);
    int minY = min(position % 10, attackerPosition % 10);
    int maxY = max(position % 10, attackerPosition % 10);

    if (abs(attacker) / 10 == 4 || abs(attacker) / 10 == 1) {
        // rook or queen
        // horizontal attack
        for (int i = minX + 1; i < maxX; i++) {
            if (attacked[color - 1][i][minY].size() > 0) {
                insertAllButPawnAndKing(moves, i, minY, color);
            }
        }

        // vertical attack
        for (int i = minY + 1; i < maxY; i++) {
            if (attacked[color - 1][minX][i].size() > 0) {
                insertAllButPawnAndKing(moves, minX, i, color);
            }

            // pawn can block the attacker by moving one step forward
            int possiblePawn = board[minX - sign][i];
            if (checkCell(minX - sign, i) == true && (abs(possiblePawn) / 10 == 5) &&
                (possiblePawn * sign > 0)) {
                if (moves.find(possiblePawn) == moves.end()) {
                    vector<pair<int, int> > dummy;
                    moves[possiblePawn] = dummy;
                }

                moves[possiblePawn].push_back(make_pair(minX, i));
            }

            // pawn can block the attacker by moving 2 steps forward
            int possiblePawn2 = board[minX - 2 * sign][i];
            if (checkCell(minX - 2 * sign, i) == true && (abs(possiblePawn2) / 10 == 5) &&
                possiblePawn2 * sign > 0 && board[minX - sign][i] == EMPTY) {
                if ((color == WHITE && minX - 2 * sign == 2) || (color == BLACK && minX - 2 * sign == 7)) {
                    if (moves.find(possiblePawn2) == moves.end()) {
                        vector<pair<int, int> > dummy;
                        moves[possiblePawn2] = dummy;
                    }

                    moves[possiblePawn2].push_back(make_pair(minX, i));
                }
            }
        }
    }

    // bisop or queen
    if ((abs(attacker) / 10 == 2 || abs(attacker) / 10 == 1) &&
        (abs(position / 10 - attackerPosition / 10) > 1) &&
        (abs(position % 10 - attackerPosition % 10) > 1)) {
        // nebun sau regina
        int directionX = position / 10 > attackerPosition / 10 ? -1 : 1;
        int directionY = position % 10 > attackerPosition % 10 ? -1 : 1;
        for (int i = position / 10 + directionX,
                     j = position % 10 + directionY;
             i != attackerPosition / 10 && j != attackerPosition % 10;
             i += directionX, j += directionY) {
            if (attacked[color - 1][i][j].size() > 0) {
                insertAllButPawnAndKing(moves, i, j, color);
            }

            int possiblePawn = board[i - sign][j];
            if (checkCell(i - sign, j) == true && (abs(possiblePawn) / 10 == 5) &&
                possiblePawn * sign > 0) {
                if (moves.find(possiblePawn) == moves.end()) {
                    vector<pair<int, int> > dummy;
                    moves[possiblePawn] = dummy;
                }

                moves[possiblePawn].push_back(make_pair(i, j));
            }

            int possiblePawn2 = board[i - 2 * sign][j];
            if (checkCell(i - 2 * sign, j) == true && (abs(possiblePawn2) / 10 == 5) &&
                possiblePawn2 * 2 * sign > 0 && board[i - sign][j] == EMPTY) {
                if ((color == WHITE && i - 2 * sign == 2) || (color == BLACK && i - 2 * sign == 7)) {
                    if (moves.find(possiblePawn2) == moves.end()) {
                        vector<pair<int, int> > dummy;
                        moves[possiblePawn2] = dummy;
                    }

                    moves[possiblePawn2].push_back(make_pair(i, j));
                }
            }

        }
    }

    return moves.size() > 0 ? false : true;
}

// true if color is in check
bool isCheck(int color) {
    int king = color == WHITE ? KING_W : KING_B;
    int position = positions[king];
    int opponentColor = color == WHITE ? BLACK : WHITE;
    return (attacked[opponentColor - 1][position / 10][position % 10].size() > 0);
}

bool movePawn(int pawn) {
    int sign = pawn < 0 ? -1 : 1;

    if (checkPiecePosition(pawn) == false) {
        return false;
    }

    if (checkCell(positions[pawn] / 10 + sign, positions[pawn] % 10 + 1) == true
        &&
        board[positions[pawn] / 10 + sign][positions[pawn] % 10 + 1] != EMPTY) {
        cout << "move " << makeMove(positions[pawn] % 10,
                                    positions[pawn] % 10 + 1, positions[pawn] / 10,
                                    positions[pawn] / 10 + sign) << endl;
        return true;
    }
    if (checkCell(positions[pawn] / 10 + sign, positions[pawn] % 10 - 1) == true
        &&
        board[positions[pawn] / 10 + sign][positions[pawn] % 10 - 1] != EMPTY) {
        cout << "move " << makeMove(positions[pawn] % 10,
                                    positions[pawn] % 10 - 1, positions[pawn] / 10,
                                    positions[pawn] / 10 + sign) << endl;
        return true;
    }
    if (checkCell(positions[pawn] / 10 + sign, positions[pawn] % 10) == true &&
        board[positions[pawn] / 10 + sign][positions[pawn] % 10] == EMPTY) {
        cout << "move " << makeMove(positions[pawn] % 10, positions[pawn] % 10,
                                    positions[pawn] / 10,
                                    positions[pawn] / 10 + sign) << endl;
        return true;
    }

    return false;
}

void printAttacked(int color) {
    for(int i = 8; i >= 1; i--) {
        for(int j = 1; j <= 8; j++) {
            printf("%2lu ", attacked[color - 1][i][j].size());
        }
        cout<<endl;
    }
}

void applyStrategy() {
    pair<int, pair<int, pair<int, int>>> minimaxResult;
    
    minimaxResult = minimax_alphaBeta(engineColor, -999999, 999999, 3);

    if (minimaxResult.second.first == EMPTY) {
        return;
    }

    
    int piece = minimaxResult.second.first;
    int rowTo = minimaxResult.second.second.first;
    int colTo = minimaxResult.second.second.second;
    int rowFrom = positions[piece] / 10;
    int colFrom = positions[piece] % 10;

    bool promotionIsSet = false;

    applyMoveMinimax(piece, make_pair(rowTo, colTo), promotionIsSet);

    if (promotionIsSet == true) {
        cout << "move " << makeMove(colFrom, colTo, rowFrom, rowTo) << "q" << endl;
        return;
    }

    if ((piece == KING_B || piece == KING_W) && abs(colTo - colFrom) > 1) {
        int direction = colTo - colFrom; 
        if (engineColor == WHITE) {
            if (direction > 0) {
                cout << "move e1g1" << endl;
            } else {
                cout << "move e1c1" << endl;
            }
            return;
        } else {
            if (direction > 0) {
                cout << "move e8g8" << endl;
            } else {
                cout << "move e8c8" << endl;
            }
            return;
        }

    }

    cout<<minimaxResult.first<<endl;
    cout << "move " << makeMove(colFrom, colTo, rowFrom, rowTo) << endl;
}