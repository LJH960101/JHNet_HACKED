#include "Server/ServerNetworkSystem.h"
#include "Processor/UDP/UDPReceiveProcessor.h"
#include "Processor/Manager/RoomManager.h"
#include "Processor/Manager/PlayerManager.h"
#include "NetworkModule/Log.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/JHNETTool.h"
#include <thread>
#include <iostream>
#include "NetworkModule/PacketStruct.h"
#include <conio.h>
void ClearScreen()
{
#ifdef _WIN32
	system("cls");
#elif __linux__
	system("clear");
#endif
}

int main()
{
	CLog::WriteLog(ELogType::NetworkManager, ELogLevel::Warning, CLog::Format("Game Server Start"));
	CServerNetworkSystem* ServerNetworkSystem = CServerNetworkSystem::GetInstance();
	std::chrono::seconds sleepDuration(1);

	try
	{
		if (!ServerNetworkSystem->Run()) {
			std::cout << "FAIL!!";
			throw;
		}
		std::cout << "Server is now on playable.\n";

		while (true) {
			//	Windows Debug Only
			if (_kbhit()) {
				char a = _getch();
				if (a == 'c') {
					ClearScreen();
				}
				else if (a == 'p') {
					ServerNetworkSystem->Print();
				}
				else if (a == 's') {
					ServerNetworkSystem->PrintSummary();
				}
				else if (a == 'o') {
					printf("ObjectPools:\n");
					ServerNetworkSystem->PrintObjectPools();
				}
				fflush(stdin);
			}

			// Print Room Data
#if DEBUG_RECV_MSG
			std::this_thread::sleep_for(sleepDuration);
#else
			// Check Server State
			if (!ServerNetworkSystem->IsRun()) {
				throw;
			}

#if !CUSTOM_DEBUG_MODE
			ServerNetworkSystem->PrintSummary();
			std::this_thread::sleep_for(sleepDuration);
			ClearScreen();
#endif

#endif // DEBUG_RECV_MSG
		}
	}
	catch (const std::exception& e)
	{
		CLog::WriteLog(ELogType::NetworkManager, ELogLevel::Critical, CLog::Format("Game Server End by ERROR: %s", e.what()));
		CLog::Join();
	}
	CLog::Join();

	CLog::WriteLog(ELogType::NetworkManager, ELogLevel::Warning, CLog::Format("Game Server End Successfully."));
	
	return 0;
}