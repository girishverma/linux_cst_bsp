/*
 *  CST Custom Board.
 *  Author : Vivek Sharma
 *  Mail : vivek@circuitsutra.com
 *
 */
#include "core.h"

#include <linux/platform_device.h>
#include <linux/irq.h>
// MEMORY MAP : VIC, UART, RTC, SP804, CLCDC.
static struct map_desc cst_cb_io_desc[] __initdata = { {
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
	}, {
		.virtual	=   IO_ADDRESS (CST_CB_CLCDC_BASE),
		.pfn		= __phys_to_pfn(CST_CB_CLCDC_BASE),
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
                       CST_CB_RTC_BASE, { },
                       NULL);
static AMBA_APB_DEVICE(uart0,
                       "dev:f1",
                       0,
                       CST_CB_UART_0_BASE, {INTR_UART_0},
                       NULL);
static AMBA_APB_DEVICE(clcdc,
                       "dev:a0",
                       0,
                       CST_CB_UART_0_BASE, {INTR_CLCDC},
                       NULL);
static struct amba_device *amba_devices[] __initdata = {
	&uart0_device,
	&rtc_device,
	//&clcdc_device,
};

#define XHCI_USB_DEVICE_SUPPORTED      	   256
#define XHCI_HCS_DOORBELL_OFFSET           2048
#define XHCI_HCS_RUN_TIME_REG_OFFSET       1024


#ifdef CONFIG_USB_OTG
#define CST_XHCI_BASE_ADDRESS 0x30000000 + 0x100
#else
#define CST_XHCI_BASE_ADDRESS 0x30000000
#endif

#define CST_XHCI_END_ADDRESS  ((XHCI_USB_DEVICE_SUPPORTED*4) + XHCI_HCS_DOORBELL_OFFSET + CST_XHCI_BASE_ADDRESS)
#define CST_XHCI_SIZE                 (CST_XHCI_END_ADDRESS - CST_XHCI_BASE_ADDRESS)
#define CST_XHCI_IRQ          (32 + 11) //using GIC


#define CST_SMC91C111_BASE_ADDRESS  0x101e9000
#define CST_SMC91C111_END_ADDRESS   0x101e9400
#define CST_SMC91C111_IRQ           (32 +15) // Using GIC

#if 0

#define CST_CLCDC_BASE_ADDRESS  0x20000000
#define CST_CLCDC_END_ADDRESS   0x20000fff
#define CST_CLCDC_IRQ           (32 +16) // Using GIC

#endif

static struct resource cst_xhci_resources[] = {
        [0] = {
                .start  = CST_XHCI_BASE_ADDRESS,  /* Base address of Device */
                .end    =  CST_XHCI_END_ADDRESS,
                .flags  = IORESOURCE_MEM,
        },
        [1] = {
                //.start  =  irq_num, ///< Intrupt line given to USB dev controller  in our model
                .start  = CST_XHCI_IRQ, ///< Interrupt line used by UDC
                .end    = CST_XHCI_IRQ,
                .flags  = IORESOURCE_IRQ,
        },
};


static struct resource smc91c111_resources[] = {
        [0] = {
                .name   = "smc91x-regs",
                .start  = CST_SMC91C111_BASE_ADDRESS,  /* Base address of Device */
                .end    =  CST_SMC91C111_END_ADDRESS,
                .flags  = IORESOURCE_MEM,
        },
        [1] = {
                //.start  =  irq_num, ///< Intrupt line given to USB dev controller  in our model
                .start  = CST_SMC91C111_IRQ, ///< Interrupt line used by UDC
                .end    = CST_SMC91C111_IRQ,
                .flags  = IORESOURCE_IRQ,
        },
};
#if 0
static struct resource cst_clcdc_resources[] = {
        [0] = {
                .start  = CST_CLCDC_BASE_ADDRESS,  /* Base address of Device */
                .end    = CST_CLCDC_END_ADDRESS,
                .flags  = IORESOURCE_MEM,
        },
        [1] = {
                //.start  =  irq_num, ///< Intrupt line given to USB dev controller  in our model
                .start  = CST_CLCDC_IRQ, ///< Interrupt line used by UDC
                .end    = CST_CLCDC_IRQ,
                .flags  = IORESOURCE_IRQ,
        },
};
#endif
static void cst_xhci_release(struct device *dev) {

}

static void smc91c111_release(struct device *dev) {

}
static u64 cst_xhci_dmamask = 0xffffffffUL;
static struct platform_device cst_xhci_pdev = {
        .name = "cst_xhci",
        .id = -1,
        .num_resources = ARRAY_SIZE(cst_xhci_resources),
        .resource = cst_xhci_resources,
        .dev = {
                .release  = cst_xhci_release,
                .dma_mask  = &cst_xhci_dmamask,
                .coherent_dma_mask = 0xffffffff, //All 32 bit range

        },
};

static struct platform_device smc_91c111_pdev = {
        .name = "smc91x",
        .id = -1,
        .num_resources = ARRAY_SIZE(smc91c111_resources),
        .resource = smc91c111_resources,
        .dev = {
                .release  = smc91c111_release,
        },
};
#if 0
static struct platform_device cst_clcdc_pdev = {
        .name = "cst_clcdc",
        .id = -1,
        .num_resources = ARRAY_SIZE(cst_clcdc_resources),
        .resource = cst_clcdc_resources,
        .dev = {
		//[TODO]

        },
};
#endif
void __init cst_cb_init(void) {
	int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(amba_devices); i++) {
		struct amba_device *d = amba_devices[i];
		amba_device_register(d, &iomem_resource);
	}
	if ((platform_device_register(&cst_xhci_pdev)) < 0) {
		printk(" XHCI Platform device register Fail \n");
	}
	if ((platform_device_register(&smc_91c111_pdev)) < 0) {
		printk(" SMC 91C111  Platform device register Fail \n");
	}
}


// INTERRUPT HANDLER INSTANTIATION.
static const struct of_device_id vic_of_match[] __initconst = { { .compatible = "arm,pl192-vic", }, {}
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
static struct clk_lookup lookups[] = { {	/* AMBA bus clock */
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
int clk_enable(struct clk *clk) {
	return 0;
}

void clk_disable(struct clk *clk) {
}

unsigned long clk_get_rate(struct clk *clk) {
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
void __init cst_cb_timer_init(void) {
    // Turn OFF timers initially.
    writel(0, VA_TIMER_0_BASE + 0x8);

    // Free Running Timer as Clock Source and Scheduler Source.
	sp804_clockevents_init(VA_TIMER_0_BASE, INTR_TIMER_0, "timer0");
}
// MACHINE CALLBACKS.
MACHINE_START(CST_CB, "CircuitSutra Custom_Board")
	.atag_offset	= 0x100,
	.map_io		    = cst_cb_map_io,
	.init_early	    = cst_cb_init_early,
	.init_irq	    = cst_cb_init_irq,
	.init_time	    = cst_cb_timer_init,
	.init_machine	= cst_cb_init,
	.restart	    = NULL,
MACHINE_END
