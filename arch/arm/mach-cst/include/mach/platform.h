#define CST_CB_VIC_BASE         0x10140000
#define CST_CB_SP804_0_BASE     0x101E2000
#define CST_CB_UART_0_BASE      0x101F1000
#define CST_CB_RTC_BASE         0x101E8000

#define VA_RTC_BASE     __io_address(CST_CB_RTC_BASE)
#define VA_VIC_BASE		__io_address(CST_CB_VIC_BASE)
#define VA_TIMER_0_BASE	__io_address(CST_CB_SP804_0_BASE)
#define VA_TIMER_1_BASE	(__io_address(VERSATILE_TIMER0_1_BASE) + 0x20)

#define ONE_HZ  1
#define ONE_MHZ 1000000
