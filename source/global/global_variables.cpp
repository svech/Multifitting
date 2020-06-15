#include "boost/math/special_functions/bessel.hpp"
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

// data types
QString no_Measurement_Type = "no measurement type";
QStringList measurement_Types {	/** 1D */			/// change enum!
								"Specular scan",	// R,T vs angle, wavelength
								"Detector scan",	// R-,T-Scattering vs angle // from 0 or from specular direction
								"Rocking scan",		// R-,T-Scattering vs angle	// from 0 or from specular direction
								"Offset scan",		// R-,T-Scattering vs angle, wavelength
													/// change enum!
								/** 2D */
								"GISAS map"			// R-,T-Scattering vs theta & phi // from 0 or from specular direction
};

// argument types
QString no_Argument_Type = "no argument type";
QStringList argument_Types	{"Beam grazing angle "+Theta_Sym+Zero_Subscript_Sym, "Deviation from specular "+Delta_Big_Sym+Theta_Sym+Zero_Subscript_Sym,
							 "Detector polar angle "+Theta_Sym, "Detector azimuthal angle "+Phi_Sym, "Detector polar and azimuthal angles", "Wavelength/Energy"}; // change enum!

// value types
QString no_Value_Type = "no value type";
QStringList value_Types		  {"Reflectance", "Transmittance", "Absorptance", "Scattering", "GISAS"};  // change enum!
QStringList value_Types_Short {"R", "T", "A", "S"};											  // change enum!

// distributions
QStringList distributions { "Gate", "Cosine", "Gaussian", "Lorentz" };

// detector types
QStringList detectors = { "Slit", "Crystal", "Spherical", "Rectangular" };

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
QStringList   wavelength_Units_List				{Angstrom_Sym,"nm","eV","keV"}; // Mu_Sym + "m"	// change enum!
QList<double> wavelength_Coefficients_List		{1.           ,1E1,1.  ,1E3  }; // 1E4
QMap<QString, double>  wavelength_Coefficients_Map;

// length
QStringList   length_Units_List					{Angstrom_Sym,"nm"}; // Mu_Sym + "m"				// change enum!
QList<double> length_Coefficients_List			{1.          , 1E1}; // 1E4
QMap<QString, double>  length_Coefficients_Map;

// angle
QStringList   angle_Units_List				{Degree_Sym,Prime_Sym   ,Double_Prime_Sym,"mrad"   };  // "rad"    // change enum!
QStringList   angle_Units_Legend_List		{"degree"  ,"arc minute","arc second"    ,"mrad"   };  // "radian"
QList<double> angle_Coefficients_List		{1.		   ,1./60       ,1./3600         ,0.18/M_PI};  // 180./M_PI
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
#ifdef _WIN32
	QBrush subinterval_Plot_Brush    = QBrush(QColor(0, 0, 255, 25));
	QBrush subinterval_Plot_Brush_2D = QBrush(QColor(0, 0, 0, 90));
#endif
#ifdef __linux__
	QBrush subinterval_Plot_Brush    = QBrush(QColor(0, 0, 255, 35));
	QBrush subinterval_Plot_Brush_2D = QBrush(QColor(0, 0, 0, 111));
#endif

vector<QColor> color_Contrast_Sequence = {	QColor(0, 205, 0),
											QColor(255, 0, 0),
											QColor(0, 0, 255),
											QColor(240, 65, 255),
											QColor(0, 0, 0),
											QColor(25, 205, 205),
											QColor(0, 120, 55),
											QColor(250, 140, 0),
											QColor(0, 0, 127),
											QColor(255, 0, 127)
										  };
vector<QColor> color_Contrast_Adjoint_Sequence = {	QColor(0, 115, 40),
													QColor(125, 0, 0),
													QColor(50, 50, 125),
													QColor(120, 35, 125),
													QColor(100, 100, 100),
													QColor(1, 105, 105),
													QColor(0, 60, 27),
													QColor(125, 70, 0),
													QColor(30, 30, 94),
													QColor(127, 0, 64)
												 };

double default_Profile_Line_Thickness = 1.4;
double selected_Profile_Line_Thickness = 3.5;

// -----------------------------------------------------------------------------------------

int scale = 96;
int corner_x_shift = 0;
int corner_y_shift = 0;
int width_add = 0;
int height_add = 0;

// -----------------------------------------------------------------------------------------

QString multilayer_Approach_Default_Title = "Multifitting";

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

