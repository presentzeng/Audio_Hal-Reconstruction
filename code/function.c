#include "head.h"

card_str audio_name_map_out[AUDIO_MAP_CNT] =
{
	{"sndhdmi",	         AUDIO_NAME_HDMI,    0, UNINITIAL,  ACTIVE,	 true}, 
	{"sndspdif",         AUDIO_NAME_SPDIF,  -1, UNINITIAL,  DISABLE, false},
	{"ac100sndcodec",    AUDIO_NAME_NTP,     2, UNINITIAL,  DISABLE, true},
	{"ac100sndcodec",    AUDIO_NAME_BT,      2, UNINITIAL,  DISABLE, true},
};

card_str audio_name_map_in[AUDIO_MAP_CNT] =
{
	{"snddaudio0",       AUDIO_NAME_I2S,     0, UNINITIAL,  DISABLE, true}, 
	{"sndspdif",         AUDIO_NAME_SPDIF,  -1, UNINITIAL,  DISABLE, false},
	{"ac100sndcodec",    AUDIO_NAME_BT,      2, UNINITIAL,  ACTIVE,  true},
};

/* card config out&in*/
struct pcm_config pcm_config_out[AUDIO_MAP_CNT];
struct pcm_config pcm_config_in[AUDIO_MAP_CNT];

/* audio config out&in  */
struct audio_config audio_config_out;
struct audio_config audio_config_in;

/* resampler handler */
struct resampler_itfe *out_multi_resampler[AUDIO_MAP_CNT];
struct resampler_itfe *in_multi_resampler[AUDIO_MAP_CNT];

/* flag */
int out_standby_flag = 1;
int in_standby_flag;
int bt_mode_flag;

/* pcm strcut handler */
struct pcm* multi_pcm_out[AUDIO_MAP_CNT];
struct pcm* multi_pcm_in[AUDIO_MAP_CNT];

/* surpport card pcm strcut handler for H6 ahub only*/
struct pcm* multi_pcm_out_support[AUDIO_MAP_CNT];
struct pcm* multi_pcm_in_support[AUDIO_MAP_CNT];

/* HUB card num for H6 ahub only */
int HUB_NUM = -1;
/* lock */
pthread_mutex_t out_standby_lock;
pthread_mutex_t in_standby_lock;

/* mode */
int mode;

/* dump */
int aw_debug_en = 0;
int aw_dump_output_en = 0;
int aw_dump_input_en = 0;
FILE *aw_dump_in_pcm_fp = NULL;
FILE *aw_dump_out_pcm_fp = NULL;
const char *dump_out_pcm_path = "/data/media/out_pcm_dump.pcm";
const char *dump_in_pcm_path = "/data/media/in_pcm_dump.pcm";

/* karaok pcm */
struct pcm* pcm_karaok_in;
struct pcm* pcm_karaok_out;
struct pcm* pcm_karaok_out_support;

const char * const playback_route_configs[OUT_DEVICE_TAB_SIZE] = {
	media_speaker,		/* OUT_DEVICE_SPEAKER */
	media_bluetooth_sco,	/* OUT_DEVICE_BT_SCO*/
};

const char * const capture_route_configs[OUT_DEVICE_TAB_SIZE] = {
	media_btmic,		/* IN_SOURCE_BTMIC */
};
/* ac100 */
struct audio_route *ac100_routes;

/* bluetooth pcm */
struct pcm *pcm_bt_in;
struct pcm *pcm_bt_out;
struct pcm *pcm_ntp;

