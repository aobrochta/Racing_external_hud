#include "r3e.h"
#include "utils.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <tchar.h>
#include <winsock2.h>
//#pragma comment(lib, "ws2_32.lib")
#include <string.h>

#define ALIVE_SEC 600
#define INTERVAL_MS 100


#define PORT 22005

HANDLE map_handle = INVALID_HANDLE_VALUE;
r3e_shared* map_buffer = NULL;
BOOL mapped_r3e = FALSE;

struct veh_info {
    int speed;
    int rpm;
    int gear;
    int fuel;
    int fuel_lap;
    int toil;
    int tmot;
    int pit_limiter;
    int balance;
    int low_fuel;
    int delta;
}; typedef struct veh_info veh_info;

char* convert_time_to_string(double time)
{
    char* res = (char*)malloc(10*sizeof(int));
    int minute = 0;
    int seconde;
    double ms;
    if(time<60.0)
    {
        seconde = (int)time;
    }
    else
    {
        minute = 1;
        while(time-(minute*60)>=60)
        {
            minute+=1;
        }
        seconde = (int)time - (minute*60);
    }
    ms = (time - (int)time)*100;
    sprintf(res,"%02d:%02d:%02.0f",minute,seconde,ms);
    return res;
}

HANDLE map_open()
{
    return OpenFileMapping(
        FILE_MAP_READ,
        FALSE,
        TEXT(R3E_SHARED_MEMORY_NAME));
}

BOOL map_exists()
{
    HANDLE handle = map_open();

    if (handle != NULL)
        CloseHandle(handle);

    return handle != NULL;
}

int map_init()
{
    map_handle = map_open();

    if (map_handle == NULL)
    {
        printf("Failed to open mapping");
        return 1;
    }

    map_buffer = (r3e_shared*)MapViewOfFile(map_handle, FILE_MAP_READ, 0, 0, sizeof(r3e_shared));
    if (map_buffer == NULL)
    {
        printf("Failed to map buffer");
        return 1;
    }

    return 0;
}

void map_close()
{
    if (map_buffer) UnmapViewOfFile(map_buffer);
    if (map_handle) CloseHandle(map_handle);
}

