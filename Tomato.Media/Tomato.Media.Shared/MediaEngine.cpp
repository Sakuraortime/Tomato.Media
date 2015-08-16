//
// Tomato Media
// Media Engine
// 作者：SunnyCase
// 创建时间：2015-08-06
//
#include "pch.h"
#include "MediaEngine.h"
#include "MediaRenderSink.h"

using namespace NS_MEDIA;
using namespace concurrency;
using namespace WRL;

MediaEngine::MediaEngine()
	:sourceReader(Make<VideoSourceReader>()), mediaSink(Make<MediaRenderSink>())
{

}

void MediaEngine::SetMediaSource(IMFMediaSource* mediaSource)
{
	ConfigureSourceReader(mediaSource);
}

void MediaEngine::ConfigureSourceReader(IMFMediaSource* mediaSource)
{
	sourceReader->SetMediaSource(mediaSource);
	ConfigureMediaSink();
}

void MediaEngine::ConfigureMediaSink()
{
	ThrowIfFailed(mediaSink->GetStreamSinkByIndex(1, &videoSink));
	ComPtr<IMFMediaTypeHandler> mediaTypeHandler;
	ThrowIfFailed(videoSink->GetMediaTypeHandler(&mediaTypeHandler));
	ThrowIfFailed(mediaTypeHandler->SetCurrentMediaType(sourceReader->OutputMediaType));
}

void MediaEngine::Play()
{
	sourceReader->Start();

	create_task([=]
	{
		Sleep(2000);

		while (true)
		{
			ComPtr<IMFSample> videoSample;
			if (sourceReader->TryReadVideoSample(videoSample))
			{
				ThrowIfFailed(videoSink->ProcessSample(videoSample.Get()));
			}
			Sleep(40);
		}
	});
}

ComPtr<IVideoRender> MediaEngine::get_VideoRender() const
{
	ComPtr<IVideoRender> videoRender;
	ThrowIfFailed(MFGetService(mediaSink.Get(), MF_TM_VIDEORENDER_SERVICE, IID_PPV_ARGS(&videoRender)));
	return videoRender;
}