byte currentPieceSelection = 0;
byte currentPiecePositionX = 0, currentPiecePositionY = 0;
byte oldLoadedPiece = 0;
bool usedPieces[16];
unsigned long timerExpireOn;

// Board position reference
byte boardInitialXPosition, boardInitialYPosition;
byte pieceWidthInPixels = 8, pieceHeightInPixels = 8;

// Draw the current pieces in the board, returns true if the board is full
bool drawBoardPieces()
{
  bool isComplete = true;
  // Draw board pieces
  for (int x = 0; x < currentLevelWidth; x++)
    for (int y = 0; y < currentLevelHeight; y++)
      if (levelBoard[x][y])
        arduboy.fillRect(boardInitialXPosition + x * pieceWidthInPixels, boardInitialYPosition + y * pieceHeightInPixels,
                         pieceWidthInPixels, pieceHeightInPixels);
      else
        isComplete = false;

  return isComplete;
}

// Draws the current piece, returns true if the space is empty to drop the piece in the board
bool drawCurrentPiece(byte xoffset, byte yoffset, bool placePiece = false)
{
  bool boardCanReceivePiece = true;
  for (int x = 0; x < currentPieceWidth; x++)
    for (int y = 0; y < currentPieceHeight; y++)
    {
      if (levelPiece[x][y])
        if (levelBoard[x + xoffset][y + yoffset])
        {
          boardCanReceivePiece = false;
          arduboy.drawRoundRect(boardInitialXPosition + (xoffset + x) * pieceWidthInPixels,
                                boardInitialYPosition + (y + yoffset) * pieceHeightInPixels, pieceWidthInPixels, pieceHeightInPixels, 1, BLACK);
        }
        else
        {
          if (placePiece)
            levelBoard[x + xoffset][y + yoffset] = true;

          arduboy.fillRoundRect(boardInitialXPosition + (xoffset + x)  * pieceWidthInPixels,
                                boardInitialYPosition + (y + yoffset) * pieceHeightInPixels, pieceWidthInPixels, pieceHeightInPixels, 1);
        }
    }

  return boardCanReceivePiece;
}

void drawHUD()
{
  // Hud
  tinyfont.setCursor(0, 0);
  tinyfont.print("LEVEL ");

  if (currentLevel < 99)
    tinyfont.print("0");
  if (currentLevel < 9)
    tinyfont.print("0");
  tinyfont.print(currentLevel + 1);
  tinyfont.setCursor(WIDTH / 2 + 5, 0);
  tinyfont.print("REMAINING ");

  byte left = 0;
  for (int i = 0; i < currentLevelTotalPieces; i++)
    if (!usedPieces[i])
      left++;
  if (left < 9)
    tinyfont.print("0");
  tinyfont.print(left);
}

GameState doPlacingPiece()
{
  GameState newState = GameState::PlayingPlacingPiece;
  arduboy.clear();

  drawHUD();

  if (arduboy.justPressed(LEFT_BUTTON)) {
    tunes.tone(1100, 30);
    currentPiecePositionX -= currentPiecePositionX > 0;
  }
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    tunes.tone(1100, 30);
    currentPiecePositionX += currentPiecePositionX < currentLevelWidth - currentPieceWidth;
  }
  if (arduboy.justPressed(UP_BUTTON)) {
    tunes.tone(1100, 30);
    currentPiecePositionY -= currentPiecePositionY > 0;
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    tunes.tone(1100, 30);
    currentPiecePositionY += currentPiecePositionY < currentLevelHeight - currentPieceHeight;
  }

  // Draw board boundary
  arduboy.drawRect(boardInitialXPosition - 1, boardInitialYPosition - 1, pieceWidthInPixels * currentLevelWidth + 2,
                   pieceHeightInPixels * currentLevelHeight + 2);
  drawBoardPieces();

  bool canPieceBeDroppedHere = drawCurrentPiece(currentPiecePositionX, currentPiecePositionY);

  if (arduboy.justPressed(B_BUTTON)) // Change the piece
    newState = GameState::PlayingChoosingPiece;

  if (arduboy.justPressed(A_BUTTON)) // Try to place the piece
  {
    if (canPieceBeDroppedHere && !usedPieces[currentPieceNumber])
    {
      drawCurrentPiece(currentPiecePositionX, currentPiecePositionY, true);
      usedPieces[currentPieceNumber] = true;

      newState = drawBoardPieces() ? GameState::AdvanceToNextLevel : GameState::PlayingChoosingPiece;
      tunes.tone(1300, 50);
      delay(50);
      tunes.tone(1500, 100);
    }
    else
    {
      // TODO: Handle error or warning
      tunes.tone(400, 200);
    }
  }
  arduboy.display();

  return newState;
}