/* default config */
struct pcm_config pcm_config_out_default = 
{
    .channels = 2,
    .rate = DEFAULT_OUT_SAMPLING_RATE ,
    .period_size = SHORT_PERIOD_SIZE,
    .period_count = PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_in_default = 
{
    .channels = 1,
    .rate = DEFAULT_IN_SAMPLING_RATE ,
    .period_size =  SHORT_PERIOD_SIZE,
    .period_count =  CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_ntp = 
{
    .channels = 2,
    .rate = DEFAULT_OUT_SAMPLING_RATE ,
    .period_size =  SHORT_PERIOD_SIZE,
    .period_count =  PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

/* blue tooth config */
struct pcm_config pcm_config_bt = 
{
    .channels = 1,
    .rate = SAMPLING_RATE_8K,
    .period_size =  SHORT_PERIOD_SIZE,
    .period_count =  CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

/* pass through config */
struct pcm_config pcm_config_out_passthrough;
int passthrough_mode_flag;


ssize_t  pass_through_handler(struct audio_stream_out *stream, const void* buffer, size_t bytes)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	void *buf = buffer;
	if(1)/*pass through hdmi */
	{
#if 0
		int card = find_cardmap_index_by_nickname(AUDIO_NAME_HDMI, AUDIO_OUT);
		if (pcm_config_out_passthrough.format == AUDIO_FORMAT_PCM_24_BIT_PACKED)
		{
			int idx = 0, jdx = 0;
			char* dst = NULL;
			char* src = NULL;
			char* buf_conv = NULL;
			int ch  = pcm_config_out_passthrough.channels;
			int write_len = out_frames * frame_size;
			int buf_conv_len = 0;
			int samples = write_len / audio_bytes_per_sample(out->format) / ch;
			buf_conv_len = write_len*4/3;
			buf_conv = (char*)malloc(buf_conv_len);
			if(buf_conv == NULL)
			{
				ALOGE("No memory while malloc conventer buffer...");
				return 0;
			}

			dst = (char*)buf_conv;
			for(idx = 0; idx < ch; idx++)
			{
				dst = ((char*)buf_conv) + 4*idx;
				src = ((char*)buf) + 3*idx;
				for(jdx = 0; jdx < samples; jdx++){
					dst[0] = src[0]&0xff;
					dst[1] = src[1]&0xff;
					dst[2] = src[2]&0xff;
					dst[3] = (dst[2]&0x80)?0xff:0x00;
					dst += 4*ch;
					src += 3*ch;
				}
			}
			ret = pcm_write(out->multi_pcm[card], (void *)buf_conv, buf_conv_len);
			if(buf_conv != NULL)
			{
				free(buf_conv);
				buf_conv = NULL;
			}
		}
		else
		{
			ret = pcm_write(out->multi_pcm[card], (void *)buf, out_frames * frame_size);
		}
#endif
	}
#if 0
	else if(card == adev->cardSPDIF && !strcmp(prop_value_card, PROP_RAWDATA_MODE_SPDIF_RAW))
	{
		if (pcm_config_out_passthrough.channels == 2)
		{
			if (pcm_config_out_passthrough.format == AUDIO_FORMAT_PCM_24_BIT_PACKED)
			{
				int idx = 0, jdx = 0;
				char* dst = NULL;
				char* src = NULL;
				char* buf_conv = NULL;
				int   ch  = out->multi_config[card].channels;
				int   write_len = out_frames * frame_size;
				int   buf_conv_len = 0;
				int  samples = write_len / audio_bytes_per_sample(out->format) / ch;
				buf_conv_len = write_len*4/3;

				buf_conv = (char*)malloc(buf_conv_len);
				if(buf_conv == NULL)
				{
					ALOGE("No memory while malloc conventer buffer...");
					return 0;
				}

				dst = (char*)buf_conv;
				for(idx = 0; idx < ch; idx++)
				{
					dst = ((char*)buf_conv) + 4*idx;
					src = ((char*)buf) + 3*idx;
					for(jdx = 0; jdx < samples; jdx++){
						dst[0] = src[0]&0xff;
						dst[1] = src[1]&0xff;
						dst[2] = src[2]&0xff;
						dst[3] = (dst[2]&0x80)?0xff:0x00;
						dst += 4*ch;
						src += 3*ch;
					}
				}
				ret = pcm_write(out->multi_pcm[card], (void *)buf_conv, buf_conv_len);
				if(buf_conv != NULL)
				{
					free(buf_conv);
					buf_conv = NULL;
				}
			}
			else
			{
				ret = pcm_write(out->multi_pcm[card], (void *)buf, out_frames * frame_size);
			}
		}
	}
#endif
	return 0;
}
/* for ac100 */
void set_ac100_path(struct audio_route *audio_routes, int output_path, int input_path)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	if (audio_routes)
	{
		audio_route_reset(audio_routes);
		if ((output_path >= 0) && (output_path < OUT_DEVICE_TAB_SIZE))
		{ audio_route_apply_path(audio_routes, playback_route_configs[output_path]); }
		if ((input_path >= 0) && (input_path < IN_SOURCE_TAB_SIZE))
		{ audio_route_apply_path(audio_routes, capture_route_configs[input_path]); }
		audio_route_update_mixer(audio_routes);
	}
}

int codec_route_init()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int ac100_card_num;
	int index = find_index_by_nickname(AUDIO_NAME_NTP, AUDIO_OUT);

	ac100_card_num = audio_name_map_out[index].card_num;
	if (ac100_card_num != -1) 
	{
		ac100_routes = audio_route_init(ac100_card_num, AC100_XML_PATH);
		set_ac100_path(ac100_routes, OUT_DEVICE_SPEAKER, -1);
	}
	return 0;
}

int find_cardmap_index_by_nickname(const char* nickname, int in_out)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	char path[PATH_LEN] = {0};
	int index = 0;
	struct snd_ctl_card_info card_info;
	int ret = -1;
	int fd = -1;
	if (in_out & AUDIO_IN) 
	{
		for(; index < AUDIO_MAP_CNT; index++)
		{
			if(strncmp(nickname, audio_name_map_in[index].nickname, strlen(nickname)) == 0)
			{return index;}
		}
	}
	if (in_out & AUDIO_OUT) 
	{
		for(; index < AUDIO_MAP_CNT; index++)
		{
			if(strncmp(nickname, audio_name_map_out[index].nickname, strlen(nickname)) == 0)
			{return index;}
		}
	}
	return -1;
}
int find_real_cardnum_by_name(const char* realname)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	char path[PATH_LEN] = {0};
	int index = 0;
	struct snd_ctl_card_info card_info;
	int ret = -1;
	int fd = -1;

	for(; index < AUDIO_MAP_CNT; index++)
	{
		memset(path, 0, PATH_LEN);
		memset(&card_info, 0, sizeof(struct snd_ctl_card_info));
		sprintf(path, "/dev/snd/controlC%u", index);

		fd = open(path, O_RDONLY, 0);
		if(fd < 0)
		{return -1;}

		ret = ioctl(fd, SNDRV_CTL_IOCTL_CARD_INFO, &card_info);
		if(ret < 0)
		{return -2;};

		if(strncmp(realname, card_info.id, strlen(realname)) == 0)
		{return index;}

	}
	return -1;
}

int get_card_info_by_index(struct snd_ctl_card_info *card_info, int index)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	char path[PATH_LEN] = {0};
	int ret = -1;
	int fd = -1;

	memset(path, 0, PATH_LEN);
	memset(card_info, 0, sizeof(struct snd_ctl_card_info));
	sprintf(path, "/dev/snd/controlC%u", index);
	
	fd = open(path, O_RDONLY, 0);
	if(fd < 0)
	{return -1;}

	ret = ioctl(fd, SNDRV_CTL_IOCTL_CARD_INFO, card_info);
	if(ret < 0)
	{
		close(fd);
		return -2;
	}

	close(fd);

	return -1;
}

