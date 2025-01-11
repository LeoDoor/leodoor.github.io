#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "QTimer"
#include "QRadioButton"

#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>

#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer batteryTimer;
    QElapsedTimer elapsedTime;
    QTimer updateTimeTimer;
    QRadioButton *radioButtons[6];

    QPixmap normal_graph;
    QPixmap fibrillation_graph;
    QPixmap tachycardia_graph;
    QPixmap asystole_graph;

    QPixmap checkRPic;
    QPixmap callESPic;
    QPixmap applyPadsPic;
    QPixmap dontTouchPic;
    QPixmap chestCompPic;
    QPixmap m2mPic;


    // STATES
    int aed_on = 0; // 0 = off, 1 = on
    int aed_sTest_result = 0; // 0 = busted, 1 = good
    int aed_step_state = 0; // 0 = check r, 1 = call ES, 2 = apply pads, 3 = Dont touch, 4 = Chest compressions, 5 = M2M, 6 = shock
    int aed_pad_state = 0; //0 = not on, 1 = Adult, 2 = Child, 3 = Infant, 4 = Wide Adult
    int compression_type = 0;//0 = none, 1 = Good, 2 = Shallow, 3 = Deap
    int shock_num = 0; // counts number of shocks



    int self_test(); // checks battery percentage and electrodes to see if they are good
    int set_battery_percentage(int percentage); // sets the battery percentage, used for testing purposes
    void go_to_state(int statenum); // sets up each new state, updating the UI and prompting the user.
    void show_compression(); // This function processes the visuals for compressions (the bar moving)
    void reset_GUI(); // This function resets the state variables and the UI to what it is originally.
    void wait_time(int ms); //Adds a wait time in the GUI, but still processes other events. Used to pause whatever function it is in for x milliseconds


private slots:

    void on_pressed(); // called when the power button is pressed
    void check_res_pressed(); // called when the check responsiveness is pressed
    void call_es_pressed(); // called when Call Emergency Services button is pressed
    void setup_Apads_pressed(); // called when Setup Adult Pads are pressed
    void setup_ApadsWide_pressed(); // called when Setup Adult Wide Pads are pressed
    void setup_Cpads_pressed(); // called when Setup Child Pads are pressed
    void setup_Ipads_pressed(); // called when Setup Infant Pads are pressed
    void chest_compression_pressed(); // called when Chest Compressions button is pressed
    void cc_shallow_pressed(); // called when shallow Chest Compressions button ispressed
    void cc_deep_pressed(); // called when deep chest compressions button is pressed
    void m2m_pressed(); // called when mouth to mouth button is pressed
    void shock_pressed(); // called when shock button is pressed
    void clear_surroundings_pressed(); // called when clear surrounding button is pressed
    void electrodes_in_pressed(); // called when Electrodes In button is pressed
    void replace_batteries_pressed(); // Called when Replace Batteries button is pressed
    void go_pressed(); // called when Go button is pressed
    void es_arrived_pressed(); // Called when Emergency Services Arrive Button is pressed
    int analyze_victim(); // Checks the Victims heart rhythm after passing a self-test. Determines if shock is needed and following steps
    void bp_set_pressed(); // Called when Set Battery Percentage Button is pressed
    void hr_changed(); // Called when the Heart Rhythm Changes
    void battery_drain(); // Called At the end of a 3s timer to drain the battery
    void update_elapsed(); // Called at the end of a timer to update the label that displays the time elapsed
    void load_photos(); // Called when load photos button is toggled.
};
#endif // MAINWINDOW_H
