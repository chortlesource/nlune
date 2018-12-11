# nLune

## A c++ ncurses interface for the lune calendar


![nlune running in terminal](gfx/nlune_screenshot.jpg?raw=true "nLune")


### Description

nLune is an ncurses interface for the [lune](https://github.com/chortlesource/lune) c++ port of Igor Chubin's pyphoon; providing a simple tool to display details about the current lunar phase in the terminal window. In addition the application displays ascii art that visually represents the current phase of the moon. The application also makes predictions based on the current lunar calendar for the anticipated future phases of the moon.

I have modified the [lune](https://github.com/chortlesource/lune) port in this version to act as a standalone class and so there are distinctions between `lune.hpp` and `lune.cpp` found here and that within the main [lune](https://github.com/chortlesource/lune) repository. These changes include moving the ascii art to `nlune.cpp` and removing the print functions.


### Installation
Typically I build this project as follows:
* git clone git@github.com:chortlesource/nlune.git
* cd ./nlune
* mkdir build && cd build
* cmake ..
* ./nlune

optionally you can specify `cmake .. -DCMAKE_BUILD_TYPE=Debug` instead of `cmake ..` if you are so inclined.


### Future features

* TODO: Parse user specified dates to show phases for specified period


### Copyright / License

Adapted from "moontool.c" by John Walker, Release 2.5 (See http://www.fourmilab.ch/moontool/)
and Pyphoon by Igor Chubin, 2016 (See https://github.com/chubin/pyphoon)

Ported to C++ by Christopher M. Short 2018

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above attribution notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
