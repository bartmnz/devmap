#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>

#include "meditrik.h"

struct device* getMeditrikHeader(FILE* file, struct frame* frmPtr){
	//printf("rinning get meditric\n");
	memset(&(frmPtr->medPtr), 0, sizeof(struct meditrik));
	unsigned char temp[2] = {'0','0'};
	fread(temp, 2, 1, file);
	frmPtr->medPtr.verIN = 0;
	frmPtr->medPtr.verUC[0] = temp[0] >> 4;
	frmPtr->medPtr.seqUC[0] = (temp[0] & 8) >> 3;
	frmPtr->medPtr.seqUC[1] = (temp[1] >> 3) | ( temp[0] << 5);
	frmPtr->medPtr.typeUC[0] = temp[1] & 7;
	fread(frmPtr->medPtr.lenUC, 2, 1, file);
	fread(frmPtr->medPtr.srcUC, 4, 1, file);
	fread(frmPtr->medPtr.dstUC, 4, 1, file);
	//frmPtr->medPtr.verUC[0] = ntohs(frmPtr->medPtr.verUC[0]);
	//printf("ptr is %0x2", frmPtr->medPtr.verUC[0]);
	if(frmPtr->medPtr.verIN != 1){
		fprintf(stdout,"ERROR: Unsupported version number! (%d)\n", frmPtr->medPtr.verIN);
		return NULL;
	}
	frmPtr->medPtr.seqIN = ntohs(frmPtr->medPtr.seqIN);
	frmPtr->medPtr.srcIN = ntohl(frmPtr->medPtr.srcIN);
	frmPtr->medPtr.dstIN = ntohl(frmPtr->medPtr.dstIN);

	struct device* rValue = NULL;
	if(frmPtr->medPtr.typeIN == 0){
		getStatus(file, frmPtr);
		rValue = malloc(sizeof(*rValue));
		if ( ! rValue ){
			fprintf(stdout, "ERROR: malloc failed.\n");
			exit(0);
		}
		memset(rValue, 0, sizeof(*rValue));
		rValue->device_id = frmPtr->medPtr.srcIN;
		rValue->battery = frmPtr->stsPtr.batDB;
		rValue->latitude = STATUS_PACKET; // invalid value used for error checking
	}else if(frmPtr->medPtr.typeIN == 1){
		getCommand(file, frmPtr);
	}else if(frmPtr->medPtr.typeIN == 2){
		getGps(file, frmPtr);
		rValue = malloc(sizeof(*rValue));
		if ( ! rValue ){
			fprintf(stdout, "ERROR: malloc failed.\n");
			exit(0);
		}
		memset(rValue, 0, sizeof(*rValue));
		rValue->altitude = frmPtr->gpsPtr.altiDB * 6; // medpacket stores data in fathoms
		rValue->latitude = frmPtr->gpsPtr.latiDB;
		rValue->longitude = frmPtr->gpsPtr.longDB;
		rValue->device_id = frmPtr->medPtr.srcIN;
		rValue->next = NULL;
	}else if(frmPtr->medPtr.typeIN == 3){
		getMessage(file, frmPtr);
	}else{
 		fprintf(stderr,"ERROR: invalid message type");
		exit(0);
	}

	return rValue;
}

void getCommand(FILE* file, struct frame* frmPtr){
	fread(frmPtr->cmdPtr.comUC, 2, 1, file);
	int szData = ntohs(frmPtr->medPtr.lenIN);	
	if(szData == 16)fread(frmPtr->cmdPtr.parUC, 2, 1, file);
}

void getStatus(FILE* file, struct frame* frmPtr){ 
	fread(frmPtr->stsPtr.batUC, 8, 1, file);
//	frmPtr->stsPtr.batDB = frmPtr->stsPtr.batDB * 100;
	fread(frmPtr->stsPtr.gluUC, 2, 1, file);
	fread(frmPtr->stsPtr.capUC, 2, 1, file);
	fread(frmPtr->stsPtr.omoUC, 2, 1, file);
}

