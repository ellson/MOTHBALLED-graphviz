/* vim:set shiftwidth=4 ts=8: */
#define HAVE_PANGOCAIRO
#include <stdlib.h>
#include <string.h>

#include "gvplugin_textlayout.h"
#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>
#include "gvgetfontlist.h"
#endif

#define N_NEW(n,t)      (t*)malloc((n)*sizeof(t))

static PostscriptAlias postscript_alias[] = {
#include "ps_font_equiv.h"
};

/* Frees memory used by the Graphviz PS font definitions */
void gv_flist_free_ps(fontdef_t ** gv_ps_fontdefs_p)
{
    int i, j;

    for (i = 0; i < GV_FONT_LIST_SIZE; i++) {
	for (j = 0; j < (*gv_ps_fontdefs_p[i]).eq_sz; j++)
	    free((*gv_ps_fontdefs_p[i]).equiv[j]);
	free((*gv_ps_fontdefs_p[i]).equiv);
	free(gv_ps_fontdefs_p[i]);
    }
    free(gv_ps_fontdefs_p);
}


/* Frees memory used by the avialable system font definitions */
void gv_flist_free_af(availfont_t ** gv_af_p)
{
    int i;

    for (i = 0; i < GV_FONT_LIST_SIZE; i++) {
	free((*gv_af_p[i]).fontname);
	free(gv_af_p[i]);
    }
    free(gv_af_p);
}

/* Loads the generic and equivalent fonts for the Graphviz PS fonts */
void load_fontdefs(fontdef_t * gv_ps_fontdefs, char *generic,
		   const int arraysz, const char **font_equiv)
{
    int j;
    gv_ps_fontdefs->generic_name = generic;
    gv_ps_fontdefs->equiv = N_NEW(arraysz, char *);
    for (j = 0; j < arraysz; j++) {
	gv_ps_fontdefs->equiv[j] = strdup(font_equiv[j]);
	gv_ps_fontdefs->eq_sz = arraysz;
    }
}

/* Converts string to upper case */
void upperCase(char *src)
{
    for (; *src != '\0'; ++src) {
	if (*src >= 'a' && *src <= 'z') {
	    *src = *src - 32;
	}
    }
}


void get_faces(PangoFontFamily * family, int *availfaces)
{
    PangoFontFace **faces;
    PangoFontFace *face;
    int i, j, n_faces;
    const char *name;
    const char *facetypes[] =
	{ FNT_BOLD_ST, FNT_BOOK_ST, FNT_CONDENSED_ST, FNT_DEMI_ST,
	FNT_EXTRALIGHT_ST, FNT_ITALIC_ST, FNT_LIGHT_ST,
	FNT_MEDIUM_ST, FNT_OBLIQUE_ST, FNT_REGULAR_ST, FNT_ROMAN_ST
    };

    const int facevalue[] =
	{ FNT_BOLD, FNT_BOOK, FNT_CONDENSED, FNT_DEMI, FNT_EXTRALIGHT,
FNT_ITALIC,
	FNT_LIGHT, FNT_MEDIUM, FNT_OBLIQUE, FNT_REGULAR, FNT_ROMAN
    };
    char *ucName;
    *availfaces = 0;
    /* Get the faces (Bold, Italic, etc.) for the current font family */
    pango_font_family_list_faces(family, &faces, &n_faces);
    for (i = 0; i < n_faces; i++) {
	face = faces[i];
	name = pango_font_face_get_face_name(face);

	/* name is const char, so it needs to be copied for upper case conversion */
	ucName = strdup(name);
	upperCase(ucName);
	/* if the family face type is one of the known types, logically OR the known type value
	   to the available faces integer */
	for (j = 0; j < FONT_FACE_NUM; j++) {

	    if ((strstr(ucName, facetypes[j]) != NULL)
		|| !strcmp(ucName, facetypes[j])) {
		*availfaces |= facevalue[j];
		break;
	    }
	}
	free(ucName);
    }
    g_free(faces);
}

