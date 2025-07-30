
#include <iostream>
#include <cstdint>
#include <string.h>

typedef unsigned long long  U64;
#define C64(constantU64) constantU64##ULL
// Bit macros
#define SET_BIT(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define GET_BIT(bitboard, square) ((bitboard) & (1ULL << (square)))
#define POP_BIT(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define COUNT_BITS(bitboard) __builtin_popcountll(bitboard)
#define GET_LEAST_SIG_BIT_IND(bitboard) ((bitboard) ? __builtin_ctzll(bitboard) : -1)


// Squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, 
    noSquare
};
const char* const coords[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

U64 bitboards[12];
U64 occupancies[3];

int side;
int enpassant = noSquare;
int castle = 0;

// castle bit representation
enum {wK = 1, wQ = 2, bK = 4, bQ = 8};
// upper case -> white, lower case -> black
enum {P, N, B, R, Q, K, p, n, r, b, q, k};

const char asciiPieces[13] = "PNBRQKpnbrqk";
//const char *unicodePieces[13] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};

const int pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k
};


const int white = 0;
const int black = 1;
const int mono = 2;

const int rook = 0;
const int bishop = 1;


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

void printPieces(){
    std::cout << "\n";
    for (int rank = 0; rank < 8; rank++){
       
        std::cout << "\n";
        
        for (int file = 0; file < 8; file++){
            
            if (file == 0) std::cout << 8 - rank << " ";
            
            int square = rank * 8 + file;
            int setP = -1;

            for (int piece = P; piece <= k; piece++){

                if (GET_BIT(bitboards[piece], square)) setP = piece;
            
            }
            std::cout << ' ' << ((setP == -1) ? '.' : asciiPieces[setP]);
        }

    }
    std::cout << "\n   a b c d e f g h";
    std::cout << "\nSide to move: " << ((side == 0) ? "White" : "Black");
    std::cout << "\nEnpass: " << ((enpassant != noSquare) ? coords[enpassant] : "no");
    //std::cout <, "\n Castle: " << ((castle & wk) ? 'K' : '-')

    std::cout << "\nCastling:"
          << ((castle & wK) ? 'K' : '-')
          << ((castle & wQ) ? 'Q' : '-')
          << ((castle & bK) ? 'k' : '-')
          << ((castle & bQ) ? 'q' : '-');    
}

// not file constants numbers generated -> represent file as 0
const U64 NOT_A_FILE = 18374403900871474942ULL;
const U64 NOT_H_FILE = 9187201950435737471ULL;
const U64 NOT_HG_FILE = 4557430888798830399ULL;
const U64 NOT_AB_FILE = 18229723555195321596ULL;

const int bishopRelBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6
};

const int rookRelBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12
};

U64 rookMagicNums[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};
U64 bishopMagicNums[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};

U64 bishopMasks[64];
U64 rookMasks[64];

U64 pawnAttack[2][64];
U64 knightAttack[64];
U64 kingAttack[64];
U64 bishopAttack[64][512];
U64 rookAttack[64][4096];

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

static inline U64 getBishopAttacks(int square, U64 occupany){
    occupany &= bishopMasks[square];
    occupany *= bishopMagicNums[square];
    occupany >>= 64 - bishopRelBits[square];

    return bishopAttack[square][occupany];
}

static inline U64 getRookAttacks(int square, U64 occupany){
    occupany &= rookMasks[square];
    occupany *= rookMagicNums[square];
    occupany >>= 64 - rookRelBits[square];

    return rookAttack[square][occupany];
}

U64 setOccupancy(int index, int maskBits, U64 attackMask){

    U64 occupancy = 0ULL;

    for (int count = 0; count < maskBits; count++){
        int square = GET_LEAST_SIG_BIT_IND(attackMask);
        POP_BIT(attackMask, square);
        if (index & (1 << count)) occupancy |= (1ULL << square);
    }

    return occupancy;
}

// magic number generator
unsigned int ranState = 1804289383;

