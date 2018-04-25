#ifndef __HEAD_
#define __HEAD_

#define LOG_TAG "audio_hw_primary"
#define LOG_NDEBUG 1

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <utils/threads.h>
#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <cutils/properties.h>
#include <hardware/hardware.h>
#include <system/audio.h>
#include <hardware/audio.h>
#include <tinyalsa/asoundlib.h>
#include <audio_utils/resampler.h>
#include <audio_utils/echo_reference.h>
#include <audio_route/audio_route.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include "audio_iface.h"

#include <cutils/properties.h> 

#define F_LOG ALOGV("%s, line: %d", __FUNCTION__, __LINE__);

#define UNUSED(x) ((void)(x))

/* property define */
#define PRO_AUDIO_OUTPUT_ACTIVE     "audio.output.active"
#define PRO_AUDIO_INPUT_ACTIVE      "audio.input.active"
#define MIXER_USB_MIC_GAIN          "Mic Capture Volume"

/* sample rate define */
#define SAMPLING_RATE_8K	8000
#define SAMPLING_RATE_11K	11025
#define SAMPLING_RATE_16K	16000
#define SAMPLING_RATE_44K	44100
#define SAMPLING_RATE_48K	48000

/* basic define */
#define OUTPUT (2048) 
#define SHORT_PERIOD_SIZE OUTPUT
#define LONG_PERIOD_MULTIPLIER 6  
#define LONG_PERIOD_SIZE (SHORT_PERIOD_SIZE * LONG_PERIOD_MULTIPLIER)
#define PLAYBACK_PERIOD_COUNT 4
#define CAPTURE_PERIOD_COUNT 2

/* for resampler caculate */
#define RESAMPLER_BUFFER_FRAMES (SHORT_PERIOD_SIZE * 2)
#define RESAMPLER_BUFFER_SIZE (4 * RESAMPLER_BUFFER_FRAMES)

/* android default out sampling rate*/
#define DEFAULT_OUT_SAMPLING_RATE SAMPLING_RATE_48K
#define DEFAULT_IN_SAMPLING_RATE SAMPLING_RATE_48K

/* audio codec default sampling rate*/
#define MM_SAMPLING_RATE SAMPLING_RATE_16K

/*wifi display buffer size*/
#define AC100_XML_PATH "/system/etc/ac100_paths.xml"

typedef enum e_AUDIO_DEVICE_MANAGEMENT
{
    AUDIO_IN        = 0x01,
    AUDIO_OUT       = 0x02,
}e_AUDIO_DEVICE_MANAGEMENT;

/* define device num */
#define MAX_AUDIO_DEVICES   16
#define AUDIO_MAP_CNT   16

/* define nickname */
#define AUDIO_NAME_CODEC    "AUDIO_CODEC"
#define AUDIO_NAME_HDMI     "AUDIO_HDMI"
#define AUDIO_NAME_SPDIF    "AUDIO_SPDIF"
#define AUDIO_NAME_I2S      "AUDIO_I2S"
#define AUDIO_NAME_USB  	"AUDIO_USB"
#define AUDIO_NAME_MIX      "AUDIO_MIX"
#define AUDIO_NAME_BT       "AUDIO_BT"
#define AUDIO_NAME_NTP      "AUDIO_NTP"

/* card map */
#define NAME_LEN 128
#define PATH_LEN 128

/* for card map */
#define DISABLE 	-1
#define ACTIVE   	 1
#define UNINITIAL   -1

/* card strcut */
typedef struct map
{
	char name[NAME_LEN];		/* the name of card */
	char nickname[NAME_LEN];	/* the nickname of card */
	union 
	{
		int device_write;		/* write card num for H6 ahub only(fixed)*/	
		int device_read;		/* read card num for H6 ahub only(fixed)*/	
	};
	int card_num;				/* support card num for H6 ahub only */	
	int active;					/* active flag */
	int hub_flag;				/* if device is link to hub true for yes false for no*/
}card_str;
extern card_str audio_name_map_out[AUDIO_MAP_CNT];
extern card_str audio_name_map_in[AUDIO_MAP_CNT];

/* card config out&in*/
extern struct pcm_config pcm_config_out[AUDIO_MAP_CNT];
extern struct pcm_config pcm_config_in[AUDIO_MAP_CNT];

/* audio config out&in  */
extern struct audio_config audio_config_out;
extern struct audio_config audio_config_in;

/* resampler handler */
extern struct resampler_itfe *out_multi_resampler[AUDIO_MAP_CNT];
extern struct resampler_itfe *in_multi_resampler[AUDIO_MAP_CNT];

