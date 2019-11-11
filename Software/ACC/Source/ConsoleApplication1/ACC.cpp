// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <winsock2.h> 


#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include "SharedFileOut.h"
#pragma optimize("",off)
using namespace std;


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")




template <typename T, unsigned S>
inline unsigned arraysize(const T(&v)[S])
{
	return S;
}

struct vehicle_info_s {
	int speed;
	int rpm;
	int gear;
	int tc;
	int map;
	int abs;
	int fuel;
	int fuel_lap;
	int toil;
	int tmot;
	int pit_limiter;
	int balance;
	int low_fuel;
	int delta;
}; typedef struct vehicle_info_s vehicle_info;

struct SMElement
{
	HANDLE hMapFile;
	unsigned char* mapFileBuffer;
};

SMElement m_graphics;
SMElement m_physics;
SMElement m_static;

void initPhysics()
{
	TCHAR szName[] = TEXT("Local\\acpmf_physics");
	m_physics.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SPageFilePhysics), szName);
	if (!m_physics.hMapFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
	m_physics.mapFileBuffer = (unsigned char*)MapViewOfFile(m_physics.hMapFile, FILE_MAP_READ, 0, 0, sizeof(SPageFilePhysics));
	if (!m_physics.mapFileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
}

void initGraphics()
{
	TCHAR szName[] = TEXT("Local\\acpmf_graphics");
	m_graphics.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SPageFileGraphic), szName);
	if (!m_graphics.hMapFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
	m_graphics.mapFileBuffer = (unsigned char*)MapViewOfFile(m_graphics.hMapFile, FILE_MAP_READ, 0, 0, sizeof(SPageFileGraphic));
	if (!m_graphics.mapFileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
}

void initStatic()
{
	TCHAR szName[] = TEXT("Local\\acpmf_static");
	m_static.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SPageFileStatic), szName);
	if (!m_static.hMapFile)
	{
		MessageBoxA(GetActiveWindow(), "CreateFileMapping failed", "ACCS", MB_OK);
	}
	m_static.mapFileBuffer = (unsigned char*)MapViewOfFile(m_static.hMapFile, FILE_MAP_READ, 0, 0, sizeof(SPageFileStatic));
	if (!m_static.mapFileBuffer)
	{
		MessageBoxA(GetActiveWindow(), "MapViewOfFile failed", "ACCS", MB_OK);
	}
}

void dismiss(SMElement element)
{
	UnmapViewOfFile(element.mapFileBuffer);
	CloseHandle(element.hMapFile);
}

void printData(string name, float value)
{
	wcout << name.c_str() << " : " << value << endl;
}

template <typename T, unsigned S>
inline void printData(const string name, const T(&v)[S])
{
	wcout << name.c_str() << " : ";
    
    for (int i = 0; i < S; i++)
    {
        wcout << v[i];
        if (i < S - 1)
        {
            wcout << " , ";
        }

    }
	wcout << endl;
}

template <typename T, unsigned S, unsigned S2>
inline void printData2(const string name, const T(&v)[S][S2])
{
    wcout << name.c_str() << " : ";

    for (int i = 0; i < S; i++)
    {
        wcout << i << " : ";
        for (int j = 0; j < S2; j++) {
            wcout << v[i][j];
            if (j < S2 - 1)
            {
                wcout << " , ";
            }
        }

        wcout << ";" << endl;
       
    }

}

