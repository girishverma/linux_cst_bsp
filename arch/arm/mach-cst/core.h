/*
 *  CST Custom Board.
 *  Author : Vivek Sharma
 *  Mail : vivek@circuitsutra.com
 *
 */

#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/amba/bus.h>
#include <linux/clkdev.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/irq.h>
#include <linux/irqchip/arm-vic.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/timer-sp.h>

#include <asm/sched_clock.h>

#include <mach/platform.h>
#include <mach/hardware.h>
