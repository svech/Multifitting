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
//	create_Density_Fluctuations_Groupbox(); // TODO

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
	QLabel* show_label = new QLabel("Show:");
	groupbox_Layout->addWidget(show_label);

	// --------------------------------------------------------------------
	QCheckBox* erf_Checkbox = new QCheckBox("Erf");
		erf_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Erf]);
	groupbox_Layout->addWidget(erf_Checkbox);
	connect(erf_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Erf] = erf_Checkbox->isChecked();
		refresh_Tree_Interlayer(Erf, multilayer->imperfections_Model.use_Func[Erf]);
	});
	// --------------------------------------------------------------------
	QCheckBox* lin_Checkbox = new QCheckBox("Lin");
		lin_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Lin]);
	groupbox_Layout->addWidget(lin_Checkbox);
	connect(lin_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Lin] = lin_Checkbox->isChecked();
		refresh_Tree_Interlayer(Lin, multilayer->imperfections_Model.use_Func[Lin]);
	});
	// --------------------------------------------------------------------
	QCheckBox* exp_Checkbox = new QCheckBox("Exp");
		exp_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Exp]);
	groupbox_Layout->addWidget(exp_Checkbox);
	connect(exp_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Exp] = exp_Checkbox->isChecked();
		refresh_Tree_Interlayer(Exp, multilayer->imperfections_Model.use_Func[Exp]);
	});
	// --------------------------------------------------------------------
	QCheckBox* tanh_Checkbox = new QCheckBox("Tanh");
		tanh_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Tanh]);
	groupbox_Layout->addWidget(tanh_Checkbox);
	connect(tanh_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Tanh] = tanh_Checkbox->isChecked();
		refresh_Tree_Interlayer(Tanh, multilayer->imperfections_Model.use_Func[Tanh]);
	});
	// --------------------------------------------------------------------
	QCheckBox* sin_Checkbox = new QCheckBox("Sin");
		sin_Checkbox->setChecked(multilayer->imperfections_Model.use_Func[Sin]);
	groupbox_Layout->addWidget(sin_Checkbox);
	connect(sin_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Func[Sin] = sin_Checkbox->isChecked();
		refresh_Tree_Interlayer(Sin, multilayer->imperfections_Model.use_Func[Sin]);
	});
	// --------------------------------------------------------------------
	QCheckBox* step_Checkbox = new QCheckBox("Step");
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
	QCheckBox* sigma_Line_Drift_Checkbox = new QCheckBox("Show s lin drift");
		sigma_Line_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Sigma_Drift_Line);
	row2_Layout->addWidget(sigma_Line_Drift_Checkbox);
	connect(sigma_Line_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Sigma_Drift_Line = sigma_Line_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Sigma_Drift_Line_Value, multilayer->imperfections_Model.show_Sigma_Drift_Line);
	});
	// --------------------------------------------------------------------
	QCheckBox* sigma_Rand_Drift_Checkbox = new QCheckBox("Show s rand drift");
		sigma_Rand_Drift_Checkbox->setChecked(multilayer->imperfections_Model.show_Sigma_Drift_Rand);
	row2_Layout->addWidget(sigma_Rand_Drift_Checkbox);
	connect(sigma_Rand_Drift_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.show_Sigma_Drift_Rand = sigma_Rand_Drift_Checkbox->isChecked();
		refresh_Tree_Drift(whats_This_Sigma_Drift_Rand_Rms, multilayer->imperfections_Model.show_Sigma_Drift_Rand);
	});
	// --------------------------------------------------------------------
	QCheckBox* sigma_Sine_Drift_Checkbox = new QCheckBox("Show s sine drift");
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
		roughness_Groupbox->setChecked(multilayer->imperfections_Model.use_Roughness);
	main_Layout->addWidget(roughness_Groupbox);
	connect(roughness_Groupbox, &QGroupBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Roughness = roughness_Groupbox->isChecked();
		refresh_Tree_Roughness(multilayer->imperfections_Model.use_Roughness);
	});

	QGridLayout* groupbox_Layout = new QGridLayout(roughness_Groupbox);
		groupbox_Layout->setAlignment(Qt::AlignTop);

	// --------------------------------------------
	// approximation
	// --------------------------------------------

		QVBoxLayout* approximation_Layout = new QVBoxLayout;
			approximation_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(approximation_Layout,0,0,1,1,Qt::AlignTop);

		QLabel* approximation_Label = new QLabel("Approximation");
		approximation_Layout->addWidget(approximation_Label);

		QRadioButton* PT_Radiobutton = new QRadioButton("PT ");
			PT_Radiobutton->setChecked(multilayer->imperfections_Model.approximation == PT_approximation);
		approximation_Layout->addWidget(PT_Radiobutton);

		QRadioButton* DWBA_Radiobutton = new QRadioButton("DWBA ");
			DWBA_Radiobutton->setChecked(multilayer->imperfections_Model.approximation == DWBA_approximation);
		approximation_Layout->addWidget(DWBA_Radiobutton);

		QRadioButton* SA_Radiobutton = new QRadioButton("SA ");
			SA_Radiobutton->setChecked(multilayer->imperfections_Model.approximation == SA_approximation);
		approximation_Layout->addWidget(SA_Radiobutton);

		QRadioButton* CSA_Radiobutton = new QRadioButton("CSA ");
			CSA_Radiobutton->setChecked(multilayer->imperfections_Model.approximation == CSA_approximation);
		approximation_Layout->addWidget(CSA_Radiobutton);

		QButtonGroup* approx_Group = new QButtonGroup;
			approx_Group->addButton(PT_Radiobutton);
			approx_Group->addButton(DWBA_Radiobutton);
			approx_Group->addButton(SA_Radiobutton);
			approx_Group->addButton(CSA_Radiobutton);

	// --------------------------------------------
	// full, partial or zero vertical correlation
	// --------------------------------------------

		QVBoxLayout* vertical_Correlation_Layout = new QVBoxLayout;
			vertical_Correlation_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(vertical_Correlation_Layout,0,1,1,1,Qt::AlignTop);

		QLabel* vertical_Correlation_Label = new QLabel("Vertical\ncorrelation");
		vertical_Correlation_Layout->addWidget(vertical_Correlation_Label);

		QRadioButton* full_Radiobutton = new QRadioButton("Full");
			full_Radiobutton->setChecked(multilayer->imperfections_Model.vertical_Correlation == full_Correlation);
		vertical_Correlation_Layout->addWidget(full_Radiobutton);

		QRadioButton* partial_Radiobutton = new QRadioButton("Partial");
			partial_Radiobutton->setChecked(multilayer->imperfections_Model.vertical_Correlation == partial_Correlation);
		vertical_Correlation_Layout->addWidget(partial_Radiobutton);

		QRadioButton* zero_Radiobutton = new QRadioButton("Zero");
			zero_Radiobutton->setChecked(multilayer->imperfections_Model.vertical_Correlation == zero_Correlation);
		vertical_Correlation_Layout->addWidget(zero_Radiobutton);

		QButtonGroup* vertical_Correlation_Group = new QButtonGroup;
			vertical_Correlation_Group->addButton(full_Radiobutton);
			vertical_Correlation_Group->addButton(partial_Radiobutton);
			vertical_Correlation_Group->addButton(zero_Radiobutton);

	// --------------------------------------------
	// PSD or Cor models
	// --------------------------------------------

		QVBoxLayout* PSD_Cor_Layout = new QVBoxLayout;
			PSD_Cor_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(PSD_Cor_Layout,0,2,1,1,Qt::AlignTop);

		QLabel* PSD_Cor_Label = new QLabel("Model");
		PSD_Cor_Layout->addWidget(PSD_Cor_Label);

		QRadioButton* PSD_ABC_Radiobutton = new QRadioButton("ABC model, PSD function");
			PSD_ABC_Radiobutton->setChecked(multilayer->imperfections_Model.common_Model == ABC_model_PSD);
			PSD_ABC_Radiobutton->setDisabled(DWBA_Radiobutton->isChecked() || SA_Radiobutton->isChecked() || CSA_Radiobutton->isChecked());
		PSD_Cor_Layout->addWidget(PSD_ABC_Radiobutton);

		QRadioButton* PSD_Growth_Radiobutton = new QRadioButton("ABC+linear growth, PSD function");
			PSD_Growth_Radiobutton->setChecked(multilayer->imperfections_Model.common_Model == linear_model_PSD);
			PSD_Growth_Radiobutton->setDisabled(DWBA_Radiobutton->isChecked() ||
												SA_Radiobutton->isChecked() ||
												CSA_Radiobutton->isChecked() ||
												multilayer->imperfections_Model.vertical_Correlation != partial_Correlation);
		PSD_Cor_Layout->addWidget(PSD_Growth_Radiobutton);

		QRadioButton* Cor_ABC_Radiobutton = new QRadioButton("ABC model, correlation function");
			Cor_ABC_Radiobutton->setChecked(multilayer->imperfections_Model.common_Model == ABC_model_Cor);
			Cor_ABC_Radiobutton->setDisabled(DWBA_Radiobutton->isChecked() || SA_Radiobutton->isChecked() || CSA_Radiobutton->isChecked());
		PSD_Cor_Layout->addWidget(Cor_ABC_Radiobutton);

		QRadioButton* Cor_Exp_Radiobutton = new QRadioButton("Exponential model, correlation function");
			Cor_Exp_Radiobutton->setChecked(multilayer->imperfections_Model.common_Model == gauss_model_Cor);
			Cor_Exp_Radiobutton->setDisabled(DWBA_Radiobutton->isChecked() || SA_Radiobutton->isChecked() || CSA_Radiobutton->isChecked());
		PSD_Cor_Layout->addWidget(Cor_Exp_Radiobutton);

		QButtonGroup* use_Group = new QButtonGroup;
			use_Group->addButton(PSD_ABC_Radiobutton);
			use_Group->addButton(PSD_Growth_Radiobutton);
			use_Group->addButton(Cor_ABC_Radiobutton);
			use_Group->addButton(Cor_Exp_Radiobutton);

	// --------------------------------------------
	// common statistics
	// --------------------------------------------
		QCheckBox* common_Checkbox = new QCheckBox("Common function for all layer boundaries");
			common_Checkbox->setChecked(multilayer->imperfections_Model.use_Common_Roughness_Function);
			common_Checkbox->setDisabled(full_Radiobutton->isChecked() || partial_Radiobutton->isChecked());
		groupbox_Layout->addWidget(common_Checkbox,1,0,1,3);

	// connections
	connect(PT_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(PT_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = PT_approximation;
			PSD_ABC_Radiobutton->setDisabled(false);
			if(partial_Radiobutton->isChecked()) PSD_Growth_Radiobutton->setDisabled(false);
			Cor_ABC_Radiobutton->setDisabled(false);
			Cor_Exp_Radiobutton->setDisabled(false);
		}
	});
	connect(DWBA_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(DWBA_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = DWBA_approximation;
			PSD_ABC_Radiobutton->setDisabled(true);
			PSD_Growth_Radiobutton->setDisabled(true);

			if( !Cor_ABC_Radiobutton->isChecked() &&
				!Cor_Exp_Radiobutton->isChecked() )
			{
				Cor_ABC_Radiobutton->setChecked(true);
				Cor_ABC_Radiobutton->toggled(true);
			}
		}
	});
	connect(SA_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(SA_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = SA_approximation;
			PSD_ABC_Radiobutton->setDisabled(true);
			PSD_Growth_Radiobutton->setDisabled(true);

			if( !Cor_ABC_Radiobutton->isChecked() &&
				!Cor_Exp_Radiobutton->isChecked() )
			{
				Cor_ABC_Radiobutton->setChecked(true);
				Cor_ABC_Radiobutton->toggled(true);
			}
		}
	});
	connect(CSA_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(CSA_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = CSA_approximation;
			PSD_ABC_Radiobutton->setDisabled(true);
			PSD_Growth_Radiobutton->setDisabled(true);

			if( !Cor_ABC_Radiobutton->isChecked() &&
				!Cor_Exp_Radiobutton->isChecked() )
			{
				Cor_ABC_Radiobutton->setChecked(true);
				Cor_ABC_Radiobutton->toggled(true);
			}
		}
	});


	connect(full_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(full_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.vertical_Correlation = full_Correlation;

			if(PSD_Growth_Radiobutton->isChecked())
			{
				PSD_Growth_Radiobutton->setChecked(false);
				PSD_ABC_Radiobutton->setChecked(true);
				PSD_ABC_Radiobutton->toggled(true);
			}
			PSD_Growth_Radiobutton->setDisabled(true);

			common_Checkbox->setChecked(true);
			common_Checkbox->toggled(true);
			common_Checkbox->setDisabled(true);
		}
	});
	connect(partial_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(partial_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.vertical_Correlation = partial_Correlation;
			if(PT_Radiobutton->isChecked()) PSD_Growth_Radiobutton->setDisabled(false);

			common_Checkbox->setChecked(true);
			common_Checkbox->toggled(true);
			common_Checkbox->setDisabled(true);
		}
	});
	connect(zero_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(zero_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.vertical_Correlation = zero_Correlation;

			if(PSD_Growth_Radiobutton->isChecked())
			{
				PSD_Growth_Radiobutton->setChecked(false);
				PSD_ABC_Radiobutton->setChecked(true);
				PSD_ABC_Radiobutton->toggled(true);
			}
			PSD_Growth_Radiobutton->setDisabled(true);

			common_Checkbox->setDisabled(false);
		}
	});


	connect(PSD_ABC_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(PSD_ABC_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.common_Model = ABC_model_PSD;
			refresh_Tree_Roughness_Model();
		}
	});
	connect(PSD_Growth_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(PSD_Growth_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.common_Model = linear_model_PSD;
			refresh_Tree_Roughness_Model();
		}
	});
	connect(Cor_ABC_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(Cor_ABC_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.common_Model = ABC_model_Cor;
			refresh_Tree_Roughness_Model();
		}
	});
	connect(Cor_Exp_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(Cor_Exp_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.common_Model = gauss_model_Cor;
			refresh_Tree_Roughness_Model();
		}
	});


	connect(common_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Common_Roughness_Function = common_Checkbox->isChecked();
	});
}

