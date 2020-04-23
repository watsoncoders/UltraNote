// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2014-2017 XDN developers
// Copyright (c) 2016-2017 BXC developers
// Copyright (c) 2017-2019 UltraNote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "Currency.h"
#include <cctype>
#include <boost/math/special_functions/round.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include "../Common/Base58.h"
#include "../Common/int-util.h"
#include "../Common/StringTools.h"

#include "Account.h"
#include "CryptoNoteBasicImpl.h"
#include "CryptoNoteFormatUtils.h"
#include "CryptoNoteTools.h"
#include "TransactionExtra.h"
#include "UpgradeDetector.h"

#undef ERROR

using namespace Logging;
using namespace Common;

namespace CryptoNote {

const std::vector<uint64_t> Currency::PRETTY_AMOUNTS = {
  1, 2, 3, 4, 5, 6, 7, 8, 9,
  10, 20, 30, 40, 50, 60, 70, 80, 90,
  100, 200, 300, 400, 500, 600, 700, 800, 900,
  1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
  10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000,
  100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000,
  1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000,
  10000000, 20000000, 30000000, 40000000, 50000000, 60000000, 70000000, 80000000, 90000000,
  100000000, 200000000, 300000000, 400000000, 500000000, 600000000, 700000000, 800000000, 900000000,
  1000000000, 2000000000, 3000000000, 4000000000, 5000000000, 6000000000, 7000000000, 8000000000, 9000000000,
  10000000000, 20000000000, 30000000000, 40000000000, 50000000000, 60000000000, 70000000000, 80000000000, 90000000000,
  100000000000, 200000000000, 300000000000, 400000000000, 500000000000, 600000000000, 700000000000, 800000000000, 900000000000,
  1000000000000, 2000000000000, 3000000000000, 4000000000000, 5000000000000, 6000000000000, 7000000000000, 8000000000000, 9000000000000,
  10000000000000, 20000000000000, 30000000000000, 40000000000000, 50000000000000, 60000000000000, 70000000000000, 80000000000000, 90000000000000,
  100000000000000, 200000000000000, 300000000000000, 400000000000000, 500000000000000, 600000000000000, 700000000000000, 800000000000000, 900000000000000,
  1000000000000000, 2000000000000000, 3000000000000000, 4000000000000000, 5000000000000000, 6000000000000000, 7000000000000000, 8000000000000000, 9000000000000000,
  10000000000000000, 20000000000000000, 30000000000000000, 40000000000000000, 50000000000000000, 60000000000000000, 70000000000000000, 80000000000000000, 90000000000000000,
  100000000000000000, 200000000000000000, 300000000000000000, 400000000000000000, 500000000000000000, 600000000000000000, 700000000000000000, 800000000000000000, 900000000000000000,
  1000000000000000000, 2000000000000000000, 3000000000000000000, 4000000000000000000, 5000000000000000000, 6000000000000000000, 7000000000000000000, 8000000000000000000, 9000000000000000000,
  10000000000000000000ull
};

const std::vector<uint64_t> Currency::POWERS_OF_TEN = {
	1, 
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000,
	10000000000,
	100000000000,
	1000000000000,
	10000000000000,
	100000000000000,
	1000000000000000,
	10000000000000000,
	100000000000000000,
	1000000000000000000ull,
	10000000000000000000ull
};

bool Currency::init() {
  if (!generateGenesisBlock()) {
    logger(ERROR, BRIGHT_RED) << "Failed to generate genesis block";
    return false;
  }

  if (!get_block_hash(m_genesisBlock, m_genesisBlockHash)) {
    logger(ERROR, BRIGHT_RED) << "Failed to get genesis block hash";
    return false;
  }

  if (isTestnet()) {
    m_upgradeHeightV2 = 1;
    m_upgradeHeightV3 = 199;
    
    m_blocksFileName = "testnet_" + m_blocksFileName;
    m_blocksCacheFileName = "testnet_" + m_blocksCacheFileName;
    m_blockIndexesFileName = "testnet_" + m_blockIndexesFileName;
    m_txPoolFileName = "testnet_" + m_txPoolFileName;
    m_blockchinIndicesFileName = "testnet_" + m_blockchinIndicesFileName;
  }

  return true;
}

bool Currency::generateGenesisBlock() {
  m_genesisBlock = boost::value_initialized<Block>();

  // Hard code coinbase tx in genesis block, because "tru" generating tx use random, but genesis should be always the same
  std::string genesisCoinbaseTxHex = GENESIS_COINBASE_TX_HEX;
  BinaryArray minerTxBlob;
  
  bool r =
    fromHex(genesisCoinbaseTxHex, minerTxBlob) &&
    fromBinaryArray(m_genesisBlock.baseTransaction, minerTxBlob);

  if (!r) {
    logger(ERROR, BRIGHT_RED) << "failed to parse coinbase tx from hard coded blob";
    return false;
  }

  m_genesisBlock.majorVersion = BLOCK_MAJOR_VERSION_1;
  m_genesisBlock.minorVersion = BLOCK_MINOR_VERSION_0;
  m_genesisBlock.timestamp = GENESIS_TIMESTAMP;
  m_genesisBlock.nonce = GENESIS_NONCE;
  if (m_testnet) {
    ++m_genesisBlock.nonce;
  }
  //miner::find_nonce_for_given_block(bl, 1, 0);

  return true;
}

uint64_t Currency::baseRewardFunction(uint64_t alreadyGeneratedCoins, uint32_t height) const {
  if (height == 1) return ICO_BLOCK_REWARD;

  if (height < BLOCK_REWARD_V2_HEIGHT) {
    uint64_t incrIntervals = static_cast<uint64_t>(height) / REWARD_INCREASE_INTERVAL;
    assert(incrIntervals < POWERS_OF_TEN.size());

    uint64_t base_reward = START_BLOCK_REWARD * POWERS_OF_TEN[incrIntervals];	
    base_reward = (std::min)(base_reward, MAX_BLOCK_REWARD);
    base_reward = (std::min)(base_reward, m_moneySupply - alreadyGeneratedCoins);
  
    return base_reward;
  } else {
    return BLOCK_REWARD_V2;
  }
}

uint32_t Currency::upgradeHeight(uint8_t majorVersion) const {
    if (majorVersion == BLOCK_MAJOR_VERSION_2) {
        return m_upgradeHeightV2;
    } else if (majorVersion == BLOCK_MAJOR_VERSION_3) {
        return m_upgradeHeightV3;
    } else {
        return static_cast<uint32_t>(-1);
    }
}

bool Currency::getBlockReward(size_t medianSize, size_t currentBlockSize, uint64_t alreadyGeneratedCoins,
  uint64_t fee, uint32_t height, uint64_t& reward, int64_t& emissionChange) const {
  assert(alreadyGeneratedCoins <= m_moneySupply);

  uint64_t baseReward = baseRewardFunction(alreadyGeneratedCoins, height);

  medianSize = std::max(medianSize, m_blockGrantedFullRewardZone);
  if (currentBlockSize > UINT64_C(2) * medianSize) {
    logger(TRACE) << "Block cumulative size is too big: " << currentBlockSize << ", expected less than " << 2 * medianSize;
    return false;
  }

  uint64_t penalizedBaseReward = getPenalizedAmount(baseReward, medianSize, currentBlockSize);

  emissionChange = height < 2 ? penalizedBaseReward : (std::min)(penalizedBaseReward, MAX_BLOCK_REWARD);
  reward = height < 2 ? (penalizedBaseReward + fee) : (std::min)((penalizedBaseReward + fee), MAX_BLOCK_REWARD);
  
  return true;
}

uint64_t Currency::calculateInterest(uint64_t amount, uint32_t term, uint32_t height) const {
  assert(m_depositMinTerm <= term && term <= m_depositMaxTerm);
  assert(static_cast<uint64_t>(term)* m_depositMaxTotalRate > m_depositMinTotalRateFactor);

  uint64_t a = static_cast<uint64_t>(term) * m_depositMaxTotalRate - m_depositMinTotalRateFactor;
  uint64_t bHi;
  uint64_t bLo = mul128(amount, a, &bHi);

  uint64_t cHi;
  uint64_t cLo;
  assert(std::numeric_limits<uint32_t>::max() / 100 > m_depositMaxTerm);
  div128_32(bHi, bLo, static_cast<uint32_t>(100 * m_depositMaxTerm), &cHi, &cLo);
  assert(cHi == 0);

  //early depositor multiplier
  uint64_t interestHi;
  uint64_t interestLo;
  if (height <= CryptoNote::parameters::END_MULTIPLIER_BLOCK){
      interestLo = mul128(cLo, CryptoNote::parameters::MULTIPLIER_FACTOR, &interestHi);
      assert(interestHi == 0);
  } else {
      interestHi = cHi;
      interestLo = cLo;
  }

  return interestLo;
}

uint64_t Currency::calculateTotalTransactionInterest(const Transaction& tx, uint32_t height) const {
  uint64_t interest = 0;
  for (const TransactionInput& input : tx.inputs) {
    if (input.type() == typeid(MultisignatureInput)) {
      const MultisignatureInput& multisignatureInput = boost::get<MultisignatureInput>(input);
      if (multisignatureInput.term != 0) {
        interest += calculateInterest(multisignatureInput.amount, multisignatureInput.term, height);
      }
    }
  }

  return interest;
}

uint64_t Currency::getTransactionInputAmount(const TransactionInput& in, uint32_t height) const {
  if (in.type() == typeid(KeyInput)) {
    return boost::get<KeyInput>(in).amount;
  } else if (in.type() == typeid(MultisignatureInput)) {
    const MultisignatureInput& multisignatureInput = boost::get<MultisignatureInput>(in);
    if (multisignatureInput.term == 0) {
      return multisignatureInput.amount;
    } else {
      return multisignatureInput.amount + calculateInterest(multisignatureInput.amount, multisignatureInput.term, height);
    }
  } else if (in.type() == typeid(BaseInput)) {
    return 0;
  } else {
    assert(false);
    return 0;
  }
}

uint64_t Currency::getTransactionAllInputsAmount(const Transaction& tx, uint32_t height) const {
  uint64_t amount = 0;
  for (const auto& in : tx.inputs) {
    amount += getTransactionInputAmount(in, height);
  }
  return amount;
}

bool Currency::getTransactionFee(const Transaction& tx, uint64_t& fee, uint32_t height) const {
  uint64_t amount_in = 0;
  uint64_t amount_out = 0;

  //if (tx.inputs.size() == 0)// || tx.outputs.size() == 0) //0 outputs needed in TestGenerator::constructBlock
  //	  return false;
  
  for (const auto& in : tx.inputs) {
    amount_in += getTransactionInputAmount(in, height);
  }

  for (const auto& o : tx.outputs) {
    amount_out += o.amount;
  }

  if (amount_out > amount_in){
	if (tx.inputs.size() > 0 && tx.outputs.size() > 0 && amount_out > amount_in + parameters::MINIMUM_FEE) //interest shows up in the output of the W/D transactions and W/Ds always have min fee
	  fee = parameters::MINIMUM_FEE;
	else
	  return false;
  } else
	fee = amount_in - amount_out;

  return true;
}

uint64_t Currency::getTransactionFee(const Transaction& tx, uint32_t height) const {
  uint64_t r = 0;
  if (!getTransactionFee(tx, r, height)) {
    r = 0;
  }
  return r;
}

size_t Currency::maxBlockCumulativeSize(uint64_t height) const {
  assert(height <= std::numeric_limits<uint64_t>::max() / m_maxBlockSizeGrowthSpeedNumerator);
  size_t maxSize = static_cast<size_t>(m_maxBlockSizeInitial +
    (height * m_maxBlockSizeGrowthSpeedNumerator) / m_maxBlockSizeGrowthSpeedDenominator);
  assert(maxSize >= m_maxBlockSizeInitial);
  return maxSize;
}

bool Currency::constructMinerTx(uint32_t height, size_t medianSize, uint64_t alreadyGeneratedCoins, size_t currentBlockSize,
  uint64_t fee, const AccountPublicAddress& minerAddress, Transaction& tx,
  const BinaryArray& extraNonce/* = BinaryArray()*/, size_t maxOuts/* = 1*/) const {
  tx.inputs.clear();
  tx.outputs.clear();
  tx.extra.clear();

  KeyPair txkey = generateKeyPair();
  addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);
  if (!extraNonce.empty()) {
    if (!addExtraNonceToTransactionExtra(tx.extra, extraNonce)) {
      return false;
    }
  }

