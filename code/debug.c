#include "head.h"

#include <list.h>

#define DUMP_HAL_STATE      "hal_state"
#define ENABLE_LOG          "enable_debug_log"
#define DISABLE_LOG         "disable_debug_log"
#define ENABLE_PLAY_DUMP    "enable_play_dump"
#define DISABLE_PLAY_DUMP   "disable_play_dump"
#define ENABLE_CAP_DUMP     "enable_cap_dump"
#define DISABLE_CAP_DUMP    "disable_cap_dump"

extern int aw_debug_en;
extern int aw_dump_output_en;
extern int aw_dump_input_en ;

static const char* debug_cmd = NULL;
static pthread_mutex_t debug_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct str_node 
{
    const char* str;
    struct listnode clist;
} StrNode;

list_declare(dumpStr);
static char _dumpbuf[128];
#define insertDumpStr(...) do{ \
     snprintf(_dumpbuf, 128, __VA_ARGS__); \
     StrNode *_node = calloc(1, sizeof(StrNode)); \
     _node->str = strdup(_dumpbuf); \
     list_add_tail((&dumpStr), &_node->clist);\
     \
    }while(0);

static void clearDumpStr() 
{
    struct listnode *node = dumpStr.next;
    while( node != (&dumpStr)) 
	{
        StrNode* strNode = node_to_item(node, StrNode, clist);
        node = strNode->clist.next;
        list_remove(&strNode->clist);
        if (strNode->str) 
		{
            free(strNode->str);
            strNode->str = NULL;
        }
        free(strNode);
    }
}


int debug_mode(const char * pairs)
{
    /* debug_cmd_lock */
    if (!pairs) 
	{
        /* dump state */
        return -1;
    }
    pthread_mutex_lock(&debug_lock);
    if (debug_cmd != NULL) 
	{
        free(debug_cmd);
        debug_cmd = NULL;
    }
    debug_cmd = strdup(pairs);
    if (!debug_cmd) 
	{
        pthread_mutex_unlock(&debug_lock);
        return -1;
    }

    if (!strncmp(debug_cmd, DUMP_HAL_STATE, strlen(DUMP_HAL_STATE))) 
	{
    }
	else if (!strncmp(debug_cmd, ENABLE_LOG, strlen(ENABLE_LOG))) 
	{ aw_debug_en = 1; }
	else if (!strncmp(debug_cmd, DISABLE_LOG, strlen(DISABLE_LOG))) 
	{ aw_debug_en = 0; }
	else if (!strncmp(debug_cmd, ENABLE_PLAY_DUMP, strlen(ENABLE_PLAY_DUMP))) 
	{ aw_dump_output_en = 1; }
	else if (!strncmp(debug_cmd, DISABLE_PLAY_DUMP, strlen(DISABLE_PLAY_DUMP))) 
	{ aw_dump_output_en = 0; }
	else if (!strncmp(debug_cmd, ENABLE_CAP_DUMP, strlen(ENABLE_CAP_DUMP))) 
	{ aw_dump_input_en = 1; }
	else if (!strncmp(debug_cmd, DISABLE_CAP_DUMP, strlen(DISABLE_CAP_DUMP))) 
	{ aw_dump_input_en = 0; }
	else 
	{
        free(debug_cmd);
        debug_cmd = NULL;
        pthread_mutex_unlock(&debug_lock);
        return -1;
    }

    pthread_mutex_unlock(&debug_lock);
    return 0;
}

