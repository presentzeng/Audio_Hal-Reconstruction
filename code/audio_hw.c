/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/audio_effect.h>
#include <audio_effects/effect_aec.h>
#include "head.h"
/* unused function head*/

/* ====== adev_open ====== */
static int adev_init_check(const struct audio_hw_device *dev)
{
    UNUSED(dev);
    return 0;
}

static int adev_close(hw_device_t *device)
{
	return 0;
}
static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
	return 0;
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
	UNUSED(dev);
	UNUSED(volume);
	F_LOG;
	return -ENOSYS;
}

static int adev_get_master_volume(struct audio_hw_device *dev, float *volume)
{
	UNUSED(dev);
	UNUSED(volume);
	F_LOG;
	return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev, audio_mode_t mode)
{
#if 0
	pthread_mutex_lock(&out_standby_lock);
	do_output_standby();
	pthread_mutex_unlock(&out_standby_lock);
	pthread_mutex_lock(&in_standby_lock);
	do_input_standby();
	pthread_mutex_unlock(&in_standby_lock);
	disable_all();

	if (mode == AUDIO_MODE_IN_CALL || mode == AUDIO_MODE_IN_COMMUNICATION)
	{
		/* active the ac100 */
		bt_mode_flag = 1;
		int index;
		for (index = 0; index < AUDIO_MAP_CNT; index++)
		{
			if (strncmp(audio_name_map_out[index].nickname, AUDIO_NAME_BT,
					strlen(audio_name_map_out[index].nickname)) == 0)
				audio_name_map_out[index].active =ACTIVE;

			if (strncmp(audio_name_map_in[index].nickname, AUDIO_NAME_BT,
					strlen(audio_name_map_in[index].nickname)) == 0)
				audio_name_map_in[index].active =ACTIVE;
		}
		if (ac100_routes)
		{set_ac100_path(ac100_routes, OUT_DEVICE_BT_SCO, IN_SOURCE_BTMIC);}
	} else {
		int index;
		bt_mode_flag = 1;
		for (index = 0; index < AUDIO_MAP_CNT; index++)
		{
		#if 0
			if (snd_active & (0x1 << index))
			{audio_name_map_out[index].active = ACTIVE;}
			if (snd_active & (0x10000 << index))
			{audio_name_map_in[index].active = ACTIVE;}
		#endif
			if (strncmp(audio_name_map_out[index].nickname, AUDIO_NAME_NTP,
					strlen(audio_name_map_out[index].nickname)) == 0)
				audio_name_map_out[index].active =ACTIVE;
			if (strncmp(audio_name_map_out[index].nickname, AUDIO_NAME_HDMI,
					strlen(audio_name_map_out[index].nickname)) == 0)
				audio_name_map_out[index].active =ACTIVE;
		}
		if (ac100_routes)
			set_ac100_path(ac100_routes, OUT_DEVICE_SPEAKER, -1);
	}
#endif
	UNUSED(dev);

	return 0;
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
	if (!debug_mode(kvpairs)) {
		return 0;
	}
	hot_plug_process(kvpairs);

	return 0;
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
	return 0;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
				 struct audio_stream_out *stream)
{
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *stream)
{
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
	UNUSED(device);
	debug_dump(fd);
	return 0;
}

/* === adev_open_output_stream === */

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
	UNUSED(stream);
	UNUSED(rate);
	return 0;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
	/*size_t size = (SHORT_PERIOD_SIZE * DEFAULT_OUT_SAMPLING_RATE) / audio_config_out.sample_rate;*/
	size_t size = SHORT_PERIOD_SIZE ;
	return size * audio_stream_out_frame_size(stream);
}

static audio_channel_mask_t out_get_channels(const struct audio_stream *stream)
{
	audio_channel_mask_t outchan = AUDIO_CHANNEL_OUT_STEREO;

	if (bt_mode_flag)
	{return AUDIO_CHANNEL_OUT_MONO;}

	if (audio_config_out.channel_mask != AUDIO_CHANNEL_NONE)
		outchan =  audio_config_out.channel_mask;
	return outchan;
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
	audio_format_t outformat = AUDIO_FORMAT_PCM_16_BIT;
	if (audio_config_out.format != AUDIO_FORMAT_DEFAULT)
		outformat = audio_config_out.format;
	return outformat;
}

static int out_set_format(struct audio_stream *stream, audio_format_t format)
{
	UNUSED(stream);
	UNUSED(format);
	return 0;
}


