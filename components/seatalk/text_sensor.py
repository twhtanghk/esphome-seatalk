import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from . import SeaTalkComponent, CONF_SEATALK_ID

CONF_RAW_DATA = 'raw_data'
CONF_GPS_DATA = 'gps_data'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SEATALK_ID): cv.use_id(SeaTalkComponent),
    cv.Optional(CONF_RAW_DATA): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_GPS_DATA): text_sensor.text_sensor_schema(),
})

async def to_code(config):
    seatalk_var = await cg.get_variable(config[CONF_SEATALK_ID])
    
    if CONF_RAW_DATA in config:
        sens = await text_sensor.new_text_sensor(config[CONF_RAW_DATA])
        cg.add(seatalk_var.set_raw_data_sensor(sens))
    
    if CONF_GPS_DATA in config:
        sens = await text_sensor.new_text_sensor(config[CONF_GPS_DATA])
        cg.add(seatalk_var.set_gps_sensor(sens))
