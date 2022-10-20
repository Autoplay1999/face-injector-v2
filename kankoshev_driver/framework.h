#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX

#include "phnt/phnt_windows.h"
#include "phnt/phnt.h"

#include <memory>    // shared_ptr, exception
#include <string>    // wstring
#include <random>    // mt19937, random_device, uniform_int_distribution, numeric_limits
#include <strsafe.h> // StringCchCopyW
#include <shellapi.h>

#include "xor.h"

using namespace std;