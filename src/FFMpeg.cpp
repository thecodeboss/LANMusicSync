#include "FFMpeg.h"
#include "Debug.h"
#include <iostream>

void printAudioFrameInfo(const AVCodecContext* codecContext, const AVFrame* frame)
{
	// See the following to know what data type (unsigned char, short, float, etc) to use to access the audio data:
	// http://ffmpeg.org/doxygen/trunk/samplefmt_8h.html#af9a51ca15301871723577c730b5865c5
	ConsolePrintf("Audio frame info:\n"
		"  Sample count: %d\n"
		"  Channel count: %d\n"
		"  Format: %s\n"
		"  Bytes per sample: %d\n"
		"  Is planar? %d\n",
		frame->nb_samples, codecContext->channels, av_get_sample_fmt_name(codecContext->sample_fmt),
		av_get_bytes_per_sample(codecContext->sample_fmt), av_sample_fmt_is_planar(codecContext->sample_fmt));
// 
// 	std::cout << "frame->linesize[0] tells you the size (in bytes) of each plane\n";
// 
// 	if (codecContext->channels > AV_NUM_DATA_POINTERS && av_sample_fmt_is_planar(codecContext->sample_fmt))
// 	{
// 		std::cout << "The audio stream (and its frames) have too many channels to fit in\n"
// 			<< "frame->data. Therefore, to access the audio data, you need to use\n"
// 			<< "frame->extended_data to access the audio data. It's planar, so\n"
// 			<< "each channel is in a different element. That is:\n"
// 			<< "  frame->extended_data[0] has the data for channel 1\n"
// 			<< "  frame->extended_data[1] has the data for channel 2\n"
// 			<< "  etc.\n";
// 	}
// 	else
// 	{
// 		std::cout << "Either the audio data is not planar, or there is enough room in\n"
// 			<< "frame->data to store all the channels, so you can either use\n"
// 			<< "frame->data or frame->extended_data to access the audio data (they\n"
// 			<< "should just point to the same data).\n";
// 	}
// 
// 	std::cout << "If the frame is planar, each channel is in a different element.\n"
// 		<< "That is:\n"
// 		<< "  frame->data[0]/frame->extended_data[0] has the data for channel 1\n"
// 		<< "  frame->data[1]/frame->extended_data[1] has the data for channel 2\n"
// 		<< "  etc.\n";
// 
// 	std::cout << "If the frame is packed (not planar), then all the data is in\n"
// 		<< "frame->data[0]/frame->extended_data[0] (kind of like how some\n"
// 		<< "image formats have RGB pixels packed together, rather than storing\n"
// 		<< " the red, green, and blue channels separately in different arrays.\n";
}

FFMpeg::FFMpeg()
{
	av_register_all();
}

void FFMpeg::LoadFile( std::string fileName )
{
	frame = av_frame_alloc();
	if (!frame)
	{
		ConsolePrintf("Error allocating the frame");
		return;
	}

	formatContext = nullptr;
	if (avformat_open_input(&formatContext, fileName.c_str(), nullptr, nullptr) != 0)
	{
		av_free(frame);
		ConsolePrintf("Error opening the file");
		return;
	}

	if (avformat_find_stream_info(formatContext, nullptr) < 0)
	{
		av_free(frame);
		avformat_close_input(&formatContext);
		ConsolePrintf("Error finding the stream info");
		return;
	}

	// Find the audio stream
	AVCodec* cdc = nullptr;
	int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &cdc, 0);
	if (streamIndex < 0)
	{
		av_free(frame);
		avformat_close_input(&formatContext);
		ConsolePrintf("Could not find any audio stream in the file");
		return;
	}

	audioStream = formatContext->streams[streamIndex];
	codecContext = audioStream->codec;
	codecContext->codec = cdc;

	if (avcodec_open2(codecContext, codecContext->codec, nullptr) != 0)
	{
		av_free(frame);
		avformat_close_input(&formatContext);
		ConsolePrintf("Couldn't open the context with the decoder");
		return;
	}

	ConsolePrintf("This stream has %d channels and a sample rate of %d Hz", codecContext->channels, codecContext->sample_rate);
	ConsolePrintf("The data is in the format %s", av_get_sample_fmt_name(codecContext->sample_fmt));

	av_init_packet(&readingPacket);

	bWavFormatReady = false;
}

Buffer* FFMpeg::GetFrame()
{
	if (av_read_frame(formatContext, &readingPacket) == 0)
	{
		if (readingPacket.stream_index == audioStream->index)
		{
			AVPacket decodingPacket = readingPacket;

			// Audio packets can have multiple audio frames in a single packet
			while (decodingPacket.size > 0)
			{
				// Try to decode the packet into a frame
				// Some frames rely on multiple packets, so we have to make sure the frame is finished before
				// we can use it
				int gotFrame = 0;
				int result = avcodec_decode_audio4(codecContext, frame, &gotFrame, &decodingPacket);

				if (result >= 0 && gotFrame)
				{
					decodingPacket.size -= result;
					decodingPacket.data += result;

					// We now have a fully decoded audio frame
					printAudioFrameInfo(codecContext, frame);

					if (!bWavFormatReady) {
						setupWavFormat();
					}
				}
				else
				{
					decodingPacket.size = 0;
					decodingPacket.data = nullptr;
				}
			}
		}

		// You *must* call av_free_packet() after each call to av_read_frame() or else you'll leak memory
		av_free_packet(&readingPacket);

		int sizediff = frame->nb_samples * (WavFormat.BitsPerSample / 8) * WavFormat.NumChannels;
		Buffer* buffer = nullptr;
		if (av_sample_fmt_is_planar(codecContext->sample_fmt)) {
			buffer = new Buffer(sizediff);

			// Convert from planar to non-planar
			int16_t *dst = (int16_t *)&buffer->front();
			int16_t **src = (int16_t **)frame->extended_data;
			for (int i = 0; i < frame->nb_samples; i++) {
				for (int ch = 0; ch < frame->channels; ch++) {
					dst[i * frame->channels + ch] = src[ch][i];
				}
			}

		} else {
			buffer = new Buffer(frame->data[0], frame->data[0] + sizediff);
		}

		return buffer;
	} else {
		return nullptr;
	}
}

FFMpeg::~FFMpeg()
{
	// Clean up!
	av_free(frame);
	avcodec_close(codecContext);
	avformat_close_input(&formatContext);
}

void FFMpeg::setupWavFormat()
{
	WavFormat.Format = PulseCodeModulation;
	WavFormat.SampleRate = static_cast<unsigned>(codecContext->sample_rate);
	WavFormat.BitsPerSample = 8 * static_cast<unsigned>(av_get_bytes_per_sample(codecContext->sample_fmt));
	WavFormat.NumChannels = static_cast<unsigned>(codecContext->channels);
	WavFormat.BlockAlign = WavFormat.NumChannels * (WavFormat.BitsPerSample / 8);
	WavFormat.AvgBitsPerSecond = WavFormat.SampleRate * WavFormat.BlockAlign;

	bWavFormatReady = true;
}

WAVEFORMATEX * FFMpeg::GetWavFormat() {
	WAVEFORMATEX * format = new WAVEFORMATEX();
	memcpy(format, &WavFormat, sizeof(WavHeader));
	format->cbSize = 0; // Extra bits not in our header
	return format;
}