void Global_Variables::pseudo_Deserialize_Tree(QDataStream &in)
{
	id_Type tree_Id = 0;
	in >> tree_Id;

	int num_Items=0;
	in >> num_Items;

	for(int i=0; i<num_Items; ++i)
	{
		Data data;
		in >> data;

		QString id_Parent;
		in >> id_Parent;
	}
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

void Global_Variables::pseudo_Deserialize_Variables_List(QDataStream& in)
{
	// load number of items
	int num_Items;
	in >> num_Items;

	for(int i=0; i<num_Items; ++i)
	{
		QString text;
		in >> text;

		Independent_Indicator indicator;
		in >> indicator;
	}
}

double Global_Variables::wavelength_Energy(QString wavelength_Units, double y)
{
	double value = -1000;
	for(int i=0; i<wavelength_Units_List.size(); ++i)
	{
		if(wavelength_Units == wavelength_Units_List[i])
		{
			if( 0<=i && i<=nm)                    value = y;
			if( eV<=i && i<wavelength_Units_List.size()) value = angstrom_eV(y);
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
			if( 0<=i && i<3) value="Wavelength " + Lambda_Sym;
			if( 3<=i && i<wavelength_Units_List.size()) value="Energy E";
		}
	}
	return value;
}

QString Global_Variables::wavelength_Energy_Symbol(QString spectral_Units)
{
	QString lambda_Sym;
	if(	spectral_Units == wavelength_Units_List[angstrom] ||
		spectral_Units == wavelength_Units_List[nm]	   )
	{
		lambda_Sym = Lambda_Sym;
	} else
	{
		lambda_Sym = "E";
	}
	return lambda_Sym;
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
	if(struct_Data.item_Type == item_Type_Regular_Aperiodic){text = "Regular aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
																		" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_General_Aperiodic){text = "General aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
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
	if(struct_Data.item_Type == item_Type_Regular_Aperiodic){brackets = "Regular aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
																			" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}
	if(struct_Data.item_Type == item_Type_General_Aperiodic){brackets = "General aperiodic ("  + Locale.toString(struct_Data.first_Layer_Index) +
																			" - " + Locale.toString(struct_Data.last_Layer_Index) + ")";}

	/// optical constants
	if(	struct_Data.item_Type == item_Type_Ambient	||
		struct_Data.item_Type == item_Type_Layer	||
		struct_Data.item_Type == item_Type_Substrate )
	{
		if(whats_This == whats_This_Absolute_Density)				text = struct_Data.material + " " + brackets + " Density, " + Rho_Sym;
		if(whats_This == whats_This_Relative_Density)				text = struct_Data.material + " " + brackets + " Relative density, " + Rho_Sym;
		if(whats_This == whats_This_Permittivity)					text = struct_Data.material + " " + brackets + " Permittivity, 1-" + Epsilon_Sym;
		if(whats_This == whats_This_Absorption)						text = struct_Data.material + " " + brackets + " Absorption, " + Cappa_Sym;
		if(whats_This == whats_This_Composition && index>=0)		text = struct_Data.material + " " + brackets + " " + struct_Data.composition[index].type + " composition, " + Zeta_Sym + "_" + struct_Data.composition[index].type;
	}

	/// thickness parameters
	if(	struct_Data.item_Type == item_Type_Layer )
	{
		if(whats_This == whats_This_Thickness)						text = struct_Data.material + " " + brackets + " Thickness, z";
		if(whats_This == whats_This_Thickness_Drift_Line_Value)		text = struct_Data.material + " " + brackets + " Thickness linear drift, dz";
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		text = struct_Data.material + " " + brackets + " Thickness random drift, dz";
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	text = struct_Data.material + " " + brackets + " Thickness sine drift : amplitude, dz";
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	text = struct_Data.material + " " + brackets + " Thickness sine drift : frequency, " + Nu_Sym;
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		text = struct_Data.material + " " + brackets + " Thickness sine drift : phase, " + Phi_Sym;
	}

	/// interface parameters
	if(	struct_Data.item_Type == item_Type_Layer ||
		struct_Data.item_Type == item_Type_Substrate  )
	{
		// interlayer
		if(whats_This == whats_This_Sigma_Diffuse)					text = struct_Data.material + " " + brackets + " Diffuseness, s";
		if(whats_This == whats_This_Sigma_Drift_Line_Value)			text = struct_Data.material + " " + brackets + " Diffuseness linear drift, ds";
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			text = struct_Data.material + " " + brackets + " Diffuseness random drift, ds";
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		text = struct_Data.material + " " + brackets + " Diffuseness sine drift : amplitude, ds";
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		text = struct_Data.material + " " + brackets + " Diffuseness sine drift : frequency, " + Nu_Sym;
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			text = struct_Data.material + " " + brackets + " Diffuseness sine srift : phase, " + Phi_Sym;

		// composition
		if(whats_This == whats_This_Interlayer_Composition)			text = struct_Data.material + " " + brackets + " Interlayer composition, " + transition_Layer_Functions[index];
		if(whats_This == whats_This_Interlayer_My_Sigma_Diffuse)	text = struct_Data.material + " " + brackets + " Individual diffuseness, s_" + transition_Layer_Functions[index];

		// roughness
		if(whats_This == whats_This_Sigma_Roughness)				text = struct_Data.material + " " + brackets + " Roughness, " + Sigma_Sym;
		if(whats_This == whats_This_Correlation_Radius)				text = struct_Data.material + " " + brackets + " Correlation radius, " + Xi_Sym;
		if(whats_This == whats_This_Fractal_Alpha)					text = struct_Data.material + " " + brackets + " Fractal parameter, " + Alpha_Sym;
		if(whats_This == whats_This_Linear_PSD_Omega)				text = struct_Data.material + " " + brackets + " Particle volume, " + Omega_Big_Sym;
		if(whats_This == whats_This_Linear_PSD_Exponenta_Mu)		text = struct_Data.material + " " + brackets + " Exponenta inheritance factor, " + Mu_Sym;
	}

	/// stack parameters
	if(	struct_Data.item_Type == item_Type_Multilayer )
	{
		if(whats_This == whats_This_Num_Repetitions)				text = brackets + " Number of repetitions, N";
		if(whats_This == whats_This_Period)							text = brackets + " Period, d";
		if(whats_This == whats_This_Gamma)							text = brackets + " Thickness ratio, " + Gamma_Sym;
	}
	// TODO
	if(	struct_Data.item_Type == item_Type_General_Aperiodic )
	{

	}
	if(	struct_Data.item_Type == item_Type_Regular_Aperiodic )
	{

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
	if(id == struct_Data.wavelength.indicator.id)								return &struct_Data.wavelength;
	if(id == struct_Data.beam_Theta_0_Angle.indicator.id)						return &struct_Data.beam_Theta_0_Angle;
	if(id == struct_Data.detector_Theta_Angle.indicator.id)						return &struct_Data.detector_Theta_Angle;
	if(id == struct_Data.detector_Phi_Angle.indicator.id)						return &struct_Data.detector_Phi_Angle;

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
	if(id == struct_Data.sigma_Diffuse.indicator.id)									return &struct_Data.sigma_Diffuse;
	for(int i=0; i<transition_Layer_Functions_Size; ++i)
	{
		if(id == struct_Data.interlayer_Composition[i].interlayer.indicator.id)	return &struct_Data.interlayer_Composition[i].interlayer;
		if(id == struct_Data.interlayer_Composition[i].my_Sigma_Diffuse.indicator.id)	return &struct_Data.interlayer_Composition[i].my_Sigma_Diffuse;
	}
	if(id == struct_Data.sigma_Diffuse_Drift.drift_Line_Value.indicator.id)				return &struct_Data.sigma_Diffuse_Drift.drift_Line_Value;
	if(id == struct_Data.sigma_Diffuse_Drift.drift_Rand_Rms.indicator.id)				return &struct_Data.sigma_Diffuse_Drift.drift_Rand_Rms;
	if(id == struct_Data.sigma_Diffuse_Drift.drift_Sine_Amplitude.indicator.id)			return &struct_Data.sigma_Diffuse_Drift.drift_Sine_Amplitude;
	if(id == struct_Data.sigma_Diffuse_Drift.drift_Sine_Frequency.indicator.id)			return &struct_Data.sigma_Diffuse_Drift.drift_Sine_Frequency;
	if(id == struct_Data.sigma_Diffuse_Drift.drift_Sine_Phase.indicator.id)				return &struct_Data.sigma_Diffuse_Drift.drift_Sine_Phase;

	// roughness
	if(id == struct_Data.roughness_Model.sigma.indicator.id)				return &struct_Data.roughness_Model.sigma;
	if(id == struct_Data.roughness_Model.cor_radius.indicator.id)			return &struct_Data.roughness_Model.cor_radius;
	if(id == struct_Data.roughness_Model.fractal_alpha.indicator.id)		return &struct_Data.roughness_Model.fractal_alpha;
	if(id == struct_Data.roughness_Model.omega.indicator.id)				return &struct_Data.roughness_Model.omega;
	if(id == struct_Data.roughness_Model.mu.indicator.id)					return &struct_Data.roughness_Model.mu;

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

	// optical properties
	if(whats_This == whats_This_Absolute_Density)				{*line_edit_precision = line_edit_density_precision;		*thumbnail_precision = thumbnail_density_precision;		*units = " " + density_units;	*coeff = 1;	return &struct_Data.absolute_Density;	}
	if(whats_This == whats_This_Relative_Density)				{*line_edit_precision = line_edit_density_precision;		*thumbnail_precision = thumbnail_density_precision;		*units = "" ;					*coeff = 1;	return &struct_Data.relative_Density;	}
	if(whats_This == whats_This_Permittivity)					{*line_edit_precision = line_edit_permittivity_precision;	*thumbnail_precision = thumbnail_permittivity_precision;*units = " " + opt_const_units;	*coeff = 1;	return &struct_Data.permittivity;		}
	if(whats_This == whats_This_Absorption)						{*line_edit_precision = line_edit_absorption_precision;		*thumbnail_precision = thumbnail_absorption_precision;	*units = " " + opt_const_units;	*coeff = 1;	return &struct_Data.absorption;			}
	if(whats_This == whats_This_Composition)					{*line_edit_precision = line_edit_composition_precision;	*thumbnail_precision = thumbnail_composition_precision;	*units = "";					*coeff = 1;	return nullptr;							}

	// interface
	if(whats_This == whats_This_Sigma_Diffuse)					{*line_edit_precision = line_edit_sigma_precision;			*thumbnail_precision = thumbnail_sigma_precision;		*units = " " + length_units;	*coeff = length_Coefficients_Map.value(length_units);	return &struct_Data.sigma_Diffuse;	}
	if(whats_This == whats_This_Sigma_Drift_Line_Value)			{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Diffuse_Drift.drift_Line_Value;		}
	if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Diffuse_Drift.drift_Rand_Rms;			}
	if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Diffuse_Drift.drift_Sine_Amplitude;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1; return &struct_Data.sigma_Diffuse_Drift.drift_Sine_Frequency;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			{*line_edit_precision = line_edit_drift_precision;			*thumbnail_precision = thumbnail_drift_precision;		*units = "";					*coeff = 1;	return &struct_Data.sigma_Diffuse_Drift.drift_Sine_Phase;		}
	if(whats_This == whats_This_Interlayer_Composition)			{*line_edit_precision = line_edit_interlayer_precision;		*thumbnail_precision = thumbnail_interlayer_precision;	*units = "";					*coeff = 1;	return nullptr;											}
	if(whats_This == whats_This_Interlayer_My_Sigma_Diffuse)	{*line_edit_precision = line_edit_sigma_precision;			*thumbnail_precision = thumbnail_sigma_precision;		*units = " " + length_units;	*coeff = length_Coefficients_Map.value(length_units);	return nullptr;	}

	// roughness
	if(whats_This == whats_This_Sigma_Roughness)				{*line_edit_precision = line_edit_sigma_precision;				*thumbnail_precision = thumbnail_sigma_precision;				*units = " " + length_units;			*coeff = length_Coefficients_Map.value(length_units);		return &struct_Data.roughness_Model.sigma;				}
	if(whats_This == whats_This_Correlation_Radius)				{*line_edit_precision = line_edit_cor_radius_precision;			*thumbnail_precision = thumbnail_cor_radius_precision;			*units = " " + length_units;			*coeff = length_Coefficients_Map.value(length_units);		return &struct_Data.roughness_Model.cor_radius;			}
	if(whats_This == whats_This_Fractal_Alpha)					{*line_edit_precision = line_edit_fractal_alpha_precision;		*thumbnail_precision = thumbnail_fractal_alpha_precision;		*units = "";							*coeff = 1;													return &struct_Data.roughness_Model.fractal_alpha;		}
	if(whats_This == whats_This_Linear_PSD_Omega)				{*line_edit_precision = line_edit_omega_precision;				*thumbnail_precision = thumbnail_omega_precision;				*units = " " + length_units+Cube_Sym;   *coeff = pow(length_Coefficients_Map.value(length_units),3);return &struct_Data.roughness_Model.omega;				}
	if(whats_This == whats_This_Linear_PSD_Exponenta_Mu)		{*line_edit_precision = line_edit_mu_precision;					*thumbnail_precision = thumbnail_mu_precision;					*units = " " + length_units;			*coeff = length_Coefficients_Map.value(length_units);		return &struct_Data.roughness_Model.mu;					}

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

void Global_Variables::enable_Disable_Roughness_Model(Data& struct_Data, const Imperfections_Model& imperfections_Model)
{
	// common
	if( struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate )
	{
		struct_Data.roughness_Model.is_Enabled = imperfections_Model.use_Roughness;
	}

	// detailed correction
	if( struct_Data.item_Type == item_Type_Layer )
	{
		if( imperfections_Model.common_Model == ABC_model ||
			imperfections_Model.common_Model == fractal_Gauss_Model )
		{
			if( imperfections_Model.vertical_Correlation == full_Correlation )
			{
				if(imperfections_Model.use_Common_Roughness_Function) // always
				{
					struct_Data.roughness_Model.is_Enabled = false;
				}
			}
			if( imperfections_Model.vertical_Correlation == zero_Correlation )
			{
				if(imperfections_Model.use_Common_Roughness_Function)
				{
					struct_Data.roughness_Model.is_Enabled = false;
				} else
				{
					// only sigma, ksi, alpha can be active
//					struct_Data.make_Free(struct_Data.roughness_Model.omega);
//					struct_Data.make_Free(struct_Data.roughness_Model.mu);
				}
			}
			if(imperfections_Model.vertical_Correlation == partial_Correlation)
			{
				if(imperfections_Model.use_Common_Roughness_Function) // always
				{
					// only mu can be active
//					struct_Data.make_Free(struct_Data.roughness_Model.sigma);
//					struct_Data.make_Free(struct_Data.roughness_Model.cor_radius);
//					struct_Data.make_Free(struct_Data.roughness_Model.fractal_alpha);
//					struct_Data.make_Free(struct_Data.roughness_Model.omega);
				}
			}
		}
		if( imperfections_Model.common_Model == linear_Growth_and_ABC_Model )
		{
			if(imperfections_Model.vertical_Correlation == partial_Correlation) // the only case
			{
				if(imperfections_Model.use_Common_Roughness_Function) // always
				{
					// only omega, mu, alpha can be active
//					struct_Data.make_Free(struct_Data.roughness_Model.sigma);
//					struct_Data.make_Free(struct_Data.roughness_Model.cor_radius);
				}
			}
		}
	}
	if( struct_Data.item_Type == item_Type_Substrate)
	{
		// only sigma, ksi, alpha can be active, always
//		struct_Data.make_Free(struct_Data.roughness_Model.omega);
//		struct_Data.make_Free(struct_Data.roughness_Model.mu);
	}
}

double Global_Variables::PSD_ABC_1D(double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc)
{
	Q_UNUSED(spline)
	Q_UNUSED(acc)
	double p = k*(cos_Theta-cos_Theta_0);
	return /*4*sqrt(M_PI) * tgamma(alpha+0.5)/tgamma(alpha) * sigma*sigma*xi*/ factor / pow(1+p*p*xi*xi, alpha+0.5);
}

double Global_Variables::PSD_ABC_2D(double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi)
{
	double nu2 = k*k*(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);
	return /*4*M_PI * sigma*sigma * xi*xi * alpha*/ factor / pow(1+nu2*xi*xi, alpha+1);
}

double Global_Variables::PSD_Real_Gauss_1D(double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc)
{
	Q_UNUSED(alpha)
	Q_UNUSED(spline)
	Q_UNUSED(acc)
	double p = k*(cos_Theta-cos_Theta_0) / 2.;
	return /*2*sqrt(M_PI) * sigma*sigma*xi*/ factor * exp(-p*p*xi*xi);
}

double Global_Variables::PSD_Real_Gauss_2D(double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi)
{
	Q_UNUSED(alpha)
	double nu2 = k*k*(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi) / 4.;
	return /*M_PI * sigma*sigma*xi*xi*/ factor * exp(-nu2*xi*xi);
}

double Global_Variables::PSD_Fractal_Gauss_1D(double sigma, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc)
{
	Q_UNUSED(sigma)
	Q_UNUSED(xi)
	Q_UNUSED(alpha)
	double p = k*abs(cos_Theta - cos_Theta_0);
	return gsl_spline_eval(spline, p, acc);
}

double Global_Variables::Cor_Fractal_Gauss(double xi, double alpha, double r)
{
	if(xi > 0)	return exp(-pow(r/xi, 2*alpha));
	else		return 0;
}
double Global_Variables::Cor_Fractal_Gauss_Pow_n(double xi, double alpha, double r, double factor, int n)
{
	Q_UNUSED(factor)
	if(xi > 0)	return exp(-n*pow(r/xi, 2*alpha));
	else		return 0;
}
double Global_Variables::Cor_ABC(double xi, double alpha, double r)
{
	if(xi > 0)	return pow(2,1-alpha)/tgamma(alpha) * pow(r/xi, alpha) * boost::math::cyl_bessel_k(alpha, r/xi);
	if(xi > 0)	return pow(2,1-alpha)/tgamma(alpha) * pow(r/xi, alpha) * cyl_bessel_k(alpha, r/xi);
	else		return 0;
}
double Global_Variables::Cor_ABC_Pow_n(double xi, double alpha, double r, double factor, int n)
{
	if(xi > 0)	return factor * pow(pow(r/xi, alpha)*boost::math::cyl_bessel_k(alpha, r/xi),n);
//	if(xi > 0)	return factor * pow(pow(r/xi, alpha)*cyl_bessel_k(alpha, r/xi),n);
	else		return 0;
}

double Global_Variables::splined_Value_1D(double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc)
{
	double p = k*abs(cos_Theta - cos_Theta_0);
	return gsl_spline_eval(spline, p, acc);
}

double Global_Variables::nu_Alpha_2D(double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi)
{
	double nu2 = k*k*(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi) / (4*M_PI*M_PI);
	return 2*pow(nu2,alpha+1);
}

double Global_Variables::PSD_Linear_Growth_2D(double exponent, double nu2_mu_Alpha_2, double omega, double nu2_omega_23, double thickness)
{
	if(nu2_mu_Alpha_2 > DBL_EPSILON)
	{
		if(nu2_omega_23<2)	return omega * (1-exponent) / nu2_mu_Alpha_2;  // /(4*M_PI*M_PI)
		else				return 0;
	} else
	{
		return omega * thickness;  // /(4*M_PI*M_PI)
	}
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
			{
				global_Multilayer_Approach->save(last_file);
			}
			else
			{
				QString path = "";
				if(use_working_directory) path = working_directory + "/";
				if(use_last_directory)	  path = last_directory + "/";
				global_Multilayer_Approach->save(path + default_File);
			}
		});
		connect(save_As_Shortcut,			&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->save_As();					 });
		connect(open_Shortcut,				&QShortcut::activated, this_Widget, [=]
		{
			if(global_Multilayer_Approach->file_Was_Opened_or_Saved)
			{
				global_Multilayer_Approach->open(last_file);
			}
			else
			{
				QString path = "";
				if(use_working_directory) path = working_directory + "/";
				if(use_last_directory)	  path = last_directory + "/";
				global_Multilayer_Approach->open(path + default_File);
			}
		});
		connect(open_As_Shortcut,			&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->open_As();					 });
		connect(fit_Shortcut,				&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->start_Fitting();		     });
		connect(calc_Specular_Shortcut,		&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->calculate();				 });
		connect(calc_Confidence_Shortcut,	&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->calc_Confidence_Intervals();});
		connect(abort_Shortcut,				&QShortcut::activated, this_Widget, [=]{ global_Multilayer_Approach->abort_Calculations();		 });
	}
}

