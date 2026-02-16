/*
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 * 
 * GesturePod simulation: Bare bones code to play with on your unix system, with
 *  preloaded sensor values.
 */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"
#include "utils.h"

#include "../../container-update/container-update.h"

#define FEATURE_LENGTH 124

int main(){
    nrf_gpio_cfg_output(PIN);
    nrf_gpio_pin_set(PIN);
    nrf_gpio_pin_toggle(PIN);
    uart_init();
	int result;
    float *featureVectorF = (float *)calloc(sizeof(float), FEATURE_LENGTH); 
    float *scores = (float *)calloc(sizeof(float), NUMLABELS);
    
    int i = 0;
    while (true) {
        for (int j = 0; j < FEATURE_LENGTH; j++) { 
            featureVectorF[j] = (rand() % (200 - 0 + 1));
        } 
        printf("ff[0]: %f\r\n", featureVectorF[0]);
        result = predict(featureVectorF, FEATURE_LENGTH, scores);
        printf("The results are: %d\r\n",result);
        nrf_delay_ms(500);
        if (i == 10)  {
            printf("SWITCHING\r\n");
            nrf_delay_ms(100);
            nrf_delay_ms(100);
            if (check_hash()) switch_container(16);
        }
        if (i == 20) {
            printf("SWITCHING BACK\r\n");
            switch_container(16);
        }
        i++;
    }    

    
}

