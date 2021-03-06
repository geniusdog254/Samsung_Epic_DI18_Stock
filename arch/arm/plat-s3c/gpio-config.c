/* linux/arch/arm/plat-s3c/gpio-config.c
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * S3C series GPIO configuration core
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/io.h>

#include <mach/gpio-core.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-cfg-helpers.h>

int s3c_gpio_cfgpin(unsigned int pin, unsigned int config)
{
	struct s3c_gpio_chip *chip = s3c_gpiolib_getchip(pin);
	unsigned long flags;
	int offset;
	int ret;

	if (!chip)
		return -EINVAL;

	offset = pin - chip->chip.base;

	local_irq_save(flags);
	ret = s3c_gpio_do_setcfg(chip, offset, config);
	local_irq_restore(flags);

	return ret;
}

int s3c_gpio_setpull(unsigned int pin, s3c_gpio_pull_t pull)
{
	struct s3c_gpio_chip *chip = s3c_gpiolib_getchip(pin);
	unsigned long flags;
	int offset, ret;

	if (!chip)
		return -EINVAL;

	offset = pin - chip->chip.base;

	local_irq_save(flags);
	ret = s3c_gpio_do_setpull(chip, offset, pull);
	local_irq_restore(flags);

	return ret;
}

int s3c_gpio_setpin(unsigned int pin, s3c_gpio_pull_t level)
{
	struct s3c_gpio_chip *chip = s3c_gpiolib_getchip(pin);
	unsigned long flags;
	int offset, ret;

	if (!chip)
		return -EINVAL;

	offset = pin - chip->chip.base;

	local_irq_save(flags);
	ret = s3c_gpio_do_setpin(chip, offset, level);
	local_irq_restore(flags);

	return ret;
}

EXPORT_SYMBOL(s3c_gpio_cfgpin);
EXPORT_SYMBOL(s3c_gpio_setpull);
EXPORT_SYMBOL(s3c_gpio_setpin);

#ifdef CONFIG_S3C_GPIO_CFG_S3C24XX
int s3c_gpio_setcfg_s3c24xx_banka(struct s3c_gpio_chip *chip,
				  unsigned int off, unsigned int cfg)
{
	void __iomem *reg = chip->base;
	unsigned int shift = off;
	u32 con;

	if (s3c_gpio_is_cfg_special(cfg)) {
		cfg &= 0xf;

		/* Map output to 0, and SFN2 to 1 */
		cfg -= 1;
		if (cfg > 1)
			return -EINVAL;

		cfg <<= shift;
	}

	con = __raw_readl(reg);
	con &= ~(0x1 << shift);
	con |= cfg;
	__raw_writel(con, reg);

	return 0;
}

int s3c_gpio_setcfg_s3c24xx(struct s3c_gpio_chip *chip,
			    unsigned int off, unsigned int cfg)
{
	void __iomem *reg = chip->base;
	unsigned int shift = off * 2;
	u32 con;

	if (s3c_gpio_is_cfg_special(cfg)) {
		cfg &= 0xf;
		if (cfg > 3)
			return -EINVAL;

		cfg <<= shift;
	}

	con = __raw_readl(reg);
	con &= ~(0x3 << shift);
	con |= cfg;
	__raw_writel(con, reg);

	return 0;
}
#endif

#ifdef CONFIG_S3C_GPIO_CFG_S3C64XX
int s3c_gpio_setcfg_s3c64xx_4bit(struct s3c_gpio_chip *chip,
				 unsigned int off, unsigned int cfg)
{
	void __iomem *reg = chip->base;
	unsigned int shift = (off & 7) * 4;
	u32 con;

	if (off < 8 && chip->chip.ngpio >= 8)
		reg -= 4;

	if (s3c_gpio_is_cfg_special(cfg)) {
		cfg &= 0xf;
		cfg <<= shift;
	}

	con = __raw_readl(reg);
	con &= ~(0xf << shift);
	con |= cfg;
	__raw_writel(con, reg);

	return 0;
}
#endif /* CONFIG_S3C_GPIO_CFG_S3C64XX */

