#ifndef MYDOUBLESPINBOX_H
#define MYDOUBLESPINBOX_H

#include "global/global_definitions.h"
#include "global/global_variables.h"

class MyDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT
public:
	explicit MyDoubleSpinBox(QWidget *parent = nullptr);

	virtual void fixup(QString &input) const override;

	virtual QValidator::State validate(QString &input, int &pos) const override;
	bool isValid(const QString& text) const;

	QLineEdit* myLineEdit() const;
	void create_Text_Change_Connection();

	void setMyAdaptiveStep(bool on);

};

#endif // MYDOUBLESPINBOX_H