/* get an available position for new card */
int get_index(int in_out)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;

	if (in_out & AUDIO_IN) 
	{
		for(; index <  MAX_AUDIO_DEVICES; index++)
		{
			if(strlen(audio_name_map_in[index].name) != 0)
			{ return index; }
		}
	}
	else if (in_out & AUDIO_OUT) 
	{
		for(index = 0; index <  MAX_AUDIO_DEVICES; index++)
		{
			if(strlen(audio_name_map_out[index].name) != 0)
			{ return index; }
		}
	}
	return -1;
}

/* print card info */
int print_get_pcm_info(int card_num, int device, struct pcm_config* config, int in_out)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	if (config != NULL)
	{
		ALOGE("Card[%d-%d] %s", card_num, device, (in_out & AUDIO_IN) ? "Capture" : "Playback");
		ALOGE("        Rate:\t%uHz", config->rate);
		ALOGE("    Channels:\t%u", config->channels);
		ALOGE(" Sample bits:\t%ubits", pcm_format_to_bits(config->format));
		ALOGE(" Period size:\t%u", config->period_size);
		ALOGE("Period count:\t%u", config->period_count);
	}
	else
	{
		ALOGE("pcm_config:NULL, cannot print get pcm_config info");
		return -1;
	}
	return 0;
}

/* print map info */
int print_audio_map_info(card_str *card_map, int in_out)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	if (card_map != NULL)
	{
		ALOGE("Card[%d-%d] %s", card_map->card_num,
					((in_out & AUDIO_IN) ? card_map->device_read : card_map->device_write),
					((in_out & AUDIO_IN) ? "Capture" : "Playback"));
		ALOGE("	   name:\t%s", card_map->name);
		ALOGE("nickname:\t%s", card_map->nickname);
		ALOGE("  active:\t%d", card_map->active);
		ALOGE("hub_flag:\t%d", card_map->hub_flag);
	}
	else
	{
		ALOGE("card_map:NULL, cannot print audio map info");
		return -1;
	}
	return 0;
}

/* print card info */
int card_info_list()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	ALOGE("------> %s <------", __func__);
	for (index = 0; index < AUDIO_MAP_CNT; index++)
	{
		if (strlen(audio_name_map_out[index].name))
		{
			print_audio_map_info(&audio_name_map_out[index], AUDIO_OUT);
			print_get_pcm_info(audio_name_map_out[index].card_num,
						audio_name_map_out[index].device_write,  &pcm_config_out[index], AUDIO_OUT);
		}
	}
	for (index = 0; index < AUDIO_MAP_CNT; index++)
	{
		if (strlen(audio_name_map_in[index].name))
		{
			print_audio_map_info(&audio_name_map_in[index], AUDIO_IN);
			print_get_pcm_info(audio_name_map_in[index].card_num,
						audio_name_map_in[index].device_read,  &pcm_config_in[index], AUDIO_IN);
		}
	}
	return 0;
}


int set_usb_audio_active(int in_out, char *devices, int card_num)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	/*disable_all();*/
	int index = 0;

	if (in_out & AUDIO_IN) 
	{
		index = get_index(AUDIO_IN);
		if(index == -1)
		{ return -1; }
		strcpy(audio_name_map_in[index].name, devices);
		audio_name_map_in[index].active = ACTIVE;
		audio_name_map_in[index].card_num = card_num;
		audio_name_map_in[index].hub_flag = false;
		strcpy(audio_name_map_in[index].nickname, AUDIO_NAME_USB);
		pcm_config_in[index] = pcm_config_in_default;
		get_suitable_pcm_info(audio_name_map_in[index].card_num, 0, &pcm_config_in[index], AUDIO_IN);
		/* log show config card name */
		print_get_pcm_info(audio_name_map_in[index].card_num, 0, &pcm_config_in[index], AUDIO_IN);
		return 0;
	}

	if (in_out & AUDIO_OUT) 
	{
		index = get_index(AUDIO_OUT);
		if(index == -1)
		{ return -1; }
		
		strcpy(audio_name_map_out[index].name, devices);
		audio_name_map_out[index].active = ACTIVE;
		audio_name_map_out[index].card_num = card_num;
		audio_name_map_out[index].hub_flag = false;
		strcpy(audio_name_map_out[index].nickname, AUDIO_NAME_USB);
		pcm_config_out[index] = pcm_config_out_default;
		get_suitable_pcm_info(audio_name_map_out[index].card_num,0, &pcm_config_out[index], AUDIO_OUT);
		/* log show config card name */
		print_get_pcm_info(audio_name_map_out[index].card_num, 0, &pcm_config_out[index], AUDIO_OUT);
		return 0;
	}
	return -1;
}

