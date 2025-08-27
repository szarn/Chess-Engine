
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
enum {P, N, B, R, Q, K, p, n, b, r, q, k};

const char asciiPieces[13] = "PNBRQKpnbrqk";

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
    std::cout << "\n Bitboard: " << bitboard << "\n";

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

void fenParse(const char* fenStr)
{
    memset(bitboards,   0, sizeof(bitboards));
    memset(occupancies, 0, sizeof(occupancies));

    side = white;
    enpassant = noSquare;
    castle = 0;

    int fenItr = 0;

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;

            if (std::isalpha(fenStr[fenItr]) )
            {
                int pieceType = pieces[fenStr[fenItr]];
                SET_BIT(bitboards[pieceType], square);
                ++fenItr;
            }

            if (std::isdigit(fenStr[fenItr]) )
            {
                int offset = fenStr[fenItr] - '0';
                int pieceType = -1;

                for (int piece = P; piece <= k; piece++)
                    if (GET_BIT(bitboards[piece], square)) pieceType = piece;

                if (pieceType == -1) file--;

                file += offset;
                fenItr++;
            }

            if (fenStr[fenItr] == '/') fenItr++;
        }
    }
    
    fenItr++;
    side = (fenStr[fenItr] == 'w') ? white : black;
    fenItr += 2;
    
    while (fenStr[fenItr] != '\0' && fenStr[fenItr] != ' ') {
        switch (fenStr[fenItr]) {
            case 'K': castle |= wK; break;
            case 'Q': castle |= wQ; break;
            case 'k': castle |= bK; break;
            case 'q': castle |= bQ; break;
            default: break;
        }
            fenItr++;
        }
    fenItr++;

    if (fenStr[fenItr] != '-'){
        int file = fenStr[fenItr] - 'a';
        int rank = 8 - (fenStr[fenItr + 1] - '0');
        enpassant = rank * 8 + file;
    } else {
        enpassant = noSquare;
    }

    std::cout << "Fen: " << fenStr;


    for (int piece = P; piece <= K; piece++){
        occupancies[white] |= bitboards[piece];
    }

    for (int piece = p; piece <= k; piece++){
        occupancies[black] |= bitboards[piece];
    }

    occupancies[mono] |= occupancies[white];
    occupancies[mono] |= occupancies[black];
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


static inline U64 getBishopAttacks(int square, U64 occupancy){
    occupancy &= bishopMasks[square];
    occupancy *= bishopMagicNums[square];
    occupancy >>= 64 - bishopRelBits[square];

    return bishopAttack[square][occupancy];
}

static inline U64 getRookAttacks(int square, U64 occupancy){
    occupancy &= rookMasks[square];
    occupancy *= rookMagicNums[square];
    occupancy >>= 64 - rookRelBits[square];

    return rookAttack[square][occupancy];
}

static inline U64 getQueenAttacks(int square, U64 occupancy){
   return (getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy));
}


static inline bool squareAttackCheck(int square, int side){

    
    if ((side == white) && (pawnAttack[black][square] & bitboards[P])){
        return true;
    }
    if ((side == black) && (pawnAttack[white][square] & bitboards[p])){
        return true;
    }

    if (knightAttack[square] & ((side == white) ? bitboards[N] : bitboards[n])) return true;
    if (kingAttack[square] & ((side == white) ? bitboards[K] : bitboards[k])) return true;

    if (getBishopAttacks(square, occupancies[mono]) & ((side == white) ? bitboards[B] : bitboards[b])){
        return true;
    }
    if (getRookAttacks(square, occupancies[mono]) & ((side == white) ? bitboards[R] : bitboards[r])){
        return true;
    }
    if (getQueenAttacks(square, occupancies[mono]) & ((side == white) ? bitboards[Q] : bitboards[q])){
        return true;
    }

    return false;
}

