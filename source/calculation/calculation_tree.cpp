#include "calculation_tree.h"
#include "multilayer_approach/multilayer/multilayer.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(Multilayer* multilayer, QString calc_Mode):
	calc_Mode(calc_Mode),
	multilayer(multilayer),
	real_Struct_Tree(multilayer->structure_Tree->tree)
{	
	// initialization independent vector
	if(calc_Mode==CALCULATION)
	{
		if(multilayer->enable_Calc_Independent_Curves)
		for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
		{
			Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
			if( independent_Variables->calc_Functions.check_Enabled	)
			if(	independent_Variables->calc_Functions.if_Something_Enabled() )
			{
				independent.resize(independent.size()+1);

				independent.last().the_Class = independent_Variables;
				independent.last().curve_Class = INDEPENDENT;
			}
		}
	}

	// initialization target profiles vector
	{
		if( multilayer->enable_Calc_Target_Curves )
		for(int i=0; i<multilayer->target_Profiles_Vector.size(); ++i)
		{
			if(multilayer->target_Profiles_Vector[i]->loaded_And_Ready)
			if(multilayer->target_Profiles_Vector[i]->fit_Params.calc)
			if(calc_Mode==CALCULATION || multilayer->target_Profiles_Vector[i]->fit_Params.fit)	// check fit value only if FITTING mode (otherwise CALCULATION is enough)
			{
				target.resize(target.size()+1);

				target.last().the_Class = multilayer->target_Profiles_Vector[i];
				target.last().curve_Class = TARGET;
				target.last().active_Item_Type = item_Type_Measurement;
			}
		}
	}

	prepare_Residual_Expressions();
	create_Rand_Generator();
	check_If_Graded();
	max_Depth = Global_Variables::get_Tree_Depth(real_Struct_Tree->invisibleRootItem());	// unstratified depth
}

Calculation_Tree::~Calculation_Tree()
{
}

void Calculation_Tree::prepare_Residual_Expressions()
{
	for(Data_Element<Target_Curve>& target_Element : target)
	{
		target_Element.the_Class->fit_Params.create_Expressions_for_Residual();
	}
}

void Calculation_Tree::create_Rand_Generator()
{
	const gsl_rng_type* rng_Type;
	rng_Type = gsl_rng_default;
	r = gsl_rng_alloc (rng_Type);

	auto now = std::chrono::system_clock::now();
	gsl_rng_set(r,std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count());
}

void Calculation_Tree::check_If_Graded()
{
	QTreeWidgetItem* structure_Item;
	QTreeWidgetItemIterator it(real_Struct_Tree);
	while (*it)
	{
		structure_Item = *it;

		Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		if(struct_Data.item_Type == item_Type_Layer && struct_Data.item_Enabled)
		{
			if(struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				if(parent_Data.item_Enabled)
				{
					if(struct_Data.thickness_Drift.is_Drift_Line) depth_Grading = true;
					if(struct_Data.thickness_Drift.is_Drift_Sine) depth_Grading = true;
					if(struct_Data.thickness_Drift.is_Drift_Rand) depth_Grading = true;

					if(struct_Data.sigma_Drift.is_Drift_Line) sigma_Grading = true;
					if(struct_Data.sigma_Drift.is_Drift_Sine) sigma_Grading = true;
					if(struct_Data.sigma_Drift.is_Drift_Rand) sigma_Grading = true;
				}
			}
		}
		if(struct_Data.item_Type == item_Type_Regular_Aperiodic && struct_Data.item_Enabled)
		{
			for(int i=0; i<struct_Data.regular_Components.size(); ++i)
			{
				if(!struct_Data.regular_Components[i].is_Common_Thickness) depth_Grading = true;
				if(!struct_Data.regular_Components[i].is_Common_Sigma)	   sigma_Grading = true;
			}
		}
		++it;
	}
}

