#include "stdafx.h"
#include "taskNeverSeen.h"
#include "geomDraw.h"

namespace{
	const int FRAME_NUM = 500;
	const int FADE_FRAME = 60;
	const int TP_LIFE = 60;
	const int TP_NEXT_GEN = 30;
	const int TP_DIM = 40;
	const float TP_FAILED_RADIUS = 40.f;
    const float TP_START_RADIUS = 20.f;
    const float TP_BLOCK_RADIUS = 15.f;
	const float TP_DRAW_RADIUS = 100.f;
	const float TP_TOTAL_SCORE = 10000.f;
	const int IMG_NUM = 7;
    const int Y_OFFSET = 10;
}

std::list<float> TouchPoint::_cs;

TouchPoint::TouchPoint()
:x(-1000.f), y(-1000.f), gesId(-1), gesX(1000), gesY(1000){
	//_cs.push_back(1);
}
void TouchPoint::main(){
	std::list<float>::iterator it = _cs.begin();
	std::list<float>::iterator itEnd = _cs.end();
	for ( ; it != itEnd; ++it ){
		*it = *it+1;
	}
	it = _cs.begin();
	if ( *it >= TP_LIFE ){
		_cs.pop_front();
	}
	if ( _cs.empty() || _cs.back() >= TP_NEXT_GEN ){
		_cs.push_back(1);
	}
}
void TouchPoint::draw(float a){
	std::list<float>::iterator it = _cs.begin();
	std::list<float>::iterator itEnd = _cs.end();
	geomDrawBegin();
	lw::Color fillColor(0, 0, 0, 0);
	float r = TP_DRAW_RADIUS/TP_LIFE;
	for ( ; it != itEnd; ++it ){
		if ( *it <= TP_DIM ){
			color.a = (GLubyte)(255.f*a);
		}else{
			color.a = (GLubyte)((TP_LIFE-(*it))/(float)(TP_LIFE-TP_DIM)*255.f*a);
		}
		geomDrawCircle(x, y, *it*r, fillColor, color, 24);
	}
	geomDrawEnd();
}

bool TouchPoint::isReadyToPlay(){
	if ( gesId >= 0 ){
		cml::Vector2 v;
		v[0] = x - gesX;
		v[1] = y - gesY;
		float dist = v.length();
		if ( dist < TP_START_RADIUS ){
			return true;
		}
	}
	return false;
}

bool TouchPoint::needBlock(){
    if ( gesId >= 0 ){
        cml::Vector2 v;
		v[0] = x - gesX;
		v[1] = y - gesY;
		float dist = v.length();
		if ( dist > TP_BLOCK_RADIUS ){
			return true;
		}else{
            return false;
        }
    }
    return true;
}

HotImage::HotImage(const char* file, int u, int v, int w, int h, float x, float y, float rot, float scale, float xv, float yv, float rotv, float scalev)
:_step(READY), _frm(FRAME_NUM), _score(0){
	_pSprite = lw::Sprite::create(file, false);
	_pSprite->setUV(u, v, w, h);
	_pSprite->setAnchor(_pSprite->getWidth()*.5f, _pSprite->getHeight()*.5f);
    _pSptBlock = lw::Sprite::create("block.png");
    _pSptBlock->setUV(0, 0, 49, 40);
    _pSptBlock->setAnchor(24, 20);
	_pos[0] = x;
	_pos[1] = y;
	_rot = rot;
	_scale = scale;
	_linearV[0] = xv;
	_linearV[1] = yv;
	_rotV = rotv;
	_scaleV = scalev;
}

HotImage::~HotImage(){
	delete _pSprite;
    delete _pSptBlock;
}

void HotImage::addPoint(float x, float y){
	TouchPoint pt;
	pt.x0 = x - _pSprite->getWidth()*.5f;
	pt.y0 = y - _pSprite->getHeight()*.5f;
    pt.color.set(255, 20, 20, 255);
	_touchPoints.push_back(pt);
}

