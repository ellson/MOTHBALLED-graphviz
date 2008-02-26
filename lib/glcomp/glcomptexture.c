#include "glcomptexture.h"

glCompTexture* glCompCreateTextureFromRaw(char* filename,int width,int height,int wrap)
{
	glCompTexture* t;	
#ifdef _WIN32
	BYTE * data;
#else
	unsigned char *data;
#endif
	FILE * file;
	t=malloc(sizeof(glCompTexture));
	glGenTextures( 1, &t->id );

	// allocate buffer
	data = malloc( width * height * 3 );
	// open and read texture data
	file = fopen( filename, "rb" );
	fread( data, width * height * 3, 1, file );
	fclose( file );


// select our current texture
	glBindTexture( GL_TEXTURE_2D, t->id);

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     wrap ? GL_REPEAT : GL_CLAMP );

/*	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	
	// build our texture mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,data);*/

    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, width, height,
                       GL_RGB, GL_UNSIGNED_BYTE, data );

    // free buffer
    free( data );
	t->w=width;
	t->h=height;
	return t;
}
int glCompDeleteTexture(glCompTexture* t)
{
	return 0;
}
