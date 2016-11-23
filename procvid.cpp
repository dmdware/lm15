
#include "platform.h"
#include "utils.h"
#include "texture.h"
#include "compframes.h"
#include "procvid.h"
#include "estview.h"
#include "localmap.h"

// http://dranger.com/ffmpeg/tutorial01.html

#if 0
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
	FILE *pFile;
	char szFilename[32];
	int  y;

	// Open file
	sprintf(szFilename, "out/frame%d.ppm", iFrame);
	pFile=fopen(szFilename, "wb");
	if(pFile==NULL)
		return;

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for(y=0; y<height; y++)
		fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

	// Close file
	fclose(pFile);
}

void ProcFrame2(LoadedTex* comptex, LoadedTex* colortex)
{

}

void ProcFrames(LoadedTex* prevltex, LoadedTex* ltex, int fi, LoadedTex* outtex)
{
//
	//AllocTex(outtex, prevltex->sizex, prevltex->sizey, 3);
	//memcpy(outtex->data, prevltex->data, prevltex->sizex*prevltex->sizey*3);

	TeachSeg(prevltex, ltex, fi, outtex);
	//FeedOut(prevltex, ltex, fi, outtex);
}

void ProcFrame(AVFrame *pFrame, int width, int height, int iFrame, LoadedTex* outtex)
{
	//FILE *pFile;
	char szFilename[32];
	int  y;
	static LoadedTex ltex;
	static LoadedTex prevltex;
	static LoadedTex oldouttex;
	//LoadedTex procltex;

	//if(iFrame % 2 == 1)
	//	return;

	// Open file
	sprintf(szFilename, "out/f2/frame%09d.png", iFrame);
	//pFile=fopen(szFilename, "wb");
	//if(pFile==NULL)
	//	return;

	// Write header
	//fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	if(!ltex.data)
		AllocTex(&ltex, FITRESX, FITRESY, 3);
	if(!prevltex.data)
		AllocTex(&prevltex, FITRESX, FITRESY, 3);
	if(!oldouttex.data)
		AllocTex(&oldouttex, FITRESX, FITRESY, 3);
	if(!outtex->data)
		AllocTex(outtex, FITRESX, FITRESY*2, 3);

	// Write pixel data
	//for(y=0; y<height; y++)
	//	fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

	//for(y=0; y<FITRESY; y++)
	memcpy(prevltex.data, ltex.data, FITRESX*3*FITRESY);

	//for(y=0; y<FITRESY; y++)
	memcpy(ltex.data, oldouttex.data, FITRESX*3*FITRESY);

	LoadedTex sample;
	LoadedTex resample;
	AllocTex(&sample, width, height, 3);
	//AllocTex(&sample, FITRESX, FITRESY, 3);

	for(y=0; y<height; y++)
		memcpy(&sample.data[3*y*width], pFrame->data[0]+y*pFrame->linesize[0], width*3);

	Resample(&sample, &resample, Vec2i(FITRESX,FITRESY));

	//for(y=0; y<FITRESY; y++)
	memcpy(outtex->data, resample.data, FITRESX*3*FITRESY);
	memcpy(oldouttex.data, resample.data, FITRESX*3*FITRESY);

	// Close file
	//fclose(pFile);
#if 0
	//first frame?
	if(!prevltex.data)
	{
		AllocTex(&prevltex, width, height, 3);

		//for(y=0; y<height; y++)
		//	memcpy(&prevltex.data[3*y*width], &ltex.data[3*y*width], width*3);

		return;
	}
	//enough data available to compare frames?
	else
#endif
	printf("f%d",(int)iFrame);

	//if(iFrame >= 2)
	if(iFrame >= 01)
	{
		//SavePNG(szFilename, &prevltex);

		ProcFrames(&prevltex, &ltex, iFrame, outtex);

#if 0
		//update previous frame to current
		for(y=0; y<height; y++)
			memcpy(&prevltex.data[3*y*width], &ltex.data[3*y*width], width*3);
		for(y=0; y<height; y++)
			memcpy(&ltex.data[3*y*width], outtex.data[3*y*width], width*3);
#endif


		//sprintf(szFilename, "out/09j09.png");

		SavePNG(szFilename, outtex);
		//SavePNG(szFilename, &sample);

		//if(iFrame >= 31002)
		//exit(0);
	}

	//ltex.destroy();
}

#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */

/* add a video output stream */
static AVStream *add_video_stream(AVFormatContext *oc, enum AVCodecID codec_id, int width, int height)
{
	AVCodecContext *c;
	AVStream *st;

	st = avformat_new_stream(oc, NULL);
	if (!st) {
		fprintf(stderr, "Could not alloc stream\n");
		exit(1);
	}
	st->id = 0;

	c = st->codec;
	c->codec_id = codec_id;
	c->codec_type = AVMEDIA_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = 400000;
	/* resolution must be a multiple of two */
	c->width = width;
	c->height = height;
	/* time base: this is the fundamental unit of time (in seconds) in terms
	of which frame timestamps are represented. for fixed-fps content,
	timebase should be 1/framerate and timestamp increments should be
	identically 1. */
	c->time_base.den = STREAM_FRAME_RATE;
	c->time_base.num = 1;
	c->gop_size = 12; /* emit one intra frame every twelve frames at most */
	c->pix_fmt = STREAM_PIX_FMT;
	if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
		/* just for testing, we also add B frames */
		c->max_b_frames = 2;
	}
	if (c->codec_id == CODEC_ID_MPEG1VIDEO){
		/* Needed to avoid using macroblocks in which some coeffs overflow.
		This does not happen with normal video, it just happens here as
		the motion of the chroma plane does not match the luma plane. */
		c->mb_decision=2;
	}
	// some formats want stream headers to be separate
	if(oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return st;
}

void YUVfromRGB(double& Y, double& U, double& V, const double R, const double G, const double B)
{
	Y =  0.257 * R + 0.504 * G + 0.098 * B +  16;
	U = -0.148 * R - 0.291 * G + 0.439 * B + 128;
	V =  0.439 * R - 0.368 * G - 0.071 * B + 128;
}

void ProcVid(const char* fullpath)
{
	av_register_all();
	avfilter_register_all();

	AVFormatContext *pFormatCtx = NULL;
	//AVOutputFormat *fmt;

#if 0
	fmt = av_guess_format(NULL, fullpath, NULL);

	if(!fmt)
	{
		ErrMess("Error", "Failed to guess video format");
		g_quit = true;
		return;
	}
#endif

#if 1
	// Open video file
	if( avformat_open_input(&pFormatCtx, fullpath, NULL, 0) != 0 )
		//if(av_open_input_file(&pFormatCtx, fullpath, NULL, 0, NULL) != 0)
	{
		char m[1024 + DMD_MAX_PATH];
		sprintf(m, "Failed to open input video file: %s", fullpath);
		Log("Error %s", m);
		printf("%s\n", m);
		//g_quit = true;
		exit(0);
		return;
	}
#endif

#if 0
	pFormatCtx = avformat_alloc_context();

	if(!pFormatCtx)
	{
		ErrMess("Error", "Error allocating format context");
		g_quit = true;
		return;
	}

	pFormatCtx->oformat = fmt;
#endif



	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
	{
		// Couldn't find stream information
		//ErrMess("Error", "Couldn't find stream information");
		printf("Couldn't find stream information\n");
		//g_quit = true;
		exit(0);
		return;
	}

	// Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, fullpath, 0);

	int i;
	AVCodecContext *pCodecCtxOrig = NULL;
	AVCodecContext *pCodecCtx = NULL;

	// Find the first video stream
	int videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) 
		{
			videoStream=i;
			break;
		}
	}

	if(videoStream==-1)
	{
		// Didn't find a video stream
		//ErrMess("Error", "Didn't find a video stream");
		printf("Didn't find a video stream\n");
		//g_quit = true;
		exit(0);
		return;
	}

	// Get a pointer to the codec context for the video stream
	//pCodecCtx=pFormatCtx->streams[videoStream]->codec;
	pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;



	AVCodec *pCodec = NULL;

	// Find the decoder for the video stream
	//pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
	if(pCodec==NULL) {
		//ErrMess("Error", "Unsupported codec!");
		fprintf(stderr, "Unsupported codec!\n");
		exit(0);
		return; // Codec not found
	}
	// Copy context
	pCodecCtx = avcodec_alloc_context3(pCodec);

	int asd = 0;

	if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
		//ErrMess("Error", "Couldn't copy codec context");
		fprintf(stderr, "Couldn't copy codec context");
		exit(0);
		return; // Error copying codec context
	}
	// Open codec
	//if(avcodec_open2(pCodecCtx, pCodec)<0)
	if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
		return; // Could not open codec




	AVFrame *pFrame = NULL;
	AVFrame *pFrameRGB = NULL;

	// Allocate video frame
	pFrame=av_frame_alloc();



	// Allocate an AVFrame structure
	pFrameRGB=av_frame_alloc();
	if(pFrameRGB==NULL)
		return;



	uint8_t *buffer = NULL;
	int numBytes;
	// Determine required buffer size and allocate buffer
	numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
		pCodecCtx->height);
	buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));



	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
		pCodecCtx->width, pCodecCtx->height);




	struct SwsContext *sws_ctx = NULL;
	int frameFinished;
	AVPacket packet;
	// initialize SWS context for software scaling
	sws_ctx = sws_getContext(pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		PIX_FMT_RGB24,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
		);






	AVCodec *outcodec;
	AVCodecContext *outc= NULL;
	int out_frame_size, outx, outy, outbuf_size, outsize;
	FILE *outf;
	AVFrame *outpicture;
	uint8_t *outbuf, *out_picture_buf;
	AVStream *st;
	AVFormatContext *oc;
	AVOutputFormat *fmt;


