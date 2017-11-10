if(IOS_PLATFORM)
    add_subdirectory(libwally-core)

else(IOS_PLATFORM)
    if(LIBWALLY_BIN)
 #       if (IOS_PLATFORM)
 #           add_library(libwally-core STATIC IMPORTED GLOBAL)
 #       else(IOS_PLATFORM)
 #           add_library(libwally-core SHARED IMPORTED GLOBAL)
 #       endif(IOS_PLATFORM)

        set_target_properties(libwally-core PROPERTIES IMPORTED_LOCATION
            ${LIBWALLY_BIN}
        )
        message("Using prebuild libwally-core : ${LIBWALLY_BIN}")
    elseif(LIBWALLY_BIN_PATH)
        add_library(libwally-core SHARED IMPORTED GLOBAL)
        message("ANDROID_ABI : ${ANDROID_ABI}")
        # TODO: use FIND_LIBRARY
        if (ANDROID_ABI)
            set_target_properties(libwally-core PROPERTIES IMPORTED_LOCATION
                ${LIBWALLY_BIN_PATH}/${ANDROID_ABI}/libwallycore.so)
        else()
            set_target_properties(libwally-core PROPERTIES IMPORTED_LOCATION
                ${LIBWALLY_BIN_PATH}/libwallycore.so)
        endif()
        message("Found prebuild libwally-core at : ${LIBWALLY_BIN_PATH}")
    else()
        message("Building libwally-core from source")
        set(wally_sources
            libwally-core/src/aes.c
            libwally-core/src/base58.c
            libwally-core/src/bip32.c
            libwally-core/src/bip38.c
            libwally-core/src/bip39.c
            libwally-core/src/ccan/ccan/crypto/ripemd160/ripemd160.c
            libwally-core/src/ccan/ccan/crypto/sha256/sha256.c
            libwally-core/src/ccan/ccan/crypto/sha512/sha512.c
            libwally-core/src/ccan/ccan/str/hex/hex.c
            libwally-core/src/elements.c
            libwally-core/src/hex.c
            libwally-core/src/hmac.c
            libwally-core/src/internal.c
            libwally-core/src/mnemonic.c
            libwally-core/src/pbkdf2.c
            libwally-core/src/scrypt.c
            libwally-core/src/sign.c
            libwally-core/src/wordlist.c
        )

        add_library(libwally-core SHARED ${wally_sources})
        target_include_directories(libwally-core PRIVATE
            .
            libwally-core
            libwally-core/src
            libwally-core/src/ccan
            libwally-core/src/secp256k1
            libwally-core/src/secp256k1/include
        )
        target_link_libraries(libwally-core secp256k1)
    endif()

    if(LIBSECP256K1_BIN)
        add_library(secp256k1 SHARED IMPORTED GLOBAL)
        set_target_properties(secp256k1
            PROPERTIES IMPORTED_LOCATION
            ${LIBSECP256K1_BIN})
        message("Using prebuild secp256k1 ${LIBWALLY_BIN}")

    elseif (LIBSECP256K1_BIN_PATH)
        add_library(secp256k1 SHARED IMPORTED GLOBAL)
        if (ANDROID_ABI)
            set_target_properties(secp256k1 PROPERTIES IMPORTED_LOCATION
                ${LIBSECP256K1_BIN_PATH}/${ANDROID_ABI}/secp256k1.so)
        else()
            set_target_properties(secp256k1 PROPERTIES IMPORTED_LOCATION
                ${LIBSECP256K1_BIN_PATH}/secp256k1.so)
        endif()
        message("Using prebuild secp256k1 from ${LIBSECP256K1_BIN_PATH}")

    else()

    message("Building libsecp256k1 from source")
    # secp256k1 cmake-stuff was inspired by
    # https://github.com/chfast/secp256k1/blob/cb5dc20ecc116d27452f8ecb1ae50205834da172/CMakeLists.txt
    add_library(secp256k1 SHARED
        libwally-core/src/secp256k1/contrib/lax_der_parsing.c
        libwally-core/src/secp256k1/contrib/lax_der_privatekey_parsing.c
        libwally-core/src/secp256k1/src/secp256k1.c
    )
    target_include_directories(secp256k1
        PUBLIC
            libwally-core/src/secp256k1
            libwally-core/src/secp256k1/src
            libwally-core/src/secp256k1/include
        PRIVATE
    )
    target_compile_definitions(secp256k1
            PUBLIC
                -DHAVE_CONFIG_H
            PRIVATE
                -DENABLE_MODULE_RANGEPROOF -DENABLE_MODULE_SURJECTIONPROOF -DENABLE_MODULE_ECDH -DENABLE_MODULE_GENERATOR
                -DENABLE_MODULE_RECOVERY #optional
        )

    endif()

endif(IOS_PLATFORM)

add_library(keccak-tiny STATIC
    ../third-party/keccak-tiny/keccak-tiny.c
)

set_target_properties(keccak-tiny PROPERTIES
    C_STANDARD 11
    POSITION_INDEPENDENT_CODE ON
)

if(DEFINED WITH_TESTS)
    set(BUILD_SHARED_LIBS YES)
    add_subdirectory(googletest)
    set_target_properties(gtest PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    set_target_properties(gmock PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif(DEFINED WITH_TESTS)
