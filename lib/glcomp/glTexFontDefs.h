/*
    Copyright (c) 1999 Nate Miller
    
    Notice: Usage of any code in this file is subject to the rules
    described in the LICENSE.TXT file included in this directory.
    Reading, compiling, or otherwise using this code constitutes
    automatic acceptance of the rules in said text file.

    File        -- glTexFontDefs.h
    Date        -- 5/31/99
    Author      -- Nate 'm|d' Miller
    Contact     -- vandals1@home.com
    Web         -- http://members.home.com/vandals1

    All functions are called internally by the library, These should never be 
    called from anywhere but INSIDE the library.  Versy messy but doesn't
    matter all that much.
*/
/*
    FONT_BLOCK_COL, FONT_BLOCK_ROW
	
    Number of characters per row and per col in the font map.  
    FONT_BLOCK_COL * FONT_BLOCK_ROW must equal 256.  If it doesn't the library 
    will fail to work properly.  Why 256?  Chars range from 0 to 255.  
*/
#define FONT_BLOCK_COL              16
#define FONT_BLOCK_ROW              16
#define FONT_GET_MODES              1 
#define FONT_RESTORE_MODES          2 
#define FONT_MAX_LEN                1024 /* maximum chars to draw to the screen, used for buffers also */
#define FONT_TAB_SPACE              4    /* spaces to draw for a tab, make option? */
#define FONT_ITOF                   (float) pow (255, -1)
#define FONT_ITALIC                 8    /* italic amount in pixels */
#define fontColorCopy(a,b)          {b[0]= a[0]; b[1]= a[1]; b[2]= a[2]; b[3]= a[3];} /* copys colors */

/* various functions used by the library, see texFont.c for info */
void fontDrawChar (texFont_t* font,char c, GLfloat x, GLfloat y, GLfloat size, int shadow);
void fontScissorNormal (texFont_t* font,int xpos, int ypos, int tabs, int carrage, int size, int len);
int fontSetColorFromToken (texFont_t* font,char *s);
int fontGetCharHits (char *s, char f);
void fontMakeMap (texFont_t* font);
void fontSetModes (int state);
void fontReset (texFont_t* font);
void fontScissorTextRegion (texFont_t* font);
int fontItalicsMode (texFont_t* font,char *s);
int fontBoldMode (texFont_t* font,char *s);
void fontRenderChar (texFont_t* font,char c, GLfloat x, GLfloat y, GLfloat size);
