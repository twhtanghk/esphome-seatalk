import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import CONF_ID

DEPENDENCIES = ['uart']

# Define namespace
seatalk_ns = cg.esphome_ns.namespace('seatalk')
SeaTalkComponent = seatalk_ns.class_('SeaTalkComponent', cg.PollingComponent, uart.UARTDevice)

# Define all sensor classes - they need to inherit from sensor.Sensor, not cg.Component
SeaTalkDepthSensor = seatalk_ns.class_('SeaTalkDepthSensor', sensor.Sensor)
SeaTalkSpeedSensor = seatalk_ns.class_('SeaTalkSpeedSensor', sensor.Sensor)
SeaTalkWindAngleSensor = seatalk_ns.class_('SeaTalkWindAngleSensor', sensor.Sensor)
SeaTalkWindSpeedSensor = seatalk_ns.class_('SeaTalkWindSpeedSensor', sensor.Sensor)
SeaTalkHeadingSensor = seatalk_ns.class_('SeaTalkHeadingSensor', sensor.Sensor)
SeaTalkTemperatureSensor = seatalk_ns.class_('SeaTalkTemperatureSensor', sensor.Sensor)
SeaTalkLogSensor = seatalk_ns.class_('SeaTalkLogSensor', sensor.Sensor)

CONF_SEATALK_ID = 'seatalk_id'
MULTI_CONF = True

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SeaTalkComponent),
}).extend(cv.polling_component_schema('60s')).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

# Export everything
__all__ = [
    'seatalk_ns',
    'SeaTalkComponent',
    'SeaTalkDepthSensor',
    'SeaTalkSpeedSensor',
    'SeaTalkWindAngleSensor',
    'SeaTalkWindSpeedSensor',
    'SeaTalkHeadingSensor',
    'SeaTalkTemperatureSensor',
    'SeaTalkLogSensor',
    'CONF_SEATALK_ID',
]
