#ifndef _SCOTTZ0R_TESTS_MOCKS_AVR_WDT_INCLUDE_GUARD
#define _SCOTTZ0R_TESTS_MOCKS_AVR_WDT_INCLUDE_GUARD

#define WDTO_15MS   0
#define WDTO_30MS   1
#define WDTO_60MS   2
#define WDTO_120MS  3
#define WDTO_250MS  4
#define WDTO_500MS  5
#define WDTO_1S     6
#define WDTO_2S     7

// TODO: Probably would want these to be functions to test loop/setup for read.
#define wdt_enable(timeout)
#define wdt_disable()
#define wdt_reset()

#endif // _SCOTTZ0R_TESTS_MOCKS_AVR_WDT_INCLUDE_GUARD