  BaseInput in;
  in.blockIndex = height;

  uint64_t blockReward;
  int64_t emissionChange;
  if (!getBlockReward(medianSize, currentBlockSize, alreadyGeneratedCoins, fee, height, blockReward, emissionChange)) {
    logger(INFO) << "Block is too big";
    return false;
  }

  std::vector<uint64_t> outAmounts;
  decompose_amount_into_digits(blockReward, m_defaultDustThreshold,
    [&outAmounts](uint64_t a_chunk) { outAmounts.push_back(a_chunk); },
    [&outAmounts](uint64_t a_dust) { outAmounts.push_back(a_dust); });

  if (!(1 <= maxOuts)) { logger(ERROR, BRIGHT_RED) << "max_out must be non-zero"; return false; }
  while (maxOuts < outAmounts.size()) {
    outAmounts[outAmounts.size() - 2] += outAmounts.back();
    outAmounts.resize(outAmounts.size() - 1);
  }

  uint64_t summaryAmounts = 0;
  for (size_t no = 0; no < outAmounts.size(); no++) {
    Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
    Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();

    bool r = Crypto::generate_key_derivation(minerAddress.viewPublicKey, txkey.secretKey, derivation);

    if (!(r)) {
      logger(ERROR, BRIGHT_RED)
        << "while creating outs: failed to generate_key_derivation("
        << minerAddress.viewPublicKey << ", " << txkey.secretKey << ")";
      return false;
    }

    r = Crypto::derive_public_key(derivation, no, minerAddress.spendPublicKey, outEphemeralPubKey);

    if (!(r)) {
      logger(ERROR, BRIGHT_RED)
        << "while creating outs: failed to derive_public_key("
        << derivation << ", " << no << ", "
        << minerAddress.spendPublicKey << ")";
      return false;
    }

    KeyOutput tk;
    tk.key = outEphemeralPubKey;

    TransactionOutput out;
    summaryAmounts += out.amount = outAmounts[no];
    out.target = tk;
    tx.outputs.push_back(out);
  }