void display_available_fonts(fontdef_t ** gv_ps_fontdefs_p,
			     availfont_t ** gv_af_p)
{
    int j;

/* Displays the Graphviz PS font name, system available font name and associated faces */
    for (j = 0; j < GV_FONT_LIST_SIZE; j++) {
	printf("ps font = %s available %d font = %s\n",
	       (*gv_ps_fontdefs_p[j]).fontname, (*gv_af_p[j]).faces, (*gv_af_p[j]).fontname);
	if ((*gv_af_p[j]).faces & FNT_BOLD)
	    printf("\t%s\n", FNT_BOLD_ST);
	if ((*gv_af_p[j]).faces & FNT_BOOK)
	    printf("\t%s\n", FNT_BOOK_ST);
	if ((*gv_af_p[j]).faces & FNT_CONDENSED)
	    printf("\t%s\n", FNT_CONDENSED_ST);
	if ((*gv_af_p[j]).faces & FNT_DEMI)
	    printf("\t%s\n", FNT_DEMI_ST);
	if ((*gv_af_p[j]).faces & FNT_EXTRALIGHT)
	    printf("\t%s\n", FNT_EXTRALIGHT_ST);
	if ((*gv_af_p[j]).faces & FNT_ITALIC)
	    printf("\t%s\n", FNT_ITALIC_ST);
	if ((*gv_af_p[j]).faces & FNT_LIGHT)
	    printf("\t%s\n", FNT_LIGHT_ST);
	if ((*gv_af_p[j]).faces & FNT_MEDIUM)
	    printf("\t%s\n", FNT_MEDIUM_ST);
	if ((*gv_af_p[j]).faces & FNT_OBLIQUE)
	    printf("\t%s\n", FNT_OBLIQUE_ST);
	if ((*gv_af_p[j]).faces & FNT_REGULAR)
	    printf("\t%s\n", FNT_REGULAR_ST);
	else if ((*gv_af_p[j]).faces & FNT_ROMAN)
	    printf("\t%s\n", FNT_ROMAN_ST);
	printf("\n");

    }
}

char *get_avail_faces(availfont_t gv_af)
{
    char *faces;
    int size;

/* Determine the size of the string needed to hold the list of font faces */
    size = 1;
    if (gv_af.faces & FNT_BOLD)
	size += (strlen(FNT_BOLD_ST) + 1);
    if (gv_af.faces & FNT_BOOK)
	size += (strlen(FNT_BOOK_ST) + 1);
    if (gv_af.faces & FNT_CONDENSED)
	size += (strlen(FNT_CONDENSED_ST) + 1);
    if (gv_af.faces & FNT_DEMI)
	size += (strlen(FNT_DEMI_ST) + 1);
    if (gv_af.faces & FNT_EXTRALIGHT)
	size += (strlen(FNT_EXTRALIGHT_ST) + 1);
    if (gv_af.faces & FNT_ITALIC)
	size += (strlen(FNT_ITALIC_ST) + 1);
    if (gv_af.faces & FNT_LIGHT)
	size += (strlen(FNT_LIGHT_ST) + 1);
    if (gv_af.faces & FNT_MEDIUM)
	size += (strlen(FNT_MEDIUM_ST) + 1);
    if (gv_af.faces & FNT_OBLIQUE)
	size += (strlen(FNT_OBLIQUE_ST) + 1);
    if (gv_af.faces & FNT_REGULAR)
	size += (strlen(FNT_REGULAR_ST) + 1);
    else if (gv_af.faces & FNT_ROMAN)
	size += (strlen(FNT_ROMAN_ST) + 1);

/* Construct the list of font faces */
    faces = (char *) N_NEW(size, char *);
    faces[0] = '\0';
    if (gv_af.faces & FNT_BOLD)
	strcat(strcat(faces, FNT_BOLD_ST), " ");
    if (gv_af.faces & FNT_BOOK)
	strcat(strcat(faces, FNT_BOOK_ST), " ");
    if (gv_af.faces & FNT_CONDENSED)
	strcat(strcat(faces, FNT_CONDENSED_ST), " ");
    if (gv_af.faces & FNT_DEMI)
	strcat(strcat(faces, FNT_DEMI_ST), " ");
    if (gv_af.faces & FNT_EXTRALIGHT)
	strcat(strcat(faces, FNT_EXTRALIGHT_ST), " ");
    if (gv_af.faces & FNT_ITALIC)
	strcat(strcat(faces, FNT_ITALIC_ST), " ");
    if (gv_af.faces & FNT_LIGHT)
	strcat(strcat(faces, FNT_LIGHT_ST), " ");
    if (gv_af.faces & FNT_MEDIUM)
	strcat(strcat(faces, FNT_MEDIUM_ST), " ");
    if (gv_af.faces & FNT_OBLIQUE)
	strcat(strcat(faces, FNT_OBLIQUE_ST), " ");
    if (gv_af.faces & FNT_REGULAR)
	strcat(strcat(faces, FNT_REGULAR_ST), " ");
    else if (gv_af.faces & FNT_ROMAN)
	strcat(strcat(faces, FNT_ROMAN_ST), " ");
    return faces;
}

