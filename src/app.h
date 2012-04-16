#ifndef __APP_H__
#define __APP_H__

#include "lwTinyFrame/lwApp.h"

class LWApp : public lw::App{
public:
	LWApp();
    ~LWApp();

private:
	virtual void vInit();
	virtual void vQuit();
	virtual void vMain();
};

#endif //__APP_H__