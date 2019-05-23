#include "myutils.h"
#include <sys/stat.h>
using namespace std;
#include <sstream>

// convert a double value to string
std::string DoubleToString(double val)
{
    std::ostringstream out;
    out << val;
    return out.str();
}

// convert a int value to string
std::string IntToString(int val)
{
    std::ostringstream out;
    out << val;
    return out.str();
}

// convert a string to double
double StringToDouble(string s)
{
  double d;  stringstream(s) >> d;  return(d);
}

// convert a string to int
int StringToInt(string s)
{
  int n;  stringstream(s) >> n;  return(n);
}

// Given a color code, return its name
string ColorName(int cor)
{
  switch (cor) {
  case WHITE: return("white");
  case BLACK: return("black");
  case RED: return("red");
  case GREEN: return("green");
  case BLUE: return("blue");
  case YELLOW: return("yellow");
  case MAGENTA: return("magenta");
  case CYAN: return("cyan");
  case GREY: return("grey");
  case ORANGE: return("grey");
  }
  printf("ERROR: Unknown color number %d in routine GetColor.\n",cor);
  exit(1);
}



// return file size
unsigned long FileSize(const std::string& filename)
{
  struct stat filestatus;
  stat( filename.c_str(), &filestatus );
  return(filestatus.st_size);
}