  if (!(summaryAmounts == blockReward)) {
    logger(ERROR, BRIGHT_RED) << "Failed to construct miner tx, summaryAmounts = " << summaryAmounts << " not equal blockReward = " << blockReward;
    return false;
  }

  tx.version = TRANSACTION_VERSION_1;
  //lock
  tx.unlockTime = height + m_minedMoneyUnlockWindow;
  tx.inputs.push_back(in);
  return true;
}

bool Currency::isFusionTransaction(const std::vector<uint64_t>& inputsAmounts, const std::vector<uint64_t>& outputsAmounts, size_t size) const {
  if (size > fusionTxMaxSize()) {
    return false;
  }

  if (inputsAmounts.size() < fusionTxMinInputCount()) {
    return false;
  }

  if (inputsAmounts.size() < outputsAmounts.size() * fusionTxMinInOutCountRatio()) {
    return false;
  }

  uint64_t inputAmount = 0;
  for (auto amount: inputsAmounts) {
    if (amount < defaultDustThreshold()) {
      return false;
    }

    inputAmount += amount;
  }

  std::vector<uint64_t> expectedOutputsAmounts;
  expectedOutputsAmounts.reserve(outputsAmounts.size());
  decomposeAmount(inputAmount, defaultDustThreshold(), expectedOutputsAmounts);
  std::sort(expectedOutputsAmounts.begin(), expectedOutputsAmounts.end());

  return expectedOutputsAmounts == outputsAmounts;
}

