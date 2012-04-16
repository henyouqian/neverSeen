#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

class AudioPlayer{
public:
    AudioPlayer(const char* file);
    ~AudioPlayer();
    void play();
    void pause();
    void stop();
    void setCurrentTime(float second);
    bool isPlaying();

private:
    struct Data;
    Data* _pData;
};

#endif //__AUDIO_PLAYER_H__