/* flag */
extern int out_standby_flag;
extern int in_standby_flag;
extern int bt_mode_flag;
extern int passthrough_mode_flag;

/* passthrough config */
extern struct pcm_config pcm_config_out_passthrough;

/* pcm strcut handler */
extern struct pcm* multi_pcm_out[AUDIO_MAP_CNT];
extern struct pcm* multi_pcm_in[AUDIO_MAP_CNT];

/* surpport card pcm strcut handler for H6 ahub only*/
extern struct pcm* multi_pcm_out_support[AUDIO_MAP_CNT];
extern struct pcm* multi_pcm_in_support[AUDIO_MAP_CNT];

/* HUB card num for H6 ahub only */
extern int HUB_NUM;

/* lock */
extern pthread_mutex_t out_standby_lock;
extern pthread_mutex_t in_standby_lock;

/* ac100 */
extern struct audio_route *ac100_routes;

enum {
    OUT_DEVICE_SPEAKER,
    OUT_DEVICE_BT_SCO,
    OUT_DEVICE_TAB_SIZE,           /* number of rows in route_configs[][] */
};

enum {
    IN_SOURCE_BTMIC,
    IN_SOURCE_TAB_SIZE,            /* number of lines in route_configs[][] */
};

#define media_speaker    "media-speaker"   /* OUT_DEVICE_SPEAKER */
#define media_bluetooth_sco "com-ap-bt"/* OUT_DEVICE_BT_SCO */
#define media_btmic "com-bt-ap"           /* IN_DEVICE_HEADSET */


/* default out card num */
#define DEFAULT_OUT 1

/* default in card num */
#define DEFAULT_IN 0

/* debug switch */
#define AW_HAL_DEBUG(...) do{if (aw_debug_en) {ALOGI(__VA_ARGS__);}}while(0);

/* dump switch */
#define PROPERTY_AUDIO_DUMP_OUT_PCM "persist.audio.dump_data.out"
#define PROPERTY_AUDIO_DUMP_IN_PCM "persist.audio.dump_data.in"

/* dump declaretion */
extern int aw_debug_en;
extern int aw_dump_output_en;
extern int aw_dump_input_en ;
extern FILE *aw_dump_in_pcm_fp;
extern FILE *aw_dump_out_pcm_fp ;
extern const char *dump_out_pcm_path;
extern const char *dump_in_pcm_path ;
/* default config */
extern struct pcm_config pcm_config_out_default;
extern struct pcm_config pcm_config_in_default;


/* karaok define begin */
#define KARAOK_FUN 0
#if KARAOK_FUN 
extern struct pcm* pcm_karaok_in;
extern struct pcm* pcm_karaok_out;
extern struct pcm* pcm_karaok_out_support;

#define VOICE_BUFF 8192*4

typedef struct voice_Data{
	int buffer_size;
	int total_size;
	int old_ptr;
	int new_ptr;
	void *buffer;
	int flag;
	pthread_mutex_t lock;
}voice_Data;

voice_Data voice_data;
#endif
/* karaok define end */

/* function delcaration head*/
int debug_mode(const char * pairs);
size_t get_buffer_size(uint32_t, int, int);
int set_usb_audio_active(int in_out, char *devices, int card_num);
int get_suitable_pcm_info(int card_num, int device, struct pcm_config* config, int in_out);
int set_internal_audio_devices_active(int in_out, char * devices);
void loop();
int aw_hal_dump_close();
int aw_hal_dump_out_pcm(struct audio_stream_out *stream, const void* buffer, size_t bytes);
int aw_hal_dump_in_pcm(struct audio_stream_in *stream, const void* buffer, size_t bytes);
void reset_all();
void disable_all_output();
void disable_all_input();
int init_card_config();
int init_card_map();
void karaok_start();
void karaok_init();
void karaok_close();
int karaok_open();
void mixer_open_hub();
size_t get_buffer_size(uint32_t sample_rate, int format, int channel_count);
int find_index_by_nickname(const char* name, int in_out);
int start_output_stream();
int start_input_stream();
int do_input_standby();
int do_output_standby();
ssize_t  out_to_karaok(const void* buffer, size_t bytes);
int hot_plug_process(char * devices);
ssize_t  pass_through_handler(struct audio_stream_out *stream, const void* buffer, size_t bytes);
int find_cardmap_index_by_nickname(const char* nickname, int in_out);
/* function delcaration tail */
#endif
