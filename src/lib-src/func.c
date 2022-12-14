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

// Generate 128 bit Initialization Vector
unsigned char* gen_128_iv()
{
	unsigned char* iv = (unsigned char*) malloc(sizeof(unsigned char) * 16);
	if (iv == NULL)
	{
		printf("(ERROR) gen_128_iv: Failed to Allocate memory to IV\n");
		return NULL;
	}
	if (!RAND_bytes(iv, sizeof(iv)))
	{
		printf("(ERROR) gen_128_iv: Failed to generate iv\n");
		free(iv);
		return NULL;
	}

	return iv;
}

// Generate 256 bit key
unsigned char* gen_256_key(void (*get_pass)(char*))
{
	// Get Password
	int passLen = 0;
	char password[1024];
	if (get_pass == NULL)
	{
		printf("(ERROR) gen_256_key: Password Function not passed\n");
		return NULL;
	}
	
	(*get_pass)(password);

	for (int itr = 0; itr < 1024; itr++)
	{
		if (password[itr] == '\0')
		{
			passLen = itr + 1;
			break;
		}
	}

	// Creating 256 bit key
	EVP_MD_CTX* k_ctx = EVP_MD_CTX_new();
	if (k_ctx == NULL)
	{
		printf("(ERROR) gen_256_key: Failed to generate context during key generation. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}
	EVP_MD* sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);
	if (sha256 == NULL)
	{
		printf("(ERROR) gen_256_key: Failed to fetch algorithm during key generation. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_MD_CTX_free(k_ctx);
		return NULL;
	}

	unsigned char* key;
	int keyLen = EVP_MD_get_size(sha256);
	key = (unsigned char *) malloc(sizeof(unsigned char) * keyLen);
	if (key == NULL)
	{
		printf("(ERROR) gen_256_key: Failed to allocate memory\n");

		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return NULL;
	}

	if (!EVP_DigestInit_ex2(k_ctx, sha256, NULL))
	{
		printf("(ERROR) gen_256_key: Failed to Initiate digest. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		free(key);
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return NULL;
	}
	if (!EVP_DigestUpdate(k_ctx, password, sizeof(char) * passLen))
	{
		printf("(ERROR) gen_256_key: Failed to update digest. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		free(key);
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return NULL;
	}
	if (!EVP_DigestFinal_ex(k_ctx, key, &passLen))
	{
		printf("(ERROR) gen_256_key: Failed to digest final block. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		free(key);
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return NULL;
	}
	EVP_MD_free(sha256);
	EVP_MD_CTX_free(k_ctx);

	return key;
}

// Write Raw data from one to other
int nocrypt_write(FILE* infile, FILE* outfile)
{
	if (infile == NULL || outfile == NULL )
	{
		printf("(ERROR) nocrypt_write: One/more parameters are NULL pointers\n");
		return 1;
	}

	unsigned char buffer;
	while (fread(&buffer, sizeof(unsigned char), 1, infile) == 1)
	{ fwrite(&buffer, sizeof(unsigned char), 1, outfile); }

	return 0;
}

// Write Raw data from archive
int nocrypt_extractfile(FILE* infile, FILE* outfile, unsigned long long int fileLoc, unsigned long int fileSize)
{
	if (infile == NULL || outfile == NULL)
	{
		printf("(ERROR) nocrypt_extractfile: One/more parameters are NULL pointers\n");
		return 1;
	}

	// Seek to File Location
	fseek(infile, fileLoc, SEEK_SET);

	unsigned char buffer;
	unsigned long long int b_count = 0;	
	while (fread(&buffer, sizeof(unsigned char), 1, infile) == 1)
	{
		fwrite(&buffer, sizeof(unsigned char), 1, outfile);
		b_count += 1;

		if (b_count >= fileSize) { break; }
	}

	return 0;
}

// Draw and Update Progress Bar
void update_progress_bar(int percent_done, const char* filename, int arraysize)
{
	if (filename == NULL)
	{
		printf("(ERROR) update_progress_bar: NULL File Name passsed\n");
		return;
	}
	int bar_length = 40;
	int name_length = bar_length / 2;
	int num_char = percent_done * bar_length / 100;
	
	// Print File Name
	for (int j = 0; j < arraysize; j++)
	{
		printf("%c", filename[j]);
		if (j >= name_length - 2)
		{
			printf("..");
			break;
		}
	}
	
	printf("  [");
	for (int e = 0; e < bar_length; e++)
	{
		if (e < num_char)
		{
			printf(">");
		}
		else
		{
			printf("-");
		}
	}

	printf("]\t%d%%\n", percent_done);
}

