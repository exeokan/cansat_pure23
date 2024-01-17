import tkinter as tk
from tkinter import scrolledtext
import serial
import threading
import time
from queue import Queue  # Import for the Queue class
import queue  # Import for the general queue module

class CollectiveSensorData:
    def __init__(self):
        self.MISSION_ID = ""
        self.MISSION_TIME = ""
        self.PACKET_COUNT = ""
        self.MODE = ""
        self.STATE = ""
        self.ALTITUDE = ""
        self.PC_DEPLOYED = ""
        self.TEMPERATURE = ""
        self.PRESSURE = ""
        self.GPS_TIME = ""
        self.GPS_ALTITUDE = ""
        self.GPS_LATITUDE = ""
        self.GPS_LONGITUDE = ""
        self.GPS_SATS = ""
        self.ACC_X = ""
        self.ACC_Y = ""
        self.ACC_Z = ""
        self.MAG_X = ""
        self.MAG_Y = ""
        self.MAG_Z = ""
        self.CMD_ECHO = ""

def convert_to_struct(concatenated_data):
    data_list = concatenated_data.split(',')

    if len(data_list) != len(vars(CollectiveSensorData())):
        print("Error: Incompatible data length.")
        return None

    sensor_data = CollectiveSensorData()
    sensor_data.MISSION_ID = data_list[0]
    sensor_data.MISSION_TIME = data_list[1]
    sensor_data.PACKET_COUNT = data_list[2]
    sensor_data.MODE = data_list[3]
    sensor_data.STATE = data_list[4]
    sensor_data.ALTITUDE = data_list[5]
    sensor_data.PC_DEPLOYED = data_list[6]
    sensor_data.TEMPERATURE = data_list[7]
    sensor_data.PRESSURE = data_list[8]
    sensor_data.GPS_TIME = data_list[9]
    sensor_data.GPS_ALTITUDE = data_list[10]
    sensor_data.GPS_LATITUDE = data_list[11]
    sensor_data.GPS_LONGITUDE = data_list[12]
    sensor_data.GPS_SATS = data_list[13]
    sensor_data.ACC_X = data_list[14]
    sensor_data.ACC_Y = data_list[15]
    sensor_data.ACC_Z = data_list[16]
    sensor_data.MAG_X = data_list[17]
    sensor_data.MAG_Y = data_list[18]
    sensor_data.MAG_Z = data_list[19]
    sensor_data.CMD_ECHO = data_list[20]
    return sensor_data

def print_sensor_data(sensor_data):
    print("MISSION_ID:", sensor_data.MISSION_ID)
    print("MISSION_TIME:", sensor_data.MISSION_TIME)
    print("PACKET_COUNT:", sensor_data.PACKET_COUNT)
    print("MODE:", sensor_data.MODE)
    print("STATE:", sensor_data.STATE)
    print("ALTITUDE:", sensor_data.ALTITUDE)
    print("PC_DEPLOYED:", sensor_data.PC_DEPLOYED)
    print("TEMPERATURE:", sensor_data.TEMPERATURE)
    print("PRESSURE:", sensor_data.PRESSURE)
    print("GPS_TIME:", sensor_data.GPS_TIME)
    print("GPS_ALTITUDE:", sensor_data.GPS_ALTITUDE)
    print("GPS_LATITUDE:", sensor_data.GPS_LATITUDE)
    print("GPS_LONGITUDE:", sensor_data.GPS_LONGITUDE)
    print("GPS_SATS:", sensor_data.GPS_SATS)
    print("ACC_X:", sensor_data.ACC_X)
    print("ACC_Y:", sensor_data.ACC_Y)
    print("ACC_Z:", sensor_data.ACC_Z)
    print("MAG_X:", sensor_data.MAG_X)
    print("MAG_Y:", sensor_data.MAG_Y)
    print("MAG_Z:", sensor_data.MAG_Z)
    print("CMD_ECHO:", sensor_data.CMD_ECHO)
    print("----------------------------------------\n")

