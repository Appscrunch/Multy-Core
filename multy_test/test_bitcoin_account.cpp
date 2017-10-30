#include "multy_core/account.h"
#include "multy_core/keys.h"

#include "multy_core/internal/u_ptr.h"
#include "multy_core/internal/utility.h"

#include "multy_test/value_printers.h"

#include "gtest/gtest.h"

namespace
{
using namespace wallet_core::internal;

struct BitcoinAccountTestCase
{
    const char* private_key;
    const char* public_key;
    const char* address;
};

void PrintTo(const BitcoinAccountTestCase& c, std::ostream* out)
{
    *out << "BitcoinAccountTestCase {\n"
         << "\tprivate key: " << c.private_key << ",\n"
         << "\tpublic key: " << c.public_key << ",\n"
         << "\taddress: " << c.address << "\n"
         << "}";
}

BitcoinAccountTestCase TEST_CASES[] = {
        {
            "L33peSRvmY3MyRMdVqMKx72h4v8wugNhMwLCyzfD8xS1eZTRvkW1",
            "mUyLSFTU6XUuNYJjHEoMJiUEsZdva77TYxV8tY4PD2VN",
            "1fjRrB4XXJWeiw1686zCKYGSNjFqLchYQ"
        },
        {
            "L2ZG56cGSPDyvbAJoxUKmhUVRmKpG5bmRoY775ZYNPvvCnx58b9n",
            "oUfskkKeD4ttBMtdMkvZNHdCHjzquRqnFVuXTjkMKfYx",
            "14kHzG9194ojtiXFdbcdTkUCUsFbEfu5MW"
        },
        {
            "KwHeGE1qDjmBe4ecJoZww6WMj7BqnxsjXx4HWPneSRQDY7NBph4z",
            "fA7fqi3yibH7cUwfMH7BBaghhDYmZSLt5BmRUqe4koGH",
            "15XSfkydZEYkWyQGSUp9gGrC1dTi6wM2Ei"
        },
        {
            "L3fQB13UBadjNAGAZpL2us4xw6YWRV1ykgK8akvtQks3wQcgVAjG",
            "2Ahw45DdDSLcsSRbwiajV1QDzDaMfw2XH6Kt91ZRebDFv",
            "15prjQSoMX5cCKhxSAgcdz47bZRN2ge2th"
        },
        {
            "L3cN7tZ3gmnRQtFcqD6R44s6kzUfebUJ8GrmGCtWj65M6C9LX1J4",
            "2BQab9VLwL8HnDwJYvZqP4wntAx2MEVLHfFieTVs384Wu",
            "18NpPnJfTaDyAPaz36m3TUHcT77dGHRnoK"
        },
        {
            "KxgcSQsS3LZBHm1T6K8ijsNyobWEbN3v5QZKDPY4XSYoLCqdeHpr",
            "bjgoXKUmQnTYKxGU7VDC5RZ2WjGebudF7UoFJoY2Hqc8",
            "19cmiDmj4FwJVvcfDZXhC8kYeJLZytisZ8"
        },
        {
            "5KC4ejrDjv152FGwP386VD1i2NYc5KkfSMyv1nGy1VGDxGHqVY3",
            "MxXTacR6Fijwv6uAukGRUbb16J3UBjuxshFcCL8RzY6Cy6hM6b1Xv8kgmK3rnBTyP2"
            "m8yd7NxErcpy6vRKUZ4tRJ",
            "1F5y5E5FMc5YzdJtB9hLaUe43GDxEKXENJ"
        },
        {
            "5HtqcFguVHA22E3bcjJR2p4HHMEGnEXxVL5hnxmPQvRedSQSuT4",
            "PPT6wvt6t9EwpQpfba6LmL3ZoYqVfk5JoEEFB935rBs5t5uX4BjuhqUNB6wNwRzYv6"
            "hUFcWH4rbuwjMUMmRg2YBB",
            "1M9TbQnXzESQZjR1J3pMooQUpHEqUnrKZh"
        },
        {
            "L1gz2qcYqxn3rzMW5hdZZDAR8hB4mke6NnpYvjxGHSfYGJ9bFLJV",
            "ymybJgusbqjGD2oGw224ESjRTK3mJJKR6au6kPhagrkG",
            "1Mh7iAQstKFJDePMJ5MG1C1ztsnmuLV5Y6"
        },
        {
            "L5ABUi5Dup2KUXduFpRTa2jZyaK",
            "jxJGdL3YiAQz3kzXDU1YniQUcSKoBtLd1qZefTRmhUYc",
            "1Mu8765kCAuP5NaoKZUMgBieTT7KqcUBbZ"
        },
        {
            "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj",
            "MhYEtBZVFPa7stZRurPVT8JFGztqZLMj5f9K53BvLhqgZ9CfHsNDNFeiC5hkWb2GZs"
            "QALysHYrVPaURsNfaPER9r",
            "1QFqqMUD55ZV3PJEJZtaKCsQmjLT6JkjvJ"
        }
};

class BitcoinAccountTestP
        : public ::testing::TestWithParam<BitcoinAccountTestCase>
{
};

INSTANTIATE_TEST_CASE_P(
        Generated, BitcoinAccountTestP, ::testing::ValuesIn(TEST_CASES));

} // namespace

TEST_P(BitcoinAccountTestP, private_key)
{
    const BitcoinAccountTestCase& param = GetParam();
    AccountPtr account;
    ErrorPtr error;
    error.reset(
            make_account(
                    CURRENCY_BITCOIN, param.private_key, reset_sp(account)));
    EXPECT_NE(nullptr, error);
    ASSERT_NE(nullptr, account);

    KeyPtr private_key;
    error.reset(
            get_account_key(
                    account.get(), KEY_TYPE_PRIVATE, reset_sp(private_key)));
    EXPECT_NE(nullptr, error);
    ASSERT_NE(nullptr, private_key);

    ConstCharPtr serialized_private_key;
    error.reset(
            key_to_string(private_key.get(), reset_sp(serialized_private_key)));
    EXPECT_NE(nullptr, error);
    ASSERT_NE(nullptr, serialized_private_key);
    ASSERT_STREQ(param.private_key, serialized_private_key.get());
}
