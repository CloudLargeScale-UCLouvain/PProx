#ifndef _APP_H_
#define _APP_H_

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

# define TOKEN_FILENAME   "enclave.token"
# define ENCLAVE_FILENAME "enclave.signed.so"

#include "sgx_eid.h"

extern sgx_enclave_id_t global_eid; /* global enclave id */

#endif /* !_APP_H_ */