void Global_Variables::parallel_For(int num_Points, int num_Threads, const std::function<void(int n_Min, int n_Max, int thread_Index)>& lambda)
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
		global_Workers[thread_Index] = std::thread(lambda, n_Min, n_Max, thread_Index);
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
	plot_All_Data_in_1D_Graphs();
	plot_All_Data_in_2D_Graphs();
}

void Global_Variables::plot_All_Data_in_1D_Graphs()
{
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		for(QVector<Curve_Plot_1D*>& tab_Plots : global_Multilayer_Approach->optical_Graphs_1D->plots_1D)
		{
			for(Curve_Plot_1D* curve_Plot : tab_Plots)
			{
				curve_Plot->plot_All_Data();
			}
		}
	}
}

void Global_Variables::plot_All_Data_in_2D_Graphs()
{
	if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
	{
		for(QVector<Curve_Plot_2D*>& tab_Plots : global_Multilayer_Approach->optical_Graphs_2D->plots_2D)
		{
			for(Curve_Plot_2D* curve_Plot : tab_Plots)
			{
				curve_Plot->plot_All_Data();
			}
		}
	}
}

void Global_Variables::plot_All_Data_in_Profiles()
{
	if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))
	{
		for(Profile_Plot* profile_Plot : global_Multilayer_Approach->profile_Plots_Window->profile_Plot_Vector)
		{
			profile_Plot->plot_Data(true);
		}
	}
}

