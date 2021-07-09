#include "ccombox.h"

CComBox::CComBox(QWidget *parent)
	: QComboBox(parent)
{
}

CComBox::~CComBox()
{
}

void CComBox::showPopup()
{
	emit sendPopup();
	QComboBox::showPopup();
}