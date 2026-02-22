import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    ICON_GAUGE,
    ICON_THERMOMETER,
    ICON_WEATHER_WINDY,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_SPEED,
    DEVICE_CLASS_TEMPERATURE,
)

from . import seatalk_ns, CONF_SEATALK_ID

DEPENDENCIES = ['seatalk']

# Local constants (not imported from esphome.const)
CONF_DEPTH = 'depth'
CONF_SPEED = 'speed'
CONF_TEMPERATURE = 'temperature'
CONF_WIND_ANGLE = 'wind_angle'
CONF_WIND_SPEED = 'wind_speed'
CONF_HEADING = 'heading'
CONF_LOG_TOTAL = 'log_total'

# Sensor classes (must match C++ class names)
SeaTalkDepthSensor = seatalk_ns.class_('SeaTalkDepthSensor', sensor.Sensor)
SeaTalkSpeedSensor = seatalk_ns.class_('SeaTalkSpeedSensor', sensor.Sensor)
SeaTalkWindAngleSensor = seatalk_ns.class_('SeaTalkWindAngleSensor', sensor.Sensor)
SeaTalkWindSpeedSensor = seatalk_ns.class_('SeaTalkWindSpeedSensor', sensor.Sensor)
SeaTalkHeadingSensor = seatalk_ns.class_('SeaTalkHeadingSensor', sensor.Sensor)
SeaTalkTemperatureSensor = seatalk_ns.class_('SeaTalkTemperatureSensor', sensor.Sensor)
SeaTalkLogSensor = seatalk_ns.class_('SeaTalkLogSensor', sensor.Sensor)

# Configuration schema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SEATALK_ID): cv.use_id(SeaTalkComponent),
    cv.Optional(CONF_DEPTH): sensor.sensor_schema(
        SeaTalkDepthSensor,
        unit_of_measurement='m',
        icon=ICON_GAUGE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_DISTANCE
    ),
    cv.Optional(CONF_SPEED): sensor.sensor_schema(
        SeaTalkSpeedSensor,
        unit_of_measurement='kn',
        icon=ICON_GAUGE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_SPEED
    ),
    cv.Optional(CONF_WIND_ANGLE): sensor.sensor_schema(
        SeaTalkWindAngleSensor,
        unit_of_measurement='°',
        icon=ICON_WEATHER_WINDY,
        accuracy_decimals=0
    ),
    cv.Optional(CONF_WIND_SPEED): sensor.sensor_schema(
        SeaTalkWindSpeedSensor,
        unit_of_measurement='kn',
        icon=ICON_WEATHER_WINDY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_SPEED
    ),
    cv.Optional(CONF_HEADING): sensor.sensor_schema(
        SeaTalkHeadingSensor,
        unit_of_measurement='°',
        accuracy_decimals=1
    ),
    cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
        SeaTalkTemperatureSensor,
        unit_of_measurement='°C',
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE
    ),
    cv.Optional(CONF_LOG_TOTAL): sensor.sensor_schema(
        SeaTalkLogSensor,
        unit_of_measurement='nm',
        accuracy_decimals=1
    ),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_SEATALK_ID])
    
    if CONF_DEPTH in config:
        sens = await sensor.new_sensor(config[CONF_DEPTH])
        cg.add(parent.add_listener(sens))
    
    if CONF_SPEED in config:
        sens = await sensor.new_sensor(config[CONF_SPEED])
        cg.add(parent.add_listener(sens))
    
    if CONF_WIND_ANGLE in config:
        sens = await sensor.new_sensor(config[CONF_WIND_ANGLE])
        cg.add(parent.add_listener(sens))
    
    if CONF_WIND_SPEED in config:
        sens = await sensor.new_sensor(config[CONF_WIND_SPEED])
        cg.add(parent.add_listener(sens))
    
    if CONF_HEADING in config:
        sens = await sensor.new_sensor(config[CONF_HEADING])
        cg.add(parent.add_listener(sens))
    
    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(parent.add_listener(sens))
    
    if CONF_LOG_TOTAL in config:
        sens = await sensor.new_sensor(config[CONF_LOG_TOTAL])
        cg.add(parent.add_listener(sens))
