#ifndef WALLET_CORE_API_H
#define WALLET_CORE_API_H

#ifndef BUILDING_WALLET_CORE
#    define BUILDING_WALLET_CORE 0
#endif

#if defined(_WIN32)
#    if (BUILDING_WALLET_CORE)
#        define WALLET_CORE_API __declspec(dllexport)
#    else
#        define WALLET_CORE_API
#    endif
#elif defined(__GNUC__) && (BUILDING_WALLET_CORE)
#    define WALLET_CORE_API __attribute__ ((visibility ("default")))
#else
#    define WALLET_CORE_API
#endif

#endif // WALLET_CORE_API_H
