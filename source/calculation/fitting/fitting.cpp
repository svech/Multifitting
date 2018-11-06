// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "fitting.h"

Fitting::Fitting(Main_Calculation_Module* main_Calculation_Module):

	main_Calculation_Module(main_Calculation_Module),
	calculation_Trees	(main_Calculation_Module->calculation_Trees),
	fitables			(main_Calculation_Module->fitables),
	confidentials		(main_Calculation_Module->confidentials),

	n(num_Residual_Points()),
	p(fitables.param_Pointers.size()),

	params({main_Calculation_Module,
		   calculation_Trees,
		   fitables,
		   confidentials,
		   main_Calculation_Module->calc_Mode,
		   p,
		   f,
		   n,
		   x,

		   -2018,
		   -2018,

		   // for SwarmOps
		   0
		   })
{
	f = gsl_vector_calloc(n);
	x = gsl_vector_calloc(p);
	params.f = f;
	params.x = x;
}

Fitting::~Fitting()
{
	gsl_vector_free(f);
	gsl_vector_free(x);
}

double Fitting::func(double argument, int index)
{
	// TODO
	if(index == 0)
	{
		return log(argument+1E-5);
	} else
	{
		return argument;
	}
}

size_t Fitting::num_Residual_Points()
{
	size_t residual_Points = 0;
	// over multilayers
	for(int tab_Index=0; tab_Index<calculation_Trees.size(); ++tab_Index)
	{
		// over target curves
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			// over points
			residual_Points += target_Element.the_Class->curve.values.size();
		}
	}
	return residual_Points;
}

void Fitting::period_Subtree_Iteration(const tree<Node>::iterator& parent, double coeff)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		if(abs(coeff)>DBL_MIN) // Do we need it?
		{
			if(struct_Data.item_Type == item_Type_Layer)
			{
				struct_Data.thickness.value *= coeff;
			} else
			if(struct_Data.item_Type == item_Type_Multilayer)
			{
				struct_Data.period.value *= coeff;
				period_Subtree_Iteration(child, coeff);
			}
		}
	}
}

void Fitting::gamma_Subtree_Iteration(const tree<Node>::iterator& parent, double old_Value)
{
	Data& gamma_Multilayer_Data = parent.node->data.struct_Data;
	double old_First_Thickness = gamma_Multilayer_Data.period.value * old_Value;
	double new_First_Thickness = gamma_Multilayer_Data.period.value * gamma_Multilayer_Data.gamma.value;

	unsigned i=0;
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			child_Data.thickness.value = new_First_Thickness;
		} else
		if(child_Data.item_Type == item_Type_Multilayer)
		{
			if(abs(old_First_Thickness) > DBL_MIN)
			{
				double coeff = new_First_Thickness/old_First_Thickness;
				child_Data.period.value *= coeff;
				period_Subtree_Iteration(child, coeff);
			}
		}
	}

	double old_Second_Thickness = gamma_Multilayer_Data.period.value - old_First_Thickness;
	double new_Second_Thickness = gamma_Multilayer_Data.period.value - new_First_Thickness;

	i=1;
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			child_Data.thickness.value = new_Second_Thickness;
		} else
		if(child_Data.item_Type == item_Type_Multilayer)
		{
			if(abs(old_Second_Thickness) > DBL_MIN)
			{
				double coeff = new_Second_Thickness/old_Second_Thickness;
				child_Data.period.value *= coeff;
				period_Subtree_Iteration(child, coeff);
			}
		}
	}
}

void Fitting::slaves_Recalculation(Parameter* master, Fitting_Params* params)
{
	for(int slave_Index=0; slave_Index<master->coupled.slaves.size(); ++slave_Index)
	{
//		Parameter_Indicator& slave_Parameter_Indicator = master->coupled.slaves[slave_Index];
		Parameter* slave = master->coupled.slave_Pointers[slave_Index];

#ifdef EXPRTK
		// local parsing. not used, slower
//		exprtk::parser<double> parser;
//		exprtk::symbol_table<double> symbol_table;
//		exprtk::expression<double> expression_Exprtk;

//		symbol_table.add_variable(expression_Master_Slave_Variable, master->value);
//		symbol_table.add_constants();

//		expression_Exprtk.register_symbol_table(symbol_table);
//		parser.compile(slave_Parameter_Indicator.expression.toStdString(), expression_Exprtk);
//		slave->value = expression_Exprtk.value();

		int index = params->main_Calculation_Module->slaves_Expression_Map.value(slave->indicator.id);
		// argument changes automatically
//		*params->main_Calculation_Module->argument_Vec[index] = master->value;
		slave->value = params->main_Calculation_Module->expression_Vec[index].value();
#else
		slave->value = master->value;
#endif
		if(slave->coupled.slaves.size()>0)
		{
			slaves_Recalculation(slave, params);
		}
	}
}

