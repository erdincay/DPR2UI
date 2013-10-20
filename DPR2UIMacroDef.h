/***********************************************************************************************************************************************************/
/**************************************************************程序自定义宏*********************************************************************************/
/***********************************************************************************************************************************************************/


/*************************************************************自定义字符**************************************************************************************/
#define COMM_STR_ETH "网络通讯"
#define COMM_STR_PHO "拨号通讯"

#define STR_CHANNEL_AC_I "交流电流"
#define STR_CHANNEL_AC_U "交流电压"
#define STR_CHANNEL_DC_I "直流电流"
#define STR_CHANNEL_DC_U "直流电压"
#define STR_CHANNEL_OTHER "其他通道"

#define STR_DI_YES "YES"
#define STR_DI_NO "NO"

#define STR_SETCFG_NAME "Settings.win"

#define STR_DIALUP_ENTRY "DPR2UI 拨号网关"

const int m_iChannelTypeSum = 5;
const CString m_strChannelType[m_iChannelTypeSum] = {_T(STR_CHANNEL_AC_I),_T(STR_CHANNEL_AC_U),_T(STR_CHANNEL_DC_I),_T(STR_CHANNEL_DC_U),_T(STR_CHANNEL_OTHER)};

const int m_iDIChoose = 2;
const CString m_strDIChoose[m_iDIChoose] = {_T(STR_DI_YES),_T(STR_DI_NO)};

//#define STR_DEF_DI_OPERATE    "自定义"
//#define STR_DI_ACTIVE_ALLDO   "合->分 全部启动"
//#define STR_DI_ACTIVE_ALLUNDO "合->分 全部取消"
//#define STR_DI_NAGETIVE_ALLDO "分->合 全部启动"
//#define STR_DI_NAGETIVE_ALLUNDO "分->合 全部取消"


/*************************************************************自定义常量**************************************************************************************/
const int MAX_STATION_SUM = 16;
const int MAX_DEV_SUM  =  8;
const int DEFAULT_PORT =  2408;
const int MAX_NAME_LENGTH = 60;
const int MAX_FILEBUF_LENGTH = 0x1000000;

const int MAX_AI_SUM = 96;
const int MAX_DI_SUM = 256;
const int MAX_FRE_SUM = 1;
const int MAX_GROUP_SUM = MAX_AI_SUM / 3;
const int MAX_GROUP_MEMBERS = 4;

const int MAX_MODEM_CONN = 20;

/*************************************************************自定义标识**************************************************************************************/
const int DEF_TYPE = 0;
const int COMM_TYPE_ETH = DEF_TYPE + 1;      //网络通讯
const int COMM_TYPE_PHO = DEF_TYPE + 2;      //拨号通讯

const int DEF_ERR = 100;
const int LESS_RECV_BYTE = DEF_ERR + 1;
const int DPR_ADDR_ERR   = DEF_ERR + 2;
const int INFO_NUM_ERR   = DEF_ERR + 3;
const int NO_SYN_HEAD    = DEF_ERR + 4;
const int PROTOCOL_NO_DATABASE_ERR = DEF_ERR + 5;
const int DATABASE_NO_PROTOCOL_ERR = DEF_ERR + 6;
const int PROTOCOL_COMM_ERR = DEF_ERR + 7;
const int STATION_INDEX_ERR = DEF_ERR + 8;
const int DEV_INDEX_ERR     = DEF_ERR + 9;
const int FAULTFILE_INDEX_ERR = DEF_ERR + 10;
const int FRAME_TYPE_ERR = DEF_ERR + 11;

const int DEF_FRAME = 200;
const int SET_TIME  = DEF_FRAME + 1;
const int CALL_FFI_MENU = DEF_FRAME + 2;
const int CALL_FFI_FILE = DEF_FRAME + 3;
const int CALL_PRIMARY_DATA = DEF_FRAME + 4;
const int CALL_STATION_CFG = DEF_FRAME + 5;
const int CALL_DEV_SETTING = DEF_FRAME + 6;
const int START_DEV_SETTING = DEF_FRAME + 7;
const int SEND_DEV_SETTING = DEF_FRAME + 8;
const int END_DEV_SETTING = DEF_FRAME + 9;
const int CALL_TIME = DEF_FRAME + 10;
const int TRIG_DEV_RECORD = DEF_FRAME + 11;

const int DEF_PROTOCOL = 300;
const int H103_PROTOCOL = DEF_PROTOCOL + 1;

