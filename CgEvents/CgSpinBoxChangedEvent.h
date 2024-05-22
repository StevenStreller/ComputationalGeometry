#ifndef CGSPINBOXCHANGEDEVENT_H
#define CGSPINBOXCHANGEDEVENT_H

#include "../CgBase/CgBaseEvent.h"

class CgSpinBoxChangedEvent : public CgBaseEvent
{
public:
    //Add more buttons if needed
    enum CgSpinBoxType {kIndex = 1, kValue = 2, kdDepth = 3};

    CgSpinBoxChangedEvent();

    CgSpinBoxChangedEvent(Cg::EventType type, CgSpinBoxType spinBoxChanged, int value);

    //inherited
    Cg::EventType  getType();
    CgBaseEvent* clone();

    //own specific methods
    CgSpinBoxType spinBox();
    int getValue();

private:
    Cg::EventType m_type;
    CgSpinBoxType m_spinBox;
    int value;
};

#endif // CGSPINBOXCHANGEDEVENT_H
