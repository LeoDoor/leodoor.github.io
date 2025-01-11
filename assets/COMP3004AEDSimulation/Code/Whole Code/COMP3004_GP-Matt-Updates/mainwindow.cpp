#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QPixmap>
#include <QThread>
#include <QApplication>
#include <QTime>

#include <string>
#include <iostream>
#include <dirent.h>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Loads the pixmaps onto the appropriate labels
    load_photos();

    // Puts the radio buttons in a list for easier access
    radioButtons[0] = ui->Check_R_RB;
    radioButtons[1] = ui->Call_ES_RB;
    radioButtons[2] = ui->Setup_Pads_RB;
    radioButtons[3] = ui->Dont_Touch_RB;
    radioButtons[4] = ui->Compressions_RB;
    radioButtons[5] = ui->M2M_RB;


    connect(ui->Power_Button, SIGNAL(clicked()), this, SLOT (on_pressed()));
    connect(ui->Check_R_Button, SIGNAL(clicked()), this, SLOT (check_res_pressed()));
    connect(ui->Call_ES_Button, SIGNAL(clicked()), this, SLOT (call_es_pressed()));

    connect(ui->Setup_Adult_Pads_Button, SIGNAL(clicked()), this, SLOT (setup_Apads_pressed()));
    connect(ui->Setup_Adult_Wide_Pads_Button, SIGNAL(clicked()), this, SLOT (setup_ApadsWide_pressed()));
    connect(ui->Setup_Child_Pads_Button, SIGNAL(clicked()), this, SLOT (setup_Cpads_pressed()));
    connect(ui->Setup_Infant_Pads_Button, SIGNAL(clicked()), this, SLOT (setup_Ipads_pressed()));

    connect(ui->Chest_Compression_Button, SIGNAL(clicked()), this, SLOT (chest_compression_pressed()));
    connect(ui->CC_Shallow_Button, SIGNAL(clicked()), this, SLOT (cc_shallow_pressed()));
    connect(ui->CC_Deap_Button, SIGNAL(clicked()), this, SLOT (cc_deep_pressed()));
    connect(ui->Mouth2Mouth_Button, SIGNAL(clicked()), this, SLOT (m2m_pressed()));
    connect(ui->Shock_Button, SIGNAL(clicked()), this, SLOT (shock_pressed()));
    connect(ui->Clear_Surroundings_Button, SIGNAL(clicked()), this, SLOT (clear_surroundings_pressed()));
    connect(ui->Electrodes_In_Button, SIGNAL(clicked()), this, SLOT (electrodes_in_pressed()));
    connect(ui->Replace_Batteries_Button, SIGNAL(clicked()), this, SLOT (replace_batteries_pressed()));
    connect(ui->Go_Button, SIGNAL(clicked()), this, SLOT (go_pressed()));
    connect(ui->ES_Arrive_Button, SIGNAL(clicked()), this, SLOT (es_arrived_pressed()));
    connect(ui->Image_Load_Button, SIGNAL(clicked()), this, SLOT (load_photos()));

    connect(ui->Heart_Rhythm_CB, SIGNAL(currentIndexChanged(int)), this, SLOT (hr_changed()));

    connect(&batteryTimer, SIGNAL(timeout()), this, SLOT (battery_drain()));
    connect(ui->Set_BP_Button, SIGNAL(clicked()), this, SLOT (bp_set_pressed()));

    connect(&updateTimeTimer, SIGNAL(timeout()), this, SLOT (update_elapsed()));

    // set CB's to -1 to look empty
    ui->Victim_Size_CB->setCurrentIndex(-1);
    ui->Heart_Rhythm_CB->setCurrentIndex(-1);

    ui->Compressison_Display_Slider->setEnabled(false);

    //Hide all widgets that should not be visible on startup
    ui->AED_Widget->hide();
    ui->Actions_Widget->hide();
    ui->Clear_Surroundings_Button->hide();
    ui->Electrodes_In_Button->hide();
    ui->Replace_Batteries_Button->hide();
    ui->Time_Elapsed_Label->hide();
    reset_GUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//######################
//Private Slot Functions
//######################

