#ifndef CGRADIOBUTTONCLICKEDEVENT_H
#define CGRADIOBUTTONCLICKEDEVENT_H

#include "../CgBase/CgBaseEvent.h"

class CgRadioButtonClickedEvent : public CgBaseEvent
{
public:
    //Add more buttons if needed
    enum CgRadioButtonType {least_smoothing_biquadratic = 2, least_smoothing_bicubic = 3};

    CgRadioButtonClickedEvent();

    CgRadioButtonClickedEvent(Cg::EventType type, CgRadioButtonType radioButtonActivated);

    //inherited
    Cg::EventType  getType();
    CgBaseEvent* clone();

    //own specific methods
    CgRadioButtonType radioButton();

private:
    Cg::EventType m_type;
    CgRadioButtonType m_radioButton;

};

#endif // CGRADIOBUTTONCLICKEDEVENT_H
