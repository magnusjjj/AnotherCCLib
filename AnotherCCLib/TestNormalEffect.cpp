#include "TestNormalEffect.h"
#include <windows.h>

void TestNormalEffect::Start()
{
	MessageBox(NULL, L"Hello", L"Yo", 0);
}

void TestNormalEffect::Stop()
{
	MessageBox(NULL, L"Stopped", L"Yolo", 0);
}
