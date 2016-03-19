#ifndef STDAFX_H
#define STDAFX_H

#ifdef _WIN32
#pragma once
#endif

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
