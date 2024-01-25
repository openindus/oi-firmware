

class ConfigurationMode:
    general = 0 # general configuration
    voltage = 1 # voltage mode configuration
    current = 2 # current mode configuration

class Rights:
    READ = 1 # readable
    WH   = 2 # writable when the outputs are in high impedance only
    WS   = 4 # writable when the motor is stopped only
    WR   = 8 # always writable

class Register:
    
    def __init__(self, address, register_name, length, reset_value, rights, configuration_mode):
        self.address            = address
        self.register_name      = register_name
        self.length             = length
        self.reset_value        = reset_value
        self.rights             = rights
        self.configuration_mode = configuration_mode

    def value_mask(self):
        return 2**self.length - 1


class AvailableRegisters:

    # general config
    ABS_POS     = Register(address=0x01, register_name="ABS_POS", length=22, reset_value=0x0, rights=[Rights.READ, Rights.WS], configuration_mode=ConfigurationMode.general)
    EL_POS      = Register(address=0x02, register_name="EL_POS", length=9, reset_value=0x0, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.general)
    MARK        = Register(address=0x03, register_name="MARK", length=22, reset_value=0x0, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.general)
    SPEED       = Register(address=0x04, register_name="SPEED", length=20, reset_value=0x0, rights=[Rights.READ], configuration_mode=ConfigurationMode.general)
    ACC         = Register(address=0x05, register_name="ACC", length=12, reset_value=0x8A, rights=[Rights.READ, Rights.WS], configuration_mode=ConfigurationMode.general)
    DEC         = Register(address=0x06, register_name="DEC", length=12, reset_value=0x8A, rights=[Rights.READ, Rights.WS], configuration_mode=ConfigurationMode.general)
    MAX_SPEED   = Register(address=0x07, register_name="MAX_SPEED", length=10, reset_value=0x41, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.general)
    MIN_SPEED   = Register(address=0x08, register_name="MIN_SPEED", length=12, reset_value=0x0, rights=[Rights.READ, Rights.WS], configuration_mode=ConfigurationMode.general)
    ADC_OUT     = Register(address=0x12, register_name="ADC_OUT", length=5, reset_value=0x0, rights=[Rights.READ], configuration_mode=ConfigurationMode.general)
    OCD_TH      = Register(address=0x13, register_name="OCD_TH", length=5, reset_value=0x8, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.general)
    FS_SPD      = Register(address=0x15, register_name="FS_SPD", length=11, reset_value=0x27, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.general)
    STEP_MODE   = Register(address=0x16, register_name="STEP_MODE", length=8, reset_value=0x7, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.general)
    ALARM_EN    = Register(address=0x17, register_name="ALARM_EN", length=8, reset_value=0xFF, rights=[Rights.READ, Rights.WS], configuration_mode=ConfigurationMode.general)
    GATECFG1    = Register(address=0x18, register_name="GATECFG1", length=11, reset_value=0x0, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.general)
    GATECFG2    = Register(address=0x19, register_name="GATECFG2", length=8, reset_value=0x0, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.general)
    STATUS      = Register(address=0x1B, register_name="STATUS", length=16, reset_value=0x0, rights=[Rights.READ], configuration_mode=ConfigurationMode.general)
    CONFIG      = Register(address=0x1A, register_name="CONFIG", length=16, reset_value=0x2C88, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.general)

    # voltage mode config
    KVAL_HOLD   = Register(address=0x09, register_name="KVAL_HOLD", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.voltage)
    KVAL_RUN    = Register(address=0x0A, register_name="KVAL_RUN", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.voltage)
    KVAL_ACC    = Register(address=0x0B, register_name="KVAL_ACC", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.voltage)
    KVAL_DEC    = Register(address=0x0C, register_name="KVAL_DEC", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.voltage)
    INT_SPEED   = Register(address=0x0D, register_name="INT_SPEED", length=14, reset_value=0x408, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.voltage)
    ST_SLP      = Register(address=0x0E, register_name="ST_SLP", length=8, reset_value=0x19, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.voltage)
    FN_SLP_ACC  = Register(address=0x0F, register_name="FN_SLP_ACC", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.voltage)
    FN_SLP_DEC  = Register(address=0x10, register_name="FN_SLP_DEC", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.voltage)
    K_THERM     = Register(address=0x11, register_name="K_THERM", length=4, reset_value=0x0, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.voltage)
    STALL_TH    = Register(address=0x14, register_name="STALL_TH", length=5, reset_value=0x10, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.voltage)
    # <- CONFIG here ??

    # current mode config
    TVAL_HOLD   = Register(address=0x09, register_name="TVAL_HOLD", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.current)
    TVAL_RUN    = Register(address=0x0A, register_name="TVAL_RUN", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.current)
    TVAL_ACC    = Register(address=0x0B, register_name="TVAL_ACC", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.current)
    TVAL_DEC    = Register(address=0x0C, register_name="TVAL_DEC", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WR], configuration_mode=ConfigurationMode.current)
    T_FAST      = Register(address=0x0E, register_name="T_FAST", length=8, reset_value=0x19, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.current)
    TON_MIN     = Register(address=0x0F, register_name="TON_MIN", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.current)
    TOFF_MIN    = Register(address=0x10, register_name="TOFF_MIN", length=8, reset_value=0x29, rights=[Rights.READ, Rights.WH], configuration_mode=ConfigurationMode.current)
    # <- CONFIG here ??

