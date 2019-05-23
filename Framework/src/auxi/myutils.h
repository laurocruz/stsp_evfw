#ifndef MYUTILS_DEFINE
#define MYUTILS_DEFINE
#include <string>
#include <iostream>  
#include <stdio.h>
#include <stdlib.h> 

#define EPS 0.001
using namespace std;

// convert a double value to string
std::string DoubleToString(double val);

// convert a string to double
double StringToDouble(string s);

// convert a string to int
int StringToInt(string s);

// convert a int value to string
std::string IntToString(int val);
inline void Pause(void) {cout<<"Pause";std::cin.get();cout<<"\n";}

// colors must match the ColorName in myutils.cpp
typedef enum Color {NOCOLOR,WHITE,BLACK,RED,GREEN,BLUE,YELLOW,MAGENTA,CYAN,GREY,ORANGE} Color;

// Given a color code, return its name
std::string ColorName(int cor);

#endif
