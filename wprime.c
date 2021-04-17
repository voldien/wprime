/**
	wprime is software for computing if a natural number is a prime number.
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
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef WPRIME_MAJOR_VERSION
#define WPRIME_MAJOR_VERSION 1
#endif

#ifndef WPRIME_MINOR_VERSION
#define WPRIME_MINOR_VERSION 0
#endif

#ifndef WPRIME_REVISION_VERSION
#define WPRIME_REVISION_VERSION 0
#endif

/**
 *	solving (p -1)! + 1 === 0 mod p => p is a prime number.
 */
unsigned long long int compute_wilson_decimal_prime(unsigned long long int p) {
	unsigned long long int i = 1;
	unsigned long long int tmp = 1;

	for (; i < p; i++) {
		/*	(tmp * i) mod p === r*/
		tmp = (tmp * i) % p;
	}

	return (tmp + 1) % p;
}

int main(int argc, char **argv) {
	char buf[1024];						/*	*/
	unsigned long long int isPrime = 1; /*	*/
	unsigned long long int tmp;			/*	*/
	unsigned int c;						/*	getopt character.	*/
	unsigned int base = 10;				/*	number base.	*/
	unsigned int human = 0;				/*	readable for human.	*/
	unsigned int isPipe = 0;			/*	*/
	unsigned long long int maxp;		/*	*/

	/*	Requires either at least an argument or regular piping. */
	isPipe = isatty(STDIN_FILENO) == 0;
	if (argc <= 1 && !isPipe) {
		fprintf(stderr, "No input.\n");
		return EXIT_FAILURE;
	}

	/*	*/
	while ((c = getopt(argc, argv, "smdvHob:h")) != EOF) {
		switch (c) {
		case 'v':
			printf("version %d.%d.%d\n", WPRIME_MAJOR_VERSION, WPRIME_MINOR_VERSION, WPRIME_REVISION_VERSION);
			return EXIT_SUCCESS;
		case 'm': /*	max = 2^(bitesize/2) - 1	*/
			maxp = 1;
			maxp = (maxp << (sizeof(tmp) * 4)) - 1;
			printf("%ld\n", maxp);
			return EXIT_SUCCESS;
		case 's':
			printf("%ld\n", sizeof(unsigned long long int) * 8);
			return EXIT_SUCCESS;
		case 'h':
			human = 1;
			break;
		case 'b': /*	number base.	*/
			if (optarg) {
				base = atoi(optarg);
			}
			break;
		case 'o': /*	octal*/
			base = 8;
			break;
		case 'H': /*	Hexadecimal	*/
			base = 16;
			break;
		case 'd': /*	Decimal in base 10.	*/
			base = 10;
			break;
		case '?':
			break;
		default:
			fprintf(stderr, "Invalid flag \'%c\'.\n", optopt);
			// return EXIT_FAILURE;
		}
	}

	/*	read from argument without flag.	*/
	if (optind < argc) {
		tmp = strtoll(argv[optind], NULL, base);
	} else {
		/*	read from pipe input.	*/
		if (isPipe) {
			if (read(STDIN_FILENO, buf, sizeof(buf)) > 0) {
				tmp = strtoll(buf, NULL, base);
			}
		} else
			return EXIT_FAILURE;
	}

	/*	can divide by 0.	*/
	if (tmp == 0) {
		return EXIT_FAILURE;
	}

	/*	compute prime.	*/
	isPrime = compute_wilson_decimal_prime(tmp);
	if (!human) {
		printf("%d\n", isPrime == 0 ? 1 : 0);
	} else {
		printf("%lld is prime : %s\n", tmp, isPrime == 0 ? "true" : "false");
	}

	return EXIT_SUCCESS;
}
