#include "define/stdafx.h"
#include "api/xor.h"
#include "api/api.h"
#include "driver/driver.h"
#include "inject/injector.h"
#include "api/drvutils.h"

int main()
{
	// driver init
	if (!start_driver()) {
		system("pause");
		return 1;
	}

	cout << endl;

	// injector (Respawn001 - apex legends window class name)
	if (!face_injecor_v2(xor_a("D3D Window"), xor_a("CABAL"), xor_w(L"test.dll"))) {
		system("pause");
		return 2;
	}

	cout << endl;
	system("pause");
	return 0;
}