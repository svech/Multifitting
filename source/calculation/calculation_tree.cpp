#include "calculation_tree.h"
#include "multilayer_approach/multilayer/multilayer.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(Multilayer* multilayer, QString calc_Mode):
	calc_Mode(calc_Mode),
	multilayer(multilayer),
	real_Struct_Tree(multilayer->structure_Tree->tree)
{	
	// initialization independent vector
	if(calc_Mode==CALCULATION) // for single calculation only
	{
		if(multilayer->enable_Calc_Independent_Curves)
		{
			for(int i=0; i<multilayer->independent_Curve_Tabs->count(); ++i)
			{
				Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(i));
				if( independent_Curve->measurement.measurement_Type != no_Measurement_Type)
				if( independent_Curve->calc_Functions.check_Enabled	)
				if(	independent_Curve->calc_Functions.if_Something_Enabled_1D() ||
					independent_Curve->calc_Functions.if_Something_Enabled_2D()	)
				{
					independent.resize(independent.size()+1);

					independent.last().the_Class = independent_Curve;
					independent.last().curve_Class = INDEPENDENT;
				}
			}
		}
	}

	// initialization target vector
	// for single calculation and fitting
	{
		if( multilayer->enable_Calc_Target_Curves )
		{
			for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
			{
				if(target_Curve->loaded_And_Ready)
				if(target_Curve->fit_Params.calculate)
				if(calc_Mode==CALCULATION || target_Curve->fit_Params.fit)	// check fit value only if FITTING mode (otherwise CALCULATION is enough)
				{
					target.resize(target.size()+1);

					target.last().the_Class = target_Curve;
					target.last().curve_Class = TARGET;
				}
			}
		}
	}

	prepare_Residual_Expressions();
	create_Rand_Generator();
	check_If_Graded();
	max_Depth = Global_Variables::get_Tree_Depth(real_Struct_Tree->invisibleRootItem());	// unstratified depth
	depth_Threshold = 3;
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

					if(struct_Data.sigma_Diffuse_Drift.is_Drift_Line) sigma_Grading = true;
					if(struct_Data.sigma_Diffuse_Drift.is_Drift_Sine) sigma_Grading = true;
					if(struct_Data.sigma_Diffuse_Drift.is_Drift_Rand) sigma_Grading = true;
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

