#include "table_roughness_model_editor.h"

Table_Roughness_Model_Editor::Table_Roughness_Model_Editor(Multilayer* multilayer, QWidget *parent) :
	multilayer(multilayer),
	QWidget(parent)
{
	setWindowTitle("Set imperfections model");
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint);

	create_Main_Layout();
}

void Table_Roughness_Model_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSizeConstraint(QLayout::SetFixedSize);
		main_Layout->setSpacing(15);
//		main_Layout->setContentsMargins(0,0,0,0);

	QString old_Common_Particle_Shape = multilayer->imperfections_Model.initial_Particle_Shape;
	QString old_Common_Interference_Function = multilayer->imperfections_Model.initial_Interference_Function;
	QString old_Common_Geometric_Model = multilayer->imperfections_Model.initial_Geometric_Model;

	create_Interlayer_Groupbox();
	create_Drift_Groupbox();
	create_Roughness_Groupbox();
	create_Particles_Groupbox();

	QHBoxLayout* buttons_Layout = new QHBoxLayout;
	main_Layout->addLayout(buttons_Layout);

	QPushButton* ok_Button = new QPushButton("Close");
		ok_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		ok_Button->setFocus();
		ok_Button->setDefault(true);
	buttons_Layout->addWidget(ok_Button,0,Qt::AlignCenter);
	connect(ok_Button, &QPushButton::clicked, this, [=]
	{
		multilayer->structure_Tree->refresh_Tree_Roughness();
		multilayer->structure_Tree->refresh_Tree_Particles(old_Common_Particle_Shape        != multilayer->imperfections_Model.initial_Particle_Shape,
														   old_Common_Interference_Function != multilayer->imperfections_Model.initial_Interference_Function,
														   old_Common_Geometric_Model       != multilayer->imperfections_Model.initial_Geometric_Model);

		// common
		bool have_Scattering = false;
		bool have_GISAS = false;
		for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
		{
			if((target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] ) && target_Curve->fit_Params.calculate)
			{
				have_Scattering = true;
			}
			if(target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] && target_Curve->fit_Params.calculate)
			{
				have_GISAS = true;
			}
		}
		for(int independent_Index=0; independent_Index<multilayer->independent_Curve_Tabs->count(); ++independent_Index)
		{
			Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));
			if((independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] ) && independent_Curve->calc_Functions.check_Scattering)
			{
				have_Scattering = true;
			}
			if(independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] && independent_Curve->calc_Functions.check_GISAS)
			{
				have_GISAS = true;
			}
		}

		// cases
		bool GISAS_DWBA_SA_CSA = false;
		bool discretization_DWBA_SA_CSA = false;

		if( have_GISAS &&
           (//multilayer->imperfections_Model.PSD_Model == stretched_exp_Model ||
			multilayer->imperfections_Model.approximation == DWBA_approximation ||
			multilayer->imperfections_Model.approximation == SA_approximation   ||
			multilayer->imperfections_Model.approximation == CSA_approximation  ))
		{
			GISAS_DWBA_SA_CSA = true;
		}
		if( have_Scattering && multilayer->discretization_Parameters.enable_Discretization &&
		   (multilayer->imperfections_Model.approximation == DWBA_approximation ||
			multilayer->imperfections_Model.approximation == SA_approximation   ||
			multilayer->imperfections_Model.approximation == CSA_approximation  ))
		{
			discretization_DWBA_SA_CSA = true;
		}

		if(GISAS_DWBA_SA_CSA || discretization_DWBA_SA_CSA)
		{
			if(GISAS_DWBA_SA_CSA)
			{
				QString text;
//				if( multilayer->imperfections_Model.PSD_Model == stretched_exp_Model ) text = "Stretched exp model";
				if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "DWBA approximation";
				if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "SA approximation";
				if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "CSA approximation";

				QMessageBox::StandardButton reply = QMessageBox::question(nullptr,"GISAS simulation", text+" can't be used with 2D scattering.\nGISAS map will be disabled.\nContinue?",
																		  QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
				activateWindow();
				if (reply == QMessageBox::Yes)
				{
					for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
					{
						if(target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
						{
							target_Curve->fit_Params.calculate = false;
						}
					}
					for(int independent_Index=0; independent_Index<multilayer->independent_Curve_Tabs->count(); ++independent_Index)
					{
						Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));
						if(independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
						{
							independent_Curve->calc_Functions.check_Enabled = false;
						}
					}

					GISAS_DWBA_SA_CSA = false; // no more
					global_Multilayer_Approach->reopen_Optical_Graphs_2D(true);
					global_Multilayer_Approach->reopen_Calculation_Settings(true);
					global_Multilayer_Approach->reopen_Roughness_Plots();
					global_Multilayer_Approach->reopen_Particles_Plots();
				}
			}
			if(discretization_DWBA_SA_CSA)
			{
				QString text;
				if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "DWBA approximation.";
				if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "SA approximation.";
				if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "CSA approximation.";

				QMessageBox::StandardButton reply = QMessageBox::question(nullptr,"Discretization", "Discretization can't be used with "+text+"\nDiscretization will be disabled.\nContinue?",
																		  QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
				activateWindow();
				if (reply == QMessageBox::Yes)
				{
					// disable discretization
					multilayer->discretization_Parameters.enable_Discretization = false;
					// disable scattering
//					for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
//					{
//						if( target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
//							target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
//							target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
//						{
//							target_Curve->fit_Params.calculate = false;
//						}
//					}
//					for(int independent_Index=0; independent_Index<multilayer->independent_Curve_Tabs->count(); ++independent_Index)
//					{
//						Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));
//						if( independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
//							independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
//							independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan])
//						{
//							independent_Curve->calc_Functions.check_Scattering = false;
//						}
//					}
					discretization_DWBA_SA_CSA = false; // no more
//					global_Multilayer_Approach->reopen_Optical_Graphs_1D(true);
					global_Multilayer_Approach->reopen_Calculation_Settings(true);
					global_Multilayer_Approach->reopen_Roughness_Plots();
					global_Multilayer_Approach->reopen_Particles_Plots();
				}
			}
			if(!GISAS_DWBA_SA_CSA && !discretization_DWBA_SA_CSA)
			{
				if( multilayer->imperfections_Model.approximation == DWBA_approximation ||
					multilayer->imperfections_Model.approximation == SA_approximation   ||
					multilayer->imperfections_Model.approximation == CSA_approximation  )
				{
					QMessageBox::information(this,"Interlayer will not be used", "In DWBA, SA, CSA approximations\ndiffuse interlayers are neglected");
				}
				close();
				global_Multilayer_Approach->reopen_Table_Of_Structures(true);
				global_Multilayer_Approach->reopen_Calculation_Settings(true);
				global_Multilayer_Approach->reopen_Roughness_Plots();
				global_Multilayer_Approach->reopen_Particles_Plots();
			}
		} else
		{
			if( multilayer->imperfections_Model.approximation == DWBA_approximation ||
				multilayer->imperfections_Model.approximation == SA_approximation   ||
				multilayer->imperfections_Model.approximation == CSA_approximation  )
			{
				QMessageBox::information(this,"Interlayer will not be used", "In DWBA, SA, CSA approximations\ndiffuse interlayers are neglected");
			}
			close();
			global_Multilayer_Approach->reopen_Table_Of_Structures(true);
			global_Multilayer_Approach->reopen_Calculation_Settings(true);
			global_Multilayer_Approach->reopen_Roughness_Plots();
			global_Multilayer_Approach->reopen_Particles_Plots();
		}
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
	});

	QHBoxLayout* groupbox_Layout = new QHBoxLayout(roughness_Groupbox);

	// --------------------------------------------
	// approximation
	// --------------------------------------------

		QVBoxLayout* approximation_Layout = new QVBoxLayout;
			approximation_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(approximation_Layout);

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
		groupbox_Layout->addLayout(vertical_Correlation_Layout);

		QLabel* vertical_Correlation_Label = new QLabel("Vertical\ncorrelation");
		vertical_Correlation_Layout->addWidget(vertical_Correlation_Label);

		QRadioButton* full_Radiobutton = new QRadioButton("Full");
			full_Radiobutton->setChecked(multilayer->imperfections_Model.vertical_Correlation == full_Correlation);
		vertical_Correlation_Layout->addWidget(full_Radiobutton);

		QRadioButton* partial_Radiobutton = new QRadioButton("Partial");
			partial_Radiobutton->setChecked(multilayer->imperfections_Model.vertical_Correlation == partial_Correlation);
			partial_Radiobutton->setEnabled(multilayer->imperfections_Model.approximation == PT_approximation);
		vertical_Correlation_Layout->addWidget(partial_Radiobutton);

		QRadioButton* zero_Radiobutton = new QRadioButton("Zero");
			zero_Radiobutton->setChecked(multilayer->imperfections_Model.vertical_Correlation == zero_Correlation);
		vertical_Correlation_Layout->addWidget(zero_Radiobutton);

		QButtonGroup* vertical_Correlation_Group = new QButtonGroup;
			vertical_Correlation_Group->addButton(full_Radiobutton);
			vertical_Correlation_Group->addButton(partial_Radiobutton);
			vertical_Correlation_Group->addButton(zero_Radiobutton);

	// --------------------------------------------
	// PSD models
	// --------------------------------------------

		QVBoxLayout* PSD_Model_Layout = new QVBoxLayout;
			PSD_Model_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(PSD_Model_Layout);

		QLabel* PSD_Model_Label = new QLabel("Model");
		PSD_Model_Layout->addWidget(PSD_Model_Label);

		QRadioButton* ABC_Radiobutton = new QRadioButton("ABC");
			ABC_Radiobutton->setChecked(multilayer->imperfections_Model.PSD_Model == ABC_Model);
		PSD_Model_Layout->addWidget(ABC_Radiobutton);

        QRadioButton* fractal_Gauss_Radiobutton = new QRadioButton("Stretched exp");
            fractal_Gauss_Radiobutton->setChecked(multilayer->imperfections_Model.PSD_Model == stretched_exp_Model);
		PSD_Model_Layout->addWidget(fractal_Gauss_Radiobutton);

		QButtonGroup* use_Model_Group = new QButtonGroup;
			use_Model_Group->addButton(ABC_Radiobutton);
			use_Model_Group->addButton(fractal_Gauss_Radiobutton);


		QCheckBox* measured_PSD_1D_Checkbox = new QCheckBox("External PSD 1D");
			measured_PSD_1D_Checkbox->setChecked (multilayer->imperfections_Model.add_Measured_PSD_1D);
			measured_PSD_1D_Checkbox->setDisabled(multilayer->imperfections_Model.vertical_Correlation == partial_Correlation ||
												  multilayer->imperfections_Model.approximation == DWBA_approximation ||
												  multilayer->imperfections_Model.approximation == SA_approximation ||
												  multilayer->imperfections_Model.approximation == CSA_approximation);
		PSD_Model_Layout->addWidget(measured_PSD_1D_Checkbox);

		QCheckBox* measured_PSD_2D_Checkbox = new QCheckBox("External PSD 2D");
			measured_PSD_2D_Checkbox->setChecked (multilayer->imperfections_Model.add_Measured_PSD_2D);
			measured_PSD_2D_Checkbox->setDisabled(multilayer->imperfections_Model.approximation == DWBA_approximation ||
												  multilayer->imperfections_Model.approximation == SA_approximation ||
												  multilayer->imperfections_Model.approximation == CSA_approximation);
		PSD_Model_Layout->addWidget(measured_PSD_2D_Checkbox);

		// gauss peak
		QCheckBox* gauss_Peak_Checkbox = new QCheckBox("Add Gauss peak");
			gauss_Peak_Checkbox->setChecked (multilayer->imperfections_Model.add_Gauss_Peak);
			gauss_Peak_Checkbox->setDisabled(multilayer->imperfections_Model.approximation == DWBA_approximation ||
											 multilayer->imperfections_Model.approximation == SA_approximation ||
											 multilayer->imperfections_Model.approximation == CSA_approximation);
		PSD_Model_Layout->addWidget(gauss_Peak_Checkbox);

		// common PSD
		QCheckBox* common_Checkbox = new QCheckBox("Common PSD");
			common_Checkbox->setChecked (multilayer->imperfections_Model.use_Common_Roughness_Function);
			common_Checkbox->setDisabled(multilayer->imperfections_Model.vertical_Correlation == full_Correlation ||

										 ( multilayer->imperfections_Model.vertical_Correlation == zero_Correlation &&
										  (multilayer->imperfections_Model.add_Measured_PSD_1D ||
										   multilayer->imperfections_Model.add_Measured_PSD_2D) )
										 );
		PSD_Model_Layout->addWidget(common_Checkbox);

	// --------------------------------------------
	// inheritance models
	// --------------------------------------------

		QVBoxLayout* inheritance_Layout = new QVBoxLayout;
			inheritance_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(inheritance_Layout);

		QLabel* inheritance_Label = new QLabel("Inheritance");
		inheritance_Layout->addWidget(inheritance_Label);

		QRadioButton* replication_Factor_Radiobutton = new QRadioButton(replication_Factor_Inheritance_Model);
			replication_Factor_Radiobutton->setChecked(multilayer->imperfections_Model.inheritance_Model == replication_Factor_Inheritance_Model);
			replication_Factor_Radiobutton->setDisabled(multilayer->imperfections_Model.vertical_Correlation != partial_Correlation);
		inheritance_Layout->addWidget(replication_Factor_Radiobutton);

		QRadioButton* linear_Growth_Alpha_Radiobutton = new QRadioButton(linear_Growth_Alpha_Inheritance_Model);
			linear_Growth_Alpha_Radiobutton->setChecked(multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model);
			linear_Growth_Alpha_Radiobutton->setDisabled(multilayer->imperfections_Model.vertical_Correlation != partial_Correlation);
		inheritance_Layout->addWidget(linear_Growth_Alpha_Radiobutton);

		QRadioButton* linear_Growth_n_1_4_Radiobutton = new QRadioButton(linear_Growth_n_1_4_Inheritance_Model);
			linear_Growth_n_1_4_Radiobutton->setChecked(multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model);
			linear_Growth_n_1_4_Radiobutton->setDisabled(multilayer->imperfections_Model.vertical_Correlation != partial_Correlation);
		inheritance_Layout->addWidget(linear_Growth_n_1_4_Radiobutton);

		QButtonGroup* use_Group = new QButtonGroup;
			use_Group->addButton(replication_Factor_Radiobutton);
			use_Group->addButton(linear_Growth_Alpha_Radiobutton);
			use_Group->addButton(linear_Growth_n_1_4_Radiobutton);

	// connections
	connect(PT_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(PT_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = PT_approximation;

			// unlock Gauss peak
			gauss_Peak_Checkbox->setEnabled(true);
			// unlock measured PSD
			measured_PSD_1D_Checkbox->setEnabled(multilayer->imperfections_Model.vertical_Correlation != partial_Correlation);
			measured_PSD_1D_Checkbox->setEnabled(true);
			measured_PSD_2D_Checkbox->setEnabled(true);
			measured_PSD_2D_Checkbox->setEnabled(true);

			// unlock partial correlation
			partial_Radiobutton->setEnabled(true);
			if(partial_Radiobutton->isChecked())
			{
				replication_Factor_Radiobutton->setDisabled(false);
				linear_Growth_Alpha_Radiobutton->setDisabled(false);
				linear_Growth_n_1_4_Radiobutton->setDisabled(false);
			}
		}
	});
	connect(DWBA_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(DWBA_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = DWBA_approximation;

			// lock partial correlation
			if(partial_Radiobutton->isChecked())
			{
				partial_Radiobutton->setChecked(false);
				full_Radiobutton->setChecked(true);
				full_Radiobutton->toggled(true);
			}
			partial_Radiobutton->setEnabled(false);

			// lock measured PSD
			if(measured_PSD_1D_Checkbox->isChecked())	measured_PSD_1D_Checkbox->setChecked(false);
			measured_PSD_1D_Checkbox->setEnabled(false);
			if(measured_PSD_2D_Checkbox->isChecked())	measured_PSD_2D_Checkbox->setChecked(false);
			measured_PSD_2D_Checkbox->setEnabled(false);

			// lock Gauss peak
			gauss_Peak_Checkbox->setChecked(false);
			gauss_Peak_Checkbox->toggled(false);
			gauss_Peak_Checkbox->setEnabled(false);
		}
	});
	connect(SA_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(SA_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = SA_approximation;

			// lock partial correlation
			if(partial_Radiobutton->isChecked())
			{
				partial_Radiobutton->setChecked(false);
				full_Radiobutton->setChecked(true);
				full_Radiobutton->toggled(true);
			}
			partial_Radiobutton->setEnabled(false);

			// lock measured PSD
			if(measured_PSD_1D_Checkbox->isChecked())	measured_PSD_1D_Checkbox->setChecked(false);
			measured_PSD_1D_Checkbox->setEnabled(false);
			if(measured_PSD_2D_Checkbox->isChecked())	measured_PSD_2D_Checkbox->setChecked(false);
			measured_PSD_2D_Checkbox->setEnabled(false);

			// lock Gauss peak
			gauss_Peak_Checkbox->setChecked(false);
			gauss_Peak_Checkbox->toggled(false);
			gauss_Peak_Checkbox->setEnabled(false);
		}
	});
	connect(CSA_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(CSA_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.approximation = CSA_approximation;

			// lock partial correlation
			if(partial_Radiobutton->isChecked())
			{
				partial_Radiobutton->setChecked(false);
				full_Radiobutton->setChecked(true);
				full_Radiobutton->toggled(true);
			}
			partial_Radiobutton->setEnabled(false);

			// lock measured PSD
			if(measured_PSD_1D_Checkbox->isChecked())	measured_PSD_1D_Checkbox->setChecked(false);
			measured_PSD_1D_Checkbox->setEnabled(false);
			if(measured_PSD_2D_Checkbox->isChecked())	measured_PSD_2D_Checkbox->setChecked(false);
			measured_PSD_2D_Checkbox->setEnabled(false);

			// lock Gauss peak
			gauss_Peak_Checkbox->setChecked(false);
			gauss_Peak_Checkbox->toggled(false);
			gauss_Peak_Checkbox->setEnabled(false);
		}
	});


	connect(full_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(full_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.vertical_Correlation = full_Correlation;

			replication_Factor_Radiobutton->setDisabled(true);
			linear_Growth_Alpha_Radiobutton->setDisabled(true);
			linear_Growth_n_1_4_Radiobutton->setDisabled(true);

			common_Checkbox->setChecked(true);
			common_Checkbox->toggled(true);
			common_Checkbox->setDisabled(true);

			// unlock measured PSD
			measured_PSD_1D_Checkbox->setEnabled(true);
			measured_PSD_2D_Checkbox->setEnabled(true);
		}
	});
	connect(partial_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(partial_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.vertical_Correlation = partial_Correlation;

			replication_Factor_Radiobutton->setDisabled(false);
			linear_Growth_Alpha_Radiobutton->setDisabled(false);
			linear_Growth_n_1_4_Radiobutton->setDisabled(false);

			common_Checkbox->setDisabled(false);

			// lock measured PSD 1D
			if(measured_PSD_1D_Checkbox->isChecked())	  measured_PSD_1D_Checkbox->setChecked(false);
			measured_PSD_1D_Checkbox->setEnabled(false);
			measured_PSD_2D_Checkbox->setEnabled(true);
		}
	});
	connect(zero_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(zero_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.vertical_Correlation = zero_Correlation;

			replication_Factor_Radiobutton->setDisabled(true);
			linear_Growth_Alpha_Radiobutton->setDisabled(true);
			linear_Growth_n_1_4_Radiobutton->setDisabled(true);

                        if(multilayer->imperfections_Model.add_Measured_PSD_1D ||
                            multilayer->imperfections_Model.add_Measured_PSD_2D) {
                            common_Checkbox->setChecked(true);
                            common_Checkbox->toggled(true);
                            common_Checkbox->setDisabled(true);
                        } else
                            common_Checkbox->setDisabled(false);

			// unlock measured PSD
			measured_PSD_1D_Checkbox->setEnabled(true);
			measured_PSD_2D_Checkbox->setEnabled(true);
		}
	});


	connect(ABC_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(ABC_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.PSD_Model = ABC_Model;
		}
	});
	connect(fractal_Gauss_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(fractal_Gauss_Radiobutton->isChecked())
		{
            multilayer->imperfections_Model.PSD_Model = stretched_exp_Model;
		}
	});
	connect(measured_PSD_1D_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.add_Measured_PSD_1D = measured_PSD_1D_Checkbox->isChecked();

		if(((multilayer->imperfections_Model.add_Measured_PSD_1D ||
			 multilayer->imperfections_Model.add_Measured_PSD_2D ) &&
			 multilayer->imperfections_Model.vertical_Correlation == zero_Correlation) ||
			 multilayer->imperfections_Model.vertical_Correlation == full_Correlation)
		{
			common_Checkbox->setChecked(true);
			common_Checkbox->toggled(true);
			common_Checkbox->setDisabled(true);
		} else
		{
			common_Checkbox->setEnabled(true);
		}
	});
	connect(measured_PSD_2D_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.add_Measured_PSD_2D = measured_PSD_2D_Checkbox->isChecked();

		if(((multilayer->imperfections_Model.add_Measured_PSD_1D ||
			 multilayer->imperfections_Model.add_Measured_PSD_2D ) &&
			 multilayer->imperfections_Model.vertical_Correlation == zero_Correlation) ||
			 multilayer->imperfections_Model.vertical_Correlation == full_Correlation)
		{
			common_Checkbox->setChecked(true);
			common_Checkbox->toggled(true);
			common_Checkbox->setDisabled(true);
		} else
		{
			common_Checkbox->setEnabled(true);
		}
	});
	connect(gauss_Peak_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.add_Gauss_Peak = gauss_Peak_Checkbox->isChecked();
	});
	connect(common_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Common_Roughness_Function = common_Checkbox->isChecked();
	});

	connect(replication_Factor_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(replication_Factor_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.inheritance_Model = replication_Factor_Inheritance_Model;
		}
	});
	connect(linear_Growth_Alpha_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(linear_Growth_Alpha_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.inheritance_Model = linear_Growth_Alpha_Inheritance_Model;
		}
	});
	connect(linear_Growth_n_1_4_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(linear_Growth_n_1_4_Radiobutton->isChecked())
		{
			multilayer->imperfections_Model.inheritance_Model = linear_Growth_n_1_4_Inheritance_Model;
		}
	});
}

