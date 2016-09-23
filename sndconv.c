/* Copyright 2016 Marc Volker Dickmann */
/* Project: SndConv */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sndfile.h>
#include "sndconv.h"

static short sndconv_get_format (char *filename_src)
{
	SNDFILE *srcfile;
	SF_INFO  sfinfo;
	
	srcfile = sf_open (filename_src, SFM_READ, &sfinfo);
	if (srcfile == NULL)
	{
		printf ("Error: Couldn't open the file: %s!\n", filename_src);
		return -1;
	}
	
	printf ("Samplerate:\t%i Hz\n", sfinfo.samplerate);
	printf ("Channels:\t%i\n", sfinfo.channels);
	printf ("Format:\t\t");
	switch (sfinfo.format & SF_FORMAT_TYPEMASK)
	{
		case SF_FORMAT_WAV:
			printf ("WAV, ");
			break;
		case SF_FORMAT_VOC:
			printf ("VOC, ");
			break;
		case SF_FORMAT_FLAC:
			printf ("FLAC, ");
			break;
		case SF_FORMAT_OGG:
			printf ("OGG, ");
			break;
		default:
			printf ("Unknown, ");
			break;
	}
	
	switch (sfinfo.format & SF_FORMAT_SUBMASK)
	{
		case SF_FORMAT_PCM_S8:
			printf ("PCM 8 bit, ");
			break;
		case SF_FORMAT_PCM_16:
			printf ("PCM 16 bit, ");
			break;
		case SF_FORMAT_PCM_24:
			printf ("PCM 24 bit, ");
			break;
		case SF_FORMAT_PCM_32:
			printf ("PCM 32 bit, ");
			break;
		case SF_FORMAT_PCM_U8:
			printf ("PCM unsigned 8 bit, ");
			break;
		case SF_FORMAT_FLOAT:
			printf ("32 bit float, ");
			break;
		case SF_FORMAT_DOUBLE:
			printf ("64 bit float, ");
			break;
		case SF_FORMAT_VORBIS:
			printf ("Vorbis, ");
			break;
		default:
			printf ("Unknown, ");
			break;
	}
	
	switch (sfinfo.format & SF_FORMAT_ENDMASK)
	{
		case SF_ENDIAN_FILE:
			printf ("Default file endian-ness.\n");
			break;
		case SF_ENDIAN_LITTLE:
			printf ("Little endian-ness.\n");
			break;
		case SF_ENDIAN_BIG:
			printf ("Big endian-ness.\n");
			break;
		case SF_ENDIAN_CPU:
			printf ("CPU endian-ness.\n");
			break;
		default:
			printf ("Unknown.\n");
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
		out = (SF_FORMAT_WAV | SF_FORMAT_PCM_16) | SF_ENDIAN_FILE;
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
		printf ("Error: Unknown format, using FLAC!\n");
		out = (SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
	}
	
	return out;
}

/* Metadata */

static void sndconv_metadata_read (sc_metadata *meta, SNDFILE *srcfile)
{
	meta->title = sf_get_string (srcfile, SF_STR_TITLE);
	meta->copyright = sf_get_string (srcfile, SF_STR_COPYRIGHT);
	meta->software = sf_get_string (srcfile, SF_STR_SOFTWARE);
	meta->artist = sf_get_string (srcfile, SF_STR_ARTIST);
	meta->comment = sf_get_string (srcfile, SF_STR_COMMENT);
	meta->date = sf_get_string (srcfile, SF_STR_DATE);
	meta->album = sf_get_string (srcfile, SF_STR_ALBUM);
	meta->license = sf_get_string (srcfile, SF_STR_LICENSE);
	meta->tracknbr = sf_get_string (srcfile, SF_STR_TRACKNUMBER);
	meta->genre = sf_get_string (srcfile, SF_STR_GENRE);
}

static void sndconv_metadata_write_single (SNDFILE *destfile, int str_type, const char *str)
{
	/* Only set the string if it contains real informations [!NULL]. */
	if (str)
	{
		sf_set_string (destfile, str_type, str);
	}
}

static void sndconv_metadata_write (sc_metadata *meta, SNDFILE *destfile)
{
	sndconv_metadata_write_single (destfile, SF_STR_TITLE, meta->title);
	sndconv_metadata_write_single (destfile, SF_STR_COPYRIGHT, meta->copyright);
	sndconv_metadata_write_single (destfile, SF_STR_SOFTWARE, meta->software);
	sndconv_metadata_write_single (destfile, SF_STR_ARTIST, meta->artist);
	sndconv_metadata_write_single (destfile, SF_STR_COMMENT, meta->comment);
	sndconv_metadata_write_single (destfile, SF_STR_DATE, meta->date);
	sndconv_metadata_write_single (destfile, SF_STR_ALBUM, meta->album);
	sndconv_metadata_write_single (destfile, SF_STR_LICENSE, meta->license);
	sndconv_metadata_write_single (destfile, SF_STR_TRACKNUMBER, meta->tracknbr);
	sndconv_metadata_write_single (destfile, SF_STR_GENRE, meta->genre);
}

static short sndconv_convert (char *filename_src, char *filename_dest, int format)
{
	int readcount;
	float buffer[BUFFER_LEN];
	sc_metadata meta;
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
	
	sndconv_metadata_read (&meta, srcfile);
	sndconv_metadata_write (&meta, destfile);
	
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

static void print_version (void)
{
	printf ("SndConv v. 0.0.2 A (C) 2016 Marc Volker Dickmann\n");
	printf ("SndConv is an simple converter for audio files written in C.\n");
}

static void print_help (void)
{
	printf ("Usage: %s [OPTION]... [FILE_IN] [FILE_OUT]\n", P_CMD);
	printf ("Convert FILE_IN into another format and saves it as FILE_OUT.\n");
	
	printf ("\nWith no format specified, FLAC will be used.\n");
	
	printf ("\nMandatory arguments to long options are mandatory for short options too.\n");
	printf ("  -%c, --help\t\tdisplay this help and exit\n", SC_CMD_ARG_HELP);
	printf ("      --version\t\toutput version information and exit\n");
	
	printf ("\n      --fileinfo=FILE\toutput informations about FILE\n");
	
	printf ("\n  -%c, --format=FORMAT\tformat to convert to:\n", SC_CMD_ARG_FORMAT);
	printf ("                     \t  WAV, FLAC, OGG\n");
}

static void print_usage (void)
{
	printf ("Usage: %s --help\n", P_CMD);
}

int main (int argc, char *argv[])
{
	int optc, optindex, arg_format;
	
	struct option long_options[] = {
		{"help", no_argument, 0, SC_CMD_ARG_HELP},
		{"version", no_argument, 0, SC_CMD_ARG_VERSION},
		{"fileinfo", required_argument, 0, SC_CMD_ARG_FILEINFO},
		{"format", required_argument, 0, SC_CMD_ARG_FORMAT},
		{0, 0, 0, 0}
	};
	
	optindex = 0;

	/* Default values */	
	arg_format = sndconv_get_format_byname ("FLAC");
	
	while (1)
	{
		optc = getopt_long (argc, argv, "hf:", long_options, &optindex);
		
		if (optc == -1)
		{
			break;
		}
		
		switch (optc)
		{
			case SC_CMD_ARG_HELP:
				print_help ();
				exit (EXIT_SUCCESS);
				break;
			case SC_CMD_ARG_VERSION:
				print_version ();
				exit (EXIT_SUCCESS);
				break;
			case SC_CMD_ARG_FILEINFO:
				sndconv_get_format (optarg);
				exit (EXIT_SUCCESS);
				break;
			case SC_CMD_ARG_FORMAT:
				arg_format = sndconv_get_format_byname (optarg);
				break;
			default:
				print_usage ();
				exit (EXIT_FAILURE);
				break;
		}
	}
	
	if (argc - optind >= 2)
	{
		sndconv_convert (argv[optind], argv[optind + 1], arg_format);
	}
	else
	{
		print_usage ();
	}
	
	return 0;
}