void Calculation_Tree::fill_Tree_From_Scratch(tree<Node>& calc_Tree, QTreeWidget* item_Tree)
{
	Node empty_Top_Node;
	calc_Tree.insert(calc_Tree.begin(), empty_Top_Node);	// according to the tree.hh API

	fill_Calc_Tree_From_Item_Tree(calc_Tree.begin(), calc_Tree, item_Tree->invisibleRootItem());

	// if no substrate then add it equal to ambient
	Data last_Struct_Data = calc_Tree.child(calc_Tree.begin(), calc_Tree.begin().number_of_children()-1).node->data.struct_Data;
	if(last_Struct_Data.item_Type != item_Type_Substrate)
	{
		Data substrate = calc_Tree.child(calc_Tree.begin(), 0).node->data.struct_Data;

		for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
		{
			if(!multilayer->imperfections_Model.use_Func[interlayer_Index] || !multilayer->imperfections_Model.use_Interlayer)
			{
				substrate.interlayer_Composition[interlayer_Index].enabled = false;
			}
		}
		substrate.roughness_Model.is_Enabled = multilayer->imperfections_Model.use_Roughness;
		substrate.roughness_Model.model = multilayer->imperfections_Model.common_Model;

		// change id
		substrate.reset_All_IDs();
		substrate.item_Type = item_Type_Substrate;

		// create Item and make Node from it
		QTreeWidgetItem substrate_Item;
		QVariant var; var.setValue( substrate );
		substrate_Item.setData(DEFAULT_COLUMN, Qt::UserRole, var);
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

void Calculation_Tree::fill_Calc_Trees()
{
	if(target.size()+independent.size()>0)
	{
		// fill one "common" real_Calc_Tree for the multilayer
		///--------------------------------------------------------------------
		/// here is the entry point for fitting
		fill_Tree_From_Scratch(real_Calc_Tree, real_Struct_Tree);
		///--------------------------------------------------------------------

		// replication of real_Calc_Tree for each target and independent
		for(Data_Element<Target_Curve>& target_Data_Element : target)
		{
			target_Data_Element.calc_Tree = real_Calc_Tree;
			stratify_Calc_Tree(target_Data_Element.calc_Tree);
		}		
		for(Data_Element<Independent_Curve>& independent_Data_Element : independent)
		{
			independent_Data_Element.calc_Tree = real_Calc_Tree;
			stratify_Calc_Tree(independent_Data_Element.calc_Tree);
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
						temp_Sigma_Square += pow(interlayer.my_Sigma_Diffuse.value,2) * interlayer.interlayer.value;
					}
				}
				if(abs(sum)<DBL_EPSILON) {sum = DBL_EPSILON;	qInfo() << "Calculation_Tree::renew_Item_Tree_From_Calc_Tree :: abs(sum)<DBL_EPSILON" << endl;}

				// equalize sigma
				if(struct_Data.common_Sigma_Diffuse)
				{
					for(Interlayer& interlayer : struct_Data.interlayer_Composition)
					{
						interlayer.my_Sigma_Diffuse.value = struct_Data.sigma_Diffuse.value;
					}
				} else
				{
					struct_Data.sigma_Diffuse.value = sqrt(temp_Sigma_Square/sum);
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

			// if >=2 periods
			if(chosen_Child.node->data.struct_Data.num_Repetition.value() >= 2)
			{
				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					// save number of periods and drift for drift calculation
					tree<Node>::post_order_iterator layer = tree<Node>::child(chosen_Child,child_Index);
					if(layer.node->data.struct_Data.item_Type == item_Type_Layer)
					{
						layer.node->data.struct_Data.num_Repetition.parameter.value = chosen_Child.node->data.struct_Data.num_Repetition.parameter.value;
					}
				}
				// change data
				if(chosen_Child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic)
				{chosen_Child.node->data.struct_Data.num_Repetition.parameter.value -= 1;}

				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					calc_Tree.insert_subtree(chosen_Child, tree<Node>::child(chosen_Child,child_Index));
				}
			}
		}
	}
//	print_Tree(calc_Tree.begin(), calc_Tree);
//	qInfo() << endl << endl;
}


template<typename Type>
void Calculation_Tree::calculate_1_Kind_Preliminary(Data_Element<Type>& data_Element)
{
	if(data_Element.curve_Class == INDEPENDENT)
	{
		Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(data_Element.the_Class);
		independent_Curve->calc_Independent_cos2_k();
		data_Element.calc_Functions = independent_Curve->calc_Functions;
	}
	if(data_Element.curve_Class == TARGET)
	{
		Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);
		target_Curve->calc_Measured_cos2_k();

		if( target_Curve->curve.value_Type == value_Types[Reflectance]   )	{ data_Element.calc_Functions.check_Reflectance = true; }
		if( target_Curve->curve.value_Type == value_Types[Transmittance] )	{ data_Element.calc_Functions.check_Transmittance = true; }
		if( target_Curve->curve.value_Type == value_Types[Scattering]   )	{ data_Element.calc_Functions.check_Scattering = true; }
		if( target_Curve->curve.value_Type == value_Types[GISAS]   )		{ data_Element.calc_Functions.check_GISAS = true; }
	}

	// TODO
	// calculate instrumental function
	data_Element.the_Class->measurement.calc_Instrumental_Factor();
	data_Element.the_Class->measurement.calc_Mixed_Resolution();
}
template void Calculation_Tree::calculate_1_Kind_Preliminary<Independent_Curve>(Data_Element<Independent_Curve>&);
template void Calculation_Tree::calculate_1_Kind_Preliminary<Target_Curve>		   (Data_Element<Target_Curve>&);

