#include "CgRadioButtonClickedEvent.h"

CgRadioButtonClickedEvent::CgRadioButtonClickedEvent(Cg::EventType type, CgRadioButtonType radioButtonActivated)
{
    m_type = type;
    m_radioButton = radioButtonActivated;
}

CgBaseEvent* CgRadioButtonClickedEvent::clone()
{
    return new CgRadioButtonClickedEvent(m_type, m_radioButton);
}

Cg::EventType CgRadioButtonClickedEvent::getType()
{
    return m_type;
}

CgRadioButtonClickedEvent::CgRadioButtonType CgRadioButtonClickedEvent::radioButton()
{
    return m_radioButton;
}