/* This function creates an array of font definitions. Each entry corresponds to one of 
   the Graphviz PS fonts.  The font definitions contain the generic font name and a list 
   of equivalent fonts that can be used in place of the PS font if the PS font is not 
   available on the system
*/

availfont_t **gv_get_ps_fontlist(PangoFontMap * fontmap)
{

/* This is where the hierarchy of equivalent fonts is established. The order can be changed
   here or new equivalent fonts can be added here. Each font family used by the Graphviz
   PS fonts is set up.
*/
    const char *PS_ADVANT_E[] =
	{ SAN_1, SAN_2, SAN_3, SAN_4, SAN_5, SAN_6, SAN_7, SAN_8, SAN_9,
SAN_10 };
    const int PS_ADVANT_E_SZ = sizeof(PS_ADVANT_E) / sizeof(char *);

    const char *PS_BOOKMAN_E[] =
	{ SER_1, SER_2, SER_3, SER_4, SER_5, SER_6, SER_7, SER_8, SER_9 };
    const int PS_BOOKMAN_E_SZ = sizeof(PS_BOOKMAN_E) / sizeof(char *);

    const char *PS_COURIER_E[] =
	{ MON_1, MON_2, MON_3, MON_4, MON_5, MON_6, MON_7, MON_8 };
    const int PS_COURIER_E_SZ = sizeof(PS_COURIER_E) / sizeof(char *);

    const char *PS_HELVETICA_E[] =
	{ SAN_3, SAN_11, SAN_4, SAN_6, SAN_7, SAN_8, SAN_9, SAN_10 };
    const int PS_HELVETICA_E_SZ = sizeof(PS_HELVETICA_E) / sizeof(char *);

    const char *PS_NEWCENT_E[] = { SER_10, SER_2, SER_3, SER_4, SER_12,
	SER_5, SER_6, SER_7, SER_8, SER_9
    };
    const int PS_NEWCENT_E_SZ = sizeof(PS_NEWCENT_E) / sizeof(char *);

    const char *PS_PALATINO_E[] =
	{ SER_13, SER_2, SER_3, SER_4, SER_14, SER_15,
	SER_5, SER_6, SER_7, SER_8, SER_9
    };
    const int PS_PALATINO_E_SZ = sizeof(PS_PALATINO_E) / sizeof(char *);

    const char *PS_TIMES_E[] =
	{ SER_4, SER_2, SER_11, SER_5, SER_6, SER_7, SER_8, SER_9 };
    const int PS_TIMES_E_SZ = sizeof(PS_TIMES_E) / sizeof(char *);

    const char *PS_SYMBOL_E[] = { SYM_1, SYM_2, SYM_3, SYM_4 };
    const int PS_SYMBOL_E_SZ = sizeof(PS_SYMBOL_E) / sizeof(char *);

    const char *PS_CHANCERY_E[] =
	{ SER_16, SER_11, SER_2, SER_3, SER_4, SER_5,
	SER_6, SER_7, SER_8, SER_9
    };
    const int PS_CHANCERY_E_SZ = sizeof(PS_CHANCERY_E) / sizeof(char *);

    const char *PS_DINGBATS_E[] = { DING_1, SYM_1, SYM_2, SYM_3, SYM_4 };
    const int PS_DINGBATS_E_SZ = sizeof(PS_DINGBATS_E) / sizeof(char *);

    PangoFontFamily **families;
    PangoFontFamily *family;
    fontdef_t **gv_ps_fontdefs_p, *gv_ps_fontdefs, gv_ps_fontdef;
    int n_families;
    int i, j, k, array_sz, availfaces;
    availfont_t **gv_af_p, *gv_afs;
    const char *name;
    char *gv_ps_fontname, *family_name;
    char *gv_ps_fontnames[] = { GV_FONT_LIST };	/* array of recognized Graphviz PS font names */

    gv_ps_fontdefs_p = N_NEW(GV_FONT_LIST_SIZE, fontdef_t *);
    for (i = 0; i < GV_FONT_LIST_SIZE; i++) {
	gv_ps_fontdefs = N_NEW(1, fontdef_t);
	gv_ps_fontdefs->fontname = gv_ps_fontnames[i];	//Graphviz PS font, ie. "AvantGarde"
	gv_ps_fontdefs_p[i] = gv_ps_fontdefs;

	/* Load the font definition struct with generic and font equivalences */
	if (strcmp(gv_ps_fontnames[i], PS_AVANTGARDE) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SAN_0, PS_ADVANT_E_SZ,
			  PS_ADVANT_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_BOOKMAN) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SER_0, PS_BOOKMAN_E_SZ,
			  PS_BOOKMAN_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_COURIER) == 0) {
	    load_fontdefs(gv_ps_fontdefs, MON_0, PS_COURIER_E_SZ,
			  PS_COURIER_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_HELVETICA) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SAN_0, PS_HELVETICA_E_SZ,
			  PS_HELVETICA_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_NEWCENTURYSCHLBK) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SER_0, PS_NEWCENT_E_SZ,
			  PS_NEWCENT_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_PALATINO) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SER_0, PS_PALATINO_E_SZ,
			  PS_PALATINO_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_TIMES) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SER_0, PS_TIMES_E_SZ,
			  PS_TIMES_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_SYMBOL) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SYM_0, PS_SYMBOL_E_SZ,
			  PS_SYMBOL_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_CHANCERY) == 0) {
	    load_fontdefs(gv_ps_fontdefs, SER_0, PS_CHANCERY_E_SZ,
			  PS_CHANCERY_E);
	} else if (strcmp(gv_ps_fontnames[i], PS_DINGBATS) == 0) {
	    load_fontdefs(gv_ps_fontdefs, DING_0, PS_DINGBATS_E_SZ,
			  PS_DINGBATS_E);
	}

    }

    /* Get a list of font families installed on the system */
    pango_font_map_list_families(fontmap, &families, &n_families);

    /* Setup a pointer to available font structs */
    gv_af_p = N_NEW(GV_FONT_LIST_SIZE, availfont_t *);

    for (j = 0; j < GV_FONT_LIST_SIZE; j++) {
	/* get the Graphviz PS font information and create the
	   available font definition structs */
	gv_ps_fontname = gv_ps_fontnames[j];
if (!strcmp(gv_ps_fontname,"Symbol"))
{ int i = 0;
}
	gv_afs = N_NEW(1, availfont_t);
	gv_af_p[j] = gv_afs;
	gv_ps_fontdef = *gv_ps_fontdefs_p[j];
	family_name = '\0';
	/* Search the installed system font families for the current 
	   Graphvis PS font family name, i.e. AvantGarde */
	for (i = 0; i < n_families; i++) {
	    family = families[i];
	    name = pango_font_family_get_name(family);
	    /* if a match is found get the installed font faces */
	    if (strcasecmp(gv_ps_fontdef.fontname, name) == 0) {
		family_name = strdup(name);
		get_faces(family, &availfaces);
	    }
	    if (!(family_name == '\0'))
		break;
	}
	/* if a match is not found on the primary Graphviz font family,
	   search for a match on the equivalent font family names */
	if (family_name == '\0') {
	    array_sz = gv_ps_fontdef.eq_sz;
	    for (k = 0; k < array_sz; k++) {
		for (i = 0; i < n_families; i++) {
		    family = families[i];
		    name = pango_font_family_get_name(family);
		    if (strcasecmp(gv_ps_fontdef.equiv[k], name) == 0) {
			family_name = strdup(name);
			get_faces(family, &availfaces);
			break;
		    }
		}
		if (!(family_name == '\0'))
		    break;
	    }
	}
	/* if a match is not found on the equivalent font family names, search
	   for a match on the generic family name assigned to the Graphviz PS font */
	if (family_name == '\0') {
	    for (i = 0; i < n_families; i++) {
		family = families[i];
		name = pango_font_family_get_name(family);
		if (strcasecmp(gv_ps_fontdef.generic_name, name) == 0) {
		    family_name = strdup(name);
		    get_faces(family, &availfaces);
		    break;
		}
	    }
	}
	/* if not match is found on the generic name, set the available font
	   name to NULL */
	if (family_name == '\0') {
	    gv_afs->fontname = NULL;
	    gv_afs->gv_ps_fontname = gv_ps_fontname;
	    gv_afs->faces = 0;
	} else {
	    gv_afs->fontname = family_name;
	    gv_afs->gv_ps_fontname = gv_ps_fontname;
	    gv_afs->faces = availfaces;
	}
    }
    g_free(families);
