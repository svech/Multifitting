#ifndef FITS_SELECTOR_H
#define FITS_SELECTOR_H

#include "global/settings.h"
#include "multilayer_approach.h"

class Fits_Selector : public QWidget
{
	Q_OBJECT
public:
	explicit Fits_Selector(Multilayer_Approach* multilayer_Approach, QWidget* parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void create_List();
	void load_Data();
	void add_Item(Fitted_Structure& fitted_Structure);

	Multilayer_Approach* multilayer_Approach;
	QMap<QString, Fits_Selector*>& runned_Fits_Selectors;

	QVBoxLayout* main_Layout;

		QMap<QListWidgetItem*, Fitted_Structure> items_To_Structures_Map;
		QListWidget* fits_List;
		QVector<Fitted_Structure>& fitted_Structures;

		QPushButton* done_Button;

};

#endif // FITS_SELECTOR_H
