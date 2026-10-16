/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : CurrFliterCalc.c
** @Founder      : 
** @Date         : 
** @Function     : 
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "CurrFliterCalc.h"
#include "BSPADC.h"
#include "BSPDIDO.h"
#include "SystemData.h"
#include "BSPKA84917_async.h"
#include "drv_rn8209_interface.h"
#include "SystemPara.h"
#include "SystemInfo.h"
#include "SystemDebug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "SystemDiag.h"
#include "BSPTypeDef.h"
#include "SEGGER_RTT.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

// static St_FilterType stBatVoltFilter = { 0 };
// static St_FilterType stLinkVoltFilter = { 0 };
// static St_FilterType stShuntCurrentFilter = { 0 };
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/




/*=============================================================================================
** @Name      : bool GetFilterValue(float32 f32Input, float32 *pf32Output, St_FilterType *pstFilter)
** @Input     : 
** @Output    : 引用返回pf32Output
** @Function  : 去极值平均滤波(去掉最大值和最小值后取平均)
** @The notes : 
**===========================================================================================*/   
bool GetFilterValue(float32 f32Input, float32 *pf32Output, St_FilterType *pstFilter)
{
    uint8 i;
    float32 f32Sum = 0;
    if (pstFilter->u8InputCount == 0) {
        pstFilter->f32MaxVal = -10000.0f;
        pstFilter->f32MinVal = 10000.0f;
    }
    if (f32Input > pstFilter->f32MaxVal) {
        pstFilter->f32MaxVal = f32Input;
    }
    if (f32Input < pstFilter->f32MinVal) {
        pstFilter->f32MinVal = f32Input;
    }
    pstFilter->f32InputVal[pstFilter->u8InputCount] = f32Input;
    pstFilter->u8InputCount++;
    if (pstFilter->u8InputCount == 5U) {
        for (i = 0; i < 5U; i++) {
            f32Sum += pstFilter->f32InputVal[i];
        }
        f32Sum -= pstFilter->f32MaxVal;
        f32Sum -= pstFilter->f32MinVal;
        *pf32Output = f32Sum / 3U;
        pstFilter->u8InputCount = 0;
        return TRUE;
    } else {
        return FALSE;
    }
}


/*=============================================================================================
** @Name      : bool IsCoefValid(float32 f32K, float32 f32B)
** @Input     : K B 校准系数
** @Output    : 
** @Function  : 校准系数范围判断
** @The notes : 
**===========================================================================================*/  
bool IsCoefValid(float32 f32K, float32 f32B)
{
    if ((f32K > 0.8f) && (f32K < 1.3f) && (f32B > -50.0f) && (f32B < 50.0f)) {//此次b值偏移有时候超过10V
        return TRUE;
    } else {
        return FALSE;
    }
}