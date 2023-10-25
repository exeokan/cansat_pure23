#include "SDCard.h"
class Sattalite
{
private:
    SDCard sdCard;
public:
    Sattalite(/* args */);
    ~Sattalite();
    void SDCardTest();
};

Sattalite::Sattalite(/* args */)
{
    if(!Serial){
        Serial.begin(115200); // DANGEROUS CODE!!!!!!!!!!11
        //this Serial is for USB??
        //https://www.arduino.cc/reference/en/language/functions/communication/serial/
    }
}

Sattalite::~Sattalite()
{
}

void Sattalite::SDCardTest(){
    sdCard.listDir(SD, "/", 0);
    sdCard.createDir(SD, "/mydir");
    sdCard.listDir(SD, "/", 0);
    sdCard.removeDir(SD, "/mydir");
    sdCard.listDir(SD, "/", 2);
    sdCard.writeFile(SD, "/hello.txt", "Hello ");
    sdCard.appendFile(SD, "/hello.txt", "World!\n");
    sdCard.readFile(SD, "/hello.txt");
    sdCard.deleteFile(SD, "/foo.txt");
    sdCard.renameFile(SD, "/hello.txt", "/foo.txt");
    sdCard.readFile(SD, "/foo.txt");
    sdCard.testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}