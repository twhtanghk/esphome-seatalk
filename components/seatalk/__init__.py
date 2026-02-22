import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ['uart']

seatalk_ns = cg.esphome_ns.namespace('seatalk')
SeaTalkComponent = seatalk_ns.class_('SeaTalkComponent', cg.PollingComponent, uart.UARTDevice)

CONF_SEATALK_ID = 'seatalk_id'
MULTI_CONF = True

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SeaTalkComponent),
}).extend(cv.polling_component_schema('60s')).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

__all__ = [
    'seatalk_ns',
    'SeaTalkComponent',
    'CONF_SEATALK_ID',
]