void Global_Variables::variable_Drift(double &value, Drift &drift, int period_Index, int num_Repetition, gsl_rng* r)
{
	double drift_Factor = 1;
	double period_Index_From_Middle = period_Index - (num_Repetition/2.-0.5);
	if(drift.is_Drift_Line)
	{
		drift_Factor = drift_Factor + drift.drift_Line_Value.value*period_Index_From_Middle/100.;
	}
	if(drift.is_Drift_Sine)
	{
		drift_Factor = drift_Factor + drift.drift_Sine_Amplitude.value*sin(2*M_PI*(period_Index_From_Middle*drift.drift_Sine_Frequency.value + drift.drift_Sine_Phase.value))/100.;
	}
	if(drift.is_Drift_Rand && r!=nullptr)
	{
		drift_Factor = drift_Factor + gsl_ran_gaussian(r, drift.drift_Rand_Rms.value)/100.;
	}
	if(abs(drift_Factor)>=1E10) drift_Factor=1; // crutch
	if(drift_Factor>=0)
	{
		value *= drift_Factor;
	} else
	{
		qInfo() << "==========\nUnwrapped_Structure::variable_Drift  :  negative drifted variable!\n==========" << endl;
		value *= abs(drift_Factor);
	}
}

void Global_Variables::get_Prefix_Suffix(double& prefix, double& suffix, double max_Sigma)
{
	prefix = max(15., 5+3*max_Sigma);	// in angstroms
	suffix = max(15., 5+3*max_Sigma);	// in angstroms
}