void Fitting::calc_Residual(const gsl_vector* x, Fitting_Params* params, gsl_vector* f)
{
	// change value of real fitables
	for(size_t i=0; i<params->fitables.param_Pointers.size(); ++i)
	{
		double old_Value = params->fitables.param_Pointers[i]->value;

		// refresh
		params->fitables.param_Pointers[i]->value = params->main_Calculation_Module->unparametrize(	gsl_vector_get(x, i),
																									params->fitables.param_Pointers[i]->fit.min,
																									params->fitables.param_Pointers[i]->fit.max);
		double new_Value = params->fitables.param_Pointers[i]->value;
		change_Real_Fitables_and_Dependent(params, old_Value, new_Value, i, FITTING);
	}

	// change value of slaves of ACTIVE confidentials
	if(params->calc_Mode == CONFIDENCE )
	for(size_t i=0; i<params->confidentials.param_Pointers.size(); ++i)
	{
		if(params->confidentials.param_Pointers[i]->confidence.is_Active)
		{
			double old_Value = params->confidentials.param_Pointers[i]->value;
			double new_Value = params->confidentials.values[i];

			// refresh
			params->confidentials.param_Pointers[i]->value = params->confidentials.values[i];
			change_Real_Fitables_and_Dependent(params, old_Value, new_Value, i, CONFIDENCE);
		}
	}

	/// now all real_Calc_Tree are changed; we can replicate and stratify them

	int residual_Shift=0;

	// iterate over structures
	for(int tab_Index=0; tab_Index<params->calculation_Trees.size(); ++tab_Index)
	{
		// over target curves
		int target_Index = 0;

		// if use reference Data_Element<Target_Curve>& then addresses of slaves are changing!!! WTF?
		for(Data_Element<Target_Curve>& target_Element : params->calculation_Trees[tab_Index]->target)
		{
			// replication of real_Calc_Tree for each target
			target_Element.calc_Tree = params->calculation_Trees[tab_Index]->real_Calc_Tree;
			params->calculation_Trees[tab_Index]->stratify_Calc_Tree(target_Element.calc_Tree);

			// calculation
			params->calculation_Trees[tab_Index]->calculate_1_Kind(target_Element);

			// fill residual
			fill_Residual(params, residual_Shift, target_Element, f, target_Index);
			target_Index++;
		}
	}

	// aperiodic addition: difference between layers thicknesses
	for(int tab_Index=0; tab_Index<params->calculation_Trees.size(); ++tab_Index)
	{
		qInfo() << "calc_Residual";
//		params->main_Calculation_Module->multilayers[0]->structure_Tree->tree
		params->calculation_Trees[tab_Index]->look_Aperiodic(params->calculation_Trees[tab_Index]->real_Calc_Tree.begin());
	}
}

void Fitting::change_Real_Fitables_and_Dependent(Fitting_Params* params, double old_Value, double new_Value, size_t i, QString fit_Conf)
{
	double coeff = new_Value/old_Value;

	if(fit_Conf == FITTING)
	{
		// recalculate underlying tree if period or gamma is fitable
		if(params->fitables.param_Pointers[i]->indicator.whats_This == whats_This_Period)		{
			Fitting::period_Subtree_Iteration(params->fitables.parent_Iterators[i], coeff);
		} else
		if(params->fitables.param_Pointers[i]->indicator.whats_This == whats_This_Gamma)		{
			Fitting::gamma_Subtree_Iteration(params->fitables.parent_Iterators[i], old_Value);
		}
		// recalculate underlying slaves
		Fitting::slaves_Recalculation(params->fitables.param_Pointers[i], params);
	} else
	if(fit_Conf == CONFIDENCE)
	{
		// recalculate underlying tree if period or gamma is confidential
		if(params->confidentials.param_Pointers[i]->indicator.whats_This == whats_This_Period)		{
			Fitting::period_Subtree_Iteration(params->confidentials.parent_Iterators[i], coeff);
		} else
		if(params->confidentials.param_Pointers[i]->indicator.whats_This == whats_This_Gamma)		{
			Fitting::gamma_Subtree_Iteration(params->confidentials.parent_Iterators[i], old_Value);
		}
		// recalculate underlying slaves
		Fitting::slaves_Recalculation(params->confidentials.param_Pointers[i], params);
	}
}

