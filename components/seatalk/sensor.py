import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_DEPTH, CONF_SPEED, CONF_TEMPERATURE, CONF_ID,
    UNIT_METER, UNIT_KNOTS, UNIT_DEGREES, UNIT_CELSIUS,
    ICON_WATER, ICON_GAUGE, ICON_THERMOMETER, ICON_WEATHER_WINDY,
    DEVICE_CLASS_DISTANCE, DEVICE_CLASS_SPEED, DEVICE_CLASS_TEMPERATURE
)
from . import SeaTalkComponent, CONF_SEATALK_ID

DEPENDENCIES = ['seatalk']

# SeaTalk specific sensor types
CONF_WIND_ANGLE = 'wind_angle'
CONF_WIND_SPEED = 'wind_speed'
CONF_HEADING = 'heading'
CONF_LOG_TOTAL = 'log_total'

TYPES = {
    CONF_DEPTH: ('depth', UNIT_METER, ICON_GAUGE, DEVICE_CLASS_DISTANCE),
    CONF_SPEED: ('speed', UNIT_KNOTS, ICON_GAUGE, DEVICE_CLASS_SPEED),
    CONF_WIND_ANGLE: ('wind_angle', UNIT_DEGREES, ICON_WEATHER_WINDY, None),
    CONF_WIND_SPEED: ('wind_speed', UNIT_KNOTS, ICON_WEATHER_WINDY, DEVICE_CLASS_SPEED),
    CONF_HEADING: ('heading', UNIT_DEGREES, None, None),
    CONF_TEMPERATURE: ('temperature', UNIT_CELSIUS, ICON_THERMOMETER, DEVICE_CLASS_TEMPERATURE),
    CONF_LOG_TOTAL: ('log_total', 'nm', None, None),
}

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SEATALK_ID): cv.use_id(SeaTalkComponent),
    cv.Optional(CONF_DEPTH): sensor.sensor_schema(
        unit_of_measurement=UNIT_METER,
        icon=ICON_GAUGE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_DISTANCE
    ),
    cv.Optional(CONF_SPEED): sensor.sensor_schema(
        unit_of_measurement=UNIT_KNOTS,
        icon=ICON_GAUGE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_SPEED
    ),
    cv.Optional(CONF_WIND_ANGLE): sensor.sensor_schema(
        unit_of_measurement=UNIT_DEGREES,
        icon=ICON_WEATHER_WINDY,
        accuracy_decimals=0
    ),
    cv.Optional(CONF_WIND_SPEED): sensor.sensor_schema(
        unit_of_measurement=UNIT_KNOTS,
        icon=ICON_WEATHER_WINDY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_SPEED
    ),
    cv.Optional(CONF_HEADING): sensor.sensor_schema(
        unit_of_measurement=UNIT_DEGREES,
        accuracy_decimals=1
    ),
    cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE
    ),
    cv.Optional(CONF_LOG_TOTAL): sensor.sensor_schema(
        unit_of_measurement='nm',
        accuracy_decimals=1
    ),
})

async def to_code(config):
    seatalk_var = await cg.get_variable(config[CONF_SEATALK_ID])
    
    # Create and register sensors
    for key, (type_name, unit, icon, device_class) in TYPES.items():
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(seatalk_var, f'add_{type_name}_listener')(sens))