//    display_available_fonts(gv_ps_fontdefs_p, gv_af_p);
/* Free the Graphviz PS font definitions */
    gv_flist_free_ps(gv_ps_fontdefs_p);
    return (gv_af_p);
}

/* Returns the font corresponding to a Graphviz PS font. 
   AvantGarde-Book may return URW Gothic L, book
*/
char *gv_get_font(char *buf, availfont_t ** gv_af_p,
		  PostscriptAlias * ps_alias)
{
    char ps_param[MAX_FACE_SIZE];
    char *avail_faces;
    int i;

    buf[0] = '\0';
    for (i = 0; i < GV_FONT_LIST_SIZE; i++) {
	/* Searches the array of available system fonts for the one that
	   corresponds to the current Graphviz PS font name. Sets up the
	   font string with the avaiable font name and the installed font 
	   faces that match what are required by the Graphviz PS font.
	 */
	if ((*gv_af_p[i]).faces && strstr((*ps_alias).name, (*gv_af_p[i]).gv_ps_fontname)) {
	    strcpy(buf, (*gv_af_p[i]).fontname);
	    strcat(buf, ", ");
	    avail_faces = get_avail_faces(*gv_af_p[i]);
	    if ((*ps_alias).weight && strcpy(ps_param, (*ps_alias).weight)) {
		upperCase(ps_param);
		if (strstr(avail_faces, ps_param)) {
		    strcat(buf, " ");
		    strcat(buf, ps_param);
		}
	    } else {
		strcpy(ps_param, "REGULAR");
		if (strstr(avail_faces, ps_param)) {
		    strcat(buf, " ");
		    strcat(buf, ps_param);
		} else {
		    strcpy(ps_param, "ROMAN");
		    if (strstr(avail_faces, ps_param)) {
			strcat(buf, " ");
			strcat(buf, ps_param);
		    }
		}

	    }
	    if ((*ps_alias).stretch
		&& strcpy(ps_param, (*ps_alias).stretch)) {
		upperCase(ps_param);
		if (strstr(avail_faces, ps_param)) {
		    strcat(buf, " ");
		    strcat(buf, ps_param);
		}
	    }
	    if ((*ps_alias).style && strcpy(ps_param, (*ps_alias).style)) {
		upperCase(ps_param);
		if (strstr(avail_faces, ps_param)) {
		    strcat(buf, " ");
		    strcat(buf, ps_param);
		} else {
		    /* try to use ITALIC in place of OBLIQUE & visa versa */
		    if (!strcmp(ps_param, "ITALIC")) {
			strcpy(ps_param, "OBLIQUE");
			if (strstr(avail_faces, ps_param)) {
			    strcat(buf, " ");
			    strcat(buf, ps_param);
			}
		    } else if (!strcmp(ps_param, "OBLIQUE")) {
			strcpy(ps_param, "ITALIC");
			if (strstr(avail_faces, ps_param)) {
			    strcat(buf, " ");
			    strcat(buf, ps_param);
			}
		    }
		}
	    }
	    free(avail_faces);
	    break;
	}
    }

    return buf;
}

/* Sets up a structure array that contains the Graphviz PS font name
   and the corresponding installed font string.  
*/
void get_font_mapping(PangoFontMap * fontmap, gv_font_map * gv_fmap)
{
    char buf[1024];
    PostscriptAlias *ps_alias;
    availfont_t **gv_af_p;
    int j, ps_fontnames_sz;

    ps_fontnames_sz = sizeof(postscript_alias) / sizeof(PostscriptAlias);

    gv_af_p = gv_get_ps_fontlist(fontmap);	// get the available installed fonts
/* add the Graphviz PS font name and available system font string to the array */
    for (j = 0; j < ps_fontnames_sz; j++) {
	ps_alias = &postscript_alias[j];
	strcpy(gv_fmap[ps_alias->xfig_code].gv_ps_fontname,
	       ps_alias->name);
	buf[0] = '\0';
	gv_get_font(buf, gv_af_p, ps_alias);
	strcpy(gv_fmap[ps_alias->xfig_code].gv_font, buf);
    }
    gv_flist_free_af(gv_af_p);
}
