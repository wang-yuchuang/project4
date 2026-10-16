

#include "dbconfig.h"
BYTE gSoftDI[64] = {0};
uint8_t PM_PCS_addr[ALLGRP] = {ADDR_PCS1,
                               ADDR_PCS2,
                               ADDR_PCS3,
                               ADDR_PCS4,
                               ADDR_PCS5};
#if (GRP_MPPT > 0)
uint8_t PM_MPPT_addr[ALLGRP] = {ADDR_MPPT1, ADDR_MPPT2, ADDR_MPPT3, ADDR_MPPT4, ADDR_MPPT5};
#endif
#if (GRP_B2B_CHG > 0)
uint8_t PM_B2B_Chg_addr[ALLGRP] = {ADDR_B2B_CHG1, ADDR_B2B_CHG2, ADDR_B2B_CHG3, ADDR_B2B_CHG4, ADDR_B2B_CHG5};
#endif
#if (GRP_B2B_DIS > 0)
uint8_t PM_B2B_Dischg_addr[ALLGRP] = {ADDR_B2B_DISCHG1, ADDR_B2B_DISCHG2, ADDR_B2B_DISCHG3, ADDR_B2B_DISCHG4, ADDR_B2B_DISCHG5};
#endif
#if (GRP_BIB2B > 0)
uint8_t PM_BIB2B_addr[ALLGRP] = {ADDR_BIB2B1, ADDR_BIB2B2, ADDR_BIB2B3, ADDR_BIB2B4, ADDR_BIB2B5};
#endif
#if (GRP_BIBUS2AC > 0)
uint8_t PM_BIBUS2AC_addr[ALLGRP] = {ADDR_BIBUS2AC1, ADDR_BIBUS2AC2, ADDR_BIBUS2AC3, ADDR_BIBUS2AC4, ADDR_BIBUS2AC5};
#endif
#if (GRP_BIBUS2DC > 0)
uint8_t PM_BIBUS2DC_addr[ALLGRP] = {ADDR_BIBUS2DC1, ADDR_BIBUS2DC2, ADDR_BIBUS2DC3, ADDR_BIBUS2DC4, ADDR_BIBUS2DC5};
#endif

uint8_t *PM_addr_table[biBus2DCOut] = {
    PM_PCS_addr,
#if (GRP_MPPT > 0)
    PM_MPPT_addr,
#else
    NULL,
#endif
#if (GRP_B2B_CHG > 0)
    PM_B2B_Chg_addr,
#else
    NULL,
#endif
#if (GRP_B2B_DIS > 0)
    PM_B2B_Dischg_addr,
#else
    NULL,
#endif
#if (GRP_BIB2B > 0)
    PM_BIB2B_addr,
#else
    NULL,
#endif
#if (GRP_BIBUS2AC > 0)
    PM_BIBUS2AC_addr,
#else
    NULL,
#endif
#if (GRP_BIBUS2DC > 0)
    PM_BIBUS2DC_addr;
#else
    NULL,
#endif
}
;

BYTE Name_Barcode[24];
BYTE Name_Mac[8];
BYTE Name_IP[16];
BYTE Name_SW_Ver[8];
BYTE Name_HW_Ver[8];

char *gESPCU_siglPMBarcode[MAX_PMDEV_GRP];
char *gESPCU_DevStr[5] = {
    Name_Barcode,
    Name_Mac,
    Name_IP,
    Name_SW_Ver,
    Name_HW_Ver};
_STRUCT_gESPCU_HeartBeat_Info_ gESPCU_HeartBeat_Info;
_STRUCT_gESCC_HeartBeat_Info_ gESCC_HeartBeat_Info;
_STRUCT_gESPCU_Gset_Info_ gESPCU_Gset_Info;

_STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_PCS_ModGrp_Info = {
    PCS,
};
#if (GRP_MPPT > 0)
_STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_MPPT_ModGrp_Info = {
    MPPT,
};
#endif
#if (GRP_B2B_CHG > 0)
_STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_B2B_CHGModGrp_Info = {
    sdB2B_Chg,
};
#endif
#if (GRP_B2B_DIS > 0)
_STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_B2B_DISModGrp_Info = {
    sdB2B_Dischg,
};
#endif
#if (GRP_BIB2B > 0)
_STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_BIB2B_ModGrp_Info = {
    biB2B,
};
#endif
#if (GRP_BIBUS2AC > 0)
_STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_BIBUS2AC_ModGrp_Info = {
    biBus2ACOut,
};
#endif
#if (GRP_BIBUS2DC > 0)
_STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_BIBUS2DC_ModGrp_Info = {
    biBus2DCOut,
};
#endif

