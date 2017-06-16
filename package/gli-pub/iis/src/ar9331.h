/*
 * vim: tabstop=8 : noexpandtab
 */
#ifndef _AR7240_H
#define _AR7240_H

#include <asm/addrspace.h>

#ifdef CONFIG_MACH_HORNET
#include "ar933x.h"
#endif


typedef unsigned int ar9331_reg_t;

#define ar9331_reg_rd(_phys)	(*(volatile ar9331_reg_t *)KSEG1ADDR(_phys))
#define ar9331_reg_wr_nf(_phys, _val) \
	((*(volatile ar9331_reg_t *)KSEG1ADDR(_phys)) = (_val))

#define ar9331_reg_wr(_phys, _val) do {	\
	ar9331_reg_wr_nf(_phys, _val);	\
	ar9331_reg_rd(_phys);		\
} while(0)

#define ar9331_reg_rmw_set(_reg, _mask)	do {				\
	ar9331_reg_wr((_reg), (ar9331_reg_rd((_reg)) | (_mask)));	\
	ar9331_reg_rd((_reg));						\
} while(0)

#define ar9331_reg_rmw_clear(_reg, _mask) do {				\
	ar9331_reg_wr((_reg), (ar9331_reg_rd((_reg)) & ~(_mask)));	\
	ar9331_reg_rd((_reg));						\
} while(0)

/*
 * Address map
 */
#define AR7240_PCI_MEM_BASE		0x10000000	/* 128M */
#define AR7240_APB_BASE			0x18000000	/* 384M */
#define AR7240_GE0_BASE			0x19000000	/* 16M */
#define AR7240_GE1_BASE			0x1a000000	/* 16M */
#define AR7240_USB_OHCI_BASE		0x1b000000
#define AR7240_USB_EHCI_BASE		0x1b000000
#define AR7240_SPI_BASE			0x1f000000

/*
 * Added the PCI LCL RESET register from u-boot
 * ar9331_soc.h so that we can query the PCI LCL RESET
 * register for the presence of WLAN H/W.
 */
#define AR7240_PCI_LCL_BASE		(AR7240_APB_BASE+0x000f0000)
#define AR7240_PCI_LCL_APP		(AR7240_PCI_LCL_BASE+0x00)
#define AR7240_PCI_LCL_RESET		(AR7240_PCI_LCL_BASE+0x18)

/*
 * APB block
 */
#define AR7240_DDR_CTL_BASE		AR7240_APB_BASE+0x00000000
#define AR7240_CPU_BASE			AR7240_APB_BASE+0x00010000
#define AR7240_UART_BASE		AR7240_APB_BASE+0x00020000
#define AR7240_USB_CONFIG_BASE		AR7240_APB_BASE+0x00030000
#define AR7240_GPIO_BASE		AR7240_APB_BASE+0x00040000
#define AR7240_PLL_BASE			AR7240_APB_BASE+0x00050000
#define AR7240_RESET_BASE		AR7240_APB_BASE+0x00060000
#define AR7240_DMA_BASE			AR7240_APB_BASE+0x000A0000
#ifdef CONFIG_WASP_SUPPORT
#define AR7240_SLIC_BASE		AR7240_APB_BASE+0x000A9000
#else
#define AR7240_SLIC_BASE		AR7240_APB_BASE+0x00090000
#endif
#define AR7240_STEREO_BASE		AR7240_APB_BASE+0x000B0000
#define AR7240_PCI_CTLR_BASE		AR7240_APB_BASE+0x000F0000

/*
 * DDR block
 */
#define AR7240_DDR_CONFIG		AR7240_DDR_CTL_BASE+0
#define AR7240_DDR_CONFIG2		AR7240_DDR_CTL_BASE+4
#define AR7240_DDR_MODE			AR7240_DDR_CTL_BASE+0x08
#define AR7240_DDR_EXT_MODE		AR7240_DDR_CTL_BASE+0x0c
#define AR7240_DDR_CONTROL		AR7240_DDR_CTL_BASE+0x10
#define AR7240_DDR_REFRESH		AR7240_DDR_CTL_BASE+0x14
#define AR7240_DDR_RD_DATA_THIS_CYCLE	AR7240_DDR_CTL_BASE+0x18
#define AR7240_DDR_TAP_CONTROL0		AR7240_DDR_CTL_BASE+0x1c
#define AR7240_DDR_TAP_CONTROL1		AR7240_DDR_CTL_BASE+0x20
#define AR7240_DDR_TAP_CONTROL2		AR7240_DDR_CTL_BASE+0x24
#define AR7240_DDR_TAP_CONTROL3		AR7240_DDR_CTL_BASE+0x28

/*
 * DDR Config values
 */
#define AR7240_DDR_CONFIG_16BIT		(1 << 31)
#define AR7240_DDR_CONFIG_PAGE_OPEN	(1 << 30)
#define AR7240_DDR_CONFIG_CAS_LAT_SHIFT	27
#define AR7240_DDR_CONFIG_TMRD_SHIFT	23
#define AR7240_DDR_CONFIG_TRFC_SHIFT	17
#define AR7240_DDR_CONFIG_TRRD_SHIFT	13
#define AR7240_DDR_CONFIG_TRP_SHIFT	9
#define AR7240_DDR_CONFIG_TRCD_SHIFT	5
#define AR7240_DDR_CONFIG_TRAS_SHIFT	0

#define AR7240_DDR_CONFIG2_BL2		(2 << 0)
#define AR7240_DDR_CONFIG2_BL4		(4 << 0)
#define AR7240_DDR_CONFIG2_BL8		(8 << 0)

#define AR7240_DDR_CONFIG2_BT_IL	(1 << 4)
#define AR7240_DDR_CONFIG2_CNTL_OE_EN	(1 << 5)
#define AR7240_DDR_CONFIG2_PHASE_SEL	(1 << 6)
#define AR7240_DDR_CONFIG2_DRAM_CKE	(1 << 7)
#define AR7240_DDR_CONFIG2_TWR_SHIFT	8
#define AR7240_DDR_CONFIG2_TRTW_SHIFT	12
#define AR7240_DDR_CONFIG2_TRTP_SHIFT	17
#define AR7240_DDR_CONFIG2_TWTR_SHIFT	21
#define AR7240_DDR_CONFIG2_HALF_WIDTH_L	(1 << 31)

#define AR7240_DDR_TAP_DEFAULT		0x18

/*
 * DDR block, gmac flushing
 */
#ifdef CONFIG_WASP_SUPPORT
#define AR7240_DDR_GE0_FLUSH		AR7240_DDR_CTL_BASE+0x9c
#define AR7240_DDR_GE1_FLUSH		AR7240_DDR_CTL_BASE+0xa0
#else
#define AR7240_DDR_GE0_FLUSH		AR7240_DDR_CTL_BASE+0x7c
#define AR7240_DDR_GE1_FLUSH		AR7240_DDR_CTL_BASE+0x80
#endif
#define AR7240_DDR_USB_FLUSH		AR7240_DDR_CTL_BASE+0xa4
#define AR7240_DDR_PCIE_FLUSH		AR7240_DDR_CTL_BASE+0x88

#define AR7240_EEPROM_GE0_MAC_ADDR	0xbfff1000
#define AR7240_EEPROM_GE1_MAC_ADDR	0xbfff1006

/*
 * PLL block/CPU
 */

#define AR7240_PLL_CONFIG		AR7240_PLL_BASE+0x0


#define PLL_DIV_SHIFT			0
#define PLL_DIV_MASK			0x3ff
#define REF_DIV_SHIFT			10
#define REF_DIV_MASK			0xf
#define AHB_DIV_SHIFT			19
#define AHB_DIV_MASK			0x1
#define DDR_DIV_SHIFT			22
#define DDR_DIV_MASK			0x1
#ifdef CONFIG_WASP_SUPPORT
#define AR7240_DDR_PLL_CONFIG		AR7240_PLL_BASE+0x4
#define AR7240_DDR_CLK_CTRL		AR7240_PLL_BASE+0x8
#else
#define AR7240_ETH_PLL_CONFIG		AR7240_PLL_BASE+0x4
#endif
#define AR7242_ETH_XMII_CONFIG		AR7240_PLL_BASE+0x2c
#define AR7240_AUDIO_PLL_CONFIG		AR7240_PLL_BASE+0x30