void MainWindow::on_pressed(){
    qInfo("On button Pressed");
    if (!ui->Power_Button->isChecked()){
        //Power Off
        ui->Status_X->setStyleSheet("background-color: rgb(154, 153, 150);\ncolor: rgb(237, 51, 59);\nborder-radius: 0px;");
        ui->Status_Good->setStyleSheet("background-color: rgb(154, 153, 150);\ncolor: rgb(87, 227, 137);\nborder-radius: 0px;");
        ui->Actions_Widget->hide();
        reset_GUI();
        batteryTimer.stop();
        shock_num = 0;
        ui->Time_Elapsed_Label->hide();
        updateTimeTimer.stop();

    }
    else{
        //Power On
        aed_on = 1;
        ui->BatteryBar->show();
        batteryTimer.start(3000);
        ui->Time_Elapsed_Label->show();
        elapsedTime.restart();
        elapsedTime.start();
        updateTimeTimer.start();
        ui->Events_Display->clear();

        //Perform a self-test before allowing to continue.
        if(self_test()){
            ui->Status_Good->setStyleSheet("background-color: rgb(255, 255, 255);\ncolor: rgb(0, 255, 0);\nborder-radius: 0px;");
            ui->Status_X->setStyleSheet("background-color: rgb(154, 153, 150);\ncolor: rgb(237, 51, 59);\nborder-radius: 0px;");
            aed_sTest_result = 1;
            ui->Actions_Widget->show();
            go_to_state(0);
        }else{
            ui->Status_X->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 0, 0);\nborder-radius: 0px;");
            ui->Status_Good->setStyleSheet("background-color: rgb(154, 153, 150);\ncolor: rgb(87, 227, 137);\nborder-radius: 0px;");
            aed_sTest_result = 0;
            ui->Power_Button->setChecked(false);
        }
    }

}
void MainWindow::check_res_pressed(){
    qInfo("Check Responsiveness Pressed");
    if (!aed_on){
        qInfo("<You should turn on the AED>");
    }
    qInfo("->You are checking for responsiveness");
    if (aed_step_state == 0){
        //go to call ES state
        go_to_state(1);
    }
}
void MainWindow::call_es_pressed(){
    qInfo("Call ES Pressed");
    qInfo("->Calling Emergency Services");
    if (aed_step_state == 1){
        //go to apply pads state
        go_to_state(2);
    }
}
void MainWindow::setup_Apads_pressed(){
    qInfo("Setup Adult Pads Pressed");
    if (aed_step_state < 2){
        qInfo("<It is too early to apply pads>");
        return;
    }
    qInfo("->Setting up Pads");
    aed_pad_state = 1;
    go_to_state(3);
}
void MainWindow::setup_ApadsWide_pressed(){
    qInfo("Setup Adult Pads Pressed");
    if (aed_step_state < 2){
        qInfo("<It is too early to apply pads>");
        return;
    }
    qInfo("->Setting up Pads Wide");
    aed_pad_state = 4;
    go_to_state(3);
}

void MainWindow::setup_Cpads_pressed(){
    qInfo("Setup Adult Pads Pressed");
    if (aed_step_state < 2){
        qInfo("<It is too early to apply pads>");
        return;
    }
    qInfo("->Setting up Pads");
    aed_pad_state = 2;
    go_to_state(3);
}
void MainWindow::setup_Ipads_pressed(){
    qInfo("Setup Adult Pads Pressed");
    if (aed_step_state < 2){
        qInfo("<It is too early to apply pads>");
        return;
    }
    qInfo("->Setting up Pads");
    aed_pad_state = 3;
    go_to_state(3);
}

