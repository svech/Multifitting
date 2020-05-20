#include "table_roughness_model_editor.h"

Table_Roughness_Model_Editor::Table_Roughness_Model_Editor(Multilayer* multilayer, QWidget *parent) :
	multilayer(multilayer),
	QWidget(parent)
{
	setWindowTitle("Set imperfections model");
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Window);

	create_Main_Layout();
}

void Table_Roughness_Model_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSizeConstraint(QLayout::SetFixedSize);
		main_Layout->setSpacing(15);
//		main_Layout->setContentsMargins(0,0,0,0);

	create_Interlayer_Groupbox();
	create_Drift_Groupbox();
	create_Roughness_Groupbox();
//	create_Density_Fluctuations_Groupbox();

	QHBoxLayout* buttons_Layout = new QHBoxLayout;
	main_Layout->addLayout(buttons_Layout);

	QPushButton* ok_Button = new QPushButton("Close");
		ok_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		ok_Button->setFocus();
		ok_Button->setDefault(true);
	buttons_Layout->addWidget(ok_Button,0,Qt::AlignCenter);
	connect(ok_Button, &QPushButton::clicked, this, [=]
	{
		close();
		global_Multilayer_Approach->reopen_Table_Of_Structures(true);
	});
}

void Table_Roughness_Model_Editor::create_Interlayer_Groupbox()
{
	interlayer_Groupbox = new QGroupBox("Use transitional layer");
		interlayer_Groupbox->setCheckable(true);
		interlayer_Groupbox->setChecked(multilayer->imperfections_Model.use_Interlayer);
	main_Layout->addWidget(interlayer_Groupbox);
	connect(interlayer_Groupbox, &QGroupBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Interlayer = interlayer_Groupbox->isChecked();

		refresh_Tree_Interlayer(Erf, multilayer->imperfections_Model.use_Func[Erf]  && multilayer->imperfections_Model.use_Interlayer);
		refresh_Tree_Interlayer(Lin, multilayer->imperfections_Model.use_Func[Lin]  && multilayer->imperfections_Model.use_Interlayer);
		refresh_Tree_Interlayer(Exp, multilayer->imperfections_Model.use_Func[Exp]  && multilayer->imperfections_Model.use_Interlayer);
		refresh_Tree_Interlayer(Tanh,multilayer->imperfections_Model.use_Func[Tanh] && multilayer->imperfections_Model.use_Interlayer);
		refresh_Tree_Interlayer(Sin, multilayer->imperfections_Model.use_Func[Sin]  && multilayer->imperfections_Model.use_Interlayer);
		refresh_Tree_Interlayer(Step,multilayer->imperfections_Model.use_Func[Step] && multilayer->imperfections_Model.use_Interlayer);
	});

	QHBoxLayout* groupbox_Layout = new QHBoxLayout(interlayer_Groupbox);

	// --------------------------------------------------------------------
	QCheckBox* erf_Checkbox = new QCheckBox("Show Erf");
		erf_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Erf]);
	groupbox_Layout->addWidget(erf_Checkbox);
	connect(erf_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Erf] = erf_Checkbox->isChecked();
		refresh_Tree_Interlayer(Erf, multilayer->imperfections_Model.use_Func[Erf]);
	});
	// --------------------------------------------------------------------
	QCheckBox* lin_Checkbox = new QCheckBox("Show Lin");
		lin_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Lin]);
	groupbox_Layout->addWidget(lin_Checkbox);
	connect(lin_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Lin] = lin_Checkbox->isChecked();
		refresh_Tree_Interlayer(Lin, multilayer->imperfections_Model.use_Func[Lin]);
	});
	// --------------------------------------------------------------------
	QCheckBox* exp_Checkbox = new QCheckBox("Show Exp");
		exp_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Exp]);
	groupbox_Layout->addWidget(exp_Checkbox);
	connect(exp_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Exp] = exp_Checkbox->isChecked();
		refresh_Tree_Interlayer(Exp, multilayer->imperfections_Model.use_Func[Exp]);
	});
	// --------------------------------------------------------------------
	QCheckBox* tanh_Checkbox = new QCheckBox("Show Tanh");
		tanh_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Tanh]);
	groupbox_Layout->addWidget(tanh_Checkbox);
	connect(tanh_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Tanh] = tanh_Checkbox->isChecked();
		refresh_Tree_Interlayer(Tanh, multilayer->imperfections_Model.use_Func[Tanh]);
	});
	// --------------------------------------------------------------------
	QCheckBox* sin_Checkbox = new QCheckBox("Show Sin");
		sin_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Sin]);
	groupbox_Layout->addWidget(sin_Checkbox);
	connect(sin_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Sin] = sin_Checkbox->isChecked();
		refresh_Tree_Interlayer(Sin, multilayer->imperfections_Model.use_Func[Sin]);
	});
	// --------------------------------------------------------------------
	QCheckBox* step_Checkbox = new QCheckBox("Show Step");
		step_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Step]);
	groupbox_Layout->addWidget(step_Checkbox);
	connect(step_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Step] = step_Checkbox->isChecked();
		refresh_Tree_Interlayer(Step, multilayer->imperfections_Model.use_Func[Step]);
	});
}