#define AR7240_ETH_INT0_CLK		AR7240_PLL_BASE+0x14
#define AR7240_ETH_INT1_CLK		AR7240_PLL_BASE+0x18


/*
 * USB block
 */
#define AR7240_USB_FLADJ_VAL		AR7240_USB_CONFIG_BASE
#define AR7240_USB_CONFIG		AR7240_USB_CONFIG_BASE+0x4
#define AR7240_USB_WINDOW		0x1000000
#define AR7240_USB_MODE			AR7240_USB_EHCI_BASE+0x1a8

/*
 * PCI block
 */
#define AR7240_PCI_WINDOW		0x8000000 /* 128MB */
#define AR7240_PCI_WINDOW0_OFFSET	AR7240_DDR_CTL_BASE+0x7c
#define AR7240_PCI_WINDOW1_OFFSET	AR7240_DDR_CTL_BASE+0x80
#define AR7240_PCI_WINDOW2_OFFSET	AR7240_DDR_CTL_BASE+0x84
#define AR7240_PCI_WINDOW3_OFFSET	AR7240_DDR_CTL_BASE+0x88
#define AR7240_PCI_WINDOW4_OFFSET	AR7240_DDR_CTL_BASE+0x8c
#define AR7240_PCI_WINDOW5_OFFSET	AR7240_DDR_CTL_BASE+0x90
#define AR7240_PCI_WINDOW6_OFFSET	AR7240_DDR_CTL_BASE+0x94
#define AR7240_PCI_WINDOW7_OFFSET	AR7240_DDR_CTL_BASE+0x98

#define AR7240_PCI_WINDOW0_VAL		0x10000000
#define AR7240_PCI_WINDOW1_VAL		0x11000000
#define AR7240_PCI_WINDOW2_VAL		0x12000000
#define AR7240_PCI_WINDOW3_VAL		0x13000000
#define AR7240_PCI_WINDOW4_VAL		0x14000000
#define AR7240_PCI_WINDOW5_VAL		0x15000000
#define AR7240_PCI_WINDOW6_VAL		0x16000000
#define AR7240_PCI_WINDOW7_VAL		0x07000000

