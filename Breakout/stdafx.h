#ifndef STDAFX_H
#define STDAFX_H

#ifdef _WIN32
#pragma once
#endif

#include <cstdio>
#include <cassert>
#include <cmath>
#include <cfloat>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>

#ifdef _MSC_VER
#pragma warning(disable : 4100) // warning C4100: 'hwnd' : unreferenced formal parameter
#endif

//#define DEBUG_BUTTONS

typedef sf::Vector2f Vector;

#endif // STDAFX_H