void Global_Variables::discretize_Prefix_Suffix(double prefix, double suffix, int& num_Prefix_Slices, int& num_Suffix_Slices, vector<double>& discrete_Step_Vector, double default_Step)
{
	double adapted_Prefix_Step, adapted_Suffix_Step;
	if(discrete_Step_Vector.size()==0)
	{
		adapted_Prefix_Step = default_Step;
		adapted_Suffix_Step = default_Step;
	} else
	{
		adapted_Prefix_Step = discrete_Step_Vector.front();
		adapted_Suffix_Step = discrete_Step_Vector.back();
	}
	num_Prefix_Slices = ceil(prefix/adapted_Prefix_Step);
	num_Suffix_Slices = ceil(suffix/adapted_Suffix_Step);
	for(int i=0; i<num_Prefix_Slices; i++) {discrete_Step_Vector.insert(discrete_Step_Vector.begin(), adapted_Prefix_Step);}
	for(int i=0; i<num_Suffix_Slices; i++) {discrete_Step_Vector.push_back(adapted_Suffix_Step);}
}

void Global_Variables::get_Peak_Parameters(const vector<double>& argument, const vector<double>& values, double& max_Value_Position, double& max_Value, double& width)
{
	QVector<double> qValues = QVector<double>(values.begin(), values.end());
	max_Value = *std::max_element(values.begin(), values.end());
	int max_Value_Position_Index = qValues.indexOf(max_Value);
	max_Value_Position = argument[max_Value_Position_Index];

	// go left
	int i_0=0;
	double left_Wall_Position = argument.front();
	for(int i=max_Value_Position_Index; i>=0; i--)
	{
		if(values[i]<=max_Value/2)
		{
			left_Wall_Position = argument[i];
			i_0 = i;
			break;
		}
	}
	double curr_Value = values[i_0];
	double prev_Value = values[i_0+1];
	double a = abs(curr_Value-max_Value/2);
	double b = abs(prev_Value-max_Value/2);
	left_Wall_Position = argument[i_0]+a/(a+b)*(argument[i_0+1]-argument[i_0]);

	// go right
	double right_Wall_Position = argument.back();;
	for(int i=max_Value_Position_Index; i<values.size(); i++)
	{
		if(values[i]<=max_Value/2)
		{
			right_Wall_Position = argument[i];
			i_0 = i;
			break;
		}
	}
	curr_Value = values[i_0];
	prev_Value = values[i_0-1];
	a = abs(curr_Value-max_Value/2);
	b = abs(prev_Value-max_Value/2);
	right_Wall_Position = argument[i_0]+a/(a+b)*(argument[i_0-1]-argument[i_0]);

	width = abs(right_Wall_Position-left_Wall_Position);
}

double Global_Variables::theta_Function(double z)
{
	if(z<0)  return 0;
	if(z==0) return 0.5;
	return 1;
}

