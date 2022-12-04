#include "../libtarim.h"

// Encrypt File using AES-256
int encrypt_aes256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) encrypt_aes256: One/more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) encrypt_aes256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) encrypt_aes256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	int num_read = 0;
	int block_size = EVP_CIPHER_block_size(cipher);
	const int inLen = block_size;
	int outLen = block_size + block_size;
	unsigned char inbuffer[inLen], outbuffer[outLen]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 1, NULL))
	{
		printf("(ERROR) encrypt_aes256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, 1, NULL))
	{
		printf("(ERROR) encrypt_aes256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Loop until bytes read
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), inLen, infile);

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) encrypt_aes256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (num_read < inLen)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) encrypt_aes256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

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

// Encrypt File using ARIA-256
int encrypt_aria256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) encrypt_aria256: One/more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) encrypt_aria256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "ARIA-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) encrypt_aria256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	int num_read = 0;
	int block_size = EVP_CIPHER_block_size(cipher);
	const int inLen = block_size;
	int outLen = block_size + block_size;
	unsigned char inbuffer[inLen], outbuffer[outLen]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 1, NULL))
	{
		printf("(ERROR) encrypt_aria256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, 1, NULL))
	{
		printf("(ERROR) encrypt_aria256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Loop until bytes read
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), inLen, infile);

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) encrypt_aria256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (num_read < inLen)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) encrypt_aria256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

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

// Encrypt File using Camellia-256
int encrypt_camellia256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) encrypt_camellia256: One/more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) encrypt_camellia256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "CAMELLIA-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) encrypt_camellia256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	int num_read = 0;
	int block_size = EVP_CIPHER_block_size(cipher);
	const int inLen = block_size;
	int outLen = block_size + block_size;
	unsigned char inbuffer[inLen], outbuffer[outLen]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, 1, NULL))
	{
		printf("(ERROR) encrypt_camellia256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, 1, NULL))
	{
		printf("(ERROR) encrypt_camellia256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Loop until bytes read
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), inLen, infile);

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) encrypt_camellia256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (num_read < inLen)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) encrypt_camellia256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

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
