
#include <iostream>
#include <cstdint>

// Bitboard 64 Bit dec
typedef unsigned long long  U64;
#define C64(constantU64) constantU64##ULL

// Squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

constexpr const char* const coords[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

constexpr int white = 0;
constexpr int black = 1;

// Bit macros
#define SET_BIT(bitboard, square) (bitboard |= (1ULL << square))
#define GET_BIT(bitboard, square) (bitboard & (1ULL << square))
#define POP_BIT(bitboard, square) (GET_BIT(bitboard, square) ? bitboard ^= (1ULL << square) : 0)
#define COUNT_BITS(bitboard) __builtin_popcountll(bitboard)
#define GET_LEAST_SIG_BIT_IND(bitboard) ((bitboard) ? __builtin_ctzll(bitboard) : -1)

// static inline int bitCounter(U64 bitboard){
//     int count = 0;
//     while (bitboard > 0){  
//         count++;
//         bitboard &= bitboard - 1;
//     } 
//     return count;
// }

void printBoard(U64 bitboard){
    for (int rank = 0; rank < 8; rank++ ){
        std::cout << "\n";

        for (int file = 0; file < 8; file++){
            
            int square = rank * 8 + file;
            
            if (file == 0) std::cout << 8 - rank << " ";

            std::cout << " "<<(GET_BIT(bitboard, square) ? 1 : 0);

        }
        std::cout << "\n";
    }

    std::cout << "\n   a b c d e f g h";
    std::cout << "\n Bitboard: " << bitboard;

}

// not file constants numbers generated -> represent file as 0
constexpr U64 NOT_A_FILE = 18374403900871474942ULL;
constexpr U64 NOT_H_FILE = 9187201950435737471ULL;
constexpr U64 NOT_HG_FILE = 4557430888798830399ULL;
constexpr U64 NOT_AB_FILE = 18229723555195321596ULL;

U64 pawnAttack[2][64];
U64 knightAttack[64];
U64 kingAttack[64];

U64 maskPawnAttacks(int side, int square){
    
    U64 bitboard = 0ULL;
    U64 attacks = 0ULL;
    
    SET_BIT(bitboard, square);

    // 7 & 9 -> diagonal attacks
    if (side == white){

        attacks |= ((bitboard >> 7) & NOT_A_FILE);
        attacks |= ((bitboard >> 9) & NOT_H_FILE);

    } else {
        // black pawn
        attacks |= ((bitboard << 7) & NOT_H_FILE);
        attacks |= ((bitboard << 9) & NOT_A_FILE);
    }

    return attacks;
}

U64 maskKnightAttacks(int square){

    U64 bitboard = 0ULL;
    U64 attacks = 0ULL;
    
    SET_BIT(bitboard, square);
    
    // 17 & 15 -> Vert. L attacks
    attacks |= ((bitboard >> 17) & NOT_H_FILE);
    attacks |= ((bitboard >> 15) & NOT_A_FILE);

    attacks |= ((bitboard << 15) & NOT_H_FILE);
    attacks |= ((bitboard << 17) & NOT_A_FILE);


    // 10 & 6 -> Hori. L attacks
    attacks |= ((bitboard >> 10) & NOT_HG_FILE);
    attacks |= ((bitboard >> 6) & NOT_AB_FILE);

    attacks |= ((bitboard << 10) & NOT_AB_FILE);
    attacks |= ((bitboard << 6) & NOT_HG_FILE);


    return attacks;
}

U64 maskKingAttacks(int square){

    U64 bitboard = 0ULL;
    U64 attacks = 0ULL;
    
    SET_BIT(bitboard, square);

    // diagonal
    attacks |= ((bitboard >> 7) & NOT_A_FILE);
    attacks |= ((bitboard >> 9) & NOT_H_FILE);
    attacks |= ((bitboard << 7) & NOT_H_FILE);
    attacks |= ((bitboard << 9) & NOT_A_FILE);

    // up & down
    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if (bitboard << 8) attacks |= (bitboard << 8);

    // left & right
    attacks |= ((bitboard >> 1) & NOT_H_FILE);
    attacks |= ((bitboard << 1) & NOT_A_FILE);


    return attacks;
}

U64 maskBishopAttacks(int square){
    U64 attacks = 0ULL;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    rank = targetRank + 1, file = targetFile + 1;
    while (rank < 7 && file < 7){
        attacks |= (1ULL << (rank * 8 + file));
        rank++, file++;
    }

    rank = targetRank - 1, file = targetFile + 1;
    while (rank > 0 && file < 7){
        attacks |= (1ULL << (rank * 8 + file));
        rank--, file++;
    }
    
    rank = targetRank + 1, file = targetFile - 1; 
    while (rank < 7 && file > 0){
        attacks |= (1ULL << (rank * 8 + file));
        rank++, file--;
    }

    rank = targetRank - 1, file = targetFile - 1; 
    while (rank > 0 && file > 0){
        attacks |= (1ULL << (rank * 8 + file));
        rank--, file--;
    }

    return attacks;
}

U64 maskRookAttacks(int square){
    U64 attacks = 0ULL;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    rank = targetRank + 1;
    while (rank < 7){
        attacks |= (1ULL << (rank * 8 + targetFile));
        rank++;
    }
    rank = targetRank - 1;
    while (rank > 0){
        attacks |= (1ULL << (rank * 8 + targetFile));
        rank--;
    }

    file = targetFile + 1;
    while (file < 7){
        attacks |= (1ULL << (targetRank * 8 + file));
        file++;
    }
    file = targetFile - 1;
    while (file > 0){
        attacks |= (1ULL << (targetRank * 8 + file));
        file--;
    }

    return attacks;
}

U64 realBishopAttacks(int square, U64 blockers){
    U64 attacks = 0ULL;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    rank = targetRank + 1, file = targetFile + 1;
    while (rank <= 7 && file <= 7){
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file) & blockers)) break;
        rank++, file++;
    }

    rank = targetRank - 1, file = targetFile + 1;
    while (rank >= 0 && file <= 7){
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file) & blockers)) break;
        rank--, file++;
    }
    
    rank = targetRank + 1, file = targetFile - 1; 
    while (rank <= 7 && file >= 0){
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file) & blockers)) break;
        rank++, file--;
    }

    rank = targetRank - 1, file = targetFile - 1; 
    while (rank >= 0 && file >= 0){
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file) & blockers)) break;
        rank--, file--;
    }

    return attacks;
}