void HotImage::main(float dt){
	if ( _step == PLAYING ){
		std::vector<TouchPoint>::iterator it = _touchPoints.begin();
		std::vector<TouchPoint>::iterator itEnd = _touchPoints.end();
		for ( ; it != itEnd; ++it ){
			if ( it->gesId == -1 ){
				_step = FAILED;
                TaskNeverSeen::s().addScore(_score);
				_score = 0;
				return;
			}else{
				float dx = it->gesX-it->x;
				float dy = it->gesY-it->y;
				float a = dx*dx+dy*dy;
				float b = TP_FAILED_RADIUS*TP_FAILED_RADIUS;
				if ( a > b ){
					it->gesId = -1;
					_step = FAILED;
					TaskNeverSeen::s().addScore(_score);
					_score = 0;
					return;
				}
				float f = (b-a)/b;
				f = f*f*f*f;
				f *= f;
				_score += (int)(f*TP_TOTAL_SCORE/FRAME_NUM);
			}
		}

		_pos += _linearV;
		_rot += _rotV;
		_scale += _scaleV;
		_pSprite->setPos(_pos[0], _pos[1]);
		_pSprite->setRotate(_rot);
		_pSprite->setScale(_scale, _scale);
		_frm -= 1;
		if ( _frm == 0 ){
			_step = END;
			TaskNeverSeen::s().addScore(_score);
			_score = 0;
		}
	}

	if ( _step == READY || _step == PLAYING ){
		std::vector<TouchPoint>::iterator it = _touchPoints.begin();
		std::vector<TouchPoint>::iterator itEnd = _touchPoints.end();
		if ( _step == READY ){
			bool canPlay = true;
			for ( ; it != itEnd; ++it ){
				canPlay = it->isReadyToPlay();
				if ( !canPlay ){
					break;
				}
			}
			if ( canPlay ){
				_step = PLAYING;
			}
		}
		it = _touchPoints.begin();
		itEnd = _touchPoints.end();
		cml::Vector2 v;
		cml::Matrix33 m, m1;
		for ( ; it != itEnd; ++it ){
			v[0] = it->x0;
			v[1] = it->y0;
			cml::matrix_translation_2D(m, _pos[0], _pos[1]);
			cml::matrix_rotation_2D(m1, _rot);
			m *= m1;
			cml::matrix_scale_2D(m1, _scale, _scale);
			m *= m1;
			v = cml::transform_point_2D(m, v);
			it->x = v[0];
			it->y = v[1];
		}
        TouchPoint::main();
	}
}

void HotImage::draw(){
	float alpha = 1.f;
	if ( _step == READY || _step == PLAYING ){
		if ( _frm < FADE_FRAME ){
			alpha = (float)_frm/FADE_FRAME;
		}
		else if ( _frm > FRAME_NUM-FADE_FRAME ){
			alpha = (float)(FRAME_NUM-_frm)/FADE_FRAME;
		}
		_pSprite->setColor(lw::Color(1.f, 1.f, 1.f, alpha));
		_pSprite->collect();
		_pSprite->flush();
	}
	if ( _step == READY || _step == PLAYING ){
		std::vector<TouchPoint>::iterator it = _touchPoints.begin();
		std::vector<TouchPoint>::iterator itEnd = _touchPoints.end();
		alpha = 1.f;
		if ( _frm < FADE_FRAME ){
			alpha = (float)_frm/FADE_FRAME;
		}
		for ( ; it != itEnd; ++it ){
			it->draw(alpha);
            if ( _step == PLAYING && it->needBlock() ){
                _pSptBlock->setPos(it->x, it->y);
                _pSptBlock->collect();
            }
		}

		it = _touchPoints.begin();
		itEnd = _touchPoints.end();
		//geomDrawBegin();
		//lw::Color color1(0, 0, 0, 0);
		//lw::Color color2(255, 20, 20, (int)(alpha*255));
		//float x = 30.f;
		//float y = 30.f;
		//float r = 20.f;
		//float f = r/TP_RADIUS;
		//for ( ; it != itEnd; ++it ){
		//	geomDrawCircle(x, y, r, color1, color2, 36);
		//	geomDrawCircle(x, y, 1.5f, color2, color1, 12);
		//	if ( it->gesId != -1 ){
		//		geomDrawCircle(x+f*(it->gesX-it->x), y+f*(it->gesY-it->y), 2.f, color2, color1, 12);
		//	}
		//	x += r*2.f+5.f;
		//}
		//geomDrawEnd();
	}
}