void MainWindow::chest_compression_pressed(){
    qInfo("Chest Compression Pressed");
    if (aed_step_state != 4){
        qInfo("It is not the time for CHest Compressions");
        return;
    }
    compression_type = 1;


    ui->LCD_Display->setText("Continue Chest Compressions");

    QElapsedTimer timer;
    timer.start();

    while(aed_step_state == 4 && timer.elapsed() < 5000){
        show_compression();
    }
    if (compression_type == 1){
        go_to_state(5);
    }
}
void MainWindow::cc_shallow_pressed(){
    qInfo("Shallow CC Pressed");
    if (aed_step_state != 4){
        qInfo("It is not the time for CHest Compressions");
        return;
    }
    compression_type = 2;
    ui->LCD_Display->setText("Deeper Chest Compressions Needed");
    ui->Events_Display->addItem("\"Deeper Chest Compressions Needed\"");
    while(aed_step_state == 4){
        show_compression();
    }
}
void MainWindow::cc_deep_pressed(){
    qInfo("Deep CC Pressed");
    if (aed_step_state != 4){
        qInfo("It is not the time for CHest Compressions");
        return;
    }
    compression_type = 3;
    ui->LCD_Display->setText("More Shallow Chest Compressions Needed");
    ui->Events_Display->addItem("\"More Shallow Chest Compressions Needed\"");
    while(aed_step_state == 4){
        show_compression();
    }
}
void MainWindow::m2m_pressed(){
    qInfo("Mouth to Mouth Pressed");
    if(aed_step_state != 5){
        qInfo("It is not time for mouth to mouth");
        return;
    }
    // Use a random_device to obtain a seed for the random number engine
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 4);

    // Generate a random integer
    int randomInt = distribution(gen);

    if (randomInt == 0 && ui->Heart_Rhythm_CB->currentIndex() == 3){
        ui->Heart_Rhythm_CB->setCurrentIndex(0); //Normal / healthy
    }
    go_to_state(3);
}
void MainWindow::shock_pressed(){
    qInfo("Shock Pressed");
    if(ui->BatteryBar->value() <=30){
        go_to_state(3);
        return;
    }
    ui->BatteryBar->setValue(ui->BatteryBar->value()-10); //subtract 10
    ui->Events_Display->addItem("\"SHOCKING\"");

    //custom timer to incorperate button colour changing
    QElapsedTimer timer;
    timer.start();
    const int delayMs = 2000;

    // perform the shocking animation
    while (timer.elapsed() < delayMs) {
        if(timer.elapsed()%200 < 100){
            ui->Shock_Button->setStyleSheet("color: rgb(255, 0, 119);\nbackground-color: rgb(249, 240, 107);\nborder-radius: 35px;\nborder: 2px outset;\nborder-color: rgb(213, 202, 245);");
        }else{
            ui->Shock_Button->setStyleSheet("color: rgb(255, 0, 119);\nbackground-color: rgb(9, 27, 89);\nborder-radius: 35px;\nborder: 2px outset;\nborder-color: rgb(213, 202, 245);");
        }
        QCoreApplication::processEvents();
    }
    ui->Shock_Num_Label->setText(QString::number(++shock_num)); // Add 1 to shock count and display


    //Change the heart Rhythm to one of the 4 options.
    // Use a random_device to obtain a seed for the random number engine
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 3);

    // Generate a random integer
    int randomInt = distribution(gen);

    ui->Heart_Rhythm_CB->setCurrentIndex(randomInt);

    go_to_state(4);
}
void MainWindow::clear_surroundings_pressed(){
    qInfo("Clear Surroundings Pressed");
    if (aed_step_state == 3){
        go_to_state(3); // Restarts analyzing victim
    }
}
void MainWindow::electrodes_in_pressed(){
    qInfo("Electrodes In Pressed");
    if(aed_step_state == 3){ //Analyzing Stage
        go_to_state(3); //restart analysis
    }
}
void MainWindow::replace_batteries_pressed(){
    qInfo("Replace Batteries Pressed");
    ui->BatteryBar->setValue(100);
    if(aed_step_state == 3){
        // If trying to analyze, restart analysis.
        go_to_state(3);
    }
}
void MainWindow::go_pressed(){
    qInfo("Go Pressed");
    if(ui->Victim_Size_CB->currentIndex() == -1){
        qInfo("You Must Select a Victim Type");
        return;
    }
    if(ui->Heart_Rhythm_CB->currentIndex() == -1){
        qInfo("You Must pick a heart rhythm");
        return;
    }
    ui->AED_Widget->show();
    ui->tabWidget->setCurrentIndex(1);
    ui->Clear_Surroundings_Button->show();
    ui->Electrodes_In_Button->show();
    ui->Replace_Batteries_Button->show();
}
void MainWindow::es_arrived_pressed(){
    qInfo("ES Arrived Pressed");
    ui->Power_Button->setChecked(false);
    on_pressed();
    go_to_state(-1);
    ui->LCD_Display->setText("EMERGENCY SERVICES HAVE ARRIVED");
}
void MainWindow::bp_set_pressed(){
    qInfo("Battery percentage set Pressed");
    ui->BatteryBar->setValue(ui->Set_BP_CB->value());
}


// Leo Stuff //
// Possible Outputs:
//  1 - patient should be shocked
//  0 - there was a problem

