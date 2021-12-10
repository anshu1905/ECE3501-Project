//Internet of Things Fundamentals (ECE3501)
/* 
	Digital SSTV Encoder:
	*  Project Team:
		* Anirudh Karnik  : 19BEC0353
		* Anshuman Phadke : 19BEC0428
		* Arvind N        : 19BEC0564
		
*/


// =============================
// 				includes
// =============================

#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include "wav.h"

// =============================
// 				Prototypes
// =============================

//	Write genereated samples to the .WAV file
void Wrt2File (int NumSamps, double * LnSamps, const struct wav_info* w, FILE* fp, double dM);
//  Generation of Sampled data sequences for: Sync, Porch, Separators, and image elements
void GenSamples (double Delta, int SampFreq, double SigFreq, double in_phase, double * out_phase, int * nSamps, double * LnSamps);

//void bmpDataPart(FILE* fpbmp);
void ReadBMPData(FILE* fpbmp);

// Check BMP file for key parameters 
void TestBMPFile(FILE* fpbmp);

// For diagnostics -- pauses program
void StrategicPause(char * strng);

void SSTV_Martin();

void SSTV_Scottie();

void SSTV_WRASSE();

// *************************
//       Globals
// *************************

unsigned int OffSet = 0;    // OffSet from Header part to Data Part

int width ;          // The Width of the Data Part
int height ;         // The Height of the Data Part

uint8_t r[256][320];
uint8_t g[256][320];
uint8_t b[256][320];

double LineSamples[35000];									// Array to store digitized image data for each line
//double fR[256][320], fG[256][320], fB[256][320];			// digitised RGB data array

// =============================
// 				Main
// =============================

int main(int argc, char* argv[])
	{
		int SSTV_Mode;
		int choice;
		unsigned char *fp_temp;
		char ch;
		char fileName[200];			// Filename of BMP file to open
	    FILE *fpbmp;
	  
// Print Program Header
	printf(  "\n\n\n     ************* Digital Slow Scan Television *************\n\n\n");
	printf(  "          *  Project Team:\n");
	printf(  "                * Anirudh Karnik  : 19BEC0353 \n");
	printf(  "                * Anshuman Phadke : 19BEC0428 \n");
	printf(  "                * Arvind N        : 19BEC0564 \n\n");
    printf(  "          *  Run on:  \n                - Date: %s \n                - Time: %s\n\n", __DATE__ , __TIME__ );
	printf("     ********************************************************\n");
	
	
	    // Open bmp file 
			printf ("\n Input name of BMP file to open: ");	     
			scanf("%s", &fileName);
			//	printf ("\n File to open: %s .... Length:  %i\n", fileName, strlen(fileName));	     
	     	fpbmp= fopen(fileName, "rb");
	
		     if (fpbmp == NULL)
			     {
					 printf("*** Error .. Failed to open %s file !!!\n", fileName);
					 return 1;
			     }
	     
			//		StrategicPause("Tried opening the BMP file \n");
		//	Check to see if the BMP file has the appropriate height, width and depth for SSTV encoding
	     	TestBMPFile(fpbmp);                // Test the file is bmp file or not
	
		// Read RGB data from the BMP file
			fseek(fpbmp, 0L, SEEK_SET);			// rewind to beginning of file
			ReadBMPData(fpbmp);			// read the bmp file and fill arrays for RGB pixels

		// Close the BMP file
			fclose(fpbmp);				
			int validInput = 0;			
		// Select SSTV encoding format
		    while(validInput == 0)
			    {
			        printf("\nEncoding options for Digital SSTV :  \n");
			        printf("1. Scottie S1 \n");
			        printf("2. Martin M1\n");
			        printf("3. WRASSE SC2_180\n\n");
			        printf("Enter choice of encoding format for Digital SSTV :  ");
			        scanf("%d",&choice);
			        printf(" choice in hex: %02x .. choice as int: %i \n", choice, choice);
			        
			        switch(choice)
				        {
				            case 1:		// Scottie S1
					            {
					            	printf(" case 1 \n");
									// Encode Image data and generate te SSTV audio file
										SSTV_Scottie();
										validInput = 1;
					                break;
								}
								
				            case 2:		// Martin M1
					            {
					            	printf(" case 2 \n");
									// Encode Image data and generate te SSTV audio file
										SSTV_Martin();
										validInput = 1;
					                break;
								}
				        
				            case 3:
					            {
					            	printf(" case 3 \n");
									// Encode Image data and generate te SSTV audio file
										SSTV_WRASSE();
										validInput = 1;
					                break;
								}
				        }	// end Switch
			    }	// end While
		
	         return 0;
	}	// end Main