bool HotImage::onEvents(std::vector<lw::TouchEvent>& events){
	_gesMgr.onTouchEvent(events);
	const std::list<lw::Gesture>& gestures = _gesMgr.getGestures();
	std::list<lw::Gesture>::const_iterator it = gestures.begin();
	std::list<lw::Gesture>::const_iterator itEnd = gestures.end();
	std::vector<TouchPoint>::iterator itTP;
	std::vector<TouchPoint>::iterator itTPEnd;
	for ( ; it != itEnd; ++it ){
		if ( it->updated ){
			lw::TouchEvent evt = it->evt;
            evt.y += Y_OFFSET;
			if ( _step == READY ){
				if ( evt.type == lw::TouchEvent::TOUCH || evt.type == lw::TouchEvent::MOVE ){
					itTP = _touchPoints.begin();
					itTPEnd = _touchPoints.end();
					for ( ; itTP != itTPEnd; ++itTP ){
						if ( itTP->gesId == -1 ){
							float dx = itTP->x-evt.x;
							float dy = itTP->y-evt.y;
							if ( dx*dx+dy*dy < TP_START_RADIUS*TP_START_RADIUS ){
								itTP->gesId = it->id;
								itTP->gesX = evt.x;
								itTP->gesY = evt.y;
								break;
							}
						}else if ( itTP->gesId == it->id ){
							float dx = itTP->x-evt.x;
							float dy = itTP->y-evt.y;
							if ( dx*dx+dy*dy > TP_START_RADIUS*TP_START_RADIUS ){
								itTP->gesId = -1;
							}else{
								itTP->gesX = evt.x;
								itTP->gesY = evt.y;
							}
							break;
						}
					}
				}else if ( evt.type == lw::TouchEvent::UNTOUCH ){
					itTP = _touchPoints.begin();
					itTPEnd = _touchPoints.end();
					for ( ; itTP != itTPEnd; ++itTP ){
						if ( it->id == itTP->gesId ){
							itTP->gesId = -1;
							break;
						}
					}
				}
			}
			if ( _step == PLAYING ){
				if ( evt.type == lw::TouchEvent::MOVE ){
					itTP = _touchPoints.begin();
					itTPEnd = _touchPoints.end();
					for ( ; itTP != itTPEnd; ++itTP ){
						if ( it->id == itTP->gesId ){
							float dx = itTP->x-evt.x;
							float dy = itTP->y-evt.y;
							if ( dx*dx+dy*dy > TP_FAILED_RADIUS*TP_FAILED_RADIUS ){
								itTP->gesId = -1;
							}else{
								itTP->gesX = evt.x;
								itTP->gesY = evt.y;
							}
							break;
						}
					}
				}else if ( evt.type == lw::TouchEvent::UNTOUCH ){
					itTP = _touchPoints.begin();
					itTPEnd = _touchPoints.end();
					for ( ; itTP != itTPEnd; ++itTP ){
						if ( it->id == itTP->gesId ){
							itTP->gesId = -1;
							break;
						}
					}
				}
			}
		}
	}
	_gesMgr.main();
	return false;
}

void TaskNeverSeen::vBegin(){
	lw::App::s().setOrient(lw::App::ORIENTATION_UP);

	//_resHolder.addTexture("hotimg1.png");
	//_resHolder.addTexture("hotimg2.png");
	//_resHolder.addTexture("hotimg3.png");
	//_resHolder.addTexture("hotimg4.png");
	//_resHolder.addTexture("hotimg5.png");
	//_resHolder.addTexture("hotimg6.png");
	//_resHolder.addTexture("hotimg7.png");
	_resHolder.addFont("eurostile30.fnt");

	_pTextScore = lw::UIText::create("eurostile30.fnt");
	_pTextScore->setPos(140, 10);
	_pTextScore->setText(L"Score:0");

	_pHotImage = NULL;
	_pScoreDisp1 = new ScoreDisp1;

	float x = 30;
	float y = 400;
	float w = 100;
	float h = 60;
    lw::Button9Def def("ui.png", 0, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, "calibri20.fnt", GL_NEAREST);
	_pBtnQuit = lw::UIButton::create9(def);
	_pBtnQuit->setPos(x, y);
	_pBtnQuit->setSize(w, h);
	_pBtnQuit->setText(L"Quit");
	_pBtnQuit->setCallback(this);

	_pBtnRetry = lw::UIButton::create9(def);
	_pBtnRetry->setPos(320.f-x-w, y);
	_pBtnRetry->setSize(w, h);
	_pBtnRetry->setText(L"Retry");
	_pBtnRetry->setCallback(this);

	reset();
}

