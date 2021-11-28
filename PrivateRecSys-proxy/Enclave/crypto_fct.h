void print_error() {
	char err[30] = {0};
	ERR_load_crypto_strings();
	ERR_error_string(ERR_get_error(), err);
	printf("ERROR: %s\n", err);
}

int sgx_aes_encrypt(unsigned char *plaintext, int plaintext_size, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
	int len;
	int ciphertext_len;
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv);
	EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_size);
	ciphertext_len = len;
	EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
	ciphertext_len += len;
	EVP_CIPHER_CTX_free(ctx);
	return ciphertext_len;
}

int sgx_aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;
	ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv);
	EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
	plaintext_len = len;
	EVP_DecryptFinal_ex(ctx, (plaintext) + len, &len);
	plaintext_len += len;
	EVP_CIPHER_CTX_free(ctx);
	return plaintext_len;
}

RSA *createRSA(char *key, int isPublic) {
	BIO *bio_buffer;
	bio_buffer = BIO_new_mem_buf((void *) key, -1);
	if (bio_buffer == NULL) {
		printf("Failed to create BIO of key\n");
		print_error();
		return 0;
	}
	RSA *rsa = RSA_new();
	rsa = isPublic ? PEM_read_bio_RSA_PUBKEY(bio_buffer, NULL, 0, NULL) : PEM_read_bio_RSAPrivateKey(bio_buffer, NULL, 0, NULL);
	if (rsa == NULL) {
		printf("Failed to create RSA structure info\n");
		print_error();
	}
	BIO_free_all(bio_buffer);
	return rsa;
}

int public_encrypt(unsigned char *data, int data_len, RSA *rsa, unsigned char *encrypted) {
	if (rsa != NULL) {
		int ret = RSA_public_encrypt(data_len, data, encrypted, rsa, RSA_PKCS1_PADDING);
		if (ret < 1) {
			print_error();
		}
		return ret;
	} else {
		printf("RSA is NULL\n");
		encrypted = data;
		return data_len;
	}
}

int private_decrypt(unsigned char *enc_data, int data_len, RSA *rsa, unsigned char *decrypted) {
	if (rsa != NULL) {
		int ret = RSA_private_decrypt(data_len, enc_data, decrypted, rsa, RSA_PKCS1_PADDING);
		if (ret < 1) {
			print_error();
		}
		return ret;
	} else {
		printf("RSA is NULL\n");
		decrypted = enc_data;
		return data_len;
	}
}

unsigned char *base64encode(const void *b64_encode_this, int encode_this_many_bytes) {
	BIO *b64_bio, *mem_bio;
	BUF_MEM *mem_bio_mem_ptr;
	b64_bio = BIO_new(BIO_f_base64());
	mem_bio = BIO_new(BIO_s_mem());
	if (b64_bio == NULL || mem_bio == NULL) {
		print_error();
	}
	BIO_push(b64_bio, mem_bio);
	BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(b64_bio, b64_encode_this, encode_this_many_bytes);
	(void) BIO_flush(b64_bio);
	BIO_get_mem_ptr(mem_bio, &mem_bio_mem_ptr);
	(void) BIO_set_close(mem_bio, BIO_NOCLOSE);
	BIO_free_all(b64_bio);
	BUF_MEM_grow(mem_bio_mem_ptr, (*mem_bio_mem_ptr).length + 1);
	(*mem_bio_mem_ptr).data[(*mem_bio_mem_ptr).length] = 0;
	return (unsigned char *) (*mem_bio_mem_ptr).data;
}

unsigned char *base64decode(const void *b64_decode_this, int decode_this_many_bytes) {
	BIO *b64_bio, *mem_bio;
	unsigned char *base64_decoded = (unsigned char *) calloc((decode_this_many_bytes * 3) / 4 + 1, sizeof(char));
	b64_bio = BIO_new(BIO_f_base64());
	mem_bio = BIO_new(BIO_s_mem());
	if (b64_bio == NULL || mem_bio == NULL) {
		print_error();
	}
	BIO_write(mem_bio, b64_decode_this, decode_this_many_bytes);
	BIO_push(b64_bio, mem_bio);
	BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);
	int decoded_byte_index = 0;
	while (0 < BIO_read(b64_bio, base64_decoded + decoded_byte_index, 1)) {
		decoded_byte_index++;
	}
	BIO_free_all(b64_bio);
	base64_decoded[(decode_this_many_bytes * 3) / 4 + 1] = 0;
	return base64_decoded;
}

bool simpleSHA256(unsigned char *input, unsigned char *out) {
	unsigned char md[SHA256_DIGEST_LENGTH] = {0};
	SHA256_CTX context;
	if (!SHA256_Init(&context)) { return false; }
	if (!SHA256_Update(&context, input, strlen((char *) input))) { return false; }
	if (!SHA256_Final(md, &context)) { return false; }
	memcpy(out, md, IV_LEN);
	return true;
}

void generate_keys() {
	int KEY_LENGTH = 1024;
	size_t pri_len;
	size_t pub_len;
	printf("Generating RSA (%d bits) keypair...\n", KEY_LENGTH);
	RSA *keypair = RSA_new();
	BIGNUM *e = BN_new();
	BN_set_word(e, RSA_F4);
	RSA_generate_key_ex(keypair, KEY_LENGTH, e, NULL);
	BIO *pri = BIO_new(BIO_s_mem());
	BIO *pub = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
	PEM_write_bio_RSAPublicKey(pub, keypair);
	pri_len = BIO_pending(pri);
	pub_len = BIO_pending(pub);
	char pri_key[pri_len + 1] = {0};
	char pub_key[pub_len + 1] = {0};
	BIO_read(pri, pri_key, (int) pri_len);
	BIO_read(pub, pub_key, (int) pub_len);
	pri_key[pri_len] = '\0';
	pub_key[pub_len] = '\0';
	printf("\n%s%s", pri_key, pub_key);
	printf("Done.\n");
	RSA_free(keypair);
	BIO_free_all(pub);
	BIO_free_all(pri);
}