double Global_Variables::erf_Profile(double z, double sigma)
{
	if(abs(sigma)>DBL_EPSILON)	{return 0.5+0.5*erf(z/(sqrt(2)*sigma));}
	return theta_Function(z);
}

double Global_Variables::lin_Profile(double z, double sigma)
{
	if(abs(sigma)>DBL_EPSILON)
	{
		if( z< -sqrt(3)*sigma )	{return 0;}
		if((z>=-sqrt(3)*sigma ) &&
		   (z<= sqrt(3)*sigma))	{return 0.5+z/(2*sqrt(3)*sigma);}
		if( z>  sqrt(3)*sigma )	{return 1;}
	}
	return theta_Function(z);
}

double Global_Variables::exp_Profile(double z, double sigma)
{
	if(abs(sigma)>DBL_EPSILON)
	{
		if( z<0  )	{return 0.5*exp(sqrt(2)*z/sigma);}
		if( z>=0 )	{return 1-0.5*exp(-sqrt(2)*z/sigma);}
	}
	return theta_Function(z);
}

double Global_Variables::tanh_Profile(double z, double sigma)
{
	if(abs(sigma)>DBL_EPSILON)
	{
		return 0.5+0.5*tanh(M_PI*z/(2*sqrt(3)*sigma));
	}
	return theta_Function(z);
}

double Global_Variables::sin_Profile(double z, double sigma)
{
	double g = M_PI/sqrt(M_PI*M_PI - 8.);

	if(abs(sigma)>DBL_EPSILON)
	{
		if( z< -g*sigma )	{return 0;}
		if((z>=-g*sigma ) &&
		   (z<= g*sigma))	{return 0.5+0.5*sin(M_PI*z/(2*g*sigma));}
		if( z>  g*sigma )	{return 1;}
	}
	return theta_Function(z);
}

double Global_Variables::step_Profile(double z, double sigma)
{
	if(abs(sigma)>DBL_EPSILON)
	{
		if( z< -sigma )	{return 0;}
		if((z>=-sigma ) &&
		   (z<= sigma))	{return 0.5;}
		if( z>  sigma )	{return 1;}
	}
	return theta_Function(z);
}

double Global_Variables::interface_Profile_Function(double z, const QVector<Interlayer>& interlayer_Composition, bool for_Integration)
{
	double output = 0;

	// temp variables
	double norm = 0;
	double my_Sigma = 0;

	//-------------------------------------------------------------------------------
	// erf interlayer
	if(interlayer_Composition[Erf].enabled)
	if(interlayer_Composition[Erf].interlayer.value > DBL_MIN)
	{
		norm += interlayer_Composition[Erf].interlayer.value;
		my_Sigma = interlayer_Composition[Erf].my_Sigma_Diffuse.value;

		output += erf_Profile(z, my_Sigma) * interlayer_Composition[Erf].interlayer.value;
	}
	//-------------------------------------------------------------------------------
	// lin interlayer
	if(interlayer_Composition[Lin].enabled)
	if(interlayer_Composition[Lin].interlayer.value > DBL_MIN)
	{
		norm += interlayer_Composition[Lin].interlayer.value;
		my_Sigma = interlayer_Composition[Lin].my_Sigma_Diffuse.value;

		if(for_Integration)
		{
			output += sin_Profile(z, my_Sigma) * interlayer_Composition[Lin].interlayer.value;  // otherwise integration crashes
		} else
		{
			output += lin_Profile(z, my_Sigma) * interlayer_Composition[Lin].interlayer.value;
		}
	}
	//-------------------------------------------------------------------------------
	// exp interlayer
	if(interlayer_Composition[Exp].enabled)
	if(interlayer_Composition[Exp].interlayer.value > DBL_MIN)
	{
		norm += interlayer_Composition[Exp].interlayer.value;
		my_Sigma = interlayer_Composition[Exp].my_Sigma_Diffuse.value;

		output += exp_Profile(z, my_Sigma) * interlayer_Composition[Exp].interlayer.value;
	}
	//-------------------------------------------------------------------------------
	// tanh interlayer
	if(interlayer_Composition[Tanh].enabled)
	if(interlayer_Composition[Tanh].interlayer.value > DBL_MIN)
	{
		norm += interlayer_Composition[Tanh].interlayer.value;
		my_Sigma = interlayer_Composition[Tanh].my_Sigma_Diffuse.value;

		output += tanh_Profile(z, my_Sigma) * interlayer_Composition[Tanh].interlayer.value;
	}
	//-------------------------------------------------------------------------------
	// sin interlayer
	if(interlayer_Composition[Sin].enabled)
	if(interlayer_Composition[Sin].interlayer.value > DBL_MIN)
	{
		norm += interlayer_Composition[Sin].interlayer.value;
		my_Sigma = interlayer_Composition[Sin].my_Sigma_Diffuse.value;

		output += sin_Profile(z, my_Sigma) * interlayer_Composition[Sin].interlayer.value;
	}
	//-------------------------------------------------------------------------------
	// step interlayer
	if(interlayer_Composition[Step].enabled)
	if(interlayer_Composition[Step].interlayer.value > DBL_MIN)
	{
		norm += interlayer_Composition[Step].interlayer.value;
		my_Sigma = interlayer_Composition[Step].my_Sigma_Diffuse.value;

		if(for_Integration)
		{
			output += sin_Profile(z, my_Sigma) * interlayer_Composition[Step].interlayer.value;  // otherwise integration crashes
		} else
		{
			output += step_Profile(z, my_Sigma) * interlayer_Composition[Step].interlayer.value;
		}
	}
	//-------------------------------------------------------------------------------
	// normalization

	if( norm > DBL_MIN ) {output /= norm;}
	else				 {output = theta_Function(z);}

	return output;
}

struct f_params
{
	double thickness;
	QVector<Interlayer> left_Interlayer_Composition;
	QVector<Interlayer> right_Interlayer_Composition;
};

double f(double z, void* p)
{
	f_params& params= *reinterpret_cast<f_params*>(p);

	double output = Global_Variables::interface_Profile_Function(                 z, params.left_Interlayer_Composition ,true) *
					Global_Variables::interface_Profile_Function(params.thickness-z, params.right_Interlayer_Composition,true);

	return output;
}

