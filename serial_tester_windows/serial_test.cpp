#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "triple_temperature.h"

std::atomic_bool is_signaled_interrupt;

TripleTemperature tt;

static const char *error_not_open = "Error: Device not connected. Use \"open\" to open device.";

void get_status();
void get_temperature();
void open_device();
void close_device();
void poll();
void show_help();

void signal_handler(int signal);

int wmain(int argc, const wchar_t **argv)
{
    std::signal(SIGINT, signal_handler);

    std::wcout << "Triple Temperature Serial Tester. Type \"help\" to see commands." << std::endl;

    std::wstring command;

    for (;;)
    {

        std::wcout << ">";
        std::wcin >> command;

        if (command == L"status" || command == L"s")
        {
            get_status();
        }
        else if (command == L"temperature" || command == L"t")
        {
            get_temperature();
        }
        else if (command == L"open" || command == L"o")
        {
            open_device();
        }
        else if (command == L"close" || command == L"c")
        {
            close_device();
        }
        else if (command == L"poll" || command == L"p")
        {
            poll();
        }
        else if (command == L"help")
        {
            show_help();
        }
        else if (command == L"exit")
        {
            return 0;
        }
        else
        {
            std::wcout << "Unrecognized command. Type 'help' for a list of available commands." << std::endl;
        }
    }
}

void show_help()
{
    // clang-format off
    std::wcout << "Triple Temperature Serial Tester." << std::endl
        << "Commands: " << std::endl
        << "close           Close serial device. Shortcut 'c'." << std::endl
        << "exit            Exit program." << std::endl
        << "help            Show this help message." << std::endl
        << "open            Open communication with serial device. Shortcut 'o'." << std::endl
        << "poll            Poll device at a given interval. Device must be opened before using. Shortcut 'p'." << std::endl
        << "status          Send status request. Device must be opened before using. Shortcut 's'." << std::endl
        << "temperature     Send temperature request. Device must be opened before using. Shortcut 't'." << std::endl;
    // clang-format on
}

void get_status()
{
    using namespace std::chrono;

    if (!tt.is_open())
    {
        std::wcout << error_not_open << std::endl;
        return;
    }

    high_resolution_clock::time_point start = high_resolution_clock::now();
    StatusResult status;
    if (tt.get_status(status))
    {
        high_resolution_clock::time_point end = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(end - start);

        std::wcout << status;
        std::wcout << "Fetched in " << int(time_span.count() * 1000.0) << "ms" << std::endl;
    }
    else
    {
        std::wcout << "Failed to get status." << std::endl;
    }
}

void get_temperature()
{
    using namespace std::chrono;

    if (!tt.is_open())
    {
        std::wcout << error_not_open << std::endl;
        return;
    }

    high_resolution_clock::time_point start = high_resolution_clock::now();
    TemperatureResult temperature;
    if (tt.get_temperature(temperature))
    {
        high_resolution_clock::time_point end = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(end - start);

        std::wcout << temperature;

        std::wcout << "Fetched in " << int(time_span.count() * 1000.0) << "ms" << std::endl;
    }
    else
    {
        std::wcout << "Failed to get temperature." << std::endl;
    }
}

void open_device()
{
    if (tt.is_open())
    {
        std::wcout << "Error: Device is already open." << std::endl;
        return;
    }

    std::wcout << "Enter Port (Ex COM1): ";
    std::wstring port;
    std::wcin >> port;

    // Make this like \\.\ for windows to know it's serial device.
    port = L"\\\\.\\" + port;

    if (tt.connect(port))
    {
        std::wcout << "Connection established." << std::endl;
    }
    else
    {
        std::wcout << "Error: Connection failed." << std::endl;
    }
}

void close_device()
{
    if (!tt.is_open())
    {
        std::wcout << error_not_open << std::endl;
        return;
    }

    if (tt.close())
    {
        std::wcout << "Connection closed." << std::endl;
    }
    else
    {
        std::wcout << "Error: Connection failed to close." << std::endl;
    }
}

void poll()
{
    using namespace std::chrono;

    if (!tt.is_open())
    {
        std::wcout << error_not_open << std::endl;
        return;
    }

    std::wstring interval;
    std::wcout << "Enter poll interval (ms): ";
    std::wcin >> interval;
    long interval_ms = std::stol(interval);

    std::wcout << "Starting poll. Press ctrl + c to stop." << std::endl;

    unsigned long long count = 0;
    is_signaled_interrupt = false;

    for (;;)
    {
        if (is_signaled_interrupt)
        {
            break;
        }

        system("cls");
        std::wcout << "#" << count << ":" << std::endl;

        high_resolution_clock::time_point start = high_resolution_clock::now();
        TemperatureResult temperature;
        if (tt.get_temperature(temperature))
        {
            high_resolution_clock::time_point end = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(end - start);

            std::wcout << temperature;

            std::wcout << "Fetched in " << int(time_span.count() * 1000.0) << "ms" << std::endl;
        }
        else
        {
            std::wcout << "Failed to get temperature." << std::endl;
        }

        std::wcout << std::endl;
        ++count;

        std::this_thread::sleep_for(milliseconds(interval_ms));
    }
}

void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        is_signaled_interrupt = true;
    }
}
