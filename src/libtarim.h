#ifndef LIBTARIM_H_
#define LIBTARIM_H_

//*****************+
//** PREPROCESSORS |
//*****************+

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define VERSION 1
#define REVISION 0

#define CLEARSCREEN "clear" // Linux

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


//****************+
//** C STRUCTURES |
//****************+

#ifndef LIBTARIM_STRUCT
#define LIBTARIM_STRUCT

typedef enum {
	NO_ENCRYPT,
	AES_256_CBC,
	ARIA_256_CBC,
	CAMELLIA_256_CBC
} CRYPT_MODES;

typedef struct
{
	char whoami[50]; // WHOAMI
	
	unsigned int version; // Version Number
	unsigned int revision; // Revision Number
	
	uint8_t encrypt; // Encryption flag
	
	unsigned int numFile; // Number of Files
	unsigned int numFolder; // Number of Folders
	
	uint8_t iv_size; // IV Size
	unsigned char iv[256]; // IV
} METADATA;

typedef struct
{
	uint8_t type;
	char fpath[FP_MAX+1]; // File Path
	unsigned long int fsize; // Size of the File
} FILESAVE;

#endif // LIBTARIM_STRUCT

//***********************+
//** FUNCTION PROTOTYPES |
//***********************+

// Get Password
static void get_password(char*);

// Count Directories and Files
static void count_filefolder(char*, METADATA*);

// Identify Directory
int isDir(const char*);

// Identify Regular File
int isReg(const char*);

// Save File Structure Element
void saveFileELement(char*, FILE*, uint8_t);

// Populate Metadata
int update_write_metadata(METADATA*, CRYPT_MODES, int, char**, FILE*);

// Generate 256 bit key
int gen_256_key(unsigned char**, void(*)(char*));

// Generate 16 bit Initialization Vector
int gen_16_iv(unsigned char**);

// Encrypt File using AES-256
int crypt_aes256(FILE*, FILE*, unsigned char*, unsigned char*, int);

// Encrypt File using ARIA-256
int crypt_aria256(FILE*, FILE*, unsigned char*, unsigned char*, int);

// Encrypt File using Camellia-256
int crypt_camellia256(FILE*, FILE*, unsigned char*, unsigned char*, int);

#endif // LIBTARIM_H_