#define ar9331_write_pci_window(_no)	\
	ar9331_reg_wr(AR7240_PCI_WINDOW##_no##_OFFSET, AR7240_PCI_WINDOW##_no##_VAL);

/*
 * CRP. To access the host controller config and status registers
 */
#define AR7240_PCI_CRP			0x180c0000
#define AR7240_PCI_DEV_CFGBASE		0x14000000
#define AR7240_PCI_CRP_AD_CBE		AR7240_PCI_CRP
#define AR7240_PCI_CRP_WRDATA		AR7240_PCI_CRP+0x4
#define AR7240_PCI_CRP_RDDATA		AR7240_PCI_CRP+0x8
#define AR7240_PCI_ERROR		AR7240_PCI_CRP+0x1c
#define AR7240_PCI_ERROR_ADDRESS	AR7240_PCI_CRP+0x20
#define AR7240_PCI_AHB_ERROR		AR7240_PCI_CRP+0x24
#define AR7240_PCI_AHB_ERROR_ADDRESS	AR7240_PCI_CRP+0x28

#define AR7240_CRP_CMD_WRITE		0x00010000
#define AR7240_CRP_CMD_READ		0x00000000

/*
 * PCI CFG. To generate config cycles
 */
#define AR7240_PCI_CFG_AD		AR7240_PCI_CRP+0xc
#define AR7240_PCI_CFG_CBE		AR7240_PCI_CRP+0x10
#define AR7240_PCI_CFG_WRDATA		AR7240_PCI_CRP+0x14
#define AR7240_PCI_CFG_RDDATA		AR7240_PCI_CRP+0x18
#define AR7240_CFG_CMD_READ		0x0000000a
#define AR7240_CFG_CMD_WRITE		0x0000000b

#define AR7240_PCI_IDSEL_ADLINE_START	17


/*
 * gpio configs
 */
#define AR7240_GPIO_OE			AR7240_GPIO_BASE+0x0
#define AR7240_GPIO_IN			AR7240_GPIO_BASE+0x4
#define AR7240_GPIO_OUT			AR7240_GPIO_BASE+0x8
#define AR7240_GPIO_SET			AR7240_GPIO_BASE+0xc
#define AR7240_GPIO_CLEAR		AR7240_GPIO_BASE+0x10
#define AR7240_GPIO_INT_ENABLE		AR7240_GPIO_BASE+0x14
#define AR7240_GPIO_INT_TYPE		AR7240_GPIO_BASE+0x18
#define AR7240_GPIO_INT_POLARITY	AR7240_GPIO_BASE+0x1c
#define AR7240_GPIO_INT_PENDING		AR7240_GPIO_BASE+0x20
#define AR7240_GPIO_INT_MASK		AR7240_GPIO_BASE+0x24
#define AR7240_GPIO_IN_ETH_SWITCH_LED	AR7240_GPIO_BASE+0x28
#define AR7240_GPIO_OUT_FUNCTION0	AR7240_GPIO_BASE+0x2c
#define AR7240_GPIO_OUT_FUNCTION1	AR7240_GPIO_BASE+0x30
#define AR7240_GPIO_OUT_FUNCTION2	AR7240_GPIO_BASE+0x34
#define AR7240_GPIO_OUT_FUNCTION3	AR7240_GPIO_BASE+0x38
#define AR7240_GPIO_OUT_FUNCTION4	AR7240_GPIO_BASE+0x3c
#define AR7240_GPIO_OUT_FUNCTION5	AR7240_GPIO_BASE+0x40
#define AR7240_GPIO_IN_ENABLE0		AR7240_GPIO_BASE+0x44
#define AR7240_GPIO_IN_ENABLE1		AR7240_GPIO_BASE+0x48
#define AR7240_GPIO_IN_ENABLE2		AR7240_GPIO_BASE+0x4c
#define AR7240_GPIO_IN_ENABLE3		AR7240_GPIO_BASE+0x50
#define AR7240_GPIO_IN_ENABLE4		AR7240_GPIO_BASE+0x54
#define AR7240_GPIO_IN_ENABLE5		AR7240_GPIO_BASE+0x58
#define AR7240_GPIO_IN_ENABLE6		AR7240_GPIO_BASE+0x5c
#define AR7240_GPIO_IN_ENABLE7		AR7240_GPIO_BASE+0x60
#define AR7240_GPIO_IN_ENABLE8		AR7240_GPIO_BASE+0x64
#define AR7240_GPIO_IN_ENABLE9		AR7240_GPIO_BASE+0x68
#ifdef CONFIG_WASP_SUPPORT
// bit meanings have changed...
#define AR7240_GPIO_FUNCTIONS		AR7240_GPIO_BASE+0x6c
#else
#define AR7240_GPIO_FUNCTIONS		AR7240_GPIO_BASE+0x28
#define AR7240_GPIO_FUNCTION_2		AR7240_GPIO_BASE+0x30
#define AR7240_GPIO_FUNC_ZERO		AR7240_GPIO_BASE+0x30
#endif
/*
 *   LED      GPIO
 *	LED_0    GPIO_00
 *	LED_1    GPIO_01  //路由器指示
 *	LED_2    GPIO_13  //路由器指示
 *	LED_3    GPIO_14
 *	LED_4    GPIO_15
 *	LED_5    GPIO_16
 *	LED_6    GPIO_17
 *	LED_7    GPIO_27
 *	LED_8    GPIO_26
 */

#define	AR7240_LED_0	(1<<00)
#define	AR7240_LED_1	(1<<01)
#define	AR7240_LED_2	(1<<13)
#define	AR7240_LED_3	(1<<14)
#define	AR7240_LED_4	(1<<15)
#define	AR7240_LED_5	(1<<16)
#define	AR7240_LED_6	(1<<17)
#define	AR7240_LED_7	(1<<27)
#define	AR7240_LED_8	(1<<26)

#define	IIS_CONTROL_CSB  AR7240_LED_6
#define	IIS_CONTROL_SDIN AR7240_LED_8
#define	IIS_CONTROL_SCLK AR7240_LED_7

/*
 * IRQ Map.
 * There are 4 conceptual ICs in the system. We generally give a block of 16
 * irqs to each IC.
 *	CPU :    0 - 0xf
 *	MISC: 0x10 - 0x1f
 *	GPIO: 0x20 - 0x2f
 *	PCI : 0x30 - 0x40
 *
 */
#define AR7240_CPU_IRQ_BASE		0x00
#define AR7240_MISC_IRQ_BASE		0x10
#define AR7240_GPIO_IRQ_BASE		0x20
#ifdef CONFIG_WASP_SUPPORT
#define AR7240_GPIO_IRQ_COUNT		32
#else
#define AR7240_GPIO_IRQ_COUNT		16
#endif
#define AR7240_PCI_IRQ_BASE		(AR7240_GPIO_IRQ_BASE + AR7240_GPIO_IRQ_COUNT)

/*
 * Interrupts connected to the Misc->GPIO line
 */
#define AR7240_GPIO_IRQn(_gpio)		AR7240_GPIO_IRQ_BASE+(_gpio)

//#define MIPS_CPU_IRQ_BASE		AR7240_CPU_IRQ_BASE

/*
 * The IPs. Connected to CPU (hardware IP's; the first two are software)
 */
#ifdef CONFIG_WASP_SUPPORT
#define ATH_CPU_IRQ_WLAN		AR7240_CPU_IRQ_BASE+2
#define AR7240_CPU_IRQ_PCI		AR7240_CPU_IRQ_BASE+8
#elif defined (CONFIG_MACH_HORNET)
#define ATH_CPU_IRQ_WLAN		AR7240_CPU_IRQ_BASE+2
#else
#define AR7240_CPU_IRQ_PCI		AR7240_CPU_IRQ_BASE+2
#endif
#define AR7240_CPU_IRQ_USB		AR7240_CPU_IRQ_BASE+3
#define AR7240_CPU_IRQ_GE0		AR7240_CPU_IRQ_BASE+4
#define AR7240_CPU_IRQ_GE1		AR7240_CPU_IRQ_BASE+5
#define AR7240_CPU_IRQ_MISC		AR7240_CPU_IRQ_BASE+6
#define AR7240_CPU_IRQ_TIMER		AR7240_CPU_IRQ_BASE+7

/*
 * Interrupts connected to the CPU->Misc line.
 */
#define AR7240_MISC_IRQ_TIMER		AR7240_MISC_IRQ_BASE+0
#define AR7240_MISC_IRQ_ERROR		AR7240_MISC_IRQ_BASE+1
#define AR7240_MISC_IRQ_GPIO		AR7240_MISC_IRQ_BASE+2
#define AR7240_MISC_IRQ_UART		AR7240_MISC_IRQ_BASE+3
#define AR7240_MISC_IRQ_WATCHDOG	AR7240_MISC_IRQ_BASE+4
#define AR7240_MISC_IRQ_PERF_COUNTER	AR7240_MISC_IRQ_BASE+5
#define AR7240_MISC_IRQ_USB_OHCI	AR7240_MISC_IRQ_BASE+6
#define AR7240_MISC_IRQ_DMA		AR7240_MISC_IRQ_BASE+7
#define AR7240_MISC_IRQ_ENET_LINK	AR7240_MISC_IRQ_BASE+12

#define AR7240_MISC_IRQ_COUNT		13

#define MIMR_TIMER			0x01
#define MIMR_ERROR			0x02
#define MIMR_GPIO			0x04
#define MIMR_UART			0x08
#define MIMR_WATCHDOG			0x10
#define MIMR_PERF_COUNTER		0x20
#define MIMR_OHCI_USB			0x40
#define MIMR_DMA			0x80
#define MIMR_ENET_LINK			0x1000

#define MISR_TIMER			MIMR_TIMER
#define MISR_ERROR			MIMR_ERROR
#define MISR_GPIO			MIMR_GPIO
#define MISR_UART			MIMR_UART
#define MISR_WATCHDOG			MIMR_WATCHDOG
#define MISR_PERF_COUNTER		MIMR_PERF_COUNTER
#define MISR_OHCI_USB			MIMR_OHCI_USB
#define MISR_DMA			MIMR_DMA

void ar9331_gpio_irq_init(int irq_base);

void ar9331_misc_enable_irq (unsigned int mask);
void ar9331_misc_disable_irq (unsigned int mask);

unsigned int ar9331_misc_get_irq_mask (void);
unsigned int ar9331_misc_get_irq_status (void);


/* Interrupts connected to CPU->PCI */
#ifdef CONFIG_PERICOM
#	define AR7240_PRI_BUS_NO	0u
#	define AR7240_PORT0_BUS_NO	1u
#	define AR7240_PORT1_BUS_NO	2u
#	define AR7240_PCI_IRQ_DEV0	(AR7240_PCI_IRQ_BASE + 0)
#	define AR7240_PCI_IRQ_DEV1	(AR7240_PCI_IRQ_BASE + 1)
#	define AR7240_PCI_IRQ_COUNT	2
#else
#	define AR7240_PCI_IRQ_DEV0	AR7240_PCI_IRQ_BASE+0
#	define AR7240_PCI_IRQ_COUNT	1
#endif /* CONFIG_PERICOM */

/*
 * PCI interrupt mask and status
 */
#define PIMR_DEV0			0x01
#define PIMR_DEV1			0x02
#define PIMR_DEV2			0x04
#define PIMR_CORE			0x10

#define PISR_DEV0			PIMR_DEV0
#define PISR_DEV1			PIMR_DEV1
#define PISR_DEV2			PIMR_DEV2
#define PISR_CORE			PIMR_CORE

void ar9331_pci_irq_init(int irq_base); /* ??? */

/*
 * GPIO Function Enables
 */
#define AR7240_GPIO_FUNCTION_STEREO_EN	(1<<17)
#define AR7240_GPIO_FUNCTION_SLIC_EN	(1<<16)

#if 0
#define AR7240_GPIO_FUNCTION_SPI_CS_1_EN	(1<<15)
#define AR7240_GPIO_FUNCTION_SPI_CS_0_EN	(1<<14)
#define AR7240_GPIO_FUNCTION_UART_EN		(1<< 8)
#endif

#define AR7240_GPIO_FUNCTION_OVERCURRENT_EN		(1<< 4)
#define AR7240_GPIO_FUNCTION_USB_CLK_CORE_EN		(1<< 0)
#define AR7240_GPIO_FUNCTION_WMAC_LED			(1<<22)
#define AR7240_GPIO_FUNCTION_STEREO_EN			(1<<17)
#define AR7240_GPIO_FUNCTION_SLIC_EN			(1<<16)
#define AR7240_GPIO_FUNCTION_SPDIF2TCK_EN		(1<<31)
#define AR7240_GPIO_FUNCTION_SPDIF_EN			(1<<30)
#define AR7240_GPIO_FUNCTION_I2S_GPIO_18_22_EN		(1<<29)
#define AR7240_GPIO_FUNCTION_I2S_REFCLKEN		(1<<28)
#define AR7240_GPIO_FUNCTION_I2S_MCKEN			(1<<27)
#define AR7240_GPIO_FUNCTION_I2S0_EN			(1<<26)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED_DUPL_EN	(1<<25)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED_COLL	(1<<24)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED_ACTV	(1<<23)
#define AR7240_GPIO_FUNCTION_PLL_SHIFT_EN		(1<<22)
#define AR7240_GPIO_FUNCTION_EXT_MDIO_SEL		(1<<21)
#define AR7240_GPIO_FUNCTION_CLK_OBS6_ENABLE		(1<<20)
#define AR7240_GPIO_FUNCTION_CLK_OBS0_ENABLE		(1<<19)
#define AR7240_GPIO_FUNCTION_SPI_EN			(1<<18)
#define AR7240_GPIO_FUNCTION_DDR_DQOE_EN		(1<<17)
#define AR7240_GPIO_FUNCTION_PCIEPHY_TST_EN		(1<<16)
#define AR7240_GPIO_FUNCTION_S26_UART_DISABLE		(1<<15)
#define AR7240_GPIO_FUNCTION_SPI_CS_1_EN		(1<<14)
#define AR7240_GPIO_FUNCTION_SPI_CS_0_EN		(1<<13)
#define AR7240_GPIO_FUNCTION_CLK_OBS5_ENABLE		(1<<12)
#define AR7240_GPIO_FUNCTION_CLK_OBS4_ENABLE		(1<<11)
#define AR7240_GPIO_FUNCTION_CLK_OBS3_ENABLE		(1<<10)
#define AR7240_GPIO_FUNCTION_CLK_OBS2_ENABLE		(1<< 9)
#define AR7240_GPIO_FUNCTION_CLK_OBS1_ENABLE		(1<< 8)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED4_EN		(1<< 7)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED3_EN		(1<< 6)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED2_EN		(1<< 5)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED1_EN		(1<< 4)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED0_EN		(1<< 3)
#define AR7240_GPIO_FUNCTION_UART_RTS_CTS_EN		(1<< 2)
#define AR7240_GPIO_FUNCTION_UART_EN			(1<< 1)
#define AR7240_GPIO_FUNCTION_2_EN_I2WS_ON_0		(1<< 4)
#define AR7240_GPIO_FUNCTION_2_EN_I2SCK_ON_1		(1<< 3)
#define AR7240_GPIO_FUNCTION_2_I2S_ON_LED		(1<< 1)
#define AR7240_GPIO_FUNCTION_SRIF_ENABLE		(1<< 0)
#ifdef CONFIG_WASP_SUPPORT
#define AR7240_GPIO_FUNCTION_USB_LED			(1<< 4)
#define AR7240_GPIO_FUNCTION_JTAG_DISABLE		(1<< 1)
#else
#define AR7240_GPIO_FUNCTION_JTAG_DISABLE		(1<< 0)
#define AR7240_GPIO_FUNCTION_ETH_SWITCH_LED1_EN		(1<< 4)
#endif

#define AR7240_GPIO_OE_EN(x)				(x)
#define AR7240_GPIO_IN_ENABLE4_SLIC_PCM_FS_IN(x)	((0xff&x)<< 8)
#define AR7240_GPIO_IN_ENABLE4_SLIC_DATA_IN(x)		(0xff&x)
#define AR7240_GPIO_OUT_FUNCTION3_ENABLE_GPIO_15(x)	((0xff&x)<<24)
#define AR7240_GPIO_OUT_FUNCTION3_ENABLE_GPIO_14(x)	((0xff&x)<<16)
#define AR7240_GPIO_OUT_FUNCTION3_ENABLE_GPIO_13(x)	((0xff&x)<< 8)
#define AR7240_GPIO_OUT_FUNCTION3_ENABLE_GPIO_12(x)	(0xff&x)
#define AR7240_GPIO_OUT_FUNCTION2_ENABLE_GPIO_11(x)	((0xff&x)<<24)
#define AR7240_GPIO_OUT_FUNCTION2_ENABLE_GPIO_10(x)	((0xff&x)<<16)
#define AR7240_GPIO_OUT_FUNCTION2_ENABLE_GPIO_9(x)	((0xff&x)<< 8)
#define AR7240_GPIO_OUT_FUNCTION2_ENABLE_GPIO_8(x)	(0xff&x)
#define AR7240_GPIO_OUT_FUNCTION1_ENABLE_GPIO_7(x)	((0xff&x)<<24)
#define AR7240_GPIO_OUT_FUNCTION1_ENABLE_GPIO_6(x)	((0xff&x)<<16)
#define AR7240_GPIO_OUT_FUNCTION1_ENABLE_GPIO_5(x)	((0xff&x)<< 8)
#define AR7240_GPIO_OUT_FUNCTION1_ENABLE_GPIO_4(x)	(0xff&x)
#define AR7240_GPIO_OUT_FUNCTION0_ENABLE_GPIO_3(x)	((0xff&x)<<24)
#define AR7240_GPIO_OUT_FUNCTION0_ENABLE_GPIO_2(x)	((0xff&x)<<16)
#define AR7240_GPIO_IN_ENABLE1_I2SEXT_MCLK(x)		((0xff&x)<<24)
#define AR7240_GPIO_IN_ENABLE0_UART_SIN(x)		((0xff&x)<< 8)
#define AR7240_GPIO_IN_ENABLE0_SPI_DATA_IN(x)		(0xff&x)

/*
 * GPIO Access & Control
 */
void ar9331_gpio_init(void);
void ar9331_gpio_down(void);
void ar9331_gpio_up(void);

/*
 * GPIO Helper Functions
 */
void ar9331_gpio_enable_slic(void);

/* enable UART block, takes away GPIO 10 and 9 */
void ar9331_gpio_enable_uart(void);

/* enable STEREO block, takes away GPIO 11,8,7, and 6 */
void ar9331_gpio_enable_stereo(void);

/* allow CS0/CS1 to be controlled via SPI register, takes away GPIO0/GPIO1 */
void ar9331_gpio_enable_spi_cs1_cs0(void);

/* allow GPIO0/GPIO1 to be used as SCL/SDA for software based i2c */
void ar9331_gpio_enable_i2c_on_gpio_0_1(void);

/*
 * GPIO General Functions
 */
void ar9331_gpio_drive_low(unsigned int mask);
void ar9331_gpio_drive_high(unsigned int mask);

unsigned int ar9331_gpio_float_high_test(unsigned int mask);

/*
 * Software support of i2c on gpio 0/1
 */
int ar9331_i2c_raw_write_bytes_to_addr(int addr, unsigned char *buffer, int count);
int ar9331_i2c_raw_read_bytes_from_addr(int addr, unsigned char *buffer, int count);

/* SPI, SLIC and GPIO are all multiplexed on gpio pins */
#define AR7240_SPI_FS		(AR7240_SPI_BASE+0x00)
#define AR7240_SPI_READ		(AR7240_SPI_BASE+0x00)
#define AR7240_SPI_CLOCK	(AR7240_SPI_BASE+0x04)
#define AR7240_SPI_WRITE	(AR7240_SPI_BASE+0x08)
#define AR7240_SPI_RD_STATUS	(AR7240_SPI_BASE+0x0c)
#define AR7240_SPI_SHIFT_DO	(AR7240_SPI_BASE+0x10)
#define AR7240_SPI_SHIFT_CNT	(AR7240_SPI_BASE+0x14)
#define AR7240_SPI_SHIFT_DI	(AR7240_SPI_BASE+0x18)
#define AR7240_SPI_D0_HIGH	(1<<0)	/* Pin spi_do */
#define AR7240_SPI_CLK_HIGH	(1<<8)	/* Pin spi_clk */

#define AR7240_SPI_CS_ENABLE_0	(6<<16)	/* Pin gpio/cs0 (active low) */
#define AR7240_SPI_CS_ENABLE_1	(5<<16)	/* Pin gpio/cs1 (active low) */
#define AR7240_SPI_CS_ENABLE_2	(3<<16)	/* Pin gpio/cs2 (active low) */
//#define AR7240_SPI_CS_DIS	(AR7240_SPI_CS_ENABLE_0|AR7240_SPI_CS_ENABLE_1|AR7240_SPI_CS_ENABLE_2)
#define AR7240_SPI_CS_DIS	0x70000


/*
 * SOC
 */
#define AR7240_SPI_CMD_WRITE_SR		0x01
#define AR7240_SPI_CMD_WREN		0x06
#define AR7240_SPI_CMD_RD_STATUS	0x05
#define AR7240_SPI_CMD_FAST_READ	0x0b
#define AR7240_SPI_CMD_PAGE_PROG	0x02
#define AR7240_SPI_CMD_SECTOR_ERASE	0xd8

/* Functions to access SPI through software. Example:
 *
 * ar9331_spi_down(); ---------------------- disable others from accessing SPI bus taking semaphore
 * ar9331_spi_enable_soft_access(); -------- disable HW control of SPI
 *
 * <board specific chip select routine>
 *
 * <read/write SPI using using custom routine or general purposeflash routines
 * Custom routine may use:
 *
 *	ar9331_spi_raw_output_u8(unsigned char)
 *	ar9331_spi_raw_output_u32(unsigned int)
 *	ar9331_spi_raw_input_u32()
 *
 * General purpose flash routines:
 *	ar9331_spi_flash_read_page(unsigned int addr, unsigned char *data, int len);
 *	ar9331_spi_flash_write_page(unsigned int addr, unsigned char *data, int len);
 *	ar9331_spi_flash_sector_erase(unsigned int addr);
 * >
 *
 * <board specific chip deselect routine>
 *
 * ar9331_spi_disable_soft_acess(); ------- enable HW control of SPI bus
 * ar9331_spi_up(); ----------------------- enable others to access SPI bus releasing semaphore
 */
void ar9331_spi_init(void);
void ar9331_spi_down(void);
void ar9331_spi_up(void);

static inline void
ar9331_spi_enable_soft_access(void)
{
	ar9331_reg_wr_nf(AR7240_SPI_FS, 1);
}

static inline void
ar9331_spi_disable_soft_access(void)
{
	ar9331_reg_wr_nf(AR7240_SPI_WRITE, AR7240_SPI_CS_DIS);
	ar9331_reg_wr_nf(AR7240_SPI_FS, 0);
}

void ar9331_spi_raw_output_u8(unsigned char val);
void ar9331_spi_raw_output_u32(unsigned int val);
unsigned int ar9331_spi_raw_input_u8(void);
unsigned int ar9331_spi_raw_input_u32(void);

#define AR7240_SPI_SECTOR_SIZE	(1024*64)

void ar9331_spi_flash_read_page(unsigned int addr, unsigned char *data, int len);
void ar9331_spi_flash_write_page(unsigned int addr, unsigned char *data, int len);
void ar9331_spi_flash_sector_erase(unsigned int addr);

/*
 * Allow access to cs0-2 when GPIO Function enables cs0-2 through SPI register.
 */
static inline void ar9331_spi_enable_cs0(void)
{
	unsigned int cs;
	ar9331_spi_down();
	ar9331_spi_enable_soft_access();
	cs = ar9331_reg_rd(AR7240_SPI_WRITE) & ~AR7240_SPI_CS_DIS;
	ar9331_reg_wr_nf(AR7240_SPI_WRITE, AR7240_SPI_CS_ENABLE_0 | cs);
}

#ifdef CONFIG_WASP_SUPPORT
static inline void ar9331_spi_enable_cs1(void)
{
	unsigned int cs;
	ar9331_spi_down();
	ar9331_spi_init();
	ar9331_spi_enable_soft_access();
	cs = ar9331_reg_rd(AR7240_SPI_WRITE) & AR7240_SPI_CS_DIS;
	ar9331_reg_wr_nf(AR7240_SPI_WRITE, cs | AR7240_SPI_CLK_HIGH);
	cs = ar9331_reg_rd(AR7240_SPI_WRITE) & ~AR7240_SPI_CS_DIS;
	ar9331_reg_wr_nf(AR7240_SPI_WRITE, AR7240_SPI_CS_ENABLE_1 | cs | AR7240_SPI_CLK_HIGH);
	ar9331_reg_wr_nf(AR7240_SPI_WRITE, AR7240_SPI_CS_ENABLE_1 | cs);
}
#else
static inline void ar9331_spi_enable_cs1(void)
{
	unsigned int cs;
	ar9331_spi_down();
	ar9331_spi_enable_soft_access();
	cs = ar9331_reg_rd(AR7240_SPI_WRITE) & ~AR7240_SPI_CS_DIS;
	ar9331_reg_wr_nf(AR7240_SPI_WRITE, AR7240_SPI_CS_ENABLE_1 | cs);
}
#endif

static inline void ar9331_spi_disable_cs(void)
{
	unsigned int cs = ar9331_reg_rd(AR7240_SPI_WRITE) | AR7240_SPI_CS_DIS;
	ar9331_reg_wr_nf(AR7240_SPI_WRITE, cs);
	ar9331_spi_disable_soft_access();
	ar9331_spi_up();
}

/*
 * Example usage to access BOOT flash
 */
static inline void ar9331_spi_flash_cs0_sector_erase(unsigned int addr)
{
	ar9331_spi_enable_cs0();
	ar9331_spi_flash_sector_erase(addr);
	ar9331_spi_disable_cs();
}

static inline void ar9331_spi_flash_cs0_write_page(unsigned int addr, unsigned char *data, int len)
{
	ar9331_spi_enable_cs0();
	ar9331_spi_flash_write_page(addr, data, len);
	ar9331_spi_disable_cs();
}

/*
 * Reset block
 */
#define AR7240_GENERAL_TMR		AR7240_RESET_BASE+0
#define AR7240_GENERAL_TMR_RELOAD	AR7240_RESET_BASE+4
#define AR7240_WATCHDOG_TMR_CONTROL	AR7240_RESET_BASE+8
#define AR7240_WATCHDOG_TMR		AR7240_RESET_BASE+0xc
#define AR7240_MISC_INT_STATUS		AR7240_RESET_BASE+0x10
#define AR7240_MISC_INT_MASK		AR7240_RESET_BASE+0x14

#define AR7240_PCI_INT_STATUS		AR7240_PCI_CTLR_BASE+0x4c
#define AR7240_PCI_INT_MASK		AR7240_PCI_CTLR_BASE+0x50
#define AR7240_PCI_INT_A_L		(1 << 14) /* INTA Level Trigger */
#define AR7240_PCI_INT_B_L		(1 << 15) /* INTB Level Trigger */
#define AR7240_PCI_INT_C_L		(1 << 16) /* INTC Level Trigger */
#ifdef CONFIG_WASP_SUPPORT
#define AR7240_GLOBAL_INT_STATUS	AR7240_RESET_BASE+0x18
#else
#define AR7240_GLOBAL_INT_STATUS	AR7240_RESET_BASE+0x20
#endif
#define AR7240_RESET			AR7240_RESET_BASE+0x1c
#define AR7240_OBSERVATION_ENABLE	AR7240_RESET_BASE+0x28

#define ATH_BOOTSTRAP_REG		(AR7240_RESET_BASE + 0xb0)
#define ATH_REF_CLK_40			(1 << 4) /* 0 - 25MHz	1 - 40 MHz */
#define ATH_DDR_WIDTH_32		(1 << 3)

#define AR7240_PCIE_WMAC_INT_STATUS	AR7240_RESET_BASE+0xac
#	define WMAC_MISC_INT	(1 << 0)	/* Indicates there is a WMAC Intr */
#	define WMAC_TX_INT	(1 << 1)	/* Reason of interrupt */
#	define WMAC_RXLP_INT	(1 << 2)
#	define WMAC_RXHP_INT	(1 << 3)
#	define PCIE_RC_INT	(1 << 4)
#	define PCIE_RC_INT0	(1 << 5)
#	define PCIE_RC_INT1	(1 << 6)
#	define PCIE_RC_INT2	(1 << 7)
#	define PCIE_RC_INT3	(1 << 8)

#	define PCI_WMAC_INTR	(PCIE_RC_INT | PCIE_RC_INT0 | PCIE_RC_INT1 | \
				 PCIE_RC_INT2 | PCIE_RC_INT3)


