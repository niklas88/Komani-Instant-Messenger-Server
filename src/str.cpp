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
