#include "global_variables.h"
#include "multilayer_approach/multilayer_approach.h"
#include "standard/mydoublespinbox.h"

// version from save file
int loaded_Version_Major = VERSION_MAJOR;
int loaded_Version_Minor = VERSION_MINOR;
int loaded_Version_Build = VERSION_BUILD;

bool lambda_Out_Of_Range = false;

// global access to multilayer_Approach
Multilayer_Approach* global_Multilayer_Approach;

// -----------------------------------------------------------------------------------------

// calculations
std::vector<std::thread> global_Workers; // still empty

// locale
QLocale Locale;

// delimiters for file parsing
QRegExp delimiters("\\ |\\:|\\t|\\;|\\{|\\}"); // "," removed. Now "," and "." are decimal separators

// tabulated chemical elements
QStringList		element_Name    {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th","Pa","U"};
QVector<double>	element_Density {0.00008988,
								 0.0001785,
								 0.534,
								 1.848,
								 2.34,
								 2.267,
								 0.0012506,
								 0.001429,
								 0.001696,
								 0.0008999,
								 0.971,
								 1.738,
								 2.699,
								 2.3296,
								 1.82,
								 2.067,
								 0.003214,
								 0.0017837,
								 0.862,
								 1.54,
								 2.989,
								 4.540,
								 6.11,
								 7.15,
								 7.44,
								 7.874,
								 8.86,
								 8.912,
								 8.96,
								 7.134,
								 5.907,
								 5.323,
								 5.776,
								 4.809,
								 3.122,
								 0.003733,
								 1.532,
								 2.64,
								 4.469,
								 6.506,
								 8.570,
								 10.22,
								 11.50,
								 12.37,
								 12.41,
								 12.020,
								 10.501,
								 8.69,
								 7.310,
								 7.287,
								 6.685,
								 6.232,
								 4.93,
								 0.005887,
								 1.873,
								 3.594,
								 6.145,
								 6.770,
								 6.773,
								 7.007,
								 7.26,
								 7.52,
								 5.243,
								 7.895,
								 8.229,
								 8.55,
								 8.795,
								 9.066,
								 9.321,
								 6.965,
								 9.84,
								 13.31,
								 16.654,
								 19.25,
								 21.02,
								 22.610,
								 22.650,
								 21.46,
								 19.282,
								 13.5336,
								 11.85,
								 11.342,
								 9.807,
								 9.32,
								 7,
								 0.00973,
								 1.87,
								 5.50,
								 10.07,
								 11.72,
								 15.37,
								 18.95};
QMap<QString, double> sorted_Density;
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
QStringList value_R_Mode				{"R","R+"+Phi_Sym };									// change enum!
QStringList value_R_Mode_Label_1		{"R","R"          };
QStringList value_R_Mode_Label_2		{" ","Phase "+Phi_Sym+", "+Degree_Sym };
QStringList value_T_Mode				{"T"};													// change enum!
QStringList value_A_Mode				{"A"};													// change enum!

/// -------------------------------------------------------------------------
/// GSL
/// -------------------------------------------------------------------------
// GSL optimization methods
QString GSL_Group = "GSL library";
QStringList GSL_Methods			{	/* Nonlinear Least-Squares Fitting */
									"Levenberg-Marquardt",
									"Levenberg-Marquardt with Geodesic Acceleration",
									"Dogleg",
									"Double Dogleg",
									"Two Dimensional Subspace",

									/* Multidimensional Minimization (derivative) */
									"Fletcher-Reeves Conjugate Gradient",
									"Polak-Ribiere Conjugate Gradient",
									"Broyden-Fletcher-Goldfarb-Shanno : BFGS",
									"Broyden-Fletcher-Goldfarb-Shanno : BFGS-2",
									"Steepest Descent",

									/* Multidimensional Minimization (derivative-free) */
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
QStringList GSL_Scales			{"Mor"+More_Sym,
								 "Levenberg",
								 "Marquardt"
								}; // change enum!

// GSL finite differences
QStringList GSL_Fdtype			{"Forward",
								 "Central"
								}; // change enum!

/// -------------------------------------------------------------------------
/// SwarmOps
/// -------------------------------------------------------------------------
// SO optimization methods
QString SO_Group = "SwarmOps library";
QStringList SO_Methods = { /* Mesh Iteration. */
						   "Mesh iteration",

						   /* Random Sampling */
						   "Random Sampling (Uniform)",

						   /* Gradient-Based Optimization */
//						   "Gradient Descent",
//						   "Gradient Emancipated Descent",

						   /* Local Sampling */
						   "Hill-Climber",
						   "Simulated Annealing",
						   "Pattern Search",
						   "Local Unimodal Sampling",

						   /* Swarm-Based Optimization, DE and variants */
						   "Differential Evolution (Basic)",
						   "DE (Best/1/Bin/Simple)",  // Differential Evolution Suite, specified in header
						   "DE with Temporal Parameters",
						   "Jan. Differential Evolution",
						   "Evolution by Lingering Global Best",
						   "More Yo-yos doing Global optimization",

						   /* Swarm-Based Optimization, PSO and variants */
						   "Particle Swarm Optimization (Basic)",
						   "Forever Accumulating Evolution",
						   "Many Optimizing Liaisons",

						   /* Compound Methods */
						   "Layered and Interleaved Co-Evolution"
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

// size units
QStringList size_Units_List					{"mm"};				// change enum!

// -----------------------------------------------------------------------------------------

QString fit_Color							= "QWidget { background: rgb(255, 210, 210); }";
QString white_Color							= "background-color: white";
QString free_Parameter_Color				= "background-color: lightblue";
QString slave_Parameter_Color				= "QWidget { background: rgb(255, 50,  50);  }";
QString master_Parameter_Color				= "QWidget { background: rgb(50,  255, 50);  }";
QString master_Slave_Parameter_Color		= "QWidget { background: rgb(255, 255, 50);  }";
QString confidence_Parameter_Color			= "background-color: violet";
QString master_Confidence_Parameter_Color	= "QWidget { background: rgb(170, 0,   255); }";

QString common_Thickness_Color				= "QWidget { background: rgb(231, 255, 165); }";
QString common_Sigma_Color					= "QWidget { background: rgb(254, 214, 255); }";
QString common_Thickness_and_Sigma_Color	= "QWidget { background: rgb(143, 255, 225); }";
QString soft_Restriction_Color				= "QWidget { background: rgb(94,  244, 227); }";

// -----------------------------------------------------------------------------------------

QPen subinterval_Plot_Pen = QPen(Qt::transparent);
QBrush subinterval_Plot_Brush = QBrush(QColor(0, 0, 255, 15));

// -----------------------------------------------------------------------------------------

int scale = 96;
int corner_x_shift = 0;
int corner_y_shift = 0;
int width_add = 0;
int height_add = 0;

// -----------------------------------------------------------------------------------------

QString multilayer_Approach_Default_Title = "Multifitting";

// -----------------------------------------------------------------------------------------

bool can_Change_Index;

// -----------------------------------------------------------------------------------------

Optical_Constants* optical_Constants;

Global_Variables::Global_Variables()
{

}

void Global_Variables::find_Gui_Shifts()
{
#ifdef _WIN32
	// corner 100-149%
//		structure_table_x_corner = frameGeometry().x();
//		structure_table_y_corner = frameGeometry().y();

	// corner 150-174%
//		structure_table_x_corner = frameGeometry().x()-1;
//		structure_table_y_corner = frameGeometry().y()-1;

	// corner 175-199%
//		structure_table_x_corner = frameGeometry().x()-1;
//		structure_table_y_corner = frameGeometry().y();

	// corner 200-224%
//		structure_table_x_corner = frameGeometry().x();
//		structure_table_y_corner = frameGeometry().y();

	// corner 225-249%
//		structure_table_x_corner = frameGeometry().x();
//		structure_table_y_corner = frameGeometry().y()-1;

	// corner 250-299%
//		structure_table_x_corner = frameGeometry().x()-1;
//		structure_table_y_corner = frameGeometry().y();

	// corner 300%
//		structure_table_x_corner = frameGeometry().x();
//		structure_table_y_corner = frameGeometry().y();

	/// =========================================

	// all custom sizes and 100%
//		structure_table_width  = geometry().width();
//		structure_table_height = geometry().height();

	// size 125% preset without signing out
//		structure_table_width  = geometry().width()+2;
//		structure_table_height = geometry().height()+8;

	// size 150% preset without signing out
//		structure_table_width  = geometry().width()+3;
//		structure_table_height = geometry().height()+8;

	// size 175% preset without signing out
//		structure_table_width  = geometry().width()+4;
//		structure_table_height = geometry().height()+12;

	/// =========================================

	scale = qApp->desktop()->logicalDpiX()*qApp->desktop()->devicePixelRatio();
	if( 96<=scale && scale<144 ) {corner_x_shift = 0; corner_y_shift = 0;}; // 100-149%
	if(144<=scale && scale<168 ) {corner_x_shift = 1; corner_y_shift = 1;}; // 150-174%
	if(168<=scale && scale<192 ) {corner_x_shift = 1; corner_y_shift = 0;}; // 175-199%
	if(192<=scale && scale<216 ) {corner_x_shift = 0; corner_y_shift = 0;}; // 200-224%
	if(216<=scale && scale<240 ) {corner_x_shift = 0; corner_y_shift = 1;}; // 225-249%
	if(240<=scale && scale<288 ) {corner_x_shift = 0; corner_y_shift = 1;}; // 250-299%
	if(288<=scale && scale<312 ) {corner_x_shift = 0; corner_y_shift = 0;}; // 300-324%

//	if(scale==120 ) {width_add = 2; height_add = 8;};  // 125% preset without signing out
//	if(scale==144 ) {width_add = 3; height_add = 8;};  // 150% preset without signing out
//	if(scale==168 ) {width_add = 4; height_add = 12;}; // 175% preset without signing out
#endif
}

void Global_Variables::read_Optical_Constants()
{
	optical_Constants = new Optical_Constants;
}

void Global_Variables::create_Sorted_Elements_Map()
{
	// map of sorted chemical elements
	for(int i=0; i<element_Name.size(); ++i)
	{
		sorted_Elements.insert(element_Name[i],element_Mass[i]);
		sorted_Density. insert(element_Name[i],element_Density[i]);
	}
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
	while(item!=nullptr)
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

QTreeWidgetItem* Global_Variables::get_Item_From_Tree_By_Id(QTreeWidget* tree, id_Type id)
{
	QTreeWidgetItemIterator it(tree);
	while (*it)
	{
		QTreeWidgetItem* structure_Item = *it;
		if(structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>().id == id)
		{
			return structure_Item;
		}
		++it;
	}
	qInfo() << "Global_Variables::get_Item_From_Tree_By_Id  :  item for id" << id << "not found" << endl;
	return nullptr;
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

	if(struct_Data.item_Type == item_Type_Ambient)			{text = "ambient: " + struct_Data.material;}
	if(struct_Data.item_Type == item_Type_Layer)			{text = struct_Data.material + " layer (" + Locale.toString(struct_Data.layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_Substrate)		{text = struct_Data.material + " substrate";}
	if(struct_Data.item_Type == item_Type_Multilayer)		{text = "Multilayer (" + Locale.toString(struct_Data.first_Layer_Index) +
																		" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_Regular_Aperiodic){text = "Regular Aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
																		" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_General_Aperiodic){text = "General Aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
																		" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_Measurement)		{text = "Measurement";}

	return text;
}

QString Global_Variables::parameter_Name(const Data &struct_Data, QString whats_This, int index)
{
	// PARAMETER

	QString text = "variable_Name", brackets;

	if(struct_Data.item_Type == item_Type_Ambient)			{brackets = "(ambient)";}
	if(struct_Data.item_Type == item_Type_Layer)			{brackets = "(layer " + Locale.toString(struct_Data.layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_Substrate)		{brackets = "(substrate)";}
	if(struct_Data.item_Type == item_Type_Multilayer)		{brackets = "Multilayer (" + Locale.toString(struct_Data.first_Layer_Index) +
																			" - " +	Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_Regular_Aperiodic){brackets = "Regular Aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
																			" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_General_Aperiodic){brackets = "General Aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
																			" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_Measurement)		{brackets = "(measurement)";}


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
	// TODO
	if(	struct_Data.item_Type == item_Type_General_Aperiodic )
	{

	}
	if(	struct_Data.item_Type == item_Type_Regular_Aperiodic )
	{

	}

	/// measurement parameters
	if(	struct_Data.item_Type == item_Type_Measurement )
	{
		if(whats_This == whats_This_Angle)							text = struct_Data.angle_Type + " angle, " + Theta_Sym;
		if(whats_This == whats_This_Angular_Resolution)				text = "Angular resolution, " + Delta_Big_Sym + Theta_Sym;
		if(whats_This == whats_This_Wavelength)						text = Global_Variables::wavelength_Energy_Name(wavelength_units);
		if(whats_This == whats_This_Spectral_Resolution)			text = "Spectral resolution, " + Delta_Big_Sym + "E/E";
		if(whats_This == whats_This_Polarization)					text = "Polarization";
		if(whats_This == whats_This_Polarization_Sensitivity)		text = "Polarization sensitivity";

		if(whats_This == whats_This_Background)						text = "Background";
		if(whats_This == whats_This_Beam_Size)						text = "Beam width";
		if(whats_This == whats_This_Beam_Profile_Spreading)			text = "Beam spreading";
		if(whats_This == whats_This_Sample_Size)					text = "Sample size";
		if(whats_This == whats_This_Sample_Shift)					text = "Sample shift";
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
	if(id == struct_Data.background.indicator.id)								return &struct_Data.background;

	if(id == struct_Data.beam_Size.indicator.id)								return &struct_Data.beam_Size;
	if(id == struct_Data.beam_Profile_Spreading.indicator.id)					return &struct_Data.beam_Profile_Spreading;
	if(id == struct_Data.sample_Size.indicator.id)								return &struct_Data.sample_Size;
	if(id == struct_Data.sample_Shift.indicator.id)								return &struct_Data.sample_Shift;

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

	return nullptr;
}

Parameter* Global_Variables::get_Parameter_From_Struct_Item_by_Whats_This(Data& struct_Data, QString whats_This, int* line_edit_precision, int* thumbnail_precision, QString* units, double* coeff)
{
	// PARAMETER

	// if nullpointers
	int line_edit_precision_LOC;
	int thumbnail_precision_LOC;
	QString units_LOC;
	double coeff_LOC;
	if(line_edit_precision == nullptr) line_edit_precision = &line_edit_precision_LOC;
	if(thumbnail_precision == nullptr) thumbnail_precision = &thumbnail_precision_LOC;
	if(units == nullptr) units = &units_LOC;
	if(coeff == nullptr) coeff = &coeff_LOC;

	// measurement
	if(whats_This == whats_This_Angle)							{*line_edit_precision = line_edit_angle_precision;				*thumbnail_precision = thumbnail_angle_precision;				*units = angle_units;			*coeff = angle_Coefficients_Map.value(angle_units);				return &struct_Data.probe_Angle;				}
	if(whats_This == whats_This_Angular_Resolution)				{*line_edit_precision = line_edit_angle_precision;				*thumbnail_precision = thumbnail_angle_precision;				*units = angle_units;			*coeff = angle_Coefficients_Map.value(angle_units);				return &struct_Data.angular_Resolution;			}
	if(whats_This == whats_This_Wavelength)						{*line_edit_precision = line_edit_wavelength_precision;			*thumbnail_precision = thumbnail_wavelength_precision;			*units = " " + wavelength_units;*coeff = wavelength_Coefficients_Map.value(wavelength_units);	return &struct_Data.wavelength;					}
	if(whats_This == whats_This_Spectral_Resolution)			{*line_edit_precision = line_edit_spectral_resolution_precision;*thumbnail_precision = thumbnail_spectral_resolution_precision;	*units = "";					*coeff = 1;														return &struct_Data.spectral_Resolution;		}
	if(whats_This == whats_This_Polarization)					{*line_edit_precision = line_edit_polarization_precision;		*thumbnail_precision = thumbnail_polarization_precision;		*units = "";					*coeff = 1;														return &struct_Data.polarization;				}
	if(whats_This == whats_This_Polarization_Sensitivity)		{*line_edit_precision = line_edit_polarization_precision;		*thumbnail_precision = thumbnail_polarization_precision;		*units = "";					*coeff = 1;														return &struct_Data.polarization_Sensitivity;	}
	if(whats_This == whats_This_Background)						{*line_edit_precision = line_edit_background_precision;			*thumbnail_precision = thumbnail_background_precision;			*units = "";					*coeff = 1;														return &struct_Data.background;					}

	if(whats_This == whats_This_Beam_Size)						{*line_edit_precision = line_edit_beam_size_precision;		*thumbnail_precision = thumbnail_beam_size_precision;	*units = "";	*coeff = 1;	return &struct_Data.beam_Size; }
	if(whats_This == whats_This_Beam_Profile_Spreading)			{*line_edit_precision = line_edit_beam_size_precision;		*thumbnail_precision = thumbnail_beam_size_precision;	*units = "";	*coeff = 1;	return &struct_Data.beam_Profile_Spreading;}
	if(whats_This == whats_This_Sample_Size)					{*line_edit_precision = line_edit_sample_size_precision;	*thumbnail_precision = thumbnail_sample_size_precision;	*units = "";	*coeff = 1;	return &struct_Data.sample_Size;}
	if(whats_This == whats_This_Sample_Shift)					{*line_edit_precision = line_edit_sample_size_precision;	*thumbnail_precision = thumbnail_sample_size_precision;	*units = "";	*coeff = 1;	return &struct_Data.sample_Shift;}

	// optical properties
	if(whats_This == whats_This_Absolute_Density)				{*line_edit_precision = line_edit_density_precision;		*thumbnail_precision = thumbnail_density_precision;		*units = " " + density_units;	*coeff = 1;	return &struct_Data.absolute_Density;	}
	if(whats_This == whats_This_Relative_Density)				{*line_edit_precision = line_edit_density_precision;		*thumbnail_precision = thumbnail_density_precision;		*units = "" ;					*coeff = 1;	return &struct_Data.relative_Density;	}
	if(whats_This == whats_This_Permittivity)					{*line_edit_precision = line_edit_permittivity_precision;	*thumbnail_precision = thumbnail_permittivity_precision;*units = " " + opt_const_units;	*coeff = 1;	return &struct_Data.permittivity;		}
	if(whats_This == whats_This_Absorption)						{*line_edit_precision = line_edit_absorption_precision;		*thumbnail_precision = thumbnail_absorption_precision;	*units = " " + opt_const_units;	*coeff = 1;	return &struct_Data.absorption;			}
	if(whats_This == whats_This_Composition)					{*line_edit_precision = line_edit_composition_precision;	*thumbnail_precision = thumbnail_composition_precision;	*units = "";					*coeff = 1;	return nullptr;							}

	// interface
	if(whats_This == whats_This_Sigma)							{*line_edit_precision = line_edit_sigma_precision;			*thumbnail_precision = thumbnail_sigma_precision;		*units = " " + length_units;	*coeff = length_Coefficients_Map.value(length_units);	return &struct_Data.sigma;	}
	if(whats_This == whats_This_Sigma_Drift_Line_Value)			{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Drift.drift_Line_Value;		}
	if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Drift.drift_Rand_Rms;			}
	if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Drift.drift_Sine_Amplitude;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Drift.drift_Sine_Frequency;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1;	return &struct_Data.sigma_Drift.drift_Sine_Phase;		}
	if(whats_This == whats_This_Interlayer_Composition)			{*line_edit_precision = line_edit_interlayer_precision;		*thumbnail_precision = thumbnail_interlayer_precision;	*units = "";					*coeff = 1;	return nullptr;											}
	if(whats_This == whats_This_Interlayer_My_Sigma)			{*line_edit_precision = line_edit_sigma_precision;			*thumbnail_precision = thumbnail_sigma_precision;		*units = " " + length_units;	*coeff = length_Coefficients_Map.value(length_units);	return nullptr;	}

	// thickness
	if(whats_This == whats_This_Thickness)						{*line_edit_precision = line_edit_thickness_precision;		*thumbnail_precision = thumbnail_thickness_precision;	*units = " " + length_units;	*coeff = length_Coefficients_Map.value(length_units);	return &struct_Data.thickness; }
	if(whats_This == whats_This_Thickness_Drift_Line_Value)		{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1;	return &struct_Data.thickness_Drift.drift_Line_Value;		}
	if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1;	return &struct_Data.thickness_Drift.drift_Rand_Rms;			}
	if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1;	return &struct_Data.thickness_Drift.drift_Sine_Amplitude;	}
	if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1;	return &struct_Data.thickness_Drift.drift_Sine_Frequency;	}
	if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1;	return &struct_Data.thickness_Drift.drift_Sine_Phase;		}

	// multilayer
	if(whats_This == whats_This_Period)							{*line_edit_precision = line_edit_period_precision;			*thumbnail_precision = thumbnail_period_precision;		*units = " " + length_units;	*coeff = length_Coefficients_Map.value(length_units);	return &struct_Data.period; }
	if(whats_This == whats_This_Gamma)							{*line_edit_precision = line_edit_gamma_precision;			*thumbnail_precision = thumbnail_gamma_precision;		*units = "";					*coeff = 1;	return &struct_Data.gamma; }

	return nullptr;
}

void Global_Variables::copy_Tree(const QTreeWidget* from_Tree, QTreeWidget* to_Tree)
{
	to_Tree->clear();
	for(int i=0; i<from_Tree->topLevelItemCount(); i++)
	{
		to_Tree->addTopLevelItem(from_Tree->topLevelItem(i)->clone());	// the data are also copied here
	}
}

bool Global_Variables::check_Loaded_Version(int MAJOR, int MINOR, int BUILD)
{
	if( (loaded_Version_Major >MAJOR) ||
	   ((loaded_Version_Major==MAJOR) && (loaded_Version_Minor >MINOR)) ||
	   ((loaded_Version_Major==MAJOR) && (loaded_Version_Minor==MINOR) && (loaded_Version_Build>=BUILD)) )
	{
		return true;
	} else
	{
		return false;
	}
}

void Global_Variables::create_Shortcuts(QWidget* this_Widget)
{
	// shortcuts
	{
		QShortcut* save_Shortcut			= new QShortcut(QKeySequence(Qt::Key_S | Qt::CTRL),				this_Widget);
		QShortcut* save_As_Shortcut			= new QShortcut(QKeySequence(Qt::Key_S | Qt::CTRL | Qt::SHIFT), this_Widget);
		QShortcut* open_Shortcut			= new QShortcut(QKeySequence(Qt::Key_O | Qt::CTRL),				this_Widget);
		QShortcut* open_As_Shortcut			= new QShortcut(QKeySequence(Qt::Key_O | Qt::CTRL | Qt::SHIFT), this_Widget);
		QShortcut* fit_Shortcut				= new QShortcut(QKeySequence(Qt::Key_F | Qt::CTRL | Qt::SHIFT), this_Widget);
		QShortcut* calc_Specular_Shortcut	= new QShortcut(QKeySequence(Qt::Key_C | Qt::CTRL | Qt::SHIFT), this_Widget);
		QShortcut* calc_Confidence_Shortcut	= new QShortcut(QKeySequence(Qt::Key_A | Qt::CTRL | Qt::SHIFT), this_Widget);
		QShortcut* abort_Shortcut			= new QShortcut(QKeySequence(Qt::Key_Period | Qt::ALT),         this_Widget);

		connect(save_Shortcut,				&QShortcut::activated, this_Widget, [=]
		{
			if(global_Multilayer_Approach->file_Was_Opened_or_Saved)
				global_Multilayer_Approach->save(last_file);
			else
				global_Multilayer_Approach->save(default_File);
		});
		connect(save_As_Shortcut,			&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->save_As();					 });
		connect(open_Shortcut,				&QShortcut::activated, this_Widget, [=]
		{
			if(global_Multilayer_Approach->file_Was_Opened_or_Saved)
				global_Multilayer_Approach->open(last_file);
			else
				global_Multilayer_Approach->open(default_File);
		});
		connect(open_As_Shortcut,			&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->open_As();					 });
		connect(fit_Shortcut,				&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->start_Fitting();		     });
		connect(calc_Specular_Shortcut,		&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->calc_Reflection();			 });
		connect(calc_Confidence_Shortcut,	&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->calc_Confidence_Intervals();});
		connect(abort_Shortcut,				&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->abort_Calculations();		 });
	}
}

