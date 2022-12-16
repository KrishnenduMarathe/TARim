// AUTHOR: Krishnendu Marathe
// PROGRAM: TARIM Archve Manager (TAM)

/*
** 					 TARIM Archive Manager (TAM)
** 					 ---------------------------
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

#include "tam.h"

int main(int argc, char** argv)
{
	// Atleast 2 Arguments Accepted
	if (argc < 2)
	{
		printf("\nInvalid Number of Arguments\n\t(Use 'tam.x86/tam.x64 -h' for more information.)\n");
		return 1;
	}

	// Help Text
	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		help_text();
		return 0;
	}

	// Verify Arguments
	int create_flag = 0;
	int extract_flag = 0;
	int encrypt_flag = 0;
	int out_flag = 0;
	int view_flag = 0;

	int option_num = 0;
	int create_init = 0;
	int create_count = 0;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--create") == 0)
		{
			create_flag = 1;
			create_init = 1;
			if (extract_flag)
			{
				printf("\nCannot Create and Extract Archive together\n");
				return 0;
			}
			if (view_flag)
			{
				printf("\nCannot Create and View Archive together\n");
				return 0;
			}
			option_num++;
		}
		else if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--extract") == 0)
		{
			extract_flag = 1;
			create_init = 0;
			if (create_flag)
			{
				printf("\nCannot Extract and Create Archive together\n");
				return 0;
			}
			if (view_flag)
			{
				printf("\nCannot Extract and View Archive together\n");
				return 0;
			}
			if (encrypt_flag)
			{
				printf("\nCannot Extract and operate Encryption together\n");
				return 0;
			}
			option_num++;
		}
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--view") == 0)
		{
			view_flag = 1;
			create_init = 0;
			if (create_flag)
			{
				printf("\nCannot View and Create Archive together\n");
				return 0;
			}
			if (extract_flag)
			{
				printf("\nCannot View and Extract Archive together\n");
				return 0;
			}
			if (encrypt_flag)
			{
				printf("\nCannot View and operate Encryption together\n");
				return 0;
			}
			if (out_flag)
			{
				printf("\nCannot Vew and Specify Output together\n");
				return 0;
			}
			option_num++;
		}
		else
		{
			if (strcmp(argv[i], "-E") == 0 || strcmp(argv[i], "--encrypt") == 0)
			{
				encrypt_flag = 1;
				create_init = 0;
				if (view_flag)
				{
					printf("\nCannot Encrypt and View Archive together\n");
					return 0;
				}
				if (extract_flag)
				{
					printf("\nNo need to pass Encrption Algorithm when Extracting\n");
					return 0;
				}
			}
			else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--out") == 0)
			{
				out_flag = 1;
				create_init = 0;
				if (view_flag)
				{
					printf("\nCannot Specify Output and View Archive together\n");
					return 0;
				}
			}
			else
			{
				if (create_flag && create_init)
				{
					create_count++;
				}
			}
		}
	}

	// Exit if no valid options
	if (option_num == 0)
	{
		printf("\nInvalid Command Arguments\n");
		return 0;
	}
	if (create_flag && create_count <= 0)
	{
		printf("\nCreate Operaton should have atleast one Argument\n");
		return 0;
	}
	
	// Read Argments
	int read_args = 0;
	int read_itr = 0;
	if (create_count == 0)
	{ create_count = 1; }

	char** readArguments = (char**) malloc(sizeof(char*)*create_count);
	if (readArguments == NULL)
	{
		printf("\nFailed to allocate memory to create arguments\n");
		return 1;

	}
	for (int i = 0; i < create_count; i++)
	{
		readArguments[i] = (char*) malloc(sizeof(char)*FP_MAX);
		if (readArguments[i] == NULL)
		{
			printf("\nFailed to allocate memory to create arguments at stage %d\n", i);
			for (int j = 0; j < i; j++)
			{
				free(readArguments[j]);
			}
			free(readArguments);
			return 1;
		}
	}

	char extractArgument[FP_MAX];
	TARIM_CRYPT_MODES encryptionMode = NO_ENCRYPT;
	char outArgument[FP_MAX];
	char viewArgument[FP_MAX];

	for (int i = 1; i < argc; i++)
	{
		// Read Arguments
		if (read_args == 1)
		{
			strcpy(readArguments[read_itr], argv[i]);
			read_itr++;

			if (read_itr >= create_count)
			{
				read_args = 0;
			}
		}
		else if (read_args == 2)
		{
			read_args = 0;
			continue;
		}

		if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--create") == 0)
		{
			read_args = 1;
		}
		if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--extract") == 0)
		{
			read_args = 2;
			if (i+1 >= argc)
			{
				printf("\nExtract Operaton has no Argument\n");
				for (int i = 0; i < create_count; i++)
				{
					free(readArguments[i]);
				}
				free(readArguments);
				return 1;
			}
			strcpy(extractArgument, argv[i+1]);
		}
		if (strcmp(argv[i], "-E") == 0 || strcmp(argv[i], "--encrypt") == 0)
		{
			//Algorithms: aes256, aria256 & camellia256
			read_args = 2;
			if (i+1 >= argc)
			{
				printf("\nAlgorithm not specified\n");
				for (int i = 0; i < create_count; i++)
				{
					free(readArguments[i]);
				}
				free(readArguments);
				return 1;
			}
			
			if (strcmp(argv[i+1], "aes256") == 0)
			{
				encryptionMode = AES_256_CBC;
			}
			else if (strcmp(argv[i+1], "aria256") == 0)
			{
				encryptionMode = ARIA_256_CBC;
			}
			else if (strcmp(argv[i+1], "camellia256") == 0)
			{
				encryptionMode = CAMELLIA_256_CBC;
			}
			else
			{
				printf("\nUnknown Algorithm %s passed\n", argv[i+1]);
				for (int i = 0; i < create_count; i++)
				{
					free(readArguments[i]);
				}
				free(readArguments);
				return 1;
			}
		}
		if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--out") == 0)
		{
			read_args = 2;
			if (i+1 >= argc)
			{
				printf("\nOut Operaton has no Argument\n");
				for (int i = 0; i < create_count; i++)
				{
					free(readArguments[i]);
				}
				free(readArguments);
				return 1;
			}
			strcpy(outArgument, argv[i+1]);
		}
		if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--view") == 0)
		{
			read_args = 2;
			if (i+1 >= argc)
			{
				printf("\nView Operaton has no Argument\n");
				for (int i = 0; i < create_count; i++)
				{
					free(readArguments[i]);
				}
				free(readArguments);
				return 1;
			}
			strcpy(viewArgument, argv[i+1]);
		}
	}

	// Display ASCII Art
	ascii_art();

	// Required Variables
	unsigned char* key = NULL;
	TARIM_METADATA meta;
	TARIM_FILESAVE* fsave = NULL;
	
	// Out Operation
	char basePath[4096];
	if (out_flag)
	{
		strcpy(basePath, outArgument);
		if (basePath[strlen(basePath) - 1] != '/' && extract_flag)
		{
			// Linux Specific
			strcat(basePath, "/");
		}
	}
	else
	{
		if (extract_flag)
		{
			strcpy(basePath, "./");
		}
		if (create_flag)
		{
			strcpy(basePath, "./tam.tarim");
		}
	}

	// Extract & View Operation
	if ( extract_flag || view_flag)
	{
		FILE* archive = NULL;
		char buffer[FP_MAX];
		if (extract_flag)
		{
			archive = fopen(extractArgument, "rb");
			strcpy(buffer, extractArgument);
		}
		else if (view_flag)
		{
			archive = fopen(viewArgument, "rb");
			strcpy(buffer, viewArgument);
		}
		else
		{
			strcpy(buffer, "<Unknown>");
		}
		if (archive == NULL)
		{
			printf("\nFailed to open Archive %s\n", buffer);
			for (int i = 0; i < create_count; i++)
			{
				free(readArguments[i]);
			}
			free(readArguments);
			return 1;
		}	
		
		fsave = TARIM_read_metadata_filedb(&meta, archive);
		if (fsave == NULL)
		{
			fclose(archive);
			free(fsave);
			for (int i = 0; i < create_count; i++)
			{
				free(readArguments[i]);
			}
			free(readArguments);
			return 1;
		}

		// Print Metadata
		printf("Library Used: %s\n", meta.whoami);
		printf("Version: %u rev %u\n\n", meta.version, meta.revision);

		display_tree(meta, fsave);

		if (extract_flag)
		{
			int all_flag = 0;

			printf("\n[-1]  All Files\nChoose Files to Extract. Multiple files can be specifies in comma separated fashion.\n\n=> Files to Extract: ");
			char files[8096];
			scanf("%s", files);

			int o_count = 1;
			for (int i = 0; i < strlen(files); i++)
			{
				if (files[i] == ',')
				{ o_count++; }
			}
			
			int clear = 1;
			int iteration = 0;
			int file_no[o_count];
			char buffer[1024];
			for (int i = 0; i < strlen(files); i++)
			{
				if (clear)
				{
					strcpy(buffer, "");
					clear = 0;
				}
				if (files[i] == ',')
				{
					if (i > 0)
					{
						// check if buffer contains number
						file_no[iteration] = atoi(buffer);
						iteration++;
						clear = 1;
					}
				}
				else
				{
					strncat(buffer, &files[i], 1);
				}

				if (i >= strlen(files) - 1)
				{
					// check if buffer contains number
					file_no[iteration] = atoi(buffer);
					iteration++;
					clear = 1;
				}

			}

			// Check for all marker
			for (int i = 0; i < o_count; i++)
			{
				if (file_no[i] == -1)
				{
					all_flag = 1;
					break;
				}
			}

			// Check for Encryption and get Key
			if (meta.encrypt != NO_ENCRYPT)
			{
				key = TARIM_gen_256_key(&get_password);
				if (key == NULL)
				{
					printf("\nFailed to get password from function\n");
					fclose(archive);
					free(fsave);
					for (int i = 0; i < create_count; i++)
					{
						free(readArguments[i]);
					}
					free(readArguments);
					return 1;
				}
			}

			if (all_flag)
			{	
				for (long long int itr = 0; itr < meta.numFile+meta.numFolder; itr++)
				{
					if (fsave[itr].type != FS_FILE)
					{ continue; }

					// Extract File
					if (TARIM_extract_file(meta, fsave, archive, key, itr, basePath))
					{
						if (meta.encrypt != NO_ENCRYPT)
						{ free(key); }
						fclose(archive);
						free(fsave);
						for (int i = 0; i < create_count; i++)
						{
							free(readArguments[i]);
						}
						free(readArguments);
						return 1;
					}

					int percent_done =  (int) ((itr+1)*100/(meta.numFile+meta.numFolder));
					TARIM_update_progress_bar(percent_done, fsave[itr].fpath, sizeof(fsave[itr].fpath));
				}	
			}
			else
			{
				for (int i = 0; i < o_count; i++)
				{
					if (file_no[i] < 0)
					{ continue; }

					// Extract File
					if (TARIM_extract_file(meta, fsave, archive, key, file_no[i], basePath))
					{
						if (meta.encrypt != NO_ENCRYPT)
						{ free(key); }
						fclose(archive);
						free(fsave);
						for (int i = 0; i < create_count; i++)
						{
							free(readArguments[i]);
						}
						free(readArguments);
						return 1;
					}

					int percent_done = (int) ((i+1)*100/o_count);
					TARIM_update_progress_bar(percent_done, fsave[file_no[i]].fpath, sizeof(fsave[file_no[i]].fpath));
				}
			}
			printf("\r");
		}
		
		// Clean up
		if (meta.encrypt != NO_ENCRYPT)
		{ free(key); }
		fclose(archive);
		free(fsave);	
	}

	// Create Operation
	if (create_flag)
	{
		// Create Archive
		FILE* archive = fopen(basePath, "wb");
		if (archive == NULL)
		{
			printf("\nFailed to create Archive %s\n", basePath);
			for (int i = 0; i < create_count; i++)
			{
				free(readArguments[i]);
			}
			free(readArguments);
			return 1;
		}
		
		// Get Key
		if (encrypt_flag)
		{
			key = TARIM_gen_256_key(&get_password);

			if (key == NULL)
			{
				printf("\nFailed to get password from function\n");
				fclose(archive);
				free(fsave);
				for (int i = 0; i < create_count; i++)
				{
					free(readArguments[i]);
				}
				free(readArguments);
				return 1;
			}
		}
		
		if (TARIM_update_write_metadata(&meta, encryptionMode, create_count, readArguments, archive))
		{
			if (encrypt_flag)
			{ free(key); }
			fclose(archive);
			for (int i = 0; i < create_count; i++)
			{
				free(readArguments[i]);
			}
			free(readArguments);
			return 1;
		}
		fsave = TARIM_save_filefolder_metadata(meta, create_count, readArguments);
		if (fsave == NULL)
		{
			if (encrypt_flag)
			{ free(key); }
			fclose(archive);
			for (int i = 0; i < create_count; i++)
			{
				free(readArguments[i]);
			}
			free(readArguments);
			return 1;
		}

		if (TARIM_write_archive(meta, fsave, archive, key))
		{
			if (encrypt_flag)
			{ free(key); }
			fclose(archive);
			free(fsave);
			for (int i = 0; i < create_count; i++)
			{
				free(readArguments[i]);
			}
			free(readArguments);
			return 1;
		}

		// Clean up
		if (encrypt_flag)
		{ free(key); }
		fclose(archive);
		free(fsave);
	}
	
	// Free allocated space
	for (int i = 0; i < create_count; i++)
	{
		free(readArguments[i]);
	}
	free(readArguments);
	
	return 0;
}

