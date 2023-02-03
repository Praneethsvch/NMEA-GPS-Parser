#include <string.h>
#include <ctype.h>
#include "gps_parser.h"

#define MAX_LEN 100

/*
	NMEA Data format: 
		GGA 		Time, position and fix type data
		GLL 		Latitude, longitude, UTC time of position fix and status
		GSA    		GPS receiver operating mode, satellites used in the position solution, and DOP values
		GSV 		Number of GPS satellites in view satellite ID numbers, elevation, azimuth, & SNR values
		MSS 		Signal-to-noise ratio, signal strength, frequency, and bit rate from a radio-beacon receiver
		RMC 		Time, date, position, course and speed data
		VTG 		Course and speed information relative to the ground
		ZDA 		PPS timing message (synchronized to PPS)
*/

static int checkChecksum(char* nmeaString) {
    int len = strlen(nmeaString);
    int i = 0;
    int checksum = 0;
    char calculatedChecksum[3];

    // Calculate the checksum
    for (i = 1; i < len; i++) {
        if (nmeaString[i] == '*') {
            break;
        }
        checksum ^= nmeaString[i];
    }

    // Convert the calculated checksum to hexadecimal
    sprintf(calculatedChecksum, "%02X", checksum);

    // Compare the calculated checksum with the one in the NMEA string
    if (strcmp(&nmeaString[i + 1], calculatedChecksum) == 0) {
        // remove the checksum from the string if passed for parsing
        nmeaString[i] = '\0';
        return 1;
    } else {
        return 0;
    }
}

void decode_nmea(char *sentence){
	// Check for invalid sentence type
	if (*sentence++!='$'){
		printf("Invalid sentence!\n");
		return;}
	// Verify checksum
	//if (checkChecksum(sentence)==0){
	//	printf("Checksum failed!\n");
	//	return;}
	// Check type of sentence
	if (strncmp(sentence, "GPGGA", 5) == 0){
		parse_gga(sentence);
	} else if(strncmp(sentence, "GPGLL", 5) == 0){
		parse_gll(sentence);
	} else if(strncmp(sentence, "GPGSA", 5) == 0){
		parse_gsa(sentence);
	} else if(strncmp(sentence, "GPGSV", 5) == 0){
		parse_gsv(sentence);
	} else if(strncmp(sentence, "GPMSS", 5) == 0){
		parse_mss(sentence);
	} else if(strncmp(sentence, "GPRMC", 5) == 0){
		parse_rmc(sentence);
	} else if(strncmp(sentence, "GPVTG", 5) == 0){
		parse_vtg(sentence);
	} else if(strncmp(sentence, "GPZDA", 5) == 0){
		parse_zda(sentence);
	} else {
		/* no pattern matched */
	}
}

void parse_gga(char *sentence){
    gga_data data;
    // Decode the $GPGGA sentence
    char *p = sentence;
    p = strchr(p, ',') + 1;
    int index = 1;
    while (p != NULL){
        if(*p!='$' && *p!='*'){
            switch (index) {
                case 1:
                data.time = atof(p);
                break;
                case 2:
                data.latitude = atof(p);
                break;
                case 3:
                data.latitude_dir = *p;
                break;
                case 4:
                data.longitude = atof(p);
                break;
                case 5:
                data.longitude_dir = *p;
                break;
                case 6:
                data.fix_quality = atoi(p);
                break;
                case 7:
                data.satellites_used = atoi(p);
                break;
                case 8:
                data.HDOP = atof(p);
                break;
                case 9:
                data.altitude = atof(p);
                break;
                case 10:
                data.altitude_unit = *p;
                break;
                case 11:
                data.height_geoid = atof(p);
                break;
                case 12:
                data.height_geoid_unit = *p; 
                break;
                case 13:
                data.age = atof(p);
                break;
                case 14:
                strcpy(data.station_id, p);
                break;
            }
            if (index<14)
                p = strchr(p, ',')+1;
        } else if (*p == '$'){
            break;
        }
        if(index<14)
                index++;
        else
            break;
    }
    printf("\nTime (hhmmss.ss): %.2f \n", data.time);
    printf("Latitude: %.4f %c\n", data.latitude, data.latitude_dir);
    printf("Longitude: %.4f %c\n", data.longitude, data.longitude_dir);
    printf("Fix quality: %d\n", data.fix_quality);
    printf("Satellites used: %d\n", data.satellites_used);
    printf("HDOP: %.1f\n", data.HDOP);
    printf("Altitude: %.1f %c\n\n\n", data.altitude, data.altitude_unit);
}

