#ifndef _MY_ATOF
#define _MY_ATOF 1
#define my_isdigit(x) (x>='0' && x<='9')
#define my_isspace(x) (x==' ' || x=='\t' || x=='\n' || x=='\r')
#define my_tonum(x) (x-'0')

float my_atof(char *s)
{
	float f = 0.0, d = 0.0;
	char negative = 0;

	for (;(my_isspace(*s) || *s == '+')&& *s != 0;s++);

	if (*s == '-') {
		s++;
		negative = 1;
	}

	for(;my_isdigit(*s) && *s != 0;s++) {
		f  *= 10;
		f += my_tonum(*s);
	}
	if (*s == '.') {
		s++;
		d = 0.1;
		for(;my_isdigit(*s) && *s != 0;d *= 0.1,s++)
			f += my_tonum(*s)*d;
	}
	if (negative)f = -f;
	return f;
}
#endif
