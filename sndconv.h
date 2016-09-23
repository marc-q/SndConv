#ifndef __SNDCONV_H__
#define __SNDCONV_H__

#define P_CMD "sndconv"

#define BUFFER_LEN 4096

#define SC_CMD_ARG_HELP 'h'
#define SC_CMD_ARG_VERSION 'V'
#define SC_CMD_ARG_FILEINFO '1'
#define SC_CMD_ARG_FORMAT 'f'

struct _sc_metadata
{
	const char *title;
	const char *copyright;
	const char *software;
	const char *artist;
	const char *comment;
	const char *date;
	const char *album;
	const char *license;
	const char *tracknbr;
	const char *genre;
};

typedef struct _sc_metadata sc_metadata;

#endif /* __SNDCONV_H__ */
