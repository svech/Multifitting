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
QStringList transition_Layer_Functions	{"erf","lin","exp","tanh","sin"}; size_t transition_Layer_Functions_Size = transition_Layer_Functions.size();
//QStringList thickness_Drift_Functions	{"no grading","linear","parabolic","sinusoidal"}; size_t thickness_Drift_Functions_Size = thickness_Drift_Functions.size();
//QStringList drift_Models				{"no_drift","lin_sin","par_sin","hyp_sin","exp_sin","log_sin"};
QStringList tril						{TRIL_TRUE, TRIL_FALSE, TRIL_NOT_DEFINED};
// -----------------------------------------------------------------------------------------

// units

// length
QStringList   wavelength_Units_List				{Angstrom_Sym,"nm",Mu_Sym + "m","eV","keV"};
QList<double> wavelength_Coefficients_List		{1           ,1E1 ,1E4         ,1   ,1E3  };
QMap<QString, double>  wavelength_Coefficients_Map;

// length
QStringList   length_Units_List					{Angstrom_Sym,"nm",Mu_Sym + "m"};
QList<double> length_Coefficients_List			{1           ,1E1 ,1E4         };
QMap<QString, double>  length_Coefficients_Map;

// angle
QStringList   angle_Units_List				{Degree_Sym,Prime_Sym   ,Double_Prime_Sym," rad"   ," mrad"   };
QStringList   angle_Units_Legend_List		{"degree"  ,"arc minute","arc second"    ,"radian","mrad"   };
QList<double> angle_Coefficients_List		{1		   ,1./60       ,1./3600         ,180/M_PI,0.18/M_PI};
QMap<QString, double>  angle_Coefficients_Map;
QMap<QString, QString> angle_Units_Legend_Map;

// density
QStringList density_Units_List				{"g/cm" + Cube_Sym};

// optical constants
QStringList opt_Const_Units_List			{"% of nominal"};

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
	// count
	int num_Items=0;
	QTreeWidgetItemIterator it0(tree);
	while(*it0)
	{
		++it0;
		++num_Items;
	}
	out << num_Items;

	// write
	QTreeWidgetItemIterator it(tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;
		QStringList whats_This_List = item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);

		// whatsThis
		out << item->whatsThis(DEFAULT_COLUMN);

		// id
		out << item->statusTip(DEFAULT_COLUMN);

		// data
		if(whats_This_List[0] == whats_This_Measurement)out << qvariant_cast<Measurement>	(item->data(DEFAULT_COLUMN, Qt::UserRole));
		if(whats_This_List[0] == whats_This_Ambient)	out << qvariant_cast<Ambient>		(item->data(DEFAULT_COLUMN, Qt::UserRole));
		if(whats_This_List[0] == whats_This_Layer)		out << qvariant_cast<Layer>			(item->data(DEFAULT_COLUMN, Qt::UserRole));
		if(whats_This_List[0] == whats_This_Multilayer)	out << qvariant_cast<Stack_Content>	(item->data(DEFAULT_COLUMN, Qt::UserRole));
		if(whats_This_List[0] == whats_This_Substrate)	out << qvariant_cast<Substrate>		(item->data(DEFAULT_COLUMN, Qt::UserRole));


		// parent's whatsThis
		if(item->parent())
			out << item->parent()->whatsThis(DEFAULT_COLUMN);
		else
			out << QString(NOPARENT);
		++it;
	}
}

void Global_Variables::deserialize_Tree(QDataStream& in, QTreeWidget* tree)
{
	// count
	int num_Items=0;
	in >> num_Items;

	tree->clear();
	QVector<QTreeWidgetItem*> item_Vec;
	QStringList whatsThis_Parent_List;

	QMap<QString, QTreeWidgetItem*> tree_Map;

	tree_Map.insert(NOPARENT, tree->invisibleRootItem());
	for(int i=0; i<num_Items; ++i)
	{
		// create item
		QTreeWidgetItem* item = new QTreeWidgetItem;

		// read

		// whatsThis
		QString whatsThis;
		in >> whatsThis;
		item->setWhatsThis(DEFAULT_COLUMN,whatsThis);

		// id
		QString id;
		in >> id;
		item->setStatusTip(DEFAULT_COLUMN, id);

		// data
		QStringList whats_This_List = whatsThis.split(item_Type_Delimiter,QString::SkipEmptyParts);
		QVariant var;
		if(whats_This_List[0] == whats_This_Measurement){Measurement	measurement;	in >> measurement;	var.setValue(measurement);	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);}
		if(whats_This_List[0] == whats_This_Ambient)	{Ambient		ambient;		in >> ambient;		var.setValue(ambient);		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);}
		if(whats_This_List[0] == whats_This_Layer)		{Layer			layer;			in >> layer;		var.setValue(layer);		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);}
		if(whats_This_List[0] == whats_This_Multilayer)	{Stack_Content	stack_Content;	in >> stack_Content;var.setValue(stack_Content);item->setData(DEFAULT_COLUMN, Qt::UserRole, var);}
		if(whats_This_List[0] == whats_This_Substrate)	{Substrate		substrate;		in >> substrate;	var.setValue(substrate);	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);}

		// parent's whatsThis
		QString whatsThis_Parent;
		in >> whatsThis_Parent;

		tree_Map.insert(whatsThis, item);
		item_Vec.append(item);
		whatsThis_Parent_List.append(whatsThis_Parent);
	}

	for(int i=0; i<item_Vec.size(); ++i)
	{
		tree_Map.value(whatsThis_Parent_List[i])->addChild(item_Vec[i]);
	}
}

void Global_Variables::serialize_Variables_List(QDataStream& out, QListWidget* list)
{
	// save number of items
	out << list->count();

	for(int i=0; i<list->count(); ++i)
	{
		QListWidgetItem* temp_Item = list->item(i);

		// save whatsThis
		out << temp_Item->whatsThis();

		// save text
		out << temp_Item->text();

		// save active
		out << temp_Item->data(Qt::UserRole).toBool();
	}
}

void Global_Variables::deserialize_Variables_List(QDataStream& in, QListWidget* list)
{
	// load number of items
	int num_Items;
	in >> num_Items;

	list->clear();

	for(int i=0; i<num_Items; ++i)
	{
		QListWidgetItem* new_Item = new QListWidgetItem;
		list->insertItem(i,new_Item);

		// load whatsThis
		QString whats_This;
		in >> whats_This;
		new_Item->setWhatsThis(whats_This);

		// load text
		QString text;
		in >> text;
		new_Item->setText(text);

		// load active
		bool is_Active;
		in >> is_Active;
		new_Item->setData(Qt::UserRole,is_Active);
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
