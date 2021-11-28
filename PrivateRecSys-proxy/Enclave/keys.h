#include "user_types.h"

struct inRAM {
	unsigned char key[USER_KEY_LEN];
	unsigned char iv[IV_LEN];
};
struct inRAM *inRAM;
RSA *P1 = NULL;
RSA *P2 = NULL;
unsigned char symmetric_key_P1[] = "1234567890";
unsigned char symmetric_key_P2[] = "0123456789";
unsigned char symmetric_iv[] = "0123456700000000";
char private_key_P1[] = "-----BEGIN RSA PRIVATE KEY-----\n"\
"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\n"\
"-----END RSA PRIVATE KEY-----\n";
char public_key_P1[] = "-----BEGIN RSA PUBLIC KEY-----\n"\
"MIICCAKCAgEAvT8KHczKTQgxbNFdwhfw9jPv7wyI4K5i/qoh/nBcg7VlV2oQBqYf\n"\
"ke6Wkv3I9lEeDsjPhy292nKrhoswJuaNsxKSucR9DUNmMjI75A0nhNB1MCYpV7Oq\n"\
"wepubst2IzFu1PtPPch02Y4Zi1fpHM30AZ3ANi0++SDvV5Wo/JuMvfzZGnEGWyUA\n"\
"oabZvcE8Ig8/Trr6PJ4ilf9oYEwYbZ8yASlqNCE8DGvVwesB9enEsAwOybx41mGm\n"\
"3KlBDS5ashYQIoWnyBgOKAMWnzou2G9UprL5WfP/mmn6vQjVFhf6OKdlzqcYuRKp\n"\
"I4qcPV9Rrj1oAmCWYj68KZtlVtIo601luQQFrwHRnveOTKbuwED2MkkI+HNVah7V\n"\
"rWEbRI3fKtKH2Eyu91LfZDXlTmu0/GqaHa2lCmUH656jGP0ynRzdH84dnuv9k3yd\n"\
"KifwfG4khP9yZvb2zx8D6lYXS/urOxwXTQ2WmXN8cpud4aRURLHwi9ekwADFIFOi\n"\
"NhzTAJsrs4n+qnMZttdF49S6gRzDvMQ7rKm8BX2ELywXQnutOYf+9jab3vM+cvo8\n"\
"XkROGeKvP5glTLMnlnkfmApX10WniFVIqpVRI1eKa89I5ubh+h/eqDG2WTrx1D+E\n"\
"arbf4nKO8b8SDszTjIb4qQhXRdYKgR5QOD8pyDRl4ErAsxHRCI8uLhcCAQM=\n"\
"-----END RSA PUBLIC KEY-----\n";
char private_key_P2[] = "-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIJJwIBAAKCAgEApdI3K6cVOGAwp7vkOs+pIaGfalI8v96Z0db7pOvLnX4jKmYb\n"\
"UeEcwr68caxikk5hip/kQ2GxEj+NBwFMFop2TbeaUmmSbUPrQb8trlzjbn/cO4Qr\n"\
"MfyZGqb6qCg9gGl+NvWngYLLoqEDkHAkOoMkw+yy3Z0tPWMN7IB7ZKlv+sWpjkm/\n"\
"qX1lz84xygyKYVsSh+9L++rsg0VeKTJA5PvcROt/QaN9HA5JRGNPMKm+M+w/9YM6\n"\
"B11oUMYJ+a6NUnh1mfvle9+xCkF6BgeiLVQWRg8vNtvXFm4up1m8sGvUFZCwo0TY\n"\
"9Z7TB4B54EaeFsvbzM2cN8vn2fAdNAAptiFEMZi9GXX080X0UCeHPSnaOGLBi0Ry\n"\
"jELftkOFlCY1vC7X2tx+2RSy8VoklW0smj/U6D7zmVDC3GUkXway7nfknIqGuzlX\n"\
"dMXi45U6mY9WIiOuf2Z1rBFkKNsn3Kn+xcQWdPLzpdw4dZrKfk0cVOrRIgsQJVlz\n"\
"pHRhERwXKh2p33dYRfHDcFQC9HNTevF5T2TVypHjwyxmVKg7bBj6VQSRpjV4Q0x4\n"\
"/e5f2GasOxKzEDSAc99XviaeU8KKvaEImRKsX6ls3IWeUdjJVxMfmPsEbKgUSMZF\n"\
"ywjQNfGQK5UcOjLOqAqLf+uLivuEVUAfAUH1w7/KQChpDpsaopRZ2QAKGFsCAQMC\n"\
"ggIAbowkx8S40EAgb9KYJzUba8EU8Ywof+m74Tn9GJ0yaP7CHEQSNpYTLH8oS8hB\n"\
"tt7rsb/tgkEgttUIr1YyubGkM8+8NvEMSNfyK9TJHuiXnv/oJ61yIVMQvG9RxXAp\n"\
"AEZUJKPFAQHdFxYCYErC0azDLUh3PmjI05denar87cZKpy5xCYZ/xlOZNTQhMV2x\n"\
"ljy3BUoyp/HzAi4+xiGAmKfoLfJU1mz+ErQw2EI0yxvUIp1/+QImr5Oa4IQGpnRe\n"\
"Nvr5EVKY/T/LXCumrq/Bc41kLrTKJJKPZEl0b5EodZ04DmB1wi3l+RSMr6r76tm+\n"\
"ud09MzO9eofv5qATeAAbzsDYH/25i94tJLQ5mFnG4lzVMOfuYPnSyfGY2yoU+1Im\n"\
"j30Wtraf808pPxrVWg9/pigrJqbYqTxWbOWVJbrc/dSc9kFua2x6tLwueBt5N9S4\n"\
"B1V8TTHDuY8CX7z2ncRAxUq5EidvTvegOFzsDpiZb3zeEQogzNuUr59iuPo6ZmhI\n"\
"B/oez8H7wblnYkiI+AyeMrodNVPQHnj3M0GQDvd4VYHtg45LKVH3X2K9WlxWGHKX\n"\
"2pNoFxczZcd4j8VfeWDwGkQFgl0w3qpUMoRHMPmsbpomVWDWQX6MUKcGTk8GwDOp\n"\
"KmlNCzui0akEVBdjrjw6TR3C/GzFNLxHIQqtBFt83JAZRIsCggEBANIyx22cXlW8\n"\
"6LQ6LWFDWf/tD7EsrjFxqeYF0gsGd/HV6nnUAxUyUDBNpu5nfKqBNnyudFTdi2c5\n"\
"tmcvGJx1BhWf1c0LQcTauPjVWFZ6zAqZpGo1OCkTRjjDITU1xC+KqvG3YY400GuH\n"\
"tYC25y97sp60G4/HB6oJzT4VFutllfQ8SLB/CILcEHqBIkAy2kLUbyoyOH4GjI+I\n"\
"ZsaU+imm3/5jfkmbdJEKoUpmqOZCeEazRB1U+IgdVqYPybiQGy6KF04CNnnV3P3O\n"\
"VHh7oF03+Oms0DtksvB1hxiGwlKJe8TgRMuAZD2ltbVHK/Ts9+pd3iPJzaX0nsHJ\n"\
"3c9Ysz1jPqMCggEBAMn0ADsU3eHhAuyivD1XFQbu6hyJrycIw55gSR/1bQFf3lC6\n"\
"6QjCwoGW52eFpFkS98gAJyFjraWK8Ac4WRyEJRLBREmUI+bCdnMvbXnHSxjaZzGa\n"\
"3ZHNUwbyzs5n2aR2LsYVTdqTUKPgsud/RoAIVd0q1oBNlVtDLmaqeZXYPYvtxZ5K\n"\
"xTsL0IwE5J5ZKQrVi2n5XPjs+zZa7t8w2pgGgOGkBwfqekk5L5HEbYCM0BCxDR81\n"\
"rPNMJXRrvdcQVn4aBM+hWAxKKRSQOSQhUraS860mFk0Onx3ROWJ/SPUU/s/60mWW\n"\
"4TBfVyMno/DalarzavjYo2gJvklNPjvF2uqA8ukCggEBAIwh2kkS6Y598HgmyOuC\n"\
"O//zX8tzHsuhG+6ujAdZpUvj8aaNV2N24CAzxJ7vqHGreah0TY3pB5omeZofZb2j\n"\
"WWO/494HgS3nJfs45Y78iAcRGEbOJXC3hCXXa3jOgspccfZ6QQl4ivJaeQB573T9\n"\
"Ib8ivQqEr8axM364ufJDuU19hcr/WwHoCvxWFtV3PCyNn3F20FQEXbUFmdm4psZv\n"\
"P/7s/tu8+GCxwNxEcJmBpYR3gr44pbATjxlf29BgEh8GujQBeaaOk1Pe4vr9FZN6\n"\
"pfEd4CeYd0r5BLsEgYxbp9iVgzJVmCkZI84vcqNIpUbpPsKGiRlNvyvb6TTlzNOX\n"\
"fxcCggEBAIaiqtIN6UFArJ3B0tOPY1n0nBMGdMSwgmmVhhVOSKuVPuB8m1ssgau5\n"\
"75pZGDth+oVVb2uXyRkHSq965hMCw2HWLYZiwpnW+aIfnlEvh2Xm73ZnPmEzjK9M\n"\
"id7v5m2kHy643pG3ixfrIe+qLwAFjpNx5FWJDjzXdERxpmPlfl1JLmmHLiddNbKt\n"\
"7b7mG1yOXPFQ6KXzUiQ8nz915xAEVevCr1qcUYYmH7aC86sIirXLXhTOc0zdbk2d\n"\
"KToK5FQRWIprkAgxcLhgJhgWNyRh98jEDt4Jv2k2JkGqMKNjVIqnNu5kliA/j2zF\n"\
"F/XnDnH3nKXlwkVb1DDeKX0ukfGrTJsCggEAbx+6N7fr85KMP31dNCkNAGD28ik6\n"\
"fxunfIhJbF+4TEot+RTm5mgs03kczrGIp8A6FCtfC2v7/9qgxto59F/rSZbP8ZqR\n"\
"155g+bEtednckdiW/3OSFP+w2YYtXAgfDB+X05gIw/QqXFkFkCYHO0Ka3qa4Nptk\n"\
"jiGzdWZS58Hlh0sBlYiW0x4XIABqXlPUHPVMKQfUfSSsLtrQ8Ks4XOe4uitVcOf+\n"\
"OfqUAX4BRP31CEXryFJYgUd3pVWyyQu8C5v1LBiblgw1z1QBR90bo1YBcmG+wv+K\n"\
"z5IiqmvOf39/7cm9UwkROQSvJYq8862IErhons7nC5G+kfPTschyy+JTIQ==\n"\
"-----END RSA PRIVATE KEY-----\n";
char public_key_P2[] = "-----BEGIN RSA PUBLIC KEY-----\n"\
"MIICCAKCAgEApdI3K6cVOGAwp7vkOs+pIaGfalI8v96Z0db7pOvLnX4jKmYbUeEc\n"\
"wr68caxikk5hip/kQ2GxEj+NBwFMFop2TbeaUmmSbUPrQb8trlzjbn/cO4QrMfyZ\n"\
"Gqb6qCg9gGl+NvWngYLLoqEDkHAkOoMkw+yy3Z0tPWMN7IB7ZKlv+sWpjkm/qX1l\n"\
"z84xygyKYVsSh+9L++rsg0VeKTJA5PvcROt/QaN9HA5JRGNPMKm+M+w/9YM6B11o\n"\
"UMYJ+a6NUnh1mfvle9+xCkF6BgeiLVQWRg8vNtvXFm4up1m8sGvUFZCwo0TY9Z7T\n"\
"B4B54EaeFsvbzM2cN8vn2fAdNAAptiFEMZi9GXX080X0UCeHPSnaOGLBi0RyjELf\n"\
"tkOFlCY1vC7X2tx+2RSy8VoklW0smj/U6D7zmVDC3GUkXway7nfknIqGuzlXdMXi\n"\
"45U6mY9WIiOuf2Z1rBFkKNsn3Kn+xcQWdPLzpdw4dZrKfk0cVOrRIgsQJVlzpHRh\n"\
"ERwXKh2p33dYRfHDcFQC9HNTevF5T2TVypHjwyxmVKg7bBj6VQSRpjV4Q0x4/e5f\n"\
"2GasOxKzEDSAc99XviaeU8KKvaEImRKsX6ls3IWeUdjJVxMfmPsEbKgUSMZFywjQ\n"\
"NfGQK5UcOjLOqAqLf+uLivuEVUAfAUH1w7/KQChpDpsaopRZ2QAKGFsCAQM=\n"\
"-----END RSA PUBLIC KEY-----\n";

