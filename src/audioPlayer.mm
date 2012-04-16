#include "stdafx.h"
#include "audioPlayer.h"
#import "AVFoundation/AVAudioPlayer.h"

@interface AudioPlayerDelegate : NSObject<AVAudioPlayerDelegate>{
    
}
@end

@implementation AudioPlayerDelegate


@end

struct AudioPlayer::Data{
    AVAudioPlayer *player;
    AudioPlayerDelegate *delegate;
};

AudioPlayer::AudioPlayer(const char* file){
    _pData = new Data;
    
    _f f(file);
    const char* str = f.getPath();
    NSURL *fileURL = [[NSURL alloc] initFileURLWithPath: [NSString stringWithUTF8String : str]];
    
    _pData->player = [[AVAudioPlayer alloc] initWithContentsOfURL: fileURL
                                           error: nil];
    [fileURL release];
    
    _pData->delegate = [[AudioPlayerDelegate alloc] init];
    [_pData->player setDelegate:_pData->delegate];
    [_pData->player prepareToPlay];
}

AudioPlayer::~AudioPlayer(){
    [_pData->player release];
    [_pData->delegate release];
    delete _pData;
}

void AudioPlayer::play(){
    [_pData->player play];
}

void AudioPlayer::pause(){
    [_pData->player pause];
}

void AudioPlayer::stop(){
    [_pData->player stop];
}

void AudioPlayer::setCurrentTime(float second){
    _pData->player.currentTime = second;
}

bool AudioPlayer::isPlaying(){
    return _pData->player.isPlaying;
}