int debug_dump(int fd) 
{
    /* debug_cmd_lock */

    pthread_mutex_lock(&debug_lock);
    if (!debug_cmd) 
	{
        pthread_mutex_unlock(&debug_lock);
        return 0;
    }

    if (!strncmp(debug_cmd, DUMP_HAL_STATE, strlen(DUMP_HAL_STATE))) 
	{
        insertDumpStr("audio hal state:\n");
        insertDumpStr("/* print catd state */\n");
		int index = 0;
		for(; index < MAX_AUDIO_DEVICES; index++)
		{
			if(strlen(audio_name_map_out[index].name) != 0)
			{
				/* print out card */
			}
			if(strlen(audio_name_map_in[index].name) != 0)
			{
				/* print in card */
			}
		}
		insertDumpStr("\n debug config:\n");
        insertDumpStr("enable_log       :  %d\n",aw_debug_en);
        insertDumpStr("enable play dump :  %d\n", aw_dump_output_en);
        insertDumpStr("enable cap dump  :  %d\n", aw_dump_input_en);
        insertDumpStr("\n\nusage:\n");
        insertDumpStr("  dumpsys media.audio_flinger debug %s\n", DUMP_HAL_STATE);
        insertDumpStr("  dumpsys media.audio_flinger debug %s\n", ENABLE_LOG);
        insertDumpStr("  dumpsys media.audio_flinger debug %s\n", DISABLE_LOG);
        insertDumpStr("  dumpsys media.audio_flinger debug %s\n", ENABLE_PLAY_DUMP);
        insertDumpStr("  dumpsys media.audio_flinger debug %s\n", DISABLE_PLAY_DUMP);
        insertDumpStr("  dumpsys media.audio_flinger debug %s\n", ENABLE_CAP_DUMP);
        insertDumpStr("  dumpsys media.audio_flinger debug %s\n", DISABLE_CAP_DUMP);
    }
	else if (!strncmp(debug_cmd, ENABLE_LOG, strlen(ENABLE_LOG))) 
	{ insertDumpStr("enable debug log\n"); }
	else if (!strncmp(debug_cmd, DISABLE_LOG, strlen(DISABLE_LOG))) 
	{ insertDumpStr("disable debug log\n"); }
	else if (!strncmp(debug_cmd, ENABLE_PLAY_DUMP, strlen(ENABLE_PLAY_DUMP))) 
	{ insertDumpStr("enable play dump\n"); }
	else if (!strncmp(debug_cmd, DISABLE_PLAY_DUMP, strlen(DISABLE_PLAY_DUMP))) 
	{ insertDumpStr("disable play dump\n"); }
	else if (!strncmp(debug_cmd, ENABLE_CAP_DUMP, strlen(ENABLE_CAP_DUMP))) 
	{ insertDumpStr("enable cap dump\n"); }
	else if (!strncmp(debug_cmd, DISABLE_CAP_DUMP, strlen(DISABLE_CAP_DUMP))) 
	{ insertDumpStr("disable cap dump\n"); }
	else 
	{
        /* error cmd */
        insertDumpStr("error cmd : %s\n",debug_cmd);
    }
    struct listnode *node;
    list_for_each(node, &dumpStr) 
	{
        StrNode* strNode = node_to_item(node,StrNode, clist);
        if (strNode->str) {
            write(fd,strNode->str,strlen(strNode->str));
        }
    }
    if (debug_cmd != NULL) 
	{
        free(debug_cmd);
        debug_cmd = NULL;
    }
    clearDumpStr();

    pthread_mutex_unlock(&debug_lock);
    return 0;
}
#if 0
void print_time_stamp()
{
}
#endif 
#if 0
void output_test(int count)
{
	/* open all */	
	if(audio_name_map_out[index].hub_flag == true)
	{
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

		/*open support card for H6 ahub only */
		multi_pcm_out_support[index] = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_out[index]);

		/* prepare card */
		pcm_prepare(multi_pcm_out[index]);
		pcm_prepare(multi_pcm_out_support[index]);
	}
	else
	{
		/* open card*/
		multi_pcm_out[index] = pcm_open(audio_name_map_out[index].card_num, 0, PCM_OUT, &pcm_config_out[index]);
	}

	for (index = 0; index < AUDIO_MAP_CNT; index++)
	{
		pcm_write(multi_pcm_out[index], buffer, bytes);
	}

}
#endif