void Table_Roughness_Model_Editor::create_Drift_Groupbox()
{
	drift_Groupbox = new QGroupBox("Use drifts");
		drift_Groupbox->setCheckable(true);
		drift_Groupbox->setChecked(multilayer->imperfections_Model.show_Drift);
	main_Layout->addWidget(drift_Groupbox);
	connect(drift_Groupbox, &QGroupBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Drift = drift_Groupbox->isChecked();

		refresh_Tree_Drift(whats_This_Thickness_Drift_Line_Value,multilayer->imperfections_Model.show_Thickness_Drift_Line  && multilayer->imperfections_Model.show_Drift);
		refresh_Tree_Drift(whats_This_Thickness_Drift_Rand_Rms,  multilayer->imperfections_Model.show_Thickness_Drift_Rand  && multilayer->imperfections_Model.show_Drift);
		refresh_Tree_Drift(whats_This_Thickness_Drift_Sine,		 multilayer->imperfections_Model.show_Thickness_Drift_Sine  && multilayer->imperfections_Model.show_Drift);
		refresh_Tree_Drift(whats_This_Sigma_Drift_Line_Value,	 multilayer->imperfections_Model.show_Sigma_Drift_Line		&& multilayer->imperfections_Model.show_Drift);
		refresh_Tree_Drift(whats_This_Sigma_Drift_Rand_Rms,		 multilayer->imperfections_Model.show_Sigma_Drift_Rand		&& multilayer->imperfections_Model.show_Drift);
		refresh_Tree_Drift(whats_This_Sigma_Drift_Sine,			 multilayer->imperfections_Model.show_Sigma_Drift_Sine		&& multilayer->imperfections_Model.show_Drift);
	});

	QVBoxLayout* groupbox_Layout = new QVBoxLayout(drift_Groupbox);
	QHBoxLayout* row1_Layout = new QHBoxLayout;
		groupbox_Layout->addLayout(row1_Layout);
	QHBoxLayout* row2_Layout = new QHBoxLayout;
		groupbox_Layout->addLayout(row2_Layout);

	// --------------------------------------------------------------------
	QCheckBox* thickness_Line_Drift_Checkbox = new QCheckBox("Show z lin drift");
		thickness_Line_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Thickness_Drift_Line);
	row1_Layout->addWidget(thickness_Line_Drift_Checkbox);
	connect(thickness_Line_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Thickness_Drift_Line = thickness_Line_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Thickness_Drift_Line_Value, multilayer->imperfections_Model.show_Thickness_Drift_Line);
	});
	// --------------------------------------------------------------------
	QCheckBox* thickness_Rand_Drift_Checkbox = new QCheckBox("Show z rand drift");
		thickness_Rand_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Thickness_Drift_Rand);
	row1_Layout->addWidget(thickness_Rand_Drift_Checkbox);
	connect(thickness_Rand_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Thickness_Drift_Rand = thickness_Rand_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Thickness_Drift_Rand_Rms, multilayer->imperfections_Model.show_Thickness_Drift_Rand);
	});
	// --------------------------------------------------------------------
	QCheckBox* thickness_Sine_Drift_Checkbox = new QCheckBox("Show z sine drift");
		thickness_Sine_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Thickness_Drift_Sine);
	row1_Layout->addWidget(thickness_Sine_Drift_Checkbox);
	connect(thickness_Sine_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Thickness_Drift_Sine = thickness_Sine_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Thickness_Drift_Sine, multilayer->imperfections_Model.show_Thickness_Drift_Sine);
	});
	// --------------------------------------------------------------------
	QCheckBox* sigma_Line_Drift_Checkbox = new QCheckBox("Show "+Sigma_Sym+" lin drift");
		sigma_Line_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Sigma_Drift_Line);
	row2_Layout->addWidget(sigma_Line_Drift_Checkbox);
	connect(sigma_Line_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Sigma_Drift_Line = sigma_Line_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Sigma_Drift_Line_Value, multilayer->imperfections_Model.show_Sigma_Drift_Line);
	});
	// --------------------------------------------------------------------
	QCheckBox* sigma_Rand_Drift_Checkbox = new QCheckBox("Show "+Sigma_Sym+" rand drift");
		sigma_Rand_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Sigma_Drift_Rand);
	row2_Layout->addWidget(sigma_Rand_Drift_Checkbox);
	connect(sigma_Rand_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Sigma_Drift_Rand = sigma_Rand_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Sigma_Drift_Rand_Rms, multilayer->imperfections_Model.show_Sigma_Drift_Rand);
	});
	// --------------------------------------------------------------------
	QCheckBox* sigma_Sine_Drift_Checkbox = new QCheckBox("Show "+Sigma_Sym+" sine drift");
		sigma_Sine_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Sigma_Drift_Sine);
	row2_Layout->addWidget(sigma_Sine_Drift_Checkbox);
	connect(sigma_Sine_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Sigma_Drift_Sine = sigma_Sine_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Sigma_Drift_Sine, multilayer->imperfections_Model.show_Sigma_Drift_Sine);
	});
}