char private_key_P2_1024[] = "-----BEGIN RSA PRIVATE KEY-----\n"\
"MIICXgIBAAKBgQDU+gF6CLlUZ8smKyq2w4nDVvCylYiOAikiR8diScBr6mY6zqrO\n"\
"iEAaey2UW+L1RD35aV2vcZQuMscd/c61oYkIMweDfQcm7/JCGjwdNWsT5FYuqy+v\n"\
"9GYPTLyniku1N9qoyw5FT+09ncl3tlLRft9rX2t2seW73VHu4Bi8m9i+BwIDAQAB\n"\
"AoGBALR06U9nmPEFyUIcDWMyp8YWIniO71e0SPo865pCd4J46AooFt5M4FQPzv0I\n"\
"JnQ/UGvGdZU0naPPbpvg9Ic9ygbWSXRnqyJNUKNKO/hDi0fkV4uVPwrst206/oJj\n"\
"ufXB4pd06Gg2IgvMQpzHAMLmhCDFg/pbcxI5SZ1mkgRU8K2RAkEA9gMl9IEZeGBo\n"\
"k/jg7bmSIRhmQQ/FNWGjEq9HDYRPDYsJ/eHbiUFR346aDp6IRd5R6xCnZctwXqzW\n"\
"neVlaMtVewJBAN2fgtxiRNw2qv6/N771lSB8/OuQlAyhs66Dqo1yjoqbCSQMF1Ej\n"\
"1JbzJ73TYueFMR/gG3yc8CEy0N/n/3SHpeUCQCnmW16LZmUkNmuNZfHXO1Y7q57F\n"\
"Ktduq7GQ/M8Gi9hIqAj/oL59y44B4ac/xQHxXY5lpb0mqxrU4TC/U0hH6WUCQQCs\n"\
"9g9ABeVt47Bh7kQIYUtINqOBCK2yigRcH1/c0lnSbJqeql0MMyV3xT7v0+itfXJQ\n"\
"SSvKfgSeNDV1eW8Rt8fNAkEAsOY5WvXvvo7N5kmIu55RQ05qGsd9v35b8LyKHSKy\n"\
"4Op+xfdzwrDRpHVCTiMRT1Rgt2NO60E53OLLmRwpCfDhtQ==\n"\
"-----END RSA PRIVATE KEY-----\n";
char public_key_P2_1024[] = "-----BEGIN RSA PUBLIC KEY-----\n"\
"MIGJAoGBANT6AXoIuVRnyyYrKrbDicNW8LKViI4CKSJHx2JJwGvqZjrOqs6IQBp7\n"\
"LZRb4vVEPflpXa9xlC4yxx39zrWhiQgzB4N9Bybv8kIaPB01axPkVi6rL6/0Zg9M\n"\
"vKeKS7U32qjLDkVP7T2dyXe2UtF+32tfa3ax5bvdUe7gGLyb2L4HAgMBAAE=\n"\
"-----END RSA PUBLIC KEY-----\n";
