/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */

#include "multy_test/serialized_keys_test_base.h"


#include "multy_core/src/golos/golos_account.h"
#include "multy_core/src/api/key_impl.h"
#include "multy_core/golos.h"

#include "multy_test/supported_blockchains.h"
#include "multy_test/utility.h"

#include "gtest/gtest.h"

namespace
{
using namespace multy_core::internal;
using namespace test_utility;

const SerializedKeyTestCase GOLOS_KEYS[] =
{
    {
        "5JVFFWRLwz6JoP9kguuRFfytToGU6cLgBVTL9t6NB3D3BQLbUBS",
        "GLS58g5rWYS3XFTuGDSxLVwiBiPLoAyCZgn6aB9Ueh8Hj5qwQA3r6",
        ""
    },
    {
        "5KTUAWTc4xCGWRoMmpYBGqRYSm5oXt7u3d8i8XiBXAgo8rDrhnD",
        "GLS5RBP7ZQPkstMRKfEzSCh4naLRwdHssAjiTi2NarT8kbQQChikB",
        ""
    },
    {
        "5JHZUbXzg54nc2LXauHkFDuqJwRNp5ip6JzH2Yk7dThFdYJ8vF4",
        "GLS6GuZcZoZbVjLUgoKdYNKuRUG3wUnfp6pSJErgrG7UMr4ts3fua",
        ""
    },
    {
        "5KCUxfvXwSY5cTT1D9q6akocfQRe4TfTeNHFyC2ht3gua8kK6dG",
        "GLS6tLGuw8q7pg9UoSzTA4nxgtTBrNvghmDYpLPKbV4BKYNbcgsS4",
        ""
    },
    {
        "5KUbqZaSbRRKfnrQLD1QSGwJ147j3t9KrXum5ru22pbV4896DpG",
        "GLS719NwHP3raPpbjn5hPVBCoTUK9XEXc1j5KfcQvXCEEoPEL6xr2",
        ""
    },
    {
        "5JYYBf1woHkvdv8kL7K7i36kVVQTyvNUUD3R5Nuemto2jTZZVu1",
        "GLS7UpGbmzcHhDLaJM7SwHqXSM6ZtTejFz879q6NiviHs8W17D7Rk",
        ""
    },
    {
        "5Ji5kxCztZnrQURdvtXNuZ9fEHfxDHBvYnuqYUHMgX6PBDHkSUN",
        "GLS7di4wQ3MrpgNzXtvaURHsZ8g48h7CJxr3HKHpepugZz88TPVFK",
        ""
    },
    {
        "5K9xmjs8DPUMGMJhrR7V4TuWRtGRjDCLDXD5pSYJ25BfMotcGjn",
        "GLS7kw5KEMgZ9arJjftsspca7Uyu1JH2ULwQNzdM67pRsF3MFFZqS",
        ""
    },
    {
        "5Kh6Upg9HBKQmPpyLjVKbY4JP48NFCbJdk6pE1Z1Wun6S45PEdf",
        "GLS7pXXH8cY56Zu6tutFHepaSa71oMwQQvcYXrobyxMy5u24tTRcv",
        ""
    },
    {
        "5HrWvB9PMC8wwtyzSJS1B15Eye8WsQpdeJGVdGKMV6nu4kARyzS",
        "GLS7umQTC3gqZwSNXCLi8mEY68KHsHRKpfrEDoYX92swm6pqkAYZY",
        ""
    },
    {
        "5KUip2RVuTm1MKdzFwFfkoxe11DJCMr4w4BeAmHiz67HJgrEGbK",
        "GLS7vFsynJfTVs1fVBKWgWmduKnge5uqWcqhynvEHKQ5E2EiWrPhM",
        ""
    },
    {
        "5JWZ166stP4nyKD1fVSjbBc9cNkfZYWv3dbabfHwjeM7mu9fznT",
        "GLS82jz6r5ExfmtxsfWeqbuFYBufkN1qnGY9hc8FQoA3o3U8WGAiA",
        ""
    },
    {
        "5Jxwckq5KEUpL1jamArjg9LcJGz1iStWM5dB49fo4sYzYMuBgrk",
        "GLS8Ari5qhGeXcFiBKHDYHftkLh2eUEQmtgS6ZBZL6aXC1RyGLnFj",
        ""
    },
    {
        "5Hrs3q4q98GXMWAMpRNLbKjNGvGQdXYFSMSMy5Z1rCpG9qTx3Bo",
        "GLS8GQoJDABEDzu1vasHErViLwLL5WPyaSmwpYiTwaT4no6S4EhN6",
        ""
    },
    {
        "5KN93hDdQSsN5Eu4yzXvmPZroM7fJEg9WShxRd7wufKskpgqeg4",
        "GLS8PMBjjSoRgVDXDHRBBV3H2ew661wXp17eyqJr64NQFzvQrvv7t",
        ""
    },
    {
        "5JmysxAzRwdZQxV6MtXehhgbVcHdG6SzTX4Yibt42nCgnHhani8",
        "GLS8YhCkkEZ1MoJDAKQNAFTKdjU66qF8jBTDoqjLyfCC5TvzPxxLK",
        ""
    },
    {
        "5J8JZhGaUifjwHPtyStQEDHDrgkp5nzJNgiWSyJrMVe58AreL28",
        "GLS8j8GKNmhr2ZiQuD5Axn3HFjGceiwDpKF84BNnFmzJJexXXAoud",
        ""
    }
};

INSTANTIATE_TEST_CASE_P(
        Golos,
        SerializedKeyTestP,
        ::testing::Combine(
                ::testing::Values(GOLOS_MAIN_NET),
                ::testing::Values(ACCOUNT_TYPE_DEFAULT),
                ::testing::ValuesIn(GOLOS_KEYS)));

GTEST_TEST(GolosTest, private_key_sign)
{
    AccountPtr account;
    HANDLE_ERROR(make_account(
            GOLOS_MAIN_NET,
            ACCOUNT_TYPE_DEFAULT,
            "5Hq2ZdSGZokcgLoNxNGL5kHKi4e3kUUCqorgFK6T6ka7KtSvYLj",
            reset_sp(account)));

    PrivateKeyPtr prv_key =  account->get_private_key();
    // binary serialize TX to sign golos private key
    BinaryDataPtr signature = prv_key->sign(as_binary_data(from_hex("782a3039b478c839e4cb0c941ff4eaeb7df40bdd68bd441afd444b9da763de1269466c1944189"
            "fb3bb5a0102096d756c7479746573740b70617368616b6c7962696b010000000000000003474f4c4f5300000000")));
    // signature generated form golos-core
    ASSERT_EQ(as_binary_data(from_hex(
            "1f2e6bb028760bacddd31dc9772e63240fd297ee2f9fcd29f3605aeb79f774fa4b7d1b4e6dc4a1cd6fd2d4e08b2ea2758680d6a5b1e49664522f391ff949b70018")),
            *signature);
}

} // namespace