_STRUCT_gESPCU_PWRModSig_Info_ gESPCU_PCS_ModSig_Info = {
    PCS,
};
#if (GRP_MPPT > 0)
_STRUCT_gESPCU_PWRModSig_Info_ gESPCU_MPPT_ModSig_Info = {
    MPPT,
};
#endif
#if (GRP_B2B_CHG > 0)
_STRUCT_gESPCU_PWRModSig_Info_ gESPCU_B2B_CHGModSig_Info = {
    sdB2B_Chg,
};
#endif
#if (GRP_B2B_DIS > 0)
_STRUCT_gESPCU_PWRModSig_Info_ gESPCU_B2B_DISModSig_Info = {
    sdB2B_Dischg,
};
#endif
#if (GRP_BIB2B > 0)
_STRUCT_gESPCU_PWRModSig_Info_ gESPCU_BIB2B_ModSig_Info = {
    biB2B,
};
#endif
#if (GRP_BIBUS2AC > 0)
_STRUCT_gESPCU_PWRModSig_Info_ gESPCU_BIBUS2AC_ModSig_Info = {
    biBus2ACOut,
};
#endif
#if (GRP_BIBUS2DC > 0)
_STRUCT_gESPCU_PWRModSig_Info_ gESPCU_BIBUS2DC_ModSig_Info = {
    biBus2DCOut,
};
#endif

_STRUCT_gESPCU_auxDevUpload_Info_ gESPCU_auxDevUpload_Info = {
    1,
};

