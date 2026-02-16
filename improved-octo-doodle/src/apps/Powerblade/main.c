#include <string.h>
#include <stdlib.h>
#include "fastGRNNModel.h"
#include "utils.h"
#include "sampleData.h"
#include "../../container-update/container-update.h"

int main() {
    nrf_gpio_cfg_output(PIN);
    nrf_gpio_pin_set(PIN);
    nrf_gpio_pin_toggle(PIN);
    uart_init();

  // put your main code here, to run repeatedly:
	float *hiddenState                = (float *) calloc(sizeof(float), hiddenDims);
    float *preComp                    = (float *) calloc(sizeof(float),hiddenDims);
    float *tempLRW                    = (float *) calloc(sizeof(float),wRank);
    float *tempLRU                    = (float *) calloc(sizeof(float),uRank);
    float *tempFC                     = (float *) calloc(sizeof(float),FCrank);
    float *classScores                = (float *) calloc(sizeof(float),numClasses);
    float *normalizedFeats            = (float *) calloc(sizeof(float),inputDims);
	float data[210];

    // float data[210] = {1.653, 1.656, 1.679, 1.655, 1.659, 1.679, 1.6541, 1.655, 1.689, 1.651, 1.651, 1.68, 1.655, 1.655, 1.6842, 1.655, 1.652, 1.68, 1.653, 1.655, 1.685, 1.656, 1.653, 1.68, 1.652, 1.659, 1.679, 1.646998, 1.652, 1.6782, 1.655, 1.65, 1.682, 1.655, 1.66, 1.682, 1.655, 1.655, 1.681, 1.653, 1.652, 1.681, 1.655, 1.651, 1.676998, 1.6481, 1.6541, 1.6782, 1.655, 1.646998, 1.681, 1.656, 1.652, 1.6842, 1.662, 1.636998, 1.679, 1.656998, 1.565998, 1.682, 1.651, 1.337, 1.599, 1.655, 1.311998, 1.439, 1.659, 1.396998, 1.3032, 1.656, 1.376, 1.349, 1.655, 1.5832, 1.511, 1.653, 1.855999, 1.646998, 1.653, 1.768, 1.734, 1.651, 1.676998, 1.7791, 1.652, 1.632, 1.7891, 1.6581, 1.662, 1.781998, 1.649, 1.804, 1.756, 1.656998, 1.771, 1.7931, 1.652, 1.666998, 1.8132, 1.652, 1.689, 1.791998, 1.646998, 1.682, 1.746999, 1.652, 1.715, 1.7, 1.655, 1.662, 1.681, 1.65, 1.6132, 1.6842, 1.653, 1.6681, 1.67, 1.659, 1.676998, 1.675, 1.6541, 1.646998, 1.699, 1.65, 1.666998, 1.734, 1.656998, 1.6782, 1.766, 1.655, 1.733, 1.771, 1.652, 1.719, 1.776, 1.66, 1.6641, 1.781, 1.661, 1.683, 1.771, 1.66, 1.703, 1.764, 1.655, 1.706999, 1.743, 1.656998, 1.7, 1.7382, 1.655, 1.6581, 1.732, 1.66, 1.673, 1.7082, 1.656, 1.686998, 1.702, 1.66, 1.663, 1.704, 1.655, 1.66, 1.706999, 1.655, 1.6581, 1.715, 1.652, 1.686, 1.725, 1.663, 1.693, 1.730999, 1.652, 1.68, 1.735, 1.655, 1.675, 1.733, 1.655, 1.686, 1.729, 1.659, 1.6882, 1.716999, 1.659, 1.683, 1.700998, 1.661, 1.659, 1.696998, 1.655, 1.6581, 1.689, 1.66, 1.676998, 1.671, 1.66, 1.661, 1.672, 1.6541, 1.656, 1.666998, 1.656, 1.653, 1.669, 1.653, 1.662, 1.669};

    
    // To initialize data randomly. This prevents the comm cost
	// for (int i = 0; i <210; i++){
	// 	data[i] = analogRead(A0);
	// 	data[i] = data[i]/512.00;
	// 	// data[i] = rand()/1e10;
	// 	printf("%f", data[i]);
	// 	printf(",");
	// }


    int i = 0;
    while (true) {
	    memset(hiddenState, 0, sizeof(float)*hiddenDims);
	    int predicted_class = FastGRNN(W1, W2, U1, U2, Bg, Bh,
	    zeta, nu, FC1, FC2, FCbias, a, mean, stdDev,
	    hiddenState, inputDims, wRank, hiddenDims,
	    uRank, preComp, tempLRW, tempLRU, timeSteps,
	    tempFC, classScores, FCrank, numClasses, normalizedFeats);
	    printf("Predicted class: %d\r\n", predicted_class);

        if (i == 10) {
            printf("SWITCHING\r\n");
            //nrf_gpio_pin_toggle(PIN);
            if (check_hash()) {
               switch_container(16); 
            }
            //nrf_gpio_pin_toggle(PIN);
        }

        if (i == 20) {
            printf("SWITCHING BACK\r\n");
            switch_container(16);
        }
        i++;
        nrf_delay_ms(500);
    }

	return 0;
}