double Global_Variables::get_Max_Sigma_From_Interlayer_Composition(QVector<Interlayer>& interlayer_Composition)
{
	double max_Sigma = 0;
	for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
	{
		if(interlayer_Composition[interlayer_Index].enabled)
		{
			if(max_Sigma<interlayer_Composition[interlayer_Index].my_Sigma_Diffuse.value)
			{
				max_Sigma=interlayer_Composition[interlayer_Index].my_Sigma_Diffuse.value;
			}
		}
	}
	return max_Sigma;
}

double Global_Variables::layer_Normalization(double thickness, QVector<Interlayer>& left_Interlayer_Composition, QVector<Interlayer>& right_Interlayer_Composition, gsl_integration_workspace* w)
{
	f_params params;
	params.thickness = thickness;
	params.left_Interlayer_Composition = left_Interlayer_Composition;
	params.right_Interlayer_Composition= right_Interlayer_Composition;

	gsl_function F;
	F.function = &f;
	F.params = reinterpret_cast<void*>(&params);

	double result=0, error=0;
	size_t limit=10;

	double max_Sigma_Left  = get_Max_Sigma_From_Interlayer_Composition(left_Interlayer_Composition );
	double max_Sigma_Right = get_Max_Sigma_From_Interlayer_Composition(right_Interlayer_Composition);

	const double xlow =0        -3*max_Sigma_Left;
	const double xhigh=thickness+3*max_Sigma_Right;
	const double epsabs=1e-5;
	const double epsrel=1e-5;


	vector<double> pts = {xlow,xhigh};
	int code = 0;
	if(thickness<3*(max_Sigma_Left+max_Sigma_Right))
	{
		code=gsl_integration_qag( &F,
								  xlow,
								  xhigh,
								  epsabs,
								  epsrel,
								  limit,
								  GSL_INTEG_GAUSS61,
								  w,
								  &result,
								  &error);
	} else
	{
		result = thickness;
	}

//	if(code)
//	{
//	  qInfo() << "There was a problem with integration: code " << code << endl;
//	}
//	else
//	{
//	  qInfo() << "Result" << result << "+/-" << error << "from" << neval << "evaluations" << endl;
//	}
	return result;
}

double Global_Variables::beam_Profile(double x, double FWHM, double smoothing)
{
	if(smoothing < DBL_EPSILON)
	{
		if(x <  -FWHM/2) return 0;
		if(x == -FWHM/2) return 0.5;
		if(-FWHM/2 < x && x < FWHM/2) return 1;
		if(x ==  FWHM/2) return 0.5;
		if(x >   FWHM/2) return 0;
	} else
	{
		double limit = FWHM/2+1.5*smoothing;
		if(x <= -limit) return 0;
		if(-limit < x && x < limit)
		{
			double x_Factor = 4*smoothing*acosh(0.5*sqrt(2+2*sqrt(2+pow(sinh(FWHM/smoothing),2))))/FWHM;

			return (1+tanh((x*x_Factor+FWHM)/smoothing))*(1+tanh((-x*x_Factor+FWHM)/smoothing))/pow(1+tanh(FWHM/smoothing),2);
		}
		if(x >=  limit) return 0;
	}
}

double Global_Variables::distribution_Function(QString function, double FWHM, double x)
{
	if(function == distributions[Gate])		{return distribution_Gate	 (FWHM, x);} else
	if(function == distributions[Cosine])	{return distribution_Cosine	 (FWHM, x);} else
	if(function == distributions[Gaussian]) {return distribution_Gaussian(FWHM, x);} else
	if(function == distributions[Lorentz])	{return distribution_Lorentz (FWHM, x);} else
	return -2020;
}

double Global_Variables::distribution_Gate(double FWHM, double x)
{
	if(-FWHM/2-DBL_EPSILON<= x && x <= FWHM/2+DBL_EPSILON) return 1;
	else							return 0;
}

double Global_Variables::distribution_Cosine(double FWHM, double x)
{
	if(-FWHM <= x && x <= FWHM)
	{
		if(FWHM>DBL_EPSILON)
		{
			return pow(cos(M_PI_2*x/FWHM),2);
		} else
		{
			return 1;
		}
	}
	else
		return 0;
}

double Global_Variables::distribution_Gaussian(double FWHM, double x)
{
	if(-1.5*FWHM <= x && x <= 1.5*FWHM)
	{
		if(FWHM>DBL_EPSILON)
		{
			return exp(-4*log(2)*pow(x/FWHM,2));
		} else
		{
			return 1;
		}
	}
	else
		return 0;
}

double Global_Variables::distribution_Lorentz(double FWHM, double x)
{
	if(-10*FWHM <= x && x <= 10*FWHM)
	{
		if(FWHM>DBL_EPSILON)
		{
			return 1./(1.+pow(2*x/FWHM,2));
		} else
		{
			return 1;
		}
	}
	else
		return 0;
}

