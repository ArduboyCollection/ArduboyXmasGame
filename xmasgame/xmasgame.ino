#include <Tinyfont.h>
#include <Arduboy2.h>
#include <ArduboyPlaytune.h>
#include <ArdVoice.h>
//#include <PGMWrap.h>

#include "EEPROMAnything.h"
#include "structs.h"
#include "levels.h"
#include "music.h"
#include "images.h"
#include "voices.h"

//#define DEBUG
//#define MIRRORING

//Initialize the arduboy object
Arduboy2 arduboy;
ArdVoice ardvoice;
ArduboyPlaytune tunes(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

const int EEPROM_START = EEPROM_STORAGE_SPACE_START + 100;
GameState gameState = GameState::Splash;
unsigned int currentLevel = 0;
bool usedPieces[16], levelBoard[15][7], levelPiece[15][8];
byte currentPieceSelection = 0, currentPiecePositionX = 0, currentPiecePositionY = 0, oldLoadedPiece = 0,
     currentLevelWidth, currentLevelHeight, currentLevelTotalPieces, currentPieceWidth, currentPieceHeight,
     currentPieceNumber,  boardInitialXPosition, boardInitialYPosition;
int currentLevelPieceStartAddress;
unsigned long timerExpireOn;
const byte pieceWidthInPixels = 8, pieceHeightInPixels = 8;

void setup() {
  arduboy.begin();
  //arduboy.boot();
  arduboy.setFrameRate(60);
  arduboy.clear();
  arduboy.initRandomSeed();

  initializeAudio();

#if defined(DEBUG) || defined(MIRRORING)
  Serial.begin(9600);
#endif

#ifdef DEBUG
  delay(1000);
  Serial.println("Debug enabled");
#endif

  // Load highscore
  currentLevel = getSave();
}

void initializeAudio()
{
  // audio setup
  tunes.initChannel(PIN_SPEAKER_1);
#ifndef AB_DEVKIT
  // if not a DevKit
  tunes.initChannel(PIN_SPEAKER_2);
#else
  // if it's a DevKit
  tunes.initChannel(PIN_SPEAKER_1); // use the same pin for both channels
  tunes.toneMutesScore(true);       // mute the score when a tone is sounding
#endif
}

