#Startpunkt File 
# main.py
import ble_backend
import gui

if __name__ == "__main__":
    ble_backend.ble_start_thread()
    gui.create_gui()
 