void Table_Roughness_Model_Editor::create_Density_Fluctuations_Groupbox()
{
	density_Fluctuations_Groupbox = new QGroupBox("Use density fluctuations");
		density_Fluctuations_Groupbox->setCheckable(true);
		density_Fluctuations_Groupbox->setChecked(multilayer->imperfections_Model.use_Fluctuations);
	main_Layout->addWidget(density_Fluctuations_Groupbox);
	connect(density_Fluctuations_Groupbox, &QGroupBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Fluctuations = density_Fluctuations_Groupbox->isChecked();
	});
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
			if(!struct_Data.common_Sigma_Diffuse)
			{
				struct_Data.sigma_Diffuse.value = Table_Of_Structures::recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
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

			if(whats_This == whats_This_Sigma_Drift_Line_Value)	struct_Data.sigma_Diffuse_Drift.is_Drift_Line = state;
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )	struct_Data.sigma_Diffuse_Drift.is_Drift_Rand = state;
			if(whats_This == whats_This_Sigma_Drift_Sine	  )	struct_Data.sigma_Diffuse_Drift.is_Drift_Sine = state;

			QVariant var;
			var.setValue( struct_Data );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
		++it;
	}
}

void Table_Roughness_Model_Editor::refresh_Tree_Roughness(bool state)
{
	// enable and disable
	QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;
		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if( struct_Data.item_Type == item_Type_Layer ||
			struct_Data.item_Type == item_Type_Substrate )
		{
			struct_Data.roughness_Model.is_Enabled = state;

			QVariant var;
			var.setValue( struct_Data );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
		++it;
	}
}

void Table_Roughness_Model_Editor::refresh_Tree_Roughness_Model()
{
	QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;
		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if( struct_Data.item_Type == item_Type_Layer ||
			struct_Data.item_Type == item_Type_Substrate )
		{
			struct_Data.roughness_Model.model = multilayer->imperfections_Model.common_Model;

			QVariant var;
			var.setValue( struct_Data );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
		++it;
	}
}
