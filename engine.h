#ifndef LADYBUGS_ENGINE_H
#define LADYBUGS_ENGINE_H

#include "codifications.h"
// these variables store the number of moves made by the king and
// the rooks -> used for checking castling conditions
inline int whiteKingMoved = 0;
inline int blackKingMoved = 0;
inline int whiteRookLeftMoved = 0;
inline int whiteRookRightMoved = 0;
inline int blackRookLeftMoved = 0;
inline int blackRookRightMoved = 0;

// codification of promoted queens = 60 + promoted<Color>
// these variables store the number of promoted pawns
inline int promotedWhite = 0;
inline int promotedBlack = 0;

// stores the codification of the promoted pawn + new codification of queen
inline pair<int, int> promotedPawn;

// stores last pawn if the last move was made by a pawn
// and the pawn advanced 2 steps forward
inline int pawn2moves = EMPTY;

// true if the last move was enpassant
inline bool enPassant = false;

inline int board[9][9] = {0};

// positions[piece] = line * 10 + col;
inline unordered_map<int, int> positions;

inline bool force = false;

inline int colorToMove = WHITE, engineColor = NONE;

inline vector<int> knightMovesX {2, 1, -1, -2, -2, -1, 1, 2};
inline vector<int> knightMovesY {1, 2, 2, 1, -1, -2, -2, -1};
inline int KNIGHT_MOVES = 8;

inline vector<int> pawnMovesX {1, -1, 0};
inline int PAWN_MOVES = 3;

inline vector<int> kingMovesX {-1, 0, 1, -1, 1, -1, 0, 1};
inline vector<int> kingMovesY {-1, -1, -1, 0, 0, 1, 1, 1};
inline int KING_MOVES = 8;

// stores two vectors of vectors of vectors
// the first one is for white, the second one for black
// each inner vector (the 4th rank vector) stores the codifications
// for the pieces that are currently attacking a certain position on the board
// for instance, all the cells from attacked[0] that are attacked by a black
// knight will contain the knight's codification
inline vector< vector< vector< vector<int> > > > attacked
(2, vector< vector< vector< int> > >
(9, vector< vector < int> >
(9, vector <int> (0, 0))));

// calls insertRookMoves/insertBishopMoves or both if piece is queen or
// promoted pawn
void generateMoveVectors(int piece, vector<int> &movesX, vector<int> &movesY);

// for each piece, iterates the possible moves (movesX[i], movesY[i])
// if the position is on board and is empty / I can capture a piece by moving
// there, then the move is inserted in (newMovesX[j], newMovesY[j])
// if the position contains a piece of my color, the piece will
// be inserted in myPieceCovered
void getPieceMoves(int piece, int movesNr,
                   vector<int> &movesX, vector<int> &movesY,
                   vector<int> &newMovesX, vector<int> &newMovesY,
                   vector<int> &myPieceCovered);

// functions that generate moves vectors for bishop/rooks
// if one of my pieces is blocking the
// queen's/rook's/bishop's way, then the piece will be inserted in myPieceCovered
// myPieceCovered = pieces "covered" by piece X (the one I am generating the
//                   moves for)
void insertRookMoves(int x, vector<int> &movesX, vector<int> &movesY,
                     vector<int> &myPieceCovered);
void insertBishopMoves(int x, vector<int> &movesX, vector<int>&movesY,
                       vector<int> &myPieceCovered);

// translates position from board[line][col] to positions map value
void translatePosition(int line, int col, int piece);

// sets pieces back to starting positions
void initializeBoard();

// clears previously attacked positions
void initializeAttacked();

// helper methods
void printMap();
void printBoard();
void printAttacked(int color);

// reads from stdin and checks if size is < 77
void readFromInput(string &input);

// makes move on board
// returns string to print for move command
char* makeMove(int colFrom, int colTo, int lineFrom, int lineTo);

// translates string to board positions and then calls applyMoveMinimax
void markMoveOnBoard(char *move);

// receives an input char* (move made by the opponent and received from stdin)
// and translates the input to decimal values
void translateStringToPositions(char *move, int *colFrom, int *colTo,
                                int *lineFrom, int *lineTo);

// checks if cell is on board
bool checkCell(int line, int col);

// checks if piece is still on board (hasn't been taken yet)
bool checkPiecePosition(int piece);

// applies minimax algorithm and then outputs move
void applyStrategy();

// generates valid new moves for pawn
// checks for 1 or 2 rows ahead, en passant, advancing diagonally
void getPieceMovesPawn(int piece, int movesNr,
                       vector<int> &newMovesX, vector<int> &newMovesY,
                       vector<int> &myPieceCovered,
                       vector<int> &attackButNoMoveX,
                       vector<int> &attackButNoMoveY);

// generates move for every piece on the table and then inserts piece in
//      attacked table for positions that a piece can move on
// inserts in moves map if piece's color is the same as colorToMove
void markAttacked(unordered_map<int, vector< pair<int, int> > > &moves,
                  int myColoToMove);

// checks if color's king position is attacked by an enemy piece
bool isCheck(int color);

// checks if king is still attacked after performing a move
// when the attacker is queen, rook or bishop
bool kingRemainsAttacked(int row, int col, int opponentColor,
                         int rowTo, int colTo);

// inserts king's next moves in moves map
void kingMobility(int king, int color,
                  unordered_map<int, vector<pair<int, int> > > &moves);

// inserts in moves all the pieces that attack a certain position
// (and can perform a move on that position)
// useful when king is in check -> such a move could save the king
void insertInMovesMap(unordered_map<int, vector<pair<int, int> > > &moves,
                      int row, int col, int color);

// does the same thing as insertInMovesMap, but checks if piece is not
// pawn, because a pawn attacking a position can only perform that move
// if the position is not empty --> a pawn can't save the king by capturing
// the enemy piece if the attacker is a queen, rook or bishop
void insertAllButPawnAndKing(unordered_map<int, vector<pair<int, int> > > &moves,
                             int row, int col, int color);

// puts piece back on initialRow and initialCol
// captured (can be EMPTY) back on piece's current position
// checks for castling, en passant, pawn promotion
void undoMoveMinimax(int piece, int captured, int initialRow, int initialCol,
                     bool &promotionIsSet);

// true if checkMate for color
bool isMat(int color, unordered_map<int, vector< pair<int, int> > > &moves);

// applies move, returns captured piece (or EMPTY)
// performs checks for en passant, castling and then calls markAttacked
int applyMoveMinimax(int piece, pair<int, int> move, bool &promotionIsSet);

// checks castling conditions -> no pieces between king col and end col
//                            -> positions between king col and end col not
//                               attacked by opponent
//                            -> rookMoved is set on 0
bool checkLeftRightCastling(int color, int rowKing, int rookMoved, int endCol,
                            int offset, int rook);

// inserts castling move(s) in moves map if king of color hasn't been moved
// before, and checkLeftRightCastling conditions for right/left are true
void canDoCastling(int color,
                   unordered_map<int, vector<pair<int, int> > > &moves);


void undoMoveMinimax(int piece, int captured, int initialRow, int initialCol,
                     bool &promotionIsSet);

void printAttacked(int color);
#endif //LADYBUGS_CHESS_ENGINE_ENGINE_H