int MainWindow::analyze_victim(){
    qInfo("Analyzing Victim");
    if(self_test()){ // Checking that the AED has successfully passed its self-test
        qInfo("   AED has passed self-test");

        qInfo(" Analyzing ...");

        ui->LCD_Display->setText("Analyzing ...");

        //introduce a 2 second delay
        wait_time(2000);

        if((aed_pad_state == 1 && ui->Victim_Size_CB->currentIndex() == 1) || (aed_pad_state == 2 && ui->Victim_Size_CB->currentIndex() == 2) || (aed_pad_state == 3 && ui->Victim_Size_CB->currentIndex() == 3) || aed_pad_state == 4 && ui->Victim_Size_CB->currentIndex() == 0){ // Checking if the pads have been correctly attached
            qInfo("   Pads have been setup Correctly");

            qInfo("AED: Stay Clear!");
            if(ui->Clear_Surroundings_Button->isChecked()){ // environment is clear

                switch(ui->Heart_Rhythm_CB->currentIndex()){
                    case -1: //No option chosen
                        qInfo("No heart rhythm chosen");
                        return 0;
                    case 1:
                    case 2: //Fibrillation or Tachycardia
                        ui->LCD_Display->setText("Shock Advised");
                        qInfo("Shock Advised");
                        go_to_state(6);
                        return 1;
                    case 3: // Asystole
                        qInfo("CPR Advised");
                        go_to_state(4);
                        return 0;
                    case 0: // Normal
                        qInfo("Heartrate normal");
                        return 0;

                }
                return 0;
            }
            else{
                qInfo("   Environment is not clear. Cannot Advice Shock");
                ui->LCD_Display->setText("Clear the Surrounding Area");
                ui->Events_Display->addItem("\"Unable to Analyze, Clear Surrounding Area\"");
                return 0;
            }
        }
        else{
            qInfo("   Pads not on properly");
            qInfo("           %d   %d", aed_pad_state, ui->Victim_Size_CB->currentIndex());
            go_to_state(2);
            return 0;
        }
    }
    else{
        qInfo("   Self-Test Failed");
        return 0;
    }
}

void MainWindow::hr_changed(){
    if(ui->Image_Load_Button->isChecked()){
        normal_graph.load("./images/normal.png");
        fibrillation_graph.load("./images/fibrillation.png");
        tachycardia_graph.load("./images/tachycardia.png");
        asystole_graph.load("./images/asystole.png");
    }else{
        normal_graph.load("../AED_Team_Project/images/normal.png");
        fibrillation_graph.load("../AED_Team_Project/images/fibrillation.png");
        tachycardia_graph.load("../AED_Team_Project/images/tachycardia.png");
        asystole_graph.load("../AED_Team_Project/images/asystole.png");
    }


    int rhythm = ui->Heart_Rhythm_CB->currentIndex();
    if(rhythm == 0){
        qInfo("Plotting for PEA");
        ui->Rhythm_Name_Label->setText("Normal");
        ui->Graph_Label->setPixmap(normal_graph);
    }else if(rhythm == 1){
        qInfo("Plotting for Fibrillation");
        ui->Rhythm_Name_Label->setText("Ventricular Fibrillation");
        ui->Graph_Label->setPixmap(fibrillation_graph);
    }else if(rhythm == 2){
        qInfo("Plotting for Tachycardia");
        ui->Rhythm_Name_Label->setText("Ventricular Tachycardia");
        ui->Graph_Label->setPixmap(tachycardia_graph);
    }else if(rhythm == 3){
        qInfo("Plotting for Asystole");
        ui->Rhythm_Name_Label->setText("Asystole");
        ui->Graph_Label->setPixmap(asystole_graph);
    }else{
        ui->Rhythm_Name_Label->clear();
        ui->Graph_Label->clear();
    }
    go_to_state(aed_step_state);
}

void MainWindow::battery_drain(){
    if(ui->BatteryBar->value()){ //if the batteries have any power
        ui->BatteryBar->setValue(ui->BatteryBar->value()-1); //subtract one
    }else{
        ui->Power_Button->setChecked(false);
        on_pressed();
    }

}

void MainWindow::update_elapsed(){
    int min = elapsedTime.elapsed()/60000;
    int sec = (elapsedTime.elapsed()/1000)%60;
    ui->Time_Elapsed_Label->setText(QString("%1m %2s").arg(min).arg(sec));
}