void Fitting::fill_Residual(Fitting_Params* params, int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f, int index)
{
	Target_Curve* target_Curve = target_Element.the_Class;
	double fi_1, fi_2, factor;
	int N = target_Curve->curve.values.size();
	double N_sqrt = sqrt(double(N));
	double n_P_sqrt = sqrt(double(params->n-params->fitables.param_Names.size()));
	double power = target_Curve->fit_Params.power/2.;

	vector<double> model_Curve(N);

	/// -------------------------------------------------------------------------------
	/// reflectance
	/// -------------------------------------------------------------------------------

	if(target_Curve->curve.value_Mode == value_R_Mode[R] )				// R
	{
		model_Curve = target_Element.unwrapped_Reflection->R_Instrumental;
	} else
	if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )
	{
		qInfo() << "Fitting::fill_Residual  :  sorry, R_Phi is not ready";
		return;
	} else

	/// -------------------------------------------------------------------------------
	/// transmittance
	/// -------------------------------------------------------------------------------

	if(target_Curve->curve.value_Mode == value_T_Mode[T] )				// T
	{
		model_Curve = target_Element.unwrapped_Reflection->T_Instrumental;
	}

	// use_Chi2
	if(target_Curve->fit_Params.use_Chi2)
	{
		for(int point_Index=0; point_Index<N; ++point_Index)
		{
			factor = target_Curve->fit_Params.weight_Sqrt/n_P_sqrt;
			fi_1 = target_Curve->curve.values[point_Index].val_1;
			fi_2 = model_Curve[point_Index];

			gsl_vector_set(f, residual_Shift+point_Index, factor*(fi_1-fi_2)*sqrt(target_Curve->curve.beam_Intensity/fi_2)   );
		}
	} else
	// use custom expression
	{
		for(int point_Index=0; point_Index<N; ++point_Index)
		{
			// calculate with expression
			{
#ifdef EXPRTK
				target_Curve->fit_Params.expression_Argument = target_Curve->curve.values[point_Index].val_1;
				fi_1 = target_Curve->fit_Params.expression_Vec[0].value();
#else
				fi_1 = func(target_Curve->curve.values[point_Index].val_1, index);
#endif
			}
			{
#ifdef EXPRTK
				target_Curve->fit_Params.expression_Argument = model_Curve[point_Index];
				fi_2 = target_Curve->fit_Params.expression_Vec[0].value();
#else
				fi_2 = func(model_Curve[point_Index], index);
#endif
			}

			// weight
			factor = target_Curve->fit_Params.weight_Sqrt;
			if(target_Curve->fit_Params.norm) { factor /= N_sqrt; }

			// fill
			gsl_vector_set(f, residual_Shift+point_Index, factor*pow(abs(fi_1-fi_2),power));
		}
	}
	residual_Shift += N;
}

void Fitting::initialize_Position()
{
	// set value of real fitables
	for(size_t i=0; i<fitables.param_Pointers.size(); ++i)
	{
		fitables.values_Parametrized[i] = main_Calculation_Module->parametrize(fitables.param_Pointers[i]->value,
																			   fitables.param_Pointers[i]->fit.min,
																			   fitables.param_Pointers[i]->fit.max);
		gsl_vector_set(x, i, fitables.values_Parametrized[i]);
	}
	// initialize residual
	calc_Residual(params.x, &params, params.f);
	gsl_blas_ddot(params.f, params.f, &params.init_Residual);
}

void Fitting::randomize_Position(bool randomize)
{
	if(!randomize) return;

	const gsl_rng_type* T = gsl_rng_default;
	gsl_rng* r = gsl_rng_alloc (T);
	gsl_rng_env_setup();
	auto now = chrono::system_clock::now();
	auto now_ns = chrono::time_point_cast<chrono::nanoseconds>(now);
	auto value = now_ns.time_since_epoch();
	long duration = value.count();
	gsl_rng_set(r, duration);

	// set random value of real fitables
	for(size_t i=0; i<fitables.param_Pointers.size(); ++i)
	{
		fitables.values_Parametrized[i] = gsl_rng_uniform(r);
		gsl_vector_set(x, i, fitables.values_Parametrized[i]);
	}

	gsl_rng_free (r);
}

