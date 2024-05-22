#ifndef CGCHECKBOXCLICKEDEVENT_H
#define CGCHECKBOXCLICKEDEVENT_H

#include "../CgBase/CgBaseEvent.h"

class CgCheckboxClickedEvent : public CgBaseEvent
{
public:
    //Add more buttons if needed
    enum CgCheckboxType {showSmoothingMesh = 1};

    CgCheckboxClickedEvent();

    CgCheckboxClickedEvent(Cg::EventType type, CgCheckboxType checkboxChanged, bool checked);

    //inherited
    Cg::EventType  getType();
    CgBaseEvent* clone();

    //own specific methods
    CgCheckboxType checkbox();
    bool isChecked();

private:
    Cg::EventType m_type;
    CgCheckboxType m_checkbox;
    bool checked;
};

#endif // CGCHECKBOXCLICKEDEVENT_H
