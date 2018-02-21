/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_core/src/bitcoin/bitcoin_transaction.h"
#include "multy_core/src/api/properties_impl.h"

#include "multy_core/account.h"
#include "multy_core/big_int.h"
#include "multy_core/properties.h"
#include "multy_core/src/api/account_impl.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/src/exception.h"
#include "multy_core/src/exception_stream.h"
#include "multy_core/src/u_ptr.h"
#include "multy_core/src/utility.h"

#include "multy_test/utility.h"
#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

#include <memory>

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

bool inBetween(const BigInt& left, const BigInt& value, const BigInt& right)
{
    if (left > right)
    {
        THROW_EXCEPTION("Invalid values: left > right")
                << " left: " << left << ", right: " << right;
    }
    return (left <= value) && (value <= right);
}

AccountPtr make_account(
        Currency currency,
        const char* serialized_private_key)
{
    AccountPtr account;
    throw_if_error(
            make_account(currency, serialized_private_key, reset_sp(account)));
    return account;
}

struct TransactionFee
{
    BigInt amount_per_byte;
};

struct TransactionSource
{
    BigInt available;
    bytes prev_tx_hash;
    size_t prev_tx_index;
    bytes prev_tx_scrip_pubkey;
    PrivateKey* private_key;
};

struct TransactionDestination
{
    std::string address;
    BigInt amount;
    bool is_change = false;

    TransactionDestination(
            std::string address,
            BigInt amount)
        : TransactionDestination(std::move(address), std::move(amount), false)
    {}

protected:
    TransactionDestination(
            std::string address,
            BigInt amount,
            bool is_change)
        : address(std::move(address)),
          amount(std::move(amount)),
          is_change(is_change)
    {}
};

struct TransactionChangeDestination : public TransactionDestination
{
    TransactionChangeDestination(const std::string& address)
        : TransactionDestination(address, BigInt{}, true)
    {}
};

struct TransactionTemplate
{
    Account* const account;
    TransactionFee fee;
    std::vector<TransactionSource> sources;
    std::vector<TransactionDestination> destinations;
};

// Make Transaction from template using non-public API, note that returned value must not outlive the argument.
TransactionPtr make_transaction_from_template(const TransactionTemplate& tx)
{
    TransactionPtr transaction;
    throw_if_error(make_transaction(tx.account, reset_sp(transaction)));

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", tx.fee.amount_per_byte);
    }

    for (const TransactionDestination& dest : tx.destinations)
    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value("amount", dest.amount);
        destination.set_property_value("address", dest.address);
        destination.set_property_value("is_change", static_cast<int32_t>(dest.is_change));
    }

    for (const TransactionSource& src : tx.sources)
    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", src.available);
        source.set_property_value("prev_tx_hash",
                as_binary_data(src.prev_tx_hash));
        source.set_property_value("prev_tx_out_index", src.prev_tx_index);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(src.prev_tx_scrip_pubkey));
        source.set_property_value("private_key", *src.private_key);
    }
    return transaction;
}

} // namespace

