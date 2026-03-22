import dearpygui.dearpygui as dpg
import ble_backend


def create_gui():
    dpg.create_context()

    with dpg.window(label="Control Panel", width=1000, height=800):

        with dpg.table(header_row = False): 
            dpg.add_table_column(); 
            dpg.add_table_column(); 
            dpg.add_table_column(); 

            with dpg.table_row(): 

                with dpg.group():
                        dpg.add_text("Heat")
                        dpg.add_button(label="ON", callback = ble_backend.heat_on)
                        dpg.add_button(label= "OFF", callback = ble_backend.heat_off)

                with dpg.group():
                        dpg.add_text("Audio")
                        dpg.add_button(label="Baby crying", callback = ble_backend.crying)
                        dpg.add_button(label="Baby brabbeln", callback = ble_backend.brabbeln)
                        dpg.add_button(label="Baby coughing", callback = ble_backend.coughing)
                        dpg.add_button(label="Baby sneezing", callback = ble_backend.sneezing)

                with dpg.group():
                        dpg.add_text("Movement")
                        dpg.add_button(label="[TW] Twitching ",callback= ble_backend.tw)
                        dpg.add_button(label="[GM] both legs", callback= ble_backend.gm)
                        dpg.add_button(label="[LM] left leg", callback= ble_backend.lm_l_left)
                        dpg.add_button(label="[LM] right leg", callback= ble_backend.lm_l_right)

            
    dpg.create_viewport(title="BabyBot Control", width=800, height=500)
    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.start_dearpygui()
    dpg.destroy_context()
