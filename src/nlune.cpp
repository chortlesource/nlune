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

#include <nlune.hpp>

// ------- Static Constants

static const float moon_aspect = 0.5;
static const std::vector<std::string> moon_ascii {
    "             .----------.            ",
    "         .--'   o    .   `--.        ",
    "       .'@  @@@@@@ O   .   . `.      ",
    "     .'@@  @@@@@@@@   @@@@   . `.    ",
    "   .'    . @@@@@@@@  @@@@@@    . `.  ",
    "  / @@ o    @@@@@@.   @@@@    O   @\\ ",
    "  |@@@@               @@@@@@     @@| ",
    " / @@@@@   `.-.    . @@@@@@@@  .  @@\\ ",
    " | @@@@   --`-'  .  o  @@@@@@@      |",
    " |@ @@                 @@@@@@ @@@   |",
    " \\      @@    @   . ()  @@   @@@@@  /",
    "  |   @      @@@         @@@  @@@  | ",
    "  \\  .   @@  @\\  .      .  @@    o / ",
    "   `.   @@@@  _\\ /     .      o  .'  ",
    "     `.  @@    ()---           .'    ",
    "       `.     / |  .    o    .'      ",
    "         `--./   .       .--'        ",
    "             `----------'            "
};


// ------- Public nLune Implementation

void nLune::initialize() {
  // Initialize and configure ncurses
  initscr();
  noecho();
  cbreak();
  curs_set(0);

  // Check the terminal for colours
  if(has_colors() == false) {
    endwin();
    std::cerr << "[ERROR]: This terminal does not support colours.\n" << std::endl;
    return;
  }

  start_color();
  init_pair(2, COLOR_BLACK, COLOR_WHITE);

  // Configure the window and it's variables
  getmaxyx(stdscr, height, width);
  keypad(stdscr, true);

  // Configure border variables
  min_x = 2;
  max_x = width - 2;
  min_y = 2;
  max_y = height - 2;

  initialized = true;
}


void nLune::execute() {
  if(!initialized)
    return;

  while(initialized) {
    // Reset our display
    calculateResize();    // Recalculate border variables
    clear();              // Clear the window

    // Write the relevant information to the back buffer
    printBorder();
    printData();
    printMoon();

    int opt = getch();

    switch(opt) {
      case CTRL_KEY('x'):
        initialized = false;
        break;
      default:
        break;
    }
  }
}


void nLune::finalize() {
  // End the ncurses session
  endwin();
}


// ------- Private nLune Implementation


void nLune::printBorder() {
  if(!initialized)
    return;

  const char padding = ' ';   // Padding character

  // Draw our border in accordance with current variables
  wattron(stdscr, COLOR_PAIR(2));
  for(int col = min_x; col < max_x; col++) {
    for(int row = min_y; row < max_y; row++)
      mvwaddch(stdscr, row, col, padding);
  }
  wattroff(stdscr, COLOR_PAIR(2));

  // Draw our banner and footer
  const std::string banner = " | nLune - 0.01-BETA | ";
  const std::string footer = "[ PRESS CTRL + X TO EXIT ]";

  mvwprintw(stdscr, min_y - 1, max_x - banner.size(), banner.c_str());
  mvwprintw(stdscr, max_y, min_x + 1, footer.c_str());

}


void nLune::printData() {
  if(!initialized)
    return;

  // Format our data for output
  std::vector<std::string> phases = moon.getNextPhases();
  std::string date = std::string("Date: ") + moon.getDate();
  std::string jdate = std::string("Julian Date: ") + std::to_string(moon.getJulianDate());
  std::string cphase = moon.getPhaseString();
  std::string nmoon = std::string("New Moon: ") + phases[0];
  std::string fstmoon = std::string("First Quarter Moon: ") + phases[1];
  std::string fmoon = std::string("Full Moon: ") + phases[2];
  std::string lstmoon = std::string("Last Quarter Moon: ") + phases[3];
  std::string nxtmoon = std::string("Next New Moon: ") + phases[4];

  // Print the data to screen
  wattron(stdscr, COLOR_PAIR(2));

  mvwprintw(stdscr, min_y + 2, min_x + 1, cphase.c_str());
  mvwprintw(stdscr, min_y + 3, min_x + 1, date.c_str());
  mvwprintw(stdscr, min_y + 4, min_x + 1, jdate.c_str());

  mvwprintw(stdscr, min_y + 7, min_x + 1, nmoon.c_str());
  mvwprintw(stdscr, min_y + 9, min_x + 1, fstmoon.c_str());
  mvwprintw(stdscr, min_y + 11, min_x + 1, fmoon.c_str());
  mvwprintw(stdscr, min_y + 13, min_x + 1, lstmoon.c_str());
  mvwprintw(stdscr, min_y + 15, min_x + 1, nxtmoon.c_str());

  wattroff(stdscr, COLOR_PAIR(2));
}


void nLune::printMoon() {
  if(!initialized)
    return;

  // Figure out the phase
  float angphase = moon.getPhase() * 2.0 * M_PI;
  float mcap = -std::cos(angphase);

  // Figure out the size of the moon
  float yrad = moon_ascii.size() / 2.0;
  float xrad = yrad / moon_aspect;

  // Print out our moon one line at a time
  unsigned int line = 0;
  while(line < moon_ascii.size()) {
    // Compute the edges of this slice
    float y = line + 0.5 - yrad;
    float xright = xrad * std::sqrt(1.0 - (y * y) / (yrad * yrad));
    float xleft = -xright;

    if((angphase >= 0.0) && (angphase < M_PI))
      xleft = mcap * xleft;
    else
      xright = mcap * xright;

    int colleft = int(xrad + 0.5) + int(xleft + 0.5);
    int colright = int(xrad + 0.5) + int(xright + 0.5);

    // Now output the slice
    const char padding = ' ';   // Padding character
    int col = 0;

    // Print padded chars to screen
    while(col < colleft) {
      wattron(stdscr, COLOR_PAIR(2));
      mvwaddch(stdscr, (max_y - moon_ascii.size() - 1) + line, (max_x - moon_ascii[0].size() - 1) + col, padding);
      wattroff(stdscr, COLOR_PAIR(2));

      ++col;
    }

    // Print our ascii chars to screen
    while(col < colright) {
      wattron(stdscr, COLOR_PAIR(2));
      mvwaddch(stdscr, (max_y - moon_ascii.size() - 1) + line, (max_x - moon_ascii[0].size() - 1) + col, moon_ascii[line][col]);
      wattroff(stdscr, COLOR_PAIR(2));

      ++col;
    }

    ++line;
  }
}


void nLune::calculateResize() {
  if(!initialized)
    return;

  // Get the size of the screen
  getmaxyx(stdscr, height, width);

  // Reconfigure border variables
  min_x = 2;
  max_x = width - 2;
  min_y = 2;
  max_y = height - 2;
}


// ------- Main Function


int main(const int argc, const char *argv[]) {
  // Initialize our variables
  nLune moon;
  moon.initialize();

  // Run the application
  moon.execute();

  // Free relevant memory
  moon.finalize();

  return 0;
}