void printAttacked(int side){
    std::cout << "\n";
    for (int rank = 0; rank < 8; rank++){
        std::cout << "\n";

        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            
            if (file == 0) std::cout << 8 - rank << " ";

            std::cout << (squareAttackCheck(square, side) ? 1 : 0) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n  a b c d e f g h";

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

void genMoves(){
    int startSquare, targetSquare;

    U64 bitboard, attacks;

    for (int piece = P; piece <= k; piece++){
        bitboard = bitboards[piece];
        
        if (side == white){
            if (piece == P){
                while (bitboard){
                    startSquare = GET_LEAST_SIG_BIT_IND(bitboard);
                    targetSquare = startSquare - 8;
                    
                    if (!(targetSquare < a8) && !GET_BIT(occupancies[mono], targetSquare)){

                        if (startSquare >= a7 && startSquare <= h7){   
                            std::cout << "\nPawn Promo: "<< coords[startSquare] << coords[targetSquare];

                        } else {
                            std::cout << "\nPawn Push: "<< coords[startSquare] << coords[targetSquare];
                            
                            if ((startSquare >= a2 && startSquare <= h2) && !GET_BIT(occupancies[mono], targetSquare - 8)){
                                std::cout << "\nDouble Pawn Push: "<< coords[startSquare] << coords[targetSquare - 8];
                            }
                        }
                    }

                    attacks = pawnAttack[side][startSquare] & occupancies[black];

                    while (attacks){
                        targetSquare = GET_LEAST_SIG_BIT_IND(attacks);

                        if (startSquare >= a7 && startSquare <= h7){   
                            std::cout << "\nPawn Capture Promo: "<< coords[startSquare] << coords[targetSquare];

                        } else {
                            std::cout << "\nPawn Capture Push: "<< coords[startSquare] << coords[targetSquare];
                        }

                        POP_BIT(attacks, targetSquare);
                    }

                    
                    if (enpassant != noSquare){
                        U64 enpassAttacks = pawnAttack[side][startSquare] & (1ULL << enpassant);

                        if (enpassAttacks){
                            int targetEnpass = GET_LEAST_SIG_BIT_IND(enpassAttacks);
                            std::cout << "\nPawn enpass capture: " << coords[startSquare] << coords[targetEnpass];
                        }
                    }

                    POP_BIT(bitboard, startSquare);
                }
            }

            if (piece == K){
                if (castle & wK){
                    if (!GET_BIT(occupancies[mono], f1) && !GET_BIT(occupancies[mono], g1)){
                        if (!squareAttackCheck(e1, black) && !squareAttackCheck(f1, black)){
                            std::cout << "\nCastling move: e1g1";
                        }
                    }
                }

                if (castle & wQ){
                    if (!GET_BIT(occupancies[mono], d1) && !GET_BIT(occupancies[mono], c1) && !GET_BIT(occupancies[mono], b1)){
                        if (!squareAttackCheck(e1, black) && !squareAttackCheck(d1, black)){
                            std::cout << "\nCastling move: e1c1";
                        }
                    }
                }
            }

        } else {
            if (piece == p){
                while (bitboard){
                    startSquare = GET_LEAST_SIG_BIT_IND(bitboard);
                    targetSquare = startSquare + 8;
                    
                    if (!(targetSquare > h1) && !GET_BIT(occupancies[mono], targetSquare)){

                        if (startSquare >= a2 && startSquare <= h2){   
                            std::cout << "\nPawn Promo: "<< coords[startSquare] << coords[targetSquare];

                        } else {
                            std::cout << "\nPawn Push: "<< coords[startSquare] << coords[targetSquare];
                            
                            if ((startSquare >= a7 && startSquare <= h7) && !GET_BIT(occupancies[mono], targetSquare + 8)){
                                std::cout << "\nDouble Pawn Push: "<< coords[startSquare] << coords[targetSquare + 8];
                            }
                        }


                    }

                     attacks = pawnAttack[side][startSquare] & occupancies[white];

                    while (attacks){
                        targetSquare = GET_LEAST_SIG_BIT_IND(attacks);

                        if (startSquare >= a2 && startSquare <= h2){   
                            std::cout << "\nPawn Capture Promo: "<< coords[startSquare] << coords[targetSquare];

                        } else {
                            std::cout << "\nPawn Capture Push: "<< coords[startSquare] << coords[targetSquare];
                        }

                        POP_BIT(attacks, targetSquare);
                    }

                    
                    if (enpassant != noSquare){
                        U64 enpassAttacks = pawnAttack[side][startSquare] & (1ULL << enpassant);

                        if (enpassAttacks){
                            int targetEnpass = GET_LEAST_SIG_BIT_IND(enpassAttacks);
                            std::cout << "\nPawn enpass capture: " << coords[startSquare] << coords[targetEnpass];
                        }
                    }

                    POP_BIT(bitboard, startSquare);
                }
            }

            if (piece == k){
                if (castle & bK){
                    if (!GET_BIT(occupancies[mono], f8) && !GET_BIT(occupancies[mono], g8)){
                        if (!squareAttackCheck(e8, white) && !squareAttackCheck(f8, white)){
                            std::cout << "\nCastling move: e8g8";
                        }
                    }
                }

                if (castle & bQ){
                    if (!GET_BIT(occupancies[mono], d8) && !GET_BIT(occupancies[mono], c8) && !GET_BIT(occupancies[mono], b8)){
                        if (!squareAttackCheck(e8, white) && !squareAttackCheck(d8, white)){
                            std::cout << "\nCastling move: e8c8";
                        }
                    }
                }
            }
        }

        if ((side == white) ? piece == N : piece == n){
            while (bitboard){
                startSquare = GET_LEAST_SIG_BIT_IND(bitboard);

                attacks = knightAttack[startSquare] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    targetSquare = GET_LEAST_SIG_BIT_IND(attacks);

                    if (!GET_BIT(((side == white) ? occupancies[black] : occupancies[white]), targetSquare)){
                        std::cout << "\nPiece quiet move: " << coords[startSquare] << coords[targetSquare];
                    } else {
                        std::cout << "\nPiece capture: " << coords[startSquare] << coords[targetSquare];
                    }


                    POP_BIT(attacks, targetSquare);
                }

                POP_BIT(bitboard, startSquare);
            }
        }

        if ((side == white) ? piece == B : piece == b){
            while (bitboard){
                startSquare = GET_LEAST_SIG_BIT_IND(bitboard);

                attacks = getBishopAttacks(startSquare, occupancies[mono]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    targetSquare = GET_LEAST_SIG_BIT_IND(attacks);

                    if (!GET_BIT(((side == white) ? occupancies[black] : occupancies[white]), targetSquare)){
                        std::cout << "\nPiece quiet move: " << coords[startSquare] << coords[targetSquare];
                    } else {
                        std::cout << "\nPiece capture: " << coords[startSquare] << coords[targetSquare];
                    }


                    POP_BIT(attacks, targetSquare);
                }

                POP_BIT(bitboard, startSquare);
            }
        }

        if ((side == white) ? piece == R : piece == r){
            while (bitboard){
                startSquare = GET_LEAST_SIG_BIT_IND(bitboard);

                attacks = getRookAttacks(startSquare, occupancies[mono]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    targetSquare = GET_LEAST_SIG_BIT_IND(attacks);

                    if (!GET_BIT(((side == white) ? occupancies[black] : occupancies[white]), targetSquare)){
                        std::cout << "\nPiece quiet move: " << coords[startSquare] << coords[targetSquare];
                    } else {
                        std::cout << "\nPiece capture: " << coords[startSquare] << coords[targetSquare];
                    }


                    POP_BIT(attacks, targetSquare);
                }

                POP_BIT(bitboard, startSquare);
            }
        }

        if ((side == white) ? piece == Q : piece == q){
            while (bitboard){
                startSquare = GET_LEAST_SIG_BIT_IND(bitboard);

                attacks = getQueenAttacks(startSquare, occupancies[mono]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    targetSquare = GET_LEAST_SIG_BIT_IND(attacks);

                    if (!GET_BIT(((side == white) ? occupancies[black] : occupancies[white]), targetSquare)){
                        std::cout << "\nPiece quiet move: " << coords[startSquare] << coords[targetSquare];
                    } else {
                        std::cout << "\nPiece capture: " << coords[startSquare] << coords[targetSquare];
                    }


                    POP_BIT(attacks, targetSquare);
                }

                POP_BIT(bitboard, startSquare);
            }
        }

         if ((side == white) ? piece == K : piece == k){
            while (bitboard){
                startSquare = GET_LEAST_SIG_BIT_IND(bitboard);

                attacks = kingAttack[startSquare] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    targetSquare = GET_LEAST_SIG_BIT_IND(attacks);

                    if (!GET_BIT(((side == white) ? occupancies[black] : occupancies[white]), targetSquare)){
                        std::cout << "\nPiece quiet move: " << coords[startSquare] << coords[targetSquare];
                    } else {
                        std::cout << "\nPiece capture: " << coords[startSquare] << coords[targetSquare];
                    }


                    POP_BIT(attacks, targetSquare);
                }

                POP_BIT(bitboard, startSquare);
            }
        }

    }
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

//debug board pos
#define empty_board "r3k2r/p11pqpb1/bn2pnp1/2pPN3/1p2P3/2N2Q1p/PPPBBPpP/R3K2R b KQkq c6 - 0 1"
#define tricky_pos "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPpP/R3K2R w KQkq a3 0 1"

//encode
#define ENCODE_MOVE(start, target, piece, promoted, capture, double, enpassant, castling) \
    (start) | (target << 6) | (piece << 12) | (promoted << 16) | (capture << 26) | (double << 21) | \
    (enpassant << 22) | (castling << 23)

#define GET_MOVE_START(move) (move & 0x3f)
#define GET_MOVE_TARGET(move) ((move & 0xfc0) >> 6)
#define GET_MOVE_PIECE(move) ((move & 0xf000) >> 12)
#define GET_MOVE_PROMOTED(move) ((move & 0xf0000) >> 16)
#define GET_MOVE_CAPTURE(move) (move & 0x100000)
#define GET_MOVE_DOUBLE(move) (move & 0x200000)
#define GET_MOVE_ENPASSANT(move) (move & 0x400000)
#define GET_MOVE_CASTLING(move) (move & 0x800000)


int main(){

    initAll();


    int move = ENCODE_MOVE(e2, e4, P, 0,0,0,0, 0);

    int start = GET_MOVE_START(move);
    int target = GET_MOVE_TARGET(move);
    int piece = GET_MOVE_PIECE(move);

    std::cout << coords[start] << "\n";
    std::cout << target;
    std::cout << piece;


    return 0;
}