bool Fitting::run_Fitting()
{
	// GSL
	if(	GSL_Methods.contains(global_Multilayer_Approach->fitting_Settings->current_Method) )
	{
		qInfo() << "GSL " << global_Multilayer_Approach->fitting_Settings->current_Method << "optimization";
		Fitting_GSL fitting_GSL(this);
		return fitting_GSL.fit();
	}

	// SO
	if(	SO_Methods.contains(global_Multilayer_Approach->fitting_Settings->current_Method) )
	{
		qInfo() << "SO " << global_Multilayer_Approach->fitting_Settings->current_Method << "optimization";
		Fitting_SwarmOps fitting_SwarmOps(this);
		return fitting_SwarmOps.fit();
	}
	return false;
}

bool Fitting::fit()
{
	if(check_Residual_Expression()) return false;

	// --------------------------------------------------------------------------------
	if(n<=p)
	{
		QMessageBox::information(nullptr,"Insufficient number of points", "# of fitables: " + QString::number(p) + "\n# of points: " + QString::number(n) + "\n\nShould be f<p");
		return false;
	}
	// --------------------------------------------------------------------------------
	initialize_Position();
	bool fit_Return = false;
	// single fit
	if(!global_Multilayer_Approach->fitting_Settings->randomized_Start)
	{
//		add_Fit_To_File(params.x, params.init_Residual, default_Fit_Statictics_File, 0);
		main_Calculation_Module->add_Fit(fitted_State);
		fit_Return = run_Fitting();
//		add_Fit_To_File(params.x, params.final_Residual, default_Fit_Statictics_File, 1);
	} else
	// randomized fit
	{
		// save trees to fit
		main_Calculation_Module->add_Fit(fit_Run_State, 0);
		add_Fit_To_File(params.x, params.init_Residual, default_Fit_Statictics_File, 0);
		for(int run=1; run<=global_Multilayer_Approach->fitting_Settings->num_Runs; run++)
		{
			randomize_Position(run-1); // first without randomization
			fit_Return = run_Fitting();
			main_Calculation_Module->renew_Item_Trees();
			main_Calculation_Module->add_Fit(fit_Run_State, run);
			add_Fit_To_File(params.x, params.final_Residual, default_Fit_Statictics_File, run);
		}
	}
	// --------------------------------------------------------------------------------

	return fit_Return;
}

bool Fitting::confidence(const vector<double>& fitables_Pointers_Value_Backup, const vector<double>& confidentials_Pointers_Value_Backup, size_t confidence_Index)
{
	if(check_Residual_Expression()) return false;

	// --------------------------------------------------------------------------------
	if(n<=p)
	{
		QMessageBox::information(nullptr,"Insufficient number of points", "# of fitables: " + QString::number(p) + "\n# of points: " + QString::number(n) + "\n\nShould be f<p");
		return false;
	}
	// --------------------------------------------------------------------------------

	bool fit_Return = false;
	double min     = confidentials.param_Pointers[confidence_Index]->confidence.min;
	double max     = confidentials.param_Pointers[confidence_Index]->confidence.max;
	int num_Points = confidentials.param_Pointers[confidence_Index]->confidence.num_Points;
	double step = (max-min)/(num_Points-1);
	double& real_Conf_Value = confidentials.values[confidence_Index]; // not pointers! pointers will be renewed in calc_Residual
	int point_Index = 0;

	// changing value step by step
	for(real_Conf_Value = min; real_Conf_Value <= (max+step/2); real_Conf_Value += step)
	{
		initialize_Position();

		for(size_t i=0; i<confidentials.param_Pointers.size(); i++)		{
			qInfo() << confidentials.param_Pointers[i]->value;
		}qInfo() << "";

		qInfo() << "fitables.param_Pointers.size()"<<fitables.param_Pointers.size();
		qInfo() << "fitables.param_Names"<<fitables.param_Names;
		// single fit
		if(fitables.param_Pointers.size()>0)
		{
			if(!global_Multilayer_Approach->fitting_Settings->randomized_Start)
			{
				fit_Return = run_Fitting();

				// reset to initial state
				for(size_t i=0; i<fitables.param_Pointers.size(); i++)			{
					fitables.param_Pointers[i]->value = fitables_Pointers_Value_Backup[i];
				}

				// write point to file
				add_Confidence_Distribution_To_File(real_Conf_Value, default_Confidence_Distribution_File, confidence_Index, point_Index, params.final_Residual, nullptr);
			} else
			// randomized fit
			{
				vector<double> rand_Fit_Residuals(global_Multilayer_Approach->fitting_Settings->num_Runs);
				for(size_t rand_Index=0; rand_Index<rand_Fit_Residuals.size(); rand_Index++)
				{
					randomize_Position(rand_Index); // first without randomization
					fit_Return = run_Fitting();
					rand_Fit_Residuals[rand_Index] = params.final_Residual;

					// reset to initial state
					for(size_t i=0; i<fitables.param_Pointers.size(); i++)			{
						fitables.param_Pointers[i]->value = fitables_Pointers_Value_Backup[i];
					}
				}
				// sort residuals
				sort(rand_Fit_Residuals.begin(), rand_Fit_Residuals.end());

				// take best
				params.final_Residual = rand_Fit_Residuals.front();

				// write point to file
				add_Confidence_Distribution_To_File(real_Conf_Value, default_Confidence_Distribution_File, confidence_Index, point_Index, params.final_Residual, &rand_Fit_Residuals);
			}
		} else
		{
			fit_Return = true;
			params.final_Residual = params.init_Residual;
			add_Confidence_Distribution_To_File(real_Conf_Value, default_Confidence_Distribution_File, confidence_Index, point_Index, params.final_Residual, nullptr);
		}
		qInfo() << "real_Conf_Value = " << real_Conf_Value << "~ final_Residual =" << params.final_Residual;
		++point_Index;
	}
	// restore current confidential to init value
	real_Conf_Value = confidentials_Pointers_Value_Backup[confidence_Index];
	confidentials.param_Pointers[confidence_Index]->value = confidentials_Pointers_Value_Backup[confidence_Index];

	// --------------------------------------------------------------------------------
	return fit_Return;
}