void Table_Roughness_Model_Editor::create_Particles_Groupbox()
{
	particles_Groupbox = new QGroupBox("Use particles");
		particles_Groupbox->setCheckable(true);
		particles_Groupbox->setChecked(multilayer->imperfections_Model.use_Particles);
	main_Layout->addWidget(particles_Groupbox);
	connect(particles_Groupbox, &QGroupBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Particles = particles_Groupbox->isChecked();
		multilayer->structure_Tree->refresh_Tree_Particles();
	});

	QHBoxLayout* groupbox_Layout = new QHBoxLayout(particles_Groupbox);

	// --------------------------------------------
	// full, partial or zero vertical correlation
	// --------------------------------------------

		QVBoxLayout* vertical_Correlation_Layout = new QVBoxLayout;
			vertical_Correlation_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(vertical_Correlation_Layout);

		QLabel* vertical_Correlation_Label = new QLabel("Vertical\ncorrelation");
		vertical_Correlation_Layout->addWidget(vertical_Correlation_Label);

		QRadioButton* full_Radiobutton = new QRadioButton("Full");
			full_Radiobutton->setChecked(multilayer->imperfections_Model.particle_Vertical_Correlation == full_Correlation);
		vertical_Correlation_Layout->addWidget(full_Radiobutton);

		QRadioButton* partial_Radiobutton = new QRadioButton("Partial");
			partial_Radiobutton->setChecked(multilayer->imperfections_Model.particle_Vertical_Correlation == partial_Correlation);
		vertical_Correlation_Layout->addWidget(partial_Radiobutton);

		QRadioButton* zero_Radiobutton = new QRadioButton("Zero");
			zero_Radiobutton->setChecked(multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation);
		vertical_Correlation_Layout->addWidget(zero_Radiobutton);

		QButtonGroup* vertical_Correlation_Group = new QButtonGroup;
			vertical_Correlation_Group->addButton(full_Radiobutton);
			vertical_Correlation_Group->addButton(partial_Radiobutton);
			vertical_Correlation_Group->addButton(zero_Radiobutton);

	// --------------------------------------------
	// initial common in-layer interference function
	// --------------------------------------------

		QVBoxLayout* interference_Function_Layout = new QVBoxLayout;
			interference_Function_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(interference_Function_Layout);

		QLabel* interference_Function_Label = new QLabel("Lateral order");
			interference_Function_Layout->addWidget(interference_Function_Label);

		QRadioButton* disorder_Radiobutton = new QRadioButton("Disorder");
			disorder_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Interference_Function == disorder);
		interference_Function_Layout->addWidget(disorder_Radiobutton);

		QRadioButton* radial_Paracrystal_Radiobutton = new QRadioButton("Radial paracrystal");
			radial_Paracrystal_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Interference_Function == radial_Paracrystal);
		interference_Function_Layout->addWidget(radial_Paracrystal_Radiobutton);

		QButtonGroup* interference_Function_Group = new QButtonGroup;
			interference_Function_Group->addButton(disorder_Radiobutton);
			interference_Function_Group->addButton(radial_Paracrystal_Radiobutton);

		// particles material
		QCheckBox* particles_Material_Checkbox = new QCheckBox("Specify material");
			particles_Material_Checkbox->setChecked(multilayer->imperfections_Model.use_Particles_Material);
		interference_Function_Layout->addWidget(particles_Material_Checkbox);

		// common parameters
		QCheckBox* common_Checkbox = new QCheckBox("Common parameters\nfor all layers");
			common_Checkbox->setChecked (multilayer->imperfections_Model.use_Common_Particle_Function);
		interference_Function_Layout->addWidget(common_Checkbox);

	// --------------------------------------------
	// initial common model
	// --------------------------------------------

		QVBoxLayout* model_Layout = new QVBoxLayout;
			model_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(model_Layout);

        QLabel* model_Label = new QLabel("Lattice type");
			model_Layout->addWidget(model_Label);

		QRadioButton* hexagonal_Radiobutton = new QRadioButton("Hexagonal");
			hexagonal_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Geometric_Model == hexagonal_Model);
		model_Layout->addWidget(hexagonal_Radiobutton);

		QRadioButton* square_Radiobutton = new QRadioButton("Square");
			square_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Geometric_Model == square_Model);
		model_Layout->addWidget(square_Radiobutton);

		QRadioButton* pure_Radial_Radiobutton = new QRadioButton("Radial");
			pure_Radial_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Geometric_Model == pure_Radial_Model);