bool blink = false;
bool doSplash()
{
  arduboy.clear();
  arduboy.drawCompressed(0, 0, splash_packer);

  if (arduboy.everyXFrames(blink ? 30 : 10))
    blink = !blink;

  if (blink)
  {
    arduboy.setCursor(45, HEIGHT - 8);
    arduboy.print("Press B");
  }

  arduboy.display();
  return arduboy.justPressed(B_BUTTON) || arduboy.justPressed(A_BUTTON);
}

void loadPieceToPlay(byte piece)
{
  currentPiecePositionX = 0;
  currentPiecePositionY = 0;
  loadPiece(piece);
}

bool doChoosingPiece()
{
  arduboy.clear();

  arduboy.setCursor(0, 0);
  arduboy.print("Select GIFT pattern");

  tinyfont.setCursor(0, HEIGHT - 4);
  tinyfont.print("A:Select B:Back A+B:Reset");

  const byte xini = 0, yini = 20;
  for (int i = 0; i < currentLevelTotalPieces; i++)
  {
    loadPiece(i);

    for (int x = 0; x < currentPieceWidth; x++)
      for (int y = 0; y < currentPieceHeight; y++)
        arduboy.drawPixel(xini + 1 + i * 15 + x, yini + y, levelPiece[x][y]);

    if (currentPieceSelection == i)
      arduboy.drawRect(xini + i * 15, yini, 15, 8);

    if (usedPieces[i])
      arduboy.drawLine(xini + i * 15, yini, 10 + xini + i * 15, 10 + yini);
  }

  if (arduboy.justPressed(LEFT_BUTTON)) {
    tunes.tone(1100, 30);
    currentPieceSelection -= currentPieceSelection > 0;
  }

  if (arduboy.justPressed(RIGHT_BUTTON)) {
    tunes.tone(1100, 30);
    currentPieceSelection += currentPieceSelection < currentLevelTotalPieces - 1;
  }

  arduboy.display();

  bool exit = false;

  if (arduboy.justPressed(A_BUTTON))
    if (usedPieces[currentPieceSelection])
    {
      // TODO: Handle error or warning
      tunes.tone(400, 200);
    }
    else
    {
      tunes.tone(1500, 100);
      delay(50);
      tunes.tone(1300, 50);

      loadPieceToPlay(currentPieceSelection);
      exit = true;
    }

  if (arduboy.justPressed(B_BUTTON))
  {
    loadPieceToPlay(oldLoadedPiece);
    exit = true;
  }
  return exit;
}