bool Currency::isFusionTransaction(const Transaction& transaction, size_t size) const {
  assert(getObjectBinarySize(transaction) == size);

  std::vector<uint64_t> outputsAmounts;
  outputsAmounts.reserve(transaction.outputs.size());
  for (const TransactionOutput& output : transaction.outputs) {
    outputsAmounts.push_back(output.amount);
  }

  return isFusionTransaction(getInputsAmounts(transaction), outputsAmounts, size);
}

bool Currency::isFusionTransaction(const Transaction& transaction) const {
  return isFusionTransaction(transaction, getObjectBinarySize(transaction));
}

bool Currency::isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold) const {
  uint8_t ignore;
  return isAmountApplicableInFusionTransactionInput(amount, threshold, ignore);
}

bool Currency::isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold, uint8_t& amountPowerOfTen) const {
  if (amount >= threshold) {
    return false;
  }

  if (amount < defaultDustThreshold()) {
    return false;
  }

  auto it = std::lower_bound(PRETTY_AMOUNTS.begin(), PRETTY_AMOUNTS.end(), amount);
  if (it == PRETTY_AMOUNTS.end() || amount != *it) {
    return false;
  } 

  amountPowerOfTen = static_cast<uint8_t>(std::distance(PRETTY_AMOUNTS.begin(), it) / 9);
  return true;
}

std::string Currency::accountAddressAsString(const AccountBase& account) const {
  return getAccountAddressAsStr(m_publicAddressBase58Prefix, account.getAccountKeys().address);
}

std::string Currency::accountAddressAsString(const AccountPublicAddress& accountPublicAddress) const {
  return getAccountAddressAsStr(m_publicAddressBase58Prefix, accountPublicAddress);
}

bool Currency::parseAccountAddressString(const std::string& str, AccountPublicAddress& addr) const {
  uint64_t prefix;
  if (!CryptoNote::parseAccountAddressString(prefix, addr, str)) {
    return false;
  }

  if (prefix != m_publicAddressBase58Prefix) {
    logger(DEBUGGING) << "Wrong address prefix: " << prefix << ", expected " << m_publicAddressBase58Prefix;
    return false;
  }

  return true;
}

std::string Currency::formatAmount(uint64_t amount) const {
  std::string s = std::to_string(amount);
  if (s.size() < m_numberOfDecimalPlaces + 1) {
    s.insert(0, m_numberOfDecimalPlaces + 1 - s.size(), '0');
  }
  s.insert(s.size() - m_numberOfDecimalPlaces, ".");
  return s;
}

std::string Currency::formatAmount(int64_t amount) const {
  std::string s = formatAmount(static_cast<uint64_t>(std::abs(amount)));

  if (amount < 0) {
    s.insert(0, "-");
  }

  return s;
}

bool Currency::parseAmount(const std::string& str, uint64_t& amount) const {
  std::string strAmount = str;
  boost::algorithm::trim(strAmount);

  size_t pointIndex = strAmount.find_first_of('.');
  size_t fractionSize;
  if (std::string::npos != pointIndex) {
    fractionSize = strAmount.size() - pointIndex - 1;
    while (m_numberOfDecimalPlaces < fractionSize && '0' == strAmount.back()) {
      strAmount.erase(strAmount.size() - 1, 1);
      --fractionSize;
    }
    if (m_numberOfDecimalPlaces < fractionSize) {
      return false;
    }
    strAmount.erase(pointIndex, 1);
  } else {
    fractionSize = 0;
  }

  if (strAmount.empty()) {
    return false;
  }

  if (!std::all_of(strAmount.begin(), strAmount.end(), ::isdigit)) {
    return false;
  }

  if (fractionSize < m_numberOfDecimalPlaces) {
    strAmount.append(m_numberOfDecimalPlaces - fractionSize, '0');
  }

  return Common::fromString(strAmount, amount);
}


// @nesterow: old difficulty alhorithm, i.e. 1st
//
// HF version 0 = Monero DAA

difficulty_type Currency::nextDifficulty1(std::vector<uint64_t> timestamps,
  std::vector<difficulty_type> cumulativeDifficulties) const {
  assert(m_difficultyWindow >= 2);
  
  if (timestamps.size() > m_difficultyWindow) {
    timestamps.resize(m_difficultyWindow);
    cumulativeDifficulties.resize(m_difficultyWindow);
  }

  size_t length = timestamps.size();
  assert(length == cumulativeDifficulties.size());
  assert(length <= m_difficultyWindow);
  if (length <= 1) {
    return 1;
  }

  sort(timestamps.begin(), timestamps.end());

  size_t cutBegin, cutEnd;
  assert(2 * m_difficultyCut <= m_difficultyWindow - 2);
  if (length <= m_difficultyWindow - 2 * m_difficultyCut) {
    cutBegin = 0;
    cutEnd = length;
  } else {
    cutBegin = (length - (m_difficultyWindow - 2 * m_difficultyCut) + 1) / 2;
    cutEnd = cutBegin + (m_difficultyWindow - 2 * m_difficultyCut);
  }
  assert(/*cut_begin >= 0 &&*/ cutBegin + 2 <= cutEnd && cutEnd <= length);
  uint64_t timeSpan = timestamps[cutEnd - 1] - timestamps[cutBegin];
  if (timeSpan == 0) {
    timeSpan = 1;
  }

  difficulty_type totalWork = cumulativeDifficulties[cutEnd - 1] - cumulativeDifficulties[cutBegin];
  assert(totalWork > 0);

  uint64_t low, high;
  low = mul128(totalWork, m_difficultyTarget, &high);
  if (high != 0 || low + timeSpan - 1 < low) {
    return 0;
  }

  return (low + timeSpan - 1) / timeSpan;
}

