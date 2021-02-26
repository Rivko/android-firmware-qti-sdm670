/******************************************************************************
 *      CPU-VOLTAGE-MATCHER.C
 *
 *  General Description
 *  Utility for Matching Voltages Across CPUs
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <linux/types.h>
#include <sys/types.h>
#include "cpu-voltage-matcher.h"

#define CLS0_OPP_TABLE "/sys/devices/system/cpu/cpu0/opp_table"
#define CLS1_OPP_TABLE "/sys/devices/system/cpu/cpu2/opp_table"
#define MAX_FREQ_GAP_KHZ 307200

int main(int argc, char *argv[])
{
	unsigned long input_freq = 0;
	unsigned long input_freq_actual = 0;
	unsigned long input_volt = 0;
	unsigned long target_freq = 0;
	FILE* cls0_opp_fd = NULL;
	FILE* cls1_opp_fd = NULL;
	char* line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	char* token = NULL;
	unsigned long temp_freq = 0;
	unsigned long temp_volt = 0;

	if (argc != 2) {
		msg("Error: Need frequency in kHz as single argument\n");
		goto out;
	}

	// Get input frequency
	input_freq = strtoul(argv[1], NULL, 10);

	// Open input files
	cls0_opp_fd = fopen(CLS0_OPP_TABLE, "r");
	if (cls0_opp_fd == NULL) {
		msg("Error opening CLS0 OPP table %d\n", errno);
		goto out;
	}

	cls1_opp_fd = fopen(CLS1_OPP_TABLE, "r");
	if (cls1_opp_fd == NULL) {
		msg("Error opening CLS1 OPP table %d\n", errno);
		goto out;
	}

	// Parse Cluster 1 OPP table for closest frequency and get voltage
	while ((read = getline(&line, &len, cls1_opp_fd)) != -1) {
		token = strtok(line, " \t");
		if (token == NULL)
			continue;
		// check frequency
		temp_freq = strtoul(token, NULL, 10);
		temp_freq /= 1000;
		if (temp_freq >= input_freq) {
			// get voltage
			token = strtok(NULL, " \t");
			input_volt = strtoul(token, NULL, 10);
			input_freq_actual = temp_freq;
			break;
		}
	}

	// If we didn't find frequency, goto end
	if (input_volt == 0) {
		msg("Error: Could not find a frequency in OPP table close to input_freq:: %lu\n", input_freq);
		goto out;
	}

	// Parse Cluster 0 OPP table for closest frequency to input_voltage
	while ((read = getline(&line, &len, cls0_opp_fd)) != -1) {
		token = strtok(line, " \t");
		if (token == NULL)
			continue;
		// get frequency
		temp_freq = strtoul(token, NULL, 10);
		temp_freq /= 1000;
		// check voltage
		token = strtok(NULL, " \t");
		temp_volt = strtoul(token, NULL, 10);
		// Compare voltge and continue to next frequency if gap is too large
		if ((target_freq == 0) && (temp_volt > input_volt) && ((temp_freq + MAX_FREQ_GAP_KHZ) >= input_freq_actual)) {
			// set target_freq
			// NOTE: currently blocking higher frequency than input frequency
			if (temp_freq > input_freq)
				target_freq = input_freq;
			else
				target_freq = temp_freq;
		}
	}

	// If we didn't find a frequency, either input voltage is too high
	// or gap was too large (inut frequency was too high)
	// In either case, return Cluster 0 Fmax as this is the best possible frequency
	if (target_freq == 0)
		target_freq = temp_freq;

out:
	// Return target_freq
	fprintf(stdout, "%lu\n", target_freq);

	if (cls0_opp_fd != NULL)
		fclose(cls0_opp_fd);
	if (cls1_opp_fd != NULL)
		fclose(cls1_opp_fd);
	return 0;
}
