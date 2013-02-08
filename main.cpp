#include <string>
#include <iostream>

#include "MSPBSL_Connection5xxUSB.h"
#include "MSPBSL_PhysicalInterfaceUSB.h"
#include "MSPBSL_PacketHandler5xxUSB.h"

int main(void) {
  MSPBSL_Connection5xxUSB *theBSLConnection = new MSPBSL_Connection5xxUSB("");
  MSPBSL_PhysicalInterfaceUSB* s  = new MSPBSL_PhysicalInterfaceUSB(""); // Parity handled in object;
  MSPBSL_PacketHandler5xxUSB* p = new MSPBSL_PacketHandler5xxUSB("");
  p->setPhysicalInterface(s);
  theBSLConnection->setPacketHandler(p);

  printf("%p\n", theBSLConnection);
  ((theBSLConnection->getPacketHandler())->getPhysicalInterface())->invokeBSL();

  uint8_t databuf[0x20000];
  for (unsigned i=0; i<0x20000; i++)
    databuf[i] = 0x00FF;

  string returnstring;
  int i;
  std::cout << "Transmitting password... ";
  i = theBSLConnection->RX_Password(databuf);
  std::cout << (i == 0 ? "OK" : "FAILED") << std::endl;

  std::cout << "Loading BSL into RAM... ";
  i = theBSLConnection->loadRAM_BSL();
  std::cout << (i == 0 ? "OK" : "FAILED") << std::endl;

  std::cout << "Obtaining BSL version... ";
  i = theBSLConnection->TX_BSL_Version(returnstring);
  std::cout << (i == 0 ? "OK" : "FAILED") << std::endl;
  if (i == 0)
    std::cout << returnstring << std::endl;

#if 0
  //	initialize some buffers and variables that are being used in this example
  uint32_t addr=0x0220;
  uint32_t size=0x0010;
  uint8_t databuf[0x20000];
  uint8_t returnbuf[0x20000];
  std::string returnstring;

  for (unsigned i=0; i<0x20000; i++)
    databuf[i] = (0x00FF & i); //initialize buffer with ascending bytes
  memset(returnbuf, 0, sizeof(returnbuf))

  // Erase the whole device memory
  // Returns 0 if everything worked correctly
  i = theBSLConnection->massErase();

/*
 *	Unlock the BSL using the default password.
 *  Returns 0 if the everything worked correctly and the BSL is unlocked
 */
  i = theBSLConnection->RX_Password();

/*
 *	Unlock the BSL using a 32 byte password array.
 *  Returns 0 if everything worked correctly and the BSL is unlocked
 */

  i = theBSLConnection->RX_Password(databuf);

/*
 *	Get the BSL version and chip ID
 *  Returns 0 if everything worked correctly and the data has been written to the string
 */
  i = theBSLConnection->TX_BSL_Version(returnstring);

/*
 *	Set the program counter to a particular adress
 *  Returns 0 if everything worked correctly and the program counter has been set accordingly
 */
  i = theBSLConnection->setPC(addr);

/*
 *	Read a block of data from the device
 *  Returns 0 if everything worked correctly and the data has been written to the buffer
 */
  i = theBSLConnection->TX_DataBlock(returnbuf, addr, size);

/*
 *	Write a block of data to the device
 *  Returns 0 if everything worked correctly and the data has been written and verified correctly
 */

  i = theBSLConnection->RX_DataBlock(databuf, addr, size);

/*
 *	Read a TI-TXT-file at the specified path and write it to the device.
 *  Returns 0 if everything worked correctly and the data has been written and verified correctly
 */
  i = theBSLConnection->loadFile("testfile.txt");

/*
 *	This Function returns a string which fully describes an error code
 */
  returnstring = theBSLConnection->getErrorInformation(i);
#endif

  return 0;
}