void parse_gll(char *sentence){
    gll_data data;
    // Decode the $GPGLL sentence
    char *p = sentence;
    p = strchr(p, ',')+1;
    int index = 1;
    while (p != NULL){
        if(*p!='*' && *p!='$'){
            switch (index) {
                case 1:
                data.latitude = atof(p);
                break;
                case 2:
                data.latitude_dir = *p;
                break;
                case 3:
                data.longitude = atof(p);
                break;
                case 4:
                data.longitude_dir = *p;
                break;
                case 5:
                data.time= atof(p);
                break;
                case 6:
                data.status = *p; 
                break;
                case 7:
                data.mode = *p;
                break;
            }
            if (index<7)  
                p=strchr(p, ',')+1;
        } else if(*p=='$'){
            break;
        }
        if(index<7)  
            index++;
        else 
            break;
    }
    printf("\nLatitude: %.4f %c\n", data.latitude, data.latitude_dir);
    printf("Longitude: %.4f %c\n", data.longitude, data.longitude_dir);
    printf("Time(hhmmss.ss): %.2f \n", data.time);
    printf("Status: %c\n", data.status);
    printf("Mode Indicator: %c\n\n\n", data.mode);
}

void parse_gsa(char *sentence){
    gsa_data data;
    char *p = sentence; 
    p = strchr(p, ',') + 1;
    int index = 1;
    int sat_index = 0;
    while (p!= NULL){
        if(*p!='*' &&  *p!='$'){
            // not empty
            switch (index) {
                case 1:
                data.mode = *p;
                break;
                case 2:
                data.fix_type = *p; 
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
                if(atoi(p)!=0)
                    // ID cant be zero unless data is missing
                    data.sat_ids[sat_index++] = atoi(p);                
                break;
                case 15:
                data.pdop = atof(p);
                break;
            case 16:
                data.hdop = atof(p);
                break;
            case 17:
                data.vdop = atof(p);    
                break;
            }
            if(index<17)
                p = strchr(p, ',') + 1;
        } else if (*p=='$'){
            break;
        }
        if(index<17)    
            index++;
        else
            break;
    }
    printf("\nMode: %c\n", data.mode);
    printf("Fix Type: %d\n", data.fix_type);
    printf("Satellites: ");
    for (int i = 0; i < sat_index; i++) {
        printf("%d ", data.sat_ids[i]);
    }
    printf("\n");
    printf("sat_index: %d\n", sat_index);
    printf("PDOP: %.2f\n", data.pdop);
    printf("HDOP: %.2f\n", data.hdop);
    printf("VDOP: %.2f\n\n\n", data.vdop);
}


void parse_gsv(char *sentence){
    gsv_data data;
    char *p = sentence;
    int sat_index = 0;
    p = strchr(p, ',') + 1;
    int index = 1;
    while( p!= NULL){
        if(*p!='*' && *p!='$'){
            // not empty
            switch (index) {
            case 1:
                data.total_msgs = atoi(p);
                break;
            case 2:
                data.msg_number = atoi(p);
                break;
            case 3:
                data.total_sats = atoi(p);
                break;
            case 4:
                data.satellites[sat_index].id = atoi(p);
                break;
            case 5:
                data.satellites[sat_index].elevation = atoi(p);
                break;
            case 6:
                data.satellites[sat_index].azimuth = atoi(p);
                break;
            case 7:
                data.satellites[sat_index].snr = atoi(p);
                sat_index++;
                printf("I am here---\n");
                index = 3;
                break;
            }
            p = strchr(p, ',') + 1;
        } else if(*p == ','){
            // nothing to process jump to next
            p = strchr(p, ','); 
        } else if(*p == '*'){
            // at checksum
        } else if( *p=='$'){
            // new sentence creeped in, break to avoid conflicts with corrupt data
            break;
        }
        index++;
    } 
    printf("\nTotal Messages: %d\n", data.total_msgs);
    printf("Message Number: %d\n", data.msg_number);
    printf("Total Satellites: %d\n", data.total_sats);
    printf("Satellites:\n");
    for (int i = 0; i < sat_index; i++) {
        printf("\tID: %d\n", data.satellites[i].id);
        printf("\tElevation: %d\n", data.satellites[i].elevation);
        printf("\tAzimuth: %d\n", data.satellites[i].azimuth);
        printf("\tSNR: %d\n", data.satellites[i].snr);
    }
    printf("\n\n");
}


