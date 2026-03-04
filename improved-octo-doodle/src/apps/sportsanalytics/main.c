#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"
#include "utils.h"
#include "data.h"
#include "../../container-update/container-update.h"

int main()
{    
    nrf_gpio_cfg_output(PIN);
    nrf_gpio_pin_set(PIN);
    nrf_gpio_pin_toggle(PIN);
    uart_init();

	int class, i = 0;
	float *inter_1 = (float *)calloc(sizeof(float), HIDDEN_DIM);
 	float *inter_2 = (float *)calloc(sizeof(float), OUTPUT_DIM);

    while (true) {
        class = 0;
        memset(inter_1, 0, sizeof(float)*HIDDEN_DIM);
        memset(inter_2, 0, sizeof(float)*OUTPUT_DIM);
  	    class = PrateekModel(W1, W2, B1, B2, sampleData, INPUT_DIM, HIDDEN_DIM, 
        	OUTPUT_DIM, inter_1, inter_2);
	    printf("Predicted Class: %d\r\n", class);

        nrf_delay_ms(500);
        if (i == 10) {
            printf("switching\r\n");
            if (check_hash()) {
                switch_container(16);
            }
        }
        if (i == 20) {
            printf("switching back\r\n");
            //
            // NOTE: dont check hash again, since it would fail the second time!
            //
            switch_container(16);
        }
        i++;
    }
 	
 	
	
	return 0;
}
