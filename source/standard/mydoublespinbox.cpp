#include "mydoublespinbox.h"

const QRegExp positive_Symbols = QRegExp("[^0-9"+QString(Locale.decimalPoint())+"]");
const QRegExp negative_Symbols = QRegExp("[^0-9"+QString(Locale.decimalPoint())+"\\-]");

MyDoubleSpinBox::MyDoubleSpinBox(QWidget *parent, bool auto_Resize):
	QDoubleSpinBox(parent),
	auto_Resize(auto_Resize)
{
	create_Text_Change_Connection();
}

QValidator::State MyDoubleSpinBox::validate(QString &input, int &pos) const
{
	Q_UNUSED(pos);

	bool true_Size = true;
	QList<QString> list = input.split(Locale.decimalPoint());
	if(list.size() > 1)
	{
		if(list[1].size() > decimals()) true_Size = false;
	}

	if(true_Size)
	{
		if(isValid(input))
		{
			return QValidator::Acceptable;
		}
	}
	return QValidator::Intermediate;
}

void MyDoubleSpinBox::fixup(QString &input) const
{
	QRegExp symbols;
	if(minimum()<0)
	{symbols = negative_Symbols;}
	else
	{symbols = positive_Symbols;}

	input.remove(symbols);
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
	QRegExp symbols;
	if(minimum()<0)
	{symbols = negative_Symbols;}
	else
	{symbols = positive_Symbols;}

	int idx = text.indexOf(symbols, 0);
	if(idx != -1 || text.isEmpty() || text=="-" || text=="." || text=="-." || text==".-")	{
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

		// remove prefix and suffix
		clean_Text.remove(-suffix().size(),suffix().size());
		clean_Text.remove(0,prefix().size());

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

					setValue(value);
				} else
				{
					blockSignals(true);
					setValue(value);
					blockSignals(false);
				}
			} else
			{
				blockSignals(true);
				setValue(value);
				blockSignals(false);
			}
		}
		if(auto_Resize)	{Global_Variables::resize_Line_Edit(this);}

	}, Qt::UniqueConnection);
	connect(this, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		if(auto_Resize)	{Global_Variables::resize_Line_Edit(this);}
	}, Qt::UniqueConnection);
	connect(myLineEdit(), &QLineEdit::editingFinished, this, [=]
	{
		if(auto_Resize) {Global_Variables::resize_Line_Edit(this);}
	}, Qt::UniqueConnection);
}
