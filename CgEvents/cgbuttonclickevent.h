#ifndef CGBUTTONCLICKEVENT_H
#define CGBUTTONCLICKEVENT_H

#include "../CgBase/CgBaseEvent.h"

class CgButtonClickEvent : public CgBaseEvent
{
public:
    //Add more buttons if needed
    enum CgButtonType {SubdivideButton = 1, SmoothButton = 2};
    CgButtonClickEvent();

    CgButtonClickEvent(Cg::EventType type, CgButtonType buttonClicked);

    //inherited
    Cg::EventType  getType();
    CgBaseEvent* clone();

    //own specific methods
    CgButtonType button();

private:
    Cg::EventType m_type;
    CgButtonType m_button;
};

#endif // CGBUTTONCLICKEVENT_H
