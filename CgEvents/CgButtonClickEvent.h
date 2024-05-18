#ifndef CGBUTTONCLICKEVENT_H
#define CGBUTTONCLICKEVENT_H

#include "../CgBase/CgBaseEvent.h"
#include "../CgBase/CgEnums.h"
#include <string>
#include <iostream>

class CgButtonClickEvent : public CgBaseEvent {

public:

    enum CgButtonType {
        SubdivButton = 0
    };

    CgButtonClickEvent();

    CgButtonClickEvent(Cg::EventType
    eventType,
    CgButtonType buttonClicked
    );

    //inherited
    Cg::EventType getType();

    CgBaseEvent *clone();

    //own specific methods
    CgButtonType button() const;


private:
    Cg::EventType m_type;
    CgButtonType m_button;

};

#endif // CGBUTTONCLICKEVENT_H
