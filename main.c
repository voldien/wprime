/**
    Copyright (C) 2015  Valdemar Lindberg


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
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>


/*	solving (p -1)! + 1 === 0 mod p => p is a prime number. */
unsigned long long int compute_wilson_decimal_prime(unsigned long long int p){
	unsigned long long int i = 1;
	unsigned long long int tmp = 1;

	for( ; i < p; i++){
		/*	(tmp * i) mod p === r*/
		tmp = (tmp * i) % p;
	}
	return (tmp + 1) % p;
}

int main(int argc, char** argv){
	unsigned long long int isPrime = 1;
	unsigned int c;

	if(argc <= 1){
		return EXIT_FAILURE;
	}


	while(( c = getopt(argc, argv, ":d:v")) != EOF){
		switch(c){
		case 'v':
			printf("version 1.0.0. \n");
			return EXIT_SUCCESS;
		break;
		case 'd':
		default:{
			if(optarg){
				unsigned long long int tmp = strtoll(optarg, NULL, 10);
				isPrime = compute_wilson_decimal_prime(tmp);
				printf("%lld is prime : %s\n", tmp, isPrime == 0 ? "true" : "false");
			}
		}break;
		}
	}


	return EXIT_SUCCESS;
}
