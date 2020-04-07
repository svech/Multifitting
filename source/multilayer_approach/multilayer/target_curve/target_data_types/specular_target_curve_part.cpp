#include "specular_target_curve_part.h"

Specular_Target_Curve_Part::Specular_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent) :
	QWidget(parent)
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(4,4,4,0);
		main_Layout->setSpacing(0);

	create_Argument_GroupBox();
	create_Value_GroupBox();
	create_Beam_GroupBox();
	create_Detector_GroupBox();
}

void Specular_Target_Curve_Part::create_Argument_GroupBox()
{
	QGroupBox* argument_GroupBox = new QGroupBox("Argument", this);
	bottom_Part_Layout->addWidget(argument_GroupBox);

	QVBoxLayout* argument_GroupBox_Layout = new QVBoxLayout(argument_GroupBox);
	argument_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	QGridLayout* interval_Layout = new QGridLayout;
		interval_Layout->setAlignment(Qt::AlignLeft);
	argument_GroupBox_Layout->addLayout(interval_Layout);

	QGridLayout* grid_1_Layout = new QGridLayout;
		grid_1_Layout->setAlignment(Qt::AlignLeft);
	argument_GroupBox_Layout->addLayout(grid_1_Layout);

	// specifying interval for fitting
	{
		QCheckBox* main_Subinterval_Checkbox = new QCheckBox("Use only data between argument");
			main_Subinterval_Checkbox->setCheckable(true);
			main_Subinterval_Checkbox->setChecked(target_Curve->curve.use_Subinterval);
		interval_Layout->addWidget(main_Subinterval_Checkbox,0,0,1,1,Qt::AlignLeft);
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
		{
			interval_Layout->addWidget(main_Subinterval_Checkbox,0,0,2,1,Qt::AlignLeft);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		QLabel* horizontal_Label = new QLabel;
		interval_Layout->addWidget(horizontal_Label,0,1,1,1,Qt::AlignLeft);
		QLabel* vertical_Label = new QLabel;
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
		{
			horizontal_Label->setText(Theta_Sym+" :");
			vertical_Label->setText(Phi_Sym+" :");
			interval_Layout->addWidget(vertical_Label,1,1,1,1,Qt::AlignLeft);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		MyDoubleSpinBox* horizontal_From_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_From_Subinterval_SpinBox->setAccelerated(true);
		if(target_Curve->loaded_And_Ready)
		{
			if(	target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan])
			{
				double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				horizontal_From_Subinterval_SpinBox->setRange(min,max);
			}
			if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
			{
				// TODO GISAS
			}
		} else
		{
			horizontal_From_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
		}
		horizontal_From_Subinterval_SpinBox->setDecimals(4);
		horizontal_From_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Left);
		horizontal_From_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/100.);
		horizontal_From_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		interval_Layout->addWidget(horizontal_From_Subinterval_SpinBox,0,2,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_From_Subinterval_SpinBox);

		MyDoubleSpinBox* vertical_From_Subinterval_SpinBox = new MyDoubleSpinBox;
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
		{
			vertical_From_Subinterval_SpinBox->setAccelerated(true);
			if(target_Curve->loaded_And_Ready)
			{
				// TODO GISAS
			} else
			{
				vertical_From_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
			}
			vertical_From_Subinterval_SpinBox->setDecimals(4);
			vertical_From_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Bottom);
			vertical_From_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Top-target_Curve->curve.subinterval_Bottom)/100.);
			vertical_From_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			interval_Layout->addWidget(vertical_From_Subinterval_SpinBox,1,2,1,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(vertical_From_Subinterval_SpinBox);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		QLabel* horizontal_And_Subinterval_Label = new QLabel("  and  ");
		interval_Layout->addWidget(horizontal_And_Subinterval_Label,0,3,1,1,Qt::AlignLeft);
		QLabel* vertical_And_Subinterval_Label = new QLabel;
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
		{
			vertical_And_Subinterval_Label ->setText("  and  ");
			interval_Layout->addWidget(vertical_And_Subinterval_Label,1,3,1,1,Qt::AlignLeft);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		MyDoubleSpinBox* horizontal_To_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_To_Subinterval_SpinBox->setAccelerated(true);
		if(target_Curve->loaded_And_Ready)
		{
			if(	target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan])
			{
				double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				horizontal_To_Subinterval_SpinBox->setRange(min,max);
			}
			if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
			{
				// TODO GISAS
			}
		} else
		{
			horizontal_To_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
		}
		horizontal_To_Subinterval_SpinBox->setDecimals(4);
		horizontal_To_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Right);
		horizontal_To_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/100.);
		horizontal_To_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		interval_Layout->addWidget(horizontal_To_Subinterval_SpinBox,0,4,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_To_Subinterval_SpinBox);

		MyDoubleSpinBox* vertical_To_Subinterval_SpinBox = new MyDoubleSpinBox;
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
		{
			vertical_To_Subinterval_SpinBox->setAccelerated(true);
			if(target_Curve->loaded_And_Ready)
			{
				// TODO GISAS
			} else
			{
				vertical_To_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
			}
			vertical_To_Subinterval_SpinBox->setDecimals(4);
			vertical_To_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Top);
			vertical_To_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Top-target_Curve->curve.subinterval_Bottom)/100.);
			vertical_To_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			interval_Layout->addWidget(vertical_To_Subinterval_SpinBox,1,4,1,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(vertical_To_Subinterval_SpinBox);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		connect(main_Subinterval_Checkbox, &QCheckBox::toggled, this, [=]
		{
			target_Curve->curve.use_Subinterval = main_Subinterval_Checkbox->isChecked();

			horizontal_Label					->setDisabled(!target_Curve->curve.use_Subinterval);
			vertical_Label						->setDisabled(!target_Curve->curve.use_Subinterval);
			horizontal_From_Subinterval_SpinBox	->setDisabled(!target_Curve->curve.use_Subinterval);
			vertical_From_Subinterval_SpinBox	->setDisabled(!target_Curve->curve.use_Subinterval);
			horizontal_And_Subinterval_Label	->setDisabled(!target_Curve->curve.use_Subinterval);
			vertical_And_Subinterval_Label		->setDisabled(!target_Curve->curve.use_Subinterval);
			horizontal_To_Subinterval_SpinBox	->setDisabled(!target_Curve->curve.use_Subinterval);
			vertical_To_Subinterval_SpinBox		->setDisabled(!target_Curve->curve.use_Subinterval);

			horizontal_From_Subinterval_SpinBox	->setMaximum(target_Curve->curve.subinterval_Right);
			horizontal_To_Subinterval_SpinBox	->setMinimum(target_Curve->curve.subinterval_Left);
			vertical_From_Subinterval_SpinBox	->setMaximum(target_Curve->curve.subinterval_Top);
			vertical_To_Subinterval_SpinBox		->setMinimum(target_Curve->curve.subinterval_Bottom);

			// show/hide subinterval on plot
			target_Curve_Plot->start_Rect->setVisible(target_Curve->curve.use_Subinterval);
			target_Curve_Plot->end_Rect->setVisible(target_Curve->curve.use_Subinterval);
			if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS] )
			{
				target_Curve_Plot->top_Rect->setVisible(target_Curve->curve.use_Subinterval);
				target_Curve_Plot->bottom_Rect->setVisible(target_Curve->curve.use_Subinterval);
			}
			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});

		connect(horizontal_From_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.subinterval_Left = horizontal_From_Subinterval_SpinBox->value();
			horizontal_To_Subinterval_SpinBox->setRange(target_Curve->curve.subinterval_Left,horizontal_To_Subinterval_SpinBox->maximum());
			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});
		connect(vertical_From_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.subinterval_Bottom = vertical_From_Subinterval_SpinBox->value();
			vertical_To_Subinterval_SpinBox->setRange(target_Curve->curve.subinterval_Bottom,vertical_To_Subinterval_SpinBox->maximum());
			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});
		connect(horizontal_To_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.subinterval_Right = horizontal_To_Subinterval_SpinBox->value();
			horizontal_From_Subinterval_SpinBox->setRange(horizontal_From_Subinterval_SpinBox->minimum(),target_Curve->curve.subinterval_Right);
			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});
		connect(vertical_To_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.subinterval_Top = vertical_To_Subinterval_SpinBox->value();
			vertical_From_Subinterval_SpinBox->setRange(vertical_From_Subinterval_SpinBox->minimum(),target_Curve->curve.subinterval_Top);
			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});
		main_Subinterval_Checkbox->toggled(target_Curve->curve.use_Subinterval);
	}

	// argument type
	{
		argument_Type_ComboBox = new QComboBox;
			argument_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
			argument_Type_ComboBox->addItem(argument_Types[Beam_Grazing_Angle]);
			argument_Type_ComboBox->addItem(argument_Types[Wavelength_Energy]);
			grid_1_Layout->addWidget(argument_Type_ComboBox,0,0,1,1,Qt::AlignLeft);
		}
		if( target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] )
		{
			argument_Type_ComboBox->addItem(argument_Types[Detector_Polar_Angle]);
			grid_1_Layout->addWidget(argument_Type_ComboBox,0,0,1,1,Qt::AlignLeft);
		}
		if( target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] )
		{
			argument_Type_ComboBox->addItem(argument_Types[Beam_Grazing_Angle]);
			grid_1_Layout->addWidget(argument_Type_ComboBox,0,0,1,1,Qt::AlignLeft);
		}
		if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS] )
		{
			argument_Type_ComboBox->addItem(argument_Types[Detector_Polar_Angle]);
			grid_1_Layout->addWidget(argument_Type_ComboBox,0,0,1,1,Qt::AlignLeft);

			// "vertical" argument
			QComboBox* second_Argument_Type_ComboBox = new QComboBox;
				second_Argument_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			second_Argument_Type_ComboBox->addItem(argument_Types[Detector_Azimuthal_Angle]);

			second_Argument_Type_ComboBox->adjustSize();
			argument_Type_ComboBox->adjustSize();
			second_Argument_Type_ComboBox->setFixedWidth(max(second_Argument_Type_ComboBox->width(),argument_Type_ComboBox->width()));
			argument_Type_ComboBox->setFixedWidth(max(second_Argument_Type_ComboBox->width(),argument_Type_ComboBox->width()));
			grid_1_Layout->addWidget(second_Argument_Type_ComboBox,1,0,1,1,Qt::AlignLeft);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		argument_Units_ComboBox = new QComboBox;
			argument_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		grid_1_Layout->addWidget(argument_Units_ComboBox,0,1,1,1,Qt::AlignLeft);
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
		{
			grid_1_Layout->addWidget(argument_Units_ComboBox,0,1,2,1,Qt::AlignLeft);
		}
	}

	// argument shift/scale
	{
		QLabel* horizontal_Arg_Offset_Label = new QLabel("   Scale shift");
		grid_1_Layout->addWidget(horizontal_Arg_Offset_Label,0,2,1,1,Qt::AlignLeft);
		if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS] )
		{
			QLabel* vertical_Arg_Offset_Label = new QLabel("   Scale shift");
			grid_1_Layout->addWidget(vertical_Arg_Offset_Label,1,2,1,1,Qt::AlignLeft);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		double coeff = 1;
		if(target_Curve->measurement.argument_Type == whats_This_Beam_Theta_0_Angle)	  { coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units); }
		if(target_Curve->measurement.argument_Type == whats_This_Wavelength){ coeff = 0.1/wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units); }

		MyDoubleSpinBox* horizontal_Arg_Offset_SpinBox = new MyDoubleSpinBox;
			horizontal_Arg_Offset_SpinBox->setAccelerated(true);
			horizontal_Arg_Offset_SpinBox->setRange(-100000, MAX_DOUBLE);
			horizontal_Arg_Offset_SpinBox->setDecimals(4);
			horizontal_Arg_Offset_SpinBox->setValue(target_Curve->curve.horizontal_Arg_Shift);
			horizontal_Arg_Offset_SpinBox->setSingleStep(0.001/coeff);
			horizontal_Arg_Offset_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		grid_1_Layout->addWidget(horizontal_Arg_Offset_SpinBox,0,3,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_Arg_Offset_SpinBox);

		MyDoubleSpinBox* vertical_Arg_Offset_SpinBox = new MyDoubleSpinBox;
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS] )
		{
				vertical_Arg_Offset_SpinBox->setAccelerated(true);
				vertical_Arg_Offset_SpinBox->setRange(-100000, MAX_DOUBLE);
				vertical_Arg_Offset_SpinBox->setDecimals(4);
				vertical_Arg_Offset_SpinBox->setValue(target_Curve->curve.vertical_Arg_Shift);
				vertical_Arg_Offset_SpinBox->setSingleStep(0.001/coeff);
				vertical_Arg_Offset_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			grid_1_Layout->addWidget(vertical_Arg_Offset_SpinBox,1,3,1,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(vertical_Arg_Offset_SpinBox);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		QLabel* horizontal_Arg_Factor_Label = new QLabel("   Scale factor");
		grid_1_Layout->addWidget(horizontal_Arg_Factor_Label,0,4,1,1,Qt::AlignLeft);
		if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS] )
		{
			QLabel* vertical_Arg_Factor_Label = new QLabel("   Scale factor");
			grid_1_Layout->addWidget(vertical_Arg_Factor_Label,1,4,1,1,Qt::AlignLeft);
		}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		MyDoubleSpinBox*  horizontal_Arg_Factor_SpinBox = new MyDoubleSpinBox;
			horizontal_Arg_Factor_SpinBox->setAccelerated(true);
			horizontal_Arg_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			horizontal_Arg_Factor_SpinBox->setDecimals(4);
			horizontal_Arg_Factor_SpinBox->setValue(target_Curve->curve.horizontal_Arg_Factor);
			horizontal_Arg_Factor_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			horizontal_Arg_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		grid_1_Layout->addWidget(horizontal_Arg_Factor_SpinBox,0,5,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_Arg_Factor_SpinBox);

		MyDoubleSpinBox* vertical_Arg_Factor_SpinBox = new MyDoubleSpinBox;
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS] )
		{
			vertical_Arg_Factor_SpinBox->setAccelerated(true);
			vertical_Arg_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			vertical_Arg_Factor_SpinBox->setDecimals(4);
			vertical_Arg_Factor_SpinBox->setValue(target_Curve->curve.vertical_Arg_Factor);
			vertical_Arg_Factor_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			vertical_Arg_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			grid_1_Layout->addWidget(vertical_Arg_Factor_SpinBox,1,5,1,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(vertical_Arg_Factor_SpinBox);
		}
	}

}

void Specular_Target_Curve_Part::create_Value_GroupBox()
{

}

void Specular_Target_Curve_Part::create_Beam_GroupBox()
{

}

void Specular_Target_Curve_Part::create_Detector_GroupBox()
{

}
