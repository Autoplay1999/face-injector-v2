#include "define/stdafx.h"
#include <kankoshev/driver.hpp>
#include "api/xor.h"
#include "inject/injector.h"

bool start_driver() {
	auto& drv = kankoshev::Driver::GetInstance();

	if (!drv.AttachDriver()) {
		cout << xor_a("driver initialize...") << endl;
		
		if (!drv.DriverInitialize()) {
			cout << xor_a("driver initialize error =<") << endl;
			return false;
		}
	}

	if (!drv.AttachDriver()) {
		cout << xor_a("driver initialize error =<") << endl;
		return false;
	}

	cout << xor_a("driver initialized!") << endl;
	return true;
}

int main() {
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