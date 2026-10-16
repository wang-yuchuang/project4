/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SystemDiag.c
** @Founder      : WYC834
** @Date         : 2026.1.8
** @Function     : 自身诊断模块
** @Instructions : 系统诊断自身故障告警
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "SystemDiag.h"
#include "SystemPara.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
u16 SystemDiag_57[SystemDiag_Len];//系统信息

t_FilState gErrFilTab[eERR_ID_FINISH] = {0};                /*故障诊断滤波处理(判断故障发生或消除状态)*/

const t_FilPara gErrFilPara[eERR_ID_FINISH] =               /*故障诊断滤波参数(故障滤波时间和自恢复次数)*/
{
	/*故障类型/检测滤波时间(ms)/恢复滤波时间(ms)/允许自恢复次数*/
	/*故障类型:[0:正常(不检测),1:仅提示类，2:充电限制类,3:放电限制类,4:充放电限制类,5:充电保护类,6:放电保护类,7:充放电保护类,8:系统故障类]*/
	/*以下为告警*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[0]高压采集芯片917初始化异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[1]高压采集芯片917通信异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[2]高压采样芯片+5V回检异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[3]高压采样芯片-5V回检异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[4]LINK总压采样值异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[5]电池总压采样值异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[6]霍尔过载故障*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[7]霍尔供电异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[8]霍尔AD值异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[9]电池端分流器采样值异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[10]分流器与霍尔采样偏差过大*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[11]负载端分流器采样值异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[12]绝缘检测电压不稳*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[13]绝缘检测电压过小或负值*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[14]菊花链芯片通信异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[15]计量芯片通信异常*/

	{eErr_SysErr,    1000, 10000, 255},                       /*[16]板子5V供电电压回检异常*/
	{eErr_SysErr,    1000, 10000, 255},                       /*[17]电流采样自检异常*/


};
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=================================================================================================
** @Name      : static void DiagSetErrState(e_ERR_ID id, u8 state)
** @Input     : id:故障码ID state:设置状态:0清除状态 1:发生状态
** @Output    : void
** @Function  : 设置故障码状态
** @The notes : 清0或置1
**===============================================================================================*/
static void DiagSetErrState(e_ERR_ID id, u8 state);
// /*=================================================================================================
// ** @Name      : static u8 DiagGetExpBefState(e_ERR_ID id)
// ** @Input     : id:故障码ID
// ** @Output    : 故障前置状态:0:正常 1:故障/告警
// ** @Function  : 获取故障前置状态
// ** @The notes : 当前显示状态
// **===============================================================================================*/
// static u8 DiagGetExpBefState(e_ERR_ID id);


/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
void InitSystemDiag(void)
{
	for(int i  = 0;i < SystemDiag_Len; i++)
	{
		SystemDiag_57[i] = 0;
	}
}

/*=================================================================================================
** @Name      : void DiagErrFilterInit(void)
** @Input     : swhSort:接触器名称[0,11] state:控制接触器状态 0:断开 1:闭合
** @Output    : void
** @Function  : 故障诊断滤波初始化
** @The notes : 滤波处理数组清零
**===============================================================================================*/
void DiagErrFilterInit(void)
{
	e_ERR_ID i = eERR_ID_FINISH;

	/*所有故障位*/
	for(i = (e_ERR_ID)0; i < eERR_ID_FINISH; i++)
	{
	    /*根据故障检测类型设置是否使能故障检测*/
	    if(gErrFilPara[i].errSort > 0)
	    {
	        gErrFilTab[i].enable = 1;
	    }
	    else
	    {
	        gErrFilTab[i].enable = 0;
	    }

	    /*复位所有故障滤波参数*/
	    gErrFilTab[i].nowState = eErr_A;
	    gErrFilTab[i].filState = eErr_A;
	    gErrFilTab[i].resTimes = 0;
	    gErrFilTab[i].timer = 0;
	}
}