#define AR7240_WD_ACT_MASK		3u
#define AR7240_WD_ACT_NONE		0u /* No Action */
#define AR7240_WD_ACT_GP_INTR		1u /* General purpose intr */
#define AR7240_WD_ACT_NMI		2u /* NMI */
#define AR7240_WD_ACT_RESET		3u /* Full Chip Reset */

#define AR7240_WD_LAST_SHIFT		31
#define AR7240_WD_LAST_MASK		((uint32_t)(1 << AR7240_WD_LAST_SHIFT))



/*
 * Performace counters
 */
#define AR7240_PERF0_COUNTER		AR7240_GE0_BASE+0xa0
#define AR7240_PERF1_COUNTER		AR7240_GE1_BASE+0xa0

/*
 * SLIC/STEREO DMA Size Configurations
 */
#define AR7240_DMA_BUF_SIZE_4X2		0x00
#define AR7240_DMA_BUF_SIZE_8X2		0x01
#define AR7240_DMA_BUF_SIZE_16X2	0x02
#define AR7240_DMA_BUF_SIZE_32X2	0x03
#define AR7240_DMA_BUF_SIZE_64X2	0x04
#define AR7240_DMA_BUF_SIZE_128X2	0x05
#define AR7240_DMA_BUF_SIZE_256X2	0x06
#define AR7240_DMA_BUF_SIZE_512X2	0x07

