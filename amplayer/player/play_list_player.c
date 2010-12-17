#include <stdio.h>
#include <sys/time.h> 
#include <time.h> 
#include <pthread.h>
#include <player.h>

#include "player_priv.h"
#include "play_list.h"

#define PLAY_LIST_STOP		(0x1)
#define PLAY_NEXT_FILE		(0x2)
#define	PLAY_LIST_END		(0x3)
#define PLAY_LIST_STOPED	(0x10001)
#define PLAY_LIST_FILE_END	(0x20001)

static pthread_t playlist_thread_id;
static update_state_fun_t up_callback_fn=NULL;
static play_list_thread_para_t play_list_ctrl_para;
int play_list_play_status;

static int play_list_ctrl_init(play_control_t *ctrl_param)
{	
	if(!ctrl_param)
		return -1;	
	
	ctrl_param->video_index = -1;
	ctrl_param->audio_index = -1;	
	ctrl_param->sub_index = -1;	
	return 0;
}
static int play_list_play_file(char *filename,unsigned long args)
{	
	play_control_t *plist_ctrl;
	unsigned long priv;	
	play_list_thread_para_t *plist_thread_para = (play_list_thread_para_t *)args;
	
	if(plist_thread_para)
	{		
		plist_ctrl = &plist_thread_para->play_ctrl_para;
		priv = plist_thread_para->priv;
		log_print("[%s]cb=%p\n",__FUNCTION__,&plist_ctrl->callback_fn);			
	}
	else
	{
		log_error("[%s]play list thread parameter error!",__FUNCTION__);
		return -1;
	}	
		
	log_print("try play file=%s\n",filename);	

	if(plist_ctrl->callback_fn.update_statue_callback)
		up_callback_fn=plist_ctrl->callback_fn.update_statue_callback;	
	if(plist_ctrl->file_name)
	{
		FREE(plist_ctrl->file_name);
		plist_ctrl->file_name = NULL;
	}
	plist_ctrl->file_name = MALLOC(strlen(filename)+2);
	plist_ctrl->file_name[0]='s';
	strcpy(plist_ctrl->file_name+1,filename);
	plist_ctrl->file_name[strlen(filename)+1]='\0';
	log_print("[%s]list->filename=%s\n",__FUNCTION__,plist_ctrl->file_name);
	return player_start(plist_ctrl,priv);	
}
int play_list_update_state(int pid,player_info_t *info) 
{
	play_list_t *list = player_get_playlist();	
	if(info->status != info->last_sta)
		log_print("play_list_update_status:0x%x last:0x%x",info->status,info->last_sta);
	if(info->status == PLAYER_STOPED)
		play_list_play_status = PLAY_LIST_STOPED;
	else if(info->status == PLAYER_PLAYEND || info->status == PLAYER_ERROR)
	{
		play_list_play_status = PLAY_LIST_FILE_END;
		log_print("list->out_list_index=%d list->list_num=%d",list->out_list_index,list->list_num);
		if(list->out_list_index < list->list_num)
			info->status = PLAYER_PLAY_NEXT;
	}
	log_print("[play_list_update_state]sta=0x%x curtime=%d lasttime=%d\n",info->status,info->current_time,info->last_time);
	return 0;
}

static int play_list_ctrl_play(int pid)
{
	int flag;
	do
	{
		if(play_list_play_status == PLAY_LIST_STOPED)
		{
			flag = PLAY_LIST_STOP;
			break;
		}
		else if(play_list_play_status == PLAY_LIST_FILE_END)
		{
			flag = PLAY_NEXT_FILE;
			break;
		}
	}while(1);
	return flag;
}

static void *play_list_play(void *args)
{
	char *file_name;	
	int pid;
	int flag;	
	
	play_list_t *list = player_get_playlist();	
	log_print("Enter play list play thread!\n");
	do{		
		file_name = play_list_get_file();
		if(file_name)						
		{
			pid = play_list_play_file(file_name, (unsigned long)args);	
			if(pid < 0) 
			{
				log_error("Play start file %s failed!ret=%d\n",file_name,pid);
				goto err;
			}
			flag = play_list_ctrl_play(pid);
			if(flag == PLAY_NEXT_FILE)
			{
				player_exit(pid);
				log_print("play next file !\n");
				continue;
			}
			else if(flag == PLAY_LIST_STOP)
			{
				log_print("stop play list play !\n");
				break;
			}
		}
		else
		{
			log_error("can't get file from play list!\n");
			goto err;
		}
	}while(file_name);
err:	
	play_list_release();
	if(play_list_ctrl_para.play_ctrl_para.file_name)
	{
		//FREE(play_list_ctrl_para.play_ctrl_para.file_name);
		play_list_ctrl_para.play_ctrl_para.file_name = NULL;
	}
	pthread_exit(NULL);
	log_print("Exit play list play thread!\n");
	return NULL;		
}

int play_list_player(play_control_t *pctrl,unsigned long priv)
{
	int num;
	int ret = -1;
	play_list_t *play_list = NULL;
	play_control_t *m_play_ctrl;
	play_list_init();
	m_play_ctrl = &play_list_ctrl_para.play_ctrl_para;
	MEMCPY(m_play_ctrl,pctrl,sizeof(pctrl));	
	play_list_ctrl_para.priv = priv;
	if(player_register_update_callback(&m_play_ctrl->callback_fn, pctrl->callback_fn.update_statue_callback,pctrl->callback_fn.update_interval)<0)
		log_print("[%s:%d]Warning: callback register failed!\n",__FUNCTION__,__LINE__);
	num=play_list_parser_m3u(pctrl->file_name);
	if(num > 0)			
	{
		play_list = player_get_playlist();
		if(play_list)
		{		
			m_play_ctrl->is_playlist = 1;			
			log_print("[%s:%d]is_playlist=%d\n",__FUNCTION__,__LINE__,m_play_ctrl->is_playlist);
			ret = pthread_create(&playlist_thread_id, NULL, play_list_play, (void *)&play_list_ctrl_para);
		}
    }
	return ret;     
}
int check_url_type(char *filename)
{
	if(!filename)
	{
		log_error("[%s]parameters NULL!\n",__FUNCTION__);
		return -1;
	}
	ByteIOContext *s;
	int err;
	char line[MAX_PATH];	
	char *pline;	
	log_print("[%s]filename=%s!\n",__FUNCTION__,filename);
	if((err=url_fopen(&s,filename,URL_RDONLY))<0)
	{
		log_error("[%s]open %s error!\n",__FUNCTION__,filename);
		return err;
	}	
	if(play_list_get_line(s,line,sizeof(line))==0)
	{
		log_print("[%s:%d]list_line=%s!\n",__FUNCTION__,__LINE__,line);
		if(strcmp(line,"#EXTM3U")==0)
		{
			log_print("[%s:%d]M3U FOUND!\n",__FUNCTION__,__LINE__);
			url_fclose(s);
			return 1;
		}
	}	
	if((match_ext(filename, 'm3u'))||(match_ext(filename, 'm3u8'))) 
	{
		url_fclose(s);	
		return 1;
	}
	else
	{
		url_fclose(s);	
		return 0;
	}
}
