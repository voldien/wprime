/**
    Copyright (C) 2016 Valdemar Lindberg

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

*/


__kernel void wprime(__global unsigned int* subresult,
						unsigned int len,
						unsigned int offset,
						unsigned int p){

	/*	*/
	unsigned int gidi = get_global_id(0);
	unsigned int gid = (gidi + offset);
	
	/*	*/
	unsigned int i = ( gid * len ) + 1;
	unsigned int end = ( i ) + len;
	unsigned long factori = 1;
	for(; i < end; i++){
		factori *= (unsigned long)i;
		factori %= (unsigned long)p;
	}
	
	subresult[gid] = (unsigned int)factori;	
}
