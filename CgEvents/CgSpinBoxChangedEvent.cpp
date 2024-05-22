#include "CgSpinBoxChangedEvent.h"

CgSpinBoxChangedEvent::CgSpinBoxChangedEvent(Cg::EventType type, CgSpinBoxType spinBoxChanged, int value)
{
    m_type = type;
    m_spinBox = spinBoxChanged;
    this->value = value;
}

CgBaseEvent* CgSpinBoxChangedEvent::clone()
{
    return new CgSpinBoxChangedEvent(m_type, m_spinBox, value);
}

Cg::EventType CgSpinBoxChangedEvent::getType()
{
    return m_type;
}

CgSpinBoxChangedEvent::CgSpinBoxType CgSpinBoxChangedEvent::spinBox()
{
    return m_spinBox;
}

int CgSpinBoxChangedEvent::getValue()
{
    return value;
}
