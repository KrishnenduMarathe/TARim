#ifndef RWSTRUCT_H
#define RWSTRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

// Encryption Modes
#ifndef ENCRYPTION_MODES
#define ENCRYPTION_MODES
#define NO_ENCRYPT 0
#endif // ENCRYPTION_MODES

// File Save Types
#ifndef FILESAVE_TYPES
#define FILESAVE_TYPES
#define FS_FILE 1
#define FS_FOLDER 2
#endif // FILESAVE_TYPES

// Max Characters for File Path (OS Dependent)
#ifndef FILEPATH_MAX_LEN
#define FILEPATH_MAX_LEN
#define FP_MAX PATH_MAX /* Linux */
#endif // FILEPATH_MAX_LEN

typedef struct
{
	char whoami[50]; // WHOAMI
	unsigned int version; // Version Number
	unsigned int revision; // Revision Number
	unsigned int encrypt; // Encryption flag
	unsigned int numFile; // Number of Files
	unsigned int numFolder; // Number of Folders
} METADATA;

typedef struct
{
	uint8_t type;
	char fpath[FP_MAX+1]; // File Path
	unsigned long int fsize; // Size of the File
} FILESAVE;

#endif // RWSTRUCT_H