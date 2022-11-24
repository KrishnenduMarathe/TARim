#include "../libtarim.h"

// Get Password
static void get_password(char* passwd)
{
	printf("Enter Password: ");
	scanf("%s", passwd);
	system(CLEARSCREEN);
}

// Count Directories and Files
static void count_filefolder(char* basePath, METADATA* meta)
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
				printf("(ERROR) count_filefolder: Cannot allocate memory to character array\n");
				return;
			}

			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			if (isDir(path))
			{ meta->numFolder += 1; }
			
			if (isReg(path))
			{ meta->numFile += 1; }
			
			count_filefolder(path, meta);
			free(path);
		}
	}

	closedir(dir);
}

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

// Populate Metadata
int update_write_metadata(METADATA* meta, CRYPT_MODES mode, int arg_num, char** args, FILE* archive)
{
	if (meta == NULL)
	{
		printf("(ERROR) update_write_metadata: No Metadata was passed\n");
		return 1;
	}
	if (archive == NULL)
	{
		printf("(ERROR) update_write_metadata: Archive is a NULL Pointer\n");
		return 1;
	}

	// Initiate Metadata values
	strcpy(meta->whoami, "TARim: KodeSpace");
	meta->version = VERSION;
	meta->revision = REVISION;
	meta->encrypt = mode;
	meta->numFile = 0;
	meta->numFolder = 0;

	for (unsigned int itr = 0; itr < arg_num; itr++)
	{

		if (isDir(args[itr])) { meta->numFolder += 1; }
		if (isReg(args[itr])) { meta->numFile += 1; }
		
		count_filefolder(args[itr], meta);
	}

	// Write Metadata
	fwrite(meta, sizeof(METADATA), 1, archive);
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

// Generate 16 bit Initialization Vector
int gen_16_iv(unsigned char** iv)
{
	if (*iv != NULL)
	{
		printf("(ERROR) gen_16_iv: iv passed is pre-allocated\n");
		return 1;
	}

	*iv = (unsigned char*) malloc(sizeof(unsigned char) * 16);
	if (*iv == NULL)
	{
		printf("(ERROR) gen_16_iv: Failed to allocate memory\n");
		return 1;
	}

	if (!RAND_bytes(*iv, sizeof(*iv)))
	{
		printf("(ERROR) gen_16_iv: Failed to generate iv\n");
		free(*iv);
		return 1;
	}

	return 0;
}

// Encrypt File using AES-256
int crypt_aes256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, int enc)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) crypt_aes256: One more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) crypt_aes256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) crypt_aes256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	int num_read, outLen;
	int block_size = EVP_CIPHER_block_size(cipher);
	unsigned char inbuffer[1024], outbuffer[1024 + block_size]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, enc, NULL))
	{
		printf("(ERROR) crypt_aes256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == sizeof(key)/sizeof(unsigned char));
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, enc, NULL))
	{
		printf("(ERROR) crypt_aes256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Loop until bytes read
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), 1024, infile);

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) crypt_aes256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (num_read < 1024)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) crypt_aes256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

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
int crypt_aria256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, int enc)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) crypt_aria256: One more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) crypt_aria256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "ARIA-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) crypt_aria256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	int num_read, outLen;
	int block_size = EVP_CIPHER_block_size(cipher);
	unsigned char inbuffer[1024], outbuffer[1024 + block_size]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, enc, NULL))
	{
		printf("(ERROR) crypt_aria256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == sizeof(key)/sizeof(unsigned char));
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, enc, NULL))
	{
		printf("(ERROR) crypt_aria256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Loop until bytes read
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), 1024, infile);

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) crypt_aria256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (num_read < 1024)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) crypt_aria256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

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
int crypt_camellia256(FILE* infile, FILE* outfile, unsigned char* key, unsigned char* iv, int enc)
{
	if (infile == NULL || outfile == NULL || key == NULL || iv == NULL)
	{
		printf("(ERROR) crypt_camellia256: One more parameters are NULL pointers\n");
		return 1;
	}

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		printf("(ERROR) crypt_camellia256: Failed to generate cipher context. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}
	EVP_CIPHER* cipher = EVP_CIPHER_fetch(NULL, "CAMELLIA-256-CBC", NULL);
	if (cipher == NULL)
	{
		printf("(ERROR) crypt_camellia256: Failed to fetch cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	int num_read, outLen;
	int block_size = EVP_CIPHER_block_size(cipher);
	unsigned char inbuffer[1024], outbuffer[1024 + block_size]; // Allow space for additional block in outbuffer
	
	if (!EVP_CipherInit_ex2(ctx, cipher, NULL, NULL, enc, NULL))
	{
		printf("(ERROR) crypt_camellia256: Failed to Initiate Cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Assert size of key and iv
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == sizeof(key)/sizeof(unsigned char));
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	// Set key and iv
	if (!EVP_CipherInit_ex2(ctx, NULL, key, iv, enc, NULL))
	{
		printf("(ERROR) crypt_camellia256: Failed to set key and iv. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
		
		EVP_CIPHER_free(cipher);
		EVP_CIPHER_CTX_free(ctx);
		return 1;
	}

	// Loop until bytes read
	while (1)
	{
		num_read = fread(inbuffer, sizeof(unsigned char), 1024, infile);

		if (!EVP_CipherUpdate(ctx, outbuffer, &outLen, inbuffer, num_read))
		{
			printf("(ERROR) crypt_camellia256: Failed to pass bytes to the cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
			
			EVP_CIPHER_free(cipher);
			EVP_CIPHER_CTX_free(ctx);
			return 1;
		}
		fwrite(outbuffer, sizeof(unsigned char), outLen, outfile);

		// EOF
		if (num_read < 1024)
		{ break; }
	}

	// Cipher Final block with padding
	if (!EVP_CipherFinal_ex(ctx, outbuffer, &outLen))
	{
		printf("(ERROR) crypt_camellia256: Failed to pass bytes from final block to cipher. OpenSSL: %s\n", ERR_error_string(ERR_get_error(), NULL));

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
