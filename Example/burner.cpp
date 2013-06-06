#include <string>
#include <iostream>

#include "MSPBSL_Connection5xxUSB.h"
#include "MSPBSL_PhysicalInterfaceUSB.h"
#include "MSPBSL_PacketHandler5xxUSB.h"


int main(int argc, char* argv[])
{
       MSPBSL_Connection5xxUSB *theBSLConnection = new MSPBSL_Connection5xxUSB("");
       MSPBSL_PhysicalInterfaceUSB* s  = new MSPBSL_PhysicalInterfaceUSB(""); // Parity handled in object;
       MSPBSL_PacketHandler5xxUSB* p = new MSPBSL_PacketHandler5xxUSB("");

       std::string returnstring;
       int i = theBSLConnection->TX_BSL_Version(returnstring);
       std::cout << (i == 0 ? "OK" : "FAILED") << std::endl;
       if (i == 0)
               std::cout << returnstring << std::endl;

       delete p;
       delete s;
       delete theBSLConnection;
       return 0;
       }

