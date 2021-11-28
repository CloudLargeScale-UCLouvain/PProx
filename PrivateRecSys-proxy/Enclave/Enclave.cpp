#include "Enclave_t.h"
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "keys.h"
#include "err_handling.h"
#include "crypto_fct.h"
#include "user_types.h"

bool debugSgx;
int PROXY_TYPE;
int ENABLE_ENCRYPTION;
int ENABLE_ITEMS_DECR;
char *addr = NULL;

size_t get_pld_pos(std::string fmsg) {
	size_t pld_pos = fmsg.find("\r\n\r\n");
	pld_pos = (pld_pos != std::string::npos) ? pld_pos + 4 : pld_pos;
	return pld_pos;
}

size_t get_hdr_lgt(std::string str, size_t begin) {
	return str.find("\r\n", begin) - begin;
}

std::string get_encoding_escape(std::string str, int *len) {
	if (str.find("\\\"")) {
		*len = 2;
		return "\\\"";
	} else if (str.find("\"")) {
		*len = 1;
		return "\"";
	}
	*len = 0;
	return "";
}

char *copy_string(std::string str, int len) {
	char *s = new char[len + 1];
	std::memcpy(s, str.c_str(), len);
	s[len] = 0;
	return s;
}

std::string extract_pld(std::string fmsg, size_t msgsize) {
	size_t pld_pos = get_pld_pos(fmsg);
	if (pld_pos != std::string::npos && pld_pos < msgsize) {
		return fmsg.substr(pld_pos, msgsize);
	}
	return fmsg;
}

char *json_get_id_value(char *msg, size_t msgsize, std::string id, int *vlen, size_t *start) {
	std::string json = extract_pld(std::string(msg, 0, msgsize), msgsize);
	delete msg;
	size_t posID = json.find(id, *start);
	size_t posV = json.find(":", posID + id.length());
	if (json[posV + 1] == ' ') {
		posV += 1;
	}
	size_t posEnd = json.find(",", posV);
	posEnd = (posEnd == std::string::npos) ? json.find("}", posV) : posEnd;
	*start = posEnd;
	if (posID != std::string::npos && posV != std::string::npos && posEnd != std::string::npos) {
		int wrap_len = 0;
		(void) get_encoding_escape(json, &wrap_len);
		posV += wrap_len;
		*vlen = (int) (posEnd - posV);
		return copy_string(json.substr(posV, (*vlen) - 1), (*vlen));
	}
	return NULL;
}

char *json_change_id_value(char *msg, size_t msgsize, std::string id, std::string value, size_t start) {
	std::string fmsg(msg, 0, msgsize);
	delete msg;
	size_t pld_pos = get_pld_pos(fmsg);
	if (pld_pos == std::string::npos || pld_pos >= msgsize) {
		pld_pos = 0;
	}
	std::string json = fmsg.substr(pld_pos, msgsize);
	size_t posID = json.find(id, start);
	size_t posV = json.find(":", posID + id.length());
	if (json[posV + 1] == ' ') {
		posV += 1;
	}
	size_t posEnd = json.find(",", posV);
	posEnd = (posEnd == std::string::npos) ? json.find("}", posV) : posEnd;
	if (posID != std::string::npos && posV != std::string::npos && posEnd != std::string::npos) {
		int wrap_len = 0;
		(void) get_encoding_escape(json, &wrap_len);
		posV += wrap_len;
		json.replace(posV, posEnd - posV - 1, value);
		fmsg.replace(pld_pos, msgsize - pld_pos, json);
		return copy_string(fmsg, (int) fmsg.length());
	}
	return copy_string(fmsg, (int) msgsize);
}