/* set internal audio device which means it is already registed in map */
int set_internal_audio_devices_active(int in_out, char * devices)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int cnt = 0;
	char name[8][32];
	char str[128];
	strcpy(str, devices);
	char * pval = str;
	char *seps = " ,";

	pval = strtok(pval, seps);
	while (pval != NULL) 
	{
		strcpy(name[cnt++], pval);
		pval = strtok(NULL, seps);
	}
	
	if (in_out & AUDIO_IN) 
	{
		int i = 0;
		for (; i < cnt; i++) 
		{
			int index = 0;
			for(;index < MAX_AUDIO_DEVICES; index++)
			{
				if(strcmp(audio_name_map_in[index].nickname, name[i]) == 0)
				{ 
					audio_name_map_in[index].active = ACTIVE;
					/* log show */
					goto find_in;
				}
			}
		}
find_in:
		if(index == MAX_AUDIO_DEVICES) 
		{ ALOGE("set %s as input failed, nothing change\n", devices); }
		else
		{
			int ret = property_set(PRO_AUDIO_INPUT_ACTIVE, devices);
			if (ret < 0) 
			{ ALOGE("set property %s: %s failed", PRO_AUDIO_INPUT_ACTIVE, devices); } 
			else
			{ ALOGV("set property %s: %s ok", PRO_AUDIO_INPUT_ACTIVE, devices); }
			return 0;
		}
	}

	if (in_out & AUDIO_OUT) 
	{
		int i = 0;
		for (; i < cnt; i++) 
		{
			int index = 0;
			for(;index < MAX_AUDIO_DEVICES; index++)
			{
				if(strcmp(audio_name_map_out[index].nickname, name[i]) == 0)
				{ 
					audio_name_map_out[index].active = ACTIVE;
					/* log show */
					goto find_out;
				}
			}
		}
find_out:
		if(index == MAX_AUDIO_DEVICES) 
		{ ALOGE("set %s as output failed, nothing change\n", devices); }
		else
		{
			int ret = property_set(PRO_AUDIO_OUTPUT_ACTIVE, devices);
			if (ret < 0) 
			{ ALOGE("set property %s: %s failed", PRO_AUDIO_OUTPUT_ACTIVE, devices); }
			else 
			{ ALOGV("set property %s: %s ok", PRO_AUDIO_OUTPUT_ACTIVE, devices); }
			return 0;
		}
	}
	return -1;
}

int hot_plug_process(char * devices)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	struct str_parms *parms;
	char value[32];
	int ret;

	parms    = str_parms_create_str(devices);
	ret = str_parms_get_str(parms,  AUDIO_PARAMETER_DEVICES_IN_ACTIVE, value, sizeof(value));
	if(strncmp(AUDIO_NAME_USB, value, strlen(AUDIO_NAME_USB)) == 0)
	{
		int index = find_real_cardnum_by_name(value+10);
		ALOGE("in usb index %d\n", index);
		/* usb device */
		set_usb_audio_active(AUDIO_IN, value, index);
#if KARAOK_FUN 
		karaok_open();
		karaok_start();
#endif
		return 0;
	}

	memset(value, 0, 32);
	ret = str_parms_get_str(parms,  AUDIO_PARAMETER_DEVICES_OUT_ACTIVE, value, sizeof(value));
	if(strncmp(AUDIO_NAME_USB, value, strlen(AUDIO_NAME_USB)) == 0)
	{
		int index = find_real_cardnum_by_name(value+10);
		/* usb device */
		ALOGE("out usb %s, index %d\n", value, index);
		set_usb_audio_active(AUDIO_OUT, value, index);
		out_standby_flag = 0;
		do_output_standby();
		return 0;
	}

	memset(value, 0, 32);
	ret = str_parms_get_str(parms,  AUDIO_PARAMETER_DEVICES_OUT_ACTIVE, value, sizeof(value));
	if(strncmp(AUDIO_NAME_HDMI, value, strlen(AUDIO_NAME_HDMI)) == 0)
	{
		ALOGE("out hdmi %s\n", value);
		set_internal_audio_devices_active(AUDIO_OUT, value);
		out_standby_flag = 0;
		do_output_standby();
	}
	if(strncmp(AUDIO_NAME_CODEC, value, strlen(AUDIO_NAME_CODEC)) == 0)
	{
		ALOGE("in codec\n");
		set_internal_audio_devices_active(AUDIO_IN, AUDIO_NAME_CODEC);
#if KARAOK_FUN 
		karaok_close();
#endif
		return 0;
	}
	return 0;
}

/* karaok thread */
void loop()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
#if KARAOK_FUN 
	unsigned int size =  2048;
	char * usb_buffer = malloc(size);
	char * usb_buffer_2ch = malloc(2 * size);
	char * mixbuffer = malloc(2 * size);
	short *it1, *it2, *it3;
	int index;
	while (!pcm_read_ex(pcm_karaok_in, usb_buffer, size)) 
		{
			char* itsrc = usb_buffer;
			char* itdest = usb_buffer_2ch;
			index = 0;
			while(index < 2*size)
			{
				memcpy(itdest, itsrc, 2);
				itsrc+=2;
				itdest+=2;

				memcpy(itdest, itsrc, 2);
				itdest+=2;
				index+=2;
			}
			index = 0;
			it1 = usb_buffer_2ch;
			it3 = mixbuffer;
			pthread_mutex_lock(&voice_data.lock);

			/* get buffer size */
			if(voice_data.buffer_size > 0)
			{
				it2 = voice_data.buffer + voice_data.old_ptr;
				/* buffer mix */
				while(index < size )
				{
					it3[index] = (it1[index] + it2[index])/2;
					index++;
				}
				voice_data.old_ptr = (voice_data.old_ptr + 2*size)%voice_data.total_size;
				voice_data.buffer_size -= 2*size;
				pthread_mutex_unlock(&voice_data.lock);

				/* write mix buffer */
				pcm_write(pcm_karaok_out, mixbuffer, 2*size);
			}
			else
			{
				pcm_write(pcm_karaok_out, usb_buffer_2ch, 2*size);
				pthread_mutex_unlock(&voice_data.lock);
			}
		}
#endif	
}
/* karaok thread end */

/* hal dump begin */
int aw_hal_dump_close()
{
	if (aw_dump_out_pcm_fp) {
		fclose(aw_dump_out_pcm_fp);
		aw_dump_out_pcm_fp = NULL;
	}

	if (aw_dump_in_pcm_fp) {
		fclose(aw_dump_in_pcm_fp);
		aw_dump_in_pcm_fp = NULL;
	}

	return 0;
}

