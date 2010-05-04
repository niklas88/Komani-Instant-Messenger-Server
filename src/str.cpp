/*# Komani Instant Messenger Server
# Copyright (C) 2010 Niklas Schnelle
# Copyright (C) 2010 Konstantin Weitz
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#include "../include/str.h"

std::string str(int value, int base) { 		
	enum { kMaxDigits = 35 };	
	std::string buf;	
	buf.reserve( kMaxDigits ); // Pre-allocate enough space. 	
	
	// check that the base is valid 	
	if (base < 2 || base > 16) return buf;		
	int quotient = value;		
	
	// Translating number to string with base: 	
	do {	
		buf += "0123456789abcdef"[ std::abs( quotient % base ) ];	
		quotient /= base;	
	} while ( quotient );		
	// Append the negative sign for base 10 	
	if ( value < 0 && base == 10) buf += '-';		
	std::reverse( buf.begin(), buf.end() );	
	return buf;	
}
