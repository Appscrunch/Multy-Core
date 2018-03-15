/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BLOCKCHAIN_H
#define MULTY_CORE_BLOCKCHAIN_H

#include "multy_core/api.h"

#include <stddef.h>

struct Error;

#ifdef __cplusplus
extern "C" {
#endif

// See: https://github.com/satoshilabs/slips/blob/master/slip-0044.md
enum Blockchain
{
    BLOCKCHAIN_BITCOIN = 0x00,
    BLOCKCHAIN_ETHEREUM = 0x3c,
    BLOCKCHAIN_GOLOS = 0x060105
};

enum BlockchainNetType
{
    BLOCKCHAIN_NET_TYPE_MAINNET = 0,
    BLOCKCHAIN_NET_TYPE_TESTNET = 1,
};

struct BlockchainType
{
    enum Blockchain blockchain;
    size_t net_type; // blockchain-specific net type, 0 for MAINNET.
};

/** Validate an address for given blockchain
 *  @param address - address
 *  @param blockchain_type - Blockchain to use address for.
 *  @return null ptr if address is valid, Error if it is not.
 */
MULTY_CORE_API struct Error* validate_address(
        struct BlockchainType blockchain,
        const char* address);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MULTY_CORE_BLOCKCHAIN_H