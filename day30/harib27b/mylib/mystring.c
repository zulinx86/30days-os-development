#include "mystring.h"

int mystrcmp(const unsigned char *s1, const unsigned char *s2)
{
	int i = 0, ans = 0;
	while (s1[i] != 0 && s2[i] != 0) {
		if (s1[i] == s2[i]) {
			++i;
		}
		else if (s1[i] > s2[i]) {
			ans = 1;
			break;
		}
		else if (s1[i] < s2[i]) {
			ans = -1;
			break;
		}
	}
	if (s1[i] != 0 && s2[i] == 0) { ans = 1; }
	if (s1[i] == 0 && s2[i] != 0) { ans = -1; }
	return ans;
}

int mystrncmp(const unsigned char *s1, const unsigned char *s2, unsigned int n)
{
	int i = 0, ans = 0;
	while (s1[i] != 0 && s2[i] != 0 && i < n) {
		if (s1[i] == s2[i]) {
			++i;
		}
		else if (s1[i] > s2[i]) {
			ans = 1;
			break;
		}
		else if (s1[i] < s2[i]) {
			ans = -1;
			break;
		}
	}

	if (i != n) {
		if (s1[i] == 0) {
			ans = -1;
		}
		if (s2[i] == 0) {
			ans = 1;
		}
	}
	return ans;
}

int mystrlen(const unsigned char *s)
{
	int i = 0;
	while (*(s + i) != 0)
		++i;
	return i;
}