int aw_hal_dump_out_pcm(struct audio_stream_out *stream,
		const void* buffer, size_t bytes)
{
	/* debug */
	/* aw_dump_output_en = property_get_bool(PROPERTY_AUDIO_DUMP_OUT_PCM, false); */
	if (aw_dump_output_en && !aw_dump_out_pcm_fp) {
		/* open */
		aw_dump_out_pcm_fp = fopen(dump_out_pcm_path, "a+");
		if(!aw_dump_out_pcm_fp)
		{
			ALOGE("Unable to create file '%s', errno: '%d'\n",
					dump_out_pcm_path, errno);
			return -1;
		}
	}

	if (aw_dump_output_en && aw_dump_out_pcm_fp) {
		if (fwrite(buffer, 1, bytes, aw_dump_out_pcm_fp) != bytes) {
			fprintf(stderr,"[%s] Error fwrite! '%s'\n",
					__FUNCTION__, dump_out_pcm_path);
		}
	}

	if (!aw_dump_output_en) {
		aw_hal_dump_close();
	}
	return 0;
}

int aw_hal_dump_in_pcm(struct audio_stream_in *stream,
		const void* buffer, size_t bytes)
{
	aw_dump_input_en = property_get_bool(PROPERTY_AUDIO_DUMP_IN_PCM, false);
	if (aw_dump_input_en && !aw_dump_in_pcm_fp) {
		/* open */
		aw_dump_in_pcm_fp = fopen(dump_in_pcm_path, "a+");
		if(!aw_dump_in_pcm_fp)
		{
			fprintf(stderr,"Unable to create file '%s'\n",
					dump_in_pcm_path);
			return -1;
		}
	}

	if (aw_dump_input_en && aw_dump_in_pcm_fp) {
		if (fwrite(buffer, 1, bytes, aw_dump_in_pcm_fp) != bytes) {
			fprintf(stderr,"[%s] Error fwrite! '%s'\n",
				__FUNCTION__, dump_in_pcm_path);
		}
	}

	if (!aw_dump_input_en) {
		aw_hal_dump_close();
	}
	return 0;
}
/* hal dump end */

/* get max card info */
int get_max_info(int card_num, struct pcm_config* config, int in_out)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	struct pcm_params *params;
	unsigned int min = 0;
	unsigned int max = 0;

	params = pcm_params_get(card_num, 0, in_out == 0 ? PCM_OUT : PCM_IN);
	if (params == NULL)
	{
		AW_HAL_DEBUG("Not exist the card[%d]", card_num);
		return -1;
	}
	min = pcm_params_get_min(params, PCM_PARAM_RATE);
	max = pcm_params_get_max(params, PCM_PARAM_RATE);

	config->rate = pcm_params_get_max(params, PCM_PARAM_RATE);
	config->channels = pcm_params_get_max(params, PCM_PARAM_CHANNELS);
	config->format = pcm_params_get_max(params, PCM_PARAM_SAMPLE_BITS);
	config->period_size = pcm_params_get_max(params, PCM_PARAM_PERIOD_SIZE);
	config->period_count = pcm_params_get_max(params, PCM_PARAM_PERIODS);
	return 0;
}

/* get suitable card info */
int get_suitable_pcm_info(int card_num, int device, struct pcm_config* config, int in_out)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	struct pcm_params *params;
	unsigned int min = 0;
	unsigned int max = 0;
	unsigned int index = 0;
	unsigned int temp_pcm_param = 0;
	unsigned int temp_config_member = 0;

	params = pcm_params_get(card_num, device, (in_out & AUDIO_OUT) ? PCM_OUT : PCM_IN);
	if (params == NULL)
	{
		AW_HAL_DEBUG("Not exist the card[%d]", card_num);
		return -1;
	}

	for (index = 0; index < 5; index++)
	{
		switch (index)
		{
			case 0:
				temp_pcm_param = PCM_PARAM_RATE;
				temp_config_member = config->rate;
			break;
			case 1:
				temp_pcm_param = PCM_PARAM_CHANNELS;
				temp_config_member = config->channels;
			break;
			case 2:
				temp_pcm_param = PCM_PARAM_SAMPLE_BITS;
				temp_config_member = config->format;
			break;
			case 3:
				temp_pcm_param = PCM_PARAM_PERIOD_SIZE;
				temp_config_member = config->period_size;
			break;
			case 4:
				temp_pcm_param = PCM_PARAM_PERIODS;
				temp_config_member = config->period_count;
			break;
			default:
				AW_HAL_DEBUG("not exist the pcm_params, cannot get info!");
				pcm_params_free(params);
				return -1;
			break;
		}
		min = pcm_params_get_min(params, temp_pcm_param);
		max = pcm_params_get_max(params, temp_pcm_param);
		if (min > temp_config_member)
		{
			temp_config_member = min;
		}
		else if (max < temp_config_member)
		{
			temp_config_member = max;
		}
	}
	pcm_params_free(params);

	return 0;
}

/* reset all */
void reset_all()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	for(; index < AUDIO_MAP_CNT; index++)
	{
		audio_name_map_out[index].card_num = UNINITIAL;
		audio_name_map_in[index].card_num = UNINITIAL;
	}
}
/* disable all */
void disable_all_output()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	for(; index < AUDIO_MAP_CNT; index++)
	{
		audio_name_map_out[index].active = DISABLE;
	}
}
void disable_all_input()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	for(; index < AUDIO_MAP_CNT; index++)
	{
		audio_name_map_in[index].active = DISABLE;
	}
}
/* init card config */
int init_card_config()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	for(; index < AUDIO_MAP_CNT; index++)
	{
		pcm_config_out[index] = pcm_config_out_default;
		pcm_config_in[index] = pcm_config_in_default;
		multi_pcm_out[index] = NULL;
		multi_pcm_in[index] = NULL;
	}
	return 0;
}

