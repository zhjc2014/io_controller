//#define DEF_DBG_I
//#define DEF_DBG_W
//#define DEF_DBG_E
//#include "debug.h"

//#include "bsp_adc.h"
//#include "x_frame.h"

//#if ENABLE_RTT_DRV

//#define BATTERY_SERIAL_NUM                    ((int)3)
//#define BATTERY_VOLTAGE_FULL                  ((int)(12.6 * 1000) / BATTERY_SERIAL_NUM) //mV
//#define BATTERY_VOLTAGE_EMPTY                 ((int)(9 * 1000) / BATTERY_SERIAL_NUM)    //mV
//#define BATTERY_VOLTAGE_K                     1.0f
//#define FILTER_SIZE                           10

//static uint32_t batteryElectricityPercentage = 0;
//static uint32_t batteryElectricity = 0;
//static float batteryVoltageK = BATTERY_VOLTAGE_K;

//static const float batteryOcv[] = {
//    4.20,  3.97,   3.87,   3.79,  3.73,   3.68,   3.65, 3.62, 3.58, 3.51, 3.42, 3.00
//};
//static const float batterySoc[] = {
//    100,   90,     80,     70,    60,     50,     40,   30,   20,   10,   5,    0
//};

//static int raw_batv_read(uint16_t *raw_voltage)
//{
//    static const double refPos = 3300.0; //mV
//    static const double refNeg = 0.0;
//    static const int maxAdc = 4095;
//    static const double r1 = 30.0;    //korm
//    static const double r2 = 10.0;    //korm
//    uint16_t adc_value;
//    double adc_voltage;
////    hal_app_read(ADC_DEV, (uint8_t*)&adc_value, sizeof(uint16_t), 0);
//    adc_value = bsp_adc_read  (0);
//    adc_voltage = adc_value * (refPos - refNeg) / maxAdc;
//    *raw_voltage = adc_voltage * (r1 + r2) / r2;
//    return 0;
//}


//static int batv_read(uint16_t *voltage)
//{
//    uint16_t raw_voltage;
//    raw_batv_read(&raw_voltage);
//    *voltage = raw_voltage * batteryVoltageK;
//    return 0;
//}

//static int batv_write(uint16_t voltage)
//{
//    const static float max_batv_ratio = 1.4;
//    const static float min_batv_ratio = 0.6;
//    float bat_ratio;
//    uint16_t raw_voltage;
//    raw_batv_read(&raw_voltage);
//    bat_ratio = voltage * 1.0 / raw_voltage;
//    DBG_I("bat_ratio :%f", bat_ratio);
//    if (bat_ratio < max_batv_ratio && bat_ratio > min_batv_ratio ) {
//        batteryVoltageK = bat_ratio;
////        return paramWriteSingle(g_save_param_info, (uint32_t)&g_base_bot_data.batVolK);
//        return 0;
//    } else {
//        return -1;
//    }
//}

//static void get_electricity(void)
//{
//    uint32_t volSum;
//    uint16_t voltage = 0;
//    int i;
//    static int volIndex = 0;
//    static uint16_t voltageArr[FILTER_SIZE] = {0};
//    batv_read(&voltage);
//    voltageArr[++volIndex % FILTER_SIZE] = voltage;
//    volSum = 0;
//    for (i = 0; i < FILTER_SIZE; i++) {
//        volSum += voltageArr[i];
//    }
//    batteryElectricity = volSum / FILTER_SIZE / BATTERY_SERIAL_NUM;
//}
//REG_TASK(ID_TASK, 50, get_electricity);

//static uint8_t get_electricitypercentage(void)
//{
//    return batteryElectricityPercentage;
//}

//static void cal_electricitypercentage(void)
//{
//    uint32_t currentVolt;
//    currentVolt = batteryElectricity;
//    if (currentVolt < BATTERY_VOLTAGE_EMPTY) {
//        batteryElectricityPercentage = 0;
//    } else if (currentVolt > BATTERY_VOLTAGE_FULL) {
//        batteryElectricityPercentage = 100;
//    } else {
//        int i;
//        for (i = 1; i < sizeof(batteryOcv) / sizeof(batteryOcv[0]); i++) {
//            if (currentVolt / 1000.0 > batteryOcv[i]) {
//                batteryElectricityPercentage = \
//                                               ((batterySoc[i - 1] - batterySoc[i]) / (batteryOcv[i - 1] - batteryOcv[i])) * \
//                                               (currentVolt / 1000.0 - batteryOcv[i]) + batterySoc[i];
//                if (batteryElectricityPercentage > 100) {
//                    batteryElectricityPercentage = 100;
//                }
////                DBG_I("%d %f %f %d, %d", i, batteryOcv[i], batterySoc[i], \
////                currentVolt, batteryElectricityPercentage);
//                break;
//            }
//        }
//    }
//}
//REG_TASK(ID_TASK, 500, cal_electricitypercentage);

//static int32_t drv_bat_init        (void)
//{
//    bsp_adc_init();
//    return 0;
//}

//static int32_t drv_bat_write    (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
//{
//    uint16_t voltage;
//    voltage = *(uint16_t*)pSrc;
//    if (p_real){
//        *p_real = sizeof voltage;
//    }
//    return batv_write(voltage);
//}

//static int32_t drv_bat_read    (uint8_t* p_dest, uint32_t len    , uint32_t offset, uint32_t* p_real)
//{
//    *p_dest = get_electricitypercentage();
//    if (p_real){
//        *p_real = 1;
//    }
//    return 0;
//}

//REG_DEV(
//    DRV_BAT,                        /*driver id         */
//    DEV_AUTO_INIT,                  /*driver auto init  */
//    drv_bat_init,                   /*driver init func  */
//    drv_bat_write,                  /*driver write func */
//    drv_bat_read,                   /*driver read  func */
//    NULL,                           /*driver ctrl  func */
//    NULL                            /*driver close func */
//);

//#endif


