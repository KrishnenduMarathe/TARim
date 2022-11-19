#include "../src/rwstruct.h"

// Extract Files from Archive
void extractArchive(FILE* archive, METADATA* meta)
{
	FILESAVE element;
	
	// Create Directory Structure on disk -- later add check for selective extractions and skip this step
	for (unsigned int itr = 0; itr < meta->numFolder; itr++)
	{
		fread(&element, sizeof(FILESAVE), 1, archive);
		if (element.type != FS_FOLDER)
		{
			printf("(ERROR) Corrupted Folder Structure data\n");
			fclose(archive);
			exit(1);
		}

		DIR* doesitexist = opendir(element.fpath);
		if (doesitexist)
		{ closedir(doesitexist); }
		else {
			closedir(doesitexist);

			// DEBUG
			//printf("FOLDER: '%s'\n", element.fpath);

			if (mkdir(element.fpath, 0777))
			{
				printf("(ERROR) Creating Folder '%s' failed\n", element.fpath);
				fclose(archive);
				exit(1);
			}
		}
	}

	// Read File Structure
	FILESAVE* fileMetadata = (FILESAVE*) malloc(sizeof(FILESAVE) * meta->numFile);
	if (fileMetadata == NULL)
	{
		printf("(ERROR) Cannot allocate memory to File Metadata.\n");
		exit(1);
	}
	
	// Store in fileMetadata
	for (unsigned int itr = 0; itr < meta->numFile; itr++)
	{
		fread(&fileMetadata[itr], sizeof(FILESAVE), 1, archive);

		// DEBUG
		//printf("FILE[%u]: '%s' (%lu)\n", fileMetadata[itr].type, fileMetadata[itr].fpath, fileMetadata[itr].fsize);
	}

	// Extract Files
	unsigned char buffer;
	unsigned long int count;

	/*unsigned long long int pos = 0L;
	pos += sizeof(*meta); // consider metadata
	pos += (meta->numFolder * sizeof(FILESAVE)); // consider folder structure
	pos += (meta->numFile * sizeof(FILESAVE)); // consider file structure
	
	fseek(archive, pos, SEEK_SET);*/

	for (unsigned int itr = 0; itr < meta->numFile; itr++)
	{
		FILE* out = fopen(fileMetadata[itr].fpath, "wb");
		if (out == NULL || fileMetadata[itr].type != FS_FILE)
		{
			printf("(ERROR) Faied to extract file '%s'\n", fileMetadata[itr].fpath);
			break;
		} else {
			printf("-> Extracting '%s' (%lu)\n", fileMetadata[itr].fpath, fileMetadata[itr].fsize);
		}
		
		count = 0L;
		while (!feof(archive))
		{
			if (count >= fileMetadata[itr].fsize)
			{ break; }

			fread(&buffer, sizeof(unsigned char), 1, archive);
			fwrite(&buffer, sizeof(unsigned char), 1, out);
			count += sizeof(unsigned char);
			//pos += sizeof(unsigned char);
		}

		fclose(out);
	}

	// Free Allocated Memory
	free(fileMetadata);
}

int main(int argc, char** argv)
{
	// Check number of arguments
	if (argc == 1)
	{
		printf("No Arguments passed!\n");
		exit(0);
	} else {
		if (argc > 2)
		{
			printf("Too many arguments\n");
			exit(0);
		}
	}

	METADATA meta;

	// Open Archive
	FILE* archive = fopen(argv[1], "rb");
	if (archive == NULL)
	{
		printf("(ERROR) Cannot open archive file.\n");
		exit(1);
	}

	// Read Metadata
	fread(&meta, sizeof(METADATA), 1, archive);
	// Debug
	printf("METADATA:\n");
	printf("\tWHO: %s\n\tVer: %u\n\tRev: %u\n\tENCRYPT: %u\n\tFiles: %u\n\tFolders: %u\n\n", meta.whoami, meta.version, meta.revision, meta.encrypt, meta.numFile, meta.numFolder);

	// Extract Files
	extractArchive(archive, &meta);

	fclose(archive);
	return 0;
}