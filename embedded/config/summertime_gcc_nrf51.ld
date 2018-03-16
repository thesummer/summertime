/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

  /*
   * 256kB flash
   * S130 version 8.0.0 takes 108kB (0x1B000 bytes) of flash, rest for app.
   */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x0 + 108K, LENGTH = 256K - 108K
  RAM (rwx) :  ORIGIN = 0x20000000 + 0x1fe8, LENGTH = 16K - 0x1fe8
}

SECTIONS
{
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
  .pwr_mgmt_data :
  {
    PROVIDE(__start_pwr_mgmt_data = .);
    KEEP(*(.pwr_mgmt_data))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > RAM
} INSERT AFTER .data;

INCLUDE "nrf5x_common.ld"
