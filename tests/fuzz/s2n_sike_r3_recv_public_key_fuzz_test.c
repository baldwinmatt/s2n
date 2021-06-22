/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/* Target Functions: s2n_kem_recv_public_key s2n_kem_encapsulate SIKE_P434_r3_crypto_kem_enc */

#include "tests/s2n_test.h"
#include "tests/testlib/s2n_testlib.h"
#include "tls/s2n_kem.h"
#include "utils/s2n_safety.h"
#include "pq-crypto/s2n_pq.h"

/* The valid_public_key in the corpus directory was generated by taking the first public
 * key (count = 0) from sike_r3.kat and prepending S2N_SIKE_P434_R3_PUBLIC_KEY_BYTES as two
 * hex-encoded bytes. This is how we would expect it to appear on the wire. */
static struct s2n_kem_params kem_params = { .kem = &s2n_sike_p434_r3 };

int s2n_fuzz_test(const uint8_t *buf, size_t len) {
    /* Test the portable C code */
    POSIX_GUARD_RESULT(s2n_disable_sikep434r3_asm());
    POSIX_GUARD(s2n_kem_recv_public_key_fuzz_test(buf, len, &kem_params));

    /* Test the assembly, if available; if not, don't bother testing the C again */
    POSIX_GUARD_RESULT(s2n_try_enable_sikep434r3_asm());
    if (s2n_sikep434r3_asm_is_enabled()) {
        POSIX_GUARD(s2n_kem_recv_public_key_fuzz_test(buf, len, &kem_params));
    }
    return S2N_SUCCESS;
}

S2N_FUZZ_TARGET(NULL, s2n_fuzz_test, NULL)