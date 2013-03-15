#ifndef __HLS_M3ULIVESESSION_H__
#define __HLS_M3ULIVESESSION_H__

/******************************************************************************

                  ��Ȩ���� (C), amlogic

 ******************************************************************************
  �� �� ��   : hls_m3ulivesession.h
  �� �� ��   : ����
  ��    ��   : peter
  ��������   : 2013��2��21�� ������
  ����޸�   :
  ��������   : hls_m3ulivesession.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��2��21�� ������
    ��    ��   : peter
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/





/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/





#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/



int m3u_session_open(const char* baseUrl,const char* headers,void** hSession);
int m3u_session_is_seekable(void* hSession);
int64_t m3u_session_seekUs(void* hSession,int64_t posUs);

int m3u_session_get_durationUs(void*session,int64_t* dur);

int m3u_session_get_cached_data_time(void*session,int* time);

int m3u_session_get_estimate_bandwidth(void*session,int* bps);

int m3u_session_get_error_code(void*session,int* errcode);

int m3u_session_get_stream_num(void* session,int* num);
int m3u_session_get_cur_bandwidth(void* session,int* bw);

int m3u_session_set_codec_data(void* session,int time);

int m3u_session_read_data(void* session,void* buf,int len);

int m3u_session_close(void* hSession);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HLS_M3ULIVESESSION_H__ */