template<typename Type>
void Calculation_Tree::calculate_1_Kind(Data_Element<Type>& data_Element)
{
	auto start = std::chrono::system_clock::now();
	calculate_Intermediate_Values_1_Tree(data_Element.calc_Tree, data_Element.the_Class->measurement, data_Element.calc_Tree.begin());
	if(lambda_Out_Of_Range) return;
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "\nIntermediate:   "<< elapsed.count()/1000000. << " seconds" << endl;

	start = std::chrono::system_clock::now();
	calculate_Unwrapped_Structure		(data_Element.calc_Functions, data_Element.calc_Tree, data_Element.the_Class->measurement, data_Element.unwrapped_Structure);
	end = std::chrono::system_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "Unwrap:         "<< elapsed.count()/1000000. << " seconds" << endl;

	start = std::chrono::system_clock::now();
	calculate_Unwrapped_Reflectivity	(data_Element.calc_Functions, data_Element.the_Class->calculated_Values, data_Element.the_Class->measurement, data_Element.unwrapped_Structure, data_Element.unwrapped_Reflection);
	end = std::chrono::system_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "Unwrap Reflect: "<< elapsed.count()/1000000. << " seconds" << endl;
}
template void Calculation_Tree::calculate_1_Kind<Independent_Curve>(Data_Element<Independent_Curve>&);
template void Calculation_Tree::calculate_1_Kind<Target_Curve>	   (Data_Element<Target_Curve>&);

void Calculation_Tree::calculate_Intermediate_Values_1_Tree(tree<Node>& calc_Tree, const Data& measurement, const tree<Node>::iterator& parent, Node* above_Node)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		child.node->data.calculate_Intermediate_Points(measurement, above_Node, depth_Grading, sigma_Grading, multilayer->discretization_Parameters.enable_Discretization);

		if( child.node->data.struct_Data.item_Type != item_Type_Multilayer &&
			child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic &&
			child.node->data.struct_Data.item_Type != item_Type_General_Aperiodic )
		{
			above_Node = &child.node->data;
		} else
		{
			calculate_Intermediate_Values_1_Tree(calc_Tree, measurement, child, above_Node);
		}
	}
}

void Calculation_Tree::calculate_Unwrapped_Structure(const Calc_Functions& calc_Functions, tree<Node>& calc_Tree, const Data& measurement, Unwrapped_Structure*& unwrapped_Structure_Vec_Element)
{
	delete unwrapped_Structure_Vec_Element;
	num_Media = get_Total_Num_Layers(calc_Tree.begin(), calc_Tree);

	Unwrapped_Structure* new_Unwrapped_Structure = new Unwrapped_Structure (calc_Functions, calc_Tree, measurement, num_Media, max_Depth, depth_Threshold, depth_Grading, sigma_Grading, multilayer->discretization_Parameters, r);
	unwrapped_Structure_Vec_Element = new_Unwrapped_Structure;

	if(unwrapped_Structure_Vec_Element->discretization_Parameters.enable_Discretization)
	{
		num_Media = unwrapped_Structure_Vec_Element->num_Discretized_Media;
	}
}

void Calculation_Tree::calculate_Unwrapped_Reflectivity(const Calc_Functions& calc_Functions, Calculated_Values& calculated_Values, const Data& measurement, Unwrapped_Structure* unwrapped_Structure_Vec_Element, Unwrapped_Reflection*& unwrapped_Reflection_Vec_Element)
{
	delete unwrapped_Reflection_Vec_Element;	
	Unwrapped_Reflection* new_Unwrapped_Reflection = new Unwrapped_Reflection(unwrapped_Structure_Vec_Element, num_Media, measurement, depth_Grading, sigma_Grading, calc_Functions, calculated_Values, calc_Mode);
	unwrapped_Reflection_Vec_Element = new_Unwrapped_Reflection;

//	auto start = std::chrono::system_clock::now();
	unwrapped_Reflection_Vec_Element->calc_Specular();
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//	qInfo() << "Bare Reflectivity:      "<< elapsed.count()/1000000. << " seconds" << endl;
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