void getGps(FILE* file, struct frame* frmPtr){
	fread(frmPtr->gpsPtr.longUC, 8, 1, file);
	fread(frmPtr->gpsPtr.latiUC, 8, 1, file);
	fread(frmPtr->gpsPtr.altiUC, 4, 1, file);
}

void getMessage(FILE* file, struct frame* frmPtr){
	unsigned char zero = '\0';
	int size = ntohs(frmPtr->medPtr.lenIN);
	fread(frmPtr->msgPtr->message, size-12, 1, file);
	frmPtr->msgPtr->message[size-1] = '\n';
	fprintf(stdout,"Message: %s%c\n", frmPtr->msgPtr->message, zero);

}

void setIpHeader(FILE* file, struct ipv4Header* ipv4Header,int size,
						 unsigned char* first){
	
	if(size == 20){
		unsigned char temp[2];
		ipv4Header->verUC[0] = first[0] & 240;
		ipv4Header->hlenUC[0] = first[0] & 15;
		fread(ipv4Header->TOS, 1, 1, file);
		fread(ipv4Header->totalLength, 2, 1, file);
		fread(ipv4Header->identification, 2, 1, file);
		fread(temp, 2, 1, file);
		ipv4Header->flagUC[0] = temp[0] & 240;
		ipv4Header->foffUC[0] = temp[0] & 15;
		ipv4Header->foffUC[1] = temp[1];
		fread(ipv4Header->TTL, 1,1, file);
		fread(ipv4Header->nextProtocol, 1, 1, file);
		fread(ipv4Header->headerCheckSum, 2, 1, file);
		fread(ipv4Header->sourceAddress, 4, 1, file);
		fread(ipv4Header->destinationAddress, 4, 1, file);
		//printf(" dest addr = (%02x)",ipv4Header->dstLN );
	}
}

void setIP6header(FILE* file, struct ipv6Header* ipv6Header, unsigned char* first){
	ipv6Header->verUC[0] = first[0] & 240;
	ipv6Header->trafficClass[0] = first[0] & 15;
	ipv6Header->trafficClass[0] = ipv6Header->trafficClass[0] << 4;
	unsigned char temp1[3];
	fread(temp1, 3, 1, file);
	ipv6Header->flowLabel[0] = temp1[0] & 15;
	ipv6Header->flowLabel[1] = temp1[1];
	ipv6Header->flowLabel[2] = temp1[2];
	ipv6Header->trafficClass[0] = ipv6Header->trafficClass[0] | (temp1[0] >> 4);
	fread(ipv6Header->payLoadLen, 2, 1, file);
	fread(ipv6Header->nextHeadder, 1, 1, file);
	fread(ipv6Header->hopLimit, 1, 1, file);
	fread(ipv6Header->sourceUC, 16, 1, file);
	fread(ipv6Header->destIN, 16, 1, file);
	
}

int getIpLen(unsigned char* bits, int size){
	if (size < 0 || bits == NULL){
		fprintf(stderr, "ERROR: Invalid IP Length");
		return 0;
	}
	unsigned char leftSide = bits[0] >> 4;
	if (leftSide == 4){
		return 4 * (bits[0] & 15);
	}else if( leftSide == 6){
		return -1;
	}else{
		fprintf(stderr, "ERROR: not IPV4 or IPV6\n");
		exit(0);
	}
	return 0;
}

void setEthernetHeader(FILE* file, struct ethernetFrame* ethernetFrame){
        if (!file){
        	return;
        } 
        fread(ethernetFrame->dstUC, 6, 1, file);
        fread(ethernetFrame->srcUC, 6, 1, file);
        fread(ethernetFrame->nxtUC, 2, 1, file);
        
}

void setUdpHeader(FILE* file, struct udpHeader* udp){
	fread(udp->srcUC, 2, 1, file);
	fread(udp->dstUC, 2, 1, file);
	fread(udp->lenUC, 2, 1, file);
	fread(udp->chkUC, 2, 1, file);
}