void parse_mss(char *sentence){
    mss_data data;
    char *p = sentence;
    p = strchr(p, ',') + 1;
    int index = 1;  
    while (p != NULL){
        if(*p!='$' && *p!='*'){
            switch(index){
                case 1:
                    data.signalStrength = atof(p);
                    break;
                case 2:
                    data.snr = atof(p);
                    break;
                case 3:
                    data.beacon_freq = atof(p);
                    break;
                case 4:
                    data.beacon_bitrate = atoi(p);
                    break;
                case 5:
                    data.channel_number = atoi(p);
                    break;
                }   
                if (index<5)
                    p = strchr(p, ',') + 1;
        } else if (*p == '$'){
            break;
        }
        if(index<5)
            index++;
        else
            break;
    }
    printf("\nSignal strength is: %.2f\n", data.signalStrength);
    printf("Signal to Noise ratio: %.2f\n", data.snr);
    printf("Beacon frequency(KHz): %.2f\n", data.beacon_freq);
    printf("Beacon bitrate (bits per second) : %d\n", data.beacon_bitrate);
    printf("Channel number: %d\n\n\n", data.channel_number);
}


void parse_rmc(char *sentence){
    rmc_data data;
    char *p = sentence;
    p = strchr(p, ',') + 1;
    int index = 1;  
    while (p != NULL){
        if(*p!='$' && *p!='*'){
            switch(index) {
            case 1:
                data.time = atof(p);
                break;
            case 2:
                data.status = *p;
                break;
            case 3:
                data.latitude = atof(p);
                break;
            case 4:
                data.latitude_dir = *p;
                break;
            case 5:
                data.longitude = atof(p);
                break;
            case 6:
                data.longitude_dir = *p;
                break;
            case 7:
                data.speed = atof(p);
                break;
            case 8:
                data.heading = atof(p);
                break;
            case 9:
                data.date = atol(p);
                break;
            case 10:
                data.magnetic_variation = atof(p);
                break;
            case 11:
                data.direction = *p;
                break;
            case 12:
                data.mode = *p;
                break;
            }
            if (index<12)
                p = strchr(p, ',') + 1;
        } else if (*p == '$'){
            break;
        }
        if(index<12)
            index++;
        else
            break;
    }
    printf("\nTime UTC (hhmmss.sss): %.3f\n", data.time);
    printf("Status %s\n", data.status=='A'?"Valid data.":"Invalid data.");
    printf("Latitude: %.4f %c\n", data.latitude, data.latitude_dir);
    printf("Longitude: %.4f %c\n", data.longitude, data.longitude_dir);
    printf("Speed over ground: %.4f knots\n", data.speed);
    printf("Course over ground: %.4f degrees\n", data.heading);
    printf("Date (ddmmyy): %ld\n", data.date);
    printf("Magnetic variation: %f degrees %c\n", data.magnetic_variation, data.direction);
    printf("Mode: %c\n\n\n", data.mode);
}

void parse_vtg(char *sentence){
    vtg_data data;
    char *p = sentence;
    p = strchr(p, ',') + 1;
    int index = 1;  
    while (p != NULL){
        if(*p!='$' && *p!='*'){
            switch(index){
            case 1:
                data.course1 = atof(p);
                break;
            case 2:
                data.reference1 = *p;
            case 3:
                data.course2 = atof(p);
                break;
            case 4:
                data.reference2 = *p;
            case 5:
                data.speed1 = atof(p);
                break;
            case 6:
                data.units1 = *p;
                break;
            case 7:
                data.speed2 = atof(p);
                break;
            case 8:
                data.units2 = *p;
                break;
            case 9:
                data.mode = *p;
                break;
            }
            if (index<9)
                p = strchr(p, ',') + 1;
        } else if (*p == '$'){
            break;
        }
        if(index<9)
            index++;
        else
            break;
    }
    printf("\nCourse1: %.4f degrees %c\n", data.course1, data.reference1);
    printf("Course2: %.4f degrees %c\n", data.course2, data.reference2);
    printf("Speed1: %.4f degrees %c\n", data.speed1, data.units1);
    printf("Speed2: %.4f degrees %c\n", data.speed2, data.units2);
    printf("Mode: %c\n\n\n", data.mode);

}

void parse_zda(char *sentence){
    zda_data data;
    char *p = sentence;
    p = strchr(p, ',') + 1;
    int index = 1;  
    while (p != NULL){
        if(*p!='$' && *p!='*'){
            switch(index){
                case 1:
                    data.utcTime = atof(p);
                    break;
                case 2:
                    data.day = atoi(p);
                case 3:
                    data.month = atoi(p);
                    break;
                case 4:
                    data.year = atoi(p);
                case 5:
                    data.localZoneHr = atoi(p);
                    break;
                case 6:
                    data.localZoneMin = atoi(p);
                    break;
                }
                if (index<6)
                    p = strchr(p, ',') + 1;
            } else if (*p == '$'){
                break;
            }
        if(index<6)
            index++;
        else
            break;
    }
    printf("\nTime(UTC): %.2f\n", data.utcTime);
    printf("Day/Month/Year: %d/%d/%d\n", data.day, data.month, data.year);
    printf("Local zone (hh:mm): %d:%d\n\n\n", data.localZoneHr, data.localZoneMin);
}