#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>

#include "MSPBSL_Connection5xxUSB.h"
#include "MSPBSL_PhysicalInterfaceUSB.h"
#include "MSPBSL_PacketHandler5xxUSB.h"

#define DefaultAddr 0x0220

#define EXIT_SUCCESS 0
#define EXIT_WITH_ERROR -1
#define FALSE 0
#define TRUE 1
#define LAST_OPTION -1

MSPBSL_PhysicalInterfaceUSB* Interface;
MSPBSL_PacketHandler5xxUSB* PacketHandler;

static const char *optString = "r:vmo:bh";
struct globalArgs_t {
	int version;                
	std::string inFile;    
	int massErase;
	int newBsl;
	int resetTime;
	int help;     
} globalArgs;

int CheckPassword(MSPBSL_Connection5xxUSB* theBSLConnection)
{
	uint8_t databuf[0x20000];
	for (unsigned i=0; i<0x20000; i++)
		databuf[i] = 0x00FF;
	int i = (theBSLConnection)->RX_Password(databuf);
	printf("%s: %s \n",__func__ ,(i == EXIT_SUCCESS ? "OK" : "FAILED"));
	return i; 
}

int LoadBSL(MSPBSL_Connection5xxUSB* theBSLConnection)
{
	int i = theBSLConnection->loadRAM_BSL();
	printf("%s: %s \n",__func__ ,(i == EXIT_SUCCESS ? "OK" : "FAILED"));
	return i; 
}

int PrintBSLVersion(MSPBSL_Connection5xxUSB* theBSLConnection)
{
	std::string returnstring;
	int i = theBSLConnection->TX_BSL_Version(returnstring);
	printf("%s:  %s value %s \n",__func__ ,(i == EXIT_SUCCESS ? "OK" : "FAILED"), returnstring.c_str());
	return i;
}

int MassErase(MSPBSL_Connection5xxUSB *theBSLConnection)
{
	int i = theBSLConnection->massErase();
	printf("%s: %s \n",__func__ ,(i == EXIT_SUCCESS ? "OK" : "FAILED")); 
	return i;
}

MSPBSL_Connection5xxUSB* OpenConnection()
{
	MSPBSL_Connection5xxUSB *theBSLConnection = new MSPBSL_Connection5xxUSB("");

	Interface  = new MSPBSL_PhysicalInterfaceUSB("");
	if (!Interface->isValid())
	{
		delete Interface;
		delete theBSLConnection;
		return NULL;
	}

	PacketHandler = new MSPBSL_PacketHandler5xxUSB("");

	PacketHandler->setPhysicalInterface(Interface);
	theBSLConnection->setPacketHandler(PacketHandler);
	((theBSLConnection->getPacketHandler())->getPhysicalInterface())->invokeBSL();

	return theBSLConnection;
}

void CloseConnetion(MSPBSL_Connection5xxUSB *theBSLConnection)
{
	delete PacketHandler;
	delete Interface;
	delete theBSLConnection;
}

int Run (MSPBSL_Connection5xxUSB *theBSLConnection,uint32_t addr)
{
	int i = theBSLConnection->setPC(addr);
	printf("%s: %s \n",__func__ ,(i == EXIT_SUCCESS ? "OK" : "FAILED"));
	return i; 
}

int LoadFirmware(MSPBSL_Connection5xxUSB *theBSLConnection,std::string Firmware)
{
	struct stat s;
	if (stat(Firmware.c_str(),&s))
	{
		printf("%s\n","Firmware not found!");
		return EXIT_WITH_ERROR;
	}
	if (!globalArgs.massErase) MassErase(theBSLConnection);
	if (!globalArgs.newBsl) LoadBSL (theBSLConnection); 

	printf("Loading firmware...");
	int i = theBSLConnection->loadFile(Firmware);
	printf("%s: %s \n",__func__ ,(i == EXIT_SUCCESS ? "OK" : "FAILED")); 

	int j =Run(theBSLConnection, DefaultAddr);

	return (i & j); 
}

void Reset(int time)
{
	FILE* file = fopen ("/sys/class/gpio/gpio93/value","w");
	if (file != NULL)
	{
		printf("%s\n", "Reseting...");
		char buf1[] = "1\n";
		char buf0[] = "0\n";
		fwrite(buf0,sizeof(buf1),1,file);
		fflush(file);
		sleep(time);
		fwrite(buf1,sizeof(buf1),1,file);
		fflush(file);
		sleep(time);
		fclose (file);		
	}
	else printf("%s\n","Not found reset gpio!");
}

void ArgvInit()
{
	globalArgs.version = 0;
	globalArgs.resetTime= 0;
	globalArgs.massErase = FALSE;
	globalArgs.inFile = "";
	globalArgs.newBsl = FALSE;
	globalArgs.help = TRUE;
}

void ArgvParser(int argc, char* argv[])
{
	int opt = 0;
	ArgvInit();
	
	opt = getopt(argc, argv, optString);

	while (opt != LAST_OPTION) 
	{
		switch (opt) {
			case 'v':
			globalArgs.version = TRUE;
			globalArgs.help = FALSE;
			break;

			case 'm':
			globalArgs.massErase = TRUE;
			globalArgs.help = FALSE;
			break;

			case 'b':
			globalArgs.newBsl = TRUE;
			globalArgs.help = FALSE;
			break;

			case 'o':
			globalArgs.inFile = optarg;
			globalArgs.help = FALSE;
			break;

			case 'r':
			globalArgs.resetTime = atoi(optarg);
			globalArgs.help = FALSE;
			break;

			default:
			break;
		}

		opt = getopt( argc, argv, optString );
	}

}

int main(int argc, char *argv[])
{
	ArgvParser(argc,argv);

	if (globalArgs.resetTime != 0)
	{ 
		Reset(globalArgs.resetTime);
		return EXIT_SUCCESS;
	}

	if (globalArgs.help)
	{
		printf("%s\n","-m MassErase");
		printf("%s\n", "-r [time] Reset");
		printf("%s\n","-o [file] MassErase & LoadFirmware & Run" );
		printf("%s\n", "-b LoadBSL");
		printf("%s\n", "-v VersionBSL");
		return EXIT_SUCCESS;
	}

	MSPBSL_Connection5xxUSB *theBSLConnection = OpenConnection();
	if (theBSLConnection == NULL)
	{
		printf("MSP not found!\n");
		printf("Use -r for reset it\n");
		return EXIT_WITH_ERROR;
	}
	//CheckPassword(theBSLConnection);

	if (globalArgs.newBsl) LoadBSL(theBSLConnection);

	if (globalArgs.massErase) MassErase(theBSLConnection);

	if (globalArgs.version) PrintBSLVersion(theBSLConnection);

	if (!(globalArgs.inFile == "")) LoadFirmware(theBSLConnection,globalArgs.inFile);

	CloseConnetion(theBSLConnection);
	return EXIT_SUCCESS;
}