void Fitting::add_Fit_To_File(const gsl_vector* x, double residual, QString filename, int run)
{
	QFile file(filename);
	if(file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream out(&file);
		out.setFieldAlignment(QTextStream::AlignLeft);
		QString current_String;
		int index=0;
		int spacer = 5;

		if(run==0)
		{
			if(out.pos()!=0)
			{
				out << qSetFieldWidth(0) << endl;
			}
			QDateTime date_Time = QDateTime::currentDateTime();
			out << date_Time.toString("< dd.MM.yyyy | hh:mm:ss >") << qSetFieldWidth(0) <<endl;

			widths.clear();
			//------------------------------------------------------
			current_String = "run";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;

			current_String = "residual";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;
			//------------------------------------------------------
			out.setFieldWidth(35);
			for(size_t param_Index=0; param_Index<params.p; ++param_Index)
			{
				//------------------------------------------------------
				current_String = params.fitables.param_Names[param_Index];
				widths.append(current_String.length()+spacer);
				out << qSetFieldWidth(widths[index]) << current_String;
				index++;
				//------------------------------------------------------
			}
			//------------------------------------------------------
			out << qSetFieldWidth(0) <<endl;
			//------------------------------------------------------
		}
		//------------------------------------------------------
		index=0;

		current_String = QString::number(run);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;

		current_String = QString::number(residual,'e',3);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;

		//------------------------------------------------------
		for(size_t param_Index=0; param_Index<params.p; ++param_Index)
		{
			double unparametrized = params.main_Calculation_Module->unparametrize(	gsl_vector_get(x, param_Index),
																					params.fitables.param_Pointers[param_Index]->fit.min,
																					params.fitables.param_Pointers[param_Index]->fit.max);
			if(params.fitables.param_Pointers[param_Index]->indicator.whats_This == whats_This_Num_Repetitions)
			{
				current_String = QString::number(round(unparametrized));
			} else
			{
				current_String = QString::number(unparametrized,'f',4);
			}
			//------------------------------------------------------
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;
			//------------------------------------------------------
		}
		//------------------------------------------------------
		out << qSetFieldWidth(0) <<endl;
		//------------------------------------------------------

	file.close();
	} else
	{
		QMessageBox::critical(nullptr, "Fitting::add_Fit_To_File", "Can't write file " + filename);
		exit(EXIT_FAILURE);
	}
}

