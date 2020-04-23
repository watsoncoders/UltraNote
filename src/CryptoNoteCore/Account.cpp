// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2014-2017 XDN developers
// Copyright (c) 2016-2017 BXC developers
// Copyright (c) 2017 UltraNote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "Account.h"
#include "CryptoNoteSerialization.h"
#include "crypto/crypto.h"
#include "crypto/keccak.c"

namespace CryptoNote {
//-----------------------------------------------------------------
AccountBase::AccountBase() {
  setNull();
}
//-----------------------------------------------------------------
void AccountBase::setNull() {
  m_keys = AccountKeys();
}
//-----------------------------------------------------------------
void AccountBase::generate() {

  Crypto::generate_keys(m_keys.address.spendPublicKey, m_keys.spendSecretKey);

  /* We derive the view secret key by taking our spend secret key, hashing
     with keccak-256, and then using this as the seed to generate a new set
     of keys - the public and private view keys. See generate_keys_from_seed */

  generateViewFromSpend(m_keys.spendSecretKey, m_keys.viewSecretKey, m_keys.address.viewPublicKey);
  m_creation_timestamp = time(NULL);

}
void AccountBase::generateViewFromSpend(Crypto::SecretKey &spend, Crypto::SecretKey &viewSecret, Crypto::PublicKey &viewPublic) {
  Crypto::SecretKey viewKeySeed;

  keccak((uint8_t *)&spend, sizeof(spend), (uint8_t *)&viewKeySeed, sizeof(viewKeySeed));

  Crypto::generate_keys_from_seed(viewPublic, viewSecret, viewKeySeed);
}

void AccountBase::generateViewFromSpend(Crypto::SecretKey &spend, Crypto::SecretKey &viewSecret) {
  /* If we don't need the pub key */
  Crypto::PublicKey unused_dummy_variable;
  generateViewFromSpend(spend, viewSecret, unused_dummy_variable);
}
//-----------------------------------------------------------------
void AccountBase::generateDeterministic() { 
  Crypto::SecretKey second;
  Crypto::generate_keys(m_keys.address.spendPublicKey, m_keys.spendSecretKey);
  keccak((uint8_t *)&m_keys.spendSecretKey, sizeof(Crypto::SecretKey), (uint8_t *)&second, sizeof(Crypto::SecretKey));
  Crypto::generate_deterministic_keys(m_keys.address.viewPublicKey, m_keys.viewSecretKey, second);
  m_creation_timestamp = time(NULL);
}
//-----------------------------------------------------------------
Crypto::SecretKey AccountBase::generate_key(const Crypto::SecretKey& recovery_key, bool recover, bool two_random) {
  Crypto::SecretKey first = generate_m_keys(m_keys.address.spendPublicKey, m_keys.spendSecretKey, recovery_key, recover);

  // rng for generating second set of keys is hash of first rng.  means only one set of electrum-style words needed for recovery
  Crypto::SecretKey second;
  keccak((uint8_t *)&first, sizeof(Crypto::SecretKey), (uint8_t *)&second, sizeof(Crypto::SecretKey));

  generate_m_keys(m_keys.address.viewPublicKey, m_keys.viewSecretKey, second, two_random ? false : true);

  struct tm timestamp;
  timestamp.tm_year = 2016 - 1900;  // year 2016
  timestamp.tm_mon = 5 - 1;  // month May
  timestamp.tm_mday = 30;  // 30 of May
  timestamp.tm_hour = 0;
  timestamp.tm_min = 0;
  timestamp.tm_sec = 0;

  if (recover)
   {
     m_creation_timestamp = mktime(&timestamp);
   }
   else
   {
     m_creation_timestamp = time(NULL);
   }
  return first;
}
//-----------------------------------------------------------------
const AccountKeys &AccountBase::getAccountKeys() const {
  return m_keys;
}

void AccountBase::setAccountKeys(const AccountKeys &keys) {
  m_keys = keys;
}
//-----------------------------------------------------------------

void AccountBase::serialize(ISerializer &s) {
  s(m_keys, "m_keys");
  s(m_creation_timestamp, "m_creation_timestamp");
}
}
