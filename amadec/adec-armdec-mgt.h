#ifndef ADEC_ARMDEC_MGT_H
#define ADEC_ARMDEC_MGT_H


#include <stdlib.h>
#include<stdio.h>
#include <string.h>

//#define AUDIO_ARC_DECODER 0
//#define AUDIO_ARM_DECODER 1
//#define AUDIO_FFMPEG_DECODER 2

#define READ_ABUFFER_SIZE 2*10*1024 //default in buffer size
#define DEFAULT_PCM_BUFFER_SIZE 192000*2//default out buffer size

typedef struct _audio_info {
    int bitrate;
    int samplerate;
    int channels;
    int file_profile;
}AudioInfo;

/* audio decoder operation*/
typedef struct audio_decoder_operations audio_decoder_operations_t;
struct audio_decoder_operations 
{
	    const char * name;
	    int nAudioDecoderType;
	    int nInBufSize;
	    int nOutBufSize;
	    int (*init)(audio_decoder_operations_t *);
	    int (*decode)(audio_decoder_operations_t *, char *outbuf, int *outlen, char *inbuf, int inlen);
	    int (*release)(audio_decoder_operations_t *);
	    int (*getinfo)(audio_decoder_operations_t *,AudioInfo *pAudioInfo);
	    void * priv_data;//point to audec
	    void * priv_dec_data;//decoder private data
};

#endif
