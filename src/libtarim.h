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
#ifndef TARIM_FILESAVE_TYPES
#define TARIM_FILESAVE_TYPES
#define FS_FILE 1
#define FS_FOLDER 2
#endif // TARIM_FILESAVE_TYPES

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
} TARIM_CRYPT_MODES;

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
} TARIM_METADATA;

typedef struct
{
	uint8_t type;
	char fpath[FP_MAX+1]; // File Path
	unsigned long int fsize; // Size of the File
} TARIM_FILESAVE;

#endif // LIBTARIM_STRUCT

//***********************+
//** FUNCTION PROTOTYPES |
//***********************+

// #############
// # decrypt.c #
// #############

// Decrypt File using AES-256
int decrypt_aes256(FILE*, FILE*, unsigned char*, unsigned char*, unsigned long long int, unsigned long int);
// Decrypt File using ARIA-256
int decrypt_aria256(FILE*, FILE*, unsigned char*, unsigned char*, unsigned long long int, unsigned long int);
// Decrypt File using Camellia-256
int decrypt_camellia256(FILE*, FILE*, unsigned char*, unsigned char*, unsigned long long int, unsigned long int);

// #############
// # encrypt.c #
// #############

// Encrypt File using AES-256
int encrypt_aes256(FILE*, FILE*, unsigned char*, unsigned char*);
// Encrypt File using ARIA-256
int encrypt_aria256(FILE*, FILE*, unsigned char*, unsigned char*);
// Encrypt File using Camellia-256
int encrypt_camellia256(FILE*, FILE*, unsigned char*, unsigned char*);

// ##########
// # func.c #
// ##########

// Get Password
static void get_password(char*);
// Generate 128 bit Initialization Vector
int gen_128_iv(unsigned char*);
// Generate 256 bit key
int gen_256_key(unsigned char**, void (*)(char*));
// Write Raw data from one to other
int nocrypt_write(FILE*, FILE*);
// Write Raw data from archive
int nocrypt_extractfile(FILE*, FILE*, unsigned long long int, unsigned long int);

// ###############
// # readwrite.c #
// ###############

// Identify Directory
static int isDir(const char*);
// Identify Regular File
static int isReg(const char*);
// Count Directories and Files
static void count_filefolder(char*, TARIM_METADATA*);
// Populate Tarim Filesave structure to an array
static void recursiveFilesaveArray(char*, long long int*, TARIM_FILESAVE*);
// Gather Files and Folders Objects
TARIM_FILESAVE* save_filefolder_metadata(const TARIM_METADATA, int, char**);
// Populate Metadata
int update_write_metadata(TARIM_METADATA*, TARIM_CRYPT_MODES, int, char**, FILE*);
// Read Metadata and File Database
TARIM_FILESAVE* read_metadata_filedb(TARIM_METADATA*, FILE*);
// Write File Database and Archive
int write_archive(const TARIM_METADATA, const TARIM_FILESAVE*, FILE*, unsigned char*);
// Extract File from Archie
int extract_file(const TARIM_METADATA, const TARIM_FILESAVE*, FILE*, unsigned char*, long long int);

#endif // LIBTARIM_H_