void Global_Variables::color_Scheme_Change(QCPColorMap* color_Map, QCustomPlot* main_Plot, QCPColorGradient::GradientPreset* color_Scheme)
{
	main_Plot->blockSignals(true);
	QDialog* choice_Color_Scheme_Window = new QDialog(main_Plot);
		choice_Color_Scheme_Window->setWindowTitle("Color scheme");
		choice_Color_Scheme_Window->setWindowModality(Qt::ApplicationModal);
		choice_Color_Scheme_Window->setAttribute(Qt::WA_DeleteOnClose);
		choice_Color_Scheme_Window->setWindowFlags(Qt::Tool);
		choice_Color_Scheme_Window->show();

	QGridLayout* choice_Color_Scheme_Layout = new QGridLayout(choice_Color_Scheme_Window);
		choice_Color_Scheme_Layout->setSizeConstraint(QLayout::SetFixedSize);
		choice_Color_Scheme_Layout->setSpacing(5);
		choice_Color_Scheme_Layout->setContentsMargins(5,5,5,5);

	// group box
	QGroupBox*  choice_Data_Color_Scheme_Box = new QGroupBox;
		choice_Data_Color_Scheme_Box->setObjectName("choice_Data_Color_Scheme_Box");
		choice_Data_Color_Scheme_Box->setStyleSheet("QGroupBox#choice_Data_Color_Scheme_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");

	// inside group box
	{
		QGridLayout* choice_Data_Type_Group_Box_Layout = new QGridLayout(choice_Data_Color_Scheme_Box);

		int col = 0;
		QCPColorGradient::GradientPreset preset;
		QButtonGroup* button_Group = new QButtonGroup;
		// Grayscale
		{
			preset = QCPColorGradient::gpGrayscale;
			QCustomPlot* plot = new QCustomPlot;
			color_Scheme_Example(plot, preset);
			choice_Data_Type_Group_Box_Layout->addWidget(plot,0,col);

			// ------------------------------------------------

			QRadioButton* radio_Button = new QRadioButton("Grayscale");
				radio_Button->setChecked(*color_Scheme == preset);
			button_Group->addButton(radio_Button);
			choice_Data_Type_Group_Box_Layout->addWidget(radio_Button,1,col,Qt::AlignHCenter);
			connect(radio_Button, &QRadioButton::clicked, [=]
			{
				*color_Scheme = preset;
				color_Map->setGradient(preset);
				main_Plot->replot();
			});
			col++;
		}
		// Spectrum
		{
			preset = QCPColorGradient::gpSpectrum;
			QCustomPlot* plot = new QCustomPlot;
			color_Scheme_Example(plot, preset);
			choice_Data_Type_Group_Box_Layout->addWidget(plot,0,col);

			// ------------------------------------------------

			QRadioButton* radio_Button = new QRadioButton("Spectrum");
				radio_Button->setChecked(*color_Scheme == preset);
			button_Group->addButton(radio_Button);
			choice_Data_Type_Group_Box_Layout->addWidget(radio_Button,1,col,Qt::AlignHCenter);
			connect(radio_Button, &QRadioButton::clicked, [=]
			{
				*color_Scheme = preset;
				color_Map->setGradient(preset);
				main_Plot->replot();
			});
			col++;
		}
		// Jet
		{
			preset = QCPColorGradient::gpJet;
			QCustomPlot* plot = new QCustomPlot;
			color_Scheme_Example(plot, preset);
			choice_Data_Type_Group_Box_Layout->addWidget(plot,0,col);

			// ------------------------------------------------

			QRadioButton* radio_Button = new QRadioButton("Jet");
				radio_Button->setChecked(*color_Scheme == preset);
			button_Group->addButton(radio_Button);
			choice_Data_Type_Group_Box_Layout->addWidget(radio_Button,1,col,Qt::AlignHCenter);
			connect(radio_Button, &QRadioButton::clicked, [=]
			{
				*color_Scheme = preset;
				color_Map->setGradient(preset);
				main_Plot->replot();
			});
			col++;
		}
		// Polar
		{
			preset = QCPColorGradient::gpPolar;
			QCustomPlot* plot = new QCustomPlot;
			color_Scheme_Example(plot, preset);
			choice_Data_Type_Group_Box_Layout->addWidget(plot,0,col);

			// ------------------------------------------------

			QRadioButton* radio_Button = new QRadioButton("Polar");
				radio_Button->setChecked(*color_Scheme == preset);
			button_Group->addButton(radio_Button);
			choice_Data_Type_Group_Box_Layout->addWidget(radio_Button,1,col,Qt::AlignHCenter);
			connect(radio_Button, &QRadioButton::clicked, [=]
			{
				*color_Scheme = preset;
				color_Map->setGradient(preset);
				main_Plot->replot();
			});
			col++;
		}
		// Thermal
		{
			preset = QCPColorGradient::gpThermal;
			QCustomPlot* plot = new QCustomPlot;
			color_Scheme_Example(plot, preset);
			choice_Data_Type_Group_Box_Layout->addWidget(plot,0,col);

			// ------------------------------------------------

			QRadioButton* radio_Button = new QRadioButton("Thermal");
				radio_Button->setChecked(*color_Scheme == preset);
			button_Group->addButton(radio_Button);
			choice_Data_Type_Group_Box_Layout->addWidget(radio_Button,1,col,Qt::AlignHCenter);
			connect(radio_Button, &QRadioButton::clicked, [=]
			{
				*color_Scheme = preset;
				color_Map->setGradient(preset);
				main_Plot->replot();
			});
			col++;
		}
	}

	// close button
	QPushButton* close_Button = new QPushButton("Close");
		close_Button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
		close_Button->setAutoDefault(true);
	choice_Color_Scheme_Layout->addWidget(close_Button,1,0,Qt::AlignHCenter);
	choice_Color_Scheme_Layout->addWidget(choice_Data_Color_Scheme_Box,0,0);
	connect(close_Button, &QPushButton::clicked, choice_Color_Scheme_Window, &QDialog::close);

	// crutch
	choice_Color_Scheme_Window->setGeometry(QApplication::desktop()->screen()->width()/2-652,
											QApplication::desktop()->screen()->height()/2-167,
											choice_Color_Scheme_Window->width(),
											choice_Color_Scheme_Window->height());
	main_Plot->blockSignals(false);
}

void Global_Variables::color_Scheme_Example(QCustomPlot* plot, QCPColorGradient::GradientPreset preset)
{
	plot->setMinimumSize(250,250);

	plot->setNoAntialiasingOnDrag(false);

	// frame
	plot->xAxis2->setVisible(true);
	plot->yAxis2->setVisible(true);
	plot->xAxis->setTicks(false);
	plot->xAxis2->setTicks(false);
	plot->yAxis->setTicks(false);
	plot->yAxis2->setTicks(false);

	// add a color scale:
	QCPColorMap* map = new QCPColorMap(plot->xAxis, plot->yAxis);
	map->setTightBoundary(true);
	map->setGradient(preset);

	// data
	int nx = 100;
	int ny = 100;
	map->data()->setSize(nx, ny);
	map->data()->setRange(QCPRange(-7, 7), QCPRange(-7, 7));
	double x,y;
	for (int x_Index=0; x_Index<nx; ++x_Index)
	{
		for (int y_Index=0; y_Index<ny; ++y_Index)
		{
			map->data()->cellToCoord(x_Index, y_Index, &x, &y);
			double r = sqrt(x*x+y*y)/r;
			double val = sin(x)*sin(y);
			map->data()->setCell(x_Index, y_Index, val);
		}
	}
	map->rescaleAxes();
	map->rescaleDataRange(); // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient
	plot->replot();
}