//		model_Layout->addWidget(pure_Radial_Radiobutton);

		QButtonGroup* model_Group = new QButtonGroup;
			model_Group->addButton(hexagonal_Radiobutton);
			model_Group->addButton(square_Radiobutton);
			model_Group->addButton(pure_Radial_Radiobutton);


	// --------------------------------------------
	// initial common particles shape
	// --------------------------------------------

		QVBoxLayout* shape_Layout = new QVBoxLayout;
			shape_Layout->setAlignment(Qt::AlignTop);
		groupbox_Layout->addLayout(shape_Layout);

		QLabel* shape_Label = new QLabel("Particle shape");
		shape_Layout->addWidget(shape_Label);

		QRadioButton* full_Sphere_Radiobutton = new QRadioButton("Spheres ");
			full_Sphere_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Particle_Shape == full_Sphere);
		shape_Layout->addWidget(full_Sphere_Radiobutton);

		QRadioButton* full_Spheroid_Radiobutton = new QRadioButton("Spheroids ");
			full_Spheroid_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Particle_Shape == full_Spheroid);
		shape_Layout->addWidget(full_Spheroid_Radiobutton);

		QRadioButton* full_Cylinder_Radiobutton = new QRadioButton("Cylinders ");
			full_Cylinder_Radiobutton->setChecked(multilayer->imperfections_Model.initial_Particle_Shape == cylinder);
		shape_Layout->addWidget(full_Cylinder_Radiobutton);

		QButtonGroup* shape_Group = new QButtonGroup;
			shape_Group->addButton(full_Sphere_Radiobutton);
			shape_Group->addButton(full_Spheroid_Radiobutton);
			shape_Group->addButton(full_Cylinder_Radiobutton);

	/// connections

	connect(full_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(full_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.particle_Vertical_Correlation = full_Correlation;
		}
	});
	connect(partial_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(partial_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.particle_Vertical_Correlation = partial_Correlation;
		}
	});
	connect(zero_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(zero_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.particle_Vertical_Correlation = zero_Correlation;
		}
	});

	// interference
	connect(disorder_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(disorder_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Interference_Function = disorder;
		}
	});
	connect(radial_Paracrystal_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(radial_Paracrystal_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Interference_Function = radial_Paracrystal;
		}
	});
	connect(common_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Common_Particle_Function = common_Checkbox->isChecked();
	});
	connect(particles_Material_Checkbox, &QCheckBox::toggled, this, [=]
	{
		multilayer->imperfections_Model.use_Particles_Material = particles_Material_Checkbox->isChecked();
	});

	// model
	connect(hexagonal_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(hexagonal_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Geometric_Model = hexagonal_Model;
		}
	});
	connect(square_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(square_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Geometric_Model = square_Model;
		}
	});
	connect(pure_Radial_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(pure_Radial_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Geometric_Model = pure_Radial_Model;
		}
	});

	// shape
	connect(full_Sphere_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(full_Sphere_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Particle_Shape = full_Sphere;
		}
	});
	connect(full_Spheroid_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(full_Spheroid_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Particle_Shape = full_Spheroid;
		}
	});
	connect(full_Cylinder_Radiobutton, &QRadioButton::toggled, this, [=]
	{
		if(full_Cylinder_Radiobutton->isChecked())	{
			multilayer->imperfections_Model.initial_Particle_Shape = cylinder;
		}
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

//void Table_Roughness_Model_Editor::refresh_Tree_Roughness()
//{
//	QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
//	while(*it)
//	{
//		QTreeWidgetItem* item = *it;
//		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
//		if( struct_Data.item_Type == item_Type_Layer ||
//			struct_Data.item_Type == item_Type_Substrate )
//		{
//			bool is_Last_Layer = Global_Variables::if_Last_Layer(multilayer->structure_Tree->tree, item);
//			Global_Variables::enable_Disable_Roughness_Model(struct_Data, multilayer->imperfections_Model, is_Last_Layer);

//			QVariant var;
//			var.setValue( struct_Data );
//			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
//		}
//		++it;
//	}
//}

//void Table_Roughness_Model_Editor::refresh_Tree_Particles(bool refresh_Shape, bool refresh_Interference_Function, bool refresh_Geometry)
//{
//	QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
//	while(*it)
//	{
//		QTreeWidgetItem* item = *it;
//		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
//		if( struct_Data.item_Type == item_Type_Layer )
//		{
//			Global_Variables::enable_Disable_Particles_Model(struct_Data, multilayer->imperfections_Model);

//			if(refresh_Shape)
//			{
//				struct_Data.particles_Model.particle_Shape = multilayer->imperfections_Model.initial_Particle_Shape;
//			}
//			if(refresh_Interference_Function)
//			{
//				struct_Data.particles_Model.particle_Interference_Function = multilayer->imperfections_Model.initial_Interference_Function;
//			}
//			if(refresh_Geometry)
//			{
//				struct_Data.particles_Model.geometric_Model = multilayer->imperfections_Model.initial_Geometric_Model;
//			}

//			QVariant var;
//			var.setValue( struct_Data );
//			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
//		}
//		++it;
//	}
//}
