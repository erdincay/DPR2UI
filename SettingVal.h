#pragma once

/*
#define MAX_BUSES                  4        // 4 buses for switching
#define MAX_SWITCH_DI              8        // 8 bits
#define CH_ID_LEN                  41       // maximum 20 double-byte characters
#define MAX_PMUS			       1        //maixiang080713
#define MAX_DCS                3
#define AI_CHANNELS                96
#define DI_WORDS                   16
#define IEEE1344_ID_CODE_LEN       8
#define IEEE1344_NAME_LEN          16
#define MAX_GROUPS                 (AI_CHANNELS/3)  // A, B, C, 0 phases
#define MAX_LINES                  (MAX_GROUPS-1)   // one voltage, current for ther rest
#define BITS_IN_WORD               16
#define BITS_IN_DWORD              32
#define MAX_GENERATORS             2  // 2 groups(V,I) per generator
*/

struct LIMIT_SET_STRUCT
{
	float  low;   //下限
	float  high;   //上限
};

struct LINE_STRUCT
{
	unsigned short  v_group[4];
	unsigned short  i_group;
	unsigned short  switch_rule_di_idx[8];
	unsigned short  switch_rule_di_mask[4];
	unsigned short  switch_rule_di_status[4];
};

struct DI_SET_STRUCT
{
	unsigned int  negative_di_change;      //合到开
	unsigned int  positive_di_change;      //开到合
	unsigned int  di_normal;               // 
	unsigned int  dummy;                 
};

struct NET_TRIG_SET_STRUCT
{
	unsigned int    ipaddr;
	unsigned short  port;
	unsigned short  dummy;
	int             send_rate;                        // in 1/2 cycles. 0: Disable net trigger
};

struct IEEE1344_SET_STRUCT
{
	unsigned int    ipaddr;
	unsigned short  port;
	unsigned short  dummy;
	int             send_rate;                        // cycles per real-time phasor send
};

struct GENERATOR_SET_STRUCT
{
	char            dummy[41];                   // Generator ID
	unsigned short  line;                               // Line index
	unsigned short  tail_3U0;                           // Channel number tail 3U0
	unsigned short  tail_I_group;                       // Tail current
	unsigned short  U_pole_to_pole_excite;              // Channel number for U_pole_to_pole_excite
	unsigned short  U_Plus_excite;                      // Channel number for U+
	unsigned short  U_Minus_excite;                     // Channel number for U-
	unsigned short  dummy2;                            
	unsigned short  UF_ch;                              // Channel number for voltage of exciting
	unsigned short  IF_ch;                              // Channel number for current of exciting
	unsigned short  poles;                             
	unsigned short  GP_pulse;                           // 0 if GP pulse, 1 if angular vilocity pulse
	float           reverse_power;                      // Reverse power threshold
	float           reverse_reactive_power;
	float           overexcited;                          
	float           underexcited;                         
	float           rotator_unbalence_grounding;    
	float           negative_sequence_increase;
	float           head_tail_zero_sequence_3rd_harmonic_ratio;

	float           rated_CT;
	float           rated_PT;
	float           Xd;
	float           Xq;
};

struct SET_STRUCT
{
	// Configuration settings
	char                        stn[16+1];          // Station name
	char                        idcode[9];    // Station name
	char                        dummy2[15];               // Station name（实际长度多一个字节）
	unsigned short              pmu_idx;                           // PMU index
	unsigned short              sync_config;                       
	unsigned short              GPS_mode;                          
	unsigned short              GPS_baudrate;                          
	unsigned short              line_frequency;                    // Standard line frequency
	unsigned int                sample_rate;                       // Sample rate setting
	unsigned int                resamples_per_frame;               // ReSample rate
	unsigned int                filter_rate;                       // Filtering rate setting
	float                       rated_PT_voltage;                  // Standard PT voltage
	int                        UTC_time_zone_diff;                // Difference between local time and UTC in seconds
	unsigned char               channel_type[96];         // 通道类型
	float                       pt_ct_ratio[96];               // CT/PT变比
	char                        AI_channel_id[96][41];  // 通道名称
	char                        AI_group_id[32][41];     // 电压或电流组名
	char                        DI_channel_id[16*16][41]; // 开关量名称
	// Frequency settings       
	struct   LIMIT_SET_STRUCT   freq_dis_limit;    //频率
	struct   LIMIT_SET_STRUCT   freq_limit;        //频率上下限值
	float                       freq_change_rate;   //频率变化率, time frame: 5 cycles
	// AI settings              
	unsigned short              ai_channels;                       //实际模拟量数
	struct   LIMIT_SET_STRUCT   amplitude_limit[96];      //通道上下限值
	float                       amplitude_null[96];       // 通道零门槛
	float                       amplitude_change[96];     //通道突变量
	float                      slow_amplitude_change[96]; //缓变
	// AI group settings        
	unsigned short              total_groups;                      // 总电压、电流组数
	unsigned char               group[32][4];              // 电压电流组对应通道
	unsigned char               dummy1[32];                
	unsigned char               sel_group_1344[32];        // force include in 1344 packet 
	unsigned short              dummy4;      

