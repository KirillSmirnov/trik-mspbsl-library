cmake_minimum_required(VERSION 2.8.1)

project(MSPBSL_Library)

list(APPEND CMAKE_MODULE_PATH "cmake")

# Find Boost
# We depend on the Boost serial port api
set(BOOST_USE_STATIC_LIBS       ON)
set(BOOST_USE_MULTITHREADED     ON)
set(BOOST_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS system thread)
if (NOT Boost_FOUND)
  message(SEND_ERROR "Boost library is required!")
  return()
endif()

# Add Boost header include path
include_directories(${Boost_INCLUDE_DIRS})
# Add Boost library path
link_directories(${Boost_LIBRARY_DIRS})

## Detect USB support
find_package(hidapi)

if (HIDAPI_FOUND)
  # hidapi found add usb souce files to the build
  message(STATUS "USB Supported")
  include_directories(${HIDAPI_INCLUDE_DIRS})
  # enable usb specific code in the Factory.
  add_definitions(-D MSPBSL_USB)
  set(USB_SRC 
      Connections/MSPBSL_Connection5xxUSB.cpp
      Physical_Interfaces/MSPBSL_PhysicalInterfaceUSB.cpp)
else(LIBUSB_FOUND)
  # hidapi not found
  message(STATUS "USB Not Supported")
endif()


include_directories(Connections Packet_Handlers Physical_Interfaces Utility_Classes)

add_library(msp430bsl SHARED
  MSPBSL_Factory.cpp
  Connections/MSPBSL_Connection.cpp
  Connections/MSPBSL_Connection5438Family.cpp
  Connections/MSPBSL_Connection5xx.cpp
  Connections/MSPBSL_Connection5xxUART.cpp
  Connections/MSPBSL_ConnectionFRAMFamily.cpp
  Packet_Handlers/MSPBSL_PacketHandler.cpp
  Packet_Handlers/MSPBSL_PacketHandler5xxUART.cpp
  Packet_Handlers/MSPBSL_PacketHandler5xxUSB.cpp
  Physical_Interfaces/MSPBSL_PhysicalInterfaceSerialUART.cpp
  Utility_Classes/MSPBSL_CRCEngine.cpp
  ${USB_SRC}
)

add_executable(example-bsl
               Example/main.cpp)

target_link_libraries(example-bsl msp430bsl ${HIDAPI_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} -lpthread -lm )

install(TARGETS msp430bsl DESTINATION ${LIB_INSTALL_PATH})