/*=================================================================================================
** @Name      : e_ErrSort DiagGetGroupNowErrLevelAPI(void)
** @Input     : void
** @Output    : 当前故障级别[0,8]
** @Function  : 获取当前故障级别
** @The notes : 当前故障的最高级别
**===============================================================================================*/
e_ErrSort DiagGetGroupNowErrLevelAPI(void)
{
	e_ERR_ID i = eERR_ID_FINISH;
	e_ErrSort sort = eErr_Norm;

    for(i = (e_ERR_ID)0; i < eERR_ID_FINISH; i++)
    {
        if(eErr_Norm == DiagGetFilErrSortAPI(i))            /*不检测类型故障*/
        {
            ;                                               /*无故障*/
        }
        else if(eErr_Warm == DiagGetFilErrSortAPI(i))       /*提示类型故障*/
        {
            if(eErr_B == DiagGetErrLastStateAPI(i))         /*出现提示类异常*/
            {
                if(sort < eErr_Warm)                        /*未检测到级别更高的异常*/
                {
                    sort = eErr_Warm;                       /*设置为提示级别类型*/
                }
            }
        }
        else if(eErr_SysErr == DiagGetFilErrSortAPI(i))     /*故障类型故障*/
        {
            if(eErr_B == DiagGetErrLastStateAPI(i))         /*出现停机类异常*/
            {
                sort = eErr_SysErr;                         /*设置为停机级别类型*/
                break;                                      /*已是最高级,不需再检测*/
            }
        }

    }

    return(sort);
}

/*=================================================================================================
** @Name      : e_ErrSort DiagGetFilErrSortAPI(e_ERR_ID id)
** @Input     : id:故障ID
** @Output    : 故障类型[0,8]
** @Function  : 获取故障类型
** @The notes :
**===============================================================================================*/
e_ErrSort DiagGetFilErrSortAPI(e_ERR_ID id)
{
	if(id < eERR_ID_FINISH)
	{
        /*返回故障类型*/
        return(gErrFilPara[id].errSort);
	}
	else
	{
		return(eErr_Norm);
	}
}