#if 00
	//fmt = av_guess_format("mpeg", NULL, NULL);
	//oc = avformat_alloc_context();
	avformat_alloc_output_context2(&oc, NULL, NULL, "out/out.mpg");

	if (!oc) {
         printf("Could not deduce output format from file extension: using MPEG.\n");
         avformat_alloc_output_context2(&oc, NULL, "mpeg", "out/out.mpg");
     }
     if (!oc) {
	printf("!oc");
		exit(0);
         return;
     }

	//oc->oformat = fmt;
	//sprintf(oc->filename, "out/%s.mpg", FileDateTime().c_str());
	fmt = oc->oformat;
	st = add_video_stream(oc, fmt->video_codec, pCodecCtx->width, pCodecCtx->height);

#if 0
	if (av_set_parameters(oc, NULL) < 0) {
		fprintf(stderr, "Invalid output format parameters\n");
		exit(1);
	}
#endif

	/* Write the stream header, if any. */
     if (avformat_write_header(oc, NULL) < 0) {
         fprintf(stderr, "Error occurred when opening output file\n");
		exit(0);
         return;
     }
#endif




#if 0
	outcodec = avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
	outc = avcodec_alloc_context3(outcodec);
	outpicture= avcodec_alloc_frame();

	outc->bit_rate = 400000;

	/* resolution must be a multiple of two */
	outc->width = pCodecCtx->width/2*2;
	outc->height = pCodecCtx->height/2*2;
	AVRational avr;
	avr.den = 1;
	avr.num = 25;
	outc->time_base= avr;

	outc->gop_size = 10; /* emit one intra frame every ten frames */
	outc->max_b_frames=0;//1;
	outc->pix_fmt = PIX_FMT_YUV420P;

#if 0
	outc->keyint_min = 1;                                      // minimum GOP size
	outc->i_quant_factor = (float)0.71;                        // qscale factor between P and I frames
	outc->b_frame_strategy = 20;                               ///// find out exactly what this does
	outc->qcompress = (float)0.6;                              ///// find out exactly what this does
	outc->qmin = 20;                                           // minimum quantizer
	outc->qmax = 51;                                           // maximum quantizer
	outc->max_qdiff = 4;                                       // maximum quantizer difference between frames
	outc->refs = 4;                                            // number of reference frames
	outc->trellis = 1;                                         // trellis RD Quantization
	outc->pix_fmt = PIX_FMT_YUV420P;                           // universal pixel format for video encoding
	outc->codec_id = CODEC_ID_H264;
	outc->codec_type = AVMEDIA_TYPE_VIDEO;
#endif

	int avcodecopenerr = avcodec_open2(outc, outcodec, NULL);

	if (avcodecopenerr < 0)
	{
		char m[128];
		sprintf(m, "Out context fail \n err=%d", avcodecopenerr);
		ErrMess("Error", m);
		g_quit = true;
		return;
	}

	outf = fopen("out/out.mpg", "wb");



	outbuf_size = 100000;
	outbuf = (uint8_t*)malloc(outbuf_size);
	outsize = outc->width * outc->height;
	out_picture_buf = (uint8_t*)malloc((outsize * 3) / 2); /* size for YUV 420 */

#if 1
	outpicture->data[0] = out_picture_buf;
	outpicture->data[1] = outpicture->data[0] + outsize;
	outpicture->data[2] = outpicture->data[1] + outsize / 4;
	outpicture->linesize[0] = outc->width;
	outpicture->linesize[1] = outc->width / 2;
	outpicture->linesize[2] = outc->width / 2;
#else
	/* the image can be allocated by any means and av_image_alloc() is
	* just the most convenient way if av_malloc() is to be used */
	av_image_alloc(outpicture->data, outpicture->linesize,
		outc->width, outc->height, outc->pix_fmt, 1);
#endif

	int out_size;