class SerialCommunicationApp:
    def __init__(self, master):
        self.master = master
        self.master.title("ESP32 Serial Communication")
        self.master.geometry("600x400")

        # Serial Port Settings
        self.serial_port = 'COM9'  # Replace with the appropriate serial port on Windows or '/dev/ttyUSB0' on Linux and macOS
        self.baud_rate = 115200

        # Create Serial Port
        self.ser = serial.Serial(self.serial_port, self.baud_rate, timeout=1)

        # GUI Elements
        self.message_entry = tk.Entry(self.master, width=40)
        self.message_entry.grid(row=0, column=0, padx=10, pady=10)

        # buttons
        self.send_button = tk.Button(self.master, text="Send", command=self.send_message)
        self.send_button.grid(row=0, column=1, padx=10, pady=10)

        self.release_button = tk.Button(self.master, text="Start", command=self.send_start_command)
        self.release_button.grid(row=1, column=0, padx=10, pady=10)

        self.release_button = tk.Button(self.master, text="Release", command=self.send_release_command)
        self.release_button.grid(row=1, column=1, padx=10, pady=10)
        
        self.release_button = tk.Button(self.master, text="Set DSC", command=self.send_dsc_command)
        self.release_button.grid(row=2, column=0, padx=10, pady=10)

        self.release_button = tk.Button(self.master, text="Finish", command=self.send_finish_command)
        self.release_button.grid(row=2, column=1, padx=10, pady=10)

        self.received_text = scrolledtext.ScrolledText(self.master, width=80, height=10)
        self.received_text.grid(row=3, column=0, columnspan=3, padx=10, pady=10)
 
        # Event to signal the receiving thread to exit
        self.exit_event = threading.Event()

        # Queue for communication between threads
        self.queue = Queue()

        # Start a thread for receiving data
        self.receive_thread = threading.Thread(target=self.receive_data)
        self.receive_thread.start()

        # Start a thread for processing data from the queue
        self.process_data_thread = threading.Thread(target=self.process_data)
        self.process_data_thread.start()

        # Configure the close button to call the close_serial_port method
        self.master.protocol("WM_DELETE_WINDOW", self.close_serial_port)

    def send_message(self):
        message = self.message_entry.get()
        if message:
            self.ser.write(message.encode())
            self.message_entry.delete(0, tk.END)

    def send_start_command(self):
        self.ser.write(b"STA")
    def send_release_command(self):
        self.ser.write(b"REL")  # to release esp
    def send_dsc_command(self):
        self.ser.write(b"DSC")
    def send_finish_command(self):
        self.ser.write(b"FIN")

    def receive_data(self):
        while not self.exit_event.is_set():
            try:
                received_data = self.ser.readline().decode('utf-8').strip()
                if received_data:
                    self.queue.put(received_data)

            except serial.SerialException as e:
                print(f"Error: {e}")
                break
            time.sleep(0.1)  # Allow some time before checking for new data

    def process_data(self):
        while not self.exit_event.is_set():
            try:
                received_data = self.queue.get(timeout=0.1)
                self.received_text.insert(tk.END, f"{received_data}\n")

                if not received_data.startswith("Bytes"):
                    sensor_data = convert_to_struct(concatenated_data=received_data)
                    if sensor_data is not None:
                        self.update_sensor_data(sensor_data)

                # Scroll to the end after updating
                self.received_text.see(tk.END)

            except queue.Empty:
                pass
            except serial.SerialException as e:
                print(f"Error: {e}")
                break

    def update_sensor_data(self, sensor_data):
        # Update GUI or perform any other actions with sensor data
        print_sensor_data(sensor_data)

    def close_serial_port(self):
        # Set the exit event to signal the receiving thread to exit
        self.exit_event.set()

        # Wait for the receiving thread to finish
        self.receive_thread.join()

        # Wait for the data processing thread to finish
        self.process_data_thread.join()

        # Close the serial port
        self.ser.close()

        # Close the Tkinter application
        self.master.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialCommunicationApp(root)
    root.mainloop()
