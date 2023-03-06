#include "FtdCard.h"
#include "Utils.h"
#include <sys/time.h>

namespace SP {

  using std::cout;
  using std::cerr;
  using std::endl;
  using std::string;
  
  const int FtdCard::idVendor  = 0x0403;  // vendor ID
  const int FtdCard::idProduct = 0x6001; // product ID
  const unsigned int FtdCard::timeout = 500; // USB command timeout
  const unsigned int FtdCard::numPins = 8;
  
  
  FtdCard::FtdCard(Config conf, bool debugmode) : debug(debugmode), conf(conf) {}

  bool FtdCard::Init(const Sock& soc) {
    int i;
    int dev_cnt;
    libusb_device **devs;
    libusb_context *ctx = NULL;
    libusb_device_descriptor desc;
  
    try{
  
      if (libusb_init(&ctx) < 0)  {
        throw FtdCardException("libusb (1.0) couldn't be initialized.");
      }
  
  //    libusb_set_debug(ctx, 0);
      
      dev_cnt = libusb_get_device_list(ctx, &devs);
      if (dev_cnt < 0)  {
        throw FtdCardException("The USB device list couldn't be read.");
      }
  
      for (i=0; i<dev_cnt; ++i)  {
        r = libusb_get_device_descriptor(devs[i], &desc);
        if ((desc.idVendor == idVendor) && (desc.idProduct == idProduct) )  {
          if ((ftDev = libusb_open_device_with_vid_pid(ctx, idVendor, idProduct)) == NULL)  {
            throw FtdCardException("The ftd USB device was found but couldn't be opened");
          }  else  {
            break;
          }
        }
      }
      if (i == dev_cnt)  {
        throw FtdCardException("The ftd USB device couldn't be found.");
      }
  
      if (libusb_kernel_driver_active(ftDev, 0) == 1)  {
        if (libusb_detach_kernel_driver(ftDev, 0) != 0)  {
          throw FtdCardException("A kernel driver attached to the ftd USB device couldn't be detached");
        }
      }
  
      if (libusb_claim_interface(ftDev, 0) < 0)  {
        throw FtdCardException("The ftd USB device has been opened but couldn't be claimed");
      }
     
      if (debug){
        cerr << "Initialized ftd card driver." << endl;
      }

      this->soc = soc; 

    } catch (FtdCardException &e)  {
      e.debug_print();
      cerr << "An error occurred during the initialization of the ftd card." << endl;
      return false;
    }
    
    return true;
  }
  
  
  FtdCard::~FtdCard(){
    libusb_release_interface(ftDev, 0);
    libusb_close(ftDev);
    if (debug)  {
      cerr << "Closed ftd card(s)." << endl;
    }
  }
  
  
  unsigned char FtdCard::binaryStringToNum(unsigned char *str){
  
    if (strlen((const char *)str)!=numPins){
      throw FtdCardException("Invalid signal string.");
  //    printf("Invalid signal string.");
    }
  
    unsigned char* pStr = str;
    unsigned char numVal = 0;
    while(*pStr != 0)  {
      numVal <<= 1;
      numVal += (*pStr == '0') ? 1 : 0;  // (*pStr == '0') => inversion
      ++pStr;
    }
  
    return numVal;
  }
  
  
  
  void FtdCard::write(unsigned char *s){
    unsigned char d;
    int actual_bytes_transferred;
  
    try{
      d = binaryStringToNum(s);
      
      r = libusb_bulk_transfer(ftDev, (2 | LIBUSB_ENDPOINT_OUT), &d, sizeof(d), &actual_bytes_transferred, 0);
      if (r != 0)  {
        cerr << "An error occured while trying to send a pulse via the ftd USB device." << endl;
        throw FtdCardException("Failed to send USB pulse.");
      } else if (actual_bytes_transferred != sizeof(d))  {
        cerr << "Failed to send USB pulse." << endl;
      }
    }
    catch (FtdCardException &e){
      e.debug_print();
      throw; //rethrow so the exception appears in python
    }
  }
  
  
  void FtdCard::allOn(){
    unsigned char d = 0;
    int actual_bytes_transferred;
  
    r = libusb_bulk_transfer(ftDev, (2 | LIBUSB_ENDPOINT_OUT), &d, sizeof(d), &actual_bytes_transferred, 0);
      if (r != 0)  {
        cerr << "An error occured while trying to send a pulse via the ftd USB device." << endl;
        throw FtdCardException("Failed to send USB pulse.");
      }  else if (actual_bytes_transferred != sizeof(d))  {
        cerr << "Failed to send USB pulse." << endl;
      }
  }
  
  
  void FtdCard::allOff(){
    ; // NO-OP (pulse is deactivated automatically after 10 ms
  }
  
  void FtdCard::Run() {
    std::string line;
  
    try {
      while (true) { // Loop until closed by network exception.
        soc.Recv(line);
        const std::vector<std::string> cmd = SplitCSV(line);
  
        if (cmd.size() < 1) {
          continue;
        }
  
        if (cmd.at(0) == "NSBSYNCPULSE") {
          this->allOn();
        } else if (cmd.at(0) == "NSBHEARTBEAT") {
          soc.Send("NSBHEARTBEAT_OK");
        } else if (cmd.at(0) == "NSBOPENUSB") {
          soc.Send("NSBOPENUSB_OK");
        } else if (cmd.at(0) == "NSBCLOSEUSB") {
          return;
        } else {
          soc.Send(std::string("NSBERROR,") + CleanError("FreiburgNetworkSyncBox command not "
                "recognized:  \"", cmd.at(0), "\""));
        }
      }
    }
    catch (std::runtime_error& e) {
      cerr << e.what() << endl;;
      // Connection closed, so proceed to terminate.
    }
  }

}