#endif


	i=0;
	while(av_read_frame(pFormatCtx, &packet)>=0)
	{
		// Is this a packet from the video stream?
		if(packet.stream_index==videoStream)
		{
			// Decode video frame
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

			// Did we get a video frame?
			if(frameFinished)
			{
				// Convert the image from its native format to RGB
				sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);

				// Save the frame to disk
				//if(++i<=5)
				//	SaveFrame(pFrameRGB, pCodecCtx->width, 
				//	pCodecCtx->height, i);

				LoadedTex outtex;

				//procframe
				//if(i > 90)
				ProcFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i, &outtex);

#if 0
				if(outtex.data)
				{

					/* Y */
					for(outy=0;outy<outc->height;outy++) {
						for(outx=0;outx<outc->width;outx++) {
							double Y, Cb, Cr;
							double R = outtex.data[ (outx + outy * outtex.sizex) * outtex.channels + 0 ];
							double G = outtex.data[ (outx + outy * outtex.sizex) * outtex.channels + 1 ];
							double B = outtex.data[ (outx + outy * outtex.sizex) * outtex.channels + 2 ];
							YUVfromRGB(Y, Cb, Cr, R, G, B);
							outpicture->data[0][outy * outpicture->linesize[0] + outx] = Y;
						}
					}


					/* Cb and Cr */
					for(outy=0;outy<outc->height/2;outy++) {
						for(outx=0;outx<outc->width/2;outx++) {
							double Y, Cb, Cr;
							double R = outtex.data[ (outx*2 + outy*2 * outtex.sizex) * outtex.channels + 0 ];
							double G = outtex.data[ (outx*2 + outy*2 * outtex.sizex) * outtex.channels + 1 ];
							double B = outtex.data[ (outx*2 + outy*2 * outtex.sizex) * outtex.channels + 2 ];
							YUVfromRGB(Y, Cb, Cr, R, G, B);
							outpicture->data[1][outy * outpicture->linesize[1] + outx] = Cb;
							outpicture->data[2][outy * outpicture->linesize[2] + outx] = Cr;
						}
					}
#endif

#if 0
					/* encode the image */
					out_size = avcodec_encode_video(outc, outbuf, outbuf_size, outpicture);
					fwrite(outbuf, 1, out_size, outf);

#if 0
					if (out_size > 0) {
						AVPacket pkt;
						av_init_packet(&pkt);

						if (c->coded_frame->pts != AV_NOPTS_VALUE)
							pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
						if(c->coded_frame->key_frame)
							pkt.flags |= AV_PKT_FLAG_KEY;
						pkt.stream_index= st->index;
						pkt.data= outbuf;
						pkt.size= out_size;

						/* write the compressed frame in the media file */
						int ret = av_interleaved_write_frame(oc, &pkt);

					}
#endif

#elif 00
					int ret;

					if (filter_ctx[stream_index].filter_graph) {
						av_log(NULL, AV_LOG_DEBUG, "Going to reencode&filter the frame\n");
						frame = av_frame_alloc();
						if (!frame) {
							ret = AVERROR(ENOMEM);
							break;
						}
						av_packet_rescale_ts(&packet,
							ifmt_ctx->streams[stream_index]->time_base,
							ifmt_ctx->streams[stream_index]->codec->time_base);
						dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 :
							avcodec_decode_audio4;
						ret = dec_func(ifmt_ctx->streams[stream_index]->codec, frame,
							&got_frame, &packet);
						if (ret < 0) {
							av_frame_free(&frame);
							av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
							break;
						}

						if (got_frame) {
							frame->pts = av_frame_get_best_effort_timestamp(frame);
							ret = filter_encode_write_frame(frame, stream_index);
							av_frame_free(&frame);
							if (ret < 0)
								goto end;
						} else {
							av_frame_free(&frame);
						}
					} else {
						/* remux this frame without reencoding */
						av_packet_rescale_ts(&packet,
							ifmt_ctx->streams[stream_index]->time_base,
							ofmt_ctx->streams[stream_index]->time_base);

						ret = av_interleaved_write_frame(ofmt_ctx, &packet);
						if (ret < 0)
							goto end;
					}
#endif

					i++;
				//}
			}
end:
			// Free the packet that was allocated by av_read_frame
			av_free_packet(&packet);
		}
	}

#if 0
	/* get the delayed frames */
	for(; out_size; i++) {
		out_size = avcodec_encode_video(outc, outbuf, outbuf_size, NULL);
		fwrite(outbuf, 1, out_size, outf);
	}


	/* add sequence end code to have a real mpeg file */
	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x01;
	outbuf[3] = 0xb7;

	fwrite(outbuf, 1, 4, outf);

	fclose(outf);

	free(out_picture_buf);
	free(outbuf);

	//avcodec_close(st->codec);
	avcodec_close(outc);
	av_free(outc);
	av_free(outpicture);
	
#endif






	// Free the RGB image
	av_free(buffer);
	av_free(pFrameRGB);

	// Free the YUV frame
	av_free(pFrame);

	// Close the codecs
	avcodec_close(pCodecCtx);
	avcodec_close(pCodecCtxOrig);

	// Close the video file
	avformat_close_input(&pFormatCtx);
}
#endif