_TAB_2ESCC_DATABASE_ gtabESCCDatabase[] = {
    //
    {NULL, &gESCC_HeartBeat_Info.Run_Statu, 0, DataType_int, "Run_Statu"},
    {NULL, &gESCC_HeartBeat_Info.OnOff_Statu, 0, DataType_int, "OnOff_Statu"},
    {NULL, &gESCC_HeartBeat_Info.Alm1_Sys, 0, DataType_int, "Alm1_Sys"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESCC_HeartBeat_Info.##desc, MAX_BAT_GRP, Data##type, #desc},
#define ESPCU_DBSTD_BMS_DATA
#include "ESPCU_dbstd.def"
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESCC_HeartBeat_Info.##desc, MAX_PCS_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PCS_ADJ
#include "ESPCU_dbstd.def"
    ESPCU_DBSTD_LASTFIELD};

#if (IMEU > 0)
_TAB_2ESCC_DATABASE_ gtabESCC_auxDevUpload_Database[] = {
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESPCU_auxDevUpload_Info.##desc, MAX_IMEU_GRP, Data##type, #desc},
#define ESPCU_DBSTD_AUX_DEV_UPLOAD
#include "ESPCU_dbstd.def"
    {NULL, NULL, 0, DataType_Ending, NULL}

};
#endif

_TAB_2ESCC_DATABASE_ gtabESPCU_HeartBeat_Database[] = {
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, &gESPCU_HeartBeat_Info.##desc, num, Data##type, #desc},
#define ESPCU_DBSTD_HEARTBEATESPCU
#include "ESPCU_dbstd.def"
    ESPCU_DBSTD_LASTFIELD};

_TAB_2ESCC_CTRLCORRES_ gtabESPCU_CtrlReq_Database[] = {
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {num, type, dev, #desc},
#define ESPCU_DBSTD_CTRLREQ
#include "ESPCU_dbstd.def"
    {NULL, NULL, NULL, NULL}

};

_TAB_2ESCC_DATABASE_ gtabESCC_PwrModGrp_Database[] = {
    ESPCU_DBSTD_LASTFIELD

    {NULL, &gESPCU_PCS_ModGrp_Info.PM_Tp, 0, DataType_float, "PM_Tp"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESPCU_PCS_ModGrp_Info.##desc, MAX_PMDEV_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PM_GRP_DATA
#include "ESPCU_dbstd.def"

#if (GRP_MPPT > 0)
    ESPCU_DBSTD_LASTFIELD

    {NULL, &gESPCU_MPPT_ModGrp_Info.PM_Tp, 0, DataType_float, "PM_Tp"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESPCU_MPPT_ModGrp_Info.##desc, MAX_PMDEV_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PM_GRP_DATA
#include "ESPCU_dbstd.def"

#endif

#if (GRP_B2B_CHG > 0)
    ESPCU_DBSTD_LASTFIELD

    {NULL, &gESPCU_B2B_CHGModGrp_Info.PM_Tp, 0, DataType_float, "PM_Tp"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, &gESPCU_B2B_CHGModGrp_Info.##desc, MAX_PMDEV_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PM_GRP_DATA
#include "ESPCU_dbstd.def"

#endif

#if (GRP_B2B_DIS > 0)
    ESPCU_DBSTD_LASTFIELD

    {NULL, &gESPCU_B2B_DISModGrp_Info.PM_Tp, 0, DataType_float, "PM_Tp"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESPCU_B2B_DISModGrp_Info.##desc, MAX_PMDEV_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PM_GRP_DATA
#include "ESPCU_dbstd.def"

#endif

#if (GRP_BIB2B > 0)
    ESPCU_DBSTD_LASTFIELD

    {NULL, &gESPCU_BIB2B_ModGrp_Info.PM_Tp, 0, DataType_float, "PM_Tp"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESPCU_BIB2B_ModGrp_Info.##desc, MAX_PMDEV_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PM_GRP_DATA
#include "ESPCU_dbstd.def"

#endif

#if (GRP_BIBUS2AC > 0)
    ESPCU_DBSTD_LASTFIELD

    {NULL, &gESPCU_BIBUS2AC_ModGrp_Info.PM_Tp, 0, DataType_float, "PM_Tp"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESPCU_BIBUS2AC_ModGrp_Info.##desc, MAX_PMDEV_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PM_GRP_DATA
#include "ESPCU_dbstd.def"

#endif

#if (GRP_BIBUS2DC > 0)
    ESPCU_DBSTD_LASTFIELD

    {NULL, &gESPCU_BIBUS2DC_ModGrp_Info.PM_Tp, 0, DataType_float, "PM_Tp"},
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, gESPCU_BIBUS2DC_ModGrp_Info.##desc, MAX_PMDEV_GRP, Data##type, #desc},
#define ESPCU_DBSTD_PM_GRP_DATA
#include "ESPCU_dbstd.def"

#endif
    {NULL, NULL, 0, DataType_Ending, "NULL"}};

_TAB_2ESCC_DATABASE_ gtabESPCU_GSetReq_Database[] = {
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD(num, desc, type, dev) {NULL, &gESPCU_Gset_Info.##desc, num, Data##type, #desc},
#define ESPCU_DBSTD_SETREQ
#include "ESPCU_dbstd.def"
    ESPCU_DBSTD_LASTFIELD};

static void dbprint_table(_TAB_2ESCC_DATABASE_ *pDatabase)
{

    while (pDatabase->pstring != NULL)
    {
        DBG_PRINTF("%s\t\t", pDatabase->pstring);
        switch (pDatabase->ucdatatype)
        {
        case DataType_int:
            DBG_PRINTF("%d\n", (int)*pDatabase->pdata);
            break;
        case DataType_float:
            DBG_PRINTF("%.3f\n", *pDatabase->pdata);
            break;
        case DataType_int_array:
            for (int i = 0; i < pDatabase->data.data_integer; i++)
            {
                DBG_PRINTF("%d\t", (int)pDatabase->pdata[i]);
            }
            DBG_PRINTF("\n");
            break;
        case DataType_float_array:
            for (int i = 0; i < pDatabase->data.data_integer; i++)
            {
                DBG_PRINTF("%.3f\t", pDatabase->pdata[i]);
            }
            DBG_PRINTF("\n");
            break;

        case DataType_string:
            break;
        }
        pDatabase++;
    }
}
void dbprint(void)
{
    //  DBG_PRINTF("================ESPCU_HeartBeat_Info================\n");
    // dbprint_table(gtabESPCU_HeartBeat_Database);
    // DBG_PRINTF("================ESCC_HeartBeat_Info================\n");
    // dbprint_table(gtabESCCDatabase);
    // DBG_PRINTF("================ESPCU_CtrlReq_Info================\n");
    // dbprint_table(gtabESPCU_CtrlReq_Database);
    DBG_PRINTF("================ESPCU_GSetReq_Info================\n");
    dbprint_table(gtabESPCU_GSetReq_Database);
#if (IMEU > 0)
    DBG_PRINTF("================ESPCU_auxDevUpload_Info================\n");
    dbprint_table(gtabESCC_auxDevUpload_Database);
#endif
}