GTEST_TEST(BitcoinTransactionTest, create_raw_transaction_public_api)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            CURRENCY_BITCOIN,
            "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
            reset_sp(account)));
    ASSERT_NE(nullptr, account);

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    {
        Properties* source = nullptr;
        HANDLE_ERROR(transaction_add_source(transaction.get(), &source));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("2000500", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(source, "amount", amount.get()));

        const bytes prev_tx_hash_bytes = from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391");
        const BinaryData prev_tx_hash = as_binary_data(prev_tx_hash_bytes);
        HANDLE_ERROR(properties_set_binary_data_value(source, "prev_tx_hash", &prev_tx_hash));

        HANDLE_ERROR(properties_set_int32_value(source, "prev_tx_out_index", 0));

        const bytes prev_tx_out_pubkey_script_bytes = from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac");
        const BinaryData prev_tx_out_pubkey_script = as_binary_data(prev_tx_out_pubkey_script_bytes);
        HANDLE_ERROR(properties_set_binary_data_value(source, "prev_tx_out_script_pubkey", &prev_tx_out_pubkey_script));

        PrivateKey* private_key_ptr;
        HANDLE_ERROR(account_get_key(account.get(), KEY_TYPE_PRIVATE, reinterpret_cast<Key**>(&private_key_ptr)));
        PrivateKeyPtr private_key(private_key_ptr);

        HANDLE_ERROR(properties_set_private_key_value(source, "private_key", private_key.get()));
    }

    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("1000000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(destination, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(destination, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }

    {
        // change:
        Properties* change = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &change));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("1000000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(change, "amount", amount.get()));
        HANDLE_ERROR(properties_set_string_value(change, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }

    {
        Properties* fee = nullptr;
        HANDLE_ERROR(transaction_get_fee(transaction.get(), &fee));

        BigIntPtr amount;
        HANDLE_ERROR(make_big_int("10000", reset_sp(amount)));
        HANDLE_ERROR(properties_set_big_int_value(fee, "amount_per_byte", amount.get()));
    }

    BinaryDataPtr serialized;
    HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized)));
    ASSERT_NE(nullptr, serialized);
    EXPECT_NE(0, serialized->len);
    EXPECT_NE(nullptr, serialized->data);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available(BigInt(1000) * 1000 * 1000 * 1000 * 1000);
    const BigInt out_1(BigInt(500) * 1000 * 1000 * 1000 * 1000);
    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key",
                *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property_value("amount", out_1);
    }

    {
        Properties& change = transaction->add_destination();
        change.set_property_value(
                "address", "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM");
        change.set_property_value("amount", out_1);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", BigInt(1000));
        // allowing zero-fee transaction.
        fee.set_property_value("min_amount_per_byte", BigInt(0));
    }

    BinaryDataPtr serialized = transaction->serialize();
    ASSERT_NE(nullptr, serialized);
    EXPECT_NE(0, serialized->len);
    EXPECT_NE(nullptr, serialized->data);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_explicit_change)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available(BigInt(1000) * 1000 * 1000 * 1000 * 1000);
    const BigInt out_1(BigInt(500) * 1000 * 1000 * 1000 * 1000);
    const BigInt fee_per_byte(BigInt(1000));
    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key",
                *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property_value("amount", out_1);
    }

    Properties& change = transaction->add_destination();
    change.set_property_value("address", "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM");
    change.set_property_value("is_change", 1);
    EXPECT_THROW(change.set_property_value("amount", BigInt(1)), Exception);

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", fee_per_byte);
    }

    BinaryDataPtr serialized = transaction->serialize();
    ASSERT_NE(nullptr, serialized);
    EXPECT_NE(0, serialized->len);
    EXPECT_NE(nullptr, serialized->data);

    // NOTE: this would not work for SegWit transactions.
    const BigInt expected_total_fee = static_cast<uint64_t>(serialized->len) * fee_per_byte;

    const double delta_factor = 0.00001;

    // check that actual fee is within delta of value set by user.
    const BigInt delta(static_cast<uint64_t>(expected_total_fee.get<uint64_t>() * delta_factor));
    EXPECT_PRED3(inBetween,
            expected_total_fee - delta,
            transaction->get_total_fee(),
            expected_total_fee + delta);

    BigInt change_amount;
    change.get_property_value("amount", &change_amount);

    ASSERT_LT(0, change_amount);
    EXPECT_PRED3(inBetween,
            available - out_1 - static_cast<uint64_t>(expected_total_fee.get<uint64_t>() * (1 + delta_factor)),
            change_amount,
            available - out_1 - static_cast<uint64_t>(expected_total_fee.get<uint64_t>() * (1 - delta_factor)));

    ASSERT_GE(available - out_1 - expected_total_fee, change_amount);
}

