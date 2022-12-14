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

#include "../libtarim.h"

// Identify Directory
static int isDir(const char* filePath)
{
	struct stat path;
	stat(filePath, &path);
	return S_ISDIR(path.st_mode);
}

// Identify Regular File
static int isReg(const char* filePath)
{
	struct stat path;
	stat(filePath, &path);
	return S_ISREG(path.st_mode);
}

// Count Directories and Files
static void count_filefolder(char* basePath, TARIM_METADATA* meta)
{
	char *path;
	struct dirent *dp;
	
	DIR *dir = opendir(basePath);
	if (!dir) { return; }

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			path = (char*) malloc(sizeof(basePath) + sizeof("/") + sizeof(dp->d_name));
			if (path == NULL)
			{
				printf("(ERROR) count_filefolder: Cannot allocate memory to character array\n");
				return;
			}

			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			if (isDir(path))
			{ meta->numFolder += 1; }
			
			if (isReg(path))
			{ meta->numFile += 1; }
			
			count_filefolder(path, meta);
			free(path);
		}
	}

	closedir(dir);
}

// Populate Tarim Filesave structure to an array
static void recursiveFilesaveArray(char* basePath, long long int* fc, TARIM_FILESAVE* fArray)
{
	char *path;
	struct dirent *dp;
	
	DIR *dir = opendir(basePath);
	if (!dir) { return; }

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			path = (char*) malloc(FP_MAX+1);
			if (path == NULL)
			{
				printf("(ERROR) recursiveFilesaveArray: Cannot allocate memory to writeFileStructure's character array\n");
				exit(1);
			}

			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			path[(sizeof(basePath)+sizeof("/")+sizeof(dp->d_name)) / sizeof(char)] = '\0';

			if (isDir(path))
			{
				uint8_t flag = 1;
				for (long long int itr = 0; itr < *fc; itr++)
				{
					if (strcmp(fArray[itr].fpath, path) == 0)
					{
						flag = 0;
						break;
					}
				}
				if (flag)
				{
					fArray[*fc].type = FS_FOLDER;

					strncpy(fArray[*fc].fpath, path, FP_MAX);
					fArray[*fc].fpath[FP_MAX] = '\0';

					fArray[*fc].fsize = 0L;
					*fc += 1;
				}
			}

			if (isReg(path))
			{
				fArray[*fc].type = FS_FILE;
				
				strncpy(fArray[*fc].fpath, path, FP_MAX);
				fArray[*fc].fpath[FP_MAX] = '\0';

				FILE* regfile = fopen(path, "rb");
				if (regfile == NULL)
				{
					fArray[*fc].fsize = 0L;
				} else {
					fseek(regfile, 0L, SEEK_END);
					fArray[*fc].fsize = ftell(regfile);
				}
				fclose(regfile);
				*fc += 1;
			}

			recursiveFilesaveArray(path, fc, fArray);
			free(path);
		}
	}

	closedir(dir);
}

// Gather Files and Folders Objects
TARIM_FILESAVE* save_filefolder_metadata(const TARIM_METADATA meta, int arg_num, char** args)
{
	TARIM_FILESAVE* fileFolderObjects = NULL;
	long long int fcount = 0;
	long long int fileFolderObjectsSize = -1;

	fileFolderObjectsSize = meta.numFile + meta.numFolder;
	fileFolderObjects = (TARIM_FILESAVE* ) calloc(fileFolderObjectsSize, sizeof(TARIM_FILESAVE));
	if (fileFolderObjects == NULL)
	{
		printf("(ERROR) get_all_filefolder: Failed to allocate memory to fileFolderObjects Array\n");
		return NULL;
	}

	for (unsigned int itr = 0; itr < arg_num; itr++)
	{

		if (isDir(args[itr]))
		{
			uint8_t flag = 1;
			for (long long int itr2 = 0; itr2 < fcount; itr2++)
			{
				if (strcmp(fileFolderObjects[itr2].fpath, args[itr]) == 0)
				{
					flag = 0;
					break;
				}
			}
			if (flag)
			{
				fileFolderObjects[fcount].type = FS_FOLDER;

				strncpy(fileFolderObjects[fcount].fpath, args[itr], FP_MAX);
				fileFolderObjects[fcount].fpath[FP_MAX] = '\0';

				fileFolderObjects[fcount].fsize = 0L;
				fcount += 1;
			}
		}
		if (isReg(args[itr]))
		{
			fileFolderObjects[fcount].type = FS_FILE;
				
			strncpy(fileFolderObjects[fcount].fpath, args[itr], FP_MAX);
			fileFolderObjects[fcount].fpath[FP_MAX] = '\0';

			FILE* regfile = fopen(args[itr], "rb");
			if (regfile == NULL)
			{
				fileFolderObjects[fcount].fsize = 0L;
			} else {
				fseek(regfile, 0L, SEEK_END);
				fileFolderObjects[fcount].fsize = ftell(regfile);
			}
			fclose(regfile);
			fcount += 1;
		}
		
		recursiveFilesaveArray(args[itr], &fcount, fileFolderObjects);
	}

	return fileFolderObjects;
}