static int out_dump(const struct audio_stream *stream, int fd)
{
	UNUSED(stream);
	UNUSED(fd);
	return 0;
}


static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
	UNUSED(stream);
	UNUSED(keys);
	return strdup("");
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
	return 0;
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
	UNUSED(stream);
	UNUSED(left);
	UNUSED(right);
	return -ENOSYS;
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
	UNUSED(stream);
	UNUSED(dsp_frames);
	return -EINVAL;
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
	UNUSED(stream);
	UNUSED(effect);
	return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
	UNUSED(stream);
	UNUSED(effect);
	return 0;
}

static int out_get_next_write_timestamp(const struct audio_stream_out *stream,
                                        int64_t *timestamp)
{
	UNUSED(stream);
	UNUSED(timestamp);
	return -EINVAL;
}

/* === adev_open_input_stream === */
static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
	return 0;
}

static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
	if (bt_mode_flag)
	{return SAMPLING_RATE_8K;}
	else
	{return DEFAULT_IN_SAMPLING_RATE;}
}

static int in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
	UNUSED(stream);
	UNUSED(rate);
	return 0;
}


static int in_set_format(struct audio_stream *stream, audio_format_t format)
{
	UNUSED(stream);
	UNUSED(format);
	return 0;
}

static int in_dump(const struct audio_stream *stream, int fd)
{
	UNUSED(stream);
	UNUSED(fd);
	return 0;
}
static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
	return 0;
}

static char * in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
	UNUSED(stream);
	UNUSED(keys);
	return strdup("");
}
static int in_set_gain(struct audio_stream_in *stream, float gain)
{
	UNUSED(stream);
	UNUSED(gain);
    return 0;
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev, const struct audio_config *config)
{

	size_t size = 0;

	if (bt_mode_flag)
		size = get_buffer_size(SAMPLING_RATE_8K, PCM_FORMAT_S16_LE, 1);
	else
		size = get_buffer_size(pcm_config_in_default.rate, pcm_config_in_default.format,  pcm_config_in_default.channels);

	return size;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
	struct sunxi_audio_device *adev = (struct sunxi_audio_device *)dev;
	return 0;
}
static uint32_t in_get_input_frames_lost(struct audio_stream_in *stream)
{
	UNUSED(stream);
	return 0;
}

/* unused function tail*/

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
	size_t size = 0;
	if (bt_mode_flag)
		size = get_buffer_size(SAMPLING_RATE_8K, PCM_FORMAT_S16_LE, 1);
	else
		size = get_buffer_size(pcm_config_in_default.rate, pcm_config_in_default.format,  pcm_config_in_default.channels);
	return size;
}

static audio_channel_mask_t in_get_channels(const struct audio_stream *stream)
{
	if(bt_mode_flag)
	{ return  AUDIO_CHANNEL_IN_MONO; }
	else
	{ return  AUDIO_CHANNEL_IN_STEREO; }
}

static audio_format_t in_get_format(const struct audio_stream *stream)
{
	UNUSED(stream);
	return AUDIO_FORMAT_PCM_16_BIT;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
	return 0;
}



static int in_standby(struct audio_stream *stream)
{
	pthread_mutex_lock(&in_standby_lock);
	int status = do_input_standby();
	pthread_mutex_unlock(&in_standby_lock);

	return 0;
}

static ssize_t in_read(struct audio_stream_in *stream, void* buffer,
                       size_t bytes)
{
	int ret;
	int index;
	int card;

	/* check if need standby */
	pthread_mutex_lock(&in_standby_lock);
	if (in_standby_flag)
	{
		ret = start_input_stream();
		if (ret != 0) {
			pthread_mutex_unlock(&in_standby_lock);
			goto exit;
		}
		in_standby_flag = 0;
	}
	pthread_mutex_unlock(&in_standby_lock);

	for (index = 0; index < AUDIO_MAP_CNT; index++)
	{
		if(audio_name_map_in[index].active == 1)
		{
			/* if need resampler */		
			if(NULL != in_multi_resampler[index])
			{

			}
			else
			{ pcm_read_ex(multi_pcm_in[index], buffer, bytes); }
		}
	}
	aw_hal_dump_in_pcm(stream, buffer, bytes);
exit:
	return bytes;
}

