import dearpygui.dearpygui as dpg
import ble_backend


def create_gui():
    dpg.create_context()

    with dpg.window(label="Control Panel", width=400, height=200):
        dpg.add_button(label="Rotate Motor1",callback= ble_backend.rotate1)
        dpg.add_button(label="Rotate Motor2", callback= ble_backend.rotate2)

    dpg.create_viewport(title="BabyBot Control", width=400, height=200)
    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.start_dearpygui()
    dpg.destroy_context()
