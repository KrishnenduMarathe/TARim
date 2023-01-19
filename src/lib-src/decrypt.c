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

// Decrypt File using AES-256
int TARIM_decrypt_aes256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) decrypt_aes256: One/more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) decrypt_aes256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) decrypt_aes256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Disable Padding
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	int num_read = 0;
	int block_size = EVP_CIPHER_block_size(cipher);
	int inLen = block_size;
	int outLen = inLen + block_size;
	unsigned char inbuffer[inLen], outbuffer[outLen]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 0, NULL))
	{
		printf("(ERROR) decrypt_aes256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, 0, NULL))
	{
		printf("(ERROR) decrypt_aes256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Seek to File Location
	fseek(infile, fileLoc, SEEK_SET);

	// Loop until bytes read
	int l_exit = 0;
	unsigned long long int b_count = 0;
	while (!l_exit)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), inLen, infile);
		b_count += num_read;
		
		// Manual Paddng at EOF
		if (b_count >= fileSize)
		{
			num_read -= (b_count - fileSize);
		}
		if (num_read < inLen)
		{
			l_exit = 1;
			for (int cc = num_read; cc < inLen; cc++)
			{
				inbuffer[cc] = '\0';
			}
		}

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) decrypt_aes256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);
	}

	// Cipher Final block with padding
	/*if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) decrypt_aes256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}
	fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);*/

	// Clean up
	EVP_CIPHER_free(cipher);
	EVP_CIPHER_CTX_free(ctx);
	
	return 0;
}

// Decrypt File using ARIA-256
int TARIM_decrypt_aria256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) decrypt_aria256: One/more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) decrypt_aria256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "ARIA-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) decrypt_aria256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Disable Padding
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	int num_read = 0;
	int block_size = EVP_CIPHER_block_size(cipher);
	int inLen = block_size;
	int outLen = inLen + block_size;
	unsigned char inbuffer[inLen], outbuffer[outLen]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 0, NULL))
	{
		printf("(ERROR) decrypt_aria256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, 0, NULL))
	{
		printf("(ERROR) decrypt_aria256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Seek to File Location
	fseek(infile, fileLoc, SEEK_SET);

	// Loop until bytes read
	unsigned long long int b_count = 0;
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), inLen, infile);
		b_count += num_read;

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) decrypt_aria256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (b_count >= fileSize || num_read < inLen)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) decrypt_aria256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}
	fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

	// Clean up
	EVP_CIPHER_free(cipher);
	EVP_CIPHER_CTX_free(ctx);
	
	return 0;
}

// Decrypt File using Camellia-256
int TARIM_decrypt_camellia256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) decrypt_camellia256: One/more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) decrypt_camellia256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "CAMELLIA-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) decrypt_camellia256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Disable Padding
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	int num_read = 0;
	int block_size = EVP_CIPHER_block_size(cipher);
	int inLen = block_size;
	int outLen = inLen + block_size;
	unsigned char inbuffer[inLen], outbuffer[outLen]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 0, NULL))
	{
		printf("(ERROR) decrypt_camellia256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, 0, NULL))
	{
		printf("(ERROR) decrypt_camellia256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Seek to File Location
	fseek(infile, fileLoc, SEEK_SET);

	// Loop until bytes read
	unsigned long long int b_count = 0;
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), inLen, infile);
		b_count += num_read;

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) decrypt_camellia256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (b_count >= fileSize || num_read < inLen)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) decrypt_camellia256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}
	fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

	// Clean up
	EVP_CIPHER_free(cipher);
	EVP_CIPHER_CTX_free(ctx);
	
	return 0;
}