// @nesterow: 2nd diff algo, Sumokoin impl
//
// HF v1 = Zawy/Sumokoin Poisson
difficulty_type Currency::nextDifficulty2(std::vector<uint64_t> timestamps,
  std::vector<difficulty_type> cumulativeDifficulties) const {
  assert(m_difficultyWindow >= 2);
  
  size_t window_v1 = parameters::DIFFICULTY_WINDOW_V1;
  
  if (timestamps.size() > window_v1) {
    timestamps.resize(window_v1);
    cumulativeDifficulties.resize(window_v1);
  }

  size_t length = timestamps.size();
  assert(length == cumulativeDifficulties.size());
  assert(length <= window_v1);
  if (length <= 1) {
    return 1;
  }

  sort(timestamps.begin(), timestamps.end());
  
  size_t cutBegin, cutEnd;
  assert(2 * m_difficultyCut <= window_v1 - 2);
  if (length <= window_v1 - 2 * window_v1) {
    cutBegin = 0;
    cutEnd = length;
  } else {
    cutBegin = (length - (window_v1 - 2 * m_difficultyCut) + 1) / 2;
    cutEnd = cutBegin + (m_difficultyWindow - 2 * m_difficultyCut);
  }
  assert(/*cut_begin >= 0 &&*/ cutBegin + 2 <= cutEnd && cutEnd <= length);
  
  uint64_t timeSpan = timestamps[cutEnd - 1] - timestamps[cutBegin];
  if (timeSpan == 0) {
    timeSpan = 1;
  }
  
  uint64_t timespanMedian = 0;
  if (cutBegin > 0 && length >= cutBegin * 2 + 3){
      std::vector<std::uint64_t> time_spans;
      for (size_t i = length - cutBegin * 2 - 3; i < length - 1; i++){
        uint64_t time_span = timestamps[i + 1] - timestamps[i];
        if (time_span == 0) {
          time_span = 1;
        }
        time_spans.push_back(time_span);
      }
      double median;
      size_t spans_size = time_spans.size();
      sort(time_spans.begin(), time_spans.end());
      if (spans_size % 2 == 0){
        median = (time_spans[spans_size / 2 - 1] + time_spans[spans_size / 2]) / 2.0;
      }
      else{
        median = static_cast<double>(time_spans[spans_size / 2]);
      }
      timespanMedian = static_cast<uint64_t>(median);
  }
  
  uint64_t timespanLength = length - cutBegin * 2 - 1;
  uint64_t totalTimespanMedian = timespanMedian > 0 ? timespanMedian * timespanLength : timeSpan * 7 / 10; 
  uint64_t adjustedTotalTimespan = (timeSpan * 8 + totalTimespanMedian * 3) / 10; //Sumocoin: [ref: poisson distribution]
  if (adjustedTotalTimespan > MAX_AVERAGE_TIMESPAN * timespanLength){
      adjustedTotalTimespan = MAX_AVERAGE_TIMESPAN * timespanLength;
  }
  if (adjustedTotalTimespan < MIN_AVERAGE_TIMESPAN * timespanLength){
      adjustedTotalTimespan = MIN_AVERAGE_TIMESPAN * timespanLength;
  }
  
  difficulty_type totalWork = cumulativeDifficulties[cutEnd - 1] - cumulativeDifficulties[cutBegin];
  assert(totalWork > 0);

  uint64_t low, high;
  low = mul128(totalWork, m_difficultyTarget, &high);
  if (high != 0 || low + timeSpan - 1 < low) {
    return 0;
  }
  
  uint64_t nextDiff = (low + adjustedTotalTimespan - 1) / adjustedTotalTimespan;
 if (nextDiff < 1) {
  nextDiff = 1;
  }
  return nextDiff;
 }

//@nesterow: Third diff algo, zawy's LWMA
//
// HF v3 - Zawy LWMA

// LWMA difficulty algorithm
// Copyright (c) 2017-2018 Zawy
// https://github.com/zawy12/difficulty-algorithms/issues/3
// https://github.com/zawy12/difficulty-algorithms/issues/3#issuecomment-375600752
difficulty_type Currency::nextDifficulty3(std::vector<uint64_t> timestamps,
  std::vector<difficulty_type> cumulativeDifficulties) const {

  const int64_t T = static_cast<int64_t>(m_difficultyTarget);

  int64_t N = static_cast<int64_t>(parameters::DIFFICULTY_WINDOW_V2) -1;

  if ( timestamps.size() < 4)
  { 
    return 1;
  } else if(
    static_cast<int64_t>(timestamps.size()) < N + 1 // -Wsign-compare
  ) 
  {
    N = timestamps.size() - 1;
  }
  else
  {
    timestamps.resize(N + 1);
    cumulativeDifficulties.resize(N + 1);
  }

  const double adjust = 0.998;
  
  // The divisor k normalizes the LWMA sum to a standard LWMA.
  const double k = N * (N + 1) / 2.0;
  
  double LWMA(0), sum_inverse_D(0), harmonic_mean_D(0), nextDifficulty(0);
  int64_t solveTime(0);
  uint64_t difficulty(0); 
  
  // Loop through N most recent blocks. N is most recently solved block.
    for (int64_t i = 1; i <= N; i++) {
      solveTime = static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i - 1]);
      solveTime = std::min<int64_t>((T*7), std::max<int64_t>(solveTime, (-7 * T)));
      difficulty = cumulativeDifficulties[i] - cumulativeDifficulties[i - 1];
      LWMA += (int64_t)(solveTime * i) / k;
      sum_inverse_D += 1 / static_cast<double>(difficulty);
  }

  harmonic_mean_D = N / sum_inverse_D * adjust;

  // Limit LWMA same as Bitcoin's 1/4 in case something unforeseen occurs.
  if (static_cast<int64_t>(boost::math::round(LWMA)) < T / 4)
    LWMA = static_cast<double>(T / 4);

  nextDifficulty = harmonic_mean_D * T / LWMA;
  uint64_t next_difficulty = static_cast<uint64_t>(nextDifficulty);

 //@karbowanec: minimum limit
  logger(TRACE) << "Timestamps size: " << timestamps.size();
  logger(TRACE) << "Block expected diff (next_difficulty LWMA-1): " << next_difficulty << ".  (TESTNET DEBUGGING)";

  if (next_difficulty < 100000) {
    next_difficulty = 100000; }

  return next_difficulty;
}