int card_map_check_for_usb(char * devices, int card_num)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	char path[PATH_LEN] = {0};
	int fd = -1;

	sprintf(path, "/proc/asound/card%d/usbid", card_num);
	fd = open(path, O_RDONLY, 0);
	if(fd < 0)
	{}
	else /* usb device */
	{
		sprintf(path, "/proc/asound/card%d/pcm0p", card_num);
		fd = open(path, O_RDONLY, 0);
		if(fd < 0)
		{}
		else
		{ 
			ALOGE("three\n");
			set_usb_audio_active(AUDIO_OUT, devices, card_num);
			close(fd);
		}

		sprintf(path, "/proc/asound/card%d/pcm0c", card_num);
		fd = open(path, O_RDONLY, 0);
		if(fd < 0)
		{}
		else
		{ 
			set_usb_audio_active(AUDIO_IN, devices, card_num);
			close(fd);
		}
	}
	return 0;
}

/* init card map */
int init_card_map()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	char path[PATH_LEN] = {0};
	char name[NAME_LEN] = {0};
	int index = 0;
	int ret = -1;
	int fd = -1;

	for(; index < AUDIO_MAP_CNT; index++)
	{
		memset(path, 0, PATH_LEN);
		memset(name, 0, NAME_LEN);
		sprintf(path, "/proc/asound/card%d/id", index);

		fd = open(path, O_RDONLY, 0);
		if(fd < 0)
		{return -1;}

		ret = read(fd, name, NAME_LEN);
		if(ret < 0)
		{return -2;};
		
		card_map_check_for_usb(name, index);
		/* get hub number for H6 ahub only */
		if(strncmp("sndahub", name, strlen("sndahub")) == 0)
		{ HUB_NUM = index; }

		int innerindex = 0;
		for(; innerindex < AUDIO_MAP_CNT; innerindex++)
		{
			/* search for card num */
			if((strlen(audio_name_map_out[innerindex].name) != 0)
				&& (strncmp(audio_name_map_out[innerindex].name, name, strlen(audio_name_map_out[innerindex].name)) == 0))
			{ audio_name_map_out[innerindex].card_num = index; }
			else
			{}
		}
        
		for(innerindex = 0; innerindex < AUDIO_MAP_CNT; innerindex++)
		{
			/* search for card num */
			if((strlen(audio_name_map_in[innerindex].name) != 0)
				&& (strncmp(audio_name_map_in[innerindex].name, name, strlen(audio_name_map_in[innerindex].name)) == 0))
			{ audio_name_map_in[innerindex].card_num = index; }
			else
			{}
		}
	close(fd);
    }
    return 1;
}

/* create and start karaok thread */
void karaok_start()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
#if KARAOK_FUN 
	pthread_t thread;
	pthread_attr_t attr;
	struct sched_param sched = {0};
	sched.sched_priority = ANDROID_PRIORITY_AUDIO;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
	pthread_attr_setschedparam(&attr, &sched);
	if(pthread_create(&thread, &attr, loop, NULL))
	{
            ALOGE("%s() pthread_create loop failed!!!", __func__);
            pthread_attr_destroy(&attr);
	}
#endif
}
void karaok_close()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	do_output_standby();
	do_input_standby();
}
void karaok_init()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
#if KARAOK_FUN 
	/* init ring buffer */
	voice_data.buffer = (void*)malloc(VOICE_BUFF);
	voice_data.total_size = VOICE_BUFF;
	voice_data.buffer_size = 0;
	voice_data.new_ptr = voice_data.old_ptr = 0;
#endif
}
int karaok_open()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
#if KARAOK_FUN 
	/* input card select  */
	int index = 0;
	for(; index <  MAX_AUDIO_DEVICES; index++)
	{
		if(strncmp(AUDIO_NAME_USB, audio_name_map_in[index].nickname, strlen(AUDIO_NAME_USB)) == 0
				&& audio_name_map_in[index].active == ACTIVE)
		{ break; }
	}

	/* couldnot find */
	if(index == MAX_AUDIO_DEVICES)
	{ return -1; }

	ALOGE("index %d, card_num %d\n", index, audio_name_map_in[index].card_num);
	/* input card open */
	pcm_karaok_in = pcm_open(audio_name_map_in[index].card_num, 0, PCM_IN, &pcm_config_in[index]);

	/* output card select */
	index = find_index_by_nickname(AUDIO_NAME_HDMI, AUDIO_OUT);
	if(index != -1)
	{
		/*disable_all();*/
		audio_name_map_out[index].active = ACTIVE;
		if(audio_name_map_out[index].hub_flag == true)
		{
			/* output card open */
			pcm_karaok_out = pcm_open(HUB_NUM, audio_name_map_out[index].device_write, PCM_OUT, &pcm_config_out_default);

			pcm_karaok_out_support = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_out_default);
			pcm_prepare(pcm_karaok_out);
			pcm_prepare(pcm_karaok_out_support);
		}
		else
		{
			pcm_karaok_out = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_out_default);
		}
	}
#endif
	return 0;
}

int find_index_by_nickname(const char* name, int in_out)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	for (; index < AUDIO_MAP_CNT; index++)
	{
		if (in_out & AUDIO_IN) 
		{
			if(strncmp(audio_name_map_in[index].nickname, name, strlen(audio_name_map_in[index].nickname)) == 0)
			{ return index; }
		}
		if (in_out & AUDIO_OUT) 
		{
			if(strncmp(audio_name_map_out[index].nickname, name, strlen(audio_name_map_out[index].nickname)) == 0)
			{ return index; }
		}
	}
	return -1;
}

