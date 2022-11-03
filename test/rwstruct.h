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

typedef struct
{
	unsigned int numFile;
	unsigned int numFolder;
} NODES;

typedef struct
{
	// WhoAmI
	char whoami[50];

	// Version of the software used to package the file
	unsigned int version;
	unsigned int revision;

	// Encryption used
	unsigned int encrypt;

	// Size of File Structure
	unsigned long int size;
} METADATA;

typedef struct
{
	char** files;
	unsigned long int* sizes;
} FILESTRUCTURE;

#endif // RWSTRUCT_H