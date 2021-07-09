#pragma once

#include <QtWidgets/QComboBox>

class CComBox : public QComboBox
{
	Q_OBJECT

public:
	CComBox(QWidget *parent);
	~CComBox();

	void showPopup() override;

signals:
	void sendPopup();
};
