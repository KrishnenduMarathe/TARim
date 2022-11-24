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
static void recursiveFilesaveArray(char* basePath, long int* fc, TARIM_FILESAVE* fArray)
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
				for (long int itr = 0; itr < *fc; itr++)
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
	TARIM_FILESAVE* fileFolderObjects;
	long int fcount = 0;
	long int fileFolderObjectsSize = -1;

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
			for (long int itr2 = 0; itr2 < fcount; itr2++)
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
	if (gen_128_iv(meta->iv)) { return 1; }

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

// Write File Database and Archive
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
	for (long int itr = 0; itr < (meta.numFile + meta.numFolder); itr++)
	{
		fwrite(&fArray[itr], sizeof(TARIM_FILESAVE), 1, archive);
	}

	// Gather IV from metadata
	unsigned char iv[meta.iv_size];
	strncpy(iv, meta.iv, meta.iv_size);

	// Write Files to the Archive
	FILE* infile;
	for (long int itr = 0; itr < (meta.numFile + meta.numFolder); itr++)
	{
		if (fArray[itr].type == FS_FOLDER)
		{ continue; }

		// STDOUT Message
		printf("-> (write_archive) Writing '%s'\n", fArray[itr].fpath);
		
		infile = fopen(fArray[itr].fpath, "rb");
		if (infile == NULL)
		{
			printf("(ERROR) write_archive: Failed to open file '%s'\n", fArray[itr].fpath);
			return 1;
		}

		// Write File Data to the Archive
		switch (meta.encrypt)
		{
		case NO_ENCRYPT:
			if (nocrypt_write(infile, archive)) { return 1; }
			break;
		
		case AES_256_CBC:
			if (crypt_aes256(infile, archive, key, iv, 1)) { return 1; }
			break;
		
		case ARIA_256_CBC:
			if (crypt_aria256(infile, archive, key, iv, 1)) { return 1; }
			break;

		case CAMELLIA_256_CBC:
			if (crypt_camellia256(infile, archive, key, iv, 1)) { return 1; }
			break;
		
		default:
			break;
		}

		fclose(infile);
	}

	return 0;
}
