Team 57 members:

Mahmud Ali
Christian Devey
Leo Keenan
Matthew Valenta

Work Breakdown
    Code Review:
        Matthew Valenta
        Mahmud Ali
    Use Cases: 
        Matthew Valenta
        Christian Devey
        Leo Keenan
    UML Diagram:
        Matthew Valenta
    Sequence Diagrams:
        Leo Keenan
        Christian Devey
    Source Code:
        Matthew Valenta
        Leo Keenan
        Mahmud Ali
    Traceability Matrix:
        Mahmud Ali
    Videos:
        Matthew Valenta



Adding Images to new directory:
    When running This application from QtCreator, a new directory is created called "Team_Project" which contains another folder called "AED_Team_Project_Release". In order to see the photos in the application, The images folder from "AED_Team_Project" must be either copied over to "AED_Team_Project_Release", or the original "AED_Team_Project" folder must be placed inside of the "Team_Folder" directory.


Running the application:

On startup, you are met with two combo boxes that are used to determine the type of victim and initial heart rhythm that will be treated. Once both of these are selected, press GO.

Once GO is pressed, the AED will appear. There is a power button bottom right of the AED, which will perform a self-test upon being pressed. In order to pass the self-test, you must replace the batteries and toggle the "Electrodes In" button.

Now that the AED is on and passed its self-test, you are able to see the action buttons on the right side of the application. The AED asks the user to Check for Responsiveness and the assosiated button will light up informing you on which needs to be pressed.

After going through the first 3 stages, applying the correct pads to the victim, the AED will analyze the victim, but will fail if the surroundings are not cleared. Once surroundings have been cleared and the victim has been analyzed, depending on the heart rhythm of the victim, either a shock will be advised, CPR will be advised, or you will be told that the victim's heart rhythm is normal and your job is done.

If the victim needs to be shocked the middle shock button will turn deep purple and you are instructed to press that button. After the victim has been shocked, their heart rhythm will randomly change to one of the four options. If the new heart rhythm is still not normal, you will be instructed to perform CPR.

For chest compressions, you are able to simulate improper compressions with the small arrows to the right of the main chest compressions button. If performing improper chest compressions, you will be instructed to correct them before proceeding to mouth to mouth. After mouth to mouth has been performed, the victim will be reanalyzed and the cycle continues until the victims heart rate is normal, or Emergency Services have arrived which is toggled by a button in the Event tab top right.