void Fitting::add_Confidence_Distribution_To_File(double real_Conf_Value, QString filename, size_t confidence_Index, int point_Index, double residual, vector<double>* residuals_Set)
{
	QFile file(filename);
	if(file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream out(&file);
		out.setFieldAlignment(QTextStream::AlignLeft);
		QString current_String;
		int index=0;
		int spacer = 5;

		if(point_Index==0)
		{
			if(out.pos()!=0)
			{
				out << qSetFieldWidth(0) << endl;
			}
			QDateTime date_Time = QDateTime::currentDateTime();
			out << date_Time.toString("< dd.MM.yyyy | hh:mm:ss >") << qSetFieldWidth(0) <<endl;

			widths.clear();
			//------------------------------------------------------

			out << "----------------------------------------------------" << qSetFieldWidth(0) <<endl;
			current_String = confidentials.param_Names[confidence_Index];
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String << qSetFieldWidth(0) <<endl;
			index++;
			out << "----------------------------------------------------" << qSetFieldWidth(0) <<endl;
			//------------------------------------------------------

			current_String = "value";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;

			current_String = "likehood";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;

			out << qSetFieldWidth(0) <<endl <<endl;
			//------------------------------------------------------

		}
		//------------------------------------------------------
		index=1;

		current_String = QString::number(real_Conf_Value);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;

		current_String = QString::number(residual,'e',3);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;
		//------------------------------------------------------

		// whole set of values
		if(residuals_Set != nullptr)
		{
			out << qSetFieldWidth(2) <<  "(";
			for(size_t i=0; i<residuals_Set->size(); ++i)
			{
				current_String = QString::number((*residuals_Set)[i],'e',3);
				out << qSetFieldWidth(0) << current_String;
				if(i!=residuals_Set->size()-1)
					out << qSetFieldWidth(3) << "," <<qSetFieldWidth(2) ;
			}
			out << qSetFieldWidth(0) <<  ")";
		}

		//------------------------------------------------------
		out << qSetFieldWidth(0) <<endl;
		//------------------------------------------------------

	file.close();
	} else
	{
		QMessageBox::critical(nullptr, "Fitting::add_Confidence_Distribution_To_File", "Can't write file " + filename);
		exit(EXIT_FAILURE);
	}
}

bool Fitting::check_Residual_Expression()
{
	// over multilayers
	for(int tab_Index=0; tab_Index<calculation_Trees.size(); tab_Index++)
	{
		// over target curves
		for(int target_Element_Index=0; target_Element_Index<calculation_Trees[tab_Index]->target.size(); target_Element_Index++)
		{
			Target_Curve* target_Curve = calculation_Trees[tab_Index]->target[target_Element_Index].the_Class;
			QString struct_Name = "  " + Medium_BlackCircle_Sym + "  <" + main_Calculation_Module->multilayer_Tabs->tabText(tab_Index) + "> ";

			/// -------------------------------------------------------------------------------
			/// weights
			/// -------------------------------------------------------------------------------

			if(abs(target_Curve->fit_Params.weight) < DBL_MIN)
			{
				QMessageBox::information(nullptr,"Bad weight", "Weight coefficient in\n\n" +
															struct_Name + ", measured curve #" + QString::number(target_Element_Index+1) +
															"\n\nshould be positive");
				return true;
			}

			/// -------------------------------------------------------------------------------
			/// 1 value
			/// -------------------------------------------------------------------------------

			if(target_Curve->curve.value_Mode == value_R_Mode[R] || 			// R
			   target_Curve->curve.value_Mode == value_T_Mode[T] )				// T
			{
				if(target_Curve->fit_Params.expression_Vec.size() < 1)
				{
					QMessageBox::information(nullptr,"Bad expression", "Residual function\n\n\"" +
																	target_Curve->fit_Params.fit_Function +
																	"\"\n\nin\n\n" +
																	struct_Name + ", measured curve #" + QString::number(target_Element_Index+1) +
																	"\n\nshould contains >=1 expression, separated by \"" + fit_Function_Separator + "\"");
					return true;
				}
			}

			/// -------------------------------------------------------------------------------
			/// 2 values
			/// -------------------------------------------------------------------------------

			if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )
			{
				if(target_Curve->fit_Params.expression_Vec.size() < 2)
				{
					QMessageBox::information(nullptr,"Bad expression", "Residual function\n\n\"" +
																	target_Curve->fit_Params.fit_Function +
																	"\"\n\nin\n\n" +
																	struct_Name + ", measured curve #" + QString::number(target_Element_Index+1) +
																	"\n\nshould contains >=1 expression, separated by \"" + fit_Function_Separator + "\"");
					return true;
				}
			}
		}
	}
	return false;
}
