/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

NLUNE - nlune.hpp

Adapted from "moontool.c" by John Walker, Release 2.5 (See http://www.fourmilab.ch/moontool/)
and Pyphoon by Igor Chubin, 2016 (See https://github.com/chubin/pyphoon)

Ported to C++ by Christopher M. Short 2018

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above attribution notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _NLUNE_HPP
#define _NLUNE_HPP


#define CTRL_KEY(x) ((x) & 0x1f)


// ------- Includes

// C Library Includes
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <ncurses.h>

// C++ Library Includes
#include <iostream>
#include <string>
#include <vector>

// Local Includes
#include <lune.hpp>


// ------- nLune class

class nLune {
private:
  bool initialized;
  //WINDOW *window;
  Lune moon;

  // nLune Terminal Variables
  int height;
  int width;

  // nLune Border Variables
  int min_x;
  int max_x;
  int min_y;
  int max_y;

  // Private Functions
  void printBorder();
  void printData();
  void printMoon();

  void calculateResize();

  // Convenience Functions
  void refresh() { wrefresh(stdscr); }

public:
  nLune() {}
  ~nLune() {}

  // Public Functions
  void initialize();
  void execute();
  void finalize();

};


#endif // _NLUNE_HPP
