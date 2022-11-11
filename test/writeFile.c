#include "../src/rwstruct.h"

// Identify Directory
int isDir(const char* filePath)
{
	struct stat path;
	stat(filePath, &path);
	return S_ISDIR(path.st_mode);
}

// Identify Regular File
int isReg(const char* filePath)
{
	struct stat path;
	stat(filePath, &path);
	return S_ISREG(path.st_mode);
}

// Count Directory and Files
void countDirFile(char *basePath, METADATA* meta)
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
				printf("(ERROR) Cannot allocate memory to countDirFile's character array\n");
				exit(1);
			}

			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			if (isDir(path))
			{
				//printf("DIR:: %s\n", dp->d_name);
				meta->numFolder += 1;
			}
			
			if (isReg(path))
			{
				//printf("FILE:: %s\n", path);
				meta->numFile += 1;
			}
			
			countDirFile(path, meta);
			free(path);
		}
	}

	closedir(dir);
}

// Save File Structure Element
void saveFileELement(char* filePath, FILE* archive, uint8_t ftype)
{
	FILESAVE fsave;
	fsave.type = ftype;
	strncpy(fsave.fpath, filePath, FP_MAX);
	fsave.fpath[FP_MAX] = '\0';

	if (ftype == FS_FILE)
	{
		FILE* regfile = fopen(filePath, "r");
		if (regfile == NULL)
		{
			fsave.fsize = 0L;
		} else {
			fseek(regfile, 0L, SEEK_END);
			fsave.fsize = ftell(regfile);
		}
		fclose(regfile);
	} else
	{ fsave.fsize = 0L; }

	fwrite(&fsave, sizeof(FILESAVE), 1, archive);
}

// Read Folder Structure
void writeFolderStructure(char* basePath, unsigned int* fc, METADATA* meta, FILE* archive, char** folderPaths)
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
				printf("(ERROR) Cannot allocate memory to writeFileStructure's character array\n");
				exit(1);
			}

			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			path[(sizeof(basePath)+sizeof("/")+sizeof(dp->d_name)) / sizeof(char)] = '\0';

			if (isDir(path))
			{
				int flag = 1;
				for (unsigned int itr = 0; itr < *fc; itr++)
				{
					if (strcmp(folderPaths[itr], path) == 0)
					{
						flag = 0;
						break;
					}
				}
				if (flag == 1)
				{
					saveFileELement(path, archive, FS_FOLDER);

					// Add path to the folderPathsArray
					strcpy(folderPaths[*fc], path);
					*fc += 1;
				}
			}
			
			writeFolderStructure(path, fc, meta, archive, folderPaths);
			free(path);
		}
	}

	closedir(dir);
}

// Write File Structure
void writeFileStructure(char* basePath, unsigned int* fc, METADATA* meta, FILE* archive, char** filePaths)
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
				printf("(ERROR) Cannot allocate memory to writeFileStructure's character array\n");
				exit(1);
			}

			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			path[(sizeof(basePath)+sizeof("/")+sizeof(dp->d_name)) / sizeof(char)] = '\0';

			if (isReg(path))
			{
				saveFileELement(path, archive, FS_FILE);

				// Add path to the filePathsArray
				strcpy(filePaths[*fc], path);
				*fc += 1;
			}
			
			writeFileStructure(path, fc, meta, archive, filePaths);
			free(path);
		}
	}

	closedir(dir);
}

