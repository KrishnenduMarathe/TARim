#include <stdio.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

// ENCRYPT: aes.o -e <regular file> <encrypted file>
// DECRYPT: aes.o -d <encrypted file> <regular file>
int main(int argc, char** argv)
{
	// Encoding value
	int enc = 0;

	if (argc != 4)
	{
		printf("(ERROR) Requires four arguments\n");
		return 0;
	}

	if (strcmp(argv[1], "-e") != 0)
	{
		if (strcmp(argv[1], "-d") != 0)
		{
			if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
			{
				printf("Help:\n\texecutable [-e/-d] [regular file/encrypted file] [out file]\n");
				return 0;
			} else {
				printf("(ERROR) Invalid command '%s'\n", argv[1]);
				return 1;
			}
		}
	}

	if (strcmp(argv[1], "-e") == 0) { enc = 1; }

	FILE* input = fopen(argv[2], "rb");
	if (input == NULL)
	{
		printf("(ERROR) File not found\n");
		return 1;
	}
	fclose(input);

	// Get Password
	char password[1024]; // pasword has garbage values after \0 hence fails
	unsigned int passLen = 0;

	printf("Enter Password: ");
	scanf("%s", password);
	int password_len = 0;
	for (int i = 0; i < 1024; i++)
	{
		if (password[i] == '\0')
		{
			password_len = i+1;
			break;
		}
	}
	// Creating 256 bit key
	EVP_MD_CTX* k_ctx = EVP_MD_CTX_new();
	if (k_ctx == NULL)
	{
		printf("(ERROR) Failed to generate context during key generation. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_MD* sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);
	if (sha256 == NULL)
	{
		printf("(ERROR) Failed to fetch algorithm during key generation. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}

	int digestLen = EVP_MD_get_size(sha256);
	unsigned char digest[digestLen];

	if (!EVP_DigestInit_ex2(k_ctx, sha256, NULL))
	{
		printf("(ERROR) Failed to Initiate digest. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}
	if (!EVP_DigestUpdate(k_ctx, password, sizeof(char) * password_len))
	{
		printf("(ERROR) Failed to update digest. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}
	if (!EVP_DigestFinal_ex(k_ctx, digest, &passLen))
	{
		printf("(ERROR) Failed to digest final block. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}
	EVP_MD_free(sha256);
	EVP_MD_CTX_free(k_ctx);

	// DEBUG
	printf("(DEBUG) Key Generated: %X\n", digest);

	// Set up IV
	unsigned char iv[16];
	if (enc == 1)
	{
		if (!RAND_bytes(iv, sizeof(iv)))
		{
			printf("(ERROR) Failed to generate iv\n");
			return 1;
		}

		FILE* iv_file = fopen(".iv_file", "wb");
		if (iv_file == NULL)
		{
			printf("(ERROR) Failed to create iv file\n");
			return 1;
		}
		fwrite(iv, sizeof(unsigned char), 16, iv_file);
		fclose(iv_file);
	} else {
		FILE* iv_file = fopen(".iv_file", "rb");
		if (iv_file == NULL)
		{
			printf("(ERROR) Failed to reads iv file\n");
			return 1;
		}
		fread(iv, sizeof(unsigned char), 16, iv_file);
		fclose(iv_file);
	}

	// DEBUG
	printf("(DEBUG) IV Generated/Read: %X\n", iv);
	
	// Start Encryption
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "CAMELLIA-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}
	
	FILE* file_in = fopen(argv[2], "rb");
	FILE* file_out = fopen(argv[3], "wb");
	if (file_out == NULL)
	{
		printf("(ERROR) Failes to create output file\n");
		return 1;
	}

	int num_read, outLen;
	int block_size = EVP_CIPHER_block_size(cipher);
	unsigned char inbuffer[1024], outbuffer[1024 + block_size]; // Allow space for additional block in outbuffer

	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, enc, NULL))
	{
		printf("(ERROR) Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		fclose(file_in);
		fclose(file_out);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == digestLen);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, digest, iv, enc, NULL))
	{
		printf("(ERROR) Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		fclose(file_in);
		fclose(file_out);
		return 1;
	}
	
	while (1) // !feof(file_in)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), 1024, file_in);

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			fclose(file_in);
			fclose(file_out);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, file_out);

		// EOF
		if (num_read < 1024)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		fclose(file_in);
		fclose(file_out);
		return 1;
	}
	fwrite(outbuffer, sizeof(unsigned char), outLen, file_out);

	// Clean up
	EVP_CIPHER_free(cipher);
	EVP_CIPHER_CTX_free(ctx);
	fclose(file_in);
	fclose(file_out);
	return 0;
}