void MainWindow::load_photos(){
    if(ui->Image_Load_Button->isChecked()){
        checkRPic.load("./images/CheckR.png");
        callESPic.load("./images/CallES.png");
        applyPadsPic.load("./images/ApplyPads.png");
        dontTouchPic.load("./images/DontTouch.png");
        chestCompPic.load("./images/ChestComp.png");
        m2mPic.load("./images/MouthToMouth.png");
        ui->Check_R_Picture->setPixmap(checkRPic);
        ui->Call_ES_Picture->setPixmap(callESPic);
        ui->Setup_Pads_Picture->setPixmap(applyPadsPic);
        ui->Dont_Touch_Picture->setPixmap(dontTouchPic);
        ui->Compressions_Picture->setPixmap(chestCompPic);
        ui->M2M_Picture->setPixmap(m2mPic);
    }else{
        checkRPic.load("../AED_Team_Project/images/CheckR.png");
        callESPic.load("../AED_Team_Project/images/CallES.png");
        applyPadsPic.load("../AED_Team_Project/images/ApplyPads.png");
        dontTouchPic.load("../AED_Team_Project/images/DontTouch.png");
        chestCompPic.load("../AED_Team_Project/images/ChestComp.png");
        m2mPic.load("../AED_Team_Project/images/MouthToMouth.png");
        ui->Check_R_Picture->setPixmap(checkRPic);
        ui->Call_ES_Picture->setPixmap(callESPic);
        ui->Setup_Pads_Picture->setPixmap(applyPadsPic);
        ui->Dont_Touch_Picture->setPixmap(dontTouchPic);
        ui->Compressions_Picture->setPixmap(chestCompPic);
        ui->M2M_Picture->setPixmap(m2mPic);
    }



    hr_changed();
}


//############################################
//############################################

int MainWindow::self_test(){
    //Self-test
    // returns 0 if bad, returns 1 if good
    qInfo("AED: Self-Test:");
    qInfo("   Checking Battery %");
    if (ui->BatteryBar->value() <= 50){
         qInfo("        ^ Failed Here");
         ui->LCD_Display->setText("REPLACE BATTERIES");
         ui->Events_Display->addItem("\"Replace Batteries\"");
        return 0;
    }
    qInfo("   Checking Electrode Connection");
    if (!ui->Electrodes_In_Button->isChecked()){
        qInfo("        ^ Failed Here");
        ui->LCD_Display->setText("ATTACH ELECTRODES");
        ui->Events_Display->addItem("\"Pads Must be Plugged In\"");
        return 0;
    }
    qInfo("   SUCCESS");
    return 1;
}

