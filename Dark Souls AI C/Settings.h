#ifndef Settings_H
#define Settings_H

//used in handler
#define AutoRedSign 0
#define DisableAi 0
#define TrainNeuralNet 0
    //used in initalizeFann
    #define TrainAttackNet 0
    #define TrainBackstabNet 1
#define FeedNeuralNet 0

//*****NOTE****: if this is longer than ~120 characters FANN will crash when trying to open the .net file. Try not to do that????? Sorry
#define NeuralNetFolderLocation "C:\Users\unda\Documents\Bot King\Dark-Souls-PvP-AI-master modular\Neural Nets" 
//used in helper utils (for camera)
#define OolicelMap 0 //0 for Burg, Final Destination - 1 for Township,
                     //CE Camera Burg Rotation also for -

#define BackstabMetaOnly 0

//used in gui
#define ENABLEGUI 1
#define ENABLEPRINT 0
#define REDIRECTTOFILE 0 //WARNING: produces 1GB every 2 min
#define PORT 4149

#endif