void Calculation_Tree::fill_Tree_From_Scratch(tree<Node>& calc_Tree, QTreeWidget* item_Tree, QString curve_Class)
{
	Node empty_Top_Node;
	calc_Tree.insert(calc_Tree.begin(), empty_Top_Node);	// according to the tree.hh API

	fill_Calc_Tree_From_Item_Tree(calc_Tree.begin(), calc_Tree, item_Tree->invisibleRootItem());

	// if no substrate then add it equal to ambient
//	Data last_Struct_Data = item_Tree->topLevelItem(item_Tree->topLevelItemCount()-1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Data last_Struct_Data = calc_Tree.child(calc_Tree.begin(),calc_Tree.begin().number_of_children()-1).node->data.struct_Data;

	if(last_Struct_Data.item_Type != item_Type_Substrate)
	{
		QTreeWidgetItem substrate_Item;

		// the first (0) is the ambient in real tree, (1) is the ambient in independent copy // WARNING !
		int ambient_Index_In_Item_Tree = -1;
		if(curve_Class == INDEPENDENT) ambient_Index_In_Item_Tree = 1;
		if(curve_Class == TARGET)	   ambient_Index_In_Item_Tree = 0;
//		Data substrate = item_Tree->topLevelItem(ambient_Index_In_Item_Tree)->data(DEFAULT_COLUMN, Qt::UserRole).template value<Data>();
		Data substrate = calc_Tree.child(calc_Tree.begin(),ambient_Index_In_Item_Tree).node->data.struct_Data;

		// change id
		substrate.reset_All_IDs();
		substrate.item_Type = item_Type_Substrate;

		// set data
		QVariant var; var.setValue( substrate );
		substrate_Item.setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// create node
		Node substrate_Node(&substrate_Item);

		// add last node
		calc_Tree.append_child(calc_Tree.begin(), substrate_Node);
	}
}

void Calculation_Tree::fill_Calc_Tree_From_Item_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item)
{
	int calc_Counter = 0;
	for(int i=0; i<item->childCount(); ++i)
	{
		Node temp_Node(item->child(i));

		if(temp_Node.struct_Data.item_Enabled)
		{
			calc_Tree.append_child(parent, temp_Node);	// Multilayer and Aperiodic items are also here

			if(item->child(i)->childCount()>0)
			{
				fill_Calc_Tree_From_Item_Tree(calc_Tree.child(parent,calc_Counter), calc_Tree, item->child(i));
			}
			calc_Counter++;
		}
	}
}

void Calculation_Tree::fill_Independent_Calc_Trees()
{
	// for independent
	for(Data_Element<Independent_Variables>& data_Element : independent)
	{
		fill_Tree_From_Scratch(data_Element.calc_Tree, data_Element.the_Class->struct_Tree_Copy, data_Element.curve_Class);
		stratify_Calc_Tree(data_Element.calc_Tree);

		// remove measurement
		data_Element.calc_Tree.erase(data_Element.calc_Tree.child(data_Element.calc_Tree.begin(),0));
	}
}

void Calculation_Tree::fill_Target_Calc_Trees()
{
	// for target
	if(target.size()>0)
	{
		// fill one "common" real_Calc_Tree for the multilayer
		///--------------------------------------------------------------------
		/// here is the entry point for fitting
		fill_Tree_From_Scratch(real_Calc_Tree, real_Struct_Tree, TARGET);
		///--------------------------------------------------------------------

		// replication of real_Calc_Tree for each target
		for(Data_Element<Target_Curve>& data_Element : target)
		{
			data_Element.calc_Tree = real_Calc_Tree;
			stratify_Calc_Tree(data_Element.calc_Tree);
		}
	}
}

void Calculation_Tree::renew_Item_Tree_From_Calc_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item)
{
	int calc_Counter = 0;
	for(int item_Counter=0; item_Counter<item->childCount(); ++item_Counter)
	{
		Data old_Item_Data = item->child(item_Counter)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();;
		if(old_Item_Data.item_Enabled)
		{
			tree<Node>::pre_order_iterator child = tree<Node>::child(parent,calc_Counter);

			// refesh my_Sigma and interlayers
			Data& struct_Data = child.node->data.struct_Data;
			if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
			{
				double temp_Sigma_Square=0;
				double sum = 0;
				for(Interlayer& interlayer : struct_Data.interlayer_Composition)
				{
					if(interlayer.enabled)
					{
						sum += interlayer.interlayer.value;
						temp_Sigma_Square += pow(interlayer.my_Sigma.value,2) * interlayer.interlayer.value;
					}
				}
				if(abs(sum)<DBL_EPSILON) {sum = DBL_EPSILON;	qInfo() << "Calculation_Tree::renew_Item_Tree_From_Calc_Tree :: abs(sum)<DBL_EPSILON" << endl;}

				// equalize sigma
				if(struct_Data.common_Sigma)
				{
					for(Interlayer& interlayer : struct_Data.interlayer_Composition)
					{
						interlayer.my_Sigma.value = struct_Data.sigma.value;
					}
				} else
				{
					struct_Data.sigma.value = sqrt(temp_Sigma_Square/sum);
				}

				// norm weights
				for(Interlayer& interlayer : struct_Data.interlayer_Composition)
				{
					if(interlayer.enabled)
					{
						interlayer.interlayer.value /= sum;
					}
				}
			}

			QVariant var; var.setValue( /*child.node->data.*/struct_Data );
			item->child(item_Counter)->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			if(item->child(item_Counter)->childCount()>0)
			{
				renew_Item_Tree_From_Calc_Tree(calc_Tree.child(parent,calc_Counter), calc_Tree, item->child(item_Counter));
			}
			calc_Counter++;
		}
	}
}

