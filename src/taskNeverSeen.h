#ifndef __TASK_NEVERSEEN_H__
#define __TASK_NEVERSEEN_H__

#include "lwTinyFrame/lwResourceHolder.h"

class TouchPoint{
public:
	TouchPoint();
	static void main();
	void draw(float alpha);
	bool isReadyToPlay();
    bool needBlock();

private:
	static std::list<float> _cs;

public:
	float x0, y0;
	float x, y;
	int gesId;
	float gesX, gesY;
    lw::Color color;
};

class HotImage{
public:
	HotImage(const char* file, int u, int v, int w, int h, float x, float y, float rot, float scale, float xv, float yv, float rotv, float scalev);
	~HotImage();
	void addPoint(float x, float y);
	void main(float dt);
	void draw();
	bool onEvents(std::vector<lw::TouchEvent>& events);
	enum Step{
		READY,
		PLAYING,
		END,
		FAILED,
	};
	Step getStep(){
		return _step;
	}
	int getScore(){
		return _score;
	}

private:
	std::vector<TouchPoint> _touchPoints;
	lw::Sprite* _pSprite;
    lw::Sprite* _pSptBlock;
	lw::GestureMgr _gesMgr;
	
	Step _step;
	int _frm;
	cml::Vector2 _pos;
	float _rot;
	float _scale;
	cml::Vector2 _linearV;
	float _rotV;
	float _scaleV;
	int _score;
};

class ScoreDisp1{
public:
	ScoreDisp1();
	~ScoreDisp1();
	void hide();
	void reset(int score);
	void main();
	void draw();

private:
	lw::Font* _pFont;
	int _frm;
	float _x, _y;
};

class TaskNeverSeen : public lw::Task, public lw::ButtonCallback, public lw::Singleton<TaskNeverSeen>{
public:
	virtual void vBegin();
	virtual void vEnd();
	virtual void vMain(float dt);
	virtual void vDraw(float dt);
	virtual bool vOnTouchEvent(std::vector<lw::TouchEvent>& events);
    virtual void vGesture(const lw::Gesture* pGst);
	void addScore(int score);
	virtual void vOnClick(lw::UIButton* pButton);

private:
	void reset();
	void createHotImage();
	HotImage* _pHotImage;
	lw::UIText* _pTextScore;
	float _totalScore;
	lw::ResourceHolder _resHolder;
	std::vector<int> _scores;
	enum Step{
		STEP_PLAY,
		STEP_SCORE,
		STEP_FAILED,
	};
	Step _step;
	ScoreDisp1* _pScoreDisp1;
	lw::UIButton* _pBtnQuit;
	lw::UIButton* _pBtnRetry;
    std::list<TouchPoint> _touchPoints;
};


#endif //__TASK_NEVERSEEN_H__