/*
 * SLIC/STEREO DMA Assignments
 */
#define AR7240_DMA_CHAN_SLIC0_RX	0
#define AR7240_DMA_CHAN_SLIC1_RX	1
#define AR7240_DMA_CHAN_STEREO_RX	2
#define AR7240_DMA_CHAN_SLIC0_TX	3
#define AR7240_DMA_CHAN_SLIC1_TX	4
#define AR7240_DMA_CHAN_STEREO_TX	5

/* Low-level routines */
void ar9331_dma_addr_wr(int chan, unsigned int val);
void ar9331_dma_config_wr(int chan, unsigned int val);
void ar9331_dma_update_wr(int chan, unsigned int val);

unsigned int ar9331_dma_addr_rd(int chan);
unsigned int ar9331_dma_config_rd(int chan);

/* Use this routine to configure DMA access. Example:
 *
 * ar9331_dma_config_buffer( AR7240_DMA_CHAN_SLIC0_TX,
 *				< address of buffer >,
 *				AR7240_DMA_BUF_SIZE_512X2
 */
void ar9331_dma_config_buffer(int chan, void *buffer, int sizeCfg);

/*
 * MBOX register definitions
 */
#define AR7240_MBOX_FIFO			(AR7240_DMA_BASE+0x00)
#define AR7240_MBOX_FIFO_STATUS			(AR7240_DMA_BASE+0x08)
#define AR7240_MBOX_SLIC_FIFO_STATUS		(AR7240_DMA_BASE+0x0c)
#define AR7240_MBOX_DMA_POLICY			(AR7240_DMA_BASE+0x10)
#define AR7240_MBOX_SLIC_DMA_POLICY		(AR7240_DMA_BASE+0x14)
#define AR7240_MBOX_DMA_RX_DESCRIPTOR_BASE0	(AR7240_DMA_BASE+0x18)
#define AR7240_MBOX_DMA_RX_CONTROL0		(AR7240_DMA_BASE+0x1c)
#define AR7240_MBOX_DMA_TX_DESCRIPTOR_BASE0	(AR7240_DMA_BASE+0x20)
#define AR7240_MBOX_DMA_TX_CONTROL0		(AR7240_DMA_BASE+0x24)
#define AR7240_MBOX_DMA_RX_DESCRIPTOR_BASE1	(AR7240_DMA_BASE+0x28)
#define AR7240_MBOX_DMA_RX_CONTROL1		(AR7240_DMA_BASE+0x2c)
#define AR7240_MBOX_DMA_TX_DESCRIPTOR_BASE1	(AR7240_DMA_BASE+0x30)
#define AR7240_MBOX_DMA_TX_CONTROL1		(AR7240_DMA_BASE+0x34)
#define AR7240_MBOX_FRAME			(AR7240_DMA_BASE+0x34)
#define AR7240_MBOX_SLIC_FRAME			(AR7240_DMA_BASE+0x3c)
#define AR7240_MBOX_FIFO_TIMEOUT		(AR7240_DMA_BASE+0x40)
#define AR7240_MBOX_INT_STATUS			(AR7240_DMA_BASE+0x44)
#define AR7240_MBOX_SLIC_INT_STATUS		(AR7240_DMA_BASE+0x48)
#define AR7240_MBOX_INT_ENABLE			(AR7240_DMA_BASE+0x4c)
#define AR7240_MBOX_SLIC_INT_ENABLE		(AR7240_DMA_BASE+0x50)
#define AR7240_MBOX_FIFO_RESET			(AR7240_DMA_BASE+0x58)
#define AR7240_MBOX_SLIC_FIFO_RESET		(AR7240_DMA_BASE+0x5c)

