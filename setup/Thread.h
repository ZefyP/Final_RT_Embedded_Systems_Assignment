extern int Init_Blink_LED_Thread (void); // Standard format to declare the function to initialise the main thread function.
void Blink_LED_Thread (void const *argument); // Standard format to declare the main thread function.

//extern int Init_Button_Thread (void); 
//void Button_Thread (void const *argument);

extern int Init_Tilt_Thread (void); 
void Tilt_Thread (void const *argument); 

extern int Init_Blink_Pattern_Thread  (void); 
void Blink_Pattern_Thread (void const *argument); 