void Calculation_Tree::stratify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Nodes)
{
	// iterate over tree, looking for fixed depth
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);

		if(tree<Node>::depth(child) == depth)
		{
			if( child.node->data.struct_Data.item_Type == item_Type_Multilayer ||
				child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic ||
				child.node->data.struct_Data.item_Type == item_Type_General_Aperiodic )
			{
				chosen_Nodes.append(child);
			}
		}
		if(child.number_of_children()>0)
		{
			stratify_Calc_Tree_Iteration(child, depth, chosen_Nodes);
		}
	}
}

void Calculation_Tree::stratify_Calc_Tree(tree<Node>& calc_Tree)
{
	for(int depth=max_Depth-1; depth>0; depth--)
	{
		QVector<tree<Node>::iterator> chosen_Nodes;

		stratify_Calc_Tree_Iteration(calc_Tree.begin(), depth, chosen_Nodes);

		for(int vec_Index=chosen_Nodes.size()-1; vec_Index>=0; --vec_Index)
		{
			tree<Node>::iterator chosen_Child = chosen_Nodes[vec_Index];

			// if 0 periods
			if(chosen_Child.node->data.struct_Data.num_Repetition.value() == 0)
			{
				// delete
				if(chosen_Child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic)
				{calc_Tree.erase(chosen_Child);} // anyway, not the case of regular aperiodic
			} else

			// if 1 period
			if(chosen_Child.node->data.struct_Data.num_Repetition.value() == 1)
			{
				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					calc_Tree.insert_subtree(chosen_Child, tree<Node>::child(chosen_Child,child_Index));
				}
				// delete
				if(chosen_Child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic)
				{calc_Tree.erase(chosen_Child);}
				// not delete
//				chosen_Child.node->data.struct_Data.num_Repetition.value = 0;
			} else

			/// add before and after
//			// if 2 periods
//			if(chosen_Child.node->data.struct_Data.num_Repetition.value() == 2)
//			{
//				tree<Node>::iterator next = chosen_Child;
//				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
//				{
//					calc_Tree.insert_subtree     (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
//					next = calc_Tree.insert_subtree_after(next, tree<Node>::child(chosen_Child,child_Index));
//				}
//				// delete
//				calc_Tree.erase(chosen_Child);
//				// not delete
////				chosen_Child.node->data.stack_Content.num_Repetition.value = 0;
//			} else

//			// if >=3 periods
//			if(chosen_Child.node->data.struct_Data.num_Repetition.value() >= 3)
//			{
//				tree<Node>::iterator next = chosen_Child;
//				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
//				{
//					calc_Tree.insert_subtree     (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
//					next = calc_Tree.insert_subtree_after(next, tree<Node>::child(chosen_Child,child_Index));
//				}
//				// change data
//				chosen_Child.node->data.struct_Data.num_Repetition.parameter.value -= 2;
//			}

			/// add before only
			// if >=2 periods
			if(chosen_Child.node->data.struct_Data.num_Repetition.value() >= 2)
			{
				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					// save number of periods and drift for drift calculation
					tree<Node>::post_order_iterator layer = tree<Node>::child(chosen_Child,child_Index);
					layer.node->data.struct_Data.num_Repetition.parameter.value  = chosen_Child.node->data.struct_Data.num_Repetition.parameter.value;

					calc_Tree.insert_subtree     (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
				}
				// change data
				if(chosen_Child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic)
				{chosen_Child.node->data.struct_Data.num_Repetition.parameter.value -= 1;}
			}
		}
	}
}


