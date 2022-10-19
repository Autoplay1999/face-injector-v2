#pragma once

bool start_driver()
{
	driver().handle_driver();

	if (!driver().is_loaded())
	{
		cout << xor_a("driver initialize...") << endl;
		mmap_driver();
	}

	driver().handle_driver();

	if (!driver().is_loaded()) {
		cout << xor_a("driver initialize error =<") << endl;
		return false;
	}

	cout << xor_a("driver initialized!") << endl;

	return true;
}

