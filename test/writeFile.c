#include "rwstruct.h"

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
void countDirFile(char *basePath, NODES* node)
{
	char *path;
	struct dirent *dp;
	
	DIR *dir = opendir(basePath);
	if (!dir) { return; }

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			path = malloc(sizeof(basePath) + sizeof("/") + sizeof(dp->d_name));
			
			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			if (isDir(path))
			{
				//printf("DIR:: %s\n", dp->d_name);
				node->numFolder += 1;
			}
			
			if (isReg(path))
			{
				//printf("FILE:: %s\n", path);
				node->numFile += 1;
			}
			
			countDirFile(path, node);
			free(path);
		}
	}

	closedir(dir);
}

// Go through path recursively
void walkDirectory(char *basePath, FILESTRUCTURE* fstruct)
{
	char *path;
	struct dirent *dp;
	static unsigned int itrfile = 0;
	
	DIR *dir = opendir(basePath);
	if (!dir) { return; }

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			path = malloc(sizeof(basePath) + sizeof("/") + sizeof(dp->d_name));
			
			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			//if (isDir(path)) { printf("DIR:: %s\n", dp->d_name); }

			if (isReg(path))
			{
				//printf("FILE:: %s\n", path);
				strcpy(fstruct->files[itrfile], path);

				FILE* regfile = fopen(path, "r");
				if (regfile == NULL)
				{
					fstruct->sizes[itrfile] = 0L;
				} else
				{
					fseek(regfile, 0L, SEEK_END);
					fstruct->sizes[itrfile] = ftell(regfile);
				}

				fclose(regfile);
				itrfile++;
			}
			
			walkDirectory(path, fstruct);
			free(path);
		}
	}

	closedir(dir);
}


// later add special tags to specify files and folders
int main(int argc, char** argv)
{
	// Number of Files nad Folder
	NODES node;
	node.numFile = 0;
	node.numFolder = 0;

	// File Metadata
	METADATA meta;
	// Dummy values for metadata
	strcpy(meta.whoami, "TARim - KodeSpace:YouTube");
	meta.version = 0;
	meta.revision = 1;
	meta.encrypt = NO_ENCRYPT;

	// FIle Structure
	FILESTRUCTURE fstruct;

	// check if argments are files or folders & innclude them in the directory list
	// Open the directories
	for (unsigned int itr = 1; itr < argc; itr++)
	{
		countDirFile(argv[itr], &node);
	}

	// Initialise file structure
	fstruct.files = malloc(sizeof(char*) * node.numFile);
	for (unsigned int i = 0; i < node.numFile; i++)
	{ fstruct.files[i] = malloc(sizeof(char) * PATH_MAX); }

	fstruct.sizes = calloc(node.numFile, sizeof(unsigned long int));

	// Traverse directories
	for (unsigned int itr = 1; itr < argc; itr++)
	{
		walkDirectory(argv[itr], &fstruct);
	}

	// Assign fle structure size
	meta.size = sizeof(fstruct);

	// DEBUG -> print files and sizes from filestructure
	/*for (unsigned int itr = 0; itr < node.numFile; itr++)
	{
		printf("%s -> %lu\n", fstruct.files[itr], fstruct.sizes[itr]);
	}*/

	// Write Files to a default file
	FILE* archive = fopen("test.tarim", "wb");

	//printf("(sizeof) Metadata=%lu, FileStruct=%lu\n", sizeof(METADATA), sizeof(fstruct));

	// Write Metadata
	fwrite(&meta, sizeof(METADATA), 1, archive);
	// Write FileStructure
	fwrite(&fstruct, sizeof(fstruct), 1, archive);
	// Write Files
	for (unsigned int itr = 0; itr < node.numFile; itr++)
	{
		if (fstruct.sizes[itr] != 0)
		{
			printf("-> Writing '%s'\n", fstruct.files[itr]);
			FILE* in = fopen(fstruct.files[itr], "rb");
			if (in == NULL)
			{
				printf("(ERROR) Could not open file '%s'", fstruct.files[itr]);

				fclose(archive);

				// Free Space
				for (unsigned int i = 0; i < node.numFile; i++)
				{ free(fstruct.files[i]); }
				free(fstruct.files);

				free(fstruct.sizes);

				// EXIT
				exit(1);
			}

			unsigned char buffer[4];
			while (fread(buffer, sizeof(buffer), 1, in) == 1)
			{
				fwrite(buffer, sizeof(buffer), 1, archive);
			}

			fclose(in);
		}
	}

	fclose(archive);

	// Free Space
	for (unsigned int i = 0; i < node.numFile; i++)
	{ free(fstruct.files[i]); }
	free(fstruct.files);

	free(fstruct.sizes);
	
	return 0;
}