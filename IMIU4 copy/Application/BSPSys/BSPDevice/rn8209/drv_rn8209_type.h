/**
  ******************************************************************************
  * @file    drv_rn8209_type.h
  * @author  ZZC955
  * @version V1.0
  * @date    2023-9-9
  * @brief   rn8209读写等操作对外数据类型定义
  * @history 2023-9-9 ZZC955 创建
  ******************************************************************************
*/

#ifndef DRV_RN8209_TYPE_H
#define DRV_RN8209_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RN8209_STATUS_ACCESS_OK,
    RN8209_STATUS_ACCESS_FAIL,
    RN8209_NUM_STATUS,
} DRV_RN8209_StatusType;

#ifdef __cplusplus
}
#endif
#endif