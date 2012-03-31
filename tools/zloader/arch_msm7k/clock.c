/*
 * Copyright (c) 2011, Triple Oxygen - OpenZeebo Project
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <boot/boot.h>
#include <msm7k/gpt.h>

static inline unsigned rd_cycle_count(void) 
{
    unsigned cc;
    asm volatile (
        "mrc p15, 0, %0, c15, c12, 1\n" :
        "=r" (cc) 
        );   
    return cc;
}

static inline unsigned rd_dtcm(void) 
{
    unsigned cc;
    asm volatile (
        "mrc p15, 0, %0, c9, c1, 0\n" :
        "=r" (cc) 
        );   
    return cc;
}

static inline unsigned rd_itcm(void) 
{
    unsigned cc;
    asm volatile (
        "mrc p15, 0, %0, c9, c1, 1\n" :
        "=r" (cc) 
        );   
    return cc;
}

static inline void perf_enable(void)
{
    asm volatile (
        "mcr p15, 0, %0, c15, c12, 0\n" : : "r" (7) 
        );
}

static inline void perf_disable(void)
{
    asm volatile (
        "mcr p15, 0, %0, c15, c12, 0\n" : : "r" (0)
        );
}

unsigned cycles_per_second(void)
{
    unsigned T0, T1;

    perf_enable();
    
    writel(0, GPT_CLEAR);
    writel(0, GPT_ENABLE);
    while(readl(GPT_COUNT_VAL) != 0) ;
    
    writel(GPT_ENABLE_EN, GPT_ENABLE);    
    T0 = rd_cycle_count();
    while(readl(GPT_COUNT_VAL) < 32766) ;
    T1 = rd_cycle_count();

    writel(0, GPT_ENABLE);
    writel(0, GPT_CLEAR);

    perf_disable();
    
    return T1-T0;
}

void mdelay(unsigned msecs)
{
    msecs *= 33;
    
    writel(0, GPT_CLEAR);
    writel(0, GPT_ENABLE);
    while(readl(GPT_COUNT_VAL) != 0) ;
    
    writel(GPT_ENABLE_EN, GPT_ENABLE);    
    while(readl(GPT_COUNT_VAL) < msecs) ;

    writel(0, GPT_ENABLE);
    writel(0, GPT_CLEAR);    
}

void udelay(unsigned usecs)
{
    usecs = (usecs * 33 + 1000 - 33) / 1000;
    
    writel(0, GPT_CLEAR);
    writel(0, GPT_ENABLE);
    while(readl(GPT_COUNT_VAL) != 0) ;
    
    writel(GPT_ENABLE_EN, GPT_ENABLE);    
    while(readl(GPT_COUNT_VAL) < usecs) ;

    writel(0, GPT_ENABLE);
    writel(0, GPT_CLEAR);    
}

void nopdelay(void)
{
    unsigned int i = 20;
    
    while(i--)
        asm volatile ("nop");
}

void print_cpu_speed(void)
{
    unsigned cps = cycles_per_second();
    dprintf("1 second = %d cycles\n%d MHz\n", cps, cps / 1000000);
}

#define A11S_CLK_CNTL 0xC0100100
#define A11S_CLK_SEL  0xC0100104
#define MSM_CLK_CTL_BASE    0xA8600000

#define C A11S_CLK_CNTL
#define S A11S_CLK_SEL

#if FROM_APPSBOOT_MBN
static unsigned tbl_old[] = {
    C, 0x640000,
    S, 2,
    C, 0x640010,
    C, 0x64001F,
    S, 3,
    C, 0x64101F,
    C, 0x64171F,
    S, 2,
    C, 0x64171F,
    C, 0x641715,
    S, 3,
    S, 5,
    C, 0x641715,
    C, 0x641315,
    S, 4,
    S, 6,
    C, 0x641315,
    C, 0x641312,
    S, 7,
    C, 0x641312,
    C, 0x641112,
    S, 6,
    0
};
#endif

static unsigned tbl[] = {
#if EXPLORE
    C, 0x640000, S, 2,
    C, 0x640001, S, 3,
    C, 0x640201, S, 2,        
    C, 0x640203, S, 3,
    C, 0x640403, S, 2,
    C, 0x640405, S, 3,
    C, 0x640605, S, 2,
    C, 0x640607, S, 3,
    C, 0x640807, S, 2,
    C, 0x640809, S, 3,
    C, 0x640A09, S, 2,
    C, 0x640A0B, S, 3,
    C, 0x640C0B, S, 2,
    C, 0x640C0D, S, 3,
    C, 0x640E0D, S, 2,
    C, 0x640E0F, S, 3,
#endif
    C, 0x640000, S, 2,
    C, 0x64001F, S, 3,
    C, 0x64171F, S, 2,
    C, 0x641715, S, 5,
    C, 0x641315, S, 6,
    C, 0x641312, S, 7,
    C, 0x641112, S, 6,
    C, 0x642221, S, 7,
    0
};

#undef C
#undef S

#if 1
unsigned cc_div[16] = { 
	1, 2, 3, 4,  5, 8, 6, 16,  
	1, 1, 1, 1,  1, 1, 1, 32  /* guess */ 
};