// Populate Metadata
int update_write_metadata(TARIM_METADATA* meta, TARIM_CRYPT_MODES mode, int arg_num, char** args, FILE* archive)
{
	if (meta == NULL)
	{
		printf("(ERROR) update_write_metadata: No Metadata was passed\n");
		return 1;
	}
	if (archive == NULL)
	{
		printf("(ERROR) update_write_metadata: Archive is a NULL Pointer\n");
		return 1;
	}

	// Initiate Metadata values
	strcpy(meta->whoami, "TARim: KodeSpace");
	meta->version = VERSION;
	meta->revision = REVISION;
	meta->encrypt = mode;
	meta->numFile = 0;
	meta->numFolder = 0;
	meta->iv_size = 16;

	unsigned char* iv = gen_128_iv();
	if (iv == NULL) { return 1; }
	strncpy(meta->iv, iv, meta->iv_size);

	for (unsigned int itr = 0; itr < arg_num; itr++)
	{

		if (isDir(args[itr])) { meta->numFolder += 1; }
		if (isReg(args[itr])) { meta->numFile += 1; }
		
		count_filefolder(args[itr], meta);
	}

	// Write Metadata
	fwrite(meta, sizeof(TARIM_METADATA), 1, archive);
	return 0;
}

// Read Metadata and File Database
TARIM_FILESAVE* read_metadata_filedb(TARIM_METADATA* meta, FILE* archive)
{
	if (meta == NULL)
	{
		printf("(ERROR) read_metadata_filedb: Metadata is not initialised\n");
		return NULL;
	}
	if (archive == NULL)
	{
		printf("(ERROR) read_metadata_filedb: Archive is a NULL Pointer\n");
		return NULL;
	}

	TARIM_FILESAVE* fileFolderObjects = NULL;
	long long int fileFolderObjectsSize = -1;

	// Read Metadata
	fread(meta, sizeof(TARIM_METADATA), 1, archive);

	fileFolderObjectsSize = meta->numFile + meta->numFolder;
	fileFolderObjects = (TARIM_FILESAVE* ) calloc(fileFolderObjectsSize, sizeof(TARIM_FILESAVE));
	if (fileFolderObjects == NULL)
	{
		printf("(ERROR) read_metadata_filedb: Failed to allocate memory to fileFolderObjects Array\n");
		return NULL;
	}

	// Read File and Folder Metadata
	for (long long int itr = 0; itr < fileFolderObjectsSize; itr++)
	{
		fread(&fileFolderObjects[itr], sizeof(TARIM_FILESAVE), 1, archive);
	}

	return fileFolderObjects;
}

// Write File Database and File Data in Archive
int write_archive(const TARIM_METADATA meta, const TARIM_FILESAVE* fArray, FILE* archive, unsigned char* key)
{
	if (fArray == NULL)
	{
		printf("(ERROR) write_archive: File-Folder Object array is NULL Pointer\n");
		return 1;
	}
	if (archive == NULL)
	{
		printf("(ERROR) write_archive: Archive is a NULL Pointer\n");
		return 1;
	}

	// Write File and Folder Metadata
	for (long long int itr = 0; itr < (meta.numFile + meta.numFolder); itr++)
	{
		fwrite(&fArray[itr], sizeof(TARIM_FILESAVE), 1, archive);
	}

	// Gather IV from metadata
	unsigned char iv[meta.iv_size];
	strncpy(iv, meta.iv, meta.iv_size);

	// Write Files to the Archive
	FILE* infile;
	for (long long int itr = 0; itr < meta.numFile+meta.numFolder; itr++)
	{
		if (fArray[itr].type != FS_FILE)
		{ continue; }

		infile = fopen(fArray[itr].fpath, "rb");
		if (infile == NULL)
		{
			printf("(ERROR) write_archive: Failed to open file '%s'\n", fArray[itr].fpath);
			return 1;
		}

		// Write File Data to the Archive
		if (meta.encrypt != NO_ENCRYPT && key == NULL)
		{
			printf("(ERROR) write_archive: NULL key passed for encryption\n");
			fclose(infile);
			return 1;
		}
		
		switch (meta.encrypt)
		{
		case NO_ENCRYPT:
			if (nocrypt_write(infile, archive)) { return 1; }
			break;
		
		case AES_256_CBC:
			if (encrypt_aes256(infile, archive, key, iv)) { return 1; }
			break;
		
		case ARIA_256_CBC:
			if (encrypt_aria256(infile, archive, key, iv)) { return 1; }
			break;

		case CAMELLIA_256_CBC:
			if (encrypt_camellia256(infile, archive, key, iv)) { return 1; }
			break;
		
		default:
			break;
		}

		fclose(infile);

		int percent_done = (int) ((itr+1)*100/(meta.numFile+meta.numFolder));
		update_progress_bar(percent_done, fArray[itr].fpath, sizeof(fArray[itr].fpath));
	}

	return 0;
}