std::string iterative_encryption(std::string buff, const char *ID, unsigned char *sym_key, unsigned char *sym_iv, unsigned char *u_key, unsigned char *u_iv) {
	int vlen = 0;
	size_t pos_end = 0;
	char *aenc = json_get_id_value(strndup(buff.c_str(), buff.length()), buff.length(), ID, &vlen, &pos_end);
	while (true) {
		char *ret = NULL;
		unsigned char clear[vlen] = {0};
		if (ENABLE_ITEMS_DECR == 1) {
			unsigned char *base64_decoded = base64decode(aenc, vlen);
			vlen = (vlen * 3) / 4;
			vlen = (vlen == 513) ? 512 : vlen;
			sgx_aes_decrypt(base64_decoded, vlen, sym_key, sym_iv, clear);
			delete base64_decoded;
		} else {
			std::memcpy(clear, aenc, vlen);
		}
		unsigned char enc[vlen] = {0};
		sgx_aes_encrypt(clear, vlen, u_key, u_iv, enc);
		unsigned char *base64_encoded = base64encode(enc, vlen);
		ret = json_change_id_value(strndup(buff.c_str(), buff.length()), buff.length(), ID, std::string(reinterpret_cast<char *>(base64_encoded)), pos_end);
		std::string final_ret(ret);
		delete base64_encoded;
		delete aenc;
		delete ret;
		aenc = json_get_id_value(strndup(final_ret.c_str(), final_ret.length()), final_ret.length(), ID, &vlen, &pos_end);
		if (aenc == NULL) {
			return final_ret;
		}
	}
}

char *encrypt_items_list(char *msg, size_t msgsize, unsigned char *sym_iv, unsigned char *u_key) {
	if (u_key != NULL) {
		std::string fmsg(msg, 0, msgsize);
		size_t pld_pos = get_pld_pos(fmsg);
		if (pld_pos != std::string::npos && pld_pos < msgsize) {
			std::string json = fmsg.substr(pld_pos, msgsize);
			if (json.find(RESULTS) != std::string::npos) {
				std::string updated = iterative_encryption(json, QUERIES_ITEMID, symmetric_key_P2, sym_iv, u_key, symmetric_iv);
				fmsg.replace(pld_pos, msgsize - pld_pos, updated);
				return copy_string(fmsg, (int) fmsg.length());
			}
		}
	}
	return copy_string(msg, (int) msgsize);
}

char *encrypt_json_field(char *original_msg, int size, const char *ID, unsigned char *sym_iv, unsigned char *u_key) {
	if (ID != NULL) {
		int vlen = 0;
		size_t start = 0;
		char *aenc = json_get_id_value(strndup(original_msg, size), size, ID, &vlen, &start);
		if (aenc != NULL) {
			char *ret = NULL;
			unsigned char *base64_decoded = base64decode(aenc, vlen);
			if (vlen == 173) {
				vlen = 128;
			} else {
				vlen = (vlen * 3) / 4;
			}
			vlen = (vlen == 513) ? 512 : vlen;
			unsigned char clear[vlen] = {0};
			RSA *rsa = (PROXY_TYPE == 1) ? P1 : P2;
			vlen = private_decrypt(base64_decoded, vlen, rsa, clear);
			if (u_key == NULL) {
				if (PROXY_TYPE == 2 && ENABLE_ITEMS_DECR == 0) {
					ret = json_change_id_value(strndup(original_msg, size), size, ID, std::string(reinterpret_cast<char *>(clear)), 0);
				} else {
					unsigned char enc[vlen] = {0};
					unsigned char *sym_key = (PROXY_TYPE == 1) ? symmetric_key_P1 : symmetric_key_P2;
					sgx_aes_encrypt(clear, vlen, sym_key, sym_iv, enc);
					unsigned char *base64_encoded = base64encode(enc, vlen);
					ret = json_change_id_value(strndup(original_msg, size), size, ID, std::string(reinterpret_cast<char *>(base64_encoded)), 0);
					delete base64_encoded;
				}
			} else {
				ret = json_change_id_value(strndup(original_msg, size), size, ID, "", 0);
				memcpy(u_key, clear, USER_KEY_LEN);
			}
			delete base64_decoded;
			delete aenc;
			return ret;
		}
	}
	return copy_string(original_msg, size);
}