/*
 * MBOX Enables
 */
#define AR7240_MBOX_DMA_POLICY_RX_QUANTUM		(1<< 1)
#define AR7240_MBOX_DMA_POLICY_TX_QUANTUM		(1<< 3)
#define AR7240_MBOX_DMA_POLICY_TX_FIFO_THRESH(x)	((0xff&x)<< 4)

/*
 * SLIC register definitions
 */
#ifdef CONFIG_WASP_SUPPORT
#define AR7240_SLIC_SLOT			(AR7240_SLIC_BASE+0x00)
#define AR7240_SLIC_CLOCK_CTRL			(AR7240_SLIC_BASE+0x04)
#define AR7240_SLIC_CTRL			(AR7240_SLIC_BASE+0x08)
#define AR7240_SLIC_TX_SLOTS1			(AR7240_SLIC_BASE+0x0c)
#define AR7240_SLIC_TX_SLOTS2			(AR7240_SLIC_BASE+0x10)
#define AR7240_SLIC_RX_SLOTS1			(AR7240_SLIC_BASE+0x14)
#define AR7240_SLIC_RX_SLOTS2			(AR7240_SLIC_BASE+0x18)
#define AR7240_SLIC_TIMING_CTRL			(AR7240_SLIC_BASE+0x1c)
#define AR7240_SLIC_INTR			(AR7240_SLIC_BASE+0x20)
#define AR7240_SLIC_SWAP			(AR7240_SLIC_BASE+0x24)
#else
#define AR7240_SLIC_STATUS			(AR7240_SLIC_BASE+0x00)
#define AR7240_SLIC_CNTRL			(AR7240_SLIC_BASE+0x04)
#define AR7240_SLIC_SLOT0_NUM			(AR7240_SLIC_BASE+0x08)
#define AR7240_SLIC_SLOT1_NUM			(AR7240_SLIC_BASE+0x0c)
#define AR7240_SLIC_SAM_POS			(AR7240_SLIC_BASE+0x2c)
#define AR7240_SLIC_FREQ_DIV			(AR7240_SLIC_BASE+0x30)
#endif

/*
 * SLIC Control bits
 */
#define AR7240_SLIC_CNTRL_ENABLE		(1<<0)
#define AR7240_SLIC_CNTRL_SLOT0_ENABLE		(1<<1)
#define AR7240_SLIC_CNTRL_SLOT1_ENABLE		(1<<2)
#define AR7240_SLIC_CNTRL_IRQ_ENABLE		(1<<3)

/*
 * SLIC Helper Functions
 */
unsigned int ar9331_slic_status_rd(void);
unsigned int ar9331_slic_cntrl_rd(void);

void ar9331_slic_cntrl_wr(unsigned int val);
void ar9331_slic_0_slot_pos_wr(unsigned int val);
void ar9331_slic_1_slot_pos_wr(unsigned int val);
void ar9331_slic_freq_div_wr(unsigned int val);
void ar9331_slic_sample_pos_wr(unsigned int val);

void ar9331_slic_setup(int _sam, int _s0n, int _s1n);

/*
 * STEREO register definitions
 */
#define AR7240_STEREO_CONFIG			(AR7240_STEREO_BASE+0x00)
#define AR7240_STEREO_VOLUME			(AR7240_STEREO_BASE+0x04)
#define AR7240_STEREO_CLK_DIV			(AR7240_STEREO_BASE+0x1C)//gl-inet
#define AR7240_STEREO_MCLK			(AR7240_STEREO_BASE+0x08)

/*
 * Stereo Configuration Bits
 */
#define AR7240_STEREO_CONFIG_SPDIF_ENABLE		(1<<23)
#define AR7240_STEREO_CONFIG_ENABLE			(1<<21)
#define AR7240_STEREO_CONFIG_RESET			(1<<19)
#define AR7240_STEREO_CONFIG_DELAY			(1<<18)
#define AR7240_STEREO_CONFIG_PCM_SWAP			(1<<17)
#define AR7240_STEREO_CONFIG_MIC_WORD_SIZE		(1<<16)
#define AR7240_STEREO_CONFIG_MODE(x)			((3&x)<<14)
#define AR7240_STEREO_MODE_STEREO			0
#define AR7240_STEREO_MODE_LEFT				1
#define AR7240_STEREO_MODE_RIGHT			2
#define AR7240_STEREO_CONFIG_DATA_WORD_SIZE(x)		((3&x)<<12)
#define AR7240_STEREO_CONFIG_I2S_32B_WORD		(1<<11)
#define AR7240_STEREO_CONFIG_I2S_MCLK_SEL		(1<<10)
#define AR7240_STEREO_CONFIG_SAMPLE_CNT_CLEAR_TYPE	(1<<9)
#define AR7240_STEREO_CONFIG_MASTER			(1<<8)
#define AR7240_STEREO_CONFIG_PSEDGE(x)			(0xff&x)

