#ifndef FTD_CARD_H
#define FTD_CARD_H

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "NetClient.h"
#include "Config.h"
#include "libusb.h"


namespace SP {

  using std::cerr;
  using std::endl;
  using std::string;
  
  class FtdCard{
    bool debug;
    unsigned char number_of_boards;
  
    struct libusb_device_handle *ftDev;
    static const int idVendor;  // vendor ID
    static const int idProduct; // product ID
    static const unsigned int timeout; // USB command timeout
    static const unsigned int numPins;
    int r;

    Config conf;
    Sock soc;
  
   public:
    FtdCard(Config conf, bool debug=false);
    ~FtdCard();
    bool Init(const Sock& soc);
    void write(unsigned char *s);
    void allOn();
    void allOff();
    void Run();
   private:
    unsigned char binaryStringToNum(unsigned char *);
  };
  
  class FtdCardException{
    string msg;
   public:
   FtdCardException(string s):msg(s){}
    void debug_print(){ cerr << "ftd card error: " << msg << endl; }
  };

}

#endif