U64 realRookAttacks(int square, U64 blockers){
    U64 attacks = 0ULL;

    int rank, file;
    int targetRank = square / 8;
    int targetFile = square % 8;

    rank = targetRank + 1;
    while (rank <= 7){
        attacks |= (1ULL << (rank * 8 + targetFile));
        if ((1ULL << (rank * 8 + targetFile)) & blockers) break;
        rank++;
    }
    rank = targetRank - 1;
    while (rank >= 0){
        attacks |= (1ULL << (rank * 8 + targetFile));
        if ((1ULL << (rank * 8 + targetFile)) & blockers) break;
        rank--;
    }

    file = targetFile + 1;
    while (file <= 7){
        attacks |= (1ULL << (targetRank * 8 + file));
        if ((1ULL << (targetRank * 8 + file)) & blockers) break;
        file++;
    }
    file = targetFile - 1;
    while (file >= 0){
        attacks |= (1ULL << (targetRank * 8 + file));
        if ((1ULL << (targetRank * 8 + file)) & blockers) break;
        file--;
    }

    return attacks;
}



void initLeaperAttacks(){

    for (int square = 0; square < 64; square++){

        pawnAttack[white][square] = maskPawnAttacks(white, square);
        pawnAttack[black][square] = maskPawnAttacks(black, square);

        knightAttack[square] = maskKnightAttacks(square);

        kingAttack[square] = maskKingAttacks(square);
    }

}



int main(){

    // generate attack tables
    initLeaperAttacks();

    //printBoard(realRookAttacks(d4, 0ULL));
    
    U64 block = 0ULL;
    SET_BIT(block, d7);
    SET_BIT(block, a2);
    SET_BIT(block, b3);

    printBoard(block);

    std::cout << "\nindex: " << GET_LEAST_SIG_BIT_IND(block) << "\ncoordinates: " << coords[GET_LEAST_SIG_BIT_IND(block)];

   // U64 test = 0ULL;
   // SET_BIT(test, GET_LEAST_SIG_BIT_IND(block));

   // printBoard(test);

    return 0;
}