template<typename Type>
void Calculation_Tree::calculate_1_Kind_Preliminary(Data_Element<Type>& data_Element)
{
	data_Element.calc_Functions.check_Reflectance = true;

	// calculation of wavenumbers and cos squares
	if(data_Element.curve_Class == INDEPENDENT)	{data_Element.the_Class->measurement.calc_Independent_cos2_k();} else
	if(data_Element.curve_Class == TARGET)		{data_Element.the_Class->measurement.calc_Measured_cos2_k();}

	// find active node
	if(data_Element.curve_Class == INDEPENDENT)
	{
		Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(data_Element.the_Class);

		// find whats_This of active item
		for(int item_Index=0; item_Index<independent_Variables->independent_Variables_List->count(); ++item_Index)
		{
			QListWidgetItem* list_Item = independent_Variables->independent_Variables_List->item(item_Index);
			Independent_Indicator item_Indicator = list_Item->data(Qt::UserRole).value<Independent_Indicator>();

			// if active
			if(item_Indicator.is_Active)
			{
				data_Element.active_Item_Type			 = item_Indicator.item_Type;
				data_Element.active_Item_Id				 = item_Indicator.item_Id;
				data_Element.active_Parameter_Whats_This = item_Indicator.parameter_Whats_This;
			}

			data_Element.calc_Functions = independent_Variables->calc_Functions;
		}
	} else
	if(data_Element.curve_Class == TARGET)
	{
		Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);

		// only measurement can be active here
		data_Element.active_Item_Type			 = item_Type_Measurement;
		data_Element.active_Item_Id				 = target_Curve->measurement.id;
		data_Element.active_Parameter_Whats_This = target_Curve->curve.argument_Type;

		if( target_Curve->curve.value_Function == specular_Value_Function[Transmittance])
		{
			data_Element.calc_Functions.check_Transmittance = true;
		}
	}

	// calculate instrumental function
	data_Element.the_Class->measurement.calc_Instrumental_Factor(data_Element.active_Parameter_Whats_This);
	data_Element.the_Class->measurement.calc_Mixed_Resolution(data_Element.active_Parameter_Whats_This);
}
template void Calculation_Tree::calculate_1_Kind_Preliminary<Independent_Variables>(Data_Element<Independent_Variables>&);
template void Calculation_Tree::calculate_1_Kind_Preliminary<Target_Curve>		   (Data_Element<Target_Curve>&);

template<typename Type>
void Calculation_Tree::calculate_1_Kind(Data_Element<Type>& data_Element)
{
	// find corresponding node for active variable
//		tree<Node>::iterator active_Iter = find_Node_By_Item_Id(data_Element.calc_Tree.begin(), data_Element.active_Item_Id, data_Element.calc_Tree);
	// ....................................................................
	// emergency case
//		if (active_Iter.node->data.stop_Calculation) { qInfo() << "stop_Calculation" << endl; return; }
	// ....................................................................
	// if measurement is not active, create tree for each plot point
	if(data_Element.active_Item_Type != item_Type_Measurement)
	{
		// TODO
		qInfo() << "Calculation_Tree::calculate_1_Kind  :  Active" << data_Element.active_Item_Type << endl;
	} else
	if(data_Element.active_Item_Type == item_Type_Measurement)
	{
//		auto start = std::chrono::system_clock::now();
		calculate_Intermediate_Values_1_Tree(data_Element.calc_Tree, data_Element.the_Class->measurement, data_Element.active_Parameter_Whats_This, data_Element.calc_Tree.begin());
		if(lambda_Out_Of_Range) return;
//		auto end = std::chrono::system_clock::now();
//		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		qInfo() << "\nIntermediate:   "<< elapsed.count()/1000000. << " seconds" << endl;

//		start = std::chrono::system_clock::now();
		calculate_Unwrapped_Structure		(data_Element.calc_Functions, data_Element.calc_Tree, data_Element.the_Class->measurement, data_Element.active_Parameter_Whats_This, data_Element.unwrapped_Structure);
//		end = std::chrono::system_clock::now();
//		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		qInfo() << "Unwrap:         "<< elapsed.count()/1000000. << " seconds" << endl;

//		start = std::chrono::system_clock::now();
		calculate_Unwrapped_Reflectivity	(data_Element.calc_Functions, data_Element.the_Class->calculated_Values, data_Element.the_Class->measurement, data_Element.active_Parameter_Whats_This, data_Element.unwrapped_Structure, data_Element.unwrapped_Reflection);
//		end = std::chrono::system_clock::now();
//		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		qInfo() << "Unwrap Reflect: "<< elapsed.count()/1000000. << " seconds" << endl;
	}
}
template void Calculation_Tree::calculate_1_Kind<Independent_Variables>(Data_Element<Independent_Variables>&);
template void Calculation_Tree::calculate_1_Kind<Target_Curve>		   (Data_Element<Target_Curve>&);

