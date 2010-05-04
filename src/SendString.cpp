/*
# Komani Instant Messenger Server
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
#include "../include/TCPNet.h"
*/

int  SendStringWBLen(CSocket* Socket,const std::string& String){
	unsigned char length=String.length();//htonl(String.length());
	std::cout<<"Entering SendStringWBLen"<<std::endl;
	int ret =  Socket->send((char*)&length,sizeof(length),0);
	std::cout<<"SendStringWBLen len="<<(unsigned int)length<<std::endl;
	ret     += Socket->send(String);
	std::cout<<"SendStringWBLen sent="<<ret<<std::endl;
	
	return ret;
}
