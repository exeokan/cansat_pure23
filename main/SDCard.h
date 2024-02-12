/**
 * @file SDCard.h
 * @brief The SDCard class provides functionality for interacting with an SD card. 
 * Majortiy of the methods are taken from the SD example in the Arduino IDE.
 */

#pragma once

#include "FS.h"
#include "SD.h"
#include "SPI.h"

class SDCard
{
private:
    bool working;
public:
    SDCard(/* args */);
    ~SDCard();
    void listDir( const char * dirname, uint8_t levels);
    void createDir( const char * path);
    void removeDir( const char * path);
    void readFile( const char * path);
    void writeFile( const char * path, const char * message);
    void appendFile( const char * path, const char * message);
    void renameFile( const char * path1, const char * path2);
    void deleteFile( const char * path);
    void testFileIO( const char * path);
    void testRun();

    bool isWorking();
};