void Calculation_Tree::calculate_Intermediate_Values_1_Tree(tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, const tree<Node>::iterator& parent, Node* above_Node)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		child.node->data.calculate_Intermediate_Points(measurement, above_Node, active_Parameter_Whats_This, depth_Grading, sigma_Grading, multilayer->discretization_Parameters.enable_Discretization);

		if( child.node->data.struct_Data.item_Type != item_Type_Multilayer &&
			child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic &&
			child.node->data.struct_Data.item_Type != item_Type_General_Aperiodic )
		{
			above_Node = &child.node->data;
		} else
		{
			calculate_Intermediate_Values_1_Tree(calc_Tree, measurement, active_Parameter_Whats_This, child, above_Node);
		}
	}
}

tree<Node>::iterator Calculation_Tree::find_Node_By_Item_Id(const tree<Node>::iterator& parent, id_Type active_Item_Id, tree<Node>& calc_Tree)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		if(child.node->data.struct_Data.id == active_Item_Id)
		{
			return calc_Tree.child(parent,i);
		} else
		if( child.node->data.struct_Data.item_Type == item_Type_Multilayer ||
			child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic ||
			child.node->data.struct_Data.item_Type == item_Type_General_Aperiodic )
		{
			return find_Node_By_Item_Id(child, active_Item_Id, calc_Tree);
		}
	}
	// error handling
	{
		QMessageBox::critical(0, "Error", "Calculation_Tree::find_Node\nerror: node \"" + Locale.toString(active_Item_Id) + "\"not found");
		parent.node->data.stop_Calcuation = true;
	}
	return parent;
}

void Calculation_Tree::calculate_Unwrapped_Structure(const Calc_Functions& calc_Functions, tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure*& unwrapped_Structure_Vec_Element)
{
	delete unwrapped_Structure_Vec_Element;
	num_Media = get_Total_Num_Layers(calc_Tree.begin(), calc_Tree);

	Unwrapped_Structure* new_Unwrapped_Structure  = new Unwrapped_Structure (calc_Functions, calc_Tree, measurement, active_Parameter_Whats_This, num_Media, max_Depth, depth_Grading, sigma_Grading, multilayer->discretization_Parameters, r);
	unwrapped_Structure_Vec_Element = new_Unwrapped_Structure;

	if(unwrapped_Structure_Vec_Element->discretization_Parameters.enable_Discretization)
	{
		num_Media = unwrapped_Structure_Vec_Element->num_Discretized_Media;
	}
}

