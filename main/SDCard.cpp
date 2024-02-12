#include "SDCard.h"

SDCard::SDCard()
{
    if(!Serial){
        Serial.begin(115200); 
    }
    if(!SD.begin()){
        working=false;
        Serial.println("SDCard Mount Failed");
    }
    else{
        Serial.println("SDCard found!");
        working=true;
    }
}

void SDCard::listDir(const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = SD.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void SDCard::createDir(const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(SD.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void SDCard::removeDir(const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(SD.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void SDCard::readFile(const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = SD.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void SDCard::writeFile(const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = SD.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void SDCard::appendFile(const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = SD.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void SDCard::renameFile(const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (SD.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void SDCard::deleteFile(const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(SD.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}


void SDCard::testFileIO(const char * path){
    File file = SD.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = SD.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void SDCard::testRun()
{
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    //read/write
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    listDir("/", 0);
    createDir("/mydir");
    listDir("/", 0);
    removeDir("/mydir");
    listDir("/", 2);
    writeFile("/hello.txt", "Hello ");
    appendFile("/hello.txt", "World!\n");
    readFile("/hello.txt");
    deleteFile("/foo.txt");
    renameFile("/hello.txt", "/foo.txt");
    readFile("/foo.txt");
    testFileIO("/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

bool SDCard::isWorking()
{
    return working;
}
