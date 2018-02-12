// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "global_variables.h"

// -----------------------------------------------------------------------------------------

// tabulated chemical elements
QStringList		element_Name {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th","Pa","U"};
QVector<double> element_Mass {1.00794,
							 4.002602,
							 6.941,
							 9.012182,
							 10.811,
							 12.0107,
							 14.0067,
							 15.9994,
							 18.9984032,
							 20.1791,
							 22.98976928,
							 24.3050,
							 26.9815386,
							 28.0855,
							 30.973762,
							 32.065,
							 35.453,
							 39.948,
							 39.0983,
							 40.078,
							 44.955912,
							 47.867,
							 50.9415,
							 51.9961,
							 54.938045,
							 55.845,
							 58.933195,
							 58.6934,
							 63.546,
							 65.38,
							 69.723,
							 72.63,
							 74.92160,
							 78.96,
							 79.904,
							 83.798,
							 85.4678,
							 87.62,
							 88.90585,
							 91.224,
							 92.90638,
							 95.96,
							 98.,
							 101.07,
							 102.90550,
							 106.42,
							 107.8682,
							 112.411,
							 114.818,
							 118.710,
							 121.760,
							 127.60,
							 126.90447,
							 131.293,
							 132.9054519,
							 137.327,
							 138.90547,
							 140.116,
							 140.90765,
							 144.242,
							 145.,
							 150.36,
							 151.964,
							 157.25,
							 158.92535,
							 162.500,
							 164.93032,
							 167.259,
							 168.93421,
							 173.054,
							 174.9668,
							 178.49,
							 180.94788,
							 183.84,
							 186.207,
							 190.23,
							 192.217,
							 195.084,
							 196.966569,
							 200.59,
							 204.3833,
							 207.2,
							 208.98040,
							 209.,
							 210.,
							 222.,
							 223.,
							 226.,
							 227.,
							 232.03806,
							 231.03586,
							 238.02891};
QMap<QString, double> sorted_Elements;

// lists of types
QStringList transition_Layer_Functions	{"erf","lin","exp","tanh","sin","step"}; int transition_Layer_Functions_Size = int(transition_Layer_Functions.size());
QStringList tril						{TRIL_TRUE, TRIL_FALSE, TRIL_NOT_DEFINED};

// measured curves
QStringList argument_Types				{"Grazing angle","Incident angle","Wavelength/Energy"}; // change enum!
QStringList value_Function				{"Reflectance","Transmittance","Absorptance"};			// change enum!
QStringList value_R_Mode				{"R","R+"+Phi_Sym,"r, Re+Im","r, |r|+"+Phi_Sym};		// change enum!
QStringList value_T_Mode				{"T"};													// change enum!
QStringList value_A_Mode				{"A"};													// change enum!

// optimization methods
QString GSL_Group = "GSL library";
QStringList GSL_Methods				{"Levenberg-Marquardt",
									 "Levenberg-Marquardt with Geodesic Acceleration",
									 "Dogleg",
									 "Double Dogleg",
									 "Two Dimensional Subspace",

									 "Fletcher-Reeves Conjugate Gradient",
									 "Polak-Ribiere Conjugate Gradient",
									 "Broyden-Fletcher-Goldfarb-Shanno : BFGS",
									 "Broyden-Fletcher-Goldfarb-Shanno : BFGS-2",
									 "Steepest Descent",

									 "Nelder-Mead Simplex",
									 "Nelder-Mead Simplex-2",
									 "Nelder-Mead Simplex-2 Randomized"
									}; // change enum!

// GSL solvers
QStringList GSL_Solvers			{"QR decomposition",
								 "Cholesky decomposition",
								 "Singular value decomposition"
								}; // change enum!

// GSL scaling
QStringList GSL_Scales			{"More",
								 "Levenberg",
								 "Marquardt"
								}; // change enum!

// GSL finite differences
QStringList GSL_Fdtype			{"Forward",
								 "Central"
								}; // change enum!

// -----------------------------------------------------------------------------------------

// units

// wavelength
QStringList   wavelength_Units_List				{Angstrom_Sym,"nm",Mu_Sym + "m","eV","keV"};	// change enum!
QList<double> wavelength_Coefficients_List		{1.           ,1E1,1E4         ,1.  ,1E3  };
QMap<QString, double>  wavelength_Coefficients_Map;

// length
QStringList   length_Units_List					{Angstrom_Sym,"nm",Mu_Sym + "m"};				// change enum!
QList<double> length_Coefficients_List			{1.           ,1E1,1E4         };
QMap<QString, double>  length_Coefficients_Map;

// angle
QStringList   angle_Units_List				{Degree_Sym,Prime_Sym   ,Double_Prime_Sym," rad"   ," mrad"  }; // change enum!
QStringList   angle_Units_Legend_List		{"degree"  ,"arc minute","arc second"    ,"radian" ,"mrad"   };
QList<double> angle_Coefficients_List		{1.		   ,1./60       ,1./3600         ,180./M_PI,0.18/M_PI};
QMap<QString, double>  angle_Coefficients_Map;
QMap<QString, QString> angle_Units_Legend_Map;

// density
QStringList density_Units_List				{"g/cm" + Cube_Sym};

// optical constants
QStringList opt_Const_Units_List			{"% of nominal"};

// drift
QStringList drift_Units_List				{"% per period"};

// -----------------------------------------------------------------------------------------

Optical_Constants* optical_Constants;

Global_Variables::Global_Variables()
{

}

void Global_Variables::read_Optical_Constants()
{
	optical_Constants = new Optical_Constants;
}

void Global_Variables::create_Sorted_Elements_Map()
{
	// map of sorted chemical elements
	for(int i=0; i<element_Name.size(); ++i)
		sorted_Elements.insert(element_Name[i],element_Mass[i]);
}

void Global_Variables::serialize_Tree(QDataStream &out, QTreeWidget* tree)
{
	// save tree id
	out << qvariant_cast<id_Type>(tree->property(id_Property));

	// save number of items in tree
	int num_Items=0;
	QTreeWidgetItemIterator it0(tree);
	while(*it0)
	{
		++it0;
		++num_Items;
	}
	out << num_Items;

	// save tree
	QTreeWidgetItemIterator it(tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;

		// save data
		out << item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		// save parent's id as string
		if(item->parent())
			out << QString::number(item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().id);
		else
			out << QString(NOPARENT);
		++it;
	}
}

void Global_Variables::deserialize_Tree(QDataStream& in, QTreeWidget* tree)
{
	// clear tree
	tree->clear();

	// load tree id
	id_Type tree_Id = 0;
	in >> tree_Id;
	tree->setProperty(id_Property, tree_Id);

	// load number of items in tree
	int num_Items=0;
	in >> num_Items;

	// load tree
	QVector<QTreeWidgetItem*> item_Vec;
	QStringList id_Parent_List;
	QMap<QString, QTreeWidgetItem*> tree_Map;
	tree_Map.insert(NOPARENT, tree->invisibleRootItem());
	for(int i=0; i<num_Items; ++i)
	{
		// create item
		QTreeWidgetItem* item = new QTreeWidgetItem;

		// load data
		QVariant var;
		Data data;
		in >> data;
		var.setValue(data);
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// load parent's id
		QString id_Parent;
		in >> id_Parent;

		tree_Map.insert(QString::number(data.id), item);
		item_Vec.append(item);
		id_Parent_List.append(id_Parent);
	}

	// set hierarchy
	for(int i=0; i<item_Vec.size(); ++i)
	{
		tree_Map.value(id_Parent_List[i])->addChild(item_Vec[i]);
	}

	tree->expandAll();
}

void Global_Variables::serialize_Variables_List(QDataStream& out, QListWidget* list)
{
	// save number of items
	out << list->count();

	for(int i=0; i<list->count(); ++i)
	{
		QListWidgetItem* temp_Item = list->item(i);

		// save text
		out << temp_Item->text();

		// save data
		out << temp_Item->data(Qt::UserRole).value<Independent_Indicator>();
	}
}

void Global_Variables::deserialize_Variables_List(QDataStream& in, QListWidget* list)
{
	list->clear();

	// load number of items
	int num_Items;
	in >> num_Items;

	for(int i=0; i<num_Items; ++i)
	{
		QListWidgetItem* new_Item = new QListWidgetItem;
		list->addItem(new_Item);

		// load text
		QString text;
		in >> text;
		new_Item->setText(text);

		// load data
		Independent_Indicator indicator;
		in >> indicator;
		QVariant var;
		var.setValue(indicator);
		new_Item->setData(Qt::UserRole,var);
	}
}

double Global_Variables::wavelength_Energy(QString wavelength_Units, double y)
{
	double value = -1000;
	for(int i=0; i<wavelength_Units_List.size(); ++i)
	{
		if(wavelength_Units == wavelength_Units_List[i])
		{
			if( 0<=i && i<3)                            value = y;
			if( 3<=i && i<wavelength_Units_List.size()) value = angstrom_eV(y);
		}
	}
	return value;
}

QString Global_Variables::wavelength_Energy_Name(QString wavelength_Units)
{
	QString value = "ERROR";
	for(int i=0; i<wavelength_Units_List.size(); ++i)
	{
		if(wavelength_Units == wavelength_Units_List[i])
		{
			if( 0<=i && i<3) value="Wavelength, " + Lambda_Sym;
			if( 3<=i && i<wavelength_Units_List.size()) value="Energy, E";
		}
	}
	return value;
}

double Global_Variables::angstrom_eV(double x)
{
	return 12398.41930092394328/x;
}

void Global_Variables::fill_Units_Maps()
{
	// wavelength
	for(int i=0; i<wavelength_Units_List.size(); i++)
	{
		wavelength_Coefficients_Map.insert(wavelength_Units_List[i], wavelength_Coefficients_List[i]);
	}
	// length
	for(int i=0; i<length_Units_List.size(); i++)
	{
		length_Coefficients_Map.insert(length_Units_List[i], length_Coefficients_List[i]);
	}
	// angle
	for(int i=0; i<angle_Units_List.size(); i++)
	{
		angle_Coefficients_Map.insert(angle_Units_List[i], angle_Coefficients_List[i]);
		angle_Units_Legend_Map.insert(angle_Units_List[i], angle_Units_Legend_List[i]);
	}
}

int Global_Variables::get_Item_Depth(QTreeWidgetItem* item)
{
	int depth = 0;
	while(item!=NULL)
	{
	  item = item->parent();
	  ++depth;
	}
	return depth;
}

int Global_Variables::get_Number_Of_Items(QTreeWidget* tree)
{
	int counter = 0;
	QTreeWidgetItemIterator it(tree);
	while (*it)
	{
		++counter;
		++it;
	}
	return counter;
}

int Global_Variables::get_Tree_Depth(QTreeWidgetItem* item)
{
	int depth = 0;
	QVector<int> child_Depth;
	if(item->childCount()>0)
	{
		for(int item_Child_Index=0; item_Child_Index<item->childCount(); item_Child_Index++)
		{
			child_Depth.append(get_Tree_Depth(item->child(item_Child_Index)));
		}
		depth=1+*(std::minmax_element(child_Depth.begin(),child_Depth.end()).second);
	}
	return depth;
}

QString Global_Variables::structure_Item_Name(const Data& struct_Data)
{
	// PARAMETER

	QString text = "item_Name";

	if(struct_Data.item_Type == item_Type_Ambient)		text = "ambient: " + struct_Data.material;
	if(struct_Data.item_Type == item_Type_Layer)		text = struct_Data.material + " layer (" + QString::number(struct_Data.layer_Index) + ")";
	if(struct_Data.item_Type == item_Type_Substrate)	text = struct_Data.material + " substrate";
	if(struct_Data.item_Type == item_Type_Multilayer)	text = "Multilayer (" + QString::number(struct_Data.first_Layer_Index) +
																		" - " + QString::number(struct_Data.last_Layer_Index) + ")";
	if(struct_Data.item_Type == item_Type_Measurement)	text = "Measurement";

	return text;
}

QString Global_Variables::parameter_Name(const Data &struct_Data, QString whats_This, int index)
{
	// PARAMETER

	QString text = "variable_Name", brackets;

	if(struct_Data.item_Type == item_Type_Ambient)		brackets = "(ambient)";
	if(struct_Data.item_Type == item_Type_Layer)		brackets = "(layer " + QString::number(struct_Data.layer_Index) + ")";
	if(struct_Data.item_Type == item_Type_Substrate)	brackets = "(substrate)";
	if(struct_Data.item_Type == item_Type_Multilayer)	brackets = "Multilayer (" + QString::number(struct_Data.first_Layer_Index) + " - " +
																					QString::number(struct_Data.last_Layer_Index) + ")";
	if(struct_Data.item_Type == item_Type_Measurement)	brackets = "(measurement)";


	/// optical constants
	if(	struct_Data.item_Type == item_Type_Ambient	||
		struct_Data.item_Type == item_Type_Layer	||
		struct_Data.item_Type == item_Type_Substrate )
	{
		if(whats_This == whats_This_Absolute_Density)				text = struct_Data.material + " " + brackets + " Density, " + Rho_Sym;
		if(whats_This == whats_This_Relative_Density)				text = struct_Data.material + " " + brackets + " Relative Density, " + Rho_Sym;
		if(whats_This == whats_This_Permittivity)					text = struct_Data.material + " " + brackets + " Permittivity, 1-" + Epsilon_Sym;
		if(whats_This == whats_This_Absorption)						text = struct_Data.material + " " + brackets + " Absorption, " + Cappa_Sym;
		if(whats_This == whats_This_Composition && index>=0)		text = struct_Data.material + " " + brackets + " " + struct_Data.composition[index].type + " Composition, " + Zeta_Sym + "_" + struct_Data.composition[index].type;
	}

	/// thickness parameters
	if(	struct_Data.item_Type == item_Type_Layer )
	{
		if(whats_This == whats_This_Thickness)						text = struct_Data.material + " " + brackets + " Thickness, z";
		if(whats_This == whats_This_Thickness_Drift_Line_Value)		text = struct_Data.material + " " + brackets + " Thickness Linear Drift, dz";
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		text = struct_Data.material + " " + brackets + " Thickness Random Drift, dz";
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	text = struct_Data.material + " " + brackets + " Thickness Sine Drift : Amplitude, dz";
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	text = struct_Data.material + " " + brackets + " Thickness Sine Drift : Frequency, " + Nu_Sym;
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		text = struct_Data.material + " " + brackets + " Thickness Sine Drift : Phase, " + Phi_Sym;
	}

	/// interface parameters
	if(	struct_Data.item_Type == item_Type_Layer ||
		struct_Data.item_Type == item_Type_Substrate  )
	{
		if(whats_This == whats_This_Sigma)							text = struct_Data.material + " " + brackets + " Roughness/Diffuseness, " + Sigma_Sym;
		if(whats_This == whats_This_Sigma_Drift_Line_Value)			text = struct_Data.material + " " + brackets + " Roughness/Diffuseness Linear Drift, d" + Sigma_Sym;
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			text = struct_Data.material + " " + brackets + " Roughness/Diffuseness Random Drift, d" + Sigma_Sym;
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		text = struct_Data.material + " " + brackets + " Roughness/Diffuseness Sine Drift : Amplitude, d" + Sigma_Sym;
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		text = struct_Data.material + " " + brackets + " Roughness/Diffuseness Sine Drift : Frequency, " + Nu_Sym;
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			text = struct_Data.material + " " + brackets + " Roughness/Diffuseness Sine Drift : Phase, " + Phi_Sym;

		if(whats_This == whats_This_Interlayer_Composition)			text = struct_Data.material + " " + brackets + " Interlayer Composition, " + transition_Layer_Functions[index];
		if(whats_This == whats_This_Interlayer_My_Sigma)			text = struct_Data.material + " " + brackets + " Individual Roughness/Diffuseness, " + Sigma_Sym + "_" + transition_Layer_Functions[index];
	}

	/// stack parameters
	if(	struct_Data.item_Type == item_Type_Multilayer )
	{
		if(whats_This == whats_This_Num_Repetitions)				text = brackets + " Number of repetitions, N";
		if(whats_This == whats_This_Period)							text = brackets + " Period, d";
		if(whats_This == whats_This_Gamma)							text = brackets + " Thickness Ratio, " + Gamma_Sym;
	}

	/// measurement parameters
	if(	struct_Data.item_Type == item_Type_Measurement )
	{
		if(whats_This == whats_This_Angle)							text = struct_Data.angle_Type + " angle, " + Theta_Sym;		
		if(whats_This == whats_This_Angular_Resolution)				text = "Angular resolution, " + Delta_Big_Sym + Theta_Sym;
		if(whats_This == whats_This_Wavelength)						text = Global_Variables::wavelength_Energy_Name(wavelength_units);
		if(whats_This == whats_This_Spectral_Resolution)			text = "Spectral resolution" + Delta_Big_Sym + "E/E";
		if(whats_This == whats_This_Polarization)					text = "Polarization";
		if(whats_This == whats_This_Polarization_Sensitivity)		text = "Polarization sensitivity";
	}

	return text;
}

bool Global_Variables::expression_Is_Valid(QString expression_String, QStringList expression_Variables)
{
#ifdef EXPRTK
	double temp_Argument = 1;
	exprtk::symbol_table<double> symbol_table;
	for(QString& expr_Var : expression_Variables)
	{
		symbol_table.add_variable(expr_Var.toStdString(), temp_Argument);
	}
	symbol_table.add_constants();

	exprtk::expression<double> expression;
	expression.register_symbol_table(symbol_table);

	exprtk::parser<double> parser;
	return parser.compile(expression_String.toStdString(), expression);
#else
	return true;
#endif
}

Parameter* Global_Variables::get_Parameter_From_Struct_Item_by_Id(Data& struct_Data, id_Type id)
{
	// PARAMETER

	// measurement
	if(id == struct_Data.probe_Angle.indicator.id)								return &struct_Data.probe_Angle;
	if(id == struct_Data.angular_Resolution.indicator.id)						return &struct_Data.angular_Resolution;
	if(id == struct_Data.wavelength.indicator.id)								return &struct_Data.wavelength;
	if(id == struct_Data.spectral_Resolution.indicator.id)						return &struct_Data.spectral_Resolution;
	if(id == struct_Data.polarization.indicator.id)								return &struct_Data.polarization;
	if(id == struct_Data.polarization_Sensitivity.indicator.id)					return &struct_Data.polarization_Sensitivity;

	// optical properties
	if(id == struct_Data.absolute_Density.indicator.id)							return &struct_Data.absolute_Density;
	if(id == struct_Data.relative_Density.indicator.id)							return &struct_Data.relative_Density;
	if(id == struct_Data.permittivity.indicator.id)								return &struct_Data.permittivity;
	if(id == struct_Data.absorption.indicator.id)								return &struct_Data.absorption;
	for(int i=0; i<struct_Data.composition.size(); ++i)
	{
		if(id == struct_Data.composition[i].composition.indicator.id)			return &struct_Data.composition[i].composition;
	}

	// interface
	if(id == struct_Data.sigma.indicator.id)									return &struct_Data.sigma;
	for(int i=0; i<transition_Layer_Functions_Size; ++i)
	{
		if(id == struct_Data.interlayer_Composition[i].interlayer.indicator.id)	return &struct_Data.interlayer_Composition[i].interlayer;
		if(id == struct_Data.interlayer_Composition[i].my_Sigma.indicator.id)	return &struct_Data.interlayer_Composition[i].my_Sigma;
	}
	if(id == struct_Data.sigma_Drift.drift_Line_Value.indicator.id)				return &struct_Data.sigma_Drift.drift_Line_Value;
	if(id == struct_Data.sigma_Drift.drift_Rand_Rms.indicator.id)				return &struct_Data.sigma_Drift.drift_Rand_Rms;
	if(id == struct_Data.sigma_Drift.drift_Sine_Amplitude.indicator.id)			return &struct_Data.sigma_Drift.drift_Sine_Amplitude;
	if(id == struct_Data.sigma_Drift.drift_Sine_Frequency.indicator.id)			return &struct_Data.sigma_Drift.drift_Sine_Frequency;
	if(id == struct_Data.sigma_Drift.drift_Sine_Phase.indicator.id)				return &struct_Data.sigma_Drift.drift_Sine_Phase;

	// thickness
	if(id == struct_Data.thickness.indicator.id)								return &struct_Data.thickness;
	if(id == struct_Data.thickness_Drift.drift_Line_Value.indicator.id)			return &struct_Data.thickness_Drift.drift_Line_Value;
	if(id == struct_Data.thickness_Drift.drift_Rand_Rms.indicator.id)			return &struct_Data.thickness_Drift.drift_Rand_Rms;
	if(id == struct_Data.thickness_Drift.drift_Sine_Amplitude.indicator.id)		return &struct_Data.thickness_Drift.drift_Sine_Amplitude;
	if(id == struct_Data.thickness_Drift.drift_Sine_Frequency.indicator.id)		return &struct_Data.thickness_Drift.drift_Sine_Frequency;
	if(id == struct_Data.thickness_Drift.drift_Sine_Phase.indicator.id)			return &struct_Data.thickness_Drift.drift_Sine_Phase;

	// multilayer
	if(id == struct_Data.period.indicator.id)									return &struct_Data.period;
	if(id == struct_Data.gamma.indicator.id)									return &struct_Data.gamma;

	return NULL;
}