void Global_Variables::parallel_For(int num_Points, int num_Threads, const std::function<void(int n_Min, int n_Max)>& lambda)
{
	if(num_Threads>global_Workers.size())
	{
		qInfo() << "Global_Variables::parallel_For  : num_Threads>workers.size()" << endl;
		QMessageBox::critical(nullptr, "Global_Variables::parallel_For", "num_Threads>workers.size()");
		exit(EXIT_FAILURE);
	}

	int delta_N = num_Points / num_Threads;	// number of points per thread
	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		int n_Min = thread_Index*delta_N;
		int n_Max = (thread_Index + 1)*delta_N;
		if(thread_Index == (num_Threads-1))
		{
			n_Max = num_Points;
		}
		global_Workers[thread_Index] = std::thread(lambda, n_Min, n_Max);
	}
	for(std::thread& worker : global_Workers) { if(worker.joinable()) worker.join(); }
}

bool validate (double number)
{
	if(number>=1 && number<10) return true;
	else					   return false;
}
double Global_Variables::get_Order_Of_Magnitude(double number)
{
	double order = 1;
	if(number>1)
	{
		while(!validate(number))
		{
			number = number/10;
			order = order*10;
		}
		return order;
	} else
	{
		if(number>0)
		{
			if(abs(number-1)<DBL_EPSILON)
			{
				return 1;
			}
			else
			{
				while(!validate(number))
				{
					number = number*10;
					order = order/10;
				}
				return order;
			}
		} else
		{
			return 1e-5;
		}
	}
}

