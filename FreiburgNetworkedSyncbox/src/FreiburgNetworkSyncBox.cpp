#include "NetClient.h"
#include "FtdCard.h"
#include "Config.h"
#include "Utils.h"

#include <iostream>
#include <string>

#define MYTIMESTAMP __TIMESTAMP__


int main(int argc, char* argv[]) {
  unsigned long long port = 0;
  std::string host;
  std::string config_file;

  if (argc < 4) {
    std::cerr << "FreiburgNetworkSyncBox, " << MYTIMESTAMP << std::endl;
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
    while(true) {
      SP::Sock soc;
      SP::Net::Listener listener(std::to_string(port));
      std::cout << "Listening on port: " << port << "\n";
      listener.Accept(soc);
      //SP::Net::Connect(soc, host, std::to_string(port));
      try {
        SP::Config config{config_file};
        SP::FtdCard ftd_card(config, true);
        if (!ftd_card.Init(soc)) { return -4; }
        ftd_card.Run();
        std::cerr << "Finished running" << std::endl;
      }
      catch (std::exception &ex) {
        if (soc.CanSend(false)) {
          std::string errmsg = std::string("NSBERROR,") + SP::CleanStr(ex.what());
          soc.Send(errmsg, false);
        }
        else {
          std::cerr << "Error: " << ex.what() << "\n";
        }
      }
    }
  }
  catch (std::exception &ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    return -3;
  }

  return 0;
}

