const byte CRC = 0xAE, OFFSET = 12;
const unsigned int CRC_C = 48000;

// Get the latest solved puzzle
unsigned int getSave()
{
  unsigned int level, crc;

  bool isValid = false;
  if (EEPROM.read(EEPROM_START) == CRC)
  {
    EEPROM_readAnything(EEPROM_START + OFFSET, level);
    EEPROM_readAnything(EEPROM_START + OFFSET + sizeof(level), crc);

    if (level + crc == CRC_C)
      isValid = true;
  }

  return isValid ? level : 0;
}

// Save the last solved puzzle
void setSave(const unsigned int & level)
{
  EEPROM.write(EEPROM_START, CRC);
  EEPROM_writeAnything(EEPROM_START + OFFSET, level);
  EEPROM_writeAnything(EEPROM_START + OFFSET + sizeof(level), CRC_C - level);
}