void Calculation_Tree::calculate_Unwrapped_Reflectivity(const Calc_Functions& calc_Functions, Calculated_Values& calculated_Values, const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure* unwrapped_Structure_Vec_Element, Unwrapped_Reflection*& unwrapped_Reflection_Vec_Element)
{
	delete unwrapped_Reflection_Vec_Element;	
	Unwrapped_Reflection* new_Unwrapped_Reflection = new Unwrapped_Reflection(unwrapped_Structure_Vec_Element, num_Media, active_Parameter_Whats_This, measurement, depth_Grading, sigma_Grading, calc_Functions, calculated_Values, calc_Mode);
	unwrapped_Reflection_Vec_Element = new_Unwrapped_Reflection;

//	auto start = std::chrono::system_clock::now();
	unwrapped_Reflection_Vec_Element->calc_Specular();
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//	qInfo() << "Bare Reflectivity:      "<< elapsed.count()/1000000. << " seconds" << endl;

	// make a copy for plotting
	calculated_Values.R=QVector<double>::fromStdVector(unwrapped_Reflection_Vec_Element->R_Instrumental);
	if(abs(measurement.polarization.value-1)<DBL_EPSILON)	{ calculated_Values.Phi_R = QVector<double>::fromStdVector(unwrapped_Reflection_Vec_Element->Phi_R_s); } else
	if(abs(measurement.polarization.value+1)<DBL_EPSILON)	{ calculated_Values.Phi_R = QVector<double>::fromStdVector(unwrapped_Reflection_Vec_Element->Phi_R_p); } else
															{ calculated_Values.Phi_R.clear(); }
	calculated_Values.T=QVector<double>::fromStdVector(unwrapped_Reflection_Vec_Element->T_Instrumental);
	calculated_Values.A=QVector<double>::fromStdVector(unwrapped_Reflection_Vec_Element->A_Instrumental);
	// TODO user function
//	calculated_Values.=QVector<double>::fromStdVector(unwrapped_Reflection_Vec_Element->A_Instrumental);



//	cout << "r_s     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->r_s[0] << endl;
//	cout << "r_p     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->r_p[0] << endl;

//	cout << "R_s     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->R_s[0] << endl;
//	cout << "R_p     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->R_p[0] << endl;
//	cout << "--------------------------------\n";
}

int Calculation_Tree::get_Total_Num_Layers(const tree<Node>::iterator& parent, const tree<Node>& calc_Tree)
{
	int num_Media_Local = 0;
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		if(child.node->data.struct_Data.item_Type == item_Type_Ambient ||
		   child.node->data.struct_Data.item_Type == item_Type_Layer   ||
		   child.node->data.struct_Data.item_Type == item_Type_Substrate )
		{
			num_Media_Local += 1;
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			num_Media_Local += child.node->data.struct_Data.num_Repetition.value() * get_Total_Num_Layers(child, calc_Tree);
		}		
		if( child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic)
		{
			num_Media_Local += (child.node->data.struct_Data.num_Repetition.value()-1) * get_Total_Num_Layers(child, calc_Tree);
		}
		if(child.node->data.struct_Data.item_Type == item_Type_General_Aperiodic)
		{
			num_Media_Local += get_Total_Num_Layers(child, calc_Tree);
		}
	}
	return num_Media_Local;
}

void Calculation_Tree::print_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);
		{
			std::cout << "node :  depth : " << calc_Tree.depth(child) << "   ";
			for(int y=0; y<calc_Tree.depth(child)-1; y++)
			{	std::cout << "\t";}
			std::cout << child.node->data.struct_Data.item_Type.toStdString();
			if( child.node->data.struct_Data.item_Type == item_Type_Multilayer ||
				child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic ||
				child.node->data.struct_Data.item_Type == item_Type_General_Aperiodic )
			{	std::cout << " : " << child.node->data.struct_Data.num_Repetition.value();	}
			std::cout << std::endl;
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Multilayer ||
			child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic ||
			child.node->data.struct_Data.item_Type == item_Type_General_Aperiodic )
		{
			print_Tree(child, calc_Tree);
		}
	}
}

//void Calculation_Tree::print_Flat_list(QList<Node> flat_List)
//{
//	for(int i=0; i<flat_List.size(); ++i)
//	{
//		std::cout << "list : \t\t" << flat_List[i].struct_Data.item_Type.toStdString() << std::endl;
//	}
//}

//void Calculation_Tree::print_Item_Tree(QTreeWidgetItem* item)
//{
//	for(int i=0; i<item->childCount(); ++i)
//	{
//		QTreeWidgetItem* child = item->child(i);

//		Data struct_Data = child->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

//		{
//			std::cout << "item :  depth : " << Global_Variables::get_Item_Depth(child) << "   ";
//			for(int y=0; y<Global_Variables::get_Item_Depth(child)-1; y++)
//			{	std::cout << "\t";}
//			std::cout << child->whatsThis(DEFAULT_COLUMN).toStdString();
//			if(struct_Data.item_Type == item_Type_Multilayer)
//			{	std::cout << " : " << child->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().num_Repetition.value;	}
//			std::cout << std::endl;
//		}

//		if(struct_Data.item_Type == item_Type_Multilayer ||
//		   struct_Data.item_Type == item_Type_Aperiodic  )
//		{
//			print_Item_Tree(child);
//		}
//	}
//}
