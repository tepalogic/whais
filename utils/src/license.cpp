/******************************************************************************
WHAISC - A compiler for whais programs
Copyright(C) 2009  Iulian Popa

Address: Str Olimp nr. 6
         Pantelimon Ilfov,
         Romania
Phone:   +40721939650
e-mail:  popaiulian@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include <iostream>
#include <iomanip>

#include "license.h"

using namespace std;



#define xstringify(x) stringify(x)
#define stringify(x) #x


extern "C" void
showBanner(const char* const programName,
            const uint_t      verMajor,
            const uint_t      verMinor)
{
  displayBanner(cout, programName, verMajor, verMinor);
}


extern "C" void
showLicenseInformation(const char* const programName,
                        const char* const programDescription)
{
  displayLicenseInformation(cout, programName, programDescription);
}



void
displayBanner(std::ostream&      os,
               const char* const  programName,
               const uint_t       verMajor,
               const uint_t       verMinor)
{
  os << programName << " version "
     << verMajor << '.' << setw(2) << setfill('0') << verMinor
     << setw(0) << setfill(' ') << " for "
     << xstringify(WOS) << ' ' << xstringify(WARCH)
     << "\nCopyright(C) 2015 by Iulian Popa(popaiulian@gmail.com)\n";
}

void
displayLicenseInformation(std::ostream&     os,
                           const char* const programName,
                           const char* const programDescription)
{
  os << programName;
  if (programDescription != nullptr)
    os << " - " << programDescription;

  os <<
    "\nCopyright(C) 2015  Iulian Popa\n"
    "\n"
    "Address: Str. Olimp nr. 6\n"
    "         Pantelimon, Ilfov\n"
    "         Romania\n"
    "Phone:   +40721939650\n"
    "e-mail:  popaiulian@gmail.com\n"
    "\n"
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n"
    "\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
    "\n";
}