void loop()
{
  if (!arduboy.nextFrame())
    return;

  arduboy.pollButtons();

  switch (gameState)
  {
    case GameState::AdvanceToNextLevel:
      // TODO: Handle winning, ending, etc
      drawHUD();
      arduboy.display();
      
      currentLevel++;
      gameState = GameState::LevelStart;
      break;

    case GameState::Splash:
      if (!tunes.playing())
        tunes.playScore(splash);

      if (doSplash())
      {
        tunes.stopScore();
        gameState = GameState::LevelStart;
      }
      break;

    case GameState::LevelStart:
      loadLevel(currentLevel);

      boardInitialXPosition = (WIDTH - currentLevelWidth * pieceWidthInPixels) / 2;
      boardInitialYPosition = 4 + (HEIGHT - currentLevelHeight * pieceHeightInPixels) / 2;

      currentPieceSelection = 0;
      loadPieceToPlay(currentPieceSelection);

      for (int i = 0; i < currentLevelTotalPieces; i++)
        usedPieces[i] = false;
      currentPieceSelection = 0;

      timerExpireOn = millis() + 1000;
      gameState = GameState::LevelStartPreSplash;

      tunes.stopScore();

      if (currentLevel == 0)
      {
        TCCR4A = bit(COM4A0) | bit(PWM4A); // enable PWM on speaker pins
        ardvoice.playVoice(hohoho);
      }
      else
      {
        TCCR4A = 0; // disable PWM on speaker pins
        tunes.playScore(win);
      }

      break;

    case GameState::LevelStartPreSplash:
      if (millis() > timerExpireOn)
      {
        gameState = GameState::LevelStartSplash;
        timerExpireOn = millis() + 1000;
      }
      break;

    case GameState::LevelStartSplash:
      arduboy.clear();
      arduboy.setCursor(26, (HEIGHT - 8) / 2);
      arduboy.print("Level ");
      arduboy.print(currentLevel + 1);
      arduboy.print(" Start!");
      arduboy.display();

      if (millis() > timerExpireOn)
      {
        TCCR4A = 0; // disable PWM on speaker pins
        gameState = GameState::PlayingPlacingPiece;
      }
      break;

    case GameState::PlayingPlacingPiece:
      if (!tunes.playing())
        tunes.playScore(level);

      gameState = doPlacingPiece();

      //if (gameState != GameState::PlayingPlacingPiece)
      //  tunes.stopScore();
      //byte oldLoadedPiece = currentPieceNumber;
      break;

    case GameState::PlayingChoosingPiece:
      /*if (!tunes.playing())
        tunes.playScore(choose);*/

      if (doChoosingPiece())
      {
        //tunes.stopScore();
        gameState = GameState::PlayingPlacingPiece;
      }
      break;

    /*
       case GameState::PlayingPlacingPiece:
       {
       if (arduboy.justPressed(LEFT_BUTTON))
       {
       currentLevel--;
       }

       if (arduboy.justPressed(RIGHT_BUTTON))
       {
       currentLevel++;
       }

       if (arduboy.justPressed(UP_BUTTON))
       {
       currentPieceNumber++;
       loadPiece(currentPieceNumber);
       }

       if (arduboy.justPressed(DOWN_BUTTON))
       {
       currentPieceNumber--;
       loadPiece(currentPieceNumber);
       }

       if (arduboy.justPressed(A_BUTTON))
       {
       loadLevel(currentLevel);
       Serial.println("Level loaded.");
       }

       arduboy.clear();
       tinyfont.setCursor(0, 0);
       tinyfont.print("LEVEL: ");
       tinyfont.print(currentLevel);
       tinyfont.print("\nTOTAL PIECES: ");
       tinyfont.print(currenLevelTotalPieces);
       tinyfont.print("\nLEVEL SIZE W,H: ");
       tinyfont.print(currentLevelWidth);
       tinyfont.print(",");
       tinyfont.print(currentLevelHeight);
       tinyfont.print("\nPIECE: ");
       tinyfont.print(currentPieceNumber);
       tinyfont.print("\nPIECE SIZE W,H: ");
       tinyfont.print(currentPieceWidth);
       tinyfont.print(",");
       tinyfont.print(currentPieceHeight);
       tinyfont.print("\n\nLOAD BOARD WITH (A)\nBOARD:       PIECE: ");

       // Draw level and piece
       int xini = 3;
       int yini = HEIGHT - 12;

       arduboy.drawRect(xini - 1, yini - 1, currentLevelWidth + 2, currentLevelHeight + 2);

       for (int x = 0; x < currentLevelWidth; x++)
       for (int y = 0; y < currentLevelHeight; y++)
       if (levelBoard[x][y])
        arduboy.drawPixel(xini + x, yini + y);

       xini = 30;
       //arduboy.drawRect(xini - 1, yini - 1, currentPieceWidth + 2, currentPieceHeight + 2);


       for (int x = 0; x < currentPieceWidth; x++)
       for (int y = 0; y < currentPieceHeight; y++)
       if (levelPiece[x][y])
       {
        arduboy.drawPixel(xini + x, yini + y);
       }

       arduboy.display();
       }
       break // */

    default: // Level
      /*if (!tunes.playing() && gameState == GameState::Playing)
        tunes.playScore(level);*/
      //gameState = doGame();
      //gameState = doGame(gameState == GameState::Playing);

      break;
  }
}


