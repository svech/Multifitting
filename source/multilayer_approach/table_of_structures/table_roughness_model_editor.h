#ifndef TABLE_ROUGHNESS_MODEL_EDITOR_H
#define TABLE_ROUGHNESS_MODEL_EDITOR_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/multilayer.h"

class Table_Roughness_Model_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Table_Roughness_Model_Editor(Multilayer* multilayer, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Interlayer_Groupbox();
	void create_Drift_Groupbox();
	void create_Roughness_Groupbox();
	void create_Density_Fluctuations_Groupbox();
	void refresh_Tree_Interlayer(int interlayer_Index, bool state);
	void refresh_Tree_Drift(QString whats_This, bool state);
	void refresh_Tree_Roughness();
	void refresh_Tree_Fluctuations(bool refresh_Shape = false, bool refresh_Interference_Function = false);

	Multilayer* multilayer;

	QVBoxLayout* main_Layout;
	QGroupBox* interlayer_Groupbox;
	QGroupBox* drift_Groupbox;
	QGroupBox* roughness_Groupbox;
	QGroupBox* density_Fluctuations_Groupbox;
};

#endif // TABLE_ROUGHNESS_MODEL_EDITOR_H
