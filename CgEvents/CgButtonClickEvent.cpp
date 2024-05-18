#include "CgButtonClickEvent.h"

CgButtonClickEvent::CgButtonClickEvent() {

}

CgButtonClickEvent::CgButtonClickEvent(Cg::EventType type, CgButtonType buttonClicked) {
    m_type = type;
    m_button = buttonClicked;
}

CgBaseEvent *CgButtonClickEvent::clone() {
    return new CgButtonClickEvent(m_type, m_button);
}

Cg::EventType CgButtonClickEvent::getType() {
    return m_type;
}

CgButtonClickEvent::CgButtonType CgButtonClickEvent::button() const {
    return m_button;
}