unsigned int getRandomU32(){
    unsigned int number = ranState;

    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    ranState = number;
    
    return ranState;
}

U64 getRandomU64(){

    U64 n1, n2, n3, n4;

    n1 = (U64)(getRandomU32()) & 0xFFFF;
    n2 = (U64)(getRandomU32()) & 0xFFFF;
    n3 = (U64)(getRandomU32()) & 0xFFFF;
    n4 = (U64)(getRandomU32()) & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

U64 genMagicNum(){
    return getRandomU64() & getRandomU64() & getRandomU64();
}

U64 findMagicNum(int square, int relBits, int bishop){

    U64 occupancy[4096];
    U64 attacks[4096];
    U64 attacksUsed[4096];

    U64 maskAttacks = bishop ? maskBishopAttacks(square) : maskRookAttacks(square);

    int occupyInd = 1 << relBits;

    for (int i = 0; i < occupyInd; i++){
        occupancy[i] = setOccupancy(i, relBits, maskAttacks);
        
        if (bishop == true){
            attacks[i] = realBishopAttacks(square, occupancy[i]);
        } else {
            // rook
            attacks[i] = realRookAttacks(square, occupancy[i]);
        }
    }

    for (int i = 0; i < 100000000; i++){

        U64 magicNum = genMagicNum();

        if (COUNT_BITS((maskAttacks * magicNum) & (0xFFULL << 56)) < 6) continue;
        memset(attacksUsed, 0ULL, sizeof(attacksUsed));
        
        int index, fail;
        for (index = 0, fail = 0; !fail && index < occupyInd; index++){
            
            int magicInd = (int)((occupancy[index] * magicNum) >> (64 - relBits));
            
            if (attacksUsed[magicInd] == 0ULL){
                attacksUsed[magicInd] = attacks[index];
            } else {
                fail = 1;
            }
        }

        if (!fail) return magicNum;
    }
    std::cout << "Magic Number Failed" << std::endl;
    return 0ULL;
}

void initLeaperAttacks(){

    for (int square = 0; square < 64; square++){

        pawnAttack[white][square] = maskPawnAttacks(white, square);
        pawnAttack[black][square] = maskPawnAttacks(black, square);

        knightAttack[square] = maskKnightAttacks(square);

        kingAttack[square] = maskKingAttacks(square);
    }

}

void initSliderAttacks(int bishop){

    for (int square = 0; square < 64; square++){
        bishopMasks[square] = maskBishopAttacks(square);
        rookMasks[square] = maskRookAttacks(square);

        U64 attackMask = bishop ? bishopMasks[square] : rookMasks[square];

        int relBitsCount = COUNT_BITS(attackMask);
        int occupyInd = (1 << relBitsCount);

        for (int i = 0; i < occupyInd; i++){
            if (bishop){
                U64 occupancy = setOccupancy(i, relBitsCount, attackMask);
                int magicInd = (occupancy * bishopMagicNums[square]) >> (64 - bishopRelBits[square]);
                bishopAttack[square][magicInd] = realBishopAttacks(square, occupancy);
            } else {
                U64 occupancy = setOccupancy(i, relBitsCount, attackMask);
                int magicInd = (occupancy * rookMagicNums[square]) >> (64 - rookRelBits[square]);
                rookAttack[square][magicInd] = realRookAttacks(square, occupancy);
            }
        }
    }
}

void initMagicNum(){
    
    for (int square = 0; square < 64; square++){
        rookMagicNums[square] = findMagicNum(square, rookRelBits[square], rook);
    }
    for (int square = 0; square < 64; square++){
        bishopMagicNums[square] = findMagicNum(square, bishopRelBits[square], bishop);
    }

}

void initAll(){

    // generate attack tables
    initLeaperAttacks();
    initSliderAttacks(bishop);
    initSliderAttacks(rook);
    //initMagicNum();
}

int main(){

    initAll();

    //printBoard(bitboards[P]);
    printPieces();

    return 0;
}