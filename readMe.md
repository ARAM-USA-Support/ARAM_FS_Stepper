This is a library for use with ARAM's line of Full-Step Stepper Motor Drivers

Usage is fairly simple:


To declare a stepper using pins A and B for control
    ARAM_FS_STEPPER stepper;
    stepper = ARAM_FS_STEPPER( A , B );

To move one step clockwise
    stepper.moveStepperCW();

To move one step counter clockwise
    stepper.moveStepperCCW();