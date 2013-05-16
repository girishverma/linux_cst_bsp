/*
 *  CST Custom Board.
 *  Author : Vivek Sharma
 *  Mail : vivek@circuitsutra.com
 *
 */
#include "core.h"


// MEMORY MAP : VIC, UART, RTC, SP804.
static struct map_desc cst_cb_io_desc[] __initdata = {
	{
		.virtual	=   IO_ADDRESS (CST_CB_VIC_BASE),
		.pfn		= __phys_to_pfn(CST_CB_VIC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	=   IO_ADDRESS (CST_CB_UART_0_BASE),
		.pfn		= __phys_to_pfn(CST_CB_UART_0_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	=   IO_ADDRESS (CST_CB_RTC_BASE),
		.pfn		= __phys_to_pfn(CST_CB_RTC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	=   IO_ADDRESS (CST_CB_SP804_0_BASE),
		.pfn		= __phys_to_pfn(CST_CB_SP804_0_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
};
void __init cst_cb_map_io(void) {
    iotable_init(cst_cb_io_desc, ARRAY_SIZE(cst_cb_io_desc));
}


// Device registration on amba bus.
static AMBA_APB_DEVICE(rtc,
                       "dev:e0",
                       0,
                       CST_CB_RTC_BASE,
                       { },
                       NULL);
static AMBA_APB_DEVICE(uart0,
                       "dev:f1",
                       0,
                       CST_CB_UART_0_BASE,
                       {INTR_UART_0},
                       NULL);
static struct amba_device *amba_devices[] __initdata = {
	&uart0_device,
	&rtc_device,
};
void __init cst_cb_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(amba_devices); i++) {
		struct amba_device *d = amba_devices[i];
		amba_device_register(d, &iomem_resource);
	}
}


// INTERRUPT HANDLER INSTANTIATION.
static const struct of_device_id vic_of_match[] __initconst = {
	{ .compatible = "arm,pl192-vic", },
	{}
};
void __init cst_cb_init_irq(void) {
	struct device_node *np;
	np = of_find_matching_node_by_address(NULL, vic_of_match, CST_CB_VIC_BASE);
	__vic_init(VA_VIC_BASE, IRQ_START, VALID_SOURCES, 0, np);
}


// Clock declaration.
static struct clk dummy_apb_pclk;
static struct clk sp804_clk = {
	.rate	= ONE_MHZ,
};
static struct clk_lookup lookups[] = {
	{	/* AMBA bus clock */
		.con_id		= "apb_pclk",
		.clk		= &dummy_apb_pclk,
	}, {	/* UART_0 clock*/
		.dev_id		= "dev:f1",
		.clk		= &sp804_clk,
	}, {	/* SP804 timers clock */
		.dev_id		= "sp804",
		.clk		= &sp804_clk,
	}
};


//LINUX Scheduler clock init using RTC-counter <without IRQ>.
int clk_enable(struct clk *clk)
{
	return 0;
}

void clk_disable(struct clk *clk)
{
}

unsigned long clk_get_rate(struct clk *clk)
{
	return clk->rate;
}

static u32 notrace read_sched_clock(void) {
	if (ctr)
		return readl(ctr);

	return 0;
}

void __init cst_cb_init_early(void) {
	void __iomem *rtc_counter = __io_address(CST_CB_RTC_BASE);

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

    // Enable RTC.
	writel(1, VA_RTC_BASE + 0xC);

    //Setup basic board clock.
    ctr = rtc_counter;
    setup_sched_clock(read_sched_clock, 32, ONE_HZ);
}


// Linux EVENT-fd & Timer-fd Counter init using sp804 <with IRQ>.
static void __init cst_cb_timer_init(void)
{
    // Turn OFF timers initially.
    writel(0, VA_TIMER_0_BASE + 0x8);

    // Free Running Timer as Clock Source and Scheduler Source.
	sp804_clockevents_init(VA_TIMER_0_BASE, INTR_TIMER_0, "timer0");
}
struct sys_timer cst_cb_timer = {
	.init		= cst_cb_timer_init,
};



// MACHINE CALLBACKS.
MACHINE_START(CST_CB, "CircuitSutra Custom_Board")
	.atag_offset	= 0x100,
	.map_io		    = cst_cb_map_io,
	.init_early	    = cst_cb_init_early,
	.init_irq	    = cst_cb_init_irq,
	.handle_irq	    = vic_handle_irq,
	.timer		    = &cst_cb_timer,
	.init_machine	= cst_cb_init,
	.restart	    = NULL,
MACHINE_END