void TaskNeverSeen::reset(){
	_step = STEP_PLAY;
	_scores.clear();
	_totalScore = 0;
	_pScoreDisp1->hide();
	_pBtnQuit->show(false);
	_pBtnRetry->show(false);
	createHotImage();
    _touchPoints.clear();
}

void TaskNeverSeen::createHotImage(){
	if ( _pHotImage ){
		delete _pHotImage;
		_pHotImage = NULL;
	}
	int n = (int)_scores.size();
    const int w = 320;
    const int h = 480;
    const float x = 160.f;
    const float y = 240.f;
    _touchPoints.clear();
	if ( n == 0 ){
		_pHotImage = new HotImage("hotimg1.png", 0, 0, w, h, x, y, 0.f, 1.0f, -0.2f, -0.2f, -0.0001f, 0.001f);
		_pHotImage->addPoint(197.f, 352.f);
	}else if ( n == 1 ){
		_pHotImage = new HotImage("hotimg2.png", 0, 0, w, h, x, y, 0.f, 1.0f, -0.2f, 0.1f, 0.0005f, 0.002f);
		_pHotImage->addPoint(216.f, 189.f);
	}else if ( n == 2 ){
		_pHotImage = new HotImage("hotimg3.png", 0, 0, w, h, x, y, 0.f, 1.f, -0.1f, -0.f, -0.001f, 0.002f);
		_pHotImage->addPoint(170.f, 222.f);
	}else if ( n == 3 ){
		_pHotImage = new HotImage("hotimg4.png", 0, 0, w, h, x, y, 0.f, 1.f, 0.2f, 0.f, 0.001f, 0.002f);
		_pHotImage->addPoint(149.f, 247.f);
	}else if ( n == 4 ){
		_pHotImage = new HotImage("hotimg5.png", 0, 0, w, h, x, y, 0.f, 1.3f, 0.05f, 0.f, -0.001f, 0.0015f);
		_pHotImage->addPoint(128.f, 195.f);
		_pHotImage->addPoint(215.f, 200.f);
	}else if ( n == 5 ){
		_pHotImage = new HotImage("hotimg6.png", 0, 0, w, h, x, y, 0.f, 1.f, 0.1f, 0.f, 0.0002f, 0.002f);
		_pHotImage->addPoint(112.f, 252.f);
		_pHotImage->addPoint(199.f, 255.f);
	}else if ( n == 6 ){
		_pHotImage = new HotImage("hotimg7.png", 0, 0, w, h, x, y, 0.f, 1.f, 0.1f, 0.1f, 0.0001f, 0.0f);
		_pHotImage->addPoint(155.f, 165.f);
	}
}

void TaskNeverSeen::vEnd(){
	delete _pHotImage;
	delete _pTextScore;
	delete _pScoreDisp1;
	delete _pBtnQuit;
	delete _pBtnRetry;
	_resHolder.clear();
    _touchPoints.clear();
	lw::App::s().popOrient();
}

void TaskNeverSeen::vMain(float dt){
    int score = _totalScore;
    if ( _pHotImage ){
        _pHotImage->main(dt);
        score += _pHotImage->getScore();
        HotImage::Step imgStep = _pHotImage->getStep();
        if ( imgStep == HotImage::END ){
            createHotImage();
        }
        
        if ( _step == STEP_PLAY ){
            if ( imgStep == HotImage::FAILED ){
                _pBtnQuit->show(true);
                _pBtnRetry->show(true);
                _step = STEP_FAILED;
            }
        }
    }

	std::wstringstream ss;
	ss << L"Score:" << score;
	_pTextScore->setText(ss.str().c_str());
	_pScoreDisp1->main();
}