template<typename Type>
void Global_Variables::resize_Line_Edit(Type* line_Edit, bool adjust_Window)
{
	if(line_Edit == nullptr)
	{
		qInfo() << "Global_Variables::resize_Line_Edit  :  line_Edit == nullptr" << endl;
		QMessageBox::critical(nullptr, "Global_Variables::resize_Line_Edit", "line_Edit == nullptr");
		return;
	}

	QWidget* window = line_Edit->window();
	QString text = line_Edit->text();

	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text) + QLINEEDIT_RESIZE_MARGIN;
	if(	typeid(Type) == typeid(QSpinBox) ||
		typeid(Type) == typeid(QDoubleSpinBox) ||
		typeid(Type) == typeid(MyDoubleSpinBox))
	{
		if(qobject_cast<QAbstractSpinBox*>(line_Edit)->buttonSymbols() != QAbstractSpinBox::NoButtons)
		{
			width += SPINBOX_BUTTONS_RESIZE_MARGIN;
		}
	}
	if(width>line_Edit->property(min_Size_Property).toInt())
	{
		line_Edit->setFixedWidth(width);
		if(adjust_Window) QMetaObject::invokeMethod(window, "adjustSize", Qt::QueuedConnection);
	} else
	{
		line_Edit->setFixedWidth(line_Edit->property(min_Size_Property).toInt());
		if(adjust_Window) QMetaObject::invokeMethod(window, "adjustSize", Qt::QueuedConnection);
	}
