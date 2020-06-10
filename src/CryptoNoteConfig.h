// Copyright (c) 2011-2017 The Cryptonote developers
// Copyright (c) 2014-2017 XDN developers
// Copyright (c) 2016-2017 BXC developers
// Copyright (c) 2017-2020 UltraNote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

//pragma once not working so using belo
#ifndef CRYPTONOTECONFIG
#define CRYPTONOTECONFIG

#include <cstdint>
#include <initializer_list>
#include <map>

namespace CryptoNote {

namespace parameters {

const uint64_t DIFFICULTY_TARGET                             = 120; // seconds

const uint64_t CRYPTONOTE_MAX_BLOCK_NUMBER                   = 500000000;
const size_t   CRYPTONOTE_MAX_BLOCK_BLOB_SIZE                = 500000000;
const size_t   CRYPTONOTE_MAX_TX_SIZE                        = 1000000000;
const uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX       = 0x1a2638; // addresses start with "Xun3"
const size_t   CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW          = 10;
const uint64_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT            = 60 * 60 * 2;
const uint64_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V2         = 6 * 60 * 2;
const uint64_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V3         = DIFFICULTY_TARGET * 3;
const uint64_t CRYPTONOTE_DEFAULT_TX_SPENDABLE_AGE           = 10;

const size_t   BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW             = 60;
const size_t   BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW_V2          = 11;

const uint64_t MONEY_SUPPLY				                     = UINT64_C(2100000000000000000);

const size_t   CRYPTONOTE_REWARD_BLOCKS_WINDOW               = 100;
const size_t   CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE     = 100000; //size of block (bytes) after which reward for block calculated using block size
const size_t   CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE        = 600;
const size_t   CRYPTONOTE_DISPLAY_DECIMAL_POINT              = 6;
// COIN - number of smallest units in one coin
const uint64_t POINT                                         = UINT64_C(1000);        // pow(10, 3)
const uint64_t COIN                                          = UINT64_C(1000000);     // pow(10, 6)
const uint64_t MINIMUM_FEE                                   = UINT64_C(1000);        // pow(10, 3)
const uint64_t DEFAULT_DUST_THRESHOLD                        = UINT64_C(100);         // pow(10, 2)

const uint64_t EXPECTED_NUMBER_OF_BLOCKS_PER_DAY             = 24 * 60 * 60 / DIFFICULTY_TARGET;

const size_t   DIFFICULTY_WINDOW                             = 20;
const size_t   DIFFICULTY_CUT                                = 5;

const size_t   DIFFICULTY_WINDOW_V1                          = 35;
const size_t   DIFFICULTY_WINDOW_V2                          = 60;
const size_t   DIFFICULTY_WINDOW_V3                          = 60 + 1;

const size_t   DIFFICULTY_LAG                                = 1; //not used in v1,v2


static_assert(2 * DIFFICULTY_CUT <= DIFFICULTY_WINDOW - 2, "Bad DIFFICULTY_WINDOW or DIFFICULTY_CUT");

const uint64_t DEPOSIT_MIN_AMOUNT                            = 1 * COIN;
const uint32_t DEPOSIT_MIN_TERM                              = 22000; //~1 month
const uint32_t DEPOSIT_MAX_TERM                              = 1 * 12 * 22000; //~1 year
const uint64_t DEPOSIT_MIN_TOTAL_RATE_FACTOR                 = 0; //rate is constant
const uint64_t DEPOSIT_MAX_TOTAL_RATE                        = 3; //percentage rate for DEPOSIT_MAX_TERM
static_assert(DEPOSIT_MIN_TERM > 0, "Bad DEPOSIT_MIN_TERM");
static_assert(DEPOSIT_MIN_TERM <= DEPOSIT_MAX_TERM, "Bad DEPOSIT_MAX_TERM");
static_assert(DEPOSIT_MIN_TERM * DEPOSIT_MAX_TOTAL_RATE > DEPOSIT_MIN_TOTAL_RATE_FACTOR, "Bad DEPOSIT_MIN_TOTAL_RATE_FACTOR or DEPOSIT_MAX_TOTAL_RATE");

const uint64_t MULTIPLIER_FACTOR                             = 250; //early depositor multiplier
const uint32_t END_MULTIPLIER_BLOCK                          = 3600; //block at which the multiplier ceases to be applied

const size_t   MAX_BLOCK_SIZE_INITIAL                        = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE * 10;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR         = 100 * 1024;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR       = 365 * 24 * 60 * 60 / DIFFICULTY_TARGET;

const uint64_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS     = 1;
const uint64_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS    = DIFFICULTY_TARGET * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS;

const uint64_t CRYPTONOTE_MEMPOOL_TX_LIVETIME                = (60 * 60 * 14); //seconds, 14 hours
const uint64_t CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME = (60 * 60 * 24); //seconds, one day
const uint64_t CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL = 7;  // CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL * CRYPTONOTE_MEMPOOL_TX_LIVETIME = time to forget tx

const size_t   FUSION_TX_MAX_SIZE                            = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE * 30 / 100;
const size_t   FUSION_TX_MIN_INPUT_COUNT                     = 12;
const size_t   FUSION_TX_MIN_IN_OUT_COUNT_RATIO              = 4;

const uint32_t UPGRADE_HEIGHT_V2                             = 1;
const uint32_t UPGRADE_HEIGHT_V3                             = 191998;

const unsigned UPGRADE_VOTING_THRESHOLD                      = 90;               // percent
const size_t   UPGRADE_VOTING_WINDOW                         = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;  // blocks
const size_t   UPGRADE_WINDOW                                = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;  // blocks
static_assert(0 < UPGRADE_VOTING_THRESHOLD && UPGRADE_VOTING_THRESHOLD <= 100, "Bad UPGRADE_VOTING_THRESHOLD");
static_assert(UPGRADE_VOTING_WINDOW > 1, "Bad UPGRADE_VOTING_WINDOW");

const char     CRYPTONOTE_BLOCKS_FILENAME[]                  = "blocks.dat";
const char     CRYPTONOTE_BLOCKINDEXES_FILENAME[]            = "blockindexes.dat";
const char     CRYPTONOTE_BLOCKSCACHE_FILENAME[]             = "blockscache.dat";
const char     CRYPTONOTE_POOLDATA_FILENAME[]                = "poolstate.bin";
const char     P2P_NET_DATA_FILENAME[]                       = "p2pstate.bin";
const char     CRYPTONOTE_BLOCKCHAIN_INDICES_FILENAME[]      = "blockchainindices.dat";
const char     MINER_CONFIG_FILE_NAME[]                      = "miner_conf.json";

} // parameters

const uint64_t START_BLOCK_REWARD                            = (UINT64_C(150) * parameters::COIN);
const uint64_t ICO_BLOCK_REWARD	                             = (UINT64_C(18446744073) * parameters::COIN); // 18.4 billion ICO
const uint64_t BLOCK_REWARD_V2                               = (UINT64_C(300) * parameters::COIN);
const uint32_t BLOCK_REWARD_V2_HEIGHT                        = 456000;
const uint64_t MAX_BLOCK_REWARD                              = (UINT64_C(1500) * parameters::COIN);
const uint64_t REWARD_INCREASE_INTERVAL                      = (UINT64_C(264000));

const char     CRYPTONOTE_NAME[]                             = "UltraNote";
const char     GENESIS_COINBASE_TX_HEX[]                     = "010a01ff000180a3c347029b2e4c0281c0b02e7c53291a94d1d0cbff8883f8024f5142ee494ffbbd0880712101677642503be486533f0aacb7ccce24a3d4931635fa700d62bf6b383cf26436e9";
const uint32_t GENESIS_NONCE                                 = 70;
const uint64_t GENESIS_TIMESTAMP                             = 1496764095;

const uint8_t  TRANSACTION_VERSION_1                         =  1;
const uint8_t  TRANSACTION_VERSION_2                         =  2;

const uint8_t  BLOCK_MAJOR_VERSION_1                         =  1;
const uint8_t  BLOCK_MAJOR_VERSION_2                         =  2;
const uint8_t  BLOCK_MAJOR_VERSION_3                         =  3;
const uint8_t  BLOCK_MAJOR_VERSION_4                         =  4;

const uint8_t  BLOCK_MINOR_VERSION_0                         =  0;
const uint8_t  BLOCK_MINOR_VERSION_1                         =  1;

const size_t   BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT        =  10000;  //by default, blocks ids count in synchronizing
const size_t   BLOCKS_SYNCHRONIZING_DEFAULT_COUNT            =  200;    //by default, blocks count in blocks downloading
const size_t   COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT         =  1000;

const int      P2P_DEFAULT_PORT                              = 32000;
const int      RPC_DEFAULT_PORT                              = 33000;

const size_t   P2P_LOCAL_WHITE_PEERLIST_LIMIT                =  1000;
const size_t   P2P_LOCAL_GRAY_PEERLIST_LIMIT                 =  5000;

const size_t   P2P_CONNECTION_MAX_WRITE_BUFFER_SIZE          = 64 * 1024 * 1024; // 64 MB
const uint32_t P2P_DEFAULT_CONNECTIONS_COUNT                 = 8;
const size_t   P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT     = 70;
const uint32_t P2P_DEFAULT_HANDSHAKE_INTERVAL                = 60;            // seconds
const uint32_t P2P_DEFAULT_PACKET_MAX_SIZE                   = 50000000;      // 50000000 bytes maximum packet size
const uint32_t P2P_DEFAULT_PEERS_IN_HANDSHAKE                = 250;
const uint32_t P2P_DEFAULT_CONNECTION_TIMEOUT                = 5000;          // 5 seconds
const uint32_t P2P_DEFAULT_PING_CONNECTION_TIMEOUT           = 2000;          // 2 seconds
const uint64_t P2P_DEFAULT_INVOKE_TIMEOUT                    = 60 * 2 * 1000; // 2 minutes
const size_t   P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT          = 5000;          // 5 seconds
const char     P2P_STAT_TRUSTED_PUB_KEY[]                    = "0000000000000000000000000000000000000000000000000000000011111111";

const std::initializer_list<const char*> SEED_NODES = {
   "136.244.105.109:32000",
   "195.178.167.189:32000",
   "208.167.255.71:32000",
   "superblockchain.con-ip.com:32000",
   
};

struct CheckpointData {
  uint32_t height;
  const char* blockId;
};


#ifdef __GNUC__
__attribute__((unused))
#endif

// You may add here other checkpoints using the following format:
// {<block height>, "<block hash>"},
const std::initializer_list<CheckpointData> CHECKPOINTS = {
 { 3700, "34126fea8a0168a41885358fa1f3d793a3a6d28e650da6cc9bc2de9d6ad5fc24" }, 
 { 9900, "15f3ecbb6324991c653ceed03619a735b40142d1b4777161593510f0f1d1a098" },
 { 22000, "8d444f19cc5cd46738583a001135a72b5824aa380e5006f422108eb50d9b9cca" },
 { 33000, "4e8958bcf1673bc326d6285c5f3df256f2c48c7ff85db7ef13637421a561e70e" },
 { 55000, "3da7429c64b8c0d4b2314ffe1a9df99bbda4a035196037f3f7f4f397ba3bac4c" },
 { 88000, "7edbe42a8a4ea71d8b77cc9dde78c9e6ac15cf6ca461a2e301a8a723de251373" },
 { 100000, "8f492fd39ce91174e65e5890560be8de5bc98b784b5067b3a20bf5a366524f6d" },
 
 //v2 checkponts
 { 114117, "250f9d28333fd3db9d0fad3ea2577c5f9ac3b85d85effcd666db39c2af8f03d8" },
 { 114118, "dc3f4e73ab4ef5b39aa43b55f34cb5597ac60f7d7d0e2978439b78c6db9e8487" },
 { 114119, "15431e8515f1c410af43dd34d60c844328852018fd88df0d96e25379f481731e" },
 { 114120, "2aded551461f15676f1caf24bdb1b464d840928232ff15a06d782ac671840e24" },
 { 114121, "4afd8c187955eca58ea0d208cef705677f976679266d202276cba57708ea07e9" },
 { 114122, "be98eae41b38a0aaaf9a10abff45d545b5e89453fa4cd5f1ac63e6e56730ec3d" },
 { 114123, "da1fb6075ee86ddc5bf62c1ba8c3a785b38e70cb34ba63cee4bc3685713feae7" },
 { 114124, "ae23b4e002d6ace6d7f3338ca767a5af959cd317ddaa1af7326b39b1f398d15c" },
 { 114125, "692d44097f1965555da0d6198c79c72bd5ebc8175553531f92ef1287b1d6d7de" },
 { 114126, "01ab8f89ff810af1407ebff6d3951184d84b4ed8fe6881a04ebe48f402fa6ee3" },
 { 114127, "9db4950326d3bfc8d6db40d22017a0afd0ef69044a6ae807b8c3a43c2042d9c0" },
 { 114540, "1efdb6d9265c3138213eb2b733fca9a3d33c474a4b9286b561c96b3017082a32" },
 { 114560, "eab548a8102fa6ef6fe50c7f8ac3954e12db472ca5b1da2722cf4d4a59f5e32f" },
 { 114580, "c970dcf9a4a5c78642c7aa7b7daff0b395d6323557cf54b96190fa7f41a37952" },
 { 114684, "179c80e9ca15b11e59df5970c92a82326618a94f256fc7ad9669899c87bebdb2" },
 { 114685, "289cd8188f58306af953a099d2fadba5647711bbc77102ade4dbc607d973aa09" },
 { 114686, "6a634121bd4c63198d924237501383a51b5491e30fa57a7aa6d41a13512ddc3c" },
 { 114687, "ca74f18e54b1e0c8c623ca4956d90bb4620f03dcc858189161eaf16ce5840bd4" },
 { 114688, "6b7283c86bd5509619dcc63d1f3da1ed884db3fe3fbafaf310407b35507a6ff1" },
 { 114689, "2a270477326bfc0bcd7d394f924a206e9f2e1ecddcaa145b0abf11e147103224" },
 { 114745, "ee42567b0027ed982775ff10cf21f1849d04fed1263c32be9abe8301e1db78fa" },
 { 114750, "dc02f008cf4d16f4a9b6c8246b8f4f28b8ef0ac854d62a685da39507f2c7dc93" },
 { 114760, "94b906bf900980ba05a611410842fa0aaa7673df6f95239de36d3b7f8f8aa82c" },
 { 114770, "39dc9a5466741a8a04782059926b3d9b2c11c784c4f5b8bb90085bb56724e1d4" },
 { 115000, "2e3686d4371be0e80b55f860ab506dc3172117a41a63fd9ad4787153937b0d69" },
 { 115001, "274ab9f4167c8a67808cc2ce0d916db4c7700196ccc404799d41d4545043a9b9" },
 { 153702, "5eb63d86e6afa2910b7b6ebb66e947031979af30b333a87b6737eaff45c0db1a" },
 { 155147, "6fe27d8f59ada032434dfbcf23d8e4769473e94c8fb5e468f5ae428331739c0d" },
 { 158478, "3057adb5cbe3af55a2b78c1b54d19d609157e2e2e8e49bb02e2f44f1a1e020f9" },
 { 162313, "8efa9ad5e1f2bdbbd1aa3ec3f1217781fba960da803bdc024b5f3a39e4bb180a" },
 { 162325, "3e509771b3e0b06e851843b8ff651a790c9f46fc1b6648d53bd73901a1886166" },
 { 162338, "bde957d99a09f70925e02f535a2dfe63274c0c2e18f936bda9aadd9c6b3ba6da" },
 { 162340, "a7726d8f86ff67a8d66f665d65013a04a6e2dc5c650d02b5999842b9ef9e95e1" },
 { 162342, "ccc56b74cc2b55802cba745f809f4b344eb447006d8bef5302bf8597337a0d92" },
 { 162344, "5ca60da58572627913aa7dac32d6985e96b4b431aa4077e3d193a76b03f7a5d7" },
 { 164332, "577cf64d91d3637ce6d11ab115b33214908f5ce579d3c3e05f916184db901b2d" },
 { 164350, "db9043b38796f3f700f54d14c00a37c5fff3702c73402d190cb6a5fc67fd9e18" },
 { 164382, "7eedbf762f926f5db4416683b8e19dc93c69176778077c5706c70fc79a3d3b6e" },
 { 164433, "0d6f935c0b0d1ccfa285f8ba74d572a80f391ff4d94ee84f1e5f83f3495d3112" },
 { 164459, "898caac3efdc7fb053efd35141293b6b7837209f29fddb76050237ee6fbf6b73" },
 { 164465, "0068e371e6c135ec770a9af696b2c51a680108fdcc8cc7abc3eb330d8d82f6cd" },
 { 164503, "de012fe2a7741fec20d77d23cc9a2b2322c9c5ccb96e91bf0734dc3f8fc9d70e" },
 { 164516, "90e8a6e130cca850ffe56f12fcf3c06534be95ba8ad35b2ccc335fbf0b10947c" },
 { 164574, "6a28a1087f742ae623c6d3eb486a42deaed22d5fd59e5e379e2a562ebbea6346" },
 { 164583, "a2d16fd4468600f022635b73953713b9232dd3b8b7ffd04c5951089dd9761af0" },
 { 164589, "ebdbb231d58f651c929019bb2076ebb9491979f2b750a7be8227207c79d1720f" },
 { 164591, "f5d885f962c5d31a02f2ab0bb36a3a469b02b27e1bbaae7a3f132a006cea2797" },
 { 164597, "b532b636ba7935952e494d2ce9b1aec844d2a43bc608ddac9014c419492e0d7b" },
 { 164603, "171dc65beb66d5cfb44b72e48654d902e32de9337bd7c7cd80b57c366dc7e0e8" },
 { 164606, "cdc125ad3dbd4022b0d804b93e5ea73fd518dee55051c66ef15cea2c54d13af0" },
 { 164611, "4a56c8279c75b96e1ec5ae9a54f4e58a191232ec4688dfcd05babb4cd11ed787" },
 { 164620, "3988e9ca481cf907f27d4393a4b3bc4c18a2e64072c1eb0dc7d978edeb8426e3" },
 { 164628, "36c703d24d8980d45f33318d3cb3e6c96ec29f07a92167d5d4814f6d2aa2fb9f" },
 { 164636, "8007dadbf506b5256315163f388034944a1df576cffdf6006f63f159a96d572e" },
 { 164645, "3e641b8b6aa14d091312a7dd6c0628c5c55b7dde7a8c9aac018dae1c598ace42" },
 { 164651, "4edce22652ce2f259ff16a1f43f966543ca51ee8ad9a75981df5a11706a7d653" },
 { 164657, "2c72cd112db13063694665cb7e49a8e9ef92917cd8896872a66d5e7f8dde177a" },
 { 164689, "90f11a993d81036b7fd2c0ed5ea9472c3478d513ce3ea3ded1c87a51b224add7" },
 { 164696, "e9e64101304c3a4f1508816b53abb74a0dea8edcb1cdfb72e287a14519b469d1" },
 { 164709, "a38263a20a34722e60427eb265471fd58672589558803c1b81bee8b400b46b02" },
 { 164731, "cea08184b798e97cf0b2c54655f9703ada2dfb310d635cb4245e88206bb7da0f" },
 { 164823, "a79b8fee3274f62d8e4b9d0ed7734afae6e46ca3cc20dce0f1c22eb8dfcbf2c9" },
 { 164857, "4948f737f7a3e78d51b1821ebafa4f2e233c7a5906fec6835ef92ab4200df250" },
 { 164937, "3999218ac4ed5029714cb9082675807484af0fae39341aad12fb9a1b4222b7b8" },
 { 165021, "c1963173982295af1eb602ce10c4d41c602fb17ab240512011eab5ffd9c59341" },
 { 165097, "40bac34e6395135e511ea0fdd6da38cc88daaefb20499790647a6dde3f5eea24" },
 { 165129, "45e41f13e38e77c0146c30004887b67eb68ef84e8893d6f9b6bef9ee047f32b9" },
 { 165152, "13228022fcd6c9d26818e633c92b0556238c190671692b25a715a6da1a37f323" },
 { 165200, "74b097fec3498e10278d02401ce19a3c9b8db5f566f46baffca998df24c1e5de" },
 { 165234, "45e43d2032f01f762a96280077f9628df9fd3c58706f5bcf439506928e4d4c5b" },
 { 165503, "b299afc90cbddb3f43a57f30c46edbe0dd0838efdd5416308665a27ef04ea6fc" },
 { 165550, "d9f7bb3e185c4a1abb685c8bbcfc5c19eea785af7b70437b71389022d77908db" },
 { 165602, "86f32a1242d29e3563459b1a5c0f74d11b78d01118daf957cf92365c3222c3be" },
 { 165630, "6203616387227572adce8039cbf2828f989677b31523583db0c1c316ade053c9" },
 { 165812, "23872ac3ab84257c7cbe80347cd9d71fd70a78dea04ee0b62d8597786d14e85e" },
 { 165883, "694a43e8df42738ff8d1e5191e0001534fee848118241b2af271bfcf3f61f18e" },
 { 165988, "411835b7e187878a5ea0f8e8d774bc2ca0d6490579427e38b40192409ce86176" },
 { 166337, "ae5672b37bdccedfe1bc156bfeee5e215f855ce0c6b8d38e83ab47a2389f961a" },
 { 166409, "8a942d9093b6ee4a53eb850b0d479b6cd3fdac61fcc7e7cd9d539766f22bc28a" },
 { 166453, "0bd7f7001463e717cab5cbcc8cfc9571df4bcc1cde266399c2acf173183ed7b1" },
 { 166480, "a55649f690390805bf4d565d42a434c70fbec167f81780dba8a0dec660e98ea2" },
 { 166504, "df8c8fe3209c7a4c22978d7cad5a174bf12c7650e722e31eaf402249e472aacb" },
 { 166520, "5527f4517f1c208f57dc93982a92b1a879d55901a69fa3fe5dc4dd8a4f8e8353" },
 { 166597, "dc3d0d910c6665fcb4322a585078d653d24e9c78a6c8a9b7beb6eb83f66af37c" },
 { 166639, "b9dd028eaec19e8a247ddc19ac9443167e089d13b06ae68543f4ca8962f037b6" },
 { 166827, "66efea586b968e9c757a380b3b80800d22ce3cc1bee9f81b29bf632c5e684b04" },
 { 166829, "2b8abb0e9400c13379e32007057aeb53a5edb80a17bb43b924863f815961feec" },
 { 166830, "4c11934a33ed193b8122ace844087f45ae15484f9f0906d588b5227d792e277d" },
 { 166833, "c4f425d4bc160ece278ed53ec7e70cc6705aa35e9c043fbf203df194904526fc" },
 { 166836, "eafcee45ed1802ab1b56b401125352bcc1de759dfb94604317446212a94b1e3e" },
 { 166837, "722b871402cda22350d243e60600ce8e38b6caf8c9037c4a6074fbf5b52ef265" },
 { 166906, "efc2c89012749fc6a5096b1d466d116362924741d215605cd0d568bc197000de" }, 
 { 166927, "a10c020135ae6d8517eba09781392fb724344bfccd2f8153687f9fc82a7b5b5c" },
 { 166980, "6f5c51aaaf55e04b6ec5750fded02f1e50755c5baac3644203256d2aef88d9b0" },
 { 167066, "ab2836e42508157de2e7de435c1718859c486e8974671448265f112e91565e74" },
 { 167120, "90b0621aeb0ee08b76096f94348e384aa283df538b21b289afd1524274729f08" },
 { 167174, "7421184d7c2ada2e7648c53569013927b73b060f461f123eedefd423d0f6dae2" },
 { 167239, "f1a0e3253d3460ff1ccceaf9c64ee52d618fc665a5c6b1c66e366998e5f99577" },
 { 167265, "387aeed9d97adc11565751c45f1e740b604391b0f04c45ef77ecf79c8350006c" }, 
 { 167291, "9f6075c69fe78648c382b052a0ddcbce82d7104e6cf154408bc646572465b103" },
 { 167322, "e40788653d2e7b64ae72f6495ea8f834ef81309e66a9dcdc3f524d40aa6406d9" },
 { 167331, "5fb2259b99987baf41f23b8c7de5461d8b1b4f2cacca01e6d135fe01d8383c8c" },
 { 167335, "ec607f479a9a0db49ee0b07ae8a160a7b52cc0b72a928c7df7b6f108fc172744" },
 { 167339, "3c372dad4ba00541aba74ee862d08c90ebea75d0ee9e4217151a9375467ac818" },
 { 167340, "19e6d417526e07131575c99d0ea5e2249be6dfce1a4ed638332307b41135632d" },
 { 169161, "e6baf62c61fccce552bda8ec5242d9a823f69d34674c3ca274e9903ce2f01e5f" },
 { 169169, "dcf61c0fe7eae008eeff16789919178b39eea95e2f37858b7d3bdacb111e0554" },
 { 169186, "8b6f3f261a1f3dd3605c95972595cb801839e39eded6a626062183d5b95ead51" },
 { 169203, "7857cb9e561819069f8cf8a357fd4affb513697138ba4d10a2d69c6f28fc6e7c" },
 { 169221, "0d9022321698dde5b12065e2457f85132da4d19bc9085adaad5ff3f847b9370b" },
 { 169297, "d4cb2b63e8a844291d72b85ede9244a548fb6202b9596bef315112db0b2c025b" },
 
 //v3 checkponts
  { 533583, "e407f5b859e991c1f5319ff7f7664a517d8c9af6bc8aa5107319ea22bd33ed54" },
  { 533683, "efeb964e107809e0c1ae70b027306a8913f7cecaa74466d90a024b7b211f7a25" },
  { 534772, "162a76cfff56ef776c60a7e36cfeee109366e4dfe118e78f5bc04d1513b8f4fa" },
  { 534802, "46649642b3039dbfd1fd0431dba402fdd8415a3d326b914d8a73cf0c63e6abbc" },
  { 535137, "cf14db70a9e653917fe7eed07fa3241b3053c696bb86f81eb0d5b8f0c9ca6f15" },
  { 535145, "707af5a60ae22d3398e20af25446e30931ff88078d3d358fe3b64ec1587270f1" },
  { 535155, "3111af91e3ba1e9a59df8fcc6fc710978c1f3e18a510d66973f33b9b678b7ebb" },
  { 535165, "d55ed1a6f590a3b5226b458e879b9d3d54369e8dbee6ac9ea666feeaac32f16b" },
};

// {BlockIndex , Version}
const std::map<const uint32_t, const uint8_t> Version = {
    {34147, 1},
    {115000, 2},
    {170000, 3},
    {192000, 4},
};

// {BlockIndex , Version}
const std::map<const uint32_t, const uint8_t> TestNetVersion = {
    {60, 1},
    {120, 2},
    {180, 3},
    {240, 4},
};

// {BlockIndex , Hash}
const std::map<const uint32_t, const char*> VersionCheck = {
    {34147, "cdc7e1b2bb3df0d48d271b7f8429a847ca661a112ce5ed5898b429983e257082"}
};

} // CryptoNote

#define ALLOW_DEBUG_COMMANDS

#endif