void TaskNeverSeen::vDraw(float dt){
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if ( _step == STEP_PLAY ){
		_pHotImage->draw();
        std::list<TouchPoint>::iterator it = _touchPoints.begin();
        std::list<TouchPoint>::iterator itend = _touchPoints.end();
        for ( ; it != itend; ++it ){
            it->draw(1.f);
        }
	}else if ( _step == STEP_SCORE ){

	}
	_pScoreDisp1->draw();
    
}

void TaskNeverSeen::addScore(int score){
	_totalScore += score;
	_scores.push_back(score);
	_pScoreDisp1->reset(score);
	if ( _scores.size() == IMG_NUM ){
		_step = STEP_SCORE;
		_pBtnQuit->show(true);
		_pBtnRetry->show(true);
	}
}

void TaskNeverSeen::vOnClick(lw::UIButton* pButton){
	if ( pButton == _pBtnRetry  ){
		reset();
	}else if ( pButton == _pBtnQuit ) {
		//stop();
    }
}

bool TaskNeverSeen::vOnTouchEvent(std::vector<lw::TouchEvent>& events){
    if ( _pHotImage ){
        return _pHotImage->onEvents(events);
    }
    return false;
}

void TaskNeverSeen::vGesture(const lw::Gesture* pGst){
    lw::TouchEvent evt = pGst->evt;
    evt.y += Y_OFFSET;
    if ( evt.type == lw::TouchEvent::TOUCH ){
        TouchPoint tp;
        tp.x = evt.x;
        tp.y = evt.y;
        tp.gesId = pGst->id;
        tp.color.set(20, 255, 20, 255);
        _touchPoints.push_back(tp);
    }else if ( evt.type == lw::TouchEvent::MOVE ){
        std::list<TouchPoint>::iterator it = _touchPoints.begin();
        std::list<TouchPoint>::iterator itend = _touchPoints.end();
        for ( ; it != itend; ++it ){
            if ( it->gesId == pGst->id ){
                it->x = evt.x;
                it->y = evt.y;
                break;
            }
        }
    }else if ( evt.type == lw::TouchEvent::UNTOUCH ){
        std::list<TouchPoint>::iterator it = _touchPoints.begin();
        std::list<TouchPoint>::iterator itend = _touchPoints.end();
        for ( ; it != itend; ++it ){
            if ( it->gesId == pGst->id ){
                _touchPoints.erase(it);
                break;
            }
        }
    }
}

ScoreDisp1::ScoreDisp1(){
	_pFont = lw::Font::create("eurostile30.fnt");
	_pFont->setAlign(lw::ALIGN_BOTTOM_MID);
	
}
ScoreDisp1::~ScoreDisp1(){
	delete _pFont;
}

namespace{
	const int SCORE_DISP_FRM = 160;
	const int SCORE_DISP_DIM = 10;
	const float SCORE_DISP_MAX_Y = 40;
}

void ScoreDisp1::hide(){
	_frm = SCORE_DISP_FRM-1;
}

void ScoreDisp1::reset(int score){
	std::wstringstream ss;
	ss << score;
	_pFont->setText(ss.str().c_str());
	_frm = 0;
	_x = 50.f;
	_y = 0.f;
	_pFont->setPos(_x, _y);
}
void ScoreDisp1::main(){
	if ( _frm < SCORE_DISP_FRM-1 ){
		++_frm;
	}
	if ( _frm < SCORE_DISP_DIM ){
		_y = (float)_frm/SCORE_DISP_DIM*SCORE_DISP_MAX_Y;
	}else if ( _frm >= SCORE_DISP_FRM-SCORE_DISP_DIM ){
		_y = ((float)SCORE_DISP_FRM-_frm)/SCORE_DISP_DIM*SCORE_DISP_MAX_Y;
	}else{
		_y = (float)SCORE_DISP_MAX_Y;
	}
	_pFont->setPos(_x, _y);
}
void ScoreDisp1::draw(){
	if ( _frm < SCORE_DISP_FRM-1 ){
		_pFont->collect();
	}
}
