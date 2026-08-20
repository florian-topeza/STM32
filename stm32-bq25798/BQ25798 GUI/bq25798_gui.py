##
#
# @file        bq25798_gui.py
# @author      Florian Topeza
# @brief       GUI to interface a BQ25798 battery charger with a computer serial monitor, through an STM32 microcontroller using
#              the BQ25798 driver for STM32 microcontrollers
# @version     0.1
# @date        2026-05-29
#
# @copyright Copyright (c) 2026 Florian Topeza
#
#
# @attention
#
# This software is licensed under terms that can be found in the LICENSE file
# in the root directory of this software component.
# If no LICENSE file comes with this software, it is provided AS-IS.
#


# Import required libraries
import sys                              # For system arguments and program exit
import serial                           # For serial communication with COM ports
import serial.tools.list_ports          # To list available serial ports
from PyQt6 import QtWidgets, QtCore     # For creating the GUI with PyQt6

## Main window class definition
class MainWindow(QtWidgets.QMainWindow):

    ## The constructor
    #
    # @param self The object pointer.
    # @param port The COM port to use for serial communication
    #
    def __init__(self, port):
        super().__init__()
        self.ser = serial.Serial(port, 115200, timeout=1)
        self.setWindowTitle('BQ25798 Monitor GUI - to use with BQ25798 CLI driver,  by Florian Topeza, 2026.')
        self.resize(800, 600)  # Augmente la taille pour mieux accommoder les éléments

        # Create 4 text areas for ADC, STATUS, CONF, FLAGS
        self.text_adc = QtWidgets.QTextEdit(self)
        self.text_adc.setReadOnly(True)
        self.text_status = QtWidgets.QTextEdit(self)
        self.text_status.setReadOnly(True)
        self.text_conf = QtWidgets.QTextEdit(self)
        self.text_conf.setReadOnly(True)
        self.text_flags = QtWidgets.QTextEdit(self)
        self.text_flags.setReadOnly(True)

        # Create a button to reset the charger
        btn_rst_chg = QtWidgets.QPushButton('RESET CHARGER', self)
        btn_rst_chg.clicked.connect(lambda: self.send_command("RESET CHARGER"))

        # Create command input and send button
        self.command_input = QtWidgets.QLineEdit(self)
        self.command_input.setPlaceholderText("Enter command here...")
        btn_send = QtWidgets.QPushButton('Send', self)
        btn_send.clicked.connect(lambda:self.send_command(self.command_input.text()))

        # Create mode buttons
        btn_enable_backup = QtWidgets.QPushButton('ENABLE BACKUP', self)
        btn_enable_backup.clicked.connect(lambda: self.send_command("ENABLE BACKUP"))

        btn_enable_idle = QtWidgets.QPushButton('ENABLE IDLE', self)
        btn_enable_idle.clicked.connect(lambda: self.send_command("ENABLE IDLE"))

        btn_enable_ship = QtWidgets.QPushButton('ENABLE SHIP', self)
        btn_enable_ship.clicked.connect(lambda: self.send_command("ENABLE SHIP"))

        btn_enable_shutdown = QtWidgets.QPushButton('ENABLE SHUTDOWN', self)
        btn_enable_shutdown.clicked.connect(lambda: self.send_command("ENABLE SHUTDOWN"))

        # Create a grid layout to organize everything
        grid = QtWidgets.QGridLayout()

        # Add labels for each data section
        grid.addWidget(QtWidgets.QLabel("ADC DATA"), 0, 0)
        grid.addWidget(QtWidgets.QLabel("STATUS REGISTERS"), 0, 1)
        grid.addWidget(QtWidgets.QLabel("CHARGING CONFIGURATION"), 2, 0)
        grid.addWidget(QtWidgets.QLabel("FLAG REGISTERS"), 2, 1)

        # Add text areas
        grid.addWidget(self.text_adc, 1, 0)
        grid.addWidget(self.text_status, 1, 1)
        grid.addWidget(self.text_conf, 3, 0)
        grid.addWidget(self.text_flags, 3, 1)

        # Add command section
        grid.addWidget(QtWidgets.QLabel("Command:"), 4, 0, 1, 2)
        grid.addWidget(self.command_input, 5, 0)
        grid.addWidget(btn_send, 5, 1)

        # Add mode buttons
        grid.addWidget(btn_enable_backup, 6, 0)
        grid.addWidget(btn_enable_idle, 6, 1)
        grid.addWidget(btn_enable_ship, 7, 0)
        grid.addWidget(btn_enable_shutdown, 7, 1)

        # Add reset charger button at the bottom
        grid.addWidget(btn_rst_chg, 8, 0, 1, 2)

        # Create a central widget and set the layout
        w = QtWidgets.QWidget(self)
        w.setLayout(grid)
        self.setCentralWidget(w)

        # Timer for refresh
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.refresh)
        self.timer.start(150)

        # Status bar for error messages
        self.status_bar = QtWidgets.QStatusBar(self)
        self.setStatusBar(self.status_bar)

        # Buffer for accumulating data
        self.buffer = ""

        # Send initial commands
        self.read_all()



   ## Send a command via the serial port
   #
   # @param self The object pointer
   # @param cmd The command to be sent
   #
    def _send(self, cmd):
        # Send the command followed by a newline in ASCII
        self.ser.write((cmd.strip() + '\n').encode('ascii'))



    ## Receive data from the serial port
    #
    # @param self The object pointer
    # @param timeout Time duration during data is being read
    #
    # @return ASCII decoded data
    #
    def _recv(self, timeout=1.0):
        data = b''  # Initialize received data as bytes
        start = QtCore.QTime.currentTime()  # Start time of reception

        # Loop to read data during the specified timeout
        while (QtCore.QTime.currentTime().msecsSinceStartOfDay() - start.msecsSinceStartOfDay()) < timeout*1000:
            chunk = self.ser.read(256)  # Read up to 256 bytes
            if chunk:
                data += chunk  # Add read data
            else:
                break  # Stop if no data is read

        # Decode data as ASCII, replacing invalid characters
        return data.decode('ascii', errors='replace')

    ## Send a command to the STM32 via serial port.
    #
    # @param self The object pointer
    # @param command The command to be sent
    #
    def send_command(self, command):
        try:
            self._send(command)
            self.command_input.clear()  # Clear input after sending
        except Exception as e:
            self.statusBar().showMessage(f'Error sending command: {e}', 5000)

    ## Refresh data displayed in the main window
    #
    # @param self The object pointer
    #
    def refresh(self):
        self.timer.stop()  # Stop the timer during reception
        try:
            payload = self._recv(0.4)  # Receive data with a timeout of 0.4 seconds
            if payload:
                # Handle ANSI clear screen sequence
                if "\033[2J\033[H" in payload:
                    self.text_adc.clear()
                    self.text_status.clear()
                    self.text_conf.clear()
                    self.text_flags.clear()
                    payload = payload.replace("\033[2J\033[H", "")

                self.buffer += payload
                self._parse_buffer()
        except Exception as e:
            self.statusBar().showMessage(f'Error: {e}', 5000)
        finally:
            self.timer.start(150)  # Restart the timer

    ## Parse received data
    #
    # @param self The object pointer
    #
    def _parse_buffer(self):
        headers = [
            "---- BQ25798 ADC DATA ----",
            "---- BQ25798 CHARGING CONFIGURATION ----",
            "---- BQ25798 STATUS REGISTERS ----",
            "---- BQ25798 FLAG REGISTERS ----"
        ]
        for header in headers:
            if header in self.buffer:
                start = self.buffer.find(header)
                next_header_pos = len(self.buffer)
                for h in headers:
                    pos = self.buffer.find(h, start + 1)
                    if pos != -1 and pos < next_header_pos:
                        next_header_pos = pos
                block = self.buffer[start:next_header_pos]
                if header == "---- BQ25798 ADC DATA ----":
                    self.text_adc.setPlainText(block.replace(header, "").strip())
                elif header == "---- BQ25798 STATUS REGISTERS ----":
                    self.text_status.setPlainText(block.replace(header, "").strip())
                elif header == "---- BQ25798 CHARGING CONFIGURATION ----":
                    self.text_conf.setPlainText(block.replace(header, "").strip())
                elif header == "---- BQ25798 FLAG REGISTERS ----":
                    self.text_flags.setPlainText(block.replace(header, "").strip())
                self.buffer = self.buffer[next_header_pos:]

    ## Send GET commands at startup of the GUI to receive the charger data
    #
    # @param self The object pointer
    #
    def read_all(self):
        self.timer.stop()
        try:
            self._send('GET ADC')
            QtCore.QThread.msleep(200)
            self._send('GET CONF')
            QtCore.QThread.msleep(200)
            self._send('GET STATUS')
            QtCore.QThread.msleep(200)
            self._send('GET FLAGS')
            QtCore.QThread.msleep(200)
        except Exception as e:
            self.text_adc.setPlainText(f'Error: {e}')
        finally:
            self.timer.start(150)



## Automatically find an available serial port
#
# @return comport (or None if no port available)
#
def find_port():
    ports = list(serial.tools.list_ports.comports())  # List all available serial ports
    # Filter ports whose description contains "USB" or "STMicroelectronics" (e.g., STM32 board)
    candidates = [p.device for p in ports if 'USB' in p.description or 'STMicroelectronics' in p.description]
    if candidates:
        return candidates[0]  # Return the first matching port
    if ports:
        return ports[0].device  # Return the first available port if no candidate is found
    return None  # Return None if no port is available



# Program entry point
if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)  # Create the Qt application
    #port = find_port()  # Find an available serial port

    # if port is None:
    #     # Display an error dialog if no port is found
    #     QtWidgets.QMessageBox.critical(None, 'Error', 'No serial port found')
    #     sys.exit(1)  # Exit the program with an error code

    win = MainWindow('COM9')  # Create the main window with the found port
    win.show()  # Show the window
    sys.exit(app.exec())  # Start the main Qt application loop