void mixer_open_hub()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	/* open hub */
	struct mixer *mixer = mixer_open(HUB_NUM);
	struct mixer_ctl *ctl;

	/* i2s0->APBIF_RXDIF0 */
	ctl = mixer_get_ctl_by_name(mixer, "APBIF0 Src Select");
	mixer_ctl_set_value(ctl, 0, 4);

	/* enble i2s0 in */
	ctl = mixer_get_ctl_by_name(mixer, "I2S0IN Switch");
	mixer_ctl_set_value(ctl, 0, 1);

	/* APBIF_RXDIF0->i2s1 */ 
	ctl = mixer_get_ctl_by_name(mixer, "I2S1 Src Select");
	mixer_ctl_set_value(ctl, 0, 1);

	/* enable i2s1 out */
	ctl = mixer_get_ctl_by_name(mixer, "I2S1OUT Switch");
	mixer_ctl_set_value(ctl, 0, 1);

	/* enable ntp out */
	ctl = mixer_get_ctl_by_name(mixer, "I2S2 Src Select");
	mixer_ctl_set_value(ctl, 0, 3);

	/* enable i2s2 out */
	ctl = mixer_get_ctl_by_name(mixer, "I2S2OUT Switch");
	mixer_ctl_set_value(ctl, 0, 1);
}

size_t get_buffer_size(uint32_t sample_rate, int format, int channel_count)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	size_t size;
	size_t device_rate;
#if 0
	if (check_input_parameters(sample_rate, format, channel_count) != 0)
	{return 0;}
#endif

	size = ((pcm_config_in_default.period_size + 15) / 16 ) * 16;
	return size * channel_count * sizeof(short);
}

int start_output_stream()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index;

	/* enable pass through mode */
	if(passthrough_mode_flag  == 1)
	{
		 index = find_cardmap_index_by_nickname(AUDIO_NAME_HDMI, AUDIO_OUT);
		 disable_all_output();
		 audio_name_map_out[index].active = ACTIVE;
		 pcm_config_out[index].raw_flag = 1;
		 /* open card*/
		 multi_pcm_out[index] = pcm_open(HUB_NUM,  audio_name_map_out[index].device_write, PCM_OUT, &pcm_config_out[index]);
		 /*open support card for H6 ahub only */
		 multi_pcm_out_support[index] = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_out[index]);
		 return 0;
	}
	for (index = 0; index < AUDIO_MAP_CNT; index++)
	{
		/* select */
		ALOGE("check %s to playback audio, active is %d\n", audio_name_map_out[index].name, audio_name_map_out[index].active );
		if(audio_name_map_out[index].active == ACTIVE)
		{
			ALOGV("use %s to playback audio, device is %d\n", audio_name_map_out[index].name, audio_name_map_out[index].device_write);

			/* check if device is link to hub or not */
			if(audio_name_map_out[index].hub_flag == true)
			{
				/* for ac100 */
				if (strncmp(audio_name_map_out[index].nickname, AUDIO_NAME_BT, strlen(audio_name_map_out[index].nickname)) == 0)
				{
#if 0
					pcm_bt_out = pcm_open(audio_name_map_out[index].card_num, audio_name_map_out[index].device_read, PCM_IN, &pcm_config_bt);
					multi_pcm_out_support[index] = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_bt);
#endif
				}
				else
				{
					/*open support card for H6 ahub only */
					multi_pcm_out_support[index] = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_out[index]);
				}

				/* open card*/
				multi_pcm_out[index] = pcm_open(HUB_NUM,  audio_name_map_out[index].device_write, PCM_OUT, &pcm_config_out[index]);

				/* open failed */
				if (!pcm_is_ready(multi_pcm_out[index])) {
					ALOGV("cannot open pcm driver: %s", pcm_get_error(multi_pcm_out));
					pcm_close(multi_pcm_out[index]);
					multi_pcm_out[index] = NULL;
					audio_name_map_out[index].active = DISABLE;
					return -ENOMEM;
				}

				/* prepare card */
				pcm_prepare(multi_pcm_out[index]);
				pcm_prepare(multi_pcm_out_support[index]);
			}
			else
			{
				/* open card*/
				multi_pcm_out[index] = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_out[index]);
#if 0
				/* for ac100*/
				if (strncmp(audio_name_map_out[index].nickname, AUDIO_NAME_BT, strlen(audio_name_map_out[index].nickname)) == 0)
				{
					pcm_bt_in = pcm_open(audio_name_map_out[index].card_num, 2, PCM_IN, &pcm_config_bt);
				}
#endif
			}
		}

		/* if need resampler */
		if (strncmp(audio_name_map_out[index].nickname, AUDIO_NAME_BT, strlen(audio_name_map_out[index].nickname)) != 0) {
			if (DEFAULT_OUT_SAMPLING_RATE != pcm_config_out[index].rate)
			{
				int ret = create_resampler(DEFAULT_OUT_SAMPLING_RATE, pcm_config_out[index].rate,
					2, RESAMPLER_QUALITY_DEFAULT, NULL, &out_multi_resampler[index]);
				if (ret != 0)
				{
					ALOGE("create out resampler failed, %d -> %d", DEFAULT_OUT_SAMPLING_RATE, pcm_config_out[index].rate);
					return ret;
				}

			}
		}
		else
		{ ALOGV("do not need out resampler"); }

		if (out_multi_resampler[index])
		{out_multi_resampler[index]->reset(out_multi_resampler[index]); }
	}
	return 0;
}

