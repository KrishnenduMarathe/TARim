// AUTHOR: Krishnendu Marathe
// PROGRAM: TARIM

/*
** 							  TARIM
** 							  -----
** 	  Archive Manager for custom tape archive format with added features
**    ".tarim" (for  tar improved).
**
** 					 	  	 License
** 	  						 -------
**
** 	  This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

// Max Characters for File Path
#define FP_MAX 4096

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
int decrypt_aes256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize);
// Decrypt File using ARIA-256
int decrypt_aria256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize);
// Decrypt File using Camellia-256
int decrypt_camellia256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize);

// #############
// # encrypt.c #
// #############

// Encrypt File using AES-256
int encrypt_aes256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv);
// Encrypt File using ARIA-256
int encrypt_aria256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv);
// Encrypt File using Camellia-256
int encrypt_camellia256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv);

// ##########
// # func.c #
// ##########

// Get Password
static void get_password(char* passwd);
// Generate 128 bit Initialization Vector
unsigned char* gen_128_iv();
// Generate 256 bit key
unsigned char* gen_256_key(void (*get_pass)(char*));
// Write Raw data from one to other
int nocrypt_write(FILE* infile, FILE* outfile);
// Write Raw data from archive
int nocrypt_extractfile(FILE* infile, FILE* outfile, unsigned long long int fileLoc, unsigned long int fileSize);

// ###############
// # readwrite.c #
// ###############

// Identify Directory
static int isDir(const char* filePath);
// Identify Regular File
static int isReg(const char* filePath);
// Count Directories and Files
static void count_filefolder(char* basePath, TARIM_METADATA* meta);
// Populate Tarim Filesave structure to an array
static void recursiveFilesaveArray(char* basePath, long long int* fc, TARIM_FILESAVE* fArray);
// Gather Files and Folders Objects
TARIM_FILESAVE* save_filefolder_metadata(const TARIM_METADATA meta, int arg_num, char** args);
// Populate Metadata
int update_write_metadata(TARIM_METADATA* meta, TARIM_CRYPT_MODES mode, int arg_num, char** args, FILE* archive);
// Read Metadata and File Database
TARIM_FILESAVE* read_metadata_filedb(TARIM_METADATA* meta, FILE* archive);
// Write File Database and File Data in Archive
int write_archive(const TARIM_METADATA meta, const TARIM_FILESAVE* fArray, FILE* archive, unsigned char* key);
// Extract File from Archie
int extract_file(const TARIM_METADATA meta, const TARIM_FILESAVE* fArray, FILE* archive, unsigned char* key, long long int option_num, char* basePath);

#endif // LIBTARIM_H_
