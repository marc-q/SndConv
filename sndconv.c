/* Copyright 2016 Marc Volker Dickmann */
/* Project: SndConv */
#include <stdio.h>
#include <string.h>
#include <sndfile.h>

#define BUFFER_LEN 4096

static short sndconv_get_format (char *filename_src)
{
	SNDFILE *srcfile;
	SF_INFO  sfinfo;
	
	srcfile = sf_open (filename_src, SFM_READ, &sfinfo);
	if (srcfile == NULL)
	{
		printf ("Error: couldn't open the file: %s!\n", filename_src);
		return -1;
	}

	printf ("Format: ");
	switch (sfinfo.format & SF_FORMAT_TYPEMASK)
	{
		case SF_FORMAT_WAV:
			printf ("WAV\n");
			break;
		case SF_FORMAT_VOC:
			printf ("VOC\n");
			break;
		case SF_FORMAT_FLAC:
			printf ("FLAC\n");
			break;
		case SF_FORMAT_OGG:
			printf ("OGG\n");
			break;
		default:
			printf ("Unknown\n");
			break;
	}
	
	sf_close (srcfile);
	
	return 0;
}

static int sndconv_get_format_byname (char *name)
{
	int out;
	
	if (strcmp (name, "WAV") == 0)
	{
		out = (SF_FORMAT_WAV | SF_FORMAT_PCM_16) | SF_ENDIAN_LITTLE;
	}
	else if (strcmp (name, "FLAC") == 0)
	{
		out = (SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
	}
	else if (strcmp (name, "OGG") == 0)
	{
		out = (SF_FORMAT_OGG | SF_FORMAT_VORBIS);
	}
	else
	{
		printf ("Error: Unknown format, use FLAC!\n");
		out = (SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
	}
	
	return out;
}

static short sndconv_convert (char *filename_src, char *filename_dest, int format)
{
	int readcount;
	float buffer[BUFFER_LEN];
	SNDFILE *srcfile, *destfile;
	SF_INFO  sfinfo;
	
	srcfile = sf_open (filename_src, SFM_READ, &sfinfo);
	if (srcfile == NULL)
	{
		printf ("Error: Couldn't open the file: %s!\n", filename_src);
		return -1;
	}
	
	sfinfo.format = format;
	if (!sf_format_check (&sfinfo))
	{
		sf_close (srcfile);
		printf ("Error: Invalid format!\n");
		return -2;
	}
	
	destfile = sf_open (filename_dest, SFM_WRITE, &sfinfo);
	if (destfile == NULL)
	{
		sf_close (srcfile);
		printf ("Error: Couldn't open the file: %s!\n", filename_dest);
		return -3;
	}
	
	while (1)
	{
		readcount = sf_read_float (srcfile, buffer, BUFFER_LEN);
		
		if (readcount <= 0)
		{
			break;
		}
		
		sf_write_float (destfile, buffer, readcount);
	}
	
	sf_close (srcfile);
	sf_close (destfile);
	
	return 0;
}

static void print_about (void)
{
	printf ("SndConv is an simple converter for Audio files written in C.\n");
}

static void print_usage (void)
{
	printf ("SndConv:\n");
	printf ("\t-h\t\tHelp\n");
}

int main (int argc, char *argv[])
{
	printf ("SndConv v. 0.0.1 A (C) 2016 Marc Volker Dickmann\n");
	
	if (argc == 2)
	{
		sndconv_get_format (argv[1]);
	}
	else if (argc == 3)
	{
		sndconv_convert (argv[1], argv[2], sndconv_get_format_byname ("FLAC"));
	}
	else if (argc == 4)
	{
		sndconv_convert (argv[1], argv[2], sndconv_get_format_byname (argv[3]));
	}
	else
	{
		print_about ();
		printf ("\n");
		print_usage ();
	}
	
	return 0;
}
