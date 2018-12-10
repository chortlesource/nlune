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

#ifndef _LUNE_HPP
#define _LUNE_HPP


// ------- Moon Class

class Lune {
private:
  // Current time variables
  time_t current_time;
  std::string t_date;       // Formatted date string
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

  void printLune();

  const float& getPhase() { return m_phase; }
  const int& getJulianDate() { return jdate; }
  const std::string& getDate() { return t_date; }
  const std::string& getPhaseString() { return m_string; }
  const std::vector<std::string>& getNextPhases() { return m_phases; }
};


#endif // _MOONAR_HPP