// HFv4 - LWMA-2, some remnants of LWMA-1 mixed in too...
// Note: version 4b "lower" boolean parameter needed to remove
// lower bound from L708 because it is screwing things up
// We really should NOT need a lower limit anyway, but I
// assume it was put here to hardcode diff through a fork..

// LWMA-2 difficulty algorithm
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
// See commented version below for required config file changes.
// Make sure timestamps and cumulativeDifficulties vectors are sized N+1
// and most recent element (Nth one) is most recently solved block.
// difficulty_type should be uint64_t
difficulty_type Currency::nextDifficulty(std::vector<uint64_t> timestamps, std::vector<difficulty_type> cumulativeDifficulties, bool lower) const {
    int64_t  T = parameters::DIFFICULTY_TARGET;
    int64_t  N = parameters::DIFFICULTY_WINDOW_V3 - 1; // N=45, 60, and 90 for T=600, 120, 60.
    int64_t  FTL = static_cast<int64_t>(parameters::CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V3); // FTL=3xT
    int64_t  L(0), ST, sum_3_ST(0), next_D, prev_D;

    #ifndef _WIN32
    std::ignore = FTL;
    #endif

  if ( timestamps.size() < 4)
  {
    return 1;
  } else if(
    static_cast<int64_t>(timestamps.size()) < N + 1 // -Wsign-compare
  )
  {
    N = timestamps.size() - 1;
  }
  else
  {
    timestamps.resize(N + 1);
    cumulativeDifficulties.resize(N + 1);
  }

  const double k = N * (N + 1) / 2.0;

  double LWMA(0), sum_inverse_D(0);
  int64_t solveTime(0);
  uint64_t difficulty(0);

  // Loop through N most recent blocks. N is most recently solved block.
    for (int64_t i = 1; i <= N; i++) {
      solveTime = static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i - 1]);
      solveTime = std::min<int64_t>((T*7), std::max<int64_t>(solveTime, (-7 * T)));
      difficulty = cumulativeDifficulties[i] - cumulativeDifficulties[i - 1];
      LWMA += (int64_t)(solveTime * i) / k;
      sum_inverse_D += 1 / static_cast<double>(difficulty);
  }

    logger(TRACE) << "Timestamps size: " << timestamps.size() << ", expected more than " << static_cast<uint64_t>(N);

    if ((lower) && (timestamps.size() <= static_cast<uint64_t>(N)))
    {
        uint64_t difficulty_guess = 10000;
        return difficulty_guess;
    }

   else if ((!lower) && (timestamps.size() <= static_cast<uint64_t>(N)))
    {
        uint64_t difficulty_guess = 100000;
        return difficulty_guess;
    }

//    uint64_t initial_difficulty_guess = 100;
//    if (timestamps.size() <= static_cast<uint64_t>(N)) {
//        return initial_difficulty_guess;
//    }

    for ( int64_t i = 1; i <= N; i++) {
        ST = std::max(-6*T, std::min( static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i - 1]), 6*T));
        L +=  ST * i ;
        if ( i > N-3 ) { sum_3_ST += ST; }
    }
    next_D = ((cumulativeDifficulties[N] - cumulativeDifficulties[0])*T*(N+1)*99)/(100*2*L);

    // implement LWMA-2 changes from LWMA
    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N-1];
    // If N !=60 adjust 3 integers: 67*N/60, 150*60/N, 110*60/N
    next_D = std::max((prev_D*67)/100, std::min( next_D, (prev_D*150)/100));
    if ( sum_3_ST < (8*T)/10) {  next_D = std::max(next_D,(prev_D*110)/100); }

    return static_cast<uint64_t>(next_D);
}

bool Currency::checkProofOfWork(Crypto::cn_context& context, const Block& block, difficulty_type currentDiffic, Crypto::Hash& proofOfWork) const {

    switch (block.majorVersion) {
    case BLOCK_MAJOR_VERSION_1:
    case BLOCK_MAJOR_VERSION_2:
        return checkProofOfWorkV1(context, block, currentDiffic, proofOfWork);

    case BLOCK_MAJOR_VERSION_3:
        return checkProofOfWorkV2(context, block, currentDiffic, proofOfWork);
    }

    logger(ERROR, BRIGHT_RED) << "Unknown block major version: " << block.majorVersion << "." << block.minorVersion;
    return false;
}

