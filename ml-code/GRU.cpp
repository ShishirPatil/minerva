#include <Arduino.h>
#include "grucell_v1.h"
#include "linear_data_train.h"


void setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.print("Total no of instances of data:");
	// float *data = liner_data_train;
	Serial.println(((sizeof(data))/sizeof(*data))/200/6);
	int input_dim = 6;
	int hidden_dim = 8;
	static const int num_labels = 5;
	float scores[num_labels]={0};
	/* 
	 * GRUCell with constructor(input_dim, hidden_dim, num_labels)
	*/
	GRUCell GRUCell1(input_dim,hidden_dim,num_labels);
	if(GRUCell1.initStatus()==1)
		Serial.println("GRUCell initialised successfully");
	else
	{
		Serial.println("Unable to initialise GRUCell");
		exit(-1);
	}
	int iter=0;
	float data_nonConst[hidden_dim];
	unsigned long startTime, endTime;
	unsigned long time=0;
	int timeIter = 0;
	startTime = micros();
	for (int i = 0; i < 12000; i+=input_dim)
	{	
		iter +=1;
		Serial.print("Iter:"); Serial.println(iter);
		for (int k = 0; k < input_dim; k++)
		{
			data_nonConst[k] = data[i+k]; 
		}
		// startTime = micros();
		GRUCell1.updateCell(data_nonConst,input_dim);
		// endTime = micros();
		if (i%20==0)
		{	timeIter++;
			// Serial.println("Entering Get Output");
			GRUCell1.getOutput(scores);
			Serial.print("\nThe score is:");
			for (int j = 0; j < num_labels; j++)
			{
				 // Print the Scores 
				Serial.print(scores[j]);
				Serial.print(",");
			}
		}
		time += (endTime-startTime);

	}
	// Serial.print("Avg. Time: ");
	// Serial.println(time);
	endTime = micros();
	Serial.print("Time taken:");
	Serial.println(endTime-startTime);
	Serial.println("\nCompleted prediction for 12000 timesteps");


}

void loop()
{
	digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);  
}