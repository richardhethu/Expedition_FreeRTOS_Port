Expedition_FreeRTOS_Port
========================
FreeRTOS porting to Expedition Cortex-M3 chip

1. Terminal Interaction: terminal_printf(...) and terminal_Scanf(...)
2. Ethernet Interaction: ethernet_send(...) and ethernet_receive(...)
3. Wireless Interaction: wireless_send(...) and wireless_receive(...)
4. Power sensor reading: read_power(...)
5. 23 on-chip sensor reading: sensor_read(...)


This is the source code of FreeRTOS porting to Expedition 45nm Cortex-M3 chip. The FreeRTOS kernel code is in Source directory and demo application is in Demo/cortex_m3_gcc directory.

To build demo application: $ cd Demo/cortex_m3_gcc/build $ make all

A text file named "system.hex" will be generated. It can then be copied to MBED USB drive be loaded to Expedition chip by MBED.

To remove demo application built before: $ cd Demo/cortex_m3_gcc/build $ make clean
