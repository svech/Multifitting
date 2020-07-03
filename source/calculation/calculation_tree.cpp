#include "calculation_tree.h"
#include "multilayer_approach/multilayer/multilayer.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(Multilayer* multilayer, QString calc_Mode):
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

void Calculation_Tree::fill_Tree_From_Scratch(tree<Node>& calc_Tree, QTreeWidget* item_Tree, Multilayer* multilayer)
{
	Node empty_Top_Node;
	calc_Tree.insert(calc_Tree.begin(), empty_Top_Node);	// according to the tree.hh API

	fill_Calc_Tree_From_Item_Tree(calc_Tree.begin(), calc_Tree, item_Tree->invisibleRootItem());

	// if no substrate then add it equal to ambient
	// substrate is always exists but to be sure
	Data last_Struct_Data = calc_Tree.child(calc_Tree.begin(), calc_Tree.begin().number_of_children()-1).node->data.struct_Data;
	if(last_Struct_Data.item_Type != item_Type_Substrate)
	{
		qInfo()  << endl << "Calculation_Tree::fill_Tree_From_Scratch  :  last item is not a substrate!" << endl << endl;
		Data substrate = calc_Tree.child(calc_Tree.begin(), 0).node->data.struct_Data;

		for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
		{
			if(!multilayer->imperfections_Model.use_Func[interlayer_Index] || !multilayer->imperfections_Model.use_Interlayer)
			{
				substrate.interlayer_Composition[interlayer_Index].enabled = false;
			}
		}
		Global_Variables::enable_Disable_Roughness_Model(substrate, multilayer->imperfections_Model);

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
		fill_Tree_From_Scratch(real_Calc_Tree, real_Struct_Tree, multilayer);

		// get common number of media
		num_Media = get_Total_Num_Layers(real_Calc_Tree.begin()); // non-discretized
		num_Media_Sharp = num_Media;							  // store it here anyway
		///--------------------------------------------------------------------

		// replication of real_Calc_Tree for each target and independent
		for(Data_Element<Target_Curve>& target_Data_Element : target)
		{
			target_Data_Element.calc_Tree = real_Calc_Tree;

			// unstratified
			target_Data_Element.media_Data_Map_Vector.resize(num_Media_Sharp);
			target_Data_Element.media_Period_Index_Map_Vector.resize(num_Media_Sharp);
			unwrap_Calc_Tree_Data(real_Calc_Tree.begin(), target_Data_Element.media_Data_Map_Vector, target_Data_Element.media_Period_Index_Map_Vector);

			stratify_Calc_Tree(target_Data_Element.calc_Tree);

			// stratified
			target_Data_Element.media_Node_Map_Vector.resize(num_Media_Sharp);
			flatten_Tree(target_Data_Element.calc_Tree, target_Data_Element.flat_Calc_Tree);
			short_Tree(target_Data_Element.flat_Calc_Tree, target_Data_Element.short_Flat_Calc_Tree);
			unwrap_Calc_Tree_Node(target_Data_Element.calc_Tree.begin(), target_Data_Element.media_Node_Map_Vector);
		}
		for(Data_Element<Independent_Curve>& independent_Data_Element : independent)
		{
			independent_Data_Element.calc_Tree = real_Calc_Tree;

			// unstratified
			independent_Data_Element.media_Data_Map_Vector.resize(num_Media_Sharp);
			independent_Data_Element.media_Period_Index_Map_Vector.resize(num_Media_Sharp);
			unwrap_Calc_Tree_Data(real_Calc_Tree.begin(), independent_Data_Element.media_Data_Map_Vector, independent_Data_Element.media_Period_Index_Map_Vector);

			stratify_Calc_Tree(independent_Data_Element.calc_Tree);

			// stratified
			independent_Data_Element.media_Node_Map_Vector.resize(num_Media_Sharp);
			flatten_Tree(independent_Data_Element.calc_Tree, independent_Data_Element.flat_Calc_Tree);
			short_Tree(independent_Data_Element.flat_Calc_Tree, independent_Data_Element.short_Flat_Calc_Tree);
			unwrap_Calc_Tree_Node(independent_Data_Element.calc_Tree.begin(), independent_Data_Element.media_Node_Map_Vector);
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
	int max_Depth = Global_Variables::get_Tree_Depth(real_Struct_Tree->invisibleRootItem());	// unstratified depth

	for(int depth=max_Depth-1; depth>0; depth--)
	{
		QVector<tree<Node>::iterator> chosen_Nodes;

		stratify_Calc_Tree_Iteration(calc_Tree.begin(), depth, chosen_Nodes);

		for(int vec_Index=chosen_Nodes.size()-1; vec_Index>=0; --vec_Index)
		{
			tree<Node>::iterator chosen_Child = chosen_Nodes[vec_Index];

			// if 0 periods ( Multilayers only )
			if(chosen_Child.node->data.struct_Data.num_Repetition.value() == 0)
			{
				// delete
				{calc_Tree.erase(chosen_Child);} // anyway, not the case of regular aperiodic
			} else

			// if 1 period ( Regular Aperiodic, Multilayer, General Aperiodic )
			if(chosen_Child.node->data.struct_Data.num_Repetition.value() == 1)
			{
				if(chosen_Child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic)
				{
					for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
					{
						calc_Tree.insert_subtree(chosen_Child, tree<Node>::child(chosen_Child,child_Index));
					}
					// delete
					{calc_Tree.erase(chosen_Child);}
					// not delete
//					chosen_Child.node->data.struct_Data.num_Repetition.value = 0;
				}
			} else

			// if >=2 periods ( Regular Aperiodic and Multilayer )
			if(chosen_Child.node->data.struct_Data.num_Repetition.value() >= 2)
			{
				if(chosen_Child.node->data.struct_Data.item_Type != item_Type_Regular_Aperiodic)
				{
					for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
					{
						// save number of periods and drift for drift calculation
						tree<Node>::post_order_iterator layer = tree<Node>::child(chosen_Child,child_Index);
						Data& layer_Data = layer.node->data.struct_Data;
						if(layer_Data.item_Type == item_Type_Layer)
						{
							layer_Data.num_Repetition.parameter.value = chosen_Child.node->data.struct_Data.num_Repetition.parameter.value;
						}
					}
					// change data
					{chosen_Child.node->data.struct_Data.num_Repetition.parameter.value -= 1;}
				}

				// both Regular and Multilayer
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

void Calculation_Tree::flatten_Tree(const tree<Node>& calc_Tree, vector<Node*>& flat_Calc_Tree)
{
	tree<Node>::pre_order_iterator iter = tree<Node>::child(calc_Tree.begin(),0);

	flat_Calc_Tree.clear();
	while(calc_Tree.is_valid(iter))
	{
		Node& node = iter.node->data;
		const Data& item = node.struct_Data;

		if(item.item_Type == item_Type_Ambient || item.item_Type == item_Type_Layer || item.item_Type == item_Type_Substrate)
		{
			flat_Calc_Tree.push_back(&node);
		}
		iter++;
	}
}

void Calculation_Tree::short_Tree(const vector<Node*>& flat_Calc_Tree, vector<Node*>& short_Flat_Calc_Tree)
{
	for(int i=flat_Calc_Tree.size()-1; i>=1; i--)
	{
		Node* node = flat_Calc_Tree[i];

		// look for existing nodes
		bool contains = false;
		for(size_t k=0; k<short_Flat_Calc_Tree.size(); k++)
		{
			if(short_Flat_Calc_Tree[k]->struct_Data.id == node->struct_Data.id) contains = true;
		}
		if(!contains)
		{
			short_Flat_Calc_Tree.insert(short_Flat_Calc_Tree.begin(), node);
		}
	}
}

int Calculation_Tree::unwrap_Calc_Tree_Node(const tree<Node>::iterator& parent, vector<Node*>& media_Node_Map_Vector, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		Node& child_Node = child.node->data;
		Data& child_Data = child_Node.struct_Data;

		if(child_Data.item_Type == item_Type_Ambient ||
		   child_Data.item_Type == item_Type_Layer   ||
		   child_Data.item_Type == item_Type_Substrate )
		{
			media_Node_Map_Vector[media_Index] = &child_Node;
			++media_Index;
		}
		// for regular aperiodic and its children
		if( child_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			for(int period_Index=0; period_Index<child_Data.num_Repetition.value()-1; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					Node& grandchild_Node = grandchild.node->data;
					media_Node_Map_Vector[media_Index] = &grandchild_Node;
					++media_Index;
				}
			}
		}
		// go deeper
		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value(); ++period_Index)
			{
				media_Index = unwrap_Calc_Tree_Node(child, media_Node_Map_Vector, media_Index);
			}
		}
	}
	return media_Index;
}

int Calculation_Tree::unwrap_Calc_Tree_Data(const tree<Node>::iterator& parent, vector<Data*>& media_Data_Map_Vector, vector<int>& media_Period_Index_Map_Vector, int media_Index, int period_Index)
{
	const Data& parent_Data = parent.node->data.struct_Data;
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		Node& child_Node = child.node->data;
		Data& child_Data = child_Node.struct_Data;

		if(child_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			if(child_Data.item_Type == item_Type_Ambient ||
			   child_Data.item_Type == item_Type_Layer   ||
			   child_Data.item_Type == item_Type_Substrate )
			{
				media_Data_Map_Vector[media_Index] = &child_Data;
				// for drifts
				if(child_Data.parent_Item_Type == item_Type_Multilayer )
				{
					media_Period_Index_Map_Vector[media_Index] = period_Index;
					media_Data_Map_Vector[media_Index]->num_Repetition.parameter.value = parent_Data.num_Repetition.value();
				}
				++media_Index;
			}
		}
		// for regular aperiodic and its children
		if(child_Data.item_Type == item_Type_Regular_Aperiodic)
		{
			for(int period_Index=0; period_Index<child_Data.num_Repetition.value(); ++period_Index)
			{
				for(int cell_Index=0; cell_Index<child_Data.regular_Components.size(); ++cell_Index)
				{
					media_Data_Map_Vector[media_Index] = &(child_Data.regular_Components[cell_Index].components[period_Index]);
					++media_Index;
				}
			}
		}
		// go deeper
		if(child_Data.item_Type == item_Type_Multilayer)
		{
			for(int period_Index=0; period_Index<child_Data.num_Repetition.value(); ++period_Index)
			{
				media_Index = unwrap_Calc_Tree_Data(child, media_Data_Map_Vector, media_Period_Index_Map_Vector, media_Index, period_Index);
			}
		}
	}
	return media_Index;
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

	// calculate instrumental function
	data_Element.the_Class->measurement.calc_Instrumental_Factor();
	data_Element.the_Class->measurement.calc_Mixed_Resolution();
}
template void Calculation_Tree::calculate_1_Kind_Preliminary<Independent_Curve>(Data_Element<Independent_Curve>&);
template void Calculation_Tree::calculate_1_Kind_Preliminary<Target_Curve>		   (Data_Element<Target_Curve>&);

template<typename Type>
void Calculation_Tree::calculate_1_Kind(Data_Element<Type>& data_Element, QString mode)
{
	auto start = std::chrono::system_clock::now();
	calculate_Intermediate_Values_1_Tree(data_Element.flat_Calc_Tree, data_Element.short_Flat_Calc_Tree, data_Element.the_Class->measurement, mode);
	if(lambda_Out_Of_Range) return;
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "\nIntermediate:   "<< elapsed.count()/1000000. << " seconds" << endl;

	start = std::chrono::system_clock::now();
	calculate_Unwrapped_Structure(data_Element.calc_Functions, data_Element.media_Node_Map_Vector, data_Element.media_Data_Map_Vector, data_Element.media_Period_Index_Map_Vector, data_Element.the_Class->measurement, data_Element.unwrapped_Structure);
	end = std::chrono::system_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "Unwrap:         "<< elapsed.count()/1000000. << " seconds" << endl;

	start = std::chrono::system_clock::now();
	calculate_Unwrapped_Reflectivity(data_Element.short_Flat_Calc_Tree, data_Element.the_Class->calculated_Values, data_Element.unwrapped_Structure, data_Element.unwrapped_Reflection, mode);
	end = std::chrono::system_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "Unwrap Reflect: "<< elapsed.count()/1000000. << " seconds" << endl;

	clear_Spline_1_Tree(data_Element.short_Flat_Calc_Tree, mode);
}
template void Calculation_Tree::calculate_1_Kind<Independent_Curve>(Data_Element<Independent_Curve>&, QString);
template void Calculation_Tree::calculate_1_Kind<Target_Curve>	   (Data_Element<Target_Curve>&, QString);

void Calculation_Tree::calculate_Intermediate_Values_1_Tree(vector<Node*>& flat_Calc_Tree, vector<Node*>& short_Flat_Calc_Tree, const Data& measurement, QString mode)
{
	for(size_t node_Index = 0; node_Index<flat_Calc_Tree.size(); node_Index++)
	{
		Node* above_Node = NULL;
		if(node_Index>=1) above_Node = flat_Calc_Tree[node_Index-1];

		bool inconvenient_Approximation = (multilayer->imperfections_Model.approximation == DWBA_approximation ||
										   multilayer->imperfections_Model.approximation == SA_approximation   ||
										   multilayer->imperfections_Model.approximation == CSA_approximation  ) && (measurement.measurement_Type != measurement_Types[Specular_Scan]);
		flat_Calc_Tree[node_Index]->calculate_Intermediate_Points(measurement, above_Node, depth_Grading, sigma_Grading, inconvenient_Approximation, multilayer->discretization_Parameters.enable_Discretization, mode);
	}
	for(size_t node_Index = 0; node_Index<short_Flat_Calc_Tree.size(); node_Index++)
	{
		if( mode == SCATTERED_MODE )
		{
			short_Flat_Calc_Tree[node_Index]->create_Spline_PSD_Fractal_Gauss_1D(measurement, multilayer->imperfections_Model);
		}
	}
	for(size_t node_Index = 0; node_Index<short_Flat_Calc_Tree.size(); node_Index++)
	{
		if( measurement.measurement_Type == measurement_Types[Specular_Scan])
		{
			// here we create DW sigmas
			short_Flat_Calc_Tree[node_Index]->calc_Debye_Waller_Sigma(measurement, multilayer->imperfections_Model);
		}
	}
}
void Calculation_Tree::clear_Spline_1_Tree(vector<Node*>& short_Flat_Calc_Tree, QString mode)
{
	for(size_t node_Index = 0; node_Index<short_Flat_Calc_Tree.size(); node_Index++)
	{
		if( mode == SCATTERED_MODE )
		{
			short_Flat_Calc_Tree[node_Index]->clear_Spline_PSD_Fractal_Gauss(multilayer->imperfections_Model);
		}
	}
}

void Calculation_Tree::calculate_Unwrapped_Structure(const Calc_Functions& calc_Functions,
													 const vector<Node*>& media_Node_Map_Vector,
													 const vector<Data*>& media_Data_Map_Vector,
													 const vector<int>& media_Period_Index_Map_Vector,
													 const Data& measurement,
													 Unwrapped_Structure*& unwrapped_Structure_Vec_Element)
{
	delete unwrapped_Structure_Vec_Element;
	Unwrapped_Structure* new_Unwrapped_Structure = new Unwrapped_Structure (multilayer,
																			calc_Functions,
																			media_Node_Map_Vector,
																			media_Data_Map_Vector,
																			media_Period_Index_Map_Vector,
																			measurement,
																			num_Media_Sharp,
																			depth_Grading,
																			sigma_Grading,
																			r);
	unwrapped_Structure_Vec_Element = new_Unwrapped_Structure;
}

void Calculation_Tree::calculate_Unwrapped_Reflectivity(const vector<Node*>& short_Flat_Calc_Tree,
														Calculated_Values& calculated_Values,
														Unwrapped_Structure* unwrapped_Structure,
														Unwrapped_Reflection*& unwrapped_Reflection_Vec_Element,
														QString mode)
{
	delete unwrapped_Reflection_Vec_Element;

//	auto start = std::chrono::system_clock::now();
	Unwrapped_Reflection* new_Unwrapped_Reflection = new Unwrapped_Reflection(short_Flat_Calc_Tree, calculated_Values, unwrapped_Structure, mode);
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//	qInfo() << "new_Unwrapped_Reflection    :      "<< elapsed.count()/1000000. << " seconds" << endl;

	unwrapped_Reflection_Vec_Element = new_Unwrapped_Reflection;

//	auto start1 = std::chrono::system_clock::now();
	unwrapped_Reflection_Vec_Element->calc_Specular();
//	auto end1 = std::chrono::system_clock::now();
//	auto elapsed1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
//	qInfo() << "calc_Specular    :      "<< elapsed1.count()/1000000. << " seconds" << endl;
}

int Calculation_Tree::get_Total_Num_Layers(const tree<Node>::iterator& parent)
{
	int num_Media_Local = 0;
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,i);

		if(child.node->data.struct_Data.item_Type == item_Type_Ambient ||
		   child.node->data.struct_Data.item_Type == item_Type_Layer   ||
		   child.node->data.struct_Data.item_Type == item_Type_Substrate )
		{
			num_Media_Local += 1;
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			num_Media_Local += child.node->data.struct_Data.num_Repetition.value() * get_Total_Num_Layers(child);
		}		
		if( child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic)
		{
			// previous and wrong: num_Media_Local += (child.node->data.struct_Data.num_Repetition.value()-1) * get_Total_Num_Layers(child);
			num_Media_Local += (child.node->data.struct_Data.num_Repetition.value()) * get_Total_Num_Layers(child);
		}
		if(child.node->data.struct_Data.item_Type == item_Type_General_Aperiodic)
		{
			num_Media_Local += get_Total_Num_Layers(child);
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
