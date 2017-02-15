// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "grading_editor.h"

Grading_Editor::Grading_Editor(QTreeWidgetItem* item, QString parameter_Name, QWidget *parent):
    item(item),
    parameter_Name(parameter_Name),
    QDialog(parent)
{
    create_Main_Layout();

    setWindowTitle(parameter_Name + " Grading");
    set_Window_Geometry();
    setAttribute(Qt::WA_DeleteOnClose);
}

void Grading_Editor::emit_Refresh()
{
    emit refresh();
}

void Grading_Editor::create_Main_Layout()
{
    main_Layout = new QVBoxLayout(this);
    main_Layout->setSpacing(0);
    main_Layout->setContentsMargins(4,0,4,0);

    create_Interface();
        main_Layout->addWidget(group_Box);

    done_Button = new QPushButton("Done", this);
        done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        done_Button->setFocus();
        done_Button->setDefault(true);
    main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

    connect(done_Button, SIGNAL(clicked()), this, SLOT(close()));
}

void Grading_Editor::create_Interface()
{
    group_Box = new QGroupBox(this);
        group_Box->setObjectName("group_Box");
        group_Box->setStyleSheet("QGroupBox#group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
                                 "QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");

    group_Box_Layout = new QVBoxLayout(group_Box);
        group_Box_Layout->setAlignment(Qt::AlignLeft);

    QHBoxLayout* layout = new QHBoxLayout;
        layout->setAlignment(Qt::AlignLeft);
    group_Box_Layout->addLayout(layout);

    read_Drift_From_Item();

//    layout->addWidget(line_Check);
}

void Grading_Editor::read_Drift_From_Item()
{
    qInfo() << "wtf : " << item->whatsThis(0);
    Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

//    Parameter parameter;
//    if(parameter_Name==whats_This_Thickness) { parameter = layer.thickness;}
//    if(parameter_Name==whats_This_Sigma)     { parameter = layer.sigma;    }

//    line_Check = new QCheckBox("Linear Drift",this);
//    line_Line

}

void Grading_Editor::set_Window_Geometry()
{
    adjustSize();
    setFixedSize(size());
}