const int DEF_ITEM_TYPE      = 400;
const int ROOT_ITEM_TYPE     = DEF_ITEM_TYPE + 1;
const int ID_ITEM_TYPE       = DEF_ITEM_TYPE + 2;
const int COMM_ITEM_TYPE     = DEF_ITEM_TYPE + 3;
const int DEV_ITEM_TYPE      = DEF_ITEM_TYPE + 4;
const int FAULT_ITEM_TYPE    = DEF_ITEM_TYPE + 5;
const int SETTING_ITEM_TYPE  = DEF_ITEM_TYPE + 6;
const int FILE_ITEM_TYPE     = DEF_ITEM_TYPE + 7;
const int MENU_ITEM_TYPE     = DEF_ITEM_TYPE + 8;
const int XML_ITEM_TYPE      = DEF_ITEM_TYPE + 9;
const int WIN_ITEM_TYPE      = DEF_ITEM_TYPE + 10;
const int AI_ITEM_TYPE       = DEF_ITEM_TYPE + 11;
const int DI_ITEM_TYPE       = DEF_ITEM_TYPE + 12;
const int FRE_ITEM_TYPE      = DEF_ITEM_TYPE + 13;
const int SEQ_ITEM_TYPE      = DEF_ITEM_TYPE + 14;
const int AI_VAL_ITEM        = DEF_ITEM_TYPE + 15;
const int DI_VAL_ITEM        = DEF_ITEM_TYPE + 16;
const int FRE_VAL_ITEM       = DEF_ITEM_TYPE + 17;
const int SEQ_VAL_ITEM       = DEF_ITEM_TYPE + 18;
const int OP_ITEM_TYPE       = DEF_ITEM_TYPE + 19;

const int CHANNEL_AC_I = 1;
const int CHANNEL_DC_I = 2;
const int CHANNEL_AC_U = 3;
const int CHANNEL_DC_U = 4;
const int CHANNEL_OTHER = 5;

const int INF_FILE_TYPE = 1;
const int CFG_FILE_TYPE = 2;
const int DAT_FILE_TYPE = 3;
const int SETTING_FILE_TYPE = 4;

const int NO_STATION_SEl   = -1;  //未选择子站
const int NO_DEV_SEL       = -1;  //未选择装置
const int NO_FAULTFILE_SEL = -1;  //未选择装置

/*************************************************************自定义消息**************************************************************************************/
const int DEF_MESSAGE = WM_USER;
const int SEND_FRAME_MSG = DEF_MESSAGE + 1;
const int RECV_FRAME_MSG = DEF_MESSAGE + 2;
const int WM_UPDATELIST  = DEF_MESSAGE + 3;
const int FAULT_MENU_MSG = DEF_MESSAGE + 4;
const int FAULT_FILE_MSG = DEF_MESSAGE + 5;
const int SETTING_FILE_MSG = DEF_MESSAGE + 6;
const int WM_PROGESS_START = DEF_MESSAGE + 7;
const int WM_PROGESS_STEP  = DEF_MESSAGE + 8;
const int WM_PROGESS_END   = DEF_MESSAGE + 9;
const int COMM_ACTIVE_MSG  = DEF_MESSAGE + 10;
const int WM_PATCH_PROCESS = DEF_MESSAGE + 11;
const int TRIG_RECORD_ACTIVE = DEF_MESSAGE + 12;
const int TRIG_RECORD_NEGATIVE = DEF_MESSAGE + 13;
const int CALL_TIME_ACTIVE = DEF_MESSAGE + 14;
const int CALL_TIME_NEGATINE = DEF_MESSAGE + 15;
const int SET_TIME_ACTIVE = DEF_MESSAGE + 16;
const int SET_TIME_NEGATIVE = DEF_MESSAGE + 17;
const int REFRESH_STATION_VIEW = DEF_MESSAGE + 18;
const int REFRESH_PROPERTY_VIEW = DEF_MESSAGE + 19;
const int SET_SETTING_ACTIVE = DEF_MESSAGE + 20;
const int SET_SETTING_NEGATIVE = DEF_MESSAGE + 21;

const DWORD Dword_mask[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,0x1000,0x2000,0x4000,0x8000,0x10000,0x20000,0x40000,0x80000,0x100000,0x200000,0x400000,0x800000,0x1000000,0x2000000,0x4000000,0x8000000,0x10000000,0x20000000,0x40000000,0x80000000};