void MainWindow::go_to_state(int statenum){
    //clean GUI
    ui->Check_R_Button->setStyleSheet("");
    ui->Call_ES_Button->setStyleSheet("");
    ui->Setup_Adult_Pads_Button->setStyleSheet("");
    ui->Setup_Adult_Wide_Pads_Button->setStyleSheet("");
    ui->Setup_Child_Pads_Button->setStyleSheet("");
    ui->Setup_Infant_Pads_Button->setStyleSheet("");
    ui->Chest_Compression_Button->setStyleSheet("");
    ui->Mouth2Mouth_Button->setStyleSheet("");
    ui->Shock_Button->setStyleSheet("color: rgb(255, 0, 119);\nbackground-color: rgb(203, 192, 235);\nborder-radius: 35px;\nborder: 2px outset;\nborder-color: rgb(213, 202, 245);");
    ui->Shock_Button->setEnabled(false);
    if(ui->Heart_Rhythm_CB->currentIndex() == 0){ //normal / healthy
        statenum = -1;
    }
    aed_step_state = statenum;

    if (statenum < 6 && statenum > -1){
        radioButtons[statenum]->setChecked(true);
    }
    if(!aed_on){ // if aed is not on
        return;
    }
    qInfo("Going to State %d",statenum);
    switch (statenum) {
        case -1:
            ui->LCD_Display->setText("Patient Heart Rhythm Normal.  Standby");
            radioButtons[3]->setChecked(true);
            break;
        case 0:
            ui->LCD_Display->setText("Check For Responsiveness");
            ui->Check_R_Button->setStyleSheet("background-color: rgb(249, 240, 107);");
            ui->Events_Display->addItem("\"Check for Responsiveness\"");
            break;
        case 1:
            ui->LCD_Display->setText("Call Emergency Services");
            ui->Call_ES_Button->setStyleSheet("background-color: rgb(249, 240, 107);");
            ui->Events_Display->addItem("\"Call Emergency Services\"");
            break;
        case 2:
            ui->LCD_Display->setText("Apply Pads");
            ui->Events_Display->addItem("\"Apply Correct Pads\"");
            if(ui->Victim_Size_CB->currentIndex() == 0){
                ui->Setup_Adult_Wide_Pads_Button->setStyleSheet("background-color: rgb(249, 240, 107);");

            }else if(ui->Victim_Size_CB->currentIndex() == 1){
                ui->Setup_Adult_Pads_Button->setStyleSheet("background-color: rgb(249, 240, 107);");

            }else if(ui->Victim_Size_CB->currentIndex() == 2){
                ui->Setup_Child_Pads_Button->setStyleSheet("background-color: rgb(249, 240, 107);");

            }else if(ui->Victim_Size_CB->currentIndex() == 3){
                ui->Setup_Infant_Pads_Button->setStyleSheet("background-color: rgb(249, 240, 107);");

            }
            break;
        case 3:
            ui->LCD_Display->setText("Don't Touch! Analyzing");
            ui->Events_Display->addItem("\"Stand Clear\"");
            ui->Events_Display->addItem("\"Analyzing\"");
            analyze_victim();
            break;
        case 4:
            ui->LCD_Display->setText("Proceed with 30 Chest Compressions");
            ui->Chest_Compression_Button->setStyleSheet("background-color: rgb(249, 240, 107);");
            ui->Events_Display->addItem("\"Begin 30 Chest Compressions\"");
            break;
        case 5:
            ui->LCD_Display->setText("Proceed with 2 breathes of mouth to mouth");
            ui->Mouth2Mouth_Button->setStyleSheet("background-color: rgb(249, 240, 107);");
            ui->Events_Display->addItem("\"Begin 2 Breathes of Mouth to Mouth\"");

            break;
        case 6:
            ui->Shock_Button->setEnabled(true);
            ui->Shock_Button->setStyleSheet("color: rgb(255, 0, 119);\nbackground-color: rgb(192, 97, 203);\nborder-radius: 35px;\nborder: 2px outset;\nborder-color: rgb(213, 202, 245);");
            ui->Events_Display->addItem("\"Shock Advised\"");
            break;
    }
}

void MainWindow::show_compression(){
    //Moves the compression bar slider up and down
    const int delayMs = 20;
    int iJump = 5;
    if (compression_type == 0){
        return;
    }else if(compression_type == 1){
        for(int i=0; i<75; i+=iJump){

            ui->Compressison_Display_Slider->setValue(i);
            wait_time(delayMs);

        }
        for(int i=75; i>= 0; i-=iJump){
            ui->Compressison_Display_Slider->setValue(i);
            wait_time(delayMs);

        }
    }else if (compression_type == 2){
        for(int i=0; i<50; i+=iJump){
            ui->Compressison_Display_Slider->setValue(i);
            wait_time(delayMs);
        }
        for(int i=50; i>=0; i-=iJump){
            ui->Compressison_Display_Slider->setValue(i);
            wait_time(delayMs);
        }
    }else if (compression_type == 3){
        for(int i=0; i<100; i+=iJump){
            ui->Compressison_Display_Slider->setValue(i);
            wait_time(delayMs);
        }
        for(int i=100; i>=0; i-=iJump){
            ui->Compressison_Display_Slider->setValue(i);
            wait_time(delayMs);
        }
    }
}

void MainWindow::reset_GUI(){
    //Reset States
    aed_on = 0;
    aed_sTest_result = 0;
    aed_step_state = 0;
    aed_pad_state = 0;
    compression_type = 0;

    //Reset widgets
    for (int i=0; i<6; ++i){
        radioButtons[i]->setChecked(false);
    }
    ui->LCD_Display->clear();
    ui->Shock_Button->setEnabled(false);

    ui->Check_R_Button->setStyleSheet("");
    ui->Call_ES_Button->setStyleSheet("");
    ui->Setup_Adult_Pads_Button->setStyleSheet("");
    ui->Setup_Child_Pads_Button->setStyleSheet("");
    ui->Setup_Infant_Pads_Button->setStyleSheet("");
    ui->Chest_Compression_Button->setStyleSheet("");
    ui->Mouth2Mouth_Button->setStyleSheet("");
}

void MainWindow::wait_time(int ms){
    QElapsedTimer timer;
    timer.start();
    const int delayMs = ms;

    while (timer.elapsed() < delayMs) {
        QCoreApplication::processEvents();
    }
}