int start_input_stream()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index;
	for (index = 0; index < AUDIO_MAP_CNT; index++)
	{
		ALOGE("start input %s\n", 
		audio_name_map_in[index].name, 	audio_name_map_in[index].active);
		/* select */
		if(audio_name_map_in[index].active == ACTIVE)
		{
			ALOGE("use %s to record audio, device is %d, %d\n", audio_name_map_in[index].name, audio_name_map_in[index].active);
	
			/* check if device is link to hub or not */
			if(audio_name_map_in[index].hub_flag == true)
			{
				/* for ac100 codec */
				if (strncmp(audio_name_map_in[index].nickname, AUDIO_NAME_BT, strlen(audio_name_map_in[index].nickname)) == 0)
				{
#if 0
					pcm_bt_in = pcm_open(audio_name_map_in[index].card_num, 2, PCM_OUT, &pcm_config_bt);
					multi_pcm_in_support[index] = pcm_open(audio_name_map_in[index].card_num, 0, PCM_IN, &pcm_config_bt);
#endif
				}
				else
				{
					/*open support card for H6 ahub only */
					multi_pcm_in_support[index] = pcm_open(audio_name_map_in[index].card_num, 0, PCM_IN, &pcm_config_in[index]);
				}
				/* open card*/
				multi_pcm_in[index] = pcm_open(HUB_NUM,  audio_name_map_in[index].device_read, PCM_IN, &pcm_config_in[index]);

				/* open failed */
				if (!pcm_is_ready(multi_pcm_in[index])) {
					ALOGV("cannot open pcm driver: %s", pcm_get_error(multi_pcm_in));
					pcm_close(multi_pcm_in[index]);
					multi_pcm_in[index] = NULL;
					audio_name_map_in[index].active = DISABLE;
					return -ENOMEM;
				}

				/* prepare card */
				pcm_prepare(multi_pcm_in[index]);
				pcm_prepare(multi_pcm_in_support[index]);
			}
			else
			{
				ALOGE("%d %d %d\n", pcm_config_in[index].rate,  pcm_config_in[index].channels, audio_name_map_in[index].card_num);
				multi_pcm_in[index] = pcm_open(audio_name_map_in[index].card_num, 0, PCM_IN, &pcm_config_in[index]);
#if 0
				/* for ac100 codec */
				if (strncmp(audio_name_map_in[index].nickname, AUDIO_NAME_BT, strlen(audio_name_map_in[index].nickname)) == 0)
				{
					pcm_bt_in = pcm_open(audio_name_map_in[index].card_num, 2, PCM_OUT, &pcm_config_bt);
				}
#endif
			}
		}

		/* if need resampler */
		if (strncmp(audio_name_map_in[index].nickname, AUDIO_NAME_BT, strlen(audio_name_map_in[index].nickname)) != 0) {
			if (DEFAULT_IN_SAMPLING_RATE != pcm_config_in[index].rate)
			{
				int ret = create_resampler(DEFAULT_IN_SAMPLING_RATE, 
						pcm_config_in[index].rate,
					2, RESAMPLER_QUALITY_DEFAULT, NULL, &in_multi_resampler[index]);
				if (ret != 0)
				{
					ALOGE("create in resampler failed, %d -> %d", DEFAULT_IN_SAMPLING_RATE, pcm_config_in[index].rate);
					return ret;
				}

			}
		}
		else
		{ ALOGV("do not need in resampler"); }

		if (in_multi_resampler[index])
		{in_multi_resampler[index]->reset(in_multi_resampler[index]); }
	}
	return 0;
}

/* must be called with hw device and output stream mutexes locked */
int do_output_standby()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	/* close all */
	if (!out_standby_flag) {
		for (index = 0; index < AUDIO_MAP_CNT; index++)
		{
			if (multi_pcm_out[index])
			{
				pcm_close(multi_pcm_out[index]);
				multi_pcm_out[index] = NULL;
			}
			if (multi_pcm_out_support[index])
			{
				pcm_close(multi_pcm_out_support[index]);
				multi_pcm_out_support[index] = NULL;
			}
			if (out_multi_resampler[index])
			{
				release_resampler(out_multi_resampler[index]);
				out_multi_resampler[index] = NULL;
			}
		}
		out_standby_flag = 1;
		/* for dump pcm data close file! */
		aw_hal_dump_close();
	}
	return 0;
}
int do_input_standby()
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	int index = 0;
	/* close all */
	if (!in_standby_flag) {
		for (index = 0; index < AUDIO_MAP_CNT; index++)
		{
			if (multi_pcm_in[index])
			{
				pcm_close(multi_pcm_in[index]);
				multi_pcm_in[index] = NULL;
			}
			if (in_multi_resampler[index])
			{
				release_resampler(in_multi_resampler[index]);
				in_multi_resampler[index] = NULL;
			}
		}
		in_standby_flag = 1;
		/* for dump pcm data close file! */
		/* aw_hal_dump_close(); */
	}
	return 0;
}

#if KARAOK_FUN 
ssize_t  out_to_karaok(const void* buffer, size_t bytes)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	pthread_mutex_lock(&voice_data.lock);
	/* if it is full toss some buf*/
	if(voice_data.buffer_size + bytes >= voice_data.total_size)
	{
		voice_data.old_ptr = (voice_data.old_ptr + bytes)%voice_data.total_size;
		voice_data.buffer_size -= bytes;
	}

	/* check boundary && memcpy */
	if(voice_data.new_ptr >= voice_data.total_size)  
	{
		voice_data.new_ptr = 0;
		memcpy(voice_data.buffer + voice_data.new_ptr, buffer, bytes);
		voice_data.new_ptr += bytes;
		voice_data.buffer_size += bytes;
	}
	else
	{
		memcpy(voice_data.buffer + voice_data.new_ptr, buffer, bytes);
		voice_data.new_ptr += bytes;
		voice_data.buffer_size += bytes;
	}

	pthread_mutex_unlock(&voice_data.lock);
	usleep(40000);
	return bytes;
}
#endif
/* function tail */
