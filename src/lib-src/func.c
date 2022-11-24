#include "../libtarim.h"

// Get Password
static void get_password(char* passwd)
{
	printf("Enter Password: ");
	scanf("%s", passwd);
	system(CLEARSCREEN);
}

// Generate 128 bit Initialization Vector
int gen_128_iv(unsigned char* iv)
{
	if (!RAND_bytes(iv, sizeof(iv)))
	{
		printf("(ERROR) gen_16_iv: Failed to generate iv\n");
		return 1;
	}

	return 0;
}

// Generate 256 bit key
int gen_256_key(unsigned char** key, void (*get_pass)(char*))
{
	if (*key != NULL)
	{
		printf("(ERROR) gen_256_key: key passed is pre-allocated\n");
		return 1;
	}

	// Get Password
	int passLen = 0;
	char password[1024];
	if (get_pass == NULL)
	{ get_pass = &get_password; }
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
		return 1;
	}
	EVP_MD* sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);
	if (sha256 == NULL)
	{
		printf("(ERROR) gen_256_key: Failed to fetch algorithm during key generation. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}

	int keyLen = EVP_MD_get_size(sha256);
	*key = (unsigned char *) malloc(sizeof(unsigned char) * keyLen);
	if (*key == NULL)
	{
		printf("(ERROR) gen_256_key: Failed to allocate memory\n");

		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}

	if (!EVP_DigestInit_ex2(k_ctx, sha256, NULL))
	{
		printf("(ERROR) gen_256_key: Failed to Initiate digest. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		free(*key);
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}
	if (!EVP_DigestUpdate(k_ctx, password, sizeof(char) * passLen))
	{
		printf("(ERROR) gen_256_key: Failed to update digest. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		free(*key);
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}
	if (!EVP_DigestFinal_ex(k_ctx, *key, &passLen))
	{
		printf("(ERROR) gen_256_key: Failed to digest final block. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		free(*key);
		EVP_MD_free(sha256);
		EVP_MD_CTX_free(k_ctx);
		return 1;
	}
	EVP_MD_free(sha256);
	EVP_MD_CTX_free(k_ctx);

	return 0;
}

// Write Raw data from one to other
int nocrypt_write(FILE* infile, FILE* outfile)
{
	if (infile == NULL || outfile == NULL )
	{
		printf("(ERROR) nocrypt_write: One more parameters are NULL pointers\n");
		return 1;
	}

	unsigned char buffer;
	while (fread(&buffer, sizeof(unsigned char), 1, infile) == 1)
	{ fwrite(&buffer, sizeof(unsigned char), 1, outfile); }

	return 0;
}
