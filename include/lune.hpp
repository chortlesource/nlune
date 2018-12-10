/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

LUNE - lune.hpp

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

#ifndef _LUNE_HPP
#define _LUNE_HPP


// ------- Includes

// C Library Includes
#include <ctime>
#include <cmath>

// C++ Library Includes
#include <iostream>
#include <string>
#include <vector>


// ------- Astronomical Constants

static const float epoch = 2444238.5;        // 1980 January 0.0


// ------- Constants Defining the Sun's apparent orbit

static const float s_elonge = 278.833540;      // Ecliptic longitude of the Sun at epoch 1980.0
static const float s_elongp = 282.596403;      // Ecliptic longitude of the Sun at perigee
static const float s_eccent = 0.016718;        // Eccentricity of earths orbit
static const float s_smax = 1.49585e8;      // Semi-major axis of Earth's orbit, in kilometers
static const float s_angsiz = 0.533128;     // Sun's angular size, in degrees, at semi-major axis distance


// ------- Elements of the Moon's Orbit

static const float m_mlong = 64.975464;       // Moon's mean longitude at the epoch
static const float m_mlongp = 349.383063;     // Mean longitude of the perigee at the epoch
static const float m_mlnode = 151.950429;      // Mean longitude of the node at the epoch
static const float m_inc = 5.145396;          // Inclination of the Moon's orbit
static const float m_mecc = 0.054900;          // Eccentricity of the Moon's orbit
static const float m_angsiz = 0.5181;         // Moon's angular size at distance a from Earth
static const float m_smax = 384401.0;         // Semi-mojor axis of the Moon's orbit, in kilometers
static const float m_parallax = 0.9507;       // Parallax at a distance a from Earth
static const float synmonth = 29.53058868;   // Synodic month (new Moon to new Moon), in days
static const float lunatbase = 2423436.0;    // Base date for E. W. Brown's numbered series of lunations (1923 January 16)


// ------- Properties of the Earth

static const float earthrad = 6378.16;       // Properties of the Earth


// ------- Other static variables used

static const float precision = 0.05;
static const float newmoon = 0 / 4.0;
static const float firstmoon = 1 / 4.0;
static const float fullmoon = 2 / 4.0;
static const float lastmoon = 3 / 4.0;
static const float nextmoon = 4 / 4.0;
static const float moon_aspect = 0.5;

static const std::vector<std::string> phase_label {
  "New Moon",
  "Waxing Crescent Moon",
  "First Quarter Moon",
  "Waxing Gibbous Moon",
  "Full Moon",
  "Waning Gibbous Moon",
  "Last Quarter Moon",
  "Waning Crescent Moon",
  "New Moon"
};

static const std::vector<std::string> moon_ascii {
  "                  .------------.                 ",
  "             .---' o     .  .   `---.            ",
  "          .-'   .    O    .       .  `-.         ",
  "        .'@   @@@@@@@   .   @@@@@       `.       ",
  "      .'@@  @@@@@@@@@@@    @@@@@@@   .    `.     ",
  "     /    o @@@@@@@@@@@    @@@@@@@       .  \\    ",
  "    /@  o   @@@@@@@@@@@.    @@@@@@@   O      \\   ",
  "   /@@@   .   @@@@@@@o     @@@@@@@@@@     @@@ \\  ",
  "  /@@@@@               .  @@@@@@@@@@@@@ o @@@@ \\ ",
  "  |@@@@  O  `.-./  .       @@@@@@@@@@@@    @@  | ",
  " / @@@@    --`-'       o      @@@@@@@@ @@@@     \\ ",
  " |@ @@@     @  `           .   @@     @@@@@@@   |",
  " |      @           o          @      @@@@@@@   |",
  " \\       @@            .-.      @@@    @@@@  o  /",
  "  | . @        @@@     `-'    . @@@@           | ",
  "  \\      @@   @@@@@ .            @@   .        / ",
  "   \\    @@@@  @\\@@    /  .   O    .     o   . /  ",
  "    \\ o  @@     \\ \\  /          .    .       /   ",
  "     \\     .    .\\.-.___    .      .   .-.  /    ",
  "      `.          `-'                 `-' .'     ",
  "        `.   o   / |      o    O   .    .'       ",
  "          `-.   /      .       .     .-'         ",
  "             `---.       .      .---'            ",
  "                  `------------'                 "
};


// ------- Useful mathematical functions

float fixedangle(float value) { return value - 360.0 * std::floor(value / 360.0); }
float todeg(float value) { return value * 180.0 / M_PI; }
float torad(float value) { return value * M_PI / 180.0; }
float dsin(float value) { return std::sin(torad(value)); }
float dcos(float value) { return std::cos(torad(value)); }


// ------- Moon Class

class Lune {
private:
  // Current time variables
  time_t current_time;
  int jdate;

  // Moon Phase Calculation Variables
  float m_phase;
  float m_illuminated;
  float m_age;
  float m_dist;
  float m_angdia;
  float s_dist;
  float s_angdia;

  // Moon Strings
  std::string m_string;
  std::vector<std::string> m_phases;

  // Phase Calculation functions
  void calculatePhase();
  float calculateMeanPhase(const int& jdn, const float& k);
  float calculateTruePhase(const float& k, const float& tphase);
  float calculateKepler(const float& m, const float& ecc);

  // Other Calculation functions
  void calculatePhaseString();
  void calculateNextPhase();

  // Calendar Calclation Functions
  void calculateJulianFromDate(const int& dd, const int& mm, const int& yyyy, int& jdn);
  void calculateJulianFromTime(time_t* t, int& jdn);
  void calculateGregorian(const int& jdn, int& dd, int& mm, int& yyyy);
  std::string calculateGregorianString(const int& jdn);

  // Math Calculation Functions
  float calculateFunc(const float& v);
  void bisect(const float& a, const float& b, float& c);
  void calculateRelativeDate(time_t *tin, time_t *tout, const int& days);

public:
  Lune();
  ~Lune() {}

  void print();
  void printLune();
};

#endif // _MOONAR_HPP