/*=================================================================================================
** @Name      : void DiagErrFilterTask(u8 from, u8 to)
** @Input     : from-to:检测故障滤波起始和结束ID(包括from不包to)
** @Output    : void
** @Function  : 故障诊断滤波任务
** @The notes : 50ms周期调用,与DIAG_FILTER_TICK一致
**===============================================================================================*/
void DiagErrFilterTask(e_ERR_ID from, e_ERR_ID to)
{
	e_ERR_ID i = eERR_ID_FINISH;
	e_ERR_ID temp = eERR_ID_FINISH;

    if(to > temp)                                          /*to必须小于最大值*/
    {
        to = temp;
    }

    if(from > temp)                                        /*from必须小于最大值*/
    {
        from = temp;
    }

    if(from > to)                                           /*from 必须小于等于to*/
    {
        temp = from;
        from = to;
        to = temp;
    }

    for(i = from; i < to; i++)                              /*逐个检测指定故障id*/
    {
        if(IsEqual(gErrFilTab[i].enable, 1))                /*该故障id使能检测*/
        {
        	/*当前稳定状态*/
            switch(gErrFilTab[i].filState)
            {
                case eErr_A:                                /*正常状态*/
                    if(IsEqual(gErrFilTab[i].nowState, eErr_B))/*如果其瞬态值是eErr_B则设置为正常到故障的过渡态*/
                    {
                    	gErrFilTab[i].filState = eErr_AtoB;
                    	gErrFilTab[i].timer = 0;
                    }
                    break;

                case eErr_AtoB:                             /*正常到故障的过渡态*/
                    if(IsEqual(gErrFilTab[i].nowState, eErr_B))
                    {
                    	gErrFilTab[i].timer++;              /*如果次数达到了设置值则变化为故障状态*/
                        if(gErrFilTab[i].timer > (gErrFilPara[i].AtoBTime / DIAG_FILTER_TICK))/*50ms周期*/
                        {
                        	/*计算A->B的次数*/
                            if(gErrFilTab[i].resTimes < 254)/*防止越界*/
                            {
                            	gErrFilTab[i].resTimes++;   /*次数加1*/
                            }

                            gErrFilTab[i].filState = eErr_B;
                            gErrFilTab[i].timer = 0;
                        }
                    }
                    else if(IsEqual(gErrFilTab[i].nowState, eErr_A))/*A->B过渡状态下如果其瞬态值是eErr_A的则设置其恢复为正常*/
                    {
                    	gErrFilTab[i].filState = eErr_A;
                    	gErrFilTab[i].timer = 0;
                    }
                    break;

                case eErr_B:                                /*故障状态*/
                    if(IsEqual(gErrFilTab[i].nowState, eErr_A))/*如果其瞬态值是eErr_A则设置为故障到正常的过渡态*/
                    {
                    	gErrFilTab[i].filState = eErr_BtoA;
                    	gErrFilTab[i].timer = 0;
                    }
                    break;

                case eErr_BtoA:                             /*故障到正常的过渡态*/
                    if(IsEqual(gErrFilTab[i].nowState, eErr_A))
                    {
                    	gErrFilTab[i].timer++;              /*如果次数达到了设置值则变化为正常状态*/
                        if(gErrFilTab[i].timer > (gErrFilPara[i].BtoATime / DIAG_FILTER_TICK))/*50ms周期*/
                        {
                        	gErrFilTab[i].filState = eErr_A;
                        	gErrFilTab[i].timer = 0;
                        }
                    }
                    else if(IsEqual(gErrFilTab[i].nowState, eErr_B))/*B->A过渡状态下如果其瞬态值是eErr_B的则设置恢复为故障*/
                    {
                    	gErrFilTab[i].filState = eErr_B;
                    	gErrFilTab[i].timer = 0;
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

/*=================================================================================================
** @Name      : void DiagSetErrNowStateAPI(e_ERR_ID id, e_ErrState state)
** @Input     : id:故障ID state:目标故障状态
** @Output    : void
** @Function  : 设置故障诊断滤波当前瞬时状态
** @The notes :
**===============================================================================================*/
void DiagSetErrNowStateAPI(e_ERR_ID id, e_ErrState state)
{
	if((id < eERR_ID_FINISH) && (1 == gErrFilTab[id].enable))
	{
	    /*检测是否可以设置恢复为A*/
	    if((255 != gErrFilPara[id].maxTimes)                /*非无限次恢复*/
	        && (eErr_B == gErrFilTab[id].filState)          /*正处于B状态*/
	        && (eErr_A == state))                           /*需要设置为A状态*/
	    {
	        if(gErrFilTab[id].resTimes >= gErrFilPara[id].maxTimes)/*达到最大恢复次数*/
	        {
	            return;                                     /*不能设置恢复为A*/
	        }
	    }

		gErrFilTab[id].nowState = state;
	}
}

/*=================================================================================================
** @Name      : void DiagSetErrFilStateAPI(e_ERR_ID id, e_ErrState state)
** @Input     : id:故障ID state:目标故障状态
** @Output    : void
** @Function  : 设置故障诊断滤波当前稳定状态
** @The notes : 只能eErr_A或eErr_B
**===============================================================================================*/
void DiagSetErrFilStateAPI(e_ERR_ID id, e_ErrState state)
{
	if((id < eERR_ID_FINISH) && (1 == gErrFilTab[id].enable)
		&& ((eErr_A == state) || (eErr_B == state)))
	{
	    /*检测是否可以设置恢复为A*/
	    if((255 != gErrFilPara[id].maxTimes)                /*非无限次恢复*/
	        && (eErr_B == gErrFilTab[id].filState)          /*正处于B状态*/
	        && (eErr_A == state))                           /*需要设置为A状态*/
	    {
	        if(gErrFilTab[id].resTimes >= gErrFilPara[id].maxTimes)/*达到最大恢复次数*/
	        {
	            return;                                     /*不能设置恢复为A*/
	        }
	    }

	    /*计算A->B的次数*/
	    if((eErr_A == DiagGetErrLastStateAPI(id))           /*正处于A状态*/
	        && (eErr_B == state))                           /*需要设置为B状态*/
	    {
	        if(gErrFilTab[id].resTimes < 254)               /*防止越界*/
	        {
	        	gErrFilTab[id].resTimes++;                  /*次数加1*/
	        }
	    }

		gErrFilTab[id].nowState = state;
		gErrFilTab[id].filState = state;
	}
}

/*=================================================================================================
** @Name      : void DiagErrDisplayTask(void)
** @Input     : void
** @Output    : void
** @Function  : 故障显示任务
** @The notes : 100ms周期,将滤波故障转入故障显示位
**===============================================================================================*/
void DiagErrDisplayTask(void)
{
	// u16 flag = 0;
	e_ERR_ID i = eERR_ID_FINISH;
	// e_ERR_ID index = eERR_ID_FINISH;
	// static u8 sFlag = 0;

    for(i = (e_ERR_ID)0; i < eERR_ID_FINISH; i++)           /*检测滤波最大个数*/
    {
		if(eErr_B == DiagGetErrLastStateAPI(i))             /*出现告警*/
		{
			DiagSetErrState(i, 1);                   /*设置产生异常状态*/
		}
		else                                                /*无异常*/
		{
			DiagSetErrState(i, 0);                   /*清除异常状态*/
		}
    }
}

/*=================================================================================================
** @Name      : e_ErrState DiagGetErrLastStateAPI(e_ERR_ID id)
** @Input     : id:故障ID
** @Output    : 当前稳定故障状态
** @Function  : 获取故障当前稳定状态
** @The notes : 不包括过渡状态
**===============================================================================================*/
e_ErrState DiagGetErrLastStateAPI(e_ERR_ID id)
{
	if(id < eERR_ID_FINISH)
	{
	    /*如果a to b 则返回a*/
		if(IsEqual(gErrFilTab[id].filState, eErr_AtoB))
		{
		    return(eErr_A);
		}
	    /*如果b to a 则返回b*/
		else if(IsEqual(gErrFilTab[id].filState, eErr_BtoA))
		{
		    return(eErr_B);
		}

		return(gErrFilTab[id].filState);
	}

	return(eErr_A);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=================================================================================================
** @Name      : static void DiagSetErrState(e_ERR_ID id, u8 state)
** @Input     : id:故障码ID state:设置状态:0清除状态 1:发生状态
** @Output    : void
** @Function  : 设置故障码状态
** @The notes : 清0或置1
**===============================================================================================*/
static void DiagSetErrState(e_ERR_ID id, u8 state)
{
    u8 index = 0;
    u8 expbit = 0;

	u8 Current_mode = GetCurrentSamplingMode();

	//故障屏蔽
	switch (Current_mode)
	{
		case 0://仅霍尔
			if ((id == eERR_ID_SHU_BSA)   /*电池端分流器采样值异常*/     
			|| (id == eERR_ID_SHU_DIF)    /*电池端分流器与霍尔采样偏差过大*/
		 	|| (id == eERR_ID_CUR_SAM ))  /*电池端分流器采样自检异常*/
			{
				return;
			}
		
			break;

		case 1://仅分流器
			if ((id == eERR_ID_HAL_OVE)	/*霍尔过载故障*/
			|| (id == eERR_ID_HAL_SUP)	/*霍尔供电异常*/
			||(id == eERR_ID_HAL_ADC)	/*霍尔AD值异常*/
			|| (id == eERR_ID_SHU_DIF)) /*电池端分流器与霍尔采样偏差过大*/
				{
					return;
				}
			break;

		case 2://霍尔和分流器
		/* */
		break;
               
	default:
		break;
	}




	if(id < eERR_ID_FINISH)                            /*故障ID(按位显示)*/
    {
        /*计算故障位*/
        index = (u32)id / FAULT_DATATYPE;/*获得故障id的数组子索引*/
        if(index >= FAULT_INDEX_LEN)                        /*故障数组子索引越界*/
        {
            return;                                         /*无效*/
        }

        expbit = id % FAULT_DATATYPE;                       /*获得故障相应位*/

        if(1 == state)                                      /*故障发生*/
        {
            BitSet(SystemDiag_57[index], expbit);         /*设置故障位状态置1*/
        }
        else
        {
        	BitClr(SystemDiag_57[index], expbit);          /*消除故障显示位状态清0*/
        }
    }
}


/*=================================================================================================
** @Name      : u8 GetCurrentSamplingMode(void)
** @Input     : void
** @Output    : 电流采样通道设置:0:HALL 1:分流器 2：HALL和分流器
** @Function  : 获取电流采样通道设置
** @The notes :
**===============================================================================================*/
u8 GetCurrentSamplingMode(void)
{ 
	return stParameter.u16ArraySet[CurrentSampChannel];
}
// /*=================================================================================================
// ** @Name      : static u8 DiagGetExpBefState(e_ERR_ID id)
// ** @Input     : id:故障码ID
// ** @Output    : 故障前置状态:0:正常 1:故障/告警
// ** @Function  : 获取故障前置状态
// ** @The notes : 当前显示状态
// **===============================================================================================*/
// static u8 DiagGetExpBefState(e_ERR_ID id)
// {
//     u8 state = 0;
//     u8 index = 0;
//     u8 expbit = 0;

//     if(id < eERR_ID_FINISH)                            //故障ID(按位显示)
//     {
//         //计算故障位
//         index = (u32)(id) / FAULT_DATATYPE;//获得故障id的数组子索引
//         if(index >= SystemDiag_Len)                        //故障数组子索引越界
//         {
//             return(0xff);                                   //返回无效
//         }

//         expbit = id % FAULT_DATATYPE;                       //获得故障相应位

//         state = (u8)BitGet(SystemDiag_57[index], expbit); //获得故障位状态
//     }
//     else                                                    //ID无效
//     {
//     	return(0xff);                                       //返回无效
//     }

//     return(state);
// }


// void DiagCheckCurrentDeviationAPI(s32 HallCurrent,s32 fShuntCurrent)
// {
	
	
	
// }