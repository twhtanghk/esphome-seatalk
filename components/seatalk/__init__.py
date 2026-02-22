import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ['uart']

# Define namespace
seatalk_ns = cg.esphome_ns.namespace('seatalk')
SeaTalkComponent = seatalk_ns.class_('SeaTalkComponent', cg.PollingComponent, uart.UARTDevice)

# Define sensor classes
SeaTalkDepthSensor = seatalk_ns.class_('SeaTalkDepthSensor', cg.Component)
SeaTalkSpeedSensor = seatalk_ns.class_('SeaTalkSpeedSensor', cg.Component)
SeaTalkWindAngleSensor = seatalk_ns.class_('SeaTalkWindAngleSensor', cg.Component)
SeaTalkWindSpeedSensor = seatalk_ns.class_('SeaTalkWindSpeedSensor', cg.Component)
SeaTalkHeadingSensor = seatalk_ns.class_('SeaTalkHeadingSensor', cg.Component)
SeaTalkTemperatureSensor = seatalk_ns.class_('SeaTalkTemperatureSensor', cg.Component)
SeaTalkLogSensor = seatalk_ns.class_('SeaTalkLogSensor', cg.Component)

CONF_SEATALK_ID = 'seatalk_id'
MULTI_CONF = True

# Component configuration schema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SeaTalkComponent),
}).extend(cv.polling_component_schema('60s')).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    # Get the UART bus
    uart_parent = await cg.get_variable(config[uart.CONF_UART_ID])
    
    # Create component with UART parent
    var = cg.new_Pvariable(config[CONF_ID], uart_parent)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

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
