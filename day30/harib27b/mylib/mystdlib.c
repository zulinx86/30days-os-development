#include "mystdlib.h"

#define INVALID -0x7fffffff

static int get_single_digit(const unsigned char *nptr, int radix)
{
	if (radix == 8) {
		if (*nptr < '0' || '8' <= *nptr)
			return INVALID;
		else
			return *nptr - '0';
	} else if (radix == 10) {
		if (*nptr < '0' || '9' < *nptr)
			return INVALID;
		else
			return *nptr - '0';
	} else if (radix == 16) {
		if ('0' <= *nptr && *nptr <= '9')
			return *nptr - '0';
		else if ('a' <= *nptr && *nptr <= 'f')
			return 10 + (*nptr - 'a');
		else
			return INVALID;
	}

	return INVALID;
}

long mystrtol(const unsigned char *nptr, unsigned char **endptr)
{
	long result = 0;
	int radix, sign = 1, digit;

	while (*nptr == ' ')
		++nptr;
	
	switch (*nptr) {
	case '-':
		sign = -1;
		++nptr;
		break;
	case '+':
		++nptr;
		break;
	}

	if (*nptr == '0') {
		++nptr;
		if (*nptr == 'x') {
			radix = 16;
			++nptr;
		} else {
			radix = 8;
		}
	} else {
		radix = 10;
	}

	while ((digit = get_single_digit(nptr, radix)) != INVALID) {
		++nptr;
		result = result * radix + digit;
	}

	*endptr = nptr;
	return sign * result;
}

