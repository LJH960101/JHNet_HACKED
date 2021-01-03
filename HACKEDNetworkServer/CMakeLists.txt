cmake_minimum_required(VERSION 3.14)
PROJECT(JHNET)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Set GCC, G++ Setting
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILET "g++")

SET (SRCS
	JHNETGameServer.cpp
	JHNETGameServer.h
	
	NetworkModule/JHNETTool.cpp
	NetworkModule/JHNETTool.h
	NetworkModule/Log.cpp
	NetworkModule/Log.h
	NetworkModule/NetworkData.h
	NetworkModule/PacketStruct.h
	NetworkModule/PacketStructMacro.h
	NetworkModule/Serializer.cpp
	NetworkModule/Serializer.h
	NetworkModule/UtilityMacro.h
	NetworkModule/Spinlock.cpp
	NetworkModule/Spinlock.h
	NetworkModule/MPSCQueue.cpp
	NetworkModule/MPSCQueue.h
	NetworkModule/MessageQueue.h
	
	NetworkModule/ConfigParser/ConfigParser.cpp
	NetworkModule/ConfigParser/ConfigParser.h
	
	Processor/GameProcessor.cpp
	Processor/GameProcessor.h
	Processor/GameProcessorMessage.h
	
	Processor/Manager/PlayerManager.cpp
	Processor/Manager/PlayerManager.h
	Processor/Manager/RoomManager.cpp
	Processor/Manager/RoomManager.h
	
	Processor/DataType/GameState.h
	Processor/DataType/ObjectPool.h
	Processor/DataType/Overlapped.h
	Processor/DataType/Player.cpp
	Processor/DataType/Player.h
	Processor/DataType/Room.cpp
	Processor/DataType/Room.h
	Processor/DataType/Socket.cpp
	Processor/DataType/Socket.h
	
	Server/EpollServer.cpp
	Server/EpollServer.h
	Server/IOCPServer.cpp
	Server/IOCPServer.h
	Server/ServerNetworkSystem.cpp
	Server/ServerNetworkSystem.h
	
	Processor/TCP/TCPReceiveProcessor.cpp
	Processor/TCP/TCPReceiveProcessor.h
	
	Processor/UDP/UDPReceiveProcessor.cpp
	Processor/UDP/UDPReceiveProcessor.h
)

#Create Visual Studio Directories
foreach(FILE ${SRCS}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Source Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Header Files\\${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()
	
#Inclue Headers
INCLUDE_DIRECTORIES(include .)

#Add Files
ADD_EXECUTABLE(JHNET ${SRCS})

#Enable Thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(JHNET Threads::Threads)