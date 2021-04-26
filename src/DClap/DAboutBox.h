// DAboutBox.h
// d.g.gilbert


#ifndef _DABOUTBOX_
#define _DABOUTBOX_

#include "Dvibrant.h"
#include "DWindow.h"
#include "DPanel.h"


class DAboutBoxPanel : public DPanel 
{
public:
	char* fAboutText;
	Boolean fKeepItShort;
	short	fAboutLines, fDclapLines, fNcbiLines;

	DAboutBoxPanel(DWindow* itsWindow, const char* aboutText);
	~DAboutBoxPanel();
	virtual void SizeOneText(const char* text, long& maxlen, short& linecount);
	virtual void SizeInfo();
	virtual void Draw();
	virtual void Release(Nlm_PoinT mouse);
};

class DAboutBoxWindow : public DWindow 
{
public:
	DAboutBoxPanel* fMypanel;
	DAboutBoxWindow(const char* aboutText = NULL);
	~DAboutBoxWindow();
	virtual void Close();
};


#endif
