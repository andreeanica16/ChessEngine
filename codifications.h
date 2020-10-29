#ifndef LADYBUGS_CODIFICATIONS_H
#define LADYBUGS_CODIFICATIONS_H

#include <fstream>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unordered_map>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <vector>
//#include <zconf.h>

using namespace std;

#define WHITE     1
#define BLACK     2
#define NONE      3
#define forever while(1)
#define EMPTY      0

#define KING_W    10
#define QUEEN_W   11
#define BISHOP1_W  21
#define BISHOP2_W  22
#define KNIGHT1_W  31
#define KNIGHT2_W  32
#define ROOK1_W    41
#define ROOK2_W    42
#define PAWN1_W    51
#define PAWN2_W    52
#define PAWN3_W    53
#define PAWN4_W    54
#define PAWN5_W    55
#define PAWN6_W    56
#define PAWN7_W    57
#define PAWN8_W    58
#define PROMOTED_QUEEN_W 60

#define KING_B   -10
#define QUEEN_B  -11
#define BISHOP1_B  -21
#define BISHOP2_B  -22
#define KNIGHT1_B  -31
#define KNIGHT2_B  -32
#define ROOK1_B    -41
#define ROOK2_B    -42
#define PAWN1_B    -51
#define PAWN2_B    -52
#define PAWN3_B    -53
#define PAWN4_B    -54
#define PAWN5_B    -55
#define PAWN6_B    -56
#define PAWN7_B    -57
#define PAWN8_B    -58
#define PROMOTED_QUEEN_B -60

#endif //LADYBUGS_CODIFICATIONS_H
