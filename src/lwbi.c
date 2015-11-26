/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BR_SIZE 512

int get_battery_percent(char *forbattery) {
    char *path = "/sys/class/power_supply";
    FILE *fp;
    char b_read[BR_SIZE];
    char tmp[64];

    int energy_now = 1;
    int energy_full = 1;

    snprintf(b_read, BR_SIZE, "%s/%s/%s", path, forbattery, "energy_now");
    fp = fopen(b_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_now = atoi(tmp);
        fclose(fp);
    } else {
        snprintf(b_read, BR_SIZE, "%s/%s/%s", path, forbattery, "charge_now");
        fp = fopen(b_read, "r");
        if(fp != NULL) {
            if (fgets(tmp, sizeof tmp, fp)) energy_now = atoi(tmp);
            fclose(fp);
        } else {
            perror(b_read);
            return -1;
        }
    }

    snprintf(b_read, BR_SIZE, "%s/%s/%s", path, forbattery, "energy_full");
    fp = fopen(b_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_full = atoi(tmp);
        fclose(fp);
    } else {
        snprintf(b_read, BR_SIZE, "%s/%s/%s", path, forbattery, "charge_full");
        fp = fopen(b_read, "r");
        if(fp != NULL) {
            if (fgets(tmp, sizeof tmp, fp)) energy_full = atoi(tmp);
            fclose(fp);
        } else {
            return -2;
        }
    }
    return energy_now / (energy_full / 100);
}