// Extract File from Archie
int extract_file(const TARIM_METADATA meta, const TARIM_FILESAVE* fArray, FILE* archive, unsigned char* key, long long int option_num, char* basePath)
{
	if (fArray == NULL)
	{
		printf("(ERROR) extract_file: File-Folder Object array is NULL Pointer\n");
		return 1;
	}
	if (archive == NULL)
	{
		printf("(ERROR) extract_file: Archive is a NULL Pointer\n");
		return 1;
	}
	if (option_num < 0 || option_num >= meta.numFile+meta.numFolder)
	{
		printf("(ERROR) extract_file: File Index out of bounds\n");
		return 1;
	}
	if (basePath == NULL)
	{
		printf("(ERROR) extract_file: basePath is a NULL Pointer\n");
		return 1;
	}
	if (fArray[option_num].type == FS_FOLDER)
	{
		printf("(ERROR) extract_file: Invalid Object type passed for extaction\n");
		return 1;
	}

	// Gather IV from metadata
	unsigned char iv[meta.iv_size];
	strncpy(iv, meta.iv, meta.iv_size);

	// Get Cipher Block Size
	unsigned int block_size;
	EVP_CIPHER* cipher;
	switch (meta.encrypt)
	{
	case NO_ENCRYPT:
		block_size = 0;
		break;
		
	case AES_256_CBC:
		
		cipher = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
		if (cipher == NULL)
		{
			printf("(ERROR) extract_file: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			return 1;
		}
		block_size = EVP_CIPHER_block_size(cipher);
		EVP_CIPHER_free(cipher);
		break;
		
	case ARIA_256_CBC:
		
		cipher = EVP_CIPHER_fetch(NULL, "ARIA-256-CBC", NULL);
		if (cipher == NULL)
		{
			printf("(ERROR) extract_file: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			return 1;
		}
		block_size = EVP_CIPHER_block_size(cipher);
		EVP_CIPHER_free(cipher);
		break;

	case CAMELLIA_256_CBC:
		
		cipher = EVP_CIPHER_fetch(NULL, "CAMELLIA-256-CBC", NULL);
		if (cipher == NULL)
		{
			printf("(ERROR) extract_file: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			return 1;
		}
		block_size = EVP_CIPHER_block_size(cipher);
		EVP_CIPHER_free(cipher);
		break;
	
	default:
		break;
	}

	// Get start location
	unsigned long long int startLoc = (meta.numFile + meta.numFolder) * sizeof(TARIM_FILESAVE) + sizeof(TARIM_METADATA);

	// Get Relative Location
	unsigned long long int relativeLoc = 0;
	for (long long int itr = 0; itr < option_num; itr++)
	{
		if (block_size == 0 || fArray[itr].fsize == 0)
		{
			relativeLoc += fArray[itr].fsize;
		} else {
			// Adjust for padding
			relativeLoc += ((fArray[itr].fsize / block_size) + 1) * block_size;
		}
	}

	// Create Directory Structre necessary for extraction - sloppy
	for (long long int itr = 0; itr <= option_num; itr++)
	{
		if (fArray[itr].type != FS_FOLDER)
		{ continue; }

		DIR* doesitexist = opendir(fArray[itr].fpath);
		if (!doesitexist)
		{
			if (mkdir(fArray[itr].fpath, 0777))
			{
				printf("(ERROR) extract_file: Failed creating folder '%s'\n", fArray[itr].fpath);
				closedir(doesitexist);
				return 1;
			}
		}
		closedir(doesitexist);
	}

	// STDOUT Message
	//printf("-> (extract_file) Extracting '%s'\n", fArray[option_num].fpath);
	
	// Extract Path
	char extractPath[8192];
	strcpy(extractPath, basePath);
	strcat(extractPath, fArray[option_num].fpath);
	FILE* outfile = fopen(extractPath, "wb");
	if (outfile == NULL)
	{
		printf("(ERROR) extract_file: Failed to open file '%s'\n", extractPath);
		return 1;
	}
	
	// Start Extraction of the file
	if (meta.encrypt != NO_ENCRYPT && key == NULL)
	{
		printf("(ERROR) extract_file: NULL key passed for decryption\n");
		fclose(outfile);
		return 1;
	}
	
	switch (meta.encrypt)
	{
	case NO_ENCRYPT:
		if (nocrypt_extractfile(archive, outfile, startLoc+relativeLoc, fArray[option_num].fsize)) { return 1; }
		break;
		
	case AES_256_CBC:
		if (decrypt_aes256(archive, outfile, key, iv, startLoc+relativeLoc, fArray[option_num].fsize)) { return 1; }
		break;
		
	case ARIA_256_CBC:
		if (decrypt_aria256(archive, outfile, key, iv, startLoc+relativeLoc, fArray[option_num].fsize)) { return 1; }
		break;

	case CAMELLIA_256_CBC:
		if (decrypt_camellia256(archive, outfile, key, iv, startLoc+relativeLoc, fArray[option_num].fsize)) { return 1; }
		break;
	
	default:
		break;
	}

	fclose(outfile);
	return 0;
}