// Method to encode the array of RGB values from the BMP file to a SSTV WAV file
void SSTV_Scottie()
{ 
	// File Details for the WAV file header
    struct wav_info w;
    w.num_channels=1;
    w.bits_per_sample=16;
    w.sample_rate=44100;

// =============================
	// Declare variables 
// =============================
    char* fname = "DSSTV_Scottie_1_encoder.wav";    	// File name for encoded DSSTV signal
	uint_fast32_t sample_rate_Fs;
    double duration;									// pixel duration in mS
    double freq1, phase, Phfactr, SigFac, ComputedPhase;
    int NumSampPerPxl;									// Number of samples per pixel .. integer part only
    double s,t;
    int_fast32_t M;
    int_fast32_t sample[w.num_channels];
	int iLns, iPxls, jc;
	int iR, iP;
	int TotCount;
	unsigned char uchr, uchg, uchb;
	double PixelFreq;

// =============================
// 		Initialise variables
// =============================
	duration = 0.4320;						// Pixel duration for Scottie S1 encoding
	sample_rate_Fs = 44100;
	NumSampPerPxl = (int) ((duration * sample_rate_Fs) / 1000);
	phase = 0.0;
	Phfactr = 2*M_PI*duration/1000.0;
	SigFac = 2 * M_PI / sample_rate_Fs;
	iR = 0;
	iP = 0;
	TotCount = 0;

// Initiate File open
	//    printf("Preparing to write SSTV File to %s:\n",fname);
	//    print_wav_info(&w);

    FILE* fp = fopen(fname,"wb");

    if(!fp) 
		{
	        fprintf(stderr,"Error opening %s for output!\n",fname);
	        return;
	    }

	// Write the WAV file header
    write_wav_hdr(&w,fp);

    switch(w.bits_per_sample) 
	{
	    case 8:
	      M=0x7F;
	      break;
	    case 16:
	      M=0x7FFF;
	      break;
	    case 24:
	      M=0x7FFFFF;
	      break;
	    case 32:
	      M=0x7FFFFFFF;
	      break;
	    default:
	      printf("Error: bits_per_sample must be 8,16,24, or 32.\n");
	      return;
    }

    double dM = (double) M;
	double i_phase, o_phase;
	int NmSamps, tot_samps; 
	i_phase = 0.0;
	NmSamps = 0;
	tot_samps = 0;

	// Generate SSTV File
	
	// Attention tones -- generate data samples in the following order.  
	// Values shown are: Duration in mS and Frequency in Hz
		//	100.0, 1900.0
		//	100.0, 1500.0
		//	100.0, 1900.0
		//	100.0, 1500.0
		//	100.0, 2300.0
		//	100.0, 1500.0
		//	100.0, 2300.0
		//	100.0, 1500.0

	i_phase = 0.0;			// Initial starting phase = 0
	tot_samps = 0;			// number of discrete samples generated
	GenSamples (100.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;			// Initial phase set to the phase from last sample
	tot_samps = 0;				// Reset count
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 2300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 2300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	// Generate Calibration Header - VIS data samples.  
		// 		VIS code for Scottie S1 is: 60 decimal
		//		Generate data samples in the following order.  
		//		Values shown below are: Duration in mS and Frequency in Hz.
		//	300.0, 1900.0
		//	 10.0, 1200.0
		//	300.0, 1900.0
		
		//	 30.0, 1200.0		// Start pulse
		//	 30.0, 1300.0		// LSB 0
		//	 30.0, 1300.0		// 0
		//	 30.0, 1100.0		// 1
		// 	 30.0, 1100.0		// 1
		// 	 30.0, 1100.0		// 1
		//	 30.0, 1100.0		// 1
		//	 30.0, 1300.0		// 0 (MSB)
		//	 30.0, 1300.0		// Even Parity 0
		//	 30.0, 1200.0		// VIS Stop Bit
			
	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (300.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (10.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (300.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	// Generate Pre-First Line Sync Pulse -- required for Scottie S1 format
		//	9.0, 1200.0		-- Duration in mS and Frequency in Hz
	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (9.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file
	
	// Generate data samples for each line of the image
	for (iLns = 0; iLns < 256; iLns++)						// discrete sample generation for each of the 256 lines of the image
		{
				i_phase = o_phase;
				tot_samps = 0;
			// Separator Pulse	-- 1.5 mS, 1500.00 Hz
				GenSamples (1.5, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
				
			//	**** Generation data samples for Green pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * g[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		
					
			// Separator Pulse	-- 1.5 mS, 1500.00 Hz
				i_phase = o_phase;
				GenSamples (1.5, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
				
			//	**** Generation data samples for Blue pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * b[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		
					
			// Horizontal Sync Pulse	-- 9.0 mS, 1200.00 Hz
				i_phase = o_phase;
				GenSamples (9.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
		
			// Separator Pulse	-- 1.5 mS, 1500.00 Hz
				i_phase = o_phase;
				GenSamples (1.5, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	
			//	**** Generation data samples for Red pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * r[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		

				Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file
		}	// Loop for image Rows
	
    	fclose(fp);											// Close file
}		// end Scottie S1


// Method to encode the array of RGB values from the BMP file to a SSTV WAV file
void SSTV_Martin()
{ 
	// File Details for the WAV file header
    struct wav_info w;
    w.num_channels=1;
    w.bits_per_sample=16;
    w.sample_rate=44100;

// =============================
	// Declare variables 
// =============================
    char* fname = "DSSTV_Martin_1_encoder.wav";    	// File name for encoded DSSTV signal
	uint_fast32_t sample_rate_Fs;
    double duration;									// pixel duration in mS
    double freq1, phase, Phfactr, SigFac, ComputedPhase;
    int NumSampPerPxl;									// Number of samples per pixel .. integer part only
    double s,t;
    int_fast32_t M;
    int_fast32_t sample[w.num_channels];
	int iLns, iPxls, jc;
	int iR, iP;
	int TotCount;
	unsigned char uchr, uchg, uchb;
	double PixelFreq;

// =============================
// 		Initialise variables
// =============================
	duration = 0.4576;						// Pixel duration for Martin M1 encoding
	sample_rate_Fs = 44100;
	NumSampPerPxl = (int) ((duration * sample_rate_Fs) / 1000);
	phase = 0.0;
	Phfactr = 2*M_PI*duration/1000.0;
	SigFac = 2 * M_PI / sample_rate_Fs;
	iR = 0;
	iP = 0;
	TotCount = 0;

// Initiate File open
	//    printf("Preparing to write SSTV File to %s:\n",fname);
	//    print_wav_info(&w);

    FILE* fp = fopen(fname,"wb");

    if(!fp) 
		{
	        fprintf(stderr,"Error opening %s for output!\n",fname);
	        return;
	    }

	// Write the WAV file header
    write_wav_hdr(&w,fp);

    switch(w.bits_per_sample) 
	{
	    case 8:
	      M=0x7F;
	      break;
	    case 16:
	      M=0x7FFF;
	      break;
	    case 24:
	      M=0x7FFFFF;
	      break;
	    case 32:
	      M=0x7FFFFFFF;
	      break;
	    default:
	      printf("Error: bits_per_sample must be 8,16,24, or 32.\n");
	      return;
    }

    double dM = (double) M;
	double i_phase, o_phase;
	int NmSamps, tot_samps; 
	i_phase = 0.0;
	NmSamps = 0;
	tot_samps = 0;

	// Generate SSTV File
	
	// Attention tones -- generate data samples in the following order.  
	// Values shown are: Duration in mS and Frequency in Hz
		//	100.0, 1900.0
		//	100.0, 1500.0
		//	100.0, 1900.0
		//	100.0, 1500.0
		//	100.0, 2300.0
		//	100.0, 1500.0
		//	100.0, 2300.0
		//	100.0, 1500.0

	i_phase = 0.0;			// Initial starting phase = 0
	tot_samps = 0;			// number of discrete samples generated
	GenSamples (100.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;			// Initial phase set to the phase from last sample
	tot_samps = 0;				// Reset count
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 2300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 2300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	// Generate Calibration Header - VIS data samples.  
		//		Generate data samples in the following order.  
		//		Values shown below are: Duration in mS and Frequency in Hz.
		//	300.0, 1900.0
		//	 10.0, 1200.0
		//	300.0, 1900.0
		//	 30.0, 1200.0
		
	// VIS data bits (Martin 1) -- 44 decimal, 0x2C	
		//	 30.0, 1300.0		// LSB 0
		//	 30.0, 1300.0		// 0
		//	 30.0, 1100.0		// 1
		// 	 30.0, 1100.0		// 1
		// 	 30.0, 1300.0		// 0
		//	 30.0, 1100.0		// 1
		//	 30.0, 1300.0		// 0
		//	 30.0, 1100.0		// Even Parity 1
		//	 30.0, 1200.0		// VIS Stop Bit
	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (300.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (10.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (300.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

//	// Generate Pre-First Line Sync Pulse -- required for Scottie S1 format
//		//	9.0, 1200.0		-- Duration in mS and Frequency in Hz
//	i_phase = o_phase;
//	tot_samps = 0;
//	GenSamples (9.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
//	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file
	
	// Generate data samples for each line of the image
	for (iLns = 0; iLns < 256; iLns++)						// discrete sample generation for each of the 256 lines of the image
		{
				i_phase = o_phase;
				tot_samps = 0;
			// Sync Pulse	-- 4.862 mS, 1200.00 Hz
				GenSamples (4.862, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
				
				i_phase = o_phase;
			// Sync porch	-- 0.572 mS, 1500.00 Hz
				GenSamples (0.572, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
				
			//	**** Generation data samples for Green pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * g[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		
					
			// Separator Pulse	-- 0.572 mS, 1500.00 Hz
				i_phase = o_phase;
				GenSamples (0.572, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
				
			//	**** Generation data samples for Blue pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * b[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		
					
			// Separator Pulse	-- 0.572 mS, 1500.00 Hz
				i_phase = o_phase;
				GenSamples (0.572, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	
			//	**** Generation data samples for Red pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * r[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		

					
			// Separator Pulse	-- 0.572 mS, 1500.00 Hz
				i_phase = o_phase;
				GenSamples (0.572, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	
				Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file
		}	// Loop for image Rows
	
    	fclose(fp);											// Close file
}		// end Martin M1

// Method to encode the array of RGB values from the BMP file to a SSTV WAV file
void SSTV_WRASSE()
{ 
	// File Details for the WAV file header
    struct wav_info w;
    w.num_channels=1;
    w.bits_per_sample=16;
    w.sample_rate=44100;

// =============================
	// Declare variables 
// =============================
    char* fname = "DSSTV_WRASSE_encoder.wav";    	// File name for encoded DSSTV signal
	uint_fast32_t sample_rate_Fs;
    double duration;									// pixel duration in mS
    double freq1, phase, Phfactr, SigFac, ComputedPhase;
    int NumSampPerPxl;									// Number of samples per pixel .. integer part only
    double s,t;
    int_fast32_t M;
    int_fast32_t sample[w.num_channels];
	int iLns, iPxls, jc;
	int iR, iP;
	int TotCount;
	unsigned char uchr, uchg, uchb;
	double PixelFreq;

// =============================
// 		Initialise variables
// =============================
	duration = 0.7344;						// Pixel duration for WRASSE encoding
	sample_rate_Fs = 44100;
	NumSampPerPxl = (int) ((duration * sample_rate_Fs) / 1000);
	phase = 0.0;
	Phfactr = 2*M_PI*duration/1000.0;
	SigFac = 2 * M_PI / sample_rate_Fs;
	iR = 0;
	iP = 0;
	TotCount = 0;

// Initiate File open
	//    printf("Preparing to write SSTV File to %s:\n",fname);
	//    print_wav_info(&w);

    FILE* fp = fopen(fname,"wb");

    if(!fp) 
		{
	        fprintf(stderr,"Error opening %s for output!\n",fname);
	        return;
	    }

	// Write the WAV file header
    write_wav_hdr(&w,fp);

    switch(w.bits_per_sample) 
	{
	    case 8:
	      M=0x7F;
	      break;
	    case 16:
	      M=0x7FFF;
	      break;
	    case 24:
	      M=0x7FFFFF;
	      break;
	    case 32:
	      M=0x7FFFFFFF;
	      break;
	    default:
	      printf("Error: bits_per_sample must be 8,16,24, or 32.\n");
	      return;
    }

    double dM = (double) M;
	double i_phase, o_phase;
	int NmSamps, tot_samps; 
	i_phase = 0.0;
	NmSamps = 0;
	tot_samps = 0;

	// Generate SSTV File
	
	// Attention tones -- generate data samples in the following order.  
	// Values shown are: Duration in mS and Frequency in Hz
		//	100.0, 1900.0
		//	100.0, 1500.0
		//	100.0, 1900.0
		//	100.0, 1500.0
		//	100.0, 2300.0
		//	100.0, 1500.0
		//	100.0, 2300.0
		//	100.0, 1500.0

	i_phase = 0.0;			// Initial starting phase = 0
	tot_samps = 0;			// number of discrete samples generated
	GenSamples (100.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;			// Initial phase set to the phase from last sample
	tot_samps = 0;				// Reset count
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 2300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 2300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (100.0, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	// Generate Calibration Header - VIS data samples.  
	// 		VIS code for WRASSE_SC2_180 is: 55 decimal
		//		Generate data samples in the following order.  
		//		Values shown below are: Duration in mS and Frequency in Hz.
		//	300.0, 1900.0
		//	 10.0, 1200.0
		//	300.0, 1900.0
		//	 30.0, 1200.0
		
	// 		VIS code for WRASSE_SC2_180 is: 55 decimal  = 0x37
	//	 30.0, 1100.0		// LSB 1
	//	 30.0, 1100.0		// 1
	//	 30.0, 1100.0		// 1
	// 	 30.0, 1300.0		// 0
	
	// 	 30.0, 1100.0		// 1
	//	 30.0, 1100.0		// 1
	//	 30.0, 1300.0		// 0
	
	//	 30.0, 1100.0		// Even Parity 1
		
		//	 30.0, 1200.0		// VIS Stop Bit


	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (300.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (10.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (300.0, 44100, 1900.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1300.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1100.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file


	i_phase = o_phase;
	tot_samps = 0;
	GenSamples (30.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file

//	// Generate Pre-First Line Sync Pulse -- required for Scottie S1 format
//		//	9.0, 1200.0		-- Duration in mS and Frequency in Hz
//	i_phase = o_phase;
//	tot_samps = 0;
//	GenSamples (9.0, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
//	Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file
	
	// Generate data samples for each line of the image
	for (iLns = 0; iLns < 256; iLns++)						// discrete sample generation for each of the 256 lines of the image
		{
				i_phase = o_phase;
				tot_samps = 0;
			// Sync Pulse	-- 5.5225 mS, 1200.00 Hz
				GenSamples (5.5225, 44100, 1200.0, i_phase, &o_phase, &tot_samps, LineSamples);
				
				i_phase = o_phase;
			// Sync porch	-- 0.5 mS, 1500.00 Hz
				GenSamples (0.5, 44100, 1500.0, i_phase, &o_phase, &tot_samps, LineSamples);
				
			//	**** Generation data samples for red pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * r[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		
					
			//	**** Generation data samples for Green pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * g[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		
	
			//	**** Generation data samples for Blue pixels in the line	
				for (int i = 0; i < 320; i++)
					{
						i_phase = o_phase;
						PixelFreq = (double) (1500.0 + ((800.0 * b[iLns][i]) / 255.0));
						GenSamples (duration, 44100, PixelFreq, i_phase, &o_phase, &tot_samps, LineSamples);
					}		

	
				Wrt2File (tot_samps, LineSamples, &w, fp, dM);			// write generated samples to the WAV file
		}	// Loop for image Rows
	
    	fclose(fp);											// Close file
}		// end WRASSE SC2_180


//	Method to write generated samples to the .WAV file
void Wrt2File (int NumSamps, double * LnSamps, const struct wav_info* w, FILE* fp, double dM)
	{
		int_fast32_t smpl[w->num_channels];
		int_fast32_t  SampRate;
		double t, s;
		
	    for(int n=0; n < NumSamps; n++) 
			{
		        for(int c=0; c < w->num_channels; c++) 
				{
		            s = LnSamps[n];
		            smpl[c] = (int_fast32_t)(dM * s);
		        }
		        write_sample(w,fp,smpl);
		    }
	}


//  Generation of Sampled data sequences for: Sync, Porch, Separators, and image elements
void GenSamples (double Delta, int SampFreq, double SigFreq, double in_phase, double * out_phase, int * nSamps, double * LnSamps)
	{
		int NumOfSamples;
		double comphase;
		int indx;
		indx = *nSamps;

			NumOfSamples = (int) (Delta * SampFreq) / 1000.00;
			for (int i = 0; i < NumOfSamples; i++)
				{
					// Generate samples
					LnSamps[i + indx] = sin(((2.0 * M_PI * SigFreq * i) / SampFreq) + in_phase);
				}
					comphase = (2.0 * M_PI * Delta * SigFreq / 1000.0) + in_phase;
					comphase = fmod(comphase, (2.0 * M_PI));
					*out_phase = comphase;
			*nSamps = *nSamps + NumOfSamples;
	}

//  Method to read the BMP file and fill in arrays for RGB pixel values
void ReadBMPData(FILE* fpbmp)
	{
	    int i, j=0;
//		unsigned char* pix=NULL;
     	uint8_t FilDat[1000];									// one line of RGB data
	
		fseek(fpbmp, 0L, SEEK_SET);								// Rewind to beginning of the file

	for(j=0; j<height; j++)										// Loop through all rows
		{
			fseek(fpbmp, 0L, SEEK_SET);							// Rewind to beginning of the file
			fseek(fpbmp, (long) (j * 960L + 54L), SEEK_SET);	// Point to the j * 960 byte in the file
	     	fread(&FilDat, sizeof(uint8_t), 960, fpbmp);		// Read 3x320 bytes -- BGR pixel values
		   	for(i=0; i<width; i++)								// Loop through all the pixels in the line
		        {
		            r[height - 1 - j][i] = FilDat[i*3+2];		// Store R values in the r[][] array
		            g[height - 1 - j][i] = FilDat[i*3+1];		// Store G values in the g[][] array
		            b[height - 1 - j][i] = FilDat[i*3];			// Store B values in the b[][] array
		            											// Rem: BMP data is stored from last image row to 1st
		        }		// Width, or pixels in the line
		}	 // Height, or rows
	}	// end method


//  Method to check the BMP file for key parameters 
//	File Size, Image height or rows, Inage width or pixels in a line, Bits per pixel, ....
void TestBMPFile(FILE* fpbmp)
{     
     uint8_t FilDat[20];
 	 char ch;
 	 int PixelDataOffset, PicHeight, PicWidth, NumBits;

 	 ch = 'A';

     fseek(fpbmp, 0L, SEEK_SET);			//seek_set
     fread(&FilDat, sizeof(uint8_t), 14, fpbmp);
		//// Diagnostic print
		// 		printf("FilDat[0]: %02x .. FilDat[1] %02x\n", (uint8_t) FilDat[0], (uint8_t)  FilDat[1]);
		// 		printf("FilDat[0]: %c .. FilDat[1] %c \n", (uint8_t) FilDat[0], (uint8_t)  FilDat[1]);
 		
		if ((FilDat[0] != 'B') || (FilDat[1] != 'M'))		// Check if header has BM in it
			{
				 printf("This file is not in the BMP file format !!\n");
				 exit(1);
			}
 			PixelDataOffset  = FilDat[13]* (256^3) + FilDat[12]* (256^2) + FilDat[11]* (256) + FilDat[10];
			printf("\nPixel Data Offset: %i bytes \n", (int) PixelDataOffset);
		
   		// Set file pointer to the beginning of the file
   		fseek(fpbmp, 0, SEEK_SET); 							// We may also use rewind(fpbmp)
		
		fseek(fpbmp, 18L, SEEK_SET);						// Point to the 19th byte in the file
     	fread(&FilDat, sizeof(uint8_t), 12, fpbmp);			// Read 12 bytes
		 			
			// Compute the image width, height and # of bits per pixel (8x R, G, B) 			
 			PicWidth  = FilDat[3]* (256^3) + FilDat[2]* (256^2) + FilDat[1]* (256) + FilDat[0];
 			PicHeight = FilDat[7]* (256^3) + FilDat[6]* (256^2) + FilDat[5]* (256) + FilDat[4];
 			NumBits   = FilDat[11]* (256) + FilDat[10];
 			
 			height = PicHeight;
 			width  = PicWidth;
			// must be: 320, 256, 24 respectively, 
			// For SSTV in the Scottie S1, Martin M1 and the WRASSE_SC2_180 modes
			//// Diagnostic print
			 		printf("Image Width   : %i pixels \n", (int) PicWidth);
			 		printf("Image Height  : %i pixels \n", (int) PicHeight);
			 		printf("Bits per pixel: %i bits \n", (int) NumBits);
			 		
			if (!((PicWidth = 320) && (PicHeight = 256) && (NumBits = 24)))
				{
					printf (" \n  **** Image file is not suitable for the selected SSTV Modes ****\n");
				}
}


// Method to pause execution of the program for Diagnostic purposes
// Waits for keyboard input; proceeds if a '#' character was input
void StrategicPause(char * strng)
	{
		// for Diagnostics only ---- Pause execution
		char ch;
			
	  printf("%s .. Enter '#' to continue, ^C to terminate ... \n", (char *) strng);
	  while (1)
		  {
		  	ch = getchar();
		  	if (ch == '#')
		  		{
		  			printf(" Character input: %c\n", (char) ch);
		  			break;
			  	}
		  }
	}