/*
 * Word sizes to use with common configurations:
 */
#define AR7240_STEREO_WS_8B		0
#define AR7240_STEREO_WS_16B		1
#define AR7240_STEREO_WS_24B		2
#define AR7240_STEREO_WS_32B		3

/*
 * Slic Configuration Bits
 */
#define AR7240_SLIC_SLOT_SEL(x)					(0x7f&x)
#define AR7240_SLIC_CLOCK_CTRL_DIV(x)				(0x3f&x)
#define AR7240_SLIC_CTRL_CLK_EN					(1<<3)
#define AR7240_SLIC_CTRL_MASTER					(1<<2)
#define AR7240_SLIC_CTRL_EN					(1<<1)
#define AR7240_SLIC_TX_SLOTS1_EN(x)				(x)
#define AR7240_SLIC_TX_SLOTS2_EN(x)				(x)
#define AR7240_SLIC_RX_SLOTS1_EN(x)				(x)
#define AR7240_SLIC_RX_SLOTS2_EN(x)				(x)
#define AR7240_SLIC_TIMING_CTRL_RXDATA_SAMPLE_POS_EXTEND	(1<<11)
#define AR7240_SLIC_TIMING_CTRL_DATAOEN_ALWAYS			(1<<9)
#define AR7240_SLIC_TIMING_CTRL_RXDATA_SAMPLE_POS(x)		((0x3&x)<<7)
#define AR7240_SLIC_TIMING_CTRL_TXDATA_FS_SYNC(x)		((0x3&x)<<5)
#define AR7240_SLIC_TIMING_CTRL_LONG_FSCLKS(x)			((0x7&x)<<2)
#define AR7240_SLIC_TIMING_CTRL_FS_POS				(1<<1)
#define AR7240_SLIC_TIMING_CTRL_LONG_FS				(1<<0)
#define AR7240_SLIC_INTR_MASK(x)				(0x1f&x)
#define AR7240_SLIC_SWAP_RX_DATA				(1<<1)
#define AR7240_SLIC_SWAP_TX_DATA				(1<<0)

#define AR7240_SLIC_TIMING_CTRL_RXDATA_SAMPLE_POS_2_NGEDGE	2
#define AR7240_SLIC_TIMING_CTRL_RXDATA_SAMPLE_POS_1_NGEDGE	1
#define AR7240_SLIC_TIMING_CTRL_TXDATA_FS_SYNC_NXT_PSEDGE	2
#define AR7240_SLIC_TIMING_CTRL_TXDATA_FS_SYNC_NXT_NGEDGE	3
#define AR7240_SLIC_TIMING_CTRL_LONG_FSCLKS_1BIT		0
#define AR7240_SLIC_TIMING_CTRL_LONG_FSCLKS_8BIT		7
#define AR7240_SLIC_INTR_STATUS_NO_INTR				0
#define AR7240_SLIC_INTR_STATUS_UNEXP_FRAME			1
#define AR7240_SLIC_INTR_MASK_RESET				0x1f
#define AR7240_SLIC_INTR_MASK_0					1
#define AR7240_SLIC_INTR_MASK_1					2
#define AR7240_SLIC_INTR_MASK_2					4
#define AR7240_SLIC_INTR_MASK_3					8
#define AR7240_SLIC_INTR_MASK_4					16

/*
 * Audio data is little endian so 16b values must be swapped in the DMA buffers.
 */
static inline int ar9331_stereo_sample_16b_cvt(unsigned int _v) { return (((_v<<8)&0xff00)|((_v>>8)&0xff)) & 0xffff; }

/* Low level read/write of configuration */
void ar9331_stereo_config_wr(unsigned int val);
void ar9331_stereo_volume_wr(unsigned int val);

unsigned int ar9331_stereo_config_rd(void);
unsigned int ar9331_stereo_volume_rd(void);

/*
 * Common configurations for stereo block
 */
#define AR7240_STEREO_CFG_MASTER_STEREO_FS32_48KHZ(ws) ( \
	AR7240_STEREO_CONFIG_DELAY | \
	AR7240_STEREO_CONFIG_RESET | \
	AR7240_STEREO_CONFIG_DATA_WORD_SIZE(ws) | \
	AR7240_STEREO_CONFIG_MODE(AR7240_STEREO_MODE_LEFT) | \
	AR7240_STEREO_CONFIG_MASTER | \
	AR7240_STEREO_CONFIG_PSEDGE(26))

#define AR7240_STEREO_CFG_MASTER_STEREO_FS64_48KHZ(ws) ( \
	AR7240_STEREO_CONFIG_DELAY | \
	AR7240_STEREO_CONFIG_RESET | \
	AR7240_STEREO_CONFIG_DATA_WORD_SIZE(ws) | \
	AR7240_STEREO_CONFIG_MODE(AR7240_STEREO_MODE_STEREO) | \
	AR7240_STEREO_CONFIG_I2S_32B_WORD | \
	AR7240_STEREO_CONFIG_MASTER | \
	AR7240_STEREO_CONFIG_PSEDGE(13))

#define AR7240_STEREO_CFG_SLAVE_STEREO_FS32_48KHZ(ws) ( \
	AR7240_STEREO_CONFIG_RESET | \
	AR7240_STEREO_CONFIG_DATA_WORD_SIZE(ws) | \
	AR7240_STEREO_CONFIG_MODE(AR7240_STEREO_MODE_STEREO) | \
	AR7240_STEREO_CONFIG_PSEDGE(26))

#define AR7240_STEREO_CFG_SLAVE_STEREO_FS64_48KHZ(ws) ( \
	AR7240_STEREO_CONFIG_RESET | \
	AR7240_STEREO_CONFIG_I2S_32B_WORD | \
	AR7240_STEREO_CONFIG_DATA_WORD_SIZE(ws) | \
	AR7240_STEREO_CONFIG_MODE(AR7240_STEREO_MODE_STEREO) | \
	AR7240_STEREO_CONFIG_PSEDGE(13))

/* Routine sets up STEREO block for use. Use one of the predefined
 * configurations. Example:
 *
 * ar9331_stereo_config_setup(
 *	AR7240_STEREO_CFG_MASTER_STEREO_FS32_48KHZ(AR7240_STEREO_WS_16B))
 *
 */
void ar9331_stereo_config_setup(unsigned int cfg);

/* 48 kHz, 16 bit data & i2s 32fs */
static inline void ar9331_setup_for_stereo_master(int ws)
{ ar9331_stereo_config_setup(AR7240_STEREO_CFG_MASTER_STEREO_FS32_48KHZ(ws)); }

/* 48 kHz, 16 bit data & 32fs i2s */
static inline void ar9331_setup_for_stereo_slave(int ws)
{ ar9331_stereo_config_setup(AR7240_STEREO_CFG_SLAVE_STEREO_FS32_48KHZ(ws)); }

/*
 * PERF CTL bits
 */
#define PERF_CTL_PCI_AHB_0		( 0)
#define PERF_CTL_PCI_AHB_1		( 1)
#define PERF_CTL_USB_0			( 2)
#define PERF_CTL_USB_1			( 3)
#define PERF_CTL_GE0_PKT_CNT		( 4)
#define PERF_CTL_GEO_AHB_1		( 5)
#define PERF_CTL_GE1_PKT_CNT		( 6)
#define PERF_CTL_GE1_AHB_1		( 7)
#define PERF_CTL_PCI_DEV_0_BUSY		( 8)
#define PERF_CTL_PCI_DEV_1_BUSY		( 9)
#define PERF_CTL_PCI_DEV_2_BUSY		(10)
#define PERF_CTL_PCI_HOST_BUSY		(11)
#define PERF_CTL_PCI_DEV_0_ARB		(12)
#define PERF_CTL_PCI_DEV_1_ARB		(13)
#define PERF_CTL_PCI_DEV_2_ARB		(14)
#define PERF_CTL_PCI_HOST_ARB		(15)
#define PERF_CTL_PCI_DEV_0_ACTIVE	(16)
#define PERF_CTL_PCI_DEV_1_ACTIVE	(17)
#define PERF_CTL_PCI_DEV_2_ACTIVE	(18)
#define PERF_CTL_HOST_ACTIVE		(19)

