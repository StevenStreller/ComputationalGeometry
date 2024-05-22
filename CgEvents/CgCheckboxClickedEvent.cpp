#include "CgCheckboxClickedEvent.h"

CgCheckboxClickedEvent::CgCheckboxClickedEvent(Cg::EventType type, CgCheckboxType checkboxChanged, bool checked)
{
    m_type = type;
    m_checkbox = checkboxChanged;
    this->checked = checked;
}

CgBaseEvent* CgCheckboxClickedEvent::clone()
{
    return new CgCheckboxClickedEvent(m_type, m_checkbox, checked);
}

Cg::EventType CgCheckboxClickedEvent::getType()
{
    return m_type;
}

CgCheckboxClickedEvent::CgCheckboxType CgCheckboxClickedEvent::checkbox()
{
    return m_checkbox;
}

bool CgCheckboxClickedEvent::isChecked()
{
    return checked;
}
