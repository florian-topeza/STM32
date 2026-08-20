# BQ25798 GUI for BQ25798 CLI driver, by Florian Topeza

## How to use the serial GUI for BQ25798 (Python)

The given Python scripts work without any further modification.

### bq25798_gui.py (user interface)
```bash
python bq25798_gui.py
```

**Important**: The script automatically detects the COM Port of the ST-LINK V3 (STM32 debugger).
Verify that the COM Port is visible in the device manager (for instance `COM3`, `COM4`).

## Troubleshooting

### The COM Port does not show up
- Check thatt the STM32 is properly connected to the debugger, and that the debugger is properly connected to the PC through USB
- Update the ST-Link firmware if necessary
- Check the USART/UART port used in the firmare of the STM32 to connect it to the debugger

### No data received
- Check the firmware of the STM32 (UART/USART port used for instance)
- Verify that the Python baud rate (115200) is the one used on the STM32.


## Available commands

Through the serial command line interface, the following commands can be sent :

```

GET ADC (to display ADC data)
GET CONF (to display charge configuration)
GET STATUS (to display charger status)
GET FLAGS (to display charge interrupt flags)

CLEAR ADC (to stop displaying ADC data)
CLEAR CONF (to stop displaying charge configuration)
CLEAR STATUS (to stop displaying charger status)
CLEAR FLAGS (to stop displaying charge interrupt flags)
CLEAR ALL (to not display any data)

SET VREG <value_mV>
SET VSYSMIN <value_mV>
SET VOTG <value_mV>
SET VBATLOWV <hex> (enter decimal value to set the VBAT_LOWV threshold, for instance 01 for 62,2% of VREG)
SET ICHG <value_mA>
SET IPCHG <value_mA>
SET WATCHDOG <hex> (enter decimal value to set the watchdog timer, for instance 01 for 0,5s)

ENABLE BACKUP (to enable backup mode)
ENABLE IDLE (to switch back to idle mode)
ENABLE SHIP (to enter ship mode)
ENABLE SHUTDOWN (to enter shutdown mode)

RESET CHARGER (to reset charger to default configuration)
RESET CLI (to reset command line interface configuration)

Enter EXIT to quit help menu.

```