struct audio_stream_in in_stream;
static int adev_open_input_stream(struct audio_hw_device *dev,
                                  audio_io_handle_t handle,
                                  audio_devices_t devices,
                                  struct audio_config *config,
                                  struct audio_stream_in **stream_in,
                                  audio_input_flags_t flags __unused,
                                  const char *address __unused,
                                  audio_source_t source __unused)
{
	*stream_in = NULL;
	in_stream.common.get_sample_rate	= in_get_sample_rate;
	in_stream.common.set_sample_rate	= in_set_sample_rate;
	in_stream.common.get_buffer_size	= in_get_buffer_size;
	in_stream.common.get_channels		= in_get_channels;
	in_stream.common.get_format		= in_get_format;
	in_stream.common.set_format		= in_set_format;
	in_stream.common.standby		= in_standby;
	in_stream.common.dump			= in_dump;
	in_stream.common.set_parameters		= in_set_parameters;
	in_stream.common.get_parameters		= in_get_parameters;
	/* in_stream.common.add_audio_effect  = in_add_audio_effect; 		*/
	/* in_stream.common.remove_audio_effect = in_remove_audio_effect;	*/
	in_stream.set_gain = in_set_gain;
	in_stream.read				= in_read;
	in_stream.get_input_frames_lost		= in_get_input_frames_lost;

	*stream_in  = &in_stream;

	/* save audio_config for output */	
	audio_config_in = *config;

	return 0;
}

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
	uint32_t  outrate = DEFAULT_OUT_SAMPLING_RATE;
	/*if (audio_config_out.sample_rate != 0)
		outrate = audio_config_out.sample_rate;*/
	if (bt_mode_flag)
	{return SAMPLING_RATE_8K;}
	else
	{return DEFAULT_OUT_SAMPLING_RATE;}
}


static int out_standby(struct audio_stream *stream)
{
	pthread_mutex_lock(&out_standby_lock);
	int status = do_output_standby();
	pthread_mutex_unlock(&out_standby_lock);

	return status;
}

static int64_t __systemTime()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000000LL + t.tv_nsec;
}


static ssize_t out_write(struct audio_stream_out *stream, const void* buffer, size_t bytes)
{
#if KARAOK_FUN
	return out_to_karaok(buffer, bytes);
#endif

	/* for pass through only */
	if(passthrough_mode_flag  == 1)
	{ return pass_through_handler(stream, buffer, bytes);}

	int ret = 0;
	int index = 0;
	int card = 0;
	/* resampler buffer caculate */
	size_t frame_size = audio_stream_out_frame_size((const struct audio_stream_out *)stream);
	size_t in_frames = bytes / frame_size;
	size_t out_frames = RESAMPLER_BUFFER_SIZE / frame_size;
	char * resampler_buffer = malloc(RESAMPLER_BUFFER_SIZE); 

	/* check if need standby */
	pthread_mutex_lock(&out_standby_lock);
	if (out_standby_flag)
	{
		ret = start_output_stream();
		if (ret != 0) 
		{
			pthread_mutex_unlock(&out_standby_lock);
			goto exit;
		}
		out_standby_flag = 0;
	}
	pthread_mutex_unlock(&out_standby_lock);

	for (index = 0; index < AUDIO_MAP_CNT; index++)
	{
		if(audio_name_map_out[index].active == 1)
		{
			/* if need resampler */		
			if(NULL != out_multi_resampler[index])
			{
				out_multi_resampler[index]->resample_from_input(out_multi_resampler[index], (int16_t *)buffer, &in_frames, (int16_t *)resampler_buffer, &out_frames);
				pcm_write(multi_pcm_out[index], resampler_buffer, out_frames * frame_size);
			}
			else 
			{pcm_write(multi_pcm_out[index], buffer, bytes);}
		}
	}
	aw_hal_dump_out_pcm(stream, buffer, bytes);
exit:
    return bytes;
}