class CommandByte:
    
    def to_int(bin_list):
        return sum([bin_list[i] << len(bin_list)-i-1 for i in range(len(bin_list))])

    # powerstep01 doc table 60 page 73
    NOP             = lambda : CommandByte.to_int([0,0,0,0,0,0,0,0])
    SET_PARAM       = lambda param: CommandByte.to_int([0,0,0,0,0,0,0,param])
    GET_PARAM       = lambda param: CommandByte.to_int([0,0,1,0,0,0,0,param])
    RUN             = lambda dire: CommandByte.to_int([0,1,0,1,0,0,0,dire])
    STEP_CLOCK      = lambda dire: CommandByte.to_int([0,1,0,1,1,0,0,dire])
    MOVE            = lambda dire: CommandByte.to_int([0,1,0,0,0,0,0,dire])
    GO_TO           = lambda : CommandByte.to_int([0,1,1,0,0,0,0,0])
    GO_TO_DIR       = lambda dire: CommandByte.to_int([0,1,1,0,1,0,0,dire])
    GO_UNTIL        = lambda act, dire: CommandByte.to_int([1,0,0,0,act,0,1,dire])
    RELEASE_SW      = lambda act, dire: CommandByte.to_int([1,0,0,1,act,0,1,dire])

    GO_HOME         = lambda : CommandByte.to_int([0,1,1,1,0,0,0,0])
    GO_MARK         = lambda : CommandByte.to_int([0,1,1,1,1,0,0,0])
    RESET_POS       = lambda : CommandByte.to_int([1,1,0,1,1,0,0,0])
    RESET_DEVICE    = lambda : CommandByte.to_int([1,1,0,0,0,0,0,0])
    SOFT_STOP       = lambda : CommandByte.to_int([1,0,1,1,0,0,0,0])
    HARD_STOP       = lambda : CommandByte.to_int([1,0,1,1,1,0,0,0])
    SOFT_HIZ        = lambda : CommandByte.to_int([1,0,1,0,0,0,0,0])
    HARD_HIZ        = lambda : CommandByte.to_int([1,0,1,0,1,0,0,0])
    GET_STATUS      = lambda : CommandByte.to_int([1,1,0,1,0,0,0,0])
    RESERVED_1      = lambda : CommandByte.to_int([1,1,1,0,1,0,1,1])
    RESERVED_2      = lambda : CommandByte.to_int([1,1,1,1,1,0,0,0])


class CommandFrame:
    
    NOP             = CommandByte.NOP
    SET_PARAM       = lambda param, value: CommandByte.SET_PARAM(param) << 3 * 8 | value
    GET_PARAM       = CommandByte.GET_PARAM
    RUN             = lambda dire, spd: CommandByte.RUN(dire) << 3 * 8 | spd
    STEP_CLOCK      = CommandByte.STEP_CLOCK
    MOVE            = lambda dire, n_step: CommandByte.MOVE(dire) << 3 * 8 | n_step
    GO_TO           = lambda abs_pos: CommandByte.GO_TO() << 3 * 8 | abs_pos
    GO_TO_DIR       = lambda dire, abs_pos: CommandByte.GO_TO_DIR(dire) << 3 * 8 | abs_pos
    GO_UNTIL        = lambda act, dire, spd: CommandByte.GO_UNTIL(act, dire) << 3 * 8 | spd
    RELEASE_SW      = CommandByte.RELEASE_SW

    GO_HOME         = CommandByte.GO_HOME     
    GO_MARK         = CommandByte.GO_MARK     
    RESET_POS       = CommandByte.RESET_POS   
    RESET_DEVICE    = CommandByte.RESET_DEVICE
    SOFT_STOP       = CommandByte.SOFT_STOP   
    HARD_STOP       = CommandByte.HARD_STOP   
    SOFT_HIZ        = CommandByte.SOFT_HIZ    
    HARD_HIZ        = CommandByte.HARD_HIZ    
    GET_STATUS      = CommandByte.GET_STATUS  
    RESERVED_1      = CommandByte.RESERVED_1  
    RESERVED_2      = CommandByte.RESERVED_2  


direction = {
    'FORWARD' : 1,
    'BACKWARD' : 0
}