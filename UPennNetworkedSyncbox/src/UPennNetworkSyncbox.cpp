#include "NetClient.h"
#include "Config.h"
#include "Utils.h"
#include "Plugin.h"

#include <iostream>
#include <string>

#define MYTIMESTAMP __TIMESTAMP__

bool Run(SP::Sock& soc) {
  std::string line;

  try {
    while (true) { // Loop until closed by network exception.
      soc.Recv(line);
      const std::vector<std::string> cmd = SP::SplitCSV(line);

      if (cmd.size() < 1) {
        continue;
      }

      if (cmd.at(0) == "FNSBSYNCPULSE") {
        SyncPulse();
      } else if (cmd.at(0) == "FNSBHEARTBEAT") {
        soc.Send("FNSBHEARTBEAT_OK");
      } else if (cmd.at(0) == "FNSBOPENUSB") {
        auto openMsg = OpenUSB();
        std::cerr << openMsg << std::endl;
        if (strcmp(openMsg, "didn't open USB...") == 0) {
          soc.Send(std::string("FNSBERROR: ") + SP::CleanError("Syncbox didn't open"));
          soc.Close();
          return -4;
        }
        soc.Send("FNSBOPENUSB_OK");
      } else if (cmd.at(0) == "FNSBCLOSEUSB") {
        std::cerr << CloseUSB() << std::endl;
        soc.Send("FNSCLOSEUSB_OK");
        break;
      } else {
        soc.Send(std::string("FNSBERROR: ") + SP::CleanError("UPennNetworkSyncbox command not "
              "recognized:  \"", cmd.at(0), "\""));
      }
    }
  }
  catch (std::runtime_error& e) {
    std::cerr << e.what() << std::endl;;
    // Connection closed, so proceed to terminate.
    return -5;
  }
  
  std::cerr << "Finished running" << std::endl;
  soc.Close();
  return 0;
}

int main(int argc, char* argv[]) {
  unsigned long long port = 0;
  std::string host;
  std::string config_file;

  if (argc < 4) {
    std::cerr << "UPennNetworkSyncbox, " << MYTIMESTAMP << std::endl;
    std::cerr << argv[0] << " [host] [port] [config_file]" << std::endl;
    return -1;
  }

  host = argv[1];

  try {
    port = std::stoull(argv[2]);
  }
  catch (...) {
    std::cerr << "Could not convert " << argv[2] << " to port number.\n";
    return -2;
  }

  config_file = argv[3];

  try {
    SP::Sock soc;
    SP::Net::Listener listener(std::to_string(port));
    listener.Accept(soc);
    //SP::Net::Connect(soc, host, std::to_string(port));
    try {
      SP::Config config{config_file};
      
      Run(soc);
    }
    catch (std::exception &ex) {
      if (soc.CanSend(false)) {
        std::string errmsg = std::string("FNSBERROR,") + SP::CleanStr(ex.what());
        soc.Send(errmsg, false);
      }
      throw;
    }
  }
  catch (std::exception &ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    return -3;
  }

  return 0;
}