bool Currency::checkProofOfWorkV1(Crypto::cn_context& context, const Block& block, difficulty_type currentDiffic,
        Crypto::Hash& proofOfWork) const {
    if (!get_block_longhash(context, block, proofOfWork)) {
        return false;
    }

    return check_hash(proofOfWork, currentDiffic);
}

bool Currency::checkProofOfWorkV2(Crypto::cn_context& context, const Block& block, difficulty_type currentDiffic,
        Crypto::Hash& proofOfWork) const {

    if (block.majorVersion <= BLOCK_MAJOR_VERSION_3 + 10) {
	return true;
    }

    if (!get_block_longhash(context, block, proofOfWork)) {
        return false;
    }

    if (!check_hash(proofOfWork, currentDiffic)) {
        return false;
    }

    if (8 * sizeof(m_genesisBlockHash) < block.rootBlock.blockchainBranch.size()) {
        return false;
    }

    Crypto::Hash auxBlockHeaderHash;
    if (!get_aux_block_header_hash(block, auxBlockHeaderHash)) {
        return false;
    }
    return true;
}

size_t Currency::getApproximateMaximumInputCount(size_t transactionSize, size_t outputCount, size_t mixinCount) const {
  const size_t KEY_IMAGE_SIZE = sizeof(Crypto::KeyImage);
  const size_t OUTPUT_KEY_SIZE = sizeof(decltype(KeyOutput::key));
  const size_t AMOUNT_SIZE = sizeof(uint64_t) + 2; //varint
  const size_t GLOBAL_INDEXES_VECTOR_SIZE_SIZE = sizeof(uint8_t);//varint
  const size_t GLOBAL_INDEXES_INITIAL_VALUE_SIZE = sizeof(uint32_t);//varint
  const size_t GLOBAL_INDEXES_DIFFERENCE_SIZE = sizeof(uint32_t);//varint
  const size_t SIGNATURE_SIZE = sizeof(Crypto::Signature);
  const size_t EXTRA_TAG_SIZE = sizeof(uint8_t);
  const size_t INPUT_TAG_SIZE = sizeof(uint8_t);
  const size_t OUTPUT_TAG_SIZE = sizeof(uint8_t);
  const size_t PUBLIC_KEY_SIZE = sizeof(Crypto::PublicKey);
  const size_t TRANSACTION_VERSION_SIZE = sizeof(uint8_t);
  const size_t TRANSACTION_UNLOCK_TIME_SIZE = sizeof(uint64_t);

  const size_t outputsSize = outputCount * (OUTPUT_TAG_SIZE + OUTPUT_KEY_SIZE + AMOUNT_SIZE);
  const size_t headerSize = TRANSACTION_VERSION_SIZE + TRANSACTION_UNLOCK_TIME_SIZE + EXTRA_TAG_SIZE + PUBLIC_KEY_SIZE;
  const size_t inputSize = INPUT_TAG_SIZE + AMOUNT_SIZE + KEY_IMAGE_SIZE + SIGNATURE_SIZE + GLOBAL_INDEXES_VECTOR_SIZE_SIZE + GLOBAL_INDEXES_INITIAL_VALUE_SIZE +
                            mixinCount * (GLOBAL_INDEXES_DIFFERENCE_SIZE + SIGNATURE_SIZE);

  return (transactionSize - headerSize - outputsSize) / inputSize;
}

