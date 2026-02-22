import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    ICON_GAUGE,
    ICON_THERMOMETER,
    ICON_WEATHER_WINDY,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_SPEED,
    DEVICE_CLASS_TEMPERATURE,
)

# Import everything from __init__.py
from . import (
    seatalk_ns,
    SeaTalkComponent,
    SeaTalkDepthSensor,
    SeaTalkSpeedSensor,
    SeaTalkWindAngleSensor,
    SeaTalkWindSpeedSensor,
    SeaTalkHeadingSensor,
    SeaTalkTemperatureSensor,
    SeaTalkLogSensor,
    CONF_SEATALK_ID,
)

DEPENDENCIES = ['seatalk']

# Local constants
CONF_DEPTH = 'depth'
CONF_SPEED = 'speed'
CONF_TEMPERATURE = 'temperature'
CONF_WIND_ANGLE = 'wind_angle'
CONF_WIND_SPEED = 'wind_speed'
CONF_HEADING = 'heading'
CONF_LOG_TOTAL = 'log_total'

# Configuration schema - using the imported sensor classes directly
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SEATALK_ID): cv.use_id(SeaTalkComponent),
    cv.Optional(CONF_DEPTH): sensor.sensor_schema(
        SeaTalkDepthSensor,
        unit_of_measurement='m',
        icon=ICON_GAUGE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_DISTANCE
    ).extend({
        cv.GenerateID(CONF_ID): cv.declare_id(SeaTalkDepthSensor),
    }),
    cv.Optional(CONF_SPEED): sensor.sensor_schema(
        SeaTalkSpeedSensor,
        unit_of_measurement='kn',
        icon=ICON_GAUGE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_SPEED
    ).extend({
        cv.GenerateID(CONF_ID): cv.declare_id(SeaTalkSpeedSensor),
    }),
    cv.Optional(CONF_WIND_ANGLE): sensor.sensor_schema(
        SeaTalkWindAngleSensor,
        unit_of_measurement='°',
        icon=ICON_WEATHER_WINDY,
        accuracy_decimals=0
    ).extend({
        cv.GenerateID(CONF_ID): cv.declare_id(SeaTalkWindAngleSensor),
    }),
    cv.Optional(CONF_WIND_SPEED): sensor.sensor_schema(
        SeaTalkWindSpeedSensor,
        unit_of_measurement='kn',
        icon=ICON_WEATHER_WINDY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_SPEED
    ).extend({
        cv.GenerateID(CONF_ID): cv.declare_id(SeaTalkWindSpeedSensor),
    }),
    cv.Optional(CONF_HEADING): sensor.sensor_schema(
        SeaTalkHeadingSensor,
        unit_of_measurement='°',
        accuracy_decimals=1
    ).extend({
        cv.GenerateID(CONF_ID): cv.declare_id(SeaTalkHeadingSensor),
    }),
    cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
        SeaTalkTemperatureSensor,
        unit_of_measurement='°C',
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE
    ).extend({
        cv.GenerateID(CONF_ID): cv.declare_id(SeaTalkTemperatureSensor),
    }),
    cv.Optional(CONF_LOG_TOTAL): sensor.sensor_schema(
        SeaTalkLogSensor,
        unit_of_measurement='nm',
        accuracy_decimals=1
    ).extend({
        cv.GenerateID(CONF_ID): cv.declare_id(SeaTalkLogSensor),
    }),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_SEATALK_ID])
    
    if CONF_DEPTH in config:
        sens = cg.new_Pvariable(config[CONF_DEPTH][CONF_ID], parent)
        await sensor.register_sensor(sens, config[CONF_DEPTH])
        cg.add(parent.add_listener(sens))
    
    if CONF_SPEED in config:
        sens = cg.new_Pvariable(config[CONF_SPEED][CONF_ID], parent)
        await sensor.register_sensor(sens, config[CONF_SPEED])
        cg.add(parent.add_listener(sens))
    
    if CONF_WIND_ANGLE in config:
        sens = cg.new_Pvariable(config[CONF_WIND_ANGLE][CONF_ID], parent)
        await sensor.register_sensor(sens, config[CONF_WIND_ANGLE])
        cg.add(parent.add_listener(sens))
    
    if CONF_WIND_SPEED in config:
        sens = cg.new_Pvariable(config[CONF_WIND_SPEED][CONF_ID], parent)
        await sensor.register_sensor(sens, config[CONF_WIND_SPEED])
        cg.add(parent.add_listener(sens))
    
    if CONF_HEADING in config:
        sens = cg.new_Pvariable(config[CONF_HEADING][CONF_ID], parent)
        await sensor.register_sensor(sens, config[CONF_HEADING])
        cg.add(parent.add_listener(sens))
    
    if CONF_TEMPERATURE in config:
        sens = cg.new_Pvariable(config[CONF_TEMPERATURE][CONF_ID], parent)
        await sensor.register_sensor(sens, config[CONF_TEMPERATURE])
        cg.add(parent.add_listener(sens))
    
    if CONF_LOG_TOTAL in config:
        sens = cg.new_Pvariable(config[CONF_LOG_TOTAL][CONF_ID], parent)
        await sensor.register_sensor(sens, config[CONF_LOG_TOTAL])
        cg.add(parent.add_listener(sens))
