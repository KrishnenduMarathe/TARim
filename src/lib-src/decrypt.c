#include "../libtarim.h"

// Decrypt File using AES-256
int decrypt_aes256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize)
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

	int num_read, outLen;
	int block_size = EVP_CIPHER_block_size(cipher);
	unsigned char inbuffer[1024], outbuffer[1024 + block_size]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 0, NULL))
	{
		printf("(ERROR) decrypt_aes256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == sizeof(key)/sizeof(unsigned char));
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
	int loop_ok = 1;
	unsigned long long int b_count = 0;
	while (loop_ok)
	{
		num_read = 0;
		for (int i = 0; i < 1024; i++)
		{
			num_read += fread(&inbuffer[i], sizeof(unsigned char), 1, infile);
			b_count += 1;

			// EOF Reached
			if (b_count >= fileSize || feof(infile))
			{
				loop_ok = 0;
				break;
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
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) decrypt_aes256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

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

// Decrypt File using ARIA-256
int decrypt_aria256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize)
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

	int num_read, outLen;
	int block_size = EVP_CIPHER_block_size(cipher);
	unsigned char inbuffer[1024], outbuffer[1024 + block_size]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 0, NULL))
	{
		printf("(ERROR) decrypt_aria256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == sizeof(key)/sizeof(unsigned char));
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
	int loop_ok = 1;
	unsigned long long int b_count = 0;
	while (loop_ok)
	{
		num_read = 0;
		for (int i = 0; i < 1024; i++)
		{
			num_read += fread(&inbuffer[i], sizeof(unsigned char), 1, infile);
			b_count += 1;

			// EOF Reached
			if (b_count >= fileSize || feof(infile))
			{
				loop_ok = 0;
				break;
			}
		}

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) decrypt_aria256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);
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
int decrypt_camellia256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, unsigned long long int fileLoc, unsigned long int fileSize)
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

	int num_read, outLen;
	int block_size = EVP_CIPHER_block_size(cipher);
	unsigned char inbuffer[1024], outbuffer[1024 + block_size]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 0, NULL))
	{
		printf("(ERROR) decrypt_camellia256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == sizeof(key)/sizeof(unsigned char));
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
	int loop_ok = 1;
	unsigned long long int b_count = 0;
	while (loop_ok)
	{
		num_read = 0;
		for (int i = 0; i < 1024; i++)
		{
			num_read += fread(&inbuffer[i], sizeof(unsigned char), 1, infile);
			b_count += 1;

			// EOF Reached
			if (b_count >= fileSize || feof(infile))
			{
				loop_ok = 0;
				break;
			}
		}

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) decrypt_camellia256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);
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