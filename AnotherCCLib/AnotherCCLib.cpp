// AnotherCCLib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CCConnector.h"
#include "AnotherCCLib.h"
#include <regex>
#include "CCEffect.h"
#include "TestNormalEffect.h"
#include <chrono>
#include <thread>
using namespace std;

// Todo:
// Handle disconnections
// Memory leaks
// Stress tests
// Logging callbacks
// Respond to status messages
// Write tests
// Responses for *failure* and simple blocking.
// Respect the 'stop all effects of type %s' message.
// Write documentation

int main()
{
    CCConnector* connector = new CCConnector();
    auto lambda = []() {return new TestNormalEffect(); };
    connector->AddEffect(new std::regex("playsound.*"), []() {return (CCEffect*) new TestNormalEffect(); });
    connector->AddEffect(new std::regex("controlsfasttraveloff_0_60"), []() {return (CCEffect*) new TestNormalEffect(); });
    
    std::cout << "Starting to connect" << std::endl;
    while (true) {
        if (connector->Connect()) {
            break;
        } else {
            if (connector->hasError)
            {
                std::cout << connector->error << std::endl;
            }
            std::this_thread::sleep_for(500ms);
        }
        
    }
    std::cout << "Connected!" << std::endl;
    connector->StartTimerThread();
    
    while (true) {
        std::this_thread::sleep_for(500ms);
    }

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