int start_server()
{
    WSADATA WSAData;
    SOCKET sock;
    SOCKET csock;
    SOCKADDR_IN sin;
    SOCKADDR_IN csin;
    WSAStartup(MAKEWORD(2,0), &WSAData);
    ////////
    veh_info vi;
    vi.speed=-1;
    vi.rpm=-1;
    vi.fuel=-1;
    vi.gear=-2;
    vi.fuel_lap=-1;
    vi.toil=-1;
    vi.tmot=-1;
    vi.pit_limiter=-1;
    vi.balance=-1;
    vi.low_fuel=0;
    vi.delta=-1000;
    ////////
    int int_tmp;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    bind(sock, (SOCKADDR *)&sin, sizeof(sin));
    listen(sock, 0);
    printf("Server start listening port %d\n", PORT);
    printf("Waiting for client...\n");
    char buffer[64];
    while(1) /* Boucle infinie. Exercice : améliorez ce code. */
    {
        int sinsize = sizeof(csin);
        if((csock = accept(sock, (SOCKADDR *)&csin, &sinsize)) != INVALID_SOCKET)
        {
            printf("Connection from client\n");
            Sleep(2000);
            while (mapped_r3e)
            {
                if (map_buffer->engine_rps > -1.f)
                {
                    int_tmp=(int)map_buffer->engine_rps * RPS_TO_RPM;
                    if(vi.rpm!=int_tmp)
                    {
                        vi.rpm=int_tmp;
                        sprintf(buffer,"1%d\r\n",int_tmp);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    int_tmp=(int)map_buffer->car_speed * MPS_TO_KPH;
                    if(vi.speed!=int_tmp)
                    {
                        vi.speed=int_tmp;
                        sprintf(buffer,"2%d\r\n", int_tmp);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    if(vi.gear!=map_buffer->gear)
                    {
                        vi.gear=map_buffer->gear;
                        if(vi.gear==-1)
                            sprintf(buffer,"3R\r\n");
                        else
                            sprintf(buffer,"3%i\r\n", vi.gear);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    int_tmp=(int)(map_buffer->fuel_left*10.0);
                    if(vi.fuel!=int_tmp)
                    {
                        vi.fuel=int_tmp;
                        sprintf(buffer,"4%.1f\r\n", map_buffer->fuel_left);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    int_tmp=(int)(map_buffer->fuel_per_lap*10);
                    if(vi.fuel_lap!=int_tmp)
                    {
                        vi.fuel_lap=int_tmp;
                        sprintf(buffer,"5%.1f\r\n", map_buffer->fuel_per_lap);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }
                    /*
                    sprintf(buffer,"6\"%s\"\r\n", convert_time_to_string(map_buffer->lap_time_current_self));
                    printf(buffer);
                    send(csock, buffer, strlen(buffer), 0);
                    */
                    int_tmp=(int)map_buffer->engine_oil_temp;
                    if(vi.toil!=int_tmp)
                    {
                        vi.toil=int_tmp;
                        sprintf(buffer,"7%d\r\n",vi.toil);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    int_tmp=(int)map_buffer->engine_water_temp;
                    if(vi.tmot!=int_tmp)
                    {
                        vi.tmot=int_tmp;
                        sprintf(buffer,"8%d\r\n",vi.tmot);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    int_tmp=map_buffer->pit_limiter;
                    if(vi.pit_limiter!=int_tmp)
                    {
                        vi.pit_limiter=int_tmp;
                        sprintf(buffer,"9%d\r\n",vi.pit_limiter);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    int_tmp=(int)round(100-(map_buffer->brake_bias*100.0));
                    if(vi.balance!=int_tmp)
                    {
                        vi.balance=int_tmp;
                        sprintf(buffer,"a%d,0\r\n",vi.balance);
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    if(vi.fuel<=50 && !vi.low_fuel)
                    {
                        vi.low_fuel=1;
                        sprintf(buffer,"b1\r\n");
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }
                    else if(vi.fuel>50 && vi.low_fuel)
                    {
                        vi.low_fuel=0;
                        sprintf(buffer,"b0\r\n");
                        printf(buffer);
                        send(csock, buffer, strlen(buffer), 0);
                    }

                    int_tmp=(int)(map_buffer->time_delta_best_self*100);
                    if(vi.delta!=int_tmp)
                    {
                        if(int_tmp==-10000)
                            sprintf(buffer,"c0\r\n");
                        else if(vi.delta!=1000 && int_tmp>=1000)
                        {
                            vi.delta=1000;
                            sprintf(buffer,"c10.0\r\n");
                            printf(buffer);
                            send(csock, buffer, strlen(buffer), 0);
                        }
                        else if(vi.delta!=-1000 && int_tmp<=-1000)
                        {
                            vi.delta=-1000;
                            sprintf(buffer,"c-10.0\r\n");
                            printf(buffer);
                            send(csock, buffer, strlen(buffer), 0);
                        }
                        else if(int_tmp < 1000 && int_tmp > -1000)
                        {
                            sprintf(buffer,"c%.2f\r\n",map_buffer->time_delta_best_self);
                            printf(buffer);
                            send(csock, buffer, strlen(buffer), 0);
                        }
                        if(int_tmp<1000 && int_tmp>-1000)
                            vi.delta=int_tmp;
                        //printf("delta=%d;tmp=%d;\n",vi.delta,int_tmp);
                    }
                }
                Sleep(150);
            }
            closesocket(csock);
            printf("Client disconnect\n");
        }

    }
    /* On devrait faire closesocket(sock); puis WSACleanup(); mais puisqu'on a entré une boucle infinie ... */
    return 0;
}

int main()
{
    clock_t clk_start = 0, clk_last = 0;
    clock_t clk_delta_ms = 0, clk_elapsed = 0;
    int err_code = 0;

    clk_start = clock();
    clk_last = clk_start;

    printf("Looking for RRRE.exe...\n");

    for(;;)
    {
        clk_elapsed = (clock() - clk_start) / CLOCKS_PER_SEC;
        if (clk_elapsed >= ALIVE_SEC)
            break;

        clk_delta_ms = (clock() - clk_last) / CLOCKS_PER_MS;
        if (clk_delta_ms < INTERVAL_MS)
        {
            Sleep(1);
            continue;
        }

        clk_last = clock();

        if (!mapped_r3e && is_r3e_running() && map_exists())
        {
            printf("Found RRRE.exe, mapping shared memory...\n");

            err_code = map_init();
            if (err_code)
                return err_code;

            printf("Memory mapped successfully\n");

            mapped_r3e = TRUE;
            clk_start = clock();
        }

        start_server();

        if (mapped_r3e)
        {
            if (map_buffer->gear > -2)
            {
                printf("Gear: %i\n", map_buffer->gear);
            }

            if (map_buffer->engine_rps > -1.f)
            {
                printf("RPM: %.0f\n", map_buffer->engine_rps * RPS_TO_RPM);
                printf("Speed: %.0f km/h\n", map_buffer->car_speed * MPS_TO_KPH);
            }

            if(map_buffer->pit_limiter > -1)
            {
                printf("Pit limiter: %i\n", map_buffer->pit_limiter);
            }
            if(map_buffer->lap_time_current_self > -1)
            {
                printf("Lap: %s\n", convert_time_to_string(map_buffer->lap_time_current_self));
            }
            if(map_buffer->time_delta_best_self > -1000.0)
            {
                printf("Delta: %.2f\n", map_buffer->time_delta_best_self);
                printf("Delta: %s\n", convert_time_to_string(map_buffer->time_delta_best_self));
            }
            if(map_buffer->engine_oil_temp > -1)
            {
                printf("toil: %.0f\n", map_buffer->engine_oil_temp);
            }
            if(map_buffer->engine_water_temp > -1)
            {
                printf("tmot: %.0f\n", map_buffer->engine_water_temp);
            }
            if(map_buffer->fuel_per_lap > -1)
            {
                printf("fuel_lap: %.1f\n", map_buffer->fuel_per_lap);
            }
            if(map_buffer->fuel_left > -1)
            {
                printf("fuel_tot: %.1f\n", map_buffer->fuel_left);
            }

                /*
            if (map_buffer->r3e_aid_settings->abs >= 0)
            {
                printf("Abs: %i\n", map_buffer->abs);
            }

            */
        }
    }

    map_close();

    printf("All done!");
    system("PAUSE");

    return 0;
}