// Add Support for Encryption Later
// Check if arguments are files in the main program
int main(int argc, char** argv)
{
	METADATA meta;

	// Initiate Metadata values
	strcpy(meta.whoami, "TARim: KodeSpace");
	meta.version = 0;
	meta.revision = 1;
	meta.encrypt = NO_ENCRYPT;
	meta.numFile = 0;
	meta.numFolder = 0;

	// Count All Directories and Folders
	for (unsigned int itr = 1; itr < argc; itr++)
	{
		if (isDir(argv[itr])) { meta.numFolder += 1; }
		if (isReg(argv[itr])) { meta.numFile += 1; }
		countDirFile(argv[itr], &meta);
	}

	// Create Archive
	FILE* archive = fopen("test.tarim", "wb");
	if (archive == NULL)
	{
		printf("(ERROR) Failed to create directory.\n");
		exit(1);
	}

	// Write Metadata
	fwrite(&meta, sizeof(METADATA), 1, archive);
	// Debug
	printf("METADATA:\n");
	printf("\tWHO: %s\n\tVer: %u\n\tRev: %u\n\tENCRYPT: %u\n\tFiles: %u\n\tFolders: %u\n\n", meta.whoami, meta.version, meta.revision, meta.encrypt, meta.numFile, meta.numFolder);

	// Folder Paths Array
	char** folderPathArray = (char**) malloc(sizeof(char*) * meta.numFolder);
	if (folderPathArray == NULL)
	{
		printf("(ERROR) Cannot allocate memory to Folder Path Array.\n");
		exit(1);
	}
	for (unsigned int itr = 0; itr < meta.numFolder; itr++)
	{
		folderPathArray[itr] = (char*) malloc(sizeof(char) * (FP_MAX+1));
		if (folderPathArray[itr] == NULL)
		{
			for (unsigned int itr2 = 0; itr2 < itr; itr2++)
			{
				free(folderPathArray[itr2]);
			}
			free(folderPathArray);

			printf("(ERROR) Cannot Allocate memeory for folder path{%u}\n", itr);
			exit(1);
		}
	}

	// Write Folder Structure
	unsigned int fcount = 0;
	for (unsigned int itr = 1; itr < argc; itr++)
	{
		if (isDir(argv[itr]))
		{
			saveFileELement(argv[itr], archive, FS_FOLDER);
			strcpy(folderPathArray[fcount], argv[itr]);
			fcount++;
		}
	}
	for (unsigned int itr = 1; itr < argc; itr++)
	{ writeFolderStructure(argv[itr], &fcount, &meta, archive, folderPathArray); }

	// Free Allocated Space
	for (unsigned int itr = 0; itr < meta.numFolder; itr++)
	{ free(folderPathArray[itr]); }
	free(folderPathArray);

	// File Paths Array
	char** filePathsArray = (char**) malloc(sizeof(char*) * meta.numFile);
	if (filePathsArray == NULL)
	{
		printf("(ERROR) Cannot allocate memory to File Path Array.\n");
		exit(1);
	}
	for (unsigned int itr = 0; itr < meta.numFile; itr++)
	{
		filePathsArray[itr] = (char*) malloc(sizeof(char) * (FP_MAX+1));
		if (filePathsArray[itr] == NULL)
		{
			for (unsigned int itr2 = 0; itr2 < itr; itr2++)
			{
				free(filePathsArray[itr2]);
			}
			free(filePathsArray);
			
			printf("(ERROR) Cannot Allocate memeory for file path{%u}\n", itr);
			exit(1);
		}
	}

	// Write File Structure
	fcount = 0;
	for (unsigned int itr = 1; itr < argc; itr++)
	{
		if (isReg(argv[itr]))
		{
			saveFileELement(argv[itr], archive, FS_FILE);
			strcpy(filePathsArray[fcount], argv[itr]);
			fcount++;
		}
	}
	for (unsigned int itr = 1; itr < argc; itr++)
	{ writeFileStructure(argv[itr], &fcount, &meta, archive, filePathsArray); }

	// Write Files
	for (unsigned int itr = 0; itr < meta.numFile; itr++)
	{
		// DEBUG
		//printf("%u. %s\n", itr, filePathsArray[itr]);

		printf("-> Writing '%s'\n", filePathsArray[itr]);

		FILE* in = fopen(filePathsArray[itr], "rb");
		if (in == NULL)
		{
			printf("(ERROR) Could not open file '%s'\n", filePathsArray[itr]);
			break;
		}

		unsigned char buffer[4];
		while (fread(buffer, sizeof(buffer), 1, in) == 1)
		{ fwrite(buffer, sizeof(buffer), 1, archive); }

		fclose(in);
	}
	fclose(archive);

	// Free filePathsArray
	for (unsigned int itr = 0; itr < meta.numFile; itr++)
	{ free(filePathsArray[itr]); }
	free(filePathsArray);

	return 0;
}