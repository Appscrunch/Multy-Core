/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#ifndef MULTY_CORE_BITCOIN_TRANSACTION_H
#define MULTY_CORE_BITCOIN_TRANSACTION_H

#include "multy_core/src/transaction_base.h"

#include <memory>
#include <vector>

namespace multy_core
{
namespace internal
{
enum DestinationsToUse
{
    WITHOUT_ZERO_CHANGE,
    WITH_ZERO_CHANGE
};

class BitcoinAccount;
class BitcoinTransactionDestination;
class BitcoinTransactionFee;
class BitcoinTransactionSource;
class BitcoinStream;

typedef std::unique_ptr<BitcoinTransactionDestination> BitcoinTransactionDestinationPtr;
typedef std::unique_ptr<BitcoinTransactionFee> BitcoinTransactionFeePtr;
typedef std::unique_ptr<BitcoinTransactionSource> BitcoinTransactionSourcePtr;

class BitcoinTransaction : public TransactionBase
{
public:
    BitcoinTransaction();

    // Transaction
    BigInt get_total_fee() const override;
    BigInt estimate_total_fee(size_t sources_count, size_t destinations_count) const override;
    BinaryDataPtr serialize() override;
    Properties& add_source() override;
    Properties& add_destination() override;
    Properties& get_fee() override;

private:
    BigInt calculate_diff() const;
    void verify() const;
    void update_state();
    void sign();

    template <typename T>
    void serialize_to_stream(T* stream, DestinationsToUse destinations_to_use) const;
    bool is_segwit() const;

    typedef std::vector<const BitcoinTransactionDestination*> Destinations;
    Destinations get_non_zero_destinations(DestinationsToUse destinations_to_use) const;

    size_t estimate_transaction_size() const;

private:
    int32_t m_version;
    int32_t m_is_segwit_transaction;
    uint32_t m_lock_time;

    BitcoinTransactionFeePtr m_fee;
    std::vector<BitcoinTransactionSourcePtr> m_sources;
    std::vector<BitcoinTransactionDestinationPtr> m_destinations;
};

} // namespace internal
} // namespaec multy_core

#endif // MULTY_CORE_BITCOIN_TRANSACTION_H