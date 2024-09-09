#include "TestNormalEffect.h"
#include <windows.h>
#include <iostream>

void TestNormalEffect::Start()
{
	this->RespondOK();
	std::cout << "STARTED"; //MessageBox(NULL, L"Hello", L"Yo", 0);
}

void TestNormalEffect::Stop()
{
	//MessageBox(NULL, L"Stopped", L"Yolo", 0);
	std::cout << "STOPPED";
}