GTEST_TEST(BitcoinTransactionTest, Unprofitable_change)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available("10000");
    const BigInt out_without_change("9559");
    const BigInt out_with_change("9000");
    const BigInt fee_per_byte_one("1");
    const BigInt fee_per_byte_two("2");
    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
        source.set_property_value("prev_tx_hash",
                as_binary_data(from_hex("3696c42469785af61f38a40677d6175d83d7aa987b88aeb0bf896a47873626f0")));
        source.set_property_value("prev_tx_out_index", 1u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key",
                *account->get_private_key());
    }

    Properties& Recipient = transaction->add_destination();
    Recipient.set_property_value("address",
                                   "mpJDSHJcytfxp9asgo2pqihabHmmJkqJuM");
    Recipient.set_property_value("amount", out_without_change);

    Properties& change = transaction->add_destination();
    change.set_property_value("address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
    change.set_property_value("is_change", 1);

    {
        Properties& fee = transaction->get_fee();
        EXPECT_THROW(fee.set_property_value("amount_per_byte", fee_per_byte_one), Exception);
        fee.set_property_value("amount_per_byte", fee_per_byte_two);
    }

    BinaryDataPtr serialized_without_change = transaction->serialize();
    ASSERT_NE(nullptr, serialized_without_change);
    EXPECT_NE(0, serialized_without_change->len);
    EXPECT_NE(nullptr, serialized_without_change->data);

    BigInt change_amount;
    change.get_property_value("amount", &change_amount);
    ASSERT_EQ("0", change_amount);

    //NOTE: if change address amount == 0 serialize without change address
    //      TXid: 667b0fb64e1afd75efc8d32508904bbf781335b6e30f84a07cef756bf4260ba9
    ASSERT_EQ(as_binary_data(from_hex(
            "0100000001f0263687476a89bfb0ae887b98aad7835d17d67706a4381ff65a786924c49636010000006b4830450221008990bfa3875ebd4d270d91ce70aa45be56e037adf038389516dd071651a0eefd022022423e9b93893f0d0c1471725aa90e16d7c845e48cc9b8043d3b86ae4c361ca1012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff0157250000000000001976a91460505d4554b5f7b939142cf1efa566d95a31268788ac00000000")),
            *serialized_without_change);
    const BigInt fee_without_change = transaction->get_total_fee();

    // Change destination amount for core lib create Tx with change destination
    Recipient.set_property_value("amount", out_with_change);
    BinaryDataPtr serialized_with_change = transaction->serialize();
    ASSERT_GT(serialized_with_change->len, serialized_without_change->len);

    change.get_property_value("amount", &change_amount);
    ASSERT_NE("0", change_amount);

//    TODO: remove comment in task MUL-911 where value fee will more accurate
//    const BigInt fee_with_change = transaction->get_total_fee();
//   std::cerr << "fee difference: " << (fee_without_change-fee_with_change).get_value() << std::endl;
//    ASSERT_GT(fee_without_change, fee_with_change);
    // TODO: Add serrialize check. Add checking amount per byte > 1 satoshi
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet2)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available(BigInt(1000) * 1000 * 1000 * 1000 * 1000);
    const BigInt dest_amount(BigInt(129) * 1000 * 1000);
    const BigInt fee_value(BigInt(1) * 1000 * 1000);

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(
                        from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key", *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property_value("amount", dest_amount);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", fee_value);
    }

    const BinaryDataPtr serialied = transaction->serialize();
    // TODO: should re-serializing (and re-signing) produce same result ?
    const BinaryDataPtr serialied2 = transaction->serialize();

    ASSERT_EQ(*serialied, *serialied2);
    ASSERT_EQ(as_binary_data(from_hex(
            "010000000191a34273f5cb57244de3d7b27678b3ad385ac46c7df2c440f3f7b5ad23929748000000006a473044022064d09103c9d48c8b094db03227621ced41732a74963578d3495bac4f7f65b40e02201f2f7adf872c1de2af5027edefdf29379faf9fe8f5751015c974e064a9d9d6e0012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff014062b007000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet2_with_key_to_source)
{
    AccountPtr account;
    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available(BigInt(1000) * 1000 * 1000 * 1000 * 1000);
    const BigInt dest_amount(BigInt(129) * 1000 * 1000);
    const BigInt fee_per_byte(BigInt(1) * 1000 * 1000);

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(
                        from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391")));
        source.set_property_value("prev_tx_out_index", 0u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key", *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property_value("amount", dest_amount);
    }

    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", fee_per_byte);
    }

    const BinaryDataPtr serialied = transaction->serialize();
    {
        //Multy-core don't return excess fee to change addres
        //This test check what estimated fee falls into between +25% -5% real size
        BigInt estimated_fee = transaction->estimate_total_fee(1,1);

        // if we have uncompressed format public key, mistake about +25%
        uint64_t max_total_fee = serialied->len * fee_per_byte.get_value_as_uint64() * 1.25;
        uint64_t min_total_fee = serialied->len * fee_per_byte.get_value_as_uint64() * 0.95;
        EXPECT_LE(min_total_fee, estimated_fee.get_value_as_uint64());
        EXPECT_GE(max_total_fee, estimated_fee.get_value_as_uint64());
    }

    ASSERT_EQ(as_binary_data(from_hex(
            "010000000191a34273f5cb57244de3d7b27678b3ad385ac46c7df2c440f3f7b5ad23929748000000006a473044022064d09103c9d48c8b094db03227621ced41732a74963578d3495bac4f7f65b40e02201f2f7adf872c1de2af5027edefdf29379faf9fe8f5751015c974e064a9d9d6e0012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff014062b007000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_testnet3)
{
    AccountPtr account;

    HANDLE_ERROR(make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available(BigInt(12) * 1000 * 1000 * 100);
    const BigInt dest_amount(BigInt(100000000));
    const BigInt change_value(BigInt(999000000));
    const BigInt fee_value(BigInt(1) * 1000 * 1000);

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(
                        from_hex("13ae654ae5609bd74ee1840fb5e4694580659e4cfe477b303e68162f20a81cda")));
        source.set_property_value("prev_tx_out_index", 1u);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));

        source.set_property_value("private_key",
                *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A");
        destination.set_property_value("amount", dest_amount);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mk6a6qeXNXuQDpA4DPxuouTJJTeFYJAkep");
        destination.set_property_value("amount", dest_amount);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property_value("amount", change_value);
    }
    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", fee_value);
    }

    const BinaryDataPtr serialied = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex(
            "0100000001da1ca8202f16683e307b47fe4c9e65804569e4b50f84e14ed79b60e54a65ae13010000006b483045022100e217cfb5920878da55069a919029ab910ff106cfb20fd901e82de041b149d71902202756c5700377294837893cca854e60b6cca86423f4407b8faf92ff898aded00a012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff0300e1f505000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac00e1f505000000001976a914323c1ea8756feaaaa85d0d0e51b0cc07b4c7ac5e88acc0878b3b000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_with_many_input_from_one_addreses_testnet)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
                    CURRENCY_BITCOIN,
                    "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU", account->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available1("229999999");
    const BigInt available2("459999999");
    const BigInt dest_amount("100000000");
    const BigInt change_value("588999998");
    const BigInt fee_value("1000000");

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available1);
        source.set_property_value("prev_tx_hash",
                as_binary_data(
                        from_hex("4c0a9df13d1d85d20bfc5bb5d38937290d273b7655ff3d50d43db81900546f8a")));
        source.set_property_value("prev_tx_out_index", 0);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key", *account->get_private_key());
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available2);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(
                        from_hex("c51b8890ad84fab4577785908d12b6f8195c69efe5a348fc7d6d88fc1ce97d17")));
        source.set_property_value("prev_tx_out_index", 0);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac")));
        source.set_property_value("private_key", *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A");
        destination.set_property_value("amount", dest_amount);
    }

    {
        Properties& change = transaction->add_destination();
        change.set_property_value(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        change.set_property_value("amount", change_value);
    }
    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", fee_value);
    }

    const BinaryDataPtr serialied = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex("01000000028a6f540019b83dd4503dff55763b270d293789d3b55bfc0bd2851d3df19d0a4c000000006a4730440220661ae5dd08bb4576a04c76114e980d9431c0d2f477dd8c71acfb7f77c8dd1670022022037a1939ae556881a93f41efcf7c3da756b7141b7245971c7337e4a859623d012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff177de91cfc886d7dfc48a3e5ef695c19f8b6128d90857757b4fa84ad90881bc5000000006a4730440220757edec6ee1fbc52c9046dc80c618001a9a7d4162f0a3abf81f27f09005f77e70220042c890f4702dda612883ceb25db4590c1f8459b0bdd88cd9858d4b7565e997b012102163387c2c86f897b8aef15ee24e1f135da70c52e7dde12c06e122891c704d694ffffffff0200e1f505000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac3e6d1b23000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, SmokeTest_with_many_input_from_different_addreses_testnet)
{
    AccountPtr account1;
    AccountPtr account;

    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cScuLx5taDyuAfCnin5WWZz65yGCHMuuaFv6mgearmqAHC4p53sz",
                    reset_sp(account)));
    ASSERT_NE(nullptr, account);
    EXPECT_EQ("mfgq7S1Va1GREFgN66MVoxX35X6juKov6A", account->get_address());

    HANDLE_ERROR(
            make_account(
                    CURRENCY_BITCOIN,
                    "cVbMJKcfEGi4wgsN39rMPkYVAaLeRaPPbrPpJfcH9B9dZCPbS7kT",
                    reset_sp(account1)));
    ASSERT_NE(nullptr, account1);
    EXPECT_EQ("mk6a6qeXNXuQDpA4DPxuouTJJTeFYJAkep", account1->get_address());

    TransactionPtr transaction;
    HANDLE_ERROR(make_transaction(account.get(), reset_sp(transaction)));
    ASSERT_NE(nullptr, transaction);

    const BigInt available1(BigInt(1) * 1000 * 1000 * 100);
    const BigInt available2(BigInt(1) * 1000 * 1000 * 100);
    const BigInt dest_amount(BigInt("190000")*1000);
    const BigInt change_value(BigInt("9000")*1000);
    const BigInt fee_value(BigInt(1) * 1000 * 1000);

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available1);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(
                        from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948")));
        source.set_property_value("prev_tx_out_index", 1);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a914323c1ea8756feaaaa85d0d0e51b0cc07b4c7ac5e88ac")));
        source.set_property_value("private_key",
                *account1->get_private_key());
    }

    {
        Properties& source = transaction->add_source();
        source.set_property_value("amount", available2);
        source.set_property_value(
                "prev_tx_hash",
                as_binary_data(
                        from_hex("a1fdb0d8776cfd43b66cfc0ee49cad2763fdbeca67af8ef40479624716ea8948")));
        source.set_property_value("prev_tx_out_index", 0);
        source.set_property_value("prev_tx_out_script_pubkey",
                as_binary_data(from_hex("76a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac")));
        source.set_property_value("private_key",
                *account->get_private_key());
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU");
        destination.set_property_value("amount", dest_amount);
    }

    {
        Properties& destination = transaction->add_destination();
        destination.set_property_value(
                "address", "mfgq7S1Va1GREFgN66MVoxX35X6juKov6A");
        destination.set_property_value("amount", change_value);
    }
    {
        Properties& fee = transaction->get_fee();
        fee.set_property_value("amount_per_byte", fee_value);
    }
    const BinaryDataPtr serialied = transaction->serialize();
    ASSERT_EQ(as_binary_data(from_hex("01000000024889ea1647627904f48eaf67cabefd6327ad9ce40efc6cb643fd6c77d8b0fda1010000006a47304402200efd6929fcf32210e32194fc8468354deaf67060466710441075dab31afa31b30220350c72e95803ad14ce3fe3baa73e0a2288bf46df44e8c3d686e9692e7689cb7301210217fc7a7cc7f8b41b8e886703b95f087cd6e82ccbe6ee2ff27101b6d69ca2e868ffffffff4889ea1647627904f48eaf67cabefd6327ad9ce40efc6cb643fd6c77d8b0fda1000000006a473044022063a2925d2693033aa9735f412258c93f80f9bf980c688fbe5634b7fd6af958f40220506064007962d15ed0473ec617f1c38c80bd82af864050bf5e406ed4cf2951cf012102a6492c6dd74e49c4b7a4bd507baac3abf25fb26b97e362c3c0cb28b91a043da2ffffffff02802b530b000000001976a914d3f68b887224cabcc90a9581c7bbdace878666db88ac40548900000000001976a91401de29d6f0aaf3467da7881a981c5c5ef90258bd88ac00000000")),
            *serialied);
}

GTEST_TEST(BitcoinTransactionTest, transaction_update)
{
    // Verify that transaction_update() modifies TX internal state.
    const AccountPtr account = make_account(CURRENCY_BITCOIN, "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");
    const PrivateKeyPtr private_key = account->get_private_key();

    const TransactionTemplate TEST_TX
    {
        account.get(),
        TransactionFee
        { // fee:
            BigInt{100}
        },
        { // Sources
            {
                BigInt{2000500},
                from_hex("48979223adb5f7f340c4f27d6cc45a38adb37876b2d7e34d2457cbf57342a391"),
                0,
                from_hex("76a914d3f68b887224cabcc90a9581c7bbdace878666db88ac"),
                private_key.get()
            }
        },
        { // Destinations
            TransactionDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU",
                BigInt{1000000}
            },
            TransactionChangeDestination
            {
                "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"
            }
        }
    };
    TransactionPtr transaction = make_transaction_from_template(TEST_TX);

    // Single destination spends only portion of data, and the change is not updated
    // untill you invoke transaction_update(), hence the fee is really high.
    BigIntPtr total_fee;
    HANDLE_ERROR(transaction_get_total_fee(transaction.get(), reset_sp(total_fee)));
    EXPECT_NE("0", *total_fee);

    // Change is going to be updated and next call to transaction_get_total_fee()
    // should return fee that is much smaller.
    HANDLE_ERROR(transaction_update(transaction.get()));

    BigIntPtr updated_total_fee;
    HANDLE_ERROR(transaction_get_total_fee(transaction.get(), reset_sp(updated_total_fee)));
    EXPECT_NE("0", *updated_total_fee);
    
    // Verifying that excess value was moved to the change address and fee is minimized.
    EXPECT_LT(*updated_total_fee, *total_fee);

    BinaryDataPtr serialized;
    HANDLE_ERROR(transaction_serialize(transaction.get(), reset_sp(serialized)));

    // Verifying that fee is within some bounds of user set value.
    const double delta = 0.0001;
    EXPECT_PRED3(inBetween,
            TEST_TX.fee.amount_per_byte * static_cast<int64_t>(serialized->len * (1 - delta)),
            *updated_total_fee,
            TEST_TX.fee.amount_per_byte * static_cast<int64_t>(serialized->len * (1 + delta))
    );
}

GTEST_TEST(BitcoinTransactionTest, destination_address_verification)
{
    const AccountPtr account = make_account(CURRENCY_BITCOIN, "cQeGKosJjWPn9GkB7QmvmotmBbVg1hm8UjdN6yLXEWZ5HAcRwam7");

    TransactionPtr transaction;
    make_transaction(account.get(), reset_sp(transaction));
    {
        Properties* destination = nullptr;
        HANDLE_ERROR(transaction_add_destination(transaction.get(), &destination));

        EXPECT_ERROR(properties_set_string_value(destination, "address", ""));
        EXPECT_ERROR(properties_set_string_value(destination, "address", " "));
        EXPECT_ERROR(properties_set_string_value(destination, "address", "123"));
        // valid address mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU
        EXPECT_ERROR(properties_set_string_value(destination, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5D"));

        HANDLE_ERROR(properties_set_string_value(destination, "address", "mzqiDnETWkunRDZxjUQ34JzN1LDevh5DpU"));
    }
}