//	window->adjustSize();
}
template void Global_Variables::resize_Line_Edit<QLineEdit>		 (QLineEdit*,		bool);
template void Global_Variables::resize_Line_Edit<QSpinBox>		 (QSpinBox*,		bool);
template void Global_Variables::resize_Line_Edit<QDoubleSpinBox> (QDoubleSpinBox*,  bool);
template void Global_Variables::resize_Line_Edit<MyDoubleSpinBox>(MyDoubleSpinBox*, bool);

void Global_Variables::change_Parent_Period_Gamma_Thickness(QTreeWidgetItem* current_Item)
{
	// nearest parent
	QTreeWidgetItem* parent_Item = current_Item->parent();
	if(parent_Item)
	{
		Data parent_Data = parent_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(parent_Data.item_Type == item_Type_Multilayer)
		{
			double first_Thickness = 0;
			parent_Data.period.value = 0;
			for(int i = 0; i<parent_Item->childCount(); i++)
			{
				const Data child_Data = parent_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				if(child_Data.item_Type == item_Type_Layer)
				{
					parent_Data.period.value += child_Data.thickness.value;
					if(i==0) first_Thickness  = child_Data.thickness.value;
				}
				if(child_Data.item_Type == item_Type_Multilayer || child_Data.item_Type == item_Type_General_Aperiodic)
				{
					parent_Data.period.value += child_Data.period.value*child_Data.num_Repetition.value();
					if(i==0) first_Thickness  = child_Data.period.value*child_Data.num_Repetition.value();
				}
				if(child_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					double sum = 0;
					for(int j=0; j<child_Data.regular_Components.size(); ++j)
					{
						for(int n=0; n<child_Data.num_Repetition.value(); ++n)
						{
							sum += child_Data.regular_Components[j].components[n].thickness.value;
						}
					}
					parent_Data.period.value += sum;
					if(i==0) first_Thickness  = sum;
				}
			}
			if(parent_Item->childCount() == 2)
			{
				parent_Data.gamma.value = first_Thickness/parent_Data.period.value;
			}
			QVariant var;
			var.setValue( parent_Data );
			parent_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// further parents
		if(parent_Item->parent())
			change_Parent_Period_Gamma_Thickness(parent_Item);
	}
}

void Global_Variables::plot_All_Data_in_Graphs()
{
	if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))
	{
		for(Curve_Plot* curve_Plot : global_Multilayer_Approach->optical_Graphs->plots)
		{
			curve_Plot->plot_All_Data();
		}
	}
}

void Global_Variables::replot_All_Graphs()
{
	if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))
	{
		for(Curve_Plot* curve_Plot : global_Multilayer_Approach->optical_Graphs->plots)
		{
			curve_Plot->custom_Plot->replot();
			curve_Plot->custom_Plot->xAxis->rangeChanged(curve_Plot->custom_Plot->xAxis->range()); // to cause normal replotting. crutch?
		}
	}
}