#define ar9331_perf0_ctl(_val) ar9331_reg_wr(AR7240_PERF_CTL, (_val))
#define ar9331_perf1_ctl(_val) ar9331_reg_rmw_set(AR7240_PERF_CTL, ((_val) << 8))


/* These are values used in platform.inc to select PLL settings */

#define AR7240_REV_ID			(AR7240_RESET_BASE + 0x90)
#define AR7240_REV_ID_MASK		0xffff
#define AR7240_REV_ID_AR7130		0xa0
#define AR7240_REV_ID_AR7141		0xa1
#define AR7240_REV_ID_AR7161		0xa2
#define AR7240_REV_1_0			0xc0
#define AR7240_REV_1_1			0xc1
#define AR7240_REV_1_2			0xc2
#define AR7241_REV_1_0			0x0100
#define AR7242_REV_1_0			0x1100
#define AR7241_REV_1_1			0x0101
#define AR7242_REV_1_1			0x1101

#define AR9330_REV_1_0			0x0110
#define AR9331_REV_1_0			0x1110
#define AR9330_REV_1_1			0x0111
#define AR9331_REV_1_1			0x1111
#define AR9330_REV_1_2			0x0112
#define AR9331_REV_1_2			0x1112

#define AR9344_REV_1_0			0x2120
#define AR9342_REV_1_0			0x1120
#define AR9341_REV_1_0			0x0120

#undef is_ar9331
#undef is_ar7241
#undef is_ar7242

#define is_ar9331()	(((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR7240_REV_1_2) || \
			 ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR7240_REV_1_1) || \
			 ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR7240_REV_1_0))

#define is_ar7241()	(((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR7241_REV_1_0) || \
			 ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR7241_REV_1_1))

#define is_ar7242()	(((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR7242_REV_1_0) || \
			 ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR7242_REV_1_1))
#undef is_ar933x
#undef is_ar9331
#undef is_ar9330

#define is_ar9330() (((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9330_REV_1_0) || \
                        ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9330_REV_1_1) || \
                        ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9330_REV_1_2))

#define is_ar9331() (((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9331_REV_1_0) || \
                        ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9331_REV_1_1) || \
                        ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9331_REV_1_2))

#define is_ar933x_10()  (((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9330_REV_1_0) || \
                            ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9331_REV_1_0))

#define is_ar933x_11()  (((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9330_REV_1_1) || \
                            ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9331_REV_1_1))
   
#define is_ar933x_12()  (((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9330_REV_1_2) || \
                            ((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9331_REV_1_2))
                        
#define is_ar933x() (is_ar9330() || is_ar9331())

#define is_ar9344()	((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9344_REV_1_0)
#define is_ar9342()	((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9342_REV_1_0)
#define is_ar9341()	((ar9331_reg_rd(AR7240_REV_ID) & AR7240_REV_ID_MASK) == AR9341_REV_1_0)

#define is_ar934x()	(is_ar9344() || is_ar9342() || is_ar9341())

#ifdef CONFIG_AR7240_EMULATION
#define is_emu()	(1)
#else
#define is_emu()	(0)
#endif

#define AR7240_PLL_USE_REV_ID		0
#define AR7240_PLL_200_200_100		1
#define AR7240_PLL_300_300_150		2
#define AR7240_PLL_333_333_166		3
#define AR7240_PLL_266_266_133		4
#define AR7240_PLL_266_266_66		5
#define AR7240_PLL_400_400_200		6
#define AR7240_PLL_600_400_150		7


/*
 * AR7240_RESET bit defines
 */
#define AR7240_RESET_SLIC			(1 << 30)
#define AR7240_RESET_EXTERNAL			(1 << 28)
#define AR7240_RESET_FULL_CHIP			(1 << 24)
#define AR7240_RESET_GE0_MDIO			(1 << 22)
#define AR7240_RESET_CPU_NMI			(1 << 21)
#define AR7240_RESET_CPU_COLD_RESET_MASK	(1 << 20)
#define AR7240_RESET_DMA			(1 << 19)
#define AR7240_RESET_STEREO			(1 << 17)
#define AR7240_RESET_DDR			(1 << 16)
#define AR7240_RESET_GE1_MAC			(1 << 13)
#define AR7240_RESET_GE1_PHY			(1 << 12)
#ifdef CONFIG_MACH_HORNET
#define AR7240_RESET_WMAC			(1 << 11)
#else
#define AR7240_RESET_USB_PHY_ANALOG		(1 << 11)
#endif
#define AR7240_RESET_PCIE_PHY_SHIFT		(1 << 10)
#define AR7240_RESET_GE0_MAC			(1 << 9)
#define AR7240_RESET_GE0_PHY			(1 << 8)
#define AR7240_RESET_USBSUS_OVRIDE		(1 << 3)
#define AR7240_RESET_USB_OHCI_DLL		(1 << 3)
#define AR7240_RESET_USB_HOST			(1 << 5)
#define AR7240_RESET_USB_PHY			(1 << 4)
#define AR7240_RESET_PCI_BUS			(1 << 1)
#define AR7240_RESET_PCI_CORE			(1 << 0)
#define AR7240_RESET_I2S			(1 << 0)

#ifdef CONFIG_WASP_SUPPORT
#define is_wasp()	is_ar934x()
#define ATH_WMAC_BASE	AR7240_APB_BASE + 0x100000
#define ATH_WMAC_LEN		0x1ffff /* XXX:Check this */
#else
#define is_wasp()		(0)
#endif

#ifdef CONFIG_MACH_HORNET
#define ATH_WMAC_BASE	AR7240_APB_BASE + 0x100000
#define ATH_WMAC_LEN		0x1ffff /* XXX:Check this */
#endif

void ar9331_reset(unsigned int mask);

/*
 * Mii block
 */
#define AR7240_MII0_CTRL		0x18070000
#define AR7240_MII1_CTRL		0x18070004

//#define BIT(_x) (1 << (_x))

#define ar9331_get_bit(_reg, _bit)	(ar9331_reg_rd((_reg)) & (1 << (_bit)))

#define ar9331_flush_ge(_unit) do { \
	u32 reg = (_unit) ? AR7240_DDR_GE1_FLUSH : AR7240_DDR_GE0_FLUSH; \
	ar9331_reg_wr(reg, 1); \
	while((ar9331_reg_rd(reg) & 0x1)); \
	ar9331_reg_wr(reg, 1); \
	while((ar9331_reg_rd(reg) & 0x1)); \
} while(0)

#define ar9331_flush_pcie() do { \
	ar9331_reg_wr(AR7240_DDR_PCIE_FLUSH, 1); \
	while((ar9331_reg_rd(AR7240_DDR_PCIE_FLUSH) & 0x1)); \
	ar9331_reg_wr(AR7240_DDR_PCIE_FLUSH, 1); \
	while((ar9331_reg_rd(AR7240_DDR_PCIE_FLUSH) & 0x1)); \
} while(0)

#define ar9331_flush_USB() do { \
	ar9331_reg_wr(AR7240_DDR_USB_FLUSH, 1); \
	while((ar9331_reg_rd(AR7240_DDR_USB_FLUSH) & 0x1)); \
	ar9331_reg_wr(AR7240_DDR_USB_FLUSH, 1); \
	while((ar9331_reg_rd(AR7240_DDR_USB_FLUSH) & 0x1)); \
} while(0)

int ar9331_local_read_config(int where, int size, u32 *value);
int ar9331_local_write_config(int where, int size, u32 value);
int ar9331_check_error(int verbose);
unsigned char __ar9331_readb(const volatile void __iomem *p);
unsigned short __ar9331_readw(const volatile void __iomem *p);
void ap_usb_led_on(void);
void ap_usb_led_off(void);
#endif