#ifdef CONFIG_S3C_GPIO_CFG_S5PC1XX
#if defined(CONFIG_PLAT_S5PC1XX)
int s3c_gpio_setcfg_s5pc1xx(struct s3c_gpio_chip *chip,
				 unsigned int off, unsigned int cfg)
{
	void __iomem *reg = chip->base;
	unsigned int shift = (off & 7) * 4;
	u32 con;

	if (s3c_gpio_is_cfg_special(cfg)) {
		cfg &= 0xf;
		cfg <<= shift;
	}

	con = __raw_readl(reg);
	con &= ~(0xf << shift);
	con |= cfg;
	__raw_writel(con, reg);

	return 0;
}
#elif defined(CONFIG_PLAT_S5PC11X)
int s3c_gpio_setcfg_s5pc11x(struct s3c_gpio_chip *chip,
				 unsigned int off, unsigned int cfg)
{
	void __iomem *reg = chip->base;
	unsigned int shift = (off & 7) * 4;
	u32 con;

	if (s3c_gpio_is_cfg_special(cfg)) {
		cfg &= 0xf;
		cfg <<= shift;
	}

#if CONFIG_ARIES_VER_B1 //UART_CHANGE
    if((cfg&0xfffffff0) == 0) {
		cfg &= 0xf;
		cfg <<= shift;
    }	
#endif		
	

	con = __raw_readl(reg);
	con &= ~(0xf << shift);
	con |= cfg;
	__raw_writel(con, reg);
#ifdef  S5PC11X_ALIVEGPIO_STORE
	con = __raw_readl(reg);
#endif

	return 0;
}
#endif /* CONFIG_PLAT_S5PC1XX */
#endif /* CONFIG_S3C_GPIO_CFG_S5PC1XX */

#ifdef CONFIG_S3C_GPIO_CFG_S5P64XX
int s3c_gpio_setcfg_s5p64xx(struct s3c_gpio_chip *chip,
				 unsigned int off, unsigned int cfg)
{
	void __iomem *reg = chip->base;
	unsigned int shift = (off & 7) * 4;
	u32 con;

	if (s3c_gpio_is_cfg_special(cfg)) {
		cfg &= 0xf;
		cfg <<= shift;
	}

	con = __raw_readl(reg);
	con &= ~(0xf << shift);
	con |= cfg;
	__raw_writel(con, reg);

	return 0;
}
#endif	/* CONFIG_S3C_GPIO_CFG_S5P64XX */

#ifdef CONFIG_S3C_GPIO_PULL_UPDOWN
int s3c_gpio_setpull_updown(struct s3c_gpio_chip *chip,
			    unsigned int off, s3c_gpio_pull_t pull)
{
	void __iomem *reg = chip->base + 0x08;
	int shift = off * 2;
	u32 pup;

	pup = __raw_readl(reg);
	pup &= ~(3 << shift);
	pup |= (pull << shift);
	__raw_writel(pup, reg);
#ifdef  S5PC11X_ALIVEGPIO_STORE
	pup = __raw_readl(reg);
#endif

	return 0;
}

s3c_gpio_pull_t s3c_gpio_getpull_updown(struct s3c_gpio_chip *chip,
					unsigned int off)
{
	void __iomem *reg = chip->base + 0x08;
	int shift = off * 2;
	u32 pup = __raw_readl(reg);

	pup >>= shift;
	pup &= 0x3;
	return (__force s3c_gpio_pull_t)pup;
}

int s3c_gpio_setpin_updown(struct s3c_gpio_chip *chip,
			    unsigned int off, s3c_gpio_pull_t level)
{
	void __iomem *reg = chip->base + 0x04;
	unsigned int baseaddr = (unsigned int)chip->base;
	u32 lvl;
#if defined CONFIG_CPU_S3C6410
	if(((baseaddr & 0xFFF) == 0x0E0) /* If it is GPH */
		|| ((baseaddr & 0xFFF) == 0x800) /* If it is GPK */
		|| ((baseaddr & 0xFFF) == 0x810)) /* If it is GPL */
	   reg += 0x4;
#elif defined CONFIG_CPU_S5P6440
	if(((baseaddr & 0xFFF) == 0x0E0) /* If it is GPH */
		|| ((baseaddr & 0xFFF) == 0x290)) /* If it is GPR */
	   reg += 0x4;
#elif defined CONFIG_CPU_S5PC100
#endif
	lvl = __raw_readl(reg);
	lvl &= ~(1 << off);
	lvl |= (level << off);
	__raw_writel(lvl, reg);
#ifdef  S5PC11X_ALIVEGPIO_STORE
	lvl = __raw_readl(reg);
#endif
	return 0;
}

#endif
