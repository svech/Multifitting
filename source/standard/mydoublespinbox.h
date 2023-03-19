#ifndef MYDOUBLESPINBOX_H
#define MYDOUBLESPINBOX_H

#include "global/global_definitions.h"
#include "global/global_variables.h"

class MyDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT
public:
	explicit MyDoubleSpinBox(QWidget *parent = nullptr, bool auto_Resize = true);

	virtual void fixup(QString &input) const override;

	virtual QValidator::State validate(QString &input, int &pos) const override;
	bool isValid(const QString& text) const;

	QLineEdit* myLineEdit() const;
	bool auto_Resize;
	void create_Text_Change_Connection();

	void setMyAdaptiveStep(bool on);

    QRegExp positive_Symbols;
    QRegExp negative_Symbols;
};

#endif // MYDOUBLESPINBOX_H
