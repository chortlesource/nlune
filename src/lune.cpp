/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

NLUNE - lune.hpp

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
// static const float moon_aspect = 0.5;

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

/*static const std::vector<std::string> moon_ascii {
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
*/

// ------- Useful mathematical functions

static float fixedangle(float value) { return value - 360.0 * std::floor(value / 360.0); }
static float todeg(float value) { return value * 180.0 / M_PI; }
static float torad(float value) { return value * M_PI / 180.0; }
static float dsin(float value) { return std::sin(torad(value)); }
static float dcos(float value) { return std::cos(torad(value)); }


// ------- Lune Private Implementation

void Lune::calculatePhase() {

  //// SOLAR CALCULATIONS ////

  // Calculate the date within the epoch
  float day = jdate - epoch;

  // Calculate the mean anomaly of the Sun
  float n = fixedangle((350/365.2422) * day);
  // Convert from perigee coordinates to epoch 1980
  float m = fixedangle(n + s_elonge - s_elongp);

  // Solve Kepler's equation
  float ecc = calculateKepler(m, s_eccent);
  ecc = std::sqrt((1 + s_eccent) / (1 - s_eccent)) * std::tan(ecc/2.0);

  // True anomaly
  ecc = 2 * todeg(std::atan(ecc));

  // Suns's geometric eliptic longuitude
  float lambda_sun = fixedangle(ecc + s_elongp);

  // Orbital distance factor
  float f = (1 + s_eccent * std::cos(torad(ecc))) / (1 - std::pow(s_eccent, 2.0));

  // Distance to sun in km
  s_dist = s_smax / f;
  s_angdia = f * s_angsiz;


  //// LUNAR CALCULATIONS ////

  // Moon's mean longitude
  float moon_longitude = fixedangle(13.1763966 * day + m_mlong);

  // Moon's mean anomaly
  float mm = fixedangle(moon_longitude - 0.1114041 * day - m_mlongp);

  // Moon's ascending node mean longitude
  float evection = 1.2739 * std::sin(torad(2*(moon_longitude - lambda_sun) - mm));

  // Annual equation
  float annual_eq = 0.1858 * std::sin(torad(m));

  // Correction term
  float a3 = 0.37 * std::sin(torad(m));

  float mmp = mm + evection - annual_eq - a3;

  // Correction for the equation of the centre
  float mec = 6.2886 * std::sin(torad(mmp));

  // Another correction term
  float a4 = 0.214 * std::sin(torad(2 * mmp));

  // Corrected longitude
  float lp = moon_longitude + evection + mec - annual_eq + a4;

  // Variation
  float variation = 0.6593 * std::sin(torad(2*(lp - lambda_sun)));

  // True longitude
  float llp = lp + variation;

  // Age of the moon in degrees
  float moon_age = llp - lambda_sun;


  //// CACLUATE FINAL VARIABLES ////
  m_phase = fixedangle(moon_age) / 360.0;
  m_illuminated = (1 - std::cos(torad(moon_age))) / 2.0;
  m_age = synmonth * fixedangle(moon_age) / 360.0;

  // Calculate distance of the moon from the centre of the earth
  m_dist = (m_smax * (1 - std::pow(m_mecc, 2.0)))
      / (1 + m_mecc * std::cos(torad(mmp + mec)));

  // Calculate the moon's angular diameter
  float moon_diam_frac = m_dist / m_smax;
  m_angdia = m_angsiz / moon_diam_frac;
}


float Lune::calculateMeanPhase(const int& jdn, const float& k) {
  // Obtain time in Julian Centuries from 1900 January 1, 12:00
  int jcent;
  calculateJulianFromDate(1, 1, 1900, jcent);

  int delta = jdn - jcent;
  int t = delta / 36525;

  // Convenience Math
  int t2 = t * t;    // Square
  int t3 = t2 * t;   // Cube

  return (
        2415020.75933 + synmonth * k + 0.0001178 * t2 -
        0.000000155 * t3 + 0.00033 * dsin(166.56 + 132.87 * t -
        0.009173 * t2)
      );
}