	unsigned short              total_lines;                       
	struct LINE_STRUCT          line[31];                   // 3 phase voltage and 3 phase current
	float                       pos_null[32];              //正序零门槛
	struct   LIMIT_SET_STRUCT   posi_limit[32];            // 正序上下限
	float                       neg_limit[32];             // 负序上限
	float                       zero_limit[32];            // 零序上限
	// DI settings              
	unsigned short              di_words;                          // 开关量数(X32)
	struct   DI_SET_STRUCT      di[16/2];                    //开关量定值
	// Generator settings       
	unsigned short              total_generator_number;            // Total generator number, <= MAX_GENERATOR
	struct GENERATOR_SET_STRUCT generator[2];
	// Net trigger settings
	struct NET_TRIG_SET_STRUCT  net_trigger;                       // Nettrigger multicast ip/port 
	// Sample recording settings
	unsigned char               sample_storage_mode;               // CONTINUOUS or TRIGGERED
	char                        automatic_disk_clean_up;
	unsigned int                pre_fault_samples;                 // Pre-fault length in samples
	unsigned int                post_fault_quiet_samples;          // Post fault length in samples
	unsigned int                max_samples_per_file;              // Longest length in samples.
	unsigned int                max_resamples_per_file;            // Longest length in samples.
	// DC file transfer proxy ports
	unsigned short              dc_file_proxy_port;
	// DC settings
	struct IEEE1344_SET_STRUCT  dc[3];
	// DC down stream port number
	unsigned short              dc_down_stream_port;               
	unsigned char               recording_UTC_timimg;                // 1 if store in TUC, otherwise in Bejing time       
	unsigned char               dummy3;               

	unsigned char               sel_AI_1344[96];          // force include in 1344 packet 
	unsigned int                sel_di_1344[16/2];             // include in 1344 packet 
	int                         dummy[18];  // Reserved

	unsigned short              settings_struct_size;
	unsigned short              checksum, anti_checksum;
};

class CDPRDev;

class CSettingVal
{
protected:
	bool bInitSetting;
	CDPRDev * parentDev;
	WCHAR SettingCatalog[MAX_NAME_LENGTH];
	SET_STRUCT settingVal;
public:
	CSettingVal(void);
	CSettingVal(CDPRDev * ptr);
	~CSettingVal(void);
	void setParentDev(CDPRDev * ptr);
	const CDPRDev * getParentDev();
	WCHAR * getSettingCatalog(void);
	int setSettingCatalog(WCHAR * log);
	int LoadSettingCFG(CString fileName);
	int SaveSettingCFG(CString fileName);

	//SettingVal
	int getAIChannelSum();
	int setAIChannelSum(int val);
	const char * getAIChannelName(int index);
	int setAIChannelName(int index,const char * name);
	int getChannelType(int index);
	int setChannelType(int index,byte channelType);
	float getChannelPtCt(int index);
	int setChannelPtCt(INT index,float val);
	float getChannelUpperLimit(int index);
	float getChannelLowerLimit(int index);
	int setChannelUpperLimit(int index,float val);
	int setChannelLowerLimit(int index,float val);
	float getChannelAmplitudeChange(int index);
	int setChannelAmplitudeChange(int index,float val);
	float getChannelSlowAmplitudeChange(int index);
	int setChannelSlowAmplitudeChange(int index,float val);
	float getChannelAmplitudeNull(int index);
	int setChannelAmplitudeNull(int index,float val);
	float getFreqUpperLimit(int index = 0);
	int setFreqUpperLimit(int index = 0,float val = 0);
	float getFreqLowerLimit(int index = 0);
	int setFreqLowerLimit(int index = 0,float val = 0);
	float getFreqUpperDisLimit(int index = 0);
	int setFreqUpperDisLimit(int index = 0, float val = 0);
	float getFreqLowerDisLimit(int index = 0);
	int setFreqLowerDisLimit(int index = 0, float val = 0);
	float getFreqChangeRate(int index = 0);
	int setFreqChangeRate(int index = 0,float val = 0);
	int getDIChannelSum(void);
	int setDIChannelSum(int val);
	const char * getDIChannelName(int index);
	int setDIChannelName(int index,const char * name);
	int getDINegativeChangeVal(int index);
	int setDINegativeChangeVal(int index,bool val);
	int getDIPositiveChangeVal(int index);
	int setDIPositiveChangeVal(int index,bool val);
	int getGroupSum(void);
	int setGroupSum(int val);
	float getPosiNull(int index);
	int setPosiNull(int index, float val);
	float getPosiUpperLimit(int index);
	int setPosiUpperLimit(int index, float val);
	float getPosiLowerLimit(int index);
	int setPosiLowerLimit(int index, float val);
	float getNegaUpperLimit(int index);
	int setNegaUpperLimit(int index, float val);
	float getZeroUpperLimit(int index);
	int setZeroUpperLimit(int index, float val);
	const unsigned char * getGroupIDs(int index);
	int setGroupIDs(int index, const unsigned char * val);
	int setGroupID(int index,int secondaryIndex,byte val);
	const char * getGroupName(int index);
	int setGroupName(int index, const char * name);
	bool getbInitStruct(void);
	void setbInitStruct(bool val);
	int setCheckSum(WORD val);
	int getCheckSum();
	int setAnti_CheckSum(WORD val);
	int getAnti_CheckSum();
};
