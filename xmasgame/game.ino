bool levelBoard[15][7], levelPiece[15][7];
byte currentLevelWidth, currentLevelHeight, currentLevelTotalPieces;
byte currentPieceWidth , currentPieceHeight, currentPieceNumber;
int currentLevelPieceStartAddress;

// Load a piece into memory
void loadPiece(byte pieceNumber)
{
  byte pieceNum = pgm_read_byte_near(currentLevelPieceStartAddress + pieceNumber);
  int piecePos = pgm_read_word_near(pieces + pieceNum);
  int pieceEnd = pgm_read_word_near(pieces + pieceNum + 1);

  byte w = pgm_read_byte_near(piecesBytes + piecePos);

  // Retrieve piece
  byte totalBytes = pieceEnd - piecePos - 1;
  int totalBits = totalBytes * 8;
  bool bits[totalBits];

  for (int i = 0; i < totalBytes; i++)
  {
    byte pieceByte = pgm_read_byte_near(piecesBytes + piecePos + 1 + i);
    for (int b = 0; b < 8; b++)
      bits[(i * 8) + b] = bitRead(pieceByte, b);
  }

  byte lastBit = 0;
  for (int i = 0; i < totalBits; i++)
  {
    levelPiece[i % w][i / w] = bits[i];

    if (bits[i])
      lastBit = i;
  }

  currentPieceWidth = w;
  currentPieceHeight = 1+(lastBit / w);
  currentPieceNumber = pieceNumber;
}

// Load a level into memory
void loadLevel(unsigned int levelNumber)
{
  int currentLevelPos = pgm_read_word_near(levels + levelNumber);
  int currentLevelEnd = pgm_read_word_near(levels + levelNumber + 1);

  currentLevelWidth = pgm_read_byte_near(levelsBytes + currentLevelPos);
  currentLevelHeight = pgm_read_byte_near(levelsBytes + currentLevelPos + 1);

  for (int i = 0; i < currentLevelWidth; i++)
    for (int j = 0; j < currentLevelHeight; j++)
      levelBoard[i][j] = false;

  currentLevelPieceStartAddress = levelsBytes + currentLevelPos + 2;
  currentLevelTotalPieces = currentLevelEnd - currentLevelPos - 2;
  currentPieceNumber = 0;
  currentLevel = levelNumber;
}
