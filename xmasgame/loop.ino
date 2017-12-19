bool resetSound = true;

void loop()
{
  if (!arduboy.nextFrame())
    return;

  arduboy.pollButtons();

  // Reset routine
  if (gameState == GameState::PlayingPlacingPiece || gameState == GameState::PlayingChoosingPiece)
  {
    if (arduboy.pressed(A_BUTTON + B_BUTTON))
    {
      resetSound = true;
      for (int i = 0; i < 255; i++)
      {
        analogWrite(RED_LED,i);
        tunes.tone(2000 - i * 40, 6);
        delay(4);
      }

      arduboy.setRGBled(0, 0, 0);
      gameState = GameState::LevelStart;
    }
  }

  switch (gameState)
  {
    case GameState::AdvanceToNextLevel:
      // TODO: Handle winning, ending, etc
      drawHUD();
      arduboy.display();

      currentLevel++;
      setSave(currentLevel);
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
      boardInitialYPosition = 3 + (HEIGHT - currentLevelHeight * pieceHeightInPixels) / 2;

      currentPieceSelection = 0;
      loadPieceToPlay(currentPieceSelection);

      for (int i = 0; i < currentLevelTotalPieces; i++)
        usedPieces[i] = false;
      currentPieceSelection = 0;

      timerExpireOn = millis() + 1000;
      gameState = GameState::LevelStartPreSplash;

      tunes.stopScore();

      if (currentLevel == 0 || resetSound)
      {
        TCCR4A = bit(COM4A0) | bit(PWM4A); // enable PWM on speaker pins
        ardvoice.playVoice(hohoho);
        resetSound = false;
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
      arduboy.setCursor(24, (HEIGHT - 8) / 2);
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