CurrencyBuilder::CurrencyBuilder(Logging::ILogger& log) : m_currency(log) {
  maxBlockNumber(parameters::CRYPTONOTE_MAX_BLOCK_NUMBER);
  maxBlockBlobSize(parameters::CRYPTONOTE_MAX_BLOCK_BLOB_SIZE);
  maxTxSize(parameters::CRYPTONOTE_MAX_TX_SIZE);
  publicAddressBase58Prefix(parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
  minedMoneyUnlockWindow(parameters::CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW);

  timestampCheckWindow(parameters::BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW);
  blockFutureTimeLimit(parameters::CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT);
  blockFutureTimeLimit_v2(parameters::CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V2);
  
  moneySupply(parameters::MONEY_SUPPLY);
//  genesisBlockReward(parameters::GENESIS_BLOCK_REWARD);

  rewardBlocksWindow(parameters::CRYPTONOTE_REWARD_BLOCKS_WINDOW);
  blockGrantedFullRewardZone(parameters::CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
  minerTxBlobReservedSize(parameters::CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE);

  numberOfDecimalPlaces(parameters::CRYPTONOTE_DISPLAY_DECIMAL_POINT);

  mininumFee(parameters::MINIMUM_FEE);
  defaultDustThreshold(parameters::DEFAULT_DUST_THRESHOLD);

  difficultyTarget(parameters::DIFFICULTY_TARGET);
  difficultyWindow(parameters::DIFFICULTY_WINDOW);
  difficultyLag(parameters::DIFFICULTY_LAG);
  difficultyCut(parameters::DIFFICULTY_CUT);

  depositMinAmount(parameters::DEPOSIT_MIN_AMOUNT);
  depositMinTerm(parameters::DEPOSIT_MIN_TERM);
  depositMaxTerm(parameters::DEPOSIT_MAX_TERM);
  depositMinTotalRateFactor(parameters::DEPOSIT_MIN_TOTAL_RATE_FACTOR);
  depositMaxTotalRate(parameters::DEPOSIT_MAX_TOTAL_RATE);

  maxBlockSizeInitial(parameters::MAX_BLOCK_SIZE_INITIAL);
  maxBlockSizeGrowthSpeedNumerator(parameters::MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR);
  maxBlockSizeGrowthSpeedDenominator(parameters::MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR);

  lockedTxAllowedDeltaSeconds(parameters::CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS);
  lockedTxAllowedDeltaBlocks(parameters::CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS);

  mempoolTxLiveTime(parameters::CRYPTONOTE_MEMPOOL_TX_LIVETIME);
  mempoolTxFromAltBlockLiveTime(parameters::CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME);
  numberOfPeriodsToForgetTxDeletedFromPool(parameters::CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL);

  upgradeHeightV2(parameters::UPGRADE_HEIGHT_V2);
  upgradeHeightV3(parameters::UPGRADE_HEIGHT_V3);
  upgradeVotingThreshold(parameters::UPGRADE_VOTING_THRESHOLD);
  upgradeVotingWindow(parameters::UPGRADE_VOTING_WINDOW);
  upgradeWindow(parameters::UPGRADE_WINDOW);

  fusionTxMaxSize(parameters::FUSION_TX_MAX_SIZE);
  fusionTxMinInputCount(parameters::FUSION_TX_MIN_INPUT_COUNT);
  fusionTxMinInOutCountRatio(parameters::FUSION_TX_MIN_IN_OUT_COUNT_RATIO);

  blocksFileName(parameters::CRYPTONOTE_BLOCKS_FILENAME);
  blocksCacheFileName(parameters::CRYPTONOTE_BLOCKSCACHE_FILENAME);
  blockIndexesFileName(parameters::CRYPTONOTE_BLOCKINDEXES_FILENAME);
  txPoolFileName(parameters::CRYPTONOTE_POOLDATA_FILENAME);
  blockchinIndicesFileName(parameters::CRYPTONOTE_BLOCKCHAIN_INDICES_FILENAME);

  testnet(false);
}

Transaction CurrencyBuilder::generateGenesisTransaction() {
  CryptoNote::Transaction tx;
  CryptoNote::AccountPublicAddress ac = boost::value_initialized<CryptoNote::AccountPublicAddress>();
  m_currency.constructMinerTx(0, 0, 0, 0, 0, ac, tx); // zero fee in genesis

  return tx;
}

// Transaction CurrencyBuilder::generateGenesisTransaction(const std::vector<AccountPublicAddress>& targets) {
//     assert(!targets.empty());
// 
//     CryptoNote::Transaction tx;
//     tx.inputs.clear();
//     tx.outputs.clear();
//     tx.extra.clear();
//     tx.version = TRANSACTION_VERSION_1;
//     tx.unlockTime = m_currency.m_minedMoneyUnlockWindow;
//     KeyPair txkey = generateKeyPair();
//     addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);
//     BaseInput in;
//     in.blockIndex = 0;
//     tx.inputs.push_back(in);
//     uint64_t block_reward = m_currency.m_genesisBlockReward;
//     uint64_t target_amount = block_reward / targets.size();
//     uint64_t first_target_amount = target_amount + block_reward % targets.size();
//     for (size_t i = 0; i < targets.size(); ++i) {
//       Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
//       Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();
//       bool r = Crypto::generate_key_derivation(targets[i].viewPublicKey, txkey.secretKey, derivation);
//       assert(r == true);
 //      CHECK_AND_ASSERT_MES(r, false, "while creating outs: failed to generate_key_derivation(" << targets[i].viewPublicKey << ", " << txkey.sec << ")");
 //      r = Crypto::derive_public_key(derivation, i, targets[i].spendPublicKey, outEphemeralPubKey);
//       assert(r == true);
//  //     CHECK_AND_ASSERT_MES(r, false, "while creating outs: failed to derive_public_key(" << derivation << ", " << i << ", " << targets[i].spendPublicKey << ")");
//       KeyOutput tk;
//       tk.key = outEphemeralPubKey;
//       TransactionOutput out;
//       out.amount = (i == 0) ? first_target_amount : target_amount;
//       std::cout << "outs: " << std::to_string(out.amount) << std::endl;
//       out.target = tk;
//       tx.outputs.push_back(out);
//     }
//     return tx;
// }

CurrencyBuilder& CurrencyBuilder::numberOfDecimalPlaces(size_t val) {
  m_currency.m_numberOfDecimalPlaces = val;
  m_currency.m_coin = 1;
  for (size_t i = 0; i < m_currency.m_numberOfDecimalPlaces; ++i) {
    m_currency.m_coin *= 10;
  }

  return *this;
}

CurrencyBuilder& CurrencyBuilder::difficultyWindow(size_t val) {
  if (val < 2) {
    throw std::invalid_argument("val at difficultyWindow()");
  }
  m_currency.m_difficultyWindow = val;
  return *this;
}

CurrencyBuilder& CurrencyBuilder::upgradeVotingThreshold(unsigned int val) {
  if (val <= 0 || val > 100) {
    throw std::invalid_argument("val at upgradeVotingThreshold()");
  }
  
  m_currency.m_upgradeVotingThreshold = val;
  return *this;
}

CurrencyBuilder& CurrencyBuilder::upgradeWindow(size_t val) {
  if (val <= 0) {
    throw std::invalid_argument("val at upgradeWindow()");
  }
  
  m_currency.m_upgradeWindow = static_cast<uint32_t>(val);
  return *this;
}

}
