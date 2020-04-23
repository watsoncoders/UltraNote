Added since 1.0.8:

- Properly address spammy compiler warnings  
- Only build gtest when wanting to run tests  
>external/CMakeLists.txt  
>CMakeLists.txt  
>Makefile  
- Only use `MINIUPNPC_GET_SRC_ADDR` macro  if it is defined. (`#if` changed to `#ifdef`)  
>external/miniupnpc/receivedata.c
- Fix `minerTxValid` warning in Core.
- Silence warnings for implicit fallthrough (Base58.cpp)
- Silence warnings for Unused functions and variables

Added in 1.0.8:

- Tracking (view only) wallets
- Private keys
- Change password option
- Remote daemon option
- Determistic wallets
- Mnemonic seeds
- Import GUI key
- Add searchby payment ID
