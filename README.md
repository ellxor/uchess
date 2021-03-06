## u-chess

A small chess implementation focused on storing the position in as little space
as possbile. Currently the size of the position is 256 bits.

_Note: this code relies on the [BMI2 instruction set](https://en.wikipedia.org/wiki/X86_Bit_manipulation_instruction_set#BMI2_(Bit_Manipulation_Instruction_Set_2))._

### Uses:
- chess databases
- chess engines (can fit more into caches / LUTs)
- any chess application

### Usage:
Run `make` to build the `libuchess.a` archive to be used with `uchess.h`.
Run `make unittest` to test and benchmark the library.

### Design:

The position is rotated to the perspective of the current side to move, so
'white' always refers to the side to move, regardless of the actual color.

The position itself is stored in 4 [bitboards](https://www.chessprogramming.org/Bitboards):
- 1 white bitboard, storing all the 'white' pieces.
- 3 auxiliary bitboards, forming a board table.

The nth bit of each auxiliary bitboard (X,Y,Z) forms a 3 bit pattern: ZYX.
These three bits represent the piece that occupies the nth square on the
board (A1 = 0, H8 = 63).

The three bit pattern is as follows:
```
000: None         100: Rook
001: Pawn         101: Queen
010: Knight       110: King
011: Bishop       111: Info
```

The info bitboard contains all of the info relating to the position. The least
significant 8 bits store the en-passant mask, which represents the 6th rank.
Finally, the last 4 bits store the castling rights, again where 'white' refers
to the side to move, not the actual white side.

Example info bits:
```
v~~~ castling rights (right to left: KQkq)
111100000000
    ^~~~~~~~ en passant bits (right to left: a-h), max of one set bit)
```

The info bits are located on the non-occupied squares of the board, of which
there must be at least 32. Therefore, the `pext` instruction is required to
align the info to the correct bit positions.

### Implementation:
The move generation is achieved using `pdep/pext` [magic bitboards](https://www.chessprogramming.org/Magic_Bitboards#Fancy)
for sliding piece attacks. The move generation is fully legal, preventing
the king to walk into check, and doing a post-filter to remove moving pinned
pieces and allowing check.

### Performance:
|position |depth|    nodes|speed (Mnps)|
|---------|:---:|:-------:|:----------:|
|startpos |    6|119060324|         201|
|kiwipete |    5|193690690|         244|
|position3|    7|178633661|         171|
|position4|    6|706045033|         241|
|position5|    5| 89941194|         230|
|position6|    5|164075551|         240|

_(measured on Intel i5-9400, using clang 11.0.1, best of 3)_
