#include "App.h"
#include "Enclave_u.h"
#include "server.h"
#include "sgx_urts.h"
#include <stdio.h>
#include <string>

sgx_enclave_id_t global_eid = 0;
std::string hst;
std::string prt;

typedef struct _sgx_errlist_t {
	sgx_status_t err;
	const char *msg;
	const char *sug;
} sgx_errlist_t;

static sgx_errlist_t sgx_errlist[] = {{SGX_ERROR_UNEXPECTED,          "Unexpected error occurred.",      NULL},
                                      {SGX_ERROR_INVALID_PARAMETER,   "Invalid parameter.",              NULL},
                                      {SGX_ERROR_OUT_OF_MEMORY,       "Out of memory.",                  NULL},
                                      {SGX_ERROR_ENCLAVE_LOST,        "Power transition occurred.",      "Please refer to the sample \"PowerTransition\" for details."},
                                      {SGX_ERROR_INVALID_ENCLAVE,     "Invalid enclave image.",          NULL},
                                      {SGX_ERROR_INVALID_ENCLAVE_ID,  "Invalid enclave identification.", NULL},
                                      {SGX_ERROR_INVALID_SIGNATURE,   "Invalid enclave signature.",      NULL},
                                      {SGX_ERROR_OUT_OF_EPC,          "Out of EPC memory.",              NULL},
                                      {SGX_ERROR_NO_DEVICE,           "Invalid SGX device.",             "Please make sure SGX module is enabled in the BIOS,and install SGX driver afterwards."},
                                      {SGX_ERROR_MEMORY_MAP_CONFLICT, "Memory map conflicted.",          NULL},
                                      {SGX_ERROR_INVALID_METADATA,    "Invalid enclave metadata.",       NULL},
                                      {SGX_ERROR_DEVICE_BUSY,         "SGX device was busy.",            NULL},
                                      {SGX_ERROR_INVALID_VERSION,     "Enclave version was invalid.",    NULL},
                                      {SGX_ERROR_INVALID_ATTRIBUTE,   "Enclave was not authorized.",     NULL},
                                      {SGX_ERROR_ENCLAVE_FILE_ACCESS, "Can't open enclave file.",        NULL},};

void print_error_message(sgx_status_t ret) {
	size_t idx = 0;
	size_t ttl = sizeof sgx_errlist / sizeof sgx_errlist[0];
	for (idx = 0; idx < ttl; idx++) {
		if (ret == sgx_errlist[idx].err) {
			if (NULL != sgx_errlist[idx].sug) {
				printf("Info: %s\n", sgx_errlist[idx].sug);
			}
			printf("Error: %s\n", sgx_errlist[idx].msg);
			break;
		}
	}
	if (idx == ttl) {
		printf("Return value: %X\n", ret);
		fflush(stdout);
	}
}

int initialize_enclave(void) {
	sgx_status_t ret = SGX_ERROR_UNEXPECTED;
	ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
	if (ret != SGX_SUCCESS) {
		print_error_message(ret);
		exit(-1);
	}
	return 0;
}

int SGX_CDECL

main(int argc, char *argv[]) {
	(void) (argc);
	(void) (argv);
	uint16_t port = (uint16_t) atoi(std::getenv("TCP_PORT"));
	size_t size = strlen(std::getenv("TARGET_HOST")) + strlen(std::getenv("TARGET_PORT")) + 1;
	hst = std::getenv("TARGET_HOST");
	prt = std::getenv("TARGET_PORT");
	std::string outAddr = hst + ":" + prt;
	printf("Proxy port: %i\n", port);
	printf("Forwarding: %s\n", outAddr.c_str());
	printf("Proxy type: P%i\n", atoi(std::getenv("PROXY_TYPE")));
	printf("Encryption: %i\n", atoi(std::getenv("ENABLE_ENCRYPTION")));
	printf("Items_decr: %i\n", atoi(std::getenv("ENABLE_ITEMS_DECR")));
	printf("NB_threads: %i\n", atoi(std::getenv("NB_THREADS")));
	printf("Buff_shuff: %i\n", atoi(std::getenv("BUFFER_SHUFFLING")));
	printf("Epoll_size: %i\n", atoi(std::getenv("EPOLL_SIZE")));
	printf("DEBUG_mode: %i\n", atoi(std::getenv("DEBUG")));
	fflush(stdout);
	initialize_enclave();
	ecall_init(global_eid, atoi(std::getenv("DEBUG")), atoi(std::getenv("PROXY_TYPE")), atoi(std::getenv("ENABLE_ENCRYPTION")), atoi(std::getenv("ENABLE_ITEMS_DECR")), strdup(outAddr.c_str()), size);
	fflush(stdout);
	server_loop(port);
	sgx_destroy_enclave(global_eid);
	return 0;
}
