/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

_flash_start = 0x0;
_flash_length = 0xff000;

__ContainerCodeStart  = 0x20000000;/*0xff000;*/
__ContainerLength = 0x1000;
__ContainerDataLength = __ContainerLength / 2;
__ContainerCodeLength = __ContainerLength / 2;
__ContainerDataStart = __ContainerCodeStart + __ContainerCodeLength;

/*PROVIDE(__ContainerStart = ABSOLUTE(0xff128));*/
/*PROVIDE(__ContainerLength = 0x200);*/

MEMORY
{
  FLASH (rx)           : ORIGIN = _flash_start,         LENGTH = _flash_length
  CONTAINER_CODE (rwx) : ORIGIN = __ContainerCodeStart, LENGTH = __ContainerCodeLength
  CONTAINER_DATA (rwx) : ORIGIN = __ContainerDataStart, LENGTH = __ContainerDataLength
  RAM (rwx)            : ORIGIN = 0x20000000 + __ContainerLength,           LENGTH = 0x40000  - __ContainerLength
}


FLASH_PAGE_SIZE       = 4096;
FLASH_DATA_PAGES_USED = 4;

SECTIONS
{
  /* TODO: set the align properly */
  .container-code :
  {
    /* KEEP(container.o(*)); */
    KEEP   (*(.container-entry));
    KEEP   (*(.container-code));
  } > CONTAINER_CODE

  .container-data :
  {
    KEEP   (*(.container-data));
  } > CONTAINER_DATA
}

SECTIONS
{
  . = ALIGN(4);
  .svc_data :
  {
    PROVIDE(__start_svc_data = .);
    KEEP(*(.svc_data))
    PROVIDE(__stop_svc_data = .);
  } > RAM
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
  .log_dynamic_data :
  {
    PROVIDE(__start_log_dynamic_data = .);
    KEEP(*(SORT(.log_dynamic_data*)))
    PROVIDE(__stop_log_dynamic_data = .);
  } > RAM
  .cli_sorted_cmd_ptrs :
  {
    PROVIDE(__start_cli_sorted_cmd_ptrs = .);
    KEEP(*(.cli_sorted_cmd_ptrs))
    PROVIDE(__stop_cli_sorted_cmd_ptrs = .);
  } > RAM
} INSERT AFTER .data;

SECTIONS
{
  .pwr_mgmt_data :
  {
    PROVIDE(__start_pwr_mgmt_data = .);
    KEEP(*(SORT(.pwr_mgmt_data*)))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > FLASH
  .log_const_data :
  {
    PROVIDE(__start_log_const_data = .);
    KEEP(*(SORT(.log_const_data*)))
    PROVIDE(__stop_log_const_data = .);
  } > FLASH
  .nrf_balloc :
  {
    PROVIDE(__start_nrf_balloc = .);
    KEEP(*(.nrf_balloc))
    PROVIDE(__stop_nrf_balloc = .);
  } > FLASH
  .nrf_queue :
  {
    PROVIDE(__start_nrf_queue = .);
    KEEP(*(.nrf_queue))
    PROVIDE(__stop_nrf_queue = .);
  } > FLASH
  .cli_command :
  {
    PROVIDE(__start_cli_command = .);
    KEEP(*(.cli_command))
    PROVIDE(__stop_cli_command = .);
  } > FLASH
  .sdh_stack_observers :
  {
    PROVIDE(__start_sdh_stack_observers = .);
    KEEP(*(SORT(.sdh_stack_observers*)))
    PROVIDE(__stop_sdh_stack_observers = .);
  } > FLASH
  .sdh_req_observers :
  {
    PROVIDE(__start_sdh_req_observers = .);
    KEEP(*(SORT(.sdh_req_observers*)))
    PROVIDE(__stop_sdh_req_observers = .);
  } > FLASH
  .sdh_state_observers :
  {
    PROVIDE(__start_sdh_state_observers = .);
    KEEP(*(SORT(.sdh_state_observers*)))
    PROVIDE(__stop_sdh_state_observers = .);
  } > FLASH
  .sdh_ant_observers :
  {
    PROVIDE(__start_sdh_ant_observers = .);
    KEEP(*(SORT(.sdh_ant_observers*)))
    PROVIDE(__stop_sdh_ant_observers = .);
  } > FLASH
  .sdh_ble_observers :
  {
    PROVIDE(__start_sdh_ble_observers = .);
    KEEP(*(SORT(.sdh_ble_observers*)))
    PROVIDE(__stop_sdh_ble_observers = .);
  } > FLASH
  .sdh_soc_observers :
  {
    PROVIDE(__start_sdh_soc_observers = .);
    KEEP(*(SORT(.sdh_soc_observers*)))
    PROVIDE(__stop_sdh_soc_observers = .);
  } > FLASH
  .crypto_data :
  {
    PROVIDE(__start_crypto_data = .);
    KEEP(*(SORT(.crypto_data*)))
    PROVIDE(__stop_crypto_data = .);
  } > FLASH
} INSERT AFTER .text

SECTIONS
{
    __stop_ot_flash_data = (ORIGIN(FLASH) + LENGTH(FLASH) - FLASH_PAGE_SIZE);
    __start_ot_flash_data = (__stop_ot_flash_data - (FLASH_PAGE_SIZE * FLASH_DATA_PAGES_USED));

    /* Assure that code does not overlap flash data area.*/
    ASSERT((__start_ot_flash_data >= __etext + SIZEOF(.data)), "Error: Code overlaps flash data area.")
}

INCLUDE "nrf_common.ld"