unsigned cc_base[4] = {
	19200000,
    245760000,
    800000000,
    0
};

unsigned cs_div[4] = {
	1, 2, 3, 4
};

void info(unsigned c, unsigned s)
{
    unsigned src_sel, src_div;

    if(s & 1) {
            /* src1 selected */
        src_sel = (c >> 4) & 0x7;
        src_div = c & 0xF;
    } else {
            /* src0 selected */
        src_sel = (c >> 12) & 0x7;
        src_div = (c >> 8) & 0xF;
    }
    
    unsigned src = s & 1;
    unsigned pdiv = cs_div[(s >> 1) & 3];
    unsigned div = cc_div[src_div];
    unsigned clk = cc_base[src_sel] / div;
    unsigned pclk = clk / pdiv;

    unsigned cps = cycles_per_second();

    dprintf("CC=0x%x CS=0x%x SRC=%d PDIV=%d SEL=%d DIV=%d CPS=%d ACLK=%d\n",
            c, s, src, pdiv, src_sel, div, cps, clk);
}


void arm11_clock_test(void) 
{
    unsigned c, s;
    unsigned *x = tbl;

    while(*x) {
        unsigned *ptr = (unsigned*) *x++;
        unsigned val = *x++;
        *ptr = val;

        if(ptr == ((unsigned*) A11S_CLK_CNTL)) {
            c = val;
        } else {
            s = val;
            info(c, s);
        }
    }
}
#endif

void arm11_clock_init(void) 
{
    unsigned *x = tbl;
    while(*x) {
        unsigned *ptr = (unsigned*) *x++;
        unsigned val = *x++;
        *ptr = val;
    }
}

void pll_init(void)
{
    //# PLL0
	//mww phys 0xa8600300 0x7
	//mww phys 0xa8600304 0xc
	//mww phys 0xa8600308 0x4
	//mww phys 0xa860030c 0x5

	//# PLL1
	//mww phys 0xa860031c 0x7
	//mww phys 0xa8600320 0x28
	//mww phys 0xa8600324 0x0
	//mww phys 0xa8600328 0x1

	//# PLL2
	//mww phys 0xa8600338 0x7
	//mww phys 0xa860033c 0x37
	//mww phys 0xa8600340 0x0
	//mww phys 0xa8600344 0x1

	//# PLL3
	//mww phys 0xa8600354 0x0
	//mww phys 0xa8600358 0x2d
	//mww phys 0xa860035c 0x0
	//mww phys 0xa8600360 0x1
    
    // PLL0
    writel(7    , MSM_CLK_CTL_BASE + 0x300);
    writel(0xC  , MSM_CLK_CTL_BASE + 0x304);
    writel(4    , MSM_CLK_CTL_BASE + 0x308);
    writel(5    , MSM_CLK_CTL_BASE + 0x30C);
    
    // PLL1
    writel(7    , MSM_CLK_CTL_BASE + 0x31C);
    writel(0x28 , MSM_CLK_CTL_BASE + 0x320);
    writel(0    , MSM_CLK_CTL_BASE + 0x324);
    writel(1    , MSM_CLK_CTL_BASE + 0x328);
    
    // PLL2
    writel(7    , MSM_CLK_CTL_BASE + 0x338);
    writel(0x37 , MSM_CLK_CTL_BASE + 0x33C);
    writel(0    , MSM_CLK_CTL_BASE + 0x340);
    writel(1    , MSM_CLK_CTL_BASE + 0x344);
    
    // PLL3
    writel(0    , MSM_CLK_CTL_BASE + 0x354);
    writel(0x2D , MSM_CLK_CTL_BASE + 0x358);
    writel(0    , MSM_CLK_CTL_BASE + 0x35C);
    writel(1    , MSM_CLK_CTL_BASE + 0x360);
}

void uart_clock_init()
{
	writel(0x30 , MSM_CLK_CTL_BASE + 0xe0);
}

void peripheral_clock_init(void) 
{
    // FREE AXI FROM RESET
    writel(0x00000023, MSM_CLK_CTL_BASE + 0x208);
    writel(0x00000001, MSM_CLK_CTL_BASE + 0x210);
    writel(0x00020001, MSM_CLK_CTL_BASE + 0x214);
    
    // UART
    uart_clock_init();
    
    // Dunno...
    writel(0x10480, MSM_CLK_CTL_BASE + 0x314);
       
    // TV_ENC + TV_DAC
    writel(0x00e1f7ff, MSM_CLK_CTL_BASE + 0xC8);
    writel(0xf8e01b44, MSM_CLK_CTL_BASE + 0xCC);
}