int _tmain(int argc, _TCHAR* argv[])
{
	initPhysics();
	initGraphics();
	initStatic();

	int int_tmp;
	vehicle_info vi;

	//VI INIT
	vi.gear = -2;
	vi.rpm = -1;
	vi.speed = -1;
	vi.tc = -1;
	vi.map = -2;
	vi.abs = -1;
	vi.fuel = -1;
	vi.fuel_lap = -1;
	vi.toil = -1;
	vi.tmot = -1;
	vi.pit_limiter = -1;
	vi.balance = -1;
	vi.low_fuel = 0;
	vi.delta = -1000;
	//

	WSADATA WSAData;

	SOCKET server, client;

	SOCKADDR_IN serverAddr, clientAddr;

	WSAStartup(MAKEWORD(2, 0), &WSAData);
	server = socket(AF_INET, SOCK_STREAM, 0);

	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(22005);

	bind(server, (SOCKADDR*)& serverAddr, sizeof(serverAddr));

	int is_alive;

	cout << "Listening for incoming connections..." << endl;

	char buffer[1024];
	int clientAddrSize = sizeof(clientAddr);

	while (1)
	{
		listen(server, 0);
		if ((client = accept(server, (SOCKADDR*)& clientAddr, &clientAddrSize)) != INVALID_SOCKET)
		{
			cout << "Client connected!" << endl;
			is_alive = 1;
			//tgear and toil have constant values in ACC

			//OIL TEMP 67°C
			sprintf_s(buffer, "767\r\n");
			send(client, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);
			while (is_alive != -1)
			{
				SPageFilePhysics* pf = (SPageFilePhysics*)m_physics.mapFileBuffer;
				SPageFileGraphic* pf2 = (SPageFileGraphic*)m_graphics.mapFileBuffer;

				//CHECK CONNECTION
				sprintf_s(buffer, "0\r\n");
				is_alive = send(client, buffer, sizeof(buffer), 0);

				//RPM
				int_tmp = (int)(pf->rpms);
				if (vi.rpm != int_tmp)
				{
					vi.rpm = int_tmp;
					sprintf_s(buffer, "1%d\r\n", int_tmp);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//SPEED
				int_tmp = (int)(pf->speedKmh);
				if (vi.speed != int_tmp)
				{
					vi.speed = int_tmp;
					sprintf_s(buffer, "2%d\r\n", int_tmp);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//GEAR
				int_tmp = (int)(pf->gear);
				if (vi.gear != int_tmp)
				{
					vi.gear = int_tmp;
					if (vi.gear == 0)
						sprintf_s(buffer, "3R\r\n");
					else
						sprintf_s(buffer, "3%i\r\n", int_tmp - 1);
					printf(buffer);
					send(client, buffer, sizeof(buffer), 0);
				}

				//TC
				int_tmp = (int)(pf2->TC);
				if (vi.tc != int_tmp)
				{
					vi.tc = int_tmp;
					sprintf_s(buffer, "d%d\r\n", int_tmp);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//MAP
				int_tmp = (int)(pf2->EngineMap);
				if (vi.map != int_tmp)
				{
					vi.map = int_tmp;
					sprintf_s(buffer, "e%d\r\n", int_tmp + 1);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//ABS
				int_tmp = (int)(pf2->ABS);
				if (vi.abs != int_tmp)
				{
					vi.abs = int_tmp;
					sprintf_s(buffer, "f%d\r\n", int_tmp);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//FUEL
				int_tmp = (int)(pf->fuel * 10);
				if (vi.fuel != int_tmp)
				{
					vi.fuel = int_tmp;
					sprintf_s(buffer, "4%.1f\r\n", pf->fuel);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//FUEL PER LAP
				int_tmp = (int)(pf2->fuelXLap * 10);
				if (vi.fuel_lap != int_tmp)
				{
					vi.fuel_lap = int_tmp;
					sprintf_s(buffer, "5%.1f\r\n", pf2->fuelXLap);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//WATER TEMP
				int_tmp = (int)(pf->waterTemp);
				if (vi.tmot != int_tmp)
				{
					vi.tmot = int_tmp;
					sprintf_s(buffer, "8%d\r\n", int_tmp);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//PIT LIMITER
				int_tmp = pf->pitLimiterOn;
				if (vi.pit_limiter != int_tmp)
				{
					vi.pit_limiter = int_tmp;
					sprintf_s(buffer, "9%d\r\n", int_tmp);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				//BRAKE BALANCE
				int_tmp = (int)(pf->brakeBias * 1000);
				if (vi.balance != int_tmp && int_tmp > 0)
				{
					vi.balance = int_tmp;
					sprintf_s(buffer, "a%.1f\r\n", (pf->brakeBias * 100.0) - 14);
					send(client, buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}

				if (vi.fuel <= 50 && !vi.low_fuel)
				{
					vi.low_fuel = 1;
					sprintf_s(buffer, "b1\r\n");
					printf(buffer);
					send(client, buffer, strlen(buffer), 0);
				}
				else if (vi.fuel > 50 && vi.low_fuel)
				{
					vi.low_fuel = 0;
					sprintf_s(buffer, "b0\r\n");
					printf(buffer);
					send(client, buffer, strlen(buffer), 0);
				}

				Sleep(200);
			}
		}
		closesocket(client);
		cout << "Client disconnected." << endl;
	}

	dismiss(m_graphics);
	dismiss(m_physics);
	dismiss(m_static);

	return 0;
}


