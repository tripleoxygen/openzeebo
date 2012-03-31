
#include <boot/boot.h>
#include <boot/uart.h>
#include <boot/tags.h>
#include <boot/flash.h>
#include <boot/board.h>

#define BLOCK_NUM_110
#define BLOCK_NUM_112

#define PAGE_NUM_110
#define PAGE_NUM_112

#define CRC_110
#define CRC_112

#define DEFAULT_CFG0_A  0xa25400c0
#define DEFAULT_CFG0_A  0xaa5400c0
#define DEFAULT_CFG1    0x0004745e

#define NAND_ID         0x5580b1ad

#define PAGE_SIZE           2048
#define PAGES_PER_BLOCK     64
#define CW_SIZE             512
#define CW_PER_PAGE         4

static unsigned char block_buf[(1 << 17)];
static unsigned CFG0, CFG1;

int flash_read_page(unsigned page, void *data)
{
    unsigned cw = CW_PER_PAGE;
    unsigned status;
    int ret = 0;
    
    writel((page << 16), NAND_ADDR0);
    writel(0,            NAND_ADDR1);
    writel(CFG0, NAND_DEV0_CFG0);
    writel(CFG1, NAND_DEV0_CFG1);
    
    writel(NAND_CMD_PAGE_READ_ECC, NAND_FLASH_CMD);
    
    while(cw--) {
        
        writel(1, NAND_EXEC_CMD);
        delay(1);
        status = readl(NAND_FLASH_STATUS);
        
        if(status & (1 << 4) || ((status >> 12) & 3) != cw) {
            ret = -1;
            break;
        }
        
        memcpy(data + (CW_SIZE * cw), NAND_FLASH_BUFFER, CW_SIZE);        
    }
    
    return ret;
}

int flash_read_block(unsigned block, void *data)
{
    unsigned start_page = (block << 6);
    unsigned page;
    int ret = 0;
        
    for(page = 0; page < PAGES_PER_BLOCK; page++) {
        ret = flash_read_page(start_page + page, data + (PAGE_SIZE * page));
        
        if(ret != 0)
            break;
    }
    
    return ret;
}

int flash_erase_block(unsigned block)
{
    unsigned page = (block << 6), status;
    int ret = 0;
    
    writel(page, NAND_ADDR0);
    writel(0,    NAND_ADDR1);
    writel(CFG0 & ~(7 << 6), NAND_DEV0_CFG0);
    
    writel(NAND_CMD_BLOCK_ERASE, NAND_FLASH_CMD);
    
    writel(1, NAND_EXEC_CMD);
    delay(1);
    status = readl(NAND_FLASH_STATUS);
    
    if(status & 0x110 || !(status & 0x80)) {
        ret = -1;
        break;
    }
    
    return ret;
}

void flash_write_page(unsigned page, void *data)
{
    
}

unsigned flash_check_compat(void)
{
        
}