void Table_Roughness_Model_Editor::create_Roughness_Groupbox()
{
	roughness_Groupbox = new QGroupBox("Use roughness");
		roughness_Groupbox->setCheckable(true);
	main_Layout->addWidget(roughness_Groupbox);
}

void Table_Roughness_Model_Editor::create_Density_Fluctuations_Groupbox()
{
	density_Fluctuations_Groupbox = new QGroupBox("Use density fluctuations");
		density_Fluctuations_Groupbox->setCheckable(true);
	main_Layout->addWidget(density_Fluctuations_Groupbox);
}

void Table_Roughness_Model_Editor::refresh_Tree_Interlayer(int interlayer_Index, bool state)
{
	if(state == true) return; // only disable, not enable

	QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;
		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		if( struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		if( struct_Data.item_Type == item_Type_Layer ||
			struct_Data.item_Type == item_Type_Substrate)
		{
			struct_Data.interlayer_Composition[interlayer_Index].enabled = state;
			if(!struct_Data.common_Sigma)
			{
				struct_Data.sigma.value = Table_Of_Structures::recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
			}

			QVariant var;
			var.setValue( struct_Data );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
		++it;
	}
}

void Table_Roughness_Model_Editor::refresh_Tree_Drift(QString whats_This, bool state)
{
	if(state == true) return; // only disable, not enable

	QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;
		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if( struct_Data.item_Type == item_Type_Layer ||
			struct_Data.item_Type == item_Type_Substrate )
		{
			if(whats_This == whats_This_Thickness_Drift_Line_Value)	struct_Data.thickness_Drift.is_Drift_Line = state;
			if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	struct_Data.thickness_Drift.is_Drift_Rand = state;
			if(whats_This == whats_This_Thickness_Drift_Sine      )	struct_Data.thickness_Drift.is_Drift_Sine = state;

			if(whats_This == whats_This_Sigma_Drift_Line_Value)	struct_Data.sigma_Drift.is_Drift_Line = state;
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )	struct_Data.sigma_Drift.is_Drift_Rand = state;
			if(whats_This == whats_This_Sigma_Drift_Sine	  )	struct_Data.sigma_Drift.is_Drift_Sine = state;

			QVariant var;
			var.setValue( struct_Data );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
		++it;
	}
}