struct audio_stream_out out_stream;
static int adev_open_output_stream(struct audio_hw_device *dev,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   struct audio_stream_out **stream_out,
                                   const char *address __unused)
{
	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);
	out_stream.common.get_sample_rate	= out_get_sample_rate;
	out_stream.common.set_sample_rate  	= out_set_sample_rate;
	out_stream.common.get_buffer_size  	= out_get_buffer_size;
	out_stream.common.get_channels     	= out_get_channels;
	out_stream.common.get_format       	= out_get_format;
	out_stream.common.set_format       	= out_set_format;
	out_stream.common.standby          	= out_standby;
	out_stream.common.dump             	= out_dump;
	out_stream.common.set_parameters   	= out_set_parameters;
	out_stream.common.get_parameters	= out_get_parameters;
	out_stream.common.add_audio_effect	= out_add_audio_effect;
	out_stream.common.remove_audio_effect	= out_remove_audio_effect;
	out_stream.get_latency          	= out_get_latency;
	out_stream.set_volume      		= out_set_volume;
	out_stream.write           		= out_write;
	out_stream.get_render_position     	= out_get_render_position;
	out_stream.get_next_write_timestamp 	= out_get_next_write_timestamp;
	/*out->stream.get_presentation_position = out_get_presentation_position;*/


	/* pass through mode only */
    if (0) 
	{
		passthrough_mode_flag = 1;
        if (config->sample_rate == 0)
            config->sample_rate = DEFAULT_OUT_SAMPLING_RATE;
        if (config->channel_mask == 0)
            config->channel_mask = AUDIO_CHANNEL_OUT_STEREO;
          pcm_config_out_passthrough.format = config->format;
          pcm_config_out_passthrough.rate = config->sample_rate;
          /*pcm_config_out_passthrough.channel_mask = config->channel_mask;*/
          /*pcm_config_out_passthrough.config = pcm_config_aux_digital;*/
          pcm_config_out_passthrough.rate = config->sample_rate;
          pcm_config_out_passthrough.channels = popcount(config->channel_mask);
          pcm_config_out_passthrough.raw_flag = (config->format == AUDIO_FORMAT_AC3)?2:
                               (config->format == AUDIO_FORMAT_E_AC3)?10:
                               (config->format == AUDIO_FORMAT_DTS_HD)?11:
                               (config->format == AUDIO_FORMAT_DTS)?7:1;

        if(pcm_config_out_passthrough.raw_flag == 1)
        {
            if(pcm_config_out_passthrough.format == AUDIO_FORMAT_PCM_32_BIT)
            { pcm_config_out_passthrough.format = PCM_FORMAT_S32_LE; }
            else if(pcm_config_out_passthrough.format == AUDIO_FORMAT_PCM_24_BIT_PACKED)
            { pcm_config_out_passthrough.format = PCM_FORMAT_S24_LE; }
        }
    }
	*stream_out = &out_stream;
	/* save audio_config for output */	
	audio_config_out = *config;
	return 0;
}

struct audio_hw_device hw_device;
static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
	hw_device.common.tag			= HARDWARE_DEVICE_TAG;
	hw_device.common.version		= AUDIO_DEVICE_API_VERSION_2_0;
	hw_device.common.module			= (struct hw_module_t *) module;
	hw_device.common.close			= adev_close;
	hw_device.init_check			= adev_init_check;
	hw_device.set_voice_volume		= adev_set_voice_volume;
	hw_device.set_master_volume		= adev_set_master_volume;
	hw_device.get_master_volume		= adev_get_master_volume;
	hw_device.set_mode			= adev_set_mode;
	hw_device.set_mic_mute			= adev_set_mic_mute;
	hw_device.get_mic_mute			= adev_get_mic_mute;
	hw_device.set_parameters		= adev_set_parameters;
	hw_device.get_parameters		= adev_get_parameters;
	hw_device.get_input_buffer_size		= adev_get_input_buffer_size;
	hw_device.open_output_stream		= adev_open_output_stream;
	hw_device.close_output_stream		= adev_close_output_stream;
	hw_device.open_input_stream		= adev_open_input_stream;
	hw_device.close_input_stream		= adev_close_input_stream;
	hw_device.dump				= adev_dump;

	*device = &hw_device.common;

	AW_HAL_DEBUG("[%s]-->line:%d", __FUNCTION__, __LINE__);

	/* card map init */
	reset_all();
	init_card_config();
	init_card_map();
	mixer_open_hub();
#if KARAOK_FUN 
	karaok_init();
#endif
	return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
	.common = {
		.tag			= HARDWARE_MODULE_TAG,
		.module_api_version	= AUDIO_MODULE_API_VERSION_0_1,
		.hal_api_version	= HARDWARE_HAL_API_VERSION,
		.id			= AUDIO_HARDWARE_MODULE_ID,
		.name			= "sunxi audio HW HAL",
		.author			= "author",
		.methods		= &hal_module_methods,
	},
};
