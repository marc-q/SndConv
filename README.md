# SndConv
SndConv is a program for working with Audio files. Mainly it's used to convert them. It is written in C with the use of the libsndfile1 library.

### How it started
SndConv started as an weekend project. I needed an audio converter and started this, because no other tool satisfied my need's.
Because it should be done quickly, in addition to some other reasons, i use the libsndfile library. It's an easy to use library that integrates with various formats.

### Currently supported formats
Format | State | Notes
------- | ---- | -----
WAV | Supported | 
FLAC | Supported | The default format to convert to.
OGG | Supported | 

### How to compile the source code
To compile the source code you just need to make sure that *libsndfile1-dev* is installed and run this command inside the directory containing the Makefile.

```SH
$ make
```

### Example: How to convert WAV to OGG
To convert an WAV file into an OGG file, use the following command while inside the directory containing the WAV file.

```SH
$ ./sndconv --format="OGG" "example.wav" "example.ogg"
```

### Example: How to get informations from a file
The following command outputs some informations like samplerate, its format, etc. about an file.
Run it inside the directory containing the file.

```SH
$ ./sndconv --fileinfo="example.flac"
Samplerate:	44100 Hz
Channels:	2
Format:		FLAC, PCM 16 bit, Default file endian-ness.
```