float Lune::calculateTruePhase(const float& k, const float& tphase) {
  bool apcor = false;

  // Add phase to new moon time
  float k2 = k + tphase;

  // Time in julian centuries from 1900 January 0.5
  float t = k2 / 1236.85;

  // Convenience math
  float t2 = t * t;   // Square
  float t3 = t2 * t;  // cube

  // Mean time of phase
  float pt = (2415020.75933 + synmonth * k2 + 0.0001178 * t2 -
      0.000000155 * t3 + 0.00033 * dsin(166.56 + 132.87 * t -
      0.009173 * t2));

  // Sun's mean anomaly
  float m = 359.2242 + 29.10535608 * k2 - 0.0000333 * t2 - 0.00000347 * t3;

  // Moon's mean anomaly
  float mprime = 306.0253 + 385.81691806 * k2 + 0.0107306 * t2 + 0.00001236 * t3;

  // Moon's argument of latitude
  float f = 21.2964 + 390.67050646 * k2 - 0.0016528 * t2 - 0.00000239 * t3;

  if((tphase < 0.01) || (std::abs(tphase - 0.5) < 0.01)) {
    // Corrections for new and full moon_longitude
    pt += (
            (0.1734 - 0.000393 * t) * dsin(m)
            + 0.0021 * dsin(2 * m)
            - 0.4068 * dsin(mprime)
            + 0.0161 * dsin(2 * mprime)
            - 0.0004 * dsin(3 * mprime)
            + 0.0104 * dsin(2 * f)
            - 0.0051 * dsin(m + mprime)
            - 0.0074 * dsin(m - mprime)
            + 0.0004 * dsin(2 * f + m)
            - 0.0004 * dsin(2 * f - m)
            - 0.0006 * dsin(2 * f + mprime)
            + 0.0010 * dsin(2 * f - mprime)
            + 0.0005 * dsin(m + 2 * mprime)
          );
    apcor = true;
  }
  else if((std::abs(tphase - 0.25) < 0.01) || (std::abs(tphase - 0.75) < 0.01)) {
    pt += (
            (0.1721 - 0.0004 * t) * dsin(m)
            + 0.0021 * dsin(2 * m)
            - 0.6280 * dsin(mprime)
            + 0.0089 * dsin(2 * mprime)
            - 0.0004 * dsin(3 * mprime)
            + 0.0079 * dsin(2 * f)
            - 0.0119 * dsin(m + mprime)
            - 0.0047 * dsin(m - mprime)
            + 0.0003 * dsin(2 * f + m)
            - 0.0004 * dsin(2 * f - m)
            - 0.0006 * dsin(2 * f + mprime)
            + 0.0021 * dsin(2 * f - mprime)
            + 0.0003 * dsin(m + 2 * mprime)
            + 0.0004 * dsin(m - 2 * mprime)
            - 0.0003 * dsin(2 * m + mprime)
          );

    if(tphase < 0.5) {
      // First Quarter correction
      pt +=  + 0.0028 - 0.0004 * dcos(m) + 0.0003 * dcos(mprime);
    } else {
      // Last Quarter correction
      pt += -0.0028 + 0.0004 * dcos(m) - 0.0003 * dcos(mprime);

    }
    apcor = true;
  }

  if(!apcor) {
    std::cout << "[ERROR]: Truephase called with invalid phase selector.";
    return 0;
  }

  return pt;
}


float Lune::calculateKepler(const float& m, const float& ecc) {
  // Solve the Kepler equation
  float epsilon = 1e-6;

  float m2 = torad(m);
  float e = m2;

  while(1) {
    float delta = e - ecc * std::sin(e) - m2;
    e -= delta / (1 - ecc * std::cos(e));

    if(std::abs(delta) <= epsilon)
      break;
  }

  return e;
}


void Lune::calculatePhaseString() {
  // Our values for the precise phases of the moon
  float i;
  const std::vector<float> breakpoints {
    newmoon + precision,      // New Moon
    firstmoon - precision,    // Waxing Crescent Moon
    firstmoon + precision,    // First Quarter Moon
    fullmoon - precision,     // Waxing Gibbous Moon
    fullmoon + precision,     // Full Moon
    lastmoon - precision,     // Waning Gibbous Moon
    lastmoon + precision,     // Last Quarter Moon
    nextmoon - precision,     // Waning Crescent Moon
    nextmoon + precision,     // New Moon
  };

  // Calculate which string to display
  for(unsigned int index = 0; index < breakpoints.size(); index++) {
    if(index < 8) {
      // Bisect our breakpoints to obtain the precise midpoint
      bisect(breakpoints[index], breakpoints[index + 1], i);

      // If it is less than the bisected value it's the phase we want
      if(m_phase < i) {
        m_string = phase_label[index];
        return;
      } else {
        continue;
      }
    } else {
      m_string = phase_label[index];
    }
  }
}


void Lune::calculateNextPhase() {
  // Calculate our Julian Period
  time_t adate;
  calculateRelativeDate(&current_time, &adate, -45);

  // Obtain relevant information
  struct tm *timestamp = localtime(&adate);
  int Y = timestamp->tm_year;
  int M = timestamp->tm_mon + 1;

  // Calculate our synmonth
  int k1 = std::floor((Y + ((M - 1) * (1.0/12.0)) - 1900) * 12.3685);

  // Get the julian date number
  int ajdn;
  calculateJulianFromTime(&adate, ajdn);

  // Calculate the mean phase of the moon
  int nt1 = calculateMeanPhase(ajdn, k1);
  ajdn = nt1;

  while(1) {
    ajdn = ajdn + synmonth;
    int k2 = k1 + 1;
    int nt2 = calculateMeanPhase(ajdn, k2);
    if(nt1 <= jdate && jdate < nt2)
      break;
    nt1 = nt2;
    k1 = k2;
  }

  // Calculate the Julian Date number for each event

  float newmoon_jdn = calculateTruePhase(k1, newmoon);
  float firstmoon_jdn = calculateTruePhase(k1, firstmoon);
  float fullmoon_jdn = calculateTruePhase(k1, fullmoon);
  float lastmoon_jdn = calculateTruePhase(k1, lastmoon);
  float nextmoon_jdn = calculateTruePhase(k1 + 1, newmoon);

  m_phases.push_back(calculateGregorianString(newmoon_jdn));
  m_phases.push_back(calculateGregorianString(firstmoon_jdn));
  m_phases.push_back(calculateGregorianString(fullmoon_jdn));
  m_phases.push_back(calculateGregorianString(lastmoon_jdn));
  m_phases.push_back(calculateGregorianString(nextmoon_jdn));

}