char *update_headers(char *msg, char *address, size_t msgsize) {
	std::string fmsg(msg, 0, msgsize);
	if (address != NULL) {
		size_t posHost = fmsg.find("Host: ") + 6;
		size_t posCon = fmsg.find("Connection: ");
		if (posCon == std::string::npos) {
			fmsg.insert(fmsg.find("\r\n", posHost) + 2, "Connection: Close\r\n");
		} else {
			posCon = posCon + 12;
			fmsg.replace(posCon, get_hdr_lgt(fmsg, posCon), "Close");
		}
		posHost = fmsg.find("Host: ") + 6;
		fmsg.replace(posHost, get_hdr_lgt(fmsg, posHost), address);
	}
	size_t posCL = fmsg.find("Content-Length: ");
	if (posCL != std::string::npos) {
		posCL += 16;
		size_t pld_pos = get_pld_pos(fmsg);
		std::string CL = (pld_pos != std::string::npos && pld_pos < fmsg.length()) ? std::to_string(fmsg.length() - pld_pos) : "0";
		fmsg.replace(posCL, get_hdr_lgt(fmsg, posCL), CL);
	}
	return copy_string(fmsg, (int) fmsg.length());
}

void init_ivs(int sock, unsigned char *out) {
	unsigned char buffer[10] = {0};
	snprintf((char *) buffer, 10, "%d", sock);
	memset(out, 0, IV_LEN);
	simpleSHA256(buffer, out);
	out[IV_LEN] = 0;
}

void ecall_proxy(int csock, char *buff, size_t size, char *buff_final, size_t *size2, int ifreq) {
	unsigned char *u_key = NULL;
	char *req_processed = NULL, *req_tmp = NULL;
	char *fwd_addr = (ifreq == 0) ? addr : NULL;
	if (PROXY_TYPE == 0 || ENABLE_ENCRYPTION == 0) {
		req_processed = copy_string(buff, (int) size);
	} else if (ifreq == 0) { // [C]-->[S]
		const char *ID = NULL;
		init_ivs(csock, inRAM[csock].iv);
		if (std::string(buff).find(QUERIES_URI) != std::string::npos) {
			ID = (PROXY_TYPE == 1) ? QUERIES_USERID : QUERIES_KEYID;
			u_key = (PROXY_TYPE == 2) ? inRAM[csock].key : u_key;
		} else if (std::string(buff).find(EVENTS_URI) != std::string::npos) {
			ID = (PROXY_TYPE == 1) ? EVENTS_USERID : EVENTS_MOVIEID;
		}
		req_processed = encrypt_json_field(buff, (int) size, ID, inRAM[csock].iv, u_key);
	} else { // [C]<--[S]
		u_key = (PROXY_TYPE == 2) ? inRAM[csock].key : u_key;
		req_processed = encrypt_items_list(buff, size, inRAM[csock].iv, u_key);
	}
	req_tmp = update_headers(req_processed, fwd_addr, strlen(req_processed));
	memcpy(buff_final, req_tmp, strlen(req_tmp));
	*size2 = strlen(buff_final);
	delete req_processed;
	delete req_tmp;
}

void ecall_init(int debug, int proxy_type, int enable_encryption, int enable_items_decr, char *outAddr, size_t size) {
	debugSgx = true;
	inRAM = (struct inRAM *) malloc(65536 * sizeof(struct inRAM));
	memset(inRAM, 0, (65536 * sizeof(struct inRAM)));
	P1 = createRSA(private_key_P1, 0);
	P2 = createRSA(private_key_P2, 0); // TODO private_key_P2_1024
	PROXY_TYPE = proxy_type;
	ENABLE_ENCRYPTION = enable_encryption;
	ENABLE_ITEMS_DECR = enable_items_decr;
	addr = new char[size];
	std::memcpy(addr, outAddr, size);
	addr[size] = 0;
	printf("Forwarding: %s\n", addr);
	printf("Proxy type: P%i\n", PROXY_TYPE);
	printf("Encryption: %i\n", ENABLE_ENCRYPTION);
	printf("Items_decr: %i\n", ENABLE_ITEMS_DECR);
	printf("DEBUG_mode: %i\n", debug);
	debugSgx = (debug == 1);
}

