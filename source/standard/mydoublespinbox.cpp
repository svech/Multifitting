#include "mydoublespinbox.h"

MyDoubleSpinBox::MyDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
	create_Text_Change_Connection();
}

QValidator::State MyDoubleSpinBox::validate(QString &input, int &pos) const
{
	Q_UNUSED(pos);

	if(isValid(input))	{
		return QValidator::Acceptable;
	}
	return QValidator::Intermediate;
}

void MyDoubleSpinBox::fixup(QString &input) const
{
	input.remove(QRegExp("[^0-9"+QString(Locale.decimalPoint())+"]"));
	QList<QString> list = input.split(Locale.decimalPoint());
	if(list.size() > 1)
	{
		if(list[1].size() > decimals())
			list[1].remove(decimals(),list[1].size()-decimals());
		input = list[0] + Locale.decimalPoint() + list[1];
	}
}

bool MyDoubleSpinBox::isValid(const QString& text) const
{
	int idx = text.indexOf(QRegExp("[^0-9"+QString(Locale.decimalPoint())+"]"), 0);
	if(idx != -1)	{
		return false;
	}
	QList<QString> list = text.split(Locale.decimalPoint());
	if(list.size() > 2)	{
		return false;
	}
	return true;
}

QLineEdit *MyDoubleSpinBox::myLineEdit() const
{
	return lineEdit();
}

void MyDoubleSpinBox::create_Text_Change_Connection()
{
	connect(myLineEdit(), &QLineEdit::textChanged, this, [=]
	{
		QString clean_Text = text();
		clean_Text.remove(-suffix().size(),suffix().size());

		double value = valueFromText(clean_Text);
		if(isValid(clean_Text))
		{
			QList<QString> list = clean_Text.split(Locale.decimalPoint());
			if(list.size() > 1)
			{
				if(list[1].size() > decimals())
				{
					list[1].remove(decimals(),list[1].size()-decimals());
					value = valueFromText(list[0]+Locale.decimalPoint()+list[1]);
				}
			}
			setValue(value);
		} else
		{
			Global_Variables::resize_Line_Edit(this);
		}
	}, Qt::UniqueConnection);
}
