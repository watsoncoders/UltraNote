// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2014-2017 XDN developers
// Copyright (c) 2016-2017 BXC developers
// Copyright (c) 2017 UltraNote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hash-ops.h"
#include "keccak.h"

void hash_permutation(union hash_state *state) {
  keccakf((uint64_t*)state, 24);
}

void hash_process(union hash_state *state, const uint8_t *buf, size_t count) {
  keccak1600(buf, (int)count, (uint8_t*)state);
}

void cn_fast_hash(const void *data, size_t length, char *hash) {
  union hash_state state;
  hash_process(&state, data, length);
  memcpy(hash, &state, HASH_SIZE);
}