#pragma once

// #define FLASH_SIZE_128K
#define FLASH_SIZE_256K

#ifdef FLASH_SIZE_256K
#define SECTOR_SIZE 2048
#elif
#define SECTOR_SIZE 1024
#endif

u8 flash_buffer[SECTOR_SIZE];

void flash_read(u32 addr, u8 *p_buffer, u32 num_read)
{
    for (u32 i = 0; i < num_read; i++) {
        p_buffer[i] = *(u8 *)(addr);
        addr += 1;
    }
}

void flash_write_processer(u32 addr, u8 *p_buffer, u32 num_write)
{
    for (u16 i = 0; i < num_write; i++) {
        flash_byte_program(addr, p_buffer[i]);
        addr += 1;
    }

}

void flash_write(u32 addr, u8 *p_buffer, u32 num_write)
{
    u32 offset_addr, sector_pos;
    u16 sector_offset, sector_remain, i;
    flash_unlock();
    offset_addr = addr;
    addr = FLASH_BASE + addr;
    sector_pos = offset_addr / SECTOR_SIZE;
    sector_offset = offset_addr % SECTOR_SIZE;
    sector_remain = SECTOR_SIZE - sector_offset;
    if (num_write <= sector_remain) sector_remain = num_write;
    while (1) {
        flash_read(sector_pos * SECTOR_SIZE + FLASH_BASE, flash_buffer, SECTOR_SIZE);
        for (i = 0; i < sector_remain; i++)
            if (flash_buffer[sector_offset + i] != 0xFF) break;
        if (i < sector_remain) {
            flash_sector_erase(sector_pos * SECTOR_SIZE + FLASH_BASE);
            for (i = 0; i < sector_remain; i++)
                flash_buffer[i + sector_offset] = p_buffer[i];
            flash_write_processer(sector_pos * SECTOR_SIZE + FLASH_BASE, flash_buffer, SECTOR_SIZE);
        } else {
            flash_write_processer(addr, p_buffer, sector_remain);
        }
        if (num_write == sector_remain)
            break;
        else {
            sector_pos++;
            sector_offset = 0;
            p_buffer += sector_remain;
            addr += sector_remain;
            num_write -= sector_remain;
            if (num_write > SECTOR_SIZE)
                sector_remain = SECTOR_SIZE;
            else
                sector_remain = num_write;
        }
    }

    flash_lock();
}

u8 get_address_value()
{
    return 0;
}