// ------- Private Helper functions


void Lune::calculateJulianFromDate(const int& dd, const int& mm, const int& yyyy, int& jdn) {
  // Obtain relevant information
  int Y = yyyy;
  int M = mm;
  int D = dd;

  // Process into Julian time
  jdn = (1461 * (Y + 4800 + (M - 14)/12))/4 +(367 * (M - 2 - 12 * ((M - 14)/12)))/12 - (3 * ((Y + 4900 + (M - 14)/12)/100))/4 + D - 32075;
}


void Lune::calculateJulianFromTime(time_t* t, int& jdn) {
  struct tm *timestamp;
  timestamp = localtime(t);

  // Obtain relevant information
  int Y = timestamp->tm_year + 1900;
  int M = timestamp->tm_mon + 1;
  int D = timestamp->tm_mday;

  // Process into Julian time and return our julian day number
  jdn = (1461 * (Y + 4800 + (M - 14)/12))/4 +(367 * (M - 2 - 12 * ((M - 14)/12)))/12 - (3 * ((Y + 4900 + (M - 14)/12)/100))/4 + D - 32075;
}


void Lune::calculateGregorian(const int& jdn, int& dd, int& mm, int& yyyy) {
  // The Richards algorythm for converting Julian to Gregorian
  int f = int(jdn) + 1401 + (((4 * int(jdn) + 274277) / 146097) * 3) / 4 + -38;

  int e = 4 * f + 3;
  int g = (e % 1461) / 4;
  int h = 5 * g + 2;

  dd = (h % 153) / 5 + 1;
  mm = ((h /  153 + 2) % 12) + 1;
  yyyy = (e / 1461) - 4716 + (12 + 2 - mm) / 12;
}


std::string Lune::calculateGregorianString(const int& jdn) {
  // The Richards algorythm for converting Julian to Gregorian
  int f = int(jdn) + 1401 + (((4 * int(jdn) + 274277) / 146097) * 3) / 4 + -38;

  int e = 4 * f + 3;
  int g = (e % 1461) / 4;
  int h = 5 * g + 2;

  int dd = (h % 153) / 5 + 1;
  int mm = ((h /  153 + 2) % 12) + 1;
  int yyyy = (e / 1461) - 4716 + (12 + 2 - mm) / 12;

  std::string str = std::to_string(dd) + '/' + std::to_string(mm) + '/' + std::to_string(yyyy);
  return str;
}


float Lune::calculateFunc(const float& v) {
  return v*v*v - 2*v*v + 3;
}


void Lune::bisect(const float& a, const float& b, float &c) {
  c = a;
  float d = a;
  float e = b;

  while((e - d) >= 0.01) {
    c = (d + e) / 2;
    if(calculateFunc(c) == 0.0)
      break;
    else if(calculateFunc(c) * calculateFunc(d) < 0)
      e = c;
    else
      d = c;
  }
}


void Lune::calculateRelativeDate(time_t *tin, time_t *tout, const int& days) {
  // Our day time constant
  const time_t one_day = 24 * 60 * 60;

  // Calculate days from our start date
  *tout = *tin + (one_day * days);
}


// ------- Lune Private Implementation


Lune::Lune() {
  // Get the current time stamp
  time(&current_time);

  calculateJulianFromTime(&current_time, jdate);
  calculatePhase();
  calculatePhaseString();
  calculateNextPhase();

  t_date = calculateGregorianString(jdate);
}


/*void Lune::print() {
  if(m_phases.empty() || m_phases.size() != 5) {
    std::cerr << "[ERROR]: Unable to display data." << std::endl;
    return;
  } else {
    std::cout << "\n\n\t\t\tPhases of the Moon\n"
    << "--------------------------------------------------------------------------------\n"
    << "\nDate: " << t_date << "\t\tJulian Date: " << jdate
    << "\nCurrent Phase: " << m_string
    << "\n\nNew Moon: " << m_phases[0] << "\t\tFirst Quarter Moon: " << m_phases[1]
    << "\nFull Moon: " << m_phases[2] << "\t\tLast Quarter Moon: " << m_phases[3]
    << "\nNext New Moon: " << m_phases[4] << "\n"
    << "--------------------------------------------------------------------------------\n"
    << "\n"
    << std::endl;
  }
}

void Lune::printLune() {
  // Figure out the phase
  float angphase = m_phase * 2.0 * M_PI;
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
    int col = 0;
    while(col < colleft) {
      std::cout << " ";
      ++col;
    }

    while(col < colright) {
      std::cout << moon_ascii[line][col];
      ++col;
    }

    std::cout << std::endl;
    ++line;
  }
}
*/
