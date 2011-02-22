
#define GV_FONT_LIST PS_AVANTGARDE,PS_BOOKMAN,PS_COURIER,PS_HELVETICA,	PS_NEWCENTURYSCHLBK,PS_PALATINO,PS_SYMBOL,PS_TIMES,PS_CHANCERY,PS_DINGBATS
#define GV_FONT_LIST_SIZE 10
#define MAX_FACE_SIZE	12
#define MAX_GV_PS_FONTS 35
#define FONT_FACE_NUM	11

#define PS_AVANTGARDE "AvantGarde"
#define PS_BOOKMAN "Bookman"
#define PS_COURIER "Courier"
#define PS_HELVETICA SAN_5
#define PS_NEWCENTURYSCHLBK "NewCenturySchlbk"
#define PS_PALATINO "Palatino"
#define PS_SYMBOL "Symbol"
#define PS_TIMES SER_3
#define PS_CHANCERY "ZapfChancery"
#define PS_DINGBATS "ZapfDingbats"

#define FNT_BOLD	1
#define FNT_BOOK	1<<2
#define FNT_CONDENSED	1<<3
#define FNT_DEMI	1<<4
#define FNT_EXTRALIGHT	1<<5
#define FNT_ITALIC	1<<6
#define FNT_LIGHT	1<<7
#define FNT_MEDIUM	1<<8
#define FNT_OBLIQUE	1<<9
#define FNT_REGULAR	1<<10
#define FNT_ROMAN	1<<10

#define FNT_BOLD_ST	"BOLD"
#define FNT_BOOK_ST	"BOOK"
#define FNT_CONDENSED_ST	"CONDENSED"
#define FNT_DEMI_ST	"DEMI"
#define FNT_EXTRALIGHT_ST	"EXTRALIGHT"
#define FNT_ITALIC_ST	"ITALIC"
#define FNT_LIGHT_ST	"LIGHT"
#define FNT_MEDIUM_ST	"MEDIUM"
#define FNT_OBLIQUE_ST	"OBLIQUE"
#define FNT_REGULAR_ST	"REGULAR"
#define FNT_ROMAN_ST	"ROMAN"

#define SAN_0		"sans"
#define SAN_1		"URW Gothic L"
#define SAN_2		"Charcoal"
#define SAN_3		"Nimbus Sans L"
#define SAN_4		"Verdana"
#define SAN_5		"Helvetica"
#define SAN_6		"Bitstream Vera Sans"
#define SAN_7		"DejaVu Sans"
#define SAN_8		"Liberation Sans"
#define SAN_9		"Luxi Sans"
#define SAN_10		"FreeSans"
#define SAN_11		"Arial"

#define SER_0		"serif"
#define SER_1		"URW Bookman L"
#define SER_2		"Times New Roman"
#define SER_3		"Times"
#define SER_4		"Nimbus Roman No9 L"
#define SER_5		"Bitstream Vera Serif"
#define SER_6		"DejaVu Serif"
#define SER_7		"Liberation Serif"
#define SER_8		"Luxi Serif"
#define SER_9		"FreeSerif"
#define SER_10		"Century Schoolbook L"
#define SER_11		"Charcoal"
#define SER_12		"Georgia"
#define SER_13		"URW Palladio L"
#define SER_14		"Norasi"
#define SER_15		"Rekha"
#define SER_16		"URW Chancery L"

#define MON_0		"monospace"
#define MON_1		"Nimbus Mono L"
#define MON_2		"Inconsolata"
#define MON_3		"Courier New"
#define MON_4		"Bitstream Vera Sans Mono"
#define MON_5		"DejaVu Sans Mono"
#define MON_6		"Liberation Mono"
#define MON_7		"Luxi Mono"
#define MON_8		"FreeMono"

#define SYM_0		"fantasy"
#define SYM_1		"Impact"
#define SYM_2		"Copperplate Gothic Std"
#define SYM_3		"Cooper Std"
#define SYM_4		"Bauhaus Std"

#define DING_0		"fantasy"
#define DING_1		"Dingbats"
#define DING_2		"Impact"
#define DING_3		"Copperplate Gothic Std"
#define DING_4		"Cooper Std"
#define DING_5		"Bauhaus Std"

#define MAX_FONTNAME	100
#define MAX_FONTSZ	512



typedef struct {
	char*  generic_name;
	char*  fontname;
	char** equiv;
	int    eq_sz;
} fontdef_t;

typedef struct {
	char * gv_ps_fontname;
	char * fontname;
	int faces;
} availfont_t;

typedef struct {
	char gv_ps_fontname[MAX_FONTNAME];
	char gv_font[MAX_FONTSZ];
}gv_font_map;

void get_font_mapping(PangoFontMap *pfm, gv_font_map *gfm);

 



