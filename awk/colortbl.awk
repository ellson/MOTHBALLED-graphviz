# Copyright (c) AT&T Corp. 1994, 1995.
# This code is licensed by AT&T Corp.  For the
# terms and conditions of the license, see
# http://www.research.att.com/orgs/ssr/book/reuse

function rgb_to_hsb(r,g,b) {
	r = r / 255.0; g = g / 255.0; b = b / 255.0;
	max = r; if (max < g) max = g; if (max < b) max = b;
	min = r; if (min > g) min = g; if (min > b) min = b;
	v = max;
	if (max != 0) s = (max - min) / max;
	else s = 0;
	if (s == 0) h = 0;
	else {
		delta = max - min;
		rc = (max - r)/delta;
		gc = (max - g)/delta;
		bc = (max - b)/delta;
		if (r == max) h = bc - gc;
		else {
			if (g == max) h = 2.0 + (rc - bc);
			else h = 4.0 + (gc - rc);
		}
		h = h * 60.0;
		if (h < 0.0) h = h + 360.0;
	}
	h = h / 360.0 * 255.0;
	s = s * 255.0;
	v = v * 255.0;
}

BEGIN	{ s = ARGV[1]; gsub("\\.","_",s); printf("hsbcolor_t %s[] = {\n",s); }
/^$/	{ next; }
/^#/	{ next; }
		{
			rgb_to_hsb($2,$3,$4);
			printf("{\"%s\",%d,%d,%d},\n",$1,h,s,v);
		}
END		{ printf("};\n"); }
